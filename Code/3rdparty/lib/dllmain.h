/**
* param	pfWave : 원소스 데이터. (phase + 512개의 데이터) * nNofWave.
* param	pnOutput : 출력결과값. 데이터의 갯수와 동일한 INDEX 값이 리턴됨.
* param	nNofWave : 데이터 갯수.
* param	V : 알고리즘 분석에 필요한 기본 데이터
* param	W : 알고리즘 분석에 필요한 기본 데이터
*/


#pragma pack (push, 1)
typedef struct _recvbody {
	int phase;
	short data[512];
} RECVBODY;

typedef struct _prpdData
{
	short phase;
	short smax;
	short smin;
	char bType;
} PRPDDATA;

typedef struct _div20Data {
	int nrealidx;
	int ndiff;
} DIV20DATA;


typedef struct _qminfo
{
	float pdqmmax;			// pd qmmax
	float noiseqmmax;		// noise qm max
	float pdpps;			// pd pps
} QMINFO;

typedef struct _pulseinfo	// 전체 데이터를 20등분으로 나눔. 
{
	float time_dur;
	float pulse_tan;
	float pulse_area;
	float stan_devi;
	int ngain;
	int npdqm;
	int nnoiseqm;
	int npd;
	int nnoise;
	int resultstate;
} PULSINFO;

typedef struct _resltinfo
{
	PULSINFO pulseinfo;
	short prpdinfo[3][256][256];
	short PPRS[60][256];
	short pulse[20][512];
} RESULTINFO;
#pragma pack (pop)


#define byte unsigned char


void run(byte** pdata, int nchcnt, _resltinfo** result);  