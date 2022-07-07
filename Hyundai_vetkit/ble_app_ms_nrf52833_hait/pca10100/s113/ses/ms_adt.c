/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_adt.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_adt.h"


#if defined (__cplusplus)
extern "C" {
#endif


/**
 * @brief Function to set the key value
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
__INLINE void msSetKeyValues(UUKEY *key, I32 code, U32 data, I64 value1, U64 value2) {
  key->code = code;
  key->data = data;
  key->key0 = value1;
  key->key1 = value2;
}


/**
 * @brief Function to allocate a Node
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DNODE* LDNODE_Alloc(U64 UUID, U64 code, UUKEY *pKey, U64 user)
{
  MS_DNODE	*pNode;

  if ((pNode = (MS_DNODE*)msOS_MEMAlloc(sizeof(MS_DNODE)))) {
    pNode->m_uuid = UUID;
    pNode->m_code = code;
    if (pKey)
          pNode->m_key = *pKey;
    else  pNode->m_key.key0 = MS_INVALID_ID;
    pNode->m_user = user;
    pNode->m_prev = NULL;
    pNode->m_next = NULL;
    pNode->m_fncFree = NULL;
  }
  return pNode;
}


/**
 * @brief Function to free a Node
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  void  LDNODE_Free(MS_DNODE *pNode)
{
  if (pNode)
    msOS_MEMFree((HAND)pNode);
}


/**
 * @brief Function to get the next node
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DNODE*  LDNODE_Next(MS_DNODE *pNode)
{
  return pNode->m_next;
}


/**
 * @brief Function to allocate a LDLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DLIST* LDLIST_Alloc(UUKEY *pStamp, void (*fncFree)(void* user))
{
  MS_DLIST *pList;

  if ((pList = (MS_DLIST*)msOS_MEMAlloc(sizeof(MS_DLIST)))) {	
    pList->m_size = 0;
    if (pStamp)
          pList->m_stamp = *pStamp;
    else  pList->m_stamp.key0 = MS_INVALID_ID;
    pList->m_fncFree = fncFree;
    pList->m_head = NULL;
    pList->m_tail = NULL;
    pList->m_next = NULL;
  }
  return pList;
}


/**
 * @brief Function to initialize a LDLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  void  LDLIST_Init(MS_DLIST *pList, UUKEY *pStamp, void (*fncFree)(void* user))
{
  pList->m_size = 0;
  if (pStamp)
        pList->m_stamp = *pStamp;
  else  pList->m_stamp.key0 = MS_INVALID_ID;
  pList->m_fncFree = fncFree;
  pList->m_head = NULL;
  pList->m_tail = NULL;
  pList->m_next = NULL;
}


/**
 * @brief Function to attach a Node
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  I32 LDLIST_Attach(MS_DLIST *pList, MS_DNODE *pNew) 
{
  MS_DNODE  *pNode = pList->m_tail;

  if (!pNode) {
    pList->m_head = pNew;
    pList->m_head->m_prev = NULL;
    pList->m_head->m_next = NULL;
  } else {
    pNew->m_next  = NULL;
    pNew->m_prev  = pNode;
    pNode->m_next = pNew;
  }
  pList->m_tail = pNew;
  pList->m_size++;
  return MS_TRUE;
}


/**
 * @brief Function to compare two key values
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT
  I64 CompareKeyValues(UUKEY *pSrc, UUKEY *pDst)
{
  if (pDst->key0 != MS_INVALID_ID && 
      pSrc->key0 != MS_INVALID_ID) {
    return (I64)(pSrc->key1 - pDst->key1);
  }
  return 0; 
}


/**
 * @brief Function to attach a DLIST sorted by Node's key
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  I32 LDLIST_AttachKeySorted(MS_DLIST *pList, MS_DNODE *pNew)
{
  MS_DNODE  *pNode = pList->m_tail;

  if (!pNode) {	
    pList->m_head = pNew;
    pList->m_tail = pNew;
  } else { // sorted by the key		
    do { 
      if ((pNew->m_key.key1 > pNode->m_key.key1) ||  // scheduling policy 1) key value 2) priority, the smaller is placed the eariler than the higher (less emergency)
          (pNew->m_key.key1 == pNode->m_key.key1 && pNew->m_key.priority >= pNode->m_key.priority)) {
        pNew->m_next = pNode->m_next;
        pNew->m_prev = pNode;
        if (pNode->m_next != NULL)
          pNode->m_next->m_prev = pNew;
        pNode->m_next = pNew;
        break;
      }
      if (pNode == pList->m_head) { // the new ndoe is the smallest key in list 	
        pNew->m_next = pNode;
        pNode->m_prev = pNew;
        break;
      }
      pNode = pNode->m_prev;
    } while (pNode != NULL);
    if (!pNew->m_prev)
      pList->m_head = pNew;
    if (!pNew->m_next)
      pList->m_tail = pNew;
  }
  pList->m_size++;
  return MS_TRUE;
}



/**
 * @brief Function to detach a node from DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DNODE* LDLIST_Detach(MS_DLIST *pList)
{
  MS_DNODE *pNode;

  if ((pNode = pList->m_head)) {
    pList->m_head = pNode->m_next;
    if (!pList->m_head)
          pList->m_tail = NULL;
    else  pNode->m_next->m_prev = NULL;
    if (--pList->m_size <= 0)
      pList->m_size = 0;
  }
  return pNode;
}


/**
 * @brief Function to peek a node has UUID from DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DNODE* LDLIST_Peek(MS_DLIST *pList,  U64 UUID)
{
  MS_DNODE *pNode = pList->m_head;

  pNode = pList->m_head;
  while (pNode) {	
    if (pNode->m_uuid == UUID)
      return pNode;
    pNode = pNode->m_next;
  }
  return NULL;
}


/**
 * @brief Function to count the number of node has UUID from DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  U32 LDLIST_Count(MS_DLIST *pList,  U64 UUID)
{
  U32 count = 0;
  MS_DNODE *pNode = pList->m_head;

  pNode = pList->m_head;
  while (pNode) {	
    if (pNode->m_uuid == UUID) count++;
    pNode = pNode->m_next;
  }
  return count;
}


/**
 * @brief Function to peek the first node from DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  MS_DNODE* LDLIST_PeekNext(MS_DLIST *pList)
{
  return pList->m_head;
}


/**
 * @brief Function to delete a node has UUID from DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  I32 LDLIST_Delete(MS_DLIST *pList, MS_DNODE *pNode)
{
  if (pNode == pList->m_head) {	
    pList->m_head = pNode->m_next;
    if (!pList->m_head)
         pList->m_tail = NULL;
    else pNode->m_next->m_prev = NULL;
  } else {
    if (!pList->m_head)
      return MS_FALSE;
    pNode->m_prev->m_next = pNode->m_next;
    if (!pNode->m_next)
         pList->m_tail = pNode->m_prev;
    else pNode->m_next->m_prev = pNode->m_prev;
  }
  if (--pList->m_size <= 0)
    pList->m_size = 0;
  return MS_TRUE;
}


/**
 * @brief Function to clear DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  void  LDLIST_Clear(MS_DLIST *pList, I32 freeMode)
{
  MS_DNODE	*pNode;

  while (pList->m_size > 0) {
    pNode = LDLIST_Detach(pList);
    if (pNode) {
#if defined(MS_APP_SDK_NORDIC_NRF5)
      if (pList->m_fncFree)
	pList->m_fncFree((void*)((U32)pNode->m_user));
#else
      if (pList->m_fncFree)
	pList->m_fncFree((void*)pNode->m_user);
#endif
      LDNODE_Free(pNode);
    }
  }
}


/**
 * @brief Function to free DLIST
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  void  LDLIST_Free(MS_DLIST *pList, I32 freeMode)
{
  LDLIST_Clear(pList, freeMode);
  if (freeMode == ALLOC_FREE)
    msOS_MEMFree((HAND)pList);
}


/**
 * @brief Function to initialize Basic Circluar Queue
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_Initialize(MS_BCIRCQ *pQueue, VOIDPTR pAddr, U32 size, U32 unitSizeInBytes, void (*fncCopy)(void* pSrc, void* pDst, I32 size))
{
  if ((U08*)pAddr == NULL) return MS_FAILURE;
  pQueue->m_ix = 0;
  pQueue->m_ox = 0;
  pQueue->m_array = (U08*)pAddr;
  pQueue->m_size = size;
  pQueue->m_unit = msMAXC(sizeof(U08), unitSizeInBytes);
  return MS_SUCCESS;
}


/**
 * @brief Function to get a writable size on queue
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_GetWritable(MS_BCIRCQ *pQueue)
{
  I32 ix = (pQueue->m_ix + 1) % pQueue->m_size;
  return (ix <= pQueue->m_ox) ? (pQueue->m_ox - ix) : (pQueue->m_size - ix + pQueue->m_ox); // size = (ix - ox)
}


/**
 * @brief Function to get a readable size on queue
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_GetReadable(MS_BCIRCQ *pQueue)
{
  return (pQueue->m_ox <= pQueue->m_ix) ? (pQueue->m_ix - pQueue->m_ox) : (pQueue->m_size - pQueue->m_ox + pQueue->m_ix); 
}


/**
 * @brief Function to read a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_ReadValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr) 
{
  U32 i, ox = pQueue->m_ox;

  if (ox == pQueue->m_ix) return 0;
  U08* pSrc = pQueue->m_array + ox * pQueue->m_unit;
  U08 *pDst = (U08*)pAddr;
  for (i = 0; i < pQueue->m_unit; i++) {
    *pDst++ = *pSrc++;
  }
  pQueue->m_ox = (ox + 1) % pQueue->m_size;
  return 1;
}


/**
 * @brief Function to read a array defined as size
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_ReadArray(MS_BCIRCQ *pQueue, VOIDPTR pAddr, I32 size)
{
  U32 i, cn = 0;
  U32 ox = pQueue->m_ox;  
  
  U08* pSrc = pQueue->m_array + ox * pQueue->m_unit;
  U08 *pDst = (U08*)pAddr;
  while (size-- > 0) {
    if (ox == pQueue->m_ix) break;
    for (i = 0; i < pQueue->m_unit; i++) { // read as many as the unit number
      *pDst++ = *pSrc++;
    }
    cn++;
    ox = (ox + 1) % pQueue->m_size;
  }
  pQueue->m_ox = ox;
  return cn;
}

/**
 * @brief Function to skip a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
ADT 
  I32 BCIRCQ_SkipValue(MS_BCIRCQ *pQueue) 
{
  U32 i, ox = pQueue->m_ox;

  if (ox == pQueue->m_ix) return 0;
  pQueue->m_ox = (ox + 1) % pQueue->m_size;
  return 1;
}


/**
 * @brief Function to peek a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_PeekValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr) 
{
  U32 i, ox = pQueue->m_ox;

  if (ox == pQueue->m_ix) return 0;
  U08* pSrc = pQueue->m_array + ox * pQueue->m_unit;
  U08 *pDst = (U08*)pAddr;
  for (i = 0; i < pQueue->m_unit; i++) {
    *pDst++ = *pSrc++;
  }
  return 1;
}

/**
 * @brief Function to flush a reading pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  void BCIRCQ_ReadFlush(MS_BCIRCQ *pQueue)
{
  pQueue->m_ox = pQueue->m_ix;
}


/**
 * @brief Function to write a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_WriteValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr)
{
  U32 i, nx, ix = pQueue->m_ix;
   
  nx = (ix + 1) % pQueue->m_size;
  if (nx == pQueue->m_ox) return 0;
  U08 *pSrc = (U08*)pAddr;
  U08* pDst = pQueue->m_array + ix * pQueue->m_unit;
  for (i = 0; i < pQueue->m_unit; i++) {
    *pDst++ = *pSrc++; 
  }
  pQueue->m_ix = nx;
  return 1;
}


/**
 * @brief Function to write a array defined as size
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  I32 BCIRCQ_WriteArray(MS_BCIRCQ *pQueue, VOIDPTR pAddr, I32 size)
{
  U32 nx, ix = pQueue->m_ix;
  U32 i, cn = 0;

  U08 *pSrc = (U08*)pAddr;
  U08* pDst = pQueue->m_array + ix * pQueue->m_unit;
  while (size-- > 0) {
    nx = (ix + 1) % pQueue->m_size;
    if (nx == pQueue->m_ox) break;
    for (i = 0; i < pQueue->m_unit; i++) {
      *pDst++ = *pSrc++; 
    }
    cn++;
    ix = nx;
  }
  pQueue->m_ix = ix;
  return cn;
}


/**
 * @brief Function to flush a writing pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
ADT 
  void BCIRCQ_WriteFlush(MS_BCIRCQ *pQueue)
{
  pQueue->m_ix = pQueue->m_ox;
}


#if defined (__cplusplus)
}
#endif