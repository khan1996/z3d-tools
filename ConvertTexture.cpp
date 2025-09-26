// ConvertTexture.cpp: implementation of the CConvertTexture class.
//
//////////////////////////////////////////////////////////////////////

#include <d3dx8.h>
#include "ConvertTexture.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvertTexture::CConvertTexture()
{
	m_pddNewTexture=NULL;
}

CConvertTexture::~CConvertTexture()
{
	if(m_pddNewTexture)
		m_pddNewTexture->Release();
}

void CConvertTexture::Compress(D3DFORMAT fmtTo)
{
	if(m_pddNewTexture)
	{
		m_pddNewTexture->Release();
		m_pddNewTexture=NULL;
	}
	
	int nMips=m_pddTexture->GetLevelCount();
	//ms_pd3dDevice->CreateTexture(m_dwWidth,m_dwHeight,0,0,fmtTo,D3DPOOL_MANAGED,&m_pddNewTexture);    
	ms_pd3dDevice->CreateTexture(m_dwWidth,m_dwHeight,nMips,0,fmtTo,D3DPOOL_MANAGED,&m_pddNewTexture);    
    //int nMips=m_pddNewTexture->GetLevelCount();

	LPDIRECT3DSURFACE8 pSurfaceSrc,pSurfaceDest;

	for(int cLevel=0;cLevel<nMips;cLevel++)
	{
		HRESULT r;
		r=((LPDIRECT3DTEXTURE8)m_pddTexture)->GetSurfaceLevel(cLevel,&pSurfaceSrc);
		r=m_pddNewTexture->GetSurfaceLevel(cLevel,&pSurfaceDest);
		r=D3DXLoadSurfaceFromSurface(pSurfaceDest,NULL,NULL,pSurfaceSrc,NULL,NULL,D3DX_FILTER_TRIANGLE,0);
		pSurfaceSrc->Release();
		pSurfaceDest->Release();
	}	
}

void CConvertTexture::GenerateMipMaps(bool bGenMipmap)
{
	//m_pddNewTexture이넘에 밉맵이 된 텍스쳐가 들어감
	
	
	D3DFORMAT fmt;
	D3DSURFACE_DESC sd;
	((LPDIRECT3DTEXTURE8)m_pddTexture)->GetLevelDesc(0, &sd);
    fmt = sd.Format;
	if(bGenMipmap)
		ms_pd3dDevice->CreateTexture(m_dwWidth,m_dwHeight,0,0,fmt,D3DPOOL_MANAGED,&m_pddNewTexture);
	else
		ms_pd3dDevice->CreateTexture(m_dwWidth,m_dwHeight,1,0,fmt,D3DPOOL_MANAGED,&m_pddNewTexture);

	LPDIRECT3DSURFACE8 pSurfaceSrc,pSurfaceDest;
	((LPDIRECT3DTEXTURE8)m_pddNewTexture)->GetSurfaceLevel(0,&pSurfaceDest);
	((LPDIRECT3DTEXTURE8)m_pddTexture)->GetSurfaceLevel(0,&pSurfaceSrc);

	D3DXLoadSurfaceFromSurface(pSurfaceDest, NULL, NULL, pSurfaceSrc, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
	pSurfaceSrc->Release();
	pSurfaceDest->Release();
	if(bGenMipmap)
		D3DXFilterTexture(m_pddNewTexture, NULL, 0, D3DX_FILTER_TRIANGLE);
	
	/*
    
	D3DXFilterTexture(pmiptexNew, NULL, 0, D3DX_FILTER_TRIANGLE);
    D3DXFilterTexture(m_pddNewTexture, NULL, 0, D3DX_FILTER_TRIANGLE);
	
    m_ptexOrig = pmiptexNew;

    if (m_ptexNew != NULL)
    {        
        D3DSURFACE_DESC sd;
        ((LPDIRECT3DCUBETEXTURE8)m_ptexNew)->GetLevelDesc(0, &sd);
        fmt = sd.Format;        
        Compress(fmt);
    }    
    return;
	*/
}

void CConvertTexture::SaveDDS(char *strFilename)
{
	FILE *fp=fopen(strFilename,"wb");  
    DWORD dwMagic;    
	dwMagic=MAKEFOURCC('D','D','S',' ');
	//dwMagic=0xffffffff;
	fwrite(&dwMagic,sizeof(dwMagic),1,fp);

	DDS_HEADER ddsh={sizeof(DDS_HEADER),};
	ddsh.dwHeaderFlags=DDS_HEADER_FLAGS_TEXTURE;
	ddsh.dwWidth=m_dwWidth;
	ddsh.dwHeight=m_dwHeight;
	ddsh.dwSurfaceFlags=DDS_SURFACE_FLAGS_TEXTURE;

	int nMips=m_pddNewTexture->GetLevelCount();

	if(nMips>1)
	{
		ddsh.dwHeaderFlags|=DDS_HEADER_FLAGS_MIPMAP;
		ddsh.dwSurfaceFlags|=DDS_SURFACE_FLAGS_MIPMAP;
		ddsh.dwMipMapCount=nMips;
	}

	D3DSURFACE_DESC sd;
	D3DFORMAT fmt;
	DWORD dwSize;
    DWORD dwPitch = 0;
    D3DLOCKED_RECT lr;    

	m_pddNewTexture->GetLevelDesc(0,&sd);
	fmt=sd.Format;
	dwSize=sd.Size;
	
	m_pddNewTexture->LockRect(0,&lr,NULL,D3DLOCK_READONLY);
	dwPitch=lr.Pitch;
	m_pddNewTexture->UnlockRect(NULL);	
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
			return;			
    }
	fwrite(&ddsh,sizeof(ddsh),1,fp);    

	LPDIRECT3DSURFACE8 pSurface;	
    


	for(int cLevel=0;cLevel<nMips;cLevel++)
	{
		if(FAILED(m_pddNewTexture->GetSurfaceLevel(cLevel,&pSurface)))
		{
			break;
		}		
		pSurface->GetDesc(&sd);
		pSurface->Release();
		if(FAILED(m_pddNewTexture->LockRect(cLevel,&lr,NULL,0)))
		{
			break;
		}
		if (sd.Format == D3DFMT_DXT1 ||
            sd.Format == D3DFMT_DXT2 ||
            sd.Format == D3DFMT_DXT3 ||
            sd.Format == D3DFMT_DXT4 ||
            sd.Format == D3DFMT_DXT5)
        {
			fwrite(lr.pBits,sd.Size,1,fp);
		}
		else
		{
			WORD yp;
			BYTE *pbDest=(BYTE*)lr.pBits;
			LONG dataBytesPerRow=0;

			if(sd.Format==D3DFMT_A8R8G8B8)
				dataBytesPerRow=4*sd.Width;
			else if(sd.Format==D3DFMT_R8G8B8)
				dataBytesPerRow=3*sd.Width;
			else
				dataBytesPerRow=2*sd.Width;
			for(yp=0;yp<sd.Height;yp++)
			{
				fwrite(pbDest,dataBytesPerRow,1,fp);
				pbDest+=lr.Pitch;
			}
		}
		m_pddNewTexture->UnlockRect(cLevel);
	}
	fclose(fp);
}

