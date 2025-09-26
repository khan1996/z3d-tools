// FullSceneEffect.cpp: implementation of the CFullSceneEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "FullSceneEffect.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
enum FULLSCENE_EFFECTS {
	FULLSCENE_MOTIONBLUR = 0,
	FULLSCENE_BRIGHT,
	FULLSCENE_BLACK,
	FULLSCENE_CONTRAST,
	FULLSCENE_LEVEL,
	FULLSCENE_GRAYLEVEL,
	FULLSCENE_EDGE,
	FULLSCENE_NEGATIVE,
	FULLSCENE_SEPIA,
	FULLSCENE_SOLAR,
	FULLSCENE_NUMS,	
};
#define FULLSCENE_BLURTEXNUM 6
#define FULLSCENE_BLURTEXSIZE 1024

class CFullSceneEffect  
{
protected:
	LPDIRECT3DDEVICE8	m_pDevice;

	LPDIRECT3DTEXTURE8	m_pBlurTexture[FULLSCENE_BLURTEXNUM];
	LPDIRECT3DSURFACE8	m_pBlurTextureSurf[FULLSCENE_BLURTEXNUM];

	LPDIRECT3DTEXTURE8  m_pRenderTexture;
	LPDIRECT3DSURFACE8	m_pRenderTextureSurface;
	LPDIRECT3DSURFACE8	m_pRenderZSurface;

	LPDIRECT3DSURFACE8	m_pFrameBuffer;
	LPDIRECT3DSURFACE8	m_pFrameZBuffer;
	bool m_bFlag[FULLSCENE_NUMS];

public:
	
	void Init(LPDIRECT3DDEVICE8 lpDevice);
	
	void BeginRender();
	void EndRender();
	void Render();
	void Update();
	void SetFlag(int i,bool bFlag) { m_bFlag[i] = bFlag;}

	
};


*/
CFullSceneEffect::CFullSceneEffect() : m_pDevice(0),m_pRenderZSurface(0),m_pFrameBuffer(0),m_pFrameZBuffer(0)
{

	for(int i = 0; i < FULLSCENE_BLURTEXNUM + 1; i++ ) 
	{
		m_pRenderTexture[i] = NULL;
		m_pRenderTextureSurface[i] = NULL;
	

	}

	memset(m_bFlag,0,sizeof(bool) * FULLSCENE_NUMS);
	memset(m_bLockFlag,0,sizeof(bool) * FULLSCENE_NUMS);

	m_iFrameWidth = 0;
	m_iFrameHeight = 0;	
	m_pVertexBuffer = NULL;
	m_iBlurTexUpdateFrame = 1;	// BlurTex Update 간격
	m_iBlurTexUpdateCount = 0;
	
	m_lstEffects.clear();
	m_bPixelShader = false;

	m_iBlurFirstUpdate = -1;


	m_bFullSceneEffect = true;

	m_iRenderWidth = 0;
	m_iRenderHeight = 0;


	m_pRenderZSurface = NULL;
	m_pFrameBuffer = NULL;
	m_pFrameZBuffer = NULL;
	
	m_pVertexBuffer = NULL;

	m_iMaxCountNum = 0;


}

