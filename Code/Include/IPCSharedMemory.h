/////////////////////////////////////////////////////////// 
// IPCSharedMemory.h
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(IPCSHAREDMEMORY_H__INCLUDED_)
#define IPCSHAREDMEMORY_H__INCLUDED_

#include <sys/time.h>
#include "TypeDef.h"
#include "DeviceInfoDB.h"
#include "LogMessage.h"

#define IPC_SET_DATA_SIZE		MAX_DESC_STR+1

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IPC Set Definition
enum YET_GIDIPC_Status_Tag{
	YET_SetParameterNotReady,
	YET_SetParameterReady,
	YET_SetParameterData,
	YET_SetParameterWait,
	YET_Exit
};

typedef struct YST_IPC_SET_Tag{//Inter Processor Communication for Data Set
	WORD nHID;		// Set Handler ID
	BYTE ucStatus;	// Set Parameter Class
	BYTE ucGID;		// Group ID
	BYTE ucOID;		// Object ID
	BYTE ucMIndex;	// Module(Unit) Index
	BYTE ucPIndex;	// Port(Channel) Index
	BYTE ucSize;
	char DataBuf[IPC_SET_DATA_SIZE];
} YST_IPC_SET;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct YST_IPC_Tag{
	YST_IPC_SET Set;
	YST_IPC_SET	SetTcppd;
} YST_IPC;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *InitializeYDBSharedMemory(int *shmid, size_t dwSize);
void *InitializeIPCSharedMemory(int *shmid, size_t dwSize);
void *InitializeLogSharedMemory(int *shmid, size_t dwSize);
void DestroyYDBSharedMemory(int shmid, void *pDB, int nFlag);
void DestroyIpcSetSharedMemory(int shmid, void *pIPC, int nFlag);
void DestroyLogSharedMemory(int shmid, void *pLog, int nFlag);

void InitializeIPCSetMessage(void);
int MakeIPCSetMessage(BYTE ucGID, BYTE ucOID, BYTE ucMIndex, BYTE ucPIndex, void *DataBuf, int nBufSize);
int MakeIPCSetMessageTcppd(BYTE ucGID, BYTE ucOID, BYTE ucMIndex, BYTE ucPIndex, void *DataBuf, int nBufSize);

#endif //!defined(IPCSHAREDMEMORY_H__INCLUDED_)
