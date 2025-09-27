// LoadedObj.cpp: implementation of the CLoadedObj class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )
#include <include/Caldron.h>
#include "./CaldronByteDataObj.h"
#include "LoadedObj.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace Caldron { namespace Base {
CLoadedObj::CLoadedObj(const char *strFileName) : m_bLoaded(false),m_iReloadingCount(0),m_pByteDataObj(0)
{
	strcpy(m_strName,strFileName);
}
CLoadedObj::~CLoadedObj()
{
	SafeDelete(m_pByteDataObj);
	m_pByteDataObj = 0;
}


}}