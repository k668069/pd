/////////////////////////////////////////////////////////// 
// HWControl.cpp 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "HWControl.h"
#include "StringApp.h"
#include "TimeApp.h"
#include "DeviceInfoDB.h"
#include "Agent.h"
#include "LogMessage.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Etc
void HWCtrlGetSWVersion(BYTE *ucMajor, BYTE *ucMiddle, BYTE *ucMinor, char strDate[], char strDescription[])
{
	int nNumber;
	FILE *fp;
	
	*ucMajor = *ucMiddle = *ucMinor = 0;
	strDate[0] = 0;
	strDescription[0] = 0;
	if((fp = fopen(AGENT_DIR"/Version.txt", "rt")) != NULL){
		if(ReadIntegerFromFile(fp, &nNumber, '=') == TRUE) *ucMajor = (BYTE)nNumber;
		if(ReadIntegerFromFile(fp, &nNumber, '=') == TRUE) *ucMiddle = (BYTE)nNumber;
		if(ReadIntegerFromFile(fp, &nNumber, '=') == TRUE) *ucMinor = (BYTE)nNumber;
		ReadStringFromFile(fp, strDate, 50, '=');
		ReadStringFromFile(fp, strDescription, 50, '=');
		fclose(fp);
	}
}

void HWCtrlRebootSystem(void)
{
	system("reboot -f");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// PS GPIO Handing
void PSGPIOUnExpSet(WORD wPIONo)
{
	char szCmd[100];

	sprintf(szCmd, "echo %d > /sys/class/gpio/export", wPIONo);
	system(szCmd);
	sprintf(szCmd, "echo %s > /sys/class/gpio/gpio%d/direction", (bOut)?"out":"in", wPIONo);
	system(szCmd);
}

void PSGPIOExpSet(WORD wPIONo)
{
	char szCmd[100];
	sprintf(szCmd, "echo %u > /sys/class/gpio/export", wPIONo);
	system(szCmd);
}

void PSGPIODirSet(WORD wPIONo, BYTE bOut)
{
	char szCmd[100];
	sprintf(szCmd, "echo %s > /sys/class/gpio/gpio%d/direction", (bOut)?"out":"in", wPIONo);
	system(szCmd);
}

void PSGPIOOut(WORD wPIONo, BYTE bOn)
{
	char szCmd[100];

	sprintf(szCmd, "echo %d > /sys/class/gpio/gpio%d/value", bOn, wPIONo);
	system(szCmd);
}

int PSGPIOIn(WORD wPIONo)
{
	int nValue = 0;
	char szFile[100];
	sprintf(szFile, "/sys/class/gpio/gpio%d/value", wPIONo);
	FILE *fp;
	if((fp = fopen(szFile, "rt")) == NULL) return 0;
	fscanf(fp, "%d", &nValue);
	fclose(fp);
	return nValue;
}

void HWCtrlBlinkRunLED(void)
{
	static BOOL bOn = FALSE;

	bOn ^= 0x01;
	PSGPIOOut(GPIO_LED_RUN, bOn);
}

void HWCtrlLEDOnOff(WORD wPIONo, BOOL bOn)
{
	PSGPIOOut(wPIONo, !bOn);
}

void HWCtrlResetLoRAModule(void)
{
	PSGPIOOut(GPIO_RST_LMN, 0);
	SleepMilliSeconds(100);
	PSGPIOOut(GPIO_RST_LMN, 1);
}

void HWCtrlResetHalowModule(void)
{
	PSGPIOOut(GPIO_RST_NRC, 0);
	SleepMilliSeconds(100);
	PSGPIOOut(GPIO_RST_NRC, 1);
	SleepMilliSeconds(100);
	PSGPIOOut(GPIO_RST_NRC, 0);
}

void InitializePSGPIO(void)
{
	PSGPIOUnExpSet(GPIO_LED_RUN);
	PSGPIOExpSet(GPIO_LED_RUN);
	PSGPIODirSet(GPIO_LED_RUN, 1);  // 1:out, 0:in
	
	PSGPIOUnExpSet(GPIO_RST_F3);
	PSGPIOExpSet(GPIO_RST_F3);
	PSGPIODirSet(GPIO_RST_F3, 1);
	PSGPIOOut(GPIO_RST_F3, 1);
	
	PSGPIOUnExpSet(GPIO_LED_LTX);
	PSGPIOExpSet(GPIO_LED_LTX);
	PSGPIODirSet(GPIO_LED_LTX, 1);
	PSGPIOOut(GPIO_LED_LTX, 1);
	
	PSGPIOUnExpSet(GPIO_LED_LRX);
	PSGPIOExpSet(GPIO_LED_LRX);
	PSGPIODirSet(GPIO_LED_LRX, 1);
	PSGPIOOut(GPIO_LED_LRX, 1);
	
	PSGPIOUnExpSet(GPIO_RST_LMN);
	PSGPIOExpSet(GPIO_RST_LMN);
	PSGPIODirSet(GPIO_RST_LMN, 1);
	PSGPIOOut(GPIO_RST_LMN, 1);
	
	PSGPIOUnExpSet(GPIO_RST_NRC);
	PSGPIOExpSet(GPIO_RST_NRC);
	PSGPIODirSet(GPIO_RST_NRC, 1);	// out	
	PSGPIOOut(GPIO_RST_NRC, 0);
	
	PSGPIOUnExpSet(GPIO_IO_NRC);
	PSGPIOExpSet(GPIO_IO_NRC);
	PSGPIODirSet(GPIO_IO_NRC, 1);	// out
	PSGPIOOut(GPIO_IO_NRC, 1);
		
	PSGPIOUnExpSet(GPIO_RST_PWR);
	PSGPIOExpSet(GPIO_RST_PWR);
	PSGPIODirSet(GPIO_RST_PWR, 1); // out
	PSGPIOOut(GPIO_RST_PWR, 1);
}
// End of PS GPIO Handing
/////////////////////////////////////////////////////////////////////////////////////////////////

void InitializePSHardware(void)
{
	InitializePSGPIO();
}

void DestroyPSHardware(void)
{
}




