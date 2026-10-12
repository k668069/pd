///////////////////////////////////////////////////////////
// DBFileManager.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(DBFILEMANAGER_H__INCLUDED_)
#define DBFILEMANAGER_H__INCLUDED_

#include "TypeDef.h"
#include "Agent.h"
#include "DeviceInfoDB.h"

#define YDEF_USING_DB_VERSION	0x01 // Ver1 - 101012
#define YDEF_NETWORK_DB_VERSION 0x01 // Ver1 - 101012
#define DEVICEINFO_FLASH_FILE	AGENT_DIR"/DeviceInfo.dat"
#define SDATA_LOG_FLASH_FILE	AGENT_DIR"/SDataLog.dat"
#define LOGDB_FLASH_FILE		AGENT_DIR"/LogDB.dat"

#pragma pack(push, 4)
//////////////////////////////////////////////////////////////
// Database Structure Version 1
typedef struct YST_DB_GENERAL_V1{
	BYTE ucModelID;					// 0
	BYTE Reserved[3];

	BYTE mIP[4];						// IP주소
	BYTE mSM[4];						// 넷마스크
	BYTE mGW[4];						// 게이트웨이
	BYTE mIP2[4];						// IP주소
	BYTE mEMAC[6];						// MAC주소
	BYTE Reserved2[2];

	char szPassword[MAX_PWD_STR+1];		// 접근암호
	char szSerialNo[MAX_DESC_STR+1];	// 새시 제조 일련번호 ex) JSNB101012001
	char szPoleNo[MAX_DESC_STR+1];		// 전주번호
	char szMangeNo[MAX_DESC_STR+1];		// 전산번호
	char szInstallDate[MAX_DESC_STR+1];	// 설치일자
	char szNodeName[36];				// 노드이름

	DWORD dwDebugFlag;				// 디버그 메시지 출력을 위한 플래그
	BYTE ucDMRMode;						// DMR 사용여부, 0:사용안함, 1:사용함
	BYTE ucGWIf;								// Default GW 인터페이스, 0:eth0, 1:halow	
	BYTE Reserved3[28];

	WORD wAutoRebootMin;				// 자동 재부팅 시간, 분
	BYTE mDNS[4];							// DNS ip주소
	BYTE bNTP;								// NTP 사용 유무
	BYTE ucOperMode;						// 동작모드, 0:외장형, 1:내장형
	BYTE ucOperEth0;
	BYTE ucOperEth1;	
	BYTE bPCommMode;				// Platform Comm Mode
	BYTE bTelnet;						// Telnet 사용유무
	BYTE bFtp;							// Ftp 사용유무
	BYTE Reserved4[28];

#ifdef DEF_RTCP
	BYTE mManMAC1[6];					// 접속가능 관리PC MAC주소
	BYTE mManMAC2[6];					// 접속가능 관리PC MAC주소
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

	BYTE Reserved5[28];
} YST_DB_GENERAL_V1;

typedef struct YST_DB_PLATFORM_V1{
	BYTE bRegGW;			// G/W 플랫폼 정상 등록 여부 
	BYTE bAssignOID;		// G/W의 OID를 할당 받은 경우 1
	WORD wRptTime;			// 상태보고 주기
	BYTE mMyIP[4];			// 자기 IP주소
	BYTE mPeerIP[4];		// 플랫폼 IP주소
	WORD wMyPort;			// 자기 Listen 포트번호
	WORD wPeerPort;			// 상대 Listen 포트번호
	char szGWOID[40];		// G/W OID
	char szURIPath[40];		// URI, Uniform Resource Identifier Path
	char szOIDBase[20];		// 기본 OID, ex)1.2.481.1.
	BYTE ucM2MType;			// M2M Type, 0:oneM2M, 1:LWM2M
	BYTE ucM2MVer;			// M2M Ver, 0:R0, 1:R1, 2:R2
	char szCSEID[20];		// AE-ID, CSE-ID, CSEBase Retrieve명령에 필요함
	WORD wBlockSize;			//  
	BYTE bSec;					// 
	BYTE bSecDebug;			// 
	char szSecPW[20];			// 
	BYTE Reserved2[40];
} YST_DB_PLATFORM_V1;

