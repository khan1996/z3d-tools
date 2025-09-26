// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"
#include "3DMath.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef LPDIRECT3DTEXTURE8 DXTEXTURE;
LPDIRECT3DDEVICE8 CTexture::ms_pd3dDevice;	
char CTexture::m_strPath[TEXTURENAMEBUFFER];
//CTextureContainer CTexture::m_TextureContainer;
CTextureCacheMgr CTexture::m_TextureContainer;
int CTexture::m_SkipMipLevel=0;

IDirect3DTexture8* CTexture::ms_pNullTexture = NULL;
bool CTexture::ms_bNullTextureMode = false;


unsigned char CTexture::ms_szReadBuffer[ 1398256 * 4 ] ;

int GetNumberOfBits( int mask )
{
	int nBits = 0;
    for(; mask; nBits++ )
        mask = mask & ( mask - 1 );  
    return nBits;
}

CTexture::CTexture()
{
	strcpy(m_strName,"");	
	m_pddTexture=NULL;
	m_bCreateEmpty=false;
}

CTexture::~CTexture()
{
	if(m_bCreateEmpty)
	{
		m_pddTexture->Release();
	}
	Unload();	
}

void CTexture::Init(LPDIRECT3DDEVICE8 pd3dDevice)
{
	ms_pd3dDevice=pd3dDevice;
	ms_pd3dDevice->AddRef();

	// create 1x1 texture
	if (FAILED(ms_pd3dDevice->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &ms_pNullTexture)))
	{
		ms_pd3dDevice->Release();
	}

	if(!ms_pNullTexture)
	{
		return;
	}

	D3DLOCKED_RECT lr;
	ms_pNullTexture->LockRect(0, &lr, NULL, 0);
	*((D3DCOLOR*)(lr.pBits)) = D3DCOLOR_ARGB(0xFF, 0xFF, 0xFF, 0xFF);
	ms_pNullTexture->UnlockRect(0);

	ms_bNullTextureMode = false;


	strcpy(m_strPath,"");
}

void CTexture::Close()
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

void CTexture::Load(char *filename, DWORD stage)
{
	

	if(strcmp(filename,"")==0)
		return;
	
	strcpy(m_strName,filename);

	char strFullName[TEXTURENAMEBUFFER];
	memset(strFullName, 0, TEXTURENAMEBUFFER);
	if(strcmp(m_strPath, ""))
	{
		strcpy(strFullName,m_strPath);
		strcat(strFullName,"\\");
	}
	strcat(strFullName,m_strName);
	int lens=strlen(strFullName);
	strFullName[lens-1]='s';
	strFullName[lens-2]='d';
	strFullName[lens-3]='d';

	m_stage=stage;
	
	m_pddTexture = m_TextureContainer.FindTexture(strFullName);
	if(m_pddTexture)
	{
		if(m_pddTexture)
		{
			D3DSURFACE_DESC ddsc;
			((LPDIRECT3DTEXTURE8)m_pddTexture)->GetLevelDesc(0,&ddsc);
			m_dwWidth=ddsc.Width;
			m_dwHeight=ddsc.Height;				
			return;
		}

	}
	/*
	int nUsedText=m_TextureContainer.FindTexture(strFullName);
	if(nUsedText>=0)
	{
		m_pddTexture=m_TextureContainer.AddUsedTexture(nUsedText);
		if(m_pddTexture)
		{
			D3DSURFACE_DESC ddsc;
			((LPDIRECT3DTEXTURE8)m_pddTexture)->GetLevelDesc(0,&ddsc);
			m_dwWidth=ddsc.Width;
			m_dwHeight=ddsc.Height;				
			return;
		}
	}*/
	//LPDIRECT3DBASETEXTURE8 pddsTemp = NULL; 
	m_pddTexture=NULL;
	if(strstr(m_strPath,"Interface")==0)
	{
		ReadDDSTexture( m_pddTexture ,0);	
	}
	else
	{
		ReadDDSTexture( m_pddTexture ,1);	
	}
	
	//m_TextureContainer.AddTexture(strFullName,m_pddTexture);
	m_TextureContainer.AddTexture(strFullName,m_strPath,m_pddTexture);
	strcpy(m_strName,strFullName);

	//pddsTemp->Release();
	//pddsTemp=NULL;	
}

