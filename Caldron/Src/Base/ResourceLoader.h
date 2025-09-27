// ResourceLoader.h: interface for the CResourceLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCELOADER_H__16578E31_424F_4D74_9304_44BEC2BE6F95__INCLUDED_)
#define AFX_RESOURCELOADER_H__16578E31_424F_4D74_9304_44BEC2BE6F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./LoadedObj.h"
#include "./CaldronByteDataObj.h"
#include "./ThreadQueue.h"



namespace Caldron {
	namespace Base {
/* *********************************************************************  
  
  * CResourceLoaderObj  
  
  * ���� : ResourceLoader.h  
  * ��� : Caldron Engine�� CResourceLoader ���� Load Queue�� �� resource ���� 
  * �ۼ��� : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  

enum CResourceLoaderObjState {
	RLOBJSTATE_READY = 0,
	RLOBJSTATE_LOADING,
	RLOBJSTATE_LOADED,
	RLOBJSTATE_FAILED,

};
const int g_iMaxLoaderQueue = 100000;
const int g_iMaxResultQueue = 200000;
const int g_iMaxProcessNums = 10;

/*
class CResourceLoaderObj {
public:

	CResourceLoaderObj();
	CResourceLoaderObj(const char *strFileName,CLoadedObj *pObj);

	virtual ~CResourceLoaderObj();
	
	CResourceLoaderObjState m_State;
//	CByteDataObj			m_ByteData;
	char					m_strFileName[40];
	CLoadedObj				*m_pObj;

};
*/
/* *********************************************************************  
  
  * CResourceLoader  
  
  * ���� : ResourceLoader.h  
  * ��� : Caldron Engine�� ResourceLoader 
  * �ۼ��� : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  

class CResourceLoader
{
protected:
	
	static CThreadQueue<CLoadedObj *>m_ReadyQueue;
	CThreadQueue<CLoadedObj *>m_Queue;
	
	CCriticalSectionQueue<CLoadedObj *>m_ResultQueue;

	unsigned long					m_LoaderThreadID;
	unsigned long					m_ProcessThreadID;

	HANDLE							m_LoaderHandle;
	HANDLE							m_ProcessHandle;

public:
	CResourceLoader() :/*m_ReadyQueue(g_iMaxLoaderQueue),*/m_Queue(g_iMaxLoaderQueue),m_ResultQueue(g_iMaxResultQueue),
						m_LoaderThreadID(0),m_ProcessThreadID(0),
						m_LoaderHandle(0),m_ProcessHandle(0)
						{}
	virtual ~CResourceLoader()
	{
		Clear();
		CloseHandle(m_LoaderHandle);
		CloseHandle(m_ProcessHandle);

	}
	void StartLoader();
	static void AddObj(CLoadedObj *pAddObj)
	{
		m_ReadyQueue.PushBack(pAddObj);
	}
	void LoaderFunc();
	void ProcessFunc();
	void Process();
	void ProcessAllData();

	void Clear();


};
/*
class CResourceLoader  
{
public:
	CResourceLoader();
	virtual ~CResourceLoader();
	int	 FinishPercent();
	// ���� queue�ȿ� ����ִ� obj ���� �ε� ���� ��ƾ
	void Loading();
		
	void ClearAllObj();
	int AddObj(CResourceLoaderObj &);
	void SetObj(int iIndex,CResourceLoaderObj &);

	int	 GetObjNum();
	CResourceLoaderObj GetObj(int iIndex);
	// ���� �ε�����	
	CResourceLoaderObjState GetObjState(int iIndex);
	
	DWORD WaitFinished();
	// ���μ��� �����忡�� ȣ��
	void ProcessFunc();
	// �δ� �����忡�� ȣ��
	void LoaderFunc();
	bool Finished();
	void Process();

protected:
	// Loading Thread ȣ��� ��ٸ��� �ð�
	// 0 : ��ٷ� return, INFINIT : Loading ������ ���� ��ٸ�. (default : 0)
	DWORD							m_dwWait;		
	std::vector<CResourceLoaderObj> m_lstObj;
	std::vector<CResourceLoaderObj> m_lstReady;

	CThreadQueue<CResourceLoaderObj *>m_Queue;
	
	CRITICAL_SECTION				m_ObjCriticalSection;
	
	unsigned long					m_LoaderThreadID;
	unsigned long					m_ProcessThreadID;

	HANDLE							m_EndEvent;
	HANDLE							m_LoaderHandle;
	HANDLE							m_ProcessHandle;

	bool							m_bLoading;
};
*/
}}

#endif // !defined(AFX_RESOURCELOADER_H__16578E31_424F_4D74_9304_44BEC2BE6F95__INCLUDED_)
