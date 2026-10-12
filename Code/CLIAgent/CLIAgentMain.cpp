/////////////////////////////////////////////////////////// 
// CLIAgentMain.cpp
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/timeb.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>

#include "DeviceInfoDB.h"
#include "CommandManager.h"
#include "IPCSharedMemory.h"
#include "LogMessage.h"
#include "LogMessageQueue.h"
#include "TimeApp.h"
#include "StringApp.h"
#include "CommonApp.h"

YST_DB_MAIN	*gpDB = NULL;	//DeviceInfoDB.h
YST_IPC		*gpIPC = NULL;	//IPCSharedMemory.h
YST_DB_LOG	*gpLog = NULL;	//LogMessage.h
FILE *input, *output;
int gnTelnetFlag = 0;
CCommandManager Command;

void Printf(const char *strFormat, ...)
{
	va_list ap;
	char strData[500];

	va_start(ap, strFormat);
	vsprintf(strData, strFormat, ap);
	va_end(ap);
	fprintf(output, strData);
}

void PrintShowHelp(int nFlag)
{
	Printf("===============================================================================\r\n");
	Printf("  Help                                                                         \r\n");
	Printf("===============================================================================\r\n");
	Printf("help(?)         : Show this Message\r\n");
	Printf("help set(? set) : Show help message about set\r\n");
	Printf("reset(reboot 1) : Restart this system, [None:Warm, 1:Cold]\r\n");
	if(gnTelnetFlag) Printf("exit(quit, close)    : close telnet connection\r\n");
	Printf("show(sh)       : show the system data\r\n");

	Printf("sh (clear) dbglog : Show (clear) debug log file\n");
	Printf("sh (clear) log : Show (clear) message(event) log\n");
	Printf("sh (clear) syslog 1 2 : Show (clear) system log file, lines 1~2\n");
	Printf("sh (clear) sdata : Show (clear) sensor Rcv Data Log\n");
	Printf("sh sderr       : Show Sensor Rcv Data Err-Log\r\n");
	Printf("sh rmac        : Show Status of Ring MAC\r\n");
	Printf("sh platform    : Show Information of Platform\r\n");
	Printf("sh comm        : Show Information of UART Setting\r\n");
	Printf("sh rfm         : Show Information of R/F Module\r\n");
	Printf("sh lora        : Show Information of LoRa Module\r\n");
	Printf("sh snode       : Show Information of Sensor Node\r\n");
	Printf("sh ursnode     : Show Information of Unregisterd Sensor Node\r\n");
	Printf("sh sport       : Show Information of Serial 485 Port Setting\r\n");
	Printf("sh pf          : Show Information of Port Fowarding Policy\r\n");
	Printf("sh route       : Show Routing Table\r\n");
	Printf("sh pd          : Show PD Information\r\n");
	Printf("sh pddata      : Show PD Data\r\n");
	Printf("echo on/off    : Control echo on or off\r\n");
	Printf("save log       : Save log data to flash\r\n");

	Printf("\r\n");
}


void PrintSetHelp(int nFlag)
{
	Printf("===============================================================================\r\n");
	Printf("  Set Help                                                                     \r\n");
	Printf("===============================================================================\r\n");
	Printf("help(?)              : Show this Message\r\n");
	Printf("help show(? show)    : Show help message about show\r\n");
	Printf("help set(? set)      : Show help message about set\r\n");
	Printf("set ip xxx.xxx.xxx.xxx      : set system ip as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set subnet xxx.xxx.xxx.xxx  : set system subnet mask as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set gateway xxx.xxx.xxx.xxx : set system default gateway as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set gwif wan/halow             : set default gw as [wan/halow]\r\n");
	Printf("set hmode off/ap/sta/mpp/mp    : set halow mode as [ap]\r\n");
	Printf("set hch 4                      : set halow channel as [1~6]\r\n");
	Printf("set hapip xxx.xxx.xxx.xxx      : set halow AP ip as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set hapsec 0                   : set halow AP ip as [0:open,1:WPA2-PSK,2:WPA3-OWE,3:WPA3-SAE]\r\n");
	Printf("set hapid halow_id             : set halow AP SSID as [halow_id]\r\n");
	Printf("set hdhcp on/off               : set halow station ip DHCP [on/off]\r\n");
	Printf("set hstip xxx.xxx.xxx.xxx      : set halow station ip as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set hstsec 0                   : set halow station ip as [0:open,1:WPA2-PSK,2:WPA3-OWE,3:WPA3-SAE]\r\n");
	Printf("set hstid halow_id             : set halow station SSID as [halow_id]\r\n");
	Printf("set hmedhcp on/off             : set halow mesh ip DHCP [on/off]\r\n");
	Printf("set hmeip xxx.xxx.xxx.xxx      : set halow mesh ip as [xxx.xxx.xxx.xxx]\r\n");
	Printf("set hmesec 0                   : set halow mesh ip as [0:open,3:WPA3-SAE]\r\n");
	Printf("set hmeid halow_kdn            : set halow mesh SSID as [halow_kdn]\r\n");
	Printf("set pw 1234                 : set password to [1234]\r\n");
	Printf("set nodename Node-1         : set node name of RingMAC to [Node-1]\r\n");
	Printf("set rget                    : get RingMAC Information\r\n");
	Printf("set poleno 110L18           : set pole no of Chassis to [110L18]\r\n");
	Printf("set manno 0819D401          : set manage no of Chassis to [0819D401]\r\n");
	Printf("set indate 2016/10/12       : set install date of Chassis to [2016/10/12]\r\n");
	Printf("set myip 192.168.10.33      : set the myip for platform as [192.168.10.33]\r\n");
	Printf("set platip 61.40.220.127    : set the platform ip as [61.40.220.127]\r\n");
	Printf("set myport 5683             : set the CoAP port as [5683]\r\n");
	Printf("set platport 5683           : set the platform CoAP port as [5683]\r\n");
	Printf("set oid 1.2.481.1.3.1.10160828084 : set the OID as [1.2.481.1.3.1.10160828084]\r\n");
	Printf("set period 60               : set the status report period as [60]\r\n");
	Printf("set bsize 0                 : set the platform block size as [0:2048, 1:1024, 2:512]\r\n");
	Printf("set platsec on/off       : set the platform security as [on/off]\r\n");
	Printf("set secdbg 1             : set the platform security debug as [0:no, 1~4:level]\r\n");
	Printf("set secpw 1234           : set the platform security password as [1234]\r\n");
	Printf("set freq 1                  : set channel freq of LoRa module to 1\r\n");
	Printf("set sfactor 7               : set the spreading factor for LoRa module as [7]\r\n");
	Printf("set usekey 1                : set to use the crypto key for LoRa module as [1]\r\n");
	Printf("set key 1234-5678-90ab-cdef : set to use the key for LoRa as crypto data\r\n");
	if(nFlag) Printf("set loratxp 14             : set to use the Tx Power for LoRa module as [0~14]\r\n");
	Printf("set snodename m Sensor-1 : set the name for m-th sensor node as [Sensor-1]\r\n");
	Printf("set snperiod m 60        : set the report period for m-th sensor node as 60 sec\r\n");
	Printf("set snreboot m           : reboot the m-th sensor node\r\n");
	Printf("set sncfg m PR60         : set the report period for m-th sensor node as 60 sec\r\n");
	Printf("set sndest m C6,E14:0C:5B:FF:FF:00:01:AE : set the dest G/W for m-th node\r\n");
	Printf("set sntime m             : synchronize the time of m-th sensor node\r\n");
	Printf("del snode 1              : delete first sensor node\r\n");
	Printf("del snode all            : delete all invalid sensor node\r\n");
	Printf("del ursnode 1            : delete first URS node\r\n");
	Printf("del sdata                : delete sensor data table\r\n");
	Printf("set delay m 30                 : set the m-th comm. port delay time as [30ms]\r\n");
	Printf("set maxsize m 300              : set the m-th comm. port max packet size as [300]\r\n");
	Printf("set baudrate m 1200/1800/2400  : set the m-th comm. port baudrate as [1200bps]\r\n");
	Printf("    4800/9600/19200/38400/57600/115200/230400/460800/921600/1500000\r\n");
	Printf("set databits m 5/6/7/8         : set the m-th comm. port databits as [8]\r\n");
	Printf("set stopbits m 1/2             : set the m-th comm. port stopbits as [1]\r\n");
	Printf("set parity m no/odd/even       : set the m-th comm. port parity as [noparity]\r\n");
	Printf("set reinit m                   : reinitalize m-th comm. port\r\n");
	Printf("set sreset m                   : reset m-th comm. port\r\n");
	Printf("set devcnt m 3                 : set the m-th serial port device count as [3]\r\n");
	Printf("set polltime m 180             : set the m-th serial port polling interval as [180sec]\r\n");
	Printf("set waittime m 500             : set the m-th serial port polling wait time as [500ms]\r\n");
	Printf("set pfip 1 xxx.xxx.xxx.xxx  : set the 1'st port-forwarding ip as xxx.xxx.xxx.xxx\n");
	Printf("set pfsport 1 80            : set the 1'st port-forwarding source port as <8080>\n");
	Printf("set pfdport 1 80            : set the 1'st port-forwarding destination port as <80>\n");
	Printf("set pfp 1 tcp/udp/off       : set the 1'st port-forwarding protocol\n");
	Printf("set rnet 1 xxx.xxx.xxx.0    : set the 1'st routing table network as xxx.xxx.xxx.0\n");
	Printf("set rgw 1 xxx.xxx.xxx.xxx   : set the 1'st routing table gateway as xxx.xxx.xxx.xxx\n");
	Printf("set ruse 1 on/off           : set the 1'st routing table policy usage as [on/off]\n");
	Printf("set pdmonport 8000   : set monitor port as [8000]\r\n");
	Printf("set pdmode 1         : set the inner daq mode as [0:PQN, 1:SCOPE, 2:ETC]\r\n");
	Printf("set pdsync 0         : set the inner daq sync as [0:RCA, 1:Gate, 2:Virtual]\r\n");
	Printf("set pdsyncval 0      : set the inner daq sync_value as [-10~10]\r\n");
	Printf("set pdgain 0         : set the inner daq gain_step as [0~5]\r\n");
	Printf("set pdtrig 1660      : set the inner daq trigger as [mV]\r\n");
	Printf("set pdfiltermin 1    : set the inner daq filter as min[1~75]\r\n");
	Printf("set pdfiltermax 75   : set the inner daq filter as max[1~75]\r\n");
	Printf("set pdtype 1         : set the inner daq sensor type as [0:UHF, 1:AE, 2:GAIN, 3:ENVELOP]\r\n");
	Printf("set pdformat 0       : set the inner daq as virtual data [0:comple, 1:binary]\r\n");
	Printf("set pdperiod 1       : set the sample pd period as 1sec\r\n");
	Printf("set pdadctest 0 1    : set the inner daq ADC as ch[0:AB, 1:A, 2:B] pattern[1~5]\r\n");
	Printf("set pdadcvir on      : set the inner daq as virtual data [on/off]\r\n");
	Printf("set pdadcread        : set the inner daq ADC as reading\r\n");
	Printf("set pddaq	     : set the inner daq status\r\n");
	Printf("set pdadcreset       : set the inner daq ADC as resetting\r\n");
	Printf("set pdidle           : set the daq precess as stoping\r\n");

	if(nFlag){
		Printf("debug                 : show debug help\r\n");
		Printf("show sizeof           : view the sizeof YST_DB\r\n");
		Printf("clear(reset) db       : Initialize DB\r\n");
		Printf("clear env             : Initialize Environment\r\n");
		Printf("set autoreboot 60           : set auto reboot time to every 60min\r\n");
		Printf("def mac 00:19:F8:01:00:01   : set mac address as [00:19:F8:01:00:01]\r\n");
		Printf("def sn JSNB101012001        : set serial number of IPTU as [JSNB101012001]\r\n");
		Printf("def rmac 36:95:51:00:00:01  : set rmac address as [36:95:51:00:00:01]\r\n");
		Printf("set dns xxx.xxx.xxx.xxx     : set DNS ip address as [xxx.xxx.xxx.xxx]\r\n");
		Printf("set ntp on/off              : set the use of NTP as on or off\r\n");
		Printf("set eth 0 10f : set Ethernet. mode [auto, 10h, 10f, 100h, 100f, 1000h, 1000f, off]\r\n");
		Printf("set telnet on/off           : set the use of TELNET as on or off\r\n");
		Printf("set ftp on/off              : set the use of FTP as on or off\r\n");
	}

	Printf("\r\n");
}


void PrintDebugHelp(int nShowMoreFlag)
{
	Printf("===============================================================================\r\n");
	Printf("  Debug Help                                                                   \r\n");
	Printf("===============================================================================\r\n");
	Printf("debug tcp        : show network processor message\r\n");
	Printf("debug upgrade    : show upgrade processor message\r\n");
	Printf("debug updata     : show upgrade data\r\n");
	Printf("debug sizeof     : show size of structs\r\n");
	Printf("debug comdata    : show raw serial data\r\n");
	Printf("debug compacket  : show serial packet\r\n");
	Printf("debug socket     : show socket message\r\n");
	Printf("debug socdata    : show socket data\r\n");
	Printf("debug udp        : show udp message\r\n");
	Printf("debug udpdata    : show udp data\r\n");
	Printf("debug coap       : show CoAP message\r\n");
	Printf("debug coapdata   : show CoAP data\r\n");
	Printf("debug coapdetail : show CoAP detail payload\r\n");
	Printf("debug platsec : show Security message for Platform\r\n");
	Printf("debug lora       : show LoRa message\r\n");
	Printf("debug loradata   : show LoRa data\r\n");
	Printf("debug scomm      : show SComm message\r\n");
	Printf("debug gps        : show SComm GPS message\r\n");
	Printf("debug i2c        : show i2c debug message\r\n");
	Printf("debug telnet     : show telnet message\r\n");
	Printf("debug pdnode     :  show internal PD-Sensor message\r\n");
	Printf("debug pdtcp      : show TCP/PD message\r\n");
	Printf("debug pdtcpdata  : show TCP/PD message data\r\n");
	Printf("debug scommsend 1 AT+ECHO=0: send packet [AT+ECHO=0\\r] to SComm Port-1\r\n");

	if(nShowMoreFlag) {
		Printf("debug scommsend 1 AT+MacPanid 0x0001\r\n");
		Printf("debug scommsend 1 AT+MacshortAddress 0x0000\r\n");
		Printf("debug scommsend 1 AT+IotNetwork 0\r\n");
		Printf("debug scommsend 1 set phyCurrentChannel 10\r\n");
		Printf("debug scommsend 1 AT+CommunicationType 0x88\r\n");
		Printf("6:LoRa, 9:UDP\r\n");
		Printf("debug flagsave : save current debug flag, and init at startup\r\n");
	}
	Printf("\r\n");
}

void PrintVersionInformation(void)
{
	BYTE *mCV = gpDB->RMac.mChipID;
	Printf("Chip Version = V%d.%d.%d, %04d/%02d/%d%d\n", mCV[0]>>4, mCV[0]&0xF, mCV[1]>>4, ((mCV[1]<<4)&0xFF) + (mCV[2]>>4) + 2000, mCV[2]&0xF, mCV[3]>>4, mCV[3]&0xF);
	Printf("F/W Version  = %d.%d.%d\n", gpDB->Version.ucMajor, gpDB->Version.ucMiddle, gpDB->Version.ucMinor);
	Printf("Build Date   = %s\n", gpDB->Version.strDate);
	Printf("Description  = %s\n", gpDB->Version.strDescription);
	system("cat /proc/version");
	system("cat /rdiskversion");
	Printf("\n");
}

