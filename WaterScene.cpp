// WaterScene.cpp: implementation of the CWaterScene class.
//
//////////////////////////////////////////////////////////////////////

#include "WaterScene.h"
#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"
#include "RenderOption.h"
#include "SceneManager.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

long CWaterScene::m_lWaterTexture;
CTexture CWaterScene::m_WaterTexture[MAX_WATERTEXTURE];
CRenderTexture CWaterScene::m_pRelectionTexture;

CRenderTexture CWaterScene::m_pRippleTexture;
TLVertex CWaterScene::m_RippleVertex[RippleVertexCount*RippleVertexCount];
WORD CWaterScene::m_RippleIndices[(RippleVertexCount-1)*(RippleVertexCount-1)*6];

LPDIRECT3DTEXTURE8 CWaterScene::m_pBumpMap[MAX_WATERTEXTURE];	
LPDIRECT3DCUBETEXTURE8 CWaterScene::m_pCubeTexture=NULL;



float CWaterScene::m_fWaveMove;

DWORD CWaterScene::m_dwWaterShader=0xffffffff;
DWORD CWaterScene::m_dwWaterPixelShader=0xffffffff;

LPDIRECT3DVERTEXBUFFER8 CWaterScene::m_pWaterVertexBuffer=NULL;

LPDIRECT3DTEXTURE8 CWaterScene::m_WaterBumpMap;
float CWaterScene::m_fUpdateWaterTexture=0.0f;

//DWORD CWaterScene::m_dwWaterVertexShader=0xffffffff;
//DWORD CWaterScene::m_dwWaterPixelShader=0xffffffff;

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

char strWaterVertexShader[]=
"vs.1.0\n"
"dp4 oPos.x, v0, c[2]\n"
"dp4 oPos.y, v0, c[3]\n"
"dp4 oPos.z, v0, c[4]\n"
"dp4 oPos.w, v0, c[5]\n"
"dp3 r2.w, v5, v5\n"
"rsq r2.w, r2.w\n"
"mul r2, v5, r2.w\n"
"dp3 r3.w, v6, v6\n"
"rsq r3.w, r3.w\n"
"mul r3, v6, r3.w\n"
"mul r1, r2.zxyw, r3.yzxw\n"
"mad r1, r2.yzxw, r3.zxyw, -r1\n"
"dp3 r1.w, r1, r1\n" 
"rsq r1.w, r1.w\n"
"mul r1.xyz, r1, r1.w\n"
"mul r2, r2, c[13].w\n"
"mul r3, r3, c[13].w\n"
"dp3 oT1.x, r2,  c[9]\n"
"dp3 oT1.y, r3,  c[9]\n"
"dp3 oT1.z, r1, c[9]\n"
"dp3 oT2.x, r2,  c[10]\n"
"dp3 oT2.y, r3,  c[10]\n"
"dp3 oT2.z, r1, c[10]\n"
"dp3 oT3.x, r2,  c[11]\n"
"dp3 oT3.y, r3,  c[11]\n"
"dp3 oT3.z, r1, c[11]\n"
"dp3 r0.x, v0, c[6]\n"
"dp3 r0.y, v0, c[7]\n"
"dp3 r0.zw, v0, c[8]\n"
"add r4, -r0, c[12]\n"
"mov oT1.w, r4.x\n"
"mov oT2.w, r4.y\n"
"mov oT3.w, r4.z\n"
"mov oD0, c[1]\n"
"mov oT0.xy, v3\n"
"dp4 oFog.x, v0, c[4]\n";
 
/*
char strWaterVertexShader[]=
"vs.1.0\n"
"m4x4 oPos, v0, c2\n"
"mov  oT0.xy, v3.xy\n"   // Bump

"mov r1,c13\n"//LightDirection
"sub  r0, c12, v0\n" //Viewer direction
//Normalize
"dp3 r11.x,r0.xyz,r0.xyz\n"
"rsq r11.xyz,r11.x\n"
"mul r0.xyz, r0.xyz, r11.xyz\n"

"add  r4.xyz, r0.xyz, r1.xyz\n" //Halfway

//;r4 = tangent space halfway vector 
"dp3  r6.x, v5, r4\n"
"dp3  r6.y, v6, r4\n"
"dp3  r6.z, v4, r4\n"

"mov  oT1.xyz,r6.xyz\n";  // Halfway vector to be renormalized by cube map
*/

