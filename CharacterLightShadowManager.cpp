// CharacterLightShadowManager.cpp: implementation of the CCharacterLightShadowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "CharacterLightShadowManager.h"
#include "SceneManager.h"
#include "SoundMgr.h"
#include "BaseGraphicsLayer.h"
#include "Z3DManagedObject.h"
#include "Z3D_GLOBALS.h"

#include "RenderOption.h"
#include "MapStorage.h"
#include "GMMemory.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }
/*
char strCharacterPerPixelLightingAndShadowMapPixelShader[]=	
"ps.1.1\n"
"def c0,0.3f,0.3f,0.3f,0.3f\n"
"def c1,0.1f,0.1f,0.1f,0.1f\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"
"dp3 r0,t1_bx2,v0_bx2\n"
"add t2,t2,c1\n"
"mul r0,r0,t2\n"
"sub r1,c0,r0\n"
"mov_sat r1,r1\n"
"add r0,r0,r1\n"
"mul_x2 r0,r0,t0\n";
*/
char strCharacterPerPixelLightingAndShadowMapPixelShader[]=	
"ps.1.1\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"

"dp3_sat r0,t1_bx2,v0_bx2\n"
"dp3_sat r1,t1_bx2,v1_bx2\n"
//"mul r1,r1,c2\n"
//"add r0,r1,r0\n"
//"add t2,t2,c1\n"
"mul r0,r0,t2\n"
"mad r0,r1,c2,r0\n"

"sub_sat r1,c0,r0\n"
"mul r0,r0,c3\n"
//"mov_sat r1,r1\n"
"add r0,r0,r1\n"
"mul_x2 r0,r0,t0\n";


char strCharacterPerPixelLightingPixelShader[]=
"ps.1.1\n"
"def c0,0.3f,0.3f,0.3f,0.3f\n"
"def c1,0.1f,0.1f,0.1f,0.1f\n"
"tex t0\n"
"tex t1\n"
"dp3 r0,t1_bx2,v0_bx2\n"
"sub r1,c0,r0\n"
"mov_sat r1,r1\n"
"add r0,r0,r1\n"
"mul_x2 r0,r0,t0\n";

/*
char strChracterSpecAndShadowMapShader[]=	
"ps.1.1\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"
"dp3 r0,t1_bx2,v1_bx2\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r0,r0,t2\n";
  
*/

char strChracterSpecAndShadowMapShader[]=	
"ps.1.1\n"
"def c0,0.10f,0.10f,0.10f,0.10f\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"

"dp3 r0,t1_bx2,v1_bx2\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r0,r0,t2\n"

"dp3 r1,t1_bx2,v0_bx2\n"

//"sub_sat t2,c0,1-r1\n"
//"add r1,t2,1-r1\n"

//"mul r1,r1,t0.a\n"
/*
"mad r1,r1,t0.a,r1
mad dest, src0, src1, src2
dest = src0 * src1 + src2
*/
"mul_x2 r0,r0,1-r1\n";

char strChracterSpecShader[]=	
"ps.1.1\n"
"tex t0\n"
"tex t1\n"
"dp3 r0,t1_bx2,v1_bx2\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n";


///12 Light Direction;
char strVertexShader[]=
"vs.1.1\n"
"m4x4 oPos, v0, c8\n"// Position World*View*Projection Transform
"m3x3 r3, v8, c0\n"	// Normal World Transform 
"m3x3 r5, v3, c0\n"	// Tangent S vector World Transform

"mul r0, -r3.zxyw, r5.yzxw\n"
"mad r4, -r3.yzxw, r5.zxyw,-r0\n"//r4 is Tangent T

"m4x4 r2, v0, c0\n"
"add r2, -r2, c24\n"//c24 Camera Position
"dp3 r11.x,  r2.xyz, r2.xyz\n"	
"rsq r11.xyz, r11.x\n"			
"mul r2.xyz, r2.xyz, r11.xyz\n"
"add r2.xyz, r2.xyz, -c12\n"	
"dp3 r11.x, r2.xyz, r2.xyz\n"	
"rsq r11.xyz, r11.x\n"
"mul r2.xyz, r2.xyz, r11.xyz\n"
//"dp3 r8.x,r3,r2\n"
//"dp3 r8.y,r4,r2\n"
//"dp3 r8.z,r5,r2\n"
//"mad oD1.xyz, r8, c33, c33\n"//Specular Vertex Color 
"dp3 r8.x, r3, -c12\n"
"dp3 r8.y, r4, -c12\n"
"dp3 r8.z, r5, -c12\n"
"mad oD0.xyz, r8.xyz, c33, c33\n"//Diffuse Vertex Color

"dp3 r8.x, r3, -c13\n"
"dp3 r8.y, r4, -c13\n"
"dp3 r8.z, r5, -c13\n"
"mad oD1.xyz, r8.xyz, c33, c33\n"//Back Diffuse Vertex Color 

"mov oT0.xy, v7.xy\n"
"mov oT1.xy, v7.xy\n"

"dp4 oT2.x,v0,c4\n"
"dp4 oT2.y,v0,c5\n"
"dp4 oT2.z,v0,c6\n"
"dp4 oT2.w,v0,c7\n";



char strVertexSpecularShader[]=
"vs.1.1\n"
"m4x4 oPos, v0, c8\n"
"m3x3 r3, v8, c0\n"	
"m3x3 r5, v3, c0\n"	
"mul r0, -r3.zxyw, r5.yzxw\n"
"mad r4, -r3.yzxw, r5.zxyw,-r0\n"
"m4x4 r2, v0, c0\n"			
"add r2, -r2, c24\n"
"dp3 r11.x,  r2.xyz, r2.xyz\n"	
"rsq r11.xyz, r11.x\n"			
"mul r2.xyz, r2.xyz, r11.xyz\n"
"add r2.xyz, r2.xyz, -c12\n"	
"dp3 r11.x, r2.xyz, r2.xyz\n"	
"rsq r11.xyz, r11.x\n"
"mul r2.xyz, r2.xyz, r11.xyz\n"
"dp3 r8.x,r3,r2\n"
"dp3 r8.y,r4,r2\n"
"dp3 r8.z,r5,r2\n"
"mad oD1.xyz, r8, c33, c33\n"
"dp3 r8.x, r3, -c12\n"
"dp3 r8.y, r4, -c12\n"
"dp3 r8.z, r5, -c12\n"
"mad oD0.xyz, r8.xyz, c33, c33\n"
"mov oT0.xy, v7.xy\n"
"mov oT1.xy, v7.xy\n"
"dp4 oT2.x,v0,c4\n"
"dp4 oT2.y,v0,c5\n"
"dp4 oT2.z,v0,c6\n"
"dp4 oT2.w,v0,c7\n";


//"mul oT2.xy, v7.xy,c[34]\n";
/*
	

//List<CZ3DGeneralChrModel*> CCharacterLightShadowManager::m_CharacterList;

//List<CollisionType> CCharacterLightShadowManager::m_CharacterCollisionType;
//List<POINT> CCharacterLightShadowManager::m_CharacterScreenPositionList;
//List<CZ3DSoundEventHelper*> CCharacterLightShadowManager::m_CharacterSoundHelperList;
//List<bool> CCharacterLightShadowManager::m_CharacterAttackEventList;
//List<bool> CCharacterLightShadowManager::m_CharacterTrailOnEventList;
//List<bool> CCharacterLightShadowManager::m_CharacterTrailOffEventList;
//List<int> CCharacterLightShadowManager::m_CharacterDelayTimer;
//List<CCharacterLightShadowManager::CChrLightNode> CCharacterLightShadowManager::m_CharacterLightList;
//List<vector3> CCharacterLightShadowManager::m_CharacterPerFrameMove;

//List<vector3> CCharacterLightShadowManager::m_CharacterRealPosition;
//List<int> CCharacterLightShadowManager::m_CharacterRandomPostionTimer;
//List<vector3> CCharacterLightShadowManager::m_CharacterRandomPostionAdder;
*/

CRenderTexture CCharacterLightShadowManager::m_pShadowTexture;//그림자
std::vector<CCharacterLightShadowManager::CCharacterDataNode> CCharacterLightShadowManager::m_CharacterList;

CCharacterLightShadowManager::CCharacterLightShadowManager()
{
	m_pCharacterLight = NULL;

}

CCharacterLightShadowManager::~CCharacterLightShadowManager()
{
	if(m_pCharacterLight != NULL)
	{
		delete m_pCharacterLight;
		m_pCharacterLight = NULL;
	}

	//CZ3DSound::Close();
	//CZ3DTimingSignaler::Close();
}
void CCharacterLightShadowManager::Create()
{	
	if(CRenderOption::m_CharacterProjectShadowTerrain)
		m_pShadowTexture.Create(256,256);

/*
	char tmp[256] = {0,};

	sprintf(tmp, "%sAni\\", CHARACTERDATAPATH);
	g_ContAniKeyPack.LoadHandleNameTable(CHARACTERANIPACK,tmp);

	sprintf(tmp, "%sMesh\\", CHARACTERDATAPATH);
	g_ContLODMesh.SetFilePath(tmp);

	sprintf(tmp, "%sAmesh\\", CHARACTERDATAPATH);
	g_ContAMesh.SetFilePath(tmp);

	if(CRenderOption::m_TextureMethod==1)
	{
		sprintf(tmp, "%sTexture\\", CHARACTERDATAPATH);
	}
	else
	{
		sprintf(tmp, "%s\\LowTexture\\", CHARACTERDATAPATH);
	}
	g_ContTexture.SetFilePath(tmp);
	Z3DTexture::_Init(BaseGraphicsLayer::GetDevice(), CHARACTERTEXTUREDATAPATH, CHRTEXTURESIZE);
	CZ3DRenderable::_Init(BaseGraphicsLayer::GetDevice());	

	CZ3DCharacterModel::_Init(CHARACTERDATAPATH);	
	if( false == CZ3DGeneralChrModel::_BuildCMDSList() )
	{
		return;
	}

	sprintf(tmp, "%s/ChrEvent.EDS", CHARACTERDATAPATH);
	if( false == CZ3DChrEventGenerator::_Load( tmp ) )
	{
		return;
	}
*/
	_Z3DGlobalInit( CHARACTERDATAPATH, BaseGraphicsLayer::GetDevice() );


	/*H3DTextureTag ttag;
	g_ContTexture.GetObject( ttag, "fx_item_marker_gradation.dds" );
	g_ContTexture.GetObject( ttag, "fx_bomb_04.dds" );
	g_ContTexture.GetObject( ttag, "fx_fire2.dds" );*/
	
	//sprintf(tmp, "%sSound\\", CHARACTERDATAPATH);
	//CZ3DSound::Init(BaseGraphicsLayer::GethWnd(), Z3DSOUND_3D, SOUNDFILEPATH );
	//g_ContTexture.GetObjectByHandle( 6100018 );
	//CZ3DSkeletonObject* pObj = new CZ3DSkeletonObject();
	//pObj->Link(NULL);

	
	char strSoundTable[256];
	sprintf(strSoundTable,"%s%s",CHARACTERDATAPATH,"EventSoundFile.EDS");	
	if(g_MapEvent2SndFile.Load(strSoundTable,SOUNDFILEPATH,g_TokEventName)==false)
	{

	}

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		LPD3DXBUFFER pCode;	

		DWORD dwDecl[] = 
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG(0,   D3DVSDT_FLOAT3), // position
			D3DVSD_REG(3,   D3DVSDT_FLOAT3), // normal        
			D3DVSD_REG(7,   D3DVSDT_FLOAT2), // tex
			D3DVSD_REG(8,   D3DVSDT_FLOAT3), // S		
			D3DVSD_END()
		};	

		
		D3DXAssembleShader(strCharacterPerPixelLightingPixelShader,strlen(strCharacterPerPixelLightingPixelShader),0,NULL,&pCode,NULL);										 
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwCharacterPixelShader);
		pCode->Release();

		D3DXAssembleShader(strChracterSpecShader,strlen(strChracterSpecShader),0,NULL,&pCode,NULL);										 
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwCharacterSpecPixelShader);    	
		D3DXAssembleShader(strVertexShader,strlen(strVertexShader),0,NULL,&pCode,NULL);	

		
		BaseGraphicsLayer::GetDevice()->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwCharacterVertexShader, 0 );
	
	}
	
}
void CCharacterLightShadowManager::RenderEnv(LPDIRECT3DDEVICE8 pd3dDevice)
{

	//CSceneManager::m_bEnv = true;
	if(!CRenderOption::m_CharacterPerPixelLighting)
	{		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);		
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);		

		pd3dDevice->LightEnable(0,TRUE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		

		D3DLIGHT8 light; 
		ZeroMemory( &light, sizeof(D3DLIGHT8) );
		light.Type        = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r   = CSceneManager::m_WeatherManager.m_InterCharacterLight.r/255.0f;
		light.Diffuse.g   = CSceneManager::m_WeatherManager.m_InterCharacterLight.g/255.0f;
		light.Diffuse.b   = CSceneManager::m_WeatherManager.m_InterCharacterLight.b/255.0f;		
		vector3 vecLight(-1.0f,-1.0f,-1.0f);
		vecLight.Normalize();
		light.Position.x   = light.Direction.x =vecLight.x;
		light.Position.y   = light.Direction.y =vecLight.y;
		light.Position.z   = light.Direction.z =vecLight.z;
		light.Range        = 1000.0f;		

		pd3dDevice->SetLight(0,&light);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterCharacterAmbient.c);		

		
		

	}
	/*	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG2);	
	pd3dDevice->SetTexture(2,CSceneManager::m_LightEffectManager.m_CharcterEnvTexture.GetTexture());
	
	D3DXMATRIX mat;
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT00, F2DW(0.1f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT01, F2DW(0.0f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT10, F2DW(0.0f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT11, F2DW(0.1f) );
	
	mat._11 = -0.5f;	mat._12 = 0;	mat._13 = 0;	mat._14 = 0;
	mat._21 = 0;	mat._22 = -0.5f;	mat._23 = 0;	mat._24 = 0;
	mat._31 = 0;	mat._32 = 0;	mat._33 = 0;	mat._34 = 0;
	mat._41 = 0.5f;	mat._42 = 0.5f;	mat._43 = 0;	mat._44 = 0;
	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &mat );	

	pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );	
	

	pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);	

	
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	pd3dDevice->SetVertexShader(BUMPVERTEXFVF);
	pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
	*/
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	//pd3dDevice->SetVertexShader(m_dwCharacterVertexShader);
	//pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
	
		

	matrix matWorld;
	matWorld.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

	CSceneNode CharScene;
	CharScene.m_fRad=300.0f;
	vector3 vecCharPos;
	vector3 vecCharScreenPos;
	float fW;

	for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
	{		
		if(m_CharacterList[cChar].m_bRender==false)
		{
			m_CharacterList[cChar].m_pChrmodel->SetCull(true);
			m_CharacterList[cChar].m_pChrmodel->SetDisable(true);
		}
		else
		{
			m_CharacterList[cChar].m_pChrmodel->SetCull(false);
			m_CharacterList[cChar].m_pChrmodel->SetDisable(false);
		}
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{			
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);
			CharScene.m_AccumulateTM.Translation(vecCharPos);
			
			if(CharScene.isCulling())
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(false);					
				BaseGraphicsLayer::TransformVector(vecCharPos,vecCharScreenPos,fW);
				m_CharacterList[cChar].m_ptScreenPosition.x=(int)vecCharScreenPos.x;
				m_CharacterList[cChar].m_ptScreenPosition.y=(int)vecCharScreenPos.y;				
				//m_CharacterScreenPositionList[cChar].x=(int)vecCharScreenPos.x;
				//m_CharacterScreenPositionList[cChar].y=(int)vecCharScreenPos.y;

				//BaseGraphicsLayer::TransformVector(m_InstanceObjectList[cObject].m_vecPosition,vecResult,w);
			}
			else
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(true);
				m_CharacterList[cChar].m_ptScreenPosition.x=-1;
				m_CharacterList[cChar].m_ptScreenPosition.y=-1;
				//m_CharacterScreenPositionList[cChar].x=-1;
				//m_CharacterScreenPositionList[cChar].y=-1;
			}
		}		
		/*
		if(cChar==0)
			m_CharacterList[cChar]->SetCull(false);
			*/
	}

	CZ3DRenderable::Process();	
	/*
	*/	

	char *pShadowCollision;
	vector3 vecChrPos;

	float fInterVal=SECTORSIZE/256.0f;
	int px,pz;
	float fPosx,fPosz;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	if(0)//CRenderOption::m_CharacterPerPixelLighting)
	{		

		D3DXVECTOR4 LightDir=D3DXVECTOR4( -1.0f, -1.0f, -1.0f, 0.0f );
		D3DXVec4Normalize(&LightDir,&LightDir);
		pd3dDevice->SetVertexShaderConstant(12, &LightDir,  1 );	// light direction   

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		//pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );	
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);	
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

		
		pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_POINT);		
		pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_POINT);
	}
	
	vector3 vecSunLightDir( CSceneManager::m_WeatherManager.m_SunLight.Direction.x,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.y,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.z);

	
	float fBias=0.0f;
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));	

	color m_InterCharacterAmbient;
	color m_InterCharacterLight;
	
	//float fAmbientColor[4]={0.0f,0.0f,0.0f,0.0f};
	float fAmbientColor[4]={CSceneManager::m_WeatherManager.m_InterCharacterAmbient.r/255.0f,
							CSceneManager::m_WeatherManager.m_InterCharacterAmbient.g/255.0f,
							CSceneManager::m_WeatherManager.m_InterCharacterAmbient.b/255.0f,1.0f};	


	float fShadowColor[4]={0.0f,0.0f,0.0f,0.0f};
	
	float fLightColor[4]={CSceneManager::m_WeatherManager.m_InterCharacterLight.r/255.0f,
						  CSceneManager::m_WeatherManager.m_InterCharacterLight.g/255.0f,
						  CSceneManager::m_WeatherManager.m_InterCharacterLight.b/255.0f,1.0f};						

	float fBackLightColor[4]={fLightColor[0]*0.6f,fLightColor[1]*0.6f,fLightColor[2]*0.6f,1.0f};

	pd3dDevice->SetPixelShaderConstant(0,fAmbientColor,1);
	pd3dDevice->SetPixelShaderConstant(1,fShadowColor,1);
	pd3dDevice->SetPixelShaderConstant(2,fBackLightColor,1);		
	pd3dDevice->SetPixelShaderConstant(3,fLightColor,1);


	for(int cChar=(int)m_CharacterList.size()-1 ;cChar >= 0  ;cChar--)
	{		
		if(m_CharacterList[cChar].m_bRender==false)
			continue;		
		D3DVIEWPORT8 m_pTempViewPort;
		pd3dDevice->GetViewport(&m_pTempViewPort);
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);	

		bool bInShadowCheck=false;
		
		if( m_CharacterList[cChar].m_CollisionDetectType == CDT_FULL)
			//(m_CharacterList[cChar].m_CollisionType == CT_TERRAIN))
		{
			bInShadowCheck=true;
		}
		else if( m_CharacterList[cChar].m_CollisionDetectType==CDT_ONLYTERRAIN )
		{
			bInShadowCheck=true;
		}
		else if( m_CharacterList[cChar].m_CollisionDetectType==CDT_NONE)
		{
			bInShadowCheck=true;
		}		

		//if( m_CharacterList[cChar].m_CollisionType != CT_BOTTOM	)			
		if(bInShadowCheck)			
		{
			// 밤이면 modulate
			if((CSceneManager::GetWeatherTime() <= 19) && (CSceneManager::GetWeatherTime() >= 5)) {

				pShadowCollision=m_pHeightField->GetHeightFieldShadowCollision(vecChrPos);	
				px=(int)(vecChrPos.x/SECTORSIZE);
				pz=(int)(vecChrPos.z/SECTORSIZE);
				fPosx=vecChrPos.x-(px*SECTORSIZE);
				fPosz=vecChrPos.z-(pz*SECTORSIZE);	
				
				if(pShadowCollision)	
				{
					fPosx/=fInterVal;
					fPosz/=fInterVal;
					if(pShadowCollision[(int)fPosx+((int)fPosz)*256]&0x01)
					{
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff999999);
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);
						pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
					}
					else
					{
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);						
						pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

						if(BaseGraphicsLayer::m_VoodooOption)
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
						else
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
					}			
				}
				else
				{
					pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
					//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);						
					
				}
			}
			else	// 밤일때
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			}
		}
		else
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
		}
		
		pd3dDevice->SetStreamSource( 0, NULL, 0 );
		pd3dDevice->SetStreamSource( 1, NULL, 0 );
		pd3dDevice->SetStreamSource( 2, NULL, 0 );

		if(m_CharacterList[cChar].m_Light.m_dwTimer>0)		
		//if(m_CharacterLightList[cChar].m_dwTimer>0)
		{
			//pd3dDevice->SetLight(1,&m_CharacterLightList[cChar].m_Light);
			pd3dDevice->SetLight(1,&m_CharacterList[cChar].m_Light.m_Light);
			pd3dDevice->LightEnable(1,TRUE);
			pd3dDevice->LightEnable(0,FALSE);
		}
		else
		{			
			pd3dDevice->LightEnable(1,FALSE);
			pd3dDevice->LightEnable(0,TRUE);
		}		
