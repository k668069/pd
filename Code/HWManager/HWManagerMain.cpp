/////////////////////////////////////////////////////////// 
// HWManager.cpp
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>

#include "DeviceInfoDB.h"
#include "HWControl.h"
#include "LogMessage.h"
#include "IPCSharedMemory.h"
#include "DBFileManager.h"
#include "TimeApp.h"
#include "StringApp.h"
#include "CommonApp.h"
#include "Network.h"
#include "I2CApp.h"
#include "RingMACComm.h"

YST_DB_MAIN	gDBOld;
YST_DB_MAIN	*gpDB = NULL;	//DeviceInfoDB.h
YST_IPC		*gpIPC = NULL;	//IPCSharedMemory.h
YST_DB_LOG	*gpLog = NULL;	//LogMessage.h

 //SCommAgentMain.cpp
extern void InitializeSCommModule(int nMIdx);
extern void InitializeSPortMemory(int nPIdx);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// On Case RequestSet Information
static void OnRequestSetMACAddress(BYTE mMAC[])
{
	if(memcmp(gpDB->General.mMAC, mMAC, 6) == 0) return;
	memcpy(gpDB->General.mMAC, mMAC, 6);
	GeneralLogMsg("The MAC-address is changed to %s.", MakeMACAddrString(gpDB->General.mMAC));
}

static void OnRequestSetIPAddress(BYTE mIP[])
{
	if(memcmp(gpDB->General.mIP, mIP, 4) == 0) return;
	memcpy(gpDB->General.mIP, mIP, 4);
	memcpy(gpDB->Platform.mMyIP, mIP, 4);
	GeneralLogMsg("The IP-address is changed to %s.", MakeIPAddrString(gpDB->General.mIP));
	SetNetworkConfig(gpDB->General.mIP, gpDB->General.mSM);
}

static void OnRequestSetIP2Address(BYTE mIP[])
{
	if(memcmp(gpDB->General.mIP2, mIP, 4) == 0) return;
	memcpy(gpDB->General.mIP2, mIP, 4);
	GeneralLogMsg("The IP2-address is changed to %s.", MakeIPAddrString(gpDB->General.mIP2));
	SetNetworkConfig2(gpDB->General.mIP2);
}

static void OnRequestSetSubnetMask(BYTE mSM[])
{
	if(memcmp(gpDB->General.mSM, mSM, 4) == 0) return;
	memcpy(gpDB->General.mSM, mSM, 4);
	GeneralLogMsg("The subnet mask is changed to %s.", MakeIPAddrString(gpDB->General.mSM));
	SetNetworkConfig(gpDB->General.mIP, gpDB->General.mSM);
}

static void OnRequestSetGateWay(BYTE mGW[])
{
	if(memcmp(gpDB->General.mGW, mGW, 4) == 0) return;
	memcpy(gpDB->General.mGW, mGW, 4);
	GeneralLogMsg("The network gateway is changed to %s.", MakeIPAddrString(gpDB->General.mGW));
	SetNetworkGateway(gpDB->General.mGW);
}

static void OnRequestSetSerialNo(char szSerialNo[])
{
	szSerialNo[MAX_DESC_STR] = 0;

	if(strcmp(gpDB->Chassis.szSerialNo, szSerialNo) == 0) return;
	strcpy(gpDB->Chassis.szSerialNo, szSerialNo);
	GeneralLogMsg("The serial no is changed to [%s].", gpDB->Chassis.szSerialNo);
}

static void OnRequestSetPoleNo(char szPoleNo[])
{
	szPoleNo[MAX_DESC_STR] = 0;
	if(strcmp(gpDB->Chassis.szPoleNo, szPoleNo) == 0) return;
	strcpy(gpDB->Chassis.szPoleNo, szPoleNo);
	GeneralLogMsg("The Poll no is changed to %s.", szPoleNo);
}

static void OnRequestSetMangeNo(char szMangeNo[])
{
	szMangeNo[MAX_DESC_STR] = 0;
	if(strcmp(gpDB->Chassis.szMangeNo, szMangeNo) == 0) return;
	strcpy(gpDB->Chassis.szMangeNo, szMangeNo);
	GeneralLogMsg("The Manage no is changed to %s.", szMangeNo);
}

static void OnRequestSetInstallDate(char szInstallDate[])
{
	szInstallDate[MAX_DESC_STR] = 0;
	if(strcmp(gpDB->Chassis.szInstallDate, szInstallDate) == 0) return;
	strcpy(gpDB->Chassis.szInstallDate, szInstallDate);
	GeneralLogMsg("The Install date is changed to %s.", szInstallDate);
}

static void OnRequestSetRMACAddress(BYTE mRMAC[])
{
	if(memcmp(gpDB->RMac.mRMAC, mRMAC, 6) == 0) return;
	memcpy(gpDB->RMac.mRMAC, mRMAC, 6);
	GeneralLogMsg("The Ring-MAC is changed to %s.", MakeMACAddrString(gpDB->RMac.mRMAC));
}

static void OnRequestSetRMacNodeName(char szNodeName[])
{
	szNodeName[32] = 0;
	if(strcmp(gpDB->RMac.szNodeName, szNodeName) == 0) return;
	strcpy(gpDB->RMac.szNodeName, szNodeName);
	GeneralLogMsg("The RingMAC Node Name is changed to %s.", szNodeName);
}

static void OnRequestSetPlatformMyIP(BYTE mMyIP[])
{
	if(memcmp(gpDB->Platform.mMyIP, mMyIP, 4) == 0) return;
	memcpy(gpDB->Platform.mMyIP, mMyIP, 4);
	GeneralLogMsg("The MyIP(Platform) is changed to %s.", MakeIPAddrString(gpDB->Platform.mMyIP));
}

