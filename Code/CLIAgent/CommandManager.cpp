/////////////////////////////////////////////////////////// 
// CommandManager.cpp
// 
// copyright : (c) 2022 by Ziwooit
// Date      : 2022. 02. 07 
//
#include <stdio.h>
#include <string.h>
#include "CommandManager.h"
#include "KeyDef.h"
#include "TimeApp.h"
#include "LogMessage.h"

CCommandManager::CCommandManager()
{
	m_nQCnt = 0;
}

CCommandManager::~CCommandManager()
{
}

void CCommandManager::InsertCommandToQ(char strCmd[])
{
	int i;

	if(m_nQCnt && strcmp(m_strCmdQ[m_nQCnt-1], strCmd) == 0) return;
	if(m_nQCnt < 10){
		strcpy(m_strCmdQ[m_nQCnt], strCmd);
		m_nQCnt++;
	}
	else{
		for(i = 0 ; i < 9 ; i++){
			if(strcmp(m_strCmdQ[i], strCmd) == 0) break;
		}
		if(i >= 9) i = 0;
		for( ; i < 9 ; i++){
			strcpy(m_strCmdQ[i], m_strCmdQ[i+1]);
		}
		strcpy(m_strCmdQ[9], strCmd);
	}
}

BOOL CCommandManager::GetCommandFromCmdQ(int nIndex, char strCmd[])
{
	if(nIndex <= 0 || nIndex > m_nQCnt) return FALSE;
	strcpy(strCmd, m_strCmdQ[m_nQCnt-nIndex]); 
	return TRUE;
}

void CCommandManager::InitializeTerminal(FILE *input, FILE *output)
{
	struct termios NewSetting;
	
	m_input = input;
	m_output = output;
	
	tcgetattr(fileno(m_input), &m_InitSetting);
	NewSetting = m_InitSetting;
	NewSetting.c_lflag &= ~ICANON;//Non-canonical mode
	NewSetting.c_lflag &= ~ECHO;	//No echo
	NewSetting.c_lflag &= ~ISIG;	//Not allow Ctrl+C exit
	NewSetting.c_cc[VMIN] = 1;	//VMIN을 1글자로 설정한다.
	NewSetting.c_cc[VTIME] = 0;	//VTIME을 0초로 설정한다.
	tcsetattr(fileno(m_input), TCSANOW, &NewSetting);
}//fgetc(input)은 반드시 1글자의 입력이 있어야지 리턴된다.

void DisableCtrlCInput(FILE *input)
{
	struct termios NewSetting;
	
	tcgetattr(fileno(input), &NewSetting);
	NewSetting.c_lflag &= ~ISIG;	//Not allow Ctrl+C exit
	tcsetattr(fileno(input), TCSANOW, &NewSetting);
}

void EnableCtrlCInput(FILE *input)
{
	struct termios NewSetting;
	
	tcgetattr(fileno(input), &NewSetting);
	NewSetting.c_lflag |= ISIG;	//Allow Ctrl+C exit
	tcsetattr(fileno(input), TCSANOW, &NewSetting);
}

void CCommandManager::DestructTerminal(void)
{
	tcsetattr(fileno(m_input), TCSANOW, &m_InitSetting);
}

int CCommandManager::ReadChar(int nNoWait)
{
	int ch;
		
	if(nNoWait){
		struct termios OldSetting, NewSetting;
		tcgetattr(fileno(m_input), &OldSetting);
		NewSetting = OldSetting;
		NewSetting.c_cc[VMIN] = 0;
		tcsetattr(fileno(m_input), TCSANOW, &NewSetting);
		ch = fgetc(m_input);
		tcsetattr(fileno(m_input), TCSANOW, &OldSetting);
	}
	else ch = fgetc(m_input);
	
	if(ch != EOF) return ch;
	return -1;
}

