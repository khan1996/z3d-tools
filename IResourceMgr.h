#pragma once

namespace CROSSM {
class IResourceMgr
{
public:

	IResourceMgr(void)
	{
	}

	virtual ~IResourceMgr(void)
	{
	}
	virtual void ReleaseAllData() = 0;
	virtual void ReleaseData(const char *strName) = 0;
	virtual void *GetData(const char *) = 0;
	virtual void Update() = 0;

	// 2005.01.10 yundi ImmediateLoad Ãß°¡
	virtual void LockImmediateLoad() = 0;
	virtual void UnlockImmediateLoad() = 0;

	void SetType(int iType)	{ m_iResourceType = iType; }
	int GetType() { return m_iResourceType; }


	protected:
		virtual void *LoadData(const char *) = 0;
		int	m_iResourceType;						// ResourceType


};

}