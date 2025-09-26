// GlareTexture.cpp: implementation of the CGlareTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "GlareTexture.h"
#include "BaseGraphicsLayer.h"

#define CV_WORLDVIEWPROJ_0   0
#define CV_WORLDVIEWPROJ_1   1
#define CV_WORLDVIEWPROJ_2   2
#define CV_WORLDVIEWPROJ_3   3

#define CV_UV_OFFSET_TO_USE  4

#define CV_T0_BASE           8
#define CV_T1_BASE          13
#define CV_T2_BASE          18
#define CV_T3_BASE          23

#define CV_UV_T0_NO_OFFSET   8
#define CV_UV_T0_TYPE1       9
#define CV_UV_T0_TYPE2      10
#define CV_UV_T0_TYPE3      11
#define CV_UV_T0_TYPE4      12

#define CV_UV_T1_NO_OFFSET  13
#define CV_UV_T1_TYPE1      14
#define CV_UV_T1_TYPE2      15
#define CV_UV_T1_TYPE3      16
#define CV_UV_T1_TYPE4      17

#define CV_UV_T2_NO_OFFSET  18
#define CV_UV_T2_TYPE1      19
#define CV_UV_T2_TYPE2      20
#define CV_UV_T2_TYPE3      21
#define CV_UV_T2_TYPE4      22

#define CV_UV_T3_NO_OFFSET  23
#define CV_UV_T3_TYPE1      24
#define CV_UV_T3_TYPE2      25
#define CV_UV_T3_TYPE3      26
#define CV_UV_T3_TYPE4      27




char strBlurVertexShader[]=
"vs.1.1\n"
"dp4 oPos.x, v0, c[0]\n"
"dp4 oPos.y, v0, c[1]\n"
"dp4 oPos.z, v0, c[2]\n"
"dp4 oPos.w, v0, c[3]\n"

"mov a0.x, c[4]\n"

"add oT0, v1, c[a0.x + 8]\n"
"add oT1, v1, c[a0.x + 13]\n"
"add oT2, v1, c[a0.x + 18]\n"
"add oT3, v1, c[a0.x + 23]\n";

char strBlurPixelShader[]=
"ps.1.1\n"
//"def c0, 0.0f, 0.0f, 1.0f, 0.0f\n"
//"def c0, 0.25f, 0.25f, 0.25f, 0.25f\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"
"tex t3\n"
//"mov r0,c0\n";
"mul r0, c0, t0\n"
"mad r0, c0, t1, r0\n"
"mad r0, c0, t2, r0\n"
"mad r0, c0, t3, r0\n";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECT3DSURFACE8 CGlareTexture::m_pRenderZBuffer=NULL;	

CGlareTexture::CGlareTexture()
{

}

CGlareTexture::~CGlareTexture()
{

}