void PrintChassisInformation(int nShowMoreFlag)
{
	Printf("Device ID             : %d\n", gpDB->Chassis.ucModelID);
	Printf("Model Name            : %s\n", gpDB->Chassis.szDevName);
	Printf("Serial Number         : %s\n", gpDB->Chassis.szSerialNo);
	Printf("Pole Number(poleno)   : %s\n", gpDB->Chassis.szPoleNo);
	Printf("Manage Number(manno)  : %s\n", gpDB->Chassis.szMangeNo);
	Printf("Install Date(indate)  : %s\n", gpDB->Chassis.szInstallDate);
}



void PrintSystemInformation(int nShowMoreFlag)
{
	Printf("=============================================================================\r\n");
	Printf("  System Configuration                                                       \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	Printf("Current Time(time)   : %s\r\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
	Printf("Password(pw)         : %s\r\n", gpDB->General.szPassword);	
	Printf("IP Address(ip)       : %s\r\n", MakeIPAddrString(gpDB->General.mIP));
	if(gpDB->General.ucOperEth1 == YET_ETH_OFF)
		   Printf("IP Address2(ip2)     : Off\r\n");
	else Printf("IP Address2(ip2)     : %s\r\n", MakeIPAddrString(gpDB->General.mIP2));
	Printf("Subnet Mask(netmask) : %s\r\n", MakeIPAddrString(gpDB->General.mSM));
	Printf("Gateway(gateway)     : %s\r\n", MakeIPAddrString(gpDB->General.mGW));
	Printf("Default G/W If(gwif)      : %s\r\n", (gpDB->General.ucGWIf==1)?"halow":"wan");
	Printf("Halow Mode(hmode)         : %s\r\n", (gpDB->General.ucHMode==4)?"MP":(gpDB->General.ucHMode==3)?"MPP":(gpDB->General.ucHMode==2)?"AP":(gpDB->General.ucHMode==1)?"Station":"Not Use");
	Printf("Halow Channel(hch)        : %d(%s)\r\n", gpDB->General.ucHChannel, GetHalowFrequency(gpDB->General.ucHChannel));
	Printf(" AP IP(hapip)             : %s\r\n", MakeIPAddrString(gpDB->General.mHMIP1));
	Printf(" AP Security(hapsec)      : %d\r\n", gpDB->General.ucHMSEC1);
	Printf(" AP SSID(hapid)           : %s\r\n", gpDB->General.szHMSSID1);
	Printf(" Station DHCP(hstdhcp)    : %s\r\n", (gpDB->General.ucHMDHCP2)?"DHCP":"Static");
	Printf(" Station IP(hstip)        : %s\r\n", MakeIPAddrString(gpDB->General.mHMIP2));
	Printf(" Station Security(hstsec) : %d\r\n", gpDB->General.ucHMSEC2);
	Printf(" Station SSID(hstid)      : %s\r\n", gpDB->General.szHMSSID2);
	Printf(" Mesh DHCP(hmedhcp)       : %s\r\n", (gpDB->General.ucHMDHCP3)?"DHCP":"Static");
	Printf(" Mesh IP(hmeip)           : %s\r\n", MakeIPAddrString(gpDB->General.mHMIP3));
	Printf(" Mesh Security(hmesec)    : %d\r\n", gpDB->General.ucHMSEC3);
	Printf(" Mesh SSID(hmeid)         : %s\r\n", gpDB->General.szHMSSID3);
	PrintChassisInformation(nShowMoreFlag);

	if(nShowMoreFlag){
		Printf("Auto Reboot(autoreboot) : %dmin\r\n", gpDB->System.wAutoRebootMin);
		Printf("MAC Address(mac)     : %s\r\n", MakeMACAddrString(gpDB->General.mMAC));
		Printf("DNS Address(dns)     : %s\r\n", MakeIPAddrString(gpDB->General.mDNS));
		Printf("NTP use(ntp)         : %s\r\n", (gpDB->General.bNTP) ? "On" : "Off");
		Printf("Ethernet mode        : eth0-%s, eth1-%s\r\n", GetEthType(gpDB->General.ucOperEth0), GetEthType(gpDB->General.ucOperEth1));
		Printf("TELNET use(telnet)   : %s\r\n", (gpDB->General.bTelnet) ? "On" : "Off");
		Printf("FTP use(ftp)         : %s\r\n", (gpDB->General.bFtp) ? "On" : "Off");
		Printf("Manager List         : %d\r\n", gpDB->System.ucManagerCount);
		Printf("Idx | Valid | %15s:%5s | ID(Ver) | Managed(sec) | TimeSync\r\n", "IP Address", "Port#");
		DDWORD dwNow = GetTimeTickCount();
		for(int i = 0 ; i < MAX_TCPCON_CNT ; i++){
			if(gpDB->System.M[i].mPeerIP[0] == 0) continue;
			Printf(" %02d | %5u | %15s:%5d | %02X(V%02X) | %8s | %s\r\n", i+1, gpDB->System.M[i].ucValid, MakeIPAddrString(gpDB->System.M[i].mPeerIP), 
				gpDB->System.M[i].wPeerPortNo, gpDB->System.M[i].wManID, gpDB->System.M[i].wManVer, GetSecTimeString((long)((dwNow - gpDB->System.M[i].dwLastSendTime)/1000)),
				gpDB->System.M[i].bTimeSync ? "TRYE" : "FALSE");
		}
	}
	Printf("\r\n");
}

void PrintPortForwardInformation(int nFlag)
{
	const char *strProtocol[3] = {"OFF", "TCP", "UDP"};
	int nProtocol, i;

	Printf("=============================================================================\r\n");
	Printf("  Port Forward Configuration                                                 \r\n");
	Printf("=============================================================================\r\n");
	Printf("\n");
	Printf("Idx Protocol %15s %5s to %15s %5s\r\n", "Source IP", "Port#", "Destination IP", "Port#");
	for(i = 0 ; i < MAX_NAT_PFWD_CNT ; i++){
		nProtocol = gpDB->PFwd[i].ucProtocol;
		if(nProtocol < 0 || nProtocol > 2) nProtocol = 0;
		Printf("%3d %8s %5d -> %15s:%5d\r\n", i+1, strProtocol[nProtocol], gpDB->PFwd[i].nSrcPort, MakeIPAddrString(gpDB->PFwd[i].mPFWDIP), gpDB->PFwd[i].nDestPort);
		if(gpDB->PFwd[i].ucProtocol == 0) break;
	}
}

void PrintRouteTableInformation(int nFlag)
{
	Printf("=============================================================================\r\n");
	Printf("  Routing Table                                                   \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	Printf("Idx Usage %15s to %15s\r\n", "Network", "Gateway");
	for(int i = 0 ; i < MAX_ROUTE_TBL_CNT ; i++){
		Printf("%3d %5s %15s -> %15s\r\n", i+1, gpDB->Route[i].ucUse?"On":"Off", MakeIPAddrString(gpDB->Route[i].mRNet), MakeIPAddrString(gpDB->Route[i].mRGW, 1));
	}
}

void PrintRMacInformation(int nShowMoreFlag)
{
	BYTE *mLink = gpDB->RMac.mLink;
	BYTE *mMiscable = gpDB->RMac.mMiscable;

	BYTE *mCV = gpDB->RMac.mChipID;
	BYTE *mFV = gpDB->RMac.mFWVersion;
	Printf("=============================================================================\r\n");
	Printf("  RMAC Status                                                                \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	Printf("Device ID      : %s\r\n", GetDevType(gpDB->RMac.ucDevID));
	Printf("Chip Version   : V%d.%d.%d, %04d/%02d/%d%d\r\n", mCV[0]>>4, mCV[0]&0xF, mCV[1]>>4, ((mCV[1]<<4)&0xFF) + (mCV[2]>>4) + 2000, mCV[2]&0xF, mCV[3]>>4, mCV[3]&0xF);
	Printf("F/W Version    : %04d/%02d/%d-%d\r\n", mFV[0] + 2000, mFV[1], mFV[2], mFV[3]);
	Printf("Ring MAC Addr  : %s\r\n", MakeMACAddrString(gpDB->RMac.mRMAC));
	Printf("Man IP Address : %s\r\n", MakeIPAddrString(gpDB->RMac.mManIPAddr));
	Printf("Closed Ring    : %s\r\n", (gpDB->RMac.bClosedRing) ? "Closed" : "Open");
	Printf("Link E/W/H/L   : %s / %s / %s / %s\r\n", GetRMacLink(mLink[0], mMiscable[0]), GetRMacLink(mLink[1], mMiscable[1]), GetMacLink(mLink[2]), GetMacLink(mLink[3]));
	Printf("Node Count E/W : %d / %d\r\n", gpDB->RMac.mNodeCount[0], gpDB->RMac.mNodeCount[1]);

	if(nShowMoreFlag){
		int i, j, nNodeCnt;
		for(i = 0 ; i < 2 ; i++){
			Printf("%s Node : \r\n", i ? "West" : "East");
			Printf("Idx DevID FW CR Link E/W/H/L    RingMAC IP-Address\r\n");
			nNodeCnt = MAX_NODE_CNT;
			if(nNodeCnt > gpDB->RMac.mNodeCount[i]) nNodeCnt = gpDB->RMac.mNodeCount[i];
			for(j = 0 ; j < nNodeCnt ; j++){
				mLink = gpDB->Node[i][j].mLink;
				mMiscable = gpDB->Node[i][j].mMiscable;
				BYTE *mMAC = gpDB->Node[i][j].mRMAC;
				BYTE *mIP = gpDB->Node[i][j].mManIPAddr;
				Printf("%3d %s %2d ", j+1, GetDevType(gpDB->Node[i][j].ucDevID), gpDB->Node[i][j].ucFWVersion);
				Printf("%2s %4s/%4s/%4s/%4s ", (gpDB->Node[i][j].bClosedRing) ? "C" : "O", GetRMacLink2(mLink[0], mMiscable[0]), GetRMacLink2(mLink[1], mMiscable[1]), GetMacLink2(mLink[2]), GetMacLink2(mLink[3]));
				Printf("%02X%02X%02X %02X%02X%02X%02X\r\n", mMAC[3], mMAC[4], mMAC[5], mIP[0], mIP[1], mIP[2], mIP[3]);
			}
		}
	}

	Printf("\n");
}


void PrintPlatformInformation(int nFlag)
{
	Printf("=============================================================================\r\n");
	Printf("  Platform Information                                                    \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	Printf("My IP Address(myip)     : %s\r\n", MakeIPAddrString(gpDB->Platform.mMyIP));
	Printf("Platform IP(platip)     : %s\r\n", MakeIPAddrString(gpDB->Platform.mPeerIP));
	Printf("My CoAP Port(myport)    : %d\r\n", gpDB->Platform.wMyPort);
	Printf("Platform Port(platport) : %d\r\n", gpDB->Platform.wPeerPort);
	Printf("URI(uri)                : %s\r\n", gpDB->Platform.szURIPath);
	Printf("CSE-ID(cseid)           : %s\r\n", gpDB->Platform. szCSEID);
	Printf("Type/Ver(m2mver)        : %s / %d\r\n", (gpDB->Platform.ucM2MType)?"None":"oneM2M", gpDB->Platform.ucM2MVer);
	Printf("G/W OID(oid)/Assign     : %s / %d\r\n", gpDB->Platform.szGWOID, gpDB->Platform.bAssignOID);
	Printf("Report Period(period)   : %dsec\r\n", gpDB->Platform.wRptTime);
	Printf("Block Size(bsize)       : %d\r\n", gpDB->Platform.wBlockSize);
	Printf("Security(platsec)       : %s\r\n", (gpDB->Platform.bSec == 0) ? "Off" : "On");
	if(nFlag) {
		Printf("Security debug(secdbg)  : %d\r\n", gpDB->Platform.bSecDebug);
		Printf("Security password(secpw): %s\r\n", gpDB->Platform.szSecPW);
	}
	if(nFlag) {
		Printf("G/W OID Base            : %s\r\n", gpDB->Platform.szOIDBase);
		if(gpDB->Platform.ucM2MVer == 0) {
			Printf("bRegGW/bRegNode/bRegCSE     : %d / %d / %d\r\n", gpDB->Platform.bRegGW, gpDB->Platform.bRegNode, gpDB->Platform.bRegCSE);
			Printf("bRegLocS/bRegStatus/bRegLocP: %d / %d / %d\r\n", gpDB->Platform.bRegLocS, gpDB->Platform.bRegStatus, gpDB->Platform.bRegLocP);
			Printf("bRegDevInfo/bCommOK: %d / %d\r\n", gpDB->Platform.bRegDevInfo, gpDB->Platform.bCommOK);
		}
		else // oneM2M
		{
			Printf("bRegGW/bRegCSE          : %d / %d\r\n", gpDB->Platform.bRegGW, gpDB->Platform.bRegCSE);
			Printf("bRegLoc/bRegDevInfo     : %d / %d\r\n", gpDB->Platform.bRegLocS, gpDB->Platform.bRegDevInfo);
			Printf("bRegStatus/bRegCommand  : %d / %d\r\n", gpDB->Platform.bRegStatus, gpDB->Platform.bRegControl);
			Printf("bRptLoc/bRptControl     : %d / %d\r\n", gpDB->Platform.bRptGWLoc, gpDB->Platform.bRptGWControl);
			Printf("bRptDevInfo/bRptStatus  : %d / %d\r\n", gpDB->Platform.bRptDevInfo, gpDB->Platform.bRptStatus);
			Printf("Comm. : %s\r\n", (gpDB->Platform.bCommOK) ? "OK" : "Fail");
		}
		if(gpDB->Platform.ucM2MVer == 0) Printf("G/W Node ID  : %s\r\n", gpDB->Platform.szNID);
		DDWORD dwNow = GetTimeTickCount();
		Printf("G/W Last Rpt Time : %sms\r\n", GetMilliSecTimeStr((long)(dwNow - gpDB->Platform.dwLastRptTime)));
		Printf("Current Step : %d\r\n", gpDB->Platform.ucPStep);
		Printf("Current SndCnt : %d\r\n", gpDB->Platform.ucSndCnt);
	}
	Printf("\r\n");
}

void PrintSCommInformation(int nFlag)
{
	int i;
	
	Printf("=============================================================================\n");
	Printf("  Serial Port Information                                           \n");
	Printf("=============================================================================\n");
	Printf("\n");

	const char *strBaudrate[8] = {"Unknown", "2400bps", "4800bps", "9600bps", "19200bps", "38400bps", "57600bps", "115200bps"};
	const char *strDataBits[5] = {"Unknown", "5bits", "6bits", "7bits", "8bits"};
	const char *strStopBits[3] = {"Unknown", "1bit", "2bits"};
	const char *strParity[4] = {"Unknown", "No Parity", "Odd Parity", "Even Parity"};
	const char *strPortName[MAX_SCOMM_CNT] = {"Wi-SUN", "GPS", "RingMAC", "RS-232", "RS-422"};
	int nBaudrate, nDataBits, nStopBits, nParity;
	for(i = 0 ; i < MAX_SCOMM_CNT ; i++){
		if(gpDB->SComm[i].ucBaudrate > 8) nBaudrate = 0;
		else nBaudrate = gpDB->SComm[i].ucBaudrate;
		if(gpDB->SComm[i].ucDataBits > 4) nDataBits = 0;
		else nDataBits = gpDB->SComm[i].ucDataBits;
		if(gpDB->SComm[i].ucStopBits > 2) nStopBits = 0;
		else nStopBits = gpDB->SComm[i].ucStopBits;
		if(gpDB->SComm[i].ucParity > 3) nParity = 0;
		else nParity = gpDB->SComm[i].ucParity;
		Printf("Port-%d(COM%d) : %s ***********************************\n", i+1, gpDB->SComm[i].ucPortID, strPortName[i]);
		Printf("Baudrate/DataBits  : %s / %s\n", strBaudrate[nBaudrate], strDataBits[nDataBits]);
		Printf("Parity / StopBits  : %s / %s\n", strParity[nParity], strStopBits[nStopBits]);
		Printf("Delay / MaxPktSize : %dms / %d bytes\n", gpDB->SComm[i].wDelayTime, gpDB->SComm[i].wMaxPktSize);
		Printf("Comm. Status       : %s\n", gpDB->SComm[i].szCommStatus);
	}
	Printf("\n");
}

void PrintModuleInformation(int nFlag)
{	
	Printf("=============================================================================\r\n");
	Printf("  Module Information                                           \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	
	const char *strPortName = "LoRa";
	
	//DDWORD dwNow = GetTimeTickCount();
	DDWORD dwNow = GetCurrentMilliSecond(); // shadd
	// for(i = 0 ; i < MAX_MODULE_CNT-1; i++)
	{
		Printf("Module-%d Status(%s) ***********************************\r\n", 1, strPortName);
		Printf("Alive / RcvTime    : %d / %s\r\n", gpDB->Module.bModuleOK, GetMilliSecTimeStr((long)(dwNow - gpDB->Module.dwLastRcvTime)));
		Printf("Comm. Status : %d\r\n", gpDB->Module.ucPStep);
	}
	Printf("\r\n");
}

void PrintLoraInformation(int nFlag)
{
	Printf("=============================================================================\r\n");
	Printf("  LoRa Information                                                    \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	
	Printf("Module Valid         : %d\r\n", gpDB->Module.bModuleOK);
	Printf("EUI                  : %s\r\n", MakeEUIAddrString(gpDB->Lora.mEUI));
  Printf("RF Frequency(chfreq) : %3.1fMHz(%d)\r\n", 920.3+(gpDB->Lora.ucRFFreq*0.2), gpDB->Lora.ucRFFreq);
	Printf("Channel B/W          : 125kHz\r\n");
	Printf("Spreading Factor(sf) : %d\r\n", gpDB->Lora.ucSPFactor);
	Printf("Code Rate            : 4/5\r\n");
	Printf("Use Key(usekey)      : %d\r\n", gpDB->Lora.bUseEnDec);
	Printf("Lora Key(lorakey)    : %s\r\n", gpDB->Lora.szLoraKey);
	if(nFlag) {
		Printf("Tx Power(loratxp)    : %d\r\n", gpDB->Lora.ucTxp);
		Printf("Comm. Step           : %u\r\n", gpDB->Module.ucPStep);
	}
	Printf("\r\n");
}

void PrintSNodeInformationAll(int nFlag)
{
	int i;
	DDWORD dwNow;
	
	Printf("=============================================================================\n");
	Printf("  Sensor Node Information                                             \n");
	Printf("=============================================================================\n");
	Printf("\n");
	
	dwNow = GetTimeTickCount();
	const char *szRFMName[6] = {"UKN", "Zigbee", "LoRA", "Slv-Zigbee", "Slv-LoRA", "UDP"};
	Printf("Idx %10sOID%10s  EUI   C RFModule   Time\n", "", "");
	for(i = 0 ; i < MAX_SNODE_CNT ; i++){
		if(!gpDB->SNode[i].szOID[0]) break;
		int nRFModule = gpDB->SNode[i].ucPIdx;
		if(nRFModule == 3) nRFModule += gpDB->SNode[i].ucRFModule;
		if(nRFModule == UDP_PORT_IDX+1) nRFModule = 5;
		if(nRFModule >= 6) nRFModule = 0;
		Printf("%03d %-23s %6s %c %8s %s(%s)\n", i+1, gpDB->SNode[i].szOID, MakeEUIAddrStr3(gpDB->SNode[i].mEUI), (gpDB->SNode[i].bCommFail)?'F':'T', szRFMName[nRFModule], GetDateTimeStr(gpDB->SNode[i].dwLastRcvTime, TIME_FORMAT_ONLY_TIME), GetMilliSecTimeStr((long)(dwNow - gpDB->SNode[i].dwLastRcvTime)));

	}
	if(i == 0) Printf("No Registered Sensor Node\n");
	Printf("\n");
}

void PrintSNodeInformation(int nIdx, int nFlag)
{
	DDWORD dwNow;
	
	Printf("=============================================================================\r\n");
	Printf("  Sensor Node-%d Information                                             \r\n", nIdx+1);
	Printf("=============================================================================\r\n");
	Printf("\r\n");
	
	if(nIdx >= MAX_SNODE_CNT) {
		Printf("Index is invalid\r\n");
		return;
	}
	if(!gpDB->SNode[nIdx].szOID[0]) {
		Printf("No Sensor Node Information\r\n");
		return;
	}

	dwNow = GetCurrentMilliSecond();
	const char *szRFMName[8] = {"UKN", "Zigbee", "LoRa", "UDP", "WiSUN", "GPS", "SPORT", "TCPX"};
	int nRFModule = gpDB->SNode[nIdx].ucRFModule+1;
	if(nRFModule >= 8) nRFModule = 0;
	Printf("RF EUI / OID : %s / %s\r\n", MakeEUIAddrString(gpDB->SNode[nIdx].mEUI), gpDB->SNode[nIdx].szOID);
	Printf("First Reg. Time : %s\r\n", GetDateTimeStr(gpDB->SNode[nIdx].dwFRegTime, TIME_FORMAT_NORMAL));
	Printf("Name(snodename) : %s\r\n", gpDB->SNode[nIdx].szSNodeName);
	Printf("RptTime(snperiod)/Comm : %d / %s\r\n", gpDB->SNode[nIdx].wRptTime, (gpDB->SNode[nIdx].bCommFail)?"Fail":"OK");
	Printf("Using RF Module : %s\r\n", szRFMName[nRFModule]);
	Printf("Slv Idx/bRegSNode      : %d / %d\r\n", gpDB->SNode[nIdx].ucSlvIdx, gpDB->SNode[nIdx].bRegSNode);

	if(nFlag) {
		if(gpDB->Platform.ucM2MVer == 0)
		{
			Printf("bRegNode/bRegAE/bRegStatus : %d / %d / %d\r\n", gpDB->SNode[nIdx].bRegNode, gpDB->SNode[nIdx].bRegAE, gpDB->SNode[nIdx].bRegStatus);
			Printf("bRegAE/bStatus : %d / %d\r\n", gpDB->SNode[nIdx].bRegAE, gpDB->SNode[nIdx].bRegStatus);
			Printf("bRegDevInfo  : %d / %d\r\n", gpDB->SNode[nIdx].bRegDevInfo);
			Printf("bRegSctrl/bRegScnfg: %d / %d\r\n", gpDB->SNode[nIdx].bRegSctrl, gpDB->SNode[nIdx].bRegScnfg);
		}
		else
		{
			Printf("bRegAE/bRegDevInfo/bRegStatus : %d / %d / %d\r\n", gpDB->SNode[nIdx].bRegAE, gpDB->SNode[nIdx].bRegDevInfo, gpDB->SNode[nIdx].bRegStatus);
			Printf("bRegCommand/bRegSctrl/bRegScnfg : %d / %d / %d\r\n", gpDB->SNode[nIdx].bRegControl, gpDB->SNode[nIdx].bRegSctrl, gpDB->SNode[nIdx].bRegScnfg);
			Printf("bRptCommand/bRptSctrl/bRptScnfg/bRptDevInfo : %d / %d / %d / %d\r\n", gpDB->SNode[nIdx].bRptControl, gpDB->SNode[nIdx].bRptSctrl, gpDB->SNode[nIdx].bRptScnfg, gpDB->SNode[nIdx].bRptDevInfo);
		}
		Printf("bCtrlCfg/bCtrlDst/bCtrlTime: %d / %d / %d\r\n", gpDB->SNode[nIdx].bCtrlCfg, gpDB->SNode[nIdx].bCtrlDst, gpDB->SNode[nIdx].bCtrlTime);
		Printf("bRcvSFSET/bRcvSCNFG/bRcvSName : %d / %d / %d\r\n", gpDB->SNode[nIdx].bRcvSFSET, gpDB->SNode[nIdx].bRcvSCNFG, gpDB->SNode[nIdx].bRcvSName);
		Printf("bStep  : %d\r\n", gpDB->SNode[nIdx].ucPStep);
		Printf("DestGW : %s\r\n", gpDB->SNode[nIdx].szDestGW);
		
		if(gpDB->Platform.ucM2MVer == 0) Printf("Node ID  : %s\r\n", gpDB->SNode[nIdx].szNID);
	}

	Printf("Device : %s / %s / %s / %d\r\n", gpDB->SNode[nIdx].szManufacture, gpDB->SNode[nIdx].szDevName, gpDB->SNode[nIdx].szFWVer, gpDB->SNode[nIdx].ucLWMType);
	Printf("RF Rcv Pwr/Time : %ddBm / %s(%s)\r\n", gpDB->SNode[nIdx].nRSSI, GetDateTimeStr(gpDB->SNode[nIdx].dwLastRcvTime, TIME_FORMAT_NORMAL), GetMilliSecTimeStr((long)(dwNow - gpDB->SNode[nIdx].dwLastRcvTime)));
	BOOL bPrint = FALSE;
	if(gpDB->SNode[nIdx].bUseSName) Printf("Payload Name    : %s\r\n", gpDB->SNode[nIdx].szSDataName);
	for(int j = 0 ; j < MAX_PAYLD_CNT ; j++){
		if(!gpDB->SNode[nIdx].mRcvPayload[j]) continue;
		Printf("Last Payload-%d  : %s\r\n", j, gpDB->SNode[nIdx].szPayload[j]);
		bPrint = TRUE;
	}
	if(!bPrint) Printf("Last Payload    : XXXX\r\n");
	Printf("Last Ukn Payload: %s\r\n", gpDB->SNode[nIdx].szUkPayload);
	for(int j = 0 ; j < MAX_SNR_CFG_CNT ; j++){
		if(!gpDB->SNode[nIdx].szSCNFG[j][0]) break;
		Printf("Config %c%c : %s\r\n", gpDB->SNode[nIdx].szSCNFG[j][0], gpDB->SNode[nIdx].szSCNFG[j][1], gpDB->SNode[nIdx].szSCNFG[j]+2);
	}
	Printf("\r\n");
}

void PrintURSNodeInformation(int nFlag)
{
	int i;
	DDWORD dwNow;
	
	Printf("=============================================================================\r\n");
	Printf("  Unknown Sensor Node Information                                          \r\n");
	Printf("=============================================================================\r\n");
	Printf("\r\n");	
	dwNow = GetCurrentMilliSecond();
	const char *szRFMName[6] = {"UKN", "Zigbee", "LoRA", "UDP", "GPS", "SPORT"};
	for(i = 0 ; i < MAX_URSNODE_CNT ; i++){
		if(!memcmp(gpDB->URSNode[i].mEUI, "\x0\x0\x0\x0\x0\x0\x0\x0", 8)) break;
		int nRFModule = gpDB->SNode[i].ucRFModule;
		if(nRFModule >= 6) nRFModule = 0;
		Printf("Unknown Sensor-%d Status ***********************************\r\n", i+1);
		Printf("EUI : %s\r\n", MakeEUIAddrString(gpDB->URSNode[i].mEUI));
		Printf("OID : %s\r\n", gpDB->URSNode[i].szOID);
		Printf("Using RF Module : %s\r\n", szRFMName[nRFModule]);
		Printf("RF Rcv Pwr/Time : %ddBm / %s\r\n", gpDB->URSNode[i].nRSSI, GetMilliSecTimeStr((long)(dwNow - gpDB->URSNode[i].dwLastRcvTime)));
		Printf("Last Payload    : %s\r\n", gpDB->URSNode[i].szPayload);
		Printf("Err Reason      : %s\r\n", gpDB->URSNode[i].szErrReason);
	}
	if(i == 0) Printf("No Unknown Sensor Node\r\n");
	Printf("\r\n");
}

void PrintSPortInformation(int nFlag)
{
	int i=0;
	
	Printf("=============================================================================\r\n");
	Printf("  Serial Port Information                                        \r\n");
	Printf("=============================================================================\r\n");
	
	const char *szSPortName = "RS485";

		Printf("\nPort-%d, %s Information ***********************************\r\n", i+1, szSPortName);
		Printf("Device Cnt   : %d\r\n", gpDB->SPort.ucDevCnt);
		Printf("Polling Time : %dsec\r\n", gpDB->SPort.wPollInt);
		Printf("Wait Time    : %dms\r\n", gpDB->SPort.wWaitTime);
		Printf("Payload Cnt  : ");
		for(int j = 0 ; j < gpDB->SPort.ucDevCnt ; j++) {
			if(j) Printf("/%d", gpDB->SPort.ucSSSNCnt[j]);
			else Printf("%d", gpDB->SPort.ucSSSNCnt[j]);
		}
		Printf("\r\n");
}



void PrintPDInformation(int nFlag)
{
	DDWORD dwNow = GetTimeTickCount();
	char szOIDtmp1[60] = {0,};
	char szOIDtmp2[60] = {0,};
	sprintf(szOIDtmp1, "%sPD", gpDB->Platform.szGWOID);

	Printf("=============================================================================\r\n");
	Printf("  PD Information\r\n");
	Printf("=============================================================================\r\n");
	Printf("Current Time    : %s\r\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
	Printf("\r\n");
	for(int i = 0 ; i < MAX_PDS_CNT ; i++){
		sprintf(szOIDtmp2, "%s%s", gpDB->Platform.szOIDBase, gpDB->PDS[i].szOID);
		if(!memcmp(szOIDtmp1, szOIDtmp2, 40)) 
		{
			Printf("=============================================\r\n");
			Printf("Internal DAQ\r\n");
			Printf("  Monitor (pdmonport)     : %d\r\n", gpDB->PdNode.wMonPort);
			Printf("  Mode (pdmode)           : %s\r\n", \
				(gpDB->PdNode.ucMode == YST_PQN)?"PQN":(gpDB->PdNode.ucMode == YST_SCOPE)?"SCOPE":"ETC");
			Printf("  Sync (pdsync)           : %d(%s)\r\n", gpDB->PdNode.ch_set[0].sync_sel, \
				(gpDB->PdNode.ch_set[0].sync_sel==0)?"RCA":(gpDB->PdNode.ch_set[0].sync_sel==1)?"Gate":"Virtual");
			if(nFlag) {
			Printf("  SyncVal (pdsyncval)     : %d\r\n", gpDB->PdNode.ch_set[0].sync_sel_val);
			Printf("  On/Off                  : %d\r\n", gpDB->PdNode.ch_set[0].ch_on);
			Printf("  Gain (pdgain)           : %d(%ddB)\r\n", GetPdGainStep(gpDB->PdNode.ch_set[0].ch_gain), 26 - gpDB->PdNode.ch_set[0].ch_gain);
			Printf("  Trig (pdtrig) (%8s): %dmV[%d]\r\n", GetPdTrigRange(gpDB->PdNode.ch_set[0].ch_gain), GetPdTrigLevel((double)gpDB->PdNode.ch_set[0].trig), gpDB->PdNode.ch_set[0].trig);
			Printf("  FilterMin (pdfiltermin) : %d\r\n", gpDB->PdNode.ch_set[0].filter_min);
			Printf("  FilterMax (pdfiltermax) : %d\r\n", gpDB->PdNode.ch_set[0].filter_max);
			Printf("  Type (pdtype)           : %d\r\n", gpDB->PdNode.ch_set[0].SensorType);
			Printf("  Format (pdformat)       : %s\r\n", (gpDB->PdNode.ucFormat == 0)?"complement":"binary");
			Printf("  Period (pdperiod)       : %d\r\n", gpDB->PdNode.wRptTime);  // 시험을 위해 앞줄에 표시
			Printf("  EventFlag               : %d\r\n", gpDB->PdNode.phase);
			Printf("  Test Pattern (pdadctest): %s(%d)\r\n", \
				(gpDB->PdNode.bAdcTCh==0)?"AB":(gpDB->PdNode.bAdcTCh==1)?"A":"B", gpDB->PdNode.bAdcTPattern);
			Printf("  Test Virtual (pdadcvir) : %s\r\n", (gpDB->PdNode.bAdcVirtual)?"On":"Off");
			Printf("  Step / Progress         : %d / %d\r\n", gpDB->PdNode.ucCollect, gpDB->PdNode.ucProgress);
			}
		}else
		{
			Printf("=============================================\r\n");
			Printf("External DAQ\r\n");
		}

		Printf("PD-%d, Valid/SNodeIdx : %d / %d\r\n", i+1, gpDB->PDS[i].bValid, gpDB->PDS[i].ucSNodeIdx);
		if(gpDB->PDS[i].szPDFileName[0]) {
			Printf("  Last Rcvd Data : %s\r\n", gpDB->PDS[i].szPDFileName);
			Printf("  Last Rcvd Time : %s\r\n", GetDateTimeStr(gpDB->PDS[i].dwLastRcvTime, 0));
			Printf("  Last Send Time : %s\r\n", GetDateTimeStr(gpDB->PDS[i].dwLastSndTime, 0));
		}

		for(int j = 0 ; j < MAX_SNR_CFG_CNT ; j++){
			if(!gpDB->PDS[i].szCFGN[j][0]) break;
			Printf("  Cfg-%s : %s\r\n", gpDB->PDS[i].szCFGN[j], gpDB->PDS[i].szCFGD[j]);
		}
		for(int j = 0 ; j < MAX_SNR_CFG_CNT ; j++){
			if(!gpDB->PDS[i].szStsN[j][0]) break;
			Printf("  Status-%s : %s\r\n", gpDB->PDS[i].szStsN[j], gpDB->PDS[i].szStsD[j]);
		}
		if(nFlag) {
			Printf("  Last Req Time   : %s %d\r\n", GetMilliSecTimeStr((long)(dwNow - gpDB->PDS[i].dwLastReqTime)), gpDB->PDS[i].bReqSndTryCnt);
			Printf("  bRcvdPVer/bRcvdDevInfo/bRcvdCfg: %d / %d / %d\r\n", gpDB->PDS[i].bRcvdPVer, gpDB->PDS[i].bRcvdDevInfo, gpDB->PDS[i].bRcvdCfg);
			Printf("  bRcvdTime/bRcvdStatus/bRcvdData: %d / %d / %d\r\n", gpDB->PDS[i].bRcvdTime, gpDB->PDS[i].bRcvdStatus, gpDB->PDS[i].bRcvdData);
		}

		if(!gpDB->PDS[i].bValid) break;
		Printf("\n");
	}
	for(int j = 0 ; j < gpDB->System.ucPDSCfgCnt ; j++){
		Printf("  SCfg-%s, %s : %s\r\n", gpDB->System.szSCFGOID[j], gpDB->System.szSCFGN[j], gpDB->System.szSCFGD[j]);
	}
	Printf("\r\n");
}

void PrintLogMessageQueue(int nCount)
{
	int nLMQSize, i;
	YST_LOG_DATA LogTemp;
	
	nLMQSize = GetLogMessageQueueDataSize();
	Printf("=============================================================================\r\n");
	Printf("  Log Message Table(Total %d Message)\n", nLMQSize);
	Printf("=============================================================================v\n");
	Printf(" Idx | %-19s | Message \n", "Date & Time");
	Printf("=============================================================================\r\n");
	if(nCount <= 0 || nCount > nLMQSize) nCount = nLMQSize;
	for(i = nLMQSize-nCount ; i < nLMQSize ; i++){
		ReadFromLogMessageQueue(i, &LogTemp);
		Printf("%4d | %19s | %s\r\n", i+1, GetDateTimeStr(LogTemp.dwTime, TIME_FORMAT_INCLUDE_MILLI), LogTemp.strMessage);
	}
	Printf("Current Time   : %s\r\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
}

void PrintSDataMessageQueue(int nCount)
{
	int nLMQSize, i, nViewCnt=0;
	YST_SDLOG_DATA Log;
	
	nLMQSize = GetSensorDataMsgQueueDataSize();
	Printf("=============================================================================\r\n");
	Printf("  Sensor Data Table(Total %d Message) \n", nLMQSize);
	Printf("=============================================================================\r\n");
	Printf(" Idx | %-18s  | Fg | EC | %-8s | Message \n", "Date & Time", "EUI Addr");
	Printf("=============================================================================\r\n");
	if(nCount <= 0 || nCount > nLMQSize) nCount = 20;
	for(i = nLMQSize-1 ; i >= 0 ; i--){
		ReadFromSensorDataMsgQueue(i, &Log);
		DDWORD dwTime = (Log.dwTime & 0xFFFFFFFFFFFFFFULL);
		BYTE ucErrCode = (BYTE)(Log.dwTime>>56);
		Printf("%4d | %18s | %02X | %-2d : %02X:%02X:%02X | %s\r\n", i+1, GetDateTimeStr(dwTime, TIME_FORMAT_NORMAL), Log.ucFlag, ucErrCode, Log.mEUI[0], Log.mEUI[1], Log.mEUI[2], Log.szMsg);
		if(++nViewCnt >= nCount) break;
	}
	Printf("Current Time   : %s\r\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
	Printf("\r\n");
}

void PrintSDataErrMessageQueue(int nCount)
{
	int nLMQSize, i, nViewCnt=0;
	YST_SDLOG_DATA Log;
	
	nLMQSize = GetSensorDataMsgQueueDataSize();
	Printf("=============================================================================\r\n");
	Printf("  Sensor Data Table(Total %d Message) \r\n", nLMQSize);
	Printf("=============================================================================\r\n");
	Printf(" Idx | %-18s  | Fg | EC | %-8s | Message \r\n", "Date & Time", "EUI Addr");
	Printf("=============================================================================\r\n");
	if(nCount == 0) nCount = 20;
	for(i = nLMQSize-1 ; i >= 0 ; i--){
		ReadFromSensorDataMsgQueue(i, &Log);
		DDWORD dwTime = (Log.dwTime & 0xFFFFFFFFFFFFFFULL);
		BYTE ucErrCode = (BYTE)(Log.dwTime>>56);
		if(!ucErrCode) continue;
		Printf("%4d | %18s | %02X | %-2d : %02X:%02X:%02X | %s\r\n", i+1, GetDateTimeStr(dwTime, TIME_FORMAT_NORMAL), Log.ucFlag, ucErrCode, Log.mEUI[0], Log.mEUI[1], Log.mEUI[2], Log.szMsg);
		if(++nViewCnt >= nCount) break;
	}
	Printf("Current Time   : %s\r\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
	Printf("\r\n");
}

int GetArgumentFromString(char strArg[], char strString[], int nRemainAll)
{
	int nSize = 0;
	int nOffset = 0;
	
	strArg[0] = 0;
	while(strString[nOffset] == ' ' || strString[nOffset] == '\t') nOffset++;
	if(nRemainAll){
		strcpy(strArg, strString+nOffset);
		return (nOffset);
	}
	while((strString[nOffset+nSize] >= 33 && strString[nOffset+nSize] <= 126) || ((signed char)strString[nOffset+nSize] < 0)){
		strArg[nSize] = strString[nOffset+nSize];
		if((signed char)strString[nOffset+nSize] < 0){
			nSize++;
			strArg[nSize] = strString[nOffset+nSize];
		}
		nSize++;
	}
	strArg[nSize] = 0;
	return (nSize+nOffset);
}

int DecodeSingleCommand(char strCommand[], int nShowMoreFlag)
{
	SystemObjData stSystem;

	if(STR_EQUAL(strCommand, "reset") || STR_EQUAL(strCommand, "reboot")){
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_Reset, 0, 0, &stSystem, sizeof(stSystem));
		return 1;
	}
	if(STR_EQUAL(strCommand, "reset db") || STR_EQUAL(strCommand, "reset DB")){
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_ResetDB, 0, 0, &stSystem, sizeof(stSystem));
		return 1;
	}
	
	if(STR_EQUAL(strCommand, "debug")){
		if(gpLog->dwDebugFlag){
			if(gpLog->dwDebugFlag & LOG_TO_FILE) PrintDebugLogMessage();
			gpLog->dwDebugFlag = 0;
			Printf("Debug Flag : 0x%08X\n", gpLog->dwDebugFlag);
		}
		else PrintDebugHelp(nShowMoreFlag);
		return 1;
	}	
	if(STR_EQUAL(strCommand, "ver") || STR_EQUAL(strCommand, "version") || STR_EQUAL(strCommand, "sh ver") || STR_EQUAL(strCommand, "sh version")){
		PrintVersionInformation();
		return 1;
	}
	if(STR_EQUAL(strCommand, "date") || STR_EQUAL(strCommand, "time") || STR_EQUAL(strCommand, "sh date") || STR_EQUAL(strCommand, "sh time")){
		Printf(" Current : %s\n", GetCurrentDateTimeStr(TIME_FORMAT_NORMAL));
		Printf(" Usage   : set date YYYY-MM-DD HH:MM:SS\n");
		return 1;
	}
	if(STR_EQUAL(strCommand, "_sizeof") || STR_EQUAL(strCommand, "sh sizeof") || STR_EQUAL(strCommand, "sizeof")){
		PrintDebugSizeof();
		return 1;
	}

	if(STR_EQUAL(strCommand, "exit-zw")) return -1;
	if(gnTelnetFlag && (STR_EQUAL(strCommand, "exit") || STR_EQUAL(strCommand, "quit") || STR_EQUAL(strCommand, "close"))) return -1;
	
	return 0;
}

int DecodeHelpCommand(char strArg2[], int nShowMoreFlag)
{
	if(STR_EQUAL(strArg2, "show")){
		PrintShowHelp(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "set")){
		PrintSetHelp(nShowMoreFlag);
		return 1;
	}
	PrintShowHelp(nShowMoreFlag);
	return 1;
}

int DecodeShowCommand(char strArg2[], char strArg3[], int nShowMoreFlag)
{
	int nCount;
	
	if(strArg2[0] == 0 || STR_EQUAL(strArg2, "system")){
		PrintSystemInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "ver") || STR_EQUAL(strArg2, "version")){
		PrintVersionInformation();
		return 1;
	}
	if(STR_EQUAL(strArg2, "dbglog") || STR_EQUAL(strArg2, "debuglog")){
		PrintDebugLogMessage();
		ClearDebugLogMessage();
		return 1;
	}
	
	if(STR_EQUAL(strArg2, "log")){
		nTmp1 = GetIntegerFromString(strArg3);
		PrintLogMessageQueue(nTmp1);
		return 1;
	}
	if(STR_EQUAL(strArg2, "syslog") || STR_EQUAL(strArg2, "systemlog")){
		nTmp1 = GetIntegerFromString(strArg3);
		nTmp2 = GetIntegerFromString(strArg4);

		if((nTmp1 <= 0) || (nTmp2 <= 0) ||  (nTmp1 > nTmp2))
		{
			Printf("Invalid show command\r\n");
			return 1;
		}

		ShowGeneralLogMsg(nTmp1, nTmp2);
		return 1;
	}
	
	if(STR_EQUAL(strArg2, "sdata")){
		nCount = GetIntegerFromString(strArg3);
		PrintSDataMessageQueue(nCount);
		return 1;
	}
	if(STR_EQUAL(strArg2, "sderr")){
		nCount = GetIntegerFromString(strArg3);
		PrintSDataErrMessageQueue(nCount);
		return 1;
	}
	if(STR_EQUAL(strArg2, "rmac")){
		PrintRMacInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "platform") || STR_EQUAL(strArg2, "plat") || STR_EQUAL(strArg2, "coap")){
		PrintPlatformInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "comm") || STR_EQUAL(strArg2, "scomm")){
		PrintSCommInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "rfm") || STR_EQUAL(strArg2, "module")){
		PrintModuleInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "lora")){
		PrintLoraInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "snode")){
		int nMIdx = GetIntegerFromString(strArg3)-1;
		if(nMIdx == -1){
			PrintSNodeInformationAll(nShowMoreFlag);
			return 1;
		}
		if(nMIdx >= 0 && nMIdx < MAX_SNODE_CNT) PrintSNodeInformation(nMIdx, nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "ursnode") || STR_EQUAL(strArg2, "unsnode") || STR_EQUAL(strArg2, "unnode")){
		PrintURSNodeInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pd")){
		PrintPDInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "sport")){
		PrintSPortInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pf") || STR_EQUAL(strArg2, "port") || STR_EQUAL(strArg2, "forward") || STR_EQUAL(strArg2, "portforward")){
		PrintPortForwardInformation(nShowMoreFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "route") || STR_EQUAL(strArg2, "rtable") || STR_EQUAL(strArg2, "rttable") || STR_EQUAL(strArg2, "routetable")){
		PrintRouteTableInformation(nShowMoreFlag);
		return 1;
	}
		
	Printf("Invalid show command\n");
	return 0;
}

int DecodeClearCommand(char strArg2[])
{
	SystemObjData stSystem;

	if(STR_EQUAL(strArg2, "dbglog") || STR_EQUAL(strArg2, "debuglog")){
		ClearDebugLogMessage();
		Printf("Debug Log File is initialized\r\n");
		return 1;
	}

	if(STR_EQUAL(strArg2, "log")){
		// ClearLogMessageQueue();
		ClearLogDBFromDisk();
		Printf("Event Log is initialized\r\n");
		return 1;
	}
	if(STR_EQUAL(strArg2, "syslog")){
		ClearGeneralLogMsg();
		Printf("General Log File is initialized\r\n");
		return 1;
	}
	if(STR_EQUAL(strArg2, "sdata")){
		ClearSDataFromDisk();
		Printf("Sdata Log is initialized\r\n");
		return 1;
	}

	if(STR_EQUAL(strArg2, "db") || STR_EQUAL(strArg2, "DB")){
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_ResetDB, 0, 0, &stSystem, sizeof(stSystem));
		return 1;
	}
	if(STR_EQUAL(strArg2, "env")){
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_ResetEnv, 0, 0, &stSystem, sizeof(stSystem));
		return 1;
	}
	
	Printf("Invalid clear command\n");
	return 0;
}

