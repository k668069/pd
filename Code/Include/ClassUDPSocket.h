///////////////////////////////////////////////////////////
// ClassUDPSocket.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#if !defined(CUDPSOCKET_H__INCLUDED_)
#define CUDPSOCKET_H__INCLUDED_

#include <netinet/in.h>
#include "TypeDef.h"
#include "ClassQueue.h"

typedef struct sockaddr SA;

class CUdpSocket
{
public:
	CUdpSocket();
	CUdpSocket(DWORD dwPFlag, int nPIdx);
	~CUdpSocket();

public:
	int m_SockFd;
	int m_nPIdx;
	struct sockaddr_in m_LastPeer;
	BYTE m_mLastRcvIP[4];
	WORD m_wLastRcvPort;
	char m_strDebugID[10];
	DWORD m_dwPFlag;		// 프로세서 Flag, 디버그로그용으로 사용, LOG_DEBUG_MAIN, LOG_DEBUG_CAMSERV, LOG_DEBUG_NETWORK, LOG_DEBUG_UPGRADE
	CQueue m_QRcv;			// 데이터 임시 저장버퍼
	
	int OpenSocket(WORD wPortNo);
	int SetSocketReceiveTimeOut(int nLimitTime);
	int ReceiveDataFromPort(void);
	void SendDataToPort(BYTE mIPAddr[], WORD nPortNo, BYTE *pData, int nDataSize, BOOL bBroadcast = FALSE);
	void SendDataToPort(char strIPAddr[], WORD nPortNo, BYTE *pData, int nDataSize, BOOL bBroadcast = FALSE);
	void SendDataToLastPeerAddress(BYTE *pData, int nDataSize);
	void SendDataToLastPeerAddress(BYTE *pData, int nDataSize, WORD wPortNo);
	WORD GetLastPeerIPAddress(char strIPAddr[]);
	void CloseSocket(void);
};

#endif //!defined(CUDPSOCKET_H__INCLUDED_)
