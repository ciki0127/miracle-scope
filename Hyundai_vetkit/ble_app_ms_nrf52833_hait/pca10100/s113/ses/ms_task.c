/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_task.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_task.h"
#include "ms_app_utils.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#if defined (__cplusplus)
extern "C" {
#endif


const nrf_drv_timer_t taskTimer = NRF_DRV_TIMER_INSTANCE(MS_APP_TSKSCH_TIMER_ID); // Timer handler for Task Scheduler

#if defined(MS_APP_SDK_NORDIC_NRF5)
  static U08 m_task_ciritical_section = 0;
#endif

nrf_timer_cc_channel_t 
  TASK_COMAPARE_CHANNEL[MS_APP_TSK_REQUEST_MAPQ_MAX] = {
            NRF_TIMER_CC_CHANNEL0,    //! reserved for task scheduler
            NRF_TIMER_CC_CHANNEL1, 
            NRF_TIMER_CC_CHANNEL2,
#if (MS_APP_TSK_REQUEST_MAPQ_MAX > 3)
            NRF_TIMER_CC_CHANNEL3
#endif
            };

typedef struct MS_TSKSCHDULER_2 {
  U32             m_initialized;
  volatile HAND   h_mutex;
  MS_TASK_QUEMAP  m_reqmap[MS_APP_TSK_REQUEST_MAPQ_MAX];
  MS_TASK_REQUEST m_reqinp[MS_APP_TSK_REQUEST_MAPQ_MAX][MS_APP_TSK_REQUEST_MAX_NUM_PER_QID];
  MS_DLIST*       m_list;
  volatile U32    m_previousTickCounter;
  volatile U64    m_tickOverflowCounter;
  U64             m_ticksPerMilliSecond;
  U64             m_counterOverflowValue;
  U64             (*m_fnGetCurrentTime)();
  TASK_HANDLER    m_fnAppBackgroudTask;
} MS_TSKSCHDULER_2;

MS_TSKSCHDULER_2 m_tskScheduler = {
      .m_initialized = MS_FALSE,
      .h_mutex = (HAND)&m_task_ciritical_section, 
      .m_list = NULL,
      .m_fnGetCurrentTime = NULL,
      .m_fnAppBackgroudTask = NULL
     };


/**
 * @brief Function to get the current timer in millisecond of the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U08 string64_2[32];
U64 msTSK_NowInMillisecond() 
{
  U32 currentTickCounter = 0;
  U64 currentInMilliSecond;

  if (m_tskScheduler.m_initialized == MS_FALSE) return 0;
  currentTickCounter = nrf_drv_timer_capture(&taskTimer, NRF_TIMER_CC_CHANNEL3); //if it uses with PPI, then should be use "nrf_drv_timer_capture_get()"
  if (currentTickCounter < m_tskScheduler.m_previousTickCounter) { // handle the overflow of timer's counter
    m_tskScheduler.m_tickOverflowCounter += m_tskScheduler.m_counterOverflowValue;
  }
  m_tskScheduler.m_previousTickCounter = currentTickCounter;
  currentInMilliSecond = (U64)((U64)(m_tskScheduler.m_tickOverflowCounter + currentTickCounter) / (F32)m_tskScheduler.m_ticksPerMilliSecond);
  return currentInMilliSecond;
}

/**
 * @brief Function to get the current timer in microsecond of the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U64 msTSK_NowInMicrosecond()
{
  U32 currentTickCounter = 0;
  U64 currentInMicroSecond;

  if (m_tskScheduler.m_initialized == MS_FALSE) return 0;
  currentTickCounter = nrf_drv_timer_capture(&taskTimer, NRF_TIMER_CC_CHANNEL3);
  if (currentTickCounter < m_tskScheduler.m_previousTickCounter) {
    m_tskScheduler.m_tickOverflowCounter += m_tskScheduler.m_counterOverflowValue;
  }
  m_tskScheduler.m_previousTickCounter = currentTickCounter;
  currentInMicroSecond = (U64)((U64)(m_tskScheduler.m_tickOverflowCounter + currentTickCounter) / (F32)nrf_drv_timer_us_to_ticks(&taskTimer, 1));
  return currentInMicroSecond;
}


/**
 * @brief Function to get the current timer in millisecond of the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msTSK_ResetTimebase(I32 readTick, nrf_drv_timer_config_t *pConfig) 
{
  m_tskScheduler.m_ticksPerMilliSecond = (readTick == MS_FALSE) ? 0 : nrf_drv_timer_ms_to_ticks(&taskTimer, 1); // ticks counter per one millisecond
  m_tskScheduler.m_tickOverflowCounter = 0;
  m_tskScheduler.m_previousTickCounter = 0;
  m_tskScheduler.m_counterOverflowValue = msTimer_CounterOverflowValue(pConfig);
}

/**
 * @brief Function to remove tasks has the requested uuid
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msTSK_RemoveAllHasUUID(MS_TASK_REQUEST *pRequest)
{
  MS_DNODE *pTask = NULL;
  U32 count = LDLIST_Count(m_tskScheduler.m_list, pRequest->m_uuid);
  U64 nowInMilli = 0;
  I32 deletedPartial = (pRequest->m_nextFired > 0) ? MS_TRUE : MS_FALSE;

  while (count-- > 0) {
    pTask = LDLIST_Peek(m_tskScheduler.m_list, pRequest->m_uuid);
    if (pTask != NULL) {
      if (deletedPartial == MS_TRUE && 
          pTask->m_key.key1 > pRequest->m_nextFired) { // to support msTSK_StopUntil()
        continue;
      }
      LDLIST_Delete(m_tskScheduler.m_list, pTask);
      LDNODE_Free(pTask);
    }
  }
  if (pRequest->m_fncHandler != NULL) {   // call fncHandler only once after deleted all task
    pRequest->m_fncHandler(pRequest->m_uuid, pRequest->m_event, pRequest->m_user);
  }
}

/**
 * @brief Function to flush all tasks in scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msTSK_FlushAll(MS_TASK_REQUEST *pRequest)
{
  MS_DNODE *pTask = NULL;
  
  while (true) {
    pTask = LDLIST_Detach(m_tskScheduler.m_list);
    if (pTask == NULL) break;
    LDNODE_Free(pTask);
  }
  if (pRequest->m_fncHandler != NULL) {   // call fncHandler only once after deleted all task
    pRequest->m_fncHandler(pRequest->m_uuid, pRequest->m_event, pRequest->m_user);
  }
}


/**
 * @brief Function to flush all tasks in scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msTSK_RegisterNew(MS_TASK_REQUEST *pRequest)
{
  MS_DNODE *pTask = NULL;
  UUKEY stamp = {
            .code = pRequest->m_event,
            .data = pRequest->m_expired,
            .key0 = pRequest->m_type,
            .key1 = pRequest->m_nextFired
           };
  pTask = LDNODE_Alloc(pRequest->m_uuid, (U64)((U32)pRequest->m_fncHandler), &stamp, pRequest->m_user);
  if (pTask != NULL) {
    LDLIST_AttachKeySorted(m_tskScheduler.m_list, pTask);  // insert a task in scheduler and sort by timestamp
  }
}

/**
 * @brief Function to process a background daemon task for scheduler
 *        1) manages timer counter overflow
 *
 * @param[in]
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msSHD_DaemonEventHandler(U64 uuid, I32 event, U64 user)
{
  U32 currentTickCounter;
  if (m_tskScheduler.m_initialized == MS_FALSE) return;
  currentTickCounter = nrf_drv_timer_capture(&taskTimer, NRF_TIMER_CC_CHANNEL3);
  if (currentTickCounter < m_tskScheduler.m_previousTickCounter) { // handle the overflow of timer's counter
    m_tskScheduler.m_tickOverflowCounter += m_tskScheduler.m_counterOverflowValue;
  }
  if (m_tskScheduler.m_fnAppBackgroudTask != NULL) {  // application defined background task
    m_tskScheduler.m_fnAppBackgroudTask(uuid, event, user);
  }
}

/**
 * @brief Function to process all requested task for scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
MS_TASK_REQUEST gm_request; // to reduce the stack push / pop amount
static void msSHD_EventHandler(nrf_timer_event_t event_type, void* p_context) 
{
  MS_DNODE *pTask = NULL;
  U64 milliTime = 0;
  U64 nextFired = 0;
  I32 qid;

  if (event_type != NRF_TIMER_EVENT_COMPARE0) { // NRF_TIMER_EVENT_COMPARE0 is only used for scheduler 
    for (qid = 0; qid < MS_APP_TSK_REQUEST_MAPQ_MAX; qid++) { // handle all pended requests
      while(msTSK_ReadRequest(qid, &gm_request) == MS_SUCCESS) {
        switch(gm_request.m_command) {
        case MS_TASK_CMD_START: 
          msTSK_RegisterNew(&gm_request); 
          break;
        case MS_TASK_CMD_STOP:
          msTSK_RemoveAllHasUUID(&gm_request);
          break;
        case MS_TASK_CMD_FLUSH: //! normally only used for clearing the scheduler
          msTSK_ReadFlush();  // flush an requested queue and all tasks
          msTSK_FlushAll(&gm_request);
          break;
        };
      }
    }
  } 

  do { // handle all tasks at a time earlier than the current time.
    pTask = LDLIST_PeekNext(m_tskScheduler.m_list);
    if (pTask == NULL) break;
    milliTime = m_tskScheduler.m_fnGetCurrentTime();
    if (pTask->m_key.key1 <= milliTime + MS_APP_TSK_GRANUALITY) {
      if (pTask->m_code) {
        ((TASK_HANDLER)((U32)pTask->m_code))(pTask->m_uuid, pTask->m_key.code, pTask->m_user); // run the task handler that user defined
      }
      pTask = LDLIST_Detach(m_tskScheduler.m_list);
      switch(pTask->m_key.key0) {
      case MS_TASK_PERIODIC:
        pTask->m_key.key1 += pTask->m_key.data; // updates the next scheduled time added by the period
        LDLIST_AttachKeySorted(m_tskScheduler.m_list, pTask);  // put it back in the scheduler
        break;
      case MS_TASK_ONESHORT: // one shot
        LDNODE_Free(pTask);
      default: 
        break;
      }
    } else {
      break;
    }
  } while (true);

  pTask = LDLIST_PeekNext(m_tskScheduler.m_list);     // peek the first in schdeuler
  if (pTask != NULL) {
    nextFired = pTask->m_key.key1;
    milliTime = m_tskScheduler.m_fnGetCurrentTime();
    if (nextFired < milliTime + MS_APP_TSK_GRANUALITY) {
      nextFired = milliTime + MS_APP_TSK_GRANUALITY;
    }
    msTimer_OneshotAddCanRepeat(&taskTimer, NRF_TIMER_CC_CHANNEL0, nextFired, true, true);  // trigger the eariest task in the scheduler
  } else {
  }
}

/**
 * @brief Function to create the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  static void msSHD_ResetRequestQ()
{
  I32 k;

  for (k = 0; k < MS_APP_TSK_REQUEST_MAPQ_MAX; k++) {
    m_tskScheduler.m_reqmap[k].m_ix   = 0;
    m_tskScheduler.m_reqmap[k].m_ox   = 0;
    m_tskScheduler.m_reqmap[k].m_size = MS_APP_TSK_REQUEST_MAX_NUM_PER_QID;
  }
}

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
  I32 msSHD_Start(U64 (*fnGetTime)(), TASK_HANDLER fnAppBackgroud, U32 expired, U64 user, U64 event)
{
  UUKEY stamp = { .key0 = 0, .key1 = msFOURCC('S', 'C', 'H', 'D') };
  nrf_drv_timer_config_t config;

  msOS_CRTSEC_P((HAND)&m_task_ciritical_section);
  msSHD_ResetRequestQ();
  m_tskScheduler.m_list = LDLIST_Alloc(&stamp, NULL);
  m_tskScheduler.m_fnGetCurrentTime = (fnGetTime != NULL) ? fnGetTime : msTSK_NowInMillisecond;
  msTimer_DefaultConfigure(&config);
  config.p_context = &m_tskScheduler;
  msTimer_Initialize(&taskTimer, &config, true, msSHD_EventHandler);
  msTSK_ResetTimebase(MS_TRUE, &config);
  msTimer_Enable(&taskTimer);
  m_tskScheduler.m_fnAppBackgroudTask = fnAppBackgroud;
  m_tskScheduler.m_initialized = MS_TRUE;
  //! should be started after m_tskScheduler.m_initialized is MS_TRUE
  if (fnAppBackgroud != NULL) {
    //expired = msMINC(expired, MS_SCH_DAEMON_MAX_PERIOD);
    if (expired > MS_SCH_DAEMON_MAX_PERIOD) expired = MS_SCH_DAEMON_MAX_PERIOD;
  } else { 
    expired = MS_SCH_DAEMON_MAX_PERIOD;
  }
  msTSK_Start(TSK_REQUEST_SCHEDULER_ID, 
      MS_TASK_PERIODIC, 
      3, 
      expired, 
      msMAKEUUID('D', 'A', 'E', 'M', 'N'), 
      user, 
      event,
      msSHD_DaemonEventHandler);
  m_task_ciritical_section = 0;
  msOS_CRTSEC_V((HAND)&m_task_ciritical_section);
  return MS_SUCCESS;
}

/**
 * @brief Function to stop the task scheduler
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msSHD_Stop()
{
  msOS_CRTSEC_P((HAND)&m_task_ciritical_section);
  msTimer_Clear(&taskTimer, NRF_TIMER_CC_CHANNEL0);
  msTSK_ResetTimebase(MS_FALSE, NULL);
  msTimer_Uninitialize(&taskTimer);
  msSHD_ResetRequestQ();
  LDLIST_Free(m_tskScheduler.m_list, DONT_FREE);
  m_tskScheduler.m_list = NULL;
  m_tskScheduler.m_fnGetCurrentTime = NULL;
  m_tskScheduler.m_fnAppBackgroudTask = NULL;
  m_task_ciritical_section = 0;
  msOS_CRTSEC_V((HAND)&m_task_ciritical_section);
  return MS_SUCCESS;
}

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
  I32 msSHD_Clear()
{
  msOS_CRTSEC_P((HAND)&m_task_ciritical_section);
  msSHD_ResetRequestQ();
  LDLIST_Clear(m_tskScheduler.m_list, DONT_FREE);
  msTimer_Clear(&taskTimer, NRF_TIMER_CC_CHANNEL0);
  m_tskScheduler.m_fnGetCurrentTime = NULL;
  m_tskScheduler.m_fnAppBackgroudTask = NULL;
  m_task_ciritical_section = 0;
  msOS_CRTSEC_V((HAND)&m_task_ciritical_section);
  return MS_SUCCESS;
}

/**
 * @brief Function to read a value sized a byte
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadRequest(I32 qid, MS_TASK_REQUEST *pRequest) 
{
  U32 ox = m_tskScheduler.m_reqmap[qid].m_ox;

  if (ox == m_tskScheduler.m_reqmap[qid].m_ix) return MS_UNDERFLOW;
  *pRequest = m_tskScheduler.m_reqinp[qid][ox];
  m_tskScheduler.m_reqmap[qid].m_ox = (ox + 1) % m_tskScheduler.m_reqmap[qid].m_size;
  return MS_SUCCESS;
}


/**
 * @brief Function to flush a reading pipe specified QID
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadFlushQid(I32 qid)
{
  m_tskScheduler.m_reqmap[qid].m_ox = m_tskScheduler.m_reqmap[qid].m_ix;
  return MS_SUCCESS;
}
 
/**
 * @brief Function to flush a reading pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_ReadFlush()
{
  I32 k;

  for (k = 0; k < MS_APP_TSK_REQUEST_MAPQ_MAX; k++) {
    m_tskScheduler.m_reqmap[k].m_ox = m_tskScheduler.m_reqmap[k].m_ix;
  }
  return MS_SUCCESS;
}

/**
 * @brief Function to write a request
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteRequest(I32 qid, MS_TASK_REQUEST *pRequest) 
{
  U32 nx, ix = m_tskScheduler.m_reqmap[qid].m_ix;

  nx = (ix + 1) % m_tskScheduler.m_reqmap[qid].m_size;
  if (nx == m_tskScheduler.m_reqmap[qid].m_ox) return MS_OVERFLOW;
  m_tskScheduler.m_reqinp[qid][ix] = *pRequest;
  m_tskScheduler.m_reqmap[qid].m_ix = nx;
  return MS_SUCCESS;
}

/**
 * @brief Function to flush a writing pipe specified QID
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteFlushQid(I32 qid)
{
  m_tskScheduler.m_reqmap[qid].m_ix = m_tskScheduler.m_reqmap[qid].m_ox;
  return MS_SUCCESS;
}

/**
 * @brief Function to flush a writing pipe
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval */
TSKAPI 
  I32 msTSK_WriteFlush()
{
  I32 k;

  for (k = 0; k < MS_APP_TSK_REQUEST_MAPQ_MAX; k++) {
    m_tskScheduler.m_reqmap[k].m_ix = m_tskScheduler.m_reqmap[k].m_ox;
  }
  return MS_SUCCESS;
}

/**
 * @brief Function to trigger the task start command 
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
  I32 msTSK_Start(I32 qid, TASK_TYPE type, I16 priority, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler)
{
  MS_TASK_REQUEST request = {
                .m_type     = type,
                .m_priority = priority,
                .m_command = (U16)MS_TASK_CMD_START,
                .m_uuid     = uuid,
                .m_fncHandler = handler,
                .m_expired  = expired,
                .m_nextFired = 0,
                .m_user     = user,
                .m_event    = event
              };
  nrf_timer_cc_channel_t cc_channel = (qid == 0) ? NRF_TIMER_CC_CHANNEL1 : NRF_TIMER_CC_CHANNEL2;

  request.m_nextFired = m_tskScheduler.m_fnGetCurrentTime() + expired;
  if (qid > MS_APP_TSK_REQUEST_MAPQ_MAX | msTSK_WriteRequest(qid, &request) != MS_SUCCESS) 
    return MS_FAILURE;
  msTimer_EventClear(&taskTimer, cc_channel);
  msTimer_OneshotAddCanRepeat(&taskTimer, cc_channel, m_tskScheduler.m_fnGetCurrentTime() + MS_TSK_REQUEST_DELAY_MS, true, true);
  return MS_SUCCESS;
}

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
  I32 msTSK_StartDelayed(I32 qid, TASK_TYPE type, I16 priority, U64 startOnTime, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler)
{
  MS_TASK_REQUEST request = {
                .m_type     = type,
                .m_priority = priority,
                .m_command = (U16)MS_TASK_CMD_START,
                .m_uuid     = uuid,
                .m_fncHandler = handler,
                .m_expired  = expired,
                .m_nextFired = 0,
                .m_user     = user,
                .m_event    = event
              };
  nrf_timer_cc_channel_t cc_channel = (qid == 0) ? NRF_TIMER_CC_CHANNEL1 : NRF_TIMER_CC_CHANNEL2;

  request.m_nextFired = startOnTime;
  if (qid > MS_APP_TSK_REQUEST_MAPQ_MAX | msTSK_WriteRequest(qid, &request) != MS_SUCCESS) 
    return MS_FAILURE;
  msTimer_EventClear(&taskTimer, cc_channel);
  msTimer_OneshotAddCanRepeat(&taskTimer, cc_channel, m_tskScheduler.m_fnGetCurrentTime() + MS_TSK_REQUEST_DELAY_MS, true, true);
  return MS_SUCCESS;
}

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
  I32 msTSK_Stop(I32 qid, U64 uuid, U64 user, I32 event, TASK_HANDLER handler)
{
  MS_TASK_REQUEST request = {
                .m_type     = MS_TASK_ONESHORT,
                .m_priority = MS_TSK_REQUEST_PRIORITY_NOR,
                .m_command  = (U16)MS_TASK_CMD_STOP,
                .m_uuid     = uuid,
                .m_fncHandler = handler,
                .m_expired  = 0,
                .m_nextFired = 0,
                .m_user     = user,
                .m_event    = event};
  nrf_timer_cc_channel_t cc_channel = (qid == 0) ? NRF_TIMER_CC_CHANNEL1 : NRF_TIMER_CC_CHANNEL2;

  if (qid > MS_APP_TSK_REQUEST_MAPQ_MAX | msTSK_WriteRequest(qid, &request) != MS_SUCCESS) 
    return MS_FAILURE;
  msTimer_EventClear(&taskTimer, cc_channel);
  msTimer_OneshotAddCanRepeat(&taskTimer, cc_channel, m_tskScheduler.m_fnGetCurrentTime() + MS_TSK_REQUEST_DELAY_MS, true, true);
  return MS_SUCCESS;
}

/**
 * @brief Function to delete a task has UUID in scheduler
 *        Among TASKs with UUIDs, TASKs are deleted before m_nextFired time
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
TSKAPI
  I32 msTSK_StopUntil(I32 qid, I16 priority, U32 expired, U64 uuid, U64 user, I32 event, TASK_HANDLER handler)
{
  MS_TASK_REQUEST request = {
                .m_type     = MS_TASK_ONESHORT,
                .m_priority = MS_TSK_REQUEST_PRIORITY_NOR,
                .m_command  = (U16)MS_TASK_CMD_STOP,
                .m_uuid     = uuid,
                .m_fncHandler = handler,
                .m_expired  = 0,
                .m_nextFired = 0,
                .m_user     = user,
                .m_event    = event};
  nrf_timer_cc_channel_t cc_channel = (qid == 0) ? NRF_TIMER_CC_CHANNEL1 : NRF_TIMER_CC_CHANNEL2;

  request.m_nextFired = m_tskScheduler.m_fnGetCurrentTime() + expired;
  if (qid > MS_APP_TSK_REQUEST_MAPQ_MAX | msTSK_WriteRequest(qid, &request) != MS_SUCCESS) 
    return MS_FAILURE;
  msTimer_EventClear(&taskTimer, cc_channel);
  msTimer_OneshotAddCanRepeat(&taskTimer, cc_channel, m_tskScheduler.m_fnGetCurrentTime() + MS_TSK_REQUEST_DELAY_MS, true, true);
  return MS_SUCCESS;
}

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
  I32 msTSK_Flush(I32 qid, U64 uuid, U64 user, I32 event, TASK_HANDLER handler)
{
  MS_TASK_REQUEST request = {
                .m_type     = MS_TASK_ONESHORT,
                .m_priority = MS_TSK_REQUEST_PRIORITY_NOR,
                .m_command = (U16)MS_TASK_CMD_FLUSH,
                .m_uuid     = uuid,
                .m_fncHandler = handler,
                .m_expired  = 0,
                .m_nextFired = 0,
                .m_user     = user,
                .m_event    = event};
  nrf_timer_cc_channel_t cc_channel = (qid == 0) ? NRF_TIMER_CC_CHANNEL1 : NRF_TIMER_CC_CHANNEL2;

  if (qid > MS_APP_TSK_REQUEST_MAPQ_MAX | msTSK_WriteRequest(qid, &request) != MS_SUCCESS) 
    return MS_FAILURE;
  msTimer_EventClear(&taskTimer, cc_channel);
  msTimer_OneshotAddCanRepeat(&taskTimer, cc_channel, m_tskScheduler.m_fnGetCurrentTime() + MS_TSK_REQUEST_DELAY_MS, true, true);
  return MS_SUCCESS;
}


#if defined (__cplusplus)
}
#endif