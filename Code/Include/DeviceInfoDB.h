///////////////////////////////////////////////////////////
// DeviceInfoDB.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(DEVICEINFODB_H__INCLUDED_)
#define DEVICEINFODB_H__INCLUDED_

#include "TypeDef.h"

#define DEF_SECU

/*
COM0 = Console
COM1 = RS232			// sport
COM2 = RS485			// sport
COM3 = LoRa Uart		// module
*/

// 사용자 정의
// #define DEF_RTC  // 커널 드라이버DEF_SCOM_COUNT로 동작
// #define DEF_TEMPERATURE			// ITTH
// #define DEF_GPS
#define DEF_SCOM_COUNT	3		// Serial 포트 갯수 (콘솔 미포함)

#define DEF_LORA		// 1채널 로라무선통신
#define DEF_HALOW
// #define DEF_RMAC

#define DEF_PD
#define DEF_LPD	// local PD수집
#define DEF_PRPS
#define MAX_CH	4
// #define DEF_DAQ_220529

#define M2M_BLOCK
#define M2M_CONTROL
#define M2M_CTRL_SVR  // oneM2M 제어를 장비단에서 서버 생성
#define M2M_ZIP

#define DEF_KOREA_ZONE		32400	// 9*60*60
// #define DEF_SLAVE


// port
// COM0, Console
#define P232_PORT_IDX		1  // COM1, RS-232
#define P485_PORT_IDX		2  // COM2, RS-485
#define LORA_PORT_IDX    3	// COM3, LoRa
#define UDP_PORT_IDX	9
#define TCPX1_PORT_IDX	10

#define DEF_LORA_IDX		0		// 로라모듈 인덱스

// Mathematics
#define MAX(x,y) (((x)<(y)) ? (y) : (x))
#define ABS(x)		( ((x)<0)?-(x):(x) ) // 절대값

////////////////////////////////////////////////////////////
// Definition
#define MAX_DESC_STR					61 // 51
#define MAX_PWD_STR					11
#define MAX_IP_STR						15
#define MAX_MAC_IP_STR				19
#define MAX_COMMUNITY_STR		11
#define RMAC_BSIZE						6
#define RMAC_HEADER_SIZE			8
#define TP_FRAME_SIZE					60
#define TOPOLOGY_CHECK_TIME	600
#define TP_FRAME_SND_INTERVAL	500
#define MAX_VLAN_ID					4094
#define MAX_SPACKET_STR			110 

//#define MAX_NODE_CNT				255		// 링에 허용되는 최대 노드수는 255이다.
#define MAX_NODE_CNT				64		// 링에 허용되는 최대 노드수는 255이다.
#define MAX_SNODE_CNT				250		// 최대 센서노드수
// #define MAX_SLAVE_CNT			10		// 최대 게이트웨이 슬레이브 수
#define MAX_URSNODE_CNT			30		// 최대 등록되지 않은 센서노드수

#define MAX_TCPCON_CNT 			3		// TCP 접속 허용 개수

#define MAX_SCOMM_CNT				3		// 시리얼 포트 사용 개수
#define MAX_MODULE_CNT			1		// LoRa
#define MAX_SPORT_CNT				1		// RS485  // RS232

#define MAX_SNR_CFG_CNT			20		// 센서노드의 설정 값 개수
#define MAX_MGMT_CNT				10		// 플랫폼에서 MGMT를 이용한 설정 가능 동시 최대값

#define MAX_LWM_SDATA_CNT		100		// LWM2M 장비에서 데이터 최대수
#define MAX_STYPE_CNT				7		// LWM2M 장비 형식 개수
#define MAX_PAYLD_CNT				10		// 센서에서 최대 10개의 Payload를 보낼 수 있다.
#define MAX_485_DEV_CNT			10		// 422/485통신 포트에서 통신 가능한 최대 장비 개수
#define MAX_NAT_PFWD_CNT		40		// NAT 포트포워딩 설정 개수
#define MAX_ROUTE_TBL_CNT		5		// 라우팅 주소 테이블 설정 개수
#define MAX_PDS_CNT					2		// 최대 PD센서 데이터 수신 개수
#define PD_SAVE_FILE_CNT			20

// Platform
#define PLAT_PERIOD_MIN 20

// log
#define LOG_Q_SIZE 250			// 500개를 잡았는데, 플래쉬가 16K이상을 쓰는데 문제가 있다. 그래서 크기를 줄인다.
#define SDLOG_Q_SIZE 8192

enum ResetModeDef{
	YET_RESET_WARM	= 0x00,
	YET_RESET_COLD		= 0x01,
};

enum OperationModeIDDef{
	YET_DOM_EXTERNAL	= 0x00,
	YET_DOM_INTERNAL	= 0x01,
};

enum OperationEthIDDef{
	YET_ETH_AUTONEG	= 0x00,
	YET_ETH_10_HALF		= 0x01,
	YET_ETH_10_FULL		= 0x02,
	YET_ETH_100_HALF	= 0x03,
	YET_ETH_100_FULL	= 0x04,
	YET_ETH_1000_HALF	= 0x05,
	YET_ETH_1000_FULL	= 0x06,
	YET_ETH_OFF			= 0x07,
};

enum LoraStepID{
	YET_LORA_PS_IDLE,				// 모듈 통신 정상, 모든 설정이 마무리됨
	YET_LORA_PS_FAIL,				// 모듈 통신 이상
	YET_LORA_PS_WAIT_ALIVE,	// 모듈이 존재하는지 확인 중
	YET_LORA_PS_EUI,				// 모듈의 EUI 확인 중
	YET_LORA_PS_RF_FREQ,		// LoRa 주파수 채널 설정중
	YET_LORA_PS_SP_FACTOR,	// LoRa Spreading Factor 설정중
	YET_LORA_PS_USE_KEY,		// LoRa 암호화 여부 설정중
	YET_LORA_PS_AES_KEY,		// LoRa AES 키 설정중
	YET_LORA_PS_TXP,				// LoRa 송출 전력 
	//YET_LORA_PS_SENDING,     // Snode 데이터 전송중
};

enum DeviceType{
	YET_DT_CHASSIS			= 0x00,
	YET_DT_STANDALONE	= 0x01,
};

enum ChassisModelIDType{
	YET_CM_SOTS				= 0x00,
};

enum STModelIDType{
	YET_STM_IPTU_COT		= 0x00,
	YET_STM_IPTU_SLOT		= 0x01,
	YET_STM_SOTS_RT		= 0x02,
	YET_STM_IPTU_RT		= 0x03,
	YET_STM_IOTGW_RT		= 0x04,
};

enum EventLogTypeDef{
	YET_ELT_NORMAL		= 0,
	YET_ELT_LINKDOWN	= 1,
	YET_ELT_LINKUP		= 2,
};

enum RingMACFrameID{
	YET_RMFID_NORMAL	= 0x00,
	YET_RMFID_TP			= 0x01,
};

enum DeviceID{
	YET_DID_IPTU_COT	= 0x00,
	YET_DID_IPTU_SLOT	= 0x01,
	YET_DID_IPTU_RT		= 0x02,
	YET_DID_SOTS_OCU	= 0x10,
	YET_DID_SOTS_RT	= 0x11,
	YET_DID_IOTGW		= 0x20,
};

enum RFModuleID{
	YET_RFID_ZIGBEE	= 0x00,
	YET_RFID_LORA	= 0x01,
	YET_RFID_UDP	= 0x02,
	YET_RFID_WISUN	= 0x03,
	YET_RFID_GPS	= 0x04,
	YET_RFID_SPORT	= 0x05,
	YET_RFID_TCPX	= 0x06,
	YET_RFID_BPLANE	= 0x07,  // YET_BVT_Z010
};

typedef struct YST_LOG_DATA{
	DDWORD dwTime;
	BYTE ucType;				// 이벤트 형태, EventLogTypeDef
	BYTE ucReserved[3];
	char strMessage[MAX_DESC_STR+1];
} YST_LOG_DATA;

typedef struct YST_DB_LOGQ{
	YST_LOG_DATA Data[LOG_Q_SIZE];
	INT16 nHead;
	INT16 nTail;
	DWORD dwCRCValue;
} YST_DB_LOGQ;

