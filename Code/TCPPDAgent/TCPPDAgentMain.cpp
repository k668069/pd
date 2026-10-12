/////////////////////////////////////////////////////////// 
// TCPPDAgentMain.cpp 
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <netinet/tcp.h>
#include <fcntl.h>

#include "TimeApp.h"
#include "LogMessage.h"
#include "DeviceInfoDB.h"
#include "IPCSharedMemory.h"
#include "ClassTCPSocket.h"
#include "ClassFDSet.h"
#include "CommonApp.h"
#include "StringApp.h"
#include "PDDevice.h"

YST_DB_MAIN	*gpDB = NULL;	//DeviceInfoDB.h
YST_IPC		*gpIPC = NULL;	//IPCSharedMemory.h
YST_DB_LOG	*gpLog = NULL;	//LogMessage.h

static int m_serverSocket = 0 ;
int m_clientSocket  = 0;

int InitializeTCPPdAgentSocket(void)
{
	int nFd = 0;

	if((nFd = TCPPdAgentListen(gpDB->PdNode.wMonPort)) == -1){
		PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] Listen error(port=%d)\r\n", gpDB->PdNode.wMonPort);
		return -1;
	}
	PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] Initialization complete.\r\n");

	return 0;
}

void DestoryTCPPdAgentSocket(void)
{	
	int ret ;
	m_flagDestroy = 1 ;

	if(m_serverSocket >= 0)
	{
		ret = close(m_serverSocket) ;
		PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] Close server(%d)\r\n", m_serverSocket);
	}

	if(m_clientSocket >= 0)
	{
		ret = close(m_clientSocket) ;
		PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] Close client(%d)\r\n", m_clientSocket);
	}
	
	m_flagDestroy = 0 ;
}

int ProcessTCPPdAgentMain(void)
{
	while(1)
	{
		// TCP connecting
		while(1)
		{
			if (gpDB->PdNode.wMonPort == 0)
			{
				PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] error, Check IP or Port.\r\n");

				SleepSeconds(5);
				continue;
			}

			if(InitializeTCPPdAgentSocket() >= 0)
			{
				SleepMilliSeconds(100);
				break;
			}else
			{
				DestoryTCPPdAgentSocket();
			}

			SleepSeconds(5);
		}
		
		// 송수신, 처리
		while(1)
		{
			if(TCPPdPacketOnProcessIdle() == -1) break;
			SleepMilliSeconds(1);		// delay 1ms
		}
		PrintDbgMsg(LOG_DEBUG_TCPPD, "[PDTcp] exit\n");
		DestoryTCPPdAgentSocket();
	}

	return -1;
}

int main(void)
{
	int nDBShmID, nIPCShmID, nLogShmID;

	gpDB = (YST_DB_MAIN *)InitializeYDBSharedMemory(&nDBShmID, sizeof(YST_DB_MAIN));
	gpIPC = (YST_IPC *)InitializeIPCSharedMemory(&nIPCShmID, sizeof(YST_IPC));
	gpLog = (YST_DB_LOG *)InitializeLogSharedMemory(&nLogShmID, sizeof(YST_DB_LOG));
	
		Signal(SIGTERM, sig_term);
	Signal(SIGPIPE, SIG_IGN);
	
	SleepMilliSeconds(100);
	AppPDCallbackFunc = AppPDCallback;
	
	TCPPdPacketOnStart();  	// TCP용 변수 초기화
	SleepMilliSeconds(100);

	while(1) {
		if(ProcessTCPPdAgentMain() < 0) break;
	}
	
	TCPPdPacketOnDestory();
	DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
	DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
	DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);

	return 0;
}


