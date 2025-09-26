// H3DContainer.h: interface for the CH3DContainer class template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H3DCONTAINER_H__EFFCAF0D_E084_4326_8D7C_BD0CAF16EB34__INCLUDED_)
#define AFX_H3DCONTAINER_H__EFFCAF0D_E084_4326_8D7C_BD0CAF16EB34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "FunctionPerformanceCheck.h"


#include <windows.h>
#include "misc.h"

//#include <io.h>


#include <map>
#include <vector>

#include "Z3DStringTable.h"



/*namespace ppuchuu{
	template<class _key_t, class _data_t>
	class map
	{
	public:
		struct _element_t
		{
			_element_t( _key_t _k, _data_t _d )
			{
				first = _k;
				second = _d;
			}

			_key_t first;
			_data_t second;
		};
		
		typedef std::vector<_element_t>::iterator iterator;

		map()
		{
		}

		~map()
		{
		}

		_data_t& operator []( _key_t &_k )
		{
			for(int i = 0; i < (int)m_vecActualData.size(); ++i )
			{
				if( _k == m_vecActualData[i].first )
				{
					return m_vecActualData[i].second;
				}
			}

			_element_t _e( _k, _data_t() );
			m_vecActualData.push_back( _e );
			return m_vecActualData[m_vecActualData.size()-1].second;
		}

		iterator find( _key_t &_k )
		{
			iterator it;
			for( it = m_vecActualData.begin(); it != m_vecActualData.end(); it++ )
			{
				if( _k == it->first )
				{
					return it;
				}
			}

			return it;
		}

		iterator begin()
		{
			return m_vecActualData.begin();
		}

		iterator end()
		{
			return m_vecActualData.end();
		}

		void erase( iterator it )
		{
			m_vecActualData.erase( it );
		}

	protected:
		std::vector<_element_t> m_vecActualData;

	};
}
*/


// UID under Win32 file system...
/*struct H3D_FILE_UID
{
public:
	H3D_FILE_UID( int i )
	{
		dwVolumeSerialNumber = 0UL;
		dwFileIndexHigh = 0UL;
		dwFileIndexLow = 0UL;
	}

	H3D_FILE_UID()
	{
	}

	bool operator == ( const H3D_FILE_UID &op ) const
	{
		return ( (dwVolumeSerialNumber == op.dwVolumeSerialNumber) &&
			(dwFileIndexHigh == op.dwFileIndexHigh) &&
			(dwFileIndexLow == op.dwFileIndexLow) );
	}

	bool operator < ( const H3D_FILE_UID &op ) const
	{
		if( dwVolumeSerialNumber < op.dwVolumeSerialNumber )
		{
			return true;
		}

		if( dwFileIndexHigh < op.dwFileIndexHigh )
		{
			return true;
		}

		if( dwFileIndexLow < op.dwFileIndexLow )
		{
			return true;
		}

		return false;
	}

	DWORD dwVolumeSerialNumber;
	DWORD dwFileIndexHigh;
	DWORD dwFileIndexLow;
};*/

template<class _obj> class CH3DContainer;

template<class _obj>
struct H3DContainerTag
{
	friend class CH3DContainer<_obj>;

public:
	H3DContainerTag()
	{
		m_rpObject = NULL;
		//m_UIDFile = H3D_FILE_UID(0);
		m_UIDFile = NULL_TOK;
		m_rpContainer = NULL;		
	}

	_obj* GetObject()
	{
		return m_rpObject;
	}

	bool Release()
	{
		bool bRet = true;

		if( m_rpContainer )
		{
			bRet = m_rpContainer->Release( m_UIDFile );
		}

		//m_UIDFile = H3D_FILE_UID(0);
		m_UIDFile = NULL_TOK;
		m_rpObject = NULL;
		m_rpContainer = NULL;

		return bRet;
	}

	bool operator == ( const H3DContainerTag<_obj> &op )
	{
		return ( m_UIDFile == op.m_UIDFile );
	}

protected:
	_obj* m_rpObject;
	//H3D_FILE_UID m_UIDFile;
	Z3DTOK m_UIDFile;
	CH3DContainer<_obj>* m_rpContainer;
};

template<class _obj>
class CH3DContainer
{
	friend struct H3DContainerTag<_obj>;

public:
	CH3DContainer()
	{
		m_szFilePath = NULL;
	}
	
	~CH3DContainer();

	void SetFilePath( const char* szPath );

	bool GetObject( H3DContainerTag<_obj> &tag, const char* szFileName );

	/*bool LoadHandleNameTable( const char* szFilename, const char* szPath = NULL )
	{
		// dummy function for intermediate compatibility
		SetFilePath( szPath );

		return true;
	}*/

	bool PreLoadAll( const char* szFileSpec );

	void FlushAll();

protected:
	struct H3DContainerElement
	{
		int lRefCount;
		_obj* pObj;

		H3DContainerElement()
		{
			lRefCount = 0;
			pObj = NULL;
		}
	};

	//bool Release( H3D_FILE_UID hf );
	bool Release( Z3DTOK hf );

	char* m_szFilePath;
	std::map<Z3DTOK, H3DContainerElement> m_mapContainerElement;
	//std::map<H3D_FILE_UID, H3DContainerElement> m_mapContainerElement;
	//ppuchuu::map<H3D_FILE_UID, H3DContainerElement> m_mapContainerElement;
};


template<class _obj>
void CH3DContainer<_obj>::SetFilePath( const char* szPath )
{
	long lLen = strlen(szPath);
	if( 0 == lLen )
	{
		return;
	}
		
	if( szPath[lLen-1] != '\\' && szPath[lLen-1] != '/' )
	{
		++lLen;
	}

	m_szFilePath = new char[lLen+1];
	strcpy( m_szFilePath, szPath );
	m_szFilePath[lLen-1] = '/';
	m_szFilePath[lLen] = '\0';
}