//////////////////////////////////////////////////////////
// SDataLog.dat
typedef struct YST_SDLOG_DATA{// 128bytes x 8192 = 1MBytes
	WORD wCRCValue;
	INT16 nRSSI;			// RF신호세기
	BYTE ucFlag;			// B2~B0(센서사용RF) - 0:ZIGBEE, 1:LORA, B5~B3(GW사용RF) - 0:ZIGBEE, 1:LORA, 2:SlvLORA, B7~B6(데이터종류) - 0:SYS, 1:S-ERR, 2:S-DATA, 3:S-ETC
	BYTE mEUI[3];			// 32-bit IEEE address
	DDWORD dwTime;
	char szMsg[112];
} YST_SDLOG_DATA;

typedef struct YST_SD_LOGQ{
	YST_SDLOG_DATA Data[SDLOG_Q_SIZE];
	INT16 nHead;
	INT16 nTail;
	// DDWORD dwLastSaveTime;
#ifdef USE_CFI_FLASH
	BYTE bNoSave[SDLOG_Q_SIZE];
	BYTE bReqSave;
#endif
	DWORD dwCRCValue;
} YST_SD_LOGQ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define Agent Database Struct

typedef struct YST_DB_GENERAL{
	BYTE mIP[4];						// IP주소
	BYTE mSM[4];						// 넷마스크
	BYTE mGW[4];						// 게이트웨이
	BYTE mIP2[4];						// IP주소-2
	BYTE mMAC[6];						// MAC주소
	char szPassword[MAX_PWD_STR+1];		// 접근암호	
	BYTE ucDMRMode;						// DMR 사용여부, 0:사용안함, 1:사용함, 이 플래그는 사용X => DMR모드시 10초에 한번씩 usb0인터페이스를 점검하려고 했으나, 수동으로 usb0인터페이스를 점검하도록 함.
	BYTE ucGWIf;						// Default GW 인터페이스, 0:wan, 1:halow

	BYTE mDNS[4];						// DNS ip주소
	BYTE bNTP;							// NTP 사용유무
	BYTE ucOperMode;					// 동작모드, 0:외장형, 1:내장형
	BYTE ucOperEth0;					// 속도 설정
	BYTE ucOperEth1;					// 속도 설정
	BYTE bPCommMode;				// Platform Comm Mode, 0:Ethernnet, 1:Serial
	BYTE bTelnet;						// Telnet 사용유무
	BYTE bFtp;							// Ftp 사용유무

#ifdef DEF_RTCP
	BYTE mManMAC1[6];				// 접속가능 관리PC MAC주소
	BYTE mManMAC2[6];				// 접속가능 관리PC MAC주소
#endif

#ifdef DEF_HALOW
	BYTE ucHMode;							// Halow 동작모드, 0:사용안함, 1:Station, 2:AP, 3:Dual => 3:Mpp, 4:Mp
	BYTE ucHChannel;						// Halow 채널, 1MHz:1/3/4/6, 2MHz:2/5
	BYTE mHMIP1[4];						// Halow AP용 IP주소
	BYTE ucHMSEC1;						// Halow AP용 Security
	char szHMSSID1[20];					// Halow AP용 SSID
	BYTE ucHMDHCP2;						// Halow Station IP, 0:static, 1:DHCP
	BYTE mHMIP2[4];						// Halow Station용 IP주소
	BYTE ucHMSEC2;						// Halow Station용 Security
	char szHMSSID2[20];					// Halow Station용 SSID(상대방SSID)
	BYTE ucHMDHCP3;						// Halow Mesh IP, 0:static, 1:DHCP
	BYTE mHMIP3[4];						// Halow Mesh용 IP주소
	BYTE ucHMSEC3;						// Halow Mesh용 Security
	char szHMSSID3[20];					// Halow Mesh용 SSID(상대방SSID)
#endif
} YST_DB_GENERAL;

typedef struct YST_DB_NAT_PFWD{// NAT 포트 포워딩 관련 설정
	BYTE mPFWDIP[4];					// 포워딩을 할 로컬 IP
	int nSrcPort;						// NAT 서버에서의 소스 포트 번호
	int nDestPort;						// 클라이언트 로컬 IP에서의 포워딩할 포트 번호.
	BYTE ucProtocol;					// 포트포워딩 사용 프로토콜, 0:사용안함, 1:TCP, 2:UDP
} YST_DB_NAT_PFWD;

typedef struct YST_DB_ROUTE_TBL{// 다른 네트워크에 대한 라우팅 주소 테이블
	BYTE ucUse;							// 라우팅 사용 여부, 0:사용안함, 1:사용함
	BYTE mRNet[4];						// 라우팅을 할 네트워크, ex) 192.168.10.x
	BYTE mRGW[4];						// 라우팅을 할 GW, ex) 192.168.11.2
} YST_DB_ROUTE_TBL;

typedef struct YST_DB_CHASSIS{
	BYTE ucModelID;						// 0
	char szDevName[36];					// Device Name
	char szSerialNo[MAX_DESC_STR+1];	// 새시 제조 일련번호 ex) JSNB101012001
	char szPoleNo[MAX_DESC_STR+1];		// 전주번호
	char szMangeNo[MAX_DESC_STR+1];		// 전산화번호
	char szInstallDate[MAX_DESC_STR+1];	// 설치일자
} YST_DB_CHASSIS;

typedef struct YST_DB_RMAC{
	BYTE ucDevID;			// 모듈종류, 0x00:IPTU-1U, 0x01:IPTU-CARD, 0x02:IPTU-Slave, 0x10:SOTS-OCU, 0x11:SOTS-Slave
	BYTE mFWVersion[4];		// FW Version, [0]=>Year, [1]=>Month, [2]=>Day, [3]=>Build
	BYTE mChipID[4];		// Chip ID & Version, 3210B720 => 3.2.1 0B/07/20
	BYTE mRMAC[6];			// 링맥주소
	BYTE mManIPAddr[4];		// 관리 IP 주소
	BOOL bClosedRing;		// Topology상태, 0:OpenRing, 1:ClosedRing
	BYTE mLink[4];			// East/West/Host 포트의 Link 상태, East:포트0,오른쪽, West:포트1,왼쪽, 0:LinkFail, 1:10Mbps-Half, 2:10Mbps-Full, 3:100Mbps-Half, 4:100Mbps-Full, 5:1Gbps-Half, 6:1Gbps-Full
	BYTE mMiscable[2];		// East/West 포트의 Miscable 상태, 0:Normal, 1:Miscalble
	BYTE mNodeCount[2];		// East/West 방향의 노드수
	
	WORD wPollInt;					// 폴링주기 sec
	DDWORD dwPollingTime;	// Milli Seconds, UART로 데이터를 출력한 마지막 시간.
} YST_DB_RMAC;

