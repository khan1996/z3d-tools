// RenderEnvTexture.cpp: implementation of the CRenderEnvTexture class.
//
//////////////////////////////////////////////////////////////////////
//#include "DXUtil.h"
#include "RenderEnvTexture.h"
#include "BaseGraphicsLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CUBEMAP_RESOLUTION 256


////////±ÍÂú¾Æ~
D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
    return matView;
}

CRenderEnvTexture::CRenderEnvTexture()
{

}

CRenderEnvTexture::~CRenderEnvTexture()
{

}

void CRenderEnvTexture::RenderCubeMap(LPDIRECT3DDEVICE8 pd3dDevice)
{
	/////////³ªµµ ÀÌÁ¨ ±ÍÂú´Ù!!!!!!!!!!!!!!///////////////
	HRESULT hr;

    // Set the projection matrix for a field of view of 90 degrees
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2.0f , 1.0f, 1.0f, 100.0f );

	D3DXMATRIX matOldView,matOldProjection;

	pd3dDevice->GetTransform(D3DTS_VIEW,&matOldView);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);	

    // Get the current view matrix, to concat it with the cubemap view vectors
    D3DXMATRIX matViewDir( matOldView);
    matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

    // Render the six cube faces into the environment map
	if(m_bCube)    
        hr = m_pRenderToEnvMap->BeginCube( m_pCubeMap );    
	else
		hr = m_pRenderToEnvMap->BeginSphere( m_pSphereMap );
	
	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

    for( UINT i = 0; i < 6; i++ )
    {
        m_pRenderToEnvMap->Face( (D3DCUBEMAP_FACES) i );

        // Set the view transform for this cubemap surface
        D3DXMATRIX matView;
        matView = D3DUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES) i );
        D3DXMatrixMultiply( &matView, &matViewDir, &matView );

		//D3DXMatrixMultiply( &matView, &matView,&matViewDir);

        // Render the scene (except for the teapot)
        //RenderScene( &matView, &matProj, FALSE );
		pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);
		switch(i)
		{
			case 0:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				//RenderSkymap(pd3dDevice);		
				break;
			case 1:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				//RenderSkymap(pd3dDevice);		
				break;				
			case 2:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				break;
			case 3:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				break;
				
			case 4:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				//RenderSkymap(pd3dDevice);		
				break;
			case 5:
				pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, 0xff000000, 1.0f, 0 );	
				//RenderSkymap(pd3dDevice);		
				break;
		}						
		RenderSkymap(pd3dDevice);		
    }

	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

    m_pRenderToEnvMap->End();

	pd3dDevice->SetTransform(D3DTS_VIEW,&matOldView);    
	pd3dDevice->SetTransform(D3DTS_PROJECTION,&matOldProjection);
}

void CRenderEnvTexture::RenderSkymap(LPDIRECT3DDEVICE8 pd3dDevice)
{
	//D3DXMATRIX matWorld;
    //D3DXMatrixScaling( &matWorld, 1.0f, 1.0f, 1.0f );

	matrix matWorld;
	matWorld.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

    //pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );   

    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_MIRROR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_MIRROR );

    // Always pass Z-test, so we can avoid clearing color and depth buffers
    //pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );    
	////////////////////
	/////Render Sky Box/
	////////////////////


	float fSize=10.0f;
	LVertex Vertex[4];
	WORD Indices[6];
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);		
	
	pd3dDevice->SetTexture(1,NULL);	
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	Indices[0]=0;
	Indices[1]=2;
	Indices[2]=1;
	Indices[3]=0;
	Indices[4]=3;
	Indices[5]=2;

	Vertex[0].v=vector3(-fSize,-fSize,fSize);
	Vertex[0].tu=0.0f;
	Vertex[0].tv=1.0f;	
	Vertex[1].v=vector3(fSize,-fSize,fSize);
	Vertex[1].tu=1.0f;
	Vertex[1].tv=1.0f;
	Vertex[2].v=vector3(fSize,-fSize,-fSize);
	Vertex[2].tu=1.0f;
	Vertex[2].tv=0.0f;
	Vertex[3].v=vector3(-fSize,-fSize,-fSize);
	Vertex[3].tu=0.0f;
	Vertex[3].tv=0.0f;

	Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=0xffffffff;

	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->SetTexture(0,m_SkyTexture[4].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(-fSize,-fSize,fSize);	
	Vertex[0].tu=0.0;
	Vertex[0].tv=1.0;


	Vertex[1].v=vector3(fSize,-fSize,fSize);
	Vertex[1].tu=1.0;
	Vertex[1].tv=1.0;

	Vertex[2].v=vector3(fSize,fSize,fSize);
	Vertex[2].tu=1.0;
	Vertex[2].tv=0.0;

	Vertex[3].v=vector3(-fSize,fSize,fSize);	
	Vertex[3].tu=0.0;
	Vertex[3].tv=0.0;

	pd3dDevice->SetTexture(0,m_SkyTexture[0].GetTexture());

	

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(fSize,-fSize,fSize);
	Vertex[1].v=vector3(fSize,-fSize,-fSize);
	Vertex[2].v=vector3(fSize,fSize,-fSize);
	Vertex[3].v=vector3(fSize,fSize,fSize);

	Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=0xffff0000;
	pd3dDevice->SetTexture(0,m_SkyTexture[1].GetTexture());

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	


	Vertex[0].v=vector3(fSize,-fSize,-fSize);
	Vertex[1].v=vector3(-fSize,-fSize,-fSize);
	Vertex[2].v=vector3(-fSize,fSize,-fSize);
	Vertex[3].v=vector3(fSize,fSize,-fSize);

	Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=0xffffff00;
	pd3dDevice->SetTexture(0,m_SkyTexture[2].GetTexture());

	
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(-fSize,-fSize,-fSize);
	Vertex[1].v=vector3(-fSize,-fSize,fSize);
	Vertex[2].v=vector3(-fSize,fSize,fSize);
	Vertex[3].v=vector3(-fSize,fSize,-fSize);
	Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=0xff0000ff;
	pd3dDevice->SetTexture(0,m_SkyTexture[3].GetTexture());
	
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);	

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
    //pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
}

void CRenderEnvTexture::Create(LPDIRECT3DDEVICE8 pd3dDevice,char strSkyTexturename[256],char strTexturePath[256],bool bCube)
{
	D3DDISPLAYMODE mode;
	BaseGraphicsLayer::GetDevice()->GetDisplayMode(&mode);
	m_bCube=bCube;

	D3DXCreateRenderToEnvMap(pd3dDevice,CUBEMAP_RESOLUTION,mode.Format,TRUE,D3DFMT_D16,&m_pRenderToEnvMap);

	if(m_bCube)
	{		
		D3DXCreateCubeTexture(pd3dDevice,CUBEMAP_RESOLUTION,1,D3DUSAGE_RENDERTARGET,mode.Format,D3DPOOL_DEFAULT,&m_pCubeMap);
	}
	else
	{
		D3DXCreateTexture(pd3dDevice,CUBEMAP_RESOLUTION,CUBEMAP_RESOLUTION,1,D3DUSAGE_RENDERTARGET,mode.Format,D3DPOOL_DEFAULT,&m_pSphereMap);
	}

	CTexture::SetPath(strTexturePath);

	char strTextureName[256];
	for(int i=1;i<6;i++)
	{
		sprintf(strTextureName,"%s%d.dds",strSkyTexturename,i);
		m_SkyTexture[i-1].Load(strTextureName);
	}
	
}
