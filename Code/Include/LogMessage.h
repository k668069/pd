///////////////////////////////////////////////////////////
// LogMessage.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(LOGMESSAGE_H__INCLUDED_)
#define LOGMESSAGE_H__INCLUDED_

#include <stdio.h>
#include "TypeDef.h"
#include "Agent.h"
#include "DeviceInfoDB.h"

#define USE_FULL_LOG_PATH 1

#if 1
#define GENERAL_LOG_FILE		AGENT_DIR"/General.log"
#define GENERAL_LOG_FLASH_FILE	 AGENT_DIR"/FGeneral.log"
#define DEBUG_LOG_FILE			AGENT_DIR"/Debug.log"
#else
#define GENERAL_LOG_FILE				"/var/General.log"			// DebugFlag와 상관없이 시스템 로그를 저장(파일크기제한)
#define GENERAL_LOG_FLASH_FILE		AGENT_DIR"/General.log"		// DebugFlag와 상관없이 시스템 로그를 저장(파일크기제한)
#define DEBUG_LOG_FILE					"/var/Debug.log"			// 디버그용 메시지를 저장(파일크기제한없슴)
#endif

#define LOG_ERR				0x00000000
#define LOG_TO_SCREEN	    0x80000000	// 기본터미널로 디버그 메시지를 보여줌
#define LOG_TO_FILE			0x40000000	// 텔넷 터미널을 위해 디버그 메시지를 DEBUG_LOG_FILE에 저장함
#define LOG_DEBUG_IPC		0x20000000	// IPC 설정 데이터를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_TCP		0x10000000	// TCP 관련 메시지를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_SOCKET  	0x08000000	// TCP 소켓 관련 메시지를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_SOCDATA	0x04000000	// TCP 소켓 데이터 입출력을 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_UDP		    0x02000000	// UDP 관련 메시지를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_UDPDATA	0x01000000	// UDP 송수신 데이터를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_SERIAL   	0x00800000	// 시리얼 통신 패킷을 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_SERDATA	0x00400000	// 시리얼 통신으로 수신된 실제 데이터를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_UPGRADE	0x00200000	// 업그레이드 관련 메시지를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌
#define LOG_DEBUG_UPDATA	    0x00100000	// 업그레이드 파일 데이터를 DEBUG_LOG_FILE에 저장 혹은 기본터미널로 보여줌

#define LOG_DEBUG_COAP			   0x00080000	// COAP 처리 과정
#define LOG_DEBUG_COAPDATA    0x00040000	// COAP 간략 데이터
#define LOG_DEBUG_COAPDETAIL  0x00020000	// COAP 자세한 데이터
#define LOG_DEBUG_SCOMM	     	0x00010000

#define LOG_DEBUG_SCOMMDATA	0x00008000
#define LOG_DEBUG_M2MXML		0x00004000

#ifdef DEF_PRPS
#define LOG_DEBUG_PRPS			0x00002000
#define LOG_DEBUG_PRPSDATA  0x00001000
#define LOG_DEBUG_TCPPD	    0x00000800
#else
#define LOG_DEBUG_GPS				0x00002000
#define LOG_DEBUG_ITTH				0x00001000
#endif

#define LOG_DEBUG_TCPPDDATA	0x00000400

#define LOG_DEBUG_LORA			0x00000200	// LORA 처리 과정
#define LOG_DEBUG_LORADATA	0x00000100	// LORA 데이터

#define LOG_DEBUG_HW1	    	0x00000080
#define LOG_DEBUG_HW2		    0x00000040

#define LOG_DEBUG_PLATSEC	    	0x00000020
// #define LOG_DEBUG_I2CDATA		0x00000010
#define LOG_DEBUG_PDNODE	    	0x00000010

#define LOG_DEBUG_TCP_XML		0x00000008
#define LOG_DEBUG_TCPX_DATA	0x00000004
#define LOG_DEBUG_RCOMM			0x00000002
#define LOG_DEBUG_DBXML			0x00000001


typedef struct YST_DB_LOG_Tag{
	DWORD dwDebugFlag;				// 디버그를 위한 플래그
	BOOL bLogProcess;				// 1:While Saving To Log File, 두개의 프로세서가 동시에 로그파일을 저장하는 것을 방지
	WORD wLogIndex;					// 여러개의 프로세서가 하나의 로그 파일에 저장하기 위해 사용되는 인덱스
	BYTE Reserved[102];
} YST_DB_LOG;

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////
// General Log Message
long GetFileSize(const char strFileName[]);
void TruncateFileWithSize(const char strFileName[], long nSizeLimit);
void SaveLogMessageToFile(const char strFileName[], long nFileSizeLimit, char strData[]);
void ReadLogFileFromDisk(void);
void GeneralLogMsg(const char *strFormat, ...);
void PrintGeneralLogMsg(const char *strFormat, ...);
void LogMessageDebugTemp(const char *strFormat, ...);
void ClearGeneralLogMsg(void);
//void ShowGeneralLogMsg(void);
void ShowGeneralLogMsg(int nStart, int nStop);

/////////////////////////////////////////////////////////////////////////////
// Debug Log Message
void ClearDebugLogMessage(void);
void PrintDebugLogMessage(void);
void CheckAndPrintDebugLogMessage(void);
void InitializeDebugLogMessage(DWORD dwDebugFlag);
FILE *OpenDebugLogFile(DWORD dwFlag);
FILE *OpenDebugLogFile2(DWORD dwFlag);
void PrintDbgData(const void* Buf, int nBufSize, FILE* fp);
void PrintDbgTextData(const void* pBuf, int nSize, FILE* fp);
void DebugLogIPCSetParameter(int nProcessor);
void PrintDbgMsg(DWORD dwFlag, const char *strFormat, ...);
void DebugLogMilliTimeCheck(DWORD dwFla, char strMessage[]);
BOOL DebugEnable(DWORD dwFlag);

/////////////////////////////////////////////////////////////////////////////
// LogDB Message
void ClearLogDBFromDisk(void);
void ReadLogDBFromDisk(void);
void SaveToEventLogMsg(BYTE ucType, const char szMsg[]);

/////////////////////////////////////////////////////////////////////////////
// General Log Message
void ReadLogFileFromDisk(void);
void PrintGeneralLogMsg(const char *strFormat, ...);
void GeneralLogMsg(const char *strFormat, ...);
void ClearGeneralLogMsg(void);
//void ShowGeneralLogMsg(void);
void ShowGeneralLogMsg(int nStart, int nStop);

/////////////////////////////////////////////////////////////////////////////
// Sensor Data Log Message
void ReadSDataLogFromDisk(void);
void ClearSDataFromDisk(void);
void SaveToSensorDataMsgQueue(int nRSSI, BYTE ucPIdx, BYTE ucRFModule, BYTE ucType, BYTE mEUI[], const char szMsg[], BYTE ucErrCode);
void CheckAndSaveSensorDataMsg(void);
int GetSensorDataMsgQueueDataSize(void);
void ReadFromSensorDataMsgQueue(int nIdx, YST_SDLOG_DATA *pSData);

#ifdef __cplusplus
}
#endif
#endif //LOGMESSAGE_H__INCLUDED_