typedef struct YST_DB_PLATFORM{
	BYTE bAssignOID;		// G/W의 OID를 할당 받은 경우 1
	BYTE bRegGW;			// G/W CSE의 플랫폼 정상 등록 여부, 플래쉬 저장용
	BYTE bRegNode;			// G/W Node의 플랫폼 정상 등록 여부
	BYTE bRegCSE;			// G/W CSE의 플랫폼 정상 등록 여부
	BYTE bRegLocS;			// G/W 위치보고의 플랫폼 정상 등록 여부
	BYTE bRegStatus;		// G/W 상태보고의 플랫폼 정상 등록 여부
	BYTE bRegLocP;			// G/W 위치정책의 플랫폼 정상 등록 여부
	BYTE bRegDevInfo;		// G/W 장비정보 보고의 플랫폼 정상 등록 여부
	BYTE bRebootWait;		// 상위단 reboot 명령이 패킷 처리 완료를 기다리는 경우 1
	BYTE bRptGWLoc;			// 상위단에 GW Location 보고를 수행한 경우 1
	BYTE bRptDevInfo;		// 상위단에 장비정보 보고를 수행한 경우 1
	BYTE bRptStatus;       // 상위단에 상태 보고를 수행한 경우 1
	BYTE bContainerReporting; // 상위단에 상태보고를 수행하고 있는 경우 1, 다른 리포팅을 해야하는지 여부를 결정
	BYTE bChangedOID;		// G/W의 OID가 수정된 경우 1
	BYTE ucRegStatus;		// LWM2M용 플랫폼 등록 상태, 서버마다 다르지만 1개의 서버만 유효함
	BYTE mMyIP[4];			// 자기 IP주소
	BYTE mPeerIP[4];		// 플랫폼 IP주소
	WORD wMyPort;			// 자기 Listen 포트번호
	WORD wPeerPort;			// 상대 Listen 포트번호
	char szGWOID[40];		// G/W OID
	char szNID[40];			// G/W 노드 ID
	BOOL bCommOK;			// 플랫폼이 정상적으로 응답한 경우 TRUE
	BYTE ucPStep;			// 플랫폼 송수신 처리 단계
	BYTE ucSndCnt;			// 플랫폼으로 송신한 회수
	BYTE ucM2MType;			// M2M Type, 0:oneM2M, 1:LWM2M
	BYTE ucM2MVer;			// M2M Ver, 0:R0, 1:R1, 2:R2
	WORD wSeqRQI;			// oneM2M RQI 일련번호
	WORD wRptTime;			// 상태보고 주기
	DWORD dwSeqToken;		// CoAP 토큰 일련번호
	char szRQI[16];			// oneM2M RQI 일련번호
	char szURIPath[40];		// URI, Uniform Resource Identifier Path, ex) Mobius, ~/oneM2M/Tester
	char szOIDBase[20];		// 기본 OID, ex)1.2.481.1.
	char szLWLocation[60];	// LWM2M Location, 서버마다 다르지만 1개의 서버만 유효함
	char szCSEID[20];		// AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
#if 1
	BYTE bRegControl;			// G/W 제어정보의 플랫폼 정상 등록 여부
	BYTE bRegMgmt;			// G/W mgmtCmd의 플랫폼 정상 등록 여부
	BYTE bRptGWControl;		// 상위단에 GW Control 보고를 수행한 경우 1
#endif
#if 1
	BYTE bCtrlPeriod;			// 상위단에서 주기보고 변경이 설정된 경우 1, 상위단에 전달하면 0
	BYTE bCtrlReboot;			// 상위단에서 reboot이 설정된 경우 1, 상위단에 전달하면 0
	BYTE bCtrlLoc;				// 위치보고
	BYTE bCtrlInfo;				// 상위단에서 장비정보보고 요청이 설정된 경우 1, 상위단에 전달하면 0
	BYTE bCtrlControl;			// 제어보고
	BYTE bCtrlReport;			// 상위단에서 상태보고 요청이 설정된 경우 1, 상위단에 전달하면 0
	BYTE bCtrlSave;				// 저장
	char szCtrlDat[62];			// 상위단에서 날짜시간 변경이 설정된 경우 ex) 2021/08/01 01:02:03.004,  상위단에 전달하면 NULL
	BYTE bCtrlLfr;				// LoRa 주파수
	BYTE bCtrlLsf;				// LoRa 스프레딩 팩터
	BYTE bCtrlLea;				// LoRa 암호 여부
	BYTE bCtrlLek;				// LoRa 암호 키
	char szCtrlLtmp[20];		// LoRa 제어문 저장
#endif
#if 1
	WORD wBlockSize;			// 0:2048, 1:1024, 2:512  
	BYTE bSec;					// DTLS 사용 여부
	BYTE bSecDebug;			// DTLS 디버깅 level
	char szSecPW[20];			// DTLS Password
#endif
	DDWORD dwRebootRcvTime;	// Milli Seconds, 플랫폼으로 부터 재부팅 명령을 받은 시간.
	DDWORD dwLastRcvTime;	// Milli Seconds, 플랫폼으로 부터 데이터가 입력된 마지막 시간.
	DDWORD dwLastSndTime;	// Milli Seconds, 플랫폼으로 데이터를 출력한 마지막 시간.
	DDWORD dwLastRptTime;	// Milli Seconds, 플랫폼으로 실시간 데이터를 전송한 마지막 시간.
} YST_DB_PLATFORM;

typedef struct YST_DB_BLOCK_WISE{
	BYTE bSndingBWF;		// Blockwise 파일 송신을 하는 중
	BYTE bRcvingBWF;		// Blockwise 파일 수신을 하는 중
	BYTE bWaitAck;			// Block 전송에 대한 응답을 받아야 하는 경우 1 
	DDWORD dwWaitAckSTime;	// Milli Seconds, 응답을 기다리기 시작한 시간.
	WORD wBlkIdx;			// 전송 Block 인덱스
	WORD wTBlkCnt;			// 전체 전송 Block 수
	int nSIdx;				// 센서 인덱스
	int nSndFileSize;		// 송신 데이터 저장 파일크기
	int nRetryCnt;			// 재전송 시도회수
	void *pCoapMsg;			// Coap 패킷 저장용
	int nCoapMsgOptIdx;		// Coap 패킷 최종 Option Index
	char szSndFileName[50];	// 송신 데이터 저장 파일이름
	char szRcvFileName[50];	// 수신 데이터 저장 파일이름
} YST_DB_BLOCK_WISE;

typedef struct YST_DB_WISUN{
	BYTE bUseEnDec;		// 송수신 암호화를 수행하는 경우 1
	BYTE ucRFChan;			// 주파수 채널(0~31)
	WORD wSAddr;			// Device 의 ShortAddress, 0~0xFFFD, 코디네이터는 무조건 0
	WORD wPanID;			// Private Area Network ID, 구성할 네트워크 ID
	BYTE mEUI[8];			// 64-bit IEEE address
} YST_DB_WISUN;

typedef struct YST_DB_LORA{
	BYTE ucRFFreq;			// 주파수 채널(0~15), 0:920.3MHz, 0.2MHz 증가
	BYTE ucSPFactor;		// Spreading Factor 값(7~12), 값이 높을 수록 도달 거리는 늘어나며, 속도는 감소함
									// 7:5.4kbps -125dB, 8:3.1kbps -128dB, 9:1.7kbps -131dB, 10:0.9kbps -134dB, 11:0.5kbps -136dB, 12:0.3kbps -137dB
	BYTE bUseEnDec;		// 송수신 암호화를 수행하는 경우 1
	char szLoraKey[20];	// LoRA 송수신 암호화 키 16바이트
	BYTE mEUI[8];			// 64-bit IEEE address
	BYTE ucTxp;				// 송출	전력(0-14dBm)
} YST_DB_LORA;

typedef struct YST_DB_SCOMM{ 
	WORD wDelayTime;		// Milli Seconds, UART로 부터 입력된 데이터를 하나의 패킷으로 전송하기 위해 기다리는 시간.
							// 1200bps에서 1바이트는 8ms정도 걸리므로, 최소값을 20ms로 잡자.
	WORD wMaxPktSize;		// UART로 부터 입력된 데이터가 이 숫자보다 크면 DelayTime이전에도 패킷으로 전송한다.
	BYTE ucPortID;			// 1:COM1 ~ 10:COM0
	BYTE ucBaudrate;		// 1:B2400, B4800, B9600, B19200, B38400, B57600, B115200
	BYTE ucDataBits;		// 1:CS5, CS6, CS7, CS8
	BYTE ucStopBits;		// 1:1bit, 2:2bits
	BYTE ucParity;			// 1:NoParity, 2:OddParity, 3:EvenParity
	char szCommStatus[50];
} YST_DB_SCOMM;

typedef struct YST_DB_MODULE{ 
	BYTE ucMType;			// 0:Zigbee, 1:LoRA, 2:UDP, 2:Wi-SUN, 3:UART, 4:GPS
	BOOL bModuleOK;			// 모듈이 정상적으로 응답한 경우 TRUE
	BYTE ucPStep;			// 송수신 처리 단계
	BYTE ucSndCnt;			// 송신 회수
	DDWORD dwLastRcvTime;	// Milli Seconds, UART로 부터 데이터가 입력된 마지막 시간.
	DDWORD dwLastSndTime;	// Milli Seconds, UART로 데이터를 출력한 마지막 시간.
	char szLastSnd[50];		// 최종으로 내린 명령을 저장한다. 응답 체크용
} YST_DB_MODULE;