static void OnRequestSetPlatformPeerIP(BYTE mPeerIP[])
{
	if(memcmp(gpDB->Platform.mPeerIP, mPeerIP, 4) == 0) return;
	memcpy(gpDB->Platform.mPeerIP, mPeerIP, 4);
	GeneralLogMsg("The Platform IP is changed to %s.", MakeIPAddrString(gpDB->Platform.mPeerIP));
	for(int i = 0 ; i < MAX_SNODE_CNT ; i++){
		gpDB->SNode[i].bRegSNode = 0;// 무조건 다시 등록한다.
	}
}

static void OnRequestSetPlatformMyPort(WORD wMyPort)
{
	gpDB->Platform.wMyPort = wMyPort;
	GeneralLogMsg("The MyPort(Platform) is changed to [%d].", wMyPort);
}

static void OnRequestSetPlatformPeerPort(WORD wPeerPort)
{
	gpDB->Platform.wPeerPort = wPeerPort;
	GeneralLogMsg("The PeerPort(Platform) is changed to [%d].", wPeerPort);
}

static void OnRequestSetPlatformGWOID(char szGWOID[])
{
	szGWOID[39] = 0;
	if(strcmp(gpDB->Platform.szGWOID, szGWOID) == 0) return;
	strcpy(gpDB->Platform.szGWOID, szGWOID);
	gpDB->Platform.bAssignOID = gpDB->Platform.bChangedOID = 1;
	gpDB->Platform.bRegGW = 0;
	GeneralLogMsg("The G/W OID is changed to %s.", szGWOID);
}

static void OnRequestSetPlatformURI(char szURI[])
{
	szURI[39] = 0;
	if(strcmp(gpDB->Platform.szURIPath, szURI) == 0) return;
	strcpy(gpDB->Platform.szURIPath, szURI);
	GeneralLogMsg("The URI Path of Platform is changed to %s.", szURI);
}

static void OnRequestSetM2MType(BYTE ucM2MType)
{
	gpDB->Platform.ucM2MType = ucM2MType;
	GeneralLogMsg("The M2M Type is changed to [%s].", ucM2MType ? "LWM2M" : "oneM2M");
}

static void OnRequestSetM2MVersion(BYTE ucM2MVer)
{
	gpDB->Platform.ucM2MVer = ucM2MVer;
	GeneralLogMsg("The OneM2M Version is changed to [%d].", ucM2MVer+1);
}

static void OnRequestSetPlatformRptTime(WORD wRptTime)
{
	gpDB->Platform.wRptTime = wRptTime;
	if(gpDB->Platform.wRptTime < 20) gpDB->Platform.wRptTime = 20;
	GeneralLogMsg("The Rpt Period(Platform) is changed to [%d].", wRptTime);
}

static void OnRequestSetWisunRFChannel(BYTE ucRFChan)
{
	if(ucRFChan > 31) {
		GeneralLogMsg("Invalid RF Freq for WiSUN : %d.", ucRFChan);
		return;
	}
	gpDB->Wisun.ucRFChan = ucRFChan;
	GeneralLogMsg("The RF Freq for WiSUN is changed to %d.", ucRFChan);
}

static void OnRequestSetWisunPanID(WORD wPanID)
{
	gpDB->Wisun.wPanID = wPanID;
	GeneralLogMsg("The PAN-ID for WiSUN is changed to %d.", wPanID);
}

static void OnRequestSetLoraRFFreq(BYTE ucRFFreq)
{
	if(ucRFFreq > 15) {
		GeneralLogMsg("Invalid RF Freq for LoRA : %d.", ucRFFreq);
		return;
	}
	gpDB->LoraM.ucRFFreq = ucRFFreq;
	GeneralLogMsg("The RF Freq for LoRA is changed to %d.", ucRFFreq);
}

static void OnRequestSetLoraChBW(BYTE ucChBW)
{
	if(ucChBW > 2) {
		GeneralLogMsg("Invalid Channel BW for LoRA : %d.", ucChBW);
		return;
	}
	gpDB->LoraM.ucChBW = ucChBW;
	GeneralLogMsg("The Channel BW for LoRA is changed to %d.", ucChBW);
}
static void OnRequestSetLoraSPFactor(BYTE ucSPFactor)
{
	if(ucSPFactor < 7 || ucSPFactor > 12) {
		GeneralLogMsg("Invalid Spreading Factor for LoRA : %d.", ucSPFactor);
		return;
	}
	gpDB->LoraM.ucSPFactor = ucSPFactor;
	GeneralLogMsg("The Spreading Factor for LoRA is changed to %d.", ucSPFactor);
}

static void OnRequestSetLoraCodeRate(BYTE ucCodeRate)
{
	if(ucCodeRate < 1 || ucCodeRate > 4) {
		GeneralLogMsg("Invalid Coding Rate for LoRA : %d.", ucCodeRate);
		return;
	}
	gpDB->LoraM.ucCodeRate = ucCodeRate;
	GeneralLogMsg("The Coding Rate for LoRA is changed to %d.", ucCodeRate);
}

static void OnRequestSetLoraPanID(WORD wPanID)
{
	gpDB->LoraM.wPanID = wPanID;
	GeneralLogMsg("The PAN-ID for LoRA is changed to %d.", wPanID);
}

static void OnRequestSetLoraUseCrypto(BYTE ucEnDecType)
{
	gpDB->LoraM.ucEnDecType = ucEnDecType;
	GeneralLogMsg("The Crypto for LoRA is changed to %d.", ucEnDecType);
}

