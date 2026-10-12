/////////////////////////////////////////////////////////// 
// ClassQueue.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(QUEUE_H__INCLUDED_)
#define QUEUE_H__INCLUDED_

#include "TypeDef.h"

#define QBUFF_SIZE 1024L

enum QB_INDEX{		// Queue Buffer States
	QB_DATASIZESMALL,
	QB_DATACORRECT,
	QB_DATANOTCORRECT
};

class CQueue
{
public : 

	char *QBuffer;
	int m_nHead, m_nTail;
	int m_nBufferSize;
	char m_Data[QBUFF_SIZE];

	CQueue(DWORD nSize=0);
	~CQueue();
	
	void Initialize(DWORD nSize=0);
	void Destroy(void);
	void ClearQueue(void);
	int GetQueueDataSize(void);
	int SaveToQueue(const void *Data, int nCount=1);
	int LoadFromQueue(void *Data, int nCount=1);
	int DeleteFromQueue(int nCount);
	int ReadQueueForTest(void *Data, int nCount, int nOffset=0);
	int TestQueueData(char aCh, int nIndex);
	void DeleteFirstDataOfQueue(void);
};
#endif //QUEUE_H__INCLUDED_
