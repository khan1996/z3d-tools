// HazeScene.cpp: implementation of the CHazeScene class.
//
//////////////////////////////////////////////////////////////////////

#include "HazeScene.h"
#include "BaseGraphicsLayer.h"
#include "BaseDataDefine.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHazeScene::CHazeScene()
{

}

CHazeScene::~CHazeScene()
{

}

void CHazeScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	/*
	TLVertex pVertex[8];		
	float fTextureSize=256.0f;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=fTextureSize;
	pVertex[2].v.y=fTextureSize;		

	static float fasdf=0.0f;
		
	pVertex[0].tu=0.0f+fasdf;
	pVertex[1].tu=0.0f+fasdf;

	pVertex[3].tu=1.0f+fasdf;
	pVertex[2].tu=1.0f+fasdf;

	pVertex[1].tv=0.0f+fasdf;
	pVertex[3].tv=0.0f+fasdf;

	pVertex[0].tv=1.0f+fasdf;
	pVertex[2].tv=1.0f+fasdf;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0xffffffff;
	}

	for(i=0;i<4;i++)
	{		
		pVertex[i+4].v=pVertex[i].v+vector3(512.0f,0.0f,0.0f);
		pVertex[i+4].w=pVertex[i].w;
		pVertex[i+4].Diffuse.c=pVertex[i].Diffuse.c;
		pVertex[i+4].Specular.c=pVertex[i].Specular.c;
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	

	RECT rc={100,100,100+512,100+512};

	pd3dDevice->SetTexture(0,CRenderTargetTexture::GetTexture(pd3dDevice,rc));

	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,4,pVertex,sizeof(TLVertex));	
	WORD pIndices[8];
	pIndices[0]=0;
	pIndices[1]=1;
	pIndices[2]=2;
	pIndices[3]=3;

	pIndices[4]=4;
	pIndices[5]=5;
	pIndices[6]=6;
	pIndices[7]=7;

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,8,2,pIndices,D3DFMT_INDEX16,pVertex,sizeof(TLVertex));
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	*/
}

void CHazeScene::Create(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(GF3OPTION)
	{			
		char strShaderPath[256];
		
		sprintf(strShaderPath,"%s%s",SHADERPATH,"SampleFilter.psh");

		LPD3DXBUFFER pCode;		
		D3DXAssembleShaderFromFile(strShaderPath,0,NULL,&pCode,NULL);    

		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),
                                     &m_dwSampleFilterPixelShader);
		pCode->Release();			
		
	}		
}