void CTexture::ReadDDSTexture(LPDIRECT3DBASETEXTURE8 &pddsTemp,int nMethod)
{    
	char strFullName[TEXTURENAMEBUFFER];
	memset(strFullName, 0, TEXTURENAMEBUFFER);
	if(strcmp(m_strPath, ""))
	{
		strcpy(strFullName,m_strPath);
		strcat(strFullName,"\\");
	}
	strcat(strFullName,m_strName);
	int lens=strlen(strFullName);
	strFullName[lens-1]='s';
	strFullName[lens-2]='d';
	strFullName[lens-3]='d';	

	if(nMethod)
	{
		FILE* fp=fopen(strFullName,"rb");
		if(fp==NULL)
		{
			char msg[256];
			sprintf(msg,"File not found %s",strFullName);
			MessageBox(NULL,msg,0,0);
		}

		
		
		DWORD dwMagic;
		DDS_HEADER ddsh;    
		LPDIRECT3DTEXTURE8 pmiptex = NULL;
		fread(&dwMagic,sizeof(dwMagic),1,fp);    
		
		fread(&ddsh,sizeof(ddsh),1,fp);    
		if (ddsh.dwSize != sizeof(ddsh))
			CGraphicLayerError("CTexture:ReadDDSTexture , This is not dds format");

		DWORD dwWidth = ddsh.dwWidth;
		DWORD dwHeight = ddsh.dwHeight;
		DWORD numMips = ddsh.dwMipMapCount;
		DWORD dwDepth=0;

		m_dwWidth=ddsh.dwWidth;
		m_dwHeight=ddsh.dwHeight;

		if (numMips == 0)
			numMips = 1;
    
		if (ddsh.dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME)
			dwDepth = ddsh.dwDepth;
		else
			dwDepth = 0;

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
			return;
		
		if (FAILED(ms_pd3dDevice->CreateTexture(dwWidth>>m_SkipMipLevel, dwHeight>>m_SkipMipLevel, numMips-m_SkipMipLevel, 0, fmt, D3DPOOL_MANAGED, &pmiptex)))    
		{
			CGraphicLayerError("CTexture:ReadDDSTexture , CreateTexture is failed");		
		}
		if (FAILED(LoadAllMipSurfaces(pmiptex, numMips, fp)))
		{
			MessageBox(NULL,"LoadAllMipSurface Load Failed",0,0);
			MessageBox(NULL,strFullName,0,0);
			CGraphicLayerError("CTexture:ReadDDSTexture , LoadAllMipSurfaces is failed");
		}
		fclose(fp);
		pddsTemp = pmiptex;	
	}
	else
	{	

		HANDLE hFile;
		hFile=CreateFile(strFullName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);		  
		if( hFile==INVALID_HANDLE_VALUE)
		{
			char msg[256];
			sprintf(msg,"File not found %s",strFullName);
			MessageBox(NULL,msg,0,0);
		}
		DWORD dwTextureFileSize=GetFileSize(hFile,NULL);
		//unsigned char *pFileBuffer=new unsigned char[dwTextureFileSize];

		DWORD dwReaded;
		ReadFile(hFile,ms_szReadBuffer,dwTextureFileSize,&dwReaded,NULL);
		CloseHandle(hFile);
		DWORD dwMagic=MAKEFOURCC('D','D','S',' ');		
		DWORD *p=(DWORD*)ms_szReadBuffer;
		*p=dwMagic;
		LPDIRECT3DTEXTURE8 pmiptex = NULL;
		HRESULT hr=D3DXCreateTextureFromFileInMemory(ms_pd3dDevice,ms_szReadBuffer,dwTextureFileSize,&pmiptex);  	
		pddsTemp = pmiptex;	
	
	}
	
}



void CTexture::DeleteTexture()
{
	if(m_pddTexture)
	{
		//m_TextureContainer.DeleteTexture(m_strName);
		m_TextureContainer.DeleteData(m_strName);
	}
}

void CTexture::CreateEmpty(long lSx, long lSy,D3DFORMAT format,D3DPOOL pool,long mip)
{	
	LPDIRECT3DTEXTURE8 pddEmpty;	
	ms_pd3dDevice->CreateTexture(lSx,lSy,mip,0,format,pool,&pddEmpty);	
	m_pddTexture=pddEmpty;
	m_bCreateEmpty=true;
	m_dwWidth=lSx;
	m_dwHeight=lSy;	
}

