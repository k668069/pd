/////////////////////////////////////////////////////////// 
// Network.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#if !defined(NETWORK_H__INCLUDED_)
#define NETWORK_H__INCLUDED_

#include "TypeDef.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Network
void DeleteDefaultGW(void);
void SetNetworkConfig(char strIPAddress[], char strSubnetMask[]);
void SetNetworkConfig(BYTE mIP[], BYTE mSM[]);
void SetNetworkGateway(char strGateway[]);
void SetNetworkGateway(BYTE mGW[]);
void SetNetworkConfig2(char strIPAddress2[]);
void SetNetworkConfig2(BYTE mIP[]);
void GetNetworkConfig(char strIPAddress[], char strSubnetMask[], char strGateway[]);
void CheckNetworkConfig(char strIPAddress[], char strSubnetMask[], char strGateway[]);

void SetNetworkConfigHalow(void);
void ModifyPortFowardPolicy(void);
void AddRoutingTable(BYTE mRNet[], BYTE mRGW[]);
void DeleteRoutingTable(BYTE mRNet[], BYTE mRGW[]);
void SetDNSConfig(BYTE mDNS[]);
void SetEthConfig(BYTE nIdx, BYTE ethMode);
void InitializeNetwork(void);
#endif //!defined(NETWORK_H__INCLUDED_)