int DecodeDebugCommand(char strArg2[], char strArg3[], char strArg3E[], char strArg4[], char strArg4E[], char strArg5[], int nDebugToFileFlag)
{
	SystemObjData stSystem;//DeviceInfoDB.h
	int nMIdx;

	if(nDebugToFileFlag){
		gpLog->dwDebugFlag |= LOG_TO_FILE;
		gpLog->dwDebugFlag &= ~LOG_TO_SCREEN;
		Command.m_bCheckFile = 1;
	}
	else gpLog->dwDebugFlag |= LOG_TO_SCREEN;


	if(STR_EQUAL(strArg2, "ipc")){
		gpLog->dwDebugFlag |= LOG_DEBUG_IPC;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "network") || STR_EQUAL(strArg2, "tcp") || STR_EQUAL(strArg2, "ethernet")){
		gpLog->dwDebugFlag |= LOG_DEBUG_TCP;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "upgrade") || STR_EQUAL(strArg2, "up")){
		gpLog->dwDebugFlag |= LOG_DEBUG_UPGRADE;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "updata") || STR_EQUAL(strArg2, "up-data")){
		gpLog->dwDebugFlag |= LOG_DEBUG_UPDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "comdata") || STR_EQUAL(strArg2, "com-data")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SERDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "compacket") || STR_EQUAL(strArg2, "com-packet") || STR_EQUAL(strArg2, "serial")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SERIAL;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "socket") || STR_EQUAL(strArg2, "soc")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SOCKET;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "socdata") || STR_EQUAL(strArg2, "sdata")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SOCDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "udp") || STR_EQUAL(strArg2, "udppacket")){
		gpLog->dwDebugFlag |= LOG_DEBUG_UDP;
		Printf("Debug Flag : 0x%08X\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "udpdata")){
		gpLog->dwDebugFlag |= LOG_DEBUG_UDPDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}

	if(STR_EQUAL(strArg2, "lora") || STR_EQUAL(strArg2, "LORA")){
		gpLog->dwDebugFlag |= LOG_DEBUG_LORA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "loradata")){
		gpLog->dwDebugFlag |= LOG_DEBUG_LORADATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}

	if(STR_EQUAL(strArg2, "coap") || STR_EQUAL(strArg2, "COAP")){
		gpLog->dwDebugFlag |= LOG_DEBUG_COAP;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "coappacket") || STR_EQUAL(strArg2, "coapdata")){
		gpLog->dwDebugFlag |= LOG_DEBUG_COAPDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "coappayload") || STR_EQUAL(strArg2, "coapdetail")){
		gpLog->dwDebugFlag |= LOG_DEBUG_COAPDETAIL;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}	

	if(STR_EQUAL(strArg2, "m2mxml")){
		gpLog->dwDebugFlag |= LOG_DEBUG_M2MXML;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "platsec")){
		gpLog->dwDebugFlag |= LOG_DEBUG_PLATSEC;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "rcomm") || STR_EQUAL(strArg2, "rcom")){
		gpLog->dwDebugFlag |= LOG_DEBUG_RCOMM;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pdnode")){
		gpLog->dwDebugFlag |= LOG_DEBUG_PDNODE;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pdtcp")){
		gpLog->dwDebugFlag |= LOG_DEBUG_TCPPD;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "scomm") || STR_EQUAL(strArg2, "comm")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SCOMM;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "scommdata") || STR_EQUAL(strArg2, "commdata")){
		gpLog->dwDebugFlag |= LOG_DEBUG_SCOMMDATA;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hw1")){
		gpLog->dwDebugFlag |= LOG_DEBUG_HW1;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hw2")){
		gpLog->dwDebugFlag |= LOG_DEBUG_HW2;
		Printf("Debug Flag : 0x%08X\r\n", gpLog->dwDebugFlag);
		return 1;
	}

	if(STR_EQUAL(strArg2, "flagsave") || STR_EQUAL(strArg2, "saveflag") || STR_EQUAL(strArg2, "save")){
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_DebugFlag, 0, 0, (char *)&stSystem, sizeof(stSystem));
		return 1;
	}
	if(STR_EQUAL(strArg2, "scommsend") || STR_EQUAL(strArg2, "scommsnd") || STR_EQUAL(strArg2, "scomsnd")){
		nPIdx = GetIntegerFromString(strArg3)-1;
		if(nPIdx < 0 || (nPIdx >= MAX_SCOMM_CNT && nPIdx != (UDP_PORT_IDX-1) && nPIdx != (LORA_PORT_IDX-1))){
			Printf("SComm Port-%d is not valid.\n", nPIdx+1);
			return 1;
		}
		Printf("Snd [%s\\r] to SComm-%d\n", strArg4E, nPIdx+1);
		strcat(strArg4E, "\r");
		strArg4E[MAX_DESC_STR] = 0;
		strcpy((char *)stSystem.mPacket, strArg4E);
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_SComSend, nPIdx, 0, (char *)&stSystem, sizeof(stSystem));
		gpLog->dwDebugFlag |= LOG_DEBUG_SCOMM;
		return 1;
	}
	if(STR_EQUAL(strArg2, "packetsend")){// CLI->HWManager에게 요청할 송신패킷(디버그용)
		nPIdx = GetIntegerFromString(strArg3)-1;
		if(nPIdx < 0 || (nPIdx >= MAX_SCOMM_CNT && nPIdx != (UDP_PORT_IDX-1))){
			Printf("SComm Port-%d is not valid.\n", nPIdx+1);
			return 1;
		}
		GetPacketFromString(stSystem.mPacket, strArg4E, MAX_DESC_STR+1);
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_ComPacket, nPIdx, 0, (char *)&stSystem, sizeof(stSystem));
		Printf("Send %dbyte To COM : ", stSystem.mPacket[0]);
		for(int i = 0 ; i < stSystem.mPacket[0] ; i++){
			Printf("%02X ", stSystem.mPacket[i+1]);
		}
		Printf("\n");
		return 1;
	}
	
	Printf("Invalid debug name\n");
	return 0;
}

