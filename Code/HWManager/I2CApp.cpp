///////////////////////////////////////////////////////////
// I2CApp.c
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#include "TimeApp.h"
#include "LogMessage.h"

void DebugLogMessageI2CPacket(BYTE pData[], int nSize, BOOL bSend)
{
	static int nIndex = 0;
	FILE *fp = OpenDebugLogFile(LOG_DEBUG_I2CDATA);
	if(fp == NULL) return;

	if(bSend) fprintf(fp, "%03d <%s> SND=>", nIndex++, GetCurrentDateTimeStr(TIME_FORMAT_ONLY_TIME_MILLI));
	else fprintf(fp, "%03d <%s> RCV<=", nIndex++, GetCurrentDateTimeStr(TIME_FORMAT_ONLY_TIME_MILLI));
	int i;
	for(i = 0 ; i < nSize ; i++){
		fprintf(fp, " %02X", pData[i]);
	}
	fprintf(fp, "\n");

	nIndex %= 1000;
	if(fp != stdout) fclose(fp);
}

// i2c-dev는 read/write 함수 호출시 기본적으로 START와 STOP을 발생시킨다
static BOOL WriteDataToIIC(int nFd, BYTE ucDevAddr, BYTE ucAddr, BYTE Data[], WORD wSize)
{
	BYTE Buf[100];

	if(nFd < 0) return FALSE;
	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	Buf[0] = ucAddr;
	memcpy(Buf+1, Data, wSize);
	DebugLogMessageI2CPacket(Buf, wSize+1, TRUE);
	WORD wWSize = write(nFd, Buf, wSize+1);
	return (wWSize == (wSize+1));
}

BOOL WriteDataToIICWORD(int nFd, BYTE ucDevAddr, WORD wAddr, BYTE Data[], WORD wSize)
{
	BYTE Buf[100];

	if(nFd < 0) return FALSE;
	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	Buf[0] = (BYTE)(wAddr & 0xFF);
	Buf[1] = (BYTE)((wAddr>>8) & 0xFF);
	memcpy(Buf+2, Data, wSize);
	DebugLogMessageI2CPacket(Buf, wSize+2, TRUE);
	WORD wWSize = write(nFd, Buf, wSize+2);
	return (wWSize == (wSize+2));
}

static BOOL ReadDataFromIIC(int nFd, BYTE ucDevAddr, BYTE ucAddr, BYTE Data[], WORD wSize)
{
	BYTE Addr[2];

	if(nFd < 0) return FALSE;
	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	Addr[0] = ucAddr;
	DebugLogMessageI2CPacket(Addr, 1, TRUE);
	if(write(nFd, Addr, 1) < 1) return FALSE;

	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	WORD wRSize = read(nFd, Data, wSize);
	DebugLogMessageI2CPacket(Data, wRSize, FALSE);
	return (wRSize == wSize);
}

static BOOL ReadDataFromIIC2(int nFd, BYTE ucDevAddr, BYTE ucAddr, BYTE Data[], WORD wSize)
{
	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg             i2cmsg;

	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;

	i2cmsg.addr  = ucAddr;
	i2cmsg.flags = I2C_M_RD;
	i2cmsg.len   = wSize;
	i2cmsg.buf   = Data;
	if(ioctl(nFd, I2C_RDWR, &msg_rdwr) < 0) {
		printf("read: %s\n", strerror(errno));
		return FALSE;
	}
	DebugLogMessageI2CPacket(Data, wSize, FALSE);
	return TRUE;
}

BOOL ReadDataFromIICWORD(int nFd, BYTE ucDevAddr, WORD wAddr, BYTE Data[], WORD wSize)
{
	BYTE Addr[2];

	if(nFd < 0) return FALSE;
	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	Addr[0] = (BYTE)(wAddr & 0xFF);
	Addr[1] = (BYTE)((wAddr>>8) & 0xFF);
	DebugLogMessageI2CPacket(Addr, 2, TRUE);
	if(write(nFd, Addr, 2) < 2) return FALSE;

	ioctl(nFd, I2C_SLAVE, ucDevAddr);
	WORD wRSize = read(nFd, Data, wSize);
	DebugLogMessageI2CPacket(Data, wRSize, FALSE);
	return (wRSize == wSize);
}

BOOL ReadDataFromIICWORD2(int nFd, BYTE ucDevAddr, WORD wAddr, BYTE Data[], WORD wSize)
{
	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg             i2cmsg;

	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;

	i2cmsg.addr  = wAddr;
	i2cmsg.flags = I2C_M_RD;
	i2cmsg.len   = wSize;
	i2cmsg.buf   = Data;
	if(ioctl(nFd, I2C_RDWR, &msg_rdwr) < 0) {
		printf("read: %s\n", strerror(errno));
		return FALSE;
	}
	DebugLogMessageI2CPacket(Data, wSize, FALSE);
	return TRUE;
}

#define I2C_DEV_FILE_TH    "/dev/i2c-1" //
#define I2C_DEV_ADDR_TH	(0x51)

void ReadTempAndHumidFromTH(double &fTemperature, double &fHumidity)
{
	BYTE Addr[2];
	BYTE Data[4] = {0,};
	int nFd;

	fTemperature = fHumidity = 0;
	if((nFd = open(I2C_DEV_FILE_TH, O_RDWR)) < 0) return;
	ioctl(nFd, I2C_SLAVE, I2C_DEV_ADDR_TH);
	Addr[0] = 0;
	DebugLogMessageI2CPacket(Addr, 1, TRUE);
	if(write(nFd, Addr, 1) < 1) {
		close(nFd);
		return;
	}

	SleepMilliSeconds(35);
	ioctl(nFd, I2C_SLAVE, I2C_DEV_ADDR_TH);
	WORD wRSize = read(nFd, Data, 4);
	DebugLogMessageI2CPacket(Data, wRSize, FALSE);
	close(nFd);

	fHumidity = 100. * ((Data[0]<<8) + Data[1])/0x3FFF;
	fTemperature = 165. * ((Data[2]<<6) + (Data[3]>>2))/0x3FFF - 40;
}

BOOL InitializeI2C(void)
{
	int nFd = open(I2C_DEV_FILE_TH, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(nFd < 0){
		PrintDbgMsg(LOG_ERR, "[I2C] Sensor Device Err\n");
		return FALSE;
	}
	close(nFd);

	return TRUE;
}

void DestroyI2C(void)
{
}