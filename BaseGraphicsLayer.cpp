// BaseGraphic.cpp: implementation of the BaseGraphic class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseGraphicsLayer.h"
#include <d3d8.h>
#include <d3dx8.h>
#include "Vertex.h"
#include "FastMath.h"
#include "dxutil.h"
#include "NTexture.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewCamera BaseGraphicsLayer::m_ViewCamera;

LPDIRECT3DDEVICE8 BaseGraphicsLayer::m_pd3dDevice;
LPDIRECT3D8	BaseGraphicsLayer::	m_pD3D;
D3DPRESENT_PARAMETERS BaseGraphicsLayer::m_d3dpp;

long BaseGraphicsLayer::m_lScreenSx;
long BaseGraphicsLayer::m_lScreenSy;	
color	BaseGraphicsLayer::m_ClearColor;

HWND BaseGraphicsLayer::m_hWnd;	

bool BaseGraphicsLayer::m_VoodooOption=false;
int BaseGraphicsLayer::m_iGraphicCards = 0;

char BaseGraphicsLayer::m_aszCustomMsg[10][300];
CD3DFont* BaseGraphicsLayer::m_pFont = NULL;


BaseGraphicsLayer::BaseGraphicsLayer()
{	
	m_hWnd=NULL;
	m_pd3dDevice=NULL;
	m_pD3D=NULL;
	m_fFov=m_fNear=m_fFar=0.0f;	
	m_ClearColor.c=0x0;
	m_bEditorMode=false;
	m_iGraphicCards = 0;

    memset(m_aszCustomMsg, 0, sizeof(m_aszCustomMsg));
}

BaseGraphicsLayer::~BaseGraphicsLayer()
{
	CROSSM::CNTexture::Close();
	CTexture::Close();

	if(m_pd3dDevice) {
		m_pd3dDevice->Release();
		m_pd3dDevice = NULL;
	}
	if(m_pD3D) {
		m_pD3D->Release();
		m_pD3D = NULL;	
	}
	if(m_pFont) {		
		delete 	m_pFont;
		m_pFont = NULL;
	}
}

