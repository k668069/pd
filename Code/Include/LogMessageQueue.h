///////////////////////////////////////////////////////////
// LogMessageQueue.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#if !defined(LOGMESSAGEQUEUE_H__INCLUDED_)
#define LOGMESSAGEQUEUE_H__INCLUDED_

#include <time.h>
#include "TypeDef.h"
#include "DeviceInfoDB.h"

void ClearLogMessageQueue(void);
int GetLogMessageQueueDataSize(void);
int WriteToLogMessageQueue(BYTE ucType, const char strMessage[]);
void ReadFromLogMessageQueue(int nIndex, YST_LOG_DATA *pLog);

#endif //!defined(LOGMESSAGEQUEUE_H__INCLUDED_)
