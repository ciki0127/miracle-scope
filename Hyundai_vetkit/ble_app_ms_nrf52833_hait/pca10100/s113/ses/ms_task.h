/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_task.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#ifndef MS_TASK_H
#define MS_TASK_H
#include "ms_app_config.h"
#include "ms_timer.h"
#include "ms_adt.h"
#include "ms_os.h"


/**
 * @brief defines useful macros
 */
#ifndef TSKAPI
  #define TSKAPI
#endif

#ifndef MS_APP_TSK_GRANUALITY
#define MS_APP_TSK_GRANUALITY                   1
#endif

#ifndef MS_APP_TSK_REQUEST_MAPQ_MAX
#define MS_APP_TSK_REQUEST_MAPQ_MAX             6   // 4
#endif

#ifndef MS_APP_TSK_REQUEST_MAX_NUM_PER_QID
#define MS_APP_TSK_REQUEST_MAX_NUM_PER_QID      10
#endif



#if defined (__cplusplus)
extern "C" {
#endif

typedef void (*TASK_HANDLER)(U64 uuid, I32 event, U64 user);

typedef enum {
  /* insert here !! */
  MS_TASK_UNDEFINE    = 0,
  MS_TASK_ONESHORT    = 1,
  MS_TASK_PERIODIC    = 2,
} TASK_TYPE;

typedef enum {
  /* insert here !! */
  MS_TASK_CMD_UNDEFINE  = 0,
  MS_TASK_CMD_START     = 1,
  MS_TASK_CMD_STOP      = 2,
  MS_TASK_CMD_FLUSH     = 3,
} TASK_CMD_TYPE;

enum {
  /* insert here !! */
  MS_TASK_OP_UNDEFINE   = 0,
  MS_TASK_OP_READ       = 1,
  MS_TASK_OP_WRITE      = 2,
  MS_TASK_OP_READWRITE  = 3,
  MS_TASK_OP_WRITEREAD  = 4
};

#ifndef MS_TSK_REQUEST_PRIORITY_MIN
#define MS_TSK_REQUEST_PRIORITY_MIN     1
#endif

#ifndef MS_TSK_REQUEST_PRIORITY_MAX
#define MS_TSK_REQUEST_PRIORITY_MAX     7
#endif

#ifndef MS_TSK_REQUEST_PRIORITY_NOR
#define MS_TSK_REQUEST_PRIORITY_NOR     4
#endif

#ifndef MS_TSK_REQUEST_DELAY_MS
#define MS_TSK_REQUEST_DELAY_MS         1
#endif

#ifndef MS_TSK_REQUEST_MAP_INVALID_ID
#define MS_TSK_REQUEST_MAP_INVALID_ID   -1
#endif

#ifndef MS_SCH_DAEMON_MAX_PERIOD              //! should be smaller than the half value of the TIMER Counter overflow
#define MS_SCH_DAEMON_MAX_PERIOD        60000 // 1 minute(normally)
#endif


typedef struct MS_TASK_QUEMAP {
  volatile U32        m_ix;
  volatile U32        m_ox;
  U32                 m_size;
} MS_TASK_QUEMAP;


typedef struct MS_TASK_REQUEST {
  TASK_TYPE           m_type;
  I16                 m_priority;       // [1 .. 7], the lower value is more higher urgency
  U16                 m_command;        // TASK_CMD_TYPE
  I32                 m_event;
  TASK_HANDLER        m_fncHandler;
  U64                 m_uuid;           // defined using msFOURCC('N', 'U', 'L', 'L') macro
  U64                 m_expired;        // period
  U64                 m_nextFired;      // the next fired time in millisecond
  U64                 m_user;
} MS_TASK_REQUEST;


/**
 * @brief Function to get the current timer in millisecond of the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U64 msTSK_NowInMillisecond();

/**
 * @brief Function to get the current timer in microsecond of the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U64 msTSK_NowInMicrosecond();

/**
 * @brief Function to create the task scheduler that can process fnAppBackground 
 *        predefined MS_SCH_DAEMON_RUN_IN_MIILISECOND period 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msSHD_Start(U64 (*fnGetTime)(), TASK_HANDLER fnAppBackgroud, U32 expired, U64 user, U64 event);

/**
 * @brief Function to delete the task scheduler.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msSHD_Stop();

/**
 * @brief Function to clear all task in scheduler.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msSHD_Clear();

/**
 * @brief Function to get an access right of creatical section
 *        UUID is basically managed as one session from msTSK_Start() to msTSK_Stop().
 *        A 2-byte session ID is included in UUID in msMAKEUUID macro. There may be 
 *        multiple TASKs having the same UUID, and using the TASK UUID, it is possible
 *        to create a task and delete all tasks.ㅁㅁ
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_Start(I32 qid, TASK_TYPE type, I16 priority, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler);

/**
 * @brief Function to trigger the task start command after delayed the specified time 
 *        UUID is basically managed as one session from msTSK_Start() to msTSK_Stop().
 *        A 2-byte session ID is included in UUID in msMAKEUUID macro. There may be 
 *        multiple TASKs having the same UUID, and using the TASK UUID, it is possible
 *        to create a task and delete all tasks.ㅁ
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_StartDelayed(I32 qid, TASK_TYPE type, I16 priority, U64 startOnTime, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler);

/**
 * @brief Function to delete a task has UUID in scheduler
 *        UUID is basically managed as one session from msTSK_Start() to msTSK_Stop().
 *        A 2-byte session ID is included in UUID in msMAKEUUID macro. There may be 
 *        multiple TASKs having the same UUID, and using the TASK UUID, it is possible
 *        to create a task and delete all tasks.ㅁ
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_Stop(I32 qid, U64 uuid, U64 user, I32 event, TASK_HANDLER handler);

/**
 * @brief Function to delete a task has UUID in scheduler
 *        Among TASKs with UUIDs, TASKs are deleted before m_nextFired time
 *        In particular, it is appropriate and useful for MS_TASK_ONESHOT.
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_StopUntil(I32 qid, I16 priority, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler);

/**
 * @brief Function to trigger the task flush command 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_Flush(I32 qid, U64 uuid, U64 user, I32 event, TASK_HANDLER handler);


/**
 * @brief Function to read a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadRequest(I32 qid, MS_TASK_REQUEST *pRequest);

/**
 * @brief Function to flush a reading pipe specified QID
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadFlushQid(I32 qid);

/**
 * @brief Function to flush a reading pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadFlush();

/**
 * @brief Function to write a request
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteRequest(I32 qid, MS_TASK_REQUEST *pRequest);

/**
 * @brief Function to flush a writing pipe specified QID
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteFlushQid(I32 qid);

/**
 * @brief Function to flush a writing pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteFlush();

#if defined (__cplusplus)
}
#endif

#endif // MS_TASK_H