char strWaterPixelShader[]=
"ps.1.0\n"
"tex t0	\n"		
"texm3x3pad t1, t0\n"
"texm3x3pad t2, t0\n"	
"texm3x3vspec t3,t0\n"
"mul r0,c0,t3\n";


/*
char strWaterPixelShader[]=	
"ps.1.1\n"
"def c0, 0.4, 0.4, 0.4, 0.4\n"
"def c1, 0.25, 0.25, 0.25, 0.25\n"
"tex t0\n"//normal
"tex t1\n"//normalize cube
"mul r1,t0,c0\n"
"dp3_sat r0,r1_bx2,t1_bx2\n"//N.H
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r1,r0,r0\n"
"mul r0,r1,c1\n";
*/


CWaterScene::CWaterScene()
{
	m_lWaterTexture=0;
}

CWaterScene::~CWaterScene()
{

}

void CWaterScene::Create()
{
	m_WaterVertex[0].v=vector3(0.0f,0.0f,0.0f);
	m_WaterVertex[1].v=vector3(SECTORSIZE,0.0f,0.0f);
	m_WaterVertex[2].v=vector3(0.0f,0.0f,SECTORSIZE);
	m_WaterVertex[3].v=vector3(SECTORSIZE,0.0f,SECTORSIZE);

	m_WaterVertex[0].tu=0.0f;
	m_WaterVertex[0].tv=0.0f;

	m_WaterVertex[1].tu=5.0f;
	m_WaterVertex[1].tv=0.0f;

	m_WaterVertex[2].tu=0.0f;
	m_WaterVertex[2].tv=5.0f;

	m_WaterVertex[3].tu=5.0f;
	m_WaterVertex[3].tv=5.0f;

	m_WaterVertex[0].tu1=0.0f;
	m_WaterVertex[0].tv1=0.0f;

	m_WaterVertex[1].tu1=5.0f;
	m_WaterVertex[1].tv1=0.0f;

	m_WaterVertex[2].tu1=0.0f;
	m_WaterVertex[2].tv1=5.0f;

	m_WaterVertex[3].tu1=5.0f;
	m_WaterVertex[3].tv1=5.0f;

	m_WaterVertex[0].spec.c=0x0;
	m_WaterVertex[1].spec.c=0x0;
	m_WaterVertex[2].spec.c=0x0;
	m_WaterVertex[3].spec.c=0x0;

	m_WaterVertex[0].diff.c=0xccccddf0;
	m_WaterVertex[1].diff.c=0xccccddf0;
	m_WaterVertex[2].diff.c=0xccccddf0;
	m_WaterVertex[3].diff.c=0xccccddf0;

	char strWaterTextureName[256];

	CTexture::SetPath(NATURETEXTUREPATH);

	static bool bFirstInit=false;
	//if(m_WaterTexture[0].GetTexture()==NULL)
	if(!bFirstInit)
	{
		bFirstInit=true;
		m_fWaveMove=0.0f;

		//m_pRelectionTexture.Create(SHADOWSIZE,SHADOWSIZE);
		//m_pRippleTexture.Create(SHADOWSIZE,SHADOWSIZE);

		int ix,iy;

		for(ix=0;ix<RippleVertexCount;ix++)
		{
			for(iy=0;iy<RippleVertexCount;iy++)
			{
				m_RippleVertex[ix+iy*RippleVertexCount].v.x=ix*(SHADOWSIZE/(RippleVertexCount-2));
				m_RippleVertex[ix+iy*RippleVertexCount].v.y=iy*(SHADOWSIZE/(RippleVertexCount-2));

				m_RippleVertex[ix+iy*RippleVertexCount].v.z=0.1f;
				m_RippleVertex[ix+iy*RippleVertexCount].w=0.1f;

				m_RippleVertex[ix+iy*RippleVertexCount].Diffuse.c=0xffffffff;
				m_RippleVertex[ix+iy*RippleVertexCount].Specular.c=0x0;

				m_RippleVertex[ix+iy*RippleVertexCount].tu=0.0f;
				m_RippleVertex[ix+iy*RippleVertexCount].tv=0.0f;
			}
		}		

		WORD *pIndices=m_RippleIndices;

		for(ix=0;ix<RippleVertexCount-1;ix++)
		{
			for(iy=0;iy<RippleVertexCount-1;iy++)
			{
				*(pIndices++)=(ix+0)+(iy+0)*RippleVertexCount;
				*(pIndices++)=(ix+0)+(iy+1)*RippleVertexCount;
				*(pIndices++)=(ix+1)+(iy+0)*RippleVertexCount;

				*(pIndices++)=(ix+1)+(iy+0)*RippleVertexCount;
				*(pIndices++)=(ix+0)+(iy+1)*RippleVertexCount;
				*(pIndices++)=(ix+1)+(iy+1)*RippleVertexCount;		
			}
		}
		
		if(CRenderOption::m_WaterBumpEnvRendering)
		{
			char strBumpName[256];			

			for(int i=0;i<MAX_WATERTEXTURE;i++)
			{
				sprintf(strWaterTextureName,"water%4d_bump.dds",i);
				int strLens=strlen(strWaterTextureName);
				for(int cStr=0;cStr<strLens;cStr++)
				{
					if(strWaterTextureName[cStr]==' ')
						strWaterTextureName[cStr]='0';
				}
				sprintf(strBumpName,"%s\\%s",NATURETEXTUREPATH,strWaterTextureName);							

				D3DXCreateTextureFromFileEx(BaseGraphicsLayer::GetDevice(), 
					strBumpName,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					1,
					0,
					(D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S'),
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					0,
					NULL,
					NULL,
					&m_pBumpMap[i]);
			}		

			sprintf(strBumpName,"%s\\%s",NATURETEXTUREPATH,"sky_cube_mipmap.dds");

			D3DXCreateCubeTextureFromFileExA( BaseGraphicsLayer::GetDevice(),
					strBumpName, 
					D3DX_DEFAULT,
					0,
					0,
					D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					0,
					NULL,
					NULL,
					&m_pCubeTexture);

			DWORD dwWaterVertexDecl[] =
			{
				D3DVSD_STREAM( 0 ),					
				D3DVSD_REG(0,D3DVSDT_FLOAT3 ), // Position 
				D3DVSD_REG(1, D3DVSDT_FLOAT3), // Normal
				D3DVSD_REG(2, D3DVSDT_FLOAT3), // Diffuse
				D3DVSD_REG(3, D3DVSDT_FLOAT2), // Texture
				D3DVSD_REG(5, D3DVSDT_FLOAT3), // S
				D3DVSD_REG(6, D3DVSDT_FLOAT3), // T
				D3DVSD_REG(4, D3DVSDT_FLOAT3), // SxT			
				D3DVSD_END()
			};
			
			/*
			char strShaderPath[256];
			sprintf(strShaderPath,"%s%s",SHADERPATH,"WaterProjection.vsh");

			LPD3DXBUFFER pCode;    			    
			D3DXAssembleShaderFromFile(strShaderPath,0,NULL,&pCode,NULL);    

			BaseGraphicsLayer::GetDevice()->CreateVertexShader( dwWaterVertexDecl, 
                                         (DWORD*)pCode->GetBufferPointer(),
                                         &m_dwWaterShader, 0 );
			pCode->Release();

			
			sprintf(strShaderPath,"%s%s",SHADERPATH,"WaterProjection.psh");
			
			D3DXAssembleShaderFromFile(strShaderPath,0,NULL,&pCode,NULL);    

			BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),
                                         &m_dwWaterPixelShader);
			pCode->Release();			
			*/

			BaseGraphicsLayer::GetDevice()->CreateVertexBuffer(4*sizeof(Dot3Vertex),
												   D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,
												   &m_pWaterVertexBuffer);

			Dot3Vertex *pVertex;			
			m_pWaterVertexBuffer->Lock(0,0,(BYTE**)&pVertex,0);	

			pVertex[0].v=vector3(0.0f,0.0f,0.0f);
			pVertex[1].v=vector3(SECTORSIZE,0.0f,0.0f);
			pVertex[2].v=vector3(0.0f,0.0f,SECTORSIZE);
			pVertex[3].v=vector3(SECTORSIZE,0.0f,SECTORSIZE);

			pVertex[0].Normal=vector3(0.0f,1.0f,0.0f);
			pVertex[1].Normal=vector3(0.0f,1.0f,0.0f);
			pVertex[2].Normal=vector3(0.0f,1.0f,0.0f);
			pVertex[3].Normal=vector3(0.0f,1.0f,0.0f);

			pVertex[0].tu=0.0f;
			pVertex[0].tv=0.0f;
			
			pVertex[1].tu=43.0f;
			pVertex[1].tv=0.0f;

			pVertex[2].tu=0.0f;
			pVertex[2].tv=43.0f;

			pVertex[3].tu=43.0f;
			pVertex[3].tv=43.0f;
			
			CalcBasicVector3(pVertex);

			m_pWaterVertexBuffer->Unlock();	

			LPD3DXBUFFER pCode;
			
			D3DXAssembleShader(strWaterPixelShader,strlen(strWaterPixelShader),0,NULL,&pCode,NULL);										 
			BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwWaterPixelShader);    
			pCode->Release();		

			D3DXAssembleShader(strWaterVertexShader,strlen(strWaterVertexShader),0,NULL,&pCode,NULL);				
			BaseGraphicsLayer::GetDevice()->CreateVertexShader( dwWaterVertexDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwWaterShader, 0 );
			pCode->Release();		

			
			

			//CreateWaterBumpTexture();
		}		
	}
}

void CWaterScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	if(CRenderOption::m_WaterBumpEnvRendering)
	{
		
		matrix matWorld,matView,matProj,matWVP,matLastWVP;
		pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->GetTransform(D3DTS_VIEW,matView);	
		pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);	

		matWVP=matWorld*matView;
		matWVP=matWVP*matProj;

		matLastWVP=matWVP;
		matWVP.Transpose();

		float fZero[4]={0.0f,0.0f,0.0f,0.0f};
		float fOne[4]={1.0f,1.0f,1.0f,1.0f};

		//vector3 *vecViewPos=BaseGraphicsLayer::m_ViewCamera.GetPosition();	
		matrix matInvView;
		matInvView.Inverse(matView);
		vector3 vecViewPos;
		vecViewPos.x = matInvView._41;
		vecViewPos.y = matInvView._42;
		vecViewPos.z = matInvView._43;

		float fViewPos[4]={vecViewPos.x,vecViewPos.y,vecViewPos.z,0.0f};

		//float fViewPos[4]={vecViewPos->x,vecViewPos->y,vecViewPos->z,0.0f};
		float fBumpScale[4]={0.0f,0.0f,0.0f,0.10f};		

		pd3dDevice->SetVertexShaderConstant(0,fZero,1);
		pd3dDevice->SetVertexShaderConstant(1,fOne,1);
		pd3dDevice->SetVertexShaderConstant(2,&matWVP,4);	

		//world		
		pd3dDevice->SetVertexShaderConstant(6,&matWorld._11,1);
		pd3dDevice->SetVertexShaderConstant(7,&matWorld._21,1);
		pd3dDevice->SetVertexShaderConstant(8,&matWorld._31,1);	
		//Basic
		pd3dDevice->SetVertexShaderConstant(9,&matWorld._11,1);
		pd3dDevice->SetVertexShaderConstant(10,&matWorld._21,1);
		pd3dDevice->SetVertexShaderConstant(11,&matWorld._31,1);

		fViewPos[0]-=matWorld._41;
		fViewPos[1]-=matWorld._42;
		fViewPos[2]-=matWorld._43;

		pd3dDevice->SetVertexShaderConstant(12,fViewPos,1);
		pd3dDevice->SetVertexShaderConstant(13,fBumpScale,1);

		pd3dDevice->SetTexture(0, m_pBumpMap[m_lWaterTexture]);
		pd3dDevice->SetTexture(1, NULL);
		pd3dDevice->SetTexture(2, NULL);
		//pd3dDevice->SetTexture(3, m_pCubeTexture);	
		pd3dDevice->SetTexture(3, CSceneManager::m_pWorldCubemap);

		pd3dDevice->SetVertexShader(m_dwWaterShader);
		pd3dDevice->SetPixelShader(m_dwWaterPixelShader);		
		/*
		float fWaterColor[4]={CSceneManager::m_WeatherManager.m_InterWaterColor.r/255.0f,
							  CSceneManager::m_WeatherManager.m_InterWaterColor.g/255.0f,
							  CSceneManager::m_WeatherManager.m_InterWaterColor.b/255.0f,1.0f};
		
		pd3dDevice->SetPixelShaderConstant(0,fWaterColor,1);		
		*/
		float fWaterColor[4]={m_WaterVertex[0].diff.r/255.0f,
							  m_WaterVertex[0].diff.g/255.0f,
							  m_WaterVertex[0].diff.b/255.0f,
							  m_WaterVertex[0].diff.a/255.0f};
		
		pd3dDevice->SetPixelShaderConstant(0,fWaterColor,1);	

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    	

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);		

		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

		pd3dDevice->SetStreamSource(0,m_pWaterVertexBuffer,sizeof(Dot3Vertex));
		
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		pd3dDevice->SetPixelShader(NULL);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		pd3dDevice->SetTexture(3,NULL);	


		/*
		matrix matWorld,matView,matProj,matWVP,matLastWVP;
		pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->GetTransform(D3DTS_VIEW,matView);	
		pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);	

		matWVP=matWorld*matView;
		matWVP=matWVP*matProj;

		matLastWVP=matWVP;
		matWVP.Transpose();
		pd3dDevice->SetVertexShaderConstant(0,&matWVP,4);

		matrix matTex,matTexScale;

		matTexScale.MakeIdent();
		matTexScale._11=0.5f;
		matTexScale._22=-0.5f;	
		matTexScale._33=0.0f;
		matTexScale._41=0.5f;
		matTexScale._42=0.5f;
		matTexScale._43=1.0f;
		matTexScale._44=1.0f;
		
		matTex=matLastWVP*matTexScale;
		matTex.Transpose();
		pd3dDevice->SetVertexShaderConstant(4,&matTex,4);

		pd3dDevice->SetStreamSource(0,m_pWaterVertexBuffer,sizeof(MultiVertex));
		*/

		/*
		matrix m_matBumpMat;
		m_matBumpMat.MakeIdent();
		
		static float m_fTime = 0.0f;
		m_fTime+=0.01f;
		float r = 0.04f;
		m_matBumpMat._11 =  r * cosf( m_fTime * 9.0f );
		m_matBumpMat._12 = -r * sinf( m_fTime * 9.0f );
		m_matBumpMat._21 =  r * sinf( m_fTime * 9.0f );
		m_matBumpMat._22 =  r * cosf( m_fTime * 9.0f );

		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00,   F2DW( m_matBumpMat._11 ) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01,   F2DW( m_matBumpMat._12 ) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10,   F2DW( m_matBumpMat._21 ) );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11,   F2DW( m_matBumpMat._22 ) );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );					
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);	
		
		pd3dDevice->SetTexture(0,m_WaterBumpMap);		
		pd3dDevice->SetTexture(1,m_pRelectionTexture.GetTexture());
		//pd3dDevice->SetTexture(1,m_pRippleTexture.GetTexture());
		
		//pd3dDevice->SetVertexShader(m_dwWaterShader);

		pd3dDevice->SetVertexShader(MultiFVF);

		pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
		pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);	
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_WaterVertex,sizeof(MultiVertex));	

		for(int i=0;i<4;i++)
		{
			float tu,tv;
			BaseGraphicsLayer::ProjectiveTextureMapping(m_WaterVertex[i].v,tu,tv);			
			m_WaterVertex[i].tu1=tu;
			m_WaterVertex[i].tv1=tv;
		}		
		//pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
		*/
	}
	else
	{
		/*
		matrix matView,matProj;	
		pd3dDevice->GetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);	
		pd3dDevice->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);	

		matrix matTexScale;
		matTexScale.MakeIdent();
		matTexScale._11=0.5f;
		matTexScale._22=-0.5f;	
		matTexScale._33=0.0f;
		matTexScale._41=0.5f;
		matTexScale._42=0.5f;
		matTexScale._43=1.0f;
		matTexScale._44=1.0f;

		matrix matInvView;
		matrix matTex2;

		matInvView.Inverse(matView);

		matrix mat,mat2;
		
		matTex2=matProj*matTexScale;
		
		pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex2);		
		
		pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);		
		*/
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);		
		pd3dDevice->SetVertexShader(MultiFVF);		
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_WaterVertex,sizeof(MultiVertex));			
		//pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		//pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

		//matTexScale.MakeIdent();
		//pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTexScale);
	}
}

