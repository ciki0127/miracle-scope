/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_boards.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/20 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_APP_BOARDS_H
#define MS_APP_BOARDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"
#include "nrf_saadc.h"
#include "ms_chip_hw.h"



#define MS_APP_LEDS_NUMBER    4                         // LEDs definitions for HAIT_BOARD_V1

// basic naming based on HW schematic
#define ADC0                  NRF_SAADC_INPUT_AIN0      // input, ADC, floating, BIO sensor
#define ADC1                  NRF_SAADC_INPUT_AIN1      // input, ADC, floating, Pressure sensor
#define ADC_BAT               NRF_SAADC_INPUT_AIN2      // input, ADC, floating, Battery
#define PULSE_CNTL            NRF_GPIO_PIN_MAP(0,28)    // output, pulldown, low state, PWM output to this gpio for magnetic field treatment
#define LED_R_I               NRF_GPIO_PIN_MAP(0,29)    // output, pulldown, low state, gpio for LED Red(1 RGB LED)
#define LED_G_I               NRF_GPIO_PIN_MAP(0,30)    // output, pulldown, low state, gpio for LED Green(1 RGB LED)
#define LED_G_O               NRF_GPIO_PIN_MAP(0,20)    // gpio out for LED Green(1 RGB LED)
#define LED_R_O               NRF_GPIO_PIN_MAP(0, 9)    // gpio out for LED Red(1 RGB LED), mux with NFC1
#define SCL0                  NRF_GPIO_PIN_MAP(0,31)    // output, floating, gpio in for RTC SCL
#define SDA0                  NRF_GPIO_PIN_MAP(0, 5)    // output, floating, gpio in for RTC SDA
#define XIN                   0                         // innput, XL1(Crystal), Floating, for External LFCLK 32.768KHz
#define XTO                   1                         // output, XL2(Crystal), Floating, for External LFCLK 32.768KHz
#define RX                    NRF_GPIO_PIN_MAP(1, 9)    // input, floating, for uart rx
#define PWR_ON                NRF_GPIO_PIN_MAP(1, 9)    // input, floating, for uart rx
#define TX                    NRF_GPIO_PIN_MAP(0,11)    // output, floating, for uart tx (DEBUG_TX)
#define VCC_3V3               17                        // VDD
#define VDDH                  18                        // mux with 17(VDD)
#define VCCUSB                19                        // VBUS
#define PWR_HOLD              NRF_GPIO_PIN_MAP(0,15)    // output, pulldown, low state, high to low sense interrupt, gpio for power on
#define PULSE_PWR_EN          NRF_GPIO_PIN_MAP(0,11)    // gpio output for uart tx
#define SHORT_SW              NRF_GPIO_PIN_MAP(0,17)    // input, floating, high to low sense interrupt, for short teraphy SW 
#define STD_SW                NRF_GPIO_PIN_MAP(0,10)    // input, floating, low state, high to low sense interrupt, for standard teraphy SW, mux with NFC2

// Alias are given to make it easier to recognize 
#define ADC_BIOSENSOR         ADC0
#define ADC_PRSSENSOR         ADC1
#define ADC_BATTERY           ADC_BAT
#define PWM_THERAPHY          PULSE_CNTL
#define LED_R_BLE_CONNECTED   LED_R_I
#define LED_G_BLE_CONNECTED   LED_G_I
#define LED_R_FNC_OPSTATE     LED_R_O                        // status led to indicate for several key functions
#define LED_G_FNC_OPSTATE     LED_G_O                        // status led to indicate for several key functions
#define PWR_BUTTON            PWR_ON
#define RTC_SCL               SCL0
#define RTC_SDA               SDA0
#define DEBUG_UART_RX         RX
#define DEBUG_UART_TX         TX
#define VDD                   VCC_3V3
#define BTN_SHORT_THERAPHY    SHORT_SW
#define BTN_STAND_THERAPHY    STD_SW

//C:\NordicWorkspace\projects\ble_app_hait_sensor\pca10100\s113\ses\ms_app_boards.h

#ifdef __cplusplus
}
#endif

#endif //MS_APP_BOARDS_H