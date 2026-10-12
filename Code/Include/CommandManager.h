///////////////////////////////////////////////////////////
// CommandManager.h
//
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//

#if !defined(COMMANDMANAGER_H__INCLUDED_)
#define COMMANDMANAGER_H__INCLUDED_

#include <termios.h>
#include "TypeDef.h"

void DisableCtrlCInput(FILE *input);
void EnableCtrlCInput(FILE *input);

class CCommandManager
{
public:
	CCommandManager();
	~CCommandManager();

public:
	int m_nQCnt;
	char m_strCmdQ[10][301];
	FILE *m_input, *m_output;
	struct termios m_InitSetting;
	char m_strCmd[301];
	char m_strCmdTemp[301];
	char m_bCheckFile;
	
	void InsertCommandToQ(char strCommand[]);
	BOOL GetCommandFromCmdQ(int nIndex, char strCommand[]);
	void InitializeTerminal(FILE *input, FILE *output);
	void DestructTerminal(void);
	int ReadChar(int nNoWait);
	int Bioskey(int nNoWait);
	void PrintBackspace(int nCount);
	void ReadCommand(char strCommand[]);
	void ReadPassword(char strCommand[]);
};

#endif //!defined(COMMANDMANAGER_H__INCLUDED_)
