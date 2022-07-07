/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ppi.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_ppi.h"
#include "ms_app_utils.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


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
void msPPI_Initialize() {
  U32 status = nrf_drv_ppi_init();
  APP_ERROR_CHECK(status);
}

/**
 * @brief Function to allocate 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msPPI_ChannelAlloc(nrf_ppi_channel_t *channel) 
{
  U32 status = nrf_drv_ppi_channel_alloc(channel);
  APP_ERROR_CHECK(status);
}

#if defined (__cplusplus)
}
#endif
