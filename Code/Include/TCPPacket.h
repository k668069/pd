/////////////////////////////////////////////////////////// 
// TCPPacket.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#include "DeviceInfoDB.h"

#if !defined(TCPPACKET_H__INCLUDED_)
#define TCPPACKET_H__INCLUDED_

#define MAX_DATA_RCV_SIZE			1024

typedef struct YST_TCP_PACKET_HEADER_Tag{
	char cSOP[3];			// Start Of Protocol - '!', '@', '#'
	char cPacketID;			// Packet ID
	BYTE ucGID;				// Object Group ID
	BYTE ucOID;				// Object ID
	BYTE ucMIndex;			// Line card ModuleIndex
	BYTE ucPIndex;			// Port Index
	BYTE ucVersion;			// DB Structure Version
	BYTE Reserved;
	BYTE ucSize[2];			// Data Length
	BYTE CRC16DH;			// Data Field CRC Check
	BYTE CRC16DL;			// Data Field CRC Check
	BYTE CRC16HH;			// Header Field CRC Check
	BYTE CRC16HL;			// Header Field CRC Check
} YST_TCP_PACKET_HEADER;

////////// Define Packet ID
enum TCPPacketIDDef{
	YET_PID_GET 			= 'G',
	YET_PID_SET 			= 'S',
	YET_PID_REPLY 			= 'R',
	YET_PID_ACK				= 'A',
	YET_PID_BROADCAST		= 'B',
	YET_PID_WARNING 		= 'W',
	YET_PID_PERMISSON		= 'P',
	YET_PID_PACKET_ERROR	= 'E',
	YET_PID_QUIT			= 'Q'
};

////////// Define Object ID And Data Structure
#define MAN_ACCESS_PASSWORD		"YOUSTER_APP"
typedef struct ManagerInfoObjData{
	BYTE ManID[2];			// Manager ID
	BYTE ManVer[2];			// Manager Version
	char szPassword[12];	// Connection Access Password
	BYTE mManMAC[6];		// Manager MAC Address
	BYTE Reserved[8];
} ManagerInfoObjData;

enum ConnectPermissionTypeDef{
	YET_PS_INVALID	= 0x00,
	YET_PS_OK		= 0x01,
	YET_PS_NOK		= 0x02
};

enum ConnectDenyReasonTypeDef{
	YET_CDT_NO_ERROR		= 0,
	YET_CDT_OVER_CONNECT	= 1,			// 최대 접속 허용수 초과
	YET_CDT_PASSWORD_ERR	= 2,			// 접속 PASSWORD 오류
	YET_CDT_INFO_CRC_ERR	= 3,			// Manager Info가 유효하지 않음(CRC Error)
	YET_CDT_INFO_SIZE_ERR	= 4,			// Manager Info가 유효하지 않음(Size Error)
	YET_CDT_RCV_TIMEOUT		= 5,			// Manager Info를 접속후 10초 동안 받지 않음
	YET_CDT_MMAC_ERR		= 6,			// 접속 PC의 MAC주소 오류
};

typedef struct AcceptConnectionObjData_Tag{
	BYTE ucPermission;						// 1:Permission OK, 2:Permision Deny
	BYTE ucDenyReason;						// Denied Reason if connection is denied
	BYTE ucAgentID;							// Agent ID
	BYTE ucBDVersion;						// Agent Board Version
	BYTE ucVersion[4];						// Agent S/W Version
	BYTE ucReserved[4];
} AcceptConnectionObjData;

// 이 스트럭트의 크기가 2바이트인지, 4바이트인지 일정치 않아 사용하지 않는다.
// 직접 2바이트 배열로 선언하자.
//typedef struct YST_WORD_Tag{
//	BYTE Word[2];
//} YST_WORD;

typedef struct YST_DWORD_Tag{
	BYTE DWord[4];
} YST_DWORD;

