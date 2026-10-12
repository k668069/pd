/////////////////////////////////////////////////////////// 
// DesEncryption.h
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 

#if !defined(DESENCRYPTION_H__INCLUDED_)
#define DESENCRYPTION_H__INCLUDED_

typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;

// nSize값은 반드시 8의 배수가 되어야 한다.
int EncryptDataWithDES16(uchar Data[], int nSize);
int DecryptDataWithDES16(uchar Data[], int nSize);

#endif //!defined(DESENCRYPTION_H__INCLUDED_)
