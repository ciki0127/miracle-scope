/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_Encode.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/24 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_ble_packed.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#if defined (__cplusplus)
extern "C" {
#endif


/**
 * @brief Function to set the packed format of the timestampe value to get the server datetime
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodePingDateTime(VDATEPING* pPing, U08* pPacked, U32 nSize)
{
  pPacked[0] = (pPing->stamp >> 56) & 0xFF; 
  pPacked[1] = (pPing->stamp >> 48) & 0xFF; 
  pPacked[2] = (pPing->stamp >> 40) & 0xFF; 
  pPacked[3] = (pPing->stamp >> 32) & 0xFF; 
  pPacked[4] = (pPing->stamp >> 24) & 0xFF; 
  pPacked[5] = (pPing->stamp >> 16) & 0xFF;
  pPacked[6] = (pPing->stamp >>  8) & 0xFF; 
  pPacked[7] = (pPing->stamp      ) & 0xFF;
  pPacked += MS_APP_BLE_TIMESTAMP_SIZE;
  for (U32 k = 0; k < MS_APP_BLE_DATETIME_SIZE; k++) {
    pPacked[k] = 0;
  }
}

/**
 * @brief Function to get the packed format of the timestampe value to get the server datetime
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodePingDateTime(VDATEPING* pPing, U08* pPacked, U32 nSize)
{
  U64 stamp;

  stamp  = (((U64)pPacked[0]) << 56);
  stamp |= (((U64)pPacked[1]) << 48);
  stamp |= (((U64)pPacked[2]) << 40);
  stamp |= (((U64)pPacked[3]) << 32);
  stamp |= (((U64)pPacked[4]) << 24);
  stamp |= (((U64)pPacked[5]) << 16);
  stamp |= (((U64)pPacked[6]) <<  8);
  stamp |= (((U64)pPacked[7]));
  pPing->stamp = stamp;
  pPacked += MS_APP_BLE_TIMESTAMP_SIZE;
  msBLE_APP_DecodeDateTime(&pPing->date, pPacked, MS_APP_BLE_DATETIME_SIZE);
}

/**
 * @brief Function to set the packed format of the current pressure value and bio signal to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBioSignal(VSENSOR* pPressure, I16* pSignal, U08* pPacked, U32 nSize)
{
  pPacked[0] = (pPressure->state.condition >> 3) & 0x07;
  pPacked[1] = (pPressure->state.condition       & 0x07);
  pPacked[2] = (pPressure->state.value     >> 8) & 0xFF;
  pPacked[3] = (pPressure->state.value           & 0xFF);
  pPacked += 4;
  nSize -= 4;
  nSize /= 2;
  for (U32 k = 0; k < nSize; k++) {
    pPacked[0] = (pSignal[k] >> 8) & 0xFF;
    pPacked[1] = (pSignal[k] & 0xFF);
    pPacked += 2;
  }
}

/**
 * @brief Function to get the packed format of the current pressure value and bio signal to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBioSignal(VSENSOR* pPressure, I16* pSignal, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_BIO_N_PRESS_SIGNAL_SIZE) {
    return;
  }
  pPressure->state.condition = ((pPacked[0] & 0x07) << 3) | (pPacked[1] & 0x07);
  pPressure->state.value = (pPacked[2] << 8) | pPacked[3];
  pPacked += 4;
  nSize -= 4;
  nSize /= 2;
  for (U32 k = 0; k < nSize; k++) {
    pSignal[k] = (pPacked[0] << 8) | pPacked[1];
    pPacked += 2;
  }
}

/**
 * @brief Function to set the packed format of the current power status to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodePowerState(VPOWER* pPower, U08* pPacked, U32 nSize)
{
  if (nSize < MS_APP_BLE_POWER_STATE_SIZE) return;
  switch(pPower->state) {
  case MS_POWER_ON: 
  case MS_POWER_SAVING_L1:
    pPacked[0] = 0; 
    break;
  case MS_POWER_SAVING_L2: 
  case MS_POWER_SAVING_L3:
    pPacked[0] = 1; 
    break;
  default: //
    pPacked[0] = 2;
    break;
  }
}

/**
 * @brief Function to get the packed format of the current battery level to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBatteryLevel(VBATTERY* pBattery, U08* pPacked, U32 nSize)
{
  pPacked[0] = (U08)pBattery->level;
}

/**
 * @brief Function to set the packed format of the current battery level to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBatteryLevel(VBATTERY* pBattery, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_BATTERY_LEVEL_SIZE) {
    return;
  }
  pBattery->level = (U16)pPacked[0];
}

/**
 * @brief Function to set the packed format of the BPM state
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBpmState(VSENSOR* pBio, VSENSOR* pRespiration, VSENSOR* pHeart, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_BPM_NOTICE_SIZE) {
    return;
  }
  pPacked[0] = (pRespiration->state.condition ) & 0x07; 
  pPacked[1] = (pRespiration->state.value >> 8) & 0xFF;
  pPacked[2] = (pRespiration->state.value     ) & 0xFF;
  pPacked[3] = (pHeart->state.condition       ) & 0x07; 
  pPacked[4] = (pHeart->state.value       >> 8) & 0xFF;
  pPacked[5] = (pHeart->state.value           ) & 0xFF;
}

/**
 * @brief Function to get the packed format of the BPM state
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBpmState(VSENSOR* pBio, VSENSOR* pRespiration, VSENSOR* pHeart, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_BPM_NOTICE_SIZE) {
    return;
  }

  pRespiration->state.condition = (pPacked[0]);    
  pRespiration->state.value     = (pPacked[1] << 8) | pPacked[2];
  pHeart->state.condition       = (pPacked[3]);   
  pHeart->state.value           = (pPacked[4] << 8) | pPacked[5];

  if (pRespiration->state.condition == SS_DEFINITELY_ABNORMAL || pHeart->state.condition == SS_DEFINITELY_ABNORMAL) {
    pBio->state.condition = SS_DEFINITELY_ABNORMAL;
  } else {
    pBio->state.condition = SS_CAN_OPERATE;
  }
}

/**
 * @brief Function to set the packed format of the current therapy choice to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_EncodeTherapyChoice(VTHERAPHY* pChoice, U08* pPacked, U32 nSize)
{
  msOS_MEMSet(pPacked, 0, nSize);
  msBLE_APP_EncodeDateTime(&pChoice->tstart, pPacked, MS_APP_BLE_DATETIME_SIZE);
  pPacked += MS_APP_BLE_DATETIME_SIZE;
  pPacked[0] = (pChoice->duration >> 8) & 0xFF; 
  pPacked[1] = (pChoice->duration & 0xFF); 
  pPacked[2] = pChoice->mode; 
  pPacked[3] = pChoice->number; 
  pPacked[4] = pChoice->command;
}


/**
 * @brief Function to get the packed format of the current therapy choice from the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeTherapyChoice(VTHERAPHY* pChoice, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_THERAPY_STATE_SIZE) {
    return;
  }
  msOS_MEMSet(pChoice, 0, sizeof(VTHERAPHY));
  msBLE_APP_DecodeDateTime(&pChoice->tstart, pPacked, MS_APP_BLE_DATETIME_SIZE);
  pPacked += MS_APP_BLE_DATETIME_SIZE;
  pChoice->duration = (pPacked[0] << 8) | pPacked[1];
  pChoice->mode = pPacked[2];
  pChoice->number = pPacked[3];
  pChoice->command = pPacked[4];
}

/**
 * @brief Function to set the date to the byte format
 *        Note that the year is an offset based on the year 1970
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_EncodeDateTime(VDATE* pDate, U08* pPacked, U32 nSize)
{
  msOS_MEMSet(pPacked, 0, nSize);
  pPacked[0] = (pDate->year - 1970) % 1000; 
  pPacked[1] = pDate->month;
  pPacked[2] = pDate->day;
  pPacked[3] = pDate->wday;
  pPacked[4] = pDate->hour;
  pPacked[5] = pDate->minute;
  pPacked[6] = pDate->second;
  pPacked[7] = (pDate->millis >> 8) & 0xFF;  
  pPacked[8] = (pDate->millis & 0xFF);  
}

/**
 * @brief Function to get the date from the packet byte format
 *        Note that the year is an offset based on the year 1970
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_DecodeDateTime(VDATE* pDate, U08* pPacked, U32 nSize)
{
  if (nSize != MS_APP_BLE_DATETIME_SIZE) {
    return;
  }
  msOS_MEMSet(pDate, 0, sizeof(VDATE));
  pDate->year = 1970;
  pDate->year += pPacked[0]; 
  pDate->month = pPacked[1];
  pDate->day = pPacked[2];
  pDate->wday = pPacked[3];
  pDate->hour = pPacked[4];
  pDate->minute = pPacked[5];
  pDate->second = pPacked[6];
  pDate->millis = (pPacked[7] << 8) + pPacked[8];
}

#if defined (__cplusplus)
}
#endif
