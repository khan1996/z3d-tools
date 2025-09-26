#pragma once
#include "./CrossMHeader.h"
#include "./IResourceMgr.h"

namespace CROSSM {
			
typedef std::map<int , IResourceMgr *> RESOURCETABLE;
typedef RESOURCETABLE::value_type RESOURCETABLEOBJ;
typedef RESOURCETABLE::iterator RESOURCETABLEITER;

class CResource
{
public:
	CResource(void);
	~CResource(void);
	void Init();

	void AddResourceMgr(int ,IResourceMgr *);
	void ReleaseAllData();
	void Release();
	void ReleaseObj(int iType,char *pObj);

	void *GetData(int iType,const char *);
	void Update();

	void LockImmediateLoad(int iType);
	void UnlockImmediateLoad(int iType);


protected:
	RESOURCETABLE	m_ResourceMgrMap;


};

}