void BaseGraphicsLayer::Create(HWND hWnd, bool bWindowed,bool Editor, long screenx, long screeny, long screenwidth, long screenheight)
{	
	//LogMessage("BaseGraphicsLayer Create Start");	
	m_hWnd=hWnd;
	m_bWindowed=bWindowed;
	m_bEditorMode=Editor;
	GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

	if(NULL == (m_pD3D=Direct3DCreate8(D3D_SDK_VERSION)))
	{
		throw CGraphicLayerError("BaseGraphicsLayer:Create,GetDirect3D Interface getting fail");
	}
	//LogMessage("Direct3D 8.0 Create");	
	/*
    D3DAdapterInfo* pAdapterInfo = &CEnumD3D::m_Adapters[CEnumD3D::m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*	pModeInfo	= &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];    	
	*/

	D3DAdapterInfo* pAdapterInfo = &CEnumD3D::m_Adapters[CEnumD3D::m_nAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[CEnumD3D::m_nDevice];
    D3DModeInfo*	pModeInfo	= &pDeviceInfo->modes[CEnumD3D::m_nMode];

	if(strstr(pAdapterInfo->d3dAdapterIdentifier.Description,"Voodoo"))
	{
		m_VoodooOption=true;
		m_iGraphicCards = 0;
		//MessageBox(NULL,"Voodoo",0,0);
	}
	else
	{
		m_VoodooOption=false;
		if(strstr(pAdapterInfo->d3dAdapterIdentifier.Description,"TNT")) {
			m_iGraphicCards = 1;
		}
		else if(strstr(pAdapterInfo->d3dAdapterIdentifier.Description,"GeForce2")) {
			m_iGraphicCards = 2;
		}
		else if(strstr(pAdapterInfo->d3dAdapterIdentifier.Description,"GeForce3")) {
			m_iGraphicCards = 3;
		}
		else if(strstr(pAdapterInfo->d3dAdapterIdentifier.Description,"GeForce4")) {
			m_iGraphicCards = 4;
		
		}
		else {
			m_iGraphicCards = 5;
		}
	}
	
    // Set up the presentation parameters
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.Windowed               = bWindowed;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.MultiSampleType        = pDeviceInfo->MultiSampleType;
    //m_d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY ;
	m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD ;
	m_d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	
	m_d3dpp.hDeviceWindow=m_hWnd;		
	m_d3dpp.EnableAutoDepthStencil=TRUE;
	
	m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if(bWindowed)
    {
        m_lScreenSx=m_d3dpp.BackBufferWidth  = screenwidth ; //m_rcWindowClient.right - m_rcWindowClient.left;
        m_lScreenSy=m_d3dpp.BackBufferHeight = screenheight ;//m_rcWindowClient.bottom - m_rcWindowClient.top;

		if(pAdapterInfo->d3ddmDesktop.Format==D3DFMT_X8R8G8B8)
		{
			m_d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;			
		}
		else
		{
			m_d3dpp.BackBufferFormat=pAdapterInfo->d3ddmDesktop.Format;
		}
        
    }
    else
    {		
        m_lScreenSx=m_d3dpp.BackBufferWidth  = screenwidth ; //pModeInfo->Width;
        m_lScreenSy=m_d3dpp.BackBufferHeight = screenheight ; //pModeInfo->Height;
        m_d3dpp.BackBufferFormat = pModeInfo->Format;		
		m_d3dpp.FullScreen_PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
    }    
	if(Editor)
	{
		if(FAILED(m_pD3D->CreateDevice( CEnumD3D::m_dwAdapter, pDeviceInfo->DeviceType,
								   m_hWnd,
								   //D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								   //D3DCREATE_MIXED_VERTEXPROCESSING,								   
								   pModeInfo->dwBehavior,
								   &m_d3dpp,
								   &m_pd3dDevice )))
		{		
			throw CGraphicLayerError("BaseGraphicsLayer:Create, CreateDevice is failed");
		}
	}
	else
	{
		if(FAILED(m_pD3D->CreateDevice( CEnumD3D::m_dwAdapter, pDeviceInfo->DeviceType,
								   m_hWnd,
								   //D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								   //D3DCREATE_MIXED_VERTEXPROCESSING,
								   pModeInfo->dwBehavior,
								   &m_d3dpp,
								   &m_pd3dDevice )))
		{
			throw CGraphicLayerError("BaseGraphicsLayer:Create, CreateDevice is failed");
		}
	}

	/*
	D3DVIEWPORT8 d3dViewport ;
	d3dViewport.Width = screenwidth ;
	d3dViewport.Height = screenheight ;
	d3dViewport.X = screenx ;
	d3dViewport.Y = screeny ;
	d3dViewport.MaxZ = 1.0f ;
	d3dViewport.MinZ = 0.0f ;

	m_pd3dDevice->SetViewport( &d3dViewport ) ;
	*/
	m_dwStartRemainTextureMemory=m_pd3dDevice->GetAvailableTextureMem();	

	//LogMessage("Direct3D Device Create");	
	//LogMessage("Direct3D 7.0 Create");
	// Follow code is to Initial RenderState( light ,matrix,material);		
	CTexture::Init(m_pd3dDevice);		
	matrix matProj;
	float fAspect=(float)m_d3dpp.BackBufferWidth/(float)m_d3dpp.BackBufferHeight;
	fAspect=1.0f/fAspect;
	//fAspect=1.0f;	

	// edith 프로젝션 투영 변경.
	matProj.MakeProjection(3.1415f/3.0f,fAspect,50.0f, 320000.0f);	
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);
	matrix matView;
	matView.MakeIdent();
	m_pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
	matrix matWorld;
	matWorld.MakeIdent();
	m_pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matWorld);
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER,FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_CLIPPING,TRUE);

	m_pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	m_pd3dDevice->SetTextureStageState(1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);	

	m_pd3dDevice->SetTextureStageState(2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(2,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	m_pd3dDevice->SetTextureStageState(3,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(3,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetTextureStageState(3,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	CROSSM::CNTexture::Init(m_pd3dDevice);

	//m_pd3dDevice->SetRenderState(D3DRS_CLIPPING,FALSE);


	D3DLIGHT8 light; 

	ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type        = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r   = 1.0f;
    light.Diffuse.g   = 1.0f;
    light.Diffuse.b   = 1.0f;
    light.Position.x   = light.Direction.x = 0.0f;
    light.Position.y   = light.Direction.y = -1.0f;
    light.Position.z   = light.Direction.z = 0.0f;
	light.Direction.x = 0.0f;    
    light.Range        = 1000.0f;	
    m_pd3dDevice->SetLight( 0,&light );
	m_pd3dDevice->LightEnable( 0,TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,0xffffffff);		
	

	D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    m_pd3dDevice->SetMaterial( &mtrl );
	
	
	if(m_bEditorMode)
	{
		m_pFont= new CD3DFont( _T("Arial"), 10, 0 );
		m_pFont->InitDeviceObjects( m_pd3dDevice );
		m_pFont->RestoreDeviceObjects();
	}
	else
		m_pFont = NULL;

	// For Init View Frustum //	
	m_ViewCamera.BuildFrustum(fAspect,3.1415f/3.0f,40.0f,1300000.0f);	
	//LogMessage("Setting Default Device RenderState");

	CFastMath::Init();	

	m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE,TRUE);

	//float fMipLodBias=-2.0f;
	//m_pd3dDevice->SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,*((LPDWORD)(&fMipLodBias)));
	//LogMessage("FastMath Class Init");
}
void BaseGraphicsLayer::GetPickPoint(long lScreenX, long lScreenY, float &fsX, float &fsY, float &fsZ)
{	
	RECT rect;
	rect.left=lScreenX;
	rect.right=lScreenX+1;
	rect.top=lScreenY;
	rect.bottom=lScreenY+1;
	POINT p;
	p.x=0;
	p.y=0;
	D3DLOCKED_RECT d3dlocked_rect;	
	
	LPDIRECT3DSURFACE8 pBackBuffer;
	m_pd3dDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);			
	pBackBuffer->LockRect(&d3dlocked_rect,NULL,D3DLOCK_READONLY|D3DLOCK_NO_DIRTY_UPDATE );	
	DWORD *e=(DWORD*)d3dlocked_rect.pBits;		
	pBackBuffer->UnlockRect();
	pBackBuffer->Release();	
	

	LPDIRECT3DSURFACE8 pZBuffer;
	m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );		
	pZBuffer->LockRect(&d3dlocked_rect,&rect,D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_READONLY);	

	if(d3dlocked_rect.pBits==NULL)
		return;
	BYTE *pb=(BYTE*)d3dlocked_rect.pBits;
	WORD wLen=0;	
	wLen|=*(pb+1)<<8;
	wLen|=*(pb);
	pZBuffer->UnlockRect();
	pZBuffer->Release();	
	
	matrix matView,matProj;
	m_pd3dDevice->GetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);

	matrix matSet=matView*matProj;

	matrix matInv;

	//matInv.Inverse(matSet);
	float d;
	D3DXMatrixInverse((D3DXMATRIX*)&matInv,&d,(D3DXMATRIX*)&matSet);  		

	float fX,fY,fZ;
	fX=2.0/m_d3dpp.BackBufferWidth;
	fY=2.0/m_d3dpp.BackBufferHeight;	

	fZ=(float)wLen/(float)0xffff;
	fX=lScreenX*(fX)-1.0f;
	fY=1.0f-(lScreenY)*(fY);

	float fXp=matInv._11*fX + matInv._21*fY + matInv._31*fZ + matInv._41;
	float fYp=matInv._12*fX + matInv._22*fY + matInv._32*fZ + matInv._42;
	float fZp=matInv._13*fX + matInv._23*fY + matInv._33*fZ + matInv._43;
	float fWp=matInv._14*fX + matInv._24*fY + matInv._34*fZ + matInv._44;	

	fsX=fXp/fWp;
	fsY=fYp/fWp;
	fsZ=fZp/fWp;
}

