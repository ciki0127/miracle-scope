/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_gpiote.c							                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_gpiote.h"

U32   m_gpiote_initialized = MS_FALSE;


/**
 * @brief Function to initialize GPIOTE
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msGPIOTE_Initialize() 
{
  U32 status = MS_FAILURE;

  if(!nrf_drv_gpiote_is_init()) {
    status = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(status);
    return MS_SUCCESS;
  }
  return MS_FAILURE;
}
