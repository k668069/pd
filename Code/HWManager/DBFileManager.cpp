/////////////////////////////////////////////////////////// 
// DBFileManager.cpp 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "TypeDef.h"
#include "DeviceInfoDB.h"
#include "LogMessage.h"
#include "DBFileManager.h"
#include "CommonApp.h"
#include "TimeApp.h"

extern YST_DB_MAIN *gpDB;	// DeviceInfoDB.h
extern YST_DB_LOG *gpLog;	// LogMessage.h
void ResetDeviceInfoDB(BOOL bInit)
{
	BYTE mMAC[6];
	char szSerialNo[32];
	BYTE mDNS[4];
	
	BYTE bOperIF;
	BYTE bOperMode;
	char szGWOID[40];
	int i;

	if(bInit) PrintGeneralLogMsg("Device Info DB - Initialized !");
	else {
		memcpy(mMAC, gpDB->General.mMAC, 6);
		strncpy(szSerialNo, gpDB->Chassis.szSerialNo, sizeof(szSerialNo));

		memcpy(mDNS, gpDB->General.mDNS, 4);
		bOperIF = gpDB->General.ucGWIf;
		bOperMode = gpDB->General.ucOperMode;
		strncpy(szGWOID, gpDB->Platform.szGWOID, sizeof(szGWOID));
	}
	memset(gpDB, 0, sizeof(YST_DB_MAIN)-sizeof(YST_DB_LOGQ)-sizeof(YST_SD_LOGQ));
	if(bInit) {
		memcpy(gpDB->General.mMAC, "\x00\x19\xF8\x01\x00\x00", 6);// MAC주소
		//time_t now = time(NULL);
		//struct tm *TmNow = localtime(&now);
		// sprintf(gpDB->Chassis.szSerialNo, "ZSNB%02d%02d%02d-IGW001", TmNow->tm_year%100, TmNow->tm_mon+1, TmNow->tm_mday);
	}
	else {
		memcpy(gpDB->General.mMAC, mMAC, 6);
		strncpy(gpDB->Chassis.szSerialNo, szSerialNo, sizeof(szSerialNo));

		memcpy(gpDB->General.mDNS, mDNS, 4);
		gpDB->General.ucGWIf = bOperIF;
		gpDB->General.ucOperMode = bOperMode;
		strncpy(gpDB->Platform.szGWOID, szGWOID, sizeof(szGWOID));
	}

	gpDB->General.ucDMRMode = 0;
	gpDB->General.bPCommMode = 0;  // ethernet
	gpDB->General.bNTP = 0;		// 비활성
	gpDB->General.bTelnet = 1;	// 활성
	gpDB->General.bFtp = 1;		// 활성
	////////////////////////////////////////////////////////////////////////////////////
	// General Group
	memcpy(gpDB->General.mIP, "\xC0\xA8\x0A\xFA", 4);			// IP주소, 192.168.10.250
	memcpy(gpDB->General.mIP2, "\xC0\xA8\x01\x01", 4);		// IP주소, 192.168.1.1
	memcpy(gpDB->General.mSM, "\xFF\xFF\xFF\x00", 4);			// 넷마스크
	memcpy(gpDB->General.mGW, "\xC0\xA8\x0A\x01", 4);		// 게이트웨이
	strcpy(gpDB->General.szPassword, "1234");				// 접근암호

	gpDB->General.ucOperEth0= YET_ETH_AUTONEG;
	gpDB->General.ucOperEth1= YET_ETH_AUTONEG;  // YET_ETH_OFF;

	////////////////////////////////////////////////////////////////////////////////////
	// Halow Group
	gpDB->General.ucHMode = 0;
	gpDB->General.ucHChannel = 4; // 928.5MHz
	memcpy(gpDB->General.mHMIP1, "\xC0\xA8\x02\x01", 4);	// IP주소, 192.168.2.1
	memcpy(gpDB->General.mHMIP2, "\xC0\xA8\x03\x02", 4);	// IP주소, 192.168.3.2
	memcpy(gpDB->General.mHMIP3, "\xC0\xA8\x04\x02", 4);	// IP주소, 192.168.4.2
	strcpy(gpDB->General.szHMSSID1, "halow_id_mic");			// Halow AP용 SSID
	strcpy(gpDB->General.szHMSSID2, "halow_id_mic");			// Halow Station용 SSID(상대방SSID)
	strcpy(gpDB->General.szHMSSID3, "halow_id_mic");			// Halow Mesh용 SSID(상대방SSID)

	////////////////////////////////////////////////////////////////////////////////////
	// PLATFORM Group
	gpDB->Platform.wRptTime = 60;
	memcpy(gpDB->Platform.mMyIP, gpDB->General.mIP, 4);				// 자기 IP주소
	memcpy(gpDB->Platform.mPeerIP, "\x3D\x28\xDC\x7F", 4);			// 플랫폼 IP주소, 61.40.220.127
	gpDB->Platform.wMyPort = 5683;									// 자기 Listen 포트번호
	gpDB->Platform.wPeerPort = 5683;								// 상대 Listen 포트번호
	strcpy(gpDB->Platform.szGWOID, "1.2.481.1.3.1.00000000000");	// 등록안된 G/W OID
	strcpy(gpDB->Platform.szOIDBase, "1.2.481.1.");					// 기본 OID, ex)1.2.481.1.
	strcpy(gpDB->Platform.szURIPath, "CSEBase");				// URI(Uniform Resource Identifier Path), AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
	strcpy(gpDB->Platform.szCSEID, "Cae-admin");					// AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
	gpDB->Platform.ucM2MType = 0; // oneM2M
	gpDB->Platform.ucM2MVer = 1; // IMR
	gpDB->Platform.wBlockSize = 2048;
	gpDB->Platform.bSec = 0; // Security off
	gpDB->Platform.bSecDebug = 0; // Security debug level
	strcpy(gpDB->Platform.szSecPW, "0000-000B");

	////////////////////////////////////////////////////////////////////////////////////
	// SCOMM Group
	// P1 : COM1, RS232
	// P2 : COM2, RS485
	// P3 : COM3, LoRa
	for(i = 0 ; i < MAX_SCOMM_CNT ; i++){
		gpDB->SComm[i].wDelayTime = 20;		// Milli Seconds, UART로 부터 입력된 데이터를 하나의 패킷으로 전송하기 위해 기다리는 시간.
		gpDB->SComm[i].wMaxPktSize = 250;		// UART로 부터 입력된 데이터가 이 숫자보다 크면 DelayTime이전에도 패킷으로 전송한다.
		gpDB->SComm[i].ucPortID = i+1;
		gpDB->SComm[i].ucBaudrate = 3;		// 1:B2400, B4800, B9600, B19200, B38400, B57600, B115200
		gpDB->SComm[i].ucDataBits = 4;		// 1:5bits, 2:6bits, 3:7bits, 4:8bits
		gpDB->SComm[i].ucStopBits = 1;		// 1:1bit, 2:2bits
		gpDB->SComm[i].ucParity = 1;			// 1:NoParity, 2:OddParity, 3:EvenParity
	}
	
	gpDB->SComm[0].ucBaudrate = 5;  //   B38400  for RS232
	gpDB->SComm[1].ucBaudrate = 6;  //   B57600  for RS485
	gpDB->SComm[2].ucBaudrate = 4;  //   B19200  for lora

	////////////////////////////////////////////////////////////////////////////////////
	// LORA Group
	gpDB->Lora.bUseEnDec = 0;
	gpDB->Lora.ucRFFreq = 0;			// 주파수 채널(0~15), 0:920.3MHz, 0.2MHz 증가, 0-9:센서용, 10-15:슬레이브용
	gpDB->Lora.ucSPFactor = 7;		// Spreading Factor 값(7~12), 값이 높을 수록 도달 거리는 늘어나며, 속도는 감소함
	strcpy(gpDB->Lora.szLoraKey, "1234-5678-90ab-cdef");	// LoRA 송수신 암호화 키 16바이트
	gpDB->Lora.ucTxp = 14;

	////////////////////////////////////////////////////////////////////////////////////
	// SPORT Group
	// P1 : RS232
	// P2 : RS485
		gpDB->SPort.ucDevCnt = 1;		// 병렬로 접속된 디바이스 개수
		gpDB->SPort.wPollInt = 300;		// 폴링주기 sec (default 5분)
		gpDB->SPort.wWaitTime = 500;		// 장비 응답 대기시간, ms

	////////////////////////////////////////////////////////////////////////////////////
	// SNODE Group
	for(i = 0 ; i < MAX_SNODE_CNT ; i++){
		gpDB->SNode[i].ucSlvIdx = 0;				// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
		memset(gpDB->SNode[i].mEUI, 0, 8);			// 64-bit IEEE address
		memset(gpDB->SNode[i].szOID, 0, 40);		// Sensor OID
		memset(gpDB->SNode[i].szSNodeName, 0, 36);	// 센서노드 이름
	}

	////////////////////////////////////////////////////////////////////////////////////
	// PD Group
	gpDB->PdNode.wRptTime = 0;
	gpDB->PdNode.wMonPort = 8000;
	gpDB->PdNode.ucMode = YST_SCOPE;	// event
	gpDB->PdNode.ucFormat = F_COMPLE;	// adc format

	for(i=0; i<MAX_CH; i++)
	{
		gpDB->PdNode.ch_set[i].sync_sel = 0;			// External
		gpDB->PdNode.ch_set[i].sync_sel_val = 0;
		gpDB->PdNode.ch_set[i].ch_on = 1;
		gpDB->PdNode.ch_set[i].ch_gain = 35;
		gpDB->PdNode.ch_set[i].trig = 62;
		gpDB->PdNode.ch_set[i].filter_min = 1;
		gpDB->PdNode.ch_set[i].filter_max = 75;
		gpDB->PdNode.ch_set[i].SensorType = YST_CH13_UHF;
	}
}

