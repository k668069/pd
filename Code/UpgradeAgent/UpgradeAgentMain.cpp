/////////////////////////////////////////////////////////// 
// UpgradeAgentMain.cpp 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>

#include "UpgradePacket.h"
#include "TimeApp.h"
#include "LogMessage.h"
#include "IPCSharedMemory.h"
#include "ClassTCPSocket.h"
#include "ClassFDSet.h"

#define BACKLOG  50
#define UPGRADE_AGENT_LISTEN_PORT_NUM	7001
#define REMOTE_SERVER_LISTEN_PORT_NUM	7002
#define REMOTE_SERVER_GATE_LISTEN_PORT_NUM	80

YST_DB_MAIN	*gpDB = NULL;	//DeviceInfoDB.h
YST_IPC		*gpIPC = NULL;	//IPCSharedMemory.h
YST_DB_LOG	*gpLog = NULL;	//LogMessage.h
CTCPSocket gListenSock(LOG_DEBUG_UPGRADE);
CFDSet gRFDSet;
CFDSet gWFDSet;
int gnHighestSockFd;
int gnClientCount = 0;
int gnConnectCount = 0;

typedef void Sigfunc(int);
void sig_term(int signo)
{
	close(gListenSock.m_nFd);
	return;
}

Sigfunc *Signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(sigaction(signo, &act, &oact) < 0);
	return(oact.sa_handler);
}