void CGlareTexture::GenerateGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DBASETEXTURE8 pTexture,vector3 vecNeighbor,int nDepth)
{	
	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;

	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	long QUADVERTEX2FVF=D3DFVF_XYZRHW | D3DFVF_TEX4;


	/*
	CreateAndWriteUVOffsets(m_nSize,m_nSize);

	matrix matOldWorld,matOldProjection,matOldView;

	pd3dDevice->GetTransform(D3DTS_WORLD,matOldWorld);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->GetTransform(D3DTS_VIEW,matOldView);
	*/
	
	//pd3dDevice->EndScene();
	////////////////////

	/*
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;
	D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);
	offset.x = 2.0f;
	pd3dDevice->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pd3dDevice->SetVertexShader(m_BlurVertexShader);
	pd3dDevice->SetStreamSource(0,m_pVertexBuffer,sizeof(QuadVertex));

	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	pd3dDevice->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	*/

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	pd3dDevice->SetPixelShader(m_BlurPixelShader);
	pd3dDevice->SetVertexShader(QUADVERTEX2FVF);

	QuadVertex2 pVertex[4];		

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)m_nSize;
	pVertex[3].v.x=(float)m_nSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)m_nSize;
	pVertex[2].v.y=(float)m_nSize;	
		
	pVertex[0].tu0=0.0f;
	pVertex[1].tu0=0.0f;	
	pVertex[0].tu1=0.0f;
	pVertex[1].tu1=0.0f;
	pVertex[0].tu2=0.0f;
	pVertex[1].tu2=0.0f;
	pVertex[0].tu3=0.0f;
	pVertex[1].tu3=0.0f;

	float fXPerPixel=(1.0f/m_nSize)*0.5f*(800.0f/1024.0f);
	float fYPerPixel=(1.0f/m_nSize)*0.5f*(600.0f/1024.0f);

	pVertex[3].tu0=800.0f/1024.0f;//-fXPerPixel;
	pVertex[2].tu0=800.0f/1024.0f;//-fXPerPixel;

	pVertex[3].tu1=800.0f/1024.0f+fXPerPixel*2.0f;
	pVertex[2].tu1=800.0f/1024.0f+fXPerPixel*2.0f;

	pVertex[3].tu2=800.0f/1024.0f;//-fXPerPixel;
	pVertex[2].tu2=800.0f/1024.0f;//-fXPerPixel;

	pVertex[3].tu3=800.0f/1024.0f+fXPerPixel*2.0f;
	pVertex[2].tu3=800.0f/1024.0f+fXPerPixel*2.0f;

	pVertex[1].tv0=0.0f;
	pVertex[3].tv0=0.0f;
	pVertex[1].tv1=0.0f;
	pVertex[3].tv1=0.0f;
	pVertex[1].tv2=0.0f;
	pVertex[3].tv2=0.0f;
	pVertex[1].tv3=0.0f;
	pVertex[3].tv3=0.0f;

	pVertex[0].tv0=600.0f/1024.0f;//-fYPerPixel;
	pVertex[2].tv0=600.0f/1024.0f;//-fYPerPixel;	
	pVertex[0].tv1=600.0f/1024.0f;//-fYPerPixel;
	pVertex[2].tv1=600.0f/1024.0f;//-fYPerPixel;

	pVertex[0].tv2=600.0f/1024.0f+fYPerPixel*2.0f;
	pVertex[2].tv2=600.0f/1024.0f+fYPerPixel*2.0f;
	pVertex[0].tv3=600.0f/1024.0f+fYPerPixel*2.0f;
	pVertex[2].tv3=600.0f/1024.0f+fYPerPixel*2.0f;
	

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;		
	}

	float fConst[4];
	fConst[0]=vecNeighbor.x;
	fConst[1]=vecNeighbor.y;
	fConst[2]=vecNeighbor.z;
	fConst[3]=1.0f;

	pd3dDevice->SetPixelShaderConstant(0,fConst, 1);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	for(i=0;i<nDepth;i++)
	{
		pd3dDevice->SetRenderTarget(m_pRenderSurface[i%2],m_pRenderZBuffer);
		//pd3dDevice->SetRenderTarget(m_pRenderSurface[i%2],NULL);

		D3DVIEWPORT8 viewData = { 0, 0, m_nSize, m_nSize, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);					
		//pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0xFF, 0x0, 0x0 ), 1.0, 0);
		//pd3dDevice->BeginScene();

		if(i>0)
		{
			fXPerPixel=(1.0f/m_nSize)*0.5f;
			fYPerPixel=(1.0f/m_nSize)*0.5f;

			pVertex[3].tu0=1.0f+fXPerPixel;
			pVertex[2].tu0=1.0f+fXPerPixel;

			pVertex[3].tu1=1.0f+fXPerPixel*2.0f;
			pVertex[2].tu1=1.0f+fXPerPixel*2.0f;

			pVertex[3].tu2=1.0f+fXPerPixel;
			pVertex[2].tu2=1.0f+fXPerPixel;

			pVertex[3].tu3=1.0f+fXPerPixel*2.0f;
			pVertex[2].tu3=1.0f+fXPerPixel*2.0f;

			pVertex[0].tv0=1.0f+fYPerPixel;
			pVertex[2].tv0=1.0f+fYPerPixel;	
			pVertex[0].tv1=1.0f+fYPerPixel;
			pVertex[2].tv1=1.0f+fYPerPixel;

			pVertex[0].tv2=1.0f+fYPerPixel*2.0f;
			pVertex[2].tv2=1.0f+fYPerPixel*2.0f;
			pVertex[0].tv3=1.0f+fYPerPixel*2.0f;
			pVertex[2].tv3=1.0f+fYPerPixel*2.0f;
			
		}
		if(i==0)
		{			
			pd3dDevice->SetTexture(0,pTexture);
			pd3dDevice->SetTexture(1,pTexture);
			pd3dDevice->SetTexture(2,pTexture);
			pd3dDevice->SetTexture(3,pTexture);
		}
		else
		{
			pd3dDevice->SetTexture(0,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(1,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(2,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(3,m_pRenderTexture[(i-1)%2]);
		}
		//pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(QuadVertex2));
		//pd3dDevice->EndScene();
		m_nFinalRenderTexture=i%2;
	}
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

	//pd3dDevice->BeginScene();	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	/*
	pd3dDevice->SetTransform(D3DTS_WORLD,matOldWorld);
	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->SetTransform(D3DTS_VIEW,matOldView);
	*/

	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	

	pd3dDevice->SetPixelShader(NULL);
}

void CGlareTexture::Create(int nSize)
{	
	m_nSize=nSize;
	D3DDISPLAYMODE mode;
	BaseGraphicsLayer::GetDevice()->GetDisplayMode(&mode);		
	BaseGraphicsLayer::GetDevice()->CreateTexture(nSize,nSize,1,D3DUSAGE_RENDERTARGET,BaseGraphicsLayer::m_d3dpp.BackBufferFormat,D3DPOOL_DEFAULT,&m_pRenderTexture[0]);
	m_pRenderTexture[0]->GetSurfaceLevel(0, &m_pRenderSurface[0]);

	BaseGraphicsLayer::GetDevice()->CreateTexture(nSize,nSize,1,D3DUSAGE_RENDERTARGET,BaseGraphicsLayer::m_d3dpp.BackBufferFormat,D3DPOOL_DEFAULT,&m_pRenderTexture[1]);
	m_pRenderTexture[1]->GetSurfaceLevel(0, &m_pRenderSurface[1]);

	if(m_pRenderZBuffer==NULL)
		BaseGraphicsLayer::GetDevice()->CreateDepthStencilSurface(nSize,nSize,BaseGraphicsLayer::m_d3dpp.AutoDepthStencilFormat,D3DMULTISAMPLE_NONE,&m_pRenderZBuffer);		
	

	LPD3DXBUFFER pCode;	
    D3DXAssembleShader(strBlurPixelShader,strlen(strBlurPixelShader),0,NULL,&pCode,NULL);										 
	BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_BlurPixelShader);    
	pCode->Release();

	DWORD Declaration[] =
	{
		D3DVSD_STREAM( 0 ),					
		D3DVSD_REG(0,D3DVSDT_FLOAT3), // Position 
		D3DVSD_REG(1,D3DVSDT_FLOAT2), // Texture Coordinate			
		D3DVSD_END()
	};
	

	D3DXAssembleShader(strBlurVertexShader,strlen(strBlurVertexShader),0,NULL,&pCode,NULL);										 	

	BaseGraphicsLayer::GetDevice()->CreateVertexShader( Declaration, 
                                         (DWORD*)pCode->GetBufferPointer(),
                                         &m_BlurVertexShader, 0 );

	pCode->Release();


	BaseGraphicsLayer::GetDevice()->CreateVertexBuffer( 4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer);

	QuadVertex      *pBuff;
	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Lock(0, 4 * sizeof(QuadVertex),(BYTE**)&pBuff, 0);		
        for (int i = 0; i < 4; ++i)
        {
            pBuff->Position = D3DXVECTOR3((i==0 || i==3) ? -1.0f : 1.0f,
                                          (i<2)          ? -1.0f : 1.0f,
                                          0.0f);
			
		    pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? 0.0f : 1.0f, 
                                          (i<2)          ? 1.0f : 0.0f);
			/*

			pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? 0.0f : 1024.0f, 
                                          (i<2)          ? 1024.0f : 0.0f);			
										  */
			
		    pBuff++;
        }
        m_pVertexBuffer->Unlock();
    }
}

