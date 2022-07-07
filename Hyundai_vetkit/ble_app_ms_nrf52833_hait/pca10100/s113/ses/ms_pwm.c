/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_pwm.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_pwm.h"
#include "ms_app_utils.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


typedef struct MS_PWM_INSTANCE {
  I32                         m_state;
  U32                         m_nSequence;
  U32                         m_nLoop;
  U32                         m_flags;
  PWM_EVENT_HANDLER           m_fnEventHandler; 
  U64                         m_user;      
  nrf_drv_pwm_t               m_pwm;
  nrf_drv_pwm_config_t        m_config;
  nrf_pwm_sequence_t          m_pSequence[MS_PWM_SEQUENCE_MAX_ID];
} MS_PWM_INSTANCE;

static MS_PWM_INSTANCE* pwmInstance[MS_PWM_INSTANCE_MAX_ID] = {NULL, NULL, NULL, NULL};

#define MS_
#if defined (__cplusplus)
extern "C" {
#endif


/**
 * @brief Function to process all event for PWM
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */

static void msPWM_Common_EventHandler(U32 id, nrfx_pwm_evt_type_t eventType)
{
  U64 currentInMicrosecond = msTSK_NowInMillisecond();
 
  if (pwmInstance[id]->m_state == CUI_STS_NULL) return;
  if (pwmInstance[id]->m_fnEventHandler != NULL) {
    pwmInstance[id]->m_fnEventHandler(id, (I32)eventType, pwmInstance[id]->m_user);
  }
  switch(eventType) {
  case NRFX_PWM_EVT_FINISHED:
    break; 
  case NRFX_PWM_EVT_END_SEQ0:
    break;
  case NRFX_PWM_EVT_END_SEQ1:
    break; 
  case NRFX_PWM_EVT_STOPPED:  
    break;
  }
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of PWM0
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msPWM_0_EventHandler(nrfx_pwm_evt_type_t eventType)
{
  msPWM_Common_EventHandler(0, eventType);
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of PWM1
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msPWM_1_EventHandler(nrfx_pwm_evt_type_t eventType)
{
  msPWM_Common_EventHandler(1, eventType);
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of PWM2
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msPWM_2_EventHandler(nrfx_pwm_evt_type_t eventType)
{
  msPWM_Common_EventHandler(2, eventType);
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of PWM3
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msPWM_3_EventHandler(nrfx_pwm_evt_type_t eventType)
{
  msPWM_Common_EventHandler(3, eventType); 
}

/**
 * @brief Function to callback the termination of playback
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msPWM_Terminate(U64 uuid, I32 event, U64 user)
{
  MS_PWM_INSTANCE* pInstance = (MS_PWM_INSTANCE*)((U32)user);

  for (U32 id = 0; id < MS_PWM_INSTANCE_MAX_ID; id++) {
    if (pInstance == pwmInstance[id]) { 
      if (pwmInstance[id]->m_fnEventHandler != NULL) {
        pwmInstance[id]->m_fnEventHandler(id, event, pwmInstance[id]->m_user);
      }
      msPWM_Uninitialize(id);
      return;
    }
  }
}

const nrfx_pwm_handler_t  pwmfnHandler[MS_PWM_INSTANCE_MAX_ID] = { msPWM_0_EventHandler, msPWM_1_EventHandler, msPWM_2_EventHandler, msPWM_3_EventHandler };

/**
 * @brief Function to initialize the sequence
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static I32 msPWM_InitializeSequence(U32 id, nrf_pwm_dec_load_t type, nrf_pwm_sequence_t* pSequence, U32 nSequence)
{
  I32 i, m = 0, n = 0;
  nrf_pwm_values_common_t* pCommon;
  I32 status = MS_TRUE;
    
  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] == NULL) return MS_FAILURE;
  // be careful ! the member variables of m_pSequence[n].values are loaded using EasyDMA, 
  // so the memory must be in RAM area. it must not be in the stack area
  for (n = 0; n < pwmInstance[id]->m_nSequence; n++) {              // clear the previous allocated dynamic memory
    if (pwmInstance[id]->m_pSequence[n].values.p_common != NULL) {  // after doing Initialize(), when UpdateSequence() is called to change the sequence
      msOS_MEMFree((MHDL)pwmInstance[id]->m_pSequence[n].values.p_common);
      pwmInstance[id]->m_pSequence[n].values.p_common = NULL;
     }
  }
  pwmInstance[id]->m_nSequence = msMAXV(2, nSequence);
  for (i = 0; i < pwmInstance[id]->m_nSequence; i++) {
    pCommon = (nrf_pwm_values_common_t*)msOS_MEMAlloc(sizeof(U16) * pSequence[i].length); // allocate new memory and copy values of sequence
    if (pCommon != NULL) {
      msOS_MEMCopy((VOIDPTR)pCommon, (VOIDPTR)pSequence[i].values.p_common, sizeof(U16) * pSequence[i].length); //can this, because pSequence[i].values is union struct.
      pwmInstance[id]->m_pSequence[i].values.p_common = pCommon;
      pwmInstance[id]->m_pSequence[i].length = pSequence[i].length;
      pwmInstance[id]->m_pSequence[i].repeats = pSequence[i].repeats;
      pwmInstance[id]->m_pSequence[i].end_delay = pSequence[i].end_delay;
    } else {
      status = MS_FALSE;
      break;
    }
  }
  if (status == MS_TRUE) return MS_SUCCESS;
  if (status != MS_TRUE) {
    i--;
    for (; i >= 0; i--) {
      if (pwmInstance[id]->m_pSequence[i].values.p_common != NULL) {
        msOS_MEMFree((MHDL)pwmInstance[id]->m_pSequence[i].values.p_common);
        pwmInstance[id]->m_pSequence[i].values.p_common = NULL;
      }
    }
  }
  return MS_FAILURE;
}

/**
 * @brief Function to check whether it is initialized or not
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_IsInitialized(U32 id)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_INVALID_ID;
  return (pwmInstance[id] == NULL) ? MS_FALSE : MS_TRUE;
}

/**
 * @brief Function to initialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
I32 msPWM_Initialize(U32 id, 
          nrf_drv_pwm_t* pPWM, 
          nrf_drv_pwm_config_t* pConfig,
          nrf_pwm_sequence_t* pSequence, 
          U32 nSequence,
          U32 nLoop,
          U32 flags,
          PWM_EVENT_HANDLER fnUserHandler,
          U64 user)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] == NULL) {
    pwmInstance[id] = (MS_PWM_INSTANCE*)msOS_MEMAlloc(sizeof(MS_PWM_INSTANCE));
    if (pwmInstance[id] != NULL) {
      pwmInstance[id]->m_state = CUI_STS_NULL;
      pwmInstance[id]->m_pwm = *pPWM;
      pwmInstance[id]->m_config = *pConfig;
      for (U32 n = 0; n < MS_PWM_SEQUENCE_MAX_ID; n++) {
        pwmInstance[id]->m_pSequence[n].values.p_common = NULL; // clear a memory pointer of sequences
      }
      pwmInstance[id]->m_nSequence = 0;
      msPWM_InitializeSequence(id, pConfig->load_mode, pSequence, nSequence);
      pwmInstance[id]->m_nLoop = msMAXV(1, nLoop);
      pwmInstance[id]->m_flags = flags; // such as NRF_DRV_PWM_FLAG_LOOP | NRFX_PWM_FLAG_STOP
      pwmInstance[id]->m_fnEventHandler = fnUserHandler;
      pwmInstance[id]->m_user = user;
      U32 status = nrf_drv_pwm_init(&pwmInstance[id]->m_pwm, &pwmInstance[id]->m_config, pwmfnHandler[id]);
      APP_ERROR_CHECK(status);
      pwmInstance[id]->m_state = CUI_STS_INIT;
      return MS_SUCCESS;
    }
  }
  return MS_FAILURE;
}

/**
 * @brief Function to uninitialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
I32 msPWM_Uninitialize(U32 id)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] != NULL) {
    msPWM_Stop(id, true);
    nrf_drv_pwm_uninit(&pwmInstance[id]->m_pwm);
    for (U32 i = 0; i < MS_PWM_SEQUENCE_MAX_ID; i++) {
      if (pwmInstance[id]->m_pSequence[i].values.p_common != NULL) {
        msOS_MEMFree((MHDL)pwmInstance[id]->m_pSequence[i].values.p_common); // free the allocate memory of sequence value
        pwmInstance[id]->m_pSequence[i].values.p_common = NULL;
        pwmInstance[id]->m_pSequence[i].length = 0;
      }
    }
    pwmInstance[id]->m_state = CUI_STS_NULL;
    msOS_MEMFree((MHDL)pwmInstance[id]); // free PWM[id] instance
    pwmInstance[id] = NULL;
  }
  return MS_SUCCESS;
}

/**
 * @brief Function to start
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_Start(U32 id, U64 stopAfter)
{
  U32 status;

  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] != NULL) {
    switch(pwmInstance[id]->m_state) {
    case CUI_STS_INIT:
    case CUI_STS_STOP:
      if (pwmInstance[id]->m_nSequence == 1) {
        status = nrf_drv_pwm_simple_playback(&pwmInstance[id]->m_pwm, 
                        &pwmInstance[id]->m_pSequence[0],
                        pwmInstance[id]->m_nLoop, 
                        pwmInstance[id]->m_flags);
      } else {
        status = nrf_drv_pwm_complex_playback(&pwmInstance[id]->m_pwm,
                        &pwmInstance[id]->m_pSequence[0], 
                        &pwmInstance[id]->m_pSequence[1], 
                        pwmInstance[id]->m_nLoop, 
                        pwmInstance[id]->m_flags);
      }
      APP_ERROR_CHECK(status);
      pwmInstance[id]->m_state = CUI_STS_START;
      if (stopAfter > 0) {
        // after the last value in the sequence has been loaded and started executing,
        // a SEQEND[n] event is generated. The PWM generation will then continue the last loaded value.
        U64 expiredAfterInMilli = msMAXC(stopAfter, MS_TSK_REQUEST_DELAY_MS);
        msTSK_Start(TSK_REQUEST_EXECUTIVE_ID, 
                MS_TASK_ONESHORT, 
                2, // more higher priority than other, because it can help to save the power consumtion according to subsequent actions (e.g, uninitialize)
                expiredAfterInMilli,
                msMAKEUUID('P', 'W', 'M', '0', 1), 
                (U64)((U32)pwmInstance[id]), // user
                MS_PWM_EVENT_STOP,
                msPWM_Terminate);
      }
      return MS_SUCCESS;
    }
  }
  return MS_FAILURE;
}

/**
 * @brief Function to stop
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_Stop(U32 id, bool waitUntilStopped)
{ 
  if (id < MS_PWM_INSTANCE_MAX_ID) {
    if (pwmInstance[id] != NULL) {
      switch(pwmInstance[id]->m_state) {
      case CUI_STS_START:
        nrf_drv_pwm_stop(&pwmInstance[id]->m_pwm, waitUntilStopped);
        pwmInstance[id]->m_state = CUI_STS_STOP;
      case CUI_STS_STOP:
        return MS_SUCCESS;
      }
    }
  }
  return MS_FAILURE;
}

/**
 * @brief Function to update the sequence
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_UpdateSequence(U32 id, U32 sid, nrf_pwm_sequence_t* pSequence)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID || sid > MS_PWM_SEQUENCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] != NULL) { // in case CUI_STS_INIT, CUI_STS_START, CUI_STS_STOP
    if (pwmInstance[id]->m_pSequence[sid].length == pSequence->length) {
      msOS_MEMCopy((VOIDPTR)pwmInstance[id]->m_pSequence[sid].values.p_common, 
                (VOIDPTR)pSequence->values.p_common, 
                sizeof(U16) * pSequence->length); // total length = sequence.length * sizeof(U16)
      pwmInstance[id]->m_pSequence[sid].repeats = pSequence->repeats;
      pwmInstance[id]->m_pSequence[sid].end_delay = pSequence->end_delay;
      nrfx_pwm_sequence_update(&pwmInstance[id]->m_pwm, sid, pSequence);
    } else {
      if (pwmInstance[id]->m_pSequence[sid].values.p_common != NULL) {
        msOS_MEMFree((MHDL)pwmInstance[id]->m_pSequence[sid].values.p_common);
        pwmInstance[id]->m_pSequence[sid].values.p_common = NULL;
        pwmInstance[id]->m_pSequence[sid].length = 0;
      }
      nrf_pwm_values_common_t* pCommon = (nrf_pwm_values_common_t*)msOS_MEMAlloc(sizeof(U16) * pSequence->length);
      if (pCommon != NULL) {
        msOS_MEMCopy((VOIDPTR)pCommon, (VOIDPTR)pSequence->values.p_common, sizeof(U16) * pSequence->length);
        pwmInstance[id]->m_pSequence[sid].values.p_common = pCommon;
        pwmInstance[id]->m_pSequence[sid].length = pSequence->length;
        pwmInstance[id]->m_pSequence[sid].repeats = pSequence->repeats;
        pwmInstance[id]->m_pSequence[sid].end_delay = pSequence->end_delay;
        nrfx_pwm_sequence_update(&pwmInstance[id]->m_pwm, sid, pSequence);
      } else {
        return MS_FAILURE;
      }
    }
  }
  return MS_SUCCESS;
}

/**
 * @brief Function to update the sequence value
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msPWM_UpdateSequenceValue(U32 id, U32 sid, nrf_pwm_values_t values)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID || sid > MS_PWM_SEQUENCE_MAX_ID) return MS_FAILURE;
  if (pwmInstance[id] != NULL) { // in case CUI_STS_INIT, CUI_STS_START, CUI_STS_STOP
    msOS_MEMCopy((VOIDPTR)pwmInstance[id]->m_pSequence[sid].values.p_common, 
            (VOIDPTR)values.p_common, 
            sizeof(U16) * pwmInstance[id]->m_pSequence[sid].length);
    nrfx_pwm_sequence_values_update(&pwmInstance[id]->m_pwm, sid, pwmInstance[id]->m_pSequence[sid].values);
    return MS_SUCCESS;
  }
  return MS_FAILURE;
}


#if defined (__cplusplus)
}
#endif
