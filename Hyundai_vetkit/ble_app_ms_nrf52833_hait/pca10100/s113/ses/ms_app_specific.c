/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_specific.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ms_app_specific.h"
#include "ms_os.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief Function to initialize the list of log of THERAPHY Treatments
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msAPP_TERALOG_Initialize(VTHERAPHY_LOG* pLog, U32 number)
{
  U08* pAddr = (U08*)msOS_MEMAlloc(sizeof(VTHERAPHY) * (number + 1)); 
  return BCIRCQ_Initialize(&pLog->m_list, (VOIDPTR)pAddr, (number + 1), sizeof(VTHERAPHY), NULL);
}

/**
 * @brief Function to write a new log (overwriten if it is full) of THERAPHY Treatments
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msAPP_TERALOG_Write(VTHERAPHY_LOG* pLog, VTHERAPHY *pData)
{
  if (BCIRCQ_GetWritable(&pLog->m_list) <= 0) { 
    BCIRCQ_SkipValue(&pLog->m_list);
  }
  return BCIRCQ_WriteValue(&pLog->m_list, (VOIDPTR)pData);
}

/**
 * @brief Function to read the oldest log of THERAPHY Treatments
 *
 * @param[in] 
 * @param[out]
 *
 * @retval
 * @retval
 */
I32 msAPP_TERALOG_Read(VTHERAPHY_LOG* pLog, VTHERAPHY *pData)
{
  return BCIRCQ_PeekValue(&pLog->m_list, (VOIDPTR)pData);
}


#if defined (__cplusplus)
}
#endif