int UpgradeAgentListenPort(int nPortNum, int nBackLog)
{
	struct sockaddr_in ListenAddr;
	int nTemp, flags;
	int nFd;

	if((nFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
		PrintGeneralLogMsg("[UpgradeAgent]listen socket error");
		return -1;
	}

	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_port   = htons(nPortNum);
	ListenAddr.sin_addr.s_addr = INADDR_ANY;
    
	nTemp = 1;
	if(setsockopt(nFd, SOL_SOCKET, SO_REUSEADDR, &nTemp, sizeof(int)) < 0){
		PrintGeneralLogMsg("[UpgradeAgent]setsockopt-reuseaddr error");
	}
    
	nTemp = 1;
	if(setsockopt(nFd, SOL_SOCKET, SO_KEEPALIVE, &nTemp, sizeof(int)) < 0){
		PrintGeneralLogMsg("[UpgradeAgent]setsockopt-keepalive error");
	}

	if((flags = fcntl(nFd, F_GETFL, 0)) < 0){
		PrintGeneralLogMsg("[UpgradeAgent] F_GETFL error");
	}
	if(fcntl(nFd, F_SETFL, flags | O_NONBLOCK) < 0){//Non-blocking으로 처리한다.
		PrintGeneralLogMsg("[UpgradeAgent] F_SETFL error");
	}

	if(bind(nFd, (struct sockaddr *)&ListenAddr, sizeof(ListenAddr)) < 0){
		PrintGeneralLogMsg("[UpgradeAgent] bind error");
		return -1;
	}

	if(listen(nFd, nBackLog) < 0){
		PrintGeneralLogMsg("[UpgradeAgent] listen error");
		return -1;
	}

	return nFd;
}

CTCPSocket *UpgradeAgentAcceptClientSocket(void)
{
	CTCPSocket *pSockInfo;
	struct sockaddr Addr;
	struct sockaddr_in *pTemp;
	unsigned int addrlen = sizeof(Addr);
  	int nFd, nFlag;

	if((nFd = accept(gListenSock.m_nFd, &Addr, &addrlen)) == -1) return NULL;

	if((nFlag = fcntl(nFd, F_GETFL, 0)) < 0){
		PrintGeneralLogMsg("[UpgradeAgent] Accept client fcntl get error !");
		return NULL;
	}
	if(fcntl(nFd, F_SETFL, nFlag | O_NONBLOCK) < 0){//Non-Blocked Socket
		PrintGeneralLogMsg("[UpgradeAgent] Accept client fcntl set error !");
		return NULL;
	}

	if((pSockInfo = (CTCPSocket *)malloc(sizeof(CTCPSocket))) == NULL) return NULL;
	pSockInfo->Initialize(LOG_DEBUG_UPGRADE, SIZE_128K);
	pTemp = (struct sockaddr_in *)&Addr;
	pSockInfo->SetInformation(nFd, ntohs(pTemp->sin_port), SOCKET_CLASS_CLIENT);
	pSockInfo->SetRemoteName(inet_ntoa(pTemp->sin_addr));
	strcpy(pSockInfo->m_strDebugID, "UpC");
	
	return pSockInfo;
}

int AddToReadWriteSocket(CTCPSocket *pSockInfo)
{
	if(gRFDSet.RegisterFD(pSockInfo->m_nFd, pSockInfo) == -1){
		PrintGeneralLogMsg("[UpgradeMain]Register Cam server to read sock set error !");
		return -1;
	}

	if(gWFDSet.RegisterFD(pSockInfo->m_nFd, pSockInfo) == -1){
		PrintGeneralLogMsg("[UpgradeMain]Register Cam server to write sock set error !");
		gRFDSet.DeleteFD(pSockInfo->m_nFd);
		return -1;
	}

	gnHighestSockFd = MAX(gnHighestSockFd, pSockInfo->m_nFd);
	
	return 0;
}

void DeleteFromReadWriteSocket(CTCPSocket *pSockInfo)
{
	gRFDSet.DeleteFD(pSockInfo->m_nFd);
	gWFDSet.DeleteFD(pSockInfo->m_nFd);
}
void UpgradeAgentDestroyRemoteClient(CTCPSocket *pSockInfo);
void UpgradeAgentOnConnect(CTCPSocket *pSockInfo)
{
    gnClientCount++;
	pSockInfo->m_nStatus = SOCKET_STATUS_CONNECTED;
}

int UpgradeAgentInitializeRemoteClient(CTCPSocket *pSockInfo)
{
	if(AddToReadWriteSocket(pSockInfo) == -1){
		PrintGeneralLogMsg("[UpgradeMain]Accepting New socket : register client error");
		pSockInfo->Destroy();
		free(pSockInfo);
		return -1;
	}

	return 0;
}

void UpgradeAgentDestroyRemoteClient(CTCPSocket *pSockInfo)
{
	DeleteFromReadWriteSocket(pSockInfo);
	if(gnClientCount) gnClientCount--;

	if(pSockInfo != NULL) {
		pSockInfo->Destroy();
		free(pSockInfo);
	}
}

int InitializeUpgradeAgentSocket(void)
{
	int nFd;

	if((nFd = UpgradeAgentListenPort(UPGRADE_AGENT_LISTEN_PORT_NUM, BACKLOG)) == -1){
		PrintGeneralLogMsg("[UpgradeMain]- listen error(port=%d), program quit", UPGRADE_AGENT_LISTEN_PORT_NUM);
		return -1;
	}
	gListenSock.SetInformation(nFd, UPGRADE_AGENT_LISTEN_PORT_NUM, SOCKET_CLASS_LISTEN);
	gListenSock.SetRemoteName("Any Host");
	gnHighestSockFd = nFd;

	if(gRFDSet.RegisterFD(nFd, &gListenSock) == -1){
		PrintGeneralLogMsg("[UpgradeMain]Error adding listen sock to ReadSockSet !");
		return -1;
	}

	return 0;
}

void DestroyUpgradeAgentSocket(void)
{
	int i, nCount;

	gListenSock.CloseSocket();
	
	nCount = gWFDSet.m_nRegCount;
	for(i = 0 ; i < nCount ; i++){
		UpgradeAgentDestroyRemoteClient((CTCPSocket *)gWFDSet.m_RInfo[0].pObject);
	}
}

int UpgradeAgentReadDataFromClient(CTCPSocket *pSockInfo)
{
	return(pSockInfo->ReadDataFromTCPPortAndSaveToQueue());
}

int UpgradeAgentServiceActivatedSocketsForRead(void)
{
	int nActivedSockCount, i, nResult;
	void *pObject[10];
	CTCPSocket *pSockInfo;

	if((nActivedSockCount = gRFDSet.GetActivedFDSet(pObject, 10)) == 0) return 0;
	for(i = 0 ; i < nActivedSockCount ; i++){
		if((CTCPSocket *)pObject[i] == &gListenSock){
			if((pSockInfo = UpgradeAgentAcceptClientSocket()) == NULL){
				PrintGeneralLogMsg("[UpgradeMain]Accepting New socket error");
			}
			else{
				if(UpgradeAgentInitializeRemoteClient(pSockInfo) != -1){
					UpgradeAgentOnConnect(pSockInfo);
				}
			}
		}
		else{
			pSockInfo = (CTCPSocket *)pObject[i];
			if(pSockInfo->m_nStatus != SOCKET_STATUS_CONNECTED) continue;
			nResult = UpgradeAgentReadDataFromClient(pSockInfo);
			if(nResult == SOCK_IO_CLOSE){
				PrintDbgMsg(LOG_DEBUG_UPGRADE, "[UpgradeMain]Client's(IP-%s) connection is unstable for read.", pSockInfo->m_strRemoteName);
				UpgradeAgentDestroyRemoteClient(pSockInfo);
			}
			else{
				if(!gWFDSet.IsWriteHeldFD(pSockInfo->m_nFd)){
					nResult = pSockInfo->SendRegisteredDataToTCPPort();
					if(nResult == SOCK_IO_CLOSE){
						PrintDbgMsg(LOG_DEBUG_UPGRADE, "[UpgradeMain] Client's(IP-%s) connection is unstable for write.", pSockInfo->m_strRemoteName);
						UpgradeAgentDestroyRemoteClient(pSockInfo);
					}
					else if(nResult == SOCK_IO_DONE){
						gWFDSet.WriteHoldFD(pSockInfo->m_nFd);	//There is no data to write, so hold it
					}
				}
			}
		}
	}
	return 0;
}

int UpgradeAgentServiceActivatedSocketsForWrite(void)
{
	int nActivedSockCount, i, nResult;
	void *pObject[10];
	CTCPSocket *pSockInfo;

	if((nActivedSockCount = gWFDSet.GetActivedFDSet(pObject, 10)) == 0) return 0;
	for(i = 0 ; i < nActivedSockCount ; i++){
		pSockInfo = (CTCPSocket *)pObject[i];
		if(pSockInfo->m_nStatus != SOCKET_STATUS_CONNECTED) continue;
		nResult = pSockInfo->SendRegisteredDataToTCPPort();
		if(nResult == SOCK_IO_CLOSE){
			PrintDbgMsg(LOG_DEBUG_UPGRADE, "[UpgradeMain] Client's(IP-%s) connection is unstable for write.", pSockInfo->m_strRemoteName);
			UpgradeAgentDestroyRemoteClient(pSockInfo);
		}
		else if(nResult == SOCK_IO_DONE){
			gWFDSet.WriteHoldFD(pSockInfo->m_nFd);	//There is no data to write, so hold it
		}
	}
	return 0;
}

void UpgradeAgentFlushSocketsForWrite(CTCPSocket *pSockInfo)
{
	if(pSockInfo->m_nStatus != SOCKET_STATUS_CONNECTED) return;
	int nResult = SOCK_IO_REMAIN;
	do{
		nResult = pSockInfo->SendRegisteredDataToTCPPort();
		if(nResult == SOCK_IO_CLOSE){
			PrintDbgMsg(LOG_DEBUG_UPGRADE, "[UpgradeMain] Client's(IP-%s) connection is unstable for write.", pSockInfo->m_strRemoteName);
			UpgradeAgentDestroyRemoteClient(pSockInfo);
		}
		else if(nResult == SOCK_IO_DONE){
			gWFDSet.WriteHoldFD(pSockInfo->m_nFd);	//There is no data to write, so hold it
		}
	} while(nResult == SOCK_IO_REMAIN);
}

int ProcessUpgradeAgentMain(void)
{
	struct timeval TimeOut;
	int nResult;

	if(InitializeUpgradeAgentSocket() < 0) return -1;

	while(1){
		gRFDSet.ResetActiveFDSet();
		gWFDSet.ResetActiveFDSet();
		TimeOut.tv_sec = 0;
		TimeOut.tv_usec = 10000;

		nResult = select(gnHighestSockFd+1, &gRFDSet.m_ActSet, &gWFDSet.m_ActSet, NULL, &TimeOut);
		if(nResult < 0 && errno != EINTR ) {
			PrintGeneralLogMsg("[UpgradeMain]select error = %s", strerror(errno));
			break;
		}

		if(UpgradeAgentServiceActivatedSocketsForRead() == -1) break;
		if(UpgradeAgentServiceActivatedSocketsForWrite() == -1) break;
	}

	DestroyUpgradeAgentSocket();

	return 0;
}

int main(int argc, char **argv)
{
	int nDBShmID, nIPCShmID, nLogShmID;

	gpDB = (YST_DB_MAIN *)InitializeYDBSharedMemory(&nDBShmID, sizeof(YST_DB_MAIN));
	gpIPC = (YST_IPC *)InitializeIPCSharedMemory(&nIPCShmID, sizeof(YST_IPC));
	gpLog = (YST_DB_LOG *)InitializeLogSharedMemory(&nLogShmID, sizeof(YST_DB_LOG));

	Signal(SIGTERM, sig_term);
	Signal(SIGPIPE, SIG_IGN);

	if(ProcessUpgradeAgentMain() < 0) return -1;

	DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
	DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
	DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);

	return 0;
}