typedef struct YST_DB_WISUN_V1{
	BYTE bUseEnDec;			// 송수신 암호화를 수행하는 경우 1
	BYTE ucRFChan;			// 주파수 채널(0~31)
	WORD wPanID;			// Private Area Network ID, 구성할 네트워크 ID
	BYTE Reserved[40];
} YST_DB_WISUN_V1;

typedef struct YST_DB_SLAVE_V1{
	BYTE mEUI[8];			// 64-bit IEEE address
	char szSlvOID[36];		// 슬레이브 이름
	BYTE Reserved[40];
} YST_DB_SLAVE_V1;

typedef struct YST_DB_SNODE_V1{
	BYTE bRegSNode;			// 센서노드 AE의 플랫폼 정상 등록 여부 
	BYTE Reserved[3];
	BYTE mEUI[8];			// 64-bit IEEE address
	char szOID[40];			// Sensor OID
	char szSNodeName[36];	// 센서노드 이름
	DDWORD dwFRegTime;		// 처음으로 데이터를 수신하여 G/W에 등록된 시간
	BYTE Reserved2[32];
} YST_DB_SNODE_V1;

typedef struct YST_DB_SCOMM_V1{ 
	WORD wDelayTime;		// Milli Seconds, UART로 부터 입력된 데이터를 하나의 패킷으로 전송하기 위해 기다리는 시간.
	WORD wMaxPktSize;		// UART로 부터 입력된 데이터가 이 숫자보다 크면 DelayTime이전에도 패킷으로 전송한다.
	BYTE ucPortID;			// 1:COM1 ~ 10:COM0
	BYTE ucBaudrate;		// 1:1200bps ~ 13:1500000bps
	BYTE ucDataBits;		// 1:5bits, 2:6bits, 3:7bits, 4:8bits
	BYTE ucStopBits;		// 1:1bit, 2:2bits
	BYTE ucParity;			// 1:NoParity, 2:OddParity, 3:EvenParity
	BYTE Reserved[40];
} YST_DB_SCOMM_V1;

typedef struct YST_DB_LORA_V1{
	BYTE bUseEnDec;		// 송수신 암호화를 수행하는 경우 1
	BYTE ucRFFreq;			// 주파수 채널(0~15), 0:920.3MHz, 0.2MHz 증가
	BYTE ucSPFactor;		// Spreading Factor 값(7~12), 값이 높을 수록 도달 거리는 늘어나며, 속도는 감소함
							        // 7:5.4kbps -125dB, 8:3.1kbps -128dB, 9:1.7kbps -131dB, 10:0.9kbps -134dB, 11:0.5kbps -136dB, 12:0.3kbps -137dB
	char szLoraKey[20];	// LoRA 송수신 암호화 키 16바이트
	BYTE ucTxp;				// 송출	전력(0-14dBm)
	BYTE Reserved[20];
} YST_DB_LORA_V1;

typedef struct YST_DB_SPORT_V1{ 
	BYTE ucDevCnt;			// 병렬로 접속된 디바이스 개수
	BYTE ucReserved;
	WORD wPollInt;			// 폴링주기 sec
	WORD wWaitTime;			// 장비 응답 대기시간, ms
	WORD ucReserved2;
} YST_DB_SPORT_V1;


typedef struct YST_DB_NAT_PFWD_V1{
	BYTE mPFWDIP[4];		// 포워딩을 할 로컬 IP
	int nSrcPort;			// NAT 서버에서의 소스 포트 번호
	int nDestPort;			// 클라이언트 로컬 IP에서의 포워딩할 포트 번호.
	BYTE ucProtocol;		// 포트포워딩 사용 프로토콜, 0:사용안함, 1:TCP, 2:UDP
	BYTE Reserved[3];
} YST_DB_NAT_PFWD_V1;

