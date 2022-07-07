/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_saadc2.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/20 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_SAADC_2_H
#define MS_SAADC_2_H
#include "ms_app_config.h"
#include "nrf_drv_ppi.h"    // continuous sampling using TIMER and PPI
#include "nrf_drv_saadc.h"
#include "ms_app_boards.h"
#include "ms_task.h"


#if defined (__cplusplus)
extern "C" {
#endif

#define SAADC_CALIBRATION_INTERVAL      5   //! should be tested later

typedef void (*SAADC_HANDLER)(nrf_drv_saadc_evt_t* pEvent);
typedef void (*SAADC_DATA_HANDLER)(U32 channel, I16 *pArray, I32 nArray, U64 timeStampInMicroSecond, U64 user);

typedef enum {
  /* insert here !! */
  MS_SAADC_EVENT_UNDEFINE   = 100,  // in order to avoid overlapping with the nordic HAL event number, our numbering was started with a large value
  MS_SAADC_EVENT_INIT       = 101,
  MS_SAADC_EVENT_START      = 102,
  MS_SAADC_EVENT_STOP       = 103,
  MS_SAADC_EVENT_UNINIT     = 104
} SAADC_EVENT_TYPE;

typedef enum {
  SAADC_LOWEST_POWER      = 0,
  SAADC_LOW_POWER         = 1, // normal mode
  SAADC_HIGH_POWER        = 2,
  SAADC_USER_DRIVEN_POWER = 3
} SAADC_POWER_MODE;

typedef struct MS_SAADCCTL_MODE {
  U16     reserved        : 12;
  U16     power_mode      : 2; // 0 - ultra low power, 1 - low power(normal mode), 2 - hight power, 3 - user driven
  U16     periodic        : 1; // use timer for continuose sampling
  U16     use_ppi         : 1;
} MS_SAADCCTL_MODE;

typedef struct MS_SAADC_CHANNEL2 {
  I16                         m_id;  // channel id. -1 is invalid
  nrf_saadc_channel_config_t  m_config;
  SAADC_DATA_HANDLER          m_fnDataHandler; // the user defined function forf a sampled data processing 
  U64                         m_user; // user context
} MS_SAADC_CHANNEL2;

/**
 * @brief Function to initialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
I32 msSAADC2_Initialize(nrfx_saadc_config_t* pConfig,
      SAADC_POWER_MODE power_mode,
      SAADC_HANDLER fnEventHandler,
      nrf_drv_timer_config_t* pTmConfig,
      MS_SAADC_CHANNEL2* pChannel,
      I32 nChannel, 
      U32 samplingRate,
      U32 szPooling);

/**
 * @brief Function to reinitialize only if existing set information is used as it is, and only the HW IP block is initialized and used.
 *
 * @param[in]
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
I32 msSAADC2_ReinitializeForLowestPower();

/**
 * @brief Function to uninitialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msSAADC2_Uninitialize();

/**
 * @brief Function to start
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msSAADC2_Start();

/**
 * @brief Function to stop
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msSAADC2_Stop();

/**
 * @brief Function to read a value of the specified channel
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msSAADC2_ReadSample(I16 id, I16 *pValue);


#if defined (__cplusplus)
}
#endif

#endif // MS_SAADC_2_H