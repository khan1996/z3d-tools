// Z3DTexture.cpp: implementation of the Z3DTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DTexture.h"
#include <stdio.h>
#include <dds.h>
#include "GMMemory.h"


IDirect3DDevice8* Z3DTexture::ms_pDevice = NULL;
//char* Z3DTexture::ms_szDataPath = NULL;
SimpleString Z3DTexture::ms_strDataPath;
long Z3DTexture::ms_lDetailLevel = 0;

long Z3DTexture::ms_lMaxCacheCount;
long Z3DTexture::ms_lCacheCount;
Z3DTextureCacheNode* Z3DTexture::ms_pCacheChainHeader;

IDirect3DTexture8* Z3DTexture::ms_pNullTexture = NULL;
bool Z3DTexture::ms_bNullTextureMode = false;


bool Z3DTexture::_Init( IDirect3DDevice8* pDevice, const char* szPath, long level, long lMaxCacheCount )
{
	_ASSERT( pDevice );
	ms_pDevice = pDevice;
	ms_pDevice->AddRef();
	
	_ASSERT( szPath );
	int l = strlen( szPath );
	if( szPath[l-1] == '\\' || szPath[l-1] == '/' )
	{
		ms_strDataPath( szPath );
	}
	else
	{
		ms_strDataPath( szPath, "/" );
	}
	
	ms_lDetailLevel = level;

	ms_lMaxCacheCount = lMaxCacheCount;
	ms_lCacheCount = 0;
	ms_pCacheChainHeader = NULL;

	// create 1x1 texture
	if (FAILED(ms_pDevice->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &ms_pNullTexture)))
	{
		ms_pDevice->Release();
		return false;
	}
	D3DLOCKED_RECT lr;
	ms_pNullTexture->LockRect(0, &lr, NULL, 0);
	*((D3DCOLOR*)(lr.pBits)) = D3DCOLOR_ARGB(0xFF, 0xFF, 0xFF, 0xFF);
	ms_pNullTexture->UnlockRect(0);

	ms_bNullTextureMode = false;

	
	return true;
}

void Z3DTexture::_Close()
{
	Z3DTextureCacheNode* pChainNode = ms_pCacheChainHeader;
	Z3DTextureCacheNode* pTmp;

	while( pChainNode )
	{
		pTmp = pChainNode;
		pChainNode = pChainNode->pNext;
		SAFE_RELEASE( pTmp->pTexture );
		SAFE_DELETE( pTmp );
	}
	//SAFE_DELETE( ms_szDataPath );

	if (NULL != ms_pNullTexture)
	{
		ms_pNullTexture->Release();
		ms_pNullTexture = NULL;
	}

	if (NULL != ms_pDevice)
	{
		ms_pDevice->Release();
		ms_pDevice = NULL;
	}
}

IDirect3DTexture8* Z3DTexture::_GetD3DTextureInterface( long lWidth, long lHeight, long lMipCount, D3DFORMAT fmt )
{
	IDirect3DTexture8* pTexture = NULL;
	Z3DTextureCacheNode* pChainNode = ms_pCacheChainHeader;
	Z3DTextureCacheNode* pPrevNode = NULL;

	while( pChainNode )
	{
		if(	lWidth == pChainNode->lWidth &&
			lHeight == pChainNode->lHeight &&
			lMipCount == pChainNode->lMipCount &&
			fmt == pChainNode->format )
		{
			pTexture = pChainNode->pTexture;

			// 삭제될 노드의 앞-뒤 노드간 연결 설정
			if( pPrevNode )
			{
				pPrevNode->pNext = pChainNode->pNext;
			}
			
			// 삭제될 노드가 head일경우 head pointer 조정
			if( ms_pCacheChainHeader == pChainNode )
			{
				ms_pCacheChainHeader = pChainNode->pNext;
			}

			// 노드 삭제
			SAFE_DELETE( pChainNode );
			--ms_lCacheCount;

			break;
		}

		pPrevNode = pChainNode;
		pChainNode = pChainNode->pNext;
	}
	
	if( pTexture )
	{
		return pTexture;
	}

	if( FAILED(ms_pDevice->CreateTexture( lWidth, lHeight, lMipCount, 0, fmt, D3DPOOL_MANAGED, &pTexture )) )
	{
		return NULL;
	}

	return pTexture;
}

