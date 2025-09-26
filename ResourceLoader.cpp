#include ".\resourceloader.h"
#include "GMMemory.h"
namespace CROSSM {

unsigned int __stdcall LoaderThreadProc(void *lpParam) 
{
	CResourceLoader *pLoader = (CResourceLoader *)(lpParam);
	pLoader->LoaderFunc();
	
	return 0;

}
unsigned int __stdcall ProcessThreadProc(void *lpParam) 
{
	CResourceLoader *pLoader = (CResourceLoader *)(lpParam);
	pLoader->ProcessFunc();
	return 0;

}

void CResourceLoader::StartLoader()
{
	if(!m_LoaderHandle)
	{

		if((m_LoaderHandle =(HANDLE)_beginthreadex(NULL,0,LoaderThreadProc,(LPVOID)this,0,&m_LoaderThreadID)) == NULL ) {
			//CLogMgr::_LogError("CResourceLoader::Loading() : Create Loader Thread Failed");
		}
	}
	if(!m_ProcessHandle)
	{
		if((m_ProcessHandle =(HANDLE)_beginthreadex(NULL,0,ProcessThreadProc,(LPVOID)this,0,&m_ProcessThreadID)) == NULL ) {
			//CLogMgr::_LogError("CResourceLoader::Loading() : Create Process Thread Failed");
		}


	}
}
void CResourceLoader::LoaderFunc()
{	
	CResourceObj *pObj = NULL;

	while( m_ReadyQueue.RemoveFront(pObj))
	{
		if(pObj)
		{
			pObj->Init();

			if(!(pObj->LoadByte())) 
			{
				pObj->SetLoaded(false);

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

	CResourceObj *pObj = NULL;
	while( m_Queue.RemoveFront(pObj))
	{
		if(pObj)
		{
			if(!pObj->Load())		
			{	// 로딩 실패시에 다시 queue에 넣어서 ReLoading 시도 한다.												
				if(pObj->Reloading() < MAX_RELOADING) {
					m_Queue.PushBack(pObj);
					//CLogMgr::_LogError("CResourceLoader::ProcessFunc: Load fail, Reloading.");
				}
				else
				{
					pObj->ReleaseByte();
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
		
		if(!m_ReadyQueue.GetQueueSize() && 
			!m_Queue.GetQueueSize())
		{
			
			break;
		}
		Sleep(2000);
	}
	if(m_ResultQueue.GetQueueSize())
	{
		int iProcess = m_ResultQueue.GetQueueSize(); 
		CResourceObj *pObj = NULL;

		while(iProcess)
		{
			if(m_ResultQueue.RemoveFront(pObj))
			{
				pObj->PostLoad();
				pObj->SetLoaded(true);
				pObj->ReleaseByte();
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
		CResourceObj *pObj = NULL;

		while(iProcess)
		{
			if(m_ResultQueue.RemoveFront(pObj))
			{
				pObj->PostLoad();
				pObj->SetLoaded(true);
				pObj->ReleaseByte();

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

// 2005.01.10 yundi ImmediateLoad 추가
void CResourceLoader::ImmediateLoad(CResourceObj *pObj)
{
	pObj->Init();

	if(!(pObj->LoadByte())) 
	{
		pObj->SetLoaded(false);
		return;
	}

	if(!pObj->Load())		
	{
		// 로딩 실패시 종료
		pObj->ReleaseByte();
		return;
	}

	pObj->PostLoad();
	pObj->SetLoaded(true);
	pObj->ReleaseByte();
}
}