////////////////
		//Sector Light 캐릭터 세팅
		if((CSceneManager::GetWeatherTime() > 19) || (CSceneManager::GetWeatherTime() < 5))
		{
			D3DXVECTOR3 vecPos = D3DXVECTOR3(m_CharacterList[cChar].m_vecRealPosition.x,
											m_CharacterList[cChar].m_vecRealPosition.y,
											m_CharacterList[cChar].m_vecRealPosition.z);


			int indexx=(int)(vecPos.x/SECTORSIZE);
			int indexy=(int)(vecPos.z/SECTORSIZE);
			for(int cSector = 0; cSector < CSceneManager::m_HeightField.GetLSizeX()*CSceneManager::m_HeightField.GetLSizeY(); cSector++ )
			{

				int sectorindexx = CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
				int sectorindexy = CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;
				
				if(sectorindexx == indexx &&
					sectorindexy == indexy)
				{
					CSectorScene *pSector = &(CSceneManager::m_HeightField.m_SectorScene[cSector]);
					if(pSector != NULL){
						pSector->m_SectorLightManager.SelectObjLight(vecPos.x,vecPos.y,vecPos.z);
						if(pSector->m_SectorLightManager.m_iSelectObjLights)
						{
							D3DLIGHT8 pCurrentLight;
						
							ZeroMemory(&(pCurrentLight),sizeof(D3DLIGHT8));
							pCurrentLight.Type=D3DLIGHT_POINT;					
							pCurrentLight.Attenuation0=0.2f;					
							pCurrentLight.Attenuation1=0.005f;
							pCurrentLight.Attenuation2=0.0f;
							
							pCurrentLight.Diffuse.r = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.x / 255.0f);
							pCurrentLight.Diffuse.g = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.y / 255.0f);
							pCurrentLight.Diffuse.b = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.z / 255.0f);

							pCurrentLight.Position.x = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.x;
							pCurrentLight.Position.y = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.y;
							pCurrentLight.Position.z = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.z;
							pCurrentLight.Range= pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_fRange;
							pCurrentLight.Falloff = 1.0f;

							pd3dDevice->SetLight(0,&(pCurrentLight));
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
								
							//pd3dDevice->LightEnable(1,TRUE);
						}
						else
						{
							//pd3dDevice->LightEnable(1,FALSE);
						}
					}
				}
				

			}

		}

/////////////////
		
		if(0)//CRenderOption::m_CharacterPerPixelLighting)
		{
			D3DXMATRIX matWorld,matView,matProj;
			vector3 vecChrPos;
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos);
			D3DXMatrixTranslation(&matWorld,vecChrPos.x,vecChrPos.y,vecChrPos.z);

			pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);				

			D3DXMATRIX  matTemp;
			D3DXMatrixTranspose(&matTemp,&(matWorld * matView * matProj));
			pd3dDevice->SetVertexShaderConstant(8, &matTemp, 4);			
			
			D3DXMatrixTranspose(&matTemp,&matWorld);
			pd3dDevice->SetVertexShaderConstant(0, &matTemp, 4);
			
			
			D3DXVECTOR4 tmp;
			tmp.x = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().x;
			tmp.y = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().y;
			tmp.z = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().z;
			tmp.w = 0.0f;	
			
			pd3dDevice->SetVertexShaderConstant(24, tmp, 1);		
			
			vecChrPos.y+=150.0f;

			
			vector3 vecViewPos(tmp.x,tmp.y,tmp.z);
			vector3 vecRelateLight=vecChrPos-vecViewPos;
			//vecRelateLight.y=0.0f;
			vecRelateLight.Normalize();
			//vecRelateLight.y=0.5f;
			//vecRelateLight.Normalize();

			float fRelateLight[4]={vecRelateLight.x,vecRelateLight.y,vecRelateLight.z,0.0f};

			pd3dDevice->SetVertexShaderConstant(13,fRelateLight,1);
			
			D3DXVECTOR4 half(0.5f,0.5f,0.5f,0.5f);
			pd3dDevice->SetVertexShaderConstant(33, &half, 1);			
			
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		}	
		
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{
			
			m_CharacterList[cChar].m_pChrmodel->PreRender();

			
			if(0)//CRenderOption::m_CharacterPerPixelLighting)
			{
				pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
				pd3dDevice->SetVertexShader(m_dwCharacterVertexShader);
			}			

			pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);	
			if( (m_CharacterList[cChar].m_fAlphaValue) > 0.0f )
			{
				pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);				
				pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
				pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

				color alpha;
				alpha.r = 255;
				alpha.g = 255;
				alpha.b = 255;
				alpha.a = m_CharacterList[cChar].m_fAlphaValue * 255;

				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, alpha.c );
				
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);		


			}
			//////////////////////////
		
	/*		CSceneManager::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			CSceneManager::GetDevice()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			CSceneManager::GetDevice()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);	
*/
			m_CharacterList[cChar].m_pChrmodel->Render();
			CSceneManager::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);	
		

	
			////////////////////////////

			if( m_CharacterList[cChar].m_fAlphaValue > 0.0f )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
			}

		}		
	}	
	
	fBias=0.0f;
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	

	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);			
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		


	pd3dDevice->SetPixelShader(NULL);	
	CZ3DRenderable::SecondProcess();

	for(int i=1;i<8;i++)
		pd3dDevice->LightEnable(i,FALSE);	

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );	
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);	
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);	
	pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	matrix mat;
	mat._11 = 1.0f;	mat._12 = 0;	mat._13 = 0;	mat._14 = 0;
	mat._21 = 0;	mat._22 = 1.0f;	mat._23 = 0;	mat._24 = 0;
	mat._31 = 0;	mat._32 = 0;	mat._33 = 1.0f;	mat._34 = 0;
	mat._41 = 0;	mat._42 = 0;	mat._43 = 0;	mat._44 = 1.0f;
	pd3dDevice->SetTransform( D3DTS_TEXTURE1, mat );
	
	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE);	
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetTexture(2,NULL);
	pd3dDevice->SetTexture(3,NULL);
	

}
void CCharacterLightShadowManager::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	if(!CRenderOption::m_CharacterPerPixelLighting)
	{		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);		
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);		

		pd3dDevice->LightEnable(0,TRUE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		

		D3DLIGHT8 light; 
		ZeroMemory( &light, sizeof(D3DLIGHT8) );
		light.Type        = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r   = CSceneManager::m_WeatherManager.m_InterCharacterLight.r/255.0f;
		light.Diffuse.g   = CSceneManager::m_WeatherManager.m_InterCharacterLight.g/255.0f;
		light.Diffuse.b   = CSceneManager::m_WeatherManager.m_InterCharacterLight.b/255.0f;		
		vector3 vecLight(-1.0f,-1.0f,-1.0f);
		vecLight.Normalize();
		light.Position.x   = light.Direction.x =vecLight.x;
		light.Position.y   = light.Direction.y =vecLight.y;
		light.Position.z   = light.Direction.z =vecLight.z;
		light.Range        = 1000.0f;		

		pd3dDevice->SetLight(0,&light);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterCharacterAmbient.c);		

		
		

	}

	D3DVIEWPORT8 vp;
	pd3dDevice->GetViewport( &vp );
	CZ3DGlowHandler::_SetRenderViewport( vp );
	D3DMATRIX matProj;
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	CZ3DGlowHandler::_SetRenderProjectionMatrix( matProj );

	/*	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG2);	
	pd3dDevice->SetTexture(2,CSceneManager::m_LightEffectManager.m_CharcterEnvTexture.GetTexture());
	
	D3DXMATRIX mat;
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT00, F2DW(0.1f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT01, F2DW(0.0f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT10, F2DW(0.0f) );
    pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT11, F2DW(0.1f) );
	
	mat._11 = -0.5f;	mat._12 = 0;	mat._13 = 0;	mat._14 = 0;
	mat._21 = 0;	mat._22 = -0.5f;	mat._23 = 0;	mat._24 = 0;
	mat._31 = 0;	mat._32 = 0;	mat._33 = 0;	mat._34 = 0;
	mat._41 = 0.5f;	mat._42 = 0.5f;	mat._43 = 0;	mat._44 = 0;
	pd3dDevice->SetTransform( D3DTS_TEXTURE1, &mat );	

	pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );	
	

	pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);	

	
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	pd3dDevice->SetVertexShader(BUMPVERTEXFVF);
	pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
	*/
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	//pd3dDevice->SetVertexShader(m_dwCharacterVertexShader);
	//pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
	
		

	matrix matWorld;
	matWorld.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

	CSceneNode CharScene;
	CharScene.m_fRad=300.0f;
	vector3 vecCharPos;
	vector3 vecCharScreenPos;
	float fW;
