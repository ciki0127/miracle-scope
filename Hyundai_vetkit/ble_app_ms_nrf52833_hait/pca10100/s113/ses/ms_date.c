/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_date.c					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/06/09 - 1st version released			   
 *				  															   
 *******************************************************************************/
#include "ms_date.h"


#if defined (__cplusplus)
extern "C" {
#endif

// millisecondsSinceEpoch, Unix epoch, since 1970-01-01T00:00:00Z(UTC)
// day(ms) = 86,400,000
// year(ms) = 31,536,000,000 
// YR(2000 - 1970)(ms) = 946,080,000,000 

const VDATE baseDate = { 
            .year   = 1970, 
            .month  = 1, 
            .day    = 1, 
            .wday   = 0, 
            .hour   = 0, 
            .minute = 0, 
            .second = 0, 
            .millis = 0 
          };
VDATE m_systemDate = baseDate;
U64   m_baseInMillis = 0;
U64   m_startInMillis = 0;

/**
 * @brief Function to initialize
 *
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */
void msDATE_Initialize(VDATE *date)
{
  msDATE_Uninitialize();
  m_systemDate = *date;
  m_baseInMillis = convertDateInMillisecond(&m_systemDate);
  m_startInMillis = msTSK_NowInMillisecond();
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
void msDATE_Uninitialize()
{
  m_systemDate = baseDate;
  m_baseInMillis = convertDateInMillisecond(&m_systemDate);
  m_startInMillis = m_baseInMillis = 0;
}

// day of week
static const char days[8][10] =
{
    "",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};
  
// months
static const char months[13][10] =
{
    "",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};
  
/**
 * @brief Format system time
 * @param[in] time System time
 * @param[out] str NULL-terminated string representing the specified time
 * @return Pointer to the formatted string
 **/
const I08* formatSystemTime(U64 time, I08* str)
{
  U16 hours;
  U08 minutes;
  U08 second;
  U16 millis;
  static I08 buffer[24];
  
    
  millis = time % 1000;
  time /= 1000;
  second = time % 60;
  time /= 60;
  minutes = time % 60; 
  time /= 60;
  hours = time;
  
  if(!str) {
     str = buffer;
  }
  if(hours > 0) {
  } else if(minutes > 0) {
  } else if(second > 0) {
  } else {
  }
  return str;
 }
  
/**
 * @brief Format date
 * @param[in] date Pointer to a structure representing the date
 * @param[out] str NULL-terminated string representing the specified date
 * @return Pointer to the formatted string
 **/
const I08 *formatDate(const VDATE *date, I08 *str)
{
  static I08 buffer[40];
  
  if (!str) {
     str = buffer;
  }
  if(date->wday) {
  } else {
  }
  return str;
}
  
/**
  * @brief Calculate day of week
  * @param[in] y Year
  * @param[in] m Month of year (in range 1 to 12)
  * @param[in] d Day of month (in range 1 to 31)
  * @return Day of week (in range 1 to 7)
  **/
U08 computeDayOfWeek(U16 y, U08 m, U08 d)
{
  U32 h, j, k;
  
  //January and February are counted as months 13 and 14 of the previous year
  if (m <= 2) {
    m += 12;
    y -= 1;
  }
  
  //J is the century
  j = y / 100;
  //K the year of the century
  k = y % 100;
  //Compute H using Zeller's congruence
  h = d + (26 * (m + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);
  //Return the day of the week
  return ((h + 5) % 7) + 1;
}
  
/**
 * @brief Convert millisecond timestamp to date
 * @param[in] t Unix timestamp
 * @param[out] date Pointer to a structure representing the date and time
 *
 * @retval 
 * @retval 
 */ 
void convertMillisecondInDate(U64 timeInMillis, VDATE *date)
{
  U32 a, b, c, d, e, f;

  if (timeInMillis < 1) {     // negative Unix time values are not supported
    timeInMillis = 0;
  }
  date->millis = timeInMillis % 1000;
  timeInMillis /= 1000;
  date->second = timeInMillis % 60;
  timeInMillis /= 60;
  date->minute = timeInMillis % 60;
  timeInMillis /= 60;
  date->hour = timeInMillis % 24;
  timeInMillis /= 24;
  
  //Convert Unix time to date
  a = (U32) ((4 * timeInMillis + 102032) / 146097 + 15);
  b = (U32) (timeInMillis + 2442113 + a - (a / 4));
  c = (20 * b - 2442) / 7305;
  d = b - 365 * c - (c / 4);
  e = d * 1000 / 30601;
  f = d - e * 30 - e * 601 / 1000;
  
  //January and February are counted as months 13 and 14 of the previous year
  if(e <= 13)
  {
     c -= 4716;
     e -= 1;
  }
  else
  {
     c -= 4715;
     e -= 13;
  }
  
  //Retrieve year, month and day
  date->year = c;
  date->month = e;
  date->day = f;
  //Calculate day of week
  date->wday = computeDayOfWeek(c, e, f);
}
  
/**
 * @brief Convert date to millisecond timestamp
 * @param[in] date Pointer to a structure representing the date and time
 * @return Unix timestamp
 *
 * @retval 
 * @retval 
 */ 
U64 convertDateInMillisecond(const VDATE *date)
{
  U32 y, m, d;
  U64 t;
  
  y = date->year;
  m = date->month;
  d = date->day;
  
  //January and February are counted as months 13 and 14 of the previous year
  if (m <= 2) {
    m += 12;
    y -= 1;
  }
  
  //Convert years to days
  t = (365 * y) + (y / 4) - (y / 100) + (y / 400);
  //Convert months to days
  t += (30 * m) + (3 * (m + 1) / 5) + d;
  //Unix time starts on January 1st, 1970
  t -= 719561;
  //Convert days to second
  t *= 86400;
  //Add hours, minutes and second
  t += (3600 * date->hour) + (60 * date->minute) + date->second;
  t = t * 1000 + date->millis;
  return t;
}
  
/**
 * @brief Compare dates
 * @param[in] date1 Pointer to the first date
 * @param[in] date2 Pointer to the second date
 * @return Comparison result
 *
 * @retval 
 * @retval 
 */ 
I32 compareDate(const VDATE *date1, const VDATE *date2)
{
  I32 res;
  
  if (date1->year < date2->year) {
     res = -1;
  } else if (date1->year > date2->year) {
     res = 1;
  } else if (date1->month < date2->month) {
     res = -1;
  } else if (date1->month > date2->month) {
     res = 1;
  } else if (date1->day < date2->day) {
     res = -1;
  } else if (date1->day > date2->day) {
     res = 1;
  } else if (date1->hour < date2->hour) {
     res = -1;
  } else if (date1->hour > date2->hour) {
     res = 1;
  } else if (date1->minute < date2->minute) {
     res = -1;
  } else if (date1->minute > date2->minute) {
     res = 1;
  } else if (date1->second < date2->second) {
     res = -1;
  } else if (date1->second > date2->second) {
     res = 1;
  } else if (date1->millis < date2->millis) {
     res = -1;
  } else if (date1->millis > date2->millis) {
     res = 1;
  } else {
     res = 0;
  }
  return res;
}

/**
 * @brief Function to get the current date
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
VDATE msDATE_GetCurrentTime() 
{
  U64 currentInMillis = m_baseInMillis + msTSK_NowInMillisecond() - m_startInMillis;
  VDATE date;
  convertMillisecondInDate(currentInMillis, &date);
  return date;
}


/**
 * @brief Function to get the current time in millisecond
 * @param[in] 
 * @param[out]
 *
 * @retval 
 * @retval 
 */ 
U64 msDATE_GetCurrentTimeInMillisecond() 
{
  U64 currentInMillis = m_baseInMillis + msTSK_NowInMillisecond() - m_startInMillis;
  return currentInMillis;
}

#if defined (__cplusplus)
}
#endif
