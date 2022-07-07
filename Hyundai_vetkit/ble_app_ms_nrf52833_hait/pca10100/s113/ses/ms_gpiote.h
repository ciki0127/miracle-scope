/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_gpiote.h							                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/20 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_GPIOTE_H
#define MS_GPIOTE_H
#include "nrf_drv_gpiote.h"
#include "ms_app_config.h"
#include "ms_adt.h"
#include "ms_app_boards.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to initialize GPIOTE
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msGPIOTE_Initialize();

#if defined (__cplusplus)
}
#endif

#endif // MS_GPIOTE_H