// Minotaurs 캐릭터 랜더
	for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
	{		
		if (m_CharacterList[cChar].m_bHide) continue;	// Hide 캐릭터 렌더 안함

		if(m_CharacterList[cChar].m_bRender==false)
		{
			m_CharacterList[cChar].m_pChrmodel->SetCull(true);
			m_CharacterList[cChar].m_pChrmodel->SetDisable(true);
		}
		else
		{
			m_CharacterList[cChar].m_pChrmodel->SetCull(false);
			m_CharacterList[cChar].m_pChrmodel->SetDisable(false);
		}
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{			
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);
			
			vector3 vecMin,vecMax;
			m_CharacterList[cChar].m_pChrmodel->GetBoundingBox(vecMin,vecMax);
			vecMax -= vecMin;
			
			float fLength = vecMax.GetLens();
			CharScene.m_fRad = fLength;

			CharScene.m_AccumulateTM.Translation(vecCharPos);
			if(CharScene.isCulling() )
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(false);					
				BaseGraphicsLayer::TransformVector(vecCharPos,vecCharScreenPos,fW);
				m_CharacterList[cChar].m_ptScreenPosition.x=(int)vecCharScreenPos.x;
				m_CharacterList[cChar].m_ptScreenPosition.y=(int)vecCharScreenPos.y;				
				//m_CharacterScreenPositionList[cChar].x=(int)vecCharScreenPos.x;
				//m_CharacterScreenPositionList[cChar].y=(int)vecCharScreenPos.y;

				//BaseGraphicsLayer::TransformVector(m_InstanceObjectList[cObject].m_vecPosition,vecResult,w);
			}
			else
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(true);
				m_CharacterList[cChar].m_ptScreenPosition.x=-1;
				m_CharacterList[cChar].m_ptScreenPosition.y=-1;
				//m_CharacterScreenPositionList[cChar].x=-1;
				//m_CharacterScreenPositionList[cChar].y=-1;
			}
		}		
		/*
		if(cChar==0)
			m_CharacterList[cChar]->SetCull(false);
			*/
	}

	CZ3DRenderable::Process();	
	/*
	*/	

	char *pShadowCollision;
	vector3 vecChrPos;

	float fInterVal=SECTORSIZE/256.0f;
	int px,pz;
	float fPosx,fPosz;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	if(CRenderOption::m_CharacterPerPixelLighting)
	{		

		D3DXVECTOR4 LightDir=D3DXVECTOR4( -1.0f, -1.0f, -1.0f, 0.0f );
		D3DXVec4Normalize(&LightDir,&LightDir);
		pd3dDevice->SetVertexShaderConstant(12, &LightDir,  1 );	// light direction   

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		//pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );	
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);	
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

		
		pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_POINT);		
		pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_POINT);
	}
	
	vector3 vecSunLightDir( CSceneManager::m_WeatherManager.m_SunLight.Direction.x,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.y,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.z);

	
	float fBias=0.0f;
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));	

	color m_InterCharacterAmbient;
	color m_InterCharacterLight;
	
	//float fAmbientColor[4]={0.0f,0.0f,0.0f,0.0f};
	float fAmbientColor[4]={CSceneManager::m_WeatherManager.m_InterCharacterAmbient.r/255.0f,
							CSceneManager::m_WeatherManager.m_InterCharacterAmbient.g/255.0f,
							CSceneManager::m_WeatherManager.m_InterCharacterAmbient.b/255.0f,1.0f};	


	float fShadowColor[4]={0.0f,0.0f,0.0f,0.0f};
	
	float fLightColor[4]={CSceneManager::m_WeatherManager.m_InterCharacterLight.r/255.0f,
						  CSceneManager::m_WeatherManager.m_InterCharacterLight.g/255.0f,
						  CSceneManager::m_WeatherManager.m_InterCharacterLight.b/255.0f,1.0f};						

	float fBackLightColor[4]={fLightColor[0]*0.6f,fLightColor[1]*0.6f,fLightColor[2]*0.6f,1.0f};

	pd3dDevice->SetPixelShaderConstant(0,fAmbientColor,1);
	pd3dDevice->SetPixelShaderConstant(1,fShadowColor,1);
	pd3dDevice->SetPixelShaderConstant(2,fBackLightColor,1);		
	pd3dDevice->SetPixelShaderConstant(3,fLightColor,1);


	for(int cChar=(int)m_CharacterList.size()-1 ;cChar >= 0  ;cChar--)
	{		
		if (m_CharacterList[cChar].m_bHide) continue;	// Hide 캐릭터 렌더 안함

		if(m_CharacterList[cChar].m_bRender==false)
			continue;		
		D3DVIEWPORT8 m_pTempViewPort;
		pd3dDevice->GetViewport(&m_pTempViewPort);
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);	

		bool bInShadowCheck=false;
		
		if( m_CharacterList[cChar].m_CollisionDetectType == CDT_FULL ||
			m_CharacterList[cChar].m_CollisionDetectType == CDT_SKYUNIT)
			//(m_CharacterList[cChar].m_CollisionType == CT_TERRAIN))
		{
			bInShadowCheck=true;
		}
		else if( m_CharacterList[cChar].m_CollisionDetectType==CDT_ONLYTERRAIN )
		{
			bInShadowCheck=true;
		}
		else if( m_CharacterList[cChar].m_CollisionDetectType==CDT_NONE)
		{
			bInShadowCheck=true;
		}		

		//if( m_CharacterList[cChar].m_CollisionType != CT_BOTTOM	)			
		if(bInShadowCheck)			
		{
			// 밤이면 modulate
			if(!CSceneManager::GetInView() && (CSceneManager::GetWeatherTime() <= 19) && (CSceneManager::GetWeatherTime() >= 5)) {

				pShadowCollision=m_pHeightField->GetHeightFieldShadowCollision(vecChrPos);	
				px=(int)(vecChrPos.x/SECTORSIZE);
				pz=(int)(vecChrPos.z/SECTORSIZE);
				fPosx=vecChrPos.x-(px*SECTORSIZE);
				fPosz=vecChrPos.z-(pz*SECTORSIZE);	
				
				if(pShadowCollision)	
				{
					fPosx/=fInterVal;
					fPosz/=fInterVal;
					if(pShadowCollision[(int)fPosx+((int)fPosz)*256]&0x01)
					{
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff999999);
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);
						pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
					}
					else
					{
						//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);						
						pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

						if(BaseGraphicsLayer::m_VoodooOption)
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
						else
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
					}			
				}
				else
				{
					pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
					//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);						
					
				}
			}
			else if(CSceneManager::GetInView())
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			}
			else	// 밤일때
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			}
		}
		else
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
		}
		
		pd3dDevice->SetStreamSource( 0, NULL, 0 );
		pd3dDevice->SetStreamSource( 1, NULL, 0 );
		pd3dDevice->SetStreamSource( 2, NULL, 0 );

		if(m_CharacterList[cChar].m_Light.m_dwTimer>0)		
		//if(m_CharacterLightList[cChar].m_dwTimer>0)
		{
			//pd3dDevice->SetLight(1,&m_CharacterLightList[cChar].m_Light);
			pd3dDevice->SetLight(1,&m_CharacterList[cChar].m_Light.m_Light);
			pd3dDevice->LightEnable(1,TRUE);
			pd3dDevice->LightEnable(0,FALSE);
		}
		else
		{			
			pd3dDevice->LightEnable(1,FALSE);
			pd3dDevice->LightEnable(0,TRUE);
		}		
////////////////
		//Sector Light 캐릭터 세팅
		if(CSceneManager::GetInView() || (CSceneManager::GetWeatherTime() > 19) || (CSceneManager::GetWeatherTime() < 5))
		{
			D3DXVECTOR3 vecPos = D3DXVECTOR3(m_CharacterList[cChar].m_vecRealPosition.x,
											m_CharacterList[cChar].m_vecRealPosition.y,
											m_CharacterList[cChar].m_vecRealPosition.z);


			int indexx=(int)(vecPos.x/SECTORSIZE);
			int indexy=(int)(vecPos.z/SECTORSIZE);
			for(int cSector = 0; cSector < CSceneManager::m_HeightField.GetLSizeX()*CSceneManager::m_HeightField.GetLSizeY(); cSector++ )
			{

				int sectorindexx = CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
				int sectorindexy = CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;
				
				if(sectorindexx == indexx &&
					sectorindexy == indexy)
				{
					CSectorScene *pSector = &(CSceneManager::m_HeightField.m_SectorScene[cSector]);
					if(pSector != NULL){
						pSector->m_SectorLightManager.SelectObjLight(vecPos.x,vecPos.y,vecPos.z);
						if(pSector->m_SectorLightManager.m_iSelectObjLights)
						{
							D3DLIGHT8 pCurrentLight;
						
							ZeroMemory(&(pCurrentLight),sizeof(D3DLIGHT8));
							pCurrentLight.Type=D3DLIGHT_POINT;					
							pCurrentLight.Attenuation0=0.2f;					
							pCurrentLight.Attenuation1=0.005f;
							pCurrentLight.Attenuation2=0.0f;
							
							pCurrentLight.Diffuse.r = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.x / 255.0f);
							pCurrentLight.Diffuse.g = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.y / 255.0f);
							pCurrentLight.Diffuse.b = (pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecColor.z / 255.0f);

							pCurrentLight.Position.x = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.x;
							pCurrentLight.Position.y = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.y;
							pCurrentLight.Position.z = pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_vecPos.z;
							pCurrentLight.Range= pSector->m_SectorLightManager.m_lstSelectObjLights[0].m_fRange;
							pCurrentLight.Falloff = 1.0f;

							pd3dDevice->SetLight(0,&(pCurrentLight));
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
								
							//pd3dDevice->LightEnable(1,TRUE);
						}
						else
						{
							//pd3dDevice->LightEnable(1,FALSE);
						}
					}
				}
				

			}

		}
		if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
		{
			bool bLight = true;

			if(cChar == 0)							// 자신이 아니라면.. nfocuschar
			{
														/*
				D3DXVECTOR3 vecPos = D3DXVECTOR3(m_CharacterList[cChar].m_vecRealPosition.x,
														m_CharacterList[cChar].m_vecRealPosition.y,
														m_CharacterList[cChar].m_vecRealPosition.z);
				*/
				matrix *view = CSceneManager::m_ViewCamera->GetMatPosition();

				D3DXVECTOR3 vecPos = D3DXVECTOR3(view->_41,view->_42,view->_43);
				D3DXVECTOR3 vecPersonPos = D3DXVECTOR3(m_CharacterList[cChar].m_vecRealPosition.x,
														m_CharacterList[cChar].m_vecRealPosition.y,
														m_CharacterList[cChar].m_vecRealPosition.z);

				D3DXVECTOR3 vecTmp = vecPos - vecPersonPos;
				D3DXVec3Normalize(&vecTmp,&vecTmp);
				
				CSceneManager::m_CharacterLight.Position.x = vecPersonPos.x + vecTmp.x * 250.0f;
				CSceneManager::m_CharacterLight.Position.y = vecPersonPos.y + vecTmp.y * 250.0f;
				CSceneManager::m_CharacterLight.Position.z = vecPersonPos.z + vecTmp.z * 250.0f;

				CSceneManager::m_CharacterLight.Range = 3000.f; 
			}
			else 
			{
				D3DXVECTOR3 vecPos = D3DXVECTOR3(m_CharacterList[cChar].m_vecRealPosition.x,
													m_CharacterList[cChar].m_vecRealPosition.y,
													m_CharacterList[cChar].m_vecRealPosition.z);

				
				D3DXVECTOR3 vecRange = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x - vecPos.x,
													CSceneManager::m_CharacterLight.Position.y - vecPos.y,
													CSceneManager::m_CharacterLight.Position.z - vecPos.z);
				float fRange = D3DXVec3Length(&vecRange);
				if(fRange > CSceneManager::m_CharacterLight.Range)
				{
					bLight = false;	
				}

			}
			if(bLight)
			{
				pd3dDevice->SetLight(0,&(CSceneManager::m_CharacterLight));
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
			}		
		}