void CWaterScene::DeterminWaterTexture(LPDIRECT3DDEVICE8 pd3dDevice,bool isRelection)
{	
	if(isRelection)
		//pd3dDevice->SetTexture(1,m_pRelectionTexture.GetTexture());		
		pd3dDevice->SetTexture(1,m_pRippleTexture.GetTexture());
	else
		pd3dDevice->SetTexture(1,NULL);		

	pd3dDevice->SetTexture(0,m_WaterTexture[m_lWaterTexture].GetTexture());
}

void CWaterScene::IncWaterTexture(float fUpdate)
{
	//m_lWaterTexture++;
	if(m_lWaterTexture>=MAX_WATERTEXTURE)
		m_lWaterTexture=0;	
	m_fUpdateWaterTexture+=fUpdate;

	m_lWaterTexture=((int)m_fUpdateWaterTexture)%MAX_WATERTEXTURE;
	
}

void CWaterScene::UpdateWave()
{
	m_fWaveMove+=0.0007f;

	int ix,iy;

	for(ix=0;ix<RippleVertexCount;ix++)
	{
		for(iy=0;iy<RippleVertexCount;iy++)
		{
			m_RippleVertex[ix+iy*RippleVertexCount].tu=(float)ix/(float)(RippleVertexCount-2);
			m_RippleVertex[ix+iy*RippleVertexCount].tv=(float)iy/(float)(RippleVertexCount-2);

			//m_RippleVertex[ix+iy*RippleVertexCount].tu+=cos((m_RippleVertex[ix+iy*RippleVertexCount].tv+m_fWaveMove)*130.0f)*(0.008f);
			//m_RippleVertex[ix+iy*RippleVertexCount].tu+=cos((m_RippleVertex[ix+iy*RippleVertexCount].tv+m_fWaveMove)*130.0f)*(0.008f);

			m_RippleVertex[ix+iy*RippleVertexCount].tu+=cos((m_RippleVertex[ix+iy*RippleVertexCount].tv+m_fWaveMove)*130.0f)
				*(m_RippleVertex[ix+iy*RippleVertexCount].tv)
				*(0.012f);
		}
	}

}