typedef struct YST_DB_SPORT{ 
	BYTE ucDevCnt;			// 병렬로 접속된 디바이스 개수
	BYTE ucDevPollIdx;		// 현재 폴링을 한 디바이스
	BYTE ucSSSNPollIdx;		// 현재 폴링을 한 SSSN
	WORD wPollInt;			// 폴링주기 sec
	WORD wWaitTime;			// 장비 응답 대기시간, ms
	BYTE ucSSSNCnt[MAX_485_DEV_CNT];// 장비별 SSSNN 통신 개수
	DDWORD dwPollingTime;	// Milli Seconds, UART로 데이터를 출력한 마지막 시간.
	DDWORD dwLastSndTime;	// Milli Seconds, UART로 데이터를 출력한 마지막 시간.
} YST_DB_SPORT;


typedef struct YST_DB_NODE{
	BYTE ucDevID;			// 모듈종류, 0x00:IPTU-1U, 0x01:IPTU-CARD, 0x02:IPTU-Slave, 0x10:SOTS-OCU, 0x11:SOTS-Slave
	BYTE ucFWVersion;		// FW Version
	BYTE bClosedRing;		// Topology상태, 0:OpenRing, 1:ClosedRing
	BYTE mLink[4];			// 각 노드의 East/West/Host 포트의 Link 상태
	BYTE mMiscable[2];		// East/West 포트의 Miscable 상태
	BYTE mRMAC[RMAC_BSIZE];	// 각 노드의 링맥주소
	BYTE mManIPAddr[4];		// 관리 IP 주소
} YST_DB_NODE;

typedef struct YST_DB_SLAVE{
	BYTE bCommFail;			// 0:Comm. OK, 1:Comm. Fail
	BYTE bRcvGFSET;			// GFSET 패킷을 수신한 경우 1
	BYTE bRcvGCNFG;			// GCNFG 패킷을 수신한 경우 1
	BYTE bCtrlCfg;			// 설정변경을 요청한 경우, 슬레이브에 전달하면 0
	INT16 nRSSI;			// RF신호세기
	BYTE mEUI[8];			// 64-bit IEEE address	
	WORD wRptTime;			// 상태보고 주기, 단위:sec
	DDWORD dwLastRcvTime;	// 마지막 정보 수신시간
	char szSlvOID[36];		// 슬레이브 OID
	char szManufacture[20];	// 제조사
	char szDevName[20];		// 제품명
	char szFWVer[20];		// F/W Version정보
	char szPayload[128];	// 수신 데이터
	char szSCNFG[MAX_SNR_CFG_CNT][30];	// 설정 값
} YST_DB_SLAVE;

typedef struct YST_DB_SNODE{
	BYTE bRegSNode;			// 센서노드의 플랫폼 정상 등록 여부, 플래쉬 저장용
	BYTE bRegNode;			// 센서 Node의 플랫폼 정상 등록 여부
	BYTE ucRegSNodeStatus;	// LWM2M용 센서 Node의 플랫폼 등록 상태, 서버마다 다르지만 1개의 서버만 유효함
	BYTE bRegAE;			// 센서 AE의 플랫폼 정상 등록 여부
	BYTE bRegStatus;		// 센서 상태보고의 플랫폼 정상 등록 여부
	BYTE bRegDevInfo;		// 센서 장비정보 보고의 플랫폼 정상 등록 여부
	BYTE bRegMgmt;			// 센서 mgmtCmd의 플랫폼 정상 등록 여부
	BYTE bCtrlReboot;		// reboot이 설정된 경우 1, 센서에 전달하면 0
	BYTE bCtrlPeriod;		// period가 설정된 경우 1, 센서에 전달하면 0
	BYTE bCtrlDevInfo;		// 상위단에서 장비정보 보고 요청이 설정된 경우 1, 상위단에 전달하면 0
	BYTE bRptRebootResult;	// 상위단에서 reboot에 대한 제어 결과를 송신해야 되는 경우 1
	BYTE bRptPeriodResult;	// 상위단에서 period에 대한 제어 결과를 송신해야 되는 경우 1
	BYTE bCtrlCfg;			// 설정변경을 요청한 경우, 센서에 전달하면 0
	BYTE bCtrlDst;			// G/W변경을 요청한 경우, 센서에 전달하면 0
	BYTE bCtrlTime;			// 시간변경을 요청한 경우, 센서에 전달하면 0
	BYTE bRcvSFSET;			// SFSET 패킷을 수신한 경우 1
	BYTE bRcvSCNFG;			// SCNFG 패킷을 수신한 경우 1
	BYTE bRcvSName;			// SNAME(SSSNR, SSSND) 패킷을 수신한 경우 1
	BYTE bUseSName;			// SNAME을 사용하는 경우 1
	BYTE bRcvAllPkt;		// 모든 상태 패킷을 수신한 경우 1, mRcvPayload[]가 모두 1이어야 함.
	BYTE ucSlvIdx;			// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
	BYTE ucPIdx;			// 0:Ukn, 1:Wi-SUN, 10~18:Lora, 99:UDP
	BYTE ucRFModule;		// 0:Wi-SUN, 1:Lora
	BYTE ucSFactor;			// Lora Spreading Factor
	BYTE bCommFail;			// 0:Comm. OK, 1:Comm. Fail
	BYTE ucLWMType;			// LWM2M 센서 형식, YST_DB_LWM2M_TYPE 참조
	BYTE ucSSSNCnt;			// SSSNN 통신 개수
	INT16 nRSSI;			// RF신호세기
	WORD wRptTime;			// 상태보고 주기, 단위:sec
	DDWORD dwLastRcvTime;	// RF모듈에서 마지막으로 정보를 수신한 시간
	DDWORD dwLastSndTime;	// 플렛폼으로 등록요청을 송신한 시간
	DDWORD dwFRegTime;		// 처음으로 데이터를 수신하여 G/W에 등록된 시간
	BYTE mEUI[8];			// 64-bit IEEE address
	BYTE mRcvPayload[MAX_PAYLD_CNT];// 수신 데이터 존재여부
	BYTE mRcvNewStatus[MAX_PAYLD_CNT];	// 센서 상태보고가 정상적으로 된 경우 0
	char szManufacture[20];	// 제조사
	char szDevName[20];		// 제품명
	char szFWVer[20];		// F/W Version정보
	char szDestGW[32];		// 센서에게 G/W변경 요청을 할 때 사용할 변경 채널 및 G/W EUI
	char szOID[40];			// Sensor OID
	char szNID[40];			// Sensor 노드 ID
	char szPayload[MAX_PAYLD_CNT][256];	// 수신 데이터
	char szUkPayload[256];	// 형식이 맞지 않은 수신 데이터
	char szSCNFG[MAX_SNR_CFG_CNT][30];	// 센서 설정 값
	char szSNodeName[36];	// 센서노드 이름
	char szSDataName[256];	// 센서데이터 이름, SNAME
	char szLWLocation[60];	// LWM2M Location, 서버마다 다르지만 1개의 서버만 유효함
#if 1
	BYTE bRegControl;		// 센서 SNR
	BYTE bRegSctrl;			// 센서 SCTRL
	BYTE bRegScnfg;		// 센서 SCNFG
	BYTE bRptDevInfo;
	BYTE bRptControl;
	BYTE bRptSctrl;
	BYTE bRptScnfg;
	BYTE bCtrlReport;
	BYTE bCtrlDelete;
	BYTE ucPStep;			// 현재 송수신 처리 단계
	char bCtrlPacket[101];
#endif
} YST_DB_SNODE;

typedef struct YST_DB_URSNODE{
	BYTE ucSlvIdx;			// 연결된 슬레이브 인덱스, 0:마스터에 직접 연결됨
	BYTE ucPIdx;			// 0:Ukn, 1:Zigbee, 2:Lora센서, 3:Lora슬레이브
	INT16 nRSSI;			// RF신호세기
	DDWORD dwLastRcvTime;	// 마지막 정보 수신시간
	BYTE mEUI[8];			// 64-bit IEEE address
	char szOID[40];			// Sensor OID
	char szPayload[128];	// 수신 데이터
	char szErrReason[32];	// 오류 이유
} YST_DB_URSNODE;

typedef struct YST_DB_RCV_PKT_TABLE{
	BOOL bUsed;						// 이 테이블을 참조하는 노드가 있으면 TRUE
	BYTE ucReserved;
	BYTE RMAC[RMAC_BSIZE];			// 수신한 패킷의 Ring Mac주소가 이 링맥주소와 같아야한다.
} YST_DB_RCV_PKT_TABLE;

