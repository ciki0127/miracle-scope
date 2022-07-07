/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_config.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_APP_CONFIG_H
#define MS_APP_CONFIG_H
#include "ms_app_boards.h"
#include "sdk_config.h"


#ifndef MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS
  #define MS_SYS_DELAY_POWER_ONOFF_IN_MILLIS  2000
#endif

#ifdef MS_CHIP_DATA_BUS_WIDTH
  #undef MS_CHIP_DATA_BUS_WIDTH
#endif
#define MS_CHIP_DATA_BUS_WIDTH                32  //32bit machine

#ifndef MS_APP_SDK_NORDIC_NRF5
  #define MS_APP_SDK_NORDIC_NRF5              1
#endif

#ifndef MS_APP_TSK_GRANUALITY
  #define MS_APP_TSK_GRANUALITY               1
#endif

#ifndef BIO_SENSOR_SAMPLING_RATE
  #define BIO_SENSOR_SAMPLING_RATE            20
#endif

#ifndef BIO_SENSOR_CAPTURING_RATE
  #define BIO_SENSOR_CAPTURING_RATE           1
#endif

#ifndef BIO_SENSOR_CAPTURE_MS_INTERVAL
  #define BIO_SENSOR_CAPTURE_MS_INTERVAL      (1000 / BIO_SENSOR_CAPTURING_RATE)
#endif

#ifndef RTC_CUSTOM_SCHEDULE_IN_MILLISEC
  #define RTC_CUSTOM_SCHEDULE_IN_MILLISEC     1
#endif

#ifndef RTC_CUSTOM_INT_TICK_ENABLE
  #define RTC_CUSTOM_INT_TICK_ENABLE          0
#endif

#ifndef FLASH_CUSTOM_USED_PAGE_INDEX
  #define FLASH_CUSTOM_USED_PAGE_INDEX        (MS_FLASH_MAX_PAGE_NUMBER - 1)
#endif

#ifndef FLASH_CUSTOM_USED_START_ADDRESS
  #define FLASH_CUSTOM_USED_START_ADDRESS     ((FLASH_CUSTOM_USED_PAGE_INDEX) * MS_FLASH_BYTE_SIZE_PER_PAGE)
#endif

#ifndef APP_FLASH_WRITE_MAX_SIZE
  #define APP_FLASH_WRITE_MAX_SIZE            64
#endif

#ifndef APP_DEBUG_FLASH_DATA_INTEGRITY
  #define APP_DEBUG_FLASH_DATA_INTEGRITY      1
#endif

#ifndef APP_DYNAMIC_MEMORY_AUTO_DEFINE
  #define APP_DYNAMIC_MEMORY_AUTO_DEFINE      1
#endif

#ifndef APP_DYNAMIC_ALLOC_MEMORY_SIZE
  #define APP_DYNAMIC_ALLOC_MEMORY_SIZE       2048
#endif

#ifndef MS_APP_TSK_REQUEST_MAPQ_MAX
  #define MS_APP_TSK_REQUEST_MAPQ_MAX         7     //! can't change this, should not exceed the total number of channel in TIMER_4
#endif

#ifndef MS_APP_TSK_REQUEST_MAX_NUM_PER_QID
  #define MS_APP_TSK_REQUEST_MAX_NUM_PER_QID  10
#endif

#ifndef MS_SCH_DAEMON_PERIOD_IN_MIILISECOND       // task scheduler running period
  #define MS_SCH_DAEMON_PERIOD_IN_MIILISECOND 60000 // 1 minute
#endif

#ifndef MS_APP_SAADC_TIMER_ID
  #define MS_APP_SAADC_TIMER_ID               3
#endif

#ifndef MS_APP_TSKSCH_TIMER_ID
  #define MS_APP_TSKSCH_TIMER_ID              4
#endif

#ifndef MS_APP_THERAPY_PWM_ID
  #define MS_APP_THERAPY_PWM_ID               0
#endif

