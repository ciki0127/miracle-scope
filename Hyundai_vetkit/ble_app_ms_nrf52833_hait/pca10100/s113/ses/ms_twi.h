/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_twi.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/8 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_TWI_H
#define MS_TWI_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_drv_twi.h"
#include "app_error.h"
#include "ms_app_boards.h"
#include "ms_task.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef enum {
  MS_TWI_ASYNC_MODE       = 1,
  MS_TWI_SYNC_MODE        = 2
} TWI_OPERATION_MODE;

typedef enum {
  MS_TWI_EVT_UNDEFINED    = 0,
  MS_TWI_EVT_DONE         = 1,
  MS_TWI_EVT_ADDRESS_NACK = 2,
  MS_TWI_EVT_DATA_NACK    = 3
} TWI_ERROR_CODE;

/**
 * @brief Function for initiaizing the TWI
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Initialize(U32 id, nrf_drv_twi_t* pInstance, nrf_drv_twi_config_t* pConfig, TWI_OPERATION_MODE mode, TASK_HANDLER fnUserHandler, U64 user);

/**
 * @brief Function for uninitiaizing the TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Uninitialize(U32 id);

/**
 * @brief Function to check wheter the specified twi is initialized.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_IsInitialized(U32 id);

/**
 * @brief Function to enable the specified TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Enable(U32 id);

/**
 * @brief Function to disable the specified TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Disable(U32 id);

/**
 * @brief Function to check wheter the specified twi is enabled.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_IsEnable(U32 id);

/**
 * @brief Function to read data from the specified TWI slave.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Read(U32 id, U08 slaveAddr, U08 regAddr, U08* pValue, I32 nSize, bool noStop);

/**
 * @brief Function to write values to the specified register of slave receiver.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Write(U32 id, U08 slaveAddr, U08 regAddr, U08* pValue, I32 nSize, bool noStop);

/**
 * @brief Function to find the address of the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_WhoIAm(U32 id, U08* pWhoIam);


#if defined (__cplusplus)
}
#endif

#endif // MS_TWI_H