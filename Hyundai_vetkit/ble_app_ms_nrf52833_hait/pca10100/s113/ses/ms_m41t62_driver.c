/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_m41t62_driver.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/9 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_m41t62_driver.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define CONFIG_SYS_I2C_RTC_ADDR 0xD0
#define M41T62_WHO_AM_I         (0xD0>>1) // 0x68
#define M41T62_REG_SSEC         0
#define M41T62_REG_SEC          1
#define M41T62_REG_MIN          2
#define M41T62_REG_HOUR         3
#define M41T62_REG_WDAY         4
#define M41T62_REG_DAY          5
#define M41T62_REG_MON          6
#define M41T62_REG_YEAR         7
#define M41T62_REG_ALARM_MON	0xa
#define M41T62_REG_ALARM_DAY	0xb
#define M41T62_REG_ALARM_HOUR	0xc
#define M41T62_REG_ALARM_MIN	0xd
#define M41T62_REG_ALARM_SEC	0xe
#define M41T62_REG_FLAGS	0xf
#define M41T62_DATETIME_REG_SIZE  (M41T62_REG_YEAR + 1)
#define M41T62_ALARM_REG_SIZE	\
	(M41T62_REG_ALARM_SEC + 1 - M41T62_REG_ALARM_MON)
#define M41T62_SEC_ST		(1 << 7)	/* ST: Stop Bit */
#define M41T62_ALMON_AFE	(1 << 7)	/* AFE: AF Enable Bit */
#define M41T62_ALMON_SQWE	(1 << 6)	/* SQWE: SQW Enable Bit */
#define M41T62_ALHOUR_HT	(1 << 6)	/* HT: Halt Update Bit */
#define M41T62_FLAGS_AF		(1 << 6)	/* AF: Alarm Flag Bit */
#define M41T62_FLAGS_BATT_LOW	(1 << 4)	/* BL: Battery Low Bit */
#define M41T62_FEATURE_HT	(1 << 0)
#define M41T62_FEATURE_BL	(1 << 1)

#define BCD2BIN(val)	(((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)	((((val)/10)<<4) + (val)%10)


static volatile bool twi_xfer_done = false;
static const nrf_drv_twi_t m41t62TWI = NRF_DRV_TWI_INSTANCE(MS_APP_TWI_INSTANCE_ID);
TASK_HANDLER mt41FnUserHandler = NULL;


static void M41T62_TWI_CommonHandler(nrf_drv_twi_evt_t const* pEvent, void* pUser)
{
  switch(pEvent->type) {
  case NRF_DRV_TWI_EVT_DONE:                // transfer completed event.
    twi_xfer_done = true;
    return;
  case NRF_DRV_TWI_EVT_ADDRESS_NACK:        // error event: NACK received after sending the address.
    twi_xfer_done = true;
    return;
  case NRF_DRV_TWI_EVT_DATA_NACK:           // error event: NACK received after sending a data byte.
    twi_xfer_done = true;
    return;
  default: break;
  }
  if (mt41FnUserHandler != NULL) {
    mt41FnUserHandler(MS_APP_TWI_INSTANCE_ID, (I32)pEvent, (U64)(U32)pUser);
  }
}

