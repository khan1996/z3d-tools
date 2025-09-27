// ThreadQueue.h: interface for the CThreadQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADQUEUE_H__D6165646_A901_43B4_BABC_5237337D0BFD__INCLUDED_)
#define AFX_THREADQUEUE_H__D6165646_A901_43B4_BABC_5237337D0BFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../../include/Caldron.h"
//#include "./LogMgr.h"

/* *********************************************************************  
  
  * CThreadQueue  
  
  * 파일 : ThreadQueue.cpp  
  * 기능 :  
  * 작성일 : 2004.01.09  
  * history : 
      wizardbug ( 2004.01.09)  
  
********************************************************************** */  

namespace Caldron {
	namespace Base {

enum CALDRONTHREADEVENT_ID { 
	CALDRONTHREAD_SEMAPHORE = 0,
	CALDRONTHREAD_TERMINATE, 
};
template<class T>
class CThreadQueue  
{
public:
	CThreadQueue(int iMaxQueueSize) 
	{
		InitializeCriticalSection(&m_CriticalSection);
		m_Handle[CALDRONTHREAD_SEMAPHORE] = CreateSemaphore(NULL,0,iMaxQueueSize,NULL);
		m_Handle[CALDRONTHREAD_TERMINATE] = CreateEvent(NULL,TRUE,FALSE,NULL);
		
	}
	virtual ~CThreadQueue() 
	{
		ClearQueue();	
		DeleteCriticalSection(&m_CriticalSection);
		CloseHandle(m_Handle[CALDRONTHREAD_SEMAPHORE]);
		CloseHandle(m_Handle[CALDRONTHREAD_TERMINATE]);	
	}
	bool PushBack(T Obj) 
	{
		EnterCriticalSection(&m_CriticalSection);
		m_Queue.push(Obj);
		
		// semaphore count 증가
		bool bResult = (ReleaseSemaphore(m_Handle[CALDRONTHREAD_SEMAPHORE],1,NULL) != 0);
		if(!bResult) 
		{
			m_Queue.pop();		// pop()은 가장 마지막에 다룬것을 remove 시킨다.
			//CLogMgr::_LogError("CThreadQueue::PushBack() : Queue is Full~!");
			MessageBox(NULL,"Queue Full","ThreadQueue",MB_OK);	
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
		CALDRONTHREADEVENT_ID ReturnID = (CALDRONTHREADEVENT_ID)WaitForMultipleObjects(2,m_Handle,FALSE,INFINITE);
		
		if(ReturnID == CALDRONTHREAD_SEMAPHORE) 
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
		SetEvent(m_Handle[CALDRONTHREAD_TERMINATE]);
	}
	void UnTerminate() 
	{
		ResetEvent(m_Handle[CALDRONTHREAD_TERMINATE]);
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
	void ClearQueue()
	{
		EnterCriticalSection(&m_CriticalSection);
		while(m_Queue.size())
		{
			m_Queue.pop();

		}
		LeaveCriticalSection(&m_CriticalSection);

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
protected:
	CRITICAL_SECTION m_CriticalSection;
	std::queue<T> m_Queue;

};

	}
}
#endif // !defined(AFX_THREADQUEUE_H__D6165646_A901_43B4_BABC_5237337D0BFD__INCLUDED_)
