// ChristmasParticle.cpp: implementation of the CChristmasParticle class.
//
//////////////////////////////////////////////////////////////////////

// ChristmasParticle.cpp: implementation of the CChristmasParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "ChristmasParticle.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CChristmasParticle::CChristmasParticle()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;

	m_pImageTexture = NULL;
	
	m_fHeight = 0.0f;
//	m_bStart = false;
	m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vecUp = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vecRight = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_bPointSprite = true;



}

CChristmasParticle::~CChristmasParticle()
{
	if(m_pVertexBuffer != NULL) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer= NULL;
	}
	if(m_pIndexBuffer != NULL) {
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;

	}

	if(m_pImageTexture != NULL) {
		delete m_pImageTexture;
		m_pImageTexture = NULL;

	}
}

void CChristmasParticle::Init(int iNodesNum,char *strTexture) {
	
	
	m_pDevice = CSceneManager::GetDevice();
	// Point Sprite Check
	D3DCAPS8 CapsTest;
	
	m_pDevice->GetDeviceCaps(&CapsTest);
	
	if((float)(CapsTest.MaxPointSize) <= 32.0f)
		m_bPointSprite = false;
	else
		m_bPointSprite = true;


	m_bPointSprite = false;

	CTexture::SetPath(EFFECTTEXTUREPATH);

	m_pImageTexture = new CTexture;

	m_pImageTexture->Load(strTexture);

	BYTE *pImage = (BYTE *)m_pImageTexture->Lock();

	
	InitParticle(iNodesNum,"CParticle.dds");
	SetParticlePosition(pImage,32,32);
	m_pImageTexture->Unlock();

	if(m_bPointSprite == true ) {
		m_pDevice->CreateVertexBuffer(sizeof(CChristmasParticleVertex) * iNodesNum,D3DUSAGE_WRITEONLY,dwChristmasParticleVertexDecl,D3DPOOL_DEFAULT,&m_pVertexBuffer);
	}
	else {
		
		m_pDevice->CreateVertexBuffer(sizeof(CRainParticleVertex) * m_iVertexBufferNum,D3DUSAGE_WRITEONLY,dwRainParticleVertexDecl,D3DPOOL_DEFAULT,&m_pVertexBuffer);
		m_pDevice->CreateIndexBuffer(sizeof(WORD) * m_iIndexBufferNum,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16 ,D3DPOOL_DEFAULT,&m_pIndexBuffer);

		WORD *pIndex = NULL;
		m_pIndexBuffer->Lock(0,0,(BYTE **)&pIndex,0);
		int i;

		for( i= 0 ; i < m_iNodesNum; i++ ) {
			
			pIndex[(i*6)] = (i * 4)+2;
			pIndex[(i*6)+1] = (i * 4);
			pIndex[(i*6)+2] = (i * 4)+3;
			
			pIndex[(i*6)+3] = (i * 4)+3;
			pIndex[(i*6)+4] = (i * 4);
			pIndex[(i*6)+5] = (i * 4)+1;

			//pIndex+=6;

		}
		m_pIndexBuffer->Unlock();

	}
}
/* 
		0 ---- 1
		|      |
		|      |
		2 ---- 3

		Index : 203 , 301 
*/
void CChristmasParticle::UpdateBuffer(float fStep) {

	//Update(fStep,m_fHeight);

	

	if(m_bPointSprite == true) {
		int i;
		D3DXMATRIX matWorldView;
		D3DXMATRIX matWorld,matView;
		D3DXVECTOR3 vecRight,vecUp;
		D3DXVECTOR3 vecPos,vecSize;
		DWORD		dwColor;
		D3DXVECTOR3 vecTmp;

		m_pDevice->GetTransform(D3DTS_VIEW,&matView);


		vecRight = D3DXVECTOR3(matView._11,matView._21,matView._31);
		D3DXVec3Normalize(&vecRight,&vecRight);

		vecUp = D3DXVECTOR3(matView._12,matView._22,matView._32);
		D3DXVec3Normalize(&vecUp,&vecUp);

		m_iScreenNodesNum = 0;

		CChristmasParticleVertex *pVertex = NULL;
		m_pVertexBuffer->Lock(0,0,(BYTE **)&pVertex,0);
		for(i = 0; i < 	m_iImageParticleNum; i++ ) {
			if( m_pNodes[i].m_fLife>0.0f ) {
				

				vecPos = m_pNodes[i].m_vecPos;
				vecPos -= m_vecCenter;
			
				//dwColor = D3DCOLOR_COLORVALUE(((rand() % 100) / 100.0f),((rand() % 100) / 100.0f),((rand() % 100) / 100.0f),m_pNodes[i].m_fVisible);
				
				dwColor = D3DCOLOR_COLORVALUE(m_pNodes[i].m_vecColor.x,m_pNodes[i].m_vecColor.y,m_pNodes[i].m_vecColor.z,m_pNodes[i].m_fVisible);
				pVertex[m_iScreenNodesNum].m_dwColor = dwColor;
				pVertex[m_iScreenNodesNum].m_vecPos = vecPos;

				m_iScreenNodesNum++;
				
			}
			else {
				m_pNodes[i].m_vecPos = m_vecPos;
			}
		}

		m_pVertexBuffer->Unlock();

	}
	else {	// Point Sprite Not Use.

		int i;

		D3DXMATRIX matWorldView;
		D3DXMATRIX matWorld,matView;
		D3DXVECTOR3 vecRight,vecUp;
		D3DXVECTOR3 vecPos,vecSize;
		DWORD		dwColor;
		D3DXVECTOR3 vecTmp;

		m_pDevice->GetTransform(D3DTS_VIEW,&matView);


		vecRight = D3DXVECTOR3(matView._11,matView._21,matView._31);
		D3DXVec3Normalize(&vecRight,&vecRight);

		vecUp = D3DXVECTOR3(matView._12,matView._22,matView._32);
		D3DXVec3Normalize(&vecUp,&vecUp);

		m_iScreenNodesNum = 0;
		
		CRainParticleVertex *pVertex = NULL;
		m_pVertexBuffer->Lock(0,0,(BYTE **)&pVertex,0);
		for(i = 0; i < m_iImageParticleNum; i++ ) {
			if( m_pNodes[i].m_fLife>0.0f ) {
					
				vecPos = m_pNodes[i].m_vecPos;
				vecPos -= m_vecCenter;

				vecSize = D3DXVECTOR3(m_pNodes[i].m_vecSize.x,m_pNodes[i].m_vecSize.z,0.0f);
				vecSize.z = (vecSize.x + vecSize.z) /2.0f;
				
				dwColor = D3DCOLOR_COLORVALUE(m_pNodes[i].m_vecColor.x,m_pNodes[i].m_vecColor.y,m_pNodes[i].m_vecColor.z,m_pNodes[i].m_fVisible);
				pVertex[m_iScreenNodesNum].m_dwColor = dwColor;
				pVertex[m_iScreenNodesNum + 1].m_dwColor = dwColor;
				pVertex[m_iScreenNodesNum + 2].m_dwColor = dwColor;
				pVertex[m_iScreenNodesNum + 3].m_dwColor = dwColor;

				pVertex[m_iScreenNodesNum].m_vecTexcoord = D3DXVECTOR2(0.0f,1.0f);
				pVertex[m_iScreenNodesNum + 1].m_vecTexcoord = D3DXVECTOR2(1.0f,1.0f);
				pVertex[m_iScreenNodesNum + 2].m_vecTexcoord = D3DXVECTOR2(0.0f,0.0f);
				pVertex[m_iScreenNodesNum + 3].m_vecTexcoord = D3DXVECTOR2(1.0f,0.0);

				vecTmp = (vecUp - vecRight);
				
				vecTmp.x *= vecSize.x;
				vecTmp.y *= vecSize.y;
				vecTmp.z *= vecSize.z;
				
				pVertex[m_iScreenNodesNum].m_vecPos	= ( vecTmp )+vecPos;
				
				vecTmp = (vecRight + vecUp);
				
				vecTmp.x *= vecSize.x;
				vecTmp.y *= vecSize.y;
				vecTmp.z *= vecSize.z;
				
				pVertex[m_iScreenNodesNum+1].m_vecPos = ( vecTmp )+vecPos;

				vecTmp = (vecRight + vecUp);
				
				vecTmp.x *= -vecSize.x;
				vecTmp.y *= -vecSize.y;
				vecTmp.z *= -vecSize.z;
				
				pVertex[m_iScreenNodesNum+2].m_vecPos = ( vecTmp )+vecPos;
				
				vecTmp = (vecRight - vecUp);
				
				vecTmp.x *= vecSize.x;
				vecTmp.y *= vecSize.y;
				vecTmp.z *= vecSize.z;
				
				pVertex[m_iScreenNodesNum+3].m_vecPos = ( vecTmp )+vecPos;


				m_iScreenNodesNum += 4;
			}
		}

		m_pVertexBuffer->Unlock();

	}
		
}
void CChristmasParticle::Render() {

	if(m_bPointSprite) {
		D3DXMATRIX matView;
		D3DXMatrixIdentity(&matView);
		D3DXMATRIX matInvView;

		m_pDevice->GetTransform(D3DTS_VIEW,&matView);
		matView._41 = matView._42 = matView._43 =0.0;
		D3DXMatrixInverse(&matInvView,NULL,&matView);
		
		//m_Device->SetTransform(D3DTS_WORLD,&world);	

		D3DXMATRIX matWorld;
		D3DXMATRIX matBefore;
		m_pDevice->GetTransform(D3DTS_WORLD,&matBefore);

		D3DXMatrixMultiply(&matWorld,&matBefore,&matInvView);
		matWorld._41 = m_vecCenter.x;
		matWorld._42 = m_vecCenter.y;
		matWorld._43 = m_vecCenter.z;

		m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);


		m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		m_pDevice->SetStreamSource(0,m_pVertexBuffer,sizeof(CChristmasParticleVertex));
		
		
		m_pDevice->SetTexture(0,m_pParticleTexture->GetTexture());
		m_pDevice->SetVertexShader(dwChristmasParticleVertexDecl);
		m_pDevice->SetPixelShader(NULL);

		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );


		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

		//////
		m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(SMINSIZE) );
	

		m_pDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
		m_pDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
		m_pDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
		
		if(m_bStart && !m_bExtUse) 
			m_pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(20.0f) );
		else
			m_pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(7.0f) );

		//////
		m_pDevice->DrawPrimitive( D3DPT_POINTLIST, 0,m_iScreenNodesNum);
		if(rand() % 2) {
			if(!m_bExtUse) {
				m_pDevice->SetTexture(0,m_pExtTexture->GetTexture());
			}
			m_pDevice->DrawPrimitive( D3DPT_POINTLIST, 0,m_iScreenNodesNum);

		}
	
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		
		////
		m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		m_pDevice->SetRenderState( D3DRS_POINTSIZE,   FtoDW(1.0f) );
		m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE,FALSE);
		m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,FALSE);
		m_pDevice->SetTransform(D3DTS_WORLD,&matBefore);
	}
	else {	// Point Sprite Not Use.
		
	
		D3DXMATRIX matView;
		D3DXMatrixIdentity(&matView);
		D3DXMATRIX matInvView;

		m_pDevice->GetTransform(D3DTS_VIEW,&matView);
		matView._41 = matView._42 = matView._43 =0.0;
		D3DXMatrixInverse(&matInvView,NULL,&matView);
		
		//m_Device->SetTransform(D3DTS_WORLD,&world);	

		D3DXMATRIX matWorld;
		D3DXMATRIX matBefore;
		m_pDevice->GetTransform(D3DTS_WORLD,&matBefore);

		D3DXMatrixMultiply(&matWorld,&matBefore,&matInvView);
		matWorld._41 = m_vecCenter.x;
		matWorld._42 = m_vecCenter.y;
		matWorld._43 = m_vecCenter.z;

		m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);


		m_pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		m_pDevice->SetStreamSource(0,m_pVertexBuffer,sizeof(CRainParticleVertex));
		m_pDevice->SetIndices(m_pIndexBuffer,0);
		
		m_pDevice->SetTexture(0,m_pParticleTexture->GetTexture());
		m_pDevice->SetVertexShader(dwRainParticleVertexDecl);
	
		m_pDevice->SetPixelShader(NULL);

		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );


		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	
		m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,m_iScreenNodesNum,
										0,(m_iScreenNodesNum / 2));

		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
			////
		m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		m_pDevice->SetRenderState( D3DRS_POINTSIZE,   FtoDW(1.0f) );
		m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE,FALSE);
		m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,FALSE);
		m_pDevice->SetTransform(D3DTS_WORLD,&matBefore);

	}

}