static void OnRequestSetSCommBaudrate(BYTE ucIdx, BYTE ucBaudrate)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].ucBaudrate = ucBaudrate;
	GeneralLogMsg("The Baudrate for SComm-%d is changed to %d.", ucIdx+1, ucBaudrate);
}

static void OnRequestSetSCommDataBits(BYTE ucIdx, BYTE ucDataBits)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].ucDataBits = ucDataBits;
	GeneralLogMsg("The DataBits for SComm-%d is changed to %d.", ucIdx+1, ucDataBits);
}

static void OnRequestSetSCommStopBits(BYTE ucIdx, BYTE ucStopBits)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].ucStopBits = ucStopBits;
	GeneralLogMsg("The StopBits for SComm-%d is changed to %d.", ucIdx+1, ucStopBits);
}

static void OnRequestSetSCommParity(BYTE ucIdx, BYTE ucParity)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].ucParity = ucParity;
	GeneralLogMsg("The Parity for SComm-%d is changed to %d.", ucIdx+1, ucParity);
}

static void OnRequestSetSCommDelayTime(BYTE ucIdx, WORD wDelayTime)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].wDelayTime = wDelayTime;
	GeneralLogMsg("The DelayTime for SComm-%d is changed to %d.", ucIdx+1, wDelayTime);
}

static void OnRequestSetSCommMaxPktSize(BYTE ucIdx, WORD wMaxPktSize)
{
	if(ucIdx >= MAX_SCOMM_CNT) return;
	gpDB->SComm[ucIdx].wMaxPktSize = wMaxPktSize;
	GeneralLogMsg("The MaxPktSize for SComm-%d is changed to %d.", ucIdx+1, wMaxPktSize);
}

static void OnRequestSetSNodeName(BYTE ucIdx, char szSNodeName[])
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	szSNodeName[35] = 0;
	if(strcmp(gpDB->SNode[ucIdx].szSNodeName, szSNodeName) == 0) return;
	strcpy(gpDB->SNode[ucIdx].szSNodeName, szSNodeName);
	GeneralLogMsg("The Name for SNode-%d is changed to %s.", ucIdx+1, szSNodeName);
}

void RegisterNewSensorToPlatform(void);//M2MAgentMain.cpp
static void OnRequestSetSOID(BYTE ucIdx, char szOID[])
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	szOID[39] = 0;
	if(strcmp(gpDB->SNode[ucIdx].szOID, szOID) == 0) return;
	if(memcmp(szOID, gpDB->Platform.szOIDBase, strlen(gpDB->Platform.szOIDBase))) {
		strcpy(gpDB->SNode[ucIdx].szOID, gpDB->Platform.szOIDBase);
		strcat(gpDB->SNode[ucIdx].szOID, szOID);
	}
	else strcpy(gpDB->SNode[ucIdx].szOID, szOID);
	GeneralLogMsg("OID of SNode-%d => %s.", ucIdx+1, szOID);
	gpDB->SNode[ucIdx].dwFRegTime = GetCurrentMilliSecond();
//	RegisterNewSensorToPlatform();

	// URSNODE에 등록된 내용을 삭제한다.
	for(int i = 0 ; i < MAX_URSNODE_CNT ; i++){
		if(!memcmp(gpDB->URSNode[i].mEUI, "\x0\x0\x0\x0\x0\x0\x0\x0", 8)) break;
		if(strcmp(gpDB->URSNode[i].szOID, szOID)) continue;
		memset(gpDB->URSNode+i, 0, sizeof(YST_DB_URSNODE));
		for(int j = i+1 ; j < MAX_URSNODE_CNT ; j++){
			if(!memcmp(gpDB->URSNode[j].mEUI, "\x0\x0\x0\x0\x0\x0\x0\x0", 8)) break;
			memcpy(gpDB->URSNode+j-1, gpDB->URSNode+j, sizeof(YST_DB_URSNODE));
		}
	}
}

static void OnRequestSetSNodeReboot(BYTE ucIdx)
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	gpDB->SNode[ucIdx].bCtrlReboot = TRUE;
	GeneralLogMsg("The Reboot Command for SNode-%d.", ucIdx+1);
}

static BYTE SetSensorCfg(char szSCNFG[][30], const char szLabel[], int nValue)
{
	for(int i = 0 ; i < MAX_SNR_CFG_CNT ; i++){
		if(!szSCNFG[i][0]) return FALSE;
		if(memcmp(szSCNFG[i], szLabel, 2)) continue;
		sprintf(szSCNFG[i]+2, "%d", nValue);
		return TRUE;
	}
	return FALSE;
}

static BYTE SetSensorCfg2(char szSCNFG[][30], char szText[])
{
	for(int i = 0 ; i < MAX_SNR_CFG_CNT ; i++){
		if(!szSCNFG[i][0]) return FALSE;
		if(memcmp(szSCNFG[i], szText, 2)) continue;
		strcpy(szSCNFG[i]+2, szText+2);
		return TRUE;
	}
	return FALSE;
}

static void OnRequestSetSNodeRptTime(BYTE ucIdx, WORD wRptTime)
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	gpDB->SNode[ucIdx].bCtrlPeriod = TRUE;
	gpDB->SNode[ucIdx].wRptTime = wRptTime;
	SetSensorCfg(gpDB->SNode[ucIdx].szSCNFG, "PR", wRptTime);
	GeneralLogMsg("The RptTime for SNode-%d is changed to %d.", ucIdx+1, wRptTime);
}

