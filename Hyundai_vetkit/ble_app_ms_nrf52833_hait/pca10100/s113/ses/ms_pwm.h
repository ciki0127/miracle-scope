/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_pwm.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/30 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_PWM_H
#define MS_PWM_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_drv_pwm.h"
#include "app_error.h"
#include "ms_app_boards.h"
#include "ms_task.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#if defined (__cplusplus)
extern "C" {
#endif

#define MS_PWM_SEQUENCE_MAX_ID      2

typedef enum {
  /* insert here !! */
  MS_PWM_EVENT_UNDEFINE   = 100,  // in order to avoid overlapping with the nordic HAL event number, our numbering was started with a large value
  MS_PWM_EVENT_INIT       = 101,
  MS_PWM_EVENT_START      = 102,
  MS_PWM_EVENT_STOP       = 103,
  MS_PWM_EVENT_UNINIT     = 104
} PWM_EVENT_TYPE;


typedef void (*PWM_EVENT_HANDLER)(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to check whether it is initialized or not
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_IsInitialized(U32 id);

/**
 * @brief Function to initialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
I32 msPWM_Initialize(U32 id, 
          nrf_drv_pwm_t* pPWM, 
          nrf_drv_pwm_config_t* pConfig,
          nrf_pwm_sequence_t* pSequence, 
          U32 nSequence,
          U32 nLoop,
          U32 flags,
          PWM_EVENT_HANDLER fnUserHandler,
          U64 user);

/**
 * @brief Function to uninitialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
I32 msPWM_Uninitialize(U32 id);

/**
 * @brief Function to start and stop the playback after 'stopAfter'
 *        If stopAfter is zero, it is ignored.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_Start(U32 id, U64 stopAfter);

/**
 * @brief Function to stop
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_Stop(U32 id, bool waitUntilStopped);

/**
 * @brief Function to update the sequence
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_UpdateSequence(U32 pid, U32 sid, nrf_pwm_sequence_t* pSequence);

/**
 * @brief Function to update the sequence value
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_UpdateSequenceValue(U32 pid, U32 sid, nrf_pwm_values_t values);

#if defined (__cplusplus)
}
#endif

#endif // MS_PWM_H