void BaseGraphicsLayer::Flip()
{	
	//RECT rect={100,100,800,600};
	m_pd3dDevice->Present(NULL,NULL,m_hWnd,NULL);
	//m_pd3dDevice->Present(&rect,&rect,NULL,NULL);

	/*
	LPDIRECT3DSURFACE8 pddsFrontBuffer,pddsBackBuffer;	
	m_pd3dDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pddsBackBuffer);	
	m_pd3dDevice->GetFrontBuffer(&pddsFrontBuffer);
	RECT rect={0,0,800,600};
	POINT pt={0,0};
	m_pd3dDevice->CopyRects(pddsBackBuffer,&rect,0,pddsFrontBuffer,&pt);
	pddsFrontBuffer->Release();
	pddsBackBuffer->Release();
	*/
}

void BaseGraphicsLayer::PrefareRender()
{
	m_pd3dDevice->SetTransform(D3DTS_VIEW,m_ViewCamera.GetMatView());
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, m_ClearColor.c, 1.0f, 0 );	
}

void BaseGraphicsLayer::ShowState()
{
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	/*
	TLVertex NSVertex[4];
	float fZvalue=0.01f;
	float fProjectedPositionW=0.01f;

	NSVertex[0].v=vector3(0.0f,100.0f,fZvalue);
	NSVertex[0].tu=0.0f;
	NSVertex[0].tv=1.0f;
	NSVertex[1].v=vector3(0.0f,0.0f,fZvalue);
	NSVertex[1].tu=0.0f;
	NSVertex[1].tv=0.0f;
	NSVertex[2].v=vector3(100.0f,100.0f,fZvalue);
	NSVertex[2].tu=1.0f;
	NSVertex[2].tv=1.0f;
	NSVertex[3].v=vector3(100.0f,0.0f,fZvalue);		
	NSVertex[3].tu=1.0f;
	NSVertex[3].tv=0.0f;
	
	NSVertex[0].v=vector3(100.0f,100.0f,fZvalue);
	NSVertex[1].v=vector3(100.0f,100.0f,fZvalue);
	NSVertex[2].v=vector3(100.0f,100.0f,fZvalue);
	NSVertex[3].v=vector3(100.0f,100.0f,fZvalue);

	float fNsSize=100.0f;

	NSVertex[0].v.x+=fNsSize*cosf(0.0f + 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[0].v.y+=fNsSize*sinf(0.0f + 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[1].v.x+=fNsSize*cosf(3.1415f*0.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[1].v.y+=fNsSize*sinf(3.1415f*0.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[2].v.x+=fNsSize*cosf(3.1415f*1.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[2].v.y+=fNsSize*sinf(3.1415f*1.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[3].v.x+=fNsSize*cosf(3.1415f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[3].v.y+=fNsSize*sinf(3.1415f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );	


	NSVertex[0].w=fProjectedPositionW;
	NSVertex[1].w=fProjectedPositionW;
	NSVertex[2].w=fProjectedPositionW;
	NSVertex[3].w=fProjectedPositionW;
	
	NSVertex[0].Diffuse.c=0xff000000;
	NSVertex[1].Diffuse.c=0xff000000;
	NSVertex[2].Diffuse.c=0xff000000;
	NSVertex[3].Diffuse.c=0xff000000;	
	NSVertex[0].Specular.c=0x0;
	NSVertex[1].Specular.c=0x0;
	NSVertex[2].Specular.c=0x0;
	NSVertex[3].Specular.c=0x0;

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	


	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	m_pd3dDevice->SetTexture(0,m_NsTexture.GetTexture());
	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,NSVertex,sizeof(TLVertex));	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	//*/

	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames  = 0L;

	static char strFrameStats[256];
	FLOAT fTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
	++dwFrames;

	// Update the scene stats once per second
	if( fTime - fLastTime > 1.0f )
	{
		float fFPS    = dwFrames / (fTime - fLastTime);
		fLastTime = fTime;
		dwFrames  = 0L;
		sprintf( strFrameStats, "%.02f fps", fFPS);		
	}

	// edith 2008.01.18 ShowState
	if(m_bEditorMode)
	{
		m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,0,0),strFrameStats);			

		static char strSectorPos[256];

		matrix *matPos=m_ViewCamera.GetMatPosition();		
		sprintf(strSectorPos,"sSector-X = %d , Sector-Y = %d",(int)(matPos->_41/(63.0f*500.0f)),(int)(matPos->_43/(63.0f*500.0f)));
		m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,0,0),strSectorPos);	

		sprintf(strSectorPos,"Viewer Position x=%.2f y=%.2f z=%.2f",matPos->_41,matPos->_42,matPos->_43);
		m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,0,0),strSectorPos);	

		m_dwAbleTextureMemory=m_pd3dDevice->GetAvailableTextureMem();

		sprintf(strSectorPos,"Total used texture memory %d ",m_dwStartRemainTextureMemory-m_dwAbleTextureMemory);
		m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,0,0),strSectorPos);		

		sprintf(strSectorPos,"Remain Texture memory %d ",m_dwAbleTextureMemory);
		m_pFont->DrawText( 2, 120, D3DCOLOR_ARGB(255,255,0,0),strSectorPos);		

		for (size_t i = 0; i < 10; ++i)
		{
			if(strlen(m_aszCustomMsg[i]) == 0)
				continue;

			m_pFont->DrawText( 2, 155 + i*20, D3DCOLOR_ARGB(255,255,0,0),m_aszCustomMsg[i]);	
		}
	}
}
bool BaseGraphicsLayer::TransformVector(vector3 &t,vector3 &vecResult,float &w)
{	
	DWORD dwClipWidth;
	DWORD dwClipHeight;
	
	
	dwClipWidth=m_d3dpp.BackBufferWidth/2;
	dwClipHeight=m_d3dpp.BackBufferHeight/2;	

	//dwClipWidth=256/2.0f;
	//dwClipHeight=256/2.0f;	


    
	matrix matWorld,matView,matProj;
	m_pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)&matWorld);
	m_pd3dDevice->GetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);
	//matrix matSet=matProj*matView;
	matrix matSet=matView*matProj;    
	
	float x,y,z;
	x=t.x;
	y=t.y;
	z=t.z;
	
    float xp = matSet._11*x + matSet._21*y + matSet._31*z + matSet._41;
    float yp = matSet._12*x + matSet._22*y + matSet._32*z + matSet._42;
    float zp = matSet._13*x + matSet._23*y + matSet._33*z + matSet._43;
    float wp = matSet._14*x + matSet._24*y + matSet._34*z + matSet._44;
	vector3 vecTansform;
    vecTansform.x= ( 1.0f + (xp/wp) ) * dwClipWidth;
    vecTansform.y= ( 1.0f - (yp/wp) ) * dwClipHeight;
    vecTansform.z= zp / wp;
    w = wp;   	

	vecResult=vecTansform;
	if( 0.0f < vecTansform.x &&
		vecTansform.x < m_d3dpp.BackBufferWidth &&
		0.0f < vecTansform.y &&
		vecTansform.y < m_d3dpp.BackBufferHeight)
	{
		return true;
	}
	return false;
}

