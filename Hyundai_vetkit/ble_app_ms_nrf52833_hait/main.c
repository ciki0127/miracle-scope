#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_timer.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "ble_gatts.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ms_ble_bios.h"  //cspark
#include "ms_ble_utils.h"
#include "ms_task.h"
#include "ms_gpiote.h"
#include "ms_saadc2.h"
#include "ms_app_executive.h"

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define UART_TX_BUF_SIZE                64                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                64                                         /**< UART RX buffer size. */

extern void bsp_event_handler(bsp_event_t event);

static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void uart_event_handle(app_uart_evt_t * p_event)
{
}

static void msUARTE_Initialize(void)
{
  uint32_t                     err_code;
  app_uart_comm_params_t const comm_params = {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };
  APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, uart_event_handle, APP_IRQ_PRIORITY_LOWEST, err_code);
  APP_ERROR_CHECK(err_code);
}

static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}

/**
 * @brief Function for initializing the nrf log module.
 * @param[in] 
 * @param[out]
 */
static void msLOG_Initialize(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief Function for initializing power management.
 * @param[in] 
 * @param[out]
 */
static void msPWR_MAN_Initialize(void)
{
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for handling the idle state (main loop).
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 * @param[in] 
 * @param[out]
 */
static void msSYS_IdleStateHandle(void)
{
  if (NRF_LOG_PROCESS() == false) {
    nrf_pwr_mgmt_run();
  }
}


void main (void)
{
  bool erase_bonds;

 
  #ifndef SOFTDEVICE_PRESENT
    // Softdevice will always start the HFXO before a RADIO event and release it
    // afterward to reduce the base current. The reason for this is that RADIO requires
    // an accurate clock source to transmit at the correct frequency. if you want the
    // HFXO always on you can call sd_clock_hfclk_request() after enabling Softdevice,
    // but this will most likely increase the average power consumption
    // Caution ! 
    // when using Softdevice, lfclock is initialized with clock initalization information
    // in sdk_config.h in Softdevice.
    uint32_t status = nrf_drv_clock_init(); // initializse System CloCK
    APP_ERROR_CHECK(status);
    nrf_drv_clock_lfclk_request(NULL); // initialize RTC Clock
    if (!nrf_clock_hf_is_running(NRF_CLOCK_HFCLK_HIGH_ACCURACY)) { // high frequency clock start
      nrfx_clock_hfclk_start();
    }
  #endif

  /// Initializes system basic blocks    
  msUARTE_Initialize();
  msLOG_Initialize();
  timers_init();
  msPWR_MAN_Initialize();
  msGPIOTE_Initialize();

#if defined(MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS)
  #if MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS > 0
    nrf_delay_ms(MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS);
  #endif
#endif

  msApp_EXEC_Start();

  /// Start SAADC block to capture the thredd sensor signals (bio, pressure, battery)
  nrfx_saadc_config_t adcConfig = NRFX_SAADC_DEFAULT_CONFIG;
  adcConfig.resolution         = (nrf_saadc_resolution_t)NRFX_SAADC_CONFIG_RESOLUTION;
  adcConfig.oversample         = (nrf_saadc_oversample_t)NRFX_SAADC_CONFIG_OVERSAMPLE;
  adcConfig.interrupt_priority = NRFX_SAADC_CONFIG_IRQ_PRIORITY;
  adcConfig.low_power_mode     = true; // NRFX_SAADC_CONFIG_LP_MODE, set to the low power mode enable

  nrf_drv_timer_config_t adcTimerConfig = NRF_DRV_TIMER_DEFAULT_CONFIG;
  adcTimerConfig.frequency = NRF_TIMER_FREQ_31250Hz; // set to the lowest frequency //NRF_TIMER_FREQ_1MHz;

  MS_SAADC_CHANNEL2 saadcChannel[MS_APP_SAADC_CHANNEL_NUMBER] = { // descrips channel config
            {
              .m_id = 0,  // Pressure Sensor
              .m_config = {
                .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
                .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
                .gain       = NRF_SAADC_GAIN1_6,
                .reference  = NRF_SAADC_REFERENCE_INTERNAL,
                .acq_time   = NRF_SAADC_ACQTIME_10US,
                .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
                .burst      = NRF_SAADC_BURST_DISABLED,
                .pin_p      = (nrf_saadc_input_t)(ADC_PRSSENSOR),   // ADC1, single end
                .pin_n      = NRF_SAADC_INPUT_DISABLED              // disable, single end
              },
              .m_fnDataHandler = msAPP_PressureSensor_SamplingTaskHandler,
              .m_user = 0 // user context pointer or data
            },
            {
              .m_id = 1,  // BIO (Heart Rate, Respiration) Sensor
              .m_config = {
                .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
                .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
                .gain       = NRF_SAADC_GAIN1_6,
                .reference  = NRF_SAADC_REFERENCE_INTERNAL,
                .acq_time   = NRF_SAADC_ACQTIME_10US,
                .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
                .burst      = NRF_SAADC_BURST_DISABLED,
                .pin_p      = (nrf_saadc_input_t)(ADC_BIOSENSOR),   // ADC0, single end
                .pin_n      = NRF_SAADC_INPUT_DISABLED              // disable, single end
              },
              .m_fnDataHandler = msAPP_BioSensor_SamplingTaskHandler,
              .m_user = 0 // user context pointer or data
            },
            {
              .m_id = 2,  // Battery Sensor
              .m_config = {
                .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
                .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
                .gain       = NRF_SAADC_GAIN1_6,
                .reference  = NRF_SAADC_REFERENCE_INTERNAL,
                .acq_time   = NRF_SAADC_ACQTIME_10US,
                .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
                .burst      = NRF_SAADC_BURST_DISABLED,
                .pin_p      = (nrf_saadc_input_t)(ADC_BATTERY),     // ADC_BAT, single end
                .pin_n      = NRF_SAADC_INPUT_DISABLED              // disable, single end
              },
              .m_fnDataHandler = msAPP_BatterySensor_SamplingTaskHandler,
              .m_user = 0 // user context pointer or data
            }
        };

  msSAADC2_Initialize(&adcConfig,           // intialize SAADC Handler
            SAADC_LOW_POWER,                // SAADC_LOWEST_POWER, SAADC_HIGH_POWER, SAADC_USER_DRIVEN_POWER
            NULL,                           // only for test
            &adcTimerConfig,                // timer config
            saadcChannel,                   // pointer of channel config
            MS_APP_SAADC_CHANNEL_NUMBER,    // number of channel
            MS_APP_BIOSENSOR_SAMPLING_RATE, // sampling rate (by Herz)
            MS_APP_SAADC_SPOOLING_LENGTH);  // number of pooling

  msSAADC2_Start();

  
  /// Initialize BLE SoftDevice and Start Advertising
  msApp_BLE_Initialize(&erase_bonds,        // initialize BLE Softdevice and our Custom Service
          msApp_COM_fnAdevertiseHandler,
          msApp_COM_fnConnectHandler, 
          msApp_COM_fnDisconnectHandler, 
          msApp_COM_fnWrittenHandler);
  msApp_BLE_Advertising();                  // start the advertising to any user
    
  // Start execution.
  printf("\r\nUART started.\r\n");
  NRF_LOG_INFO("Debug logging for UART over RTT started.");

  // Enter main loop.
  for (;;) {
    msSYS_IdleStateHandle();
  }
}