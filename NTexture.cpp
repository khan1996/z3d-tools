#include ".\ntexture.h"
#include <d3dx8.h>
#include "./GraphicLayerError.h"
#include "GMMemory.h"

namespace CROSSM {

char CNTexture::m_strPath[256];
LPDIRECT3DDEVICE8 CNTexture::ms_pd3dDevice = NULL;
int CNTexture::m_SkipMipLevel=0;

IDirect3DTexture8* CNTexture::ms_pNullTexture = NULL;
bool CNTexture::ms_bNullTextureMode = false;


void CNTexture::Init(LPDIRECT3DDEVICE8	lpDevice)
{
	ms_pd3dDevice = lpDevice;
	ms_pd3dDevice->AddRef();

	// create 1x1 texture
	if (FAILED(ms_pd3dDevice->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &ms_pNullTexture)))
	{
		ms_pd3dDevice->Release();
	}
	D3DLOCKED_RECT lr;
	ms_pNullTexture->LockRect(0, &lr, NULL, 0);
	*((D3DCOLOR*)(lr.pBits)) = D3DCOLOR_ARGB(0xFF, 0xFF, 0xFF, 0xFF);
	ms_pNullTexture->UnlockRect(0);

	ms_bNullTextureMode = false;
}


void CNTexture::Close()
{
	if (NULL != ms_pNullTexture)
	{
		ms_pNullTexture->Release();
		ms_pNullTexture = NULL;
	}

	if (NULL != ms_pd3dDevice)
	{
		ms_pd3dDevice->Release();
	}
}


CNTexture::CNTexture(void) :m_pddTexture(NULL), m_bInterface(false)
{
	m_dwWidth = m_dwHeight = 0;
	m_bCreateEmpty = false;

}

CNTexture::~CNTexture(void)
{
	if(m_pddTexture)
	{
		SafeRelease(m_pddTexture);
		m_pddTexture =NULL;
	}
}
bool CNTexture::Load()
{
	DDS_HEADER ddsh;

	if(strstr(m_strPath,"Interface")==0)
	{
		m_bInterface = false;
	}
	else
	{
		m_bInterface = true;	
	}

	if(m_bInterface)
	{
		if(!m_pByteDataObj)
		{
			return false;
		}
		// magic header(ignore)
		m_pByteDataObj->Read(&m_dwValue[0],sizeof(DWORD),1);
		// DDS HEADER
		m_pByteDataObj->Read(&ddsh,sizeof(ddsh),1);
		
		m_dwWidth=ddsh.dwWidth;
		m_dwHeight=ddsh.dwHeight;
		// num mip
		m_dwValue[0] = (ddsh.dwMipMapCount == 0) ? 1 : ddsh.dwMipMapCount;
		// depth	
		m_dwValue[1] = (ddsh.dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) ? ddsh.dwDepth : 0;
		
		if (ddsh.ddspf.dwFourCC == D3DFMT_DXT1)
			m_fmt = D3DFMT_DXT1;
		else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT2)
			m_fmt = D3DFMT_DXT2;
		else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT3)
			m_fmt = D3DFMT_DXT3;
		else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT4)
			m_fmt = D3DFMT_DXT4;
		else if (ddsh.ddspf.dwFourCC == D3DFMT_DXT5)
			m_fmt = D3DFMT_DXT5;
		else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 32 && ddsh.ddspf.dwABitMask == 0xff000000)
			m_fmt = D3DFMT_A8R8G8B8;
		else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 24)
			m_fmt = D3DFMT_R8G8B8;
		else if (ddsh.ddspf.dwFlags == DDS_RGB  && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwGBitMask == 0x000007e0)
			m_fmt = D3DFMT_R5G6B5;
		else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x00008000)
			m_fmt = D3DFMT_A1R5G5B5;
		else if (ddsh.ddspf.dwFlags == DDS_RGBA && ddsh.ddspf.dwRGBBitCount == 16 && ddsh.ddspf.dwABitMask == 0x0000f000)
			m_fmt = D3DFMT_A4R4G4B4;
		else
			return false;



	}

	return true;

}
bool CNTexture::Unload()
{
	return true;

}

bool CNTexture::PostLoad()
{
	if(!m_pByteDataObj)
		return false;

	LPDIRECT3DTEXTURE8 pmiptex = NULL;

	if(!m_bInterface)
	{
		DWORD dwMagic=MAKEFOURCC('D','D','S',' ');
		DWORD *pHeader = NULL;
		pHeader = (DWORD *)m_pByteDataObj->GetReadPtr();
		
		if(pHeader == NULL)
			return false;

		*pHeader = dwMagic;

		HRESULT hr=D3DXCreateTextureFromFileInMemory(ms_pd3dDevice,pHeader,m_pByteDataObj->GetByteSize(),&pmiptex);
		if(hr != D3D_OK)
		{
			printf("");
		}
		if(pmiptex == NULL)
		{
			printf("");
		}
			
	}
	else
	{
		if (FAILED(ms_pd3dDevice->CreateTexture(m_dwWidth>>m_SkipMipLevel, m_dwHeight>>m_SkipMipLevel, m_dwValue[0]-m_SkipMipLevel, 0, m_fmt, D3DPOOL_MANAGED, &pmiptex)))    
		{
			CGraphicLayerError("CTexture:ReadDDSTexture , CreateTexture is failed");		
		}
		if (FAILED(LoadAllMipSurfaces(pmiptex, m_dwValue[0])))
		{
			MessageBox(NULL,"LoadAllMipSurface Load Failed",0,0);
			CGraphicLayerError("CTexture:ReadDDSTexture , LoadAllMipSurfaces is failed");
		}

	}
	
	m_pddTexture = pmiptex;

	return true;

}
HRESULT CNTexture::LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex, long numMips)
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
            hr = pmiptex->GetSurfaceLevel(iLevel-m_SkipMipLevel, &psurf);
        if (FAILED(hr))
            return hr;
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
            return E_FAIL;
        }
        
        if (pmiptex != NULL)
            hr = pmiptex->LockRect(iLevel, &lr, NULL, 0);
        if (FAILED(hr))
            return hr;
        if (dwBytesPerRow == 0)
        {			
			//fread(lr.pBits,sd.Size,1,fp);
			m_pByteDataObj->Read(lr.pBits,sd.Size,1);

		}
        else
        {
            DWORD yp;
            BYTE* pbDest = (BYTE*)lr.pBits;
            for (yp = 0; yp < sd.Height; yp++)
            {                
				//fread(pbDest,dwBytesPerRow,1,fp);
				m_pByteDataObj->Read(pbDest,dwBytesPerRow,1);

				pbDest += lr.Pitch;
            }
        }

        if (pmiptex != NULL)
            hr = pmiptex->UnlockRect(iLevel);
        ReleasePpo(&psurf);
    }
    return S_OK;


}

LPDIRECT3DBASETEXTURE8 CNTexture::GetTexture()
{
	if (ms_bNullTextureMode)
	{
		return ms_pNullTexture;
	}

	return m_pddTexture;
}


void CNTexture::NullTextureMode(bool bNullTexture)
{
	ms_bNullTextureMode = bNullTexture;
}

}