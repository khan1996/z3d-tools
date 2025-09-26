#pragma once
#include "ipoolmgr.h"
#include "./boost/pool/pool.hpp"
#include "./boost/pool/object_pool.hpp"

namespace CROSSM {
template <class T>
class CPoolMgr	: public IPoolMgr
{
public:
	CPoolMgr(void)
	{
	}
	virtual ~CPoolMgr(void)
	{
	}
	virtual void ReleaseAll()
	{
	}
	virtual void *GetObj()
	{
		T *pObj = m_BoostPool.construct();
		if(pObj == NULL)
			MessageBox(NULL,"tt","tt",MB_OK);
		return pObj;
	}
	virtual void ReleaseObj(void *pObj)
	{
		if(pObj)
		{
			m_BoostPool.destroy((T *)pObj);
		}
	}

protected:
	boost::object_pool<T>	m_BoostPool;

};

}