typedef struct YST_DB_ROUTE_TBL_V1{// 다른 네트워크에 대한 라우팅 주소 테이블
	BYTE ucUse;							// 라우팅 사용 여부, 0:사용안함, 1:사용함
	BYTE Reserved[3];
	BYTE mRNet[4];						// 라우팅을 할 네트워크, ex) 192.168.10.x
	BYTE mRGW[4];						// 라우팅을 할 GW, ex) 192.168.11.2
} YST_DB_ROUTE_TBL_V1;

#if 0
typedef struct YST_DB_PD_SENSOR_V1{	// PD 수집부
	BYTE ucMode;					// 0:PRPD,  1:PRPS
	BYTE ucSync;						// 0:External,  1:Internal
	WORD wRptTime;				// 수신 주기, 단위:sec
	WORD wMonPort;
} YST_DB_PD_SENSOR_V1;
#endif

#if 1
typedef struct YST_PD_CH_MAIN_V1{
	char	sync_sel;			// 내부(1)/외부(0) 싱크 선택
	char	sync_sel_val;		// 레퍼런스 단계보냄 (-10 ~10)
	char	ch_on;				// 채널 On/Off 함 - 사용하지 않음, 1 : On, 0 : Off
	char	ch_gain;			// 각 채널의 게인값을 설정해줌, 최소 35 (-9 dB) ~ 최대 0 (+26 dB) 1dB Step
	short	trig;					// 각 채널의 트리거 레벨값, 0 ~ 2047
	char	filter_min;			// 필터 로우값, 1 ~ 75
	char	filter_max;			// 필터 하이값, 1 ~ 75
	BYTE	SensorType;		// ch별 한바이트씩
} YST_PD_CH_MAIN_V1;

//
typedef struct YST_PD_PD_DEV_V1{
	DDWORD	dwLastRcvTime;		// PD센서 데이터를 수집한 시간
	WORD		wRptTime;				// 수신 주기, 단위:sec
	WORD		wMonPort;
	BYTE			bValid;					// 패킷 수신
	BYTE			packet_type;			// 패킷 타입, 3개중 1개(Setting, Measure, Reset)
	int				packet_size;			// 패킷 사이즈
	BYTE			ucMode;				// 0xD1:PQN, 0xD2:SCOPE, 0xD5:ETC
	BYTE			ucFormat;				// ADC Data format
	YST_PD_CH_MAIN_V1		ch_set[4];	// 4ch
} YST_PD_PD_DEV_V1;
#endif

typedef struct YST_DB_V1{
	WORD wVersion;						// 버젼정보
	BYTE ucResetStatus;
	BYTE Reserved;
	YST_DB_GENERAL_V1	General;
	YST_DB_PLATFORM_V1	Platform;

  #ifdef DEF_LORA
	YST_DB_LORA_V1		Lora;
  #endif

	YST_DB_SCOMM_V1		SComm[MAX_SCOMM_CNT];

  #if MAX_SPORT_CNT == 1
	YST_DB_SPORT_V1		SPort;
  #else
	YST_DB_SPORT_V1		SPort[MAX_SPORT_CNT];
  #endif

	YST_DB_SNODE_V1		SNode[MAX_SNODE_CNT];

  #ifdef DEF_SLAVE
	YST_DB_SLAVE_V1		Slave[MAX_SLAVE_CNT];
  #endif

	YST_DB_NAT_PFWD_V1	PFwd[MAX_NAT_PFWD_CNT];
	YST_DB_ROUTE_TBL_V1	Route[MAX_ROUTE_TBL_CNT];
	// YST_DB_PD_SENSOR_V1	PdNode;
	YST_PD_PD_DEV_V1 PdNode;
	BYTE mReserved[160];
	DWORD dwCRCValue;						// CRC 체크
} YST_DB_V1;
// End of Version 1
///////////////////////////////////////////////////////////////////
#pragma pack(pop)

void ResetEnv(void);
void ResetDeviceInfoDB(BOOL bInit);
void ReadDeviceInfoDBFromDisk(void);
void WriteDeviceInfoDBToDisk(void);

#endif //DBFILEMANAGER_H__INCLUDED_
