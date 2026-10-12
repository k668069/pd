///////////////////////////////////////////////////////////
// ClassSocketInfo.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(CLASSSOCKETINFO_H__INCLUDED_)
#define CLASSSOCKETINFO_H__INCLUDED_

#include "ClassQueue.h"

#define BAD_SOCKET -1
#define BAD_PORT   -1
#define MAX_SOCKET_REMOTE_NAME 512

#define SOCKET_CLASS_UNKNOWN 0
#define SOCKET_CLASS_LISTEN 1	// 접속 대기 소켓
#define SOCKET_CLASS_SERVER	2	// 이쪽에서 상대방에게 접속을 시도하는 소켓.
#define SOCKET_CLASS_CLIENT 3	// 상대방이 이쪽으로 접속을 시도하는 소켓.

#define SOCKET_STATUS_DISCONNECT	0	// 접속안됨
#define SOCKET_STATUS_CONNECTING	1	// 접속중
#define SOCKET_STATUS_CONNECTED		2	// 접속됨

#define SOCK_IO_SHUTDOWN	-2		// 접속을 강제로 종료함
#define SOCK_IO_CLOSE		-1		// 접속이 상대방에 의해 끊어짐
#define SOCK_IO_OK			0		// 접속이 이상 없슴
#define SOCK_IO_DONE		0		// 전송이 완료됨
#define SOCK_IO_REMAIN		1		// 전송될 데이터가 남아 있슴

class CTCPSocket
{
public : 
	int m_nFd;				// 소켓 기술자
	BYTE m_mPeerIPAddr[4];	// 상대방 IP주소
	int m_wPeerPortNo;		// 상대방 포트번호
	// int m_nPortNum;			// 상대방 포트번호
	int m_nClass;			// 접속된 상대방의 종류, 0:알수없는상태임, 1:Listen, 2:Server, 3:Client(Browser)
	int m_nStatus;			// 접속여부, 0:접속안됨, 1:접속중, 2:접속됨.
	int m_nMIndex;			// 소켓 관리 번호
	int m_nAIndex;			// 소켓 취급(Agent) 번호(카메라 번호, UART포트번호)
	int m_nLastImgID;		// 캠서버에서 사용되며 최종으로 소켓을 통해 전송한 이미지번호.
	int m_nSndStatus;		// 소켓을 통하여 어떠한 데이터를 보냈는지 확인, 송신큐만을 사용하지 않고 Buffer를 사용하는 경우 사용.헤더보냄/데이터보냄/분리자보냄등..
	BOOL m_bPermit;			// 상대방과의 접속이 허용되었나 여부, 적절한 상대방이 아닌 경우 FALSE이며 응답을 하지 않고 접속을 종료함.
	DWORD m_dwSndBufSize;	// 송신을 큐를 사용하지 않고 버퍼 주소를 직접 지정한 경우 버퍼의 크기
	BYTE * m_pSndBuf;		// 송신을 큐를 사용하지 않고 직접 버퍼 주소를 지정한 경우.
	time_t m_nLastConTryTime;// 마지막으로 접속을 시도한 시간
	time_t m_nLastDataRcvTime;//마지막으로 데이터를 수신한 시간
	char m_strRemoteName[ MAX_SOCKET_REMOTE_NAME ];     // Remote hostname, IPAddress
	char m_strDebugID[10];
	int m_nQOffset;			// 이미지 중계시 이미지의 경계를 찾기위해 사용됨.
	DWORD m_nJpgSize;		// 이미지의 크기를 저장함.	
	BOOL m_bLogAsText;		// 디버그 통신로그를 텍스트로 보여주는 경우 1
	
	WORD m_ImgTime[7];		// 저장된 영상을 전송하는 경우 전송할 영상의 날짜/시간, 각각 년/월/일/시/분/번호/증분으로 이루어짐
	BYTE * m_pImgBuf;		// 저장된 영상을 전송하는 경우 전송할 영상 데이터
	
	CQueue* m_pQRcv;		// 데이터 임시 저장버퍼
	CQueue* m_pQSnd;		// 데이터 임시 저장버퍼
	DWORD m_dwPFlag;		// 프로세서 Flag, 디버그로그용으로 사용, LOG_DEBUG_MAIN, LOG_DEBUG_CAMSERV, LOG_DEBUG_NETWORK, LOG_DEBUG_UPGRADE
	
	CTCPSocket();
	CTCPSocket(DWORD dwPFlag, DWORD nSizeRcvQ = SIZE_1K, DWORD nSizeSndQ = SIZE_4K);
	~CTCPSocket();

	void Initialize(DWORD dwPFlag, DWORD nSizeRcvQ = SIZE_1K, DWORD nSizeSndQ = SIZE_4K);
	void Destroy(void);
	void CloseSocket(void);
	void SetRemoteName(const char strRemoteName[]);
	void SetInformation(int nFd, WORD nPortNum, int nClass);
	int RegisterDataToSendQueue(const void* pBuf, DWORD dwSize);
	void RegisterSndBuffer(const void* pBuf, DWORD dwBufSize);
	int SendRegisteredDataToTCPPort(void);
	int ReadDataFromTCPPortAndSaveToQueue(BYTE ucOperMode = 0);
	int LoadDataFromRcvQueue(void *Data, int nCount);
	int GetDataFromTCPPort(BYTE* pData, DWORD nMaxSize);
	int GetDataAndSizeFromTCPPort(BYTE* pData, DWORD nMaxSize);
};

#endif //CLASSSOCKETINFO_H__INCLUDED_
