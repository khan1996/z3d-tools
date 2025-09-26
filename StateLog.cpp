// StateLog.cpp: implementation of the CStateLog class.
//
//////////////////////////////////////////////////////////////////////

#include "StateLog.h"
#include <Windows.h>
#include "GMMemory.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FILE *CStateLog::m_fp;

CStateLog::CStateLog()
{
	m_fp=NULL;
}

CStateLog::~CStateLog()
{
	fclose(m_fp);
}

void CStateLog::Create(char *strFilename)
{
	m_fp=fopen(strFilename,"wt");
	if(m_fp==NULL)
		MessageBox(NULL,"Create Logfile failed",0,0);
}

void CStateLog::Message(char *strMessage, char *strFilename, int strLine)
{	
	if(m_fp==NULL)
	{
		CStateLog::Create("c:\\MP_Client.log");
	}
	fprintf(m_fp,"%-30s _Filename_: %-20s _Line_: %6d\n",strMessage,strFilename,strLine);	
}