CFullSceneEffect::~CFullSceneEffect()
{

	for(int i = 0; i < FULLSCENE_BLURTEXNUM + 1; i++ )  {
		if(m_pRenderTexture[i]) {
			m_pRenderTexture[i]->Release();
			m_pRenderTexture[i] = NULL;
		}
		if(m_pRenderTextureSurface[i] ) {
			m_pRenderTextureSurface[i]->Release();
			m_pRenderTextureSurface[i] = NULL;
		}
	}

	if(m_pRenderZSurface) {
		m_pRenderZSurface->Release();
		m_pRenderZSurface = NULL;
	}
	if(m_pFrameBuffer) {
		m_pFrameBuffer->Release();
		m_pFrameBuffer = NULL;
	}
	if(m_pFrameZBuffer) {
		m_pFrameZBuffer->Release();
		m_pFrameZBuffer = NULL;
	}
	if(m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	for(int i=0;i <(int)(m_lstEffects.size());i++) 
	{
		if(m_lstEffects[i] != NULL)
		{
			delete m_lstEffects[i];
			m_lstEffects[i] = NULL;
		}
	}
	m_lstEffects.clear();


}
void CFullSceneEffect::Init(LPDIRECT3DDEVICE8	lpDevice)
{
	m_iCurrentTextureNum = 0;
	m_iMaxCountNum = 0;

	if(m_bFullSceneEffect) {
		m_pDevice = lpDevice;

		m_pDevice->GetDeviceCaps(&m_pCaps);
		if(m_pCaps.PixelShaderVersion <= 1.0f)
		{
			m_bPixelShader = false;

		}
		else
			m_bPixelShader = true;

		m_pDevice->GetRenderTarget(&m_pFrameBuffer);

		D3DSURFACE_DESC FrameDecl;
		m_pFrameBuffer->GetDesc( &FrameDecl );
		
		m_iFrameWidth = FrameDecl.Width;
		m_iFrameHeight = FrameDecl.Height;

		D3DXCreateTexture(m_pDevice, m_iFrameWidth,m_iFrameHeight, 1, 
					D3DUSAGE_RENDERTARGET, BaseGraphicsLayer::m_d3dpp.BackBufferFormat,
					D3DPOOL_DEFAULT, &m_pRenderTexture[0]);

		// Render Texture Surface Get
		m_pRenderTexture[0]->GetSurfaceLevel(0, &m_pRenderTextureSurface[0]);
		m_pRenderTextureSurface[0]->GetDesc( &FrameDecl );
		
		int iTexWidth = FrameDecl.Width;
		int iTexHeight = FrameDecl.Height;

/*		
		if(BaseGraphicsLayer::GetGraphicCard() < 5)
		{
			if(iTexWidth > 1024 || iTexHeight > 1024) {	// 1024 이상은 너무 느려져서 쓸수 없다.
				m_bFullSceneEffect = false;
				if(m_pRenderTexture[0]) {
					m_pRenderTexture[0]->Release();
					m_pRenderTexture[0] = NULL;	
				}
				if(m_pRenderTextureSurface[0]) {
					m_pRenderTextureSurface[0]->Release();
					m_pRenderTextureSurface[0] = NULL;
				}
				return;
			}
		}
*/

		m_iRenderWidth = iTexWidth;
		m_iRenderHeight = iTexHeight;

		for(int iBlur = 0;iBlur < FULLSCENE_BLURTEXNUM; iBlur++) {
			D3DXCreateTexture(m_pDevice, iTexWidth,iTexHeight, 1, 
				D3DUSAGE_RENDERTARGET, BaseGraphicsLayer::m_d3dpp.BackBufferFormat,
				D3DPOOL_DEFAULT, &m_pRenderTexture[iBlur+1]);

			// Render Texture Surface Get
			m_pRenderTexture[iBlur+1]->GetSurfaceLevel(0, &m_pRenderTextureSurface[iBlur+1]);
		
		}	
		//Depth Buffer Create
		  
		m_pDevice->CreateDepthStencilSurface(iTexWidth, 
											iTexHeight,
										BaseGraphicsLayer::m_d3dpp.AutoDepthStencilFormat,
										D3DMULTISAMPLE_NONE,
										&m_pRenderZSurface);

		// Frame Surface Backup	
		m_pDevice->GetDepthStencilSurface(&m_pFrameZBuffer);

		m_pDevice->GetViewport(&m_FrameViewport);
		
		// 필터링 할때 경계색을 일정하게 유지 하기 위해 빈 공백 준다.
/*
		m_RenderViewport.X = 0 + 1;
		m_RenderViewport.Y = 0 + 1;
		m_RenderViewport.Width = iTexWidth - 2;
		m_RenderViewport.Height = iTexHeight- 2;
		m_RenderViewport.MinZ = 0.0f;
		m_RenderViewport.MaxZ = 1.0f;
*/

		m_RenderViewport.X = 0;
		m_RenderViewport.Y = 0;
		m_RenderViewport.Width = iTexWidth;
		m_RenderViewport.Height = iTexHeight;
		m_RenderViewport.MinZ = 0.0f;
		m_RenderViewport.MaxZ = 1.0f;

		m_ClearViewport.X = 0;
		m_ClearViewport.Y = 0;
		m_ClearViewport.Width = iTexWidth;
		m_ClearViewport.Height = iTexHeight;
		m_ClearViewport.MinZ = 0.0f;
		m_ClearViewport.MaxZ = 1.0f;
		
		CreateVertexBuffer();

		m_iBlurTexUpdateFrame = 1;	// BlurTex Update 간격
		m_iBlurTexUpdateCount = 0;
		if(m_bPixelShader) {
		// Filter Init
			for(int i = 0; i < FULLSCENE_NUMS;i++ )
			{
				CFullSceneShader *m_pShader = NULL;

				switch(i) {
				case FULLSCENE_BRIGHT:
					m_pShader = new CFullSceneShader("Brightness");
					m_lstEffects.push_back(m_pShader);
					break;
				case FULLSCENE_BLACK:
					m_pShader = new CFullSceneShader("Black");
					m_lstEffects.push_back(m_pShader);

					break;
	/*			case FULLSCENE_CONTRAST:
					m_pShader = new CFullSceneShader("Contrast");
					m_lstEffects.push_back(m_pShader);

					break;*/
	/*			case FULLSCENE_LEVEL:
					m_pShader = new CFullSceneShader("Levels");
					m_lstEffects.push_back(m_pShader);
					break;*/

			/*	case FULLSCENE_GRAYLEVEL:
					m_pShader = new CFullSceneShader("LevelsGrayScale.psh");
					m_lstEffects.push_back(m_pShader);

					break;*/
				case FULLSCENE_EDGE:
					m_pShader = new CFullSceneShader("Edges");
					m_lstEffects.push_back(m_pShader);

					break;
				case FULLSCENE_NEGATIVE:
					m_pShader = new CFullSceneShader("Negative");
					m_lstEffects.push_back(m_pShader);

					break;
				case FULLSCENE_SEPIA:
					m_pShader = new CFullSceneShader("Sepia");
					m_lstEffects.push_back(m_pShader);

					break;
		/*		case FULLSCENE_SOLAR:
					m_pShader = new CFullSceneShader("Solarize");
					m_lstEffects.push_back(m_pShader);

					break;*/
				case FULLSCENE_EDGE2:
					m_pShader = new CFullSceneShader("Edge2");
					m_lstEffects.push_back(m_pShader);
					break;
				case FULLSCENE_EDGE3:
					m_pShader = new CFullSceneShader("Edge3");
					m_lstEffects.push_back(m_pShader);
					break;

				case FULLSCENE_SHARPEN:
					m_pShader = new CFullSceneShader("Sharpen");
					m_lstEffects.push_back(m_pShader);
					break;
				case FULLSCENE_SATURATION:
					m_pShader = new CFullSceneShader("saturation");
					m_lstEffects.push_back(m_pShader);
					break;
				case FULLSCENE_SATURATION2:
					m_pShader = new CFullSceneShader("saturation2");
					m_lstEffects.push_back(m_pShader);
					break;
		/*		case FULLSCENE_BLUR:
					m_pShader = new CFullSceneShader("Blur.psh");
					m_lstEffects.push_back(m_pShader);
					break;
		*/
				default:
					break;

				}

			}
		}
	}


}
void CFullSceneEffect::CreateVertexBuffer()
{
	m_pDevice->CreateVertexBuffer(sizeof(TLVertex) * 4,D3DUSAGE_WRITEONLY,TLVERTEXFVF,D3DPOOL_MANAGED,&m_pVertexBuffer);
	TLVertex *pVertex = NULL;

	m_pVertexBuffer->Lock(0,0,(BYTE **)&pVertex,0);
	
	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=m_iFrameWidth;
	pVertex[3].v.x=m_iFrameWidth;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;// 
	pVertex[0].v.y=m_iFrameHeight;
	pVertex[2].v.y=m_iFrameHeight;

	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		
	
	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0xffffffff;		
	}	

	m_pVertexBuffer->Unlock();

	
}
HRESULT CFullSceneEffect::BeginRender(DWORD dwClearColor)
{
	if(m_bFullSceneEffect) {

		HRESULT hr = m_pDevice->SetRenderTarget(m_pRenderTextureSurface[m_iCurrentTextureNum], m_pRenderZSurface);//m_pFrameZBuffer);
		m_pDevice->SetViewport(&m_ClearViewport);
		
		m_pDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, dwClearColor, 1.0f, 0 );
		m_pDevice->SetViewport(&m_RenderViewport);
		return hr;
	}
	return 0;
}
HRESULT CFullSceneEffect::EndRender()
{
	if(m_bFullSceneEffect) {

		HRESULT hr = m_pDevice->SetRenderTarget(m_pFrameBuffer, m_pFrameZBuffer);
		m_pDevice->SetViewport(&m_FrameViewport);

		return hr;
	}
	return 0;
}

