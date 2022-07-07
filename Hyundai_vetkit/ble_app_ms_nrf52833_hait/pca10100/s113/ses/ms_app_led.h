/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_led.h					                  
 * Description 	: 															  
 * Authors 	: Don Woo Lee
 *                Cheol Soo Park(sawool.park@gmail.com)
 *                
 * Histroy  	: 2022/04/25 - 1st version released
 *                2022/06/01 - modified to fit the coding rules
 *                2022/06/22 - rename function according to the coding rules
 *
 *				  															   
 *******************************************************************************/
#if !defined (MS_APP_LED_H)
#define MS_APP_LED_H
#include "ms_app_comtype.h"
#include "ms_app_config.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * m_pressure state condition NORMAL
 * bio state condition NORMAL 
 */
void msLED_OnNormalState(); 

/**
 * m_pressure state condition NORMAL
 * bio state condition ABNORMAL 
 */
void msLED_OnAbnormalState(); 

/**
 * m_pressure state condition ABNORMAL
 */
void msLED_OnPressureAbnormalState(); 

/**
 * Short THERAPHY 
 */
void msLED_ShortTherapyState(); 

/**
 * Standard THERAPHY 
 */
void msLED_StandardTherapyState(); 

/**
 * BAT CAN OPERATE
 * BLE CONNECTED
 */
void msLED_OnBatteryCanOperateConnectedState(); 

/**
 * BAT CAN OPERATE
 * BLE DISCONNECTED
 */
void msLED_OnBatteryCanOperateDisconnectedState(); 

/**
 * BAT GOOD TO SLEEP
 * BLE CONNECTED
 */
void msLED_OnBatteryGoodSleepConnectedState();

/**
 * BAT GOOD TO SLEEP
 * BLE DISCONNECTED
 */
void msLED_OnBatteryGoodSleepDisconnectedState();

/**
 * BAT SHUTDOWN_INPRGORESS
 * BLE CONNECTED
 */
void msLED_OnBatteryShutdownInProgressConnectedState(); 

/**
 * BAT SHUTDOWN_INPRGORESS
 * BLE DISCONNECTED
 */
void msLED_OnBatteryShutdownInProgressDisconnectedState(); 


#if defined (__cplusplus)
}
#endif
#endif //MS_APP_LED_H