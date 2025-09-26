// Z3DContainer.h: interface for the Z3DContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DCONTAINER_H__BB4492E1_BA15_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DCONTAINER_H__BB4492E1_BA15_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

#include <map>
#include <windows.h>
#include "SimpleParser.h"
#include "SimpleString.h"



typedef long Z3DHANDLE;
const Z3DHANDLE Z3DHANDLE_NULL = 0;
const Z3DHANDLE Z3DHANDLE_ERROR = 0xFFFFFFFF;



template<class _obj> struct Z3DContainer;

template<class _obj>
struct Z3DTag
{
friend struct Z3DContainer<_obj>;
	
public:
	Z3DTag()
	{
		m_rpObject = NULL;
		m_Handle = Z3DHANDLE_NULL;
	}

	_obj* GetObject()
	{
		_ASSERT( m_rpObject );
		return m_rpObject;
	}

	const Z3DHANDLE GetHandle()
	{
		return m_Handle;
	}

	bool operator == ( const Z3DTag<_obj> &op )
	{
		return (m_Handle == op.m_Handle);
	}

protected:
	_obj* m_rpObject;
	Z3DHANDLE m_Handle;
};


template<class _obj>
struct Z3DContainer  
{
public:
	Z3DContainer()
	{
		//m_szPathName = NULL;
	}
	~Z3DContainer();

	bool LoadHandleNameTable( const char* szFilename, const char* szPath = NULL );
	Z3DTag<_obj> GetObjectByHandle( Z3DHANDLE handle );
	bool Release( Z3DTag<_obj> tag );

	struct Z3DContainerElement
	{
		int lRefCount;
		_obj* pObj;

		Z3DContainerElement()
		{
			lRefCount = 0;
			pObj = NULL;
		}
	};

	// 디스크 긱긱거리는거 막을라꼬..처음에 한번 몽창 다 로딩--; (비효율적이지만 무식하게 구현;;)
	// 반드시 LoadHandleNameTable 호출 후 실행할것
	/*bool PreLoadAll()
		
	{
		std::map<Z3DHANDLE, char*>::iterator it;
		Z3DAniKeyPackTag tag;

		for( it = mapHandle2Name.begin(); it != mapHandle2Name.end(); it++ )
		{
			tag = GetObjectByHandle( it->first );
			if( NULL == tag.GetObject() )
			{
				return false;
			}
		}

		return true;
	}*/

protected:
	//char* m_szPathName;
	std::map<Z3DHANDLE, char*> mapHandle2Name;
	std::map<Z3DHANDLE, Z3DContainerElement> mapHandle2Element;
};


template<class _obj>
Z3DContainer<_obj>::~Z3DContainer()
{
	// delete handle-name table
	std::map<Z3DHANDLE, char*>::iterator itName;

	for( itName = mapHandle2Name.begin(); itName != mapHandle2Name.end(); itName++ )
	{
		SAFE_DELETEA( (*itName).second );
	}
	mapHandle2Name.clear();

	// delete path name
	//SAFE_DELETEA( m_szPathName );

	
	// delete handle-object element map, if any remains
	std::map<Z3DHANDLE, Z3DContainerElement>::iterator itElement;
	for( itElement = mapHandle2Element.begin(); itElement != mapHandle2Element.end(); itElement++ )
	{
		SAFE_DELETE( (*itElement).second.pObj );
	}
	mapHandle2Element.clear();
}


template<class _obj>
bool Z3DContainer<_obj>::LoadHandleNameTable( const char* szFilename, const char* szPath)
{
	CSimpleParser parser;
	SimpleString strPath;
	if( szPath )
	{
		strPath( szPath );
	}

	char* tmpStr;
	Z3DHANDLE handle;
	
	if( (parser.OpenFile( szFilename )) == false )
		return false;

	while( EOF != parser.ReadLine() )
	{
		if( parser.GetTokenNum() < 2 )
			continue;

		// path name line일 경우
		const char* rpPath = NULL;
		int n;
		if( stricmp( "path", parser.GetToken(0) ) == 0 )
		{
			rpPath = parser.GetToken(1);
			n = strlen(rpPath);

			if( rpPath[n-1] != '\\' && rpPath[n-1] != '/' )
			{
				strPath( rpPath, "/" );
			}
			else
			{
				strPath( rpPath );
			}

			continue;
		}

		// handle-name line 읽어들임
		handle = aton(parser.GetToken(0));

		tmpStr = new char[strlen(parser.GetToken(1)) + strPath.Length() + 1];
		if( tmpStr == NULL )
			return false;

		strcpy( tmpStr, strPath );
		strcat( tmpStr, parser.GetToken(1) );
		mapHandle2Name[handle] = tmpStr;
	}

	return true;
}


template<class _obj>
Z3DTag<_obj> Z3DContainer<_obj>::GetObjectByHandle( Z3DHANDLE handle )
{
	Z3DTag<_obj> tag;

	tag.m_Handle = handle;
	
	Z3DContainer<_obj>::Z3DContainerElement element = mapHandle2Element[handle];
	//if( Z3DContainerElement<_obj> == element )
	if( NULL == element.pObj ) // object not loaded yet
	{
		std::map<Z3DHANDLE, char*>::iterator itName;
		itName = mapHandle2Name.find( handle );
		if( itName == mapHandle2Name.end() )
		{
			mapHandle2Element.erase(handle); // delete unwanted item
			tag.m_rpObject = NULL;
			return tag; // error : invalid handle !
		}

		element.pObj = new _obj;
		if( false == element.pObj->Load( (*itName).second ) ) // _obj type should implement 'Load()' method
		{
			mapHandle2Element.erase(handle); // delete unwanted item
			tag.m_rpObject = NULL;
			return tag; // error : invalid file name-_-;
		}
		//element.lRefCount = 1; // no need to do
	}

	element.lRefCount++;
	mapHandle2Element[handle] = element;
	tag.m_rpObject = element.pObj;
	return tag;
}


template<class _obj>
bool Z3DContainer<_obj>::Release( Z3DTag<_obj> tag )
{
	//_ASSERT( tag.m_Handle );
	if( Z3DHANDLE_NULL == tag.m_Handle ) 
	{
		return false;
	}
	_ASSERT( tag.m_rpObject );	// check if tag is not-assigned or released

	std::map<Z3DHANDLE, Z3DContainerElement>::iterator itElement;

	itElement = mapHandle2Element.find( tag.m_Handle );
	if( itElement == mapHandle2Element.end() ) // object not loaded yet
	{
		return false; // invalid handle!
	}
	else
	{
		_ASSERT( tag.m_rpObject == (*itElement).second.pObj ); // check validity of handle-ptr. in tag

		(*itElement).second.lRefCount--;
		if( !(*itElement).second.lRefCount ) // ref' count reaches zero
		{
			SAFE_DELETE( (*itElement).second.pObj );
			mapHandle2Element.erase( itElement );
		}

		return true;
	}
}

#endif // !defined(AFX_Z3DCONTAINER_H__BB4492E1_BA15_11D4_AD2B_0000E8EB4C69__INCLUDED_)
