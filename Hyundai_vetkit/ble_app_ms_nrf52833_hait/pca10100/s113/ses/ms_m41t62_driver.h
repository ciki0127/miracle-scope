/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_m41t62_driver.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/9 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_M41T62_DRIVER_H
#define MS_M41T62_DRIVER_H
#include "nrf.h"
#include "boards.h"
#include "nrf_drv_twi.h"
#include "app_error.h"
#include "ms_app_config.h"
#include "ms_app_boards.h"
#include "ms_task.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to initialize the TWI instance and enable
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_Initialize(U32 scl, U32 sda, nrf_drv_twi_frequency_t frequency, TASK_HANDLER fnUserHandler, U64 user);

/**
 * @brief Function to uninitialize the TWI instance
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void M41T62_TWI_Uninitialize();

/**
 * @brief Function to find the address of the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_ReadDate(VDATE* pDate);

/**
 * @brief Function to write the current date to the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_WriteDate(VDATE* pDate);

/**
 * @brief Function to find the address of the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_WhoIAm(U08 *pWhoIAm);


#if defined (__cplusplus)
}
#endif

#endif //MS_M41T62_DRIVER_H