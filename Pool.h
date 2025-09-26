#pragma once

#include <map>

#include "./CrossMHeader.h"
#include "./IPoolMgr.h"

namespace CROSSM {

typedef std::map<int ,IPoolMgr *> POOLTABLE;
typedef POOLTABLE::iterator	POOLTABLEITER;
typedef POOLTABLE::value_type POOLTABLEOBJ;


class CPool
{
public:
	CPool(void);
	~CPool(void);
	void Init();
	
	void Release();
	void ReleaseObj(int iType,void *pObj);
	void ReleaseAllData();
	void *GetObj(int iType);

	void AddPool(int iType,IPoolMgr *pMgr);

protected:
	POOLTABLE	m_PoolMap;


};

}