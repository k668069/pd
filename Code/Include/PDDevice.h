/////////////////////////////////////////////////////////// 
// PDDevice.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include "DeviceInfoDB.h"

#if !defined(TCPPDPACKET_H__INCLUDED_)
#define TCPPDPACKET_H__INCLUDED_

////////// Define Group ID
enum _enum_MEM_IO_DEF{
	M_RD	= 0x00,
	M_WR	= 0x01,
};

#define DEF_PD_CH	4
#define DEF_BUFLEN 4160

typedef void (*AppPDCallbackFuncDef)(char* pCmd, BYTE Data[], DWORD dwParam1, DWORD dwParam2);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define Data Struct
// size : bytes
#define SIZE_PRPS_SRC		256
#define SIZE_PRPS_CH	512  // 256*2
#define SIZE_PRPS_S		518  // 256*2 + 6
#define SIZE_PRPS_ALL	2072	// 518*4ch

#define SIZE_EVENT_SRC	512
#define SIZE_EVENT_CH 1024   // 512*2
#define SIZE_EVENT_ALL 4136	// (10 + 1024)*4ch

#define SET_HEADER_PACKET_SIZE		52
#define SET_COMMON_DAQ_NAME			"PSAM"
#define SET_COMMON_SYNC_SEL			0x00		// 내부(1)/외부(0) 
#define SET_COMMON_SYNC_SEL_VAL	0x00		// -10 ~ 10
#define SET_COMMON_CH_GAIN				0x00		//  0(+26dB) ~ 35(-9dB), Default 26(0dB)
#define SET_BODY_CH_ON						1			// 1:On, 0:Off	
#define SET_BODY_TRIG_LV					2047		// 0 ~ 2048
#define SET_BODY_FILTER_MIN				1			// 1 ~ 75
#define SET_BODY_FILTER_MAX				75			// 1 ~ 75	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DAQ
#define DAQ_BASE_ADDRESS		0x43C30000
#define DAQ_SIZE		4096UL	// 4KB
#define DAQ_MASK	(DAQ_SIZE - 1)

enum Z415_DAQ_Reg{
	DAQ_REG_CHIPID_VER	= 0x00,
	DAQ_REG_STATUS		= 0x04,
	DAQ_REG_CTRL		= 0x08,
	DAQ_REG_FIFO		= 0x0C,
};

enum SensorType_Def{
	CH13_UHF			= 0x01,
	CH13_AE			= 0x02,
	CH13_GAIN		= 0x04,
	CH13_ENVELOP	= 0x08,
	CH24_UHF			= 0x10,
	CH24_AE			= 0x20,
	CH24_GAIN		= 0x40,
	CH24_ENVELOP	= 0x80,
};

enum PacketType_Def{
	PT_SETTING		= 0xA0,	// 160
	PT_MEASURE		= 0xA1,
	PT_RESET_PC	= 0xAA,
};

enum MeasureType_Def{
	MT_PQN		= 0xD1,		// PRPS (2 x 256 x 4 = 2048bytes), preview
	MT_SCOPE	= 0xD2,		// PULSE (2 x 512 x 4 = 4096bytes), event
	MT_ETC		= 0xD5,
};

enum ModeSelect_Def{
	MS_PREVIEW	 = 0x0,
	MS_EVENT,
};

enum SyncSelect_Def{
	SYNC_OUTER	 = 0x0,
	SYNC_INNER,
};

typedef struct PD_HEADER_Tag{
	BYTE	packet_type;		// 패킷타입
	BYTE	packet_size[4];		// 패킷 사이즈, 고정 52, 리틀엔디언
} PD_HEADER_Tag;

//========================================================
// PC -> 장비

typedef struct PD_COMMON_Tag{
	char	DAQName[20];	// 장비 이름
	char	sync_sel;			// 내부(1)/외부(0) 싱크 선택
	char	sync_sel_val;		// 레퍼런스 단계보냄 (-10 ~10)
	char	ch_gain;			// 각 채널의 게인값을 설정해줌, 최소 35 (-9 dB) ~ 최대 0 (+26 dB) 1dB Step
} PD_COMMON_Tag;

typedef struct PD_BODY_Tag{
	char	ch_on;				// 채널 On/Off 함 - 사용하지 않음, 1 : Off, 1 : On
	BYTE	trig_lv[2];				// 각 채널의 트리거 레벨값, 0 ~ 2047, 리틀엔디언
	char	filter_min;			// 필터 로우값, 1 ~ 75
	char	filter_max;			// 필터 하이값, 1 ~ 75
	BYTE	SensorType;		// ch별 한바이트씩
} PD_BODY_Tag;

// 설정
typedef struct PD_PC_TO_DEV_SET_PKT1{
	PD_HEADER_Tag	header;
	PD_COMMON_Tag	common;
	PD_BODY_Tag		body[DEF_PD_CH];
} PD_PC_TO_DEV_SET_PKT1;

// 제어,  측정 및 장비 Reboot
typedef struct PD_PC_TO_DEV_CON_PKT1{
	BYTE	packet_type;		// 패킷타입, (0xA0 Setting, 0xA1 Measure, 0xAA Reset)
	BYTE	mea_mode;		// 측정타입, (0xD1 PQN, 0xD2 SCOPE, 0xD5 ETC) Mode
} PD_PC_TO_DEV_CON_PKT1;

//====================================================
typedef struct PD_BODY_MAIN{
	char	ch_on;				// 채널 On/Off 함 - 사용하지 않음, 1 : Off, 1 : On
	short	trig_lv;				// 각 채널의 트리거 레벨값, 0 ~ 2047
	char	filter_min;			// 필터 로우값, 1 ~ 75
	char	filter_max;			// 필터 하이값, 1 ~ 75
	BYTE	SensorType;		// ch별 한바이트씩
} PD_BODY_MAIN;

//
typedef struct PD_PC_TO_DEV_MAIN{
	BYTE	bValid;				// 패킷 수신
	BYTE	packet_type;		// 패킷 타입, 3개중 1개(Setting, Measure, Reset)
	int		packet_size;		// 패킷 사이즈
	BYTE	mea_mode;
	PD_COMMON_Tag	common;
	PD_BODY_MAIN		body[DEF_PD_CH];	
} PD_PC_TO_DEV_MAIN;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PD데이터 수집 및 전송파일
enum ProcessPDStep {
	PS_PD_IDLE = 0,		// 대기
	PS_PD_GATHER,			// 수집
	PS_PD_ANALY,			// 분석	
	PS_PD_PRESS,			// 압축
	PS_PD_READY,			// 전송준비
	PS_PD_TRANSFER,		// 전송시작
};

enum ProcessPDCommand {
	CMD_PD_IDLE = 0,		// 대기
	CMD_PD_RUN,			// 요청, 실행
	CMD_PD_STOP,			// 중지
};

#define COL_SIZE 100000
typedef struct _PDRECV {
	int phase;
	short data[SIZE_EVENT_SRC];
} _PDRECV;

typedef struct PD_RAW{
	DWORD		count;
	_PDRECV	recv[DEF_PD_CH][COL_SIZE];
}PD_RAW;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //!defined(TCPPDPACKET_H__INCLUDED_)


