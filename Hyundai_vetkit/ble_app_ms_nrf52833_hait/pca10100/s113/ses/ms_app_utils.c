/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_app_utils.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/04/22 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_app_utils.h"


#if defined (__cplusplus)
extern "C" {
#endif

/**
 * @brief generate 16-bits checksum code
 */
U16 msChecksum16(U16 *source, I32 bytes)
{
  U32 cksum = 0;

  while(bytes > 1) {
    cksum += *source++;
    bytes -= sizeof(U16);
  }
  if (bytes) cksum += *(U08*)source;
  cksum = (cksum >> 16) + (cksum & 0xFFFF);
  cksum += (cksum >> 16);
  return (U16)(~cksum);
}

/**
 * @brief conver 64-bits value to string ended with NIL ('\0')
 */
void msLOG_Convert64String(U64 value, U08 *string, U32 size)
{
  U32 i;
  U08 c;
  U08 buffer[32];

  for (i = 0; i < size; i++)
    string[i] = '\0';
  for (i = 0; i < size; i++) {
    c = (U08)(value % 10);
    buffer[i] = c + '0';
    value /= 10;
    if (value == 0) break;
  }
  size = i;
  for (i = 0; i <= size; i++) {
    string[i] = buffer[size-i];
  }
  string[i] = '\0';
}

U08* swap16ToLittleEndian(U16 value, U08* pValue)
{
  pValue[0] = (value >> 8) & 0xFF;
  pValue[1] = (value     ) & 0xFF;
  return pValue;
}

U08* swap32ToLittleEndian(U32 value, U08* pValue)
{
  pValue[0] = (value >> 24) & 0xFF;
  pValue[1] = (value >> 16) & 0xFF;
  pValue[2] = (value >>  8) & 0xFF;
  pValue[3] = (value      ) & 0xFF;
  return pValue;
}


#if defined (__cplusplus)
}
#endif