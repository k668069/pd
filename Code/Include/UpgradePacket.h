/////////////////////////////////////////////////////////// 
// UpgradePacket.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include "TypeDef.h"

#if !defined(UPGRADEPACKET_H__INCLUDED_)
#define UPGRADEPACKET_H__INCLUDED_

#define MAX_TCP_CONNECTION_COUNT 	1

#define	LISTENQ		1024	/* 2nd argument to listen() */
#define MAX_DESC_STR_SIZE	51
#define MAX_FILE_NAME_SIZE	51

enum UpgradePacketIDDef{
	YET_PID_REQ_DEV_INFO 	= 'G',	// 장비 정보 요청
	YET_PID_REP_DEV_INFO 	= 'I',	// 장비 정보 응답
	YET_PID_SND_FILE_INFO 	= 'F',	// 파일 정보 전달
	YET_PID_SND_DATA 		= 'D',	// 파일 데이터 전달
	YET_PID_REP_ACK			= 'A',	// 정상응답
	YET_PID_REP_NAK			= 'N',	// 수신오류
	YET_PID_REQ_STATUS		= 'P',	// 상태 정보 요청
	YET_PID_REP_STATUS		= 'S',	// 상태 정보 응답
	YET_PID_REQ_RESET 		= 'R',	// 정상종료 및 재부팅
	YET_PID_REQ_END			= 'E',	// 정상/비정상 종료 재부팅없슴
};

enum UpgradeStatusIDDef{
	YET_US_READY 	= 0,
	YET_US_DOWNLOADING,
	YET_US_CRC_ERR,
	YET_US_WRITING,
	YET_US_FAIL,
	YET_US_COMPLETE,
};

typedef struct YST_UPGRADE_PACKET_HEADER{
	char mSOP[3];			// Start Of Protocol - '!', '@', '#'
	char cPacketID;			// Packet ID
	BYTE mBlkIdx[2];		// 전체 파일에서 전송하는 데이터 블럭 인덱스
	BYTE mDataSize[2];		// 헤더와 함께 전송되는 데이터의 크기
	BYTE mCRCData[2];		// Data Field CRC Value
	BYTE mCRCHeader[2];		// Header Field CRC Value
} YST_UPGRADE_PACKET_HEADER;

typedef struct YST_AGENT_INFO{//업그레이드 서버에 전송하는 장비정보
	BYTE ucAgentID;			// Agent ID
	BYTE ucBDVersion;		// Agent Board Version
	BYTE ucReqFileType;		// 업그레이드 파일 종류. 0:요청안함, 1:MPU, 2:UNIT, 3:일반파일전송
	BYTE ucUseName;			// 업그레이드 파일이름을 직접 지정가능, 0:지정안함, 1:파일이름을 지정
	BYTE mBSize[4];			// 요청하는 전송블럭 크기, 0:64K, 128, 256, 1K

	BYTE ucMajor;			// 주버젼
	BYTE ucMiddle;			// 부버젼
	BYTE ucMinor;			// 빌드버젼
	BYTE ucBoard;			// 보드버젼
	BYTE mReserved[16];

	char strDate[MAX_DESC_STR_SIZE+1];			// 빌드 날짜-시간
	char strDescription[MAX_DESC_STR_SIZE+1];	// 버젼 변경사항 설명용
	char strFileName[MAX_FILE_NAME_SIZE+1];		// 업그레이드 파일 이름
} YST_AGENT_INFO;

typedef struct YST_UPGRADE_FILE_INFO{//업그레이드 서버에서 장비로 전송하는 정보
	BYTE ucFileType;		// 업그레이드 파일 종류. 1:MPU, 2:Unit, 3:일반파일전송
	BYTE ucUnitIdx;			// 시리얼로 연결된 부속유닛을 업그레이드 하는 경우 유닛인덱스
	BYTE mBSize[2];			// 부속유닛에 전송할 블록 사이즈, 128 또는 256
	BYTE mFileSize[4];		// 파일의 전체크기
	BYTE mBlkCnt[2];		// 전체 파일을 전송하는데 필요한 블럭 개수
	BYTE mFileCRC[2];		// 전체파일의 CRC Value
	BYTE mReserved[16];
	char strFileName[MAX_FILE_NAME_SIZE+1];
} YST_UPGRADE_FILE_INFO;

typedef struct YST_UPGRADE_STATUS{
	BYTE ucStatus;			// 0:파일다운대기중, 1:파일다운중, 2:파일CRC오류, 3:플래쉬에굽는중, 4:플래쉬에 굽는중에 오류(업그레이드 중단), 5:업그레이드 완료.
	BYTE ucRequestReset;	// 0:일반, 1:플래쉬 메모리에 쓰고 난후에 Reset을 요청
	BYTE mWritingTime[2];	// 플래쉬메모리에 쓰는데 소요되는 예상시간, 초단위
	BYTE mReserved[16];
} YST_UPGRADE_STATUS;

#endif //!defined(UPGRADEPACKET_H__INCLUDED_)