typedef struct YST_DDWORD_Tag{
	BYTE Data[8];
} YST_DDWORD;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define Signal Struct
typedef struct YST_DB_GENERAL_PKTV1{
	BYTE mIP[4];						// IP주소
	BYTE mSM[4];						// 넷마스크
	BYTE mGW[4];						// 게이트웨이
	BYTE mMAC[6];						// MAC주소
	BYTE Reserved[2];
	BYTE mManMAC1[6];					// 접속가능 관리PC MAC주소
	BYTE mManMAC2[6];					// 접속가능 관리PC MAC주소
	char szPassword[MAX_PWD_STR+1];		// 접근암호
} YST_DB_GENERAL_PKTV1;

typedef struct YST_DB_CHASSIS_PKTV1{
	BYTE ucModelID;						// 0
	BYTE ucSNodeCnt;					// 센서노드 수
	BYTE ucSlvCnt;						// 슬레이브 수
	BYTE ucURSNodeCnt;					// Ukn 센서노드 수
	char szDevName[36];					// Device Name
	char szSerialNo[MAX_DESC_STR+1];	// 새시 제조 일련번호 ex) JSNB101012001
	char szPoleNo[MAX_DESC_STR+1];		// 전주번호
	char szMangeNo[MAX_DESC_STR+1];		// 전산번호
	char szInstallDate[MAX_DESC_STR+1];	// 설치일자
} YST_DB_CHASSIS_PKTV1;

typedef struct YST_DB_RMAC_PKTV1{
	BYTE ucDevID;			// 모듈종류, 0x00:IPTU-1U, 0x01:IPTU-CARD, 0x02:IPTU-Slave, 0x10:SOTS-OCU, 0x11:SOTS-Slave
	BYTE ucClosedRing;		// Topology상태, 1:OpenRing, 2:ClosedRing
	BYTE mFWVersion[4];		// FW Version, [0]=>Year, [1]=>Month, [2]=>Day, [3]=>Build
	BYTE mChipID[4];		// Chip ID & Version, 3210B720 => 3.2.1 0B/07/20
	BYTE mRMAC[6];			// 링맥주소
	BYTE mManIPAddr[4];		// 관리 IP 주소
	BYTE ucLink[4];			// East/West/Host 포트의 Link 상태, East:포트0,오른쪽, West:포트1,왼쪽, 0:LinkFail, 1:10Mbps-Half, 2:10Mbps-Full, 3:100Mbps-Half, 4:100Mbps-Full, 5:1Gbps-Half, 6:1Gbps-Full
	BYTE ucMiscable[2];		// East/West 포트의 Miscable 상태, 0:Normal, 1:Miscalble
	BYTE mNodeCount[2];		// East/West 방향의 노드수
} YST_DB_RMAC_PKTV1;

typedef struct YST_DB_PLATFORM_PKTV1{
	BYTE mMyIP[4];			// 자기 IP주소
	BYTE mPeerIP[4];		// 플랫폼 IP주소
	BYTE mMyPort[2];		// 자기 Listen 포트번호
	BYTE mPeerPort[2];		// 상대 Listen 포트번호
	BYTE mRptTime[2];		// 상태보고 주기
	BYTE bAssignOID;		// G/W의 OID를 할당 받은 경우 1
	BYTE bCommOK;			// 플랫폼이 정상적으로 응답한 경우 TRUE
	char szGWOID[40];		// G/W OID
	char szURIPath[40];		// URI, Uniform Resource Identifier Path
	char szCSEID[20];		// AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
	char cReserved[52];
} YST_DB_PLATFORM_PKTV1;

typedef struct YST_DB_LORA_PKTV1 {
	BYTE ucRFFreq; // 0~15
	BYTE ucValid;	
	BYTE Reserved[3];
	BYTE mEUI[3];
} YST_DB_LORA_PKTV1;

