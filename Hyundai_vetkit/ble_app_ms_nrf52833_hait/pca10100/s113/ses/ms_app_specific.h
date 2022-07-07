/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_specific.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ms_app_config.h"
#include "ms_adt.h"
#include "ms_app_comtype.h"


#if !defined (MS_APP_SPECIFIC_H)
#define MS_APP_SPECIFIC_H


#if defined (__cplusplus)
extern "C" {
#endif

#define THERAPHY_PWM_COUNTERTOP                     2500   //50Hz(20ms), pwm pulse frequency = base frequency(1MHz) / countertop
#define THERAPHY_PWM_DUTY_50_VALUE                  2000   //duty 50%, (THERAPHY_PWM_COUNTERTOP / 2)
#define THERAPHY_PWM_DUTY_00_VALUE                  2500       //duty  0%, (THERAPHY_PWM_COUNTERTOP / 2)
#define THERAPHY_PWM_OUT_GPIO_PIN                   PWM_THERAPHY
#define THERAPHY_STAND_DUTY_50_REPEATS_PER_CYCLE    (3000 - 1)  // 50Hz x 60 seconds repeated
#define THERAPHY_STAND_DUTY_OO_REPEATS_PER_CYCLE    (1350 - 1)  // 50Hz x 27 seconds repeated
#define THERAPHY_SHORT_DUTY_50_REPEATS_PER_CYCLE    (3000 - 1)  // 50Hz x 60 seconds repeated
#define THERAPHY_SHORT_DUTY_OO_REPEATS_PER_CYCLE    (950 - 1)   // 50Hz x 19 seconds
#define THERAPHY_TOTAL_CYCLE                        6           // 6 cycles
#define THERAPHY_STAND_TOTAL_TIME_IN_MILLISECOND    522000      // (60 + 27) x 6 x 1000 miiliseconds
#define THERAPHY_SHORT_TOTAL_TIME_IN_MILLISECOND    474000      // (60 + 19) x 6 x 1000 miiliseconds

#define MS_APP_THERAPHY_LOG_MAX_NUMBER  50

#define MS_APP_BIOSENSOR_SAMPLING_RATE              50  // per 20 millisecond
#define MS_APP_SAADC_SPOOLING_LENGTH                ((U32)(1000 / MS_APP_BIOSENSOR_SAMPLING_RATE))   // 1 second pooling,
#define MS_APP_SAADC_CHANNEL_NUMBER                 3   // bio - breath and respiration, pressure, battery

/**
 * @brief defines macros
 */

/**
 * @brief defines the application specific data type
 */

enum {
  THERAPHY_UNDEFINED  = 0,
  THERAPHY_STANDARD   = 1,
  THERAPHY_SHORT      = 2
};

enum {
  THERAPHY_STARTED    = 1,
  THERAPHY_STOPPED    = 2
};

typedef struct VTHERAPHY {
  VDATE               tstart;     // timestamp when it was started
  U16                 duration;   // duration of a treatment
  U08                 mode;
  U08                 number;     // number of short or standard treatments
  U08                 command;    // start or stop (temporal variable)  
} VTHERAPHY;

typedef struct VTHERAPHY_LOG {
  U16                 m_shortPerDay;  // number of short treatment per day
  U08                 m_underTreatment;    // currently on therapy or nor
  U08                 m_queued;
  MS_BCIRCQ           m_list;
} VTHERAPHY_LOG;


/**
 * @brief Function to initialize the list of log of THERAPHY Treatments
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msAPP_TERALOG_Initialize(VTHERAPHY_LOG* pLog, U32 number);

/**
 * @brief Function to write a new log (overwriten if it is full) of THERAPHY Treatments 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msAPP_TERALOG_Write(VTHERAPHY_LOG* pLog, VTHERAPHY *pData);

/**
 * @brief Function to read the oldest log of THERAPHY Treatments
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msAPP_TERALOG_Read(VTHERAPHY_LOG* pLog, VTHERAPHY *pData);

#if defined (__cplusplus)
}
#endif
#endif //MS_APP_SPECIFIC_H

