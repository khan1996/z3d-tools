// Z3DTexture.h: interface for the Z3DTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DTEXTURE_H__DDBD9A61_EC01_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DTEXTURE_H__DDBD9A61_EC01_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc.h"
#include "SimpleString.h"
#include <d3d8.h>
#include <d3dx8.h>

#include <stdio.h>
#include <dds.h>

#include <Src/Base/LoadedObj.h>
#include <Src/Base/CaldronByteDataObj.h>


struct Z3DTextureCacheNode
{
	long lWidth;
	long lHeight;
	long lMipCount;
	D3DFORMAT format;
	IDirect3DTexture8* pTexture;

	Z3DTextureCacheNode* pNext;
};


struct Z3DTexture : public Caldron::Base::CLoadedObj
{
public:
	Z3DTexture()
	{
		m_pTexture = NULL;
	}

	~Z3DTexture();

	bool Create( int x, int y )
	{
		return SUCCEEDED( D3DXCreateTexture( ms_pDevice, x, y, 10, 0, D3DFMT_A1R5G5B5,
			D3DPOOL_DEFAULT, &m_pTexture ) );
		return true;
	}

//	bool Load( const char* szFileName, UINT width = 0, UINT height = 0 );
	virtual bool Load(Caldron::Base::CCaldronByteDataObj *pDataObj);
	virtual bool PostLoad();

	bool Save( const char* szFilename );

	bool Convert( const D3DFORMAT targetFormat );

	bool ReadDDSheader( const char* szFileName, DDS_HEADER* pHeader );
	//bool LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex,long numMips, FILE *fp);
	bool LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex,long numMips, Caldron::Base::CCaldronByteDataObj *pDataObj);

	bool IsEmpty()
	{
		return (NULL == m_pTexture);
	}

	void Flush()
	{
		SAFE_RELEASE( m_pTexture );
	}

	D3DFORMAT GetTextureFormat()
	{
		if( NULL == m_pTexture )
		{
			return D3DFMT_UNKNOWN;
		}
		
		D3DSURFACE_DESC sd;
		m_pTexture->GetLevelDesc( 0, &sd );

		return sd.Format;
	}

	IDirect3DTexture8* GetD3dTexture();
	

	
	static bool _Init( IDirect3DDevice8* pDevice, const char* szPath, long level = 0, long lMaxCacheCount = 20 );
	
	static void _Close();
	
	static long _GetDetailLevel()
	{
		return ms_lDetailLevel;
	}

	static IDirect3DTexture8* _GetD3DTextureInterface( long lWidth, long lHeight, long lMipCount, D3DFORMAT fmt );
	static void _ReleaseD3DTextureInterface( IDirect3DTexture8* pTexture );

	static void NullTextureMode(bool bNullTexture);
	
protected:
	IDirect3DTexture8* m_pTexture;

	static IDirect3DDevice8* ms_pDevice;
	//static char* ms_szDataPath;
	static SimpleString ms_strDataPath;

	static long ms_lDetailLevel;

	static IDirect3DTexture8* ms_pNullTexture;
	static bool ms_bNullTextureMode;

	// 캐시 리스트. SLL 이며 추가는 header, 삭제는 tail 에서 일어남
	static long ms_lMaxCacheCount;
	static long ms_lCacheCount;
	static Z3DTextureCacheNode* ms_pCacheChainHeader;
};

#endif // !defined(AFX_Z3DTEXTURE_H__DDBD9A61_EC01_11D4_AD2B_0000E8EB4C69__INCLUDED_)
