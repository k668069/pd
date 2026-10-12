///////////////////////////////////////////////////////////
// I2CApp.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include "TypeDef.h"
#include <time.h>

#if !defined(IICAPP_H__INCLUDED_)
#define IICAPP_H__INCLUDED_

////////////////////////////////////////////////////////////////////////
// 처리 함수
BOOL InitializeI2C(void);
void DestroyI2C(void);

#ifdef DEF_RTC
void ReadRealTimeClock(char szDateTime[]);
void WriteRealTimeClock(char szDateTime[]);
double ReadTempFromRTC(void);
#endif

#ifdef DEF_TEMPERATURE
void ReadTempAndHumidFromTH(double &fTemperature, double &fHumidity);
#endif

#endif //!defined(IICAPP_H__INCLUDED_)
