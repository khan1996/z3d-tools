/* *********************************************************************

  * CLogMgr
  * 파일 : LogMgr.cpp
  * 기능 : Caldron Engine내 메세지나 에러 등을 처리하는 Log System
  * 작성일 : 2003.10.23
  * 작성자 : wizardbug ( 2003.10.23)
  * 수정자 : 

********************************************************************** */

#include "./LogMgr.h"
#include "./Console.h"
#include "../Scene/MainSceneMgr.h"


//////////////////////////////////////////////////////////////////////
// static 함수, 변수 
//////////////////////////////////////////////////////////////////////

Caldron::Base::CLogMgr *Caldron::Base::CLogMgr::ms_pInstance = NULL;

/*
Caldron::Base::CLogMgr *Caldron::Base::CLogMgr::_GetInstance();
void Caldron::Base::CLogMgr::_LogMessage(const char *,...);
void Caldron::Base::CLogMgr::_LogError(const char *,...);
void Caldron::Base::CLogMgr::_SetOutDevice(Caldron::LOG_OUTDEVICES iDevice);
void Caldron::Base::CLogMgr::_SetOutMessageFlag(Caldron::LOG_MESSAGES iMessage,bool bFlag);
void Caldron::Base::CLogMgr::_Close();
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Caldron::Base::CLogMgr::CLogMgr() : m_pLogFile(0), m_iOutDevice(Caldron::LOG_FILEOUT),m_bBufferEmpty(true)
{

	
	m_pBitFlag = new CBitset;
	
	if(m_pBitFlag != NULL) 
	{

		m_pBitFlag->ResizeBits(Caldron::LOG_MESSAGES_NUM);
		m_pBitFlag->ClearBits();
	
		for(int i = 0; i < Caldron::LOG_MESSAGES_NUM; i++ ) 
		{
			m_pBitFlag->SetBit(i);
		}
	}

	memset((char *)m_pLogBuffer,sizeof(char),2560);
	memset((char *)m_pLogFileName,sizeof(char),256);
	

}
void Caldron::Base::CLogMgr::ValueRelease() 
{

	if(m_pLogFile != NULL)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
	Caldron::SafeDelete(m_pBitFlag);
	m_pBitFlag = NULL;

}
Caldron::Base::CLogMgr::~CLogMgr() 
{
}

void Caldron::Base::CLogMgr::_Close()
{
	if(ms_pInstance != NULL)
	{
		ms_pInstance->ValueRelease();	
		Caldron::SafeDelete(ms_pInstance);
		ms_pInstance = NULL;
	}

}


Caldron::Base::CLogMgr *Caldron::Base::CLogMgr::_GetInstance() 
{
	if(ms_pInstance == NULL)
	{
		ms_pInstance = new Caldron::Base::CLogMgr;
		ms_pInstance->WriteFirstRunLog();

	}

	return ms_pInstance;

}

void Caldron::Base::CLogMgr::_SetOutDevice(Caldron::LOG_OUTDEVICES iDevice) 
{
	if(ms_pInstance == NULL)
	{
		_GetInstance();	// CLogMgr 생성
	}
	
	switch(iDevice) 
	{

		case LOG_FILEOUT:
		case LOG_CONSOLEOUT:				// Console
		case LOG_MESSAGEBOXOUT:				// Window Message Box
			ms_pInstance->m_iOutDevice = iDevice;
			break;
		default:
			_LogError("CLogMgr::SetOutDevice() | 지원하지 않는 Device를 선택하셨습니다.");	
			break;


	}

}
void Caldron::Base::CLogMgr::_SetOutMessageFlag(Caldron::LOG_MESSAGES iMessage,bool bFlag) 
{
	if(ms_pInstance == NULL)
	{
		_GetInstance();	// CLogMgr 생성
	}
	if(ms_pInstance->m_pBitFlag != NULL) {
		if(bFlag == true) 
		{
			ms_pInstance->m_pBitFlag->SetBit(iMessage);
			ms_pInstance->_LogMessage("======= %s Bit On =======",LOG_LABEL[iMessage]);
		}
		else
		{
			ms_pInstance->_LogMessage("======= %s Bit Off =======",LOG_LABEL[iMessage]);
			ms_pInstance->m_pBitFlag->ClearOneBit(iMessage);
			
		}
	}
}
void Caldron::Base::CLogMgr::_LogMessage(const char *pFormat,...) 
{
	if(ms_pInstance == NULL)
	{
		_GetInstance();	// CLogMgr 생성
	}
	if((ms_pInstance->m_pBitFlag != NULL)  && (!ms_pInstance->m_pBitFlag->GetBit(Caldron::LOG_MESSAGE)))
		return;

/*
	// Checking 루틴시작
	char *pCheckptr = (char *)pFormat;   
	int iFCount = 0;	// % Count

    while((*pCheckptr) != '\0') 
    { 
		 if((*pCheckptr) == '%') 
			iFCount++; 
         pCheckptr++;

    }          

	int iExist = 0;                                         
	int iVaCount = 0; 
    va_list ap; 
      
    va_start( ap, pFormat ); 
    while( iExist != -1 ) 
    { 
		iVaCount++; 
		iExist = va_arg( ap, char); 
    } 
    va_end( ap ); 
    if( iFCount != iVaCount ) 
		return;     
    // Checking 루틴종료 
*/
	va_list ap;
	va_start(ap, pFormat);
	vsprintf(ms_pInstance->m_pLogBuffer, pFormat, ap);
	va_end(ap);
	ms_pInstance->m_bBufferEmpty = false;
	
	switch(ms_pInstance->m_iOutDevice) 
	{
		case LOG_FILEOUT:
			ms_pInstance->WriteBufferToFile(LOG_MESSAGE);
			break;
		case LOG_CONSOLEOUT:				// Console
			ms_pInstance->WriteBufferToConsole(LOG_MESSAGE);
			break;
		case LOG_MESSAGEBOXOUT:				// Window Message Box
			ms_pInstance->WriteBufferToMessageBox(LOG_MESSAGE);
			break;		
	}
}
void Caldron::Base::CLogMgr::_LogError(const char *pFormat,...) 
{
	if(ms_pInstance == NULL)
	{
		_GetInstance();	// CLogMgr 생성
	}
	
	if((ms_pInstance->m_pBitFlag != NULL)  && (!ms_pInstance->m_pBitFlag->GetBit(Caldron::LOG_ERROR)))
		return;


	va_list ap;

	va_start(ap, pFormat);
	vsprintf(ms_pInstance->m_pLogBuffer, pFormat, ap);
	va_end(ap);
	ms_pInstance->m_bBufferEmpty = false;
	
	switch(ms_pInstance->m_iOutDevice) 
	{
		case LOG_FILEOUT:
			ms_pInstance->WriteBufferToFile(LOG_ERROR);
			break;
		case LOG_CONSOLEOUT:				// Console
			ms_pInstance->WriteBufferToConsole(LOG_ERROR);
			break;
		case LOG_MESSAGEBOXOUT:				// Window Message Box
			ms_pInstance->WriteBufferToMessageBox(LOG_ERROR);
			break;		
	}
}
void Caldron::Base::CLogMgr::_LogNoHeader(const char *pFormat,...)
{
	if(ms_pInstance == NULL)
	{
		_GetInstance();	// CLogMgr 생성
	}
	
	if((ms_pInstance->m_pBitFlag != NULL)  && (!ms_pInstance->m_pBitFlag->GetBit(Caldron::LOG_NOHEADER)))
		return;


	va_list ap;

	va_start(ap, pFormat);
	vsprintf(ms_pInstance->m_pLogBuffer, pFormat, ap);
	va_end(ap);
	ms_pInstance->m_bBufferEmpty = false;
	
	switch(ms_pInstance->m_iOutDevice) 
	{
		case LOG_FILEOUT:
			ms_pInstance->WriteBufferToFile(LOG_NOHEADER);
			break;
		case LOG_CONSOLEOUT:				// Console
			ms_pInstance->WriteBufferToConsole(LOG_NOHEADER);
			break;
		case LOG_MESSAGEBOXOUT:				// Window Message Box
			ms_pInstance->WriteBufferToMessageBox(LOG_NOHEADER);
			break;		
	}
}
void Caldron::Base::CLogMgr::WriteFirstRunLog() 
{
	time_t	t = time(NULL);
	char *pDate = asctime(localtime(&t));

	if(m_pLogFile == NULL)
	{
		m_pLogFile = fopen(Caldron::Base::LOG_FILENAME,"at+");
	}
	if(m_pLogFile != NULL) 
	{
		fprintf(m_pLogFile,"\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		fprintf(m_pLogFile,"!!!!!!! Below Logs, Application Last Run !!!!!!! %s",pDate);
		fprintf(m_pLogFile,"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		fclose(m_pLogFile);
	}

	m_pLogFile = NULL;

}
void Caldron::Base::CLogMgr::WriteBufferToFile(Caldron::LOG_MESSAGES iMessage) 
{
	time_t	t = time(NULL);
	char *pDate = asctime(localtime(&t));

	if(m_pLogFile == NULL)
	{
		m_pLogFile = fopen(Caldron::Base::LOG_FILENAME,"at+");
	}
	if(m_pLogFile != NULL) 
	{
		if(iMessage == LOG_NOHEADER) 
		{
			fprintf(m_pLogFile,"%s",m_pLogBuffer);
		}
		else 
			fprintf(m_pLogFile,"%s %s | %s",LOG_LABEL[iMessage],m_pLogBuffer,pDate);
		fclose(m_pLogFile);
	}

	m_pLogFile = NULL;

}
void Caldron::Base::CLogMgr::WriteBufferToConsole(Caldron::LOG_MESSAGES iMessage)
{
	if(iMessage == LOG_NOHEADER) 
	{
		Caldron::Scene::CMainSceneMgr::_GetConsole()->AddText("%s",m_pLogBuffer);
	}
	else 
		Caldron::Scene::CMainSceneMgr::_GetConsole()->AddText("%s %s",LOG_LABEL[iMessage],m_pLogBuffer);
	
}
void Caldron::Base::CLogMgr::WriteBufferToMessageBox(Caldron::LOG_MESSAGES iMessage)
{

	MessageBox(NULL,m_pLogBuffer,LOG_LABEL[iMessage],MB_OK);

}
