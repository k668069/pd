/////////////////////////////////////////////////////////// 
// StringApp.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(STRTOINTCONVERT_H__INCLUDED_)
#define STRTOINTCONVERT_H__INCLUDED_

#include <stdio.h>
#include "TypeDef.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Function define
int HexToDecimal(char cData);
char *GetFormatedInteger(DDWORD dwNum);
BYTE GetOneByteFromNibbleString(char strData[]);
int GetTwoByteFromNibbleString(BYTE mValue[], char strData[]);
int GetNByteFromNibbleString(BYTE mValue[], char strData[], int nNByte);
void GetPacketFromString(BYTE mPacket[], char strData[], int nMaxSize);
int GetIntegerFromString(char strData[]);
int GetDecimalValueFromString(BYTE str[], int nSize);
double GetDoubleValueFromString(char strData[]);
void GetPhoneNumberFromString(char strPhoneNumber[], char strInput[], int nMax);
void GetNumericValueFromString(char strNumeric[], char strInput[], int nMax);
BOOL IsIntegerValidFromString(char strData[]);
char *GetLargeNumStr(DDWORD dwNum);

/////////////////////////////////////////////////////////////////////////////////
// Network Related function
char *MakeMACAddrString(BYTE MACAddr[]);
char *MakeRMACString(BYTE mRMAC[], int nCount);
char *MakeIPAddrString(BYTE mIP[], int nIdx=0);
char *IPAddrStr(BYTE mIP[], int nIdx=0);
char *MakeEUIAddrString(BYTE EUIAddr[]);
char *MakeEUIAddrStr3(BYTE EUIAddr[]);
char *MakeEUIAddrStr8(BYTE EUIAddr[]);
char *Make3EUIAddrStr(BYTE EUIAddr[]);
char *Make3EUIAddrStr2(BYTE EUIAddr[]);
char *GetTimeTickStr(DDWORD dwTime);
void GetMACAddrFromString(int nIP[], const char strIP[]);
void GetIPAddrFromString(int nIP[], const char strIP[]);
void GetIPArrayFromString(BYTE mIP[], const char strIP[]);
BYTE *GetIPArrayFromString(const char strIP[]);
void GetMACArrayFromString(BYTE mMAC[], const char strMAC[]);
void GetIPArrayFromDWORD(BYTE mIP[], DWORD dwIPAddr);
void GetIPAddrAndPortFromString(int nIP[], const char strIP[]);
void GetBroadcastFromIPAddr(char strIPAddress[], char strSubnetMask[], char strBroadcast[]);
int IsSameNetwork(char strIPAddress1[], char strIPAddress2[], char strSubnetMask[]);

/////////////////////////////////////////////////////////////////////////////////
// Common Read function
void FindEndOfLine(FILE *fp);
void DeleteRemarkLines(FILE *fp);
void SkipLines(FILE *fp, int nLineCount);
int CompareTitle(FILE *fp, char strTitle[]);
BOOL ReadIntegerFromFile(FILE *fp, int *nNumber, char cIndicator);
BOOL ReadLongIntegerFromFile(FILE *fp, long *lNumber, char cIndicator);
BOOL ReadStringFromFile(FILE *fp, char strData[], int nMaxCount, char cIndicator);

void StringCopy(char strDest[], const char strSrc[], int nMaxCount);

///////////////////////////////////////////////////////////////////////////////////
// Application Specific Function
void GetDeviceID(char strDeviceID[], char strFrom[]);

#endif //!defined(STRTOINTCONVERT_H__INCLUDED_)