void CTexture::FillColor(DWORD color)
{
	D3DSURFACE_DESC sd;
	LPDIRECT3DTEXTURE8 pddEmpty;
	pddEmpty=(LPDIRECT3DTEXTURE8)m_pddTexture;
	pddEmpty->GetLevelDesc(0,&sd);
	D3DLOCKED_RECT lr;
	pddEmpty->LockRect(0,&lr,NULL,0);
	DWORD *pSurface=(DWORD*)lr.pBits;
	
	//color light;//=color(rand()%255,rand()%255,rand()%255);	
	//color asdf;
	/*
	unsigned char r,g,b;
	r=(rand()%200)+55; 
	g=rand()%255;
	b=rand()%255;
	*/

	for(DWORD ix=0;ix<sd.Width;ix++)
	{
		for(DWORD iy=0;iy<sd.Height;iy++)
		{
			//pSurface[ix+iy*sd.Width]=D3DCOLOR_ARGB(0xff,r,g,b);
			//pSurface[ix+iy*sd.Width]=D3DCOLOR_ARGB(0xff,255,0,0);
			pSurface[ix+iy*sd.Width]=color;
			//pSurface[ix+iy*sd.Width]=ix*(255.0f/32.0f);
			//pSurface[ix+iy*sd.Width]=Clight.c;
		}
	}
	pddEmpty->UnlockRect(0);
	
}

HRESULT CTexture::LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex,long numMips, FILE *fp)
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
			fread(lr.pBits,sd.Size,1,fp);
        }
        else
        {
            DWORD yp;
            BYTE* pbDest = (BYTE*)lr.pBits;
            for (yp = 0; yp < sd.Height; yp++)
            {                
				fread(pbDest,dwBytesPerRow,1,fp);
                pbDest += lr.Pitch;
            }
        }

        if (pmiptex != NULL)
            hr = pmiptex->UnlockRect(iLevel);
        ReleasePpo(&psurf);
    }
    return S_OK;
}

void* CTexture::Lock()
{	
	LPDIRECT3DTEXTURE8 pddEmpty;
	pddEmpty=(LPDIRECT3DTEXTURE8)m_pddTexture;	
	D3DLOCKED_RECT lr;
	pddEmpty->LockRect(0,&lr,NULL,0);	
	return lr.pBits;		
}

void CTexture::Unlock()
{
	LPDIRECT3DTEXTURE8 pddEmpty;
	pddEmpty=(LPDIRECT3DTEXTURE8)m_pddTexture;			
	pddEmpty->UnlockRect(0);
}

void CTexture::Unload()
{
	if(m_pddTexture)
	{
	/*	char strFullName[TEXTURENAMEBUFFER];
		memset(strFullName, 0, TEXTURENAMEBUFFER);
		if(strcmp(m_strPath, ""))
		{
			strcpy(strFullName,m_strPath);
			strcat(strFullName,"\\");
		}
		strcat(strFullName,m_strName);*/
		m_TextureContainer.DeleteData(m_strName);
		//m_TextureContainer.DeleteTexture(strFullName);
	}
	m_pddTexture=NULL;
}
void CTexture::ChangeTextureIntension( byte *pImage, int iSize, float fColorFactor) {	// Texture Color ³óµµº¯È­ (D3DFMT_R8G8B8 ¿ë)

	int i;
	for( i=0; i < iSize / 3; i++ )
	{
		float fScale = 1.0f;
		float fTmp = 0.0f;

		float fR = 0.0f;
		float fG = 0.0f;
		float fB = 0.0f;
		
		fR = (float)pImage[0];
		fG = (float)pImage[1];
		fB = (float)pImage[2];

		fR = fR * fColorFactor / 255.0f;
		fG = fG * fColorFactor / 255.0f;
		fB = fB * fColorFactor / 255.0f;
		
		if( ( fR > 1.0f ) && ( fTmp = ( 1.0f / fR ) ) < fScale ) 
			fScale = fTmp;
		if( ( fG > 1.0f ) && ( fTmp = ( 1.0f / fG ) ) < fScale ) 
			fScale = fTmp;
		if( ( fB > 1.0f ) && ( fTmp = ( 1.0f / fB ) ) < fScale ) 
			fScale = fTmp;

		fScale *= 255.0f;
		
		fR *= fScale;
		fG *= fScale;
		fB *= fScale;

		pImage[0] = ( byte )fR;
		pImage[1] = ( byte )fG;
		pImage[2] = ( byte )fB;

		pImage +=3;
	}

}
void CTexture::CreateEmptyTexture( unsigned int uiX, unsigned int uiY, unsigned int uiMip,D3DFORMAT dFormat, D3DPOOL dPool) {
	
	DXTEXTURE lpEmptyTexture;
	HRESULT hr = ms_pd3dDevice->CreateTexture( uiX,uiY,uiMip,0,dFormat,dPool,&lpEmptyTexture);
	if(hr == D3DERR_INVALIDCALL)
		MessageBox(NULL,"Err1","error",MB_OK);
	else if(hr == D3DERR_OUTOFVIDEOMEMORY)
		MessageBox(NULL,"Err2","error",MB_OK);
	else if(hr == E_OUTOFMEMORY)
		MessageBox(NULL,"Err3","error",MB_OK);
	m_pddTexture = lpEmptyTexture;
	m_dwWidth = uiX;
	m_dwHeight = uiY;
	
}
void CTexture::FillTexture( byte * pByte) {

	D3DSURFACE_DESC dTextureSurf;
	DXTEXTURE	lpEmptyTexture;

	lpEmptyTexture = (DXTEXTURE)m_pddTexture;
	lpEmptyTexture->GetLevelDesc( 0, &dTextureSurf );
	D3DLOCKED_RECT lRect;
	HRESULT hr = lpEmptyTexture->LockRect( 0, &lRect, NULL,  0);
	if(hr == D3DERR_INVALIDCALL)
		MessageBox(NULL,"te","tt",MB_OK);
	byte *pSurface = (byte *)lRect.pBits;
	unsigned int iImageSize = dTextureSurf.Size;//dTextureSurf.Width * dTextureSurf.Height;

	memcpy( pSurface,pByte,sizeof(byte) * iImageSize);
	
	lpEmptyTexture->UnlockRect( 0 );
}