void Z3DTexture::_ReleaseD3DTextureInterface( IDirect3DTexture8* pTexture )
{
	if( NULL == pTexture )
	{
		return;
	}


	Z3DTextureCacheNode* pNode;
	Z3DTextureCacheNode* pPrev = NULL;

	// max cache count 가 0이거나 음수이면 캐시 상한선이 없다(무한증식-_-a)
	if( ms_lMaxCacheCount > 0 )
	{
		if( ms_lCacheCount == ms_lMaxCacheCount )
		{
			pNode = ms_pCacheChainHeader;

			while( pNode->pNext )
			{
				pPrev = pNode;
				pNode = pNode->pNext;
			}
			SAFE_RELEASE( pNode->pTexture );
			SAFE_DELETE( pNode );
			if( pPrev )
			{
				pPrev->pNext = NULL;
			}
			--ms_lCacheCount;
		}
	}

	pNode = new Z3DTextureCacheNode;
	D3DSURFACE_DESC d3dsd;
	pTexture->GetLevelDesc( 0, &d3dsd );
	pNode->lWidth = d3dsd.Width;
	pNode->lHeight = d3dsd.Height;
	pNode->format = d3dsd.Format;
	pNode->lMipCount = pTexture->GetLevelCount();
	if( 0 == pNode->lMipCount )
	{
		pNode->lMipCount = 1;
	}
	pNode->pTexture = pTexture;
	pNode->pNext = ms_pCacheChainHeader;
	ms_pCacheChainHeader = pNode;
	++ms_lCacheCount;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Z3DTexture::~Z3DTexture()
{
	/*static int cnt = 0;
	static char szFName[50];

	if( m_pTexture )
	{
		sprintf( szFName, "dump_%d.dds", cnt );
		cnt++;
		Save( szFName );
	}*/
	

	//SAFE_RELEASE( m_pTexture );
	_ReleaseD3DTextureInterface( m_pTexture );
}

bool Z3DTexture::ReadDDSheader( const char* szFileName, DDS_HEADER* pHeader )
{
	_ASSERT( pHeader );
	FILE* fp = NULL;

	if( (fp = fopen(szFileName, "rb")) == NULL )
	{
		SimpleString strFileName( ms_strDataPath, szFileName );
		
		if( (fp = fopen(strFileName, "rb")) == NULL )
		{
			return false;
		}
	}

	fseek(fp, 4, SEEK_SET);
	fread( pHeader, sizeof(DDS_HEADER), 1, fp );

	fclose(fp);

	return true;
}


bool Z3DTexture::LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex,long numMips, Caldron::Base::CCaldronByteDataObj *pDataObj)
//bool Z3DTexture::LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex,long numMips, FILE *fp)
{
	HRESULT hr;
    LPDIRECT3DSURFACE8 psurf;
    D3DSURFACE_DESC sd;    
    D3DLOCKED_RECT lr;
    LPDIRECT3DTEXTURE8 pmiptex = NULL;    
    DWORD dwBytesPerRow;

    pmiptex = (LPDIRECT3DTEXTURE8)ptex;   

	long iLevel;
    for (iLevel = 0; iLevel < numMips; iLevel++)
    {		
        if (pmiptex != NULL)
            hr = pmiptex->GetSurfaceLevel(iLevel, &psurf);
        if (FAILED(hr))
            return false;
        psurf->GetDesc(&sd);
        switch (sd.Format)
        {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5:
            dwBytesPerRow = 0; // magic value indicates texture's memory is contiguous
            break;
        case D3DFMT_A8R8G8B8:
            dwBytesPerRow = 4 * sd.Width;
            break;
        case D3DFMT_R8G8B8:
            dwBytesPerRow = 3 * sd.Width;
            break;
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_R5G6B5:
            dwBytesPerRow = 2 * sd.Width;
            break;
        default:
            return false;
        }
        
        if (pmiptex != NULL)
            hr = pmiptex->LockRect(iLevel, &lr, NULL, 0);
        if (FAILED(hr))
            return false;
        if (dwBytesPerRow == 0)
        {			
			//fread(lr.pBits,sd.Size,1,fp);
			pDataObj->Read( lr.pBits,sd.Size,1 );
		}
        else
        {
            DWORD yp;
            BYTE* pbDest = (BYTE*)lr.pBits;
            for (yp = 0; yp < sd.Height; yp++)
            {                
				//fread(pbDest,dwBytesPerRow,1,fp);
                pDataObj->Read( pbDest,dwBytesPerRow,1 );
				pbDest += lr.Pitch;
            }
        }

        if (pmiptex != NULL)
		{
            hr = pmiptex->UnlockRect(iLevel);
		}

        SAFE_RELEASE(psurf);
    }
    return true;
}
bool Z3DTexture::PostLoad()
{
	Caldron::Base::CCaldronByteDataObj *pDataObj = m_pByteDataObj;
	if(!pDataObj)
		return false;

	DDS_HEADER ddsh;
	
	if( 0 == pDataObj->GetByteSize() )
	{
		return false;
	}
	

	pDataObj->SetReadPos( 4 );
	pDataObj->Read( &ddsh, sizeof(DDS_HEADER), 1 );

    D3DFORMAT fmt;
    if (ddsh.ddspf.dwFourCC == D3DFMT_DXT1)
        fmt = D3DFMT_DXT1;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT2)
        fmt = D3DFMT_DXT2;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT3)
        fmt = D3DFMT_DXT3;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT4)
        fmt = D3DFMT_DXT4;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT5)
        fmt = D3DFMT_DXT5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 32 && ddsh.ddspf.dwABitMask == 0xff000000)
        fmt = D3DFMT_A8R8G8B8;
    else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 24)
        fmt = D3DFMT_R8G8B8;
    else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwGBitMask == 0x000007e0)
        fmt = D3DFMT_R5G6B5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x00008000)
        fmt = D3DFMT_A1R5G5B5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x0000f000)
        fmt = D3DFMT_A4R4G4B4;
    else
	{
        return false;
	}

	if( 0 == ddsh.dwMipMapCount )
	{
		ddsh.dwMipMapCount = 1;
	}
	if( NULL ==	( m_pTexture = _GetD3DTextureInterface(
					ddsh.dwWidth, ddsh.dwHeight, ddsh.dwMipMapCount, fmt ) ) )
	{
		return false;
	}

	if( false == (LoadAllMipSurfaces(m_pTexture, ddsh.dwMipMapCount, pDataObj)) )
	{
		return false;
	}
	m_bLoaded = true;

	return true;

}
//bool Z3DTexture::Load( const char* szFileName, UINT width, UINT height )
bool Z3DTexture::Load(Caldron::Base::CCaldronByteDataObj *pDataObj)
{

	return true;
}

