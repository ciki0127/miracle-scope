/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_adt.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released
 *                2022/06/03 - modify BCIRCQ APIs to handle any type of data 
 *                             which will be write to or read from
 *				  															   
 *******************************************************************************/

#if !defined (MS_ADT_H)
#define MS_ADT_H
#include "ms_datatype.h"
#include "ms_os.h"

#define DONT_FREE		0x00
#define	ALLOC_FREE		0x01
#ifndef ADT
  #define ADT
#endif

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct MS_DNODE {	
  U64                 m_user;
  U64                 m_code;
  U64                 m_uuid;
  UUKEY               m_key;
  void                (*m_fncFree)(void* pUser);
  struct MS_DNODE*    m_next;
  struct MS_DNODE*    m_prev;
} MS_DNODE;

typedef struct MS_DLIST {
  I32                 m_size;
  UUKEY               m_stamp;
  void                (*m_fncFree)(void* pUser);
  MS_DNODE*           m_head;
  MS_DNODE*           m_tail;
  struct MS_DLIST*    m_next; 
} MS_DLIST;

typedef struct MS_BCIRCQ {
  volatile U32        m_ix;
  volatile U32        m_ox;
  U08*                m_array;
  U32                 m_size;
  U32                 m_unit; // sizeof(struct or variable) in byte
} MS_BCIRCQ;


/**
 * @brief Function to set the key value
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msSetKeyValues(UUKEY *key, I32 code, U32 data, I64 value1, U64 value2);

/**
 * @brief declares function prototypes of double linked node and list
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

ADT MS_DNODE*	LDNODE_Alloc	(U64 UUID, U64 code, UUKEY *pKey, U64 user);
ADT void	LDNODE_Free	(MS_DNODE *pNode);
ADT MS_DNODE*   LDNODE_Next     (MS_DNODE *pNode);
ADT MS_DLIST*   LDLIST_Alloc    (UUKEY *pStamp, void (*fncFree)(void* user));
ADT void        LDLIST_Init     (MS_DLIST *pList, UUKEY *pStamp, void (*fncFree)(void* pUser)); 
ADT I32         LDLIST_Attach   (MS_DLIST *pList, MS_DNODE *pNew); 
ADT MS_DNODE*   LDLIST_Detach   (MS_DLIST *pList); 
ADT MS_DNODE*   LDLIST_Peek     (MS_DLIST *pList, U64 UUID);
ADT MS_DNODE*   LDLIST_PeekNext (MS_DLIST *pList);
ADT U32         LDLIST_Count    (MS_DLIST *pList,  U64 UUID);
ADT I32         LDLIST_Delete   (MS_DLIST *pList, MS_DNODE *pNode);
ADT void        LDLIST_Clear	(MS_DLIST *pList, I32 freeMode);
ADT void        LDLIST_Free	(MS_DLIST *pList, I32 freeMode);
ADT I32		LDLIST_AttachKeySorted(MS_DLIST *pList, MS_DNODE *pNew);
ADT I64         CompareKeyValues(UUKEY *pSrc, UUKEY *pDst);

ADT I32         BCIRCQ_Initialize(MS_BCIRCQ *pQueue, VOIDPTR pAddr, U32 size, U32 unitSizeInBytes, void (*fncCopy)(void* pSrc, void* pDst, I32 size));
ADT I32         BCIRCQ_GetWritable(MS_BCIRCQ *pQueue);  // writable size
ADT I32         BCIRCQ_GetReadable(MS_BCIRCQ *pQueue);  // readable size
ADT I32         BCIRCQ_ReadValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr);
ADT I32         BCIRCQ_ReadArray(MS_BCIRCQ *pQueue, VOIDPTR pAddr, I32 size);
ADT void        BCIRCQ_ReadFlush(MS_BCIRCQ *pQueue);
ADT I32         BCIRCQ_WriteValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr);
ADT I32         BCIRCQ_WriteArray(MS_BCIRCQ *pQueue, VOIDPTR pAddrr, I32 size);
ADT void        BCIRCQ_WriteFlush(MS_BCIRCQ *pQueue);
ADT I32         BCIRCQ_SkipValue(MS_BCIRCQ *pQueue);
ADT I32         BCIRCQ_PeekValue(MS_BCIRCQ *pQueue, VOIDPTR pAddr);


#if defined (__cplusplus)
}
#endif

#endif //MS_ADT_H