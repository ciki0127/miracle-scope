/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_utils.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/26 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_APP_UTILS_H
#define MS_APP_UTILS_H
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "nrf_nvmc.h"
#include "app_error.h"
#include "ms_app_config.h"
#if (APP_DEBUG_FLASH_DATA_INTEGRITY == 1)
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#endif
#include "ms_datatype.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief generate 16-bits checksum code
 */
U16 msChecksum16(U16 *source, I32 bytes);

/**
 * @brief conver 64-bits value to string ended with NIL ('\0')
 */
void msLOG_Convert64String(U64 value, U08 *string, U32 size);

U08* swap16ToLittleEndian(U16 value, U08* pValue);
U08* swap32ToLittleEndian(U32 value, U08* pValue);

#if defined (__cplusplus)
}
#endif

#endif //MS_APP_UTILS_H