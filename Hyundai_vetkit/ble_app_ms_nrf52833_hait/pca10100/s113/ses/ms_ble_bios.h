/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_bios.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef BLE_BIOSERVICE_H
#define BLE_BIOSERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ms_datatype.h"
#include "ms_ble_packed.h"
#include "ms_app_executive.h"


#if defined (__cplusplus)
extern "C" {
#endif

#ifndef BLE_BIOS_BLE_OBSERVER_PRIO
#define BLE_BIOS_BLE_OBSERVER_PRIO        2
#endif

/**@brief   Macro for defining a bio service instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_MSBIOS_DEF(_name)                                      \
static BLE_BIOSERVICE _name;                                       \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                \
                     BLE_BIOS_BLE_OBSERVER_PRIO,                   \
                     msBLE_OBSV_SRVC_OnBleEvent, &_name)
                     

// What is a UUID ?
// - UUID stands for Universally Unique Identifier. it is a 128 bits(16 bytes) value. there are two types of UUIDs.
//   Standard 16-bit UUID adn custom 128-bit UUID.
// - Standard UUIDs are defined by SIG, they are also 128 bit UUID but they will only have a 16 bit or 2 bytes of 
//   unique value and the remaining bits are assembled by the receiving device. it increase the transmission efficiency
//   as we don't have to transmit 16 bytes evey time.
//   for e.g. we have Heart Rate service UUID as 0x180D, so we set this value and transmit this value,
//            the receiving device will collect this data and join it with the full UUID which is :
//            0x 0000 xxxx 0000 1000 8000 0080 5F9B 34FB (base UUID)
//            0x 0000 180D 0000 1000 8000 0080 5F9B 34FB (16 bit Heart Rate Service UUID)
// - Custom UUID is a 128 bit value and its defined by the programmer.
//   for e.g. 0x 6E4Q 0001 B5A3 F393 E0A9 E50E 24DC CA9E (base UUID)
//                    ----
//                    0001     (user defined Custom Service UUID)
//
// Standard UUIDs by SIG Group
//  0x1800 ~ 0x26FF : Used as a universal unique identifier for the services
//  0x2700 ~ 0x27FF : Used to identify the unit of measurement
//  0x2800 ~ 0x28FF : Used to distiguish attribute types
//  0x2900 ~ 0x29FF : Used as a Characteristic description
//  0x2A00 ~ 0x7FFF : Used to distinguish Characteristic types
//
#define BIOSERVICE_UUID_BASE  {0x02, 0x00, 0x12, 0xAC, 0x42, 0x02, 0xA0, 0x8E, 0xEC, 0x11, 0xED, 0xED, 0x00, 0x00, 0xDD, 0x1C}
#define BIOSERVICE_UUID_SERVICE              0xB248
#define BIOSERVICE_UUID_BATTERY_STATE_CHAR   0xB249
#define BIOSERVICE_UUID_BIO_STATE_CHAR       0xB24A
#define BIOSERVICE_UUID_BPM_NOTICE_CHAR      0xB24B
#define BIOSERVICE_UUID_DATE_NOTICE_CHAR     0xB24C
#define BIOSERVICE_UUID_THERAPY_CHOICE_CHAR  0xB24D
#define BIOSERVICE_UUID_THERAPY_LOG_CHAR     0xB24E
#define BIOSERVICE_UUID_ALIAS_NAME_CHAR      0xB24F
#define BIOSERVICE_UUID_STATE_REPORT_CHAR    0xB250

// Forward declaration of the BLE_BIOSERVICE type.
typedef struct BLE_BIOSERVICE_ BLE_BIOSERVICE;

typedef void (*BLE_ADVERTISE_HANDLER) (U16 connection_handle, U32 user);
typedef void (*BLE_CONNECTED_HANDLER) (U16 connection_handle, U32 user);
typedef void (*BLE_DISCONNECTED_HANDLER) (U16 connection_handle, U32 user);
typedef void (*BLE_WRITTEN_HANDLER) (ble_gatts_evt_write_t const* pWrite);

struct BLE_BIOSERVICE_ {
    U16                         service_handle;       
    ble_gatts_char_handles_t    charBatteryStateHandle;   
    ble_gatts_char_handles_t    charBioStateHandle;
    ble_gatts_char_handles_t    charBpmStateHandle; 
    ble_gatts_char_handles_t    charDateNoticeHandle; 
    ble_gatts_char_handles_t    charThreapyChoiceHandle; 
    ble_gatts_char_handles_t    charTherapyLogHandle;
    ble_gatts_char_handles_t    charAliasNameHandle;
    ble_gatts_char_handles_t    charStateReportHandle;  
    U08                         uuid_type; // uuid type of service
};

enum {
  BLE_CHAR_VALUE_CHANGED    = 0,
  BLE_CHAR_CCCD_CHANGED     = 1
};

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U16 msBLE_GetConnectionHandle();

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
BLE_BIOSERVICE* msBLE_GetBioServiceHandle();

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_BIOS_Initialize(BLE_BIOSERVICE* pService);

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_Initialize(bool* erase_bonds,
            BLE_ADVERTISE_HANDLER onAdvertiseEventHandler,
            BLE_CONNECTED_HANDLER onConnectHandler, 
            BLE_DISCONNECTED_HANDLER onDisconnectHandler, 
            BLE_WRITTEN_HANDLER onWrittenHandler);

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_Advertising(void);

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_OBSV_SRVC_OnBleEvent(ble_evt_t const* pBleEvent, void* pUser);

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */    
void msBLE_SoftDevice_EventHandler(ble_evt_t const* pEvent, void* pUser);

/**
 * @brief Function to write data in characteristic attribute value as type of HVX NOTIFICATION
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_HVX_Write(U16 connectionHandle, U16 charValueHandle, U08* pData, U16 offset, U16* pSize);

/**
 * @brief Function to read a data in characteristic attribute value
 *        charHandle corresponds to characteristic's value_handle or cccd_handle
 *        if the attribute value is longer than the size of the supplied buffer,
 *        data.len will return the total attribute value length (excluding offset),
 *        and not the number of bytes actually returned in data.p_value.
 *        the application may use this information to allocate a sutiable buffer size
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_ReadValue(U16 connectionHandle, U16 charHandle, U08* pData, U16 offset, U16* pSize);

/**
 * @brief Function to write a data in characteristic attribute value
 *        Values other than system attributes can be set at any time, regardless of whether any active connections exist.
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_WriteValue(U16 connectionHandle, U16 charHandle, U08* pData, U16 offset, U16 nSize);

/**
 * @brief Function dynamically to update the device name, battery level and therapy mode in advertising packet
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_ADVT_UpdateData(U08* pAliasName, U08 batteryLevel, U08 therapyMode);


#if defined (__cplusplus)
}
#endif
#endif // BLE_BIOSERVICE_H

