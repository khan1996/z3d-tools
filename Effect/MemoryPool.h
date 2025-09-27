#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#include <cstdlib>
#include <new>

#define MEMPOOL_DBGMODE

#define ALLOC(nSize)			Allocate((nSize))
#define FREE(pDelete, nSize)	Deallocate((pDelete), (nSize))

#define DECLARE_ALLOCATOR	void* Allocate(size_t nSize)
#define DECLARE_DEALLOCATOR void  Deallocate(void* pDelete, size_t nSize)

#define DEFINE_ALLOCATOR(className)   void* className::Allocate(size_t nSize)
#define DEFINE_DEALLOCATOR(className) void  className::Deallocate(void* pDelete, size_t nSize)


class CNullLock
{
public:

	class Syncronize
	{
	public:
		Syncronize(CNullLock& nullLock) { }		
	};

	void Lock() { }
	void Unlock() { }
};

class CFixedPool
{
public:

	enum { MAX_POOL_NAME = 32 };

	CFixedPool();
	CFixedPool(size_t nPerAllocateSize, size_t nPerAllocateNum,
		const char* strMaxPoolName, size_t nNameLen);

	~CFixedPool();

	bool Initialize(size_t nPerAllocateSize, size_t nPerAllocateNum,
		const char* strMaxPoolName, size_t nNameLen);

	void Destroy();
    unsigned long GetTotalUse() const { return static_cast<unsigned long>(m_nTotalInUse); }

	static size_t GetLeastAllocateSize();
	static const char* GetErrFileName();

	size_t	m_nTotalInUse;

	DECLARE_ALLOCATOR;
	DECLARE_DEALLOCATOR;

private:

	bool AllocateChunks();
    void LogBufferOverflow(const char* lpBuffer);

	typedef CNullLock PoolLock;
	struct ChunkNode;
	struct AllocateInfo;
	
	PoolLock		m_PoolLock;

	ChunkNode*		m_pChunkHead;
	AllocateInfo*	m_pNodeHead;

	size_t	m_nOriginalSize;
	size_t	m_nPerAllocateSize;
	size_t	m_nPerAllocateNum;

	size_t	m_nTotalAllocated;
	

	char	m_pszPoolName[MAX_POOL_NAME];

	static const char* const ms_strErrLogFileName;
};


class CFixedPoolTest
{
public:
	void DoTest();
};


#endif