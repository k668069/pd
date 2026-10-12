/////////////////////////////////////////////////////////// 
// ClassXMLStreamParser.h 
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(CLASSXMLSTREAMPARSER_H__INCLUDED_)
#define CLASSXMLSTREAMPARSER_H__INCLUDED_

#include "TypeDef.h"
#include "ClassQueue.h"

#define TAG_NAME_SIZE	20
#define ATTR_SIZE		50
#define DATA_SIZE		1024L
#define TAG_DEPTH		5

class CXMLStreamParser
{
public : 
	char m_szRcvData[DATA_SIZE];			// Content 저장공간
	char m_szContent[DATA_SIZE];			// Content 저장공간
	int m_nRcvDataSize;						// 수신 데이터 크기
	int m_nContentSize;						// Content 데이터 크기

	char m_szTagName[TAG_DEPTH][TAG_NAME_SIZE];	// 수신된 태그 이름
	char m_szAttr[TAG_DEPTH][ATTR_SIZE];		// Atribute 저장공간
	int m_nTagDepth;
	int m_nContentDepth;
	CQueue *m_pQ;

	CXMLStreamParser();
	~CXMLStreamParser();
	
	void Initialize(CQueue *pQ);
	void Destroy(void);
	void SetFileSaveTag(const char szFileTag[], const char szFileName[], BYTE ucDecodeBase64);
	void SaveToFile(void);
	void SaveToFileBase64(void);
	BOOL ParseQueueData(void);
private :
	char m_szFileTag[TAG_NAME_SIZE];	// 파일로 저장해야 되는 부분의 태크이름
	char m_szFileName[100];				// 저장할 파일 이름
	char m_szFileName2[100];			// 저장할 파일 이름
	BOOL m_bSavingToFile;				// 파일로 저장하는 경우 1
	BYTE m_ucDecodeBase64;				// BASE64 디코딩을 해야 하는 경우 1, 디코딩과 원본을 같이 저장하는 경우 2

	int FindStartOfTag(void);
	int FindEndOfTag(void);
	int ParseTagName(char szTagName[], int nMaxLen);
};
#endif //CLASSXMLSTREAMPARSER_H__INCLUDED_
