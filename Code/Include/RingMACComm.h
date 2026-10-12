///////////////////////////////////////////////////////////
// RingMACComm.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(RINGMACCOMM_H__INCLUDED_)
#define RINGMACCOMM_H__INCLUDED_

#include <stdio.h>
#include "TypeDef.h"

#define MAX_RCV_PACKET_SIZE 500
#define MAX_SND_PACKET_SIZE 500

enum YET_SpecialCharacterDef{
	YET_STX = 0xFF,
	YET_ETX = 0xFE,
	YET_DLE = 0xFD
};

enum YET_CIDDef{
	YET_CID_BROAD	= 0x00,
	YET_CID_SET		= 0x01,
	YET_CID_GET		= 0x02,
	YET_CID_REPLY	= 0x03,
	YET_CID_ACK		= 0x06,
	YET_CID_TOKEN	= 0x07,
	YET_CID_NAK		= 0x15
};

#ifdef __cplusplus
extern "C" {
#endif

void InitializeRMACComm(void);
void DestroyRMACComm(void);
int ProcessRMACCommMain(void);

#ifdef __cplusplus
}
#endif
#endif //RINGMACCOMM_H__INCLUDED_