int DecodeDefineCommand(char strArg2[], char strArg3[], char strArg3E[], char strArg4[], char strArg4E[])
{
	GeneralObjData stGeneral;
	ChassisObjData stChassis;
	RMacObjData stRMac;

	if(STR_EQUAL(strArg2, "mac")){
		GetMACArrayFromString(stGeneral.mMAC, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_MAC, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("MAC : %s\n", MakeMACAddrString(gpDB->General.mMAC));
		Printf("You should reset !\n");
		return 1;
	}
	if(STR_EQUAL(strArg2, "rmac") || STR_EQUAL(strArg2, "ringmac")){
		GetMACArrayFromString(stRMac.mRMAC, strArg3);
		MakeIPCSetMessage(YET_GID_RMAC, YET_OID_RMacRMAC, 0, 0, &stRMac, sizeof(stRMac));
		Printf("Ring MAC : %s\n", MakeMACAddrString(gpDB->RMac.mRMAC));
		return 1;
	}
	if(STR_EQUAL(strArg2, "sn") || STR_EQUAL(strArg2, "sno")){
		strcpy(stChassis.szSerialNo, strArg3E);
		MakeIPCSetMessage(YET_GID_CHASSIS, YET_OID_ChSerialNo, 0, 0, &stChassis, sizeof(stChassis));
		Printf("Serial Number : %s\n", strArg3E);
		return 1;
	}

	Printf("Invalid define name\n");
	return 0;
}

int DecodeSetSystemCommand(char strArg2[], char strArg3[], char strArg3E[], char strArg4[], char strArg4E[])
{
	GeneralObjData stGeneral;
	ChassisObjData stChassis;
	SystemObjData stSystem;
	RMacObjData stRMac;

	if(STR_EQUAL(strArg2, "ip") || STR_EQUAL(strArg2, "ipaddr")){
		GetIPArrayFromString(stGeneral.mIP, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_IP, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("ip : %s\n", MakeIPAddrString(stGeneral.mIP));
		return 1;
	}
	if(STR_EQUAL(strArg2, "ip2") || STR_EQUAL(strArg2, "ipaddr2")){
		GetIPArrayFromString(stGeneral.mIP, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_IP2, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("ip2 : %s\n", MakeIPAddrString(stGeneral.mIP));
		return 1;
	}
	if(STR_EQUAL(strArg2, "subnet") || STR_EQUAL(strArg2, "netmask") || STR_EQUAL(strArg2, "mask")){
		GetIPArrayFromString(stGeneral.mSM, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_SM, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("subnet : %s\n", MakeIPAddrString(stGeneral.mSM));
		return 1;
	}
	if(STR_EQUAL(strArg2, "gateway") || STR_EQUAL(strArg2, "gw")){
		GetIPArrayFromString(stGeneral.mGW, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_GW, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("gateway : %s\n", MakeIPAddrString(stGeneral.mGW));
		return 1;
	}
	if(STR_EQUAL(strArg2, "pw") || STR_EQUAL(strArg2, "password") || STR_EQUAL(strArg2, "passwd")){
		strcpy(stGeneral.szPassword, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_Password, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("password : %s\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "key") || STR_EQUAL(strArg2, "lorakey")){
		strcpy(stGeneral.szLoraKey, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_LoraKey, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Lora Key : %s\n", strArg3);
		return 1;
	}	
	if(STR_EQUAL(strArg2, "mmac1")){
		GetMACArrayFromString(stGeneral.mManMAC1, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_ManMAC1, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Manager MAC-1 : %s\n", MakeMACAddrString(gpDB->General.mManMAC1));
		return 1;
	}
	if(STR_EQUAL(strArg2, "poleno")){
		strcpy(stChassis.szPoleNo, strArg3E);
		MakeIPCSetMessage(YET_GID_CHASSIS, YET_OID_PoleNo, 0, 0, &stChassis, sizeof(stChassis));
		Printf("Poll number of Chassis : %s\n", strArg3E);
		return 1;
	}
	if(STR_EQUAL(strArg2, "manno")){
		strcpy(stChassis.szMangeNo, strArg3E);
		MakeIPCSetMessage(YET_GID_CHASSIS, YET_OID_MangeNo, 0, 0, &stChassis, sizeof(stChassis));
		Printf("Manage number of Chassis : %s\n", strArg3E);
		return 1;
	}
	if(STR_EQUAL(strArg2, "insdate") || STR_EQUAL(strArg2, "indate")){
		strcpy(stChassis.szInstallDate, strArg3E);
		MakeIPCSetMessage(YET_GID_CHASSIS, YET_OID_InstallDate, 0, 0, &stChassis, sizeof(stChassis));
		Printf("Install date of Chassis : %s\n", strArg3E);
		return 1;
	}
	if(STR_EQUAL(strArg2, "date") || STR_EQUAL(strArg2, "time")){
		sprintf(stSystem.strDateTime, "%s %s", strArg3, strArg4);
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_DateTime, 0, 0, &stSystem, sizeof(stSystem));
		Printf("Set time to %s\n", stSystem.strDateTime);
		return 1;
	}

	if(STR_EQUAL(strArg2, "hmode") || STR_EQUAL(strArg2, "halowmode")){
		if(STR_EQUAL(strArg3, "sta") || STR_EQUAL(strArg3, "station") || STR_EQUAL(strArg3, "Station")) stGeneral.ucHMode = 1;
		else if(STR_EQUAL(strArg3, "ap") || STR_EQUAL(strArg3, "AP")) stGeneral.ucHMode = 2;
		else if(STR_EQUAL(strArg3, "mpp") || STR_EQUAL(strArg3, "MPP")) stGeneral.ucHMode = 3;
		else if(STR_EQUAL(strArg3, "mp") || STR_EQUAL(strArg3, "MP")) stGeneral.ucHMode = 4;
		else if(STR_EQUAL(strArg3, "none") || STR_EQUAL(strArg3, "no") || STR_EQUAL(strArg3, "off")) stGeneral.ucHMode = 0;
		else{
			Printf("Invalid Halow Mode !\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMode, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Mode : %s\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hch")){
		nIdx = GetIntegerFromString(strArg3);
		if(nIdx < 1 || nIdx > 6){
			Printf("Invalid channel : %d\r\n", nIdx);
			return 1;
		}
		stGeneral.ucHChannel = nIdx;
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HCH, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Channel : %d\r\n", stGeneral.ucHChannel);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hapip")){
		GetIPArrayFromString(stGeneral.mHMIP1, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMIP1, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow AP IP : %s\r\n", MakeIPAddrString(stGeneral.mHMIP1));
		return 1;
	}
	if(STR_EQUAL(strArg2, "hapsec")){
		stGeneral.ucHMSEC1 = GetIntegerFromString(strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSEC1, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow AP Security : %d\r\n", stGeneral.ucHMSEC1);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hapid")){
		StringCopy(stGeneral.szHMSSID1, strArg3E, 20);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSSID1, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow AP SSID : %s\r\n", stGeneral.szHMSSID1);
		return 1;
	}

	if(STR_EQUAL(strArg2, "hstdhcp")){
		if(STR_EQUAL(strArg3, "dhcp") || STR_EQUAL(strArg3, "on")) stGeneral.ucHMDHCP2 = 1;
		else if(STR_EQUAL(strArg3, "none") || STR_EQUAL(strArg3, "no") || STR_EQUAL(strArg3, "off")) stGeneral.ucHMDHCP2 = 0;
		else{
			Printf("Invalid Halow Station IP Addr DHCP Mode !\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMDHCP2, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Station IP DHCP : %s\r\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hstip")){
		GetIPArrayFromString(stGeneral.mHMIP2, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMIP2, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Station IP : %s\r\n", MakeIPAddrString(stGeneral.mHMIP2));
		return 1;
	}
	if(STR_EQUAL(strArg2, "hstsec")){
		stGeneral.ucHMSEC2 = GetIntegerFromString(strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSEC2, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Station Security : %d\r\n", stGeneral.ucHMSEC2);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hstid")){
		StringCopy(stGeneral.szHMSSID2, strArg3E, 20);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSSID2, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Station SSID : %s\r\n", stGeneral.szHMSSID2);
		return 1;
	}

	if(STR_EQUAL(strArg2, "hmedhcp")){
		if(STR_EQUAL(strArg3, "dhcp") || STR_EQUAL(strArg3, "on")) stGeneral.ucHMDHCP3 = 1;
		else if(STR_EQUAL(strArg3, "none") || STR_EQUAL(strArg3, "no") || STR_EQUAL(strArg3, "off")) stGeneral.ucHMDHCP3 = 0;
		else{
			Printf("Invalid Halow Mesh IP Addr DHCP Mode !\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMDHCP3, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Mesh IP DHCP : %s\r\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hmeip")){
		GetIPArrayFromString(stGeneral.mHMIP3, strArg3);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMIP3, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Mesh IP : %s\r\n", MakeIPAddrString(stGeneral.mHMIP3));
		return 1;
	}
	if(STR_EQUAL(strArg2, "hmesec")){
		stGeneral.ucHMSEC3 = GetIntegerFromString(strArg3);
		if( (stGeneral.ucHMSEC3 == 1) || (stGeneral.ucHMSEC3 == 2) ) {
			Printf("Invalid Halow Mesh Security !\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSEC3, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Mesh Security : %d\r\n", stGeneral.ucHMSEC3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "hmeid")){
		StringCopy(stGeneral.szHMSSID3, strArg3E, 20);
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_HMSSID3, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Halow Mesh SSID : %s\r\n", stGeneral.szHMSSID3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "gwif") || STR_EQUAL(strArg2, "gwinterface")){
		if(STR_EQUAL(strArg3, "halow")) stGeneral.ucGWIf = 1;
		else if(STR_EQUAL(strArg3, "wan") || STR_EQUAL(strArg3, "no") || STR_EQUAL(strArg3, "off")) stGeneral.ucGWIf = 0;
		else{
			Printf("Invalid Default G/W Interface !\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_GWIf, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("Default G/W Interface : %s\r\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "autoreboot") || STR_EQUAL(strArg2, "rebootint")){
		stSystem.wAutoRebootMin = GetIntegerFromString(strArg3);
		MakeIPCSetMessage(YET_GID_SYSTEM, YET_OID_AutoRebootMin, 0, 0, &stSystem, sizeof(stSystem));
		Printf("Set Auto Reboot time to %dmin\r\n", stSystem.wAutoRebootMin);
		return 1;
	}

	if(STR_EQUAL(strArg2, "eth")){
		nIdx = GetIntegerFromString(strArg3);
		if(nIdx < 0 || nIdx > 1){
			Printf("Invalid Port Index : %d\r\n", nIdx);
			return 1;
		}

		if(nIdx == 0){
			if(STR_EQUAL(strArg4, "auto")) stGeneral.ucOperEth0 = YET_ETH_AUTONEG;
			else if(STR_EQUAL(strArg4, "10h")) stGeneral.ucOperEth0 = YET_ETH_10_HALF;
			else if(STR_EQUAL(strArg4, "10f")) stGeneral.ucOperEth0 = YET_ETH_10_FULL;
			else if(STR_EQUAL(strArg4, "100h")) stGeneral.ucOperEth0 = YET_ETH_100_HALF;
			else if(STR_EQUAL(strArg4, "100f")) stGeneral.ucOperEth0 = YET_ETH_100_FULL;
			else if(STR_EQUAL(strArg4, "1000h")) stGeneral.ucOperEth0 = YET_ETH_1000_HALF;
			else if(STR_EQUAL(strArg4, "1000f")) stGeneral.ucOperEth0 = YET_ETH_1000_FULL;
			else{
				Printf("Invalid Operation Mode !\r\n");
				return 1;
			}
			MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_OperEth, nIdx, 0, &stGeneral, sizeof(stGeneral));
			Printf("Operation Mode : %d\r\n", stGeneral.ucOperEth0);
		}else if(nIdx == 1) {
			if(STR_EQUAL(strArg4, "auto")) stGeneral.ucOperEth1 = YET_ETH_AUTONEG;
			else if(STR_EQUAL(strArg4, "10h")) stGeneral.ucOperEth1 = YET_ETH_10_HALF;
			else if(STR_EQUAL(strArg4, "10f")) stGeneral.ucOperEth1 = YET_ETH_10_FULL;
			else if(STR_EQUAL(strArg4, "100h")) stGeneral.ucOperEth1 = YET_ETH_100_HALF;
			else if(STR_EQUAL(strArg4, "100f")) stGeneral.ucOperEth1 = YET_ETH_100_FULL;
			else if(STR_EQUAL(strArg4, "1000h")) stGeneral.ucOperEth1 = YET_ETH_1000_HALF;
			else if(STR_EQUAL(strArg4, "1000f")) stGeneral.ucOperEth1 = YET_ETH_1000_FULL;
			else if(STR_EQUAL(strArg4, "off")) stGeneral.ucOperEth1 = YET_ETH_OFF;
			else{
				Printf("Invalid Operation Mode !\r\n");
				return 1;
			}
			MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_OperEth, nIdx, 0, &stGeneral, sizeof(stGeneral));
			Printf("Operation Mode : %d\r\n", stGeneral.ucOperEth1);
		}

		return 1;
	}

	if(STR_EQUAL(strArg2, "telnet")){
		if(STR_EQUAL(strArg3, "on") || STR_EQUAL(strArg3, "On")) stGeneral.bTelnet = 1;
		else if(STR_EQUAL(strArg3, "off") || STR_EQUAL(strArg3, "Off")) stGeneral.bTelnet = 0;
		else {
			Printf("Invalid TELNET Setting\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_TELNET, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("TELNET Setting : %s\r\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "ftp")){
		if(STR_EQUAL(strArg3, "on") || STR_EQUAL(strArg3, "On")) stGeneral.bFtp = 1;
		else if(STR_EQUAL(strArg3, "off") || STR_EQUAL(strArg3, "Off")) stGeneral.bFtp = 0;
		else {
			Printf("Invalid FTP Setting\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_GENERAL, YET_OID_FTP, 0, 0, &stGeneral, sizeof(stGeneral));
		Printf("FTP Setting : %s\r\n", strArg3);
		return 1;
	}

	return 0;
}

int DecodeSetSCommCommand(char strArg2[], char strArg3[], char strArg4[])
{
	SCommObjData stData;
	int nIdx=0;

	if(STR_EQUAL(strArg2, "delay") || STR_EQUAL(strArg2, "sdelay") ||
		STR_EQUAL(strArg2, "maxsize") || STR_EQUAL(strArg2, "pktsize") ||
		STR_EQUAL(strArg2, "baudrate") || STR_EQUAL(strArg2, "brate") ||
		STR_EQUAL(strArg2, "databits") || STR_EQUAL(strArg2, "databit") || STR_EQUAL(strArg2, "reinit") ||
		STR_EQUAL(strArg2, "stopbits") || STR_EQUAL(strArg2, "stopbit") || STR_EQUAL(strArg2, "parity") ||
		STR_EQUAL(strArg2, "sreset") || STR_EQUAL(strArg2, "sreboot")){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx < 0 || nIdx >= MAX_SCOMM_CNT){
			Printf("Invalid Port Index : %d, Max = %d\n", nIdx+1, MAX_SCOMM_CNT);
			return 1;
		}
	}
	else return 0;

	if(STR_EQUAL(strArg2, "delay") || STR_EQUAL(strArg2, "sdelay")){
		stData.wDelayTime = GetIntegerFromString(strArg4);
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SDelayTime, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Delay Time : %d\n", nIdx+1, stData.wDelayTime);
		return 1;
	}
	if(STR_EQUAL(strArg2, "maxsize") || STR_EQUAL(strArg2, "pktsize")){
		stData.wMaxPktSize = GetIntegerFromString(strArg4);
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SMaxPktSize, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Max Pkt Size : %d\n", nIdx+1, stData.wMaxPktSize);
		return 1;
	}
	if(STR_EQUAL(strArg2, "baudrate") || STR_EQUAL(strArg2, "brate")){
		if(STR_EQUAL(strArg4, "2400") || STR_EQUAL(strArg4, "2400bps")) stData.ucBaudrate = 1;
		else if(STR_EQUAL(strArg4, "4800") || STR_EQUAL(strArg4, "4800bps")) stData.ucBaudrate = 2;
		else if(STR_EQUAL(strArg4, "9600") || STR_EQUAL(strArg4, "9600bps")) stData.ucBaudrate = 3;
		else if(STR_EQUAL(strArg4, "19200") || STR_EQUAL(strArg4, "19200bps")) stData.ucBaudrate = 4;
		else if(STR_EQUAL(strArg4, "38400") || STR_EQUAL(strArg4, "38400bps")) stData.ucBaudrate = 5;
		else if(STR_EQUAL(strArg4, "57600") || STR_EQUAL(strArg4, "57600bps")) stData.ucBaudrate = 6;
		else if(STR_EQUAL(strArg4, "115200") || STR_EQUAL(strArg4, "115200bps")) stData.ucBaudrate = 7;
		else{
			Printf("Invalid Serial Port Baudrate !\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SBaudrate, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Baudrate : %s\n", nIdx+1, strArg4);
		return 1;
	}
	if(STR_EQUAL(strArg2, "databits") || STR_EQUAL(strArg2, "databit")){
		if(STR_EQUAL(strArg4, "5") || STR_EQUAL(strArg4, "5bit") || STR_EQUAL(strArg4, "5bits")) stData.ucDataBits = 1;
		else if(STR_EQUAL(strArg4, "6") || STR_EQUAL(strArg4, "6bit") || STR_EQUAL(strArg4, "6bits")) stData.ucDataBits = 2;
		else if(STR_EQUAL(strArg4, "7") || STR_EQUAL(strArg4, "7bit") || STR_EQUAL(strArg4, "7bits")) stData.ucDataBits = 3;
		else if(STR_EQUAL(strArg4, "8") || STR_EQUAL(strArg4, "8bit") || STR_EQUAL(strArg4, "8bits")) stData.ucDataBits = 4;
		else{
			Printf("Invalid Serial Port Databits !\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SDataBits, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Databits : %s\n", nIdx+1, strArg4);
		return 1;
	}
	if(STR_EQUAL(strArg2, "stopbits") || STR_EQUAL(strArg2, "stopbit")){
		if(STR_EQUAL(strArg4, "1") || STR_EQUAL(strArg4, "1bit")) stData.ucStopBits = 1;
		else if(STR_EQUAL(strArg4, "2") || STR_EQUAL(strArg4, "2bit") || STR_EQUAL(strArg4, "2bits")) stData.ucStopBits = 2;
		else{
			Printf("Invalid Serial Port Stopbits !\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SStopBits, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Stopbits : %s\n", nIdx+1, strArg4);
		return 1;
	}
	if(STR_EQUAL(strArg2, "parity")){
		if(STR_EQUAL(strArg4, "no") || STR_EQUAL(strArg4, "noparity")) stData.ucParity = 1;
		else if(STR_EQUAL(strArg4, "odd") || STR_EQUAL(strArg4, "oddparity")) stData.ucParity = 2;
		else if(STR_EQUAL(strArg4, "even") || STR_EQUAL(strArg4, "evenparity")) stData.ucParity = 3;
		else{
			Printf("Invalid Serial Port Parity !\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SParity, nIdx, 0, &stData, sizeof(stData));
		Printf("SComm Port-%d Parity Method : %s\n", nIdx+1, strArg4);
		return 1;
	}
	if(STR_EQUAL(strArg2, "reinit")){
		Printf("SComm Port-%d ReIntialize !\n", nIdx+1);
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SCheckAlive, nIdx, 0, &stData, sizeof(stData));
		return 1;
	}
	if(STR_EQUAL(strArg2, "sreset") || STR_EQUAL(strArg2, "sreboot")){
		if(nIdx != LORA_PORT_IDX-1){
			Printf("Invalid Module!\r\n");
			return 1;
		}
		Printf("SComm Port-%d Module Reset !\r\n", nIdx+1);
		MakeIPCSetMessage(YET_GID_SCOMM, YET_OID_SReset, nIdx, 0, &stData, sizeof(stData));
		return 1;
	}
	return 0;
}

int DecodeSetPlatformCommand(char strArg2[], char strArg3[])
{
	PFormObjData stData;
	int nTemp;

	if(STR_EQUAL(strArg2, "myip") || STR_EQUAL(strArg2, "myipaddr")){
		GetIPArrayFromString(stData.mMyIP, strArg3);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_MyIP, 0, 0, &stData, sizeof(stData));
		Printf("platform my-ip : %s\n", MakeIPAddrString(stData.mMyIP));
		return 1;
	}
	if(STR_EQUAL(strArg2, "platip") || STR_EQUAL(strArg2, "platipaddr")){
		GetIPArrayFromString(stData.mPeerIP, strArg3);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_PeerIP, 0, 0, &stData, sizeof(stData));
		Printf("platform server-ip : %s\n", MakeIPAddrString(stData.mPeerIP));
		return 1;
	}
	if(STR_EQUAL(strArg2, "myport")){
		nTemp = GetIntegerFromString(strArg3);
		if(nTemp < 0 && nTemp >= 65535){
			Printf("Invalid Port Num : %d\n", nTemp);
			return 1;
		}
		stData.wMyPort = nTemp;
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_PFMyPort, 0, 0, &stData, sizeof(stData));
		Printf("My CoAP Port Num : %d\n", nTemp);
		return 1;
	}
	if(STR_EQUAL(strArg2, "platport")){
		nTemp = GetIntegerFromString(strArg3);
		if(nTemp < 0 && nTemp >= 65535){
			Printf("Invalid Port Num : %d\n", nTemp);
			return 1;
		}
		stData.wPeerPort = nTemp;
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_PFPeerPort, 0, 0, &stData, sizeof(stData));
		Printf("Platform CoAP Port Num : %d\n", nTemp);
		return 1;
	}
	if(STR_EQUAL(strArg2, "oid")){
		strcpy(stData.szGWOID, strArg3);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_GWOID, 0, 0, &stData, sizeof(stData));
		Printf("OID of G/W : %s\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "uri")){
		strcpy(stData.szURIPath, strArg3);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_URIPath, 0, 0, &stData, sizeof(stData));
		Printf("URI Path of Platform : %s\n", strArg3);
		return 1;
	}

	if(STR_EQUAL(strArg2, "cseid")){
		StringCopy(stData.szCSEID, strArg3, 20);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_CSEID, 0, 0, &stData, sizeof(stData));
		Printf("CSE-ID of Platform : %s\r\n", strArg3);
		return 1;
	}
	if(STR_EQUAL(strArg2, "period")){
		stData.wRptTime = GetIntegerFromString(strArg3);
		if(stData.wRptTime < PLAT_PERIOD_MIN) stData.wRptTime = PLAT_PERIOD_MIN;
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_RptTime, 0, 0, &stData, sizeof(stData));
		Printf("G/W Status Report Period : %dsec\r\n", stData.wRptTime);
		return 1;
	}


	if(STR_EQUAL(strArg2, "m2mver")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 2))
		{
			Printf("Invalid m2mver : %d\r\n", nTemp);
			return 1;
		}
		stData.ucM2MVer = nTemp;
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_M2MVer, 0, 0, &stData, sizeof(stData));
		Printf("OneM2M Version : %d\r\n", stData.ucM2MVer);
		return 1;
	}
	
	if(STR_EQUAL(strArg2, "bsize")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 2)) 
		{
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		if(nTemp == 0) 	stData.wBlockSize = 2048;
		else if(nTemp == 1) 	stData.wBlockSize = 1024;
		else stData.wBlockSize = 512;

		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_BlockSize, 0, 0, &stData, sizeof(stData));
		Printf("BlockSize set : %d\r\n", stData.wBlockSize);
		return 1;
	}
	if(STR_EQUAL(strArg2, "platsec")){
		if(STR_EQUAL(strArg3, "on") || STR_EQUAL(strArg3, "On")) stData.bSec = 1;
		else if(STR_EQUAL(strArg3, "off") || STR_EQUAL(strArg3, "Off")) stData.bSec = 0;
		else {
			Printf("Invalid platsec Setting\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_Sec, 0, 0, &stData, sizeof(stData));
		Printf("platsec set : %s\r\n", strArg3);
		return 1;
	}

	if(STR_EQUAL(strArg2, "secdbg")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 4)) 
		{
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}
		stData.bSecDebug = nTemp;
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_SecDebug, 0, 0, &stData, sizeof(stData));
		Printf("secdbg set : %d\r\n", stData.bSecDebug);
		return 1;
	}

	if(STR_EQUAL(strArg2, "secpw")){
		StringCopy(stData.szSecPW, strArg3, 20);
		MakeIPCSetMessage(YET_GID_PLATFORM, YET_OID_SecPW, 0, 0, &stData, sizeof(stData));
		Printf("secpw set : %s\r\n", strArg3);
		return 1;
	}
	
	return 0;
}

int DecodeSetLoraCommand(char strArg2[], char strArg3[])
{
	LoraObjData stData;
	int nTemp;
	
	if(STR_EQUAL(strArg2, "ch") || STR_EQUAL(strArg2, "freq") ||
		STR_EQUAL(strArg2, "chfreq") || STR_EQUAL(strArg2, "sf") ||
		STR_EQUAL(strArg2, "sfactor") || STR_EQUAL(strArg2, "usekey") || 
		STR_EQUAL(strArg2, "key") || STR_EQUAL(strArg2, "lorakey") ||
		STR_EQUAL(strArg2, "loratxp") ){
	}
	else return 0;

	if(STR_EQUAL(strArg2, "ch") || STR_EQUAL(strArg2, "freq") || STR_EQUAL(strArg2, "chfreq")){
		nTemp = GetIntegerFromString(strArg3);
		if(nTemp < 0 || nTemp > 15){
			Printf("Invalid Channel Number(%d)\r\n", nTemp);
			return 1;
		}
		stData.ucRFFreq = nTemp;
		MakeIPCSetMessage(YET_GID_LORA, YET_OID_LRFFreq, DEF_LORA_IDX, 0, &stData, sizeof(stData));
		Printf("LoRa, Frequency = %d(%3.1fMHz)\r\n", nTemp, 920.3+(nTemp*0.2));
		return 1;
	}
	if(STR_EQUAL(strArg2, "sf") || STR_EQUAL(strArg2, "sfactor")){
		nTemp= GetIntegerFromString(strArg3);
		if(nTemp < 7 || nTemp > 12){
			Printf("Invalid Spreading Factor(%d)\r\n", nTemp);
			return 1;
		}
		stData.ucSPFactor = nTemp;
		MakeIPCSetMessage(YET_GID_LORA, YET_OID_LSPFactor, DEF_LORA_IDX, 0, &stData, sizeof(stData));
		Printf("LoRa spreading factor : %d\r\n", stData.ucSPFactor);
		return 1;
	}
	if(STR_EQUAL(strArg2, "usekey")){
		nTemp = GetIntegerFromString(strArg3);
		if(nTemp < 0 || nTemp > 2){
			Printf("Invalid LoRa useKey(%d)\r\n", nTemp);
			return 1;
		}
		stData.bUseEnDec = nTemp;
		MakeIPCSetMessage(YET_GID_LORA, YET_OID_LUseEnDec, DEF_LORA_IDX, 0, &stData, sizeof(stData));
		Printf("LoRa Use Key : %d\r\n", stData.bUseEnDec);
		return 1;
	}
	if(STR_EQUAL(strArg2, "key") || STR_EQUAL(strArg2, "lorakey")){
		StringCopy(stData.szLoraKey, strArg3, 20);

		// check
		nTemp = 0;
		for(int i=4; i<15; )
		{
			if(stData.szLoraKey[i] != '-') nTemp = -1;
			i += 5;
		}
		if(strlen(stData.szLoraKey) < 19) nTemp = -1;
		if(nTemp  < 0){
			Printf("Invalid LoRa Key(%s)\r\n", stData.szLoraKey);
			return 1;
		}
		MakeIPCSetMessage(YET_GID_LORA, YET_OID_LoraKey, DEF_LORA_IDX, 0, &stData, sizeof(stData));
		Printf("LoRa Key : %s\r\n", stData.szLoraKey);
		return 1;
	}
	if(STR_EQUAL(strArg2, "loratxp")){
		nTemp = GetIntegerFromString(strArg3);
		if(nTemp < 0 || nTemp > 14){
			Printf("Invalid TxPower Number(%d)\r\n", nTemp);
			return 1;
		}
		stData.ucTxp = nTemp;
		MakeIPCSetMessage(YET_GID_LORA, YET_OID_LoraTxp, DEF_LORA_IDX, 0, &stData, sizeof(stData));
		Printf("LoRa, TxPower = %d\r\n", stData.ucTxp);
		return 1;
	}
	
	return 0;
}

int DecodeSetSNodeCommand(char strArg2[], char strArg3[], char strArg4E[])
{
	SNodeObjData stData;
	int nIdx;

	if(STR_EQUAL(strArg2, "snodename")
		|| STR_EQUAL(strArg2, "snperiod") || STR_EQUAL(strArg2, "snreboot") || STR_EQUAL(strArg2, "snreset")
		|| STR_EQUAL(strArg2, "sncfg") || STR_EQUAL(strArg2, "sndest") || STR_EQUAL(strArg2, "sntime")){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx < 0 || nIdx >= MAX_SNODE_CNT){
			Printf("Invalid Sensor Node Index : %d, Max = %d\n", nIdx+1, MAX_SNODE_CNT);
			return 1;
		}
	}
	if(STR_EQUAL(strArg2, "snodename")){
		strcpy(stData.szSNodeName, strArg4E);
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SNodeName, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Node Name : %s\n", nIdx+1, strArg4E);
		return 1;
	}
	if(STR_EQUAL(strArg2, "snperiod")){
		stData.wRptTime = GetIntegerFromString(strArg4E);
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SCtrlRptTime, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Period : %dsec\n", nIdx+1, stData.wRptTime);
		return 1;
	}
	if(STR_EQUAL(strArg2, "snreboot") || STR_EQUAL(strArg2, "snreset")){
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SCtrlReboot, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Reboot\n", nIdx+1);
		return 1;
	}
	if(STR_EQUAL(strArg2, "sncfg")){
		strcpy(stData.szCtrlCfg, strArg4E);
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SCtrlCfg, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Cfg-%c%c : %s\n", nIdx+1, strArg4E[0], strArg4E[1], strArg4E+2);
		return 1;
	}
	if(STR_EQUAL(strArg2, "sndest")){
		strcpy(stData.szDestGW, strArg4E);//sprintf(szMsg, "C%d,E%s", ucDstRFCh, MakeEUIAddrString(mDstEUI));
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SCtrlDestGW, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Dest G/W : %s\n", nIdx+1, strArg4E);
		return 1;
	}
	if(STR_EQUAL(strArg2, "sntime")){
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SCtrlTime, nIdx, 0, &stData, sizeof(stData));
		Printf("SNode-%d Time Sync\n", nIdx+1);
		return 1;
	}
	return 0;
}


int DecodeSetPdCommand(char strArg2[], char strArg3[], char strArg4E[])
{
	PdObjData stData;
	int nTemp = 0;
	int nTemp2 = 0;

	if(STR_EQUAL(strArg2, "pdmonport") ){
		stData.wMonPort = GetIntegerFromString(strArg3);
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_MONITOR_PORT, 0, 0, &stData, sizeof(stData));
		Printf("PD Monitor Port : %d\r\n", (stData.wMonPort));
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdmode")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 2)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		if(nTemp == 0) stData.bMode = YST_PQN;
		else if(nTemp == 1) stData.bMode = YST_SCOPE;
		else stData.bMode = YST_ETC;
		
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_MODE, 0, 0, &stData, sizeof(stData));
		Printf("PD Mode : %d(%s)\n", nTemp, (stData.bMode == YST_PQN)?"PQN":(stData.bMode == YST_SCOPE)?"SCOPE":"ETC");
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdsync")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 2)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.ucSync = (BYTE) nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_SYNC, 0, 0, &stData, sizeof(stData));
		Printf("PD Sync : %d(%s)\n", stData.ucSync, (stData.ucSync==0)?"RCA":(stData.ucSync==1)?"Gate":"Virtual");
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdsyncval")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < -10) || (nTemp > 10)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.ucSync = (char)nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_SYNC_VAL, 0, 0, &stData, sizeof(stData));
		Printf("PD Sync Value : %d\n", stData.ucSync);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdgain")){
		nTemp = GetIntegerFromString(strArg3);
		// if((nTemp < 0) || (nTemp > 35)){
		if((nTemp < 0) || (nTemp > 5)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}
		switch(nTemp)
		{
			// case 0 : stData.ucChGain = 26; break;
			case 1 : stData.ucChGain = 1; break;
			case 2 : stData.ucChGain = 10; break;
			case 3 : stData.ucChGain = 18; break;
			case 4: stData.ucChGain = 28; break;
			default :
			case 5: stData.ucChGain = 35; break;
		}

		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_GAIN, 0, 0, &stData, sizeof(stData));
		Printf("PD Gain : %d\n", nTemp);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdtrig")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 1660)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.ucTrigger = GetPdTrigger(nTemp);
		if(stData.ucTrigger <= 0) stData.ucTrigger = 1;	// 트리거 최소값

		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_TRIGGER, 0, 0, &stData, sizeof(stData));
		Printf("PD Trigger : %d\n", stData.ucTrigger);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdfiltermin")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 1) || (nTemp > 75)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.ucFilterMin = (char)nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_FILTER_MIN, 0, 0, &stData, sizeof(stData));
		Printf("PD Filter min : %d\n", stData.ucFilterMin);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdfiltermax")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 1) || (nTemp > 75)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.ucFilterMax = (char)nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_FILTER_MAX, 0, 0, &stData, sizeof(stData));
		Printf("PD Filter max : %d\n", stData.ucFilterMax);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdtype")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 3)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		if(nTemp == 0) stData.bType = YST_CH13_UHF;
		else if(nTemp == 1) stData.bType = YST_CH13_AE;
		else if(nTemp == 2) stData.bType = YST_CH13_GAIN;
		else stData.bType = YST_CH13_ENVELOP;

		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_TYPE, 0, 0, &stData, sizeof(stData));
		Printf("PD Type : %d(%s)\n", nTemp, (stData.bType == YST_CH13_UHF)?"UHF": \
			(stData.bType == YST_CH13_AE)?"AE":(stData.bType == YST_CH13_GAIN)?"GAIN":"ENVELOP");
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdformat")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 1)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}
		stData.ucFormat = nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_ADC_FORMAT, 0, 0, &stData, sizeof(stData));
		Printf("PD ADC Format(%d)\n", stData.ucFormat);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdperiod")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 65535)){
			Printf("Invalid Num : %d\r\n", nTemp);
			return 1;
		}

		stData.wRptTime = (WORD) nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_RPTTIME, 0, 0, &stData, sizeof(stData));
		Printf("PD test Period : %d\n", stData.wRptTime);
		return 1;
	}

	// ADC pattern test
	if(STR_EQUAL(strArg2, "pdadctest")){
		nTemp = GetIntegerFromString(strArg3);
		if((nTemp < 0) || (nTemp > 2)){
			Printf("Invalid Num1 : %d\r\n", nTemp);
			return 1;
		}

		nTemp2 = GetIntegerFromString(strArg4E);
		if((nTemp2 < 0) || (nTemp2 > 5)){
			Printf("Invalid Num2 : %d\r\n", nTemp2);
			return 1;
		}	
		stData.bAdcTest = nTemp2;

		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_ADC_TEST, nTemp, 0, &stData, sizeof(stData));
		Printf("PD ADC Test(%d, %d)\n", nTemp, stData.bAdcTest);
		return 1;
	}	

	if(STR_EQUAL(strArg2, "pdadcvir")){
		if(STR_EQUAL(strArg3, "on") || STR_EQUAL(strArg3, "On")) stData.bAdcVirtual = 1;
		else if(STR_EQUAL(strArg3, "off") || STR_EQUAL(strArg3, "Off")) stData.bAdcVirtual = 0;
		else {
			Printf("Invalid VIR Setting\r\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_ADC_VIRTUAL, 0, 0, &stData, sizeof(stData));
		Printf("PD ADC VIR(%d)\n", stData.bAdcVirtual);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdadcread")){
		nTemp = 1;
		stData.bAdcRead = nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_ADC_READ, 0, 0, &stData, sizeof(stData));
		Printf("PD ADC Read(%d)\n", stData.bAdcRead);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pddaq")){
		nTemp = 1;
		stData.bDaqStatus = nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_DAQ_STATUS, 0, 0, &stData, sizeof(stData));
		Printf("PD DAQ Status(%d)\n", stData.bDaqStatus);
		return 1;
	}
	
	if(STR_EQUAL(strArg2, "pdadcreset")){
		nTemp = 1;
		stData.bAdcReset = nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_ADC_RESET, 0, 0, &stData, sizeof(stData));
		Printf("PD ADC Reset(%d)\n", stData.bAdcReset);
		return 1;
	}

	if(STR_EQUAL(strArg2, "pdidle")){
		nTemp = 1;
		stData.bDaqIdle = nTemp;
		MakeIPCSetMessage(YET_GID_PD_NODE, YET_OID_PD_DAQ_IDLE, 0, 0, &stData, sizeof(stData));
		Printf("PD DAQ Idle(%d)\n", stData.bDaqIdle);
		return 1;
	}

	return 0;
}

