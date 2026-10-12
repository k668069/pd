///////////////////////////////////////////////////////////
// TimeApp.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(TIMEAPP_H__INCLUDED_)
#define TIMEAPP_H__INCLUDED_

#include <time.h>
#include <sys/timeb.h>
#include "TypeDef.h"

#define TIME_FORMAT_NORMAL					0
#define TIME_FORMAT_EXCLUDE_YEAR		1
#define TIME_FORMAT_INCLUDE_MILLI		2
#define TIME_FORMAT_ONLY_DATE			3
#define TIME_FORMAT_ONLY_TIME				4
#define TIME_FORMAT_ONLY_TIME_MILLI	5
#define TIME_FORMAT_YEAR_MILLI			6

#define TIMER_ID_0		0
#define TIMER_ID_1		1
#define TIMER_ID_2		2
#define TIMER_ID_3		3
#define TIMER_ID_4		4
#define TIMER_ID_5		5
#define TIMER_ID_6		6
#define TIMER_ID_7		7
#define TIMER_ID_8		8
#define TIMER_ID_9		9
void SleepMicroSeconds(int nTime);
void SleepMilliSeconds(int nTime);
void SleepSeconds(int nTime);
DDWORD GetTimeTickCount(void);
DDWORD GetCurrentMilliSecond(void);

struct tm *GetCurrentDateTime(void);
struct tm *GetYesterdayDateTime(void);
struct tm *GetTomorrowDateTime(void);
char *GetCurrentDateTimeStr(int nFormat);
char *GetCurrentDateTimeStr(void);
char *GetDateTimeStr(DDWORD dwTime, int nFormat);
char *GetDurationTimeStr(time_t Time);
char *GetSecTimeString(long nSecTime);
char *GetMilliSecTimeStr(long nMilliSecTime);
struct tm *GetDateTimeFromStr(char strDateTime[]);
char *GetMilliTimeStr(DDWORD dwMilliTime);
int IsThisDateTimeOlderThanNow(struct tm *TmComp);

DWORD EncodeCurrentDateTimeToDWORD(void);
DWORD EncodeThisDateTimeToDWORD(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSecond);
struct tm *DecodeDateTimeFromDWORD(DWORD dwTime);

int GetMilliTimeInterval(int nTimerID);
void ResetMilliSecTimer(int nTimerID);
BOOL CheckMilliSecTimer(int nTimerID, int nMilliSecond);

time_t GetSystemTimeTickSec(DDWORD dwTickRef);
DDWORD GetSystemTimeTickMilliSec(DDWORD dwTickRef);

#endif //TIMEAPP_H__INCLUDED_