void CTexture::SetBitTexture16( int iWidth, int iHeight, WORD* pSrc )
{
	D3DSURFACE_DESC		dTextureSurf ;
	DXTEXTURE			lpEmptyTexture ;
	D3DLOCKED_RECT		lRect ;

	lpEmptyTexture = ( DXTEXTURE )m_pddTexture ;
	lpEmptyTexture->GetLevelDesc( 0, &dTextureSurf ) ;
	
	HRESULT hr = lpEmptyTexture->LockRect( 0, &lRect, NULL, 0 ) ;
	if ( hr == D3DERR_INVALIDCALL )
		MessageBox( NULL,"te","tt",MB_OK ) ;

	WORD* pSurface = ( WORD * )lRect.pBits ;

//	unsigned int iImageSize = dTextureSurf.Size;//dTextureSurf.Width * dTextureSurf.Height;
//	memset( pSurface, 0, sizeof(WORD) * iImageSize ) ;

	BYTE* pbyTempBuff = ( BYTE* )pSrc ;
	WORD* pwTempBuff  = ( WORD* )( pbyTempBuff + 1 ) ;
	
	int CurSrc = 0 ;
	int CurDest = 0 ;

	for ( int ix = 0 ;ix < iHeight ; ix ++ )
	{
		CurDest = dTextureSurf.Width * ix ;
		for ( int iy = 0 ; iy < iWidth ;iy ++ )
		{
			pSurface[ CurDest ] = pwTempBuff[ CurSrc ] ;
			++ CurDest ;
			++ CurSrc ;
		}
	}

	lpEmptyTexture->UnlockRect( 0 ) ;
}

void CTexture::SetBitTexture32( int iWidth, int iHeight, DWORD* pSrc )
{
	D3DSURFACE_DESC		dTextureSurf ;
	DXTEXTURE			lpEmptyTexture ;
	D3DLOCKED_RECT		lRect ;

	lpEmptyTexture = ( DXTEXTURE )m_pddTexture ;
	lpEmptyTexture->GetLevelDesc( 0, &dTextureSurf ) ;

	HRESULT hr = lpEmptyTexture->LockRect( 0, &lRect, NULL, 0 ) ;
	if ( hr == D3DERR_INVALIDCALL )
		MessageBox( NULL,"te","tt",MB_OK ) ;

	DWORD* pSurface = ( DWORD * )lRect.pBits ;

	//	unsigned int iImageSize = dTextureSurf.Size;//dTextureSurf.Width * dTextureSurf.Height;
	//	memset( pSurface, 0, sizeof(WORD) * iImageSize ) ;

//	BYTE* pbyTempBuff = ( BYTE* )pSrc ;
//	DWORD* pwTempBuff  = ( DWORD* )( pbyTempBuff + 1 ) ;

	DWORD* pwTempBuff = ( DWORD *)pSrc ;

	int CurSrc = 0 ;
	int CurDest = 0 ;

	for ( int ix = 0 ;ix < iHeight ; ix ++ )
	{
		CurDest = dTextureSurf.Width * ix ;
		for ( int iy = 0 ; iy < iWidth ;iy ++ )
		{
			pSurface[ CurDest ] = pwTempBuff[ CurSrc ] ;
			++ CurDest ;
			++ CurSrc ;
		}
	}

	lpEmptyTexture->UnlockRect( 0 ) ;
}

IDirect3DBaseTexture8* CTexture::GetTexture()
{
	if (ms_bNullTextureMode)
	{
		return ms_pNullTexture;
	}

	return m_pddTexture;
};


void CTexture::NullTextureMode(bool bNullTexture)
{
	ms_bNullTextureMode = bNullTexture;
}