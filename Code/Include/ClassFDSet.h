///////////////////////////////////////////////////////////
// ClassFDSet.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(CLASSFDSET_H__INCLUDED_)
#define CLASSFDSET_H__INCLUDED_

#include <sys/types.h>
#include "TypeDef.h"

enum FDObjectIDDef{
	YET_FDC_SOCKET		= 0,
	YET_FDC_UART		= 1,
	YET_FDC_UDP			= 2,
};

typedef struct FDInfo_Tag{
	void *pObject;
	int nFd;
	BYTE bHold;
	BYTE ucClass;	//FDÁ¾·ù, 0:Socket, 1:UART
} FDInfo;

class CFDSet
{
public : 
	fd_set m_ActSet;
	fd_set m_RegSet;

	FDInfo m_RInfo[FD_SETSIZE];
	int m_nRegCount;

	CFDSet();
	~CFDSet();

	int RegisterFD(int nFd, void *pObject, BYTE ucClass = YET_FDC_SOCKET);
	int DeleteFD(int nFd);
	int WriteHoldFD(int nFd);
	int WriteUnHoldFD(int nFd);
	int IsWriteHeldFD(int nFd);
	void WriteUnHoldAllFD(void);
	int GetActivedFDSet(void *Object[], int nMaxCount, BYTE* pClass = NULL);
	int IsFDActivated(int nFd);
	void ResetActiveFDSet(void);
};

#endif //CLASSFDSET_H__INCLUDED_