void SetSensorCtrlUDP(YST_DB_SNODE *pNode);//SCommAgentMain.cpp
static void OnRequestSetSNodeConfig(BYTE ucIdx, char szText[])
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	gpDB->SNode[ucIdx].bCtrlCfg = TRUE;

	SetSensorCfg2(gpDB->SNode[ucIdx].szSCNFG, szText);
	GeneralLogMsg("The Sensor Cfg-%c%c for SNode-%d is changed to %s.", szText[0], szText[1], ucIdx+1, szText+2);
//	if(gpDB->SNode[ucIdx].ucPIdx == UDP_PORT_IDX+1) SetSensorCtrlUDP(gpDB->SNode+ucIdx);
}

static void OnRequestSetSNodeDestGW(BYTE ucIdx, char szText[])
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	gpDB->SNode[ucIdx].bCtrlDst = TRUE;
	strcpy(gpDB->SNode[ucIdx].szDestGW, szText);
	GeneralLogMsg("The Sensor Dest G/W for SNode-%d is changed to %s.", ucIdx+1, szText);
}

static void OnRequestSetSNodeTimeSync(BYTE ucIdx)
{
	if(ucIdx >= MAX_SNODE_CNT) return;
	gpDB->SNode[ucIdx].bCtrlTime = TRUE;
	GeneralLogMsg("The Sensor Time Sync for SNode-%d", ucIdx+1);
}

static void OnRequestSetResetSystem(void)
{
	gpDB->System.ucResetStatus = 1;
	WriteDeviceInfoDBToDisk();
	PrintGeneralLogMsg("Reboot the system.");
//	WriteLogDBToDisk();
	HWCtrlRebootSystem();
}

static void OnRequestSetCurrentDateTime(char strDateTime[])
{
	struct timeval TimeValSet;
	struct tm *TmDate = GetDateTimeFromStr(strDateTime);
	TmDate->tm_year -= 1900;
	TmDate->tm_mon -= 1;
	TimeValSet.tv_sec = mktime(TmDate);
	TimeValSet.tv_usec = 0;
	settimeofday(&TimeValSet, NULL);
	system("hwclock  --systohc");
}

static void OnRequestSetResetDB(void)
{
	PrintGeneralLogMsg("Initialize the DB.");
	ResetDeviceInfoDB(FALSE);
	OnRequestSetResetSystem(YET_RESET_WARM);
}

// void ResetEnv(void);
static void OnRequestSetResetEnv(void)
{
	PrintGeneralLogMsg("Initialize the Env.");
	ResetEnv();
	OnRequestSetResetSystem(YET_RESET_WARM);
}

static void OnRequestSetResetStatus(void)
{
	gpDB->System.ucResetStatus = 0;
	WriteDeviceInfoDBToDisk();
}
// On Case RequestSet Information
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void OnRequestDelSNode(int nIdx)
{
	if(nIdx >= MAX_SNODE_CNT) return;
	GeneralLogMsg("The SNode [%s] is deleted.", gpDB->SNode[nIdx].szOID);
	memset(gpDB->SNode+nIdx, 0, sizeof(YST_DB_SNODE));
	for(int i = nIdx ; i < MAX_SNODE_CNT-1 ; i++){
		memcpy(gpDB->SNode+i, gpDB->SNode+i+1, sizeof(YST_DB_SNODE));
		if(gpDB->SNode[i].szOID[0] == 0) break;
	}
}

static void OnRequestDelSNodeAll(void)
{
	for(int i = MAX_SNODE_CNT-1 ; i >= 0 ; i--){
		if(gpDB->SNode[i].szOID[0] == 0) continue;
		if(!CheckSNodeDuplication(i) && (gpDB->SNode[i].bCommFail == 0)) continue;
		GeneralLogMsg("The SNode [%s] is deleted.", gpDB->SNode[i].szOID);
		memset(gpDB->SNode+i, 0, sizeof(YST_DB_SNODE));
		for(int j = i ; j < MAX_SNODE_CNT-1 ; j++){
			memcpy(gpDB->SNode+j, gpDB->SNode+j+1, sizeof(YST_DB_SNODE));
			if(gpDB->SNode[j+1].szOID[0] == 0) break;
		}
	}
}

