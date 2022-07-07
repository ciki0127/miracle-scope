/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_executive.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ms_app_executive.h"
#include "ms_ble_bios.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"



#if defined (__cplusplus)
extern "C" {
#endif

APP_EXECUTIVE   appExecutive = {
          .m_power = {
            .source = MS_POWER_SOURCE_BATTERY,
            .state = MS_POWER_OFF,
            .lvlCtrl = {           
              .normalMax = 100,
              .normalMin = 30, 
              .pwrSaving = 10, 
              .shutDown  = 0
            },
            .battery = {
              .pin = ADC_BATTERY,
              .capacity = 1000,
              .level = 100,
              .state = BS_CAN_OPERATE
            }
          },
          .m_date = {
            .year = 1970,
            .month = 1,
            .day = 1,
            .hour = 0,
            .minute = 0,
            .second = 0,
            .millis = 0
          },
          .m_led = {
            {
              .pin = LED_R_BLE_CONNECTED, 
              .on = MS_POWER_OFF, 
              .state = CUI_STS_NULL,
              .value = 0
            },
            {
              .pin = LED_G_BLE_CONNECTED,
              .on = MS_POWER_OFF, 
              .state = CUI_STS_NULL,
              .value = 0
            },
            {
              .pin = LED_R_FNC_OPSTATE,
              .on = MS_POWER_OFF, 
              .state = CUI_STS_NULL,
              .value = 0
            },
            {
              .pin = LED_G_FNC_OPSTATE,
              .on = MS_POWER_OFF, 
              .state = CUI_STS_NULL,
              .value = 0
            }
          },
          .m_bio = {
            .adc = {
              .id = 0,
              .pin = ADC_BIOSENSOR,
              .state = CUI_STS_NULL,
            },
            .state = {
              .on = MS_POWER_OFF,
              .condition = SS_UNKNOWN,
              .value = 0
            }
          },
          .m_respiration = {
            .adc = {
              .id = 0,
              .pin = ADC_BIOSENSOR,
              .state = CUI_STS_NULL,
            },
            .state = {
              .on = MS_POWER_OFF,
              .condition = SS_UNKNOWN,
              .value = 0
            }
          },
          .m_heart = {
            .adc = {
              .id = 0,
              .pin = ADC_BIOSENSOR,
              .state = CUI_STS_NULL,
            },
            .state = {
              .on = MS_POWER_OFF,
              .condition = SS_UNKNOWN,
              .value = 0
            }
          },
          .m_pressure = {
            .adc = {
              .id = 1,
              .pin = ADC_PRSSENSOR,
              .state = CUI_STS_NULL,
            },
            .state = {
              .on = MS_POWER_OFF,
              .condition = SS_UNKNOWN,
              .value = 0
            }    
          },
          .m_battery = {
            .adc = {
              .id = 2,
              .pin = ADC_BATTERY,
              .state = CUI_STS_NULL,
            },
            .state = {
              .on = MS_POWER_OFF,
              .condition = SS_UNKNOWN,
              .value = 0
            }
          },
          .m_magnetic = {
            .id = MS_APP_THERAPY_PWM_ID,
            .on = MS_POWER_OFF,
            .state = CUI_STS_NULL,
          },
          .m_comport = {
            .cid = PWM_THERAPHY, 
            .state = CS_UNINITIALIZED, 
          },
          .m_therapy = {
            .mode = THERAPHY_UNDEFINED,
            .number = 0,     
            .command = THERAPHY_STOPPED
          },
          .m_teralog = {
            .m_shortPerDay = 0, 
            .m_underTreatment = MS_FALSE,
            .m_queued = THERAPHY_UNDEFINED,
            .m_list = {
              .m_ix = 0,
              .m_ox = 0,
              .m_array = NULL,
              .m_size = 0,
              .m_unit = 0
            }
          },
        };

APP_EXECUTIVE* pExecutive = &appExecutive;

static nrf_drv_pwm_t pwmTherapy = NRF_DRV_PWM_INSTANCE(MS_APP_THERAPY_PWM_ID);
static nrf_pwm_values_common_t sequence_0_values[] = {THERAPHY_PWM_DUTY_50_VALUE};
static nrf_pwm_values_common_t sequence_1_values[] = {THERAPHY_PWM_DUTY_00_VALUE};
static nrf_drv_pwm_config_t pwmConfig = { // descrips PWM config
          .output_pins  = { 
                            THERAPHY_PWM_OUT_GPIO_PIN,// | NRF_DRV_PWM_PIN_INVERTED, 
                            NRF_DRV_PWM_PIN_NOT_USED,
                            NRF_DRV_PWM_PIN_NOT_USED,
                            NRF_DRV_PWM_PIN_NOT_USED 
                          },
           .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
           .base_clock   = (nrf_pwm_clk_t)NRF_PWM_CLK_125kHz,
           .count_mode   = (nrf_pwm_mode_t)NRF_PWM_MODE_UP,
           .top_value    = THERAPHY_PWM_COUNTERTOP,
           .load_mode    = NRF_PWM_LOAD_COMMON, //NRF_PWM_LOAD_INDIVIDUAL,
           .step_mode    = NRF_PWM_STEP_AUTO};

/**
 * @brief Function to get the handle of Executive
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
APP_EXECUTIVE* msApp_EXEC_GetHandle()
{
  return pExecutive;
}

/**
 * @brief Function to initialize and start Executive instance, such as load data from flash memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msApp_EXEC_Start()
{
  //TODO,
  // 1. load system date information in m41t62 rtc and set system time
  // 2. load device name from Flash Memory
  msOS_MEMSet(appExecutive.m_alias, '\0', MS_BLE_NAME_STRING_MAX_LENGTH);
  msOS_MEMCopy(appExecutive.m_alias, DEVICE_NAME, strlen(DEVICE_NAME));
  // 3. load system maintenance information
  // 4. load therapy history information
  msAPP_GPIOTE_Initialize();

  /// Initializes the dynamic memory manager and task scheduler
  msOS_MEMSetHandle(0, APP_DYNAMIC_ALLOC_MEMORY_SIZE);
  msSHD_Start(NULL, NULL, MS_SCH_DAEMON_PERIOD_IN_MIILISECOND, 0, 0);

  //TODO, 2) and 3) is used in BLE ADVERTISING INFORMATION (ADV Packet Data, DIS, ..) 
  msTSK_Start(TSK_REQUEST_EXECUTIVE_ID, 
          MS_TASK_PERIODIC, 
          3, 250, 
          msMAKEUUID('L', 'E', 'D', '1', 1), 
          0, 0, 
          msAPP_GPIOTE_LED_OUT_EventHandler);
}

/**
 * @brief Function to handle SAADC control TASK (using msTSK scheduler)
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msSAADC_AppTerminate(U64 uuid, I32 event, U64 user)
{
  switch(event) {
  case MS_SAADC_EVENT_UNINIT:
    msSAADC2_Uninitialize();
    break;
  case MS_SAADC_EVENT_START:
    msSAADC2_Start();
    break;
  case MS_SAADC_EVENT_STOP:
    msSAADC2_Stop();
    break;
  }
}

/**
 * @brief Function to system power on
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_PWR_TurnOn(U32 pin) 
{
  nrf_gpio_pin_set(pin);         // set power hold as high, thus pown on the system
  appExecutive.m_power.state = MS_POWER_ON;
} 

/**
 * @brief Function to system power off
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_PWR_TurnOff(U32 pin) 
{
  nrf_gpio_pin_clear(pin);
  appExecutive.m_power.state = MS_POWER_OFF;
} 

/**
 * @brief Function to handle sysmte power off
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_PWR_ShutDownHandler(U64 uuid, I32 event, U64 user)
{
  //TODO notify termination to smartphones
  // save log information to Flash Memory
  switch(event) {
  case MS_POWER_DOWNING:
    #if defined(MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS)
      #if MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS > 0
        if (!nrf_gpio_pin_read(PWR_BUTTON)) { // POWER-OFF if PWR_BUTTON is LOW
          while(true) {
            if (nrf_gpio_pin_read(PWR_BUTTON)) {
              msApp_PWR_TurnOff(PWR_HOLD);
            }
            nrf_delay_ms(1);
          }
        } else {
          appExecutive.m_power.state = MS_POWER_ON;
        }
      #endif
    #else
      msApp_PWR_TurnOff(PWR_HOLD);    // power off immediately
    #endif

    break;
  default:
    break;
  }
}

/**
 * @brief Function to initialize Power Hold, Power On/Off ans Status LEDs
 *        and set initial status.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msAPP_GPIOTE_Initialize() 
{
  NRF_UICR->NFCPINS = 0xFFFFFFFE;     // change the settings of the NFC muxed pin to GPIO pin, disable NFC (muxed with GPIOS)

  nrf_gpio_cfg_output(PWR_HOLD);      // PWR_HOLD gpio configures as the output port
  msApp_PWR_TurnOn(PWR_HOLD);         // power on

  // register power button interrupt handler
  nrf_drv_gpiote_in_config_t pwrconfig = GPIOTE_CONFIG_IN_SENSE_HITOLO(true); // triggers the interrupt at falling edge 
  pwrconfig.pull = NRF_GPIO_PIN_NOPULL;
  U32 status = nrf_drv_gpiote_in_init(PWR_BUTTON, &pwrconfig, msApp_POWER_EventTaskHandler);
  APP_ERROR_CHECK(status);
  nrf_drv_gpiote_in_event_enable(PWR_BUTTON, true); // enable the button interrupt events

  nrf_gpio_cfg_output(LED_R_FNC_OPSTATE);     // set operating status leds as high (ON)
  nrf_gpio_cfg_output(LED_G_FNC_OPSTATE);
  nrf_gpio_cfg_output(LED_R_BLE_CONNECTED);   // set BLE connected status leds 
  nrf_gpio_cfg_output(LED_G_BLE_CONNECTED);

  nrf_gpio_pin_set(LED_R_FNC_OPSTATE);
  nrf_gpio_pin_set(LED_G_FNC_OPSTATE);
  nrf_gpio_pin_set(LED_R_BLE_CONNECTED);
  nrf_gpio_pin_set(LED_G_BLE_CONNECTED);

  appExecutive.m_therapy.mode = THERAPHY_UNDEFINED;

  nrf_drv_gpiote_in_config_t teraconfig = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true); 
  teraconfig.pull = NRF_GPIO_PIN_NOPULL;
  status = nrf_drv_gpiote_in_init(BTN_STAND_THERAPHY, &teraconfig, msGPIOTE_TERA_StandardEventHandler);  // standard mode
  APP_ERROR_CHECK(status);
  nrf_drv_gpiote_in_event_enable(BTN_STAND_THERAPHY, true);

  // set theraphy short button interrupt handler
  teraconfig.pull = NRF_GPIO_PIN_NOPULL;
  status = nrf_drv_gpiote_in_init(BTN_SHORT_THERAPHY, &teraconfig, msGPIOTE_TERA_ShortEventHandler);  // short mode
  APP_ERROR_CHECK(status);
  nrf_drv_gpiote_in_event_enable(BTN_SHORT_THERAPHY, true);
}

/**
 * @brief Function to handle application level LEDs control task
 *
 * LED Scenario
 * boot for 2 seconds
 *
 * pressure state condition check (SS_CAN_OPERATE, SS_IS_AMBIGUOUS, SS_MAYBE_ABNORMAL, SS_DEFINITELY_ABNORMAL)
 * NORMAL - SS_CAN_OPERATE
 * ABNORMAL - SS_IS_AMBIGUOUS, SS_MAYBE_ABNORMAL, SS_DEFINITELY_ABNORMAL
 *
 * bio state condition check (SS_CAN_OPERATE, SS_DEFINITELY_ABNORMAL)
 * NORMAL - SS_CAN_OPERATE
 * ABNORMAL - SS_DEFINITELY_ABNORMAL
 *
 * therapy mode check (THERAPHY_UNDEFINED, THERAPHY_STANDARD, THERAPHY_SHORT)
 * SHORT MODE is MAX 2 times a day
 * STANDARD MODE can be treated regardless of pressure, limited number of times X
 *
 * battery state condition check (BS_IS_FULL_CHARGED, BS_CAN_OPERATE, BS_GOOD_TO_SLEEP, BS_SHUTDOWN_INPRORESS)
 * NORMAL - BS_IS_FULL_CHARGED, BS_CAN_OPERATE (30% ~ 100%)
 * AMBIGUOUS - BS_GOOD_TO_SLEEP                (10% ~ 30%)
 * ABNORMAL - BS_SHUTDOWN_INPRORESS            ( 0% ~ 10%)
 *
 * ble comport state check (CS_CONNECTED, CS_DISCONNECTED)
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 

void msAPP_GPIOTE_LED_OUT_EventHandler(U64 UUID, I32 event, U64 user) 
{     
  static I32 led_blinking_after_booting = 0;   
  
  if(led_blinking_after_booting < 5) {
    nrf_gpio_pin_clear(LED_R_FNC_OPSTATE);
    nrf_gpio_pin_clear(LED_R_BLE_CONNECTED);
    nrf_gpio_pin_toggle(LED_G_FNC_OPSTATE);     
    nrf_gpio_pin_toggle(LED_G_BLE_CONNECTED);
    led_blinking_after_booting++;
    return;
  }     
  switch(appExecutive.m_pressure.state.condition) {
  case SS_UNKNOWN: break; 
  case SS_CAN_OPERATE:                     
    switch(appExecutive.m_bio.state.condition) {  
    case SS_CAN_OPERATE:  
      if (appExecutive.m_therapy.mode == THERAPHY_UNDEFINED) {
        msLED_OnNormalState();
      } else if(appExecutive.m_therapy.mode == THERAPHY_STANDARD) {
        msLED_StandardTherapyState();          
      } else if(appExecutive.m_therapy.mode == THERAPHY_SHORT) {
        msLED_ShortTherapyState();     
      }
      break;
    default:
      if (appExecutive.m_therapy.mode == THERAPHY_UNDEFINED) {
        msLED_OnAbnormalState();
      } else if(appExecutive.m_therapy.mode == THERAPHY_STANDARD) {
        msLED_StandardTherapyState();         
      } else if(appExecutive.m_therapy.mode == THERAPHY_SHORT) {
        msLED_ShortTherapyState();           
      }
      break;
    } 
    break;
  default:  
    if (appExecutive.m_therapy.mode == THERAPHY_UNDEFINED) {
      msLED_OnPressureAbnormalState();
    } else if(appExecutive.m_therapy.mode == THERAPHY_STANDARD) {
      msLED_StandardTherapyState();          
    } else if(appExecutive.m_therapy.mode == THERAPHY_SHORT) {
      msLED_ShortTherapyState();           
    }
  }

  switch(appExecutive.m_power.battery.state) {  
  case BS_IS_FULL_CHARGED:
  case BS_CAN_OPERATE:   
    if (appExecutive.m_comport.state == CS_CONNECTED) {          
      msLED_OnBatteryCanOperateConnectedState();
    } else {
      msLED_OnBatteryCanOperateDisconnectedState(); 
    }
    break;
  case BS_GOOD_TO_SLEEP:     
    if (appExecutive.m_comport.state == CS_CONNECTED) {
      msLED_OnBatteryGoodSleepConnectedState(); 
    } else {
      msLED_OnBatteryGoodSleepDisconnectedState(); 
    }
    break;
  case BS_SHUTDOWN_INPRORESS:
    if (appExecutive.m_comport.state == CS_CONNECTED) {
      msLED_OnBatteryShutdownInProgressConnectedState(); 
    } else {
      msLED_OnBatteryShutdownInProgressDisconnectedState();
    }
    break;
  }
}


/**
 * @brief Function to handle the system power on or off event
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_POWER_EventTaskHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
  U32 expired = 0; // power off immediately
#if defined(MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS)
  expired = MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS;
#endif

  switch(appExecutive.m_power.state) { 
  case MS_POWER_ON:
    appExecutive.m_power.state = MS_POWER_DOWNING;
    msTSK_Start(TSK_REQUEST_EXECUTIVE_ID,
            MS_TASK_ONESHORT,
            2, 
            expired, 
            msMAKEUUID('P', 'W', 'R', 'D', 'N'),
            0, 
            MS_POWER_DOWNING,
            msApp_PWR_ShutDownHandler
            );
    break;
  case MS_POWER_OFF:
    appExecutive.m_power.state = MS_POWER_ON;
    break;
  default:
    break;
  }
}

/**
 * @brief Function to handle the system power on or off event from Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_PowerEventRecvHandler(U64 uuid, I32 event, U64 user)
{ 
  if (appExecutive.m_power.state == user) return; 
  nrf_gpiote_polarity_t action = GPIOTE_CONFIG_POLARITY_LoToHi;
  msApp_POWER_EventTaskHandler((nrf_drv_gpiote_pin_t)appExecutive.m_power.battery.pin, action);
}

/**
 * @brief Function to receive and handle Date from Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_RTC_AccessTaskHandler(U64 uuid, I32 event, U64 user)
{
  switch(event) {
  case MS_TASK_OP_READ:
    M41T62_TWI_Initialize(RTC_SCL, RTC_SDA, NRF_DRV_TWI_FREQ_400K, NULL, 0); 
    M41T62_TWI_ReadDate(&appExecutive.m_date);
    M41T62_TWI_Uninitialize();
    return;
  case MS_TASK_OP_WRITE:
    M41T62_TWI_Initialize(RTC_SCL, RTC_SDA, NRF_DRV_TWI_FREQ_400K, NULL, 0); 
    M41T62_TWI_WriteDate(&appExecutive.m_date);
    M41T62_TWI_ReadDate(&appExecutive.m_date);
    msDATE_Initialize(&appExecutive.m_date);
    M41T62_TWI_Uninitialize();
    return;
  default:
    break;
  }
}

/**
 * @brief Function to capture BIO sensor's sampling signal
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_BioSensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user)
{
  static I16 id = 0;

  if (appExecutive.m_comport.state == CS_CONNECTED) {
    if (appExecutive.m_pressure.state.condition != SS_CAN_OPERATE) {
      msOS_MEMSet(pArray, 0, MS_APP_BLE_BIO_SIGNAL_SIZE);
    } 
    msApp_BLE_BioSensorEventSendHandler(0,  MS_APP_BLE_BIO_SIGNAL_SIZE, (U64)(U32)pArray);
  }
}

/**
 * @brief Function to capture Pressure sensor's sampling signal
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_PressureSensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user)
{
  I32 pressure = 0;

  for (U32 i = 0; i < nArray; i++) {
    pressure += msMAXC(0, pArray[i]); 
  }
  pressure /= nArray;

  if (pressure > MS_APP_PRESSURE_NORMAL_THRESHOLD) {
    appExecutive.m_pressure.state.condition = SS_CAN_OPERATE;
  } else if (pressure > MS_APP_PRESSURE_AMBIGUOUS_THRESHOLD) {
    appExecutive.m_pressure.state.condition = SS_IS_AMBIGUOUS;
  } else if (pressure > MS_APP_PRESSURE_ABNORMAL_THRESHOLD) {
    appExecutive.m_pressure.state.condition = SS_MAYBE_ABNORMAL;
  } else {
    appExecutive.m_pressure.state.condition = SS_DEFINITELY_ABNORMAL;
  }
  appExecutive.m_pressure.state.value = pressure;
}

/**
 * @brief Function to capture Battery voltage sensor's sampling signal
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_BatterySensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user)
{
  I32 batteryLevel = 0;

  for (U32 i = 0; i < nArray; i++) {
    batteryLevel += pArray[i];
  }
  batteryLevel = ((batteryLevel / nArray) - MS_APP_BATTERY_MIN_ADC_VALUE) * 100 / (MS_APP_BATTERY_FULL_ADC_VALUE - MS_APP_BATTERY_MIN_ADC_VALUE);
  batteryLevel = msMINC(msMAXC(0, batteryLevel), 100);

  appExecutive.m_power.battery.level = batteryLevel;

  if (batteryLevel == appExecutive.m_power.lvlCtrl.normalMax) {
    appExecutive.m_power.battery.state = BS_IS_FULL_CHARGED;
  } 
  else if (batteryLevel > appExecutive.m_power.lvlCtrl.normalMin) {
    appExecutive.m_power.battery.state = BS_CAN_OPERATE;
  } 
  else if (batteryLevel > appExecutive.m_power.lvlCtrl.pwrSaving) {
    appExecutive.m_power.battery.state = BS_CAN_OPERATE;    
  } 
  else if (batteryLevel > appExecutive.m_power.lvlCtrl.shutDown) {
    appExecutive.m_power.battery.state = BS_GOOD_TO_SLEEP;
  } 
  else {
    appExecutive.m_power.battery.state = BS_SHUTDOWN_INPRORESS;
  }
}

/**
 * @brief Function to handle THERAPY termination
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_TERA_TerminateTaskHandler(U64 uuid, I32 event, U64 user) 
{
  U16 endInSecond;

  switch(event) {
  case MS_PWM_EVENT_STOP:
    msPWM_Uninitialize(MS_APP_THERAPY_PWM_ID);
    endInSecond = (U16)(msTSK_NowInMillisecond() / 1000);
    appExecutive.m_therapy.duration = endInSecond - appExecutive.m_therapy.duration;
    msApp_BLE_TherapyEventSendHandler(0, THERAPHY_STOPPED, 0);
    appExecutive.m_teralog.m_underTreatment = MS_FALSE;
    appExecutive.m_therapy.mode = THERAPHY_UNDEFINED;
    if (appExecutive.m_teralog.m_queued != THERAPHY_UNDEFINED) { 
      appExecutive.m_therapy.mode = appExecutive.m_teralog.m_queued;
      appExecutive.m_teralog.m_queued = THERAPHY_UNDEFINED;
      switch(appExecutive.m_therapy.mode) {
      case THERAPHY_STANDARD:
        msGPIOTE_TERA_StandardEventHandler(THERAPHY_PWM_OUT_GPIO_PIN, GPIOTE_CONFIG_POLARITY_LoToHi);
        break;
      case THERAPHY_SHORT:
        msGPIOTE_TERA_ShortEventHandler(THERAPHY_PWM_OUT_GPIO_PIN, GPIOTE_CONFIG_POLARITY_LoToHi);
        break;
      }
    }
    break;
  default:
    break;
  }
}

/**
 * @brief Function to handle THERAPY STANDARD treatment mode
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msGPIOTE_TERA_StandardEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
  if (appExecutive.m_teralog.m_underTreatment == MS_TRUE) {
    return;
  }
  msPWM_Uninitialize(MS_APP_THERAPY_PWM_ID);

  //TODO !! save this to Log
  nrf_pwm_sequence_t sequence[MS_PWM_SEQUENCE_MAX_ID] = {         // descrips sequences of PWM
          { 
            .values.p_common = sequence_0_values,                 // pointer to an array with duty cycle values. this array must be in Data RAM.
            .length = NRF_PWM_VALUES_LENGTH(sequence_0_values),   // number of 16-bit values in the array pointed by @p values.
            .repeats = THERAPHY_STAND_DUTY_50_REPEATS_PER_CYCLE,  // number of times that each duty cycle is to be repeated (after being played once).
            .end_delay = 0                                        // additional time (in PWM periods) that the last duty cycle is to be kept after the sequence is played.
          },
          { 
            .values.p_common = sequence_1_values,
            .length = NRF_PWM_VALUES_LENGTH(sequence_1_values),
            .repeats = THERAPHY_STAND_DUTY_OO_REPEATS_PER_CYCLE,
            .end_delay = 0
          },
        };

  msPWM_Initialize(MS_APP_THERAPY_PWM_ID,
          &pwmTherapy, 
          &pwmConfig,
          sequence, 
          2,
          THERAPHY_TOTAL_CYCLE,
          NRF_DRV_PWM_FLAG_LOOP | NRFX_PWM_FLAG_STOP | NRFX_PWM_FLAG_SIGNAL_END_SEQ0 | NRFX_PWM_FLAG_SIGNAL_END_SEQ1,
          msApp_TERA_TerminateTaskHandler, 
          0); 

  appExecutive.m_teralog.m_underTreatment = MS_TRUE;
  appExecutive.m_therapy.number++;
  appExecutive.m_therapy.mode = THERAPHY_STANDARD;
  appExecutive.m_therapy.tstart = msDATE_GetCurrentTime();
  appExecutive.m_therapy.duration = (U16)(msTSK_NowInMillisecond() / 1000);
  msApp_BLE_TherapyEventSendHandler(0, THERAPHY_STARTED, 0); 
  msPWM_Start(MS_APP_THERAPY_PWM_ID, THERAPHY_STAND_TOTAL_TIME_IN_MILLISECOND); 
}

/**
 * @brief Function to handle THERAPY SHORT treatment mode
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msGPIOTE_TERA_ShortEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
  //TODO !! log therapy log and compare with log's shrotPerDay  
  //if (appExecutive.m_teralog.m_shortPerDay > 2) {
  //  NRF_LOG_INFO("   short moe can only works twice per day, on working, %d", appExecutive.m_therapy.mode);
  //  //if (appExecutive.m_teralog.m_queued == THERAPHY_SHORT) { // reset if the queued mode is THERAPHY_SHORT
  //  //  appExecutive.m_teralog.m_queued = THERAPHY_UNDEFINED;
  //  //}
  //  return;
  //}
  if (appExecutive.m_teralog.m_underTreatment == MS_TRUE) {
    return;
  }
  msPWM_Uninitialize(MS_APP_THERAPY_PWM_ID);

  nrf_pwm_sequence_t sequence[MS_PWM_SEQUENCE_MAX_ID] = {         // descrips sequences of PWM
          { 
            .values.p_common = sequence_0_values,                 // pointer to an array with duty cycle values. this array must be in Data RAM.
            .length = NRF_PWM_VALUES_LENGTH(sequence_0_values),   // number of 16-bit values in the array pointed by @p values.
            .repeats = THERAPHY_SHORT_DUTY_50_REPEATS_PER_CYCLE,  // number of times that each duty cycle is to be repeated (after being played once).
            .end_delay = 0                                        // additional time (in PWM periods) that the last duty cycle is to be kept after the sequence is played.
          },
          { 
            .values.p_common = sequence_1_values,
            .length = NRF_PWM_VALUES_LENGTH(sequence_1_values),
            .repeats = THERAPHY_SHORT_DUTY_OO_REPEATS_PER_CYCLE,
            .end_delay = 0
          },
        };
  msPWM_Initialize(MS_APP_THERAPY_PWM_ID, 
          &pwmTherapy, 
          &pwmConfig,
          sequence,
          2,
          THERAPHY_TOTAL_CYCLE,
          NRF_DRV_PWM_FLAG_LOOP | NRFX_PWM_FLAG_STOP | NRFX_PWM_FLAG_SIGNAL_END_SEQ0 | NRFX_PWM_FLAG_SIGNAL_END_SEQ1,
          msApp_TERA_TerminateTaskHandler,
          0); 

  appExecutive.m_teralog.m_underTreatment = MS_TRUE;
  appExecutive.m_teralog.m_shortPerDay++;
  appExecutive.m_therapy.number++;
  appExecutive.m_therapy.mode = THERAPHY_SHORT;
  appExecutive.m_therapy.tstart = msDATE_GetCurrentTime();
  appExecutive.m_therapy.duration = (U16)(msTSK_NowInMillisecond() / 1000);
  msApp_BLE_TherapyEventSendHandler(0, THERAPHY_STARTED, 0); 
  msPWM_Start(MS_APP_THERAPY_PWM_ID, THERAPHY_SHORT_TOTAL_TIME_IN_MILLISECOND);
}

/**
 * @brief Function to send BIO sensor's sampling singals (with pressureLevel) to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_BioSensorEventSendHandler(U64 uuid, I32 size, U64 array)
{
  static U08 bioPacked[MS_APP_BLE_BIO_N_PRESS_SIGNAL_SIZE];
  I16* pArray = (I16*)(U32)array;

  if (appExecutive.m_comport.state != CS_CONNECTED || size != MS_APP_BLE_BIO_SIGNAL_SIZE) return;
  size += MS_APP_BLE_PRESSURE_STATE_SIZE;
  msBLE_APP_EncodeBioSignal(&appExecutive.m_pressure, pArray, (U08*)bioPacked, size);

  U16 send = size, offset = 0;
  msBLE_HVX_Write(msBLE_GetConnectionHandle(), 
            msBLE_GetBioServiceHandle()->charBioStateHandle.value_handle, 
            bioPacked, 
            0, 
            &send);
  if (send < size) {
    offset = send;
    send = size - send;
    msBLE_HVX_Write(msBLE_GetConnectionHandle(), 
            msBLE_GetBioServiceHandle()->charBioStateHandle.value_handle, 
            bioPacked + offset, 
            offset, 
            &send);
  }
}

/**
 * @brief Function to ping DateTime to Remote User
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
bool dateSendEnabled = false;
void msApp_BLE_DateEventSendHandler(U64 uuid, I32 event, U64 user)
{
  static U08  dtEncoded[MS_APP_BLE_DATETIME_PING_SIZE]; // 17 bytes
  static U32  pingTimes = 1;
  VDATEPING   ping;

  if (!dateSendEnabled) return;

  ping.stamp = msTSK_NowInMillisecond();
  msBLE_APP_EncodePingDateTime(&ping, dtEncoded, MS_APP_BLE_DATETIME_PING_SIZE);
  U16 send = MS_APP_BLE_DATETIME_PING_SIZE;
  msBLE_HVX_Write(msBLE_GetConnectionHandle(), 
            msBLE_GetBioServiceHandle()->charDateNoticeHandle.value_handle, 
            dtEncoded, 
            0, 
            &send); 
  
  if (++pingTimes > MS_APP_PING_DATETIME_SEND_TIMES) { 
    pingTimes = 1;
    dateSendEnabled = false;
    msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,
            msMAKEUUID('D', 'A', 'T', 'E', 'P'), 0, 0, NULL);
  }
}

/**
 * @brief Function to receive and set DateTime from Remote User
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_DateEventRecvHandler(U64 uuid, I32 size, U64 user)
{
  static I64  smallestTTL = 0xFFFFFFFFFF;
  U08*        pArray = (U08*)(U32)user;  
  I64         ttl;
  VDATEPING   ping;

  msBLE_APP_DecodePingDateTime(&ping, pArray, size);
  ttl = msTSK_NowInMillisecond() - ping.stamp - 300; // 200 is server timer delay

  if (ttl < smallestTTL) {
    smallestTTL = ttl;
    U64 millisecond = convertDateInMillisecond(&ping.date);
    millisecond += (U64)(ttl / 2 + 0.5);
    convertMillisecondInDate(millisecond, &ping.date);
    appExecutive.m_date = ping.date;
    msApp_RTC_AccessTaskHandler(0, MS_TASK_OP_WRITE, (U64)(U32)&ping.date);
  }
}

/**
 * @brief Function to receive BPM from remote user
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msApp_BLE_BioBpmEventRecvHandler(U64 uuid, I32 size, U64 user)
{
  U08* pArray = (U08*)(U32)user;

  msBLE_APP_DecodeBpmState(&appExecutive.m_bio, &appExecutive.m_respiration, &appExecutive.m_heart, pArray, size);
}

/**
 * @brief Function to send the current battery level to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_BatteryLevelEventSendHandler(U64 uuid, I32 size, U64 user)
{
  if (appExecutive.m_comport.state != CS_CONNECTED) return;
  U08 batteryLevel = appExecutive.m_battery.state.value;
  U16 send = MS_APP_BLE_BATTERY_LEVEL_SIZE;
  msBLE_APP_EncodeBatteryLevel(&appExecutive.m_power.battery, &batteryLevel, send);
  msBLE_HVX_Write(msBLE_GetConnectionHandle(), 
            msBLE_GetBioServiceHandle()->charBatteryStateHandle.value_handle, 
            &batteryLevel, 
            0, 
            &send); 
}

/**
 * @brief Function to send the therapy status to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_TherapyEventSendHandler(U64 uuid, I32 event, U64 user)
{
  static U08 trEncoded[MS_APP_BLE_THERAPY_STATE_SIZE];

  if (appExecutive.m_comport.state != CS_CONNECTED) return;

  VTHERAPHY therapy = appExecutive.m_therapy;
  if (event == THERAPHY_STARTED) { 
    therapy.duration = (therapy.mode == THERAPHY_STANDARD) ?
                              (U16)((U32)(THERAPHY_STAND_TOTAL_TIME_IN_MILLISECOND / 1000)) :
                              (U16)((U32)(THERAPHY_SHORT_TOTAL_TIME_IN_MILLISECOND / 1000));
    therapy.command = THERAPHY_STARTED;
  } else {
    therapy.command = THERAPHY_STOPPED;
  }

  appExecutive.m_therapy.command = therapy.command;
  msBLE_APP_EncodeTherapyChoice(&appExecutive.m_therapy, trEncoded, MS_APP_BLE_THERAPY_STATE_SIZE);

  U16 send = MS_APP_BLE_THERAPY_STATE_SIZE;
  msBLE_HVX_Write(msBLE_GetConnectionHandle(), 
            msBLE_GetBioServiceHandle()->charThreapyChoiceHandle.value_handle, 
            trEncoded, 
            0, 
            &send);
}

/**
 * @brief Function to handle Theraphy event from Remote User
 *        schedule the next run, one of short or standard, if reservations overlap, do the most recently requested one.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_TherapyEventRecvHandler(U64 uuid, I32 size, U64 user)
{
  U08* pArray = (U08*)(U32)user;

  msBLE_APP_DecodeTherapyChoice(&appExecutive.m_therapy, pArray, size);

  if (appExecutive.m_teralog.m_underTreatment == MS_TRUE) {
    return;
  }

  switch(appExecutive.m_therapy.mode) {
  case THERAPHY_STANDARD:
    msGPIOTE_TERA_StandardEventHandler(THERAPHY_PWM_OUT_GPIO_PIN, GPIOTE_CONFIG_POLARITY_LoToHi);
    break;
  case THERAPHY_SHORT:
    msGPIOTE_TERA_ShortEventHandler(THERAPHY_PWM_OUT_GPIO_PIN, GPIOTE_CONFIG_POLARITY_LoToHi);
    break;
  }
}

/**
 * @brief Function to handle Alias of Device Name event from Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_AliasNameEventRecvHandler(U64 uuid, I32 size, U64 user)
{
  U08* pArray = (U08*)(U32)user;
  I32 i;

  size = msMINC(size, MS_BLE_NAME_STRING_MAX_LENGTH);
  for (i = 0; i < size; i++) {
    appExecutive.m_alias[i] = pArray[i];
  }
  for (; i < MS_BLE_NAME_STRING_MAX_LENGTH; i++) {
    appExecutive.m_alias[i] = '\0';
  }
  //TODO, write alias name to flash memory !!
}

/**
 * @brief Function to set the advertising data, e.g. device name, battery level, therapy mode
 *        It allows you to know the device status even when the device is not connected.
 *        This can be useful when servicing multiple devices at the same time in a smartphone app.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_UpdateAdvertiseHandler(U64 uuid, I32 size, U64 user)
{
  if (appExecutive.m_comport.state == CS_WAITING_FOR_CONNECTION ||
      appExecutive.m_comport.state == CS_DISCONNECTED) {
    msBLE_ADVT_UpdateData(appExecutive.m_alias, 
            appExecutive.m_battery.state.value, appExecutive.m_therapy.mode);
  }
}

/**
 * @brief Function to handle the connected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnAdevertiseHandler(U16 connectionHandle, U32 user) 
{
  appExecutive.m_comport.cid = msBLE_GetConnectionHandle();
  appExecutive.m_comport.state = CS_WAITING_FOR_CONNECTION;
  convertMillisecondInDate(0, &appExecutive.m_comport.connected);
  convertMillisecondInDate(0, &appExecutive.m_comport.disconnected);

  //TODO, periodically update advertising data
  msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,              // if it is rechecked whether UpdateAdvertiseTask exists, it is deleted 
          msMAKEUUID('A', 'D', 'V', 'U', 'P'), 0, 0, NULL);
  msTSK_Start(TSK_REQUEST_EXECUTIVE_ID, 
          MS_TASK_PERIODIC, 
          5, MS_APP_ADVERTISING_DATA_UPDATE_PERIOD, // send every 30 seconds
          msMAKEUUID('A', 'D', 'V', 'U', 'P'),
          0, // user 
          0, // event
          msApp_BLE_UpdateAdvertiseHandler);
}

/**
 * @brief Function to handle the connected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnConnectHandler(U16 connectionHandle, U32 user) 
{
  appExecutive.m_comport.cid = msBLE_GetConnectionHandle();
  appExecutive.m_comport.state = CS_CONNECTED;
  appExecutive.m_comport.connected = msDATE_GetCurrentTime();
  convertMillisecondInDate(0, &appExecutive.m_comport.disconnected);

  msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,             
          msMAKEUUID('A', 'D', 'V', 'U', 'P'), 0, 0, NULL);
  msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,            
          msMAKEUUID('B', 'A', 'T', 'R', 'T'), 0, 0, NULL);
  msTSK_Start(TSK_REQUEST_EXECUTIVE_ID, 
          MS_TASK_PERIODIC, 
          5, MS_APP_BATTERY_REPORT_PERIOD,
          msMAKEUUID('B', 'A', 'T', 'R', 'T'),
          0, 
          0, 
          msApp_BLE_BatteryLevelEventSendHandler);

  msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,
          msMAKEUUID('D', 'A', 'T', 'E', 'P'), 0, 0, NULL);
  msTSK_Start(TSK_REQUEST_EXECUTIVE_ID, 
          MS_TASK_PERIODIC, 
          2, MS_APP_PING_DATETIME_PERIOD,
          msMAKEUUID('D', 'A', 'T', 'E', 'P'),
          0,
          MS_TASK_OP_READ, 
          msApp_BLE_DateEventSendHandler);
}

/**
 * @brief Function to handle the disconnected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnDisconnectHandler(U16 connectionHandle, U32 user) 
{
  appExecutive.m_comport.cid = msBLE_GetConnectionHandle();
  appExecutive.m_comport.state = CS_DISCONNECTED;
  appExecutive.m_comport.disconnected = msDATE_GetCurrentTime();

  dateSendEnabled = false;
  msTSK_Stop(TSK_REQUEST_EXECUTIVE_ID,
          msMAKEUUID('B', 'A', 'T', 'R', 'T'), 0, 0, NULL);
}

/**
 * @brief Function to handle the written(updated) characterisitics event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static U08 pool[48];
void msApp_COM_fnWrittenHandler(ble_gatts_evt_write_t const* pWrite)
{
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charBpmStateHandle.value_handle) {
    return msApp_BLE_BioBpmEventRecvHandler(msFOURCC('U', 'S', 'E', 'R'), 
                  pWrite->len, (U64)(U32)pWrite->data);
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charThreapyChoiceHandle.value_handle) {
    return msApp_BLE_TherapyEventRecvHandler(msFOURCC('U', 'S', 'E', 'R'), 
                  pWrite->len, (U64)(U32)pWrite->data);  
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charDateNoticeHandle.value_handle) {
    return msApp_BLE_DateEventRecvHandler(msFOURCC('U', 'S', 'E', 'R'), 
                  pWrite->len, (U64)(U32)pWrite->data);
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charAliasNameHandle.value_handle) {
    return msApp_BLE_AliasNameEventRecvHandler(msFOURCC('U', 'S', 'E', 'R'), 
                  pWrite->len, (U64)(U32)pWrite->data);
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charBatteryStateHandle.cccd_handle) {
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    return;
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charBioStateHandle.cccd_handle) { // for future use, allow notification
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    return;
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charBpmStateHandle.cccd_handle) { // for future use, allow notification
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    return;
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charDateNoticeHandle.cccd_handle) { // for future use, allow notification
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    dateSendEnabled = notifyingEnabled;
    return;
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charThreapyChoiceHandle.cccd_handle) {
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    return;
  }
  if (pWrite->handle == msBLE_GetBioServiceHandle()->charTherapyLogHandle.cccd_handle) {
    bool notifyingEnabled = ble_srv_is_notification_enabled(pWrite->data);
    return;
  }
}


#if defined (__cplusplus)
}
#endif