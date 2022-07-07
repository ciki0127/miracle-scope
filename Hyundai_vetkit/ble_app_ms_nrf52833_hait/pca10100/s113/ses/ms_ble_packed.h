/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_Encode.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/24 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_app_config.h"
#include "ms_app_comtype.h"
#include "ms_app_specific.h"
#include "ms_os.h"

#if !defined (MS_BLE_Encode)
#define MS_BLE_Encode


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief defines macros
 */
#define MS_APP_BLE_DATETIME_SIZE                  9
#define MS_APP_BLE_TIMESTAMP_SIZE                 8                                                              
#define MS_APP_BLE_DATETIME_PING_SIZE             (MS_APP_BLE_DATETIME_SIZE + MS_APP_BLE_TIMESTAMP_SIZE)          
#define MS_APP_BLE_THERAPY_CHOICE_SIZE            5
#define MS_APP_BLE_THERAPY_STATE_SIZE             (MS_APP_BLE_DATETIME_SIZE + MS_APP_BLE_THERAPY_CHOICE_SIZE)    
#define MS_APP_BLE_THERAPY_LOG_SIZE               (M_SDH_BLE_GATT_MAX_MTU_SIZE / MS_APP_BLE_THERAPY_STATE_SIZE) 
#define MS_APP_BLE_BIO_SENSOR_SIZE                4
#define MS_APP_BLE_BATTERY_LEVEL_SIZE             1
#define MS_APP_BLE_BATTERY_LEVEL_STEP             25
#define MS_APP_BLE_POWER_STATE_SIZE               1
#define MS_APP_BLE_PRESSURE_STATE_SIZE            4
#define MS_APP_BLE_BIO_SIGNAL_SIZE                (BIO_SENSOR_SAMPLING_RATE * 2)
#define MS_APP_BLE_BIO_N_PRESS_SIGNAL_SIZE        (MS_APP_BLE_PRESSURE_STATE_SIZE + MS_APP_BLE_BIO_SIGNAL_SIZE)
#define MS_APP_BLE_BPM_NOTICE_SIZE                6
#define MS_APP_BLE_SYS_REPORT_MAX_SIZE            20

/**
 * @brief Function to set the packed format of the timestampe value to get the server datetime
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodePingDateTime(VDATEPING* pPing, U08* pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the timestampe value to get the server datetime
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodePingDateTime(VDATEPING* pPing, U08* pPacked, U32 nSize);

/**
 * @brief Function to set the packed format of the current pressure value and bio signal to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBioSignal(VSENSOR* pPressure, I16* pSignal, U08* pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the current pressure value and bio signal to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBioSignal(VSENSOR *pPressure, I16 *pSignal, U08 *pPacked, U32 nSize);

/**
 * @brief Function to set the packed format of the current power status to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
// void msBLE_APP_EncodePowerState(VPOWER* pPower, U08* pPacked, U32 nSize);

/**
 * @brief Function to set the packed format of the current battery status and BPM to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBatteryState(VBATTERY* pBattery, U08* pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the current battery level to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBatteryLevel(VBATTERY *pBattery, U08 *pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the current battery level to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBatteryLevel(VBATTERY *pBattery, U08 *pPacked, U32 nSize);

/**
 * @brief Function to set the packed format of the current therapy choice to the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeTherapyChoice(VTHERAPHY *pChoice, U08 *pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the current therapy choice from the byte format
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_DecodeTherapyChoice(VTHERAPHY* pChoice, U08* pPacked, U32 nSize);

/**
 * @brief Function to set the packed format of the BPM state
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_EncodeBpmState(VSENSOR* pBio, VSENSOR* pRespiration, VSENSOR* pHeart, U08* pPacked, U32 nSize);

/**
 * @brief Function to get the packed format of the BPM state
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msBLE_APP_DecodeBpmState(VSENSOR* pBio, VSENSOR* pRespiration, VSENSOR* pHeart, U08* pPacked, U32 nSize);

/**
 * @brief Function to set the date to the byte format
 *        Note that the year is an offset based on the year 1970
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_EncodeDateTime(VDATE* pDate, U08* pPacked, U32 nSize);

/**
 * @brief Function to get the date from the packet byte format
 *        Note that the year is an offset based on the year 1970
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msBLE_APP_DecodeDateTime(VDATE* pDate, U08* pPacked, U32 nSize);


#if defined (__cplusplus)
}
#endif
#endif //MS_BLE_Encode