// 내부온습도 표출
typedef struct YST_DB_ITH{
	BYTE bValid;						// 정상 수신한 경우
	double fInternalTemp;	        // 대기 온도
	double fInternalHumidity;		// 대기 습도
} YST_DB_ITH;

#if 1
typedef struct YST_DB_GPS{
	char szLatitude[20];				// 위도, 단위:1/60도
	char szLongitude[20];				// 경도, 단위:1/60도
	char szAltitude[20];				// 해발고도, 단위:m
} YST_DB_GPS;
#else
typedef struct YST_DB_GPS{
	BYTE bValid;						// GPS 신호를 정상 수신한 경우
	double fLatitude;					// 위도, 단위:1/100도
	double fLongitude;					// 경도, 단위:1/100도
	double fAltitude;					// 해발고도, 단위:m
//	double fTemperature;				// RTC에서 읽은 온도
	char szLatitude[20];				// 위도, 단위:1/60도
	char szLongitude[20];				// 경도, 단위:1/60도
	char szAltitude[20];				// 해발고도, 단위:m
	char szDate[20];					// 최종수신 날짜
	char szTime[20];					// 최종수신 시간
} YST_DB_GPS;
#endif

// 기울기 표출
typedef struct YST_DB_TILT{
	BYTE bValid;				// 정상 수신한 경우
	double fTiltStart;		// 최초 기울기
	double fTiltNow;	        // 현재 기울기
	double  fTiltEvent;		// 기울기이벤트 최소값
} YST_DB_TILT;

typedef struct YST_DB_LWM_SDATA{
	WORD wObjID;			// LWM2M Object ID
	WORD wInstID;			// LWM2M Instance ID
	WORD wResID;			// LWM2M Resource ID
	WORD wResInstID;		// LWM2M Resource Instance ID
	BYTE ucDType;			// 0:int, 1:float
	BYTE ucPktID;			// NMEA Packet ID, 0 ~ MAX_PAYLD_CNT-1
	char szDName[4];		// NMEA Packet Data Name
	char szDesc[20];		// Sensor Data Description
} YST_DB_LWM_SDATA;

typedef struct YST_DB_LWM2M_TYPE{
	BYTE ucDType;			// LWM2M 센서 형식, 1부터 사용
	BYTE ucPCnt;			// NMEA로 수신해야 하는 패킷 개수
	BYTE ucDCnt;			// YST_DB_LWM_SDATA 개수
	YST_DB_LWM_SDATA SData[MAX_LWM_SDATA_CNT];
} YST_DB_LWM2M_TYPE;

//==============================================================
// PD
typedef struct YST_DB_PDS_DATA{// PD 센서 데이터
	BYTE bValid;			// PD센서가 유효한 경우 1
	BYTE ucSNodeIdx;		// SNode 0번을 사용한다.
	BYTE bRcvdPVer;			// PD센서에서 프로토콜 정보를 수신한 경우 1
	BYTE bRcvdDevInfo;		// PD센서에서 장비정보를 수신한 경우 1
	BYTE bRcvdCfg;			// PD센서에서 config값을 수신한 경우 1
	BYTE bRcvdTime;			// PD센서에서 시간 값을 수신한 경우 1
	BYTE bRcvdStatus;		// PD센서에서 상태 값을 수신한 경우 1
	BYTE bRcvdData;			// PD센서에서 데이터 값을 수신한 경우 1
	BYTE bNeedTimeSync;		// PD센서의 시간동기화가 필요한 경우 1
	BYTE bNeedCfgSet;		// PD센서의 config설정이 필요한 경우 1
	BYTE bTimeSyncIng;		// PD센서의 시간동기화 요청을 수행하고 있는 경우 1
	BYTE bCfgSetIng;		// PD센서의 config설정 요청을 수행하고 있는 경우 1
	BYTE bWaitRcvAck;		// Ack 수신을 기다리는 경우 1
	BYTE bWaitRcvSubs;		// Subscribe 수신을 기다리는 경우 1
	BYTE bReqSndTryCnt;		// PD센서의 시간동기화/config설정 요청을 수행한 횟수
	BYTE ucCfgIdx;			// 설정값 수신 인덱스.
	BYTE ucStsIdx;			// 상태값 수신 인덱스.
	void *pSockInfo;		// 접속된 브라우져의 소켓 정보
	void *pXmlST;			// CXMLStreamParser 정보
	DDWORD dwLastReqTime;	// PD센서에 정보를 요청한 시간
	DDWORD dwLastHBTime;	// PD센서에 HeartBit를 전송한 시간
	DDWORD dwLastRcvTime;	// PD센서에서 데이터를 수신한 시간
	DDWORD dwLastSndTime;	// PD수신데이터를 플랫폼에 전송 완료한 시간
	char szProtoVer[20];	// TCP/XML protocol version
	char szPDFileName[50];	// 수신된 PD 데이터가 저장된 파일이름
	char szManufacture[20];	// 제조사
	char szDevName[20];		// 제품명
	char szFWVer[20];		// F/W Version정보
	char szOID[40];			// Sensor OID
	char szCFGN[MAX_SNR_CFG_CNT][30];	// 설정 값 이름
	char szCFGD[MAX_SNR_CFG_CNT][30];	// 설정 값 데이터
	char szStsN[MAX_SNR_CFG_CNT][30];	// 상태 값 이름
	char szStsD[MAX_SNR_CFG_CNT][30];	// 상태 값 데이터
} YST_DB_PDS_DATA;

#if 0
typedef struct YST_DB_PD_SENSOR{	// PD 수집부
	BYTE ucMode;					// 0:PRPD,  1:PRPS
	BYTE ucSync;						// 0:External,  1:Internal
	DDWORD dwLastRcvTime;	// PD센서 데이터를 수집한 시간
	WORD wRptTime;				// 수신 주기, 단위:sec
	WORD wMonPort;
} YST_DB_PD_SENSOR;
#endif

#if 1
enum _enum_adc_format {
	F_COMPLE = FALSE,
	F_BINARY = TRUE,
};

enum YST_MeasureType_Def{
	YST_PQN		= 0xD1,	
	YST_SCOPE	= 0xD2,	
	YST_ETC		= 0xD5,
};

enum YST_SensorType_Def{
	YST_CH13_UHF			= 0x01,
	YST_CH13_AE			= 0x02,
	YST_CH13_GAIN			= 0x04,
	YST_CH13_ENVELOP	= 0x08,
	YST_CH24_UHF			= 0x10,
	YST_CH24_AE			= 0x20,
	YST_CH24_GAIN			= 0x40,
	YST_CH24_ENVELOP	= 0x80,
};

typedef struct YST_PD_CH_MAIN{
	char	sync_sel;			// 내부(1)/외부(0) 싱크 선택
	char	sync_sel_val;		// 레퍼런스 단계보냄 (-10 ~10)
	char	ch_on;				// 채널 On/Off 함 - 사용하지 않음, 1 : On, 0 : Off
	char	ch_gain;			// 각 채널의 게인값을 설정해줌, 최소 35 (-9 dB) ~ 최대 0 (+26 dB) 1dB Step
	short	trig;					// 각 채널의 트리거 레벨값, 0 ~ 2048
	char	filter_min;			// 필터 로우값, 1 ~ 75
	char	filter_max;			// 필터 하이값, 1 ~ 75
	BYTE	SensorType;		// ch별 한바이트씩
} YST_PD_CH_MAIN;

//
typedef struct YST_PD_PD_DEV{
	DDWORD	dwLastRcvTime;		// PD센서 데이터를 수집한 시간
	WORD		wRptTime;				// 수신 주기, 단위:sec
	WORD		wMonPort;
	BYTE			bTCPValid;				// 패킷 수신
	BYTE			bAdcTCh;				// ADC test channel
	BYTE			bAdcTPattern;		// ADC test pattern
	BYTE			bAdcVirtual;			// ADC virtual
	BYTE			packet_type;			// 패킷 타입, 3개중 1개(Setting, Measure, Reset)
	int				packet_size;			// 패킷 사이즈
	BYTE			ucMode;				// 0xD1:PQN, 0xD2:SCOPE, 0xD5:ETC
	BYTE			ucFormat;				// ADC Data format,   0:complement, 1:binary
	BYTE			ucEvent;				// 상위단에서 데이터 요청
	BYTE			ucCollect;				// PD데이터 수집단계
	BYTE			ucCollectOld;			// PD데이터 수집 전단계
	BYTE			ucColRdy;				// PD데이터 수집상태
	BYTE			ucProgress;			// PD 진행상태,  0:대기, 1:수집, 2:분석, 3:전송시작
	DDWORD	ucColTime;
	int				phase;					// EventPosition 발생 검토
	YST_PD_CH_MAIN		ch_set[4];	// 4ch
} YST_PD_PD_DEV;
#endif
// PD
//=======================================================