void CFullSceneEffect::Render(int iValue) {
	if(m_bFullSceneEffect) {

		for (int i = 0; i < 4; ++i)
		{
			m_pDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			m_pDevice->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

			m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE);
			m_pDevice->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
			m_pDevice->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
		}
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);

		m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		
		
		/////////////////
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
		m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	    	
		m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
		m_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);



		m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,0);
		m_pDevice->SetTextureStageState(0 ,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		
		m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
		m_pDevice->SetRenderState(D3DRS_LOCALVIEWER,FALSE);
		m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		m_pDevice->SetVertexShader(TLVERTEXFVF);
		m_pDevice->SetStreamSource(0,m_pVertexBuffer,sizeof(TLVertex));

		m_pDevice->SetTexture(0,m_pRenderTexture[m_iCurrentTextureNum]);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

		if(m_bPixelShader) {	
			// FullScene  Filter
			for(int i=1;i <FULLSCENE_NUMS; i++ ) {
				if(m_bFlag[i] == true)
				{
					if(iValue == 0)
						m_lstEffects[i - 1]->Apply();
					else
						m_lstEffects[i - 1]->Apply(1);
				}
			}
		

			if(iValue == 1) {
				m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
				m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x66ffffff);
				
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	    	
				m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
			}

			if(m_bFlag[FULLSCENE_EDGE] == true || 
				m_bFlag[FULLSCENE_EDGE2] == true ||
				m_bFlag[FULLSCENE_EDGE3] == true ||
				m_bFlag[FULLSCENE_SHARPEN] == true)
			{
				m_pDevice->SetTexture(1,m_pRenderTexture[m_iCurrentTextureNum]);
				m_pDevice->SetTexture(2,m_pRenderTexture[m_iCurrentTextureNum]);
				m_pDevice->SetTexture(3,m_pRenderTexture[m_iCurrentTextureNum]);
				
			}
		}
		else {
			m_pDevice->SetPixelShader(0);

		}
		// Render Scene 
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		

		// FullScene MotionBlur	
		if(m_bFlag[FULLSCENE_MOTIONBLUR] == true && (m_bFlag[FULLSCENE_BRIGHT] != true) && (m_iBlurFirstUpdate >= 0)) {//&&( m_bFlag[FULLSCENE_SATURATION] != true)) {

			// FullScene  Filter
			if(m_bPixelShader) {
				for(int i=1;i <FULLSCENE_NUMS; i++ ) {
					if(m_bFlag[i] == true)
					{
						m_lstEffects[i - 1]->Apply(1);
					
					}
				}
			}
			else {
				m_pDevice->SetPixelShader(0);
			}
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x66ffffff);
			
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	    	
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);

			for(int iBlur = 0;iBlur < m_iMaxCountNum; iBlur++) {
				if(iBlur == m_iCurrentTextureNum)
					continue;

				m_pDevice->SetVertexShader(TLVERTEXFVF);
				m_pDevice->SetTexture(0,m_pRenderTexture[iBlur]);

				if(m_bPixelShader) {
					if(m_bFlag[FULLSCENE_EDGE] == true || 
						m_bFlag[FULLSCENE_EDGE2] == true ||
						m_bFlag[FULLSCENE_EDGE3] == true ||
						m_bFlag[FULLSCENE_SHARPEN] == true)
					{
						m_pDevice->SetTexture(1,m_pRenderTexture[iBlur]);
						m_pDevice->SetTexture(2,m_pRenderTexture[iBlur]);
						m_pDevice->SetTexture(3,m_pRenderTexture[iBlur]);

					}	
				}
			
				
				m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			}
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		}
		if(m_bPixelShader) {
			//Filter Pixel Shader Free
			for(int i=1;i <FULLSCENE_NUMS; i++ ) {
				if(m_bFlag[i] == true)
				{
					m_lstEffects[i - 1]->UnApply();
				}
			}
		}
		for (int i = 0; i < 4; ++i)
		{
			m_pDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			m_pDevice->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

			m_pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
			m_pDevice->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
			m_pDevice->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP);
		}

		m_pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);


		Update();
	}	
	
	

}
void CFullSceneEffect::Update()
{
	m_iCurrentTextureNum++;
	m_iMaxCountNum++;
	if(m_iCurrentTextureNum >= FULLSCENE_BLURTEXNUM +1)
		m_iCurrentTextureNum = 0;
	if(m_iMaxCountNum >= FULLSCENE_BLURTEXNUM +1)
		m_iMaxCountNum  = FULLSCENE_BLURTEXNUM +1;


/*	*/if(m_bFlag[FULLSCENE_MOTIONBLUR] == true) {
		/*
		m_iBlurTexUpdateCount++;

		if(m_iBlurTexUpdateCount == m_iBlurTexUpdateFrame) {	// Update Blur Texture
			m_iBlurFirstUpdate++;
			m_iBlurTexUpdateCount = 0;
			D3DXLoadSurfaceFromSurface(
				 m_pBlurTextureSurf[m_iCurrentBlurTex],
				 NULL,
				 NULL,
				 m_pRenderTextureSurface,
				 NULL,
				 NULL,
				 D3DX_DEFAULT,
			0);

			m_iCurrentBlurTex++;
			if(m_iCurrentBlurTex >= FULLSCENE_BLURTEXNUM)
				m_iCurrentBlurTex = 0;*/
			m_iBlurFirstUpdate++;
			if(m_iBlurFirstUpdate >= FULLSCENE_BLURTEXNUM)
				m_iBlurFirstUpdate = FULLSCENE_BLURTEXNUM;

		
			
	}
	else {
//		m_iCurrentBlurTex = 0;
		m_iBlurFirstUpdate = -1;
	}
}
