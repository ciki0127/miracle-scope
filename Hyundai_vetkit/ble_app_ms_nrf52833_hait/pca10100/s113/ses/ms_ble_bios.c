/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_ble_bios.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "sdk_common.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_dis.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"

#include "ms_ble_bios.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_BLE_CONN_CFG_TAG            1                                           // A tag identifying the SoftDevice BLE configuration.
#define APP_BLE_OBSERVER_PRIO           3                                           // Application's BLE observer priority. You shouldn't need to modify this value.
#define APP_ADV_INTERVAL                64                                          // The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms).
//#define APP_ADV_DURATION                18000                                       // The advertising duration (180 seconds) in units of 10 milliseconds.
#define APP_ADV_DURATION                0                                           // The advertising unlimited duration in units of 10 milliseconds.
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             // Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units.
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             // Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units.
#define SLAVE_LATENCY                   0                                           // Slave latency.
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             // Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units.
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       // Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      // Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           // Number of attempts before giving up the connection parameter negotiation.
#define DEAD_BEEF                       0xDEADBEEF                                  // Value used as error code on stack dump, can be used to identify stack location on stack unwind.

#if defined (__cplusplus)
extern "C" {
#endif

//TODO, A separate event observer for serive is not required, and it is unified as a SoftDevice event observer
//      For fast event handling, do not call a function with the same role twice.
//BLE_MSBIOS_DEF(m_bios);                                                           // Remove, Unified to SoftDevice Event Handler
NRF_BLE_GATT_DEF(m_gatt);                                                           // GATT module instance.
NRF_BLE_QWR_DEF(m_qwr);                                                             // Context for the Queued Write module.
BLE_ADVERTISING_DEF(m_advertising);                                                 // Advertising module instance.

static U16   m_connection_handle        = BLE_CONN_HANDLE_INVALID;                  // Handle of the current connection.
static ble_uuid_t m_adv_uuids[]         =  {                                        // Universally unique service identifier.
                        {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
                        {BIOSERVICE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}
                    };

/// Define an instance of BLE's Bio Service
static BLE_BIOSERVICE     m_bios;

/// Define handler functions that interfaces Executive and SotfDevice
BLE_ADVERTISE_HANDLER     m_onAdvertiseHandler = NULL;
BLE_CONNECTED_HANDLER     m_onConnectHandler = NULL;
BLE_DISCONNECTED_HANDLER  m_onDisconnectHandler = NULL;
BLE_WRITTEN_HANDLER       m_onWrittenHandler = NULL;


U16 msBLE_GetConnectionHandle() { return m_connection_handle; }
BLE_BIOSERVICE* msBLE_GetBioServiceHandle() { return &m_bios; }


/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void assert_nrf_callback(U16 line_num, const U08* p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void gap_params_init(void)
{
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    U32 err_code = sd_ble_gap_device_name_set(&sec_mode, msApp_EXEC_GetHandle()->m_alias, strlen(msApp_EXEC_GetHandle()->m_alias));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


#define LEDBUTTON_LED                   BSP_BOARD_LED_2                        
static void led_write_handler(U16 conn_handle, BLE_BIOSERVICE * p_lbs, U08 led_state)
{
  if (led_state) {
    bsp_board_led_on(LEDBUTTON_LED);
  } else {
    bsp_board_led_off(LEDBUTTON_LED);
  }
}


/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void on_conn_params_evt(ble_conn_params_evt_t* pEvent)
{
  U32 err_code;

  if (pEvent->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
    err_code = sd_ble_gap_disconnect(m_connection_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
    APP_ERROR_CHECK(err_code);
  }
}

static void conn_params_error_handler(U32 nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = false;
  cp_init.evt_handler                    = on_conn_params_evt;
  cp_init.error_handler                  = conn_params_error_handler;

  U32 err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void sleep_mode_enter(void)
{
  U32 err_code;
  //err_code = bsp_indication_set(BSP_INDICATE_IDLE);
  //APP_ERROR_CHECK(err_code);

  // Prepare wakeup buttons.
  //err_code = bsp_btn_ble_sleep_mode_prepare();
  //APP_ERROR_CHECK(err_code);

  // Go to system-off mode (this function will not return; wakeup will cause a reset).
  err_code = sd_power_system_off();
  APP_ERROR_CHECK(err_code);
}


#define MTU_SEND_INTERVAL 1000
APP_TIMER_DEF(m_mtu_ble);
U08 array[44];
void blink_handler(void * p_context)
{
  ble_gatts_hvx_params_t params;
  U16 len = 44;
  static U08 id = 0;
    
  if (m_connection_handle == BLE_CONN_HANDLE_INVALID) return;

  for (U32 i = 0; i < 44; i++) {
    array[i] = id;
  }
  id++;

  memset(&params, 0, sizeof(params));
  params.type   = BLE_GATT_HVX_NOTIFICATION;
  params.handle = m_bios.charBioStateHandle.value_handle;
  params.p_data = (U08*)array;
  params.p_len  = &len;

  U32 err_code = sd_ble_gatts_hvx(m_connection_handle, &params);
  if ((err_code != NRF_SUCCESS) &&
      (err_code != NRF_ERROR_INVALID_STATE) &&
      (err_code != NRF_ERROR_RESOURCES) &&         
      (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
    APP_ERROR_HANDLER(err_code);
  }
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */    
void msBLE_SoftDevice_EventHandler(ble_evt_t const* pEvent, void* pUser)
{
  U32 err_code;

  switch (pEvent->header.evt_id) {
  case BLE_GATTS_EVT_HVN_TX_COMPLETE:
    return;
  case BLE_GATTS_EVT_WRITE: 
    if (m_onWrittenHandler != NULL) m_onWrittenHandler(&pEvent->evt.gatts_evt.params.write);
    return;
  case BLE_GAP_EVT_CONNECTED:
    m_connection_handle = pEvent->evt.gap_evt.conn_handle;
    err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_connection_handle);
    APP_ERROR_CHECK(err_code);

    if (m_onConnectHandler) {
      m_onConnectHandler(m_connection_handle, 0);
    }
    return;
  case BLE_GAP_EVT_DISCONNECTED:
    m_connection_handle = BLE_CONN_HANDLE_INVALID;

    if (m_onDisconnectHandler) {
      m_onDisconnectHandler(m_connection_handle, 0);
    }
    return;
  case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
      ble_gap_phys_t const phys =
      {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
      };
      err_code = sd_ble_gap_phy_update(pEvent->evt.gap_evt.conn_handle, &phys);
      APP_ERROR_CHECK(err_code);
    } 
    return;
  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    err_code = sd_ble_gap_sec_params_reply(m_connection_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    return;
  case BLE_GATTS_EVT_SYS_ATTR_MISSING:
    err_code = sd_ble_gatts_sys_attr_set(m_connection_handle, NULL, 0, 0);
    APP_ERROR_CHECK(err_code);
    return;
  case BLE_GATTC_EVT_TIMEOUT:
    err_code = sd_ble_gap_disconnect(pEvent->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    return;
  case BLE_GATTS_EVT_TIMEOUT:
    err_code = sd_ble_gap_disconnect(pEvent->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
    break;
  default:
    break;
  }
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msBLE_SoftDevice_Initialize(void)
{
  U32 err_code = nrf_sdh_enable_request();
  APP_ERROR_CHECK(err_code);

  // Configure the BLE stack using the default settings.
  // Fetch the start address of the application RAM.
  U32 ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
  APP_ERROR_CHECK(err_code);
 
  err_code = nrf_sdh_ble_enable(&ram_start);
  APP_ERROR_CHECK(err_code);

  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, msBLE_SoftDevice_EventHandler, NULL); 
}

/**
 * @brief Function to hanle GATT event, such as change MTU size
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void gatt_evt_handler(nrf_ble_gatt_t* pGatt, nrf_ble_gatt_evt_t const * pGattEvent)
{
  //TODO, later, should be modify to handle the request of change of the mtu size
    if ((m_connection_handle == pGattEvent->conn_handle) && (pGattEvent->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
      U16 mtuSize = pGattEvent->params.att_mtu_effective;
      //U32 err_code = nrf_ble_gatt_att_mtu_periph_set(pGatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
      //APP_ERROR_CHECK(err_code);
    }
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void gatt_init(void)
{
  U32 err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
  APP_ERROR_CHECK(err_code);
  err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void bsp_event_handler(bsp_event_t event)
{
  U32 err_code;
  
  switch (event) {
  case BSP_EVENT_SLEEP:
    sleep_mode_enter();
    break;
  case BSP_EVENT_DISCONNECT:
    err_code = sd_ble_gap_disconnect(m_connection_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_ERROR_INVALID_STATE) {
      APP_ERROR_CHECK(err_code);
    }
    break;
  case BSP_EVENT_WHITELIST_OFF:
    if (m_connection_handle == BLE_CONN_HANDLE_INVALID) {
      err_code = ble_advertising_restart_without_whitelist(&m_advertising);
      if (err_code != NRF_ERROR_INVALID_STATE) {
        APP_ERROR_CHECK(err_code);
      }   
    }
    break;
  default:
    break;
  }
}

static void buttons_leds_init(bool* p_erase_bonds)
{
  bsp_event_t startup_event;

  U32 err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
  APP_ERROR_CHECK(err_code);

  err_code = bsp_btn_ble_init(NULL, &startup_event);
  APP_ERROR_CHECK(err_code);

  *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}

#ifdef USE_SERVICE_OBSERVER
/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msBLE_OBSV_SRVC_OnBleEvent(ble_evt_t const * pEvent, void* pUser)
{
  NRF_LOG_INFO("BLE_OBSV_SRVC_OnBleEvent");
  NRF_LOG_FLUSH();

  switch (pEvent->header.evt_id) {
  case BLE_GATTS_EVT_HVN_TX_COMPLETE:
    break;
  case BLE_GATTS_EVT_WRITE: // write operation performed
    NRF_LOG_INFO("Received BLE_GATTS_EVT WRITE!");
    NRF_LOG_FLUSH();
    break;
  case BLE_GAP_EVT_CONNECTED:

    NRF_LOG_INFO("BLE_SoftDevice, GAP_EVT_CONNECTED, Connected ");
    NRF_LOG_FLUSH();

    break;
  case BLE_GAP_EVT_DISCONNECTED:
    NRF_LOG_INFO("Disconnected");
    
    NRF_LOG_INFO("BLE_SoftDevice, GAP_EVT_DISCONNECTED, Disconnected ");
    NRF_LOG_FLUSH();

    break;
  case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
    NRF_LOG_INFO("BLE_SoftDevice, BLE_GATTC_EVT_EXCHANGE_MTU_RSP");
    NRF_LOG_FLUSH();

    break;
  case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    NRF_LOG_INFO("BLE_SoftDevice, GAP_EVT_PHY_UPDATE_REQUEST, PHY Update Request");
    NRF_LOG_FLUSH();

    break;
  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:

    NRF_LOG_INFO("BLE_SoftDevice, GAP_EVT_SEC_PARAMS_REQUEST");
    NRF_LOG_FLUSH();

    break;
  case BLE_GATTS_EVT_SYS_ATTR_MISSING:

    NRF_LOG_INFO("BLE_SoftDevice, GATTS_EVT_SYS_ATTR_MISSING");
    NRF_LOG_FLUSH();

    break;
  case BLE_GATTC_EVT_TIMEOUT:

    NRF_LOG_INFO("BLE_SoftDevice, GATTC_EVT_TIMEOUT, Client Event Timeout");
    NRF_LOG_FLUSH();

    break;
  case BLE_GATTS_EVT_TIMEOUT:

    NRF_LOG_INFO("BLE_SoftDevice, GATTS_EVT_TIMEOUT, Server Event Timeout");
    NRF_LOG_FLUSH();
    break;
  default:
    NRF_LOG_INFO("BLE_SoftDevice, Unknown Event");
    NRF_LOG_FLUSH();
    break;
  }
}
#endif // USE_SERVICE_OBSERVER

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_BIOS_Initialize(BLE_BIOSERVICE* pService)
{
  ble_uuid_t            ble_uuid;
  ble_add_char_params_t add_char_params;
  ble_srv_utf8_str_t    encodedString;


  ble_uuid128_t base_uuid = {BIOSERVICE_UUID_BASE}; 
  U32 err_code = sd_ble_uuid_vs_add(&base_uuid, &pService->uuid_type);
  VERIFY_SUCCESS(err_code);

  ble_uuid.type = pService->uuid_type;
  ble_uuid.uuid = BIOSERVICE_UUID_SERVICE;
  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &pService->service_handle);
  VERIFY_SUCCESS(err_code);

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_BATTERY_STATE_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_BATTERY_LEVEL_SIZE;
  add_char_params.max_len           = MS_APP_BLE_BATTERY_LEVEL_SIZE; 
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charBatteryStateHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_BIO_STATE_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_BIO_N_PRESS_SIGNAL_SIZE; 
  add_char_params.max_len           = MS_APP_BLE_BIO_N_PRESS_SIGNAL_SIZE;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charBioStateHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_BPM_NOTICE_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_BPM_NOTICE_SIZE;
  add_char_params.max_len           = MS_APP_BLE_BPM_NOTICE_SIZE;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.write  = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.write_access      = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charBpmStateHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_DATE_NOTICE_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_DATETIME_PING_SIZE; 
  add_char_params.max_len           = MS_APP_BLE_DATETIME_PING_SIZE; 
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.write  = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.write_access      = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charDateNoticeHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_THERAPY_CHOICE_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_THERAPY_STATE_SIZE;
  add_char_params.max_len           = MS_APP_BLE_THERAPY_STATE_SIZE;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.write  = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.write_access      = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charThreapyChoiceHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_THERAPY_LOG_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = MS_APP_BLE_THERAPY_LOG_SIZE; 
  add_char_params.max_len           = MS_APP_BLE_THERAPY_LOG_SIZE;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charTherapyLogHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  ble_srv_ascii_to_utf8(&encodedString, msApp_EXEC_GetHandle()->m_alias);
  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_ALIAS_NAME_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = encodedString.length;
  add_char_params.max_len           = MS_BLE_NAME_STRING_MAX_LENGTH;
  add_char_params.is_var_len        = true;
  add_char_params.p_init_value      = encodedString.p_str;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.write  = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.write_access      = SEC_OPEN;
  err_code = characteristic_add(pService->service_handle, &add_char_params, &pService->charAliasNameHandle);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  ble_srv_ascii_to_utf8(&encodedString, "OK");
  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid              = BIOSERVICE_UUID_STATE_REPORT_CHAR;
  add_char_params.uuid_type         = pService->uuid_type;
  add_char_params.init_len          = encodedString.length;
  add_char_params.max_len           = MS_BLE_STATE_STRING_MAX_LENGTH;
  add_char_params.is_var_len        = true;
  add_char_params.p_init_value      = encodedString.p_str;
  add_char_params.char_props.read   = 1;
  add_char_params.char_props.notify = 1;
  add_char_params.read_access       = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;
  return characteristic_add(pService->service_handle, &add_char_params, &pService->charStateReportHandle);
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msBLE_DIS_Initialize()
{
  ble_dis_init_t    disInit;

  memset(&disInit, 0, sizeof(disInit));
  ble_srv_ascii_to_utf8(&disInit.manufact_name_str, MS_APP_BLE_DIS_MANUFACTUER_NAME);
  ble_srv_ascii_to_utf8(&disInit.model_num_str, MS_APP_BLE_DIS_MODEL_NUMBER);
  ble_srv_ascii_to_utf8(&disInit.serial_num_str, MS_APP_BLE_DIS_SERIAL_NUMBER);
  ble_srv_ascii_to_utf8(&disInit.hw_rev_str, MS_APP_BLE_DIS_HW_REVISION);
  ble_srv_ascii_to_utf8(&disInit.fw_rev_str, MS_APP_BLE_DIS_FW_REVISION);
  ble_srv_ascii_to_utf8(&disInit.sw_rev_str, MS_APP_BLE_DIS_SW_REVISION);

  disInit.dis_char_rd_sec = SEC_OPEN;
  U32 err_code = ble_dis_init(&disInit);
  APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void nrf_qwr_error_handler(U32 nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

static void msBLE_SRVC_Initialize(void)
{
  nrf_ble_qwr_init_t qwr_init = {0};

  qwr_init.error_handler = nrf_qwr_error_handler;         // initialize Queued Write Module.
  U32 err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);
  msBLE_DIS_Initialize();                                 // add device information service
  err_code = msBLE_BIOS_Initialize(&m_bios);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void onAdvertisingEventHandler(ble_adv_evt_t ble_adv_evt)
{
  U32 err_code;

  switch (ble_adv_evt) {
  case BLE_ADV_EVT_FAST:
    //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING); // led blinking
    //APP_ERROR_CHECK(err_code);
    if (m_onAdvertiseHandler != NULL) {
      m_onAdvertiseHandler(m_connection_handle, 0);
    }
    break;
  case BLE_ADV_EVT_IDLE:
    //TODO, later !! we should implement this to save battery power
    //sleep_mode_enter();
    break; 
  default:
    break;
  }
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
static void msBLE_ADVT_Initialize(void)
{
  ble_advertising_init_t init;

  // Service Data in Advertising Packet
  // - Sometimes we want to transmit some of the service data in the advertisement packet
  //   without even connection establishment. In some cases we want to transmit some important
  //   data in the advertisement to enhance user experience.
  //   e.g. battery level, heart rate etc..
  /*
    ble_advdata_service_data_t  service_data[2] = {};
    // add battery level
    U08 batteryLevel = 80;
    uint8_array_t battery;
    battery.size                        = sizeof(batteryLevel);
    battery.p_data                      = &batteryLevel;
    service_data[0].service_uuid        = BLE_UUID_BATTERY_SERVICE;
    service_data[0].data                = battery;;
    // add therapy state (kind, duration)
    U08 therapyState[] = {6, 7, 8, 9, 10};
    uint8_array_t therapy;
    therapy.size                        = sizeof(therapyState);
    therapy.p_data                      = therapyState;
    service_data[1].service_uuid        = BIOSERVICE_UUID_THERAPY_CHOICE_CHAR;
    service_data[1].data                = therapy;

    init.advdata.name_type              = BLE_ADVDATA_FULL_NAME;
    init.advdata.p_service_data_array   = service_data;
    init.advdata.service_data_count     = 2;
  */


  U08 modelNumber[] = MS_APP_BLE_DIS_MODEL_NUMBER;
  U08 charaState [MS_BLE_NAME_STRING_MAX_LENGTH];

  charaState[0] = msApp_EXEC_GetHandle()->m_battery.state.value; // battery level
  charaState[1] = msApp_EXEC_GetHandle()->m_therapy.mode;        // therapy mode
  memcpy(charaState + 2, modelNumber, strlen(modelNumber));  // model number

  uint8_array_t serviceState;
  ble_advdata_service_data_t  service_data = {};

  serviceState.size                   = 2 + strlen(modelNumber);
  serviceState.p_data                 = charaState;
  service_data.service_uuid           = BIOSERVICE_UUID_SERVICE; //BIOSERVICE_UUID_BASE
  service_data.data                   = serviceState;
  memset(&init, 0, sizeof(init));
  init.advdata.name_type              = BLE_ADVDATA_FULL_NAME;
  init.advdata.p_service_data_array   = &service_data;
  init.advdata.service_data_count     = 1;
  // Caution !
  // BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE only advertises during APP_ADV_INTERVAL.
  // BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE is advertised without a deadline and
  // APP_ADV_INTERVAL is set to '0'
  init.advdata.include_appearance     = false;
  init.advdata.flags                  = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; //BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
  init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]); // !! should be modified
  init.srdata.uuids_complete.p_uuids  = m_adv_uuids;
  init.config.ble_adv_fast_enabled    = true;
  init.config.ble_adv_fast_interval   = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout    = APP_ADV_DURATION;
  init.evt_handler = onAdvertisingEventHandler;

  U32 err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);
  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_Advertising(void)
{
  U32 err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function to write data in characteristic attribute value as type of HVX NOTIFICATION
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_HVX_Write(U16 connectionHandle, U16 charValueHandle, U08* pData, U16 offset, U16* pSize)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.handle = charValueHandle;  // characteristic value handle
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.offset = offset; // offset within the attribute value
    params.p_len  = pSize;  // length in bytes to be written, length in bytes written after return
    params.p_data = pData;  // actual data content, use NULL to use the current attribute value
    return sd_ble_gatts_hvx(connectionHandle, &params);
}

/**
 * @brief Function to read a data in characteristic attribute value
 *        charHandle corresponds to characteristic's value_handle or cccd_handle
 *        if the attribute value is longer than the size of the supplied buffer,
 *        data.len will return the total attribute value length (excluding offset),
 *        and not the number of bytes actually returned in data.p_value.
 *        the application may use this information to allocate a sutiable buffer size
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_ReadValue(U16 connectionHandle, U16 charHandle, U08* pData, U16 offset, U16* pSize)
{
  ble_gatts_value_t   data = { .len = *pSize, .offset = offset, .p_value = pData };
  U32 err_code = sd_ble_gatts_value_get(connectionHandle, charHandle, &data);
  VERIFY_SUCCESS(err_code);
  *pSize = data.len;
  return err_code;
}

/**
 * @brief Function to write a data in characteristic attribute value
 *        Values other than system attributes can be set at any time, regardless of whether any active connections exist.
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_WriteValue(U16 connectionHandle, U16 charHandle, U08* pData, U16 offset, U16 nSize)
{
  ble_gatts_value_t   data = { .len = nSize, .offset = offset, .p_value = pData };
  U32 err_code = sd_ble_gatts_value_set(connectionHandle, charHandle, &data);
  VERIFY_SUCCESS(err_code);
  return err_code;
}

/**
 * @brief Function dynamically to update the device name, battery level and therapy mode in advertising packet
 *        
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
U32 msBLE_ADVT_UpdateData(U08* pAliasName, U08 batteryLevel, U08 therapyMode)
{
  U32                     err_code;
  ble_advdata_t           advdata;
  ble_advdata_t           srdata;
  ble_gap_conn_sec_mode_t sec_mode;


  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
  err_code = sd_ble_gap_device_name_set(&sec_mode, pAliasName, strlen(pAliasName));
  APP_ERROR_CHECK(err_code);

  U08 modelNumber[] = MS_APP_BLE_DIS_MODEL_NUMBER;
  U08 charaState [MS_BLE_NAME_STRING_MAX_LENGTH];

  charaState[0] = msApp_EXEC_GetHandle()->m_battery.state.value; // battery level
  charaState[1] = msApp_EXEC_GetHandle()->m_therapy.mode;        // therapy mode
  memcpy(charaState + 2, modelNumber, strlen(modelNumber));  // model number

  uint8_array_t serviceState;
  ble_advdata_service_data_t  service_data = {};

  serviceState.size               = 2 + strlen(modelNumber);
  serviceState.p_data             = charaState;
  service_data.service_uuid       = BIOSERVICE_UUID_SERVICE; //BIOSERVICE_UUID_BASE
  service_data.data               = serviceState;
  memset(&advdata, 0, sizeof(advdata));
  advdata.name_type               = BLE_ADVDATA_FULL_NAME;
  advdata.p_service_data_array    = &service_data;
  advdata.service_data_count      = 1;
  advdata.include_appearance      = false;
  advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  memset(&srdata, 0, sizeof(srdata));
  srdata.uuids_complete.uuid_cnt  = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]); 
  srdata.uuids_complete.p_uuids   = m_adv_uuids;
  err_code = ble_advertising_advdata_update(&m_advertising, &advdata, &srdata); 
  APP_ERROR_CHECK(err_code);
  return err_code;
}

/**
 * @brief Function to 
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void msApp_BLE_Initialize(bool* erase_bonds,
            BLE_ADVERTISE_HANDLER onAdvertiseEventHandler,
            BLE_CONNECTED_HANDLER onConnectHandler, 
            BLE_DISCONNECTED_HANDLER onDisconnectHandler, 
            BLE_WRITTEN_HANDLER onWrittenHandler)
{
  //buttons_leds_init(erase_bonds);
  msBLE_SoftDevice_Initialize();
  gap_params_init();
  gatt_init();
  msBLE_SRVC_Initialize();
  msBLE_ADVT_Initialize();
  conn_params_init();

  m_onAdvertiseHandler = onAdvertiseEventHandler;
  m_onConnectHandler = onConnectHandler;
  m_onDisconnectHandler = onDisconnectHandler; 
  m_onWrittenHandler = onWrittenHandler;
}

#if defined (__cplusplus)
}
#endif