int DecodeSetSPortCommand(char strArg2[], char strArg3[], char strArg4E[])
{
	SPortObjData stData;
	int nIdx;

	if(STR_EQUAL(strArg2, "devcnt") || STR_EQUAL(strArg2, "pollint") || STR_EQUAL(strArg2, "polltime") || STR_EQUAL(strArg2, "waittime")){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx < 0 || nIdx >= MAX_SPORT_CNT){
			Printf("Invalid Serial Port Index : %d, Max = %d\r\n", nIdx+1, MAX_SPORT_CNT);
			return 1;
		}
	}	
	if(STR_EQUAL(strArg2, "devcnt")){
		stData.ucDevCnt = GetIntegerFromString(strArg4E);
		MakeIPCSetMessage(YET_GID_SPORT, YET_OID_SPortDevCnt, nIdx, 0, &stData, sizeof(stData));
		Printf("Serial Port-%d Device Count : %d\r\n", nIdx+1, stData.ucDevCnt);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pollint") || STR_EQUAL(strArg2, "polltime")){
		stData.wPollInt = GetIntegerFromString(strArg4E);
		MakeIPCSetMessage(YET_GID_SPORT, YET_OID_SPortPollInt, nIdx, 0, &stData, sizeof(stData));
		Printf("Serial Port-%d Polling Time : %dsec\r\n", nIdx+1, stData.wPollInt);
		return 1;
	}
	if(STR_EQUAL(strArg2, "waittime")){
		stData.wWaitTime = GetIntegerFromString(strArg4E);
		MakeIPCSetMessage(YET_GID_SPORT, YET_OID_SPortWaitTime, nIdx, 0, &stData, sizeof(stData));
		Printf("Serial Port-%d Polling Wait Time : %dms\r\n", nIdx+1, stData.wWaitTime);
		return 1;
	}

	return 0;
}

