///////////////////////////////////////////////////////////
// TypeDef.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(TYPEDEF_H__INCLUDED_)
#define TYPEDEF_H__INCLUDED_

typedef char	 			SBYTE;
typedef unsigned char 		BYTE;
typedef unsigned short int 	WORD;
typedef unsigned long int 	DWORD;
typedef unsigned long long	DDWORD;
typedef char            	BOOL;
// typedef unsigned long		UINT;
typedef unsigned int		UINT;
typedef short int			INT16;
typedef long int			INT32;
typedef unsigned short int 	UINT16;
typedef unsigned long int 	UINT32;
typedef unsigned long long 	UINT64;

#ifndef TRUE
#define FALSE           0
#define TRUE            1
#endif

#define SIZE_1K		0x00000400
#define SIZE_4K		0x00001000
#define SIZE_8K		0x00002000
#define SIZE_16K	0x00004000
#define SIZE_32K	0x00008000
#define SIZE_64K	0x00010000
#define SIZE_128K	0x00020000
#define SIZE_256K	0x00040000
#define SIZE_512K	0x00080000

#define SIZE_1M		0x00100000
#define SIZE_2M		0x00200000
#define SIZE_4M		0x00400000
#define SIZE_8M		0x00800000
#define SIZE_16M	0x01000000
#define SIZE_32M	0x02000000
#define SIZE_64M	0x04000000
#define SIZE_128M	0x08000000
#define SIZE_256M	0x10000000
#define SIZE_512M	0x20000000

#define SIZE_1G		0x40000000
#define SIZE_2G		0x80000000

#endif //!defined(TYPEDEF_H__INCLUDED_)