/*
bool Z3DTexture::Load( const char* szFileName, UINT width, UINT height )
{
	FILE* fp = NULL;
	DDS_HEADER ddsh;

	if( (fp = fopen(szFileName, "rb")) == NULL )
	{
		SimpleString strFileName( ms_strDataPath, szFileName );
		
		if( (fp = fopen(strFileName, "rb")) == NULL )
		{
			return false;
		}
	}

	fseek(fp, 4, SEEK_SET);
	fread( &ddsh, sizeof(DDS_HEADER), 1, fp );

    D3DFORMAT fmt;
    if (ddsh.ddspf.dwFourCC == D3DFMT_DXT1)
        fmt = D3DFMT_DXT1;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT2)
        fmt = D3DFMT_DXT2;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT3)
        fmt = D3DFMT_DXT3;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT4)
        fmt = D3DFMT_DXT4;
    else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT5)
        fmt = D3DFMT_DXT5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 32 && ddsh.ddspf.dwABitMask == 0xff000000)
        fmt = D3DFMT_A8R8G8B8;
    else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 24)
        fmt = D3DFMT_R8G8B8;
    else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwGBitMask == 0x000007e0)
        fmt = D3DFMT_R5G6B5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x00008000)
        fmt = D3DFMT_A1R5G5B5;
    else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x0000f000)
        fmt = D3DFMT_A4R4G4B4;
    else
	{
		fclose( fp );
        return false;
	}

	if( 0 == ddsh.dwMipMapCount )
	{
		ddsh.dwMipMapCount = 1;
	}
	if( NULL ==	( m_pTexture = _GetD3DTextureInterface(
					ddsh.dwWidth, ddsh.dwHeight, ddsh.dwMipMapCount, fmt ) ) )
	{
		fclose( fp );
		return false;
	}

	if( false == (LoadAllMipSurfaces(m_pTexture, ddsh.dwMipMapCount, fp)) )
	{
		fclose( fp );
		return false;
	}

	fclose( fp );
	return true;
}
*/