int CCommandManager::Bioskey(int nNoWait)
{
	int ch;
	
	if((ch = ReadChar(nNoWait)) == -1) return -1;
	
	if(ch != YBK_ESC) return ch;//Escape sequence
	if((ch = ReadChar(nNoWait)) == -1) return YBK_ESC;
	if(ch != '[') return YBK_ESC;
	if((ch = ReadChar(nNoWait)) == -1) return YBK_ESC;
	switch(ch){
		case '1' : ReadChar(nNoWait);
			return YBK_INSERT;//	0x1B31	//Esc(0x1B),1(0x31), .(0x2E)
		case '2' : ReadChar(nNoWait);
			return YBK_HOME;//	0x1B32	//Esc(0x1B),2(0x32), .(0x2E)
		case '3' : ReadChar(nNoWait);
			return YBK_PGUP;//	0x1B33	//Esc(0x1B),3(0x33), .(0x2E)
		case '4' : ReadChar(nNoWait);
			return YBK_DELETE;//	0x1B34	//Esc(0x1B),4(0x34), .(0x2E)
		case '5' : ReadChar(nNoWait);
			return YBK_END;//	0x1B35	//Esc(0x1B),5(0x35), .(0x2E)
		case '6' : ReadChar(nNoWait);
			return YBK_PGDN;//	0x1B36	//Esc(0x1B),6(0x36), .(0x2E)
		case 'A' : return YBK_UP;//		0x1B41	//Esc(0x1B),A(0x41)
		case 'B' : return YBK_DOWN;//	0x1B42	//Esc(0x1B),B(0x42)
		case 'C' : return YBK_RIGHT;//	0x1B43	//Esc(0x1B),C(0x43)
		case 'D' : return YBK_LEFT;//	0x1B44	//Esc(0x1B),D(0x44)
		default : break;
	}
	return YBK_ESC;
}

void CCommandManager::PrintBackspace(int nCount)
{
	int i;
	
	for(i = 0 ; i < nCount ; i++) fprintf(m_output, "\b \b");
}

void CCommandManager::ReadCommand(char strCommand[])
{
	char strCommandTemp[301];
	int nOldCommandIndex = 0;
	int nIndex = 0;
	int ch;
	int nDeleteCount;
	
	strCommand[0] = 0;
	fprintf(m_output, "Command > ");
	while(nIndex < 70){
		if((ch = Bioskey(0)) != -1){
			if(ch == YBK_UP || ch == YBK_DOWN){
				nDeleteCount = strlen(strCommand);
				switch(ch){
				case YBK_UP :
					if(nOldCommandIndex == 0) strcpy(strCommandTemp, strCommand);
					if(GetCommandFromCmdQ(nOldCommandIndex+1, strCommand)){
						nOldCommandIndex++;
						PrintBackspace(nDeleteCount);
						fprintf(m_output, "%s", strCommand);
						nIndex = strlen(strCommand);
					}
					break;
				case YBK_DOWN :
					if(nOldCommandIndex > 1){
						if(GetCommandFromCmdQ(nOldCommandIndex-1, strCommand)){
							nOldCommandIndex--;
							PrintBackspace(nDeleteCount);
							fprintf(m_output, "%s", strCommand);
							nIndex = strlen(strCommand);
						}
					}
					else if(nOldCommandIndex == 1){
						nOldCommandIndex--;
						strcpy(strCommand, strCommandTemp);
						PrintBackspace(nDeleteCount);
						fprintf(m_output, "%s", strCommand);
						nIndex = strlen(strCommand);
					}
					break;
				}
			}
			else{
				nOldCommandIndex = 0;
				switch(ch){
				case YBK_BS :
				case YBK_BS2 :
				case YBK_DELETE :
				case YBK_LEFT :
					if(nIndex > 0){
						nIndex--;
						strCommand[nIndex] = 0;
						PrintBackspace(1);
					}
					break;
				case YBK_RIGHT :
					GetCommandFromCmdQ(1, strCommandTemp);
					if(strlen(strCommandTemp) > (unsigned)nIndex){
						strCommand[nIndex] = strCommandTemp[nIndex];
						strCommand[++nIndex] = 0;
						fputc(strCommand[nIndex-1], m_output);
					}
					break;
				default : break;
				}
			}
			if(ch >= 32 && ch <= 126){
				strCommand[nIndex++] = ch;
				strCommand[nIndex] = 0;
				fputc(ch, m_output);
			}
			else if(ch >= 128 && ch <= 255){
				strCommand[nIndex++] = ch;
				strCommand[nIndex] = 0;
				fputc(ch, m_output);
			}
			else if(ch == '\n') break;
		}
	}
	if(strlen(strCommand) > 0) InsertCommandToQ(strCommand);
	fprintf(m_output, "\n");
}

void CCommandManager::ReadPassword(char strCommand[])
{
	int nIndex = 0;
	int ch;
	
	strCommand[0] = 0;
	while(Bioskey(1) != -1) SleepMilliSeconds(1);
	while(nIndex < 70){
		if((ch = Bioskey(0)) != -1){
			switch(ch){
			case YBK_BS :
			case YBK_DELETE :
			case YBK_LEFT :
				if(nIndex > 0){
					nIndex--;
					strCommand[nIndex] = 0;
					PrintBackspace(1);
				}
				break;
			default : break;
			}
			if(ch >= 32 && ch <= 126){
				strCommand[nIndex++] = ch;
				strCommand[nIndex] = 0;
				fputc('*', m_output);
			}
			else if(ch == '\n') break;
		}
	}
	fprintf(m_output, "\n");
}