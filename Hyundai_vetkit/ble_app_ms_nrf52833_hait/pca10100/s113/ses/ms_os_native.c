/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_os.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_os_native.h"

#if defined (__cplusplus)
extern "C" {
#endif


#if defined(MS_APP_SDK_NORDIC_NRF5)

/**
 * @brief Function check whether Interrupt is enabled
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI I32 msOS_INTERRUPT_IsEnabled()
{
  I32 pm = __get_PRIMASK(); // returns INT MASK's value, 0 - INT is enabled, none zero - INT is diabled
  return (pm == 0) ? MS_TRUE : MS_FALSE;
}


/**
 * @brief Function enable Interrupt
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI void msOS_INTERRUPT_Enable()
{
  __enable_irq();
}


/**
 * @brief Function disable Interrupt and returns PRIMASK value to restore the state before called Disable()
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI I32 msOS_INTERRUPT_Disable()
{
  I32 pm = __get_PRIMASK();
  __disable_irq();
  return pm;
}

#endif


#if defined (__cplusplus)
}
#endif