int DecodeSetNATPFwdCommand(char strArg2[], char strArg3[], char strArg4E[])
{
	NATPFwdObjData stData;
	int nIdx;

	if(STR_EQUAL(strArg2, "pfip") || STR_EQUAL(strArg2, "pfdip") || STR_EQUAL(strArg2, "pfdestip") ||
	   STR_EQUAL(strArg2, "pfsp") || STR_EQUAL(strArg2, "pfsport") || STR_EQUAL(strArg2, "pfsrcport") ||
	   STR_EQUAL(strArg2, "pfdp") || STR_EQUAL(strArg2, "pfdport") || STR_EQUAL(strArg2, "pfdestport") ||
	   STR_EQUAL(strArg2, "pfp") || STR_EQUAL(strArg2, "pfpro") || STR_EQUAL(strArg2, "pfprotocol") ){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx < 0 || nIdx >= MAX_NAT_PFWD_CNT){
			Printf("Invalid Port Fwd Index : %d, Max = %d\r\n", nIdx+1, MAX_NAT_PFWD_CNT);
			return 1;
		}	
	}

	if(STR_EQUAL(strArg2, "pfip") || STR_EQUAL(strArg2, "pfdip") || STR_EQUAL(strArg2, "pfdestip")){
		GetIPArrayFromString(stData.mIP, strArg4E);
		MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_IPAddr, nIdx, 0, &stData, sizeof(stData));
		Printf("Port-forwarding %d-th dest IP : %s\r\n", nIdx+1, MakeIPAddrString(stData.mIP));
		return 1;
	}
	if(STR_EQUAL(strArg2, "pfsp") || STR_EQUAL(strArg2, "pfsport") || STR_EQUAL(strArg2, "pfsrcport")){
		stData.nSrcPort = GetIntegerFromString(strArg4E);
		if(stData.nSrcPort != 7000 && stData.nSrcPort != 7001 && stData.nSrcPort != 21 && stData.nSrcPort != 23 && stData.nSrcPort != 80){
			MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_SrcPort, nIdx, 0, &stData, sizeof(stData));
			Printf("Port-forwarding %d-th src port number = %d\r\n", nIdx+1, stData.nSrcPort);
			return 1;
		}
		Printf("Unable to use this port : %d\r\n", stData.nSrcPort);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pfdp") || STR_EQUAL(strArg2, "pfdport") || STR_EQUAL(strArg2, "pfdestport")){
		stData.nDestPort = GetIntegerFromString(strArg4E);
		if(stData.nDestPort <= 65535){
			MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_DestPort, nIdx, 0, &stData, sizeof(stData));
			Printf("Port-forwarding %d-th dest port number = %d\r\n", nIdx+1, stData.nDestPort);
			return 1;
		}
		Printf("Unable to use this port : %d\r\n", stData.nDestPort);
		return 1;
	}
	if(STR_EQUAL(strArg2, "pfp") || STR_EQUAL(strArg2, "pfpro") || STR_EQUAL(strArg2, "pfprotocol")){
		if(strcmp(strArg4E, "off") == 0 || strcmp(strArg4E, "no") == 0){
			stData.ucProtocol = 0;
			MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_Protocol, nIdx, 0, &stData, sizeof(stData));
			Printf("Port-forwarding %d-th : no use\r\n");
			return 1;
		}
		if(strcmp(strArg4E, "tcp") == 0 || strcmp(strArg4E, "TCP") == 0){
			stData.ucProtocol = 1;
			MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_Protocol, nIdx, 0, &stData, sizeof(stData));
			Printf("Port-forwarding %d-th : TCP\r\n");
			return 1;
		}
		if(strcmp(strArg4E, "udp") == 0 || strcmp(strArg4E, "UDP") == 0){
			stData.ucProtocol = 2;
			MakeIPCSetMessage(YET_GID_NAT_PFWD, YET_OID_PF_Protocol, nIdx, 0, &stData, sizeof(stData));
			Printf("Port-forwarding %d-th : UDP\r\n");
			return 1;
		}
		Printf("Invalid port-forwarding set data\r\n");
		return 1;
	}
	return 0;
}

