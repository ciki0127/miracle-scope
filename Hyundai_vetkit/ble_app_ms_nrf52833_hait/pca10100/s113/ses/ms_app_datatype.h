/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_datatype.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/03 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ms_app_config.h"
#include "ms_adt.h"

#if !defined (MS_APP_DATATYPE_H)
#define MS_APP_DATATYPE_H


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief defines macros
 */

/**
 * @brief defines the application specific data type
 */

typedef enum {
  BS_UNKNOWN            = 0,
  BS_MAYBE_ABNORMAL     = 1,
  BS_IS_FULL_CHARGED려ㅣ  = 2,  // near 100%
  BS_CAN_OPERATE        = 3, 
  BS_GOOD_TO_SLEEP      = 4,
  BS_SHUTDOWN_INPRORESS = 5,
  BS_SHUTDOWN           = 6
} BATSTATE;

typedef struct VBATTERY {
  U16     pin;            // # of pin
  U16     capacity;       // capa of battery
  U16     level;          // current level (0 ~ 100%)
  U16     state:          // undefined, abnormal, canoperate, maybesleeping, shutdowninprogress, shutdown
} VBATTERY;

typedef struct VRGB {
  U32     a       : 8;  // value of Alpha
  U32     r       : 8;  // value of R
  U32     g       : 8;  // value of G
  U32     b       : 8;  // value of B
} VRGB;

typedef struct VLED {
  U16     pin;            // # of pin
  U16     on;             // on or off
  VRGB    value;          // value set of A, R, B, B
} VLED;

typedef struct VDATE {
  U64     year    : 28;
  U64     month   : 4;    // 1..12
  U64     day     : 5;    // 1..31
  U64     hour    : 5;    // 0..23
  U64     minute  : 6;    // 0..59
  U64     second  : 6;    // 0..59
  U64     millis  : 10;   // 0..999
} VDATE;

typedef enum {
  CS_UNINITIALIZED             = 0,
  CS_INITIALIZED               = 1,
  CS_STARTED                   = 2,  // same value with ADVERTISING
  CS_IS_ADVERTISING            = 2,
  CS_WAITING_FOR_CONNECT       = 3,
  CS_CONNECT_WAS_REQUESTED     = 4,
  CS_CONNECTED                 = 5,
  CS_DISCONNECT_WAS_REQUESTED  = 6,
  CS_DISCONNECTED              = 7,
  CS_STOPPED                   = 8,
} CONNECTSTATE;

typedef struct VCONNECTㅉ{
  I32           cid;            // id of client
  CONNECTSTATE  state;          // uninitialized, initialized, isadvertising, waitingforconnection, connectionwasrequested, connected, disconnectionwasrequested, disconnected
  VDATE         connected;      // timestamp when it was connected with us
  VDATE         disconnected;   // timestamp when it was disconnected with us
} VCONNECT;

enum {
  WS_UNKNOWN              = 0,
  WS_MAYBE_ABNORMAL       = 1,
  WS_IS_AMBIGUOUS         = 2,
  WS_CAN_OPERATE          = 3,
  WS_DEFINITELY_ABNORMAL  = 4,
};

typedef struct VWEARINGㅉ {
  U32     reserved        : 13;
  U32     on              : 1;  // on or off
  U32     condition       : 2;  // undefined, abnormal, normal, ambiguous
  U32     value           : 16;
} VWEARING;

typedef struct MAINTENANCE {
  // insert here !!
  const I08*    manufacturer = "Miracle Scope";
  I08*    model;
  I08*    swVersion;
  I08*    hwVersion;
}


#if defined (__cplusplus)
}
#endif
#endif //MS_APP_DATATYPE_H

