#pragma once

#ifndef __CROSSM_HEADER_H__

#define __CROSSM_HEADER_H__

#pragma warning(disable:4786)
#pragma warning(disable:4251)
#pragma warning(disable:4503)

#include <windows.h>
#include <algorithm>
#include <tchar.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <list>
#include <math.h>
#include <process.h>
#include <mmsystem.h>
#include <time.h>


namespace CROSSM {
	// template function Á¤ÀÇ
	template<class _T>
	inline void SafeDelete( _T ptr )
	{
		if( NULL != ptr )
		{
			delete (ptr);
			ptr = NULL;
		}
	}

	template<class _T>
	inline void SafeDeleteA( _T ptr )
	{
		if( NULL != ptr )
		{
			delete[] (ptr);
			ptr = NULL;
		}
	}

	

	template<class _T>
	inline void SafeRelease( _T ptr )
	{
		if( NULL != ptr )
		{
			ptr->Release();
			ptr = NULL;
		}
	}
	inline unsigned long GetHashID(const char *strFileName)
	{
		unsigned long ulHashId = 0;

		int iLength = (int)strlen(strFileName);

		for(int i=0;i < iLength; i++) {
			ulHashId += (( i + 1) * strFileName[i]);
		}	
		return ulHashId;
	}




enum CROSSM_OBJ_TYPE {
	OBJ_POOL = 0,
	OBJ_RESOURCE,
	OBJ_TYPENUMS
};
enum CROSSM_POOLRESOURCE_TYPE{
	// Pool °´Ã¼
	POOL_MESSAGEOBJ	= 0,		// CSGMessage °´Ã¼.
//	POOL_BYTEDATAOBJ,			// CByteDataObj
//	POOL_RESOURCEMGROBJ,		// CResourceMgrObj
//	POOL_DELRESOURCEINFOOBJ,	// CDelResourceInfoObj
//	POOL_HOUSEOBJSCENE,			// HouseObjectScene.
//	POOL_HOUSEOBJ,				// HouseObj,
//	POOL_OBJECTSCENE,			// Object Scene
//	POOL_OCTREEOBJ,				// Octree Obj
//	POOL_POLYNODE,				// Poly Node
	// Resource °´Ã¼
	RESOURCE_TEXTURE,			// Texture
	RESOURCE_MESH,				// Mesh
	RESOURCE_OCTREESCENE,			// Octree

	POOLRESOURCE_TYPENUMS,
};
}
#endif