bool Z3DTexture::Save( const char* szFilename )
{
	if( NULL == m_pTexture )
	{
		return false;
	}

	FILE* fp;
	if( (fp = fopen( szFilename, "wb" )) == NULL )
	{
		return false;
	}

    HRESULT hr;
    DDS_HEADER ddsh;
    DWORD dwMagic;
    D3DFORMAT fmt;
    DWORD dwSize;
    DWORD dwPitch = 0;
    D3DLOCKED_RECT lr;
	D3DSURFACE_DESC sd;
	DWORD dwLCount;
	

	m_pTexture->GetLevelDesc( 0, &sd );

    dwMagic = MAKEFOURCC('D','D','S',' ');
	fwrite( &dwMagic, sizeof(DWORD), 1, fp );

    ZeroMemory(&ddsh, sizeof(ddsh));
    ddsh.dwSize = sizeof(ddsh);
    ddsh.dwHeaderFlags = DDS_HEADER_FLAGS_TEXTURE;
    ddsh.dwWidth = sd.Width;
	ddsh.dwHeight = sd.Height;
    ddsh.dwSurfaceFlags = DDS_SURFACE_FLAGS_TEXTURE;
	dwLCount = m_pTexture->GetLevelCount();
	if ( dwLCount > 1)
	{
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_MIPMAP;
		ddsh.dwSurfaceFlags |= DDS_SURFACE_FLAGS_MIPMAP;
		ddsh.dwMipMapCount = dwLCount;
	}

	m_pTexture->GetLevelDesc(0, &sd);
	fmt = sd.Format;
	dwSize = sd.Size;
    if (SUCCEEDED(m_pTexture->LockRect(0, &lr, NULL, D3DLOCK_READONLY)))
	{
		dwPitch = lr.Pitch;
		m_pTexture->UnlockRect(0);
	}

    switch (fmt)
    {
	case D3DFMT_DXT1:
		ddsh.ddspf = DDSPF_DXT1;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_LINEARSIZE;
		ddsh.dwPitchOrLinearSize = dwSize;
		break;
	case D3DFMT_DXT2:
		ddsh.ddspf = DDSPF_DXT2;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_LINEARSIZE;
		ddsh.dwPitchOrLinearSize = dwSize;
		break;
	case D3DFMT_DXT3:
		ddsh.ddspf = DDSPF_DXT3;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_LINEARSIZE;
		ddsh.dwPitchOrLinearSize = dwSize;
		break;
	case D3DFMT_DXT4:
		ddsh.ddspf = DDSPF_DXT4;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_LINEARSIZE;
		ddsh.dwPitchOrLinearSize = dwSize;
		break;
	case D3DFMT_DXT5:
		ddsh.ddspf = DDSPF_DXT5;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_LINEARSIZE;
		ddsh.dwPitchOrLinearSize = dwSize;
		break;
	case D3DFMT_A8R8G8B8:
		ddsh.ddspf = DDSPF_A8R8G8B8;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
		ddsh.dwPitchOrLinearSize = dwPitch;
		break;
	case D3DFMT_A1R5G5B5:
		ddsh.ddspf = DDSPF_A1R5G5B5;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
		ddsh.dwPitchOrLinearSize = dwPitch;
		break;
	case D3DFMT_A4R4G4B4:
		ddsh.ddspf = DDSPF_A4R4G4B4;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
		ddsh.dwPitchOrLinearSize = dwPitch;
		break;
	case D3DFMT_R8G8B8:
		ddsh.ddspf = DDSPF_R8G8B8;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
		ddsh.dwPitchOrLinearSize = dwPitch;
		break;
	case D3DFMT_R5G6B5:
		ddsh.ddspf = DDSPF_R5G6B5;
		ddsh.dwHeaderFlags |= DDS_HEADER_FLAGS_PITCH;
		ddsh.dwPitchOrLinearSize = dwPitch;
		break;
	default:
		fclose( fp );
		return false;
    }

	fwrite( &ddsh, sizeof(DDS_HEADER), 1, fp );


	LPDIRECT3DSURFACE8 pSurf;
	DWORD i;

	for( i = 0; i < dwLCount; i++ )
	{
		hr = m_pTexture->GetSurfaceLevel(i, &pSurf);
		if( FAILED(hr) )
		{
			fclose( fp );
			return false;
		}

		pSurf->GetDesc(&sd);
		hr = m_pTexture->LockRect( i, &lr, NULL, 0 );
		if( FAILED(hr) )
		{
			fclose( fp );
			return false;
		}

		if (sd.Format == D3DFMT_DXT1 ||
			sd.Format == D3DFMT_DXT2 ||
			sd.Format == D3DFMT_DXT3 ||
			sd.Format == D3DFMT_DXT4 ||
			sd.Format == D3DFMT_DXT5)
		{
			fwrite( lr.pBits, sd.Size, 1, fp );
		}
		else
		{
			DWORD yp;
			BYTE* pbDest = (BYTE*)lr.pBits;
			LONG dataBytesPerRow = 0;
			if (sd.Format == D3DFMT_A8R8G8B8)
				dataBytesPerRow = 4 * sd.Width;
			else if (sd.Format == D3DFMT_R8G8B8)
				dataBytesPerRow = 3 * sd.Width;
			else
				dataBytesPerRow = 2 * sd.Width;
			for (yp = 0; yp < sd.Height; yp++)
			{
				fwrite( pbDest, dataBytesPerRow, 1, fp );
				pbDest += lr.Pitch;
			}
		}
		hr = m_pTexture->UnlockRect(i);
		pSurf->Release();
	}

	fclose( fp );
	return true;
}


