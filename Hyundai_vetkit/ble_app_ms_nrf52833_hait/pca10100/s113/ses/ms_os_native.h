/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_os_native.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/12 - 1st version released			   
 *				  															   
 *******************************************************************************/
#if !defined (MS_OS_NATIVE_H)
#define MS_OS_NATIVE_H
#include "ms_datatype.h"
#include "ms_app_config.h"

#if defined(MS_APP_SDK_NORDIC_NRF5)
  #include "nrf_nvic.h"
  #include "app_util_platform.h"
#endif


/**
 * @brief defines useful macros
 */
#ifndef OSAPI
  #define OSAPI
#endif

#if defined(MS_WINDOW)
#elif defined(MS_APP_SDK_NORDIC_NRF5)
#else
#endif

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief declares function prototypes
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI I32 msOS_INTERRUPT_IsEnabled();
OSAPI void msOS_INTERRUPT_Enable();
OSAPI I32 msOS_INTERRUPT_Disable();


#if defined (__cplusplus)
}
#endif


#endif // MS_OS_NATIVE_H