typedef struct YST_MANAGER_INFO{
	BYTE ucValid;						// 다른 정보가 유효한 정보인가, 네트워크 관제 커넥션이 끊어지면 0으로 설정
	BYTE ucServer;						// 네트워크 관제 커넥션의 상대방이 종류. 0-Client, 1-Server
	BOOL bTimeSync;						// 시간동기화가 이루어져 있는 경우 1, 여러개의 서버와 동기화를 하지 않도록 추가함.
	BYTE mPeerIP[4];					// 네트워크 관제 커넥션의 상대방 IP
	WORD wPeerPortNo;					// 네트워크 관제 커넥션의 상대방 포트번호
	DDWORD dwLastSendTime;				// 네트워크 관제 커넥션에 마지막 정보 전송시간
	void *pSockInfo;					// 접속된 브라우져의 소켓 정보
	int nPIndex;						// UART, CAM 포트번호
	WORD wManID;						// Manager ID
	WORD wManVer;						// Manager Version
} YST_MANAGER_INFO;

typedef struct YST_DB_SYSTEM{
	DWORD dwDebugFlag;						// 디버그 메시지 출력을 위한 플래그, gpLog->dwDebugFlag를 초기화 하기 위해 사용함
	YST_MANAGER_INFO M[MAX_TCPCON_CNT];		// 네트워크를 통하여 접속된 커넥션에 대한 정보.
	BYTE ucManagerCount;					// 네트워크를 통하여 접속된 커넥션의 갯수
	BYTE ucResetStatus;						// 0:Cold Start, External hard reset, 1:Warm Start
	BYTE bDenySvrTimeSync;					// 관리서버의 시간동기화를 거부하는 경우 1
	DDWORD dwLWM2MUpdateTime;				// LWM2MAgent가 정상동작하는지 체크여부
	BYTE ucHalowMode;						// HalowModule의 초기화가 된 경우 1
	BYTE bNewSensorExist;					// 새로운 센서가 추가된 경우 1, oneM2M플랫폼으로 센서정보를 전달하기 위해 사용함
	BYTE ucPDSaveFileIdx;					// PD 파일이름 인덱스, 쓰고 있는 파일을 송신하지 않기 위해 사용함.
	char szPDFileName[50];					// 수신될 PD 데이터를 저장하기 위한 파일이름
	BYTE ucPDSCfgCnt;						// 설정값 송신 대상 수
	char szSCFGD[MAX_SNR_CFG_CNT][30];		// 설정 값 데이터
	char szSCFGN[MAX_SNR_CFG_CNT][30];		// 설정 값 이름
	char szSCFGOID[MAX_SNR_CFG_CNT][40];	// Sensor OID
	WORD wAutoRebootMin;					// 자동 재부팅 시간, 분
	DDWORD dwBootTime;						// 시스템 부팅 시간
} YST_DB_SYSTEM;

typedef struct YST_DB_MGMT{
	BYTE bValid;			// RID가 유효한 경우 1
	BYTE ucItem;			// Mgmt제어 항목, 0:reboot, 1:period, 2:info, 3:report
	INT16 nSIdx;			// SNode 인덱스, G/W인 경우 -1
	char szRID[40];			// MGMT 제어 RID
} YST_DB_MGMT;

typedef struct YST_DB_VERSION{
	BYTE ucMajor;							// 주버젼
	BYTE ucMiddle;							// 부버젼
	BYTE ucMinor;							// 빌드버젼
	BYTE ucBoard;							// 보드버젼
	char strDate[MAX_DESC_STR+1];			// 빌드 날짜-시간
	char strDescription[MAX_DESC_STR+1];	// 버젼 변경사항 설명용
} YST_DB_VERSION;

typedef struct YST_DB_MAIN{
	YST_DB_GENERAL				General;
	YST_DB_CHASSIS				Chassis;
	YST_DB_NAT_PFWD			PFwd[MAX_NAT_PFWD_CNT];
	YST_DB_ROUTE_TBL			Route[MAX_ROUTE_TBL_CNT];
	YST_DB_PLATFORM			Platform;

#if 1 // def DEF_RMAC
	YST_DB_RMAC					RMac;
#endif

#ifdef DEF_LORA
	YST_DB_LORA					Lora;
#endif

	YST_DB_SCOMM				SComm[MAX_SCOMM_CNT];

#if MAX_MODULE_CNT == 1
	YST_DB_MODULE				Module;
#else
	YST_DB_MODULE				Module[MAX_MODULE_CNT];
#endif

#if MAX_SPORT_CNT == 1
	YST_DB_SPORT					SPort;
#else
	YST_DB_SPORT					SPort[MAX_SPORT_CNT];
#endif

	YST_DB_SYSTEM				System;

	YST_DB_NODE					Node[2][MAX_NODE_CNT+1];

	YST_DB_SNODE					SNode[MAX_SNODE_CNT];
	// YST_DB_SLAVE				Slave[MAX_SLAVE_CNT];
	YST_DB_URSNODE				URSNode[MAX_URSNODE_CNT];
	YST_DB_RCV_PKT_TABLE	RcvPktTbl[MAX_NODE_CNT+1];
	YST_DB_MGMT					Mgmt[MAX_MGMT_CNT];

#ifdef DEF_ITTH
	YST_DB_ITH						Ith;
#endif

#ifdef DEF_GPS
	YST_DB_GPS						Gps;  // 수기로 입력
#endif

	YST_DB_LWM2M_TYPE		SType[MAX_STYPE_CNT];
	YST_DB_PDS_DATA			PDS[MAX_PDS_CNT];
	YST_DB_VERSION				Version;
	YST_DB_BLOCK_WISE			BlkWise;
//	YST_DB_PD_SENSOR			PdNode;
	YST_PD_PD_DEV			    PdNode;
	YST_DB_LOGQ					Log;
	YST_SD_LOGQ					SData;
} YST_DB_MAIN;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// Define Group ID
enum ObjectGroupIDDef{
	YET_GID_GENERAL	= 0x01,
	YET_GID_CHASSIS,
	YET_GID_NAT_PFWD,
	YET_GID_ROUTE_TBL,
	YET_GID_RMAC,
	YET_GID_PLATFORM,
	YET_GID_WISUN,
	YET_GID_LORA,
	YET_GID_SCOMM,
	YET_GID_SYSTEM,
	YET_GID_NODE,   // 11
	YET_GID_SNODE,
	YET_GID_SLAVE,
	YET_GID_URSNODE,
	YET_GID_VERSION,
	YET_GID_LOG,
	YET_GID_UPGRADE,
	YET_GID_NODE_ALL,
	YET_GID_SNODE_ALL,
	YET_GID_SNODE1,
	YET_GID_SNODE2, // 21
	YET_GID_SNODE3,
	YET_GID_SNODE4,
	YET_GID_STATUS,
	YET_GID_SPORT,
	YET_GID_SENSOR,
	YET_GID_PD_NODE,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// Object ID And Data Structure
// GID : YET_GID_GENERAL ////////////////////////////////////////////////
enum GeneralObjIDDef{
	YET_OID_GeneralTotal = 0,
	YET_OID_IP,
	YET_OID_IP2,
	YET_OID_SM,
	YET_OID_GW,
	YET_OID_MAC,

#ifdef DEF_RTCP
	YET_OID_ManMAC1,
	YET_OID_ManMAC2,
#endif