bool BaseGraphicsLayer::TransformVector(const vector3 &vSource, const matrix& mTransform, vector3 &vResult,float &w)
{
	DWORD dwClipWidth;
	DWORD dwClipHeight;

	dwClipWidth=m_d3dpp.BackBufferWidth/2;
	dwClipHeight=m_d3dpp.BackBufferHeight/2;	

	float x,y,z;
	x=vSource.x;
	y=vSource.y;
	z=vSource.z;

	float xp = mTransform._11*x + mTransform._21*y + mTransform._31*z + mTransform._41;
	float yp = mTransform._12*x + mTransform._22*y + mTransform._32*z + mTransform._42;
	float zp = mTransform._13*x + mTransform._23*y + mTransform._33*z + mTransform._43;
	float wp = mTransform._14*x + mTransform._24*y + mTransform._34*z + mTransform._44;
	vector3 vecTansform;
	vecTansform.x= ( 1.0f + (xp/wp) ) * dwClipWidth;
	vecTansform.y= ( 1.0f - (yp/wp) ) * dwClipHeight;
	vecTansform.z= zp / wp;
	w = wp;   	

	vResult=vecTansform;
	if( 0.0f < vecTansform.x &&
		vecTansform.x < m_d3dpp.BackBufferWidth &&
		0.0f < vecTansform.y &&
		vecTansform.y < m_d3dpp.BackBufferHeight)
	{
		return true;
	}
	return false;
}