void CGlareTexture::CreateAndWriteUVOffsets(int width, int height)
{
	float const     noOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f}; 
    float const     noOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

    float const     kPerTexelWidth  = 1.0f/static_cast<float>(width);
    float const     kPerTexelHeight = 1.0f/static_cast<float>(height);
    float           s               = 0.5f;
    float const     eps             = 10.0e-4f;
    float const     rotAngle1       = D3DXToRadian( 0.0f );
    float const     rotAngle2       = rotAngle1 + D3DXToRadian(120.0f); 
    float const     rotAngle3       = rotAngle1 + D3DXToRadian(240.0f); 

    // Change filter kernel for 9-sample box filtering, but for edge-detection we are 
    // going to use interpolated texels.  Why?  Because we detect diagonal edges only
    // and the vertical and horizontal filtering seems to help.
        
    float const type1OffsetX[4] = { -s * kPerTexelWidth, 
                                    -s * kPerTexelWidth,  
                                     s * kPerTexelWidth,   
                                     s * kPerTexelWidth  };
    float const type1OffsetY[4] = { -s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                    -s * kPerTexelHeight };

    // we have to bring the 16 texel-sample-filter a bit closer to the center to avoid 
    // separation due to floating point inaccuracies.
    float const type2OffsetX[4] = { -.5f * kPerTexelWidth + eps,  
                                    -.5f * kPerTexelWidth + eps, 
                                    1.5f * kPerTexelWidth - eps, 
                                    1.5f * kPerTexelWidth - eps };
    float const type2OffsetY[4] = { -.5f * kPerTexelHeight+ eps, 
                                    1.5f * kPerTexelHeight- eps, 
                                    1.5f * kPerTexelHeight- eps, 
                                    -.5f * kPerTexelHeight+ eps };

    float const type3OffsetX[4] = {0.0f,  sinf(rotAngle1)*kPerTexelWidth,  
                                          sinf(rotAngle2)*kPerTexelWidth,  
                                          sinf(rotAngle3)*kPerTexelWidth  };
    float const type3OffsetY[4] = {0.0f, -cosf(rotAngle1)*kPerTexelHeight, 
                                         -cosf(rotAngle2)*kPerTexelHeight, 
                                         -cosf(rotAngle3)*kPerTexelHeight };

    s = 2.0f/3.0f;      // same as type 1, except s is different
    float const type4OffsetX[4] = { -s * kPerTexelWidth, 
                                    -s * kPerTexelWidth,  
                                     s * kPerTexelWidth,   
                                     s * kPerTexelWidth  };
    float const type4OffsetY[4] = { -s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                    -s * kPerTexelHeight };
    // write all these offsets to constant memory
    for (int i = 0; i < 8; ++i)
    {
        D3DXVECTOR4  noOffset(      noOffsetX[i],    noOffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type1Offset(type1OffsetX[i], type1OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type2Offset(type2OffsetX[i], type2OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type3Offset(type3OffsetX[i], type3OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type4Offset(type4OffsetX[i], type4OffsetY[i], 0.0f, 0.0f);

    	BaseGraphicsLayer::GetDevice()->SetVertexShaderConstant(CV_UV_T0_NO_OFFSET + 5*i, &noOffset,    1);
    	BaseGraphicsLayer::GetDevice()->SetVertexShaderConstant(CV_UV_T0_TYPE1     + 5*i, &type1Offset, 1);
    	BaseGraphicsLayer::GetDevice()->SetVertexShaderConstant(CV_UV_T0_TYPE2     + 5*i, &type2Offset, 1);
    	BaseGraphicsLayer::GetDevice()->SetVertexShaderConstant(CV_UV_T0_TYPE3     + 5*i, &type3Offset, 1);
    	BaseGraphicsLayer::GetDevice()->SetVertexShaderConstant(CV_UV_T0_TYPE4     + 5*i, &type4Offset, 1);
    }
}


void CGlareTexture::ProcedualGenerateGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DBASETEXTURE8 pTexture, vector3 vecNeighbor, int nDepth)
{
	if(pTexture)
		m_nFinalRenderTexture=0;

	long QUADVERTEX2FVF=D3DFVF_XYZRHW | D3DFVF_TEX4;
	
	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;

	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	pd3dDevice->SetVertexShader(QUADVERTEX2FVF);
	pd3dDevice->SetPixelShader(m_BlurPixelShader);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);


	float fConst[4];
	fConst[0]=vecNeighbor.x;
	fConst[1]=vecNeighbor.y;
	fConst[2]=vecNeighbor.z;
	fConst[3]=0.27f;

	pd3dDevice->SetPixelShaderConstant(0,fConst, 1 );

	QuadVertex2 pVertex[4];		

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)m_nSize;
	pVertex[3].v.x=(float)m_nSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)m_nSize;
	pVertex[2].v.y=(float)m_nSize;	
		
	pVertex[0].tu0=0.0f;
	pVertex[1].tu0=0.0f;	
	pVertex[0].tu1=0.0f;
	pVertex[1].tu1=0.0f;
	pVertex[0].tu2=0.0f;
	pVertex[1].tu2=0.0f;
	pVertex[0].tu3=0.0f;
	pVertex[1].tu3=0.0f;

	pVertex[1].tv0=0.0f;
	pVertex[3].tv0=0.0f;
	pVertex[1].tv1=0.0f;
	pVertex[3].tv1=0.0f;
	pVertex[1].tv2=0.0f;
	pVertex[3].tv2=0.0f;
	pVertex[1].tv3=0.0f;
	pVertex[3].tv3=0.0f;
	

	float fXPerPixel=(1.0f/m_nSize)*0.5f*(800.0f/1024.0f);
	float fYPerPixel=(1.0f/m_nSize)*0.5f*(600.0f/1024.0f);
	


	//pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);

	if(pTexture==NULL)
	{
		fXPerPixel=(1.0f/m_nSize)*0.5f;
		fYPerPixel=(1.0f/m_nSize)*0.5f;

		pVertex[3].tu0=1.0f;
		pVertex[2].tu0=1.0f;
		pVertex[3].tu1=1.0f;
		pVertex[2].tu1=1.0f;
		pVertex[3].tu2=1.0f;
		pVertex[2].tu2=1.0f;
		pVertex[3].tu3=1.0f;
		pVertex[2].tu3=1.0f;

		pVertex[0].tv0=1.0f;
		pVertex[2].tv0=1.0f;	
		pVertex[0].tv1=1.0f;
		pVertex[2].tv1=1.0f;
		pVertex[0].tv2=1.0f;
		pVertex[2].tv2=1.0f;
		pVertex[0].tv3=1.0f;
		pVertex[2].tv3=1.0f;

		/*

		pVertex[3].tu0=1.0f+fXPerPixel;
		pVertex[2].tu0=1.0f+fXPerPixel;
		pVertex[3].tu1=1.0f+fXPerPixel*2.0f;
		pVertex[2].tu1=1.0f+fXPerPixel*2.0f;
		pVertex[3].tu2=1.0f+fXPerPixel;
		pVertex[2].tu2=1.0f+fXPerPixel;
		pVertex[3].tu3=1.0f+fXPerPixel*2.0f;
		pVertex[2].tu3=1.0f+fXPerPixel*2.0f;

		pVertex[0].tv0=1.0f+fYPerPixel;
		pVertex[2].tv0=1.0f+fYPerPixel;	
		pVertex[0].tv1=1.0f+fYPerPixel;
		pVertex[2].tv1=1.0f+fYPerPixel;
		pVertex[0].tv2=1.0f+fYPerPixel*2.0f;
		pVertex[2].tv2=1.0f+fYPerPixel*2.0f;
		pVertex[0].tv3=1.0f+fYPerPixel*2.0f;
		pVertex[2].tv3=1.0f+fYPerPixel*2.0f;
		*/

		for(int i=0;i<nDepth;i++)
		{
			pd3dDevice->SetRenderTarget(m_pRenderSurface[(m_nFinalRenderTexture+i+1)%2],m_pRenderZBuffer);
			
			pd3dDevice->SetTexture(0,m_pRenderTexture[(m_nFinalRenderTexture+i)%2]);
			pd3dDevice->SetTexture(1,m_pRenderTexture[(m_nFinalRenderTexture+i)%2]);
			pd3dDevice->SetTexture(2,m_pRenderTexture[(m_nFinalRenderTexture+i)%2]);
			pd3dDevice->SetTexture(3,m_pRenderTexture[(m_nFinalRenderTexture+i)%2]);
			
		}
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(QuadVertex2));
		m_nFinalRenderTexture=(m_nFinalRenderTexture+i)%2;	
		//m_nFinalRenderTexture=i%2;
	}
	else
	{		
		pVertex[3].tu0=800.0f/1024.0f;//-fXPerPixel;
		pVertex[2].tu0=800.0f/1024.0f;//-fXPerPixel;
		pVertex[3].tu1=800.0f/1024.0f+fXPerPixel*2.0f;
		pVertex[2].tu1=800.0f/1024.0f+fXPerPixel*2.0f;
		pVertex[3].tu2=800.0f/1024.0f;//-fXPerPixel;
		pVertex[2].tu2=800.0f/1024.0f;//-fXPerPixel;
		pVertex[3].tu3=800.0f/1024.0f+fXPerPixel*2.0f;
		pVertex[2].tu3=800.0f/1024.0f+fXPerPixel*2.0f;
		
		pVertex[0].tv0=600.0f/1024.0f;//-fYPerPixel;
		pVertex[2].tv0=600.0f/1024.0f;//-fYPerPixel;	
		pVertex[0].tv1=600.0f/1024.0f;//-fYPerPixel;
		pVertex[2].tv1=600.0f/1024.0f;//-fYPerPixel;
		pVertex[0].tv2=600.0f/1024.0f+fYPerPixel*2.0f;
		pVertex[2].tv2=600.0f/1024.0f+fYPerPixel*2.0f;
		pVertex[0].tv3=600.0f/1024.0f+fYPerPixel*2.0f;
		pVertex[2].tv3=600.0f/1024.0f+fYPerPixel*2.0f;

		for(int i=0;i<nDepth;i++)
		{
			pd3dDevice->SetRenderTarget(m_pRenderSurface[i%2],m_pRenderZBuffer);			
			if(i==0)
			{			
				if(pTexture)
				{
					pd3dDevice->SetTexture(0,pTexture);
					pd3dDevice->SetTexture(1,pTexture);
					pd3dDevice->SetTexture(2,pTexture);
					pd3dDevice->SetTexture(3,pTexture);
				}
				else
				{				
					pd3dDevice->SetTexture(0,m_pRenderTexture[m_nFinalRenderTexture]);
					pd3dDevice->SetTexture(1,m_pRenderTexture[m_nFinalRenderTexture]);
					pd3dDevice->SetTexture(2,m_pRenderTexture[m_nFinalRenderTexture]);
					pd3dDevice->SetTexture(3,m_pRenderTexture[m_nFinalRenderTexture]);
				}
			}
			else
			{
				pd3dDevice->SetTexture(0,m_pRenderTexture[(i-1)%2]);
				pd3dDevice->SetTexture(1,m_pRenderTexture[(i-1)%2]);
				pd3dDevice->SetTexture(2,m_pRenderTexture[(i-1)%2]);
				pd3dDevice->SetTexture(3,m_pRenderTexture[(i-1)%2]);
			}			
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(QuadVertex2));
			m_nFinalRenderTexture=i%2;
		}
	}

	//pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	

	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	pd3dDevice->SetPixelShader(NULL);
}

