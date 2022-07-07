/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_datatype.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/21 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#if !defined (MS_DATATYPE_H)
#define MS_DATATYPE_H


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief defines macros
 */
#define MS_UNDEFINED_PTR			NULL
#define MS_UNDEFINED_FUNC			NULL
#define MS_RESERVED_FIELD			0
#define MS_DONT_CARE				0
#define MS_INVALID_ID				-1
#define MS_VALID_ID				1

#define MS_EXTN_ALLOC				0x00
#define	MS_SELF_ALLOC				0x01
#define MS_DONT_FREE				0x00
#define	MS_ALLOC_FREE				0x01

#define msFOURCC(C0, C1, C2, C3)	\
	((U32)(U08)(C0) | ((U32)(U08)(C1) << 8) | ((U32)(U08)(C2) << 16) | ((U32)(U08)(C3) << 24))
#define msEIGHTCC(C0, C1, C2, C3, C4, C5, C6, C7)	\
	((U64)(U08)(C0) | ((U64)(U08)(C1) << 8) | ((U64)(U08)(C2) << 16) | ((U64)(U08)(C3) << 24) | ((U64)(U08)(C4) << 32) | ((U64)(U08)(C5) << 40) | ((U64)(U08)(C6) << 48) | ((U64)(U08)(C7) << 56))
#define msMAKEUUID(C0, C1, C2, C3, S0)  \
	(((U64)((U64)(U08)(C0) | ((U64)(U08)(C1) << 8) |((U64)(U08)(C2) << 16) | ((U64)(U08)(C3) << 24)) << 16) | ((U64)(U08)(S0)))

#define msMAXC(a,b) (((a)>(b))?(a):(b))
#define msMAXV(a,b)                   \
        ({                            \
            __typeof__ (a) _a = (a);  \
            __typeof__ (b) _b = (b);  \
            _a > _b ? _a : _b;        \
        })
#define msMINC(a,b) (((a)<(b))?(a):(b))
#define msMINV(a,b)                   \
        ({                            \
            __typeof__ (a) _a = (a);  \
            __typeof__ (b) _b = (b);  \
            _a < _b ? _a : _b;        \
        })
#define msABC(x) ((x<0)?(-x):(x))
#define msABS(a)                      \
        ({                            \
            __typeof__ (a) _a = (a);  \
            _a < 0 ? -_a : _a;        \
        })


#ifndef MS_CHIP_DATA_BUS_WIDTH
#define MS_CHIP_DATA_BUS_WIDTH        32  //32bit machine
#endif 

enum {	
  MS_FALSE            = 0,
  MS_TRUE             = 1
};

enum {
  /* insert here !! */
  MS_UNDERFLOW        = -5,
  MS_OVERFLOW         = -4,
  MS_INVALID_VALUE    = -3,
  MS_CONTEXT_ERROR    = -2,
  MS_FAILURE          = -1,
  MS_SUCCESS          =  0
};

/**
 * @brief define component state transition table enumerators
 */
enum {
  CUI_STS_NULL        = 0,
  CUI_STS_ALLOC       = 1, 
  CUI_STS_INIT,
  CUI_STS_START,
  CUI_STS_RUN,
  CUI_STS_PAUSE,
  CUI_STS_RESUME,
  CUI_STS_STOP,
  CUI_STS_CLOSE,
  CUI_STS_LOCK,
  CUI_STS_UNLOCK
};

/**
 * @brief defines a common data type
 */
typedef char                I08;
typedef unsigned char       U08;
typedef short               I16;
typedef unsigned short      U16;
typedef int                 I32;
typedef unsigned int        U32;
typedef long                I40;
typedef unsigned long       U40;
#ifdef USE_INTEL_64BIT
  typedef signed __int64    I64;
  typedef unsigned __int64  U64;
#else
typedef signed   long long  I64;
typedef unsigned long long  U64;
#endif

typedef void                VOID;
typedef void*               VOIDPTR;
#if (MS_CHIP_DATA_BUS_WIDTH == 32)
  typedef U32                 HAND;
#else
  typedef U64                 HAND;
#endif
typedef float               F32;
typedef double              F64;


/**
 * @brief defines data structure
 */
typedef struct UUKEY {
  I32   code;           // event id
  I32   priority;       // priority, it is an application specific can be set
  U64   data;
  I64   key0;		/* -1 => invalid stamp_id. */
  U64   key1;
} UUKEY;

typedef struct VDATE {
  U64     year    : 25;
  U64     month   : 4;    // 1..12
  U64     day     : 5;    // 1..31
  U64     wday    : 3;    // monday ~ sunday, 2022-06-08, added by cspark
  U64     hour    : 5;    // 0..23
  U64     minute  : 6;    // 0..59
  U64     second  : 6;    // 0..59
  U64     millis  : 10;   // 0..999
} VDATE;

#if defined (__cplusplus)
}
#endif
#endif //MS_DATATYPE_H