	YET_OID_Password,
	YET_OID_DMRMode,
	YET_OID_GWIf,

#ifdef DEF_HALOW
	YET_OID_HMode,
	YET_OID_HCH,
	YET_OID_HMDHCP2,
	YET_OID_HMDHCP3,
	YET_OID_HMIP1,
	YET_OID_HMIP2,
	YET_OID_HMIP3,
	YET_OID_HMSEC1,
	YET_OID_HMSEC2,
	YET_OID_HMSEC3,
	YET_OID_HMSSID1,
	YET_OID_HMSSID2,
	YET_OID_HMSSID3,
#endif

	YET_OID_DNS,
	YET_OID_NTP,
	YET_OID_OperMode,
	YET_OID_OperEth,
	YET_OID_PComm,
	YET_OID_TELNET,
	YET_OID_FTP,
};

typedef union GeneralObjData {
	BYTE mIP[4];						// IP주소
	BYTE mSM[4];						// 넷마스크
	BYTE mGW[4];						// 게이트웨이
	BYTE mMAC[6];						// MAC주소
	char szPassword[MAX_PWD_STR+1];		// 접근암호
	BYTE ucDMRMode;						// DMR 사용여부, 0:사용안함, 1:사용함, 이 플래그는 사용X => DMR모드시 10초에 한번씩 usb0인터페이스를 점검하려고 했으나, 수동으로 usb0인터페이스를 점검하도록 함.
	BYTE ucGWIf;						// Default GW 인터페이스, 0:eth0, 1:halow
	char cReserved1[20];

	BYTE mDNS[4];						// DNS ip주소
	BYTE bNTP;							// NTP 사용유무
	BYTE ucOperMode;					// 동작모드, 0:외장형, 1:내장형
	BYTE ucOperEth0;					// eth mode
	BYTE ucOperEth1;
	BYTE bPCommMode;				// Platform Comm Mode, 0:Ethernet, 1:Serial
	BYTE bTelnet;						// TELNET 사용유무
	BYTE bFtp;							// FTP 사용유무
	char cReserved2[20];

#ifdef DEF_RTCP
	BYTE mManMAC1[6];				// 접속가능 관리PC MAC주소
	BYTE mManMAC2[6];				// 접속가능 관리PC MAC주소
#endif

#ifdef DEF_HALOW
	BYTE ucHMode;							// Halow 동작모드, 0:사용안함, 1:Station, 2:AP, 3:Dual => 3:Mpp, 4:Mp	
	BYTE ucHChannel;						// Halow 채널, 1MHz:1/3/4/6, 2MHz:2/5
	BYTE mHMIP1[4];						// Halow AP용 IP주소
	BYTE ucHMSEC1;						// Halow AP용 Security
	char szHMSSID1[20];					// Halow AP용 SSID
	BYTE ucHMDHCP2;						// Halow Station IP, 0:static, 1:DHCP
	BYTE mHMIP2[4];						// Halow Station용 IP주소
	BYTE ucHMSEC2;						// Halow Station용 Security
	char szHMSSID2[20];					// Halow Station용 SSID(상대방SSID)
	BYTE ucHMDHCP3;						// Halow Mesh IP, 0:static, 1:DHCP
	BYTE mHMIP3[4];						// Halow Mesh용 IP주소
	BYTE ucHMSEC3;						// Halow Mesh용 Security
	char szHMSSID3[20];					// Halow Mesh용 SSID(상대방SSID)
#endif

	char cReserved3[28];
} GeneralObjData;

// GID : YET_GID_NAT_PFWD ////////////////////////////////////////////////
enum NATPFwdObjIDDef{
	YET_OID_PF_Total		= 0x00,
	YET_OID_PF_IPAddr		= 0x01,
	YET_OID_PF_SrcPort		= 0x02,
	YET_OID_PF_DestPort		= 0x03,
	YET_OID_PF_Protocol		= 0x04
};

typedef union NATPFwdObjData {
	BYTE mIP[4];					// 포워딩을 할 로컬 IP
	int nSrcPort;					// NAT 서버에서의 소스 포트 번호
	int nDestPort;					// 클라이언트 로컬 IP에서의 포워딩할 포트 번호.
	BYTE ucProtocol;				// 포트포워딩 사용 프로토콜, 0:사용안함, 1:TCP, 2:UDP
	char cReserved[16];
} NATPFwdObjData;

// GID : YET_GID_ROUTE_TBL ////////////////////////////////////////////////
enum RouteTblObjIDDef{
	YET_OID_RTT_Total	= 0x00,
	YET_OID_RTT_Use		= 0x01,
	YET_OID_RTT_RNet	= 0x02,
	YET_OID_RTT_RGW		= 0x03,
};

typedef union RouteTblObjData {
	BYTE ucUse;							// 라우팅 사용 여부, 0:사용안함, 1:사용함
	BYTE mRNet[4];						// 라우팅을 할 네트워크, ex) 192.168.10.x
	BYTE mRGW[4];						// 라우팅을 할 GW, ex) 192.168.11.2
	char cReserved[16];
} RouteTblObjData;

// GID : YET_GID_CHASSIS ////////////////////////////////////////////////
enum ChassisObjIDDef{
	YET_OID_ChassisTotal = 0,
	YET_OID_ChSerialNo,
	YET_OID_PoleNo,
	YET_OID_MangeNo,
	YET_OID_InstallDate,
};

typedef union ChassisObjData {
	char szSerialNo[MAX_DESC_STR+1];	// 새시 제조 일련번호 ex) JSNB101012001
	char szPoleNo[MAX_DESC_STR+1];		// 전주번호
	char szMangeNo[MAX_DESC_STR+1];		// 전산번호
	char szInstallDate[MAX_DESC_STR+1];	// 설치일자
	char cReserved[52];
} ChassisObjData;


// GID : YET_GID_RMAC ////////////////////////////////////////////////
enum RMacObjIDDef{
	YET_OID_RMacTotal = 0,
	YET_OID_RMacRMAC,
	YET_OID_RMacNodeName,
	YET_OID_RMacGet,
};

typedef union RMacObjData {
	BYTE mRMAC[6];			// 링맥주소
	char szNodeName[36];	// 노드이름
	char cReserved[52];
} RMacObjData;

// GID : YET_GID_PLATFORM ////////////////////////////////////////////////
enum PFormObjIDDef{
	YET_OID_PFormTotal = 0,
	YET_OID_MyIP,
	YET_OID_PeerIP,
	YET_OID_PFMyPort,
	YET_OID_PFPeerPort,
	YET_OID_GWOID,
	YET_OID_RegGW,
	YET_OID_RptTime,
	YET_OID_URIPath,
	YET_OID_M2MType,
	YET_OID_M2MVer,
	YET_OID_CSEID,
	YET_OID_BlockSize,
	YET_OID_Sec,
	YET_OID_SecDebug,
	YET_OID_SecPW,
};

typedef union PFormObjData {
	BYTE bRegGW;				// G/W CSE의 플랫폼 정상 등록 여부, 플래쉬 저장용
	BYTE ucM2MType;			// M2M Type, 0:oneM2M, 1:LWM2M
	BYTE ucM2MVer;			// M2M Ver, 0:R0, 1:R1, 2:R2
	BYTE mMyIP[4];				// 자기 IP주소
	BYTE mPeerIP[4];			// 플랫폼 IP주소
	WORD wMyPort;				// 자기 Listen 포트번호
	WORD wPeerPort;			// 상대 Listen 포트번호
	WORD wRptTime;			// 상태보고 주기
	char szGWOID[40];			// G/W OID
	char szURIPath[40];		// URI, Uniform Resource Identifier Path
	char szCSEID[20];			// AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
	WORD wBlockSize;			// 0:2048, 1:1024, 2:512 
	BYTE bSec;
	BYTE bSecDebug;
	char szSecPW[20];
	char cReserved[31];
} PFormObjData;

// GID : YET_GID_LORA ////////////////////////////////////////////////
enum LoraObjIDDef{
	YET_OID_LoraTotal = 0,
	YET_OID_LRFFreq,
	YET_OID_LSPFactor,
	YET_OID_LUseEnDec,
	YET_OID_LoraKey,
	YET_OID_LoraTxp,
};

typedef union LoraObjData {
	BYTE bUseEnDec;		// 송수신 암호화, 0:사용안함, 1:AES128암호화, 2:ARIA128암호화
	BYTE ucRFFreq;			// 주파수 채널(0~15), 0:920.3MHz, 0.2MHz 증가
	BYTE ucSPFactor;		// Spreading Factor 값(7~12), 값이 높을 수록 도달 거리는 늘어나며, 속도는 감소함
							    // 7:5.4kbps -125dB, 8:3.1kbps -128dB, 9:1.7kbps -131dB, 10:0.9kbps -134dB, 11:0.5kbps -136dB, 12:0.3kbps -137dB
	char szLoraKey[20];		// LoRA 송수신 암호화 키 16바이트
	BYTE ucTxp;				// 송출	전력(0-14dBm)
} LoraObjData;

// GID : YET_GID_SCOMM ////////////////////////////////////////////////
enum SCommObjIDDef{
	YET_OID_SCommTotal = 0,
	YET_OID_SDelayTime,
	YET_OID_SMaxPktSize,
	YET_OID_SBaudrate,
	YET_OID_SDataBits,
	YET_OID_SStopBits,
	YET_OID_SParity,
	YET_OID_SCheckAlive,
	YET_OID_SReset,
};

typedef union SCommObjData{
	WORD wDelayTime;		// Milli Seconds, UART로 부터 입력된 데이터를 하나의 패킷으로 전송하기 위해 기다리는 시간.
	WORD wMaxPktSize;	// UART로 부터 입력된 데이터가 이 숫자보다 크면 DelayTime이전에도 패킷으로 전송한다.
	BYTE ucBaudrate;		// 1:1200bps ~ 13:1500000bps
	BYTE ucDataBits;			// 1:5bits, 2:6bits, 3:7bits, 4:8bits
	BYTE ucStopBits;			// 1:1bit, 2:2bits
	BYTE ucParity;			// 1:NoParity, 2:OddParity, 3:EvenParity
	BYTE ucReset;
} SCommObjData;

// GID : YET_GID_SYSTEM ////////////////////////////////////////////////
enum SystemObjIDDef{
	YET_OID_SystemTotal	= 0,
	YET_OID_Reset,
	YET_OID_DateTime,
	YET_OID_ResetDB,
	YET_OID_ResetStatus,
	YET_OID_SComSend,
	YET_OID_ComPacket,
	YET_OID_DebugFlag,
	YET_OID_AutoRebootMin,
	YET_OID_ResetEnv,
	YET_OID_DateTime_GPS,
	YET_OID_Save,
};

typedef union SystemObjData {
	BYTE ucReset;					// 시스템 리셋을 위해 사용
	char strDateTime[MAX_DESC_STR+1];// 시스템 시간을 설정하기 위해 사용
	// BYTE mPacket[MAX_DESC_STR+1];	// CLI->HWManager에게 요청할 송신패킷(디버그용)
	BYTE mPacket[110];	
	DWORD dwDebugFlag;				// 디버그 메시지 출력을 위한 플래그
	WORD wAutoRebootMin;			// 자동 재부팅 시간, 분
	char cReserved[52];
} SystemObjData;

// GID : YET_GID_SNODE ////////////////////////////////////////////////
enum SNodeObjIDDef{
	YET_OID_SNodeTotal = 0,
	YET_OID_SNodeName,
	YET_OID_SOID,
	YET_OID_SNodeDel,
	YET_OID_SRegNode,
	YET_OID_SCtrlReboot,
	YET_OID_SCtrlRptTime,
	YET_OID_SCtrlCfg,
	YET_OID_SCtrlDestGW,
	YET_OID_SCtrlTime,
	YET_OID_SNodeDelAll,
	YET_OID_PDCtrlCfg,
};

typedef union SNodeObjData{
	BYTE bRegNode;
	BYTE bCtrlReboot;		// 상위단에서 reboot이 설정된 경우 1, 센서에 전달하면 0
	WORD wRptTime;			// 상태보고 주기
	BYTE mEUI[8];			// 64-bit IEEE address
	char szSNodeName[36];	// 센서노드 이름
	char szOID[40];			// Sensor OID
	char szCtrlCfg[20];		// 센서 설정 값
	char szDestGW[32];		// 센서에게 G/W변경 요청을 할 때 사용할 변경 채널 및 G/W EUI
	char szPDCtrlCfg[52];	// PD 센서 설정 값
	char cReserved[52];
} SNodeObjData;

// GID : YET_GID_SLAVE ////////////////////////////////////////////////
enum SlvObjIDDef{
	YET_OID_SlvTotal = 0,
	YET_OID_SlvEUI,
	YET_OID_SlvOID,
	YET_OID_SlvDel,
	YET_OID_SlvCfg,
};

typedef union SlvObjData{
	BYTE mEUI[8];			// 64-bit IEEE address
	char szSlvOID[36];		// 슬레이브 OID
	char szSlvCfg[20];		// 센서 설정 값
	char cReserved[52];
} SlvObjData;

// GID : YET_GID_URSNODE ////////////////////////////////////////////////
enum URSNodeObjIDDef{
	YET_OID_URSNodeTotal = 0,
	YET_OID_URSNodeDel,
};

typedef union URSNodeObjData{
	BYTE bNodeDel;
	char cReserved[32];
} URSNodeObjData;

// GID : YET_GID_SPORT ////////////////////////////////////////////////
enum SPortObjIDDef{
	YET_OID_SPortTotal = 0,
	YET_OID_SPortDevCnt,
	YET_OID_SPortPollInt,
	YET_OID_SPortWaitTime,
};

typedef union SPortObjData{ 
	BYTE ucDevCnt;			// 병렬로 접속된 디바이스 개수
	WORD wPollInt;			// 폴링주기 sec
	WORD wWaitTime;			// 장비 응답 대기시간, ms
} SPortObjData;

// GID : YET_GID_SENSOR ////////////////////////////////////////////////
enum SensorObjIDDef{
	YET_OID_SensorTotal = 0,
	YET_OID_SensorTiltInit,
	YET_OID_SensorTiltEvent,
	YET_OID_SensorTiltThresHold,
	YET_OID_SensorScaleFactor,
	YET_OID_SensorTiltRaw,
	YET_OID_SensorTiltUse,
};

typedef union SensorObjData{ 
	BYTE ucTiltInit;
	double fTiltEvent;
	BYTE ucTiltThresHold;
	BYTE ucScaleFactor;
	BYTE ucTiltRaw;
	BYTE ucTiltUse;
} SensorObjData;

// GID : YET_GID_UPGRADE ////////////////////////////////////////////////
enum UpgradeObjIDDef{
	YET_OID_UpgradeTotal		= 0x00,
	YET_OID_UpgradeRequestUnit	= 0x01,
};

typedef union UpgradeObjData_Tag {
	char strFileName[MAX_DESC_STR+1];// 업그레이드 파일 지정
	char cReserved[52];
} UpgradeObjData;

// GID : YET_GID_PD_NODE ////////////////////////////////////////////////
enum PdObjIDDef{
	YET_OID_PdTotal			= 0,
	YET_OID_PD_MONITOR_PORT,
	YET_OID_PD_MODE,
	YET_OID_PD_SYNC,
	YET_OID_PD_SYNC_VAL,
	YET_OID_PD_GAIN,
	YET_OID_PD_TRIGGER,
	YET_OID_PD_FILTER_MIN,
	YET_OID_PD_FILTER_MAX,
	YET_OID_PD_TYPE,
	YET_OID_PD_ADC_FORMAT,
	YET_OID_PD_RPTTIME,
	YET_OID_PD_ADC_TEST,
	YET_OID_PD_ADC_VIRTUAL,
	YET_OID_PD_ADC_READ,
	YET_OID_PD_DAQ_STATUS,
	YET_OID_PD_ADC_RESET,
	YET_OID_PD_DAQ_IDLE,
};

typedef union PdObjData_Tag {
	WORD wMonPort;
	BYTE bMode;
	char ucSync;	
	char ucSyncVal;
	char ucChGain;
	short ucTrigger;
	char ucFilterMin;
	char ucFilterMax;
	BYTE bType;
	BYTE ucFormat;	
	WORD wRptTime;
	BYTE bAdcTest;
	BYTE bAdcVirtual;
	BYTE bAdcRead;
	BYTE bDaqStatus;
	BYTE bAdcReset;
	BYTE bDaqIdle;
} PdObjData;

#endif //!defined(DEVICEINFODB_H__INCLUDED_)
