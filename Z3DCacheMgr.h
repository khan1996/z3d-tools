// Z3DCacheMgr.h: interface for the CZ3DCacheMgr template class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DCACHEMGR_H__F8C9C08C_D78A_458C_9BD6_07D61E2A7A0B__INCLUDED_)
#define AFX_Z3DCACHEMGR_H__F8C9C08C_D78A_458C_9BD6_07D61E2A7A0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include <io.h>

#include <Src/Base/BaseCacheMgr.h>

#include <string>

template< class _obj, class _base_class >
class CZ3DCacheMgr  : public _base_class//CBaseCacheMgr<_obj>
{
public:
	CZ3DCacheMgr()
	{
	}

	virtual ~CZ3DCacheMgr()
	{
	}

	void SetFilePath( const char* szPath );

	const std::string& GetFilePath()
	{
		return m_strFilePath;
	}

	_obj* GetObject( const char* szFileName );
	

	bool PreLoadAll( const char* szFileSpec );


	void FlushAll()
	{
		if(m_iCachesNum > 0) 
		{
			for(std::map<unsigned long , Caldron::Base::CCacheObj *>::iterator Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
			{
				if(Itr->second != NULL) 
				{
					if(Itr->second)
					{
						
						delete (Itr->second);
					}
					Itr->second = NULL;
					
				}
				
				Itr++;
			}
			m_CacheTable.clear();
			m_iCachesNum = 0;
			
		}
	}
	

protected:
	std::string m_strFilePath;

};


template< class _obj, class _base_class >
void CZ3DCacheMgr< _obj, _base_class >::SetFilePath( const char* szPath )
{
	m_strFilePath = szPath;

	int nLen = m_strFilePath.length();
	if( 0 == nLen )
	{
		return;
	}

	
	if( m_strFilePath[nLen-1] != '\\' && m_strFilePath[nLen-1] != '/' )
	{
		m_strFilePath += "/";
	}
}



template< class _obj, class _base_class >
_obj* CZ3DCacheMgr< _obj, _base_class >::GetObject( const char* szFileName )
{
	std::string strTemp;

	strTemp = m_strFilePath;
	strTemp += szFileName;

	return GetData( strTemp.c_str() );
}



template< class _obj, class _base_class >
bool CZ3DCacheMgr< _obj, _base_class >::PreLoadAll( const char* szFileSpec )
{
	std::string strFullPath = m_strFilePath + szFileSpec;
	char* szFileNamePart;
	
	
	_finddata_t fileinfo;
	long lFindHandle = _findfirst( strFullPath.c_str(), &fileinfo );
	
	if( -1 == lFindHandle )
	{
		return false;
	}
	
	do
	{
		szFileNamePart = strrchr( fileinfo.name, '\\' );
		if( NULL == szFileNamePart )
		{
			szFileNamePart = fileinfo.name;
		}
		else
		{
			++szFileNamePart;
		}
		
		GetObject( szFileNamePart );
		
	} while( 0 == _findnext( lFindHandle, &fileinfo ) );
	
	_findclose( lFindHandle );
	
	return true;
}




#endif // !defined(AFX_Z3DCACHEMGR_H__F8C9C08C_D78A_458C_9BD6_07D61E2A7A0B__INCLUDED_)
