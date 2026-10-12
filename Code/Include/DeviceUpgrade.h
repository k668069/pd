///////////////////////////////////////////////////////////
// DeviceUpgrade.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#if !defined(DEVICEUPGRADE_H__INCLUDED_)
#define DEVICEUPGRADE_H__INCLUDED_

#include "TypeDef.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Upgrade Main
int UpgradeUnitMain(BYTE ucMID, FILE *fp, int nFileSize, int nBlockSize);

/////////////////////////////////////////////////////////////////////////////////////////////////
// On Receive Upgrade Data From Line Card
int OnReceiveUpgradeRequestResponse(BYTE ucMID, BYTE Data[], int nDataSize);
int OnReceiveUpgradeStartResponse(BYTE ucMID, BYTE Data[], int nDataSize);
int OnReceiveUpgradeEndResponse(BYTE ucMID, BYTE Data[], int nDataSize);
int OnReceiveUpgradeOperModeResponse(BYTE ucMID, BYTE Data[], int nDataSize);
int OnReceiveUpgradeFileTransferResponse(BYTE ucMID, BYTE Data[], int nDataSize);
// On Receive Upgrade Data From Line Card
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
// On Receive Upgrade Data From 로컬장비
int OnReceiveUDPUpgradeRequestResponse(BYTE ucNID, BYTE Data[], int nDataSize);
int OnReceiveUDPUpgradeStartResponse(BYTE ucNID, BYTE Data[], int nDataSize);
int OnReceiveUDPUpgradeEndResponse(BYTE ucNID, BYTE Data[], int nDataSize);
int OnReceiveUDPUpgradeOperModeResponse(BYTE ucNID, BYTE Data[], int nDataSize);
int OnReceiveUDPUpgradeFileTransferResponse(BYTE ucNID, BYTE Data[], int nDataSize);
// On Receive Upgrade Data From 로컬장비
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif //DEVICEUPGRADE_H__INCLUDED_
