// IMEFont.cpp: implementation of the CIMEFont class.
//
//////////////////////////////////////////////////////////////////////

#include "IMEFont.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIMEFont::CIMEFont()
{
	for(int i=0;i<10;i++)
	{
		strcpy(m_strPrint[i],"");
		strcpy(m_strPrinted[i],"");
	}	
	m_RenderX=0;
	m_RenderY=0;
}

CIMEFont::~CIMEFont()
{
	if(m_pTexture)
		m_pTexture->Release();
}

void CIMEFont::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(isRewirte())
		MakeTexture();
	m_pVertex[0].v.x=0.0f+m_RenderX;
	m_pVertex[1].v.x=0.0f+m_RenderX;
	m_pVertex[2].v.x=(float)m_SizeX+m_RenderX;
	m_pVertex[3].v.x=(float)m_SizeX+m_RenderX;

	m_pVertex[1].v.y=0.0f+m_RenderY;
	m_pVertex[3].v.y=0.0f+m_RenderY;
	m_pVertex[0].v.y=(float)m_SizeY+m_RenderY;
	m_pVertex[2].v.y=(float)m_SizeY+m_RenderY;		
		
	m_pVertex[0].tu=0.0f;
	m_pVertex[1].tu=0.0f;

	m_pVertex[3].tu=1.0f;
	m_pVertex[2].tu=1.0f;

	m_pVertex[1].tv=0.0f;
	m_pVertex[3].tv=0.0f;

	m_pVertex[0].tv=1.0f;
	m_pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		m_pVertex[i].w=0.1f;
		m_pVertex[i].v.z=0.1f;
		m_pVertex[i].Diffuse.c=0xffffffff;
		m_pVertex[i].Specular.c=0xffffffff;
	}
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			
	pd3dDevice->SetTexture(0,m_pTexture);
	//3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);    
    //3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_pVertex,sizeof(TLVertex));
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);    
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
}	

void CIMEFont::Create(LPDIRECT3DDEVICE8 pd3dDevice,long sizeX,long sizeY)
{
	m_SizeX=sizeX;
	m_SizeY=sizeY;
	// Chatting Message Font 512,64;
	m_cLine=m_SizeY/16;
	pd3dDevice->CreateTexture(sizeX,sizeY,1,0,D3DFMT_A4R4G4B4,D3DPOOL_MANAGED,&m_pTexture);		
}

void CIMEFont::PrintToTexture(char *str, long line)
{	
	if( line >=0 && line<m_cLine)	
		strcpy(m_strPrint[line],str);	
}


bool CIMEFont::isRewirte()
{
	for(int cLine=0;cLine<m_cLine;cLine++)
	{
		if( strcmp(m_strPrint[cLine],m_strPrinted[cLine])!=0)
			return true;
	}
	return false;	
}

void CIMEFont::MakeTexture()
{
	DWORD* pBitmapBits;
	BITMAPINFO bmi;	
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );	
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_SizeX;
    bmi.bmiHeader.biHeight      = -(int)m_SizeY;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    SetMapMode( hDC, MM_TEXT );

	HFONT hFont=CreateFont(-12,6,0,0,0,FALSE,FALSE,FALSE, CHINESEBIG5_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                          DEFAULT_PITCH, "MingLiu" );	

	SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, hFont );
    
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_LEFT);  

	for(int cLine=0;cLine<m_cLine;cLine++)
	{
		ExtTextOut( hDC, 0, cLine*16, ETO_OPAQUE, NULL,m_strPrint[cLine],strlen(m_strPrint[cLine]), NULL );
		strcpy(m_strPrinted[cLine],m_strPrint[cLine]);
	}
    /*
	ExtTextOut( hDC, 0, 12, ETO_OPAQUE, NULL,m_strPrint[1],strlen(m_strPrint[1]), NULL );
	ExtTextOut( hDC, 0, 24, ETO_OPAQUE, NULL,m_strPrint[2],strlen(m_strPrint[2]), NULL );
	ExtTextOut( hDC, 0, 36, ETO_OPAQUE, NULL,m_strPrint[3],strlen(m_strPrint[3]), NULL );
	strcpy(m_strPrinted[0],m_strPrint[0]);
	strcpy(m_strPrinted[1],m_strPrint[1]);
	strcpy(m_strPrinted[2],m_strPrint[2]);
	strcpy(m_strPrinted[3],m_strPrint[3]);
	*/
	//SIZE p;
	//GetTextExtentPoint32(hDC,"´î",2,&p);
	
	//RECT rcFont={0,0,25*15,4*16};
	//DrawText(hDC,strTemp,strlen(strTemp),&rcFont,DT_WORDBREAK|DT_LEFT);	
    
    D3DLOCKED_RECT d3dlr;
    m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
    WORD* pDst16 = (WORD*)d3dlr.pBits;
    BYTE bAlpha;

	int x,y;
    for( y=0;y<m_SizeY; y++ )
    {
        for( x=0;x<m_SizeX; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[m_SizeX*y + x] & 0xff) >> 4);

            if (bAlpha > 0)
            {				
                *pDst16++ = (bAlpha << 12) | 0x0fff;					
            }
            else
            {
                *pDst16++=0x0000;
            }
        }
    }
    m_pTexture->UnlockRect(0);
    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );
}

void CIMEFont::Render(LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwColor)
{
	if(isRewirte())
		MakeTexture();
	m_pVertex[0].v.x=0.0f+m_RenderX;
	m_pVertex[1].v.x=0.0f+m_RenderX;
	m_pVertex[2].v.x=(float)m_SizeX+m_RenderX;
	m_pVertex[3].v.x=(float)m_SizeX+m_RenderX;

	m_pVertex[1].v.y=0.0f+m_RenderY;
	m_pVertex[3].v.y=0.0f+m_RenderY;
	m_pVertex[0].v.y=(float)m_SizeY+m_RenderY;
	m_pVertex[2].v.y=(float)m_SizeY+m_RenderY;		
		
	m_pVertex[0].tu=0.0f;
	m_pVertex[1].tu=0.0f;

	m_pVertex[3].tu=1.0f;
	m_pVertex[2].tu=1.0f;

	m_pVertex[1].tv=0.0f;
	m_pVertex[3].tv=0.0f;

	m_pVertex[0].tv=1.0f;
	m_pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		m_pVertex[i].w=0.1f;
		m_pVertex[i].v.z=0.1f;
		m_pVertex[i].Diffuse.c=dwColor;
		m_pVertex[i].Specular.c=dwColor;
	}
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			
	pd3dDevice->SetTexture(0,m_pTexture);
	//3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);    
    //3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_pVertex,sizeof(TLVertex));
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);    
    pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
}