bool Z3DTexture::Convert( const D3DFORMAT targetFormat )
{
	if( NULL == m_pTexture )
	{
		return false;
	}

	D3DSURFACE_DESC sd;

	m_pTexture->GetLevelDesc( 0, &sd );
	if( sd.Format == targetFormat )
	{
		return true;
	}

	int l = m_pTexture->GetLevelCount();

	IDirect3DTexture8* pTextureDest = NULL;
	if( FAILED( ms_pDevice->CreateTexture( sd.Width, sd.Width, l, 0, targetFormat,
							D3DPOOL_MANAGED, &pTextureDest ) ) )
	{
		return false;
	}

	IDirect3DSurface8* pSurfSrc = NULL;
	IDirect3DSurface8* pSurfDest = NULL;

	for( int i = 0; i < l; i++ )
	{
		m_pTexture->GetSurfaceLevel( i, &pSurfSrc );
		pTextureDest->GetSurfaceLevel( i, &pSurfDest );

		D3DXLoadSurfaceFromSurface( pSurfDest, NULL, NULL, pSurfSrc, NULL, NULL,
								D3DX_FILTER_TRIANGLE, 0 );

		SAFE_RELEASE( pSurfSrc );
		SAFE_RELEASE( pSurfDest );
	}

	SAFE_RELEASE( m_pTexture );
	m_pTexture = pTextureDest;

	return true;
}

IDirect3DTexture8* Z3DTexture::GetD3dTexture()
{
	if (ms_bNullTextureMode)
	{
		return ms_pNullTexture;
	}

	if (m_bLoaded)
	{
		return m_pTexture;
	}
	else
	{
		return NULL;
	}
}

void Z3DTexture::NullTextureMode(bool bNullTexture)
{
	ms_bNullTextureMode = bNullTexture;
}