void CConvertTexture::SaveDDS(char *strFilename, int LowerSave,int MaxTextureSize)
{
	FILE *fp=fopen(strFilename,"wb");  
    DWORD dwMagic;    
	dwMagic=MAKEFOURCC('D','D','S',' ');
	fwrite(&dwMagic,sizeof(dwMagic),1,fp);

	DDS_HEADER ddsh={sizeof(DDS_HEADER),};
	ddsh.dwHeaderFlags=DDS_HEADER_FLAGS_TEXTURE;
	
	ddsh.dwWidth=m_dwWidth/(int)pow((double)2,LowerSave);	
	ddsh.dwHeight=m_dwHeight/(int)pow((double)2,LowerSave);

	long NowTextureSize=ddsh.dwWidth>=ddsh.dwHeight ? ddsh.dwWidth:ddsh.dwHeight;
	while(NowTextureSize > MaxTextureSize)
	{
		LowerSave++;
		ddsh.dwWidth=m_dwWidth/(int)pow((double)2,LowerSave);	
		ddsh.dwHeight=m_dwHeight/(int)pow((double)2,LowerSave);
		NowTextureSize=ddsh.dwWidth>=ddsh.dwHeight ? ddsh.dwWidth:ddsh.dwHeight;
	}
	ddsh.dwSurfaceFlags=DDS_SURFACE_FLAGS_TEXTURE;
	

	int nMips=m_pddNewTexture->GetLevelCount();
	nMips-=LowerSave;

	if(nMips>1)
	{
		ddsh.dwHeaderFlags|=DDS_HEADER_FLAGS_MIPMAP;
		ddsh.dwSurfaceFlags|=DDS_SURFACE_FLAGS_MIPMAP;
		ddsh.dwMipMapCount=nMips;
	}

	D3DSURFACE_DESC sd;
	D3DFORMAT fmt;
	DWORD dwSize;
    DWORD dwPitch = 0;
    D3DLOCKED_RECT lr;    

	m_pddNewTexture->GetLevelDesc(LowerSave,&sd);
	fmt=sd.Format;
	dwSize=sd.Size;
	
	m_pddNewTexture->LockRect(LowerSave,&lr,NULL,D3DLOCK_READONLY);
	dwPitch=lr.Pitch;
	m_pddNewTexture->UnlockRect(LowerSave);	
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
			return;			
    }
	fwrite(&ddsh,sizeof(ddsh),1,fp);    

	LPDIRECT3DSURFACE8 pSurface;

	for(int cLevel=0;cLevel<nMips;cLevel++)
	{
		if(FAILED(m_pddNewTexture->GetSurfaceLevel(cLevel+LowerSave,&pSurface)))
		{
			break;
		}		
		pSurface->GetDesc(&sd);
		pSurface->Release();
		if(FAILED(m_pddNewTexture->LockRect(cLevel+LowerSave,&lr,NULL,0)))
		{
			break;
		}
		if (sd.Format == D3DFMT_DXT1 ||
            sd.Format == D3DFMT_DXT2 ||
            sd.Format == D3DFMT_DXT3 ||
            sd.Format == D3DFMT_DXT4 ||
            sd.Format == D3DFMT_DXT5)
        {
			fwrite(lr.pBits,sd.Size,1,fp);
		}
		else
		{
			WORD yp;
			BYTE *pbDest=(BYTE*)lr.pBits;
			LONG dataBytesPerRow=0;

			if(sd.Format==D3DFMT_A8R8G8B8)
				dataBytesPerRow=4*sd.Width;
			else if(sd.Format==D3DFMT_R8G8B8)
				dataBytesPerRow=3*sd.Width;
			else
				dataBytesPerRow=2*sd.Width;
			for(yp=0;yp<sd.Height;yp++)
			{
				fwrite(pbDest,dataBytesPerRow,1,fp);
				pbDest+=lr.Pitch;
			}
		}
		m_pddNewTexture->UnlockRect(cLevel+LowerSave);
	}
	fclose(fp);
}
