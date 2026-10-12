///////////////////////////////////////////////////////////
// Agent.h
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(AGENT_H__INCLUDED_)
#define AGENT_H__INCLUDED_

enum AgentIDDef{
	YET_AID_RPR_KDN		= 0x01,
	YET_AID_CMC_GB		= 0x02,
	YET_AID_CFL_50		= 0x03,
	YET_AID_S2E_2440	= 0x04,
	YET_AID_CRL_20		= 0x05,
	YET_AID_IPTU			= 0x06,
	YET_AID_IOTGW		= 0x07,
	YET_AID_ZWRTU		= 0x08,
};

enum MPUBoardVersionTypeDef{
	YET_BVT_PXA270		= 0x01,
	YET_BVT_LM32			= 0x02,
	YET_BVT_IMX6S		= 0x03,
	YET_BVT_ZYNQ			= 0x04,
};

#define MPU_AGENT_ID		YET_AID_ZWRTU
#define MPU_BOARD_VERSION	YET_BVT_ZYNQ

#define MODEL_NAME	"PSAM"	//	"IED"

#define HWPF_NAME	"ZWRTU"	// dtb, ki, ramdisk
#define AGENT_NAME	"ZWPD"   // UI
#define AGENT_DIR	"/ZWRTUAgent"
// #define FLASH_DIR	"/mnt/mmcblk0p2" 
#define FLASH_DIR	"/app" 
#define IF_NAME		"eth0" 

#endif //!defined(AGENT_H__INCLUDED_)
