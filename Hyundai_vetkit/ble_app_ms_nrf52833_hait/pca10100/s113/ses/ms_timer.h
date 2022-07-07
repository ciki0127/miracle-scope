/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_timer.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_TIMER_H
#define MS_TIMER_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_drv_timer.h"
#include "app_error.h"

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to check wheter the specified timer is enabled.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
bool msTimer_IsEnable(nrf_drv_timer_t const *timer);

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
              void* p_context);

/**
 * @brief Function for configuring the timer as default setting.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_DefaultConfigure(nrf_drv_timer_config_t *p_config);

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
              nrf_timer_event_handler_t handler);

/**
 * @brief Function for uninitiaizing the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Uninitialize(nrf_drv_timer_t const *timer);

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
              nrf_timer_event_handler_t handler);
         
/**
 * @brief Function for deleteing the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicDelete(nrf_drv_timer_t const *timer, bool try_disable);

/**
 * @brief Function for adding the compare channel of the specified timer.
 *        Interrupt handler in msTimer_Periodic_extended_create() should be assigned if you want to get an intrrupt event.
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
              bool enable_int);

/**
 * @brief Function to get the overflow maximum value of the timer counter.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
uint64_t msTimer_CounterOverflowValue(nrf_drv_timer_config_t *p_config);


/**
 * @brief Function to enable the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Enable(nrf_drv_timer_t const *timer); 


/**
 * @brief Function to disable the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Disable(nrf_drv_timer_t const *timer);


/**
 * @brief Function to pause the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicPause(nrf_drv_timer_t const *timer);


/**
 * @brief Function to resume the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_PeriodicResume(nrf_drv_timer_t const *timer);


/**
 * @brief Function to clear the specified timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_Clear(nrf_drv_timer_t const *timer, nrf_timer_cc_channel_t cc_channel);


/**
 * @brief Function to clear the specified channel's event of  the timer.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTimer_EventClear(nrf_drv_timer_t const *timer, nrf_timer_cc_channel_t cc_channel);


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
              nrf_timer_event_handler_t handler);

/**
 * @brief Function for adding the compare channel of the specified timer.
 *        Interrupt handler in msTimer_OneshotCreate() should be assigned if you want to get an intrrupt event.
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
              bool enable_int);


/**
 * @brief Function for adding the compare channel of the specified timer without MASK value.
 *        
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
              bool enable_int);

#if defined (__cplusplus)
}
#endif
#endif // MS_TIMER_H