/////////////////
		
		if(CRenderOption::m_CharacterPerPixelLighting)
		{
			D3DXMATRIX matWorld,matView,matProj;
			vector3 vecChrPos;
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos);
			D3DXMatrixTranslation(&matWorld,vecChrPos.x,vecChrPos.y,vecChrPos.z);

			pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);				

			D3DXMATRIX  matTemp;
			D3DXMatrixTranspose(&matTemp,&(matWorld * matView * matProj));
			pd3dDevice->SetVertexShaderConstant(8, &matTemp, 4);			
			
			D3DXMatrixTranspose(&matTemp,&matWorld);
			pd3dDevice->SetVertexShaderConstant(0, &matTemp, 4);
			
			
			D3DXVECTOR4 tmp;
			tmp.x = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().x;
			tmp.y = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().y;
			tmp.z = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().z;
			tmp.w = 0.0f;	
			
			pd3dDevice->SetVertexShaderConstant(24, tmp, 1);		
			
			vecChrPos.y+=150.0f;

			
			vector3 vecViewPos(tmp.x,tmp.y,tmp.z);
			vector3 vecRelateLight=vecChrPos-vecViewPos;
			//vecRelateLight.y=0.0f;
			vecRelateLight.Normalize();
			//vecRelateLight.y=0.5f;
			//vecRelateLight.Normalize();

			float fRelateLight[4]={vecRelateLight.x,vecRelateLight.y,vecRelateLight.z,0.0f};

			pd3dDevice->SetVertexShaderConstant(13,fRelateLight,1);
			
			D3DXVECTOR4 half(0.5f,0.5f,0.5f,0.5f);
			pd3dDevice->SetVertexShaderConstant(33, &half, 1);			
			
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		}	

		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{
			
			m_CharacterList[cChar].m_pChrmodel->PreRender();

			
			if(CRenderOption::m_CharacterPerPixelLighting)
			{
				pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
				pd3dDevice->SetVertexShader(m_dwCharacterVertexShader);
			}			

			if( m_CharacterList[cChar].m_fAlphaValue > 0.0f || m_CharacterList[cChar].m_pChrmodel->IsAlphaUsed() )
			{
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);		

				color alpha;
				alpha.r = 255;
				alpha.g = 255;
				alpha.b = 255;
				alpha.a = m_CharacterList[cChar].m_fAlphaValue * 255;
				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, alpha.c );

				if( m_CharacterList[cChar].m_fAlphaValue > 0.0f && m_CharacterList[cChar].m_pChrmodel->IsAlphaUsed() )
				{
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

					pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
				}
				else if( m_CharacterList[cChar].m_pChrmodel->IsAlphaUsed() )
				{
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);

					pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
					pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
					pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xA0 );
				}
				else	// if( m_fAlphaValue > 0.0f )
				{
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
					pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);

					pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
				}

				pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			}
			else
			{
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
				pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
			}

			m_CharacterList[cChar].m_pChrmodel->Render();

			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
		}		
	}	
	
	fBias=0.0f;
	pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	

	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);			
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		


	pd3dDevice->SetPixelShader(NULL);	
	CZ3DRenderable::SecondProcess();

	for(int i=1;i<8;i++)
		pd3dDevice->LightEnable(i,FALSE);	

	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );	
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);	
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);	
	pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	matrix mat;
	mat._11 = 1.0f;	mat._12 = 0;	mat._13 = 0;	mat._14 = 0;
	mat._21 = 0;	mat._22 = 1.0f;	mat._23 = 0;	mat._24 = 0;
	mat._31 = 0;	mat._32 = 0;	mat._33 = 1.0f;	mat._34 = 0;
	mat._41 = 0;	mat._42 = 0;	mat._43 = 0;	mat._44 = 1.0f;
	pd3dDevice->SetTransform( D3DTS_TEXTURE1, mat );
	
	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE);	
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetTexture(2,NULL);
	pd3dDevice->SetTexture(3,NULL);
	
	/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
	pd3dDevice->SetLight(0,&m_WeatherManager.m_SunLight);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,m_WeatherManager.m_AmbientColor->c);
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	CSceneNode CharScene;
	CharScene.m_fRad=300.0f;
	vector3 vecCharPos;
	for(int cChar=0;cChar<m_CharacterList.num;cChar++)
	{
		m_CharacterList[cChar]->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.x);
		CharScene.m_AccumulateTM.Translation(vecCharPos);
		
		if(CharScene.isCulling())
			m_CharacterList[cChar]->SetCull(true);
		else
			m_CharacterList[cChar]->SetCull(false);
		if(cChar==0)
			m_CharacterList[cChar]->SetCull(false);		
	}		
	CZ3DRenderable::Process();
	*/
}
void CCharacterLightShadowManager::RenderGlow(IDirect3DDevice8* pDevice)
{
	CZ3DGlowHandler::_PrepareGlowRender();

	for( int i = (int)m_CharacterList.size()-1 ; i >= 0  ;i--)
	{		
		if(m_CharacterList[i].m_pChrmodel->IsDisabled()==false && m_CharacterList[i].m_bHide == false )
		{
			m_CharacterList[i].m_pChrmodel->RenderGlowShape(pDevice);
		}
	}

	CZ3DGlowHandler::_EndGlowRender();
}
void CCharacterLightShadowManager::HeightFieldCharacterLightRender(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CSceneManager::m_bCharacterLight == true)
	{
	
		pd3dDevice->EndScene();		    
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		matrix matTexScale;
		matTexScale.MakeIdent();

		matTexScale._11= 0.5f;
		matTexScale._22= -0.5f;	
		matTexScale._33=0.0f;
		matTexScale._41=0.5f;
		matTexScale._42=0.5f;
		matTexScale._43=1.0f;
		matTexScale._44=1.0f;


		matrix matView,matOldProj,matInv,matTex,matIndent;			
		matIndent.MakeIdent();
		
		pd3dDevice->GetTransform(D3DTS_VIEW,matView);	
		matInv.Inverse(matView);   	

		matrix matAdjLightProj,matLightProj;
		matAdjLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f,1.0f,1.0f,500.0f,0.5f,-0.5f,(float)SHADOWSIZE, (float)SHADOWSIZE);
		matLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f, 1.0f, 1.0f, 500.0f,0.0f, 0.0f,(float)SHADOWSIZE, (float)SHADOWSIZE); 
		
		matrix matShadowProj;
		//matShadowProj.MakeProjection(3.14159f/4.0f,1.0f,10.0f,10000.0f);
		
		matShadowProj.MakeNoProjection(3.14159f/3.0f,(float)SHADOWSIZE+CSceneManager::m_fGlowPlaneRange,(float)SHADOWSIZE+CSceneManager::m_fGlowPlaneRange,10.0f,5500.f);
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->LightEnable(1,FALSE);
		pd3dDevice->LightEnable(2,FALSE);
		pd3dDevice->LightEnable(3,FALSE);
		pd3dDevice->LightEnable(4,FALSE);
		pd3dDevice->LightEnable(5,FALSE);
		pd3dDevice->LightEnable(6,FALSE);
		pd3dDevice->LightEnable(7,FALSE);
		pd3dDevice->LightEnable(8,FALSE);
	//	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	//	pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff999999);
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		vector3 vecSunLightDir( CSceneManager::m_WeatherManager.m_SunLight.Direction.x,
								CSceneManager::m_WeatherManager.m_SunLight.Direction.y,
								CSceneManager::m_WeatherManager.m_SunLight.Direction.z);
		vecSunLightDir.Normalize();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		vector3 vecSelfChrPos;
		vector3 vecLens;

		if(m_CharacterList.size()>0)
		{
			m_CharacterList[0].m_pChrmodel->GetPosition(vecSelfChrPos.x,vecSelfChrPos.y,vecSelfChrPos.z);
		}
		for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
		{
			if (m_CharacterList[cChar].m_bHide) continue;	// Hide 캐릭터 렌더 안함
			if(m_CharacterList[cChar].m_pChrmodel->IsCulled())
				continue;
			if(m_CharacterList[cChar].m_bRender==false)
				continue;		
			
			if(cChar != 0)
				continue;

			vector3 vecChrPos;
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
			if(m_pHeightField->GetHeight(vecChrPos)-10.0f > vecChrPos.y)
				continue;
			if(cChar>0)
			{
				vecLens=vecSelfChrPos-vecChrPos;
				if(vecLens.GetLens()>1500.0f)
					continue;
			}		
			
			matrix matCharCamera;
			
			matCharCamera.CameraLookAt(
				vecChrPos+vector3(0.0f,185.0f,1.0f),
				vector3(0.0f,80.0f,0.0f)+vecChrPos,
				vector3(0.0f,1.0f,0.0f));

			pd3dDevice->BeginScene();	

			std::vector<vector3> CharPoly;	
			m_pHeightField->GetHeightFieldShadowPoly(vecChrPos,CharPoly);
			vector3 vecLightPosList=vecChrPos- (vecChrPos+vector3(0.0f,105.0f,1.0f) );

			if(CRenderOption::m_CharacterProjectShadowBuilding)
				m_pHeightField->GetInHouseShadowPoly(vecChrPos,CharPoly,vecLightPosList);
			//m_pHeightField->GetShadowPoly(vecChrPos,CharPoly);

			TTVertex *pCharVertex=new TTVertex[CharPoly.size()];
			WORD *pCharIndices=new WORD[CharPoly.size()];
			for(int cIndices=0;cIndices<(int)CharPoly.size();cIndices++)
			{
				pCharVertex[cIndices].v=CharPoly[cIndices];
				pCharVertex[cIndices].diff.c=0xffff0000;
				pCharIndices[cIndices]=cIndices;
				//pCharVertex[cIndices].v.y+=2.0f;
			}
			pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		
			matrix mat,mat2;
			mat=matShadowProj*matTexScale;
			//mat=matLightProj*matTexScale;		
			mat2=matCharCamera*mat;
			matTex=matInv*mat2;
			
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);
	//		pd3dDevice->SetTexture(0,m_pCharacterLight.GetTexture());

		
			if(m_pCharacterLight == NULL) {
				m_pCharacterLight = new CTexture;
				CTexture::SetPath(EFFECTTEXTUREPATH);
				m_pCharacterLight->Load("CL_Glow.dds");
			}
			
			pd3dDevice->SetTexture(0,m_pCharacterLight->GetTexture());
	

			pd3dDevice->SetVertexShader(TTVERTEXFVF);	
			
			pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_MODULATE2X);
		
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);
		

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
			
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			
			pd3dDevice->SetTransform(D3DTS_WORLD,matIndent);	

			pd3dDevice->SetRenderState(D3DRS_ZBIAS,14);
			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

			pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

			pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)CharPoly.size(),(int)CharPoly.size()/3,pCharIndices,
				D3DFMT_INDEX16,pCharVertex,sizeof(TTVertex));	
			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

			pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);
			
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			if(pCharVertex) {
				delete [] pCharVertex;
				pCharVertex = NULL;
			}
			if(pCharIndices) {
				delete [] pCharIndices;
				pCharIndices = NULL;
			}
			pd3dDevice->EndScene();
		}
		matTex.MakeIdent();
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		pd3dDevice->BeginScene();
	}
}
void CCharacterLightShadowManager::HeightFieldShadowRender(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	if(!CRenderOption::m_CharacterProjectShadowTerrain)
		return;
	pd3dDevice->EndScene();		    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	matrix matTexScale;
	matTexScale.MakeIdent();
	matTexScale._11=0.5f;
	matTexScale._22=-0.5f;	
	matTexScale._33=0.0f;
	matTexScale._41=0.5f;
	matTexScale._42=0.5f;
	matTexScale._43=1.0f;
	matTexScale._44=1.0f;

	matrix matView,matOldProj,matInv,matTex,matIndent;			
	matIndent.MakeIdent();
	
	pd3dDevice->GetTransform(D3DTS_VIEW,matView);	
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProj);
	matInv.Inverse(matView);   	

	matrix matAdjLightProj,matLightProj;
	matAdjLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f,1.0f,1.0f,500.0f,0.5f,-0.5f,(float)SHADOWSIZE, (float)SHADOWSIZE);
	matLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f, 1.0f, 1.0f, 500.0f,0.0f, 0.0f,(float)SHADOWSIZE, (float)SHADOWSIZE); 
	
	matrix matShadowProj;
	//matShadowProj.MakeProjection(3.14159f/4.0f,1.0f,10.0f,10000.0f);
	matShadowProj.MakeNoProjection(3.14159f/3.0f,(float)SHADOWSIZE+150.0f,(float)SHADOWSIZE+150.0f,10.0f,5500.f);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->LightEnable(1,FALSE);
	pd3dDevice->LightEnable(2,FALSE);
	pd3dDevice->LightEnable(3,FALSE);
	pd3dDevice->LightEnable(4,FALSE);
	pd3dDevice->LightEnable(5,FALSE);
	pd3dDevice->LightEnable(6,FALSE);
	pd3dDevice->LightEnable(7,FALSE);
	pd3dDevice->LightEnable(8,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff999999);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	vector3 vecSunLightDir( CSceneManager::m_WeatherManager.m_SunLight.Direction.x,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.y,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.z);
	vecSunLightDir.Normalize();

	TLVertex Vertex[4];
	Vertex[0].Diffuse.c=Vertex[1].Diffuse.c=Vertex[2].Diffuse.c=Vertex[3].Diffuse.c=0xffffffff;
	Vertex[0].Specular.c=Vertex[1].Specular.c=Vertex[2].Specular.c=Vertex[3].Specular.c=0;
	Vertex[0].w=Vertex[1].w=Vertex[2].w=Vertex[3].w=0.1f;
	Vertex[0].v.z=Vertex[1].v.z=Vertex[2].v.z=Vertex[3].v.z=0.1f;

	Vertex[0].v.x=Vertex[1].v.x=0;
	Vertex[2].v.x=Vertex[3].v.x=SHADOWSIZE-1;
	Vertex[0].v.y=Vertex[2].v.y=0;
	Vertex[1].v.y=Vertex[3].v.y=SHADOWSIZE-1;

	Vertex[0].tu=Vertex[1].tu=0.0f;
	Vertex[2].tu=Vertex[3].tu=1.0f;

	Vertex[0].tv=Vertex[2].tv=0.0f;
	Vertex[1].tv=Vertex[3].tv=1.0f;

	WORD pLineIndices[8]={0,1,1,3,3,2,2,0};
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	vector3 vecSelfChrPos;
	vector3 vecLens;

	if(m_CharacterList.size()>0)
	{
		m_CharacterList[0].m_pChrmodel->GetPosition(vecSelfChrPos.x,vecSelfChrPos.y,vecSelfChrPos.z);
	}
	for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
	{
		if (m_CharacterList[cChar].m_bHide) continue;	// Hide 캐릭터 렌더 안함
		if(m_CharacterList[cChar].m_pChrmodel->IsCulled())
			continue;
		if(m_CharacterList[cChar].m_bRender==false)
			continue;		
		vector3 vecChrPos;
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		if(m_pHeightField->GetHeight(vecChrPos)-10.0f > vecChrPos.y)
			continue;
		if(cChar>0)
		{
			vecLens=vecSelfChrPos-vecChrPos;
			if(vecLens.GetLens()>1500.0f)
				continue;
		}		

		m_pShadowTexture.Begin(pd3dDevice);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->BeginScene();
		matrix matCharCamera;
		matCharCamera.CameraLookAt(
			vecChrPos-vecSunLightDir*550.0f+vector3(0.0f,85.0f,0.0f),
			vector3(0.0f,80.0f,0.0f)+vecChrPos,
			vector3(0.0f,1.0f,0.0f));
		pd3dDevice->SetTransform(D3DTS_VIEW,matCharCamera);	
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matShadowProj);
		//m_pd3dDevice->SetTransform(D3DTS_PROJECTION,matAdjLightProj);		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		if(CRenderOption::m_CharacterPerPixelLighting)
		{
			pd3dDevice->SetVertexShader(BUMPVERTEXFVF);
		}
		m_CharacterList[cChar].m_pChrmodel->DrawShadow(pd3dDevice);

		pd3dDevice->SetVertexShader(TLVERTEXFVF);
		//pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,4,4,pLineIndices,D3DFMT_INDEX16,Vertex,sizeof(TLVertex));
		
		//pd3dDevice->SetRenderState(	D3DRS_CULLMODE ,D3DCULL_NONE);		
		//pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(TLVertex));	
		//pd3dDevice->SetRenderState(	D3DRS_CULLMODE ,D3DCULL_CCW);
		


		pd3dDevice->EndScene();		
		m_pShadowTexture.End(pd3dDevice);	

		pd3dDevice->BeginScene();	
		std::vector<vector3> CharPoly;	
		m_pHeightField->GetHeightFieldShadowPoly(vecChrPos,CharPoly);
		vector3 vecLightPosList=vecChrPos-vecSunLightDir*550.0f;

		if(CRenderOption::m_CharacterProjectShadowBuilding)
			m_pHeightField->GetInHouseShadowPoly(vecChrPos,CharPoly,vecLightPosList);
		//m_pHeightField->GetShadowPoly(vecChrPos,CharPoly);

		TTVertex *pCharVertex=new TTVertex[CharPoly.size()];
		WORD *pCharIndices=new WORD[CharPoly.size()];
		for(int cIndices=0;cIndices<(int)CharPoly.size();cIndices++)
		{
			pCharVertex[cIndices].v=CharPoly[cIndices];
			pCharVertex[cIndices].diff.c=0xffff0000;
			pCharIndices[cIndices]=cIndices;
			//pCharVertex[cIndices].v.y+=2.0f;
		}
		pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		
		matrix mat,mat2;
		mat=matShadowProj*matTexScale;
		//mat=matLightProj*matTexScale;		
		mat2=matCharCamera*mat;
		matTex=matInv*mat2;
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);
		pd3dDevice->SetTexture(0,m_pShadowTexture.GetTexture());
		pd3dDevice->SetVertexShader(TTVERTEXFVF);	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);


		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		pd3dDevice->SetTransform(D3DTS_WORLD,matIndent);	

		pd3dDevice->SetRenderState(D3DRS_ZBIAS,14);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		if(CharPoly.size() > 0) {
			pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)CharPoly.size(),(int)CharPoly.size()/3,pCharIndices,
				D3DFMT_INDEX16,pCharVertex,sizeof(TTVertex));	
		}
		
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);
		
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		if(pCharVertex) {
				delete [] pCharVertex;
				pCharVertex = NULL;
			}
		if(pCharIndices) {
				delete [] pCharIndices;
				pCharIndices = NULL;
		}

	
		pd3dDevice->EndScene();
	}	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->BeginScene();
}

