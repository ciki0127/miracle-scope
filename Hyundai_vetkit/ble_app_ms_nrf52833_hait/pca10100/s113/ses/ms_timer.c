/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_timer.c							                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_timer.h"

#if defined (__cplusplus)
extern "C" {
#endif

/// ticks = (time_millis_second * 16000000) / prescaler, prescaler = 2^config.frequency
/// milli_second = prescaler * ticks / 16000000;

/**
 * @brief Function for configuring the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Configure(nrf_drv_timer_config_t *p_config, 
              nrf_timer_frequency_t frequency, 
              nrf_timer_mode_t mode, 
              nrf_timer_bit_width_t bit_width, 
              uint8_t priority, 
              void* p_context) 
{
  p_config->frequency = frequency;
  p_config->mode = mode,
  p_config->bit_width = bit_width;
  p_config->interrupt_priority = priority;
  p_config->p_context = p_context;
}

/**
 * @brief Function for configuring the timer as default setting.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_DefaultConfigure(nrf_drv_timer_config_t *p_config) 
{
  p_config->frequency = (nrf_timer_frequency_t)TIMER_DEFAULT_CONFIG_FREQUENCY; 
  p_config->mode = (nrf_timer_mode_t)NRFX_TIMER_DEFAULT_CONFIG_MODE;
  p_config->bit_width = (nrf_timer_bit_width_t)NRFX_TIMER_DEFAULT_CONFIG_BIT_WIDTH;
  p_config->interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY;
  p_config->p_context = NULL;
}


/**
 * @brief Timer event handler.
 */
static void empty_timer_handler(nrf_timer_event_t event_type, void * p_context)
{
  switch(event_type) {
  case NRF_TIMER_EVENT_COMPARE0: break;

  default: break;
  }
}


/**
 * @brief Function to check wheter the specified timer is enabled.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
bool msTimer_IsEnable(nrf_drv_timer_t const *timer)
{
  return nrf_drv_timer_is_enabled(timer);
}


/**
 * @brief Function to get the overflow maximum value of the timer counter.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
uint64_t msTimer_CounterOverflowValue(nrf_drv_timer_config_t *pconfig)
{
  if (pconfig == NULL) return 0;
  switch(pconfig->bit_width) {
  case 3: return 0x100000000; //32
  case 2: return 0x1000000;   //24
  case 1: return 0x100;       //8
  default: return 0x10000;    //16
  }
}



/**
 * @brief Function for initiaizing the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Initialize(nrf_drv_timer_t const *timer,
              nrf_drv_timer_config_t *p_config,
              bool enable_int, 
              nrf_timer_event_handler_t handler) 
{
  if (enable_int == true && handler == NULL) handler = empty_timer_handler;
  ret_code_t err_code = nrf_drv_timer_init(timer, p_config, handler);
  APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for uninitiaizing the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Uninitialize(nrf_drv_timer_t const *timer) 
{
  nrf_drv_timer_uninit(timer);
}


/**
 * @brief Function for creating the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicCreate(nrf_drv_timer_t const *timer,
              nrf_drv_timer_config_t *p_config,
              nrf_timer_cc_channel_t cc_channel, 
              uint32_t tm_interval,
              bool is_millis_interval, 
              bool enable_int, 
              nrf_timer_event_handler_t handler) 
{
  msTimer_Initialize(timer, p_config, enable_int, handler);
  msTimer_PeriodicExtendedAdd(timer, cc_channel, tm_interval, is_millis_interval, enable_int);
}


/**
 * @brief Function for deleteing the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicDelete(nrf_drv_timer_t const *timer, bool try_disable) 
{
  msTimer_Uninitialize(timer);
}


/**
 * @brief Function for adding the compare channel of the specified timer.
 *        Interrupt handler in msTimer_Initialize or msTimer_PeriodicCreate() should be assigned if you want to get an intrrupt event.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicExtendedAdd(nrf_drv_timer_t const *timer,
              nrf_timer_cc_channel_t cc_channel, 
              uint32_t tm_interval,
              bool is_millis_interval,
              bool enable_int)
{
  nrf_timer_short_mask_t  timer_short_mask;
  if (cc_channel == NRF_TIMER_CC_CHANNEL0) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL1) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL2) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL3) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK;
  }
  nrf_drv_timer_extended_compare(timer,
                                 cc_channel,
                                 (is_millis_interval == true) ? nrf_drv_timer_ms_to_ticks(timer, tm_interval) : nrf_drv_timer_us_to_ticks(timer, tm_interval),
                                 timer_short_mask,
                                 enable_int);
}


/**
 * @brief Function to enable the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Enable(nrf_drv_timer_t const *timer) 
{
  if (nrf_drv_timer_is_enabled(timer) != true) {
    nrf_drv_timer_enable(timer);
  }
}


/**
 * @brief Function to disable the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Disable(nrf_drv_timer_t const *timer)
{
  if (nrf_drv_timer_is_enabled(timer) == true) {
    nrf_drv_timer_disable(timer);
  }
}


/**
 * @brief Function to pause the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicPause(nrf_drv_timer_t const *timer)
{
  if (nrf_drv_timer_is_enabled(timer) == true) {
    nrf_drv_timer_pause(timer);
  }
}


/**
 * @brief Function to resume the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicResume(nrf_drv_timer_t const *timer)
{
  nrf_drv_timer_resume(timer);
}


/**
 * @brief Function to clear the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Clear(nrf_drv_timer_t const *timer, nrf_timer_cc_channel_t cc_channel)
{
  nrf_drv_timer_compare_int_disable(timer, cc_channel);
  nrf_drv_timer_clear(timer);
  nrf_drv_timer_compare_int_enable(timer, cc_channel);
}

/**
 * @brief Function to clear the specified channel's event of  the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_EventClear(nrf_drv_timer_t const *timer, nrf_timer_cc_channel_t cc_channel)
{
  nrf_timer_event_clear(timer->p_reg, nrf_timer_compare_event_get(cc_channel));
}


/**
 * @brief Function for creating the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_OneshotCreate(nrf_drv_timer_t const *timer,
              nrf_drv_timer_config_t *p_config,
              nrf_timer_cc_channel_t cc_channel, 
              uint32_t tm_interval,
              bool is_millis_interval, 
              bool enable_int, 
              nrf_timer_event_handler_t handler) 
{
  msTimer_Initialize(timer, p_config, enable_int, handler);
  msTimer_OneshotAdd(timer, cc_channel, tm_interval, is_millis_interval, enable_int);
}

/**
 * @brief Function for adding the compare channel of the specified timer with MASK value.
 *        Interrupt handler in msTimer_Initialize or msTimer_OneshotCreate() should be assigned if you want to get an intrrupt event.    
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_OneshotAdd(nrf_drv_timer_t const *timer,
              nrf_timer_cc_channel_t cc_channel, 
              uint32_t tm_interval,
              bool is_millis_interval, 
              bool enable_int)
{
  nrf_timer_short_mask_t  timer_short_mask;

  if (cc_channel == NRF_TIMER_CC_CHANNEL0) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE0_STOP_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL1) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE1_STOP_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL2) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE2_STOP_MASK;
  } else if (cc_channel == NRF_TIMER_CC_CHANNEL3) {
    timer_short_mask = NRF_TIMER_SHORT_COMPARE3_STOP_MASK;
  }
  nrf_drv_timer_extended_compare(timer,
                                 cc_channel,
                                 (is_millis_interval == true) ? nrf_drv_timer_ms_to_ticks(timer, tm_interval) : nrf_drv_timer_us_to_ticks(timer, tm_interval),
                                 timer_short_mask,
                                 enable_int);
}


/**
 * @brief Function for adding the compare channel of the specified timer without MASK value.
 *        Interrupt handler in msTimer_Initialize or msTimer_OneshotCreate() should be assigned if you want to get an intrrupt event. 
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_OneshotAddCanRepeat(nrf_drv_timer_t const *timer,
              nrf_timer_cc_channel_t cc_channel, 
              uint32_t tm_interval,
              bool is_millis_interval, 
              bool enable_int)
{
  nrf_drv_timer_compare(timer,
                        cc_channel,
                        (is_millis_interval == true) ? nrf_drv_timer_ms_to_ticks(timer, tm_interval) : nrf_drv_timer_us_to_ticks(timer, tm_interval),
                        enable_int);
}
#if defined (__cplusplus)
}
#endif