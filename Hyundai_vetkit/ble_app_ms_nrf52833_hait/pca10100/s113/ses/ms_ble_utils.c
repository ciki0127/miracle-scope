/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_utils.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/22 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_ble_utils.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to convert 16-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe16(U08* pPack, U16 data)
{
  U08* pUv08 = (U08*)&data;
  pPack[0] = pUv08[1];
  pPack[1] = pUv08[0];
}

/**
 * @brief Function to convert 16-bit array in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLeArray16(U08* pPack, U16* pArray, U16 nArray)
{
  U08* pUv08 = (U08*)pArray;

  for (U16 i = 0; i < nArray; i++) {
    pPack[0] = pUv08[1];
    pPack[1] = pUv08[0];
    pUv08 += 2;
    pPack += 2;
  }
}


/**
 * @brief Function to convert 32-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe32(U08* pPack, U32 data)
{
  U08* pUv08 = (U08*)&data;
  pPack[0] = pUv08[3];
  pPack[1] = pUv08[2];
  pPack[2] = pUv08[1];
  pPack[3] = pUv08[0];
}

/**
 * @brief Function to convert 32-bit array in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLeArray32(U08* pPack, U32* pArray, U16 nArray)
{
  U08* pUv08 = (U08*)pArray;

  for (U16 i = 0; i < nArray; i++) {
    pPack[0] = pUv08[3];
    pPack[1] = pUv08[2];
    pPack[2] = pUv08[1];
    pPack[3] = pUv08[0];
    pUv08 += 4;
    pPack += 4;
  }
}

/**
 * @brief Function to convert 64-bit data in little endian format and written so that MSB(most significant byte)
 *        comes first in the buffer
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_WritePackLe64(U08* pPack, U64 data)
{
  U08* pUv08 = (U08*)&data;
  pPack[0] = pUv08[7];
  pPack[1] = pUv08[6];
  pPack[2] = pUv08[5];
  pPack[3] = pUv08[4];
  pPack[4] = pUv08[3];
  pPack[5] = pUv08[2];
  pPack[6] = pUv08[1];
  pPack[7] = pUv08[0];
}

/**
 * @brief Function to read 16-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe16(U08* pPack, U16* pData)
{
  U08* pUv08 = (U08*)pData;
  pUv08[0] = pPack[1];
  pUv08[1] = pPack[0];
}

/**
 * @brief Function to read 16-bit array in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLeArray16(U08* pPack, U16* pArray, U16 nArray)
{
  U08* pUv08 = (U08*)pArray;

  for (U16 i = 0; i < nArray; i++) {
    pUv08[0] = pPack[1];
    pUv08[1] = pPack[0];
    pUv08 += 2;
    pPack += 2;
  }
}

/**
 * @brief Function to read 32-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe32(U08* pPack, U32* pData)
{
  U08* pUv08 = (U08*)pData;
  pUv08[0] = pPack[3];
  pUv08[1] = pPack[2];
  pUv08[2] = pPack[1];
  pUv08[3] = pPack[0];
}

/**
 * @brief Function to read 32-bit array in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLeArray32(U08* pPack, U32* pArray, U16 nArray)
{
  U08* pUv08 = (U08*)pArray;

  for (U16 i = 0; i < nArray; i++) {
    pUv08[0] = pPack[3];
    pUv08[1] = pPack[2];
    pUv08[2] = pPack[1];
    pUv08[3] = pPack[0];
    pUv08 += 4;
    pPack += 4;
  }
}

/**
 * @brief Function to read 64-bit data in little endian format from the buffer containing MSB first
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_UTIL_ReadPackLe64(U08* pPack, U64* pData)
{
  U08* pUv08 = (U08*)pData;
  pUv08[0] = pPack[7];
  pUv08[1] = pPack[6];
  pUv08[2] = pPack[5];
  pUv08[3] = pPack[4];
  pUv08[4] = pPack[3];
  pUv08[5] = pPack[2];
  pUv08[6] = pPack[1];
  pUv08[7] = pPack[0];
}


#if defined (__cplusplus)
}
#endif