static void GetDevInfoDBV1(YST_DB_V1 *pDB_V1)
{
	int i;

	////////////////////////////////////////////////////////////////////////////////////
	// General Group
	memcpy(gpDB->General.mIP, pDB_V1->General.mIP, 4);
	memcpy(gpDB->General.mIP2, pDB_V1->General.mIP2, 4);
	memcpy(gpDB->General.mSM, pDB_V1->General.mSM, 4);
	memcpy(gpDB->General.mGW, pDB_V1->General.mGW, 4);
	memcpy(gpDB->General.mMAC, pDB_V1->General.mEMAC, 6);		
	gpDB->General.ucDMRMode = pDB_V1->General.ucDMRMode;
	gpDB->General.ucGWIf = pDB_V1->General.ucGWIf;
	strcpy(gpDB->General.szPassword, pDB_V1->General.szPassword);
	gpDB->System.dwDebugFlag = pDB_V1->General.dwDebugFlag;
	gpDB->System.wAutoRebootMin = pDB_V1->General.wAutoRebootMin;
	memcpy(gpDB->General.mDNS, pDB_V1->General.mDNS, 4);
	gpDB->General.bNTP = pDB_V1->General.bNTP;
	gpDB->General.ucOperMode = pDB_V1->General.ucOperMode;
	gpDB->General.ucOperEth0 = pDB_V1->General.ucOperEth0;
	gpDB->General.ucOperEth1 = pDB_V1->General.ucOperEth1;
	gpDB->General.bPCommMode = pDB_V1->General.bPCommMode;
	gpDB->General.bTelnet = pDB_V1->General.bTelnet;
	gpDB->General.bFtp = pDB_V1->General.bFtp;
	gpDB->General.ucHMode = pDB_V1->General.ucHMode;
	gpDB->General.ucHChannel = pDB_V1->General.ucHChannel;
	gpDB->General.ucHMDHCP2 = pDB_V1->General.ucHMDHCP2;
	gpDB->General.ucHMDHCP3 = pDB_V1->General.ucHMDHCP3;
	memcpy(gpDB->General.mHMIP1, pDB_V1->General.mHMIP1, 4);
	memcpy(gpDB->General.mHMIP2, pDB_V1->General.mHMIP2, 4);
	memcpy(gpDB->General.mHMIP3, pDB_V1->General.mHMIP3, 4);
	gpDB->General.ucHMSEC1 = pDB_V1->General.ucHMSEC1;
	gpDB->General.ucHMSEC2 = pDB_V1->General.ucHMSEC2;
	gpDB->General.ucHMSEC3 = pDB_V1->General.ucHMSEC3;
	strcpy(gpDB->General.szHMSSID1, pDB_V1->General.szHMSSID1);
	strcpy(gpDB->General.szHMSSID2, pDB_V1->General.szHMSSID2);
	strcpy(gpDB->General.szHMSSID3, pDB_V1->General.szHMSSID3);

	////////////////////////////////////////////////////////////////////////////////////
	// Chassis Group
	strcpy(gpDB->Chassis.szSerialNo, pDB_V1->General.szSerialNo);
	strcpy(gpDB->Chassis.szPoleNo, pDB_V1->General.szPoleNo);
	strcpy(gpDB->Chassis.szMangeNo, pDB_V1->General.szMangeNo);
	strcpy(gpDB->Chassis.szInstallDate, pDB_V1->General.szInstallDate);

	////////////////////////////////////////////////////////////////////////////////////
	// NAT_PFWD Group
	for(i = 0 ; i < MAX_NAT_PFWD_CNT ; i++){
		gpDB->PFwd[i].ucProtocol = pDB_V1->PFwd[i].ucProtocol;
		gpDB->PFwd[i].nSrcPort = pDB_V1->PFwd[i].nSrcPort;
		gpDB->PFwd[i].nDestPort = pDB_V1->PFwd[i].nDestPort;
		memcpy(gpDB->PFwd[i].mPFWDIP, pDB_V1->PFwd[i].mPFWDIP, 4);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// ROUTE_TBL Group
	for(i = 0 ; i < MAX_ROUTE_TBL_CNT ; i++){
		gpDB->Route[i].ucUse = pDB_V1->Route[i].ucUse;
		memcpy(gpDB->Route[i].mRNet, pDB_V1->Route[i].mRNet, 4);
		memcpy(gpDB->Route[i].mRGW, pDB_V1->Route[i].mRGW, 4);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// PLATFORM Group
	gpDB->Platform.wRptTime = pDB_V1->Platform.wRptTime;
	// gpDB->Platform.bRegGW = pDB_V1->Platform.bRegGW;
	gpDB->Platform.bAssignOID = pDB_V1->Platform.bAssignOID;
	memcpy(gpDB->Platform.mMyIP, pDB_V1->Platform.mMyIP, 4);
	memcpy(gpDB->Platform.mPeerIP, pDB_V1->Platform.mPeerIP, 4);
	gpDB->Platform.wMyPort = pDB_V1->Platform.wMyPort;
	gpDB->Platform.wPeerPort = pDB_V1->Platform.wPeerPort;
	memcpy(gpDB->Platform.szGWOID, pDB_V1->Platform.szGWOID, 40);
	memcpy(gpDB->Platform.szURIPath, pDB_V1->Platform.szURIPath, 40);
	memcpy(gpDB->Platform.szOIDBase, pDB_V1->Platform.szOIDBase, 20);
	memcpy(gpDB->Platform.szCSEID, pDB_V1->Platform.szCSEID, 20);
	gpDB->Platform.ucM2MType = pDB_V1->Platform.ucM2MType;
	gpDB->Platform.ucM2MVer = pDB_V1->Platform.ucM2MVer;
	gpDB->Platform.wBlockSize = pDB_V1->Platform.wBlockSize;
	gpDB->Platform.bSec = pDB_V1->Platform.bSec;
	gpDB->Platform.bSecDebug = pDB_V1->Platform.bSecDebug;
	memcpy(gpDB->Platform.szSecPW, pDB_V1->Platform.szSecPW, 20);

	////////////////////////////////////////////////////////////////////////////////////
	// LORA Group
	gpDB->Lora.bUseEnDec = pDB_V1->Lora.bUseEnDec;
	gpDB->Lora.ucRFFreq = pDB_V1->Lora.ucRFFreq;
	gpDB->Lora.ucSPFactor = pDB_V1->Lora.ucSPFactor;
	strcpy(gpDB->Lora.szLoraKey, pDB_V1->Lora.szLoraKey);
	gpDB->Lora.ucTxp = pDB_V1->Lora.ucTxp;

	////////////////////////////////////////////////////////////////////////////////////
	// SCOMM Group
	for(i = 0 ; i < MAX_SCOMM_CNT ; i++){
		gpDB->SComm[i].wDelayTime = pDB_V1->SComm[i].wDelayTime;
		gpDB->SComm[i].wMaxPktSize = pDB_V1->SComm[i].wMaxPktSize;
		gpDB->SComm[i].ucPortID = pDB_V1->SComm[i].ucPortID;
		gpDB->SComm[i].ucBaudrate = pDB_V1->SComm[i].ucBaudrate;
		gpDB->SComm[i].ucDataBits = pDB_V1->SComm[i].ucDataBits;
		gpDB->SComm[i].ucStopBits = pDB_V1->SComm[i].ucStopBits;
		gpDB->SComm[i].ucParity = pDB_V1->SComm[i].ucParity;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// SPORT Group
		gpDB->SPort.ucDevCnt = pDB_V1->SPort.ucDevCnt;
		gpDB->SPort.wPollInt = pDB_V1->SPort.wPollInt;
		gpDB->SPort.wWaitTime = pDB_V1->SPort.wWaitTime;
		memset(gpDB->SPort.ucSSSNCnt, 1, MAX_485_DEV_CNT);

	////////////////////////////////////////////////////////////////////////////////////
	// SNODE Group
	for(i = 0 ; i < MAX_SNODE_CNT ; i++){
		//gpDB->SNode[i].bRegSNode = pDB_V1->SNode[i].bRegSNode;
		gpDB->SNode[i].bRegSNode = 0;// 재부팅후 무조건 다시 등록한다.
		gpDB->SNode[i].dwFRegTime = pDB_V1->SNode[i].dwFRegTime;
		memcpy(gpDB->SNode[i].mEUI, pDB_V1->SNode[i].mEUI, 8);
		memcpy(gpDB->SNode[i].szOID, pDB_V1->SNode[i].szOID, 40);
		memcpy(gpDB->SNode[i].szSNodeName, pDB_V1->SNode[i].szSNodeName, 36);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// PD Group
	gpDB->PdNode.wRptTime = pDB_V1->PdNode.wRptTime;
	gpDB->PdNode.wMonPort = pDB_V1->PdNode.wMonPort;
	gpDB->PdNode.ucMode = pDB_V1->PdNode.ucMode;
	gpDB->PdNode.ucFormat = pDB_V1->PdNode.ucFormat;

	for(i=0; i<MAX_CH; i++)
	{
		gpDB->PdNode.ch_set[i].sync_sel = pDB_V1->PdNode.ch_set[i].sync_sel;			// sync, External
		gpDB->PdNode.ch_set[i].sync_sel_val = pDB_V1->PdNode.ch_set[i].sync_sel_val;	
		gpDB->PdNode.ch_set[i].ch_on = pDB_V1->PdNode.ch_set[i].ch_on;	
		gpDB->PdNode.ch_set[i].ch_gain = pDB_V1->PdNode.ch_set[i].ch_gain;	
		gpDB->PdNode.ch_set[i].trig = pDB_V1->PdNode.ch_set[i].trig;	
		gpDB->PdNode.ch_set[i].filter_min = pDB_V1->PdNode.ch_set[i].filter_min;	
		gpDB->PdNode.ch_set[i].filter_max = pDB_V1->PdNode.ch_set[i].filter_max;	
		gpDB->PdNode.ch_set[i].SensorType = pDB_V1->PdNode.ch_set[i].SensorType;	
	}

	//
	gpDB->System.ucResetStatus = pDB_V1->ucResetStatus;
}

static void SetDevInfoDBV1(YST_DB_V1 *pDB_V1)
{
	int i;

	memset(pDB_V1, 0, sizeof(YST_DB_V1));
	pDB_V1->wVersion = 0x0001;
	
	////////////////////////////////////////////////////////////////////////////////////
	// General Group
	memcpy(pDB_V1->General.mIP, gpDB->General.mIP, 4);
	memcpy(pDB_V1->General.mIP2, gpDB->General.mIP2, 4);
	memcpy(pDB_V1->General.mSM, gpDB->General.mSM, 4);
	memcpy(pDB_V1->General.mGW, gpDB->General.mGW, 4);
	memcpy(pDB_V1->General.mEMAC, gpDB->General.mMAC, 6);
	
	pDB_V1->General.ucDMRMode = gpDB->General.ucDMRMode;
	pDB_V1->General.ucGWIf = gpDB->General.ucGWIf;

	strcpy(pDB_V1->General.szPassword, gpDB->General.szPassword);
	pDB_V1->General.dwDebugFlag = gpDB->System.dwDebugFlag;
	pDB_V1->General.wAutoRebootMin = gpDB->System.wAutoRebootMin;

	memcpy(pDB_V1->General.mDNS, gpDB->General.mDNS, 4);
	pDB_V1->General.bNTP = gpDB->General.bNTP;
	pDB_V1->General.ucOperMode = gpDB->General.ucOperMode;
	pDB_V1->General.ucOperEth0 = gpDB->General.ucOperEth0;
	pDB_V1->General.ucOperEth1 = gpDB->General.ucOperEth1;
	pDB_V1->General.bPCommMode = gpDB->General.bPCommMode;
	pDB_V1->General.bTelnet = gpDB->General.bTelnet;
	pDB_V1->General.bFtp = gpDB->General.bFtp;
	pDB_V1->General.ucHMode = gpDB->General.ucHMode;
	pDB_V1->General.ucHChannel = gpDB->General.ucHChannel;
	pDB_V1->General.ucHMDHCP2 = gpDB->General.ucHMDHCP2;
	pDB_V1->General.ucHMDHCP3 = gpDB->General.ucHMDHCP3;
	memcpy(pDB_V1->General.mHMIP1, gpDB->General.mHMIP1, 4);
	memcpy(pDB_V1->General.mHMIP2, gpDB->General.mHMIP2, 4);
	memcpy(pDB_V1->General.mHMIP3, gpDB->General.mHMIP3, 4);
	pDB_V1->General.ucHMSEC1 = gpDB->General.ucHMSEC1;
	pDB_V1->General.ucHMSEC2 = gpDB->General.ucHMSEC2;
	pDB_V1->General.ucHMSEC3 = gpDB->General.ucHMSEC3;
	strcpy(pDB_V1->General.szHMSSID1, gpDB->General.szHMSSID1);
	strcpy(pDB_V1->General.szHMSSID2, gpDB->General.szHMSSID2);
	strcpy(pDB_V1->General.szHMSSID3, gpDB->General.szHMSSID3);

	////////////////////////////////////////////////////////////////////////////////////
	// Chassis Group
	strcpy(pDB_V1->General.szSerialNo, gpDB->Chassis.szSerialNo);
	strcpy(pDB_V1->General.szPoleNo, gpDB->Chassis.szPoleNo);
	strcpy(pDB_V1->General.szMangeNo, gpDB->Chassis.szMangeNo);
	strcpy(pDB_V1->General.szInstallDate, gpDB->Chassis.szInstallDate);
	
	////////////////////////////////////////////////////////////////////////////////////
	// NAT_PFWD Group
	for(i = 0 ; i < MAX_NAT_PFWD_CNT ; i++){
		pDB_V1->PFwd[i].ucProtocol = gpDB->PFwd[i].ucProtocol;
		pDB_V1->PFwd[i].nSrcPort = gpDB->PFwd[i].nSrcPort;
		pDB_V1->PFwd[i].nDestPort = gpDB->PFwd[i].nDestPort;
		memcpy(pDB_V1->PFwd[i].mPFWDIP, gpDB->PFwd[i].mPFWDIP, 4);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// ROUTE_TBL Group
	for(i = 0 ; i < MAX_ROUTE_TBL_CNT ; i++){
		pDB_V1->Route[i].ucUse = gpDB->Route[i].ucUse;
		memcpy(pDB_V1->Route[i].mRNet, gpDB->Route[i].mRNet, 4);
		memcpy(pDB_V1->Route[i].mRGW, gpDB->Route[i].mRGW, 4);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// PLATFORM Group
	// pDB_V1->Platform.bRegGW = gpDB->Platform.bRegGW;
	pDB_V1->Platform.bAssignOID = gpDB->Platform.bAssignOID;
	pDB_V1->Platform.wRptTime = gpDB->Platform.wRptTime;
	memcpy(pDB_V1->Platform.mMyIP, gpDB->Platform.mMyIP, 4);
	memcpy(pDB_V1->Platform.mPeerIP, gpDB->Platform.mPeerIP, 4);
	pDB_V1->Platform.wMyPort = gpDB->Platform.wMyPort;
	pDB_V1->Platform.wPeerPort = gpDB->Platform.wPeerPort;
	memcpy(pDB_V1->Platform.szGWOID, gpDB->Platform.szGWOID, 40);
	memcpy(pDB_V1->Platform.szURIPath, gpDB->Platform.szURIPath, 40);
	memcpy(pDB_V1->Platform.szOIDBase, gpDB->Platform.szOIDBase, 20);
	memcpy(pDB_V1->Platform.szCSEID, gpDB->Platform.szCSEID, 20);
	pDB_V1->Platform.ucM2MType = gpDB->Platform.ucM2MType;
	pDB_V1->Platform.ucM2MVer = gpDB->Platform.ucM2MVer;
	pDB_V1->Platform.wBlockSize = gpDB->Platform.wBlockSize;
	pDB_V1->Platform.bSec = gpDB->Platform.bSec;
	pDB_V1->Platform.bSecDebug = gpDB->Platform.bSecDebug;
	memcpy(pDB_V1->Platform.szSecPW, gpDB->Platform.szSecPW, 20);

	////////////////////////////////////////////////////////////////////////////////////
	// LORA Group
	pDB_V1->Lora.bUseEnDec = gpDB->Lora.bUseEnDec;
	pDB_V1->Lora.ucRFFreq = gpDB->Lora.ucRFFreq;
	pDB_V1->Lora.ucSPFactor = gpDB->Lora.ucSPFactor;
	strcpy(pDB_V1->Lora.szLoraKey, gpDB->Lora.szLoraKey);
	pDB_V1->Lora.ucTxp = gpDB->Lora.ucTxp;

	////////////////////////////////////////////////////////////////////////////////////
	// SCOMM Group
	for(i = 0 ; i < MAX_SCOMM_CNT ; i++){
		pDB_V1->SComm[i].wDelayTime = gpDB->SComm[i].wDelayTime;
		pDB_V1->SComm[i].wMaxPktSize = gpDB->SComm[i].wMaxPktSize;
		pDB_V1->SComm[i].ucPortID = gpDB->SComm[i].ucPortID;
		pDB_V1->SComm[i].ucBaudrate = gpDB->SComm[i].ucBaudrate;
		pDB_V1->SComm[i].ucDataBits = gpDB->SComm[i].ucDataBits;
		pDB_V1->SComm[i].ucStopBits = gpDB->SComm[i].ucStopBits;
		pDB_V1->SComm[i].ucParity = gpDB->SComm[i].ucParity;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// SPORT Group
		pDB_V1->SPort.ucDevCnt = gpDB->SPort.ucDevCnt;
		pDB_V1->SPort.wPollInt = gpDB->SPort.wPollInt;
		pDB_V1->SPort.wWaitTime = gpDB->SPort.wWaitTime;

	////////////////////////////////////////////////////////////////////////////////////
	// SNODE Group
	for(i = 0 ; i < MAX_SNODE_CNT ; i++){
		pDB_V1->SNode[i].bRegSNode = gpDB->SNode[i].bRegSNode;
		pDB_V1->SNode[i].dwFRegTime = gpDB->SNode[i].dwFRegTime;
		memcpy(pDB_V1->SNode[i].mEUI, gpDB->SNode[i].mEUI, 8);
		memcpy(pDB_V1->SNode[i].szOID, gpDB->SNode[i].szOID, 40);
		memcpy(pDB_V1->SNode[i].szSNodeName, gpDB->SNode[i].szSNodeName, 36);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// PD Group
	pDB_V1->PdNode.wRptTime = gpDB->PdNode.wRptTime;
	pDB_V1->PdNode.wMonPort = gpDB->PdNode.wMonPort;
	pDB_V1->PdNode.ucMode = gpDB->PdNode.ucMode;
	pDB_V1->PdNode.ucFormat = gpDB->PdNode.ucFormat;

	for(i=0; i<MAX_CH; i++)
	{
		pDB_V1->PdNode.ch_set[i].sync_sel = gpDB->PdNode.ch_set[i].sync_sel;			// sync, External
		pDB_V1->PdNode.ch_set[i].sync_sel_val = gpDB->PdNode.ch_set[i].sync_sel_val;	
		pDB_V1->PdNode.ch_set[i].ch_on = gpDB->PdNode.ch_set[i].ch_on;	
		pDB_V1->PdNode.ch_set[i].ch_gain = gpDB->PdNode.ch_set[i].ch_gain;	
		pDB_V1->PdNode.ch_set[i].trig = gpDB->PdNode.ch_set[i].trig;	
		pDB_V1->PdNode.ch_set[i].filter_min = gpDB->PdNode.ch_set[i].filter_min;	
		pDB_V1->PdNode.ch_set[i].filter_max = gpDB->PdNode.ch_set[i].filter_max;	
		pDB_V1->PdNode.ch_set[i].SensorType = gpDB->PdNode.ch_set[i].SensorType;	
	}

	//
	pDB_V1->ucResetStatus = gpDB->System.ucResetStatus;
	pDB_V1->dwCRCValue = GetCRC32Value((BYTE *)pDB_V1, sizeof(YST_DB_V1) - sizeof(DWORD));
}

BOOL ReadDeviceInfoDBFile(BYTE *Data, int nSize)
{
	FILE *fp;

	if((fp = fopen(DEVICEINFO_FLASH_FILE, "rb")) == NULL) return FALSE;

	if(fread(Data, nSize, 1, fp) != 1){
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

BOOL WriteDeviceInfoDBFile(BYTE *Data, int nSize)
{
	FILE *fp;

	if((fp = fopen(DEVICEINFO_FLASH_FILE, "wb")) == NULL) return FALSE;

	fwrite(Data, nSize, 1, fp);
	fclose(fp);
	return TRUE;
}

int ReadDeviceInfoDB_V1(void)
{
	YST_DB_V1 DB_V1;
	DWORD dwCRCValue;

	GeneralLogMsg("Device Info DB - Read V1 from FILE(%d Bytes)", sizeof(DB_V1));
	if(!ReadDeviceInfoDBFile((BYTE *)&DB_V1, sizeof(DB_V1))){
		PrintGeneralLogMsg("Device Info DB V1 - Read Error From FILE !");
		return -1;
	}

	//PrintDbgData(&DB_V1, sizeof(DB_V1), stdout);
	if(DB_V1.wVersion != YDEF_USING_DB_VERSION){
		if(DB_V1.wVersion == 0 || DB_V1.wVersion > YDEF_USING_DB_VERSION){
			PrintGeneralLogMsg("Device Info DB V1 - Read Info Incorrect !");
			return -1;
		}
		else return DB_V1.wVersion;
	}
	
	dwCRCValue = GetCRC32Value((BYTE *)&DB_V1, sizeof(DB_V1) - sizeof(DWORD));
	if(DB_V1.dwCRCValue != dwCRCValue){
		PrintGeneralLogMsg("Device Info DB V1 - Read CRC(%08X) Err, Calc=%08X !", DB_V1.dwCRCValue, dwCRCValue);
		if(DB_V1.wVersion == 0) return -1;
		else return DB_V1.wVersion;
	}

	GetDevInfoDBV1(&DB_V1);
	GeneralLogMsg("Device Info DB - Read OK(%d bytes)", sizeof(DB_V1));
	return 0;
}

void WriteDeviceInfoDB_V1(void)
{
	YST_DB_V1 DB_V1;

	SetDevInfoDBV1(&DB_V1);
	GeneralLogMsg("Device Info DB - Write V1 to File, CRC=%08X", DB_V1.dwCRCValue);
	if(!WriteDeviceInfoDBFile((BYTE *)&DB_V1, sizeof(DB_V1))) return;

	GeneralLogMsg("Device Info DB - Write OK(%d bytes)", sizeof(DB_V1));
}

int ReadDeviceInfoOldDBFromDisk(DWORD dwDBVersion)
{
	if(dwDBVersion >= YDEF_USING_DB_VERSION) return -1;
	
	switch(dwDBVersion){
	case 0x01 : return ReadDeviceInfoDB_V1();
	default : return -1;
	}
}

void WriteDeviceInfoDBToDisk(void)
{
	WriteDeviceInfoDB_V1();
}

// 저장된 Log 데이터 읽기.
static void Readlog(void)
{	
	ReadLogDBFromDisk();		// log
	ReadSDataLogFromDisk();	// sdata
}

void ReadDeviceInfoDBFromDisk(void)
{
	int nResult;
	
	memset(gpDB, 0, sizeof(YST_DB_MAIN));

	Readlog();

	nResult = ReadDeviceInfoDB_V1();
	if(nResult == -1 || nResult == -2){
		ResetDeviceInfoDB((nResult == -1)?TRUE:FALSE);
		WriteDeviceInfoDB_V1();
	}
	else if(nResult != 0){
		if(ReadDeviceInfoOldDBFromDisk(nResult) != 0){
			ResetDeviceInfoDB(TRUE);
		}
		WriteDeviceInfoDB_V1();
	}
}
