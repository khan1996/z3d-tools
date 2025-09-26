#pragma once
#include "./Z3DByteDataObj.h"

namespace CROSSM {
#define SG_RESOURCE_RELEASEPROCESS
class CResourceObj
{
public:
	CResourceObj(void) : m_bLoaded(false),m_iReloadingCount(0),m_pByteDataObj(0),m_iRefCount(0) {}
	CResourceObj(char *strName);
	
	void Init();
	bool LoadByte(const char *strName = NULL);
	void ReleaseByte();

	void SetLoaded(bool bState) { m_bLoaded = bState; }
	bool IsLoaded()	{ return m_bLoaded;}

	virtual ~CResourceObj(void);
	
	virtual bool Load() = 0;
	virtual bool PostLoad() = 0;
	virtual bool Unload() = 0;
	
	void AddReloadingCount() { m_iReloadingCount++;}
	int GetReloadingCount() { return m_iReloadingCount;}
	
	int Reloading() 
	{
		int iReturn = m_iReloadingCount++;
		m_bLoaded = false;
		return iReturn;
	}

	int AddRef() { return ++m_iRefCount; }
	int GetRef() { return m_iRefCount; }
	
	int Release();
	int RealRelease();

	void SetType(int iType)	{ m_iResourceType = iType; }
	int GetType() { return m_iResourceType; }

	char *GetName() { return m_strName;}


	char		m_strName[256];

protected:
	int m_iRefCount;

	
	bool		m_bLoaded;
	CZ3DByteDataObj *m_pByteDataObj;

	int			m_iReloadingCount;		// ReLoading È½¼ö.
	int			m_iResourceType;		// ResourceType


};

}