void CWaterScene::RenderRipple(LPDIRECT3DDEVICE8 pd3dDevice)
{

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);			
	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);		
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);		


	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE ,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->SetTexture(0,m_pRelectionTexture.GetTexture());	
	UpdateWave();		

	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,RippleVertexCount*RippleVertexCount,
		(RippleVertexCount-1)*(RippleVertexCount-1)*2,m_RippleIndices,D3DFMT_INDEX16,m_RippleVertex,sizeof(TLVertex));
	
}

void CWaterScene::CreateWaterBumpTexture()
{
	DWORD dwWidth,dwHeight;
	dwWidth=256;
	dwHeight=256;

	BaseGraphicsLayer::GetDevice()->CreateTexture(dwWidth,dwHeight,1,0,D3DFMT_V8U8,D3DPOOL_MANAGED,&m_WaterBumpMap);        
    
    D3DLOCKED_RECT d3dlr;
    m_WaterBumpMap->LockRect( 0, &d3dlr, 0, 0 );

    CHAR* pDst = (CHAR*)d3dlr.pBits;
    CHAR  iDu, iDv;

    for( DWORD y=0; y<dwHeight; y++ )
    {
        CHAR* pPixel = pDst;

        for( DWORD x=0; x<dwWidth; x++ )
        {
            FLOAT fx = x/(FLOAT)dwWidth - 0.5f;
            FLOAT fy = y/(FLOAT)dwHeight - 0.5f;

            FLOAT r = sqrtf( fx*fx + fy*fy );

            iDu  = (CHAR)( 64 * cosf( 300.0f * r ) * expf( -r * 5.0f ) );
            iDu += (CHAR)( 32 * cosf( 150.0f * ( fx + fy ) ) );
            iDu += (CHAR)( 16 * cosf( 140.0f * ( fx * 0.85f - fy ) ) );

            iDv  = (CHAR)( 64 * sinf( 300.0f * r ) * expf( -r * 5.0f ) );
            iDv += (CHAR)( 32 * sinf( 150.0f * ( fx + fy ) ) );
            iDv += (CHAR)( 16 * sinf( 140.0f * ( fx * 0.85f - fy ) ) );

            *pPixel++ = iDu;
            *pPixel++ = iDv;
        }
        pDst += d3dlr.Pitch;
    }
    m_WaterBumpMap->UnlockRect(0);	
}

