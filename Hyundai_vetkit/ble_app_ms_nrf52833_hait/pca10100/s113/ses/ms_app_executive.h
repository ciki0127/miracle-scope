/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_executive.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_APP_EXECUTIVE_H
#define MS_APP_EXECUTIVE_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "ble.h"
#include "boards.h"
#include "nrf_drv_pwm.h"
#include "app_error.h"
#include "ms_app_utils.h"
#include "ms_app_boards.h"
#include "ms_task.h"
#include "ms_app_specific.h"
#include "ms_gpiote.h"
#include "ms_saadc2.h"
#include "ms_pwm.h"
#include "ms_date.h"
#include "ms_m41t62_driver.h"
#include "ms_ble_packed.h"
#include "ms_ble_bios.h"
#include "ms_app_led.h"



#if defined (__cplusplus)
extern "C" {
#endif

typedef void (*BLE_STACKEVENT_HANDLER)(ble_evt_t const* pEvent, U64 user);


typedef struct APP_EXECUTIVE {
  VPOWER                  m_power;
  VDATE                   m_date;
  VLED                    m_led[4];
  VSENSOR                 m_bio;
  VSENSOR                 m_respiration;
  VSENSOR                 m_heart;
  VSENSOR                 m_pressure;
  VSENSOR                 m_battery;
  VPWM                    m_magnetic;
  VCONNECT                m_comport;
  VTHERAPHY               m_therapy;
  VTHERAPHY_LOG           m_teralog;
  I08                     m_alias[MS_BLE_NAME_STRING_MAX_LENGTH + 1];
} APP_EXECUTIVE;

/**
 * @brief Function to system power on
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_PWR_TurnOn(U32 pin);

/**
 * @brief Function to initialize and start Executive instance, such as load data from flash memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msApp_EXEC_Start();

/**
 * @brief Function to get the handle of Executive
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
APP_EXECUTIVE* msApp_EXEC_GetHandle();

/**
 * @brief Function to handle the system power on or off event
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_POWER_EventTaskHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Function to initialize Power Hold, Power On/Off ans Status LEDs
 *        and set initial status.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msAPP_GPIOTE_Initialize();

/**
 * @brief Function to handle application level LEDs control task
 *
 * LED Scenario
 * boot for 2 seconds
 *
 * pressure state condition check (SS_CAN_OPERATE, SS_IS_AMBIGUOUS, SS_MAYBE_ABNORMAL, SS_DEFINITELY_ABNORMAL)
 * NORMAL - SS_CAN_OPERATE
 * ABNORMAL - SS_IS_AMBIGUOUS, SS_MAYBE_ABNORMAL, SS_DEFINITELY_ABNORMAL
 *
 * bio state condition check (SS_CAN_OPERATE, SS_DEFINITELY_ABNORMAL)
 * NORMAL - SS_CAN_OPERATE
 * ABNORMAL - SS_DEFINITELY_ABNORMAL
 *
 * therapy mode check (THERAPHY_UNDEFINED, THERAPHY_STANDARD, THERAPHY_SHORT)
 * SHORT MODE is MAX 2 times a day
 * STANDARD MODE can be treated regardless of pressure, limited number of times X
 *
 * battery state condition check (BS_IS_FULL_CHARGED, BS_CAN_OPERATE, BS_GOOD_TO_SLEEP, BS_SHUTDOWN_INPRORESS)
 * NORMAL - BS_IS_FULL_CHARGED, BS_CAN_OPERATE (30% ~ 100%)
 * AMBIGUOUS - BS_GOOD_TO_SLEEP                (10% ~ 30%)
 * ABNORMAL - BS_SHUTDOWN_INPRORESS            ( 0% ~ 10%)
 *
 * ble comport state check (CS_CONNECTED, CS_DISCONNECTED)
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_GPIOTE_LED_OUT_EventHandler(U64 UUID, I32 event, U64 user);

/**
 * @brief Function to receive BIO sensor sampling data
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_BioSensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user);

/**
 * @brief Function to receive Pressure sensor sampling data
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_PressureSensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user);

/**
 * @brief Function to receive Battery voltage sampling data
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msAPP_BatterySensor_SamplingTaskHandler(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user);

/**
 * @brief Function to send BIO sensor's sampling singal to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_BioSensorEventSendHandler(U64 uuid, I32 length, U64 array);

/**
 * @brief Function to handle SAADC control TASK (using msTSK scheduler)
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msSAADC_AppTerminate(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to send the therapy status to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_TherapyEventSendHandler(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to handle Theraphy event from Remote User
 *        schedule the next run, one of short or standard, if reservations overlap, do the most recently requested one.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_TherapyEventRecvHandler(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to send the current battery level to Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_BatteryLevelEventSendHandler(U64 uuid, I32 event, U64 use);

/**
 * @brief Function to receive BPM from remote user
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
void msApp_BLE_BioBpmEventRecvHandler(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to receive DateTime from Remote User
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_DateEventRecvHandler(U64 uuid, I32 event, U64 user);

/**
 * @brief Function to handle Alias of Device Name event from Remote User
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_AliasNameEventRecvHandler(U64 uuid, I32 size, U64 user);

/**
 * @brief Function to handle THERAPY STANDARD treatment mode
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msGPIOTE_TERA_StandardEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Function to handle THERAPY SHORT treatment mode
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msGPIOTE_TERA_ShortEventHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

/**
 * @brief Function to handle the connected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnAdevertiseHandler(U16 connectionHandle, U32 user);

/**
 * @brief Function to handle the connected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnConnectHandler(U16 connectionHandle, U32 user);

/**
 * @brief Function to handle the disconnected event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnDisconnectHandler(U16 connectionHandle, U32 user);

/**
 * @brief Function to handle the written(updated) characterisitics event from user, it will be used in BLE service
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_COM_fnWrittenHandler(ble_gatts_evt_write_t const* pWrite);


#if defined (__cplusplus)
}
#endif

#endif // MS_APP_EXECUTIVE_H