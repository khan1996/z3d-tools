// WBLogSystem.h: interface for the WBLogSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBLOGSYSTEM_H__3836C71D_3ABD_4A3D_99E8_385123E4F17C__INCLUDED_)
#define AFX_WBLOGSYSTEM_H__3836C71D_3ABD_4A3D_99E8_385123E4F17C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* *********************************************************************

  * CLogMgr
  * 파일 : LogMgr.h
  * 기능 : Caldron Engine내 메세지나 에러 등을 처리하는 Log System
  * 작성일 : 2003.10.23
  * 작성자 : wizardbug ( 2003.10.23)
  * 수정자 : 

********************************************************************** */
#include <time.h>
#include "../../include/Caldron.h"


//#define __LOG_WRITE__	// __LOG_WRITE__ 를 define 시켜주어야 Log 가 출력된다.

namespace Caldron 
{
		// 외부로 공개되는 Enum.
	enum LOG_MESSAGES 
	{
		LOG_MESSAGE,				// 일반 Log Message
		LOG_ERROR,					// Error Log Message
		LOG_NOHEADER,				// Header 없는 메세지.
		LOG_MESSAGES_NUM,		
	};

	enum LOG_OUTDEVICES 
	{ 
		LOG_FILEOUT,					// Log File
		LOG_CONSOLEOUT,				// Console 
		LOG_MESSAGEBOXOUT,				// Window Message Box
		LOG_OUTDEVICES_NUM,			
	};

	namespace Base 
	{

		const char * const LOG_LABEL[3] = { 
										" LOG[ MESSAGE ] | ",
										" LOG[ ERROR ] | ",
										""

									};
		const char LOG_FILENAME[] = "Caldron.log";

		class CLogMgr  
		{
		private:
			static CLogMgr				*ms_pInstance;
			FILE						*m_pLogFile;
			Caldron::LOG_OUTDEVICES		m_iOutDevice;
			
			char						m_pLogBuffer[2560];	// Log Buffer
			char						m_pLogFileName[256];

			bool						m_bBufferEmpty;
			CBitset						*m_pBitFlag;


		protected:
			CLogMgr();

			void						WriteBufferToFile(Caldron::LOG_MESSAGES);
			void						WriteBufferToConsole(Caldron::LOG_MESSAGES);
			void						WriteBufferToMessageBox(Caldron::LOG_MESSAGES);
			void						WriteFirstRunLog();	
		public:
			~CLogMgr();
			void						ValueRelease();

			static void					_SetOutDevice(Caldron::LOG_OUTDEVICES iDevice);
			static CLogMgr *			_GetInstance();
			static void					_LogMessage(const char *,...);
			static void					_LogError(const char *,...);
			static void					_LogNoHeader(const char *,...);

			static void					_SetOutMessageFlag(Caldron::LOG_MESSAGES iMessage,bool bFlag);
			static void					_Close();
			

		};
	};
};
#endif // !defined(AFX_WBLOGSYSTEM_H__3836C71D_3ABD_4A3D_99E8_385123E4F17C__INCLUDED_)
