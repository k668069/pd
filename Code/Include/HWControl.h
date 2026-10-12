/////////////////////////////////////////////////////////// 
// HWControl.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(HWCONTROL_H__INCLUDED_)
#define HWCONTROL_H__INCLUDED_

#include "TypeDef.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//
#define DEF_LEDCTRL	1	// LoRa, dumy

// gpio
#define	GPIO_LED_RUN	960	// Run LED (Low Active)
#define GPIO_RST_PWR  961 // Power reset
#define	GPIO_LED_LTX	962 	// LoRa_TX_LED (Low Active)
#define	GPIO_LED_LRX	963	// LoRa_RX_LED (Low Active)
#define	GPIO_RST_LMN	964	// LM100_RESETn (Low Active)
#define	GPIO_RST_F3 	965	// F3_nRST, 25MHz 분배기 Reset (Low Active)
// 966
// 967
// #define	GPIO_INT_NRC	968	// halow interrupt input
#define	GPIO_RST_NRC	969	// halow reset (High Active)
#define	GPIO_IO_NRC		970	// halow gpio // 사용안함
/////////////////////////////////////////////////////////////////////////////////////////////////
// Function Definition
void InitializePSHardware(void);
void DestroyPSHardware(void);
void HWCtrlGetSWVersion(BYTE *ucMajor, BYTE *ucMiddle, BYTE *ucMinor, char strDate[], char strDescription[]);
void HWCtrlRebootSystem(void);
void PSGPIOExpSet(WORD wPIONo);
void PSGPIODirSet(WORD wPIONo, BYTE bOut);
void PSGPIOOut(WORD wPIONo, BYTE bOn);
int PSGPIOIn(WORD wPIONo);
void HWCtrlColdRestart(void);

void HWCtrlBlinkRunLED(void);
void HWCtrlLEDOnOff(WORD wPIONo, BOOL bOn);
void HWCtrlCommLEDOn(BYTE ucIdx, BOOL bTx);
void HWCtrlCheckCommLEDOff(void);

void HWCtrlResetLoRAModule(void);
void HWCtrlWaitTo485WriteBufferEmpty(void);
void HWCtrlChange485ModeWrite(BOOL bTx);
#endif //!defined(HWCONTROL_H__INCLUDED_)