template<class _obj>
bool CH3DContainer<_obj>::GetObject( H3DContainerTag<_obj> &tag, const char* szFileName )
{
//DeclareBlockTimingCheck( "before CreateFile", aa );
//DeclareBlockTimingCheck( "CreateFile", bb );
//DeclareBlockTimingCheck( "GetFileInformationByHandle", cc );
//DeclareBlockTimingCheck( "CloseHandle", dd );


	if( NULL == szFileName )
	{
		//tag.m_UIDFile = H3D_FILE_UID(0);
		tag.m_UIDFile = NULL_TOK;
		tag.m_rpObject = NULL;
		tag.m_rpContainer = NULL;

		return false;
	}

	// 파일명으로부터 file UID 를 얻는다.
	//H3D_FILE_UID hfu;
	Z3DTOK hfu;

	static char szFullPathName[MAX_PATH];	// full path 작성
	szFullPathName[0] = '\0';
	if( m_szFilePath )
	{
		strcpy( szFullPathName, m_szFilePath );
	}
	strcat( szFullPathName, szFileName );

	/*HANDLE hFile = CreateFile( szFullPathName, GENERIC_READ, FILE_SHARE_READ,
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );*/
	// 파일의 존재와 무관하게 일단 스트링테이블에 파일명을 추가해놓는다.(등록된 것을 찾는게 아님)
	hfu = g_TokFileName.Add( szFullPathName );

	/*if( INVALID_HANDLE_VALUE == hFile )
	{
		//tag.m_UIDFile = H3D_FILE_UID(0);
		tag.m_UIDFile = NULL_TOK;
		tag.m_rpObject = NULL;
		tag.m_rpContainer = NULL;

		return false;
	}

	BY_HANDLE_FILE_INFORMATION hfi;
	if( 0 == GetFileInformationByHandle( hFile, &hfi ) )
	{
		CloseHandle( hFile );

		//tag.m_UIDFile = H3D_FILE_UID(0);
		tag.m_UIDFile = NULL_TOK;
		tag.m_rpObject = NULL;
		tag.m_rpContainer = NULL;

		return false;
	}

	hfu.dwVolumeSerialNumber = hfi.dwVolumeSerialNumber;
	hfu.dwFileIndexHigh = hfi.nFileIndexHigh;
	hfu.dwFileIndexLow = hfi.nFileIndexLow;
	CloseHandle( hFile );*/

	// map에서 file UID를 키로 엘레멘트를 얻어온다
	H3DContainerElement celem = m_mapContainerElement[hfu];
	if( NULL == celem.pObj )
	{
		// 엘레멘트의 오브젝트 포인터가 널 -> 데이터 아직 로딩되지 않은것이므로 로딩.
		celem.pObj = new _obj;
		if( false == celem.pObj->Load( szFullPathName ) )
		{
			// 파일 로딩 실패시
			//tag.m_UIDFile = H3D_FILE_UID(0);
			SAFE_DELETE( celem.pObj );
			
			tag.m_UIDFile = NULL_TOK;
			tag.m_rpObject = NULL;
			tag.m_rpContainer = NULL;

			return false;
		}
	}

	++celem.lRefCount;
	m_mapContainerElement[hfu] = celem;
	
	tag.m_rpObject = celem.pObj;
	tag.m_UIDFile = hfu;
	tag.m_rpContainer = this;

	return true;
}


template<class _obj>
//bool CH3DContainer<_obj>::Release( H3D_FILE_UID hf )
bool CH3DContainer<_obj>::Release( Z3DTOK hf )
{
	std::map<Z3DTOK, H3DContainerElement>::iterator it;
	//std::map<H3D_FILE_UID, H3DContainerElement>::iterator it;
	//ppuchuu::map<H3D_FILE_UID, H3DContainerElement>::iterator it;

	it = m_mapContainerElement.find( hf );
	if( m_mapContainerElement.end() == it )
	{
		return false;	// no proper handle
	}

	--(it->second.lRefCount);
	if( 0 == it->second.lRefCount )
	{
		SAFE_DELETE( it->second.pObj );
		m_mapContainerElement.erase( it );
	}

	return true;
}


template<class _obj>
CH3DContainer<_obj>::~CH3DContainer()
{
	SAFE_DELETEA( m_szFilePath );

	FlushAll();
}


template<class _obj>
void CH3DContainer<_obj>::FlushAll()
{
	std::map<Z3DTOK, H3DContainerElement>::iterator it;
	//std::map<H3D_FILE_UID, H3DContainerElement>::iterator it;
	//ppuchuu::map<H3D_FILE_UID, H3DContainerElement>::iterator it;
	
	for( it = m_mapContainerElement.begin(); it != m_mapContainerElement.end(); it++ )
	{
		SAFE_DELETE( it->second.pObj );
	}
}


template<class _obj>
bool CH3DContainer<_obj>::PreLoadAll( const char* szFileSpec )
{
	SimpleString strFullPath( m_szFilePath, szFileSpec );
	char* szFileNamePart;
	H3DContainerTag<_obj> tag;


	_finddata_t fileinfo;
	long lFindHandle = _findfirst( strFullPath, &fileinfo );

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

		GetObject( tag, szFileNamePart );

	} while( 0 == _findnext( lFindHandle, &fileinfo ) );

	_findclose( lFindHandle );

	return true;
}


#endif // !defined(AFX_H3DCONTAINER_H__EFFCAF0D_E084_4326_8D7C_BD0CAF16EB34__INCLUDED_)