CZ3DGeneralChrModel* CCharacterLightShadowManager::AddCharacter( const char* szGCMDSName, const char* szFaceType, const char* szHairStyle,int iValue,CZ3DGeneralChrModel **ppOld,bool bHouse,int iHouseType ,D3DXVECTOR3 vecPos)
{
	CZ3DGeneralChrModel* pChrModel;

	pChrModel = new CZ3DGeneralChrModel;
	if( false == pChrModel->Init( szGCMDSName ) )
	{		
		return NULL;
	}
	pChrModel->SetFixHeight(false);
	pChrModel->SetStaticSlot( "FACE", szFaceType );
	pChrModel->SetStaticSlot( "HAIR", szHairStyle );	
	pChrModel->SetDisable(false);

	CCharacterDataNode AddNode;
	AddNode.m_pChrmodel=pChrModel;
	AddNode.m_CollisionType=CT_NONE;
	AddNode.m_ptScreenPosition.x=0;
	AddNode.m_ptScreenPosition.y=0;
	AddNode.m_dwEvent=0;

	memset(&AddNode.m_Light,0,sizeof(CChrLightNode));
	AddNode.m_vecPerFrameMove=vector3(0.0f,0.0f,0.0f);
	AddNode.m_vecRealPosition=vector3(0.0f,0.0f,0.0f);
	AddNode.m_RandomPositionTimer=0;	
	AddNode.m_vecRandomPositionAdder=vector3(0.0f,0.0f,0.0f);
	AddNode.m_fFallSpeed=0.0f;

	AddNode.m_fAccelate=0.0f;
	AddNode.m_vecZeroVelocity=vector3(0.0f,0.0f,0.0f);

	AddNode.m_bFirstAccelate=false;	
	AddNode.m_bCollisionDetectAble=true;
	AddNode.m_DelayTimer=0;
	AddNode.m_bBuild = true;

	AddNode.m_bGravityAble=true;	
	AddNode.m_bRender=true;
	AddNode.m_bHide=false;
	if(!bHouse)
		AddNode.m_bSkipCollision = false;
	else
		AddNode.m_bSkipCollision = true;
	AddNode.m_cAttackEvent=0;
	AddNode.m_cJumpEvent=0;
	AddNode.m_cWalkEvent=0;
	AddNode.m_cCancelEvent=0;
	AddNode.m_cShotEvent=0;
	AddNode.m_cJustEvent=0;
	AddNode.m_cJustStartEvent = 0;
	AddNode.m_cJustEndEvent = 0;

	AddNode.m_fAlphaValue = -1.0f;
	
	if(iValue == -1)
		m_CharacterList.push_back(AddNode);	
	else
	{
		*(ppOld) = m_CharacterList[iValue].m_pChrmodel;

		// edith switch 함수 수정
		if((int)m_CharacterList.size() > iValue)
			m_CharacterList[iValue] = AddNode;
	}

	if(bHouse && (iHouseType >= 0))
	{

		if(iHouseType < HOUSETYPENUM)
		{

			pChrModel->SetHouse(vecPos.x,vecPos.y,vecPos.z,(char *)strHouseName[iHouseType]);
			CSceneManager::AddHouseMap(vecPos.x,vecPos.y,vecPos.z,(char *)strHouseName[iHouseType],(char *)strMedHouseName[iHouseType],(char *)strInHouseName[iHouseType],"");
			
			//for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
			for(int cSector=0;cSector<((CSceneManager::m_HeightField.m_LSizeX) * (CSceneManager::m_HeightField.m_LSizeY));cSector++)
			{
				if(	CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41 <= vecPos.x &&
					CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
					CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43 <= vecPos.z &&
					CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
				{
					CSceneManager::m_HeightField.m_SectorScene[cSector].GenerateHouseObjectLastIn();
				}
			}
		}
	}
	return pChrModel;
	/*
	m_CharacterList.Add(pChrModel);		
	m_CharacterCollisionType.Add(CT_BOTTOM);	
	POINT ptChr;
	ptChr.x=0;ptChr.y=0;
	m_CharacterScreenPositionList.Add(ptChr);
	// 사운드 관련부분 추후 수정요
	//CZ3DSoundEventHelper* pChrSoundHelper=new CZ3DSoundEventHelper();
	//pChrSoundHelper->Init(m_CharacterList[m_CharacterList.num-1]);		
	//m_CharacterSoundHelperList.Add(pChrSoundHelper);	
	m_CharacterAttackEventList.Add(false);
	m_CharacterTrailOnEventList.Add(false);
	m_CharacterTrailOffEventList.Add(false);
	m_CharacterDelayTimer.Add(0);	
	m_CharacterPerFrameMove.Add(vector3(0.0f,0.0f,0.0f));
	m_CharacterRealPosition.Add(vector3(0.0f,0.0f,0.0f));
	m_CharacterRandomPostionTimer.Add(0);
	m_CharacterRandomPostionAdder.Add(vector3(0.0f,0.0f,0.0f));	

	CChrLightNode AddLightNode;
	memset(&AddLightNode,0,sizeof(AddLightNode));

	m_CharacterLightList.Add(AddLightNode);

	return m_CharacterList[m_CharacterList.num-1];
	*/
}

CZ3DGeneralChrModel* CCharacterLightShadowManager::ReplaceCharacter( CZ3DGeneralChrModel* pChrOldModel, const char* szGCMDSName, vector3& vecPos )
{
	for(int i = 0; i < (int)m_CharacterList.size(); i++ )
	{
		if( pChrOldModel == m_CharacterList[i].m_pChrmodel )
		{
			CZ3DGeneralChrModel* pChrModel;

			pChrModel = new CZ3DGeneralChrModel;
			if( false == pChrModel->Init( szGCMDSName ) )
			{		
				return NULL;
			}
			pChrModel->SetFixHeight(false);
			pChrModel->SetDisable(false);
			pChrModel->SetPosition(vecPos);
			
			m_CharacterList[i].m_pChrmodel = pChrModel;

			return pChrModel;
		}
	}

	return NULL;
}

void CCharacterLightShadowManager::SetChrBuildEnable(bool bBuild,int nFocusCharacter)
{
	if(m_CharacterList.size() == 0 || nFocusCharacter >= (int)m_CharacterList.size())
		return;
	m_CharacterList[nFocusCharacter].m_bBuild = bBuild;
}
bool CCharacterLightShadowManager::GetChrBuildEnable(int nFocusCharacter)
{
	if(m_CharacterList.size() == 0 || nFocusCharacter >= (int)m_CharacterList.size())
		return false;
	
	vector3 vecCharPos;	
	CollisionType CT;
	vector3 vecOldPos;
	m_CharacterList[nFocusCharacter].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
	vector3 vecBeforePos = m_CharacterList[nFocusCharacter].m_pChrmodel->GetPrevPosition();
		
	vecBeforePos.y += 85.0f;

	vecOldPos.y+=85.0f;			
	vecCharPos=vecOldPos;
	vecOldPos.y-=((m_CharacterList[nFocusCharacter].m_fFallSpeed > 40.0f) ? 40.0f : m_CharacterList[nFocusCharacter].m_fFallSpeed);
	//vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
	vector3 vecDir=vecOldPos-vecCharPos;
    vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,vecBeforePos,CT,true);			
	return 	CSceneManager::m_CollisionDetection.GetBuildEnable();

	//return m_CharacterList[nFocusCharacter].m_bBuild;

}
void CCharacterLightShadowManager::UpdateFrame(int m_nFocuesCharacter)
{	
	if(m_CharacterList.size()==0)
		return;

	ActionProcess();

	static int m_UpdateCollision=0;

	bool bUpdateCollision=false;
	//if(m_UpdateCollision%2==0)
	bUpdateCollision=true;	

	m_UpdateCollision++;

	vector3 vecCharPos,vecDir;
	for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
	{	
		/*
		if(m_CharacterList[cChar]->CheckEvent()==Z3D_ES_HIT)
		{
			m_CharacterAttackEventList[cChar]=true;
		}
		if(m_CharacterList[cChar]->CheckEvent()==Z3D_ES_WEAPON_TRAIL_ON)
		{
			m_CharacterTrailOnEventList[cChar]=true;
			m_CharacterList[cChar]->Act(Z3D_CA_WEAPON_TRAIL_ON);
		}
		if(m_CharacterList[cChar]->CheckEvent()==Z3D_ES_WEAPON_TRAIL_OFF)
		{
			m_CharacterTrailOffEventList[cChar]=true;
			m_CharacterList[cChar]->Act(Z3D_CA_WEAPON_TRAIL_OFF);
		}
		*/
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled())
			continue;

		if(m_CharacterList[cChar].m_CollisionDetectType==CDT_ONLYTERRAIN)
		{
			vector3 vecChrPos;
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos);
			vecChrPos.y=CSceneManager::m_HeightField.GetHeight(vecChrPos);

			//vecChrPos.y+=300.0f;
			m_CharacterList[cChar].m_pChrmodel->SetPosition(vecChrPos);
		}		
		/*
		CollisionType CT;
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);		
		vecCharPos.y+=85.0f;
		vecDir=vecCharPos-OldPosList[cChar];
		CSceneManager::m_CollisionDetection.m_SelfChr=cChar;
		if(cChar==0)
		{
			vecDir.y-=m_CharacterList[cChar].m_fFallSpeed;
			vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(OldPosList[cChar],vecDir,CT,true);				
		}
		else
		{			
			if(bUpdateCollision)
			{
				//vecDir.y-=m_CharacterList[cChar].m_fFallSpeed;
				//vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(OldPosList[cChar],vecDir,CT,true);				
				//vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(OldPosList[cChar],vecDir,CT,false);		
			}			
		}
		
		m_CharacterList[cChar].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
		if((m_CharacterList[cChar].m_CollisionType==CT_NONE ))// && CT!=CT_NONE)
		{
			m_CharacterList[cChar].m_vecPerFrameMove.x=m_CharacterList[cChar].m_vecPerFrameMove.x;
			m_CharacterList[cChar].m_vecPerFrameMove.z=m_CharacterList[cChar].m_vecPerFrameMove.z;
			m_CharacterList[cChar].m_vecPerFrameMove.y=0.0f;
		}
		m_CharacterList[cChar].m_CollisionType=CT;
		//m_CharacterCollisionType[cChar]=CT;
//		m_CharacterSoundHelperList[cChar]->Process();		
*/
	}			

	if(m_nFocuesCharacter==-1)
		return;	

	/*
	float fChrInter=CSceneManager::GetCameraInter();
	vector3 vecChrPos;	
	m_CharacterList[m_nFocuesCharacter]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
	vecChrPos.y+=150.0f;
	vector3 vecCameraPos=CSceneManager::m_ViewCamera->m_vecCameraPosition;
	vecCameraPos=vecChrPos+vecCameraPos*fCh rInter;

	// 집충돌//
	fChrInter=GetCollisionChrCamera(vecChrPos,vecCameraPos);
	vecCameraPos=CSceneManager::m_ViewCamera->m_vecCameraPosition;
	vecCameraPos=vecChrPos+vecCameraPos*fChrInter;
	//vecCameraPos=vecChrPos+vecCameraPos*fChrInter;
	// HeightField 충돌//
	float fCameraHeight=m_pHeightField->GetHeight(vecCameraPos);
	if(vecCameraPos.y < fCameraHeight +30.0f)
	{
		vecCameraPos.y=fCameraHeight +30.0f;
	}
	matrix matView,matTrans;
	matView.CameraLookAt(vecCameraPos,vecChrPos,vector3(0.0f,1.0f,0.0f));
	matTrans.Inverse(matView);
	CSceneManager::m_ViewCamera->m_matPosition=matTrans;
	CSceneManager::m_ViewCamera->m_matView=matView;
	CSceneManager::m_ViewCamera->MoveFrustum();
	CSceneManager::m_ViewCamera->SetVecPosition(vecChrPos);

	vector3 vecLens=vecChrPos-vecCameraPos;
	float fCameraInter=vecLens.GetLens();
	if(100.0f > fCameraInter)
	{
		if(50.0f > fCameraInter)
		{
			m_CharacterList[m_nFocuesCharacter]->SetAlphaLevel(0.1f);			
		}
		else
		{
			fCameraInter-=50.0f;
			fCameraInter/=50.0f;
			if(fCameraInter < 0.1f)
				fCameraInter=0.1f;
			m_CharacterList[m_nFocuesCharacter]->SetAlphaLevel(fCameraInter);
		}		
	}
	else
	{
		m_CharacterList[m_nFocuesCharacter]->SetAlphaLevel(1.0f);
	}
	vector3 vecCamaeraPos=matTrans.GetLoc();
	//CZ3DSound::SetListenerPosition(vecCameraPos);
	//CZ3DSound::SetListenerDirection(CSceneManager::m_ViewCamera->GetRotationY());
	//GetCollisionChrCamera(vecChrPos,vecCameraPos);

	/*
	CSceneManager::m_ViewCamera->m_matExtraPosition._41=cx;
	CSceneManager::m_ViewCamera->m_matExtraPosition._42=cy+150.0f; 
	CSceneManager::m_ViewCamera->m_matExtraPosition._43=cz;
	CSceneManager::m_ViewCamera->InterfaceCharlook(0,0,0.0f);
	*/ 
}



void CCharacterLightShadowManager::CharacterLightSetting(int nChr,LPDIRECT3DDEVICE8 pd3dDevice)
{
	vector3 vecCharPos;
	std::vector<D3DLIGHT8> LightList;

	m_CharacterList[nChr].m_pChrmodel->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);
	m_pHeightField->GetLight(vecCharPos,LightList);	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_AmbientColor->c);
	color ChrColor=CSceneManager::m_WeatherManager.m_InterObjectAmbientColor;
	ChrColor.r+=140;
	ChrColor.g+=140;
	ChrColor.b+=140;
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);	
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xffffffff);	
	
	if(LightList.empty()==0)
	{
		pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);	
		pd3dDevice->LightEnable(0,TRUE);		
	}
/*
	for(int cLight=1;cLight<LightList.num;cLight++)
	{
		if(cLight==0)
		{
			/*
			if(LightList[cLight].Type==0)
			{
				color SunLightColor;
				SunLightColor.a=0xff;
				SunLightColor.r=(CSceneManager::m_WeatherManager.m_SunLight.Diffuse.r*255);
				SunLightColor.g=(CSceneManager::m_WeatherManager.m_SunLight.Diffuse.g*255);
				SunLightColor.b=(CSceneManager::m_WeatherManager.m_SunLight.Diffuse.b*255);
				color InterSunLightColor=color::Interpolation(color(0,0,0),SunLightColor,LightList[cLight].Attenuation0*LightList[cLight].Attenuation0*LightList[cLight].Attenuation0);
				D3DLIGHT8 SunLight=CSceneManager::m_WeatherManager.m_SunLight;
				SunLight.Diffuse.r=InterSunLightColor.r/255.0f;
				SunLight.Diffuse.g=InterSunLightColor.g/255.0f;
				SunLight.Diffuse.b=InterSunLightColor.b/255.0f;
				pd3dDevice->SetLight(0,&SunLight);
				pd3dDevice->LightEnable(0,TRUE);
			}
			
		}
		else
		{
			LightList[cLight].Attenuation1=0.0f;
			pd3dDevice->SetLight(cLight,&LightList[cLight]);
			pd3dDevice->LightEnable(cLight,TRUE);
		}
	}
*/
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	//m_CharacterList[cChar]->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.x);
	//m_pHeightField->m_
	/*
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);	
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	*/
}

void CCharacterLightShadowManager::InHouseShadowRender(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	//if(MAXCHRSHADOW==0)
		return;
		/*
	pd3dDevice->EndScene();
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	matrix matTexScale;
	matTexScale.MakeIdent();
	matTexScale._11=0.5f;
	matTexScale._22=-0.5f;	
	matTexScale._33=0.0f;
	matTexScale._41=0.5f;
	matTexScale._42=0.5f;
	matTexScale._43=1.0f;
	matTexScale._44=1.0f;

	matrix matView,matOldProj,matInv,matTex;
	pd3dDevice->GetTransform(D3DTS_VIEW,matView);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProj);
	matInv.Inverse(matView);   	

	matrix matAdjLightProj,matLightProj;
	matAdjLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f,1.0f,1.0f,500.0f,0.5f,-0.5f,(float)SHADOWSIZE,(float)SHADOWSIZE);
	matLightProj.MakeAdjustedProjectionMatrix(3.14159f/8.0f, 1.0f, 1.0f, 500.0f,0.0f, 0.0f,(float)SHADOWSIZE,(float)SHADOWSIZE);    
	
	matrix matShadowProj;
	//matShadowProj.MakeProjection(3.14159f/4.0f,1.0f,10.0f,10000.0f);
	matShadowProj.MakeNoProjection(3.14159f/3.0f,(float)SHADOWSIZE,(float)SHADOWSIZE,10.0f,500.f);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->LightEnable(1,FALSE);
	pd3dDevice->LightEnable(2,FALSE);
	pd3dDevice->LightEnable(3,FALSE);
	pd3dDevice->LightEnable(4,FALSE);
	pd3dDevice->LightEnable(5,FALSE);
	pd3dDevice->LightEnable(6,FALSE);
	pd3dDevice->LightEnable(7,FALSE);
	pd3dDevice->LightEnable(8,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xffcccccc);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	
	for(int cChar=0;cChar<m_CharacterList.num;cChar++)
	{
		//List<vector3> *CharPoly=new List<vector3>[MAXCHRSHADOW];
		//List<vector3> vecLightPosList;
		vector3 vecChrPos;
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		m_pHeightField->GetInHouseShadowPoly(vecChrPos,CharPoly,vecLightPosList);
		for(int cShadow=0;cShadow<MAXCHRSHADOW;cShadow++)
		{
			if(CharPoly[cShadow].num<=0)
				break;

			m_pShadowTexture.Begin(pd3dDevice);
			pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
			pd3dDevice->BeginScene();
			matrix matCharCamera;
			matCharCamera.CameraLookAt(
				vecLightPosList[cShadow],
				vector3(0.0f,80.0f,0.0f)+vecChrPos,
				vector3(0.0f,1.0f,0.0f));				
			pd3dDevice->SetTransform(D3DTS_VIEW,matCharCamera);	
			pd3dDevice->SetTransform(D3DTS_PROJECTION,matShadowProj);
			//m_pd3dDevice->SetTransform(D3DTS_PROJECTION,matAdjLightProj);		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		
			m_CharacterList[cChar].m_pChrmodel->DrawShadow(pd3dDevice);
			pd3dDevice->EndScene();
			m_pShadowTexture.End(pd3dDevice);	

			pd3dDevice->BeginScene();

			TTVertex *pCharVertex=new TTVertex[CharPoly[cShadow].num];
			WORD *pCharIndices=new WORD[CharPoly[cShadow].num];
			for(int cIndices=0;cIndices<CharPoly[cShadow].num;cIndices++)
			{
				pCharVertex[cIndices].v=CharPoly[cShadow][cIndices];
				pCharVertex[cIndices].diff.c=0xffff0000;
				pCharIndices[cIndices]=cIndices;
				pCharVertex[cIndices].v.y+=3.0f;
			}
			pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		
			matrix mat,mat2;
			mat=matShadowProj*matTexScale;			
			mat2=matCharCamera*mat;
			matTex=matInv*mat2;
			pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);
			pd3dDevice->SetTexture(0,m_pShadowTexture.GetTexture());
			pd3dDevice->SetVertexShader(TTVERTEXFVF);	
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			
			pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,CharPoly[cShadow].num,CharPoly[cShadow].num/3,pCharIndices,
				D3DFMT_INDEX16,pCharVertex,sizeof(TTVertex));
			pd3dDevice->EndScene();
			delete [] pCharVertex;
			delete [] pCharIndices;
		}
		delete [] CharPoly;
	}
	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
	pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->BeginScene();
	//List< List<PolyNode> > 
	*/
}

float CCharacterLightShadowManager::GetCollisionChrCamera(vector3 vecStart,vector3 vecEnd)
{
	std::vector<vector3> PolyList;
	m_pHeightField->GetLineIntersectPoly(vecStart,vecEnd,PolyList);
	float fCollisionLens=CSceneManager::GetCameraInter();
	float fLens;
	vector3 vecPoly[3];
	if(PolyList.size()>0)
	{
		for(int cIndices=0;cIndices<(int)PolyList.size()/3;cIndices++)
		{
			vecPoly[0]=PolyList[cIndices*3+0];
			vecPoly[1]=PolyList[cIndices*3+1];
			vecPoly[2]=PolyList[cIndices*3+2];
			
			if(CIntersection::PolygonRay(vecStart,vecEnd,vecPoly,fLens) && fCollisionLens > fLens && fLens > 0.0f)
			{
				fCollisionLens=fLens;
			}
		}
	}
	return fCollisionLens;
}

