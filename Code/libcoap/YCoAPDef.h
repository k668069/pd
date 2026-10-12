///////////////////////////////////////////////////////////
// YCoAPDef.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(YH_YDNP30COMMONDEF_H__INCLUDED_)
#define YH_YDNP30COMMONDEF_H__INCLUDED_

#define DEF_SECU

#ifdef DEF_SECU
	#define SECU_MAX_BUF						2048							//Message Max Length 
	// #define SECU_PATH					      	"./certs"		// certificate file path
	#define SECU_PATH					      	"/app/rdisk/certs"		// certificate file path
	#define SECU_SSL_CONNECT_TRY_COUNT	(int)3					// TRY COUNT
#endif

typedef float FLOAT32;
typedef double FLOAT64;
typedef unsigned long long UINT64;

#define YDEF_COAP_MAX_OPT		15		// 하나의 COAP 메시지에 전송되는 최대 OPTION 개수
#define YDEF_COAP_MAX_OPT_STR	128

// #define YDEF_COAP_MAX_PAYLOAD	1024
#define YDEF_COAP_MAX_PAYLOAD	2048 // DEF_BLOCK

#define COAP_LOG_STR_SIZE		256			// APP로 전송하는 메시지의 문자열 최대 크기

#define COAP_LOG_BYTE_SIZE		1024		// 로그 데이터 최대크기

enum YET_COAP_APP_MSG_TYPE{
	YET_AMT_APP		= 0,	// APP 로그
	YET_AMT_UDP		= 1,	// TCP/IP 로그
};

typedef struct YST_COAP_APP_LOG{		// APP로 전송되는 메시지 포맷
	BYTE ucType;						// 메시지 형식
	BYTE ucLevel;						// 0:일반로그, 1:정상로그, 2:중요로그
	UINT64 dwTime;						// 시간 1/1000초
	char szMsg[COAP_LOG_STR_SIZE];		// 데이터에 대한 설명, 제어및기능
} YST_COAP_APP_LOG;

// YST_LOG_MSG는 디버깅용으로만 사용하고 로그파일을 위해 새로이 로그포맷을 정의한다. 140327
typedef struct YST_COAP_COMM_LOG{			// APP로 전송되는 메시지 포맷
	DWORD dwIPAddr;							// 송수신 IP주소
	WORD wPortNo;							// 송수신 포트번호
	UINT64 dwTime;							// 시간 1/1000초
	BYTE bSend;								// 0:수신프레임, 1:송신프레임
	BYTE bOptLogExist[YDEF_COAP_MAX_OPT];	// CoAP 옵션 해석로그 존재함
	char szReqLog[COAP_LOG_STR_SIZE];		// 데이터에 대한 설명
	char szCoAPHdr[COAP_LOG_STR_SIZE];		// CoAP 헤더 해석로그
	char szOptLog[YDEF_COAP_MAX_OPT][COAP_LOG_STR_SIZE];		// CoAP 옵션 해석로그
	char szPayload[COAP_LOG_BYTE_SIZE];		// 메시지
	WORD wSize;								// 데이터크기
	BYTE Data[COAP_LOG_BYTE_SIZE];			// 실제 프레임 데이터
} YST_COAP_COMM_LOG;

enum YET_COAP_OPTION_CODE{
	YET_COC_IF_MATCH        = 1,
	YET_COC_URI_HOST        = 3,
	YET_COC_ETAG            = 4,
	YET_COC_IF_NONE_MATCH   = 5,
	YET_COC_OBSERVE         = 6,
	YET_COC_URI_PORT        = 7,
	YET_COC_LOCATION_PATH   = 8,
	YET_COC_URI_PATH		= 11,
	YET_COC_CONTENT_FORMAT	= 12,
	YET_COC_MAXAGE			= 14,
	YET_COC_URI_QUERY		= 15,
	YET_COC_ACCEPT			= 17,
	YET_COC_LOCATION_QUERY	= 20,
	YET_COC_BLOCK2			= 23,
	YET_COC_BLOCK1			= 27,
	YET_COC_PROXY_URI		= 35,
	YET_COC_PROXY_SCHEME	= 39,
	YET_COC_SIZE1			= 60,
	YET_COC_M2M_FR			= 256,
	YET_COC_M2M_RQI			= 257,
	YET_COC_M2M_NM			= 258,
	YET_COC_M2M_OT			= 259,
	YET_COC_M2M_RTURI		= 263,
	YET_COC_M2M_RSC			= 265,
	YET_COC_M2M_TY			= 267,
	YET_DFC_NORESPONSE		= 284,
};

typedef struct YST_COAP_OPT {
	WORD wNo;							// CoAP Option Number
	WORD wLen;							// CoAP Option Length
	BYTE mData[YDEF_COAP_MAX_OPT_STR];	// CoAP Option Data
} YST_COAP_OPT;

typedef struct YST_COAP_MSG_FMT{
	BYTE ucVer;							// CoAP Version(2bit)
	BYTE ucType;						// CoAP Type(2bit), 0:Confirmable, 1:Non-confirmable, 2:Acknowledgement, 3:Reset
	BYTE ucTKL;							// CoAP Token Length(4bit)
	BYTE ucCodeC;						// CoAP Code Class(3bit), 0:Request, 2:Sucess-Response, 4:Client-Err-Response, 5:Server-Err-Response
	BYTE ucCodeD;						// CoAP Code Detail(5bit)
	WORD wMsgID;						// CoAP Message ID(16bit)
	WORD wPayloadSize;					// CoAP Payload Size
	BYTE mToken[16];					// CoAP Token
	YST_COAP_OPT Opt[YDEF_COAP_MAX_OPT];	// CoAP Option
	BYTE mPayload[YDEF_COAP_MAX_PAYLOAD];// CoAP Payload
} YST_COAP_MSG_FMT;

#endif // !defined(YH_YDNP30COMMONDEF_H__INCLUDED_)