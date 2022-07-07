/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_utils.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/22 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_BLE_UTILS_H
#define MS_BLE_UTILS_H
#include "ms_datatype.h"


#if defined (__cplusplus)
extern "C" {
#endif

#define msBLE_UTIL_WritePackLe8(pack, data)   (pack[0]) = (data)
#define msBLE_UTIL_ReadPackLe8(pack, data)    (data) = (pack[0])

/**
 * @brief Function to convert 16-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe16(U08* pPack, U16 data);

/**
 * @brief Function to convert 16-bit array in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLeArray16(U08* pPack, U16* pArray, U16 nArray);

/**
 * @brief Function to convert 32-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe32(U08* pPack, U32 data);

/**
 * @brief Function to convert 32-bit array in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLeArray32(U08* pPack, U32* pArray, U16 nArray);

/**
 * @brief Function to convert 64-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe64(U08* pPack, U64 data);

/**
 * @brief Function to read 16-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe16(U08* pPack, U16* pData);

/**
 * @brief Function to read 16-bit array in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLeArray16(U08* pPack, U16* pArray, U16 nArray);

/**
 * @brief Function to read 32-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe32(U08* pPack, U32* pData);

/**
 * @brief Function to read 32-bit array in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLeArray32(U08* pPack, U32* pArray, U16 nArray);

/**
 * @brief Function to read 64-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe64(U08* pPack, U64* pData);


#if defined (__cplusplus)
}
#endif
#endif // MS_BLE_UTILS_H