void CGlareTexture::GenerateGlareTexture2(LPDIRECT3DDEVICE8 pd3dDevice,LPDIRECT3DBASETEXTURE8 pTexture,vector3 vecNeighbor,int nDepth)
{

	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;

	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);


	CreateAndWriteUVOffsets(m_nSize,m_nSize);

	matrix matOldWorld,matOldProjection,matOldView;

	pd3dDevice->GetTransform(D3DTS_WORLD,matOldWorld);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->GetTransform(D3DTS_VIEW,matOldView);


	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;
	D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);
	offset.x = 2.0f;
	pd3dDevice->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pd3dDevice->SetVertexShader(m_BlurVertexShader);
	pd3dDevice->SetStreamSource(0,m_pVertexBuffer,sizeof(QuadVertex));

	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	pd3dDevice->GetTransform(D3DTS_VIEW,&matView);

	for(int i=0;i<4;i++)
    {	    
        pd3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
    }

	pd3dDevice->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

	pd3dDevice->SetPixelShader(m_BlurPixelShader);
	pd3dDevice->SetVertexShader(m_BlurVertexShader);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	pd3dDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));

	float fBlurConst[4]={vecNeighbor.x,vecNeighbor.x,vecNeighbor.x,vecNeighbor.x};

	pd3dDevice->SetPixelShaderConstant(0,fBlurConst,1);

	for( i=0;i<nDepth;i++)
	{		
		//pd3dDevice->SetRenderTarget(m_pRenderSurface[i%2],NULL);		

		pd3dDevice->SetRenderTarget(m_pRenderSurface[i%2],m_pRenderZBuffer);

		//D3DVIEWPORT8 viewData = { 0, 0, m_nSize, m_nSize, 0.0f, 1.0f };
		//pd3dDevice->SetViewport(&viewData);

		if(i==0)
		{			
			pd3dDevice->SetTexture(0,pTexture);
			pd3dDevice->SetTexture(1,pTexture);
			pd3dDevice->SetTexture(2,pTexture);
			pd3dDevice->SetTexture(3,pTexture);
		}
		else
		{
			pd3dDevice->SetTexture(0,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(1,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(2,m_pRenderTexture[(i-1)%2]);
			pd3dDevice->SetTexture(3,m_pRenderTexture[(i-1)%2]);
		}		
		
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
		
		m_nFinalRenderTexture=i%2;
	}

	for(i=0;i<4;i++)
    {	    
        pd3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
	    pd3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP);
    }
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	
	pd3dDevice->SetTransform(D3DTS_WORLD,matOldWorld);
	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->SetTransform(D3DTS_VIEW,matOldView);
	

	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	

	pd3dDevice->SetPixelShader(NULL);

}
