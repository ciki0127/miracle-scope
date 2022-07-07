/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_twi.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/06 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_twi.h"
#include "ms_app_utils.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#if defined (__cplusplus)
extern "C" {
#endif

typedef struct MS_TWI_INSTANCE {
  I16                         m_state;
  struct {
   volatile I16               m_enable   : 1;
   volatile I16               m_xferDone : 7;
  } opcode;
  TWI_OPERATION_MODE          m_mode;
  TASK_HANDLER                m_fnEventHandler;
  nrf_drv_twi_t               m_twi;
  nrf_drv_twi_config_t        m_config;
} MS_TWI_INSTANCE;



static MS_TWI_INSTANCE* twiInstance[MS_TWI_INSTANCE_MAX_ID] = {NULL, NULL};

/**
 * @brief Function to check wheter the specified twi is initialized.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_IsInitialized(U32 id)
{
  return (twiInstance[id] != NULL) ? MS_TRUE : MS_FALSE;
}

/**
 * @brief Function to check wheter the specified twi is enabled.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_IsEnable(U32 id)
{
  if (twiInstance[0] != NULL) {
    if (twiInstance[0]->opcode.m_enable) return MS_TRUE;
  }
  return MS_FALSE;
}

static void msTWI_CommonHandler(U32 id, nrf_drv_twi_evt_t const* pEvent, void* pUser)
{
  if (twiInstance[id]->m_mode == MS_TWI_SYNC_MODE) {
    switch(pEvent->type) {
    case NRF_DRV_TWI_EVT_DONE:             
      twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_DONE;
      return;
    case NRF_DRV_TWI_EVT_ADDRESS_NACK:       
      twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_ADDRESS_NACK;
      return;
    case NRF_DRV_TWI_EVT_DATA_NACK:         
      twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_DATA_NACK;
      return;
    default: break;
    }
  } else { 
    if (twiInstance[id]->m_fnEventHandler != NULL) {
      twiInstance[id]->m_fnEventHandler(id, (I32)pEvent, (U64)((U32)pUser));
    }
  }
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of TWI
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msTWI_0_EventHandler(nrf_drv_twi_evt_t const* pEvent, void* pUser)
{
  if (twiInstance[0] != NULL) {
    msTWI_CommonHandler(0, pEvent, pUser);
  }
}

/**
 * @brief Function is the glue logic for going to Common_EventHandler as the event handler of TWI
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
static void msTWI_1_EventHandler(nrf_drv_twi_evt_t const* pEvent, void* pUser)
{
  if (twiInstance[1] != NULL) {
    msTWI_CommonHandler(1, pEvent, pUser);
  }
}

const nrf_drv_twi_evt_handler_t  twifnHandler[MS_TWI_INSTANCE_MAX_ID] = { msTWI_0_EventHandler, msTWI_1_EventHandler };

/**
 * @brief Function for initiaizing the TWI
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Initialize(U32 id, nrf_drv_twi_t* pInstance, nrf_drv_twi_config_t* pConfig, TWI_OPERATION_MODE mode, TASK_HANDLER fnUserHandler, U64 user)
{
  if (id >= MS_TWI_INSTANCE_MAX_ID) return MS_FAILURE;
  if (twiInstance[id] != NULL) return MS_FAILURE;
  
  twiInstance[id] = (MS_TWI_INSTANCE*)msOS_MEMAlloc(sizeof(MS_TWI_INSTANCE));
  if (twiInstance[id] != NULL) {
    twiInstance[id]->m_twi = *pInstance;
    twiInstance[id]->m_config = *pConfig;
    twiInstance[id]->m_mode = mode;
    twiInstance[id]->m_state = CUI_STS_NULL;
    U32 status = nrf_drv_twi_init(&twiInstance[id]->m_twi, &twiInstance[id]->m_config, twifnHandler[id], (VOIDPTR)((U32)user));
    APP_ERROR_CHECK(status);
    if (status == NRF_SUCCESS) {
      twiInstance[id]->m_state = CUI_STS_INIT;
      twiInstance[id]->opcode.m_enable = 0;
      twiInstance[id]->opcode.m_xferDone = 0;
      twiInstance[id]->m_fnEventHandler = fnUserHandler;
      msTWI_Enable(id);
    } else {
      msOS_MEMFree((MHDL)twiInstance[id]);
      twiInstance[id] = NULL;
      return MS_FAILURE;
    }
  }
  return MS_SUCCESS;
}

/**
 * @brief Function for uninitiaizing the TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Uninitialize(U32 id)
{
  if (id >= MS_PWM_INSTANCE_MAX_ID) return MS_FAILURE;
  if (twiInstance[id] != NULL) {
    msTWI_Disable(id);
    nrf_drv_twi_uninit(&twiInstance[id]->m_twi);
    twiInstance[id]->m_state = CUI_STS_NULL;
    msOS_MEMFree((MHDL)twiInstance[id]);
    twiInstance[id] = NULL;
  }
  return MS_SUCCESS;

}

/**
 * @brief Function to enable the specified TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Enable(U32 id)
{
  if (twiInstance[id] == NULL) return MS_FAILURE; 
  nrf_drv_twi_enable(&twiInstance[id]->m_twi);
  twiInstance[id]->opcode.m_enable = 1;
  return MS_SUCCESS;
}

/**
 * @brief Function to disable the specified TWI.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Disable(U32 id)
{
  if (twiInstance[id] == NULL) return MS_FAILURE; 
  nrf_drv_twi_disable(&twiInstance[id]->m_twi);
  twiInstance[id]->opcode.m_enable = 0;
  return MS_SUCCESS;
}

/**
 * @brief Function to read data from the specified TWI slave.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Read(U32 id, U08 slaveAddr, U08 regAddr, U08* pValue, I32 nSize, bool noStop)
{
  // General Procedure - Master want to receive/read data from a slave
  // 1. master-receiver sends a START condition and addresses the slave-receiver
  // 2. master-receiver sends the requested register to read to slave-transmitter
  // 3. master-receiver receives data from the slave-transmitter
  // 4. master-receiver terminates the transfer with a STOP condition
  if (twiInstance[id] == NULL) return MS_FAILURE;
  twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_UNDEFINED;
  U32 status = nrf_drv_twi_tx(&twiInstance[id]->m_twi, slaveAddr, &regAddr, 1, noStop); // step 1,2
  if (status == NRF_SUCCESS && twiInstance[id]->m_mode == MS_TWI_SYNC_MODE) { // NRF_ERROR_BUSY error can invoke
    do {
      __SEV(); // make sure any pending events are cleared
      __WFE(); 
      __WFE(); // enter System ON sleep mode
    } while(twiInstance[id]->opcode.m_xferDone == MS_TWI_EVT_UNDEFINED);
    if (twiInstance[id]->opcode.m_xferDone != MS_TWI_EVT_DONE) return MS_FAILURE; 
  }
  if (status == NRF_SUCCESS) {
    twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_UNDEFINED;
    status = nrf_drv_twi_rx(&twiInstance[id]->m_twi, slaveAddr, pValue, nSize); // step 3,4 
    if (status == NRF_SUCCESS && twiInstance[id]->m_mode == MS_TWI_SYNC_MODE) {
      do {
        __SEV(); // make sure any pending events are cleared
        __WFE();
        __WFE(); // enter System ON sleep mode
      } while(twiInstance[id]->opcode.m_xferDone == MS_TWI_EVT_UNDEFINED);
      if (twiInstance[id]->opcode.m_xferDone != MS_TWI_EVT_DONE) return MS_FAILURE;
    }
  }
  return (status == NRF_SUCCESS) ? MS_SUCCESS : MS_FAILURE;
} 

/**
 * @brief Function to write values to the specified register of slave receiver.
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_Write(U32 id, U08 slaveAddr, U08 regAddr, U08* pValue, I32 nSize, bool noStop)
{
  // General Procedure - Master want to send/write data to a slave
  // 1. master-transmitter sends a START condition and addresses the slave-receiver
  // 2. master-transmitter sends data to slave-receiver
  // 3. master-transmitter terminates the transfer with a STOP condition
  if (twiInstance[id] == NULL) return MS_FAILURE;
  U08 txPool[12];
  U32 i, txSize, status;

  do {  // support multiple bytes writings on a register
    txSize = msMINC(11, nSize);
    txPool[0] = regAddr;            // write the register address
    for (i = 1; i <= txSize; i++) {
      txPool[i] = *pValue++;        // write values
    }
    twiInstance[id]->opcode.m_xferDone = MS_TWI_EVT_UNDEFINED;
    status = nrf_drv_twi_tx(&twiInstance[id]->m_twi, slaveAddr, txPool, txSize + 1, noStop);
    if (status == NRF_SUCCESS && twiInstance[id]->m_mode == MS_TWI_SYNC_MODE) { // NRF_ERROR_BUSY error can invoke
      do {
        __SEV(); // make sure any pending events are cleared
        __WFE(); 
        __WFE(); // enter System ON sleep mode
      } while(twiInstance[id]->opcode.m_xferDone == MS_TWI_EVT_UNDEFINED);
      if (twiInstance[id]->opcode.m_xferDone != MS_TWI_EVT_DONE) return MS_FAILURE; 
    }
    if (status != NRF_SUCCESS) return MS_FAILURE;
    nSize -= txSize;
  } while(nSize > 0);
  return MS_SUCCESS;
}

/**
 * @brief Function to find the address of the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 msTWI_WhoIAm(U32 id, U08* pWhoIam)
{
  U32 status;
  U08 sample;    
  
  *pWhoIam = 0;
  for (U08 id = 1; id < 128; id++) {
    status = nrf_drv_twi_rx(&twiInstance[id]->m_twi, id, &sample, sizeof(sample));
    if (status == NRF_SUCCESS) {
      *pWhoIam = id;
      return MS_SUCCESS;
    }
  }
  return MS_FAILURE;
}


#if defined (__cplusplus)
}
#endif