void CCharacterLightShadowManager::CameraTest(LPDIRECT3DDEVICE8 pd3dDevice)
{
	
	vector3 vecStart,vecEnd;

	float fChrInter=-350.0f;
	vector3 vecChrPos;	
	m_CharacterList[0].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
	vecChrPos.y+=150.0f;
	vector3 vecCameraPos=CSceneManager::m_ViewCamera->m_vecCameraPosition;
	vecCameraPos=vecChrPos+vecCameraPos*fChrInter;	
	//matrix matView,matTrans;
	//matView.CameraLookAt(vecCameraPos,vecChrPos,vector3(0.0f,1.0f,0.0f));
	//matTrans.Inverse(matView);
	vecStart=vecChrPos;
	vecEnd=vecCameraPos;
	matrix matWorld;
	matWorld.MakeIdent();

	std::vector<vector3> PolyList;
	m_pHeightField->GetLineIntersectPoly(vecStart,vecEnd,PolyList);
	if(PolyList.size()>0)
	{		
		pd3dDevice->SetTexture(0,NULL);
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);	
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	
		TTVertex *pCharVertex=new TTVertex[PolyList.size()];
		WORD *pCharIndices=new WORD[PolyList.size()];		
		for(int cIndices=0;cIndices<(int)PolyList.size();cIndices++)
		{
			pCharVertex[cIndices].v=PolyList[cIndices];
			pCharVertex[cIndices].diff.c=0xffff0000;
			pCharIndices[cIndices]=cIndices;
			pCharVertex[cIndices].v.y+=2.0f;
		}
		pd3dDevice->SetVertexShader(TTVERTEXFVF);	
		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)PolyList.size(),(int)PolyList.size()/3,pCharIndices,
				D3DFMT_INDEX16,pCharVertex,sizeof(TTVertex));
		if(pCharVertex) {
			delete [] pCharVertex;
			pCharVertex = NULL;
		}
		if(pCharIndices) {
			delete [] pCharIndices;
			pCharIndices = NULL;
		}

	
	}	
	
}


void CCharacterLightShadowManager::FlushAccumulatedCharacterEvents()
{
	if( 0 == m_CharacterList.size() )
	{
		return;
	}

	for( int i = 0; i < (int)m_CharacterList.size(); ++i )
	{
		m_CharacterList[i].m_pChrmodel->FlushAccumulatedEvents();
	}
}


void CCharacterLightShadowManager::ActionProcess()
{
	int cChr;

	if(m_CharacterList.size()==0)
		return;

	CZ3DManagedObject::Process();
	bool bUpdate=true;


	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// Process() 함수 호출위치 변경
	// - Process() 함수 내부에 이벤트 타이밍 조사가 이뤄지므로, CheckEvent()
	//   호출 전에 Process()의 호출이 있어야함
	for(cChr=0;cChr<(int)m_CharacterList.size();cChr++)		
	{
		//수영관련 띄우기 wizardbug
		if(m_CharacterList[cChr].m_pChrmodel->GetFixHeight())
		{
			vector3 vecSource = m_CharacterList[cChr].m_pChrmodel->GetPosition();
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y + m_CharacterList[cChr].m_pChrmodel->m_fFixHeight,vecSource.z);
			m_CharacterList[cChr].m_pChrmodel->m_fFixHeight += 3.0f;
			if(m_CharacterList[cChr].m_pChrmodel->m_fFixHeight >= 160.0f)
				m_CharacterList[cChr].m_pChrmodel->m_fFixHeight = 160.0f;
		}
		else if(m_CharacterList[cChr].m_pChrmodel->m_fFixHeight > 0.1f)
		{
			m_CharacterList[cChr].m_pChrmodel->m_fFixHeight -= 6.0f;
			vector3 vecSource = m_CharacterList[cChr].m_pChrmodel->GetPosition();
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y + m_CharacterList[cChr].m_pChrmodel->m_fFixHeight,vecSource.z);
			if(m_CharacterList[cChr].m_pChrmodel->m_fFixHeight <= 0.0f)
				m_CharacterList[cChr].m_pChrmodel->m_fFixHeight = 0.0f;


		}


	}
	
	if(bUpdate)
		CZ3DCharacterModel::Process();

	CZ3DObject::Process();



	// 수영관련 wizardbug
	for(cChr=0;cChr<(int)m_CharacterList.size();cChr++)
	{
		if(m_CharacterList[cChr].m_pChrmodel && m_CharacterList[cChr].m_pChrmodel->GetFixHeight())
		{
			
			vector3 vecSource = m_CharacterList[cChr].m_pChrmodel->GetPrevPosition();
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y,vecSource.z);
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y,vecSource.z);
			
		}
		else if(m_CharacterList[cChr].m_pChrmodel && m_CharacterList[cChr].m_pChrmodel->m_fFixHeight > 0.1f)
		{
			
			vector3 vecSource = m_CharacterList[cChr].m_pChrmodel->GetPrevPosition();
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y,vecSource.z);
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecSource.x,vecSource.y,vecSource.z);

		}
	}

	vector3 vecChrPos;
	for(cChr=0;cChr<(int)m_CharacterList.size();cChr++)
	{			

		if( CSceneManager::m_CollisionDetection.IsDetectionLocked() )	// collision detection lock 시 충돌검사 x
		{
			m_CharacterList[cChr].m_CollisionType = CT_BOTTOM;
			continue;
		}


		if(m_CharacterList[cChr].m_pChrmodel->IsActionFinished(1))
		{
			m_CharacterList[cChr].m_bFirstAccelate=false;
		}				

		if(m_CharacterList[cChr].m_CollisionType==CT_NONE || m_CharacterList[cChr].m_CollisionType==CT_SIDE)
		{		
			if(!CSceneManager::m_CollisionDetection.IsDetectionLocked() )
			{
				m_CharacterList[cChr].m_fFallSpeed=m_CharacterList[cChr].m_fFallSpeed+98.0f/130.0f;
				if(m_CharacterList[cChr].m_fFallSpeed>80.0f)
				{
					m_CharacterList[cChr].m_fFallSpeed=80.0f;
				}
			}
		}
		else
		{
			if(m_CharacterList[cChr].m_fFallSpeed>0.0f)
				m_CharacterList[cChr].m_fFallSpeed=0.0f;
			
			if(m_CharacterList[cChr].m_fFallSpeed<0.0f)
			{
				m_CharacterList[cChr].m_fFallSpeed=m_CharacterList[cChr].m_fFallSpeed+98.0f/130.0f;
			}
			
		}
		if(m_CharacterList[cChr].m_bGravityAble==false)
		{
			m_CharacterList[cChr].m_fFallSpeed=0.0f;
		}

		if(m_CharacterList[cChr].m_fAccelate < 0.0f)
		{
			vector3 vecAdder=m_CharacterList[cChr].m_vecZeroVelocity;
			vecAdder.Normalize();
			m_CharacterList[cChr].m_vecPerFrameMove+=(m_CharacterList[cChr].m_fAccelate*vecAdder);
		}		

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("HIT"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_HIT;			
			m_CharacterList[cChr].m_cAttackEvent++;			
		}
		
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("HIT_LEFT"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_HIT_LEFT;			
			m_CharacterList[cChr].m_cAttackEvent++;			
		}

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("SHOT"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_SHOT;			
			m_CharacterList[cChr].m_cShotEvent++;			
		}

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("JUST"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_JUST;			
			m_CharacterList[cChr].m_cJustEvent++;			
		}

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("JUSTSTART"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_JUSTSTART;			
			m_CharacterList[cChr].m_cJustStartEvent++;			
		}

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("JUSTEND"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_JUSTEND;			
			m_CharacterList[cChr].m_cJustEndEvent++;			
		}

		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent( "WP_TRL_ON"))
		{
			CZ3DWeapon *pWp=m_CharacterList[cChr].m_pChrmodel->GetAttachmentObject("WEAPON");
			if(pWp)
			{
				pWp->TurnOnTrail();
			}
		}
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent( "WP_TRL_OFF"))
		{
			CZ3DWeapon *pWp=m_CharacterList[cChr].m_pChrmodel->GetAttachmentObject("WEAPON");
			if(pWp)
			{
				pWp->TurnOffTrail();
			}
		}

		Z3DTOK TokSecond;
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("SND",&TokSecond))
		{			
 			const char* strSoundFileName = g_MapEvent2SndFile.GetFileName(TokSecond);						
			if(strcmp(strSoundFileName,"")!=0)
			{
				if(strstr(strSoundFileName,"bow3.wav") != NULL) { // 활소리 렌덤
					
//					try
//					{
						char tmp_SoundFileName[256]= {0};
						int tmp_selectnum = 0;

						if((rand() % 10)<= 6) {
							if(rand() % 2)
								tmp_selectnum = 1;
							else
								tmp_selectnum = 2;
						}
						else {
							if(rand() % 2)
								tmp_selectnum = 3;
							else
								tmp_selectnum = 4;
						}

						/* 기존사운드코드 
						sprintf(tmp_SoundFileName, "%s%s%d%s",SOUNDFILEPATH,"NBOW",tmp_selectnum,".wav");
						CSoundManager &SoundManager=CSoundManager::GetInstance();
						
						ISoundObject &sb=SoundManager.GetBuffer(tmp_SoundFileName,true);
						

						vector3 vecChrPos;

						m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);


						int nBuffer=sb.Play(false);
						sb.SetPosition(nBuffer,vecChrPos.x,vecChrPos.y,vecChrPos.z);
						sb.SetDistance(nBuffer,400.0f,2000.0f);
						sb.IsPlaying(nBuffer);
						*/

						//새로들어간코드 
						
						sprintf(tmp_SoundFileName, "%s%s%d%s",SOUNDFILEPATH,"NBOW",tmp_selectnum,".wav");
						CSound3D* pSound3D; 
						if( CSoundMgr::_GetInstance()->GetSound3D( pSound3D, tmp_SoundFileName ) )
						{
							pSound3D->Play(); 			
							vector3 vecChrPos;
							m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);
							D3DVECTOR Pos; 
							Pos.x = vecChrPos.x; Pos.y = vecChrPos.y; Pos.z = vecChrPos.z; 
							pSound3D->SetPosition( Pos ); 
							pSound3D->SetMinDistance( 400.0f ); 
							pSound3D->SetMaxDistance( 2000.0f );
						}

/*					}
					catch(exception &e)
					{
						static bool bFirstShowMessage=false;
						if(bFirstShowMessage==false)
						{
							MessageBox(NULL,e.what(),0,0);
							bFirstShowMessage=true;
						}
					}
*/

				}
				else {
					if(strstr(strSoundFileName,"yangfoot"))
					{
						m_CharacterList[cChr].m_dwEvent|=EVENT_WALK;
						m_CharacterList[cChr].m_cWalkEvent++;			
					}
//					try
//					{
						/* 기존사운드코드 
						CSoundManager &SoundManager=CSoundManager::GetInstance();
						
						ISoundObject &sb=SoundManager.GetBuffer(strSoundFileName,true);
						

						vector3 vecChrPos;

						m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);


						int nBuffer=sb.Play(false);
						sb.SetPosition(nBuffer,vecChrPos.x,vecChrPos.y,vecChrPos.z);
						sb.SetDistance(nBuffer,400.0f,2000.0f);
						sb.IsPlaying(nBuffer);
						*/

						//새로들어간코드 
						
						CSound3D* pSound3D; 
						if( CSoundMgr::_GetInstance()->GetSound3D( pSound3D, strSoundFileName ) )
						{
							char szTmp[300];
							strcpy(szTmp, m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName());
							strupr(szTmp);
							if (0 == strstr("SYMBOL", szTmp))
							{
								__asm nop
							}

							pSound3D->Play(); 
							vector3 vecChrPos; 
							m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);
							D3DVECTOR Pos; 
							Pos.x = vecChrPos.x; Pos.y = vecChrPos.y; Pos.z = vecChrPos.z; 
							pSound3D->SetPosition( Pos ); 
							pSound3D->SetMinDistance( 400.0f ); 
							pSound3D->SetMaxDistance( 2000.0f );
						}
/*					}
					catch(exception &e)
					{
						static bool bFirstShowMessage=false;
						if(bFirstShowMessage==false)
						{
							MessageBox(NULL,e.what(),0,0);
							bFirstShowMessage=true;
						}
					}
*/
				}
			}			
		}
		
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("JUMP"))
		{
			
			m_CharacterList[cChr].m_dwEvent|=EVENT_JUMP;
			m_CharacterList[cChr].m_fFallSpeed=-12.0f;
			m_CharacterList[cChr].m_cJumpEvent++;			
		}
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("CANCEL"))
		{
			m_CharacterList[cChr].m_dwEvent|=EVENT_CANCEL;			
			m_CharacterList[cChr].m_cCancelEvent++;			
		}
		
		if(m_CharacterList[cChr].m_pChrmodel->CheckEvent("TAP_JUMP"))
		{
			m_CharacterList[cChr].m_fFallSpeed=-6.0f;
		}		
		if(m_CharacterList[cChr].m_RandomPositionTimer>0)
		{			
			vecChrPos=m_CharacterList[cChr].m_vecRealPosition;		
			vecChrPos+=m_CharacterList[cChr].m_vecPerFrameMove;			
			m_CharacterList[cChr].m_vecRealPosition=vecChrPos;

			vector3 vecAddRand=vector3(rand()%1000-500,rand()%1000-500,rand()%1000-500);
			vecAddRand.Normalize();						
			vecAddRand*=((m_CharacterList[cChr].m_RandomPositionTimer*m_CharacterList[cChr].m_RandomPositionTimer)/(10.0f*10.0f))*3.0f;;

			vecChrPos+=vecAddRand;			
			vecChrPos+=m_CharacterList[cChr].m_vecRandomPositionAdder;			
			m_CharacterList[cChr].m_vecRandomPositionAdder.Normalize();			
			m_CharacterList[cChr].m_vecRandomPositionAdder*=m_CharacterList[cChr].m_RandomPositionTimer;

			char szTmp[300];
			strcpy(szTmp, m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName());
			strupr(szTmp);
			if (0 == strstr("SYMBOL", szTmp))
			{
				__asm nop
			}

			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecChrPos);			
			if(--m_CharacterList[cChr].m_RandomPositionTimer==0)
			{					
				m_CharacterList[cChr].m_pChrmodel->SetPosition(m_CharacterList[cChr].m_vecRealPosition);
			}
		}
		else
		{			
			m_CharacterList[cChr].m_RandomPositionTimer=0;			
			m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);			
			vecChrPos+=(m_CharacterList[cChr].m_vecPerFrameMove);			
			m_CharacterList[cChr].m_vecRealPosition=vecChrPos;		

			char szTmp[300];
			strcpy(szTmp, m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName());
			strupr(szTmp);
			if (0 == strstr("SYMBOL", szTmp))
			{
				__asm nop
			}

			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecChrPos);
		}

		if(m_CharacterList[cChr].m_DelayTimer>0)
		{			
			m_CharacterList[cChr].m_DelayTimer--;			
		}
		else
		{
			//m_CharacterList[cChr].m_pChrmodel->Pause(false);
			m_CharacterList[cChr].m_DelayTimer=0;
		}

		//m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);		
		//vecChrPos.y-=m_CharacterList[cChr].m_fFallSpeed;
		//m_CharacterList[cChr].m_pChrmodel->SetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);	



		if (m_CharacterList[cChr].m_bHide) continue;	// Hide 캐릭터 충돌 무시

		switch(m_CharacterList[cChr].m_CollisionDetectType)
		{
			case CDT_NONE:
				{

				}
				break;
			case CDT_SKYUNIT:
				{

					vector3 vecCharPos;	
					CollisionType CT;
					vector3 vecOldPos;
					m_CharacterList[cChr].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
					vector3 vecBeforePos = m_CharacterList[cChr].m_pChrmodel->GetPrevPosition();
						
					vecBeforePos.y += 85.0f;

					vecOldPos.y+=85.0f;			
					vecCharPos=vecOldPos;
					//vecOldPos.y-=((m_CharacterList[cChr].m_fFallSpeed > 40.0f) ? 40.0f : m_CharacterList[cChr].m_fFallSpeed);
					//vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
					vector3 vecDir=vecOldPos-vecCharPos;
                    vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,vecBeforePos,CT,true);			
//					char szTmp[300];

					if(strcmp("Siege_WallBreaker.GCMDS", m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName())==0)
					{
						__asm nop

						if(vecCharPos.y<10000)
						{
							__asm nop
						}
					}

					m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
					m_CharacterList[cChr].m_CollisionType=CT;
				}
				break;

			case CDT_FULL:
				{
					vector3 vecCharPos;	
					CollisionType CT;
					vector3 vecOldPos;
					m_CharacterList[cChr].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
					vector3 vecBeforePos = m_CharacterList[cChr].m_pChrmodel->GetPrevPosition();
						
					vecBeforePos.y += 85.0f;

					vecOldPos.y+=85.0f;			
					vecCharPos=vecOldPos;
					if( !CSceneManager::m_CollisionDetection.IsDetectionLocked() )
					{
						vecOldPos.y-=((m_CharacterList[cChr].m_fFallSpeed > 40.0f) ? 40.0f : m_CharacterList[cChr].m_fFallSpeed);
					}
					//vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
					vector3 vecDir=vecOldPos-vecCharPos;
                    vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,vecBeforePos,CT,true);			

					if(strcmp("Siege_WallBreaker.GCMDS", m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName())==0)
					{
						__asm nop

						if(vecCharPos.y<10000)
						{
							__asm nop
						}
					}

					m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
