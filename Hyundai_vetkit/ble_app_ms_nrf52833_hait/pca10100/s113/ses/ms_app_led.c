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
 #include "ms_app_led.h"

#if defined (__cplusplus)
extern "C" {
#endif

 /**
 * ble toggle variable
 */
 static I32 ble1_toggle = 0;
 static I32 ShortTherapy_toggle = 0;
 /**
 * m_pressure state condition NORMAL
 * bio state condition NORMAL 
 */
void msLED_OnNormalState()
{
  nrf_gpio_pin_clear(LED_R_FNC_OPSTATE);
  nrf_gpio_pin_set(LED_G_FNC_OPSTATE); 
}

/**
 * m_pressure state condition NORMAL
 * bio state condition ABNORMAL 
 */
void msLED_OnAbnormalState() 
{
  nrf_gpio_pin_clear(LED_G_FNC_OPSTATE);
  nrf_gpio_pin_set(LED_R_FNC_OPSTATE);
}

/**
 * m_pressure state condition ABNORMAL
 */
void msLED_OnPressureAbnormalState() 
{
  nrf_gpio_pin_clear(LED_G_FNC_OPSTATE);
  nrf_gpio_pin_toggle(LED_R_FNC_OPSTATE); 
}

/**
 * SHORT THERAPHY 
 */
void msLED_ShortTherapyState() {
  if(ShortTherapy_toggle == 0) {
    nrf_gpio_pin_set(LED_G_FNC_OPSTATE);
    nrf_gpio_pin_set(LED_R_FNC_OPSTATE);
     ShortTherapy_toggle = 1;
  }
  else {
    nrf_gpio_pin_clear(LED_G_FNC_OPSTATE);
    nrf_gpio_pin_clear(LED_R_FNC_OPSTATE);
    ShortTherapy_toggle = 0;
  }    
}

/**
 * STANDARD THERAPHY 
 */
void msLED_StandardTherapyState() {
  nrf_gpio_pin_clear(LED_R_FNC_OPSTATE);
  nrf_gpio_pin_toggle(LED_G_FNC_OPSTATE);
}

/**
 * BAT CAN OPERATE
 * BLE CONNECTED
 */
void msLED_OnBatteryCanOperateConnectedState()
{
  nrf_gpio_pin_clear(LED_R_BLE_CONNECTED);
  nrf_gpio_pin_set(LED_G_BLE_CONNECTED); 
}

/**
 * BAT CAN OPERATE
 * BLE DISCONNECTED
 */
void msLED_OnBatteryCanOperateDisconnectedState() 
{
  nrf_gpio_pin_clear(LED_R_BLE_CONNECTED);
  nrf_gpio_pin_toggle(LED_G_BLE_CONNECTED); 
}

/**
 * BAT GOOD TO SLEEP
 * BLE CONNECTED
 */
void msLED_OnBatteryGoodSleepConnectedState()
{
  nrf_gpio_pin_set(LED_R_BLE_CONNECTED);
  nrf_gpio_pin_set(LED_G_BLE_CONNECTED); 
}

/**
 * BAT GOOD TO SLEEP
 * BLE DISCONNECTED
 */
void msLED_OnBatteryGoodSleepDisconnectedState() 
{
  if(ble1_toggle == 0) {
    nrf_gpio_pin_set(LED_G_BLE_CONNECTED);
    nrf_gpio_pin_set(LED_R_BLE_CONNECTED);
     ble1_toggle = 1;
  }
  else {
    nrf_gpio_pin_clear(LED_G_BLE_CONNECTED);
    nrf_gpio_pin_clear(LED_R_BLE_CONNECTED);
    ble1_toggle = 0;
  }            
}

/**
 * BAT SHUTDOWN_INPRGORESS
 * BLE CONNECTED
 */
void msLED_OnBatteryShutdownInProgressConnectedState() 
{
  nrf_gpio_pin_clear(LED_G_BLE_CONNECTED);
  nrf_gpio_pin_set(LED_R_BLE_CONNECTED); 
}

/**
 * BAT SHUTDOWN_INPRGORESS
 * BLE DISCONNECTED
 */
void msLED_OnBatteryShutdownInProgressDisconnectedState()
{
  nrf_gpio_pin_clear(LED_G_BLE_CONNECTED);
  nrf_gpio_pin_toggle(LED_R_BLE_CONNECTED); 
}

#if defined (__cplusplus)
}
#endif