/**
 * @brief Function to initialize the TWI instance and enable
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_Initialize(U32 scl, U32 sda, nrf_drv_twi_frequency_t frequency, TASK_HANDLER fnUserHandler, U64 user)
{
  const nrf_drv_twi_config_t config = {
        .scl = scl,
        .sda = sda,
        .frequency = frequency,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false
      };
  U32 status = nrf_drv_twi_init(&m41t62TWI, 
                    &config, 
                    (fnUserHandler != NULL) ? M41T62_TWI_CommonHandler : NULL, 
                    (fnUserHandler != NULL) ? (VOIDPTR)(U32)user: NULL); // blocking mode
  if (status != NRF_SUCCESS) return MS_FAILURE;
  mt41FnUserHandler = fnUserHandler;
  nrf_drv_twi_enable(&m41t62TWI);
  return MS_SUCCESS;
}

/**
 * @brief Function to uninitialize the TWI instance
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
void M41T62_TWI_Uninitialize()
{
  nrf_drv_twi_disable(&m41t62TWI);
  nrf_drv_twi_uninit(&m41t62TWI);
  mt41FnUserHandler = NULL;
}

static uint8_t date_buf[8];

/**
 * @brief Function to read the current date from the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_ReadDate(VDATE* pDate)
{
  U32 status;
  U08 address;

  for (U32 i = 0; i < 8; i++) {
    //twi_xfer_done = false;
    address = i;
    status = nrf_drv_twi_tx(&m41t62TWI, M41T62_WHO_AM_I, &address, 1, true); // step 1,2
    if (status != NRF_SUCCESS) return MS_FAILURE;
    //do {
    //  __SEV(); // make sure any pending events are cleared
    //  __WFE(); 
    //  __WFE(); // enter System ON sleep mode
    //} while(twi_xfer_done == false);

    //twi_xfer_done = false;
    status = nrf_drv_twi_rx(&m41t62TWI, M41T62_WHO_AM_I, &date_buf[i], 1); // step 3,4
    if (status != NRF_SUCCESS) return MS_FAILURE;
    //do {
    //  __SEV(); // make sure any pending events are cleared
    //  __WFE(); 
    //  __WFE(); // enter System ON sleep mode
    //} while(twi_xfer_done == false);
  }
  pDate->millis = BCD2BIN(date_buf[0]) * 10;
  pDate->second = BCD2BIN(date_buf[1] & 0x7F);
  pDate->minute = BCD2BIN(date_buf[2] & 0x7F);
  pDate->hour   = BCD2BIN(date_buf[3] & 0x3F);
  pDate->wday   = date_buf[4] & 0x07;
  pDate->day    = BCD2BIN(date_buf[5] & 0x3F);
  pDate->month  = BCD2BIN(date_buf[6] & 0x1F);
  pDate->year   = BCD2BIN(date_buf[7]) + 100 + 1900;  // assume 20YY not 19YY, and ignore the Century Bit, U-Boot needs to add 1900 here
  return MS_SUCCESS;
}

/**
 * @brief Function to write the current date to the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_TWI_WriteDate(VDATE* pDate)
{
  U32 status;
  U08 txb[2];

  date_buf[0] = BIN2BCD(pDate->millis / 10);
  date_buf[1] = BIN2BCD(pDate->second);
  date_buf[2] = BIN2BCD(pDate->minute);
  date_buf[3] = BIN2BCD(pDate->hour);
  date_buf[4] = (pDate->wday & 0x07);
  date_buf[5] = BIN2BCD(pDate->day);
  date_buf[6] = BIN2BCD(pDate->month);
  date_buf[7] = BIN2BCD(pDate->year % 100); // assume 20YY not 19YY
        
  for (U08 address = 1; address < 8; address++) {
    txb[0] = address; // write the register address and data into transmit buffer
    txb[1] = date_buf[address];
    //twi_xfer_done = false;
    status = nrf_drv_twi_tx(&m41t62TWI, M41T62_WHO_AM_I, txb, sizeof(txb), false);
    if (status != NRF_SUCCESS) return MS_FAILURE;
    //do {
    //  __SEV(); // make sure any pending events are cleared
    //  __WFE(); 
    //  __WFE(); // enter System ON sleep mode
    //} while(twi_xfer_done == false);
  }
  return MS_SUCCESS;
}


#define TWI_SLAVE_MAX_ADDRESSE  127
/**
 * @brief Function to find the address of the slave receiver
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval
 */
I32 M41T62_WhoIAm(U08 *pWhoIAm)
{
  U32 status;
  U08 sample;

  *pWhoIAm = 0;
  for (U08 address = 1; address <= TWI_SLAVE_MAX_ADDRESSE; address++) {
    status = nrf_drv_twi_rx(&m41t62TWI, address, &sample, sizeof(sample));
    if (status == NRF_SUCCESS) {
      *pWhoIAm = address;
      return MS_SUCCESS;
    }
  }
  return MS_FAILURE;
}