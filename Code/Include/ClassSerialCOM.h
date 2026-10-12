/////////////////////////////////////////////////////////// 
// SerialLinux.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <time.h>
#include "TypeDef.h"
#include "ClassQueue.h"

////////////////////////////////////////////////////////////
// Defined at <asm/termbits.h>
//
// #define CBAUD	0010017
// #define  B0	0000000
// #define  B50	0000001
// #define  B75	0000002
// #define  B110	0000003
// #define  B134	0000004
// #define  B150	0000005
// #define  B200	0000006
// #define  B300	0000007
// #define  B600	0000010
// #define  B1200	0000011
// #define  B1800	0000012
// #define  B2400	0000013
// #define  B4800	0000014
// #define  B9600	0000015
// #define  B19200	0000016
// #define  B38400	0000017
//
// #define    B57600 0010001
// #define   B115200 0010002
// #define   B230400 0010003
// #define   B460800 0010004
// #define   B500000 0010005
// #define   B576000 0010006
// #define   B921600 0010007
// #define  B1000000 0010010
// #define  B1152000 0010011
// #define  B1500000 0010012
// #define  B2000000 0010013
// #define  B2500000 0010014
// #define  B3000000 0010015
// #define  B3500000 0010016
// #define  B4000000 0010017
//
// #define CSIZE	0000060
// #define   CS5	0000000
// #define   CS6	0000020
// #define   CS7	0000040
// #define   CS8	0000060

enum COMPortID{
	COM1 = 1,
	COM2 = 2,
	COM3 = 3,
	COM4 = 4,
	COM5 = 5,
	COM6 = 6,
	COM7 = 7,
	COM8 = 8,
	COM9 = 9,
	COM0 = 10
};

#define MAX_BAUDRATE_CNT 7
enum COMPortBaudrateID{
	YET_B2400,
	YET_B4800,
	YET_B9600,
	YET_B19200,
	YET_B38400,
	YET_B57600,
	YET_B115200
};

enum COMPortDataBitsID{
	YET_CS5,
	YET_CS6,
	YET_CS7,
	YET_CS8,
};

enum COMPortParityID{
	NO_PARITY,
	EVEN_PARITY,
	ODD_PARITY
};

enum COMPortStopBitID{
	ONE_STOP_BIT,
	TWO_STOP_BIT
};

#define USE_SEND_Q_FOR_WRITE		// Tx에 대해서 Queue를 사용하는 경우 define
//#define USE_DIRECTION_CONTROL		// Tx/Rx Dirction 제어를 하는 경우 define

class CSerialCOM
{
public:
	int m_nFd;
	struct termios m_OldTerminalIO;
	struct sigaction m_SignalIO;
	char *m_strStatus;				// 상태를 저장.
	char m_strPortName[10];			// 포트이름(COM1, COM2,...)
	char m_strPortFileName[20];		// 포트 파일이름 (dev/ttyS1)
	char m_strOpenStatus[20];		// Open Status(opened/not opened/open fail)
	char m_strConfig[50];			// 포트 설정.
	BYTE m_ucPortID;				// 포트 식별자
	BYTE m_ucBaudrateID;			// 보레이트
	BYTE m_ucDataBits;				// 데이터비트
	BYTE m_ucParityID;				// 패러티 체크
	BYTE m_ucStopBitID;				// 스탑비트
	BYTE m_bFlowCtrl;				// 플로우 컨트롤
	BYTE m_bLoopBack;				// 루프백 - 시리얼 포트에서 입력된 데이터를 그대로 출력한다.
	int m_nPIdx;				// 몇번째 포트인지를 결정, 실제포트와 다를수 있슴.
	CQueue m_QRcv;				// 데이터 임시 저장버퍼
#ifdef USE_SEND_Q_FOR_WRITE
	CQueue m_QSnd;				// 데이터 임시 저장버퍼
#endif
	DDWORD m_dwRcvTick;			// 최종 데이터 수신 시간

	CSerialCOM();
	~CSerialCOM();
	int OpenCOMPort(BYTE ucPortID, int nIdx, BYTE ucBaudrateID, void (*OnCOMDataReceived)(int status)=NULL, BYTE ucDataBits=YET_CS8, BYTE ucParityID=NO_PARITY, BYTE ucStopBitID=ONE_STOP_BIT, BOOL bFlowCtrl=FALSE);
	void CloseCOMPort(void);
	void MakeConfigString(void);
	void MakeStatusString(void);
	void SetStatusStringPointer(char *strCOMStatus);
	void SetBaudRate(BYTE ucBaudrateID);
	void SetDataBits(BYTE ucByteSize);
	void SetParityBits(BYTE ucParityID);
	void SetStopBits(BYTE ucStopBitID);
	void SetFlowControl(BOOL bFlowCtrl);
	void SetLoopBack(BOOL bLoopBack);
	int WriteDataToCOMPort(BYTE *pBuff, DWORD dwSize);	// Serial Port에 데이터를 쓴다.
#ifdef USE_SEND_Q_FOR_WRITE
	DWORD RegisterDataToQueue(BYTE *pBuff, DWORD dwSize);	// 쓰기 버퍼에 데이터를 저장한다.
	DWORD SendQueueDataToCOMPort(void);					// 쓰기 버퍼에 있는 데이터를 UART로 전송한다.
#endif
	int ReadDataFromCOMPort(void); // Serial Port에서 데이터를 읽는다.
};