int DecodeSetRouteTableCommand(char strArg2[], char strArg3[], char strArg4E[])
{
	RouteTblObjData stData;
	int nIdx;

	if(STR_EQUAL(strArg2, "rnet") || STR_EQUAL(strArg2, "rtnet") || STR_EQUAL(strArg2, "routenet") ||
		STR_EQUAL(strArg2, "rgw") || STR_EQUAL(strArg2, "rtgw") || STR_EQUAL(strArg2, "routegw") ||
		STR_EQUAL(strArg2, "ruse") || STR_EQUAL(strArg2, "rtuse") || STR_EQUAL(strArg2, "routeuse") ){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx < 0 || nIdx >= MAX_ROUTE_TBL_CNT){
			Printf("Invalid Routing Table Index : %d, Max = %d\r\n", nIdx+1, MAX_ROUTE_TBL_CNT);
			return 1;
		}	
	}

	if(STR_EQUAL(strArg2, "rnet") || STR_EQUAL(strArg2, "rtnet") || STR_EQUAL(strArg2, "routenet")){
		GetIPArrayFromString(stData.mRNet, strArg4E);
		stData.mRNet[3] = 0;
		MakeIPCSetMessage(YET_GID_ROUTE_TBL, YET_OID_RTT_RNet, nIdx, 0, &stData, sizeof(stData));
		Printf("Routing Table %d-th network : %s\r\n", nIdx+1, MakeIPAddrString(stData.mRNet));
		return 1;
	}
	if(STR_EQUAL(strArg2, "rgw") || STR_EQUAL(strArg2, "rtgw") || STR_EQUAL(strArg2, "routegw")){
		GetIPArrayFromString(stData.mRGW, strArg4E);
		MakeIPCSetMessage(YET_GID_ROUTE_TBL, YET_OID_RTT_RGW, nIdx, 0, &stData, sizeof(stData));
		Printf("Routing Table %d-th gateway : %s\r\n", nIdx+1, MakeIPAddrString(stData.mRGW));
		return 1;
	}
	if(STR_EQUAL(strArg2, "ruse") || STR_EQUAL(strArg2, "rtuse") || STR_EQUAL(strArg2, "routeuse")){
		if(strcmp(strArg4E, "off") == 0 || strcmp(strArg4E, "no") == 0){
			stData.ucUse = 0;
			MakeIPCSetMessage(YET_GID_ROUTE_TBL, YET_OID_RTT_Use, nIdx, 0, &stData, sizeof(stData));
			Printf("Routing Table %d-th : no use\r\n", nIdx+1);
			return 1;
		}
		if(strcmp(strArg4E, "on") == 0 || strcmp(strArg4E, "use") == 0){
			stData.ucUse = 1;
			MakeIPCSetMessage(YET_GID_ROUTE_TBL, YET_OID_RTT_Use, nIdx, 0, &stData, sizeof(stData));
			Printf("Routing Table %d-th : use\r\n", nIdx+1);
			return 1;
		}
		Printf("Invalid Routing Table usage\r\n");
		return 1;
	}
	return 0;
}