static void OnRequestDelURSNode(int nIdx)
{
	if(nIdx >= MAX_URSNODE_CNT) return;
	GeneralLogMsg("The URSNode-%d is deleted.", nIdx+1);
	memset(gpDB->URSNode+nIdx, 0, sizeof(YST_DB_URSNODE));
	for(int i = nIdx ; i < MAX_URSNODE_CNT-1 ; i++){
		memcpy(gpDB->URSNode+i, gpDB->URSNode+i+1, sizeof(YST_DB_URSNODE));
		if(!memcmp(gpDB->URSNode[i].mEUI, "\x0\x0\x0\x0\x0\x0\x0\x0", 8)) break;
	}
}
// On Case RequestSet Information
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Process Set Parameter Message
static BOOL OnSetParameterGeneralGroup(void *DataBuf, BYTE ucOID, int nIdx)
{
	BOOL bSaveConfig = TRUE;
	GeneralObjData* pstData = (GeneralObjData*)DataBuf;

	switch(ucOID){
	case YET_OID_IP :
		OnRequestSetIPAddress(pstData->mIP);
		break;
	case YET_OID_IP2 :
		OnRequestSetIP2Address(pstData->mIP);
		break;
	case YET_OID_SM :
		OnRequestSetSubnetMask(pstData->mSM);
		break;
	case YET_OID_GW :
		OnRequestSetGateWay(pstData->mGW);
		break;
	case YET_OID_MAC : 
		OnRequestSetMACAddress(pstData->mMAC);
		break;
	case YET_OID_Password :
		OnRequestSetPassword(pstData->szPassword);
		break;
	case YET_OID_DNS :
		OnRequestSetDNS(pstData->mDNS);
		break;
	case YET_OID_NTP :
		OnRequestSetNTP(pstData->bNTP, 1);
		break;
	case YET_OID_OperMode :
		OnRequestSetOperMode(pstData->ucOperMode);
		break;
	case YET_OID_OperEth :
		if(nIdx == 0)
			OnRequestSetOperEth(nIdx, pstData->ucOperEth0);
		else
			OnRequestSetOperEth(nIdx, pstData->ucOperEth1);
		break;
	case YET_OID_PComm :
		OnRequestSetPComm(pstData->bPCommMode);
		break;
	case YET_OID_TELNET :
		OnRequestSetTelnet(pstData->bTelnet, 1);
		break;
	case YET_OID_FTP :
		OnRequestSetFtp(pstData->bFtp, 1);
		break;

	case YET_OID_DMRMode :
		OnRequestSetDMRMode(pstData->ucDMRMode);
		break;
	case YET_OID_GWIf :
		OnRequestSetGWIf(pstData->ucGWIf);
		break;
	case YET_OID_HMode :
		OnRequestSetHMode(pstData->ucHMode);
		break;
	case YET_OID_HCH :
		OnRequestSetHChannel(pstData->ucHChannel);
		break;
	case YET_OID_HMDHCP2 :
		OnRequestSetHMDHCP2(pstData->ucHMDHCP2);
		break;
	case YET_OID_HMDHCP3 :
		OnRequestSetHMDHCP3(pstData->ucHMDHCP3);
		break;
	case YET_OID_HMIP1 :
		OnRequestSetHMIP1(pstData->mHMIP1);
		break;
	case YET_OID_HMIP2 :
		OnRequestSetHMIP2(pstData->mHMIP2);
		break;
	case YET_OID_HMIP3 :
		OnRequestSetHMIP3(pstData->mHMIP3);
		break;
	case YET_OID_HMSEC1 :
		OnRequestSetHMSEC1(pstData->ucHMSEC1);
		break;
	case YET_OID_HMSEC2 :
		OnRequestSetHMSEC2(pstData->ucHMSEC2);
		break;
	case YET_OID_HMSEC3 :
		OnRequestSetHMSEC3(pstData->ucHMSEC3);
		break;
	case YET_OID_HMSSID1 :
		OnRequestSetHMSSID1(pstData->szHMSSID1);
		break;
	case YET_OID_HMSSID2 :
		OnRequestSetHMSSID2(pstData->szHMSSID2);
		break;
	case YET_OID_HMSSID3 :
		OnRequestSetHMSSID3(pstData->szHMSSID3);
		break;
	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterChassisGroup(void *DataBuf, BYTE ucOID)
{
	BOOL bSaveConfig = TRUE;
	ChassisObjData* pstData = (ChassisObjData*)DataBuf;

	switch(ucOID){
		case YET_OID_ChSerialNo :
			OnRequestSetSerialNo(pstData->szSerialNo);
			break;
		case YET_OID_PoleNo :
			OnRequestSetPoleNo(pstData->szPoleNo);
			break;
		case YET_OID_MangeNo :
			OnRequestSetMangeNo(pstData->szMangeNo);
			break;
		case YET_OID_InstallDate :
			OnRequestSetInstallDate(pstData->szInstallDate);
			break;
		default : bSaveConfig=FALSE;
			break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterPortForwardGroup(void *DataBuf, int nOID, int nIdx)
{
	BOOL bSaveConfig = TRUE;
	NATPFwdObjData* pstData = (NATPFwdObjData*)DataBuf;

	switch(nOID){
	case YET_OID_PF_IPAddr : 
		OnRequestSetPFIPAddr(pstData->mIP, nIdx);
		break;
	case YET_OID_PF_SrcPort : 
		OnRequestSetPFSrcPort(pstData->nSrcPort, nIdx);
		break;
	case YET_OID_PF_DestPort : 
		OnRequestSetPFDestPort(pstData->nDestPort, nIdx);
		break;
	case YET_OID_PF_Protocol : 
		OnRequestSetPFProtocol(pstData->ucProtocol, nIdx);
		break;
	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterRouteTblGroup(void *DataBuf, int nOID, int nIdx)
{
	BOOL bSaveConfig = TRUE;
	RouteTblObjData* pstData = (RouteTblObjData*)DataBuf;

	switch(nOID){
	case YET_OID_RTT_Use : 
		OnRequestSetRouteUse(pstData->ucUse, nIdx);
		break;
	case YET_OID_RTT_RNet : 
		OnRequestSetRouteNetwork(pstData->mRNet, nIdx);
		break;
	case YET_OID_RTT_RGW : 
		OnRequestSetRouteGateway(pstData->mRNet, nIdx);
		break;
	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterRMacGroup(void *DataBuf, BYTE ucOID)
{
	BOOL bSaveConfig = TRUE;
	RMacObjData* pstData = (RMacObjData*)DataBuf;

	switch(ucOID){

	case YET_OID_RMacRMAC :
		OnRequestSetRMACAddress(pstData->mRMAC);
		break;
	case YET_OID_RMacNodeName :
		OnRequestSetRMacNodeName(pstData->szNodeName);
		break;

	case YET_OID_RMacGet :
		OnRequestGet();
		break;

	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterPlatformGroup(void *DataBuf, BYTE ucOID)
{
	BOOL bSaveConfig = TRUE;
	PFormObjData* pstData = (PFormObjData*)DataBuf;

	switch(ucOID){
	case YET_OID_MyIP :
		OnRequestSetPlatformMyIP(pstData->mMyIP);
		break;
	case YET_OID_PeerIP :
		OnRequestSetPlatformPeerIP(pstData->mPeerIP);
		break;
	case YET_OID_PFMyPort :
		OnRequestSetPlatformMyPort(pstData->wMyPort);
		break;
	case YET_OID_PFPeerPort :
		OnRequestSetPlatformPeerPort(pstData->wPeerPort);
		break;
	case YET_OID_GWOID :
		OnRequestSetPlatformGWOID(pstData->szGWOID);
		break;
	case YET_OID_RegGW :
		break;
	case YET_OID_RptTime :
		OnRequestSetPlatformRptTime(pstData->wRptTime);
		break;
	case YET_OID_URIPath :
		OnRequestSetPlatformURI(pstData->szURIPath);
		break;
	case YET_OID_M2MType :
		OnRequestSetM2MType(pstData->ucM2MType);
		break;
	case YET_OID_M2MVer :
		OnRequestSetM2MVersion(pstData->ucM2MVer);
		break;
	case YET_OID_CSEID :
		OnRequestSetPlatformCSEID(pstData->szCSEID);
		break;
	case YET_OID_BlockSize :
		OnRequestSetPlatformBlockSize(pstData->wBlockSize);
		break;
	case YET_OID_Sec :
		OnRequestSetPlatformSec(pstData->bSec);
		break;
	case YET_OID_SecDebug :
		OnRequestSetPlatformSecDebug(pstData->bSecDebug);
		break;
	case YET_OID_SecPW :
		OnRequestSetPlatformSecPW(pstData->szSecPW);
		break;
	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterSCommGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	BOOL bSaveConfig = TRUE;
	SCommObjData* pstData = (SCommObjData*)DataBuf;

	switch(ucOID){
	case YET_OID_SBaudrate :
		OnRequestSetSCommBaudrate(ucMIdx, pstData->ucBaudrate);
		break;
	case YET_OID_SDataBits :
		OnRequestSetSCommDataBits(ucMIdx, pstData->ucDataBits);
		break;
	case YET_OID_SStopBits :
		OnRequestSetSCommStopBits(ucMIdx, pstData->ucStopBits);
		break;
	case YET_OID_SParity :
		OnRequestSetSCommParity(ucMIdx, pstData->ucParity);
		break;
	case YET_OID_SDelayTime :
		OnRequestSetSCommDelayTime(ucMIdx, pstData->wDelayTime);
		break;
	case YET_OID_SMaxPktSize :
		OnRequestSetSCommMaxPktSize(ucMIdx, pstData->wMaxPktSize);
		break;
	case YET_OID_SCheckAlive :
		if(ucMIdx == P485_PORT_IDX - 1) InitializeSPortMemory(ucMIdx);
		else if(ucMIdx == LORA_PORT_IDX - 1) InitializeSCommModule(DEF_LORA_IDX);
		else InitializeSCommModule(ucMIdx);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SReset :
		if(ucMIdx == LORA_PORT_IDX - 1) {HWCtrlResetLoRAModule();  InitializeSCommModule(DEF_LORA_IDX); }
		bSaveConfig=FALSE;
		break;
	default : bSaveConfig=FALSE;
		break;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterSNodeGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	BOOL bSaveConfig = TRUE;
	SNodeObjData* pstData = (SNodeObjData*)DataBuf;

	switch(ucOID){
	case YET_OID_SNodeName :
		OnRequestSetSNodeName(ucMIdx, pstData->szSNodeName);
		break;
	case YET_OID_SOID :
		OnRequestSetSOID(ucMIdx, pstData->szOID);
		break;
	case YET_OID_SNodeDel :
		OnRequestDelSNode(ucMIdx);
		return TRUE;
	case YET_OID_SRegNode :
		break;
	case YET_OID_SCtrlReboot :
		OnRequestSetSNodeReboot(ucMIdx);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SCtrlRptTime :
		OnRequestSetSNodeRptTime(ucMIdx, pstData->wRptTime);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SCtrlCfg :
		OnRequestSetSNodeConfig(ucMIdx, pstData->szCtrlCfg);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SCtrlDestGW :
		OnRequestSetSNodeDestGW(ucMIdx, pstData->szDestGW);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SCtrlTime :
		OnRequestSetSNodeTimeSync(ucMIdx);
		bSaveConfig=FALSE;
		break;
	case YET_OID_SNodeDelAll :
		OnRequestDelSNodeAll();
		return TRUE;
	default : bSaveConfig=FALSE;
		break;
	}

	return bSaveConfig;
}

static BOOL OnSetParameterURSNodeGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	switch(ucOID){
	case YET_OID_URSNodeDel :
		OnRequestDelURSNode(ucMIdx);
		break;
	default : break;
	}

	return FALSE;
}

static BOOL OnSetParameterSPortGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	SPortObjData* pstData = (SPortObjData*)DataBuf;
	switch(ucOID){
	case YET_OID_SPortDevCnt :
		OnRequestSetSPortDevCnt(ucMIdx, pstData->ucDevCnt);
		break;
	case YET_OID_SPortPollInt :
		OnRequestSetSPortPollInt(ucMIdx, pstData->wPollInt);
		break;
	case YET_OID_SPortWaitTime :
		OnRequestSetSPortWaitTime(ucMIdx, pstData->wWaitTime);
		break;
	default : break;
	}

	return TRUE;
}

static BOOL OnSetParameterSensorGroup(void *DataBuf, BYTE ucOID)
{
	BOOL bSaveConfig = TRUE;

	return bSaveConfig;
}

void SendSCommMsg(int nPIdx, char szMsg[]);// SCommAgentMain.cpp
static BOOL OnSetParameterSystemGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	SystemObjData * pstData = (SystemObjData *)DataBuf;
	int nSize;

	switch(ucOID){
		case YET_OID_Save :
			return TRUE;
		case YET_OID_Reset :
			OnRequestSetResetSystem(pstData->ucReset);
			break;
		case YET_OID_DateTime :
			GeneralLogMsg("The time is synchnized to [%s].", pstData->strDateTime);
			OnRequestSetCurrentDateTime(pstData->strDateTime);
			break;
		case YET_OID_DateTime_GPS :
			GeneralLogMsg("The time GPS is synchnized to [%s].", pstData->strDateTime);
			OnRequestSetCurrentDateTime_GPS(pstData->strDateTime);
			break;
		case YET_OID_ResetDB :
			OnRequestSetResetDB();
			break;
		case YET_OID_ResetStatus :
			OnRequestSetResetStatus();
			break;
		case YET_OID_SComSend :
			if(ucMIdx < MAX_SCOMM_CNT || ucMIdx == (UDP_PORT_IDX - 1)) SendSCommMsg(ucMIdx, (char*)pstData->mPacket);
			else LogMessageDebugTemp("Invalid MIdx=%d\n", ucMIdx+1);
			break;
		case YET_OID_ComPacket :// CLI->HWManager에게 요청할 송신패킷(디버그용)
			nSize = pstData->mPacket[0];
			if(nSize > MAX_DESC_STR) nSize = MAX_DESC_STR;
			break;
		case YET_OID_DebugFlag : 
			gpDB->System.dwDebugFlag = gpLog->dwDebugFlag | ((gpLog->dwDebugFlag & 0x3FFFFFFE) ? 0x0001 : 0x0);
			return TRUE;
		case YET_OID_AutoRebootMin :
			gpDB->System.wAutoRebootMin = pstData->wAutoRebootMin;
			return TRUE;
		case YET_OID_ResetEnv :
			OnRequestSetResetEnv();
			break;
		default : 
			break;
	}
	return FALSE;
}

static BOOL OnSetParameterPdGroup(void *DataBuf, BYTE ucOID, BYTE ucMIdx)
{
	BOOL bSaveConfig = TRUE;
	PdObjData * pstData = (PdObjData *)DataBuf;
	// int nSize;

	switch(ucOID){
		case YET_OID_PD_MONITOR_PORT :
			OnRequestSetPdMonitorPort(ucMIdx, pstData->wMonPort);
			break;
		case YET_OID_PD_MODE :
			OnRequestSetPdMode(ucMIdx, pstData->bMode);
			break;
		case YET_OID_PD_SYNC :
			OnRequestSetPdSync(ucMIdx, pstData->ucSync);
			break;
		case YET_OID_PD_SYNC_VAL :
			OnRequestSetPdSyncVal(ucMIdx, pstData->ucSyncVal);
			break;
		case YET_OID_PD_GAIN :
			OnRequestSetPdGain(ucMIdx, pstData->ucChGain);
			break;
		case YET_OID_PD_TRIGGER :
			OnRequestSetPdTrigger(ucMIdx, pstData->ucTrigger);
			break;
		case YET_OID_PD_FILTER_MIN :
			OnRequestSetPdFilterMin(ucMIdx, pstData->ucFilterMin);
			break;
		case YET_OID_PD_FILTER_MAX :
			OnRequestSetPdFilterMax(ucMIdx, pstData->ucFilterMax);
			break;
		case YET_OID_PD_TYPE :
			OnRequestSetPdType(ucMIdx, pstData->bType);
			break;
		case YET_OID_PD_ADC_FORMAT :
			OnRequestSetPdAdcFormat(pstData->ucFormat);
			break;
		case YET_OID_PD_RPTTIME :
			OnRequestSetPdRpttime(pstData->wRptTime);
			break;						
		case YET_OID_PD_ADC_TEST :
			OnRequestSetPdAdcTest(ucMIdx, pstData->bAdcTest);
			break;	
		case YET_OID_PD_ADC_VIRTUAL :
			OnRequestSetPdAdcVirtual(pstData->bAdcVirtual);
			break;							
		case YET_OID_PD_ADC_READ :
			OnRequestSetPdAdcRead(pstData->bAdcRead);
			break;
		case YET_OID_PD_DAQ_STATUS :
			OnRequestSetPdDaqStatus(pstData->bDaqStatus);
			break;
		case YET_OID_PD_ADC_RESET :
			OnRequestSetPdAdcReset(pstData->bAdcReset);
			break;	
		case YET_OID_PD_DAQ_IDLE :
			OnRequestSetPdDaqIdle(pstData->bDaqIdle);
			break;	
		default : bSaveConfig=FALSE;
	}

	return bSaveConfig;
}

void ProcessSetParameterMessage(void)
{
	BOOL bSaveConfig = FALSE;

	BYTE ucGID = gpIPC->Set.ucGID;
	BYTE ucOID = gpIPC->Set.ucOID;
	BYTE ucMIdx = gpIPC->Set.ucMIndex;
	void *DataBuf = &gpIPC->Set.DataBuf;

	DebugLogIPCSetParameter(0);
	switch(ucGID){
		case YET_GID_GENERAL :
			bSaveConfig = OnSetParameterGeneralGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_CHASSIS :
			bSaveConfig = OnSetParameterChassisGroup(DataBuf, ucOID);
			break;
		case YET_GID_NAT_PFWD :
			bSaveConfig = OnSetParameterPortForwardGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_ROUTE_TBL :
			bSaveConfig = OnSetParameterRouteTblGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_RMAC :
			bSaveConfig = OnSetParameterRMacGroup(DataBuf, ucOID);
			break;
		case YET_GID_PLATFORM :
			bSaveConfig = OnSetParameterPlatformGroup(DataBuf, ucOID);
			break;			
		case YET_GID_LORA :
			bSaveConfig = OnSetParameterLoraGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_SCOMM :
			bSaveConfig = OnSetParameterSCommGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_SNODE :
			bSaveConfig = OnSetParameterSNodeGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_URSNODE :
			OnSetParameterURSNodeGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_SPORT :
			OnSetParameterSPortGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_SENSOR :
			bSaveConfig = OnSetParameterSensorGroup(DataBuf, ucOID);
			break;
		case YET_GID_SYSTEM :
			bSaveConfig = OnSetParameterSystemGroup(DataBuf, ucOID, ucMIdx);
			break;
		case YET_GID_PD_NODE :
			bSaveConfig = OnSetParameterPdGroup(DataBuf, ucOID, ucMIdx);
			break;			
		default : break;
	}

	if(bSaveConfig) WriteDeviceInfoDBToDisk();
	gpIPC->Set.ucStatus = YET_SetParameterReady;
}
// End of Process Set Parameter Message
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitializeSystemSetup(void)
{
	strcpy(gpDB->Chassis.szDevName, MODEL_NAME);
	gpDB->System.dwBootTime = GetTimeTickCount();
}

void InitializeHWManager(void)
{
	ClearGeneralLogMsg();
	ReadDeviceInfoDBFromDisk();
	if(strlen(gpDB->Platform.szGWOID) > strlen(gpDB->Platform.szOIDBase) ) gpDB->Platform.bAssignOID = 1;
	OnRequestSetNTP(gpDB->General.bNTP, 0);
	OnRequestSetTelnet(gpDB->General.bTelnet, 0);
	OnRequestSetFtp(gpDB->General.bFtp, 0);
	}
	
	HWCtrlGetSWVersion(&gpDB->Version.ucMajor, &gpDB->Version.ucMiddle, &gpDB->Version.ucMinor, gpDB->Version.strDate, gpDB->Version.strDescription);
	gpDB->Version.ucBoard = MPU_BOARD_VERSION;
	GeneralLogMsg("Version = %d.%d.%d(%s)\r\n", gpDB->Version.ucMajor, gpDB->Version.ucMiddle, gpDB->Version.ucMinor, gpDB->Version.strDate);

	InitializeDebugLogMessage(gpDB->System.dwDebugFlag);
	if(gpDB->System.dwDebugFlag & 0x01) {
		gpDB->System.dwDebugFlag = 0;
		WriteDeviceInfoDBToDisk();
	}
	InitializeIPCSetMessage();
	InitializePSHardware();
	InitializeNetwork();
	InitializeI2C();
	InitializeSystemSetup();
	
	PrintGeneralLogMsg("HWManager finished Initializer !");
}

void HWManagerMain(void)
{
	HWCtrlCheckCommLEDOff();
	if(CheckMilliSecTimer(TIMER_ID_1, 250)){//250msec
		HWCtrlBlinkRunLED();
	}
	if(gpDB->System.wAutoRebootMin && (dwNow > gpDB->System.dwBootTime + gpDB->System.wAutoRebootMin*60000)){
		gpDB->System.dwBootTime = dwNow;
		PrintGeneralLogMsg("Auto Restart, Time=%dmin !", gpDB->System.wAutoRebootMin);
		OnRequestSetResetSystem(YET_RESET_WARM);
	}
}

extern void InitializeSCommAgent(void);
extern void DestroySCommAgent(void);
extern int ProcessSCommAgentMain(void);
extern void RMacDeviceBuild(); 
int main(void)
{
	int nDBShmID, nIPCShmID, nLogShmID;
	DWORD nLoopCount = 0;
	
	gpDB = (YST_DB_MAIN *)InitializeYDBSharedMemory(&nDBShmID, sizeof(YST_DB_MAIN));
	gpIPC = (YST_IPC *)InitializeIPCSharedMemory(&nIPCShmID, sizeof(YST_IPC));
	gpLog = (YST_DB_LOG *)InitializeLogSharedMemory(&nLogShmID, sizeof(YST_DB_LOG));

	InitializeHWManager();
	SleepMilliSeconds(10);

	InitializeSCommAgent();
	InitializeRMACComm();
	SleepMilliSeconds(100);	
	PDNodeOnInit();	
	SleepMilliSeconds(100);	

	if(gpIPC->Set.ucStatus == YET_SetParameterData){
		ProcessSetParameterMessage();
	}
	RMacDeviceBuild(); 

	PrintGeneralLogMsg("Program Start !");
	SaveToEventLogMsg(0, "Program Start !");
	
	while(1){
		if(++nLoopCount == 100001)	nLoopCount = 1;	// max 100000ms = 100sec		
		if(gpIPC->Set.ucStatus == YET_SetParameterData){
			ProcessSetParameterMessage();
		}

		// if(!DebugEnable(LOG_DEBUG_HW1))
			HWManagerMain();
		// if(!DebugEnable(LOG_DEBUG_HW2))
			ProcessSCommAgentMain();
			ProcessRMACCommMain();

		SleepMilliSeconds(1);		// delay 1ms 
	}//end of while
	
	DestroyI2C();
	DestroySCommAgent();
	DestroyRMACComm();
	
	DestroyYDBSharedMemory(nDBShmID, gpDB, TRUE);
	DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, TRUE);
	DestroyLogSharedMemory(nLogShmID, gpLog, TRUE);

	return 0;
}