//					m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);		
					m_CharacterList[cChr].m_CollisionType=CT;
				}
				break;
			case CDT_ONLYTERRAIN:
				{
					/*
					vector3 vecCharPos;	
					CollisionType CT;
					vector3 vecOldPos;
					m_CharacterList[cChr].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
					vecOldPos.y+=85.0f;			
					vecCharPos=vecOldPos;
					vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
					vector3 vecDir=vecOldPos-vecCharPos;
					vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,CT,false);			
					m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
					m_CharacterList[cChr].m_CollisionType=CT;	
					*/
				}
				break;			

			case CDT_FIRSTBOTTOM:
				{
					if( m_CharacterList[cChr].m_CollisionType==CT_SIDE ||
						m_CharacterList[cChr].m_CollisionType==CT_CHRSIDE ||
						m_CharacterList[cChr].m_CollisionType==CT_NONE ||
						m_CharacterList[cChr].m_CollisionType==CT_WATER )
					{
						vector3 vecCharPos;	
						CollisionType CT;
						vector3 vecOldPos;
						m_CharacterList[cChr].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
						vector3 vecBeforePos = m_CharacterList[cChr].m_pChrmodel->GetPrevPosition();
						
						vecBeforePos.y += 85.0f;

						vecOldPos.y+=85.0f;			
						vecCharPos=vecOldPos;
						if( !CSceneManager::m_CollisionDetection.IsDetectionLocked() )
						{
							vecOldPos.y-=((m_CharacterList[cChr].m_fFallSpeed > 40.0f) ? 40.0f : m_CharacterList[cChr].m_fFallSpeed);
						}
						//vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
						vector3 vecDir=vecOldPos-vecCharPos;
                        CSceneManager::m_CollisionDetection.SetFirstBottom(true);
						vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,vecBeforePos,CT,false);			
						CSceneManager::m_CollisionDetection.SetFirstBottom(false);

						char szTmp[300];
						strcpy(szTmp, m_CharacterList[cChr].m_pChrmodel->GetGCMDSFileName());
						strupr(szTmp);
						if (0 == strstr("SYMBOL", szTmp))
						{
							__asm nop
						}

                        m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
						m_CharacterList[cChr].m_CollisionType=CT;	
						
					}
					
					m_CharacterList[cChr].m_pChrmodel->SetNPC(true);


				}
				break;

		}
		/*
		if(cChr==0)
		{
			
		}
		else
		{
			vector3 vecCharPos;	
			CollisionType CT;
			vector3 vecOldPos;
			m_CharacterList[cChr].m_pChrmodel->GetPosition(vecOldPos.x,vecOldPos.y,vecOldPos.z);						
			vecOldPos.y+=85.0f;			
			vecCharPos=vecOldPos;
			vecOldPos.y-=m_CharacterList[cChr].m_fFallSpeed;
			vector3 vecDir=vecOldPos-vecCharPos;
			vecCharPos=CSceneManager::m_CollisionDetection.GetPosition(vecOldPos,vecDir,CT,true);			
			m_CharacterList[cChr].m_pChrmodel->SetPosition(vecCharPos.x,vecCharPos.y-85.0f,vecCharPos.z);		
			m_CharacterList[cChr].m_CollisionType=CT;	
		}
		*/
	
	}
	
	

	UpdateLight();	
}
void CCharacterLightShadowManager::DeleteElement(CZ3DGeneralChrModel* pDelChr)
{
	if(pDelChr)
	{
		std::vector<CCharacterLightShadowManager::CCharacterDataNode>::iterator obj = m_CharacterList.begin();

		for(int i=0;i<(int)m_CharacterList.size();i++,obj++)
		{
			if(m_CharacterList[i].m_pChrmodel==pDelChr)
			{	
				m_CharacterList.erase(obj);

				if(pDelChr) 
				{
					// wizardbug House 처리
					D3DXVECTOR3 vecPos;
					char strName[256];

					if(pDelChr->GetHouse(vecPos,strName))
					{
						// edith 2008.11.24 housemap 삭제 취소.
						CSceneManager::DeleteHouseMap(vecPos.x,vecPos.y,vecPos.z,strName);	
						
						//for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
						for(int cSector=0;cSector<((CSceneManager::m_HeightField.m_LSizeX) * (CSceneManager::m_HeightField.m_LSizeY));cSector++)
						{
							if(	CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41 <= vecPos.x &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43 <= vecPos.z &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
							{
								CSceneManager::m_HeightField.m_SectorScene[cSector].DeleteHouse(vecPos.x,vecPos.y,vecPos.z);

							}
						}

					}
				}
				return ;

			}
		}

	}
}

void CCharacterLightShadowManager::DeleteHouse( CZ3DGeneralChrModel* pDelChr )
{
	if(pDelChr)
	{
		for(int i=0;i<(int)m_CharacterList.size();i++)
		{
			if(m_CharacterList[i].m_pChrmodel==pDelChr)
			{			
				if(pDelChr) 
				{
					// wizardbug House 처리
					D3DXVECTOR3 vecPos;
					char strName[256];

					if(pDelChr->GetHouse(vecPos,strName))
					{
						// edith 2008.11.24 housemap 삭제 취소.
						CSceneManager::DeleteHouseMap(vecPos.x,vecPos.y,vecPos.z,strName);	

						//for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
						for(int cSector=0;cSector<((CSceneManager::m_HeightField.m_LSizeX) * (CSceneManager::m_HeightField.m_LSizeY));cSector++)
						{
							if(	CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41 <= vecPos.x &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43 <= vecPos.z &&
								CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
							{
								CSceneManager::m_HeightField.m_SectorScene[cSector].DeleteHouse(vecPos.x,vecPos.y,vecPos.z);

							}
						}

					}
				}
				return ;

			}
		}

	}
}



void CCharacterLightShadowManager::SwapElement( CCharacterLightShadowManager::CCharacterDataNode Node1, 
												CCharacterLightShadowManager::CCharacterDataNode Node2 )
{
	//m_CharacterList.Swap( Node1, Node2 ) ;
	int n1 = -1, n2 = -1;
	for ( int i = 0 ; i < (int)m_CharacterList.size(); i ++ )
	{
		if ( Node1.m_pChrmodel == m_CharacterList[ i ].m_pChrmodel )
			n1 = i ;

		if ( Node2.m_pChrmodel == m_CharacterList[ i ].m_pChrmodel )
			n2 = i ;
	}
    
	if(n1 != -1 && n2 != -1)
	{
		m_CharacterList[n2] = Node1;
		m_CharacterList[n1] = Node2;
	}
}

void CCharacterLightShadowManager::DeleteCharacter(CZ3DGeneralChrModel *pDelChr)
{
	std::vector<CCharacterLightShadowManager::CCharacterDataNode>::iterator obj = m_CharacterList.begin();

	for(int i=0;i<(int)m_CharacterList.size();i++,obj++)
	{
		if(m_CharacterList[i].m_pChrmodel==pDelChr)
		{			
			m_CharacterList.erase(obj);

			if(pDelChr) 
			{
				// wizardbug House 처리
				D3DXVECTOR3 vecPos;
				char strName[256];

				if(pDelChr->GetHouse(vecPos,strName))
				{
					// edith 2008.11.24 housemap 삭제 취소.
					CSceneManager::DeleteHouseMap(vecPos.x,vecPos.y,vecPos.z,strName);	

					//for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
					for(int cSector=0;cSector<((CSceneManager::m_HeightField.m_LSizeX) * (CSceneManager::m_HeightField.m_LSizeY));cSector++)
					{
						if(	CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41 <= vecPos.x &&
							CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
							CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43 <= vecPos.z &&
							CSceneManager::m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
						{
							CSceneManager::m_HeightField.m_SectorScene[cSector].DeleteHouse(vecPos.x,vecPos.y,vecPos.z);

						}
					}

				}
				

				//
				delete pDelChr;
				pDelChr = NULL;
			}

			/*
			m_CharacterList.Remove(pDelChr);			
			m_CharacterCollisionType.DelIndex(i);
			m_CharacterScreenPositionList.DelIndex(i);
			//delete m_CharacterSoundHelperList[i];
			//m_CharacterSoundHelperList.DelIndex(i);
			m_CharacterAttackEventList.DelIndex(i);
			m_CharacterTrailOnEventList.DelIndex(i);
			m_CharacterTrailOffEventList.DelIndex(i);
			m_CharacterDelayTimer.DelIndex(i);
			m_CharacterPerFrameMove.DelIndex(i);
			m_CharacterRealPosition.DelIndex(i);
			m_CharacterRandomPostionTimer.DelIndex(i);		
			m_CharacterRandomPostionAdder.DelIndex(i);
			m_CharacterLightList.DelIndex(i);
			delete pDelChr;
			*/
			return;
		}
	}	
}

CollisionType CCharacterLightShadowManager::CharacterCollisionType(CZ3DGeneralChrModel *pFindCharacter)
{	
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pFindCharacter)
		if(m_CharacterList[i].m_pChrmodel==pFindCharacter)
		{
			return m_CharacterList[i].m_CollisionType;
			//return m_CharacterCollisionType[i];
		}
	}
	return CT_BOTTOM;
}

void CCharacterLightShadowManager::HeightFiledOnChrFixPos()
{
	vector3 vecChrPos;
	float fHeight;
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterCollisionType[i]==CT_TERRAIN)
		if(m_CharacterList[i].m_CollisionType==CT_TERRAIN)
		{
			m_CharacterList[i].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
			fHeight=m_pHeightField->GetHeight(vecChrPos);
			m_CharacterList[i].m_pChrmodel->SetPosition(vecChrPos.x,fHeight,vecChrPos.z);
		}
	}
}

void CCharacterLightShadowManager::CharacterSoundPlay(CZ3DGeneralChrModel *pChrmodel, char *strSoundFile)
{
	for(int cChr=0;cChr<(int)m_CharacterList.size();cChr++)
	{
		if(m_CharacterList[cChr].m_pChrmodel==pChrmodel)
		{
//			m_CharacterSoundHelperList[cChr]->Play(strSoundFile);
			return;
		}
	}
}

int CCharacterLightShadowManager::GetCharacterEvent(CZ3DGeneralChrModel *pChrmodel,DWORD Event)
{
	for(int cChr=0;cChr<(int)m_CharacterList.size();cChr++)
	{
		if(m_CharacterList[cChr].m_pChrmodel==pChrmodel)
		{		
			if(Event & m_CharacterList[cChr].m_dwEvent)
			{
				int nResult = 0;
				switch(Event)
				{
					case EVENT_HIT:
						nResult = m_CharacterList[cChr].m_cAttackEvent;
						m_CharacterList[cChr].m_cAttackEvent = 0;
						break;
					case EVENT_HIT_LEFT:
						nResult = m_CharacterList[cChr].m_cAttackEvent;
						m_CharacterList[cChr].m_cAttackEvent = 0;
						break;

					case EVENT_JUMP:
						nResult = m_CharacterList[cChr].m_cJumpEvent;
						m_CharacterList[cChr].m_cJumpEvent = 0;
						break;

					case EVENT_WALK:
						nResult = m_CharacterList[cChr].m_cWalkEvent;
						m_CharacterList[cChr].m_cWalkEvent = 0;
						break;

					case EVENT_CANCEL:
						nResult = m_CharacterList[cChr].m_cCancelEvent;
						m_CharacterList[cChr].m_cCancelEvent = 0;
						break;

					case EVENT_SHOT:
						nResult = m_CharacterList[cChr].m_cShotEvent;
						m_CharacterList[cChr].m_cShotEvent = 0;
						break;

					case EVENT_JUST:
						nResult = m_CharacterList[cChr].m_cJustEvent;
						m_CharacterList[cChr].m_cJustEvent = 0;
						break;

					case EVENT_JUSTSTART:
						nResult = m_CharacterList[cChr].m_cJustStartEvent;
						m_CharacterList[cChr].m_cJustStartEvent = 0;
						break;

					case EVENT_JUSTEND:
						nResult = m_CharacterList[cChr].m_cJustEndEvent;
						m_CharacterList[cChr].m_cJustEndEvent = 0;
						break;
				}

				m_CharacterList[cChr].m_dwEvent = m_CharacterList[cChr].m_dwEvent & (0xFFFFFFFF - Event);				
				return nResult;				
			}				
			return false;
			
			/*
			if(z3dEvent==Z3D_ES_HIT)
			{
				if(m_CharacterAttackEventList[cChr])
				{
					m_CharacterAttackEventList[cChr]=false;
					return true;
				}
				else
					return false;
			}			
			if(z3dEvent==Z3D_ES_WEAPON_TRAIL_ON)
			{
				if(m_CharacterTrailOnEventList[cChr])
				{
					m_CharacterTrailOnEventList[cChr]=false;
					return true;
				}
				else
					return false;
			}
			if(z3dEvent==Z3D_ES_WEAPON_TRAIL_OFF)
			{
				if(m_CharacterTrailOffEventList[cChr])
				{
					m_CharacterTrailOffEventList[cChr]=false;
					return true;
				}
				else
					return false;
			}
			*/
		}		
	}
	return false;
}

void CCharacterLightShadowManager::AddDelayTimer(CZ3DGeneralChrModel *pChr, int nDelay)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChr)
		if(m_CharacterList[i].m_pChrmodel==pChr)
		{			
			//m_CharacterDelayTimer[i]+=nDelay;
			m_CharacterList[i].m_DelayTimer+=nDelay;
			return;
		}
	}
}

void CCharacterLightShadowManager::CreateEditor()
{
	//sprintf(tmp, "%sSound\\",CHARACTERDATAPATH);

	//CZ3DSound::Init(BaseGraphicsLayer::GethWnd(), Z3DSOUND_3D,"c:/MP-Project/Sound/");
}

void CCharacterLightShadowManager::AddChrLight(CZ3DGeneralChrModel *pChrmodel,vector3 vecPos,DWORD dwColor,float fLens,DWORD dwTime)
{
	static DWORD m_nTotalLight=0;

	D3DLIGHT8 Light;

	CChrLightNode AddNode;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));
	Light.Type=D3DLIGHT_POINT;					
	Light.Attenuation0=0.5f;					
	Light.Attenuation1=0.0f;	

	color Color;
	Color.c=dwColor;
	
	Light.Diffuse.r=Color.r/255.0f;
	Light.Diffuse.g=Color.g/255.0f;
	Light.Diffuse.b=Color.b/255.0f;

	Light.Position.x=Light.Direction.x=vecPos.x;
	Light.Position.y=Light.Direction.y=vecPos.y;
	Light.Position.z=Light.Direction.z=vecPos.z;
	Light.Range=fLens;
	
	AddNode.m_Light=Light;	
	AddNode.m_dwTimer=dwTime;	

	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChrmodel)
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{
			//m_CharacterLightList[i]=AddNode;
			memcpy(	&m_CharacterList[i].m_Light,&AddNode,sizeof(CChrLightNode));
		}
	}
}


//DEL D3DLIGHT8 CCharacterLightShadowManager::GetChrLight(DWORD dwLightID)
//DEL {	
//DEL 	/*
//DEL 	for(int i=0;i<m_CharacterLightList.num;i++)
//DEL 	{
//DEL 		if(m_CharacterLightList[i].m_dwLightID==dwLightID)
//DEL 		{
//DEL 			return m_CharacterLightList[i].m_Light;
//DEL 		}
//DEL 	}
//DEL 	*/
//DEL }

//DEL void CCharacterLightShadowManager::SetChrLight(DWORD dwLightID, D3DLIGHT8 Light)
//DEL {
//DEL 	for(int i=0;i<m_CharacterLightList.num;i++)
//DEL 	{
//DEL 		if(m_CharacterLightList[i].m_dwLightID==dwLightID)
//DEL 		{
//DEL 			m_CharacterLightList[i].m_Light=Light;
//DEL 		}
//DEL 	}	
//DEL }