#define SMALL_FLOAT 1e-12

void CWaterScene::CalcBasicVector3(Dot3Vertex *pVertex)
{
	for(int i=0;i<4;i++)
	{
		pVertex[i].s=vector3(0.0f,0.0f,0.0f);
		pVertex[i].t=vector3(0.0f,0.0f,0.0f);

	}
	vector3 edge01,edge02;
	vector3 cp;
	for(int i=0;i<2;i++)
	{
		Dot3Vertex& v0=pVertex[i+0];
		Dot3Vertex& v1=pVertex[i+1];
		Dot3Vertex& v2=pVertex[i+2];

		edge01=vector3(v1.v.x-v0.v.x,v1.tu-v0.tu,v1.tv-v0.tv);
		edge02=vector3(v2.v.x-v0.v.x,v2.tu-v0.tu,v2.tv-v0.tv);
		cp=edge01^edge02;
		if(fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.s.x+=-cp.y/cp.x;
			v0.t.x+=-cp.z/cp.x;

			v1.s.x+=-cp.y/cp.x;
			v1.t.x+=-cp.z/cp.x;

			v2.s.x+=-cp.y/cp.x;
			v2.t.x+=-cp.z/cp.x;
		}

		edge01=vector3(v1.v.y-v0.v.y,v1.tu-v0.tu,v1.tv-v0.tv);
		edge02=vector3(v2.v.y-v0.v.y,v2.tu-v0.tu,v2.tv-v0.tv);
		cp=edge01^edge02;
		if(fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.s.y+=-cp.y/cp.x;
			v0.t.y+=-cp.z/cp.x;

			v1.s.y+=-cp.y/cp.x;
			v1.t.y+=-cp.z/cp.x;

			v2.s.y+=-cp.y/cp.x;
			v2.t.y+=-cp.z/cp.x;
		}

		edge01=vector3(v1.v.z-v0.v.z,v1.tu-v0.tu,v1.tv-v0.tv);
		edge02=vector3(v2.v.z-v0.v.z,v2.tu-v0.tu,v2.tv-v0.tv);
		cp=edge01^edge02;
		if(fabsf(cp.x) > SMALL_FLOAT)
		{
			v0.s.z+=-cp.y/cp.x;
			v0.t.z+=-cp.z/cp.x;

			v1.s.z+=-cp.y/cp.x;
			v1.t.z+=-cp.z/cp.x;

			v2.s.z+=-cp.y/cp.x;
			v2.t.z+=-cp.z/cp.x;
		}

	}

	for(int i=0;i<4;i++)
	{
		pVertex[i].s.Normalize();
		pVertex[i].t.Normalize();

		pVertex[i].SxT=pVertex[i].s^pVertex[i].t;

		pVertex[i].Normal.Normalize();

		if((pVertex[i].SxT*pVertex[i].Normal) < 0.0f)
		{
			pVertex[i].SxT=-pVertex[i].SxT;
		}
	}   
}