int DecodeDelCommand(char strArg2[], char strArg3[])
{
	SNodeObjData stSNode;
	URSNodeObjData stURSNode;
	SlvObjData stSlave;
	int nIdx;

	if(STR_EQUAL(strArg2, "snode")){
		if(STR_EQUAL(strArg3, "all")){
			MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SNodeDelAll, 0, 0, &stSNode, sizeof(stSNode));
			Printf("Deleted all invalid SNode\n");
			return 1;
		}
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx >= MAX_SNODE_CNT){
			Printf("Invalid Sensor Node Idx\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SNODE, YET_OID_SNodeDel, nIdx, 0, &stSNode, sizeof(stSNode));
		Printf("Delete SNode-%d\n", nIdx+1);
		return 1;
	}
	if(STR_EQUAL(strArg2, "slave")){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx >= MAX_SLAVE_CNT){
			Printf("Invalid Sensor Node Idx\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_SLAVE, YET_OID_SlvDel, nIdx, 0, &stSlave, sizeof(stSlave));
		Printf("Delete Slave-%d\n", nIdx+1);
		return 1;
	}
	if(STR_EQUAL(strArg2, "ursnode")){
		nIdx = GetIntegerFromString(strArg3) -1;
		if(nIdx >= MAX_URSNODE_CNT){
			Printf("Invalid URSensor Node Idx\n");
			return 1;
		}
		MakeIPCSetMessage(YET_GID_URSNODE, YET_OID_URSNodeDel, nIdx, 0, &stURSNode, sizeof(stURSNode));
		Printf("Delete URSNode-%d\n", nIdx+1);
		return 1;
	}

	Printf("Invalid del name\n");
	return 0;
}

int DecodeSetCommand(char strArg2[], char strArg3[], char strArg3E[], char strArg4[], char strArg4E[])
{
	int nResult;

	if((nResult = DecodeSetSystemCommand(strArg2, strArg3, strArg3E, strArg4, strArg4E)) != 0) return nResult;
	if((nResult = DecodeSetSCommCommand(strArg2, strArg3, strArg4)) != 0) return nResult;
	if((nResult = DecodeSetPlatformCommand(strArg2, strArg3)) != 0) return nResult;
	if((nResult = DecodeSetLoraCommand(strArg2, strArg3)) != 0) return nResult;
	if((nResult = DecodeSetSNodeCommand(strArg2, strArg3, strArg4E)) != 0) return nResult;
	if((nResult = DecodeSetSPortCommand(strArg2, strArg3, strArg4E)) != 0) return nResult;
	if((nResult = DecodeSetSensorCommand(strArg2, strArg3, strArg4)) != 0) return nResult;
	if((nResult = DecodeSetNATPFwdCommand(strArg2, strArg3, strArg4E)) != 0) return nResult;
	if((nResult = DecodeSetRouteTableCommand(strArg2, strArg3, strArg4E)) != 0) return nResult;
	if((nResult = DecodeSetPdCommand(strArg2, strArg3, strArg4E)) != 0) return nResult;

	Printf("Invalid set name\r\n");
	return 0;
}

int DecodeCommand(char strCommand[])
{
	char strArg1[301], strArg2[301], strArg3[301], strArg4[301], strArg3E[301], strArg4E[301], strArg5[301];
	int nShowMoreFlag = 0;
	int nResult;
	int nOffset = 0;

	if(strCommand[0] == '_') nShowMoreFlag = 1;
	if((nResult = DecodeSingleCommand(strCommand+nShowMoreFlag, nShowMoreFlag)) != 0) return nResult;

	nOffset += GetArgumentFromString(strArg1, strCommand, 0);
	nOffset += GetArgumentFromString(strArg2, strCommand+nOffset, 0);
	GetArgumentFromString(strArg3E, strCommand+nOffset, 1);
	nOffset += GetArgumentFromString(strArg3, strCommand+nOffset, 0);
	GetArgumentFromString(strArg4E, strCommand+nOffset, 1);
	nOffset += GetArgumentFromString(strArg4, strCommand+nOffset, 0);
	nOffset += GetArgumentFromString(strArg5, strCommand+nOffset, 1);

	if(STR_EQUAL(strArg1, "system")){
		system(strCommand+7);
		return 0;
	}

	if(STR_EQUAL(strArg1, "?") || STR_EQUAL(strArg1, "help") ||
		STR_EQUAL(strArg1, "_?") || STR_EQUAL(strArg1, "_help")){
		return DecodeHelpCommand(strArg2, nShowMoreFlag);
	}

	if(STR_EQUAL(strArg1, "print") || STR_EQUAL(strArg1, "show") || STR_EQUAL(strArg1, "sh") ||
		STR_EQUAL(strArg1, "_print") || STR_EQUAL(strArg1, "_show") || STR_EQUAL(strArg1, "_sh")){
		return DecodeShowCommand(strArg2, strArg3, nShowMoreFlag);
	}

	if(STR_EQUAL(strArg1, "clear") || STR_EQUAL(strArg1, "clr")){
		return DecodeClearCommand(strArg2);
	}

	if(STR_EQUAL(strArg1, "debug") || STR_EQUAL(strArg1, "_debug")){
		return DecodeDebugCommand(strArg2, strArg3, strArg3E, strArg4, strArg4E, strArg5, nShowMoreFlag);
	}

	if(strcmp(strArg1, "define") == 0 || strcmp(strArg1, "def") == 0){
		return DecodeDefineCommand(strArg2, strArg3, strArg3E, strArg4, strArg4E);
	}

	if(STR_EQUAL(strArg1, "config") || STR_EQUAL(strArg1, "con") || STR_EQUAL(strArg1, "set")){
		return DecodeSetCommand(strArg2, strArg3, strArg3E, strArg4, strArg4E);
	}
	if(STR_EQUAL(strArg1, "del")){
		return DecodeDelCommand(strArg2, strArg3);
	}

	Printf("Bad Command !\n");
	return 0;
}

const char *envp_init[] = { "HOME=/", "TERM=vt100", "LD_LIBRARY_PATH=/usr/lib:/lib:/usr/local/lib", "PATH=/usr/bin:/usr/sbin:/bin:/sbin:/mnt:/usr/local/sbin:/usr/local/bin", "TZ=KST-9", NULL, };
const char *argv_init[] = { "sh", NULL, NULL };
const char shellpath[32] = "/bin/sh";
int main(int argc, char **argv)
{
	int nResult;
	int nDBShmID, nIPCShmID, nLogShmID;
	char strCommand[301];
	CCommandManager Command;
	int nTry=0;
	
	input = stdin;
	output = stdout;
	
	Command.InitializeTerminal(input, output);
	gpDB = (YST_DB_MAIN *)InitializeYDBSharedMemory(&nDBShmID, sizeof(YST_DB_MAIN));
	gpIPC = (YST_IPC *)InitializeIPCSharedMemory(&nIPCShmID, sizeof(YST_IPC));
	gpLog = (YST_DB_LOG *)InitializeLogSharedMemory(&nLogShmID, sizeof(YST_DB_LOG));
	
	Printf("\n\n\n\n\nWelcome to %s System !\r\n\n\n", MODEL_NAME);
		
	if(argc > 1){
		gnTelnetFlag = 1;
		SleepMilliSeconds(500);
		while(1) {
			if(nTry++ > 3){
				Command.DestructTerminal();
				DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
				DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
				DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);
				return -1;
			}
			Printf("Please Insert Password \n");
			Printf("Password : ");
			Command.ReadPassword(strCommand);
		
			if(strcmp(strCommand, "youster") == 0) {     //debug mode(shell)
				Command.DestructTerminal();
				DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
				DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
				DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);
				execve(shellpath, (char* const*)argv_init, (char* const*)envp_init);
				return 0;
			}
			else if(strcmp(strCommand, "zwenter") == 0) { // Anybody login
				break;
			}
			else if(strcmp(strCommand, gpDB->General.szPassword) == 0) break;
			Printf("\n");
		}
		Printf("Password is OK.\n");
		Printf("Starting Command Line Interface...\n\n\n");
	}
	
	while(1){
		Command.ReadCommand(strCommand);
		
		if(strlen(strCommand) > 0){
			if((nResult = DecodeCommand(strCommand)) == -1) break;
		}
	}//end of while
	
	Command.DestructTerminal();
	DestroyYDBSharedMemory(nDBShmID, gpDB, FALSE);
	DestroyIpcSetSharedMemory(nIPCShmID, gpIPC, FALSE);
	DestroyLogSharedMemory(nLogShmID, gpLog, FALSE);
	
	return 0;
}