void BaseGraphicsLayer::ManyTransformVector(vector3 *t, vector3 *result, float *w)
{
	/*
	DWORD dwClipWidth=m_d3dpp.BackBufferWidth/2;
	DWORD dwClipHeight=m_d3dpp.BackBufferHeight/2;
    
	matrix matView,matProj;
	m_pd3dDevice->GetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);
	//matrix matSet=matProj*matView;
	matrix matSet=matView*matProj;    
	
	float x,y,z;
	x=t.x;
	y=t.y;
	z=t.z;
	
    float xp = matSet._11*x + matSet._21*y + matSet._31*z + matSet._41;
    float yp = matSet._12*x + matSet._22*y + matSet._32*z + matSet._42;
    float zp = matSet._13*x + matSet._23*y + matSet._33*z + matSet._43;
    float wp = matSet._14*x + matSet._24*y + matSet._34*z + matSet._44;
	vector3 vecTansform;
    vecTansform.x= ( 1.0f + (xp/wp) ) * dwClipWidth;
    vecTansform.y= ( 1.0f - (yp/wp) ) * dwClipHeight;
    vecTansform.z= zp / wp;
    w = wp;   	

	vecResult=vecTansform;
	if( 0.0f < vecTansform.x &&
		vecTansform.x < m_d3dpp.BackBufferWidth &&
		0.0f < vecTansform.y &&
		vecTansform.y < m_d3dpp.BackBufferHeight)
	{
		return true;
	}	
	return false;
	*/
}