void CWaterScene::RenderFlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_WaterBumpEnvRendering)
	{
		matrix matWorld,matView,matProj,matWVP,matLastWVP;
		pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->GetTransform(D3DTS_VIEW,matView);	
		pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);	

		matWVP=matWorld*matView;
		matWVP=matWVP*matProj;

		matLastWVP=matWVP;
		matWVP.Transpose();

		float fZero[4]={0.0f,0.0f,0.0f,0.0f};
		float fOne[4]={1.0f,1.0f,1.0f,1.0f};

		vector3 *vecViewPos=BaseGraphicsLayer::m_ViewCamera.GetPosition();	

		float fViewPos[4]={vecViewPos->x-matWorld._41,vecViewPos->y-matWorld._42,vecViewPos->z-matWorld._43,0.0f};

		float fLightDirection[4]={-1.0f,-1.0f,-1.0f,0.0f};		

		pd3dDevice->SetVertexShaderConstant(0,fZero,1);
		pd3dDevice->SetVertexShaderConstant(1,fOne,1);
		pd3dDevice->SetVertexShaderConstant(2,&matWVP,4);		

		pd3dDevice->SetVertexShaderConstant(12,fViewPos,1);		
		pd3dDevice->SetVertexShaderConstant(13,fLightDirection,1);


		pd3dDevice->SetTexture(0, m_pBumpMap[m_lWaterTexture]);
		//pd3dDevice->SetTexture(1,m_pNormalizeCubeTexture);		
		//pd3dDevice->SetTexture(1,m_pNormalizeCubeTexture);		
		pd3dDevice->SetTexture(1,NULL);		
		pd3dDevice->SetTexture(2, NULL);
		pd3dDevice->SetTexture(3, NULL);
		pd3dDevice->SetTexture(3, m_pCubeTexture);	
		/*
		pd3dDevice->SetVertexShader(m_dwWaterSpecularVertexShader);
		pd3dDevice->SetPixelShader(m_dwWaterSpecularPixelShader);
		*/

		float fWaterColor[4]={CSceneManager::m_WeatherManager.m_InterWaterColor.r/255.0f,
							  CSceneManager::m_WeatherManager.m_InterWaterColor.g/255.0f,
							  CSceneManager::m_WeatherManager.m_InterWaterColor.b/255.0f,1.0f};
		
		pd3dDevice->SetPixelShaderConstant(0,fWaterColor,1);		


		pd3dDevice->SetVertexShader(m_dwWaterShader);
		pd3dDevice->SetPixelShader(m_dwWaterPixelShader);

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);    	

		pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

		pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

		pd3dDevice->SetStreamSource(0,m_pWaterVertexBuffer,sizeof(Dot3Vertex));

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);		
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

		pd3dDevice->SetPixelShader(NULL);
	}
}
