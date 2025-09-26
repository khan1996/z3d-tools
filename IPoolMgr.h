#pragma once
namespace CROSSM {
class IPoolMgr
{
public:

	IPoolMgr(void)
	{
	}

	virtual ~IPoolMgr(void)
	{
	}
	
	virtual void ReleaseAll() = 0;
	virtual void *GetObj() = 0;
	virtual void ReleaseObj(void *pObj) = 0;


};

}