void CCharacterLightShadowManager::UpdateLight()
{	
	/*
	for(int i=0;i<m_CharacterLightList.num;i++)
	{
		if(--m_CharacterLightList[i].m_dwTimer<=0)
		{			
			m_CharacterLightList[i].m_dwTimer=0;				
		}
	}
	*/
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		if(--m_CharacterList[i].m_Light.m_dwTimer<=0)
		{
			m_CharacterList[i].m_Light.m_dwTimer=0;
		}
	}
	/*
	for(i=0;i<DelList.num;i++)
	{
		DelChrLight(DelList[i]);
	}
	*/
}

void CCharacterLightShadowManager::DelChrLight(DWORD dwLightID)
{
	/*
	for(int i=0;i<m_CharacterLightList.num;i++)
	{
		if(m_CharacterLightList[i].m_dwLightID==dwLightID)
		{
			m_CharacterLightList.DelIndex(i);
			return;
		}
	}
	*/
}

void CCharacterLightShadowManager::CharacterMovementVector(CZ3DGeneralChrModel *pChrmodel, vector3 vecChrMove,float fAccelate)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChrmodel)
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{
			//m_CharacterPerFrameMove[i]=vecChrMove;
			/*
			if(fabsf(fAccelate) < 1.0f)
			{
				m_CharacterList[i].m_fAccelate=fAccelate;
				m_CharacterList[i].m_vecZeroVelocity=vecChrMove;
				m_CharacterList[i].m_vecPerFrameMove=vecChrMove;			
			}
			if( fabsf(m_CharacterList[i].m_fAccelate - fAccelate) > 0.9f)
			{
				m_CharacterList[i].m_fAccelate=fAccelate;
				m_CharacterList[i].m_vecZeroVelocity=vecChrMove;
				m_CharacterList[i].m_vecPerFrameMove=vecChrMove;			
			}
			*/
			if(fabsf(fAccelate)<=0.01f)
			{
				m_CharacterList[i].m_bFirstAccelate=false;
				m_CharacterList[i].m_fAccelate=fAccelate;
				m_CharacterList[i].m_vecZeroVelocity=vecChrMove;
				m_CharacterList[i].m_vecPerFrameMove=vecChrMove;			
			}
			else
			{
				if(m_CharacterList[i].m_bFirstAccelate==false)
				{
					m_CharacterList[i].m_fAccelate=fAccelate;
					m_CharacterList[i].m_vecZeroVelocity=vecChrMove;
					m_CharacterList[i].m_vecPerFrameMove=vecChrMove;			
					m_CharacterList[i].m_bFirstAccelate=true;
				}
			}
			break;
		}
	}
}

vector3 CCharacterLightShadowManager::GetCharacterMovementVector(CZ3DGeneralChrModel *pChrmodel)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChrmodel)
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{			
			//return m_CharacterPerFrameMove[i];			
			return m_CharacterList[i].m_vecPerFrameMove;
		}
	}
	return vector3(0.0f,0.0f,0.0f);
}

void CCharacterLightShadowManager::CharacterRandomPos(CZ3DGeneralChrModel *pChrmodel,int nTimer, vector3 vecRand)
{	
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChrmodel)
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{
			//m_CharacterRandomPostionTimer[i]=nTimer;
			//m_CharacterRandomPostionAdder[i]=vecRand;
			m_CharacterList[i].m_RandomPositionTimer=nTimer;
			m_CharacterList[i].m_vecRandomPositionAdder=vecRand;
			break;
		}
	}	
}

void CCharacterLightShadowManager::RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	//pd3dDevice->SetVertexShader(m_dwCharacterVertexShader);
	//pd3dDevice->SetPixelShader(m_dwCharacterPixelShader);
	
	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xffffffff);
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
	
	//pd3dDevice->SetTexture(0,NULL);
	//pd3dDevice->SetTexture(2,NULL);

	matrix matWorld;
	matWorld.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

	CSceneNode CharScene;
	CharScene.m_fRad=300.0f;
	vector3 vecCharPos;
	vector3 vecCharScreenPos;
	float fW;

	for(int cChar=0;cChar<(int)m_CharacterList.size();cChar++)
	{		
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{
			m_CharacterList[cChar].m_pChrmodel->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.z);
			CharScene.m_AccumulateTM.Translation(vecCharPos);
			
			if(CharScene.isCulling())
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(false);					
				BaseGraphicsLayer::TransformVector(vecCharPos,vecCharScreenPos,fW);
				//m_CharacterScreenPositionList[cChar].x=(int)vecCharScreenPos.x;
				//m_CharacterScreenPositionList[cChar].y=(int)vecCharScreenPos.y;
				m_CharacterList[cChar].m_ptScreenPosition.x=(int)vecCharScreenPos.x;
				m_CharacterList[cChar].m_ptScreenPosition.y=(int)vecCharScreenPos.y;
				//BaseGraphicsLayer::TransformVector(m_InstanceObjectList[cObject].m_vecPosition,vecResult,w);
			}
			else
			{
				m_CharacterList[cChar].m_pChrmodel->SetCull(true);
				m_CharacterList[cChar].m_ptScreenPosition.x=-1;
				m_CharacterList[cChar].m_ptScreenPosition.y=-1;
				//m_CharacterScreenPositionList[cChar].x=-1;
				//m_CharacterScreenPositionList[cChar].y=-1;
			}
		}		
	}	
	
	/*
	if(m_CharacterList.num>0)
	{
		CharacterLightSetting(0,pd3dDevice);
	}
	*/
	

	//CZ3DRenderable::Process();
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	/*

	pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState(1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState(2,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(2,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(2,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState(3,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(3,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(3,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
	*/

	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);

	

	char *pShadowCollision;
	vector3 vecChrPos;

	float fInterVal=SECTORSIZE/256.0f;
	int px,pz;
	float fPosx,fPosz;

	

	D3DXVECTOR4 LightDir=D3DXVECTOR4( -1.0f, -1.0f, -1.0f, 0.0f );
	D3DXVec4Normalize(&LightDir,&LightDir);
	pd3dDevice->SetVertexShaderConstant(12, &LightDir,  1 );	// light direction   

	vector3 vecSunLightDir( CSceneManager::m_WeatherManager.m_SunLight.Direction.x,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.y,
							CSceneManager::m_WeatherManager.m_SunLight.Direction.z);

	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_POINT);	
	pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_POINT);
	
	int cChar=0;
	for(;cChar<(int)m_CharacterList.size();cChar++)
	{		
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		pShadowCollision=m_pHeightField->GetHeightFieldShadowCollision(vecChrPos);	

		px=(int)(vecChrPos.x/SECTORSIZE);
		pz=(int)(vecChrPos.z/SECTORSIZE);
		fPosx=vecChrPos.x-(px*SECTORSIZE);
		fPosz=vecChrPos.z-(pz*SECTORSIZE);	
		
		if(pShadowCollision)
		{
			fPosx/=fInterVal;
			fPosz/=fInterVal;
			if(pShadowCollision[(int)fPosx+((int)fPosz)*256]&0x01)
			{
				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff999999);
				//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);
			}
			else
			{
				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);
			}			
		}
		else
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);
		}
		
		pd3dDevice->SetStreamSource( 0, NULL, 0 );
		pd3dDevice->SetStreamSource( 1, NULL, 0 );
		pd3dDevice->SetStreamSource( 2, NULL, 0 );

		
		//if(m_CharacterLightList[cChar].m_dwTimer>0)
		if(m_CharacterList[cChar].m_Light.m_dwTimer>0)
		{
			//pd3dDevice->SetLight(1,&m_CharacterLightList[cChar].m_Light);
			pd3dDevice->SetLight(1,&m_CharacterList[cChar].m_Light.m_Light);
			pd3dDevice->LightEnable(1,TRUE);
		}
		else
		{			
			pd3dDevice->LightEnable(1,FALSE);
		}	
		
		D3DXMATRIX matWorld,matView,matProj;
		vector3 vecChrPos;
		m_CharacterList[cChar].m_pChrmodel->GetPosition(vecChrPos);
		D3DXMatrixTranslation(&matWorld,vecChrPos.x,vecChrPos.y,vecChrPos.z);

		pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);		

		D3DXMATRIX  matTemp;
		D3DXMatrixTranspose(&matTemp,&(matWorld * matView * matProj));
		pd3dDevice->SetVertexShaderConstant(8, &matTemp, 4);			
		D3DXMatrixTranspose(&matTemp,&matWorld);
		pd3dDevice->SetVertexShaderConstant(0, &matTemp, 4);

		D3DXVECTOR4 tmp;
		tmp.x = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().x;
		tmp.y = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().y;
		tmp.z = CSceneManager::m_ViewCamera->GetMatPosition()->GetLoc().z;
		tmp.w = 0.0f;	
		
		pd3dDevice->SetVertexShaderConstant(24, tmp, 1);	
		
		D3DXVECTOR4 half(0.5f,0.5f,0.5f,0.5f);
		pd3dDevice->SetVertexShaderConstant(33, &half, 1);

		D3DXVECTOR4 ahalf(7.0f,7.0f,7.0f,7.0f);
		pd3dDevice->SetVertexShaderConstant(34, &ahalf, 1);
		
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);		
		if(m_CharacterList[cChar].m_pChrmodel->IsDisabled()==false)
		{			
			
			
			pd3dDevice->SetPixelShader(m_dwCharacterSpecPixelShader);
			pd3dDevice->SetVertexShader(m_dwCharacterSpecularVertexShader);			
			m_CharacterList[cChar].m_pChrmodel->Render();
		}		
	}	
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
	//CZ3DRenderable::SecondProcess();

	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);	
	pd3dDevice->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

	for(int i=1;i<8;i++)	
		pd3dDevice->LightEnable(i,FALSE);		
	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP, D3DTOP_DISABLE);	
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetTexture(2,NULL);
	pd3dDevice->SetTexture(3,NULL);
	pd3dDevice->SetPixelShader(NULL);	
	/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
	pd3dDevice->SetLight(0,&m_WeatherManager.m_SunLight);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,m_WeatherManager.m_AmbientColor->c);
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	CSceneNode CharScene;
	CharScene.m_fRad=300.0f;
	vector3 vecCharPos;
	for(int cChar=0;cChar<m_CharacterList.num;cChar++)
	{
		m_CharacterList[cChar]->GetPosition(vecCharPos.x,vecCharPos.y,vecCharPos.x);
		CharScene.m_AccumulateTM.Translation(vecCharPos);
		
		if(CharScene.isCulling())
			m_CharacterList[cChar]->SetCull(true);
		else
			m_CharacterList[cChar]->SetCull(false);
		if(cChar==0)
			m_CharacterList[cChar]->SetCull(false);		
	}		
	CZ3DRenderable::Process();
	*/
}

CollisionType CCharacterLightShadowManager::GetCharacterCollisionType(CZ3DGeneralChrModel *pChrmodel)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		//if(m_CharacterList[i]==pChrmodel)
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{		
			return m_CharacterList[i].m_CollisionType;	
		}
	}
	return CT_NONE;
}

float CCharacterLightShadowManager::GetCharacterVelocity(CZ3DGeneralChrModel *pChrmodel)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{		
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{		
			return m_CharacterList[i].m_fFallSpeed;	
		}
	}
	return 0.0f;
}

void CCharacterLightShadowManager::CharacterFirstAccelate(CZ3DGeneralChrModel *pChrmodel,bool bFirstAccelate)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{		
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{		
			m_CharacterList[i].m_bFirstAccelate=bFirstAccelate;
			return;
		}
	}	
}

void CCharacterLightShadowManager::CharacterCollisionAble(CZ3DGeneralChrModel *pChrmodel, bool bCollisionDetectionAble)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{		
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{		
			m_CharacterList[i].m_bCollisionDetectAble=bCollisionDetectionAble;
			return;
		}
	}	
}

void CCharacterLightShadowManager::CharacterGravityAble(CZ3DGeneralChrModel *pChrmodel, bool bGravityAble)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{		
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{		
			m_CharacterList[i].m_bGravityAble=bGravityAble;
			return;
		}
	}	
}

void CCharacterLightShadowManager::CharacterCollisionDetectType(CZ3DGeneralChrModel *pChrmodel, int nType)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		if(m_CharacterList[i].m_pChrmodel==pChrmodel)
		{
			m_CharacterList[i].m_CollisionDetectType=nType;
			return;
		}
	}
}

CCharacterLightShadowManager::CCharacterDataNode CCharacterLightShadowManager::GetCharacterDataNode(CZ3DGeneralChrModel *pChrmodel)
{
	int i;
	for(i=0;i<(int)m_CharacterList.size();i++)
	{
		if(m_CharacterList[i].m_pChrmodel!=pChrmodel)
			continue;

		break;
	}
	return m_CharacterList[i];
}

void CCharacterLightShadowManager::SetCharacterDataNode(CCharacterDataNode SetNode)
{
	for(int i=0;i<(int)m_CharacterList.size();i++)
	{
		if(m_CharacterList[i].m_pChrmodel==SetNode.m_pChrmodel)
		{
			m_CharacterList[i]=SetNode;
			return;			
		}
	}
}

CZ3DGeneralChrModel *CCharacterLightShadowManager::SwitchingModel(int iNum,CZ3DGeneralChrModel *pNew)
{
	if((int)m_CharacterList.size() <= iNum)
		return NULL;
	
	DeleteElement(pNew);

	CZ3DGeneralChrModel *pOld = NULL;
	pOld = m_CharacterList[iNum].m_pChrmodel;
	m_CharacterList[iNum].m_pChrmodel = pNew;
	return pOld;
}

VOID		CCharacterLightShadowManager::SetNodeModel( CZ3DGeneralChrModel *pOrg, CZ3DGeneralChrModel *pNew )
{
	pNew->SetPosition( pOrg->GetPosition() ) ;
	pNew->SetDirection( pOrg->GetDirection() ) ;

	DeleteCharacter( pOrg ) ;
	
	/*
	int iIndex = GetCharacterDataNodeIndex( pOrg ) ;
	
	CZ3DGeneralChrModel *pOld = NULL ;
	pOld = m_CharacterList[iIndex].m_pChrmodel ;
	m_CharacterList[iIndex].m_pChrmodel = pNew ;
	
	m_CharacterList[iIndex].m_pChrmodel->SetPosition( pOld->GetPosition() ) ;
	m_CharacterList[iIndex].m_pChrmodel->SetDirection( pOld->GetDirection() ) ;

	if ( pOld )
	{
		delete pOld ;
		pOld = NULL ;
	}
	*/
}

CCharacterLightShadowManager::CCharacterDataNode CCharacterLightShadowManager::UndeleteListChr( CZ3DGeneralChrModel* pModel ) 
{
	int cChr ;
	std::vector<CCharacterLightShadowManager::CCharacterDataNode>::iterator obj = m_CharacterList.begin();

	for ( cChr = 0 ; cChr < (int)m_CharacterList.size(); cChr ++,obj++)
	{
		if ( pModel == m_CharacterList[cChr].m_pChrmodel )
		{
			m_CharacterList.erase( obj ) ;
			break;
		}
	}

	return m_CharacterList[ cChr ] ;
}

void CCharacterLightShadowManager::UnallocListChr( CCharacterLightShadowManager::CCharacterDataNode pModel, BOOL bFirst ) 
{
	std::vector<CCharacterLightShadowManager::CCharacterDataNode>::iterator obj = m_CharacterList.begin();

	for ( int cChr = 0 ; cChr < (int)m_CharacterList.size(); cChr++,obj++ )
	{
		if ( pModel.m_pChrmodel == m_CharacterList[ cChr ].m_pChrmodel )
		{
			m_CharacterList.erase(obj) ;

			m_CharacterList.insert(m_CharacterList.begin(), pModel);
			return ;
		}
	}
	
	if ( bFirst )
		m_CharacterList.insert(m_CharacterList.begin(), pModel);
	else
		m_CharacterList.push_back(pModel);
}

CCharacterLightShadowManager::CCharacterDataNode* CCharacterLightShadowManager::GetCharacterDataNodeEx( CZ3DGeneralChrModel *pModel ) 
{
	for ( int cChr = 0 ; cChr < (int)m_CharacterList.size(); cChr ++ )
	{
		if ( pModel == m_CharacterList[ cChr ].m_pChrmodel )
		{
			return &m_CharacterList[ cChr ] ;
		}
	}

	return NULL ;
}

int					CCharacterLightShadowManager::GetCharacterDataNodeIndex( CZ3DGeneralChrModel *pModel ) 
{
	for ( int cChr = 0 ; cChr < (int)m_CharacterList.size(); cChr ++ )
	{
		if ( pModel == m_CharacterList[ cChr ].m_pChrmodel )
		{
			return cChr ;
		}
	}

	return -1 ;
}