void BaseGraphicsLayer::RectFlip(std::vector<RECT> &RectList)
{
	for(int i=0;i<(int)RectList.size();i++)
	{
		m_pd3dDevice->Present(&RectList[i],&RectList[i],NULL,NULL);
	}
}

bool BaseGraphicsLayer::ProjectiveTextureMapping(vector3 vecPoint, float &fTu, float &fTv)
{
	vector3 vecFrontPlaneNormal=(m_ViewCamera.m_vecFrustumNear[1]-m_ViewCamera.m_vecFrustumNear[0])^(m_ViewCamera.m_vecFrustumNear[2]-m_ViewCamera.m_vecFrustumNear[1]);
	vecFrontPlaneNormal.Normalize();	
	//vecFrontPlaneNormal=-vecFrontPlaneNormal;

	vector3 vecPos=m_ViewCamera.GetMatPosition()->GetLoc();

	float fInter=CIntersection::PointFromPlane(vecFrontPlaneNormal,vecPos,vecPoint);

	// TU
	float fUInter=CIntersection::PointFromPlane(m_ViewCamera.m_vecCenterAxis[0],vecPos,vecPoint);

	// TV
	float fVInter=CIntersection::PointFromPlane(m_ViewCamera.m_vecCenterAxis[1],vecPos,vecPoint);

	float fNormalSize=tanf(3.14159f/6.0f)*fInter;

	fTu=fUInter/fNormalSize;
	fTu=fTu*0.5f+0.5f;

	float fAspect=(float)m_lScreenSx/(float)m_lScreenSy;

	fNormalSize=tanf((3.14159f/6.0f))*fInter*fAspect;
	fTv=fVInter/fNormalSize;	
	fTv=fTv*0.5f+0.5f;

	vecFrontPlaneNormal^vector3(0.0f,1.0f,0.0f);

	return false;
	
	/*
	if(fInter>=0.0f)
		return true;
	return false;
	*/
}

void BaseGraphicsLayer::SetCustomMsg(size_t index, const char* sz)
{
	if (index >= 10)
	{
		return;
	}

	strncpy(m_aszCustomMsg[index], sz, 299);
	m_aszCustomMsg[index][299] = '\0';
}

void BaseGraphicsLayer::DrawText(float x, float y, DWORD dwColor, char*szText)
{
	if (NULL == m_pFont)
	{
		return;
	}

	m_pFont->DrawText(x, y, dwColor, szText);
}