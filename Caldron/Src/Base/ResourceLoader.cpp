#pragma warning( disable : 4786 )

#include "../../include/Caldron.h"
#include "./LoadedObj.h"

#include "ResourceLoader.h"
#include "GMMemory.h"

namespace Caldron {namespace Base {
/* *********************************************************************  
  
  * CResourceLoaderObj  
  
  * 파일 : ResourceLoader.cpp  
  * 기능 : Caldron Engine내 CResourceLoader 에서 Load Queue에 들어갈 resource 단위 
  * 작성일 : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  
/*
CResourceLoaderObj::CResourceLoaderObj() 
{
	m_pObj = NULL;
	m_State = RLOBJSTATE_READY;
}
CResourceLoaderObj::CResourceLoaderObj(const char *strFileName,CLoadedObj *pObj) 
{
	strcpy(m_strFileName,strFileName);
	m_pObj = pObj;
	m_pObj->m_bLoaded = false;
	m_State = RLOBJSTATE_READY;
}
CResourceLoaderObj::~CResourceLoaderObj() 
{

}
*/
/* *********************************************************************  
  
  * CResourceLoader  
  
  * 파일 : ResourceLoader.cpp  
  * 기능 : Caldron Engine내 ResourceLoader, 
           thread를 분리하여 메린 루틴 과 로딩루틴을 분리 하였다.
  * 작성일 : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  
CThreadQueue<CLoadedObj *> CResourceLoader::m_ReadyQueue(g_iMaxLoaderQueue);

DWORD WINAPI LoaderThreadProc(LPVOID lpParam) 
{
	CResourceLoader *pLoader = (CResourceLoader *)(lpParam);
	pLoader->LoaderFunc();
	
	return 0;

}
DWORD WINAPI ProcessThreadProc(LPVOID lpParam) 
{
	CResourceLoader *pLoader = (CResourceLoader *)(lpParam);
	pLoader->ProcessFunc();
	return 0;

}

void CResourceLoader::StartLoader()
{
	if(!m_LoaderHandle)
	{
		if((m_LoaderHandle = CreateThread(NULL,0,LoaderThreadProc,(LPVOID)this,0,&m_LoaderThreadID)) == NULL) {
			//CLogMgr::_LogError("CResourceLoader::Loading() : Create Loader Thread Failed");
			
		}
	}
	if(!m_ProcessHandle)
	{
		if((m_ProcessHandle = CreateThread(NULL,0,ProcessThreadProc,(LPVOID)this,0,&m_ProcessThreadID)) == NULL) {
			//CLogMgr::_LogError("CResourceLoader::Loading() : Create Process Thread Failed");
		}

	}
	
}
void CResourceLoader::LoaderFunc()
{	
	CLoadedObj *pObj = NULL;
	while( m_ReadyQueue.RemoveFront(pObj))
	{
		if(pObj)
		{
			if(pObj->m_pByteDataObj)
				delete pObj->m_pByteDataObj;
			
			pObj->m_pByteDataObj = new CCaldronByteDataObj;

			if(!(pObj->m_pByteDataObj->LoadByte(pObj->m_strName))) 
			{
				pObj->m_bLoaded = false;
			}
			else 
			{
				m_Queue.PushBack(pObj);
			}

		}
	}

}
void CResourceLoader::ProcessFunc()
{

	CLoadedObj *pObj = NULL;
	while( m_Queue.RemoveFront(pObj))
	{
		if(pObj)
		{
			if(!pObj->Load(pObj->m_pByteDataObj))		
			{	// 로딩 실패시에 다시 queue에 넣어서 ReLoading 시도 한다.												
				if(pObj->m_iReloadingCount < Caldron::MAX_RELOADING) {
					pObj->m_bLoaded = false;
					m_Queue.PushBack(pObj);
					//CLogMgr::_LogError("CResourceLoader::ProcessFunc: Load fail, Reloading.");
					pObj->m_iReloadingCount++;	
				}
				else
				{
					pObj->m_bLoaded = false;
				}
			}
			else 
			{
				m_ResultQueue.PushBack(pObj);
			}

		}
		else
		{
			//CLogMgr::_LogError("CResourceLoader::ProcessFunc: Obj Ptr is NULL");
		}
	}

}
void CResourceLoader::ProcessAllData()
{
	while(1)
	{
		Sleep(2000);
		if(!m_ReadyQueue.GetQueueSize() && 
			!m_Queue.GetQueueSize())
		{
			
			break;
		}
	}
	if(m_ResultQueue.GetQueueSize())
	{
		int iProcess = m_ResultQueue.GetQueueSize(); 
		CLoadedObj *pObj = NULL;

		while(iProcess)
		{
			if(m_ResultQueue.RemoveFront(pObj))
			{
				pObj->PostLoad();
				SafeDelete(pObj->m_pByteDataObj);
				pObj->m_pByteDataObj = NULL;
			}
			else
			{
				break;
			}
			iProcess--;

		}
	}
}

void CResourceLoader::Process()
{
	if(m_ResultQueue.GetQueueSize())
	{
		int iProcess = (m_ResultQueue.GetQueueSize() > g_iMaxProcessNums) ? g_iMaxProcessNums : m_ResultQueue.GetQueueSize(); 
		CLoadedObj *pObj = NULL;

		while(iProcess)
		{
			if(m_ResultQueue.RemoveFront(pObj))
			{
				pObj->PostLoad();
				SafeDelete(pObj->m_pByteDataObj);
				pObj->m_pByteDataObj = NULL;
			}
			else
			{
				break;
			}
			iProcess--;

		}
	}
}
void CResourceLoader::Clear()
{
	ProcessAllData();
	m_ReadyQueue.ClearQueue();
	m_Queue.ClearQueue();
	m_ResultQueue.ClearQueue();

}
/*
class CResourceLoader
{
public:
	CResourceLoader();
	virtual ~CResourceLoader();
	void StartLoader();
	int AddObj(CResourceLoaderObj *);
protected:
	void LoaderFunc();
	void ProcessFunc();
	DWORD m_dwWait;
	
	CThreadQueue<CResourceLoaderObj *>m_lstObj;
	CThreadQueue<CResourceLoaderObj *>m_Queue;
	
	CCriticalSectionQueue<CLoadedObj *>m_ResultQueue;

	unsigned long					m_LoaderThreadID;
	unsigned long					m_ProcessThreadID;

	HANDLE							m_LoaderHandle;
	HANDLE							m_ProcessHandle;


};

CResourceLoader::CResourceLoader() : m_Queue(g_iMaxLoaderQueue)
{
	m_bLoading = false;

	m_dwWait = 0;
	m_lstObj.clear();
	m_lstReady.clear();
	InitializeCriticalSection(&m_ObjCriticalSection);
	m_LoaderThreadID = m_ProcessThreadID = 0;
	m_EndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_LoaderHandle = 0;
	m_ProcessHandle = 0;


}

CResourceLoader::~CResourceLoader()
{
	DeleteCriticalSection(&m_ObjCriticalSection);
	CloseHandle(m_EndEvent);
	CloseHandle(m_LoaderHandle);
	CloseHandle(m_ProcessHandle);
	
}
void CResourceLoader::Loading() 
{
	m_bLoading = true;
	// Event 초기화
	ResetEvent(m_EndEvent);
	if((m_LoaderHandle = CreateThread(NULL,0,LoaderThreadProc,(LPVOID)this,0,&m_LoaderThreadID)) == NULL) {
		CLogMgr::_LogError("CResourceLoader::Loading() : Create Loader Thread Failed");
		
	}
	if((m_ProcessHandle = CreateThread(NULL,0,ProcessThreadProc,(LPVOID)this,0,&m_ProcessThreadID)) == NULL) {
		CLogMgr::_LogError("CResourceLoader::Loading() : Create Process Thread Failed");
	}
}

DWORD CResourceLoader::WaitFinished() 
{
	return WaitForMultipleObjects(1, &m_EndEvent, FALSE, m_dwWait);
}
int CResourceLoader::FinishPercent() {
	return 0;
}
void CResourceLoader::LoaderFunc()
{
	for(int iIndex = 0; iIndex < GetObjNum(); iIndex++) 
	{
		CResourceLoaderObj *pObj = &m_lstObj[iIndex];
		if(!(pObj->m_ByteData.LoadByte(pObj->m_strFileName))) 
		{
			pObj->m_State = RLOBJSTATE_FAILED;
			if(pObj->m_pObj)
				pObj->m_pObj->m_bLoaded = false;
		}
		else 
		{
			pObj->m_State = RLOBJSTATE_LOADING;
			m_Queue.PushBack(pObj);
		}
	}
	m_Queue.Terminate();
}
void CResourceLoader::ProcessFunc()
{
	CResourceLoaderObj *pObj = NULL;
	while( m_Queue.RemoveFront(pObj))
	{
		if((pObj) && (pObj->m_pObj))
		{
			if(!pObj->m_pObj->Load(&(pObj->m_ByteData)))		
			{	// 로딩 실패시에 다시 queue에 넣어서 ReLoading 시도 한다.												
				if(pObj->m_pObj->m_iReloadingCount < Caldron::MAX_RELOADING) {
					pObj->m_State = RLOBJSTATE_LOADING;
					pObj->m_pObj->m_bLoaded = false;
					m_Queue.PushBack(pObj);
					CLogMgr::_LogError("CResourceLoader::ProcessFunc: Load fail, Reloading.");
					pObj->m_pObj->m_iReloadingCount++;	
				}
				else 
				{
					pObj->m_State = RLOBJSTATE_FAILED;
					pObj->m_pObj->m_bLoaded = true;

				}
			}
			else 
			{
				pObj->m_State = RLOBJSTATE_LOADED;
				pObj->m_pObj->m_bLoaded = true;
			}
		}
		else 
		{
			CLogMgr::_LogError("CResourceLoader::ProcessFunc: Obj Ptr is NULL");
		}
	}
	SetEvent(m_EndEvent);
}
int CResourceLoader::AddObj(CResourceLoaderObj &Obj)
{
	if(!m_bLoading) {
		EnterCriticalSection(&m_ObjCriticalSection);
		m_lstObj.push_back(Obj);
		LeaveCriticalSection(&m_ObjCriticalSection);
		return (int)(m_lstObj.size());
	}
	else {
		m_lstReady.push_back(Obj);
		return -2;
	}
}
CResourceLoaderObj CResourceLoader::GetObj(int iIndex) 
{
	EnterCriticalSection(&m_ObjCriticalSection);
	CResourceLoaderObj Obj = m_lstObj[iIndex];
	LeaveCriticalSection(&m_ObjCriticalSection);

	return Obj;
}
void CResourceLoader::SetObj(int iIndex,CResourceLoaderObj &Obj)
{
	EnterCriticalSection(&m_ObjCriticalSection);
	m_lstObj[iIndex] = Obj;
	LeaveCriticalSection(&m_ObjCriticalSection);

}
int CResourceLoader::GetObjNum()
{
	EnterCriticalSection(&m_ObjCriticalSection);
	int iNum = (int)m_lstObj.size();
	LeaveCriticalSection(&m_ObjCriticalSection);
	
	return iNum;
}
void CResourceLoader::ClearAllObj()
{
	EnterCriticalSection(&m_ObjCriticalSection);
	m_lstObj.clear();
	LeaveCriticalSection(&m_ObjCriticalSection);

}
CResourceLoaderObjState CResourceLoader::GetObjState(int iIndex)
{
	return m_lstObj[iIndex].m_State;
}
bool CResourceLoader::Finished()
{
	if(WaitFinished() != (WAIT_OBJECT_0))
		return false;
	CloseHandle(m_LoaderHandle);
	CloseHandle(m_ProcessHandle);
	m_LoaderHandle = m_ProcessHandle = 0;

	return true;

}
void CResourceLoader::Process() {
	if((m_bLoading == true) && WaitFinished() == (WAIT_OBJECT_0))
	{
		m_bLoading = false;
		ClearAllObj();
		CloseHandle(m_LoaderHandle);
		CloseHandle(m_ProcessHandle);
		m_LoaderHandle = m_ProcessHandle = 0;
	}
	else 
	{
		if((m_lstObj.size() > 0 )&& (m_bLoading == false))
			Loading();
		return;
	}
	if(m_lstReady.size() > 0 && !m_bLoading)
	{
		
		// Ready 큐에 들어 있는 것들을 로딩 리스트로 넘긴다	
		m_lstObj.reserve(m_lstReady.size());
		std::copy(m_lstReady.begin(),m_lstReady.end(),std::back_inserter(m_lstObj));

		m_lstReady.clear();
		Loading();
	}

}*/
}}