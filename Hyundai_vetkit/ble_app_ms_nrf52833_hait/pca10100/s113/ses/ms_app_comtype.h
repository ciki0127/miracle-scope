/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_comtype.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ms_app_config.h"
#include "ms_adt.h"

#if !defined (MS_APP_COMTYPE_H)
#define MS_APP_COMTYPE_H


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief defines macros
 */

/**
 * @brief defines the application common data type
 */

typedef struct VDATEPING {    // added, cspark, 2022-06-30, to synchronize the device's date time with the server(smartphone)
  U64     stamp;              // timestamp of local system
  VDATE   date;
} VDATEPING;

typedef enum {
  BS_UNKNOWN            = 0,
  BS_MAYBE_ABNORMAL     = 1,
  BS_IS_FULL_CHARGED    = 2,  // near 100%
  BS_CAN_OPERATE        = 3, 
  BS_GOOD_TO_SLEEP      = 4,
  BS_SHUTDOWN_INPRORESS = 5,
  BS_SHUTDOWN           = 6
} BATSTATE;

typedef struct VBATTERY {
  U16     pin;            // # of pin for ADC
  U16     capacity;       // mA, capa of battery
  U16     level;          // current level (0 ~ 100%)
  U16     state;          // undefined, abnormal, canoperate, maybesleeping, shutdowninprogress, shutdown
} VBATTERY;

enum {
  // insert here !!
  MS_POWER_SOURC_UNKNOWN  = 0,
  MS_POWER_SOURCE_WIRED   = 1,
  MS_POWER_SOURCE_BATTERY = 2
};

typedef enum {
  MS_POWER_ON         = 1,  // ON
  MS_POWER_SAVING_L1  = 2,  // ON and Idle
  MS_POWER_SAVING_L2  = 3,  // ON and Sleep 
  MS_POWER_SAVING_L3  = 4,  // Hibernation (only memory and RTC are awaking)
  MS_POWER_DOWNING    = 5,
  MS_POWER_OFF        = 6
} VPOWERSTATE;

typedef struct VPOWER {
  U16           source;     // power source
  VPOWERSTATE   state;      // ON or OFF
  struct {
    U32     normalMax : 8;  // max. level for the normal operation
    U32     normalMin : 8;  // min. level for the normal operation
    U32     pwrSaving : 8;  // enter to sleep mode for power saving
    U32     shutDown  : 8;  // threshold to shutdown system
  } lvlCtrl;
  VBATTERY      battery;
} VPOWER;

typedef struct VRGB {
  U32     a       : 8;  // value of Alpha
  U32     r       : 8;  // value of R
  U32     g       : 8;  // value of G
  U32     b       : 8;  // value of B
} VRGB;

typedef struct VLED {
  U16     pin;            // # of pin
  U16     on;             // on or off
  U16     state;          // state
  VRGB    value;          // value set of A, R, B, B
} VLED;

typedef enum {
  CS_UNINITIALIZED                = 0,
  CS_INITIALIZED                  = 1,
  CS_STARTED                      = 2,  // same value with ADVERTISING
  CS_IS_ADVERTISING               = 2,
  CS_WAITING_FOR_CONNECTION       = 3,
  CS_CONNECTION_WAS_REQUESTED     = 4,
  CS_CONNECTED                    = 5,
  CS_DISCONNECTION_WAS_REQUESTED  = 6,
  CS_DISCONNECTED                 = 7,
  CS_STOPPED                      = 8
} CONNECTSTATE;

typedef struct VCONNECTㅉ{
  U32           cid;            // id of client
  CONNECTSTATE  state;          // 
  VDATE         connected;      // timestamp when it was connected with us
  VDATE         disconnected;   // timestamp when it was disconnected with us
} VCONNECT;

typedef struct MAINTENANCE {
  // insert here !!
  I08*    manufacturer;
  I08*    model;
  I08*    swVersion;
  I08*    hwVersion;
} MAINTENANCE;

enum {
  SS_UNKNOWN              = 0,
  SS_MAYBE_ABNORMAL       = 1,
  SS_IS_AMBIGUOUS         = 2,
  SS_CAN_OPERATE          = 3,
  SS_DEFINITELY_ABNORMAL  = 4
};

typedef struct VADC {
  U16     id;                   // channle id
  U16     pin;
  U16     state;
  U16     reserved;
} VADC;

typedef struct VPWMㅉ {
  U16     id;                   // channle id
  U16     on;                   // on or off
  U16     state;
  U16     reserved;
  VLED    out[4];
} VPWM;

typedef struct VSENSOR {          // heart and respiration rate
  VADC      adc;
  struct {  
    I32     on              : 4;  // 2022-06-10, modified by cspar, on or off
    I32     condition       : 6;  // heart - [0..2], respiration [3..5] undefined, abnormal, normal, ambiguous
    I32     value           : 22; // heart - [0..10], respiration [11..22]
  } state;
} VSENSOR;


enum { 
  MS_APP_DATE_NOT_UPDATED_REPORT = 1, 
  MS_APP_DATE_UPDATED_REPORT = 2
};


#if defined (__cplusplus)
}
#endif
#endif //MS_APP_COMTYPE_H

