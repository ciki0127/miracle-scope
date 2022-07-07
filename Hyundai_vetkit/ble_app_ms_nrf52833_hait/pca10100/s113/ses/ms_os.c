/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_os.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released
 *                2022/06/01 - add MEMCopy()
 *				  															   
 *******************************************************************************/
#include "ms_os.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/**
 * @brief external and internal global & static variable declaration
 */
#if defined(MS_WINDOW)
#elif defined(MS_LINUX)
    #if defined(PTHREAD)
        typedef struct MS_PMUTEX
        {   pthread_cond_t      m_Cond;
            pthread_mutex_t     m_Mutex;
            I32			m_nCount;
        } MS_PMUTEX;    
        typedef struct MS_PSEMAPHORE
        {   pthread_cond_t      m_Cond;
            pthread_mutex_t     m_Mutex;
            I32                 m_nCount;
            I32                 m_nMaxCount;
        } MS_PSEMAPHORE;
    #endif
#elif defined(MS_NUCLEUS)
#elif defined(MS_TIDSPBIOS)
#elif defined(MS_QUALCOMREX)
#else
#endif


#if defined (__cplusplus)
extern "C" {
#endif

#if (APP_DYNAMIC_MEMORY_AUTO_DEFINE == 1)
__ALIGN(4) U08  g_MemoryBase[APP_DYNAMIC_ALLOC_MEMORY_SIZE];
#else
HAND            g_MemoryBase  = 0;
#endif

MHDL		g_hHeapMemory = 0;

#if defined(MS_APP_SDK_NORDIC_NRF5)
static U08      m_os_ciritical_section = 0;
#endif

/**
 * @brief defines a memory manager data structure
 */
typedef struct MWHEAP_MASTER {
  volatile MHDL			heap_addr; // header, (1)
  volatile MHDL			heap_size; // header, (2)
  volatile U32			unique_cd;
  volatile U32			curr_size;
  volatile MHDL			prev_addr;
  volatile MHDL			next_addr;
} MWHEAP_MASTER;

typedef struct MWHEAP_BLOCK {
  volatile U32			unique_cd;
  volatile U32			curr_size;
  volatile MHDL			prev_addr;
  volatile MHDL			next_addr;
  #if defined(MEMORY_TRACE)
  I08				string[8];
  #endif
} MWHEAP_BLOCK;

typedef struct MWHEAP_DEAMON {
  I32				n_dummy;
  volatile I32			n_alloc;
  volatile HAND			h_mutex;
  volatile MWHEAP_MASTER*	p_master;
} MWHEAP_DEAMON;


/**
  @ Special Note : In Case Nest Function Call, How to properly enable/disable interrupts in ARM Cortex-M
    void ImportantFunction1(void) {
      uint32_t primask;

      // do some stuff here which can be interrupted

      // read PRIMASK register, check interrupt status before you disable them
      // return 0 if they are enabled, or non-zero if disabled
      prim = __get_PRIMASK();

      // disable interrupts
      __disable_irq();

      // do some stuff here which can not be interruted

      // call subfuncion
      ImportantFunction2();

      // do some stuff here which can not be interrupted
      // this part is still interrupte safe because ImportantFunction2 will not enable interrupts

      // enable interrupts back
      if (!primask) {
        __enable_irq();
      }

      // do some stuff here which can be interrupted
    }

    void ImportantFunction2(void) {
      uint32_t primask;

      // do some stuff here which can be interrupted

      // read PRIMASK register, check interrupt status before you disable them
      // return 0 if they are enabled, or non-zero if disabled
      prim = __get_PRIMASK();

      // disable interrupts
      __disable_irq();

      // do some stuff here which can not be interruted

      // enable interrupts back only if they were enabled before we disable it here in this function
      if (!primask) {
        __enable_irq();
      }

      // do some stuff here which can be interrupted
    }
 */

/**
 * @brief Function to create an instance of creatical section
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
#if !defined(MS_APP_SDK_NORDIC_NRF5)
  OSAPI I32 msOS_CRTSEC_C(HAND hSpec, HAND hSync)
#else
  OSAPI HAND msOS_CRTSEC_C(U08 *pCSection)
#endif
{
  #if defined(MS_WINDOW)
    if (hSync){	
      InitializeCriticalSection((CRITICAL_SECTION*)hSync);
      return MS_SUCCESS;
  }
  #elif defined(MS_LINUX)
    #if defined(PTHREAD)
      pthread_mutex_t *hMutex = (pthread_mutex_t*)hSync; 
      if (hMutex) {   
        if (!pthread_mutex_init (hMutex, NULL))
          return MS_SUCCESS;
      }        
    #endif //PTHREAD
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    *pCSection = 0;
    return (HAND)pCSection;
  #else
  #endif	
  return MS_FAILURE;
}


/**
 * @brief Function to get an access right of creatical section
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  I32 msOS_CRTSEC_P(HAND hSync)
{
  #if defined(MS_WINDOW)
    if (hSync) {
      EnterCriticalSection ((CRITICAL_SECTION*)hSync);
      return MS_SUCCESS;
    } 
    return MS_FAILURE;
  #elif defined(MS_LINUX)
    pthread_mutex_t *hMutex = (pthread_mutex_t*)hSync;
    if (hMutex) {
      if (!pthread_mutex_lock (hMutex))
        return MS_SUCCESS;
    }        
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    U08 critical_nested = 0;
    return critical_nested;
  #else
  #endif
  return MS_FAILURE;	
}


/**
 * @brief Function to release an access right of creatical section
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  I32 msOS_CRTSEC_V(HAND hSync)
{
  #if defined(MS_WINDOW)
    if (hSync) {
      LeaveCriticalSection((CRITICAL_SECTION*)hSync);
      return MS_SUCCESS;
    }
  #elif defined(MS_LINUX)
    pthread_mutex_t *hMutex = (pthread_mutex_t*)hSync;
    if (hMutex) {
      if (!pthread_mutex_unlock (hMutex))
        return MS_SUCCESS;
    }
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    return MS_SUCCESS;
  #else
  #endif
  return MS_FAILURE;
}


/**
 * @brief Function to delete an instance of creatical section
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  I32 msOS_CRTSEC_E(HAND hSync)
{
  #if defined(MS_WINDOW)
    if (hSync) {
      DeleteCriticalSection((CRITICAL_SECTION*)hSync);
      return MS_SUCCESS;
    }
  #elif defined(MS_LINUX)
    pthread_mutex_t *hMutex = (pthread_mutex_t*)hSync;
    if (hMutex) {
      if (!pthread_mutex_destroy (hMutex))
        return MS_SUCCESS;
    }
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    return MS_SUCCESS;
  #else
  #endif
  return MS_FAILURE;
}



/**
 * @brief Function to release an access right of creatical section
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */


/**
 * @brief Function to get an system tick counter
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI 
  U64 msOS_Ticks()
{
  U64 tmTicks = 0;


  #if defined(MS_WINDOW)
    tmTicks = GetTickCount ();
  #elif defined(MS_LINUX)
    struct timeval   tv;
    gettimeofday (&tv, NULL);
    tmTicks = tv.tv_sec*1000 + tv.tv_usec/1000;
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_ADIMICOM)
    tmTicks = 0;
  #else
  #endif

  return tmTicks;
}


/**
 * @brief Function to sleep in millisecond
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  void  msOS_Sleep(U32 tmElapsed)
{
  #if defined(MS_WINDOW)
    if (tmElapsed > APP_TIMER_GRANUALITY)
    {   tmElapsed -= APP_TIMER_GRANUALITY;
	if (tmElapsed <= 0)
          tmElapsed = 1;
    }
    Sleep (tmElapsed);
  #elif defined(MS_LINUX)
    if (tmElapsed > APP_TIMER_GRANUALITY)
    {   tmElapsed -= APP_TIMER_GRANUALITY;
	if (tmElapsed <= 0)
          tmElapsed = 1;
    }
    usleep (tmElapsed*1000);	//microsecon, 1 millisecond
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_ADIMICOM)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    msOS_Us_Sleep(tmElapsed * 1000);
  #else
  #endif
}


/**
 * @brief Function to sleep in microsecond
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  void  msOS_Us_Sleep(U32 tmElapsed)
{
  #if defined(MS_WINDOW)
    if (tmElapsed > APP_TIMER_GRANUALITY)
    {   tmElapsed -= APP_TIMER_GRANUALITY;
	if (tmElapsed <= 0)
          tmElapsed = 1;
    }
    Sleep (tmElapsed);
  #elif defined(MS_LINUX)
    if (tmElapsed > APP_TIMER_GRANUALITY)
    {   tmElapsed -= APP_TIMER_GRANUALITY;
	if (tmElapsed <= 0)
          tmElapsed = 1;
    }
    usleep (tmElapsed);	//microsecon, 1 millisecond
  #elif defined(MS_NUCLEUS)
  #elif defined(MS_TIDSPBIOS)
  #elif defined(MS_QUALCOMREX)
  #elif defined(MS_ADIMICOM)
  #elif defined(MS_APP_SDK_NORDIC_NRF5)
    nrf_delay_us(tmElapsed);
  #else
  #endif
}


#define MS_OSMEM_HEAP_CD		0x10011001
#define MS_OSMEM_BLOCK_CD		0x01100110
#define MS_OSMEM_CLEAR_CD		0
#define MS_OSMEM_EMPTY			0
#define MS_OSMEM_NULL			0

/**
 * @brief Function to set the base memory address for the customed dyanmic management
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI 
  void	msOS_MEMSetHandle(VOIDPTR pStartAddr, I32 heapSize)
{
  g_hHeapMemory = msOS_MEMInvoke((MHDL)pStartAddr, heapSize);
}


/**
 * @brief Function to get the base memory address
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI 
  HAND	msOS_MEMGetHandle()
{
  return g_hHeapMemory;
}


/**
 * @brief Function to release the base memory address
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI 
  void	msOS_MEMReleaseHandle (HAND heapHandle)
{
  if (heapHandle == g_hHeapMemory) {
    msOS_MEMShutDown(g_hHeapMemory);
    g_hHeapMemory = 0;
  }
}

/**
 * @brief Function to setup the memory manager
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI
  HAND	msOS_MEMInvoke (HAND pStartAddr, I32 heapSize)
{
  const I32 	szSafetyZone = 16;
  MWHEAP_DEAMON	*pHeap  = NULL;

  #if (APP_DYNAMIC_MEMORY_AUTO_DEFINE == 1)
    pStartAddr = (HAND)g_MemoryBase;
  #endif
  pHeap = (MWHEAP_DEAMON*)pStartAddr;

  #if (APP_DYNAMIC_ALLOC_MEMORY_SIZE > 0)
    heapSize = APP_DYNAMIC_ALLOC_MEMORY_SIZE;
  #endif

  pHeap->n_dummy  = 0x10011001;
  pHeap->n_alloc  = 0;
#if !defined(MS_APP_SDK_NORDIC_NRF5)
  pHeap->h_mutex  = msOS_CRTSEC_C((HAND)0, NULL);
#else
  pHeap->h_mutex  = msOS_CRTSEC_C(&m_os_ciritical_section);
#endif
  if (pHeap->h_mutex) {
    msOS_CRTSEC_P(pHeap->h_mutex);
    pHeap->p_master = (MWHEAP_MASTER*)(pStartAddr + sizeof(MWHEAP_DEAMON) + szSafetyZone);
    pHeap->p_master->heap_addr = (HAND)pHeap->p_master;
    pHeap->p_master->heap_size = heapSize - (pHeap->p_master->heap_addr - pStartAddr);
    pHeap->p_master->unique_cd = 0x01100110;
    pHeap->p_master->curr_size = 0;
    pHeap->p_master->prev_addr = 0;
    pHeap->p_master->next_addr = pHeap->p_master->heap_addr + pHeap->p_master->heap_size;
    msOS_CRTSEC_V(pHeap->h_mutex);
  } else {
    pHeap = NULL;
  }
  return (HAND)pHeap;
}

/**
 * @brief Function to shutdown the memory manager
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  I32 msOS_MEMShutDown(HAND hHeap)
{
  MWHEAP_DEAMON	*pHeap = (MWHEAP_DEAMON*)hHeap;

  if (!pHeap->n_alloc && pHeap->n_dummy == MS_OSMEM_HEAP_CD) {
    msOS_CRTSEC_P(pHeap->h_mutex);
    pHeap->n_dummy = MS_OSMEM_CLEAR_CD;
    pHeap->p_master->heap_addr = (MHDL)MS_OSMEM_NULL;
    pHeap->p_master->heap_size = MS_OSMEM_EMPTY;
    pHeap->p_master->unique_cd = MS_OSMEM_CLEAR_CD;
    pHeap->p_master->curr_size = MS_OSMEM_EMPTY;
    pHeap->p_master->next_addr = (MHDL)MS_OSMEM_NULL;
    pHeap->p_master->prev_addr = (MHDL)MS_OSMEM_NULL;
    pHeap->p_master = NULL;
    msOS_CRTSEC_V(pHeap->h_mutex);
    msOS_CRTSEC_E(pHeap->h_mutex);
    pHeap->h_mutex = (HAND)0;
    return MS_SUCCESS;
  }
  return MS_FAILURE;
}


/**
 * @brief Function to allocate the user memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI
  void* msOS_MEMAlloc (I32 szUserMem)
{
  MWHEAP_BLOCK*   tempBlock;
  MWHEAP_BLOCK*   currentBlock;
  MWHEAP_BLOCK*   nextBlock;
  MWHEAP_DEAMON*  pHeap = (MWHEAP_DEAMON*)g_hHeapMemory;
  HAND            heapEndAddr;
  HAND            pMemo = 0;
  I32             unUsedMemSz;
  I32             sizeOfBlock = sizeof(MWHEAP_BLOCK);
  I32             szMasterHeader = sizeof(HAND) * 2; 

  if (pHeap->n_dummy != 0x10011001) {
    return NULL;
  }
  szUserMem  += 7 + sizeOfBlock; // 8 bytes aligned
  szUserMem  &= 0xFFFFFFF8;
  msOS_CRTSEC_P(pHeap->h_mutex);
  currentBlock = (MWHEAP_BLOCK*)(pHeap->p_master->heap_addr + szMasterHeader); 
  heapEndAddr = pHeap->p_master->heap_addr + pHeap->p_master->heap_size;
  
  #if defined(MEMOMRY_DEBUG_PRINT)
    msOS_MemPrintDebug(currentBlock, heapEndAddr, " Alloc() - Before "); 
  #endif

  while ((HAND)currentBlock < heapEndAddr) {	
    if (!currentBlock->curr_size) {	
      unUsedMemSz = currentBlock->next_addr - (HAND)currentBlock;
      nextBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr); // 2021-11-05
      if (unUsedMemSz > szUserMem + sizeOfBlock) {	
        tempBlock = (MWHEAP_BLOCK*)((HAND)currentBlock + szUserMem);
	tempBlock->unique_cd = 0x01100110;
	tempBlock->curr_size = 0;
	tempBlock->prev_addr = (HAND)currentBlock;
	tempBlock->next_addr = currentBlock->next_addr;
	currentBlock->next_addr = (HAND)tempBlock;
        currentBlock->unique_cd = 0x01100110;
	currentBlock->curr_size = szUserMem;
	pHeap->n_alloc++;
	pMemo = (HAND)currentBlock + sizeOfBlock;
        break;
      } 
      else if (unUsedMemSz == szUserMem && nextBlock->curr_size > 0) { // 2021-11-05
        currentBlock->unique_cd = 0x01100110;
	currentBlock->curr_size = szUserMem;
	pHeap->n_alloc++;
	pMemo = (HAND)currentBlock + sizeOfBlock;
	break;
      }
    }
    currentBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
  }
  msOS_CRTSEC_V(pHeap->h_mutex);
  #if defined(MEMOMRY_DEBUG_PRINT)
    msOS_MemPrintDebug((MWHEAP_BLOCK*)(pHeap->p_master->heap_addr + szMasterHeader), heapEndAddr, " Alloc() - After "); 
  #endif
  return (void*)pMemo;
}


/**
 * @brief Function to free the allocated memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

OSAPI
  void  msOS_MEMFree (HAND hUserMem)
{	
  MWHEAP_BLOCK*	nextBlock;
  MWHEAP_BLOCK*	currentBlock;
  HAND		heapEndAddr;
  HAND		heapBlkAddr;
  I32		sizeOfBlock = sizeof(MWHEAP_BLOCK);
  MWHEAP_DEAMON	*pHeap = (MWHEAP_DEAMON*)g_hHeapMemory;

  if (hUserMem == 0) return; // add to prevent NULL pointer, 2022-06-01
  msOS_CRTSEC_P(pHeap->h_mutex);
  currentBlock = (MWHEAP_BLOCK*)((HAND)hUserMem - sizeOfBlock);
  if (pHeap->n_dummy != 0x10011001 || !currentBlock->curr_size || currentBlock->unique_cd != 0x01100110)
  {	
    msOS_CRTSEC_V(pHeap->h_mutex);
    return;
  }
  pHeap->n_alloc--;
  currentBlock->unique_cd = 0;
  currentBlock->curr_size = 0;
  heapBlkAddr = (HAND)currentBlock;
  heapEndAddr = pHeap->p_master->heap_addr + pHeap->p_master->heap_size;
  while (currentBlock->prev_addr) { // try garbage collection
    if (currentBlock->curr_size)
      break;
    currentBlock = (MWHEAP_BLOCK*)(currentBlock->prev_addr);
  }
  while ((HAND)currentBlock < heapEndAddr) {
    if (!currentBlock->curr_size) {
      nextBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
      if ((HAND)nextBlock == heapEndAddr) 
        break;	// end of heap
      if (!nextBlock->curr_size) {
        currentBlock->next_addr = nextBlock->next_addr;
	if (currentBlock->next_addr < heapEndAddr) {
          ((MWHEAP_BLOCK*)(nextBlock->next_addr))->prev_addr = (HAND)currentBlock;
        }
        nextBlock->curr_size = 0;
        nextBlock->unique_cd = 0;
      } 
      else {
        currentBlock = (MWHEAP_BLOCK*)(nextBlock->next_addr);
        if ((HAND)currentBlock > heapBlkAddr)	
          break;
      }
    } else {
      currentBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
    }
  }
  msOS_CRTSEC_V(pHeap->h_mutex);
  #if defined(MEMOMRY_DEBUG_PRINT)
    currentBlock = (MWHEAP_BLOCK*)(pHeap->p_master->heap_addr + sizeof(HAND) * 2); 
    msOS_MemPrintDebug(currentBlock, heapEndAddr, " Free() - After "); 
  #endif
}

/**
 * @brief Function to copy destination from source
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  void  msOS_MEMCopy(VOIDPTR destAddr, VOIDPTR srcAddr, U32 length)
{	
  U08 *pDstAddr = (U08*)destAddr;
  U08 *pSrcAddr = (U08*)srcAddr;
  U08 *pEndAddr;

  pEndAddr = pSrcAddr + length;
  while (pSrcAddr < pEndAddr) {
    *pDstAddr++ = *pSrcAddr++;
  }
}


/**
 * @brief Function to set all array as a value
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  void  msOS_MEMSet(VOIDPTR destAddr, I32 value, U32 length)
{	
  U08 *pDstAddr = (U08*)destAddr;
  U08 *pEndAddr;

  pEndAddr = pDstAddr + length;
  while (pDstAddr < pEndAddr) {
    *pDstAddr++ = (U08)value;
  }
}


/**
 * @brief Function to get the amount of allocated memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  I32 msOS_MEMUsage()
{
  MWHEAP_DEAMON	*pHeap = (MWHEAP_DEAMON*)g_hHeapMemory;
  MWHEAP_BLOCK*	currentBlock;
  MHDL		heapEndAddr;
  I32		usage = 0, szMasterHeader = sizeof(MHDL) * 2;

  msOS_CRTSEC_P(pHeap->h_mutex);
  currentBlock = (MWHEAP_BLOCK*)(pHeap->p_master->heap_addr + szMasterHeader); 
  heapEndAddr = pHeap->p_master->heap_addr + pHeap->p_master->heap_size;
  while((MHDL)currentBlock < heapEndAddr) {
    usage += currentBlock->curr_size; // only add an actual amount except the block's header
    currentBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
  }
  msOS_CRTSEC_V(pHeap->h_mutex);
  return usage;
}


/**
 * @brief Function to get the amount of unallocated memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
 OSAPI
  I32 msOS_MEMWritable()
{
  MWHEAP_DEAMON	*pHeap = (MWHEAP_DEAMON*)g_hHeapMemory;
  MWHEAP_BLOCK*	currentBlock;
  MHDL		heapEndAddr;
  I32		szWritable = 0;
  I32		szMasterHeader = sizeof(MHDL) * 2;

  // In case U64 has 8 bytes length, the default overhead is 56 bytes,
  // sizeof(DAEMON, 24) + sizeof(MASTER.heap_addr, 16) + sizeof(MASTER.heap_size, 8) + sizeof(SafetyZone, 8)
  // and each block has 24 bytes overhead to keep the track of heap management.
  msOS_CRTSEC_P(pHeap->h_mutex);
  currentBlock = (MWHEAP_BLOCK*)(pHeap->p_master->heap_addr + szMasterHeader); 
  heapEndAddr = pHeap->p_master->heap_addr + pHeap->p_master->heap_size;
  while((MHDL)currentBlock < heapEndAddr) {
    if (currentBlock->curr_size == MS_OSMEM_EMPTY) {
      szWritable += currentBlock->next_addr - (MHDL)currentBlock;
     }
    currentBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
  }
  msOS_CRTSEC_V(pHeap->h_mutex);
  return szWritable;
}

/**
 * @brief Function to get the base address of an allocated user memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI 
  MHDL	msOS_MEMBaseAddress(MHDL userAddress)
{
  return (MHDL)(userAddress - sizeof(MWHEAP_BLOCK));
}


#if defined(MEMOMRY_DEBUG_PRINT)
/**
 * @brief Function to get the amount of unallocated memory
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
OSAPI
  void  msOS_MemPrintDebug(MWHEAP_BLOCK *currentBlock, MHDL heapEndAddr, char *message)
{
  printf("---- mTVOS : %s ---- \n", message);
  while((MHDL)currentBlock < heapEndAddr) {
    printf("currentBlock %ld, curr_size %d, prev_addr %ld, next_addr %ld\n", (MHDL)currentBlock, currentBlock->curr_size, currentBlock->prev_addr, currentBlock->next_addr);
    currentBlock = (MWHEAP_BLOCK*)(currentBlock->next_addr);
  }
  printf("\n\n");
}
#endif


#if defined (__cplusplus)
}
#endif
