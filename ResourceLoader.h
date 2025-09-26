#pragma once
#include "./CrossMHeader.h"
#include "./ResourceObj.h"

namespace CROSSM {

enum SGTHREADEVENT_ID { 
	SGTHREAD_SEMAPHORE = 0,
	SGTHREAD_TERMINATE, 
};
template<class T>
class CThreadQueue  
{
public:
	CThreadQueue(int iMaxQueueSize) 
	{
		InitializeCriticalSection(&m_CriticalSection);
		m_Handle[SGTHREAD_SEMAPHORE] = CreateSemaphore(NULL,0,iMaxQueueSize,NULL);
		m_Handle[SGTHREAD_TERMINATE] = CreateEvent(NULL,TRUE,FALSE,NULL);
		
	}
	virtual ~CThreadQueue() 
	{
		ClearQueue();
		DeleteCriticalSection(&m_CriticalSection);
		CloseHandle(m_Handle[SGTHREAD_SEMAPHORE]);
		CloseHandle(m_Handle[SGTHREAD_TERMINATE]);	
	}
	bool PushBack(T Obj) 
	{
		EnterCriticalSection(&m_CriticalSection);
		m_Queue.push(Obj);
		
		// semaphore count 증가
		bool bResult = (ReleaseSemaphore(m_Handle[SGTHREAD_SEMAPHORE],1,NULL) != 0);
		if(!bResult) 
		{
			m_Queue.pop();		// pop()은 가장 마지막에 다룬것을 remove 시킨다.
//			//CLogMgr::_LogError("CThreadQueue::PushBack() : Queue is Full~!");
		}
		
		LeaveCriticalSection(&m_CriticalSection);
		return bResult;
	}
	void ClearQueue()
	{
		EnterCriticalSection(&m_CriticalSection);
		while(m_Queue.size())
		{
			m_Queue.pop();

		}
		LeaveCriticalSection(&m_CriticalSection);

	}
	bool RemoveFront(T &Obj) 
	{
		bool bResult = false;
		// Semarphore 에 쌓인 만큼 Return 값의 갯수가 들어간다
		SGTHREADEVENT_ID ReturnID = (SGTHREADEVENT_ID)WaitForMultipleObjects(2,m_Handle,FALSE,INFINITE);
		
		if(ReturnID == SGTHREAD_SEMAPHORE) 
		{
			bResult = true;
			EnterCriticalSection(&m_CriticalSection);
			if(m_Queue.size()) 
			{
				Obj = m_Queue.front();
				m_Queue.pop();			// pop()은 가장 마지막에 다룬것을 remove 시킨다.
			}
			else 
			{
				//CLogMgr::_LogError("CThreadQueue::RemoveFront() : Queue is Empty~!");
				bResult = false;
			}
			LeaveCriticalSection(&m_CriticalSection);
			return bResult;
		}
		return bResult;
	}
	void Terminate() 
	{
		SetEvent(m_Handle[SGTHREAD_TERMINATE]);
	}
	void UnTerminate() 
	{
		ResetEvent(m_Handle[SGTHREAD_TERMINATE]);
	}
	int GetQueueSize() 
	{ 
		return (int)m_Queue.size();
	}
protected:	

	HANDLE m_Handle[2];
	CRITICAL_SECTION m_CriticalSection;
	std::queue<T> m_Queue;

};
template<class T>
class CCriticalSectionQueue
{
public:
	CCriticalSectionQueue(int iMaxQueueSize) 
	{
		InitializeCriticalSection(&m_CriticalSection);
	}
	virtual ~CCriticalSectionQueue() 
	{
		ClearQueue();
		DeleteCriticalSection(&m_CriticalSection);
	}
	void PushBack(T Obj) 
	{
		EnterCriticalSection(&m_CriticalSection);
		m_Queue.push(Obj);
		LeaveCriticalSection(&m_CriticalSection);
	}
	bool RemoveFront(T &Obj) 
	{
		bool bReturn = false;
		EnterCriticalSection(&m_CriticalSection);
		if(m_Queue.size()) 
		{
			Obj = m_Queue.front();
			m_Queue.pop();			// pop()은 가장 마지막에 다룬것을 remove 시킨다.
			bReturn = true;
		}
		LeaveCriticalSection(&m_CriticalSection);
		return bReturn;

	}
	int GetQueueSize() 
	{ 
		return (int)m_Queue.size();
	}
	void ClearQueue()
	{
		EnterCriticalSection(&m_CriticalSection);
		while(m_Queue.size())
		{
			m_Queue.pop();

		}
		LeaveCriticalSection(&m_CriticalSection);

	}

protected:
	CRITICAL_SECTION m_CriticalSection;
	std::queue<T> m_Queue;

};

enum RESOURCELOADEROBJSTATE {
	RLOBJSTATE_READY = 0,
	RLOBJSTATE_LOADING,
	RLOBJSTATE_LOADED,
	RLOBJSTATE_FAILED,

};
const int g_iMaxLoaderQueue = 500;
const int g_iMaxResultQueue = 500;
const int g_iMaxProcessNums = 5;
const int MAX_RELOADING = 3;

class CResourceLoader
{
public:
	CResourceLoader() :m_ReadyQueue(g_iMaxLoaderQueue),m_Queue(g_iMaxLoaderQueue),m_ResultQueue(g_iMaxResultQueue),
						m_LoaderThreadID(0),m_ProcessThreadID(0),
						m_LoaderHandle(0),m_ProcessHandle(0)
						{}
	~CResourceLoader()
	{
		Clear();
		m_ReadyQueue.Terminate();
		m_Queue.Terminate();
		Sleep(3);	// Thread Terminate를 위한 sleep
		CloseHandle(m_LoaderHandle);
		CloseHandle(m_ProcessHandle);

	}
	void StartLoader();
	void AddObj(CResourceObj *pAddObj)
	{
		m_ReadyQueue.PushBack(pAddObj);
	}
	void LoaderFunc();
	void ProcessFunc();
	void Process();
	void ProcessAllData();

	void Clear();

	// 2005.01.10 yundi ImmediateLoad 추가
	void ImmediateLoad(CResourceObj *pObj);

	
protected:
	CThreadQueue<CResourceObj *>			m_ReadyQueue;
	CThreadQueue<CResourceObj *>			m_Queue;
	CCriticalSectionQueue<CResourceObj *>	m_ResultQueue;
	
	unsigned								m_LoaderThreadID;
	unsigned 								m_ProcessThreadID;
	
	HANDLE									m_LoaderHandle;
	HANDLE									m_ProcessHandle;

};
}

