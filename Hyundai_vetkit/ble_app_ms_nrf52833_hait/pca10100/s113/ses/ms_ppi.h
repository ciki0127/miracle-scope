/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ppi.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/30 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_PPI_H
#define MS_PPI_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "app_error.h"
#include "ms_app_boards.h"
#include "ms_app_config.h"
#include "ms_adt.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to initialize 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msPPI_Initialize();

/**
 * @brief Function to allocate 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msPPI_ChannelAlloc(nrf_ppi_channel_t *channel);


#if defined (__cplusplus)
}
#endif

#endif // MS_PWM_H