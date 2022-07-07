/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_date.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/09 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_DATE_H
#define MS_DATE_H
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "boards.h"
#include "ms_app_config.h"
#include "ms_app_boards.h"
#include "ms_datatype.h"
#include "ms_task.h"


#if defined (__cplusplus)
extern "C" {
#endif


/**
 * @brief Function to initialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msDATE_Initialize(VDATE *pDate);

/**
 * @brief Function to uninitialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
void msDATE_Uninitialize();

/**
 * @brief Function to get the current date
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
VDATE msDATE_GetCurrentTime();

/**
 * @brief Function to get the current time in millisecond
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
U64 msDATE_GetCurrentTimeInMillisecond();

/**
 * @brief Convert millisecond timestamp to date
 * @param[in] t Unix timestamp
 * @param[out] date Pointer to a structure representing the date and time
 *
 * @retval 
 * @retval 
 */ 
void convertMillisecondInDate(U64 timeInMillis, VDATE *date);


/**
 * @brief Convert date to millisecond timestamp
 * @param[in] date Pointer to a structure representing the date and time
 * @return Unix timestamp
 *
 * @retval 
 * @retval 
 */ 
U64 convertDateInMillisecond(const VDATE *date);

/**
 * @brief Compare dates
 * @param[in] date1 Pointer to the first date
 * @param[in] date2 Pointer to the second date
 * @return Comparison result
 *
 * @retval 
 * @retval 
 */ 
I32 compareDate(const VDATE *date1, const VDATE *date2);

/**
 * @brief Function to get the packed format current date
 *        Note that the year is an offset based on the year 2000
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
VDATE msDATE_UnpackedDateTime(U08* packed);

#if defined (__cplusplus)
}
#endif

#endif // MS_DATE_H