/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_os.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#if !defined (MS_OS_H)
#define MS_OS_H
#include "ms_datatype.h"
#include "ms_app_config.h"

#if (MS_APP_SDK_NORDIC_NRF5 == 1)
  #include "app_util_platform.h"
  #include "nrf_delay.h"
#endif


/**
 * @brief defines useful macros
 */
#ifndef OSAPI
  #define OSAPI
#endif

#define OS_CRITICAL_EXIT_WAITING_COUNT    100

#if defined(MS_WINDOW)
  #define msOS_CRTSEC_D(pObjName)	\
	CRITICAL_SECTION pObjName
  #define msOS_CRTSEC_H(hSync)		\
	((HAND)hSync)
#elif defined(MS_LINUX)
  #define msOS_CRTSEC_D(pObjName)	\
	pthread_mutex_t pObjName
  #define msOS_CRTSEC_H(hSync)		\
	((HAND)hSync)
#elif defined(MS_NUCLEUS)
  #define msOS_CRTSEC_D(pObjName)
  #define msOS_CRTSEC_H(hSync)
#elif defined(MS_TIDSPBIOS)
  #define msOS_CRTSEC_D(pObjName)
  #define msOS_CRTSEC_H(hSync)
#elif defined(MS_QUALCOMREX)
  #define msOS_CRTSEC_D(pObjName)
  #define msOS_CRTSEC_H(hSync)
#elif defined(MS_APP_SDK_NORDIC_NRF5)
#else
#endif

#ifndef INFINITE
  #define INFINITE    -1      // use after type conversion to unsigned int or long (maybe so big number)
#endif

#ifndef MS_CHIP_DATA_BUS_WIDTH
#define MS_CHIP_DATA_BUS_WIDTH    64
#endif

#if (MS_CHIP_DATA_BUS_WIDTH == 64)
  typedef U64     MHDL;
  #define MS_ADDR_ALIGNED_TRY_OFFSET    7
  #define MS_ADDR_ALIGNED_MASK          0xFFFFFFF8
#else
  typedef U32     MHDL;
  #define MS_ADDR_ALIGNED_TRY_OFFSET    3
  #define MS_ADDR_ALIGNED_MASK          0xFFFFFFF4
#endif

#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief declares function prototypes
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI HAND 	msOS_MUTEX_C		(HAND hSpec, const I08 *pObjName);
OSAPI I32 	msOS_MUTEX_P		(HAND hSync, I32 tmExpired);
OSAPI I32 	msOS_MUTEX_V		(HAND hSync);
OSAPI I32 	msOS_MUTEX_E		(HAND hSync);
OSAPI HAND 	msOS_SEMAP_C		(HAND hSpec, I32 nMaxCnt, I32 nInitCount, const I08 *pObjName);
OSAPI I32 	msOS_SEMAP_P 		(HAND hSync, I32 tmExpired);
OSAPI I32 	msOS_SEMAP_V 		(HAND hSync, I32 nCount);
OSAPI I32 	msOS_SEMAP_E 		(HAND hSync);
#if !defined(MS_APP_SDK_NORDIC_NRF5)
  OSAPI I32	msOS_CRTSEC_C		(HAND hSpec, HAND hSync);
#else
  OSAPI HAND    msOS_CRTSEC_C           (U08 *pCSection);
#endif
OSAPI I32	msOS_CRTSEC_P		(HAND hSync);
OSAPI I32	msOS_CRTSEC_V		(HAND hSync);
OSAPI I32	msOS_CRTSEC_E		(HAND hSync);
OSAPI void 	msOS_Sleep 		(U32 tmElasped);  //millisecond
OSAPI void      msOS_Us_Sleep           (U32 tmElapsed);  //microsecond
OSAPI U64	msOS_Ticks		();

OSAPI void	msOS_MEMSetHandle 	(VOIDPTR pStartAddr, I32 heapSize);
OSAPI MHDL	msOS_MEMGetHandle	();
OSAPI void	msOS_MEMReleaseHandle   (MHDL heapHandle);
OSAPI MHDL	msOS_MEMInvoke		(MHDL startAddr, I32 heapSize);
OSAPI I32	msOS_MEMShutDown	(MHDL hHeap);
OSAPI void*	msOS_MEMAlloc		(I32 szUserMem);
OSAPI void	msOS_MEMFree		(MHDL hUserMem);
OSAPI void      msOS_MEMCopy            (VOIDPTR destAddr, VOIDPTR srcAddr, U32 length);
OSAPI I32	msOS_MEMUsage		();
OSAPI I32	msOS_MEMWritable	();
OSAPI MHDL	msOS_MEMBaseAddress	(MHDL userAddress);
OSAPI void      msOS_MEMSet             (VOIDPTR destAddr, I32 value, U32 length); // add by cspark, 2022-06-09

#if defined (__cplusplus)
}
#endif


#endif // MS_OS_H