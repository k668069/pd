/////////////////////////////////////////////////////////// 
// Network.cpp 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "Network.h"
#include "StringApp.h"
#include "TimeApp.h"
#include "DeviceInfoDB.h"
#include "LogMessage.h"
#include "Agent.h"
#include "HWControl.h"

extern YST_DB_MAIN	*gpDB;//DeviceInfoDB.h
/////////////////////////////////////////////////////////////////////////////////////////////////
// Network

void SetMasquerade(void)
{
	system("iptables -A POSTROUTING -t nat -o eth0 -j MASQUERADE");
	system("echo 1 > /proc/sys/net/ipv4/ip_forward");
}

void SetNetworkConfig(char strIPAddress[], char strSubnetMask[])
{
	char strCommand[301];
	
	sprintf(strCommand, "ifconfig eth0 %s netmask %s up", strIPAddress, strSubnetMask);
	system(strCommand);
}

void SetNetworkConfig(BYTE mIP[], BYTE mSM[])
{
	char strCommand[301];
	
	sprintf(strCommand, "ifconfig eth0 %s netmask %s up", MakeIPAddrString(mIP,0), MakeIPAddrString(mSM,1));
	system(strCommand);
}

void SetNetworkConfig2(BYTE mIP[])
{
	char strCommand[301];
	
	sprintf(strCommand, "ifconfig eth1 %s netmask 255.255.255.0 up", MakeIPAddrString(mIP));
	system(strCommand);
}

void SetNetworkGateway(char strGateway[])
{
	char strCommand[301];
	
	sprintf(strCommand, "route add default gw %s", strGateway);
	system(strCommand);
}

void SetNetworkGateway(BYTE mGW[])
{
	char strCommand[301];
	
	sprintf(strCommand, "route add default gw %s", MakeIPAddrString(mGW));
	system(strCommand);
}

void SetNetworkConfig2(char strIPAddress2[])
{
	char strCommand[301];
	int nIP[5];
	char strIPStart[MAX_IP_STR+1];
	char strIPEnd[MAX_IP_STR+1];
	char strIPAddress[MAX_IP_STR+1];

	GetIPAddrFromString(nIP, strIPAddress2);
	sprintf(strIPAddress, "%d.%d.%d.%d", nIP[0], nIP[1], nIP[2], 1);	//Real 192.168.129.1
	sprintf(strIPStart, "%d.%d.%d.%d", nIP[0], nIP[1], nIP[2], 10);		//Range 192.168.129.10 - 192.168.129.254
	sprintf(strIPEnd, "%d.%d.%d.%d", nIP[0], nIP[1], nIP[2], 254);		//Range 192.168.129.10 - 192.168.129.254

	system("kill -TERM `cat /var/run/udhcpd.pid`");		//Stop DHCP demon
	sprintf(strCommand, "ifconfig eth1 %s netmask 255.255.255.0 up", strIPAddress);
	system(strCommand);
	system("ifconfig eth1 down");
	system("ifconfig eth1 up");
	sprintf(strCommand, "echo \"start   %s\nend     %s\ninterface       eth1\nlease_file      /var/udhcpd.leases"
			"\noption  subnet  255.255.255.0\noption  router  %s\noption  dns     168.126.63.1\noption  domain  local\noption  lease   864000\n\" > /etc/udhcpd.conf",
			strIPStart, strIPEnd, strIPAddress);
	system(strCommand);		//Write Configuration file
	system("udhcpd &");// Rerun DHCP demon
}

void GetIPAddress(char strIPAddress[], char strSubnetMask[])
{
	int nFd;
	struct ifreq ifr;
	struct sockaddr_in	*sin;
	char strIPAddr[100];

	strcpy(strIPAddress, "0.0.0.0");
	strcpy(strSubnetMask, "0.0.0.0");
	strcpy(ifr.ifr_name, strIfName);
	ifr.ifr_addr.sa_family = AF_INET;
	if(ioctl(nFd, SIOCGIFADDR, &ifr) == 0){
		sin = (struct sockaddr_in *)&ifr.ifr_addr;
		inet_ntop(AF_INET, &sin->sin_addr, strIPAddr, sizeof(strIPAddr));
		strcpy(strIPAddress, strIPAddr);		
		strcpy(ifr.ifr_name, strIfName);
		if (ioctl(nFd, SIOCGIFNETMASK, &ifr) < 0) PrintGeneralLogMsg("(HWManager) Get IP : Netmask - Unknown");
		else{
			sin = (struct sockaddr_in *)&ifr.ifr_netmask;
			inet_ntop(AF_INET, &sin->sin_addr, strIPAddr, sizeof(strIPAddr));
			strcpy(strSubnetMask, strIPAddr);
		}
	}
	else PrintGeneralLogMsg("(HWManager) Get IP : IPAddr - Unknown");
}

void GetGateway(char strDefaultGateway[])
{
	FILE *fp;
	char strTemp[40];
	int nIndex = 0, nCount = 0;
	int nIP[5];
	int ch;
	
	strTemp[0] = 0;
	fp = fopen("route", "rt");
	while((ch=getc(fp)) != EOF){
		if(nIndex == 7){
			if(ch >= '0' && ch <= '9'){
				strTemp[nCount++] = ch;
				nIndex++;
			}
		}
		else if(nIndex == 8){
			if((ch >= '0' && ch <= '9') || ch == '.'){
				strTemp[nCount++] = ch;
			}
			else{
				strTemp[nCount] = 0;
				break;
			}
		}
		else{
			nIndex = 0;
			nCount = 0;
		}
	}
	fclose(fp);

	strTemp[nCount] = 0;
	GetIPAddrFromString(nIP, strTemp);
	sprintf(strDefaultGateway, "%d.%d.%d.%d", nIP[0], nIP[1], nIP[2], nIP[3]);
}

void GetNetworkConfig(char strIPAddress[], char strSubnetMask[], char strDefaultGateway[])
{
	GetIPAddress(strIPAddress, strSubnetMask);
	GetGateway(strDefaultGateway);
}

void CheckNetworkConfig(char strIPAddress[], char strSubnetMask[], char strGateway[])
{
	FILE *fp;
	char strIP[50];

	if((fp = fopen(AGENT_DIR"/ipfile", "rt")) == NULL) return;
	if(!ReadStringFromFile(fp, strIP, 50, 0)) goto end;
	strcpy(strIPAddress, strIP);
	FindEndOfLine(fp);

	if(!ReadStringFromFile(fp, strIP, 50, 0)) goto end;
	strcpy(strSubnetMask, strIP);
	FindEndOfLine(fp);

	if(!ReadStringFromFile(fp, strIP, 50, 0)) goto end;
	strcpy(strGateway, strIP);
	
end :
	fclose(fp);
	system("rm -f "AGENT_DIR"/ipfile");
}

void InitializeNetwork(void)
{
	SetNetworkConfig(gpDB->General.mIP, gpDB->General.mSM);	
	SetEthConfig(1, gpDB->General.ucOperEth1);
	if(gpDB->General.ucOperEth1 < YET_ETH_OFF) SetNetworkConfig2(gpDB->General.mIP2);
	SetNetworkConfig(gpDB->General.mIP, gpDB->General.mSM);
	SetNetworkConfig2(gpDB->General.mIP2);
	SetNetworkGateway(gpDB->General.mGW);
	SetMasquerade();
}