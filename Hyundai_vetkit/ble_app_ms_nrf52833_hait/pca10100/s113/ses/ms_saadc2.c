/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_saadc2.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/26 - 2nd version released			   
 *				  															   
 *******************************************************************************/
#include "ms_app_utils.h"
#include "ms_saadc2.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#if defined (__cplusplus)
extern "C" {
#endif

typedef struct MS_SAADC2 {
  I16                         m_state;
  MS_SAADCCTL_MODE            m_mode;
  U32                         m_samplingRate; // herz
  nrfx_saadc_config_t         m_config;
  SAADC_HANDLER               m_fnEventHandler;
  U32                         m_nchannel;
  MS_SAADC_CHANNEL2           m_channel[MS_SAADC_CHANNEL_MAX_ID];
  U32                         m_npooling;
  I16*                        m_pool[2];  // double buffering
  I16*                        m_pScratch; // scratch memory of poolingg
  struct {
    U16  m_timer          : 8;
    U16  m_ppi            : 8;
  } m_tmstate;
} MS_SAADC2;

static MS_SAADC2 m_saadcInstance = {
      .m_state = CUI_STS_NULL,
      .m_mode = { 
        .reserved   = 0,
        .power_mode = SAADC_LOW_POWER,
        .periodic   = 1,
        .use_ppi    = 1
      },
      .m_samplingRate = 1,
      .m_config = {
        .resolution = (nrf_saadc_resolution_t)NRFX_SAADC_CONFIG_RESOLUTION,
        .oversample = (nrf_saadc_oversample_t)NRFX_SAADC_CONFIG_OVERSAMPLE,
        .interrupt_priority = NRFX_SAADC_CONFIG_IRQ_PRIORITY,
        .low_power_mode = NRFX_SAADC_CONFIG_LP_MODE
      },
      .m_fnEventHandler = NULL,
      .m_nchannel = 0,
      .m_npooling = 1,
      .m_pScratch = NULL,
      .m_tmstate = {
        .m_timer  = CUI_STS_NULL,
        .m_ppi    = CUI_STS_NULL
      }
    }; 

static nrf_drv_timer_config_t   saadcTmConfig = NRF_DRV_TIMER_DEFAULT_CONFIG;
static const nrf_drv_timer_t    saadcTimer = NRF_DRV_TIMER_INSTANCE(MS_APP_SAADC_TIMER_ID);
static nrf_ppi_channel_t        saadcPpiChannel;

/**
 * @brief Function to process ultra low power 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msSAAD_TIMER_EventHandler(nrf_timer_event_t eventType, void* pContext) 
{
  if (eventType == NRF_TIMER_EVENT_COMPARE0) {
    switch(m_saadcInstance.m_mode.power_mode) {
    case SAADC_LOW_POWER:
      nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
      //nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
      break;
    case SAADC_LOWEST_POWER:
      msSAADC2_ReinitializeForLowestPower();
      msSAADC2_Start();
      break;
    }
  }
}

/**
 * @brief Function to process all requested task for SAADC
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msSAADC2_EventHandler(nrf_drv_saadc_evt_t* pEvent) 
{
  U32 id, ix, np;

  if (pEvent->type == NRF_DRV_SAADC_EVT_DONE) {
    U64 capturedTimeInMicrosecond = msTSK_NowInMicrosecond();
    U32 status = nrf_drv_saadc_buffer_convert(pEvent->data.done.p_buffer, m_saadcInstance.m_nchannel * m_saadcInstance.m_npooling); // set buffers back for the next sampling
    APP_ERROR_CHECK(status);
    if (m_saadcInstance.m_fnEventHandler != NULL) { // SAADC_HANDLER has priority over SAADC_DATA_HANDLER which may be designated for each channel
      m_saadcInstance.m_fnEventHandler(pEvent);
    } else {
      if (pEvent->data.done.size != (m_saadcInstance.m_nchannel * m_saadcInstance.m_npooling)) { 
        return;
      }
      if (m_saadcInstance.m_npooling > 1) {
        I16* pArray;
        U32 nPooling = pEvent->data.done.size / m_saadcInstance.m_nchannel;
        for (id = 0; id < m_saadcInstance.m_nchannel; id++) {
          if (m_saadcInstance.m_channel[id].m_id != MS_INVALID_ID && 
              m_saadcInstance.m_channel[id].m_fnDataHandler != NULL) {
            pArray = pEvent->data.done.p_buffer + id;
            for (ix = 0, np = 0; np < nPooling; np++, pArray += m_saadcInstance.m_nchannel) {
              m_saadcInstance.m_pScratch[ix++] = *pArray;
            }
            m_saadcInstance.m_channel[id].m_fnDataHandler(id, 
                                  m_saadcInstance.m_pScratch, 
                                  nPooling, 
                                  capturedTimeInMicrosecond, 
                                  m_saadcInstance.m_channel[id].m_user);
          }
        }
      } else {
        for (ix = 0; ix < pEvent->data.done.size; ix++) {
          id = ix % m_saadcInstance.m_nchannel;
          if (m_saadcInstance.m_channel[id].m_id != MS_INVALID_ID && 
              m_saadcInstance.m_channel[id].m_fnDataHandler != NULL) {
            m_saadcInstance.m_channel[id].m_fnDataHandler(id, 
                                  pEvent->data.done.p_buffer + ix,
                                  1,        
                                  capturedTimeInMicrosecond, 
                                  m_saadcInstance.m_channel[id].m_user);
          }
        }
      }
    }
    switch(m_saadcInstance.m_mode.power_mode) {
    case SAADC_LOW_POWER:
      nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
      break;
    case SAADC_LOWEST_POWER:
      msSAADC2_Uninitialize();
      break;
    default: // SAADC_HIGH_POWER - continuous sampling, SAADC_USER_DRIVEN_POWER - doesn't use this function
      break;
    }
  }
}

/**
 * @brief Function to initialize PPI between TIMER and SAADC 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static I32 msSAADC2_PPIInitialize()
{
  U32 comparEventAddr = nrf_drv_timer_compare_event_address_get(&saadcTimer, NRF_TIMER_CC_CHANNEL0);
  U32 ppiTaskAddr = 0;
  switch(m_saadcInstance.m_mode.power_mode) {
  case SAADC_LOW_POWER: 
    ppiTaskAddr = nrf_saadc_task_address_get(NRF_SAADC_TASK_START);
    break;
  case SAADC_HIGH_POWER: 
    ppiTaskAddr = nrf_saadc_task_address_get(NRF_SAADC_TASK_SAMPLE); 
    break;
  default: 
    return MS_FAILURE;
  }
  U32 status = nrf_drv_ppi_channel_alloc(&saadcPpiChannel); 
  APP_ERROR_CHECK(status);
  status = nrf_drv_ppi_channel_assign(saadcPpiChannel, comparEventAddr, ppiTaskAddr);
  APP_ERROR_CHECK(status);
  m_saadcInstance.m_tmstate.m_ppi = CUI_STS_INIT;
  return MS_SUCCESS;
}

/**
 * @brief Function to initialize TIMER to triger the TASK_START, STOP or SAMPLE events by PPI
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static I32 msSAADC2_TimerInitialize(U32 samplingRate)
{
  bool enableInterrupt = false;
  switch(m_saadcInstance.m_tmstate.m_timer) {
  case CUI_STS_NULL:
    if (m_saadcInstance.m_mode.periodic < 0) return MS_FAILURE;
    U32 status = nrf_drv_timer_init(&saadcTimer, &saadcTmConfig, msSAAD_TIMER_EventHandler);
    APP_ERROR_CHECK(status);
    enableInterrupt = (m_saadcInstance.m_mode.use_ppi > 0) ? false : true;
    nrf_drv_timer_extended_compare(&saadcTimer,
            NRF_TIMER_CC_CHANNEL0,
            nrf_drv_timer_us_to_ticks(&saadcTimer, 1000000 / samplingRate), 
            NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
            enableInterrupt);
    if (m_saadcInstance.m_mode.use_ppi > 0) {
      msSAADC2_PPIInitialize();
    }
    m_saadcInstance.m_tmstate.m_timer = CUI_STS_INIT;
  case CUI_STS_INIT:
    return MS_SUCCESS;
  }
  return MS_FAILURE;
}

/**
 * @brief Function to uninitialize TIMER
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static I32 msSAADC2_TimerUninitialize()
{
  switch(m_saadcInstance.m_tmstate.m_timer) {
  case CUI_STS_INIT:
  case CUI_STS_STOP:
    nrf_drv_timer_uninit(&saadcTimer);
    if (m_saadcInstance.m_mode.use_ppi > 0 &&
        m_saadcInstance.m_tmstate.m_ppi == CUI_STS_STOP) {
      nrfx_ppi_channel_free(saadcPpiChannel);
      m_saadcInstance.m_tmstate.m_ppi = CUI_STS_NULL;
    }
    m_saadcInstance.m_tmstate.m_timer = CUI_STS_NULL;
  case CUI_STS_NULL:
    return MS_SUCCESS;
  }
  return MS_FAILURE; // CUI_STS_STOP
}

/**
 * @brief Function to initialize SAADC driver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static I32 msSAADC2_DriverInitialize()
{
  U32 status = nrf_drv_saadc_init(&m_saadcInstance.m_config, (nrf_drv_saadc_event_handler_t)msSAADC2_EventHandler);
  APP_ERROR_CHECK(status);
  for (U32 id = 0; id < m_saadcInstance.m_nchannel; id++) {
    status = nrf_drv_saadc_channel_init(id, &m_saadcInstance.m_channel[id].m_config); // seqentially increase
    APP_ERROR_CHECK(status);
  }
  status = nrf_drv_saadc_buffer_convert(m_saadcInstance.m_pool[0], m_saadcInstance.m_nchannel * m_saadcInstance.m_npooling);
  APP_ERROR_CHECK(status);
  status = nrf_drv_saadc_buffer_convert(m_saadcInstance.m_pool[1], m_saadcInstance.m_nchannel * m_saadcInstance.m_npooling);
  APP_ERROR_CHECK(status);
  return MS_SUCCESS;
}

/**
 * @brief Function to uninitialize SAADC driver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static I32 msSAADC2_DriverUninitialize()
{
  nrf_drv_saadc_uninit();
  return MS_SUCCESS;
}

/**
 * @brief Function to find the optimal timer's frequency
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */ 
static void msSAADC2_OptimalTimerFrequency(U32 samplingRate, nrf_drv_timer_config_t* pConfig)
{
  if (samplingRate < 31250) {
    pConfig->frequency = NRF_TIMER_FREQ_31250Hz;
  } else if (samplingRate < 62500) {
    pConfig->frequency = NRF_TIMER_FREQ_62500Hz;
  } else if (samplingRate < 125000) {
    pConfig->frequency = NRF_TIMER_FREQ_125kHz;
  } else if (samplingRate < 250000) {
    pConfig->frequency = NRF_TIMER_FREQ_250kHz;
  } else if (samplingRate < 500000) {
    pConfig->frequency = NRF_TIMER_FREQ_500kHz;
  } else if (samplingRate < 1000000) {
    pConfig->frequency = NRF_TIMER_FREQ_1MHz;
  } else { // in nRF52833, the maximum sampling rate is 200 ksps 
    pConfig->frequency = NRF_TIMER_FREQ_1MHz;
  }
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
I32 msSAADC2_Initialize(nrfx_saadc_config_t* pConfig,
      SAADC_POWER_MODE power_mode,
      SAADC_HANDLER fnEventHandler,
      nrf_drv_timer_config_t* pTmConfig,
      MS_SAADC_CHANNEL2* pChannel,
      I32 nChannel, 
      U32 samplingRate,
      U32 szPooling)
{
  I32 id;
  U32 status;
  
  if (m_saadcInstance.m_state != CUI_STS_NULL) return MS_FAILURE;

  switch(power_mode) {
  case SAADC_LOW_POWER:         // normal mode, the phase accuracy is high and the power consumption is low.
  case SAADC_HIGH_POWER:        // the phase accurary is very high, but the power consumption is relatively high than other.
    m_saadcInstance.m_mode.periodic = 1;
    m_saadcInstance.m_mode.use_ppi  = 1;
    pConfig->low_power_mode = true;
    break;
  case SAADC_LOWEST_POWER:      // on the time axis, the accuracy of the sampling phase is a bit low
    m_saadcInstance.m_mode.periodic = 1;
    m_saadcInstance.m_mode.use_ppi  = 0;
    pConfig->low_power_mode = false;
    break;
  case SAADC_USER_DRIVEN_POWER: // recommended for irregular low sampling rate, 
    m_saadcInstance.m_mode.periodic = 0; // the phase accuracy could be quite low and it can put a strain on CPU performance 
    m_saadcInstance.m_mode.use_ppi  = 0;
    pConfig->low_power_mode = false;
    break;
  default:
    return MS_FAILURE;
  }
  m_saadcInstance.m_fnEventHandler = fnEventHandler;
  msSAADC2_OptimalTimerFrequency(samplingRate, pTmConfig);    // try to find the optimal timer's frequency
  saadcTmConfig = *pTmConfig;
  m_saadcInstance.m_mode.power_mode = power_mode;
  m_saadcInstance.m_samplingRate = samplingRate;
  m_saadcInstance.m_nchannel = nChannel;
  m_saadcInstance.m_config = *pConfig; // copy config
  for (id = 0; id < MS_SAADC_CHANNEL_MAX_ID; id++) { // set channel[x] to invalid state
    m_saadcInstance.m_channel[id].m_id = MS_INVALID_ID;
  }
  for (id = 0; id < nChannel; id++) {
    m_saadcInstance.m_channel[id] = pChannel[id]; // copy a channel configuration
    m_saadcInstance.m_channel[id].m_id = id;
    pChannel[id].m_id = id;                       // return channel id for future access
  }
  if (szPooling < 1) szPooling = 1;
  m_saadcInstance.m_npooling = szPooling; // can control the invoked times, m_saadcInstance.m_pool[0] = (I16*)msOS_MEMAlloc(sizeof(I16) * nChannel * szPooling); // double buffering of Saadc EventHandler 
  m_saadcInstance.m_pool[0] = (I16*)msOS_MEMAlloc(sizeof(I16) * nChannel * m_saadcInstance.m_npooling);
  m_saadcInstance.m_pool[1] = (I16*)msOS_MEMAlloc(sizeof(I16) * nChannel * m_saadcInstance.m_npooling);
  if (m_saadcInstance.m_npooling > 1) {
    m_saadcInstance.m_pScratch = (I16*)msOS_MEMAlloc(sizeof(I16) * m_saadcInstance.m_npooling);
  } else {
    m_saadcInstance.m_pScratch = NULL;
  }
  if (m_saadcInstance.m_pool[0] == NULL ||
      m_saadcInstance.m_pool[1] == NULL || 
      ((m_saadcInstance.m_npooling > 1) && (m_saadcInstance.m_pScratch == NULL))) {
    msOS_MEMFree((MHDL)m_saadcInstance.m_pool[0]);
    msOS_MEMFree((MHDL)m_saadcInstance.m_pool[1]);
    msOS_MEMFree((MHDL)m_saadcInstance.m_pScratch);
    m_saadcInstance.m_pool[0] = NULL;
    m_saadcInstance.m_pool[1] = NULL;
    m_saadcInstance.m_pScratch = NULL;
    return MS_FAILURE;
  }
  msSAADC2_TimerInitialize(m_saadcInstance.m_samplingRate);
  msSAADC2_DriverInitialize();
  m_saadcInstance.m_state = CUI_STS_INIT; // change state

  return MS_SUCCESS;
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
I32 msSAADC2_ReinitializeForLowestPower()
{
  if (m_saadcInstance.m_state != CUI_STS_NULL || 
      m_saadcInstance.m_mode.power_mode != SAADC_LOWEST_POWER) {
    return MS_FAILURE;
  }
  msSAADC2_TimerInitialize(m_saadcInstance.m_samplingRate);
  msSAADC2_DriverInitialize();
  m_saadcInstance.m_state = CUI_STS_INIT; // change state
  return MS_SUCCESS;
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
I32 msSAADC2_Uninitialize()
{
  if (m_saadcInstance.m_state != CUI_STS_INIT) return MS_SUCCESS;
  msSAADC2_Stop(); // stop timer and ppi
  msSAADC2_TimerUninitialize();
  msSAADC2_DriverUninitialize();
  if (m_saadcInstance.m_pScratch != NULL) { //begin, free all allocated memory resources
    msOS_MEMFree((MHDL)m_saadcInstance.m_pScratch);
    m_saadcInstance.m_pScratch = NULL;
  }
  msOS_MEMFree((MHDL)m_saadcInstance.m_pool[0]);
  msOS_MEMFree((MHDL)m_saadcInstance.m_pool[1]);
  m_saadcInstance.m_pool[0] = NULL;
  m_saadcInstance.m_pool[1] = NULL;
  for (I32 id = 0; id < MS_SAADC_CHANNEL_MAX_ID; id++) {
    m_saadcInstance.m_channel[id].m_id = MS_INVALID_ID;
    m_saadcInstance.m_channel[id].m_fnDataHandler = NULL;
  }
  m_saadcInstance.m_state = CUI_STS_NULL;
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
I32 msSAADC2_Start()
{
  if (m_saadcInstance.m_state != CUI_STS_INIT) return MS_FAILURE;
  switch(m_saadcInstance.m_tmstate.m_timer) {
  case CUI_STS_INIT:
  case CUI_STS_STOP:
    nrf_drv_timer_enable(&saadcTimer);
    if (m_saadcInstance.m_mode.use_ppi > 0 &&
        (m_saadcInstance.m_tmstate.m_ppi == CUI_STS_INIT ||
         m_saadcInstance.m_tmstate.m_ppi == CUI_STS_STOP)) {
      nrf_drv_ppi_channel_enable(saadcPpiChannel);
      m_saadcInstance.m_tmstate.m_ppi = CUI_STS_START;
    }
    m_saadcInstance.m_tmstate.m_timer = CUI_STS_START;
  case CUI_STS_START:
    return MS_SUCCESS;
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
I32 msSAADC2_Stop()
{
  if (m_saadcInstance.m_state != CUI_STS_INIT) return MS_SUCCESS;
  switch(m_saadcInstance.m_tmstate.m_timer) {
  case CUI_STS_START:
    nrf_drv_timer_disable(&saadcTimer);
    nrf_drv_timer_clear(&saadcTimer);
    if (m_saadcInstance.m_mode.use_ppi > 0 &&
        m_saadcInstance.m_tmstate.m_ppi == CUI_STS_START) {
      nrf_drv_ppi_channel_disable(saadcPpiChannel);
      m_saadcInstance.m_tmstate.m_ppi = CUI_STS_STOP;
    }
    m_saadcInstance.m_tmstate.m_timer = CUI_STS_STOP;
  case CUI_STS_STOP:
    return MS_SUCCESS;
  }
  return MS_FAILURE;
}

/**
 * @brief Function to read a value of the specified channel
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
I32 msSAADC2_ReadSample(I16 id, I16 *pValue)
{
  if (m_saadcInstance.m_state != CUI_STS_INIT && // can use this function only if the current mode isn't the periodic sampling mode
      m_saadcInstance.m_mode.power_mode != SAADC_USER_DRIVEN_POWER && 
      m_saadcInstance.m_channel[id].m_id == MS_INVALID_ID) { 
      *pValue = 0;
      return MS_FAILURE;
  }
  U32 status = nrf_drv_saadc_sample_convert(id, pValue);
  APP_ERROR_CHECK(status);
  return MS_SUCCESS;
}


#if defined (__cplusplus)
}
#endif