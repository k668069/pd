/////////////////////////////////////////////////////////// 
// OneM2MAgentMain.cpp
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "DeviceInfoDB.h"
#include "IPCSharedMemory.h"
#include "LogMessage.h"
#include "TimeApp.h"
#include "StringApp.h"
#include "CommonApp.h"
#include "OneM2MCommon.h"

YST_DB_MAIN	*gpDB = NULL;	//DeviceInfoDB.h
YST_IPC		*gpIPC = NULL;	//IPCSharedMemory.h
YST_DB_LOG	*gpLog = NULL;	//LogMessage.h

void CoAPZW_Initialize(void);
void CoAPZW_Destroy(void);
int CoAPZW_SetDllLogLevelInDetail(WORD wDevID, BYTE bInDetail);
int CoAPZW_DisableComm(WORD wDevID, BYTE bDisable);
int CoAPZW_CreateDevice(WORD wDevID, const char szRTUName[], char szDstIPAddr[], WORD wDstPortNo);
void CoAPZW_DestroyDevice(WORD wDevID);
int CoAPZW_StartService(WORD wDevID, WORD wPortNo, BYTE bSec, BYTE bSecDebug, char szSecPW[]);
int CoAPZW_ProcessMsg(WORD wDevID);

int InitializeM2MAgent(void)
{
	char szIPAddr[100];
	BYTE *mIP;

	CoAPZW_Initialize();

	mIP = gpDB->Platform.mPeerIP;
	sprintf(szIPAddr, "%d.%d.%d.%d", mIP[0], mIP[1], mIP[2], mIP[3]);
	if(CoAPZW_CreateDevice(COAP_DEVID, "Name", szIPAddr, gpDB->Platform.wPeerPort) != 1){
		PrintDbgMsg(LOG_ERR, "[CoAP] CreateDevice Fail, IP=%s:%d\r\n", szIPAddr, gpDB->Platform.wPeerPort);
		return -1;
	}

	if(CoAPZW_StartService(COAP_DEVID, gpDB->Platform.wMyPort, gpDB->Platform.bSec, gpDB->Platform.bSecDebug, gpDB->Platform.szSecPW) < 1){
		PrintDbgMsg(LOG_ERR, "[CoAP] StartService Fail, Port=%d\r\n", gpDB->Platform.wMyPort);
		return -1;
	}

	memset(gpDB->Platform.szCtrlDat, 0, sizeof(gpDB->Platform.szCtrlDat));

	return 1;
}

void DestroyM2MAgent(void)
{
	CoAPZW_DestroyDevice(COAP_DEVID);
	CoAPZW_Destroy();
}

int ProcessM2MAgentMain(void)
{
	if(CoAPZW_ProcessMsg(COAP_DEVID) == -1)
	{
		PrintDbgMsg(LOG_DEBUG_PLATSEC, "[SEC] CoAPZW_ProcessMsg, exit\r\n");
		return -1;
	}
	

	PrintDbgMsg(LOG_DEBUG_PLATSEC, "[SEC] CoAPZW_ProcessMsg, ok\r\n");

	if(gpDB->System.bNewSensorExist) {
		RegisterNewSensorToPlatform();
		gpDB->System.bNewSensorExist = 0;
	}

	if(gpDB->Platform.ucM2MVer == 0) CheckAndReSndPacketR0();
	else CheckAndReSndPacketR1();

	DDWORD dwNow = GetTimeTickCount();
	if(g_dwTxLEDOnTime && (dwNow > g_dwTxLEDOnTime + 100)){
		M2MTxLEDOnOff(0); // off
		g_dwTxLEDOnTime = 0;
	}

	if(g_dwRxLEDOnTime && (dwNow > g_dwRxLEDOnTime + 100)) {
		M2MRxLEDOnOff(0); // off
		g_dwRxLEDOnTime = 0;
	}

	return 0;
}

int main(void)
{
	int nDBShmID, nIPCShmID, nLogShmID;
	DWORD nLoopCount = 0;
	
	gpDB = (YST_DB_MAIN *)InitializeYDBSharedMemory(&nDBShmID, sizeof(YST_DB_MAIN));
	gpIPC = (YST_IPC *)InitializeIPCSharedMemory(&nIPCShmID, sizeof(YST_IPC));
	gpLog = (YST_DB_LOG *)InitializeLogSharedMemory(&nLogShmID, sizeof(YST_DB_LOG));

	// OneM2M을 사용하는 경우에만 처리
	if(gpDB->Platform.ucM2MType == 0) {
		while(1) 
		{
			while(1) 
			{
				if(InitializeM2MAgent() == 1) break; 
				SleepSeconds(5);		// delay 5sec 
			}
			PrintDbgMsg(LOG_ERR, "[CoAP] InitializeM2MAgent\r\n");

			while(1)
			{
				if(++nLoopCount == 100001) nLoopCount = 1;	// max 100000ms = 100sec

				if(ProcessM2MAgentMain() == -1) break;
				SleepMilliSeconds(1);		// delay 1ms 
			}//end of while
			PrintDbgMsg(LOG_ERR, "[CoAP] ProcessM2MAgentMain, exit\r\n");
		}
		DestroyM2MAgent();
	}

	DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
	DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
	DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);

	return 0;
}