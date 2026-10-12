/////////////////////////////////////////////////////////// 
// ClassPktQueue.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(PKT_QUEUE_H__INCLUDED_)
#define PKT_QUEUE_H__INCLUDED_

#include "TypeDef.h"
#include <pthread.h>

class CPktQueue
{
public : 
	BYTE **QBuff;
	int m_nHead, m_nTail;
	WORD m_wQSize;
	WORD m_wPktSize;
	pthread_mutex_t m_mx;
	// pthread_mutexattr_t m_attr;

	CPktQueue(WORD wPktSize, WORD wQSize);
	~CPktQueue();
	
	void Initialize(WORD wPktSize, WORD wQSize);
	void Destroy(void);
	void ClearQueue(void);
	int GetSavedQueueCnt(void);
	int SaveToQueue(const void *Data);
	void *LoadFromQueue(void);
	void *GetQueueDataPtr(int nIdx);
	void LockQueue(void);
	void UnLockQueue(void);
};
#endif //PKT_QUEUE_H__INCLUDED_