typedef struct YST_DB_SCOMM_PKTV1{ 
	BYTE mDelayTime[2];			// Milli Seconds, UART로 부터 입력된 데이터를 하나의 패킷으로 전송하기 위해 기다리는 시간.
	BYTE mMaxPktSize[2];		// UART로 부터 입력된 데이터가 이 숫자보다 크면 DelayTime이전에도 패킷으로 전송한다.
	BYTE ucBaudrate;			// 1:1200bps ~ 13:1500000bps
	BYTE ucDataBits;			// 1:5bits, 2:6bits, 3:7bits, 4:8bits
	BYTE ucStopBits;			// 1:1bit, 2:2bits
	BYTE ucParity;				// 1:NoParity, 2:OddParity, 3:EvenParity
} YST_DB_SCOMM_PKTV1;

typedef struct YST_DB_SLAVE_PKTV1{
	BYTE mEUI[8];			// 64-bit IEEE address
	BYTE mRSSI[2];			// RF신호세기
	BYTE mReserved[2];
	BYTE mLastRcvTime[8];	// 마지막 정보 수신시간
	char szSlvOID[36];			// 슬레이브 이름
} YST_DB_SLAVE_PKTV1;

typedef struct YST_DB_SLAVE_PKTV2{
	BYTE mEUI[8];			// 64-bit IEEE address
	BYTE mRSSI[2];			// RF신호세기
	BYTE mReserved[2];
	BYTE mLastRcvTime[8];	// 마지막 정보 수신시간
	char szSlvOID[36];		// 슬레이브 OID
	BYTE bCommFail;			// 0:Comm. OK, 1:Comm. Fail
	BYTE ucPktVer;			// 수신 패킷 버젼
	BYTE mReserved2[2];
	char szManufacture[20];	// 제조사
	char szDevName[20];		// 제품명
	char szFWVer[20];		// F/W Version정보
	char szPayload[128];	// 수신 데이터
	char szSCNFG[MAX_SNR_CFG_CNT][20];	// 센서 설정 값
} YST_DB_SLAVE_PKTV2;

typedef struct YST_DB_SNODE_PKTV1{
	BYTE ucSlvIdx;			// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
	BYTE ucRFModule;		// (ucPIdx<<4) | ucRFModule
	BYTE bCommFail;			// 0:Comm. OK, 1:Comm. Fail
	BYTE ucLWMType;			// LWM2M 센서 형식, YST_DB_LWM2M_TYPE 참조
	BYTE mReserved[2];
	BYTE mRSSI[2];			// RF신호세기
	BYTE mLastRcvTime[8];	// 마지막 정보 수신시간
	BYTE mEUI[8];			// 64-bit IEEE address
	BYTE mRcvPayload[MAX_PAYLD_CNT];// 수신 데이터 존재여부
	char szManufacture[20];	// 제조사
	char szDevName[20];		// 제품명
	char szFWVer[20];		// F/W Version정보
	char szOID[40];			// 센서 OID
	char szPayload[MAX_PAYLD_CNT][128];	// 수신 데이터
	char szSCNFG[MAX_SNR_CFG_CNT][20];	// 센서 설정 값
	char szSNodeName[36];	// 센서노드 이름
} YST_DB_SNODE_PKTV1;

typedef struct YST_DB_SNODE_PKTV2{
	BYTE ucSlvIdx;			// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
	BYTE ucRFModule;		// (ucPIdx<<4) | ucRFModule
	BYTE bCommFail;			// 0:Comm. OK, 1:Comm. Fail
	BYTE mReserved[3];
	BYTE mRSSI[2];			// RF신호세기
	BYTE mLastRcvTime[8];	// 마지막 정보 수신시간
	char szPayload[128];	// 수신 데이터
} YST_DB_SNODE_PKTV2;

typedef struct YST_DB_URSNODE_PKTV1{
	BYTE ucSlvIdx;			// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
	BYTE ucPIdx;			// 0:Ukn, 1:Zigbee, 2:Lora센서, 3:Lora슬레이브
	BYTE mRSSI[2];			// RF신호세기
	BYTE mLastRcvTime[8];	// 마지막 정보 수신시간
	BYTE mEUI[8];			// 64-bit IEEE address
	char szOID[40];			// Sensor OID
	char szPayload[128];	// 수신 데이터
	char szErrReason[32];	// 오류 이유
} YST_DB_URSNODE_PKTV1;

