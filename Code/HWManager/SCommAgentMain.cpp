/////////////////////////////////////////////////////////// 
// SCommAgentMain.cpp
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "TimeApp.h"
#include "LogMessage.h"
#include "DeviceInfoDB.h"
#include "CommonApp.h"
#include "StringApp.h"
#include "ClassSerialCOM.h"
#include "HWManagerMain.h"
#include "ClassUDPSocket.h"
#include "IPCSharedMemory.h"
#include "DBFileManager.h"
#include "ClassFDSet.h"
#include "HWControl.h"
#include "ClassPktQueue.h"

extern YST_DB_MAIN	*gpDB;	//DeviceInfoDB.h
extern YST_DB_LOG	*gpLog;	//LogMessage.h

// Module
CSerialCOM gUartM3;	// LoRa

static CSerialCOM *gpUartM = &gUartM3;
static BYTE gUMPIdx = 2;  // 포트 번호

CSerialCOM gUartS;
static CSerialCOM *gpUartS = &gUartS;
static BYTE gUPPIdx = 1;  // 포트 번호

CUdpSocket gUdpSock(LOG_DEBUG_SCOMM, 0);
static CUdpSocket *gpUdpSock = &gUdpSock;

static BYTE GetCheckXor(void *pData, int nLen, BYTE ucChkSum)
{
	unsigned char *Data = (unsigned char *)pData;

	if(nLen < 0) return ucChkSum;
	while(nLen--){
		ucChkSum ^= Data[nLen];
	}

	return ucChkSum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic UART Operation
static void DebugLogDataSComm(const char szMsg[], const void* pData, int nSize)
{
	FILE *fp = OpenDebugLogFile(LOG_DEBUG_SCOMMDATA);
	if(fp == NULL) return;
	if(szMsg) fprintf(fp, "%s Size=%d\n", szMsg, nSize);
	PrintDbgData(pData, nSize, fp);
	if(fp != stdout) fclose(fp);
}

static void WriteDataToUDPSocket(int nPIdx, const char *szData, int nDataSize)
{
	//if(nPIdx < 0 || nPIdx >= MAX_MODULE_CNT) return;
	PrintDbgMsg(LOG_DEBUG_SCOMM, "[SComm-UDP] Snd : %s, IP=%s:5070", szData, MakeIPAddrString(gpUdpSock->m_mLastRcvIP));
	gpUdpSock->SendDataToPort(gpUdpSock->m_mLastRcvIP, 5070, (BYTE*)szData, nDataSize);
}

static void WriteDataToUART(int nPIdx, const char *szData, int nDataSize)
{
	if(nPIdx < 0 || nPIdx >= MAX_MODULE_CNT) return;
	PrintDbgMsg(LOG_DEBUG_SCOMM, "[UartM-%d] Snd : %s", nPIdx+1, szData);
	DebugLogDataSComm("Tx =>", szData, nDataSize);
	CSerialCOM *pUart = gpUartM[nPIdx];
	pUart->RegisterDataToQueue((BYTE*)szData, nDataSize);
	pUart->SendQueueDataToCOMPort();
	gpDB->Module[nPIdx].dwLastSndTime = GetTimeTickCount();
	gpDB->Module[nPIdx].ucSndCnt++;
}

static void InitializeRFModule(void)
{
		YST_DB_SCOMM* pSComm = gpDB->SComm+gUMPIdx;
		gpUartM->OpenCOMPort(pSComm->ucPortID, 0, pSComm->ucBaudrate-1, NULL, pSComm->ucDataBits-1, pSComm->ucParity-1, pSComm->ucStopBits-1);
		gpUartM->SetStatusStringPointer(pSComm->szCommStatus);
}

static void DestroyRFModule(void)
{
		gpUartM->CloseCOMPort();
}


static void ProcessUARTSndQData(int nPIdx)
{// 송신하지 못한 큐데이터를 송신한다.
	DWORD dwDataSize;

	if(nPIdx != 0) return;
	dwDataSize = gpUartM->m_QSnd.GetQueueDataSize();
	if(dwDataSize == 0) return;

	// 송신 데이터 처리
	gpUartM->SendQueueDataToCOMPort();
}

static void ProcessUARTRcvQData(int nPIdx)
{// 수신큐에 데이터를 처리한다.
	if(nPIdx != 0) return;
	int nDataSize = gpUartM->m_QRcv.GetQueueDataSize();
}

static void CheckAndReSndReqPacket(int nMIdx)
{
	if(nMIdx >= MAX_MODULE_CNT) return;
	if(gpDB->Module.ucPStep < YET_LORA_PS_WAIT_ALIVE) return;
	DDWORD dwNow = GetCurrentMilliSecond();
	if(dwNow < gpDB->Module.dwLastSndTime + SND_RETRY_TIME) return;
	if(gpDB->Module.ucSndCnt >= SND_RETRY_CNT) {
		gpDB->Module.ucPStep = YET_LORA_PS_FAIL;
		return;
	}
}

static void ProcessUARTSndQDataSPort(int nPIdx)
{// 송신하지 못한 큐데이터를 송신한다.
	DWORD dwDataSize;
	if(nPIdx < 0 || nPIdx >= MAX_SPORT_CNT) return;
	dwDataSize = gpUartS->m_QSnd.GetQueueDataSize();
	if(dwDataSize == 0) return;
	gpUartS->SendQueueDataToCOMPort();
}

static void ProcessUARTRcvQDataSPort(int nPIdx)
{// 수신큐에 데이터를 처리한다.
	if(nPIdx < 0 || nPIdx >= MAX_SPORT_CNT) return;
	int nDataSize = gpUartS->m_QRcv.GetQueueDataSize();
	//printf("nDataSize-%d = %d\n", nPIdx, nDataSize);
	if(nDataSize) AnalyzeRcvDataSPort(&gpUartS->m_QRcv, nDataSize, nPIdx);
}

static void CheckAndSndReqPacketSPort(int nPIdx)
{
	if(gpDB->SPort.wPollInt == 0) return; // off
	if(nPIdx >= MAX_SPORT_CNT) return;
	DDWORD dwNow = GetCurrentMilliSecond();
	// 폴링에 대한 응답을 기다리는 중
	if(dwNow < gpDB->SPort.dwLastSndTime + gpDB->SPort.wWaitTime) return;
		
	if(gpDB->SPort.ucDevPollIdx < gpDB->SPort.ucDevCnt) {// 모든 장비에 대한 폴링을 마치지 않은 경우
		// 다음으로 폴링할 장비와 SSSN 인덱스 결정
		int nDevIdx = gpDB->SPort.ucDevPollIdx;
		SendPollingSPort(nPIdx, nDevIdx, gpDB->SPort.ucSSSNPollIdx);
		gpDB->SPort.dwLastSndTime = dwNow;
		gpDB->SPort.ucSSSNPollIdx++;
		return;
	}
}		

static void ProcessUARTData(void)
{
	for(int i = 0 ; i < MAX_MODULE_CNT ; i++){
		ProcessUARTRcvQData(i);
		ProcessUARTSndQData(i);
		CheckAndReSndReqPacket(i);
		if(i == DEF_LORA_IDX) ProcessLoraSndData(gpSndQLora, i);
	}

	for(int i = 0 ; i < MAX_SPORT_CNT ; i++){		
		ProcessUARTRcvQDataSPort(i);
		ProcessUARTSndQDataSPort(i);
		CheckAndSndReqPacketSPort(i);
	}
}

static void ProcessUDPRcvQData(CUdpSocket *pSockUdp)
{
	//if(nPIdx < 0 || nPIdx >= MAX_MODULE_CNT) return;

	BYTE buf[SIZE_1K];
	int nDSize;
	
	if((nDSize = pSockUdp->m_QRcv.GetQueueDataSize()) <= 0) return;
	if(nDSize >= SIZE_1K) nDSize = SIZE_1K-1;
	pSockUdp->m_QRcv.LoadFromQueue(buf, nDSize);
	DebugLogDataSComm("UDP Rx =>", buf, nDSize);
	while((nDSize>0) && (buf[nDSize-1] == '\r' || buf[nDSize-1] == '\n')) nDSize--;
	buf[nDSize] = 0;
	
	PrintDbgMsg(LOG_DEBUG_SCOMM, "[SComm-UDP] Rcv : %s, IP=%s:%d", (char*)buf, MakeIPAddrString(pSockUdp->m_mLastRcvIP), pSockUdp->m_wLastRcvPort);
	OnReceivePacketFromUdp((char*)buf, nDSize, pSockUdp->m_mLastRcvIP);
}

void InitializeSCommModule(int nPIdx)
{// 모듈 초기화를 다시 시행한다.
	gpUartM[nPIdx]->m_QRcv.ClearQueue();
	if(gpDB->Module[nPIdx].ucMType == YET_RFID_WISUN) ChangeProcessStep(nPIdx, YET_PS_WS_WAIT_ALIVE);
	else ChangeProcessStep(nPIdx, YET_PS_WAIT_ALIVE);
}

void SendSCommMsg(int nPIdx, char szMsg[])
{// 각 모듈에 데이터를 전송한다.
	if(nPIdx == UDP_PORT_IDX-1) {
		if(memcmp(gpUdpSock->m_mLastRcvIP, "\x0\x0\x0\x0", 4)) gpUdpSock->SendDataToPort(gpUdpSock->m_mLastRcvIP, 5070, (BYTE*)szMsg, strlen(szMsg), FALSE);
		else gpUdpSock->SendDataToPort(gpUdpSock->m_mLastRcvIP, 5070, (BYTE*)szMsg, strlen(szMsg), TRUE);
	}
	else WriteDataToUART(nPIdx, szMsg, strlen(szMsg));
}

void SetSensorCtrlUDP(YST_DB_SNODE *pNode)
{
	char szMsg[200];
	if(pNode->bCtrlCfg || pNode->bCtrlPeriod) {
		strcpy(szMsg, "$SSCFG");
		for(int i = 0 ; i < MAX_SNR_CFG_CNT ; i++){
			if(!pNode->szSCNFG[i][0]) break;
			strcat(szMsg, ",");
			strcat(szMsg, pNode->szSCNFG[i]);
		}
		SendSensorCtrlCmd(UDP_PORT_IDX, szMsg, pNode);
		pNode->bCtrlPeriod = FALSE;
		pNode->bCtrlCfg = FALSE;
	}
}

void SetSlaveCtrl(YST_DB_SLAVE *pSlave)
{
	char szMsg[200];
	if(!pSlave->bCtrlCfg) return;
	strcpy(szMsg, "$SSCFG");
	for(int i = 0 ; i < MAX_SNR_CFG_CNT ; i++){
		if(!pSlave->szSCNFG[i][0]) break;
		strcat(szMsg, ",");
		strcat(szMsg, pSlave->szSCNFG[i]);
	}
	SendSlaveCtrlCmd(2, szMsg, pSlave);
	pSlave->bCtrlCfg = FALSE;
}

#ifdef USE_SELECT_FOR_READ_WRITE
CFDSet gRFDSet;
int gnHighestFd = 0;
#endif


void InitializeSCommAgent(void)
{
		// Module
	HWCtrlResetLoRAModule();
	gpDB->Module.ucMType = YET_RFID_LORA;
	memcpy(gpDB->Lora.mEUI, "\x00\x19\xF8\xFF\xFF", 5);//Ziwoo IT
	InitializeRFModule();
	#if MAX_MODULE_CNT == 1
	#ifdef USE_SELECT_FOR_READ_WRITE
			if(gRFDSet.RegisterFD(gpUartM->m_nFd, gpUartM, YET_FDC_UART) == -1){
				PrintGeneralLogMsg("[SComm] Register UART FD to read set error !");
				return;
			}
			gnHighestFd = MAX(gnHighestFd, gpUartM->m_nFd);
	#endif
	#else
	#ifdef USE_SELECT_FOR_READ_WRITE
		for(i = 0 ; i < MAX_MODULE_CNT ; i++){
			if(gRFDSet.RegisterFD(gpUartM[i]->m_nFd, gpUartM[i], YET_FDC_UART) == -1){
				PrintGeneralLogMsg("[SComm] Register UART FD to read set error !");
				return;
			}
			gnHighestFd = MAX(gnHighestFd, gpUartM[i]->m_nFd);
		}
	#endif	
	#endif
	// Serial Port
	InitializeSPort();
#ifdef USE_SELECT_FOR_READ_WRITE
	#if MAX_SPORT_CNT == 1
			if(gRFDSet.RegisterFD(gpUartS->m_nFd, gpUartS, YET_FDC_UART) == -1){
				PrintGeneralLogMsg("[SComm] Register UART FD to read set error !");
				return;
			}
			gnHighestFd = MAX(gnHighestFd, gpUartS->m_nFd);
	#else
		for(i = 0 ; i < MAX_SPORT_CNT ; i++){
			if(gRFDSet.RegisterFD(gpUartS[i]->m_nFd, gpUartS[i], YET_FDC_UART) == -1){
				PrintGeneralLogMsg("[SComm] Register UART FD to read set error !");
				return;
			}
			gnHighestFd = MAX(gnHighestFd, gpUartS[i]->m_nFd);
		}
	#endif
#endif
	// UDP Port
	gpUdpSock->OpenSocket(5070);
	strcpy(gpUdpSock->m_strDebugID, "SComm");
#ifdef USE_SELECT_FOR_READ_WRITE
	if(gRFDSet.RegisterFD(gpUdpSock->m_SockFd, gpUdpSock, YET_FDC_UDP) == -1){
		PrintGeneralLogMsg("[SComm] Error adding listen sock to ReadSockSet !");
		return;
	}
	gnHighestFd = MAX(gnHighestFd, gpUdpSock->m_SockFd);
#endif
	// clear memory
	InitializeSCommModule(0);	
	PrintGeneralLogMsg("SComm Initializer !");
}

void DestroySCommAgent(void)
{
	gpUdpSock->CloseSocket();
	DestroyRFModule();
	DestroySPort();
}

int ServiceActivatedFdForRead(void)
{
	int nActivedFDCount, i;
	void *pObject[10];
	BYTE ucClass[10];

	if((nActivedFDCount = gRFDSet.GetActivedFDSet(pObject, 10, ucClass)) == 0) return 0;
	for(i = 0 ; i < nActivedFDCount ; i++){
		if(ucClass[i] == YET_FDC_UART){
			CSerialCOM *gpUart = (CSerialCOM *)pObject[i];
			gpUart->ReadDataFromCOMPort();
		}
		else if(ucClass[i] == YET_FDC_UDP){
			if(gpUdpSock->ReceiveDataFromPort()) ProcessUDPRcvQData(gpUdpSock);
		}
	}
	return 0;
}

int ProcessSCommAgentMain(void)
{
#ifdef USE_SELECT_FOR_READ_WRITE
	struct timeval TimeOut;
	int nResult;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 1;
	gRFDSet.ResetActiveFDSet();

	nResult = select(gnHighestFd+1, &gRFDSet.m_ActSet, NULL, NULL, &TimeOut);
	if(nResult < 0 && errno != EINTR ) {
		PrintGeneralLogMsg("[SComm] Select error = %s", strerror(errno));
		return 0;
	}
	if(ServiceActivatedFdForRead() == -1) return 0;
#else
	if(gCom.ReadDataFromCOMPort()){
		ReceiveDataFromUART();
	}
	if(gpUdpSock->ReceiveDataFromPort()){
		ProcessUDPRcvQData(gpUdpSock);
	}
#endif
	ProcessUARTData();

	return 0;
}