#ifndef MS_SAADC_CHANNEL_MAX_ID
  #define MS_SAADC_CHANNEL_MAX_ID             3     // maximum used channel number
#endif

#ifndef MS_APP_TWI_INSTANCE_ID
  #define MS_APP_TWI_INSTANCE_ID              0
#endif

#ifndef MS_BLE_BIOTHERAPY_OBSERVER_PRIO
  #define MS_BLE_BIOTHERAPY_OBSERVER_PRIO     3
#endif

#ifndef M_SDH_BLE_GATT_MAX_MTU_SIZE
#define M_SDH_BLE_GATT_MAX_MTU_SIZE           NRF_SDH_BLE_GATT_MAX_MTU_SIZE
#endif

#ifndef MS_BLE_NAME_STRING_MAX_LENGTH
#define MS_BLE_NAME_STRING_MAX_LENGTH         16
#endif

#ifndef MS_BLE_STATE_STRING_MAX_LENGTH
#define MS_BLE_STATE_STRING_MAX_LENGTH        32
#endif 

#ifndef DEVIC_NAME
  #define DEVICE_NAME                         "VETKIT-10"              // name of device. will be included in the advertising data
#endif
#ifndef MS_APP_BLE_DIS_MANUFACTUER_NAME                             // device service information(DIS)
  #define MS_APP_BLE_DIS_MANUFACTUER_NAME     "Miracle Scope"
  #define MS_APP_BLE_DIS_MODEL_NUMBER         "VETKIT-1"
  #define MS_APP_BLE_DIS_SERIAL_NUMBER        "MS010022062100001"
  #define MS_APP_BLE_DIS_HW_REVISION          "2022.01"
  #define MS_APP_BLE_DIS_FW_REVISION          "0.1.0"
  #define MS_APP_BLE_DIS_SW_REVISION          "0.1.0"
#endif



enum {                                       
  TSK_REQUEST_SADC_ID                   = 0,      //internal SAADC capture task
  TSK_REQUEST_PWR_BTN_ID                = 1,      //external Power button task
  TSK_REQUEST_GPIOS_OUT_ID              = 2,      //external GPIO output pins
  TSK_REQUEST_GPIOS_IN_ID               = 3,      //external GPIO input pins
  TSK_REQUEST_EXECUTIVE_ID              = 4,      //Executive for Application
  TSK_REQUEST_SCHEDULER_ID              = 5       //internal Scheduler
};
 
enum {
  MS_APP_PRESSURE_NORMAL_THRESHOLD      = 2500,
  MS_APP_PRESSURE_AMBIGUOUS_THRESHOLD   = 2000,
  MS_APP_PRESSURE_ABNORMAL_THRESHOLD    = 1000
};

enum {                                       
  MS_APP_BATTERY_MAX_ADC_VALUE          = 2390,   //800mAh, 4.23V
  MS_APP_BATTERY_FULL_ADC_VALUE         = 2300,
  MS_APP_BATTERY_MIN_ADC_VALUE          = 1980    //800mAh, 3.50V
};
 

#ifndef BATTERY_LEVEL_SHUTDOWN
  #define BATTERY_LEVEL_SHUTDOWN                15
#endif

#ifndef MS_APP_BATTERY_REPORT_PERIOD
#define MS_APP_BATTERY_REPORT_PERIOD            10000   // send level to user on every 30 seconds
#endif

#ifndef MS_APP_ADVERTISING_DATA_UPDATE_PERIOD
#define MS_APP_ADVERTISING_DATA_UPDATE_PERIOD   5000    // update advertising data to user on every 5 seconds
#endif

#ifndef MS_APP_PING_DATETIME_SEND_TIMES
#define MS_APP_PING_DATETIME_SEND_TIMES         4
#endif

#ifndef MS_APP_PING_DATETIME_PERIOD
#define MS_APP_PING_DATETIME_PERIOD             500
#endif


#endif //MS_APP_CONFIG_H