typedef struct YST_DB_NODE_PKTV1{
	BYTE ucDevID;			// 모듈종류, 0x00:IPTU-1U, 0x01:IPTU-CARD, 0x02:IPTU-Slave, 0x10:SOTS-OCU, 0x11:SOTS-Slave
	BYTE mFWVersion[2];		// FW Version
	BYTE ucClosedRing;		// Topology상태, 1:OpenRing, 2:ClosedRing
	BYTE ucLink[4];			// East/West/Host 포트의 Link 상태, East:포트0,오른쪽, West:포트1,왼쪽, 1:LinkFail, 2:LinkOK
	BYTE ucMiscable[2];		// East/West 포트의 Miscable 상태, 1:Normal, 2:Miscalble
	BYTE mRMAC[6];			// 각 노드의 링맥주소
	BYTE mManIPAddr[4];		// 관리 IP 주소
} YST_DB_NODE_PKTV1;

typedef struct YST_DB_NAT_PKTV1{
	BYTE ucUseNAT;						// IP공유 기능을 사용하면 1+1
	BYTE ucPMode;						// 모든 패킷을 수신하는 모드(PromiscuousMode)이면 1+1
	BYTE ucRcvBroad;					// Broadcast 패킷을 수신하는 모드이면 1+1
	BYTE Reserved1;
	BYTE mMPort[4][2];					// 23, 7000,7001번 이외에 IPTU가 사용하는 매핑포트
	BYTE ucUseMPort[4];					// 매핑포트를 사용하면 1+1
	BYTE mHMACAddr[6];					// FRTU MAC주소
	BYTE Reserved2[2];
} YST_DB_NAT_PKTV1;

typedef struct YST_DB_STATUS_PKTV1{
	BYTE bGPSValid;						// GPS 신호를 정상 수신한 경우
	BYTE Reserved[3];
	BYTE mLatitude[4];					// 위도, 단위:1/100도
	BYTE mLongitude[4];					// 경도, 단위:1/100도
	BYTE mAltitude[4];					// 해발고도, 단위:m
	BYTE mTemperature[4];				// RTC에서 읽은 온도
} YST_DB_STATUS_PKTV1;

typedef struct YST_DB_VERSION_PKTV1{
	BYTE ucMajor;							// 주버젼
	BYTE ucMiddle;							// 부버젼
	BYTE ucMinor;							// 빌드버젼
	BYTE ucBoard;							// 보드버젼
	char strDate[MAX_DESC_STR+1];			// 빌드 날짜-시간
	char strDescription[MAX_DESC_STR+1];	// 버젼 변경사항 설명용
} YST_DB_VERSION_PKTV1;

typedef struct YST_DB_LOGQ_PKTV1{
	YST_LOG_DATA Data[LOG_Q_SIZE];
	BYTE mHead[2];
	BYTE mTail[2];
	YST_DWORD dwCRCValue;
} YST_DB_LOGQ_PKTV1;

typedef struct YST_DB_MAIN_PKTV1{
	YST_DB_GENERAL_PKTV1	General;
	YST_DB_CHASSIS_PKTV1	Chassis;

#ifdef DEF_RMAC
	YST_DB_RMAC_PKTV1		RMac;
#endif

	YST_DB_PLATFORM_PKTV1	Platform;

#ifdef DEF_LORA
	YST_DB_LORA_PKTV1		Lora;
#endif

	YST_DB_SCOMM_PKTV1		SComm[MAX_SCOMM_CNT];
	YST_DB_NODE_PKTV1		Node[2][MAX_NODE_CNT+1];
	YST_DB_SNODE_PKTV1		SNode[MAX_SNODE_CNT];
	YST_DB_URSNODE_PKTV1	URSNode[MAX_URSNODE_CNT];
	YST_DB_NAT_PKTV1		Nat;
	YST_DB_STATUS_PKTV1    Status;
	YST_DB_VERSION_PKTV1	Version;
	YST_DB_LOGQ_PKTV1		Log;
} YST_DB_MAIN_PKTV1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //!defined(TCPPACKET_H__INCLUDED_)
