// LoadedObj.h: interface for the CLoadedObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADEDOBJ_H__7BBE3A5F_6665_4F40_AA4D_82BACEBC32D3__INCLUDED_)
#define AFX_LOADEDOBJ_H__7BBE3A5F_6665_4F40_AA4D_82BACEBC32D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./CaldronByteDataObj.h"


namespace Caldron {
	namespace Base {

/* *********************************************************************  
  
  * CLoadedObj  
  
  * ���� : LoadedObj.h  
  * ��� : Caldron Engine�� CResourceMgr �� �̿��� �ε� ������ ���ҽ� obj ���� ��� class 
  * �ۼ��� : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  

class CLoadedObj  
{
public:
	CLoadedObj() : m_bLoaded(false),m_iReloadingCount(0),m_pByteDataObj(0),m_bFirst(0),m_iReferenceCount(0)
	{}
	CLoadedObj(const char *strFileName);
	virtual ~CLoadedObj();
	virtual bool Load(CCaldronByteDataObj *) = 0;
	virtual bool PostLoad() = 0;
	//virtual bool Unload() = 0;
	bool		m_bLoaded;
	bool		m_bFirst;
	char		m_strName[256];
	int			m_iReloadingCount;		// ReLoading Ƚ��.
	unsigned int m_iReferenceCount;
	CCaldronByteDataObj *m_pByteDataObj;


};


	}
}
#endif // !defined(AFX_LOADEDOBJ_H__7BBE3A5F_6665_4F40_AA4D_82BACEBC32D3__INCLUDED_)
