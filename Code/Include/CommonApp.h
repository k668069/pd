///////////////////////////////////////////////////////////
// CommonApp.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(COMMONAPP_H__INCLUDED_)
#define COMMONAPP_H__INCLUDED_

#include "TypeDef.h"
#include <stdio.h>

#include "TypeDef.h"
#define MEM_EQUAL(mem1, mem2, size) (!memcmp(mem1, mem2, size))
#define STR_EQUAL(str1, str2) (!strcmp(str1, str2))

WORD GetCRC16Value(BYTE *Data, int nLen, WORD wCRCInit = 0);
DWORD GetCRC32Value(BYTE *Data, long nSize, DWORD dwCRCInit = 0);
int EncodeWithBase64(BYTE Src[], BYTE Dst[], int nSize);
int DecodeWithBase64(BYTE Src[], BYTE Dst[], int nSize);
BOOL IsBase64EncodedData(BYTE ucData);

#endif //COMMONAPP_H__INCLUDED_
