#include <MathUtil.h>

#include "SectorScene.h"
#include <math.h>
#include "SceneManager.h"
#include <d3dx8.h>
#include "BaseGraphicsLayer.h"
#include "convertTexture.h"
#include "FogScene.h"
#include "WaveLine.h"
#include "WaterW.h"
#include "NTexture.h"



#include "RenderOption.h"
#include "GMMemory.h"


CTexture CSectorScene::m_DetailTexture;
CTexture CSectorScene::m_TerrainTexture[12];
CTexture CSectorScene::m_NormalPlantTexture;
CTexture CSectorScene::m_LayerFogTexture;
CTexture CSectorScene::m_CloudShadow;

CRenderTexture CSectorScene::m_ObjectShadowTexture;
CRenderTexture CSectorScene::m_SwapShadowTexture;
CRenderTexture CSectorScene::m_SectorShadowTexture[9];
CRenderTexture CSectorScene::m_TerrainShadeTexture;
CRenderTexture CSectorScene::m_pShadowTexture;//그림자
char CSectorScene::m_SectorShadowCollision[9][256*256];

//CTexture CSectorScene::m_CompleteShadowAndTerrainTexture[9];
int CSectorScene::m_UsedShadowTerrain[9];

CWaterScene CSectorScene::m_WaterScene;

TriTreeNode CSectorScene::m_TriPool[POOL_SIZE];
long CSectorScene::m_NextTriNode;

float CSectorScene::m_fCloudMove=0.0f;

CTexture CSectorScene::m_LODGrassTexture;
CTexture *CSectorScene::m_GrassRangeTexture;

color CSectorScene::m_TerrainDetailFixColor;

DWORD CSectorScene::m_dwObjectDiffuseVertexShader=0xffffffff;
DWORD CSectorScene::m_dwObjectSpecularVertexShader=0xffffffff;
DWORD CSectorScene::m_dwObjectDiffusePixelShader=0xffffffff;
DWORD CSectorScene::m_dwObjectSpecularPixelShader=0xffffffff;

LPDIRECT3DCUBETEXTURE8 CSectorScene::m_pNormalizeCubeMap=NULL;
CGemRender *CSectorScene::m_Boid[3] = {0,0,0};
bool CSectorScene::m_bBoidMeshSet = false;
CEffScript *CSectorScene::m_pLightEsf = NULL;

char strObjectDiffuseVertexShader[]=
"vs.1.1\n"
"m4x4 oPos,v0,c16\n"
"m4x4 r0,v0,c16\n"
"mov oT0.xy,v2.xy\n"//Diffuse
"mov oFog.x,r0.z\n"
"dp3 r2,v1,c15\n"
"max oD0,r2,c2\n";

char strObjectDiffusePixelShader[]=
"ps.1.1\n"
"def c0,0.0,0.0,0.0,1.0f\n"
"tex t0\n"//Diffuse
"tex t1\n"//Normal
"tex t2\n"//Lightmap"
"mov r0,t0\n"
"mul_x2 r0.rgb,r0,v0\n"
"+mov r0.a,t0.a\n";
//"+add r0.a,r0.a,c0.a\n";
//"mul_x2 r0,r0,t2\n";

char strObjectSpecularVertexShader[]=
"vs.1.0\n"
"m4x4 oPos, v0, c16\n"
"mov  oT0.xy, v2.xy\n"   // Texture Coords
"mov  oT1.xy, v2.xy\n"   // Bump
"mov  oT2.xy, v2.xy\n"

//;r9 = tangent U vector = normal cross tangent V vector
"mov  r8, v4\n"
"mul  r9, r8.yzxw, v1.zxyw\n"
"mad  r9, -r8.zxyw, v1.yzxw, r9\n"

"sub  r1, c32, v0\n" // Light direction
//Normalize
"dp3 r11.x,r1.xyz,r1.xyz\n"
"rsq r11.xyz,r11.x\n"
"mul r1.xyz, r1.xyz, r11.xyz\n"

"sub  r0, c35, v0\n" //Viewer direction
//Normalize
"dp3 r11.x,r0.xyz,r0.xyz\n"
"rsq r11.xyz,r11.x\n"
"mul r0.xyz, r0.xyz, r11.xyz\n"
"add  r4.xyz, r0.xyz, r1.xyz\n" //Halfway

//;r4 = tangent space halfway vector 
"dp3  r6.x, r9, r4\n"
"dp3  r6.y, r8, r4\n"
"dp3  r6.z, v1, r4\n"

"mov  oT3.xyz,r6.xyz\n";  // Halfway vector to be renormalized by cube map

char strObjectSpecularPixelShader[]=	
"ps.1.1\n"
"def c0,0.9,0.9,0.9,0.9\n"
"tex t0\n"//diffuse
"tex t1\n"//normal
"tex t2\n"//
"tex t3\n"//Normalize Cubemap
//"mov r0,t3\n";
"dp3_sat r0,t1_bx2,t3_bx2\n"//N.H
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r0,r0,c0\n"
"mul r0,r0,t0.a\n";

//"mul_x2 r0,r1,t1.a\n";	


CSectorScene::CSectorScene()
{
	//LogMessage("Constructor SectorScene");
	//
	m_isEdit=false;
	//	
	m_pROAMVertex=NULL;
	m_pROAMIndices=NULL;
	m_pParent=NULL;
	m_MapDungeon=NULL;
	m_MapHeight=NULL;
	m_MapMash=NULL;
	m_MapPlant=NULL;
	m_MapWide=NULL;
	m_MapTexture=NULL;		
	m_MapWater=NULL;
	m_MapFall=NULL;
	m_MapMustDivideVertex=NULL;
	m_MapEffect=NULL;
	m_ShadowUsed=-1;		
	m_pMapStorage=NULL;
	m_pLODGrassAlpha=NULL;
	m_WorldEffectManager.SetDevice(CSceneManager::GetDevice());
	SetBoidMesh();
	m_bNight = false;

	m_pNotCullHouseIndex = NULL;
	m_iNotCullHouseIndexCnt = 0;
	m_iNotCullHouseIndex = 0;	

	//m_TerrainDetailFixColor.c=0xff999999;
	//LogMessage("Constructor SectorScene complete");
}

CSectorScene::~CSectorScene()
{	
	m_pROAMVertex->Release();
	m_pROAMIndices->Release();

	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{
		CROSSM::SafeDelete(m_HouseObjectNode[cHouse]);
//		CSceneManager::_ReleaseObj(CROSSM::POOL_HOUSEOBJSCENE,m_HouseObjectNode[cHouse]);
		m_HouseObjectNode[cHouse] = NULL;
	}

	m_HouseObjectNode.clear();

	
	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{
		CROSSM::SafeDelete(m_ObjectSceneNode[cObject]);
//		CSceneManager::_ReleaseObj(CROSSM::POOL_OBJECTSCENE, m_ObjectSceneNode[cObject]);		
	}
	m_ObjectSceneNode.clear();
	int i;
	for(i=0;i<(int)m_LandscapeEffectNode.size();i++) {
		if(m_LandscapeEffectNode[i] != NULL) {
			switch(m_LandscapeEffectNodeValue[i]) {
			
			case 1: //fog
				delete (CFogScene *)m_LandscapeEffectNode[i];
				break;
			case 2: // boid
				delete (CBoidGroup *)m_LandscapeEffectNode[i];
				break;
			case 3: // mesh ani
				delete (CGemRender *)m_LandscapeEffectNode[i];
				break;
			case 4: // wave line
				delete (CWaveLine *)m_LandscapeEffectNode[i];
				break;
			case 5: // 격자 water
				delete (CWaterW *)m_LandscapeEffectNode[i];
			}
		}
		
	}
	DeleteBoidMesh();
	
	if(m_pLightEsf != NULL)
	{
		delete[] m_pLightEsf;
		m_pLightEsf = NULL;

	}

}	

void CSectorScene::DeleteCullHouseIndex() {
	if(m_pNotCullHouseIndex != NULL)
	{
		delete[] m_pNotCullHouseIndex;
		m_pNotCullHouseIndex = NULL;
		m_iNotCullHouseIndexCnt = 0;
	}

}
void CSectorScene::Create(CMapStorage *pMapStorage)
{		
	m_pMapStorage=pMapStorage;

	static bool bFirstInit=false;
	//LogMessage("SectorScene Create init");
	if(!bFirstInit)
	{
		//LogMessage("SectorScene Texture Init");
		CTexture::SetPath(TERRAINTEXTUREPATH);
		bFirstInit=true;
		/*
		m_TerrainTexture[0].Load("hig0.dds");
		m_TerrainTexture[1].Load("hig1.dds");
		m_TerrainTexture[2].Load("hig2.dds");
		m_TerrainTexture[3].Load("hig3.dds");	

		m_TerrainTexture[4].Load("med0.dds");		
		m_TerrainTexture[5].Load("med1.dds");
		m_TerrainTexture[6].Load("med2.dds");
		m_TerrainTexture[7].Load("med3.dds");

		m_TerrainTexture[8].Load("low0.dds");
		m_TerrainTexture[9].Load("low1.dds");
		m_TerrainTexture[10].Load("low2.dds");
		m_TerrainTexture[11].Load("low3.dds");
		*/
		/*
		m_DetailTexture.Load("detail.dds");	
		
		if(CRenderOption::m_RangeGrassRender)
		{		
			CTexture::SetPath(NATURETEXTUREPATH);

			m_LODGrassTexture.Load("LODGrass.dds");

			m_GrassRangeTexture=new CTexture();
			m_GrassRangeTexture->Load("GrassRange.dds");
		}
		*/
		//m_NormalPlantTexture.Load("NormalTreeTexture.dds");
		//m_LayerFogTexture.Load("volfog.dds");
		/*
		if(GF3OPTION)
			m_CloudShadow.Load("cloudsshadow.dds");
			*/				
		m_ObjectShadowTexture.Create(SHADOWSIZE,SHADOWSIZE);
		m_SwapShadowTexture.Create(SHADOWSIZE,SHADOWSIZE);
		m_TerrainShadeTexture.Create(SHADOWSIZE,SHADOWSIZE);		

		
		for(int cSectorTexture=0;cSectorTexture<9;cSectorTexture++)
		{
			//m_SectorShadowTexture[cSectorTexture].Create(SHADOWSIZE,SHADOWSIZE);
			m_UsedShadowTerrain[cSectorTexture]=-1;
		}		
		

		if(GF3OPTION)
		{

		}
	}	
	m_TM.MakeIdent();
	m_TM._41=-10000000000.0f;
	m_TM._43=-10000000000.0f;
	int ix,iy;
	for(ix=0;ix<SECTORSX;ix++)
	{
		for(iy=0;iy<SECTORSX;iy++)
		{
			m_SectorVertex[ix+iy*SECTORSX].v.x=ix*LINTERVAL;
			m_SectorVertex[ix+iy*SECTORSX].v.z=iy*LINTERVAL;
			m_SectorVertex[ix+iy*SECTORSX].v.y=0.0f;
			/*
			m_SectorVertex[ix+iy*SECTORSX].n.x=0.0f;
			m_SectorVertex[ix+iy*SECTORSX].n.y=1.0f;
			m_SectorVertex[ix+iy*SECTORSX].n.z=0.0f;
			*/
			m_SectorVertex[ix+iy*SECTORSX].tu=(float)ix/(float)(SECTORSX-1);
			m_SectorVertex[ix+iy*SECTORSX].tv=(float)iy/(float)(SECTORSY-1);
			/*
			m_SectorVertex[ix+iy*SECTORSX].tu1=((float)ix/(float)SECTORSX-1)*DETAILTEXTUREDEPTH;
			m_SectorVertex[ix+iy*SECTORSX].tv1=((float)iy/(float)SECTORSY-1)*DETAILTEXTUREDEPTH;
			m_SectorVertex[ix+iy*SECTORSX].spec.c=0xffffffff;		
			*/
		}
	}	
	// Terrain Vertex & Index Buffer Create//	
	///*
	//LogMessage("SectorScene VertexBuffer & IndexBuffer Create");
	CSceneManager::GetDevice()->CreateVertexBuffer(SECTORSX*SECTORSY*sizeof(SectorVertex),
												   D3DUSAGE_WRITEONLY ,SectorFVF,D3DPOOL_MANAGED,
												   &m_pROAMVertex);
	CSceneManager::GetDevice()->CreateIndexBuffer((SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE*sizeof(WORD),D3DUSAGE_WRITEONLY,
												   D3DFMT_INDEX16,D3DPOOL_MANAGED,
												   &m_pROAMIndices);
	//											   */
	//LogMessage("SectorScene VertexBuffer & IndexBuffer Complete");
	// Plant Vertex &Index Buffer Create//	

	if((CRenderOption::m_ObjectProjectShadow == 1 || CRenderOption::m_TreeProjectShadow == 1) &&
		m_pShadowTexture.GetTexture()==NULL)
	{
		m_pShadowTexture.Create(SHADOWSIZE,SHADOWSIZE);
	}
	

	m_TreeObjectNode.Create();

	if(CRenderOption::m_AllObjectBump && m_dwObjectDiffuseVertexShader==0xffffffff)
	{		

		DWORD dwBumpVertexDecl[]=
		{
			D3DVSD_STREAM( 0 ),									
			D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position 
			D3DVSD_REG( 1, D3DVSDT_FLOAT3 ), // Normal						
			D3DVSD_REG( 2, D3DVSDT_FLOAT2 ), // Tex coords 1			
			D3DVSD_REG( 4, D3DVSDT_FLOAT3 ), // Tagent U
			D3DVSD_END()
		};

		LPD3DXBUFFER pCode;

		D3DXAssembleShader(strObjectDiffusePixelShader,strlen(strObjectDiffusePixelShader),0,NULL,&pCode,NULL);										 
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwObjectDiffusePixelShader);    
		pCode->Release();		

		D3DXAssembleShader(strObjectDiffuseVertexShader,strlen(strObjectDiffuseVertexShader),0,NULL,&pCode,NULL);				
		BaseGraphicsLayer::GetDevice()->CreateVertexShader( dwBumpVertexDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwObjectDiffuseVertexShader, 0 );
		pCode->Release();		

		D3DXAssembleShader(strObjectSpecularPixelShader,strlen(strObjectSpecularPixelShader),0,NULL,&pCode,NULL);										 
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwObjectSpecularPixelShader);    
		pCode->Release();		

		D3DXAssembleShader(strObjectSpecularVertexShader,strlen(strObjectSpecularVertexShader),0,NULL,&pCode,NULL);				
		BaseGraphicsLayer::GetDevice()->CreateVertexShader( dwBumpVertexDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwObjectSpecularVertexShader, 0 );
		pCode->Release();
		

		char strNormalizeCubeMap[256];
		sprintf(strNormalizeCubeMap,"%s\\%s",NATURETEXTUREPATH,"NormalizeCubeMap.dds");	

		D3DXCreateCubeTextureFromFile(CSceneManager::GetDevice(),strNormalizeCubeMap,&m_pNormalizeCubeMap);			

	}
	
	//m_GrassObjectNode.Create();
	//m_GrassObjectNode.m_pSectorVertex=m_SectorVertex;	
	//LogMessage("SectorScene Create Complete");	
}

void CSectorScene::Generate(float fStartX,float fStartY)
{		
	m_TM.Translation(vector3(fStartX,0.0f,fStartY));	
	m_AccumulateTM=m_TM;		
	
	m_MapDungeon=m_pMapStorage->FindDungeonMap(fStartX,fStartY);	
	m_MapMash=m_pMapStorage->FindMeshMap(fStartX,fStartY);
	m_MapPlant=m_pMapStorage->FindPlantMap(fStartX,fStartY);	

	GenerateWater();
	GenerateHeight();
	GenerateWideMap();
	GenerateHouseObject();	
	GeneratePlant();	
	GenerateMustDivideVertex();
	GenerateEffect();
	GenerateLandscapeEffect();
	GenerateSectorLight();
	GenerateObject();


	Init();
	//m_ShadowUsed=-1;
}

void CSectorScene::DeterminTerrainTexture(float fHeight,float fPosx,float fPosy)
{	
	const float FIELDHIGH=10000.0f;
	const float FIELDMIDDLE=5000.0f;
	const float FIELDLOW=0.0f;
	int indexx=fPosx/SECTORSIZE;
	int indexy=fPosy/SECTORSIZE;
	indexx%=2;
	indexy%=2;

	if(FIELDHIGH<=fHeight)
	{
		if(indexx==0)
		{
			if(indexy==0)m_MapTexture=m_TerrainTexture[0].GetTexture();			
			else m_MapTexture=m_TerrainTexture[2].GetTexture();
		}			
		else
		{
			if(indexy==0)m_MapTexture=m_TerrainTexture[1].GetTexture();			
			else m_MapTexture=m_TerrainTexture[3].GetTexture();
		}
	}
	else
	{
		if(FIELDMIDDLE<=fHeight)
		{
			if(indexx==0)
			{
				if(indexy==0)m_MapTexture=m_TerrainTexture[4].GetTexture();			
				else m_MapTexture=m_TerrainTexture[6].GetTexture();
			}			
			else
			{
				if(indexy==0)m_MapTexture=m_TerrainTexture[5].GetTexture();			
				else m_MapTexture=m_TerrainTexture[7].GetTexture();
			}
		}
		else
		{
			if(indexx==0)
			{
				if(indexy==0)m_MapTexture=m_TerrainTexture[8].GetTexture();			
				else m_MapTexture=m_TerrainTexture[10].GetTexture();
			}			
			else
			{
				if(indexy==0)m_MapTexture=m_TerrainTexture[9].GetTexture();			
				else m_MapTexture=m_TerrainTexture[11].GetTexture();
			}
		}
	}	
}
void CSectorScene::RenderSectorLight()
{
	// Sector Light Render
	if(CSceneManager::m_bRenderSectorLight) 
	{
		m_SectorLightManager.RenderLight();
	}
	float fTime = CSceneManager::GetWeatherTime();
	if((fTime >= 19.0f) || (fTime <= 5.0f)) {
		matrix *matPos = CSceneManager::m_ViewCamera->GetMatPosition();

		int indexx=(int)((matPos->_41)/SECTORSIZE);
		int indexy=(int)((matPos->_43)/SECTORSIZE);

		int iSectorx = (int)((m_TM._41)/SECTORSIZE);
		int iSectory = (int)((m_TM._43)/SECTORSIZE);

		if((indexx == iSectorx) && (indexy == iSectory))	
			m_SectorLightManager.RenderTerrainLight();
	}
}
void CSectorScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	if(m_ShadowUsed!=-1)
	{			
		if(m_MapWide)
		{
			if(m_MapWide->m_WideMapTexture[0] && m_MapWide->m_WideMapTexture[0]->IsLoaded())
				pd3dDevice->SetTexture(0,m_MapWide->m_WideMapTexture[0]->GetTexture());
			else
				pd3dDevice->SetTexture(0,NULL);
			pd3dDevice->SetTexture(1,NULL);				

			//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffaaaaaa);
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,m_TerrainDetailFixColor.c);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
			//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			/*
			if(m_MapWide->m_nDetail<=1)
			{				
				pd3dDevice->SetTexture(0,m_SectorShadowTexture[m_ShadowUsed].GetTexture());
				pd3dDevice->SetTexture(1,NULL);				

				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffaaaaaa);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	

				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			}
			else
			{
				pd3dDevice->SetTexture(0,m_SectorShadowTexture[m_ShadowUsed].GetTexture());
				pd3dDevice->SetTexture(1,m_MapWide->m_WideMapTexture[0].GetTexture());

				pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,0);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);

			}
			*/
		}
		else
		{
			pd3dDevice->SetTexture(0,m_SectorShadowTexture[m_ShadowUsed].GetTexture());

			DeterminTerrainTexture(m_fAvgHeight,m_TM._41,m_TM._43);
			pd3dDevice->SetTexture(1,NULL);
			pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,0);

			//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffaaaaaa);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
			/*
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
			*/
		}		
	}
	else
	{	
		if(m_MapWide)
		{
			if(m_MapWide->m_WideMapTexture[0] && m_MapWide->m_WideMapTexture[0]->IsLoaded())
				m_MapTexture=m_MapWide->m_WideMapTexture[0]->GetTexture();
			else
				m_MapTexture=NULL;
		}
		else
		{
			DeterminTerrainTexture(m_fAvgHeight,m_TM._41,m_TM._43);
		}
		pd3dDevice->SetTexture(0,m_MapTexture);
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,m_TerrainDetailFixColor.c);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

		//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	}
	/*
	if(GF3OPTION)
	{
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		pd3dDevice->SetTexture(2,m_CloudShadow.GetTexture());

		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,0);
		pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

		matrix matTrans;
		matTrans.MakeIdent();
		matTrans._11=0.11f;
		matTrans._22=0.11f;		
		float fCloudMoveX=m_AccumulateTM._41/SECTORSIZE;
		float fCloudMoveZ=m_AccumulateTM._43/SECTORSIZE;		
		matTrans._31=m_fCloudMove+fCloudMoveX*0.11f;
		matTrans._32=m_fCloudMove+fCloudMoveZ*0.11f;			


		pd3dDevice->SetTransform( D3DTS_TEXTURE2, matTrans );
		
	}
	*/

	pd3dDevice->SetVertexShader(SectorFVF);
	///*
	pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
	pd3dDevice->SetIndices(m_pROAMIndices,0);	
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
	CSceneManager::m_cRenderPrimitive++;

	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX,1);	
	

	CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
	CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
	/*
	if(GF3OPTION)
	{		
		pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pd3dDevice->SetTexture(2,NULL);
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,2);

		pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}```
	*/
	
	if(m_isEdit)
	{		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff707070);

		pd3dDevice->SetTexture(0,NULL);
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

		pd3dDevice->SetTexture(1,m_DetailTexture.GetTexture());		
		if(m_MapMustDivideVertex)
		{
			LVertex DivideVertex[2];
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffff0000);		
			for(int cVertex=0;cVertex<(int)m_MapMustDivideVertex->m_ptMustDivideVertexPosList.size();cVertex++)
			{			
				DivideVertex[0].v=m_SectorVertex[m_MapMustDivideVertex->m_ptMustDivideVertexPosList[cVertex].x+m_MapMustDivideVertex->m_ptMustDivideVertexPosList[cVertex].y*SECTORSX].v;
				DivideVertex[1].v=DivideVertex[0].v+vector3(0.0f,1000.0f,0.0f);			
				pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP,1,DivideVertex,sizeof(LVertex));			
			}
		}		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}	
}

void CSectorScene::ComputeVariance()
{
	m_CurrentVariance = m_VarianceLeft;
	RecursComputeVariance(0,SECTORSY-1,m_HeightData[SECTORSX*(SECTORSY-1)],
		                  SECTORSX-1,0,m_HeightData[(SECTORSX-1)],
						  0,0,m_HeightData[0],1);
	m_CurrentVariance = m_VarianceRight;

	RecursComputeVariance(SECTORSX-1,0,m_HeightData[(SECTORSX-1)],
		                  0,SECTORSY-1,m_HeightData[SECTORSX*(SECTORSY-1)],
						  SECTORSX-1,SECTORSY-1,m_HeightData[(SECTORSX-1)+SECTORSX*(SECTORSY-1)],1);
}

int CSectorScene::RecursComputeVariance(int leftX,int leftY, float leftZ,
										 int rightX,int rightY,float rightZ,
										 int apexX,int apexY,float apexZ,
										 int node)
{	
	int centerX=(leftX+rightX)>>1;
	int centerY=(leftY+rightY)>>1;
	int myVariance;

	float centerZ=m_HeightData[(centerY*SECTORSX)+centerX];
	
	myVariance = abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));				
	
	if ( (abs(leftX - rightX) >= 8) ||
		 (abs(leftY - rightY) >= 8) )
	{
		myVariance = MAX( myVariance, RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
		myVariance = MAX( myVariance, RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
	}
	
	if (node < (1<<VARIANCE_DEPTH))
		m_CurrentVariance[node] = 1 + (myVariance*2.5f);
	return myVariance;	
}

void CSectorScene::Tessellate()
{
	m_CurrentVariance = m_VarianceLeft;	
	
	RecursTessellate(&m_BaseLeft,0,SECTORSY-1,
		                         SECTORSX-1,0,
								 0,0,1);
	m_CurrentVariance = m_VarianceRight;
	RecursTessellate(&m_BaseRight,SECTORSX-1,0,
		                          0,SECTORSY-1,
								  SECTORSX-1,SECTORSY-1,1);
								  //*/
}

inline void CSectorScene::RecursTessellate(TriTreeNode *tri, int leftX,int leftY,
									int rightX,int rightY,int apexX,int apexY,int node)
{
	float TriVariance;	

	int centerX=(int)((leftX+rightX)*0.5f);
	int centerY=(int)((leftY+rightY)*0.5f);

	vector3 *pos;
	float distance;
	if ( node < (1<<VARIANCE_DEPTH) )
	{
		pos=CSceneManager::GetCamera()->GetPosition();

		if(CSceneManager::m_ViewerMode==0 || CSceneManager::m_ViewerMode==2)
			pos=CSceneManager::GetCamera()->GetPosition();
		else
		{		
 			//CSceneManager::m_CharacterList[0]->GetPosition(CharPos.x,CharPos.y,CharPos.z);
			if(!CSceneManager::m_pCameraScript->ISPlay()) {
				vector3 CharPos;
				CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);
				pos=&CharPos;
			}
			else {
				D3DXVECTOR3 vecTmpPos = CSceneManager::m_pCameraScript->GetVecPos();
				vector3 vecTmpPos2 = vector3(vecTmpPos.x,vecTmpPos.y,vecTmpPos.z);
				pos = &(vecTmpPos2);

				/*matrix *matPos = CSceneManager::m_ViewCamera->GetMatPosition();
				vector3 vecTmpPos = vector3(matPos->_41,matPos->_42,matPos->_43);
				pos = &(vecTmpPos);
				*/
				//pos=CSceneManager::GetCamera()->GetPosition();
			}

		}
		float realX=m_TM._41+centerX*LINTERVAL;
		float realY=m_TM._43+centerY*LINTERVAL;
		
		//   MINOTAURS  임시 지형 LOD 무시;
		if(CRenderOption::m_TerrainRange == 7)
			distance = CFastMath::FastSqrt( (realX-pos->x)*(realX-pos->x) + (realY-pos->z)*(realY-pos->z) ) - TERRAINLOD;	
		else
			distance = 1.0f + CFastMath::FastSqrt( (realX-pos->x)*(realX-pos->x) + (realY-pos->z)*(realY-pos->z) );	


		if(distance < 1.0)
			distance = 1;
/*	
		// 과거
		distance = 1.0f + CFastMath::FastSqrt( (realX-pos->x)*(realX-pos->x) + 
			                           (realY-pos->z)*(realY-pos->z) );	
*/
		TriVariance = m_CurrentVariance[node]*500.0f / distance;
		
	}	
	if( node >= 1<<VARIANCE_DEPTH || TriVariance > DETAILGROUND )
	{
		Split(tri);
		if (tri->LeftChild && ((abs(leftX - rightX) >= 3) || (abs(leftY - rightY) >= 3)))
		{
			RecursTessellate( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1  );
			RecursTessellate( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1) );
		}
	}

}

void CSectorScene::Split(TriTreeNode *tri)
{
	if (tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
		Split(tri->BaseNeighbor);

	// Create children and link into mesh
	tri->LeftChild  = AllocateTri();
	tri->RightChild = AllocateTri();

	// If creation failed, just exit.
	if ( !tri->LeftChild )
		return;

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor  = tri->RightChild;

	tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != NULL)
	{
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != NULL)
	{
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != NULL)
	{
		if ( tri->BaseNeighbor->LeftChild )
		{
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
			Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = NULL;
		tri->RightChild->LeftNeighbor = NULL;
	}
}

TriTreeNode *CSectorScene::AllocateTri()
{
	TriTreeNode *pTri;
	
	if ( m_NextTriNode >= POOL_SIZE )
	{
		MessageBox(NULL,"너무 많다",0,0);
		return NULL;
	}

	pTri = &(m_TriPool[m_NextTriNode++]);
	pTri->LeftChild = pTri->RightChild = NULL;
	return pTri;
}



inline void CSectorScene::RecursRender(TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY)
{
	if(tri==NULL)
		return;
	if(tri->LeftChild)
	{
		//if(abs(leftX-rightX)<=1 && abs(leftY-rightY)<=1)return;
		int centerX=(leftX+rightX)>>1;
		int centerY=(leftY+rightY)>>1;
		RecursRender( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY);
		RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY);
	}	
	else									// A leaf node!  Output a triangle to be rendered.
	{	
		if(m_RenderUsedVertex[(leftY*SECTORSX)+leftX]==-1)
		{				
			m_pDrawVertex[m_UsedVertex]=m_SectorVertex[(leftY*SECTORSX)+leftX];			
			m_pDrawIndices[m_UsedIndices]=m_UsedVertex;
			m_RenderUsedVertex[(leftY*SECTORSX)+leftX]=m_UsedVertex;			
			m_UsedIndices++;						
			m_UsedVertex++;
		}
		else
		{
			m_pDrawIndices[m_UsedIndices]=m_RenderUsedVertex[(leftY*SECTORSX)+leftX];
			m_UsedIndices++;
		}		

		if(m_RenderUsedVertex[(rightY*SECTORSX)+rightX]==-1)
		{
			m_pDrawVertex[m_UsedVertex]=m_SectorVertex[(rightY*SECTORSX)+rightX];
			m_pDrawIndices[m_UsedIndices]=m_UsedVertex;			
			m_RenderUsedVertex[(rightY*SECTORSX)+rightX]=m_UsedVertex;
			m_UsedIndices++;
			m_UsedVertex++;
		}
		else
		{
			m_pDrawIndices[m_UsedIndices]=m_RenderUsedVertex[(rightY*SECTORSX)+rightX];
			m_UsedIndices++;
		}		
		if(m_RenderUsedVertex[(apexY*SECTORSX)+apexX]==-1)
		{
			m_pDrawVertex[m_UsedVertex]=m_SectorVertex[(apexY*SECTORSX)+apexX];
			m_pDrawIndices[m_UsedIndices]=m_UsedVertex;
			m_RenderUsedVertex[(apexY*SECTORSX)+apexX]=m_UsedVertex;
			m_UsedIndices++;
			m_UsedVertex++;
		}		
		else
		{
			m_pDrawIndices[m_UsedIndices]=m_RenderUsedVertex[(apexY*SECTORSX)+apexX];			
			m_UsedIndices++;		
		}				
		/*
		
		if(m_pDrawIndices[m_UsedIndices-1]==m_pDrawIndices[m_UsedIndices-2])
		{
			int a=0;
		}
		if(m_pDrawIndices[m_UsedIndices-1]==m_pDrawIndices[m_UsedIndices-3])
		{
			int a=0;
		}
		if(m_pDrawIndices[m_UsedIndices-2]==m_pDrawIndices[m_UsedIndices-3])
		{
			int a=0;
		}		
		*/
	}	
}

void CSectorScene::Reset()
{
	m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;
	
	m_BaseLeft.BaseNeighbor = &m_BaseRight;
	m_BaseRight.BaseNeighbor = &m_BaseLeft;
	
	m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor = NULL;
}

void CSectorScene::PrepareRender()
{
	m_UsedIndices=0;
	m_UsedVertex=0;	
	memset(m_RenderUsedVertex,-1,sizeof(m_RenderUsedVertex));	
	m_pROAMVertex->Lock(0,0,(BYTE**)&m_pDrawVertex,0);	
	m_pROAMIndices->Lock(0,(SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE*sizeof(WORD),(BYTE**)&m_pDrawIndices,0);
	
	RecursRender(&m_BaseLeft,0,SECTORSY-1,
		                     SECTORSX-1,0,
							 0,0);	
	RecursRender(&m_BaseRight,SECTORSX-1,0,
		                      0,SECTORSY-1,
							  SECTORSX-1,SECTORSY-1);	
	
	/*
	RecursRender(&m_BaseLeft,0,SECTORSY,
		                     SECTORSX,0,
							 0,0);	
	RecursRender(&m_BaseRight,SECTORSX,0,
		                      0,SECTORSY,
							  SECTORSX,SECTORSY);	
							  */
	m_pROAMVertex->Unlock();
	m_pROAMIndices->Unlock();

	if(m_UsedIndices>(SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE)
	{
		char Error[256];
		sprintf(Error,"%d Indices Overflow",m_UsedIndices);
		MessageBox(NULL,Error,0,0);		
	}
	if(m_UsedVertex>SECTORSX*SECTORSY)
		MessageBox(NULL,"Vertex Overflow",0,0);
}
void CSectorScene::SetSkipHouse(float fx,float fy,float fz,bool bSkip)
{
	int i;
	for(i = 0; i < (int)m_HouseObjectNode.size(); i++ )
	{
		if(fabs(m_HouseObjectNode[i]->m_AccumulateTM._41 - fx) < 1.0f &&
			fabs(m_HouseObjectNode[i]->m_AccumulateTM._42 - fy) < 1.0f &&
			fabs(m_HouseObjectNode[i]->m_AccumulateTM._43 - fz) < 1.0f)
		{
			m_HouseObjectNode[i]->SetSkipFlag(bSkip);
			break;
		}
	}
}
void CSectorScene::DeleteHouse(float fX,float fY,float fZ)
{	
	int i;
	std::vector<CHouseObjectScene*>::iterator obj = m_HouseObjectNode.begin();

	for(i = 0; i < (int)m_HouseObjectNode.size(); i++,obj++ )
	{
		if(fabs(m_HouseObjectNode[i]->m_AccumulateTM._41 - fX) < 1.0f &&
			fabs(m_HouseObjectNode[i]->m_AccumulateTM._42 - fY) < 1.0f &&
			fabs(m_HouseObjectNode[i]->m_AccumulateTM._43 - fZ) < 1.0f)
		{
			m_HouseObjectNode.erase(obj);
			break;
		}
	}
}
void CSectorScene::GenerateHouseObjectLastIn()
{
	m_MapHouse=m_pMapStorage->FindHouseMap(m_TM._41,m_TM._43);	
	if(m_MapHouse)
	{
		
		
		int cHouse = (int)m_MapHouse->m_strOutName.size() - 1;

		// 밤이라면 night setting
		float fTime = CSceneManager::GetWeatherTime();
		D3DXMATRIX matTm;
		D3DXVECTOR3 vecMaxBox,vecMinBox;

		D3DXVECTOR4 vecTrans;

		char *strOutName;
		char *strInName;
		char *strBspName;
		char *strMedName;

		
		strOutName = m_MapHouse->m_strOutName[cHouse];
		strMedName = m_MapHouse->m_strMedName[cHouse];
		strBspName = m_MapHouse->m_strBspName[cHouse];
		strInName = m_MapHouse->m_strInName[cHouse];

		//////////
		CHouseObjectScene *HouseObjectScene= new CHouseObjectScene();//(CHouseObjectScene *)CSceneManager::_CreateObj(CROSSM::POOL_HOUSEOBJSCENE);
		strcpy(HouseObjectScene->m_strOutName,strOutName);
		strcpy(HouseObjectScene->m_strMedName,strMedName);
		strcpy(HouseObjectScene->m_strInName,strInName);

		CHouseObject *pHouseObject = new CHouseObject();//(CHouseObject *)CSceneManager::_CreateObj(CROSSM::POOL_HOUSEOBJ);
			
//		char strFullName[256];
//		char strLightmap[256];
//		FILE *fp = NULL;

		// Lightmap 
		
		/*sprintf(strLightmap,"%s\\%s",LIGHTMAPTEXTUREPATH,strOutName);
		strLightmap[strlen(strLightmap)-1]='S';
		strLightmap[strlen(strLightmap)-2]='D';
		strLightmap[strlen(strLightmap)-3]='D';	
		fp=fopen(strLightmap,"rb");
		if(CRenderOption::m_BuildingLightmap)
		{
		//	fclose(fp);
		//	fp=NULL;
							
			sprintf(strLightmap,"%s",strOutName);
			strLightmap[strlen(strLightmap)-1]='S';
			strLightmap[strlen(strLightmap)-2]='D';
			strLightmap[strlen(strLightmap)-3]='D';

			CROSSM::CNTexture::SetPath(LIGHTMAPTEXTUREPATH);
			pHouseObject->LoadLightmap(strLightmap);
			
		}*/
/*		// InLightmap
		sprintf(strLightmap,"%s\\%s",LIGHTMAPTEXTUREPATH,strInName);
		strLightmap[strlen(strLightmap)-1]='S';
		strLightmap[strlen(strLightmap)-2]='D';
		strLightmap[strlen(strLightmap)-3]='D';	
		fp=fopen(strLightmap,"rb");
		if(fp && CRenderOption::m_BuildingLightmap)
		{
			fclose(fp);
			fp=NULL;
			sprintf(strLightmap,"%s",strInName);
			strLightmap[strlen(strLightmap)-1]='S';
			strLightmap[strlen(strLightmap)-2]='D';
			strLightmap[strlen(strLightmap)-3]='D';
			CROSSM::CNTexture::SetPath(LIGHTMAPTEXTUREPATH);
			pHouseObject->LoadInLightmap(strLightmap);
			
		}
	*/
		//

		/////////
	
		
		if(strstr(HouseObjectScene->m_strMedName,"_QWE"))		// 내외부 거리 컬링 x
		{
			HouseObjectScene->m_bMedCull = false;
		}
		if(strstr(HouseObjectScene->m_strOutName,"_MOD2X"))		// modulate 2x
		{
			pHouseObject->m_bModulate2X = true;
		}
		else
			pHouseObject->m_bModulate2X = false;


		HouseObjectScene->m_HouseSceneID=m_MapHouse->m_HouseSceneID[cHouse];
		HouseObjectScene->m_HouseObject=pHouseObject;		
		HouseObjectScene->m_pParent=this;
		HouseObjectScene->SetSkipFlag(true);	// 화면에 표시하지 않는다.

		//HouseObjectScene->m_AccumulateTM=m_AccumulateTM*HouseObjectScene->m_TM;
		HouseObjectScene->m_AccumulateTM=m_MapHouse->m_TM[cHouse];
		
		//pHouseObject->CalcBox(HouseObjectScene->m_vecMinBox,HouseObjectScene->m_vecMaxBox);
	
		//HouseObjectScene->Init();		
		m_HouseObjectNode.push_back(HouseObjectScene);
		DynamicLoad();
	}
}

void CSectorScene::GenerateHouseObject()
{	
	int iSize = (int)m_HouseObjectNode.size();
	for(int cHouse=0;cHouse<iSize;cHouse++)
	{
		CROSSM::SafeDelete(m_HouseObjectNode[cHouse]);
		//CSceneManager::_ReleaseObj(CROSSM::POOL_HOUSEOBJSCENE,m_HouseObjectNode[cHouse]);
		m_HouseObjectNode[cHouse] = NULL;
	}

	m_HouseObjectNode.clear();

	
	m_MapHouse=m_pMapStorage->FindHouseMap(m_TM._41,m_TM._43);	
	
	if(m_MapHouse)
	{
//		iSize = (int)m_MapHouse->m_iSize;//m_strOutName.size();
		iSize = (int)m_MapHouse->m_strOutName.size();

		if(m_pNotCullHouseIndex && (m_iNotCullHouseIndexCnt != iSize))
		{
			delete[] m_pNotCullHouseIndex;
			m_pNotCullHouseIndex = NULL;
		}
		m_iNotCullHouseIndexCnt = iSize;

		if(m_pNotCullHouseIndex == NULL && (iSize > 0))
		{
			m_pNotCullHouseIndex = new int[iSize];
		}
		m_iNotCullHouseIndex = 0;
		
		int cHouse;
	
		char *strOutName;
		char *strInName;
		char *strBspName;
		char *strMedName;

	
		FILE *fp = NULL;

		for(cHouse=0;cHouse<iSize;cHouse++)
		{
			strOutName = m_MapHouse->m_strOutName[cHouse];
			strMedName = m_MapHouse->m_strMedName[cHouse];
			strBspName = m_MapHouse->m_strBspName[cHouse];
			strInName = m_MapHouse->m_strInName[cHouse];
			
			CHouseObjectScene *HouseObjectScene= new CHouseObjectScene();//(CHouseObjectScene *)CSceneManager::_CreateObj(CROSSM::POOL_HOUSEOBJSCENE);
			strcpy(HouseObjectScene->m_strOutName,strOutName);
			strcpy(HouseObjectScene->m_strMedName,strMedName);
			strcpy(HouseObjectScene->m_strInName,strInName);			

			CHouseObject *pHouseObject = new CHouseObject();//(CHouseObject *)CSceneManager::_CreateObj(CROSSM::POOL_HOUSEOBJ);
			
			if(strstr(HouseObjectScene->m_strMedName,"_QWE"))		// 내외부 거리 컬링 x
			{
				HouseObjectScene->m_bMedCull = false;
			}
			if(strstr(HouseObjectScene->m_strOutName,"_MOD2X"))
			{
				pHouseObject->m_bModulate2X = true;
			}
			else
				pHouseObject->m_bModulate2X = false;

			HouseObjectScene->m_HouseSceneID=m_MapHouse->m_HouseSceneID[cHouse];

			HouseObjectScene->m_HouseObject=pHouseObject;
			HouseObjectScene->m_pParent=this;
			
			for(int iCheckHouse = 0; iCheckHouse < HOUSETYPENUM; iCheckHouse++)
			{
				
				if(!stricmp(HouseObjectScene->m_strOutName,strHouseName[iCheckHouse]))
				{
					HouseObjectScene->SetSkipFlag(true);
				}
			}
			//HouseObjectScene->m_AccumulateTM=m_AccumulateTM*HouseObjectScene->m_TM;
			HouseObjectScene->m_AccumulateTM=m_MapHouse->m_TM[cHouse];

		

			m_HouseObjectNode.push_back(HouseObjectScene);


		}	// for(cHouse=0;cHouse<m_MapHouse->m_strOutName.num;cHouse++)


	}
	

}

float CSectorScene::GenerateHeight()
{
	m_MapHeight=m_pMapStorage->FindHeightMap(m_TM._41,m_TM._43);	
	m_vecMinBox.x=0.0f;
	m_vecMinBox.z=0.0f;
	m_vecMaxBox.x=SECTORSIZE;
	m_vecMaxBox.z=SECTORSIZE;
	
	m_vecMaxBox.y=-10000000.0f;
	m_vecMinBox.y=10000000.0f;

	int ix,iy;
	m_fAvgHeight=0.0f;
	float fHeight;
	if(m_MapHeight==NULL)
	{		
		for(ix=0;ix<SECTORSX;ix++)
		{
			for(iy=0;iy<SECTORSX;iy++)
			{
				//fHeight=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));												
				fHeight=500.0f;
				m_HeightData[ix+iy*SECTORSX]=fHeight;
				m_SectorVertex[ix+iy*SECTORSX].v.y=fHeight;
				m_fAvgHeight+=fHeight;
				if(fHeight<m_vecMinBox.y)
					m_vecMinBox.y=fHeight;
				if(fHeight>m_vecMaxBox.y)
					m_vecMaxBox.y=fHeight;
			}
		}	
	}
	else
	{
		for(ix=0;ix<SECTORSX;ix++)
		{
			for(iy=0;iy<SECTORSX;iy++)
			{
				fHeight=m_MapHeight->m_pHeightData[ix+iy*SECTORSX];
				m_HeightData[ix+iy*SECTORSX]=fHeight;
				m_SectorVertex[ix+iy*SECTORSX].v.y=fHeight;
				m_fAvgHeight+=fHeight;
				if(fHeight<m_vecMinBox.y)
					m_vecMinBox.y=fHeight;
				if(fHeight>m_vecMaxBox.y)
					m_vecMaxBox.y=fHeight;				
			}
		}
	}
	CalcNormal();
	m_fAvgHeight/=(SECTORSX*SECTORSY);	
	m_BaseLeft.RightNeighbor = m_BaseLeft.LeftNeighbor = m_BaseRight.RightNeighbor = m_BaseRight.LeftNeighbor =
	m_BaseLeft.LeftChild = m_BaseLeft.RightChild = m_BaseRight.LeftChild = m_BaseLeft.LeftChild = NULL;
	m_BaseLeft.BaseNeighbor = &m_BaseRight;
	m_BaseRight.BaseNeighbor = &m_BaseLeft;
	ComputeVariance();

	if(m_MapHeight && m_MapHeight->m_FilePos!=0)
	{
		delete [] m_MapHeight->m_pHeightData;
		m_MapHeight->m_pHeightData=NULL;
	}
	return m_fAvgHeight;
}

void CSectorScene::GenerateWideMap()
{
	if(m_MapWide!=NULL)
	{
		m_MapTexture=NULL;
		m_MapWide->DelWideTexture();
	}		
	m_MapWide=m_pMapStorage->FindWideMap(m_TM._41,m_TM._43);
	if(m_MapWide)
	{
		m_MapWide->GetWideTexture();
	}
		//m_MapTexture=m_MapWide->GetWideTexture();		
	else
		DeterminTerrainTexture(m_fAvgHeight,m_TM._41,m_TM._43);		
}
void CSectorScene::RenderNotCullHouseObject(LPDIRECT3DDEVICE8 pd3dDevice) 
{

	int i;

	if((m_pNotCullHouseIndex == NULL) || (m_iNotCullHouseIndex <= 0) || (m_HouseObjectNode.size() <= 0))
	{
		return;

	}

	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	for( int iCount = 0; iCount < m_iNotCullHouseIndex; iCount++)
	{	
	
		i = m_pNotCullHouseIndex[iCount];

		if(CSceneManager::m_PickHouseScene!=m_HouseObjectNode[i])
		{		
		
			m_HouseObjectNode[i]->Render(pd3dDevice);
		}
		else
		{			
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_HouseObjectNode[i]->m_AccumulateTM));
			
			if(m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject)
				m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject->RenderBox(pd3dDevice);			
			else
			{
			
			}


			if(CSceneManager::m_bIsPickObjectMoving!=MOVE_HOUSE) {
			
				m_HouseObjectNode[i]->Render(pd3dDevice);		
				
			}
		}
	}



}
void CSectorScene::RenderEnvHouseObject(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	float fFar = CSceneManager::m_WeatherManager.m_InterFogFar + 200.0f;
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	D3DXVECTOR3 vecDist;
	float fDist;
	if(fFar > 48000.0f)
		fFar = 48000.0f;

	for(int i=0;i<(int)m_HouseObjectNode.size();i++)
	{			
		if(CSceneManager::m_PickHouseScene!=m_HouseObjectNode[i])
		{				
			
			vecDist.x = vecViewPos->x - m_HouseObjectNode[i]->m_AccumulateTM._41;
			vecDist.y = vecViewPos->y - m_HouseObjectNode[i]->m_AccumulateTM._42;
			vecDist.z = vecViewPos->z - m_HouseObjectNode[i]->m_AccumulateTM._43;
			fDist = D3DXVec3Length(&vecDist);
			if(fDist > fFar)
				continue;
			m_HouseObjectNode[i]->Render(pd3dDevice);
		}
		else
		{			
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_HouseObjectNode[i]->m_AccumulateTM));
			if(m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject)
				m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject->RenderBox(pd3dDevice);			
			else
			{
			
			}


//			if(CSceneManager::m_bIsPickObjectMoving!=MOVE_HOUSE)
			m_HouseObjectNode[i]->Render(pd3dDevice);							
		}
	}
}

void CSectorScene::RenderHouseObject(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	
	float fFar = CSceneManager::m_WeatherManager.m_InterFogFar + 3000.0f;
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	D3DXVECTOR3 vecDist;
	float fDist;
	
	D3DXMATRIX matTm;
	D3DXVECTOR3 vecMaxBox,vecMinBox;

	D3DXVECTOR4 vecTrans;
	int iMaxX,iMaxY;
	int iMinX,iMinY;

	for(int i=0;i<(int)m_HouseObjectNode.size();i++)
	{
		if(!m_HouseObjectNode[i]->m_HouseObject || !m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject)
		{
			continue;
		}

		if(!m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject->IsLoaded())
			continue;
		if(!m_HouseObjectNode[i]->m_bInit)
		{
			
		
			m_HouseObjectNode[i]->m_HouseObject->CalcBox(m_HouseObjectNode[i]->m_vecMinBox,m_HouseObjectNode[i]->m_vecMaxBox);

			m_HouseObjectNode[i]->Init();
			//Sector 경계 넘는 House 처리
			matTm = D3DXMATRIX((float *)m_HouseObjectNode[i]->m_AccumulateTM.m);

			vecMaxBox = D3DXVECTOR3(m_HouseObjectNode[i]->m_vecMaxBox.x,
												m_HouseObjectNode[i]->m_vecMaxBox.y,
												m_HouseObjectNode[i]->m_vecMaxBox.z);

			vecMinBox = D3DXVECTOR3(m_HouseObjectNode[i]->m_vecMinBox.x,
												m_HouseObjectNode[i]->m_vecMinBox.y,
												m_HouseObjectNode[i]->m_vecMinBox.z);

			D3DXVec3Transform(&vecTrans,&vecMaxBox,&matTm);
			iMaxX = (int)(vecTrans.x / SECTORSIZE);
			iMaxY = (int)(vecTrans.z / SECTORSIZE);

			D3DXVec3Transform(&vecTrans,&vecMinBox,&matTm);
			iMinX = (int)(vecTrans.x / SECTORSIZE);
			iMinY = (int)(vecTrans.z / SECTORSIZE);

			if((iMaxX != iMinX) || ( iMaxY != iMinY))
			{
				if( m_pNotCullHouseIndex )
				{
					m_pNotCullHouseIndex[m_iNotCullHouseIndex++] = i;
					m_HouseObjectNode[i]->m_bNotCull = true;
					// 카나번 관련 패치
					if(m_HouseObjectNode[i]->m_HouseObject->m_pOctree == NULL)
					{
						if(strcmp(m_HouseObjectNode[i]->m_strOutName,"")!=0)
						{
							// HouseObjectScene의 m_strOutName 을 복사해 확장자를 oct 로 바꾼 뒤
							// 그 아래는 octree conatiner 로부터 로딩해오는 코드가 있었으나..
							// 왠일인지 모두 주석처리되었심
						}
					}
				}
			}
			else
			{
				m_HouseObjectNode[i]->m_bNotCull = false;
			}

			// 섹터 바운딩박스의 높이에 섹터에 포함된 하우스의 높이도 포함해 계산
			m_vecMaxBox.y = max(m_vecMaxBox.y, m_HouseObjectNode[i]->m_vecMaxBox.y + m_HouseObjectNode[i]->m_AccumulateTM._42);

			m_HouseObjectNode[i]->m_bInit = true;
		}
		if(CSceneManager::m_PickHouseScene!=m_HouseObjectNode[i])
		{
			vecDist.x = vecViewPos->x - m_HouseObjectNode[i]->m_AccumulateTM._41;
			vecDist.y = vecViewPos->y - m_HouseObjectNode[i]->m_AccumulateTM._42;
			vecDist.z = vecViewPos->z - m_HouseObjectNode[i]->m_AccumulateTM._43;
			fDist = D3DXVec3Length(&vecDist);
			if(CSceneManager::m_HeightField.GetHouseCullingFlag())
			{
				if(fDist > fFar)
					continue;
			}

			m_HouseObjectNode[i]->Render(pd3dDevice);
		}
		else
		{			
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_HouseObjectNode[i]->m_AccumulateTM));
			if(m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject)
				m_HouseObjectNode[i]->m_HouseObject->m_pOutHouseObject->RenderBox(pd3dDevice);			
			else
			{
				
			}


			if(CSceneManager::m_bIsPickObjectMoving!=MOVE_HOUSE)
				m_HouseObjectNode[i]->Render(pd3dDevice);							
		}
	}
}

void CSectorScene::GeneratePlant()
{	
	m_MapPlant=m_pMapStorage->FindPlantMap(m_AccumulateTM._41,m_AccumulateTM._43);
	if(m_MapPlant)
	{
		m_TreeObjectNode.GeneratePlant(m_MapPlant,m_HeightData);
	}
	else
	{
		m_TreeObjectNode.GeneratePlant(m_AccumulateTM._41,m_AccumulateTM._43,m_HeightData,m_fAvgHeight);
	}
	m_TreeObjectNode.m_AccumulateTM=m_AccumulateTM;

	if(CRenderOption::m_RangeGrassRender)
	{
		if(m_pLODGrassAlpha)
		{
			delete m_pLODGrassAlpha;
			m_pLODGrassAlpha=NULL;
		}

		char strTextureFileName[256];
		sprintf(strTextureFileName,"%s\\GrassTexture%d_%d.dds",WIDETEXTUREPATH,(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));
		FILE *fp=fopen(strTextureFileName,"rb");
		if(fp)
		{
			m_pLODGrassAlpha=new CTexture();
			CTexture::SetPath(WIDETEXTUREPATH);
			sprintf(strTextureFileName,"GrassTexture%d_%d.dds",(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));
			m_pLODGrassAlpha->Load(strTextureFileName);
			fclose(fp);
		}
		else
		{
			m_pLODGrassAlpha=NULL;
		}
	}
	//m_GrassObjectNode.GeneratePlant(m_AccumulateTM._41,m_AccumulateTM._43,m_HeightData,m_fAvgHeight,m_AccumulateTM);
}
void CSectorScene::RenderEnvPlant(LPDIRECT3DDEVICE8	pd3dDevice)
{
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	pd3dDevice->SetTexture(0,m_NormalPlantTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);
	m_TreeObjectNode.RenderEnv(pd3dDevice);
}
void CSectorScene::RenderPlant(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	pd3dDevice->SetTexture(0,m_NormalPlantTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);
	m_TreeObjectNode.Render(pd3dDevice);
}

//DEL void CSectorScene::RenderLayerFog(LPDIRECT3DDEVICE8 pd3dDevice)
//DEL {		
//DEL 	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//DEL     pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
//DEL     pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
//DEL 	
//DEL 	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//DEL 	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSMOOTH );	
//DEL 	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );	
//DEL 	
//DEL 	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
//DEL 	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);	
//DEL 	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
//DEL 	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
//DEL 	
//DEL 	
//DEL 	
//DEL 	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
//DEL 	vector3 vecViewPos;
//DEL 	vecViewPos.x=matViewPosition->_41;
//DEL 	vecViewPos.y=matViewPosition->_42;
//DEL 	vecViewPos.z=matViewPosition->_43;				
//DEL 
//DEL 
//DEL 	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
//DEL 
//DEL 	float fHeightInter=fFogMax-fFogMin;
//DEL 	float fFogValue[4]={1.0f/fFogXRange,1.0f/fHeightInter,m_vecMinBox.y,0.0f};	
//DEL 	float fZeroOne[4]={0.0f,1.0f};
//DEL 
//DEL 	matrix matView,matProj;
//DEL 	pd3dDevice->GetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
//DEL 	pd3dDevice->GetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProj);
//DEL 	matrix matSet=m_AccumulateTM*matView*matProj;
//DEL 
//DEL 	D3DXMATRIX mat;    
//DEL     D3DXMatrixTranspose( &mat, (D3DXMATRIX*)&matSet );
//DEL 	D3DXMATRIX matTemp;
//DEL 	D3DXMatrixTranspose( &matTemp, (D3DXMATRIX*)&m_AccumulateTM);
//DEL 	///*
//DEL 	pd3dDevice->SetVertexShaderConstant(0,&mat,4);	
//DEL 	pd3dDevice->SetVertexShaderConstant(4,&matTemp,4);
//DEL 	pd3dDevice->SetVertexShaderConstant(8,&vecViewPos,1);
//DEL 	pd3dDevice->SetVertexShaderConstant(9,fFogValue,1);
//DEL 
//DEL 	pd3dDevice->SetTexture(0,m_MapTexture);
//DEL 	pd3dDevice->SetTexture(1,m_LayerFogScene.m_FogTexture.GetTexture());	
//DEL 	/*
//DEL 	pd3dDevice->SetVertexShader(m_dwTerrainLayerFogVertexShader);
//DEL 	pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
//DEL 	pd3dDevice->SetIndices(m_pROAMIndices,0);
//DEL 	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);		
//DEL 	//*/
//DEL 	
//DEL 	pd3dDevice->SetTexture(0,m_MapTexture);
//DEL 	pd3dDevice->SetTexture(1,m_LayerFogScene.m_FogTexture.GetTexture());		
//DEL 	SectorVertex *m_pVertex;
//DEL 	vector3 vecViewVertexLens;			
//DEL 	float fViewVertexLens;	
//DEL 	
//DEL 	vecViewPos.x-=m_AccumulateTM._41;
//DEL 	vecViewPos.y-=m_AccumulateTM._42;
//DEL 	vecViewPos.z-=m_AccumulateTM._43;
//DEL 	m_pROAMVertex->Lock(0,0,(BYTE**)&m_pVertex,0);	
//DEL 	for(int i=0;i<m_UsedVertex;i++)
//DEL 	{
//DEL 		vecViewVertexLens=vecViewPos-m_pVertex[i].v;
//DEL 		vecViewVertexLens.y=0.0f;
//DEL 		fViewVertexLens=vecViewVertexLens.GetLens();		
//DEL 		m_pVertex[i].tu1=fViewVertexLens/fFogXRange;		
//DEL 		m_pVertex[i].tv1=(m_pVertex[i].v.y-fFogMin)/fHeightInter;		
//DEL 		
//DEL 		if(m_pVertex[i].tu1>1.0f)
//DEL 		{
//DEL 			m_pVertex[i].tu1=1.f;
//DEL 		}	
//DEL 		
//DEL 		if(m_pVertex[i].tv1>1.f)
//DEL 		{
//DEL 			m_pVertex[i].tv1=1.f;
//DEL 		}		
//DEL 		/*		
//DEL 		if(fViewVertexLens>33000.0f)
//DEL 		{
//DEL 			m_pVertex[i].tu1=0.95f;
//DEL 			m_pVertex[i].tv1=(m_pVertex[i].v.y-m_vecMinBox.y)/fHeightInter;
//DEL 		}	
//DEL 		else
//DEL 		{
//DEL 			m_pVertex[i].tu1=fViewVertexLens/fFogXRange;
//DEL 			m_pVertex[i].tv1=(m_pVertex[i].v.y-m_vecMinBox.y)/fHeightInter;
//DEL 		}
//DEL 		*/
//DEL 		
//DEL 				
//DEL 	}
//DEL 	m_pROAMVertex->Unlock();	
//DEL 	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
//DEL 	pd3dDevice->SetVertexShader(SectorFVF);	
//DEL 	pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
//DEL 	pd3dDevice->SetIndices(m_pROAMIndices,0);
//DEL 	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);		
//DEL 	
//DEL 	
//DEL 	pd3dDevice->SetTexture(0,m_LayerFogScene.m_FogTexture.GetTexture());	
//DEL 	pd3dDevice->SetTexture(1,NULL);	
//DEL 		
//DEL 	
//DEL 	LPDIRECT3DVERTEXBUFFER8 pFogVB;
//DEL 	pd3dDevice->CreateVertexBuffer(4*sizeof(TLVertex),D3DUSAGE_WRITEONLY, TLVERTEXFVF, 
//DEL                                    D3DPOOL_MANAGED,&pFogVB);
//DEL 	TLVertex* v;
//DEL 	float sx=300.0f;
//DEL 	float sy=300.0f;
//DEL 
//DEL 	pFogVB->Lock(0,0,(BYTE**)&v,0);
//DEL 
//DEL 	v[0].v=vector3(0.0f,sy,0);v[0].w=1.0f;
//DEL 	v[0].tu=0.0f;
//DEL 	v[0].tv=1.0f;
//DEL 	v[0].Diffuse.c=0xffffffff;
//DEL 	v[0].Specular.c=0x0;
//DEL 
//DEL 	v[1].v=vector3(0.0f,0.0f,0);v[1].w=1.0f;
//DEL 	v[1].tu=0.0f;
//DEL 	v[1].tv=0.0f;
//DEL 	v[1].Diffuse.c=0xffffffff;
//DEL 	v[1].Specular.c=0x0;
//DEL 
//DEL 	v[2].v=vector3(sx,sy,0);v[2].w=1.0f;
//DEL 	v[2].tu=1.0f;
//DEL 	v[2].tv=1.0f;
//DEL 	v[2].Diffuse.c=0xffffffff;
//DEL 	v[2].Specular.c=0x0;
//DEL 
//DEL 	v[3].v=vector3(sx,0.0f,0);v[3].w=1.0f;
//DEL 	v[3].tu=1.0f;
//DEL 	v[3].tv=0.0f;
//DEL 	v[3].Diffuse.c=0xffffffff;
//DEL 	v[3].Specular.c=0x0;
//DEL 
//DEL 	pFogVB->Unlock();
//DEL 	//pd3dDevice->SetTexture(0,NULL);
//DEL 	//pd3dDevice->SetTexture(1,NULL);
//DEL 	pd3dDevice->SetStreamSource(0, pFogVB, sizeof(TLVertex));
//DEL     pd3dDevice->SetVertexShader(TLVERTEXFVF);
//DEL     pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );
//DEL 	pFogVB->Release();
//DEL     // Restore render states	pFogVB->Release();
//DEL 	/*
//DEL 	if( FAILED( m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex), D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, 
//DEL                                                   D3DPOOL_MANAGED, &m_pFogMapVB ) ) )
//DEL         return E_FAIL;
//DEL 	// Create a quad for the final pass
//DEL     TLVertex* v;
//DEL     FLOAT sx = 100.0f;
//DEL     FLOAT sy = 100.0f;
//DEL     m_pFogMapVB->Lock( 0, 0, (BYTE**)&v, 0 );
//DEL 
//DEL     v[0].Position = D3DXVECTOR4(0, sy, 0, 1);
//DEL 	v[0].Diffuse = 0x0;
//DEL 	v[0].Specular = 0;
//DEL 	v[0].Texture = D3DXVECTOR2(1, 1);
//DEL 
//DEL     v[1].Position = D3DXVECTOR4(0, 0, 0, 1);
//DEL 	v[1].Diffuse = 0x0;
//DEL 	v[1].Specular = 0;
//DEL 	v[1].Texture = D3DXVECTOR2(1, 0);
//DEL     
//DEL 	v[2].Position = D3DXVECTOR4(sx, sy, 0, 1);
//DEL 	v[2].Diffuse = 0x0;
//DEL 	v[2].Specular = 0;
//DEL 	v[2].Texture = D3DXVECTOR2(0, 1);
//DEL 
//DEL 	v[3].Position = D3DXVECTOR4(sx, 0, 0, 1);
//DEL 	v[3].Diffuse = 0x0;
//DEL 	v[3].Specular = 0;
//DEL 	v[3].Texture = D3DXVECTOR2(0, 0);
//DEL 	*/
//DEL }

void CSectorScene::CalcNormal()
{		
	/*
	CSectorHeightMap *pXNeighborMapHeight,*pYNeighborMapHeight;
	pXNeighborMapHeight=m_pMapStorage->FindHeightMap(m_TM._41+SECTORSIZE,m_TM._43);
	pYNeighborMapHeight=m_pMapStorage->FindHeightMap(m_TM._41,m_TM._43+SECTORSIZE);
	
	vector3 n;
	int ix,iy;
	long a,b,c;
	vector3 *v1,*v2,*v3;
	vector3 tempv2,tempv3;
	for(ix=0;ix<SECTORSX;ix++)
	{
		for(iy=0;iy<SECTORSX;iy++)
		{   
			a=ix+iy*SECTORSX;
			b=ix+(iy+1)*SECTORSX;
			c=ix+1+iy*SECTORSX;			
			
			if(ix!=SECTORSX-1 && iy!=SECTORSX-1)
			{
				v1 =&m_SectorVertex[a].v;
				v2 =&m_SectorVertex[b].v;
				v3 =&m_SectorVertex[c].v;

				n = (*v2-*v1)^(*v3-*v2);
				n.y*=0.2f;
				n.Normalize();				
			}
			if(ix==SECTORSX-1 && iy==SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				tempv2.x=v1->x;
				tempv2.z=SECTORSIZE+LINTERVAL;
				tempv3.x=SECTORSIZE+LINTERVAL;
				tempv3.z=v1->z;

				if(pXNeighborMapHeight)
					tempv2.y=pXNeighborMapHeight->m_pHeightData[1+iy*SECTORSX];
				else
					tempv2.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+SECTORSIZE+LINTERVAL,0.0f));


				if(pYNeighborMapHeight)
					tempv3.y=pYNeighborMapHeight->m_pHeightData[ix+1*SECTORSX];
				else
					tempv3.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+SECTORSIZE+LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));

			}
			if(ix==SECTORSX-1 && iy!=SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				v2=&m_SectorVertex[b].v;
				tempv3.x=SECTORSIZE+LINTERVAL;
				tempv3.z=v1->z;
				if(pXNeighborMapHeight)
					tempv3.y=pXNeighborMapHeight->m_pHeightData[1+iy*SECTORSX];
				else
					tempv3.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+SECTORSIZE+LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));

				n = (*v2-*v1)^(tempv3-*v2);
				n.y*=0.2f;
				n.Normalize();
			}
			if(ix!=SECTORSX-1 && iy==SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				tempv2.x=v1->x;
				tempv2.z=SECTORSIZE+LINTERVAL;				
				v3=&m_SectorVertex[c].v;
				
				if(pYNeighborMapHeight)
					tempv2.y=pYNeighborMapHeight->m_pHeightData[ix+1*SECTORSX];
				else
					tempv2.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+SECTORSIZE+LINTERVAL,0.0f));

				n = (tempv2-*v1)^(*v3-tempv2);
				n.y*=0.2f;
				n.Normalize();
			}	
			//m_SectorVertex[a].n=n;
			
		}
    }	
	*/
	
	/*
	m_MapHeight=CMapStorage::FindHeightMap(m_TM._41,m_TM._43);
	m_vecMinBox.x=0.0f;
	m_vecMinBox.z=0.0f;
	m_vecMaxBox.x=SECTORSIZE;
	m_vecMaxBox.z=SECTORSIZE;

	
	m_vecMaxBox.y=-10000000.0f;
	m_vecMinBox.y=10000000.0f;

	int ix,iy;
	m_fAvgHeight=0.0f;
	float fHeight;	
	fHeight=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));
	*/
	/*
	vector3 *pNormals=new vector3[SECTORSX*SECTORSY];	
	ZeroMemory(pNormals,sizeof(vector3)*SECTORSX*SECTORSY);	
	vector3 n1,n2;
	int ix,iy;
	long a,b,c,d,e,f;
	vector3 *v1,*v2,*v3,*v4,*v5,*v6;
	for(ix=1;ix<SECTORSX-2;ix++)
	{
		for(iy=1;iy<SECTORSX-2;iy++)
		{    
			a=ix+iy*SECTORSX;
			b=ix+(iy+1)*SECTORSX;
			c=ix+1+iy*SECTORSX;
			d=ix+1+iy*SECTORSX;
			e=ix+(iy+1)*SECTORSX;
			f=ix+1+(iy+1)*SECTORSX;

			v1 =&m_SectorVertex[a].v;
			v2 =&m_SectorVertex[b].v;
			v3 =&m_SectorVertex[c].v;		

			n1 = (*v2-*v1)^(*v3-*v2);
			n1.y*=0.2f;
			n1.Normalize();

			pNormals[a] += n1;
			pNormals[b] += n1;
			pNormals[c] += n1;

			v4 =&m_SectorVertex[d].v;
			v5 =&m_SectorVertex[e].v;
			v6 =&m_SectorVertex[f].v;

			n2 = (*v5-*v4)^(*v6-*v5);
			n2.y*=0.2f;			
			n2.Normalize();

			pNormals[d] += n2;
			pNormals[e] += n2;
			pNormals[f] += n2;
		}
    }
	
	for(int i=0;i<SECTORSX*SECTORSY;i++)
	{		
		if(pNormals[i].GetLens() < 0.1f)
			pNormals[i]=vector3(0.0f,1.0f,0.0f);
		pNormals[i].Normalize();	
		m_SectorVertex[i].n=pNormals[i];        
	}	
	delete [] pNormals;	
	*/
}	

void CSectorScene::GenerateWater()
{
	m_MapWater=m_pMapStorage->FindWaterMap(m_TM._41,m_TM._43);		
}

void CSectorScene::RenderWater(LPDIRECT3DDEVICE8 pd3dDevice,bool bFlare)
{
	if(m_MapWater)
	{
		matrix matWaterTM;		
		matWaterTM.Translation(vector3(m_AccumulateTM._41,m_MapWater->m_fWaterHeight,m_AccumulateTM._43));
		m_WaterScene.m_WaterVertex[0].diff=m_MapWater->m_WaterColor;
		m_WaterScene.m_WaterVertex[1].diff=m_MapWater->m_WaterColor;
		m_WaterScene.m_WaterVertex[2].diff=m_MapWater->m_WaterColor;
		m_WaterScene.m_WaterVertex[3].diff=m_MapWater->m_WaterColor;

		m_WaterScene.m_WaterVertex[0].v.x=m_MapWater->m_fWaterPosX;
		m_WaterScene.m_WaterVertex[0].v.z=m_MapWater->m_fWaterPosY;

		m_WaterScene.m_WaterVertex[1].v.x=m_MapWater->m_fWaterPosX+
										  m_MapWater->m_fWaterSizeX;
		m_WaterScene.m_WaterVertex[1].v.z=m_MapWater->m_fWaterPosY;

		m_WaterScene.m_WaterVertex[2].v.x=m_MapWater->m_fWaterPosX;
		m_WaterScene.m_WaterVertex[2].v.z=m_MapWater->m_fWaterPosY+
										  m_MapWater->m_fWaterSizeY;

		m_WaterScene.m_WaterVertex[3].v.x=m_MapWater->m_fWaterPosX+
										  m_MapWater->m_fWaterSizeX;
		m_WaterScene.m_WaterVertex[3].v.z=m_MapWater->m_fWaterPosY+
										  m_MapWater->m_fWaterSizeY;
		
		pd3dDevice->SetTransform(D3DTS_WORLD,matWaterTM);
		if(bFlare)
			m_WaterScene.RenderFlare(pd3dDevice);
		else
			m_WaterScene.Render(pd3dDevice);
	}
}

void CSectorScene::GenerateFall()
{
	m_MapFall=m_pMapStorage->FindFallMap(m_AccumulateTM._41,m_AccumulateTM._43);
	if(m_MapFall)
		m_FallScene.MakeFallScene(m_MapFall);
}

void CSectorScene::RenderFall(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(m_MapFall)
	{
		matrix matFallRot,matFallPos;
		matFallPos.Translation(vector3(m_MapFall->m_fFallPosX,0.0f,m_MapFall->m_fFallPosY));
		matFallRot.YRotation(m_MapFall->m_fFallRot);		
		matFallPos=matFallPos*m_AccumulateTM;
		matrix matFallTM=matFallRot*matFallPos;
		pd3dDevice->SetTransform(D3DTS_WORLD,matFallTM);
		m_FallScene.Render(pd3dDevice);
	}
}

void CSectorScene::GenerateObject()
{
	m_MapMesh=m_pMapStorage->FindMeshMap(m_TM._41,m_TM._43);
	
	int iSize = (int)m_ObjectSceneNode.size();
	for(int cObject=0;cObject<iSize;cObject++)
	{
		CROSSM::SafeDelete(m_ObjectSceneNode[cObject]);
//		CSceneManager::_ReleaseObj(CROSSM::POOL_OBJECTSCENE, m_ObjectSceneNode[cObject]);		
	}
	m_ObjectSceneNode.clear();
	
	if(m_MapMesh)
	{
		iSize = (int)m_MapMesh->m_ObjectSceneID.size();
		for(int cObject=0;cObject<iSize;cObject++)
		{
			CObjectScene *AddObjectScene= new CObjectScene();//(CObjectScene *)CSceneManager::_CreateObj(CROSSM::POOL_OBJECTSCENE);
			AddObjectScene->m_TM=m_MapMesh->m_TM[cObject];
			AddObjectScene->m_AccumulateTM=m_MapMesh->m_TM[cObject];			
			AddObjectScene->m_pObject=NULL;
			AddObjectScene->m_bInit = false;

			strcpy(AddObjectScene->m_strObjectName,m_MapMesh->m_strObjectName[cObject]);
			AddObjectScene->m_isAlpha=m_MapMesh->m_isAlpha[cObject];
			AddObjectScene->m_isLight=m_MapMesh->m_isLight[cObject];
			AddObjectScene->m_isShadowed=false;
			AddObjectScene->m_ObjectID=m_MapMesh->m_ObjectSceneID[cObject];
			//AddObjectScene->Init();
			
			m_ObjectSceneNode.push_back(AddObjectScene);
		}
	}	
}

void CSectorScene::RenderObject(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();

	vector3 vecViewPos=matViewPosition->GetLoc();
	vector3 vecViewPosInter;

	vecViewPosInter;
	float fViewInter;

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{	

		if(m_ObjectSceneNode[cObject]->m_pObject==NULL || !m_ObjectSceneNode[cObject]->m_pObject->IsLoaded())
			continue;	
		if(!m_ObjectSceneNode[cObject]->m_bInit)
		{	//초기화
			m_ObjectSceneNode[cObject]->m_vecMaxBox=m_ObjectSceneNode[cObject]->m_pObject->m_MaxBox;
			m_ObjectSceneNode[cObject]->m_vecMinBox=m_ObjectSceneNode[cObject]->m_pObject->m_MinBox;			
			m_ObjectSceneNode[cObject]->Init();

			m_ObjectSceneNode[cObject]->m_bInit = true;

		}
		if(m_ObjectSceneNode[cObject]->isCulling()==false)
			continue;
		
		pd3dDevice->SetTransform(D3DTS_WORLD,m_ObjectSceneNode[cObject]->m_TM);
	
	
		if(CSceneManager::m_PickObjectScene==m_ObjectSceneNode[cObject])
		{
			if(CSceneManager::m_bIsPickObjectMoving==MOVE_OBJECT )
				m_ObjectSceneNode[cObject]->m_pObject->RenderBox(pd3dDevice);
			else
			{
				m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);
				m_ObjectSceneNode[cObject]->m_pObject->RenderBox(pd3dDevice);
			}
		}		
		else
		{
			
			////////////

			// character Light 영향 유무
			if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
			{
			

				D3DXVECTOR3 vecPos = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x,
													CSceneManager::m_CharacterLight.Position.y,
													CSceneManager::m_CharacterLight.Position.z);
				
				D3DXVECTOR3 vecObjCenter = D3DXVECTOR3(m_ObjectSceneNode[cObject]->m_TM._41,
														m_ObjectSceneNode[cObject]->m_TM._42,
														m_ObjectSceneNode[cObject]->m_TM._43);
				if(D3DXVec3Length(&(vecPos - vecObjCenter)) <= (CSceneManager::m_CharacterLight.Range + 100.0f))
				{
					pd3dDevice->SetLight(1,&(CSceneManager::m_CharacterLight));
					pd3dDevice->LightEnable(1,TRUE);
			
		
				}					
			}

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xa0);
			
			//////////// Visible
			
			vecViewPosInter=vecViewPos-m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc();
			fViewInter=vecViewPosInter.GetLens();
			
			if((MAX_OBJECTVIEWRANBE > fViewInter) && (fViewInter > (MAX_OBJECTVIEWRANBE - (MAX_OBJECTVIEWRANBE * 0.1f))))
			{
				float fValue = (float)(MAX_OBJECTVIEWRANBE - (MAX_OBJECTVIEWRANBE * 0.9f));
				float fAlpha = (float)(MAX_OBJECTVIEWRANBE - fViewInter) / fValue;
				if(fAlpha > 1.0f)
					fAlpha = 1.0f;
				else if(fAlpha < 0.001f) {
					fAlpha = 0.0f;
					continue;
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
				DWORD dwAlphaFactor = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,fAlpha);	
				pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,dwAlphaFactor);
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

			}
			else if(MAX_OBJECTVIEWRANBE > fViewInter) {
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

			}
			else {
				continue;
			}
		
			//////////////
			m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);				
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		
			
		}
	
	
	}
	
}

float CSectorScene::GetHeight(vector3 vecPos)
{		
	vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();		

	long VertexStartX,VertexStartY,VertexEndX,VertexEndY,VertexCenterX,VertexCenterY;
	VertexStartX=0;
	VertexStartY=0;
	VertexEndX=SECTORSX-1;
	VertexEndY=SECTORSY-1;

	int nFindVertex[4];
	
	while(1)
	{
		VertexCenterX=(VertexStartX+VertexEndX)>>1;
		VertexCenterY=(VertexStartY+VertexEndY)>>1;

		if(VertexEndX-VertexStartX==1 || VertexEndY-VertexStartY==1)
		{
			nFindVertex[0]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexStartX];
			nFindVertex[1]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexEndX];
			nFindVertex[2]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexStartX];
			nFindVertex[3]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexEndX];	
			break;
		}
		
		if(m_RenderUsedVertex[VertexCenterX+VertexCenterY*SECTORSX]!=-1)
		{
			if(VertexCenterY*LINTERVAL<vecSectorPos.z)
			{
				if(VertexCenterX*LINTERVAL<vecSectorPos.x)
				{
					//0VertexEndX=VertexEndX;	VertexEndY=VertexEndY;
					VertexStartX=VertexCenterX;
					VertexStartY=VertexCenterY;					
				}
				else
				{
					//1//VertexStartX=VertexStartX;//VertexEndY=VertexEndY;										
					VertexStartY=VertexCenterY;
					VertexEndX=VertexCenterX;					
				}
			}
			else
			{
				if(VertexCenterX*LINTERVAL<vecSectorPos.x)
				{
					//2	//VertexStartY=VertexStartY;//VertexEndX=VertexEndX;
					VertexStartX=VertexCenterX;					
					VertexEndY=VertexCenterY;
				}
				else
				{
					//3	VertexStartX=VertexStartX;VertexStartY=VertexStartY;
					VertexEndX=VertexCenterX;
					VertexEndY=VertexCenterY;
				}
			}			
		}	
		else
		{
			nFindVertex[0]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexStartX];
			nFindVertex[1]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexEndX];
			nFindVertex[2]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexStartX];
			nFindVertex[3]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexEndX];	
			break;
		}
	}

	int cIndices=0;		
	std::vector<vector3> vecPolyList;
	
	for(cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
	{
		if( m_pDrawIndices[cIndices*3+0]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[3] ||
			
			m_pDrawIndices[cIndices*3+1]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[3] ||

			m_pDrawIndices[cIndices*3+2]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[3])				
		{
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
		}
	}

	vector3 v[3];
	float fLens=0.0f;	
	for(int cPoly=0;cPoly<(int)vecPolyList.size()/3;cPoly++)
	{
		v[0]=vecPolyList[cPoly*3+0];
		v[1]=vecPolyList[cPoly*3+1];
		v[2]=vecPolyList[cPoly*3+2];
		if(CIntersection::PolygonRay(vector3(vecPos.x,0.0f,vecPos.z),vector3(vecPos.x,120000.0f,vecPos.z),v,fLens))
		{
			return fLens;
		}
	}
	
	return fLens;
	/*
	vector3 v[3];
	float fLens;

	vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();	
	long ShadowPosX,ShadowPosY;
	ShadowPosX=vecSectorPos.x/LINTERVAL;
	ShadowPosY=vecSectorPos.z/LINTERVAL;

	if( m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)]==-1)
	{
		float fRealX=vecPos.x-m_AccumulateTM._41;
		float fRealZ=vecPos.z-m_AccumulateTM._43;

		int PosX=fRealX/LINTERVAL;
		int PosZ=fRealZ/LINTERVAL;
		
		//float fChecky=((PosZ+1)*LINTERVAL)+(PosX*LINTERVAL);
		float fChecky=(PosZ)*LINTERVAL - (PosX)*LINTERVAL;
		
		vector3 *pPoly[3];
		//vector3 v[3];
		//float fLens;
		if(fRealZ >=fRealX + fChecky)
		{
			pPoly[0]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+0) * SECTORSX];
			pPoly[1]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+1) * SECTORSX];
			pPoly[2]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+1) * SECTORSX];
			v[0]=*pPoly[0];
			v[1]=*pPoly[1];
			v[2]=*pPoly[2];
			CIntersection::PolygonRay(vector3(fRealX,0.0f,fRealZ),vector3(fRealX,20000.0f,fRealZ),v,fLens);
		}
		else
		{	
			pPoly[0]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+0) * SECTORSX];
			pPoly[1]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+0) * SECTORSX];
			pPoly[2]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+1) * SECTORSX];
			v[0]=*pPoly[0];
			v[1]=*pPoly[1];
			v[2]=*pPoly[2];
			CIntersection::PolygonRay(vector3(fRealX,0.0f,fRealZ),vector3(fRealX,20000.0f,fRealZ),v,fLens);	
		}
		return fLens;	
	}
	else
	{		
		int cIndices=0;
		int nShadowVertex[4];
		//List<vector3> vecPolyList;

		nShadowVertex[0]=m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX];
		nShadowVertex[1]=m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)];
		nShadowVertex[2]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX];		
		nShadowVertex[3]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)];
		for(cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
		{
			if( m_pDrawIndices[cIndices*3+0]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[3] ||
				
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[3] ||

				m_pDrawIndices[cIndices*3+2]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[3])				
			{
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
			}
		}
		

		vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();			

		for(int cPoly=0;cPoly<vecPolyList.num/3;cPoly++)
		{
			v[0]=vecPolyList[cPoly*3+0];
			v[1]=vecPolyList[cPoly*3+1];
			v[2]=vecPolyList[cPoly*3+2];
			if(CIntersection::PolygonRay(vector3(vecPos.x,0.0f,vecPos.z),vector3(vecPos.x,20000.0f,vecPos.z),v,fLens))
			{
				return fLens;
			}
		}
		return fLens;
	}
	*/
	
	
	//*/
	
	//*/
	
	
	
	/*
	if( fRealZ  >= -fRealX + fChecky)
	{
	//if( vecPos.z >= vecPos.x + fChecky)
	//{
		pPoly[0]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+0) * SECTORSX];
		pPoly[1]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+1) * SECTORSX];
		pPoly[2]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+1) * SECTORSX];
		v[0]=*pPoly[0];
		v[1]=*pPoly[1];
		v[2]=*pPoly[2];
		CIntersection::PolygonRay(vector3(fRealX,0.0f,fRealZ),vector3(fRealX,20000.0f,fRealZ),v,fLens);
	}
	else
	{		
		pPoly[0]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+1) * SECTORSX];
		pPoly[1]=(vector3*)&m_SectorVertex[ (PosX+1) + (PosZ+0) * SECTORSX];
		pPoly[2]=(vector3*)&m_SectorVertex[ (PosX+0) + (PosZ+0) * SECTORSX];
		v[0]=*pPoly[0];
		v[1]=*pPoly[1];
		v[2]=*pPoly[2];
		CIntersection::PolygonRay(vector3(fRealX,0.0f,fRealZ),vector3(fRealX,20000.0f,fRealZ),v,fLens);
	}	
	
	return fLens;
	*/
	/*
	float checky=(pz+(posz)*LInterVal)+(px+(posx)*LInterVal)+LInterVal;
	D3DVECTOR *v1,*v2,*v3;
	D3DVECTOR inter;
	if(pos.z >= -pos.x + checky)
	{			
		v1=(D3DVECTOR*)&m_geovertex[(posx+1)+(posz+1)*13];		
		v2=(D3DVECTOR*)&m_geovertex[(posx+1)+(posz)*13];
		v3=(D3DVECTOR*)&m_geovertex[(posx)+(posz+1)*13];
		CInterSection::PolygonRay(D3DVECTOR(pos.x,0,pos.z),D3DVECTOR(0,1,0),*v1,*v2,*v3,inter);			
	}
	else
	{
		v1=(D3DVECTOR*)&m_geovertex[(posx)+(posz)*13];		
		v2=(D3DVECTOR*)&m_geovertex[(posx+1)+(posz)*13];
		v3=(D3DVECTOR*)&m_geovertex[(posx)+(posz+1)*13];
		CInterSection::PolygonRay(D3DVECTOR(pos.x,0,pos.z),D3DVECTOR(0,1,0),*v1,*v2,*v3,inter);			
	} 
	if(inter.y<0.0f)
		inter.y=0.0f;
	return inter.y;	
	*/
}

void CSectorScene::GenerateShadow(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	GenerateWideMap();
	
	//if(GROUNDSHADOW==0)
	//	return;
	if(m_ShadowUsed!=-1)
		return;

	int cShadowTexture=0;
	for(;cShadowTexture<9;cShadowTexture++)
	{
		if(m_UsedShadowTerrain[cShadowTexture]==-1)
		{			
			m_UsedShadowTerrain[cShadowTexture]=1;
			m_ShadowUsed=cShadowTexture;
			break;
		}
	}
	
	
	char strGrassSectorName[256];		

	sprintf(strGrassSectorName,"%s\\GrassTable%d_%d",MAPDATAPATH,(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));
	FILE *fp=fopen(strGrassSectorName,"rb");
	if(fp)
	{
		fread(m_SectorShadowCollision[m_ShadowUsed],sizeof(char)*256*256,1,fp);
		fclose(fp);
	}
	else
	{
		memset(m_SectorShadowCollision[m_ShadowUsed],0,sizeof(char)*256*256);
	}
	/*
	for(int ix=0;ix<SHADOWSIZE;ix++)
	{
		for(int iy=0;iy<SHADOWSIZE;iy++)
		{				
			{
				m_SectorShadowCollision[m_ShadowUsed][ix+(iy)*SHADOWSIZE]|=0x80;	
			}
		}
	}
	*/

	return;

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_POINT);

	pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
	

	matrix matOldProjection,matProjection;
	matrix matOldView,matView,matInit;
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->GetTransform(D3DTS_VIEW,matOldView);

	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matInit);

	matProjection.MakeProjection(3.14159f/3.0f,1.0f,50,100000.0f);
	//D3DXMatrixOrthoLH(matProjection,10000,10000,1.0f, 100000.0f);	
	//matProjection.MakeAdjustedProjectionMatrix(3.14159f/3.0f,1.0f,10.0f,100000.0f,512,512,512,512);
	float fViewHeight=(SECTORSIZE/2.0f)/tanf(3.14159f/6.0f);

	matView.CameraLookAt(vector3(SECTORSIZE/2.0f,fViewHeight,SECTORSIZE/2.0f),vector3(SECTORSIZE/2.0f,0.0f,SECTORSIZE/2.0f),vector3(0.0f,0.0f,1.0f));
	
	pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);
	pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);

	m_SectorShadowTexture[cShadowTexture].Begin(pd3dDevice);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
	pd3dDevice->BeginScene();

	LVertex pVertex[4];

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=SECTORSIZE;
	pVertex[3].v.x=SECTORSIZE;

	pVertex[1].v.z=0.0f;
	pVertex[3].v.z=0.0f;
	pVertex[0].v.z=SECTORSIZE;
	pVertex[2].v.z=SECTORSIZE;				

	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;
	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=1.0f;
	pVertex[3].tv=1.0f;
	pVertex[0].tv=0.0f;
	pVertex[2].tv=0.0f;		
	for(int i=0;i<4;i++)
	{		
		pVertex[i].v.y=0.0f;
		pVertex[i].diff.c=0xffff0000;
		pVertex[i].spec.c=0xff000000;
	}	
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);	

	pd3dDevice->SetVertexShader(LVERTEXFVF);

	if(m_MapWide)
	{
		if(m_MapWide->m_nDetail<=1)
		{
			m_MapTexture=m_MapWide->m_WideMapTexture[0]->GetTexture();
		}
		else
		{
			m_MapTexture=m_MapWide->m_WideMapTexture[0]->GetTexture();
			//m_MapTexture=NULL;
		}
	}
	else
		DeterminTerrainTexture(0.0f,m_AccumulateTM._41,m_AccumulateTM._43);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	pd3dDevice->SetTexture(0,m_MapTexture);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));		
	pd3dDevice->EndScene();
	m_SectorShadowTexture[cShadowTexture].End(pd3dDevice);
	
	
	m_SwapShadowTexture.Begin(pd3dDevice);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xfffffff, 1.0f, 0);	
	pd3dDevice->BeginScene();
	pd3dDevice->EndScene();
	m_SwapShadowTexture.End(pd3dDevice);


	matrix matWorld,matObjectCamera;
	matrix mat,mat2;
	matrix matTexScale,matInv,matTex;
	matTexScale.MakeIdent();
	matTexScale._11=0.5f;
	matTexScale._22=-0.5f;	
	matTexScale._33=0.0f;
	matTexScale._41=0.5f;
	matTexScale._42=0.5f;
	matTexScale._43=1.0f;
	matTexScale._44=1.0f;

	matInv.Inverse(matView);	

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	

	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

		m_ObjectShadowTexture.Begin(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		//float fHeight=7000.0f;
		float fHeight=m_HouseObjectNode[cHouse]->m_fRad+17000.0f;

		matWorld=m_HouseObjectNode[cHouse]->m_AccumulateTM;		
		
		matObjectCamera.CameraLookAt(vector3(matWorld._41+fHeight-m_AccumulateTM._41,fHeight,matWorld._43+fHeight-m_AccumulateTM._43),
			vector3(matWorld._41-m_AccumulateTM._41,0.0f,matWorld._43-m_AccumulateTM._43),vector3(0.0f,0.0f,1.0f));

		//matObjectCamera.CameraLookAt(vector3(fHeight/2.0f,fHeight,fHeight/2.0f),vector3(0.0f,0.0f,0.0f),vector3(0.0f,1.0f,0.0f));		
		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);	

		matrix matObjectProj;
		D3DXMatrixOrthoLH(matObjectProj,m_HouseObjectNode[cHouse]->m_fRad*3.0f,m_HouseObjectNode[cHouse]->m_fRad*3.0f,10.0f, 100000.0f);	
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matObjectProj);

		matWorld=m_HouseObjectNode[cHouse]->m_AccumulateTM;
		matWorld._41-=m_AccumulateTM._41;
		matWorld._42=0.0f;
		matWorld._43-=m_AccumulateTM._43;

		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOutHouseObject)
			m_HouseObjectNode[cHouse]->m_HouseObject->m_pOutHouseObject->Render(pd3dDevice);		
		else
		{
			
		}

		pd3dDevice->EndScene();
		m_ObjectShadowTexture.End(pd3dDevice);
		///////////////////////////////////////////////////////		
		mat=matObjectProj*matTexScale;		
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;
		
		pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);
		pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
		pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matInit);
		
		m_SwapShadowTexture.Begin(pd3dDevice);

		//pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(0,m_ObjectShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		 
		pd3dDevice->SetVertexShader(LVERTEXFVF);		

		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		
		pd3dDevice->EndScene();		
		m_SwapShadowTexture.End(pd3dDevice);

	}
	//*/
	pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);

	int cPlant=0;
	unsigned char usTreeKind[SECTORSX*SECTORSX];
	vector3 vecTreePos[SECTORSX*SECTORSX];

	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind[cPlant]=m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]&0x3f;
				vecTreePos[cPlant].x=ix*LINTERVAL;
				vecTreePos[cPlant].z=iy*LINTERVAL;
				cPlant++;
			}
		}
	}
	matWorld.MakeIdent();

	TLVertex Vertex[4];
	Vertex[0].Diffuse.c=Vertex[1].Diffuse.c=Vertex[2].Diffuse.c=Vertex[3].Diffuse.c=0xffff0000;
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

	for(int cTree=0;cTree<cPlant;cTree++)
	{		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);   
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);	

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
		
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x99);
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);	
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

		m_ObjectShadowTexture.Begin(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);	

		pd3dDevice->BeginScene();		
		float fHeight=7000.0f;
		//float fHeight=m_HouseObjectNode[cHouse]->m_fRad+7000.0f;
		//if( usTreeKind[cTree]>=0 &&	usTreeKind[cTree] <MAX_TREEKIND)
			//m_TreeObjectNode.m_NormalTreeMesh[usTreeKind[cTree]]->Render(pd3dDevice);		
		matWorld._41=vecTreePos[cTree].x;
		matWorld._43=vecTreePos[cTree].z;
		
		matObjectCamera.CameraLookAt(vector3(matWorld._41+fHeight/2.0f,fHeight,matWorld._43+fHeight/2.0f),
			vector3(matWorld._41,0.0f,matWorld._43),vector3(0.0f,0.0f,1.0f));
		
		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);			

		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

		if( usTreeKind[cTree]>=0 &&	usTreeKind[cTree] <MAX_TREEKIND)
			m_TreeObjectNode.m_NormalTreeMesh[usTreeKind[cTree]]->Render(pd3dDevice);		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
		pd3dDevice->SetVertexShader(TLVERTEXFVF);		
		//pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,4,4,pLineIndices,D3DFMT_INDEX16,Vertex,sizeof(TLVertex));

		pd3dDevice->EndScene();
		m_ObjectShadowTexture.End(pd3dDevice);
		
		mat=matProjection*matTexScale;
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;
		
		pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
		pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matInit);
		
		m_SwapShadowTexture.Begin(pd3dDevice);

		//pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(0,m_ObjectShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		
		pd3dDevice->SetVertexShader(LVERTEXFVF);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
		
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));			
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		
		
		pd3dDevice->EndScene();	
		m_SwapShadowTexture.End(pd3dDevice);		

	}	

	WORD *pShadowPixel=(WORD*)m_SwapShadowTexture.Lock();
	for(int ix=0;ix<SHADOWSIZE;ix++)
	{
		for(int iy=0;iy<SHADOWSIZE;iy++)
		{
			if(pShadowPixel[ix+iy*SHADOWSIZE]!=(WORD)0xffff)
			{
				m_SectorShadowCollision[m_ShadowUsed][ix+(SHADOWSIZE-1-iy)*SHADOWSIZE]=1;
			}
			else
				m_SectorShadowCollision[m_ShadowUsed][ix+(SHADOWSIZE-1-iy)*SHADOWSIZE]=0;
		}
	}	
	m_SwapShadowTexture.Unlock();

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

		m_ObjectShadowTexture.Begin(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();
		
		float fHeight=1000.0f;	

		matWorld=m_ObjectSceneNode[cObject]->m_AccumulateTM;
		
		matObjectCamera.CameraLookAt(vector3(matWorld._41+fHeight/1.4f-m_AccumulateTM._41,fHeight,matWorld._43+fHeight/1.4f-m_AccumulateTM._43),
			vector3(matWorld._41-m_AccumulateTM._41,0.0f,matWorld._43-m_AccumulateTM._43),vector3(0.0f,0.0f,1.0f));

		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);
		matWorld=m_ObjectSceneNode[cObject]->m_AccumulateTM;
		matWorld._41-=m_AccumulateTM._41;
		matWorld._42=0.0f;
		matWorld._43-=m_AccumulateTM._43;

		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		
		m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);

		pd3dDevice->EndScene();
		m_ObjectShadowTexture.End(pd3dDevice);

		mat=matProjection*matTexScale;		
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;
		
		pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
		pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matInit);
		
		m_SwapShadowTexture.Begin(pd3dDevice);
		pd3dDevice->BeginScene();		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(0,m_ObjectShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		
		pd3dDevice->SetVertexShader(LVERTEXFVF);		

		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		
		pd3dDevice->EndScene();		
		m_SwapShadowTexture.End(pd3dDevice);
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);


	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

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
		pVertex[i].diff.c=0xffffffff;		
		pVertex[i].spec.c=0xff666666;		
	}

	pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	

	pShadowPixel=(WORD*)m_SectorShadowTexture[cShadowTexture].Lock();
	
	for(int ix=0;ix<SHADOWSIZE;ix++)
	{
		for(int iy=0;iy<SHADOWSIZE;iy++)
		{
			unsigned char r=(pShadowPixel[ix+iy*512]&0xf800)>>11;
			unsigned char g=(pShadowPixel[ix+iy*512]&0x07e0)>>6;
			unsigned char b=(pShadowPixel[ix+iy*512]&0x001f);
			if(g>=r && g>=b)
			{
				m_SectorShadowCollision[m_ShadowUsed][ix+(iy)*SHADOWSIZE]|=0x80;	
			}
		}
	}
	
	m_SectorShadowTexture[cShadowTexture].Unlock();


	m_SectorShadowTexture[cShadowTexture].Begin(pd3dDevice);
	pd3dDevice->BeginScene();
	
	if(m_MapWide && m_MapWide->m_nDetail>1)
	{
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);				
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE);	
		pd3dDevice->SetTexture(0,m_SwapShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);
	}
	else
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);    
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE);		
	
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);		
		
		pd3dDevice->SetTexture(0,m_SwapShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);
	}
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));		
	pd3dDevice->EndScene();
	m_SectorShadowTexture[cShadowTexture].End(pd3dDevice);
	

	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->SetTransform(D3DTS_VIEW,matOldView);

	pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
	
	//////////////////	
		
	GenerateObjectShadowed();
}

void CSectorScene::GetHeightFieldShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList)
{
	vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();		

	long VertexStartX,VertexStartY,VertexEndX,VertexEndY,VertexCenterX,VertexCenterY;
	VertexStartX=0;
	VertexStartY=0;
	VertexEndX=SECTORSX-1;
	VertexEndY=SECTORSY-1;

	int nFindVertex[4];
	
	while(1)
	{
		VertexCenterX=(VertexStartX+VertexEndX)>>1;
		VertexCenterY=(VertexStartY+VertexEndY)>>1;

		if(VertexEndX-VertexStartX==1 || VertexEndY-VertexStartY==1)
		{
			nFindVertex[0]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexStartX];
			nFindVertex[1]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexEndX];
			nFindVertex[2]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexStartX];
			nFindVertex[3]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexEndX];	
			break;
		}
		
		if(m_RenderUsedVertex[VertexCenterX+VertexCenterY*SECTORSX]!=-1)
		{
			if(VertexCenterY*LINTERVAL<vecSectorPos.z)
			{
				if(VertexCenterX*LINTERVAL<vecSectorPos.x)
				{
					//0VertexEndX=VertexEndX;	VertexEndY=VertexEndY;
					VertexStartX=VertexCenterX;
					VertexStartY=VertexCenterY;					
				}
				else
				{
					//1//VertexStartX=VertexStartX;//VertexEndY=VertexEndY;										
					VertexStartY=VertexCenterY;
					VertexEndX=VertexCenterX;					
				}
			}
			else
			{
				if(VertexCenterX*LINTERVAL<vecSectorPos.x)
				{
					//2	//VertexStartY=VertexStartY;//VertexEndX=VertexEndX;
					VertexStartX=VertexCenterX;					
					VertexEndY=VertexCenterY;
				}
				else
				{
					//3	VertexStartX=VertexStartX;VertexStartY=VertexStartY;
					VertexEndX=VertexCenterX;
					VertexEndY=VertexCenterY;
				}
			}			
		}	
		else
		{
			nFindVertex[0]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexStartX];
			nFindVertex[1]=m_RenderUsedVertex[(VertexStartY*SECTORSX)+VertexEndX];
			nFindVertex[2]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexStartX];
			nFindVertex[3]=m_RenderUsedVertex[(VertexEndY*SECTORSX)+VertexEndX];	
			break;
		}
	}

	int cIndices=0;		
	//List<vector3> vecPolyList;
	
	for(cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
	{
		if( m_pDrawIndices[cIndices*3+0]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+0]==nFindVertex[3] ||
			
			m_pDrawIndices[cIndices*3+1]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+1]==nFindVertex[3] ||

			m_pDrawIndices[cIndices*3+2]==nFindVertex[0] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[1] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[2] ||
			m_pDrawIndices[cIndices*3+2]==nFindVertex[3])				
		{
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
			vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
		}
	}

	/*
	vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();	
	long ShadowPosX,ShadowPosY;
	ShadowPosX=vecSectorPos.x/LINTERVAL;
	ShadowPosY=vecSectorPos.z/LINTERVAL;

	if( m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)]==-1)
	{
		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());		
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());
		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());

		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());		
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());
	}
	else
	{
	
		int cIndices=0;	
		int nShadowVertex[4];
		nShadowVertex[0]=m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX];			
		nShadowVertex[1]=m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)];						
		nShadowVertex[2]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX];			
		nShadowVertex[3]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)];			
		for(cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
		{
			if( m_pDrawIndices[cIndices*3+0]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[3] ||
				
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[3] ||

				m_pDrawIndices[cIndices*3+2]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[3])
				
			{
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
			}
		}
	}	
	*/
}

void CSectorScene::SetNationFlag(unsigned char cNation)
{
	// 성에 국기를변경하기 위한 코드(By Minbobo).
	for(int cHouse = 0; cHouse < (int)m_HouseObjectNode.size(); cHouse++)
	{				
		if(m_HouseObjectNode[cHouse]->GetFlag())
		{			
			char szFileName[0xff] = {0, };

			switch(cNation)
			{
			case 0 :
				sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_M.r3s");
				break;
			case 1 :
				sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_H.r3s");
				break;
			case 2 :
				sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_A.r3s");
				break;
			}

			m_HouseObjectNode[cHouse]->m_HouseObject->LoadFlag(szFileName);						
		}
	}	
}

void CSectorScene::DynamicLoad()
{
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	vector3 vecViewPos=matViewPosition->GetLoc();
	vector3 vecViewPosInter;
	float fViewInter;	
	char strFullName[256];

	float fFar = CSceneManager::m_WeatherManager.m_InterFogFar + 3000.0f;
	fFar = (fFar > MAX_HOUSEDISAPPER) ? MAX_HOUSEDISAPPER : fFar;

	if(fFar < 3001.0f)			// character selection 
		fFar = MAX_OUTVIEWRANGE;

	char strLightmap[256];
	char strOctree[256];

	int iSize = m_HouseObjectNode.size();
	int cHouse = 0;

	for(cHouse=0;cHouse<(int)iSize;cHouse++)
	{
		bool bInLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveInLightmap;
		bool bLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveLightmap;


		vecViewPosInter=m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc()-vecViewPos;
		fViewInter=vecViewPosInter.GetLens();

		///////////////////
		if(!CSceneManager::m_HeightField.GetHouseCullingFlag() || fViewInter< fFar )
		{
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOutHouseObject==NULL)
			{
				if(strcmp(m_HouseObjectNode[cHouse]->m_strOutName,"")!=0)
				{

					if(CRenderOption::m_BuildingLightmap && !m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveLightmap)	// lightmap
					{
						sprintf(strLightmap,"%s",m_HouseObjectNode[cHouse]->m_strOutName);
						strLightmap[strlen(strLightmap)-1]='S';
						strLightmap[strlen(strLightmap)-2]='D';
						strLightmap[strlen(strLightmap)-3]='D';

						CROSSM::CNTexture::SetPath(LIGHTMAPTEXTUREPATH);
						m_HouseObjectNode[cHouse]->m_HouseObject->LoadLightmap(strLightmap);						
					}
					else
					{
						bLightmap = false;
					}
					bLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveLightmap;
					
					sprintf(strFullName,"%s\\%s",HOUSEOBJECTPATH,m_HouseObjectNode[cHouse]->m_strOutName);

					// 성에 국기를 표시하기 위한 코드(By Minbobo).
					char* szTempName = m_HouseObjectNode[cHouse]->m_strOutName;

					char* strName = strupr(szTempName);
					
					// edith 2009.05.05 종족 깃발 로드하는 부분
					if(strcmp("INNERCASTLE_OUT.R3S", strName)==0)
					{						
						m_HouseObjectNode[cHouse]->SetFlag(true);

						char szFileName[0xff] = {0, };

						switch(CSceneManager::GetNationFlag())
						{
						case 0 :
							sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_M.r3s");
							break;
						case 1 :
							sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_H.r3s");
							break;
						case 2 :
							sprintf(szFileName,"%s\\%s",HOUSEOBJECTPATH, "SiezeFlag_A.r3s");
							break;
						}

						m_HouseObjectNode[cHouse]->m_HouseObject->LoadFlag(szFileName);
					}

					m_HouseObjectNode[cHouse]->m_HouseObject->LoadOut(strFullName);
					m_HouseObjectNode[cHouse]->m_SelfLoadOut=true;

					if(bLightmap == false)
					{
						m_HouseObjectNode[cHouse]->m_HouseObject->m_pOutHouseObject->SetPostConvert(LightFVF);
					}
					// Octree
					if(!m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree)
					{
						sprintf(strOctree,"%s",m_HouseObjectNode[cHouse]->m_strOutName);
						strOctree[strlen(strOctree)-1]='T';
						strOctree[strlen(strOctree)-2]='C';
						strOctree[strlen(strOctree)-3]='O';
		
						sprintf(strFullName,"%s\\%s",OCTREEPATH,strOctree);
						m_HouseObjectNode[cHouse]->m_bOctree = true;
						m_HouseObjectNode[cHouse]->m_HouseObject->LoadOctree(strFullName);
						//
					}
				}
			}
		}
		else
		{
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOutHouseObject)
			{
				m_HouseObjectNode[cHouse]->m_HouseObject->ReleaseOut();
				m_HouseObjectNode[cHouse]->m_HouseObject->ReleaseFlag();
				m_HouseObjectNode[cHouse]->m_HouseObject->ReleaseOctree();
				m_HouseObjectNode[cHouse]->m_bOctree = false;
				m_HouseObjectNode[cHouse]->m_SelfLoadOut= false;
			}
		
		}
		if(fViewInter<MAX_MEDVIEWRANGE || !m_HouseObjectNode[cHouse]->m_bMedCull)
		{
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pMedHouseObject==NULL)
			{
				if(!(strcmp(m_HouseObjectNode[cHouse]->m_strMedName,"")==0))
				{
					sprintf(strFullName,"%s\\%s",HOUSEOBJECTPATH,m_HouseObjectNode[cHouse]->m_strMedName);

					m_HouseObjectNode[cHouse]->m_HouseObject->LoadMed(strFullName);
					m_HouseObjectNode[cHouse]->m_SelfLoadMed=true;
					bLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveLightmap;
					if(bLightmap == false)
					{
						m_HouseObjectNode[cHouse]->m_HouseObject->m_pMedHouseObject->SetPostConvert(LightFVF);
					}
				} 
			}
		}
		else
		{
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pMedHouseObject!=NULL)
			{
				m_HouseObjectNode[cHouse]->m_HouseObject->ReleaseMed();
				m_HouseObjectNode[cHouse]->m_SelfLoadMed=false;
			}
		}

		if(fViewInter<MAX_INVIEWRANGE)
		{
		
			if(!(strcmp(m_HouseObjectNode[cHouse]->m_strInName,"")==0))
			{				
				if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pInHouseObject==NULL)
				{
					if(CRenderOption::m_BuildingLightmap && !m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveInLightmap)	// inner house lightmap
					{
						sprintf(strLightmap,"%s",m_HouseObjectNode[cHouse]->m_strInName);
						strLightmap[strlen(strLightmap)-1]='S';
						strLightmap[strlen(strLightmap)-2]='D';
						strLightmap[strlen(strLightmap)-3]='D';

						CROSSM::CNTexture::SetPath(LIGHTMAPTEXTUREPATH);
						m_HouseObjectNode[cHouse]->m_HouseObject->LoadInLightmap(strLightmap);						
					}
					else
					{
						bLightmap = false;
					}
					bLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveInLightmap;
				
					sprintf(strFullName,"%s\\%s",HOUSEOBJECTPATH,m_HouseObjectNode[cHouse]->m_strInName);
					m_HouseObjectNode[cHouse]->m_HouseObject->LoadIn(strFullName);
					m_HouseObjectNode[cHouse]->m_SelfLoadIn=true;
					
					bLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveLightmap;
					bInLightmap = m_HouseObjectNode[cHouse]->m_HouseObject->m_isHaveInLightmap;

					if(bLightmap == false && bInLightmap == false )
					{
						m_HouseObjectNode[cHouse]->m_HouseObject->m_pInHouseObject->SetPostConvert(LightFVF);
					}
				}
			}
		}
		else
		{
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pInHouseObject!=NULL)
			{
				matrix matInv;
				matInv.Inverse(m_HouseObjectNode[cHouse]->m_AccumulateTM);
				vector3 vecHouseLocalViewPos = vecViewPos * matInv;
				vector3 vecInHouseMin, vecInHouseMax;

				if (0 == stricmp("innercastle_in.r3s", m_HouseObjectNode[cHouse]->m_strInName))
				{
					m_HouseObjectNode[cHouse]->m_HouseObject->CalcBox(vecInHouseMin, vecInHouseMax);
				}
				else
				{
					m_HouseObjectNode[cHouse]->m_HouseObject->CalcBox(vecInHouseMin, vecInHouseMax, 1);	// 1 : in house 만 계산
				}

				CrossM::Math::VECTOR3 vHouseLocalViewPos, vInHouseMin, vInHouseMax;
				ConvVector3(vHouseLocalViewPos, vecHouseLocalViewPos);
				ConvVector3(vInHouseMin, vecInHouseMin);
				ConvVector3(vInHouseMax, vecInHouseMax);
				if (!CrossM::Math::IsPointInAABB(vHouseLocalViewPos, vInHouseMin, vInHouseMax))
				{
					m_HouseObjectNode[cHouse]->m_HouseObject->ReleaseIn();
					m_HouseObjectNode[cHouse]->m_SelfLoadIn=false;
				}
			}
		}			
	}

	char strObjFileName[256] = {0};

	// Object
	int cObject;
	iSize = m_ObjectSceneNode.size();
	for(cObject=0;cObject<(int)iSize;cObject++)
	{
		vecViewPosInter=m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc()-vecViewPos;		
		fViewInter=vecViewPosInter.GetLens();
		if(MAX_OBJECTVIEWRANBE<fViewInter)//지워 
		{
			if(m_ObjectSceneNode[cObject]->m_pObject)
			{	
				CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_ObjectSceneNode[cObject]->m_pObject);
				m_ObjectSceneNode[cObject]->m_pObject=NULL;
				
			}				
		}
		else//로드
		{
			if(m_ObjectSceneNode[cObject]->m_pObject==NULL)
			{					
				
				sprintf(strObjFileName,"%s%s",OBJECTPATH,m_ObjectSceneNode[cObject]->m_strObjectName);
				m_ObjectSceneNode[cObject]->m_pObject= (CROSSM::CNMesh *)CSceneManager::_GetResource(CROSSM::RESOURCE_MESH,strObjFileName);
				m_ObjectSceneNode[cObject]->m_pObject->SetPostConvert(LightFVF);
			}				
		}
	}

}

void CSectorScene::GetLight(vector3 vecPos,std::vector<D3DLIGHT8> &LightList)
{
	
}


void CSectorScene::GetInHouseShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList,vector3 vecChrLightPos)
{	
	long cAddShadowPolySet=0;
	vector3 vecHousePos,vecHouseInterLens;
	matrix matInHousePos,matInv;
	vector3 vecInHousePos,vecLightPos;
	vector3 vecLightInter;
	matrix matInLightPos,matTrans;		
	
	float fHouseInter;//,fLightInter;	
	CViewCamera ToChrCamera;
	ToChrCamera.BuildFrustum(1.0,3.14159f/6.0f,40,1000.0f);
	matrix matLightToChrView,matLightToChrPos;
	matrix matLightInHousePos;
//	std::vector<float> fInterList;

	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{	
		vecHousePos=m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc();
		vecHouseInterLens=vecHousePos-vecPos;
		fHouseInter=vecHouseInterLens.GetLens();

		if(fHouseInter< m_HouseObjectNode[cHouse]->m_fRad)
		{			
			matInv.Inverse(m_HouseObjectNode[cHouse]->m_AccumulateTM);
			matInHousePos.Translation(vecPos);
			matInHousePos=matInHousePos*matInv;			
			vecInHousePos=matInHousePos.GetLoc();

			matInLightPos.Translation(vecChrLightPos);
			matInLightPos=matInLightPos*matInv;			
			vecLightPos=matInLightPos.GetLoc();

			//matLightToChrView.CameraLookAt(vecInHousePos,vecLightPos,vector3(0.0f,1.0f,0.0f));

			matLightToChrView.CameraLookAt(vecLightPos,vecInHousePos,vector3(0.0f,1.0f,0.0f));

			matLightToChrPos.Inverse(matLightToChrView);
			ToChrCamera.SetMatPosition(matInLightPos);	
			ToChrCamera.MoveFrustum();
			std::vector<PolyNode*> CullPolyList;
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree
				&& 	m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->IsLoaded())
			{
				//m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->CullFrustum(&ToChrCamera,CullPolyList);
				vector3 vecDir=vecInHousePos-vecLightPos;
				vecDir.Normalize();
				m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->CullRay(vecLightPos,vecDir,10000.0f,CullPolyList);

				for(int cPoly=0;cPoly<(int)CullPolyList.size();cPoly++)
				{
					vector3 vec1,vec2,vec3;
					vector3 vecLine1,vecLine2;
					
					vector3 vecCross;
					float fDot;

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[0]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vec1 = matTrans.GetLoc();

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[1]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vec2 = matTrans.GetLoc();

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[2]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vec3 = matTrans.GetLoc();
					
					
					vecLine1 = vec2 - vec1;
					vecLine2 = vec3 - vec1;

					vecCross = vecLine2 ^ vecLine1;
					vecCross.Normalize();

					vecChrLightPos.Normalize();

					fDot = vecCross*vecChrLightPos;
					
					if(fDot <= 0.0) {
						vecPolyList.push_back(vec1);
						vecPolyList.push_back(vec2);
						vecPolyList.push_back(vec3);
					}
				}
			}
			/*
			for(int cLight=0;cLight<m_HouseObjectNode[cHouse]->m_HouseObject->m_LightList.num;cLight++)
			{				
				vecLightPos=m_HouseObjectNode[cHouse]->m_HouseObject->m_LightList[cLight]->m_TM.GetLoc();
				vecLightInter=vecInHousePos-vecLightPos;
				fLightInter=vecLightInter.GetLens();				
				if( (fLightInter < m_HouseObjectNode[cHouse]->m_HouseObject->m_LightList[cLight]->m_pLightObject->m_fLightRange + 200.0f) &&
					vecInHousePos.y <= vecLightPos.y)
				{	
					for(int cAddLight=0;cAddLight<fInterList.num;cAddLight++)
					{
						if(fInterList[cAddLight] > fLightInter)
							break;
					}
					fInterList.Insert(fLightInter,cAddLight);
					vecChrLightPos.Insert(vecLightPos,cAddLight);										
				}
			}

			for(int cShadow=0;(cShadow<MAXCHRSHADOW && cShadow<vecChrLightPos.num);cShadow++)
			{
				matLightToChrView.CameraLookAt(vecChrLightPos[cShadow],vecInHousePos,vector3(0.0f,1.0f,0.0f));
				matLightToChrPos.Inverse(matLightToChrView);
				ToChrCamera.SetMatPosition(matLightToChrPos);
				ToChrCamera.MoveFrustum();
				//List<PolyNode*> CullPolyList;
				if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree)
				{
					m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->CullFrustum(&ToChrCamera,CullPolyList);
					for(int cPoly=0;cPoly<CullPolyList.num;cPoly++)
					{
						matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[0]);
						//matTrans=m_HouseObjectNode[cHouse]->m_AccumulateTM*matTrans;
						matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
						vecPolyList[cShadow].Add(matTrans.GetLoc());

						matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[1]);
						//matTrans=m_HouseObjectNode[cHouse]->m_AccumulateTM*matTrans;
						matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
						vecPolyList[cShadow].Add(matTrans.GetLoc());

						matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[2]);
						//matTrans=m_HouseObjectNode[cHouse]->m_AccumulateTM*matTrans;
						matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
						vecPolyList[cShadow].Add(matTrans.GetLoc());
					}
				}
				matLightInHousePos.Translation(vecChrLightPos[cShadow]);
				matLightInHousePos=m_HouseObjectNode[cHouse]->m_AccumulateTM*matLightInHousePos;
				vecChrLightPos[cShadow]=matLightInHousePos.GetLoc();
			}
			return;			
			*/

		}
	}	
}

void CSectorScene::GetLineIntersectPoly(vector3 vecStart, vector3 vecEnd,std::vector<vector3> &vecPolyList)
{
	long cAddShadowPolySet=0;

	vector3 vecHousePos,vecHouseInterLens;
	matrix matInHousePos,matInv;
	vector3 vecInHousePos;	
	matrix matTrans;		
	
	float fHouseInter;	

	vector3 vecInHouseStart,vecInHouseEnd;

	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{
		vecHousePos=m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc();
		vecHouseInterLens=vecHousePos-vecStart;
		fHouseInter=vecHouseInterLens.GetLens();
		if( fHouseInter<m_HouseObjectNode[cHouse]->m_fRad)
		{
			matInv.Inverse(m_HouseObjectNode[cHouse]->m_AccumulateTM);

			matInHousePos.Translation(vecStart);
			matInHousePos=matInHousePos*matInv;
			vecInHouseStart=matInHousePos.GetLoc();

			matInHousePos.Translation(vecEnd);
			matInHousePos=matInHousePos*matInv;
			vecInHouseEnd=matInHousePos.GetLoc();

			vector3 vecInHouseDir=vecInHouseEnd-vecInHouseStart;
			float fInHouseLens=vecInHouseDir.GetLens();
			vecInHouseDir.Normalize();

			std::vector<PolyNode*> CullPolyList;

			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree
				&& m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->IsLoaded())
			{
				m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->CullRay(vecInHouseStart,vecInHouseDir,fInHouseLens,CullPolyList);
				for(int cPoly=0;cPoly<(int)CullPolyList.size();cPoly++)
				{
					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[0]);
					//matTrans=m_HouseObjectNode[cHouse]->m_AccumulateTM*matTrans;
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());
					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[1]);
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[2]);
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());
				}
			}
			return;
		}
	}
}

void CSectorScene::RenderGrass(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	if(CRenderOption::m_RangeGrassRender)
	{
		if(m_pLODGrassAlpha==NULL)
			return;	

		pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
		//pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		matrix matTex;
		matTex.MakeIdent();
		matTex._11=10;
		matTex._22=10;

		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);	

		matTex.MakeIdent();
		matTex._11=0;
		matTex._22=0;

		matTex._12=1;
		matTex._21=1;

		pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex);			
 
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );				
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );					
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);	

		pd3dDevice->SetTexture(0,m_LODGrassTexture.GetTexture());
		pd3dDevice->SetTexture(1,m_pLODGrassAlpha->GetTexture());

		pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

		pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE2X);

		//pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2, D3DTA_CURRENT);

		vector3 vecViewPos;

		if(CSceneManager::m_ViewerMode==0 || CSceneManager::m_ViewerMode==2)
		{
			vecViewPos=CSceneManager::GetCamera()->GetMatPosition()->GetLoc();				
			vecViewPos.y=CSceneManager::m_HeightField.GetHeight(vecViewPos);
			
		}
		else
		{		
			CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(vecViewPos.x,vecViewPos.y,vecViewPos.z);				
			vecViewPos.y=CSceneManager::m_HeightField.GetHeight(vecViewPos);
		}

		matrix matGrassRangeProj,matGrassRangeView,matTexScale;
		matrix matView,matInv,mat,mat2;

		matGrassRangeProj.MakeProjection(3.14159f/3.0f,1.0f,50,100000.0f);		
		D3DXMatrixOrthoLH(matGrassRangeProj,LINTERVAL*MAX_GRASSRANGE,LINTERVAL*MAX_GRASSRANGE,10.0f, 10000.0f);
		
		float fGrassRangeHeight=tanf(3.14159f/3.0f)*((LINTERVAL-1)*0.5f*MAX_GRASSRANGE);
		fGrassRangeHeight-=400.0f;

		matGrassRangeView.CameraLookAt(vecViewPos+vector3(0.0f,fGrassRangeHeight,0.0f),vecViewPos,vector3(0.0f,0.0f,1.0f));									

		matTexScale.MakeIdent();
		matTexScale._11=0.5f;
		matTexScale._22=-0.5f;	
		matTexScale._33=0.0f;
		matTexScale._41=0.5f;
		matTexScale._42=0.5f;
		matTexScale._43=1.0f;
		matTexScale._44=1.0f;

		pd3dDevice->GetTransform(D3DTS_VIEW,matView);
		matInv.Inverse(matView);	
		
		mat=matGrassRangeProj*matTexScale;
		mat2=matGrassRangeView*mat;   
		matTex=matInv*mat2;

		pd3dDevice->SetTexture(2,NULL);

		//if(m_AccumulateTM._41 <= vecViewPos.x && vecViewPos.x <= m_AccumulateTM._41+SECTORSIZE &&
		//	m_AccumulateTM._43 <= vecViewPos.z && vecViewPos.z <= m_AccumulateTM._43+SECTORSIZE  )
		{
			pd3dDevice->SetTexture(2,m_GrassRangeTexture->GetTexture());

			pd3dDevice->SetTransform(D3DTS_TEXTURE2,matTex);
			
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

			pd3dDevice->SetTexture(2,m_GrassRangeTexture->GetTexture());					
			
			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

			pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_SELECTARG2);

			pd3dDevice->SetTextureStageState(2,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState(2,D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		}

		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

		/*	
		*/

		pd3dDevice->SetVertexShader(SectorFVF);	

		pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
		pd3dDevice->SetIndices(m_pROAMIndices,0);	


		matrix  matGrassTerrain=m_AccumulateTM;	

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);				

		for(int i=0;i<6;i++)
		{
			matGrassTerrain._42+=30.0f;

			pd3dDevice->SetTransform(D3DTS_WORLD,matGrassTerrain);	
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
			CSceneManager::m_cRenderPrimitive++;
		}			
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		matTex.MakeIdent();
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTransform(D3DTS_TEXTURE2,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );			
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
		pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}

	//m_GrassObjectNode.Render(pd3dDevice);
}

void CSectorScene::GenerateMustDivideVertex()
{
	m_MapMustDivideVertex=m_pMapStorage->FindMustDivideVertexMap(m_TM._41,m_TM._43);
	if(m_MapMustDivideVertex)
	{
		memcpy(m_VarianceLeft,m_MapMustDivideVertex->m_pLeftNodeList,sizeof(int)*(1<<(VARIANCE_DEPTH)));
		memcpy(m_VarianceRight,m_MapMustDivideVertex->m_pRightNodeList,sizeof(int)*(1<<(VARIANCE_DEPTH)));
	}
}
// Sector Light 관련
void CSectorScene::GenerateSectorLight()
{

	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
	vector3 vecViewPos=matViewPosition->GetLoc();
	
	if(m_pLightEsf == NULL) 
	{
		m_pLightEsf = new CEffScript[1];
		m_pLightEsf->GetScriptBinData("TL.esf");
		m_pLightEsf->SetWorldEffect(true);
		m_pLightEsf->SetEffectRot(0.0f,0.0f,0.0f);
		m_pLightEsf->m_Device = CSceneManager::GetDevice();
	}

	CSectorLightMap *pMapLight = m_pMapStorage->FindLightMap(m_TM._41,m_TM._43);
	if(pMapLight != NULL)
	{
		int i;
		m_SectorLightManager.DeleteAllLight();
		
		int indexx=(int)(m_TM._41/SECTORSIZE);
		int indexy=(int)(m_TM._43/SECTORSIZE);

		m_SectorLightManager.m_IndexX = indexx;
		m_SectorLightManager.m_IndexY = indexy;

		m_SectorLightManager.m_pLightEsf = m_pLightEsf;

		int iSize = (int)pMapLight->m_lstLights.size();
		for( i = 0; i < iSize; i++ )
		{
			CSectorLight CurrentLight = pMapLight->m_lstLights[i];
			m_SectorLightManager.InputLight(CurrentLight.m_vecPos,CurrentLight.m_vecColor,CurrentLight.m_fRange,CurrentLight.m_strEffect,CurrentLight.m_iLightSamples,CurrentLight.m_iShadowSamples,CurrentLight.m_fShadowFactors,CurrentLight.m_iAmbient,CurrentLight.m_fExpose); 
		
		}
	}
	CSceneManager::m_pPickSectorLight = NULL;
	GenerateEffect();
}
void CSectorScene::GenerateEffect()
{
	
	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
	vector3 vecViewPos=matViewPosition->GetLoc();

	//m_MapEffect = m_pMapStorage->FindEffectMap( vecViewPos.x, vecViewPos.z ); 
	m_MapEffect = m_pMapStorage->FindEffectMap( m_TM._41,m_TM._43);
/*	for(int cEffect = 0;cEffect<m_EffectNode.num;cEffect++) {
		delete m_EffectNode[cEffect];

	}
*/	
	m_WorldEffectManager.DeleteAllWorldScript();
	if(m_MapEffect != NULL) 
	{
		int iSize = (int)m_MapEffect->m_vecEffectPos.size();
		for(int cEffect = 0;cEffect<iSize;cEffect++) 
		{
			if(strlen(m_MapEffect->m_strEffectName[cEffect]) < 1)
				continue;

			CEffScript *test_script;				

			test_script = new CEffScript;
						
			test_script->SetWorldEffect(true);
			
			if(test_script->GetScriptBinData(m_MapEffect->m_strEffectName[cEffect]))
			{

				m_WorldEffectManager.AddWorldEffScript(test_script);

				test_script->SetEffectRot(m_MapEffect->m_vecEffectRot[cEffect].x,
									   m_MapEffect->m_vecEffectRot[cEffect].y,
									   m_MapEffect->m_vecEffectRot[cEffect].z);
				test_script->SetStartPos(m_MapEffect->m_vecEffectPos[cEffect].x,
										 m_MapEffect->m_vecEffectPos[cEffect].y,
										 m_MapEffect->m_vecEffectPos[cEffect].z);

				test_script->SetEndPos(m_MapEffect->m_vecEffectPos[cEffect].x,
									   m_MapEffect->m_vecEffectPos[cEffect].y,
									   m_MapEffect->m_vecEffectPos[cEffect].z);
		

			}
			else {
				delete test_script;
				test_script = NULL;
			}
			/*CX3DEffect *pAddNode = new CX3DEffect();
			
			CSceneManager::AddEffect(pAddNode);
			
			pAddNode->Load(EFFECTPATH,m_MapEffect->m_strEffectName[cEffect]);
			pAddNode->SetLoop(TRUE);
			pAddNode->SetCenter(m_MapEffect->m_vecEffectPos[cEffect].x,
								  m_MapEffect->m_vecEffectPos[cEffect].y,
								  m_MapEffect->m_vecEffectPos[cEffect].z);
			pAddNode->SetAxis(0.0f,0.0f,0.0f);
			pAddNode->SetEffectStart(true);*/
			//m_EffectNode.Add(pAddNode);

		} //for

	} //if
	/*	
	if(m_MapEffect != NULL) {

		for(int cEffect = 0;cEffect<m_MapEffect->m_vecEffectPos.num;cEffect++) {
			CX3DEffect *pAddNode = new CX3DEffect();
			
			CSceneManager::AddEffect(pAddNode);
			
			pAddNode->Load(EFFECTPATH,m_MapEffect->m_strEffectName[cEffect]);
			pAddNode->SetLoop(TRUE);
			pAddNode->SetCenter(m_MapEffect->m_vecEffectPos[cEffect].x,
								  m_MapEffect->m_vecEffectPos[cEffect].y,
								  m_MapEffect->m_vecEffectPos[cEffect].z);
			pAddNode->SetAxis(0.0f,0.0f,0.0f);
			pAddNode->SetEffectStart(true);
			//m_EffectNode.Add(pAddNode);

		}
	}

*/	
	/*if(m_EffectNode.num) 
	{
		for(int i=0;i<m_EffectNode.num;i++)
		{
			
			CSceneManager::DeleteEffect(m_EffectNode[i]);
		}
		m_EffectNode.num=0;
	}
	CSceneManager::m_EffectNum = 0;

	m_MapEffect = m_pMapStorage->FindEffectMap(m_AccumulateTM._41,m_AccumulateTM._43);
	if(m_MapEffect) 
	{		
		for(int i=0;i<m_MapEffect->m_vecEffectPos.num;i++)
		{
			CX3DEffect *pAddEffect = new CX3DEffect();

			CSceneManager::AddEffect(pAddEffect );

			pAddEffect->Load(EFFECTPATH,m_MapEffect->m_strEffectName[i]);
			pAddEffect->SetLoop(TRUE);
			pAddEffect->SetCenter(m_MapEffect->m_vecEffectPos[i].x,
								  m_MapEffect->m_vecEffectPos[i].y,
								  m_MapEffect->m_vecEffectPos[i].z);
			pAddEffect->SetAxis(0.0f,0.0f,0.0f);
			pAddEffect->map_effect = true;

			m_EffectNode.Add(pAddEffect);
		}
	}

*/

/*
	CEffectMap *tmp;
	CX3DEffect *pEffect = new CX3DEffect();
*/
/*	if(m_MapEffect)
		delete m_MapEffect;
*/	
/*	for(int i=0;i<m_MapEffect->Effect_Num;i++) {
		tmp = m_pMapStorage->FindEffectMap(m_AccumulateTM._41,m_AccumulateTM._43);
		if(tmp != NULL) {
			m_MapEffect->m_Effect.Add(*tmp);
			CSceneManager::AddEffect(pEffect);
			pEffect->Load(EFFECTPATH,m_MapEffect->m_Effect[i].FileName);
			pEffect->SetLoop(TRUE);
			pEffect->SetCenter(m_MapEffect->m_Effect[i].pos.x,m_MapEffect->m_Effect[i].pos.y,m_MapEffect->m_Effect[i].pos.z);
			pEffect->SetAxis(0.0f,0.0f,0.0f);		
			pEffect->map_effect = true;
	
			int indexx=(int)(m_MapEffect->m_Effect[i].pos.x/SECTORSIZE);
			int indexy=(int)(m_MapEffect->m_Effect[i].pos.z/SECTORSIZE);

			pEffect->m_SectorX = indexx;
			pEffect->m_SectorY = indexy;
		}
	}
/*	m_MapEffect=m_pMapStorage->FindEffectMap(m_AccumulateTM._41,m_AccumulateTM._43);
	if(m_MapEffect)
	{
		for(int cEffect=0;cEffect<m_MapEffect->m_EffectMethod.num;cEffect++)
		{
			switch(m_MapEffect->m_EffectMethod[cEffect])
			{
				case 0:
					CSceneManager::AddParticle(m_MapEffect->m_vecEffectPos[cEffect],vector3(1.0f,1.0f,0.0f));
					break;
			}
		}
	}*/
	
}

void CSectorScene::AllObjectCalcLens()
{
	DynamicLoad();
	m_TreeObjectNode.m_AccumulateTM=m_AccumulateTM;
	m_TreeObjectNode.CalcObjectLens();

}
/*
void CSectorScene::GenerateSound()
{
	for(int cSound=0;cSound<m_SoundObjectNode.num;cSound++)
	{
		delete m_SoundObjectNode[cSound];
	}
	m_SoundObjectNode.num=0;
	m_MapSound=m_pMapStorage->FindSoundMap(m_AccumulateTM._41,m_AccumulateTM._43);
	if(m_MapSound)
	{
		for(int i=0;i<m_MapSound->m_strSoundFilenameList.num;i++)
		{
			CZ3DSound *AddNode=new CZ3DSound();
			AddNode->Load(m_MapSound->m_strSoundFilenameList[i]);
			AddNode->Loop(true);			
			AddNode->SetPosition(m_MapSound->m_SoundPosList[i]);
			AddNode->SetMinDistance(m_MapSound->m_SoundMinRangeList[i]);
			AddNode->SetMaxDistance(m_MapSound->m_SoundMaxRangeList[i]);
			AddNode->Play();
			m_SoundObjectNode.Add(AddNode);
		}
	}
}
*/
void CSectorScene::GetHeightFieldRangePoly(vector3 vecPos,std::vector<vector3> &vecPolyList,float fRange)
{
	vector3 vecSectorPos=vecPos-m_AccumulateTM.GetLoc();	
	long ShadowPosX,ShadowPosY;
	ShadowPosX=vecSectorPos.x/LINTERVAL;
	ShadowPosY=vecSectorPos.z/LINTERVAL;
	long Range=fRange/LINTERVAL;
	
	long sx,sy,ex,ey;
	if(ShadowPosX-Range<=0)
		sx=0;
	else
		sx=ShadowPosX-Range;

	if(ShadowPosY-Range<=0)
		sy=0;
	else
		sy=ShadowPosY-Range;

	if(ShadowPosX+Range>SECTORSX)
		ex=SECTORSX;
	else
		ex=ShadowPosX+Range;

	if(ShadowPosY+Range>SECTORSY)
		ey=SECTORSY;
	else
		ey=ShadowPosY+Range;

	std::vector<int> RangeVertexList;

	for(int ix=sx;ix<ex;ix++)
	{
		for(int iy=sy;iy<ey;iy++)
		{
			if(m_RenderUsedVertex[(iy*SECTORSX)+ix]!=-1)
			{
				RangeVertexList.push_back(m_RenderUsedVertex[(iy*SECTORSX)+ix]);
			}
		}
	}
	bool *pUsedRangePoly=new bool[m_UsedIndices/3];	
	memset(pUsedRangePoly,false,sizeof(bool)*m_UsedIndices/3);

	for(int cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
	{
		if(pUsedRangePoly[cIndices])
			continue;
		for(int cVertex=0;cVertex<(int)RangeVertexList.size();cVertex++)
		{
			if(pUsedRangePoly[cIndices])
				continue;
			if( m_pDrawIndices[cIndices*3+0]==RangeVertexList[cVertex] ||
				m_pDrawIndices[cIndices*3+1]==RangeVertexList[cVertex] ||
				m_pDrawIndices[cIndices*3+2]==RangeVertexList[cVertex])
			{
				vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
				vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
				vecPolyList.push_back(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
				pUsedRangePoly[cIndices]=true;
			}
		}
	}
	delete [] pUsedRangePoly;
	/*
	if( m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX]==-1 &&
		m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)]==-1)
	{
		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());		
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());
		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());

		vecPolyList.Add(m_SectorVertex[ ShadowPosX-1 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());		
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY+2) * SECTORSX ].v+m_AccumulateTM.GetLoc());
		vecPolyList.Add(m_SectorVertex[ ShadowPosX+2 + (ShadowPosY-1) * SECTORSX ].v+m_AccumulateTM.GetLoc());
	}
	else
	{
		int cIndices=0;	
		int nShadowVertex[4];
		nShadowVertex[0]=m_RenderUsedVertex[(ShadowPosY*SECTORSX)+ShadowPosX];			
		nShadowVertex[1]=m_RenderUsedVertex[((ShadowPosY)*SECTORSX)+(ShadowPosX+1)];						
		nShadowVertex[2]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+ShadowPosX];			
		nShadowVertex[3]=m_RenderUsedVertex[((ShadowPosY+1)*SECTORSX)+(ShadowPosX+1)];			
		for(cIndices=0;cIndices<m_UsedIndices/3;cIndices++)
		{
			if( m_pDrawIndices[cIndices*3+0]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+0]==nShadowVertex[3] ||
				
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+1]==nShadowVertex[3] ||

				m_pDrawIndices[cIndices*3+2]==nShadowVertex[0] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[1] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[2] ||
				m_pDrawIndices[cIndices*3+2]==nShadowVertex[3])
				
			{
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+0]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+1]].v+m_AccumulateTM.GetLoc());
				vecPolyList.Add(m_pDrawVertex[m_pDrawIndices[cIndices*3+2]].v+m_AccumulateTM.GetLoc());
			}
		}
	}
	*/
}

void CSectorScene::GenerateObjectShadowed()
{
	if(m_ShadowUsed==-1)
		return;
	char *pShadowCollision=m_SectorShadowCollision[m_ShadowUsed];	

	//float fInSectorPosX,fInSectorPosY;
	float fInterVal=SECTORSIZE/512.0f;
	vector3 vecObjectPos;

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{			
		if(m_ObjectSceneNode[cObject]->m_pObject==NULL || !m_ObjectSceneNode[cObject]->m_pObject->IsLoaded() )
			continue;
		vecObjectPos=m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc();

		int px=(int)(vecObjectPos.x/SECTORSIZE);
		int pz=(int)(vecObjectPos.z/SECTORSIZE);
		float fPosx=vecObjectPos.x-(px*SECTORSIZE);
		float fPosz=vecObjectPos.z-(pz*SECTORSIZE);
		
		fPosx/=fInterVal;
		fPosz/=fInterVal;
		if(pShadowCollision[(int)fPosx+((int)fPosz)*512]==1)
		{
			m_ObjectSceneNode[cObject]->m_isShadowed=true;
		}
	}
}

void CSectorScene::RenderDetail(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	pd3dDevice->SetVertexShader(SectorFVF);

	matrix matTex;
	matTex.MakeIdent();
	matTex._11=DETAILTEXTUREDEPTH;
	matTex._22=DETAILTEXTUREDEPTH;

	pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );				
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);	

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);


	CSceneManager::m_WeatherManager.m_InterTerrainColor;

	if(m_MapWide)
	{
		if(m_MapWide->m_nDetail<=1)
		{
			if(m_MapWide->m_WideMapTexture[0])
			{
				/*
				if(m_ShadowUsed!=-1)
				{
					m_MapTexture=m_SectorShadowTexture[m_ShadowUsed].GetTexture();
				}		
				else
				{
					m_MapTexture=m_MapWide->m_WideMapTexture[0].GetTexture();
				}
				*/
				if(m_MapWide->m_WideMapTexture[0]->IsLoaded())
					m_MapTexture=m_MapWide->m_WideMapTexture[0]->GetTexture();
				else
					m_MapTexture=NULL;
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );				
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

				pd3dDevice->SetTexture(0,m_MapTexture);	
				if(m_MapWide->m_DetailTexture[0] && m_MapWide->m_DetailTexture[0]->IsLoaded())
					pd3dDevice->SetTexture(1,m_MapWide->m_DetailTexture[0]->GetTexture());				
				else
					pd3dDevice->SetTexture(1,NULL);
				/*
				if(GF3OPTION)
				{
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					pd3dDevice->SetTexture(2,m_CloudShadow.GetTexture());
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,0);
					pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);					

					matrix matTrans;
					matTrans.MakeIdent();
					matTrans._11=0.11f;
					matTrans._22=0.11f;		
					float fCloudMoveX=m_AccumulateTM._41/SECTORSIZE;
					float fCloudMoveZ=m_AccumulateTM._43/SECTORSIZE;		
					matTrans._31=m_fCloudMove+fCloudMoveX*0.11f;
					matTrans._32=m_fCloudMove+fCloudMoveZ*0.11f;			


					pd3dDevice->SetTransform( D3DTS_TEXTURE2, matTrans );

					
				}
				*/
				
				pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
				pd3dDevice->SetIndices(m_pROAMIndices,0);
				
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
				

				CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
				CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
				/*
				if(GF3OPTION)
				{		
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
					pd3dDevice->SetTexture(2,NULL);
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,2);
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
				}
				*/
			}
		}
		else
		{
			if(m_MapWide->m_WideMapTexture[0])
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

				if(m_MapWide->m_WideMapTexture[0]->IsLoaded())
					pd3dDevice->SetTexture(0,m_MapWide->m_WideMapTexture[0]->GetTexture());
				else
					pd3dDevice->SetTexture(0,NULL);
				if(m_MapWide->m_DetailTexture[0] && m_MapWide->m_DetailTexture[0]->IsLoaded())
					pd3dDevice->SetTexture(1,m_MapWide->m_DetailTexture[0]->GetTexture());
				else
					pd3dDevice->SetTexture(1,NULL);

				pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
				pd3dDevice->SetIndices(m_pROAMIndices,0);	
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

				CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
				CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
			}			

			if(m_MapWide->m_WideMapTexture[1])
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);		

				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

				if(m_MapWide->m_WideMapTexture[1]->IsLoaded())
					pd3dDevice->SetTexture(0,m_MapWide->m_WideMapTexture[1]->GetTexture());
				else
					pd3dDevice->SetTexture(0,NULL);
				if(m_MapWide->m_DetailTexture[1] && m_MapWide->m_DetailTexture[1]->IsLoaded())
					pd3dDevice->SetTexture(1,m_MapWide->m_DetailTexture[1]->GetTexture());
				else
					pd3dDevice->SetTexture(1,NULL);

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);				

				pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
				pd3dDevice->SetIndices(m_pROAMIndices,0);	
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

				CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
				CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			}			

			if(m_MapWide->m_WideMapTexture[2])
			{
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);		

				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

				if(m_MapWide->m_WideMapTexture[2]->IsLoaded())
					pd3dDevice->SetTexture(0,m_MapWide->m_WideMapTexture[2]->GetTexture());
				else
					pd3dDevice->SetTexture(0,NULL);
				if(m_MapWide->m_DetailTexture[2] && m_MapWide->m_DetailTexture[2]->IsLoaded())
					pd3dDevice->SetTexture(1,m_MapWide->m_DetailTexture[2]->GetTexture());
				else
					pd3dDevice->SetTexture(1,NULL);

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

				pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
				pd3dDevice->SetIndices(m_pROAMIndices,0);	
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

				CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
				CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			}
			if(m_ShadowUsed!=-1)
			{
				m_MapTexture=m_SectorShadowTexture[m_ShadowUsed].GetTexture();

				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );				
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);				
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
				

				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff999999);

				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
				//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,  D3DTOP_MODULATE2X);
				

				pd3dDevice->SetTexture(0,m_MapTexture);					
				pd3dDevice->SetTexture(1,NULL);					

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
				/*
				if(GF3OPTION)
				{
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					pd3dDevice->SetTexture(2,m_CloudShadow.GetTexture());
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,0);

					pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

					matrix matTrans;
					matTrans.MakeIdent();
					matTrans._11=0.11f;
					matTrans._22=0.11f;		
					float fCloudMoveX=m_AccumulateTM._41/SECTORSIZE;
					float fCloudMoveZ=m_AccumulateTM._43/SECTORSIZE;		
					matTrans._31=m_fCloudMove+fCloudMoveX*0.11f;
					matTrans._32=m_fCloudMove+fCloudMoveZ*0.11f;			


					pd3dDevice->SetTransform( D3DTS_TEXTURE2, matTrans );					
				}
				*/

				pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
				pd3dDevice->SetIndices(m_pROAMIndices,0);	
				pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

				CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
				CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
				/*
				if(GF3OPTION)
				{		
					pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
					pd3dDevice->SetTexture(2,NULL);
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,2);
					pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
				}
				*/

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);				
			}		
		}
	}
	else
	{
		if(m_ShadowUsed!=-1)
		{
			m_MapTexture=m_SectorShadowTexture[m_ShadowUsed].GetTexture();
		}		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROPW,   D3DTOP_MODULATE2X);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		if(m_MapTexture == NULL)		// Map Texture 가 없으면 무시 (04.02)
			return;
		pd3dDevice->SetTexture(0,m_MapTexture);	

		pd3dDevice->SetTexture(1,m_DetailTexture.GetTexture());		
		/*
		if(GF3OPTION)
		{
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTexture(2,m_CloudShadow.GetTexture());
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,0);
			pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);			

			matrix matTrans;
			matTrans.MakeIdent();
			matTrans._11=0.11f;
			matTrans._22=0.11f;		
			float fCloudMoveX=m_AccumulateTM._41/SECTORSIZE;
			float fCloudMoveZ=m_AccumulateTM._43/SECTORSIZE;		
			matTrans._31=m_fCloudMove+fCloudMoveX*0.11f;
			matTrans._32=m_fCloudMove+fCloudMoveZ*0.11f;			

			pd3dDevice->SetTransform( D3DTS_TEXTURE2, matTrans );
			
		}
		*/

		pd3dDevice->SetVertexShader(SectorFVF);
		pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
		pd3dDevice->SetIndices(m_pROAMIndices,0);	
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

		CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
		CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
		/*
		if(GF3OPTION)
		{		
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
			pd3dDevice->SetTexture(2,NULL);
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,2);
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
		}
		*/

	}
	matTex.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				

	//RenderTerrainSelfShadow(pd3dDevice);
	if(m_isEdit)
	{			
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff707070);

		pd3dDevice->SetTexture(0,NULL);
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		pd3dDevice->SetVertexShader(SectorFVF);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);		
		pd3dDevice->SetTexture(1,m_DetailTexture.GetTexture());		
		if(m_MapMustDivideVertex)
		{
			LVertex DivideVertex[2];
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffff0000);		
			for(int cVertex=0;cVertex<(int)m_MapMustDivideVertex->m_ptMustDivideVertexPosList.size();cVertex++)
			{			
				DivideVertex[0].v=m_SectorVertex[m_MapMustDivideVertex->m_ptMustDivideVertexPosList[cVertex].x+m_MapMustDivideVertex->m_ptMustDivideVertexPosList[cVertex].y*SECTORSX].v;
				DivideVertex[1].v=DivideVertex[0].v+vector3(0.0f,1000.0f,0.0f);			
				pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP,1,DivideVertex,sizeof(LVertex));			
			}
		}		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );			
	}
	/*
	if(m_ShadowUsed!=-1)
	{		
		m_MapTexture=m_SectorShadowTexture[m_ShadowUsed].GetTexture();
	}
	else
	{
		m_MapTexture=m_MapWide->m_WideMapTexture[0].GetTexture();
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	
	pd3dDevice->SetTexture(0,m_MapTexture);	
	pd3dDevice->SetTexture(1,m_DetailTexture.GetTexture());

	pd3dDevice->SetVertexShader(SectorFVF);
	///*
	pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
	pd3dDevice->SetIndices(m_pROAMIndices,0);	
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

	CSceneManager::m_cRenderSectorPoly+=m_UsedIndices/3;
	CSceneManager::m_cRenderSectorVertex+=m_UsedVertex;
	*/
}

const int TERRAINLIGHTMAPSIZE=256;

class ShadowObjectCollisionPoly
{
public:
	vector3 m_vecPos;
	float m_fRad;
	std::vector<vector3> m_PolyList;
};

class ShadowHousePoly
{
public:
	CHouseObjectScene *m_House;
	matrix m_matHousePos;
};
void CSectorScene::GenerateTerrainLightmap(LPDIRECT3DDEVICE8 pd3dDevice,CSectorScene *NeighborSector[8])
{
	vector3 vecSectorMovePos[8];
	vecSectorMovePos[0]=vector3(-SECTORSIZE,0.0f,-SECTORSIZE);
	vecSectorMovePos[1]=vector3(0.0f,0.0f,-SECTORSIZE);
	vecSectorMovePos[2]=vector3(SECTORSIZE,0.0f,-SECTORSIZE);

	vecSectorMovePos[3]=vector3(-SECTORSIZE,0.0f,0.0f);
	vecSectorMovePos[4]=vector3(SECTORSIZE,0.0f,0.0f);

	vecSectorMovePos[5]=vector3(-SECTORSIZE,0.0f,SECTORSIZE);
	vecSectorMovePos[6]=vector3(0.0f,0.0f,SECTORSIZE);
	vecSectorMovePos[7]=vector3(SECTORSIZE,0.0f,SECTORSIZE);

	TLVertex TVertex[8];		
	float fTextureSize=256.0f;

	TVertex[0].v.x=0.0f;TVertex[1].v.x=0.0f;
	TVertex[2].v.x=fTextureSize;TVertex[3].v.x=fTextureSize;

	TVertex[1].v.y=0.0f;TVertex[3].v.y=0.0f;
	TVertex[0].v.y=fTextureSize;TVertex[2].v.y=fTextureSize;

	TVertex[0].tu=0.0f;TVertex[1].tu=0.0f;
	TVertex[3].tu=1.0f;TVertex[2].tu=1.0f;

	TVertex[0].tv=0.0f;TVertex[2].tv=0.0f;
	TVertex[1].tv=1.0f;TVertex[3].tv=1.0f;
	int i,cSector;
	int ix,iy;

	for(i=0;i<4;i++)
	{			
		TVertex[i].Diffuse.c=0xffaaaaaa;
		TVertex[i].Specular.c=0x0;
		TVertex[i].w=0.1f;
		TVertex[i].v.z=0.1f;
	}
	
		

	vector3 vecSector[SECTORSX*SECTORSY];
	SectorVertex *pEditSectorVertex;
	WORD *pEditSectorIndices;



	m_pROAMVertex->Lock(0,0,(BYTE**)&pEditSectorVertex,0);
	m_pROAMIndices->Lock(0,(SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE*sizeof(WORD),(BYTE**)&pEditSectorIndices,0);		
	WORD *pSectorIndices=pEditSectorIndices;
	
	for(ix=0;ix<SECTORSX;ix++)
	{
		for(iy=0;iy<SECTORSY;iy++)
		{
			pEditSectorVertex[ix+iy*SECTORSX]=m_SectorVertex[ix+iy*SECTORSX];
			vecSector[ix+iy*SECTORSX]=m_SectorVertex[ix+iy*SECTORSX].v;
		}
	}

	for(ix=0;ix<SECTORSX-1;ix++)
	{
		for(iy=0;iy<SECTORSY-1;iy++)
		{
			 *(pEditSectorIndices++)=(ix+0)+(iy+0)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
			 
			 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+1)+(iy+1)*SECTORSX;
			 
		}
	}	
	m_UsedVertex=SECTORSX*SECTORSY;
	m_UsedIndices=(SECTORSX-1)*(SECTORSY-1)*6;		

	m_ShadowVolume.MakeShadowVolumeDirectionLight(vecSector,m_UsedVertex,pSectorIndices,m_UsedIndices/3,vector3(-1.0f,-1.0f,-1.0f));

	m_pROAMVertex->Unlock();
	m_pROAMIndices->Unlock();

	for(cSector=0;cSector<8;cSector++)
	{
		NeighborSector[cSector]->m_pROAMVertex->Lock(0,0,(BYTE**)&pEditSectorVertex,0);
		NeighborSector[cSector]->m_pROAMIndices->Lock(0,(SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE*sizeof(WORD),(BYTE**)&pEditSectorIndices,0);		
		WORD *pSectorIndices=pEditSectorIndices;
		for(ix=0;ix<SECTORSX;ix++)
		{
			for(iy=0;iy<SECTORSY;iy++)
			{
				pEditSectorVertex[ix+iy*SECTORSX]=NeighborSector[cSector]->m_SectorVertex[ix+iy*SECTORSX];
				vecSector[ix+iy*SECTORSX]=NeighborSector[cSector]->m_SectorVertex[ix+iy*SECTORSX].v+vecSectorMovePos[cSector];
			}
		}

		for(ix=0;ix<SECTORSX-1;ix++)
		{
			for(iy=0;iy<SECTORSY-1;iy++)
			{
				 *(pEditSectorIndices++)=(ix+0)+(iy+0)*SECTORSX;
				 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
				 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
				 
				 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
				 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
				 *(pEditSectorIndices++)=(ix+1)+(iy+1)*SECTORSX;
				 
			}
		}	
		NeighborSector[cSector]->m_UsedVertex=SECTORSX*SECTORSY;
		NeighborSector[cSector]->m_UsedIndices=(SECTORSX-1)*(SECTORSY-1)*6;				
		m_ShadowVolume.MakeShadowVolumeDirectionLight(vecSector,m_UsedVertex,pSectorIndices,m_UsedIndices/3,vector3(-1.0f,-1.0f,-1.0f));
		NeighborSector[cSector]->m_pROAMVertex->Unlock();
		NeighborSector[cSector]->m_pROAMIndices->Unlock();
	}
	


	LVertex pVertex[4];

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=SECTORSIZE;
	pVertex[3].v.x=SECTORSIZE;

	pVertex[1].v.z=0.0f;
	pVertex[3].v.z=0.0f;
	pVertex[0].v.z=SECTORSIZE;
	pVertex[2].v.z=SECTORSIZE;				

	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;
	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=1.0f;
	pVertex[3].tv=1.0f;
	pVertex[0].tv=0.0f;
	pVertex[2].tv=0.0f;	
	
	for(i=0;i<4;i++)
	{		
		pVertex[i].v.y=0.0f;
		pVertex[i].diff.c=0x0;
		pVertex[i].spec.c=0;
	}	

	int cShadowTexture=m_ShadowUsed;
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_POINT);

	pd3dDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	matrix matOldProjection,matProjection;
	matrix matOldView,matView,matInit;
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matOldProjection);
	pd3dDevice->GetTransform(D3DTS_VIEW,matOldView);
	
	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matInit);
	
	//matProjection.MakeProjection(3.14159f/3.0f,1.0f,50,100000.0f);	
	D3DXMatrixOrthoLH(matProjection,SECTORSIZE,SECTORSIZE,10.0f, 100000.0f);

	float fViewHeight=(SECTORSIZE/2.0f)/tanf(3.14159f/6.0f);
	//float fViewHeight=60000.0f;

	matView.CameraLookAt(vector3(SECTORSIZE/2.0f,fViewHeight,SECTORSIZE/2.0f),vector3(SECTORSIZE/2.0f,0.0f,SECTORSIZE/2.0f),vector3(0.0f,0.0f,1.0f));
	
	pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);
	pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);	
	
	
	m_SwapShadowTexture.Begin(pd3dDevice);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xfffffff, 1.0f, 0);	
	
	m_SwapShadowTexture.End(pd3dDevice);
	
	/////////Terrain Shade Texturing///////
	m_TerrainShadeTexture.Begin(pd3dDevice);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|D3DCLEAR_STENCIL,0xfffffff, 1.0f, 0);	
	pd3dDevice->BeginScene();

	D3DLIGHT8 TerrainLight; 
	ZeroMemory(&TerrainLight,sizeof(D3DLIGHT8));
    TerrainLight.Type=D3DLIGHT_DIRECTIONAL;
    TerrainLight.Diffuse.r=1.0f;
    TerrainLight.Diffuse.g=1.0f;
    TerrainLight.Diffuse.b=1.0f;	
	vector3 vecLight(-1.0f,-1.0f,-1.0f);
	vecLight.Normalize();
	TerrainLight.Position.x=TerrainLight.Direction.x=vecLight.x;
    TerrainLight.Position.y=TerrainLight.Direction.y=vecLight.y;
    TerrainLight.Position.z=TerrainLight.Direction.z=vecLight.z;
    TerrainLight.Range=1000.0f;	
    pd3dDevice->SetLight( 0,&TerrainLight);		

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_CURRENT);   
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	LightVertex TerrainShadeVertex[SECTORSX*SECTORSY];
	WORD TerrainShadeIndices[(SECTORSX-1)*(SECTORSY-1)*6];

	CSectorHeightMap *pXNeighborMapHeight,*pYNeighborMapHeight;
	pXNeighborMapHeight=m_pMapStorage->FindHeightMap(m_TM._41+SECTORSIZE,m_TM._43);
	pYNeighborMapHeight=m_pMapStorage->FindHeightMap(m_TM._41,m_TM._43+SECTORSIZE);
	
	vector3 n;
	
	long a,b,c;
	vector3 *v1,*v2,*v3;
	vector3 tempv2,tempv3;
	for(ix=0;ix<SECTORSX;ix++)
	{
		for(iy=0;iy<SECTORSX;iy++)
		{   			
			a=ix+iy*SECTORSX;
			b=ix+(iy+1)*SECTORSX;
			c=ix+1+iy*SECTORSX;				

			TerrainShadeVertex[a].v=m_SectorVertex[a].v;
			TerrainShadeVertex[a].spec.c=0x0;
			TerrainShadeVertex[a].tu=0.0f;
			TerrainShadeVertex[a].tv=0.0f;
			
			if(ix!=SECTORSX-1 && iy!=SECTORSX-1)
			{
				v1 =&m_SectorVertex[a].v;
				v2 =&m_SectorVertex[b].v;
				v3 =&m_SectorVertex[c].v;

				n = (*v2-*v1)^(*v3-*v2);
				//n.y*=0.2f;
				n.Normalize();				
			}
			if(ix==SECTORSX-1 && iy==SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				tempv2.x=v1->x;
				tempv2.z=SECTORSIZE+LINTERVAL;
				tempv3.x=SECTORSIZE+LINTERVAL;
				tempv3.z=v1->z;

				if(pXNeighborMapHeight)
					tempv2.y=pXNeighborMapHeight->m_pHeightData[1+iy*SECTORSX];
				else
					tempv2.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+SECTORSIZE+LINTERVAL,0.0f));


				if(pYNeighborMapHeight)
					tempv3.y=pYNeighborMapHeight->m_pHeightData[ix+1*SECTORSX];
				else
					tempv3.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+SECTORSIZE+LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));

				n = (tempv2-*v1)^(*v3-tempv2);
				//n.y*=0.2f;
				n.Normalize();
			}
			if(ix==SECTORSX-1 && iy!=SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				v2=&m_SectorVertex[b].v;
				tempv3.x=SECTORSIZE+LINTERVAL;
				tempv3.z=v1->z;
				if(pXNeighborMapHeight)
					tempv3.y=pXNeighborMapHeight->m_pHeightData[1+iy*SECTORSX];
				else
					tempv3.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+SECTORSIZE+LINTERVAL,m_TM._43+iy*LINTERVAL,0.0f));

				n = (*v2-*v1)^(tempv3-*v2);
				//n.y*=0.2f;
				n.Normalize();
			}
			if(ix!=SECTORSX-1 && iy==SECTORSX-1)
			{
				v1=&m_SectorVertex[a].v;
				tempv2.x=v1->x;
				tempv2.z=SECTORSIZE+LINTERVAL;				
				v3=&m_SectorVertex[c].v;
				
				if(pYNeighborMapHeight)
					tempv2.y=pYNeighborMapHeight->m_pHeightData[ix+1*SECTORSX];
				else
					tempv2.y=CPerlinNoise::HybridMultifractal(vector3(m_TM._41+ix*LINTERVAL,m_TM._43+SECTORSIZE+LINTERVAL,0.0f));

				n = (tempv2-*v1)^(*v3-tempv2);
				//n.y*=0.2f;
				n.Normalize();
			}	
			TerrainShadeVertex[a].n=n;
			
		}
    }	
	pEditSectorIndices=TerrainShadeIndices;

	for(ix=0;ix<SECTORSX-1;ix++)
	{
		for(iy=0;iy<SECTORSY-1;iy++)
		{
			 *(pEditSectorIndices++)=(ix+0)+(iy+0)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
			 
			 *(pEditSectorIndices++)=(ix+1)+(iy+0)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+0)+(iy+1)*SECTORSX;
			 *(pEditSectorIndices++)=(ix+1)+(iy+1)*SECTORSX;
			 
		}
	}	

	pd3dDevice->SetVertexShader(LightFVF);
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,m_UsedVertex,m_UsedIndices/3,TerrainShadeIndices,D3DFMT_INDEX16,TerrainShadeVertex,sizeof(LightVertex));
	/*
	pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
	pd3dDevice->SetIndices(m_pROAMIndices,0);
	pd3dDevice->SetVertexShader(SectorFVF);
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);			
	*/
	///	
	
	RenderTerrainSelfShadow(pd3dDevice);
	m_ShadowVolume.Reset();		
	
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
    pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
    pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );    
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);    	
	
    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);	
	
    
    pd3dDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
    pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
    pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );		

	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);	
	pd3dDevice->SetTexture(0,NULL);	
	pd3dDevice->SetTexture(1,NULL);	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,TVertex,sizeof(TLVertex));    
    pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE);
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );		
	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,FALSE);	
	pd3dDevice->EndScene();
	m_TerrainShadeTexture.End(pd3dDevice);		
	//////////////////////	
	
	


	matrix matWorld,matObjectCamera;
	matrix mat,mat2;
	matrix matTexScale,matInv,matTex;
	matTexScale.MakeIdent();
	matTexScale._11=0.5f;
	matTexScale._22=-0.5f;	
	matTexScale._33=0.0f;
	matTexScale._41=0.5f;
	matTexScale._42=0.5f;
	matTexScale._43=1.0f;
	matTexScale._44=1.0f;

	matInv.Inverse(matView);	

	//pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);	
	pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);

	int cPlant=0;
	unsigned char usTreeKind[SECTORSX*SECTORSX*10];
	vector3 vecTreePos[SECTORSX*SECTORSX*10];

	for(ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind[cPlant]=m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]&0x3f;
				vecTreePos[cPlant].x=ix*LINTERVAL;
				vecTreePos[cPlant].z=iy*LINTERVAL;
				cPlant++;
			}
		}
	}
	
	
	for(cSector=0;cSector<8;cSector++)
	{
		for(int ix=0;ix<SECTORSX;ix++)
		{
			for(int iy=0;iy<SECTORSY;iy++)
			{
				if(NeighborSector[cSector]->m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]!=0xff)
				{
					usTreeKind[cPlant]=NeighborSector[cSector]->m_TreeObjectNode.m_usDetailTree[ix+iy*SECTORSX]&0x3f;
					vecTreePos[cPlant].x=ix*LINTERVAL+vecSectorMovePos[cSector].x;
					vecTreePos[cPlant].z=iy*LINTERVAL+vecSectorMovePos[cSector].z;
					cPlant++;
				}
			}
		}
	}
	matWorld.MakeIdent();

	TLVertex Vertex[4];
	Vertex[0].Diffuse.c=Vertex[1].Diffuse.c=Vertex[2].Diffuse.c=Vertex[3].Diffuse.c=0xffff0000;
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

	for(int cTree=0;cTree<cPlant;cTree++)
	{		
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);   
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);	

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
		
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x11);
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);	
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);		

		m_ObjectShadowTexture.Begin(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
		
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);	

		pd3dDevice->BeginScene();		
		float fHeight=7000.0f;
		//float fHeight=m_HouseObjectNode[cHouse]->m_fRad+7000.0f;
		//if( usTreeKind[cTree]>=0 &&	usTreeKind[cTree] <MAX_TREEKIND)
			//m_TreeObjectNode.m_NormalTreeMesh[usTreeKind[cTree]]->Render(pd3dDevice);		
		matWorld._41=vecTreePos[cTree].x;
		matWorld._43=vecTreePos[cTree].z;
		
		matObjectCamera.CameraLookAt(vector3(matWorld._41+fHeight/2.0f,fHeight,matWorld._43+fHeight/2.0f),
			vector3(matWorld._41,0.0f,matWorld._43),vector3(0.0f,0.0f,1.0f));
		
		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);
		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);


		pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
		if( usTreeKind[cTree]>=0 &&	usTreeKind[cTree] <MAX_TREEKIND)
			m_TreeObjectNode.m_NormalTreeMesh[usTreeKind[cTree]]->Render(pd3dDevice);		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
		pd3dDevice->SetVertexShader(TLVERTEXFVF);		
		//pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,4,4,pLineIndices,D3DFMT_INDEX16,Vertex,sizeof(TLVertex));

		pd3dDevice->EndScene();
		m_ObjectShadowTexture.End(pd3dDevice);
		
		mat=matProjection*matTexScale;
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;
		
		pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
		pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matInit);
		
		m_SwapShadowTexture.Begin(pd3dDevice);

		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(0,m_ObjectShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		
		pd3dDevice->SetVertexShader(LVERTEXFVF);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));			
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		
		
		pd3dDevice->EndScene();	
		m_SwapShadowTexture.End(pd3dDevice);		

	}
	int cHouse=0;
	std::vector<ShadowHousePoly> ShadowHouseList;
	for(cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{
		ShadowHousePoly AddNode;		
		AddNode.m_House=m_HouseObjectNode[cHouse];
		matrix matAbsHouse;
		matAbsHouse=m_HouseObjectNode[cHouse]->m_AccumulateTM;
		matAbsHouse._41-=m_AccumulateTM._41;
		matAbsHouse._43-=m_AccumulateTM._43;
		AddNode.m_matHousePos=matAbsHouse;
		ShadowHouseList.push_back(AddNode);		
	}
	for(cSector=0;cSector<8;cSector++)
	{
		for(cHouse=0;cHouse<(int)NeighborSector[cSector]->m_HouseObjectNode.size();cHouse++)
		{
			ShadowHousePoly AddNode;		
			AddNode.m_House=NeighborSector[cSector]->m_HouseObjectNode[cHouse];
			matrix matAbsHouse;
			matAbsHouse=NeighborSector[cSector]->m_HouseObjectNode[cHouse]->m_AccumulateTM;
			matAbsHouse._41-=m_AccumulateTM._41;
			matAbsHouse._43-=m_AccumulateTM._43;			
			AddNode.m_matHousePos=matAbsHouse;
			ShadowHouseList.push_back(AddNode);	
		}		
	}

	for(cHouse=0;cHouse<(int)ShadowHouseList.size();cHouse++)
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

		m_ObjectShadowTexture.Begin(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);
		pd3dDevice->LightEnable(0,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|D3DCLEAR_STENCIL,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		//float fHeight=7000.0f;
		float fHeight=ShadowHouseList[cHouse].m_House->m_fRad+17000.0f;

		matWorld=ShadowHouseList[cHouse].m_matHousePos;
		
		matObjectCamera.CameraLookAt(vector3(matWorld._41+fHeight,matWorld._42+fHeight,matWorld._43+fHeight),
			vector3(matWorld._41,matWorld._42,matWorld._43),vector3(0.0f,0.0f,1.0f));

		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);

		//matObjectCamera.CameraLookAt(vector3(fHeight/2.0f,fHeight,fHeight/2.0f),vector3(0.0f,0.0f,0.0f),vector3(0.0f,1.0f,0.0f));				


		matrix matObjectProj;
		D3DXMatrixOrthoLH(matObjectProj,ShadowHouseList[cHouse].m_House->m_fRad*3.0f,ShadowHouseList[cHouse].m_House->m_fRad*3.0f,10.0f, 100000.0f);	
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matObjectProj);		
		
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);
		matWorld.MakeIdent();
		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
		pd3dDevice->SetIndices(m_pROAMIndices,0);	
		pd3dDevice->SetVertexShader(SectorFVF);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
		
		
		matWorld=ShadowHouseList[cHouse].m_matHousePos;		

		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		if(ShadowHouseList[cHouse].m_House->m_HouseObject->m_pOutHouseObject)
			ShadowHouseList[cHouse].m_House->m_HouseObject->m_pOutHouseObject->Render(pd3dDevice);		
		else
		{
			//if(ShadowHouseList[cHouse].m_House->m_HouseObject->m_pLodHouseObject)				
			//	ShadowHouseList[cHouse].m_House->m_HouseObject->m_pLodHouseObject->Render(pd3dDevice);
		}

		pd3dDevice->EndScene();
		m_ObjectShadowTexture.End(pd3dDevice);
		///////////////////////////////////////////////////////		
		mat=matObjectProj*matTexScale;		
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;
		
		pd3dDevice->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&matProjection);
		pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matView);
		pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matInit);
		
		m_SwapShadowTexture.Begin(pd3dDevice);

		//pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER,0xffffffff, 1.0f, 0);

		pd3dDevice->BeginScene();		
		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(0,m_ObjectShadowTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		 
		pd3dDevice->SetVertexShader(LVERTEXFVF);		

		pd3dDevice->SetTransform(D3DTS_TEXTURE0,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		//pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(LVertex));		
		//pd3dDevice->SetTransform(D3DTS_WORLD,m_AccumulateTM);				
		pd3dDevice->SetStreamSource(0,m_pROAMVertex,sizeof(SectorVertex));
		pd3dDevice->SetIndices(m_pROAMIndices,0);	
		pd3dDevice->SetVertexShader(SectorFVF);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_UsedVertex,0,m_UsedIndices/3);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
		
		pd3dDevice->EndScene();		
		m_SwapShadowTexture.End(pd3dDevice);
	}

	for(i=0;i<4;i++)
	{		
		pVertex[i].v.y=0.0f;
		pVertex[i].diff.c=0xffaaaaaa;
		pVertex[i].spec.c=0;
	}	

	for(i=0;i<4;i++)
	{			
		TVertex[i].Diffuse.c=0xff555555;
		TVertex[i].Specular.c=0x0;
		TVertex[i].w=0.1f;
		TVertex[i].v.z=0.1f;
	}
	
	m_ObjectShadowTexture.Begin(pd3dDevice);
	pd3dDevice->BeginScene();			

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD);	

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);	

	pd3dDevice->SetTexture(0,m_SwapShadowTexture.GetTexture());
	pd3dDevice->SetTexture(1,m_TerrainShadeTexture.GetTexture());	

	pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,TVertex,sizeof(TLVertex));
	pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
	pd3dDevice->EndScene();
	m_ObjectShadowTexture.End(pd3dDevice);

//	CConvertTexture ShadowTexture;
//	ShadowTexture.m_pddTexture=m_ObjectShadowTexture.GetTexture();
//	ShadowTexture.m_dwHeight=TERRAINLIGHTMAPSIZE;
//	ShadowTexture.m_dwWidth=TERRAINLIGHTMAPSIZE;
//	ShadowTexture.GenerateMipMaps(true);
//	ShadowTexture.m_pddTexture=ShadowTexture.m_pddNewTexture;
//	ShadowTexture.m_pddNewTexture=NULL;
//	ShadowTexture.Compress(D3DFMT_A8R8G8B8);
//	DWORD *pShadowTexture=(DWORD*)ShadowTexture.Lock();
//
//	CConvertTexture WideTexture;		
//	memset(m_SectorShadowCollision[m_ShadowUsed],0,TERRAINLIGHTMAPSIZE*TERRAINLIGHTMAPSIZE*sizeof(char));
//
//	for(int cWideTexture=0;cWideTexture<3;cWideTexture++)
//	{
//		if(strcmp(m_MapWide->m_strWidemapName[cWideTexture],"")!=0)
//		{
//			CConvertTexture DeCompressTexture;
//			char strWideTexture[256];			
//			sprintf(strWideTexture,"%s\\%s",UNSHADOWWIDETEXTUREPATH,m_MapWide->m_strWidemapName[cWideTexture]);
//			LPDIRECT3DTEXTURE8 pddTexture=NULL;
//			//D3DXCreateTextureFromFile(pd3dDevice,strWideTexture,&pddTexture);			
//
//			D3DXCreateTextureFromFileEx(pd3dDevice,strWideTexture,TERRAINLIGHTMAPSIZE,TERRAINLIGHTMAPSIZE
//				,0,D3DPOOL_DEFAULT,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&pddTexture);
//			if(pddTexture==NULL)
//			{
//				MessageBox(NULL,strWideTexture,0,0);
//			}
//
//
//			DeCompressTexture.m_pddTexture=pddTexture;				
//			//DeCompressTexture.m_pddTexture=m_MapWide->m_WideMapTexture[cWideTexture].GetTexture();
//			DeCompressTexture.m_dwHeight=TERRAINLIGHTMAPSIZE;
//			DeCompressTexture.m_dwWidth=TERRAINLIGHTMAPSIZE;
//			DeCompressTexture.Compress(D3DFMT_A8R8G8B8);			
//			DeCompressTexture.m_pddTexture=DeCompressTexture.m_pddNewTexture;
//
//			DWORD *pWideTexture=(DWORD*)DeCompressTexture.Lock();
//			color TerrainColor,ShadowColor;
//			for(int nPixel=0;nPixel<TERRAINLIGHTMAPSIZE*TERRAINLIGHTMAPSIZE;nPixel++)
//			{
//				TerrainColor.c=pWideTexture[nPixel];
//				ShadowColor.c=pShadowTexture[nPixel];
//
//				float fTerrainR=TerrainColor.r/255.0f;
//				float fTerrainG=TerrainColor.g/255.0f;
//				float fTerrainB=TerrainColor.b/255.0f;				
//
//				float fShadowR=ShadowColor.r/255.0f;
//				float fShadowG=ShadowColor.g/255.0f;
//				float fShadowB=ShadowColor.b/255.0f;				
//				/*
//				fShadowR*=2.0f;
//				fShadowG*=2.0f;
//				fShadowB*=2.0f;
//
//				fShadowR*=fShadowR;
//				fShadowG*=fShadowG;
//				fShadowB*=fShadowB;
//
//				fShadowR/=2.0f;
//				fShadowG/=2.0f;
//				fShadowB/=2.0f;
//
//				if(fShadowR>1.0f)
//					fShadowR=1.0f;
//				if(fShadowG>1.0f)
//					fShadowG=1.0f;
//				if(fShadowB>1.0f)
//					fShadowB=1.0f;
//					*/
//				
//
//				if(fShadowR <= 0.8f)
//				{
//					m_SectorShadowCollision[m_ShadowUsed][nPixel]=1;
//				}
//				/*
//				if(fTerrainG >= fTerrainR &&  fTerrainG >= fTerrainB)
//					m_SectorShadowCollision[m_ShadowUsed][nPixel]|=0x80;				
//					*/
//				
//
//				fTerrainR=fTerrainR*fShadowR*1.0f;
//				fTerrainG=fTerrainG*fShadowG*1.0f;
//				fTerrainB=fTerrainB*fShadowB*1.0f;
//
//				if(fTerrainR>=1.0f)
//					fTerrainR=1.0f;
//				if(fTerrainG>=1.0f)
//					fTerrainG=1.0f;
//				if(fTerrainB>=1.0f)
//					fTerrainB=1.0f;
//
//				TerrainColor.r=fTerrainR*255;
//				TerrainColor.g=fTerrainG*255;
//				TerrainColor.b=fTerrainB*255;
//
//				pWideTexture[nPixel]=TerrainColor.c;
//				
//			}
//			DeCompressTexture.Unlock();						
//			
//
//			D3DXFilterTexture(DeCompressTexture.m_pddNewTexture,NULL,0,D3DX_FILTER_BOX | D3DX_FILTER_DITHER);
//			DeCompressTexture.GenerateMipMaps(true);
//
//			DeCompressTexture.Compress(D3DFMT_DXT3);			
//			char strTextureFileName[256];
//			sprintf(strTextureFileName,"%s\\%s",WIDETEXTUREPATH,m_MapWide->m_strWidemapName[cWideTexture]);
//			DeCompressTexture.SaveDDS(strTextureFileName);
//		}
//	}
//	ShadowTexture.Unlock();
//	GenerateWideMap();		
//	
//	char strGrassAlphaTexture[256];			
//	sprintf(strGrassAlphaTexture,"%s\\%d_%d_Grass.dds",UNSHADOWWIDETEXTUREPATH,(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));	
//
//	LPDIRECT3DTEXTURE8 pddGrassTexture=NULL;
//	D3DXCreateTextureFromFileEx(pd3dDevice,strGrassAlphaTexture,TERRAINLIGHTMAPSIZE,TERRAINLIGHTMAPSIZE
//				,0,D3DPOOL_DEFAULT,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL,&pddGrassTexture);	
//
//	if(pddGrassTexture)
//	{
//		DWORD *pGrassTexture;
//
//		D3DLOCKED_RECT Rect;
//		pddGrassTexture->LockRect(0,&Rect,NULL,0);
//		pGrassTexture=(DWORD*)Rect.pBits;
//
//		for(int nPixel=0;nPixel<TERRAINLIGHTMAPSIZE*TERRAINLIGHTMAPSIZE;nPixel++)
//		{		
//			color GrassAlpha;
//			GrassAlpha.c=pGrassTexture[nPixel];
//			if(GrassAlpha.a>0x88)
//				m_SectorShadowCollision[m_ShadowUsed][nPixel]|=0x80;
//		}
//		pddGrassTexture->UnlockRect(0);
//		pddGrassTexture->Release();	
//	}
//	char strGrassSectorName[256];	
//	sprintf(strGrassSectorName,"%s\\GrassTable%d_%d",MAPDATAPATH,(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));
//	FILE *fp=fopen(strGrassSectorName,"wb");
//	fwrite(m_SectorShadowCollision[m_ShadowUsed],sizeof(char)*256*256,1,fp);
//	fclose(fp);	
//
//	CTexture CGrassShadowTexture;
//	CGrassShadowTexture.CreateEmpty(256,256);
//
//	DWORD *pGrassShadow=(DWORD*)CGrassShadowTexture.Lock();
//	pShadowTexture=(DWORD*)ShadowTexture.Lock();
//
//	for(ix=0;ix<TERRAINLIGHTMAPSIZE;ix++)
//	{
//		for(int iy=0;iy<TERRAINLIGHTMAPSIZE;iy++)
//		{	
//			color GrassColor;
//			//GrassColor.c=0x00ffffff;
//			/*
//			if(m_SectorShadowCollision[m_ShadowUsed][ix+iy*TERRAINLIGHTMAPSIZE] & 0x01)
//			{
//				GrassColor.r=0x55;
//				GrassColor.g=0x55;
//				GrassColor.b=0x55;
//				
//			}
//			*/
//			GrassColor.c=pShadowTexture[ix+iy*TERRAINLIGHTMAPSIZE];
//			GrassColor.a=0x0;
//
//			if(m_SectorShadowCollision[m_ShadowUsed][ix+iy*TERRAINLIGHTMAPSIZE] & 0x80)
//			{
//				GrassColor.a=0xff;
//			}
//			
//			
//			*pGrassShadow=GrassColor.c;
//			pGrassShadow++;
//		}		
//	}
//
//	CGrassShadowTexture.Unlock();	
//	ShadowTexture.Unlock();
//
//	char strTextureFileName[256];
//	sprintf(strTextureFileName,"%s\\GrassTexture%d_%d.dds",WIDETEXTUREPATH,(int)(m_AccumulateTM._41/SECTORSIZE),(int)(m_AccumulateTM._43/SECTORSIZE));
//	
//	CConvertTexture ConvertTexture;
//	ConvertTexture.m_pddTexture=CGrassShadowTexture.GetTexture();
//	ConvertTexture.m_dwHeight=TERRAINLIGHTMAPSIZE;
//	ConvertTexture.m_dwWidth=TERRAINLIGHTMAPSIZE;
//	ConvertTexture.GenerateMipMaps(true);
//	ConvertTexture.m_pddTexture=ConvertTexture.m_pddNewTexture;
//	ConvertTexture.m_pddNewTexture=NULL;
//	ConvertTexture.Compress(D3DFMT_DXT3);
//	//ConvertTexture.SaveDDS("c:/test.dds");	
//	ConvertTexture.SaveDDS(strTextureFileName);	
//	
//	
//
//	/*
//	CConvertTexture DeCompressTexture;
//	DeCompressTexture.m_pddTexture=ConvertTexture.m_pddTexture;
//	DeCompressTexture.m_dwHeight=TERRAINLIGHTMAPSIZE;
//	DeCompressTexture.m_dwWidth=TERRAINLIGHTMAPSIZE;
//	DeCompressTexture.Compress(D3DFMT_A8R8G8B8);
//	DeCompressTexture.SaveDDS("c:/ttt.dds");
//	*/
//	
//	GeneratePlant();
//
//	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProjection);
//
//	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
//
//	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
//	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
//	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
//	pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
}

float CSectorScene::GetHighPolyHeight(vector3 vecPos)
{
	int ix=((float)vecPos.x/(float)LINTERVAL);
	int iy=((float)vecPos.z/(float)LINTERVAL);	

	float fInterLens;

	vector3 vecPoly[3];
	vecPoly[0]=m_SectorVertex[(ix+0)+(iy+0)*SECTORSX].v;
	vecPoly[1]=m_SectorVertex[(ix+0)+(iy+1)*SECTORSX].v;
	vecPoly[2]=m_SectorVertex[(ix+1)+(iy+0)*SECTORSX].v;

	if(CIntersection::PolygonRay(vector3(vecPos.x,0.0f,vecPos.z),
								 vector3(vecPos.x,100000000.0f,vecPos.z),
								 vecPoly,fInterLens))
	{
		return fInterLens;
	}

	vecPoly[0]=m_SectorVertex[(ix+1)+(iy+0)*SECTORSX].v;
 	vecPoly[1]=m_SectorVertex[(ix+0)+(iy+1)*SECTORSX].v;
	vecPoly[2]=m_SectorVertex[(ix+1)+(iy+1)*SECTORSX].v;	

	if(CIntersection::PolygonRay(vector3(vecPos.x,0.0f,vecPos.z),
								 vector3(vecPos.x,100000000.0f,vecPos.z),
								 vecPoly,fInterLens))
	{
		return fInterLens;
	}
	return 0.0f;
}


void CSectorScene::RenderTerrainSelfShadow(LPDIRECT3DDEVICE8 pd3dDevice)
{
	
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
    pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );
    pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
    pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
    pd3dDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
    pd3dDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
    pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
    pd3dDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_INCR );
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    
    pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW);    
    m_ShadowVolume.Render(pd3dDevice );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW);    
	pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );            
    m_ShadowVolume.Render(pd3dDevice );    
    pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );    
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );   	
}

void CSectorScene::FullVertexROAMVertex()
{

}
void CSectorScene::UpdateLandscapeEffect(){
	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
	vector3 vecViewPos=matViewPosition->GetLoc();

	if(m_MapLandscapeEffect)
	{
		for(int cEffect=0;cEffect<(int)m_MapLandscapeEffect->m_LandscapeEffect.size();cEffect++)
		{
			switch(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod)
			{
				case 1:
					{
						vector3 *pos;

						pos=CSceneManager::GetCamera()->GetPosition();

					/*	if(CSceneManager::m_CharacterManager.m_CharacterList.num >0) {
			
							//CSceneManager::m_CharacterList[0]->GetPosition(CharPos.x,CharPos.y,CharPos.z);
							vector3 CharPos;
							CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);
							pos=&CharPos;
						}
						*/
						((CFogScene*)m_LandscapeEffectNode[cEffect])->Update(pos->x,pos->y,pos->z);
						
					}
					break;
				case 2:
					{
						((CBoidGroup*)m_LandscapeEffectNode[cEffect])->Update();
					}
					break;

				case 3: // morphing
					{
						
							
							
							D3DXVECTOR3 tmp_vec;
							D3DXVECTOR3 tmp_user;

							tmp_vec.x = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x;
							tmp_vec.y = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.y;
							tmp_vec.z = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z;
							
							tmp_user.x =  vecViewPos.x;
							tmp_user.y =  vecViewPos.y;
							tmp_user.z =  vecViewPos.z;

							
							
							((CGemRender *)m_LandscapeEffectNode[cEffect])->Update(tmp_vec,tmp_user);			
						
					}
					break;
					case 4: {
						((CWaveLine *)m_LandscapeEffectNode[cEffect])->Update();
					}
					break;

					case 5: {
						vector3 pos;
						if((int)CSceneManager::m_CharacterManager.m_CharacterList.size() >0) {
			
							//CSceneManager::m_CharacterList[0]->GetPosition(CharPos.x,CharPos.y,CharPos.z);
							vector3 CharPos;
							CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);
							pos=CharPos;
						}
						else {
							pos.x = pos.y= pos.z= 0.0f;

						}
						((CWaterW *)m_LandscapeEffectNode[cEffect])->Update(pos.x,pos.y,pos.z);
					}
					break;
			}
		}
	}	
}
void CSectorScene::RenderEffect(bool bCull){

/*
	if(m_MapEffect) {
		if(m_MapEffect->pick_index != -1)
			CSceneManager::m_EffectManager.Render(m_MapEffect->pick_index);
		else 
			CSceneManager::m_EffectManager.Render();
	}*/
	UpdateEffect(bCull);
	m_WorldEffectManager.RenderWorldScript(bCull);

}
void CSectorScene::UpdateEffect(bool bCull) {
	int snum = m_WorldEffectManager.GetWScriptNum();
	for(int i=0;i<snum;i++) {
		m_WorldEffectManager.ProcessWScript(i,bCull);
	}
}
void CSectorScene::RenderLandscapeEffect(LPDIRECT3DDEVICE8 pd3dDevice)
{

	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
	vector3 vecViewPos=matViewPosition->GetLoc();

	if(m_MapLandscapeEffect)
	{	// z buffer 문제 때문에 모핑 부터 먼저 그린다.
		for(int cEffect=0;cEffect<(int)m_MapLandscapeEffect->m_LandscapeEffect.size();cEffect++)
		{
		
			switch(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod)
			{
				case 1:
					{
							
						/*	matrix matWorld;
							matWorld.MakeIdent();
							pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
							((CFogScene*)m_LandscapeEffectNode[cEffect])->Render();
							//((CFogScene*)m_LandscapeEffectNode[cEffect])->Update(vecViewPos.x,vecViewPos.y,vecViewPos.z);
						*/	
					}
					break;
				case 2: {
					
						}
					break;

				case 3: // morphing
					{
						//object culling 적용
						if(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[4]) {
							matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
							vector3 vecViewPos=matViewPosition->GetLoc();
							int viewindexx=(int)(vecViewPos.x/SECTORSIZE);
							int viewindexy=(int)(vecViewPos.z/SECTORSIZE);
							int meshindexx=(int)((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x)/SECTORSIZE);
							int meshindexy=(int)((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z)/SECTORSIZE);
							if(!(viewindexx == meshindexx) || !(viewindexy == meshindexy)) {
								break;
							}
						}

						matrix matWorld;
							
						D3DXVECTOR3 tmp_vec;
						D3DXVECTOR3 tmp_user;

						tmp_vec.x = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x;
						tmp_vec.y = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.y;
						tmp_vec.z = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z;
								
						tmp_user.x =  vecViewPos.x;
						tmp_user.y =  vecViewPos.y;
						tmp_user.z =  vecViewPos.z;

						matWorld.MakeIdent();

						pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
						
						((CGemRender *)m_LandscapeEffectNode[cEffect])->SetEffectPos(tmp_vec.x,tmp_vec.y,tmp_vec.z);
						((CGemRender *)m_LandscapeEffectNode[cEffect])->Render();
					
						//((CGemRender *)m_LandscapeEffectNode[cEffect])->Update(tmp_vec,tmp_user);			
						
				}
				break;
				case 4: { //wave	
					((CWaveLine *)m_LandscapeEffectNode[cEffect])->Render();
				}
				break;
				case 5: { //water space
					((CWaterW *)m_LandscapeEffectNode[cEffect])->Render();

				}
				break;

			}	
			
		}
		for(int cEffect=0;cEffect<(int)m_MapLandscapeEffect->m_LandscapeEffect.size();cEffect++)
		{	
			
				switch(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod)
				{
					case 1:
						{
							
							matrix matWorld;
							matWorld.MakeIdent();
							pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
							((CFogScene*)m_LandscapeEffectNode[cEffect])->Render();
							//((CFogScene*)m_LandscapeEffectNode[cEffect])->Update(vecViewPos.x,vecViewPos.y,vecViewPos.z);
							
						}
						break;
					case 2:
						{
							
						/*	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
							vector3 vecViewPos=matViewPosition->GetLoc();
							
							int viewindexx=(int)(vecViewPos.x/SECTORSIZE);
							int viewindexy=(int)(vecViewPos.z/SECTORSIZE);
							int boidindexx=(int)((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x)/SECTORSIZE);
							int boidindexy=(int)((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z)/SECTORSIZE);
							if(!(viewindexx == boidindexx) || !(viewindexy == boidindexy)) {
								break;
							}
						*/
							float fTime = CSceneManager::GetWeatherTime();
							if(fTime <= 19.0f && fTime >= 5.0f) {
								((CBoidGroup*)m_LandscapeEffectNode[cEffect])->Render(pd3dDevice);
							}

						//	((CBoidGroup*)m_LandscapeEffectNode[cEffect])->Update();
							
								
						}
						break;

					case 3: // morphing
						{
							
						/*		matrix matWorld;
								
								D3DXVECTOR3 tmp_vec;
								//D3DXVECTOR3 tmp_user;

								tmp_vec.x = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x;
								tmp_vec.y = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.y;
								tmp_vec.z = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z;
								
								//tmp_user.x =  vecViewPos.x;
								//tmp_user.y =  vecViewPos.y;
								//tmp_user.z =  vecViewPos.z;

								matWorld.MakeIdent();
								pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
								((CMeshMorph *)m_LandscapeEffectNode[cEffect])->Render(pd3dDevice,tmp_vec);
							//	((CMeshMorph *)m_LandscapeEffectNode[cEffect])->UpdateMesh(tmp_vec,tmp_user);			
						*/	
						}
						break;
				}
			
		}
	}	
}
/*
void CSectorScene::GenerateLandscapeEffect(CVfogArg arg)
{
	char gem_name[255];

	m_MapLandscapeEffect=m_pMapStorage->FindLandscapeEffect(m_TM._41,m_TM._43);		

	for(int cEffect=0;cEffect<m_LandscapeEffectNode.num;cEffect++)
	{
		delete m_LandscapeEffectNode[cEffect];
	}

	m_LandscapeEffectNode.num=0;

	if(m_MapLandscapeEffect)
	{
		for(int cEffect=0;cEffect<m_MapLandscapeEffect->m_LandscapeEffect.num;cEffect++)
		{
			void *pAddNode=NULL;
			switch(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod)
			{
				case 1:
					// Volumetric Fog //
					{
						CFogScene *AddNode=new CFogScene(arg.m_Num);
						//////////////////
						AddNode->Create(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos);
						
						//set fog parameter
						AddNode->SetCenter(arg.m_Center);
						AddNode->SetCenterPos(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos);
						
						AddNode->SetFlow(2000.0f);
						AddNode->SetGravity(arg.m_Gx,arg.m_Gy,arg.m_Gz);
						vector3 tm;
						tm.z = arg.m_ImZ;
						tm.x = arg.m_ImX;
						tm.y = arg.m_ImY;
					//	tm.x = tm.y = tm.z = 0.0f;
					//	tm.x = 0.3f;
					//	tm.y = 0.1f;
						AddNode->SetSize(arg.m_Size);
						AddNode->SetImpress(tm);
						AddNode->SetPos();
						//SetColor(0.8f,0.8f,0.8f,0.5f);
						AddNode->SetVertexColor(arg.m_R1,arg.m_G1,arg.m_B1,arg.m_A1,0);
						AddNode->SetVertexColor(arg.m_R2,arg.m_G2,arg.m_B2,arg.m_A2,1);
						AddNode->SetVertexColor(arg.m_R3,arg.m_G3,arg.m_B3,arg.m_A3,2);
						AddNode->SetVertexColor(arg.m_R4,arg.m_G4,arg.m_B4,arg.m_A4,3);

						AddNode->SetEtc();
						AddNode->SetRad(arg.m_Rad);
						AddNode->SetSpeed(0.0f,0.0f,0.0f);
						AddNode->SetSpeedPower(0.0f);
						AddNode->SetWind(0.0f,0.0f,0.0f);
						AddNode->Setfadein(arg.m_FadeIn);
						AddNode->SetFadeInSpeed(arg.m_FadeInSpeed);	
						AddNode->SetFadeOutSpeed(3.0f);
						AddNode->ChangeVertexBuffer();

						////////////////////
						
						
						pAddNode=AddNode;
					}
					break;
				case 2:
					// Boid //
					{
						
					}
					break;
				case 3: 
					// object ani mesh
					{
						strcpy(gem_name,OBJECTGEMPATH);
						strcat(gem_name,m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Name);
						
						CMeshMorph *AddNode = new CMeshMorph();
						AddNode->LoadWadfile(gem_name,CSceneManager::GetDevice());
						pAddNode = AddNode;
					}
					break;
			}
			m_LandscapeEffectNode.Add(pAddNode);
		}
	}
}
*/
void CSectorScene::GenerateLandscapeEffect()
{
	char gem_name[255];
	float fp;
	int ip;

	m_MapLandscapeEffect=m_pMapStorage->FindLandscapeEffect(m_TM._41,m_TM._43);		
	
	for(int cEffect=0;cEffect<(int)m_LandscapeEffectNode.size();cEffect++)
	{
		switch(m_LandscapeEffectNodeValue[cEffect]) {
		case 1: //fog
			delete (CFogScene *)m_LandscapeEffectNode[cEffect];
			break;
		case 2: // boid
			delete (CBoidGroup *)m_LandscapeEffectNode[cEffect];
			break;
		case 3: // mesh ani
			delete (CGemRender *)m_LandscapeEffectNode[cEffect];
			break;
		case 4:
			delete (CWaveLine *)m_LandscapeEffectNode[cEffect];
			break;
		case 5:
			delete (CWaterW *)m_LandscapeEffectNode[cEffect];
			break;
		}
	}

	m_LandscapeEffectNode.clear();
	m_LandscapeEffectNodeValue.clear();


	if(m_MapLandscapeEffect)
	{
		char LoadedBoid[256]={0,};


		for(int cEffect=0;cEffect<(int)m_MapLandscapeEffect->m_LandscapeEffect.size();cEffect++)
		{
			void *pAddNode=NULL;
			switch(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod)
			{
				case 1:
					// Volumetric Fog //
					{	
					
							ip = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[0];
							CFogScene *AddNode=new CFogScene(ip);

							AddNode->Create(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos);
							//set fog parameter
							fp = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[16];
							AddNode->SetCenter(fp);

							AddNode->SetCenterPos(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos);
							
							AddNode->SetGravity(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[20],
												m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[21],
												m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[22]);
							vector3 tm;
							tm.z = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[25];
							tm.x = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[23];
							tm.y = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[24];
						//	tm.x = tm.y = tm.z = 0.0f;
						//	tm.x = 0.3f;
						//	tm.y = 0.1f;
							AddNode->SetSize(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[27]);
							AddNode->SetImpress(tm);
							AddNode->SetPos();
							//SetColor(0.8f,0.8f,0.8f,0.5f);
						//	char buff[256]={0};
						//	sprintf(buff,"%f",m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0]);
						//	MessageBox(NULL,buff,"test",MB_OK);

						
							AddNode->SetVertexColor(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[4],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[8],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[12],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0],0);
							AddNode->SetVertexColor(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[5],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[9],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[13],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[1],1);

							AddNode->SetVertexColor(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[6],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[10],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[14],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[2],2);
							
							AddNode->SetVertexColor(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[7],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[11],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[15],
													m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[3],3);

							AddNode->SetEtc();
							AddNode->SetRad(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[26]);
							AddNode->SetSpeed(0.0f,0.0f,0.0f);
							AddNode->SetSpeedPower(0.0f);
							AddNode->SetWind(0.0f,0.0f,0.0f);
							AddNode->Setfadein(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[17]);
						//	AddNode->Setfadein(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0]);
							AddNode->SetBackFadeIn(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[17]);
							AddNode->SetFadeInSpeed(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[18]);
							AddNode->SetFadeInSpeed(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0]);
							AddNode->SetFadeOutSpeed(3.0f);
							AddNode->ChangeVertexBuffer();
							AddNode->SetPick((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1] > 0) ? true : false);



							////////////////////
							pAddNode=AddNode;
				
					}
					break;
				case 2:
					// Boid //
					{
						
						
						CBoidGroup *AddNode = new CBoidGroup();
						AddNode->SetPos(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x,m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.y,m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z);
						AddNode->SetBoidRange(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0],
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[2],
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[3],
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[5]);

						AddNode->SetBoidSpeed(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[1]);
						//boid gem file name
						AddNode->SetBoidName(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Name,EFFECTPATH);
						
						// boid mesh setting is wrong 
						if(m_Boid[0] == NULL) {
							
							pAddNode = NULL;
							break;
						}
						char *str = AddNode->GetBoidName();
						char *ptr = strrchr(str,'\\');
	
						// boid mesh data search
						int bindex = 0;
						for(;bindex < 3;bindex++) {
						
							if(strstr(m_Boid[bindex]->GetFileName(),ptr) != NULL) {
								break;		
							}

						}
						//mesh setting
						AddNode->SetMesh(m_Boid[bindex]);

						AddNode->SetUnitNum(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[0]);
						AddNode->SetUnitVot(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[4]);
						
						
						// 집단행동 설정 (0 : 집단 행동 1: 개별행동) 
						
						AddNode->SetPat(((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[2] == 0) ? 1 : 0));
						AddNode->SetPick((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1] > 0) ? true : false);
						
						
						pAddNode = AddNode;

					}
					break;
				case 3: 
					// object ani mesh
					{
							CGemRender *AddNode;
						

							strcpy(gem_name,OBJECTGEMPATH);
							strcat(gem_name,m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Name);
							
							AddNode = new CGemRender();
							
							// light setting & Zbuffer setting 
							AddNode->SetLight(true);

							AddNode->LoadGemFile(gem_name,CSceneManager::GetDevice());
							AddNode->SetLoop((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[0] >0) ? true : false);
							AddNode->SetNullTexture((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[2]>0) ? true : false);
							AddNode->SetVot(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[1]);
							AddNode->SetXrot(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[4]);
							AddNode->SetYrot(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[5]);
							AddNode->SetZrot(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[6]);
							AddNode->SetBlend(D3DBLEND_ONE,D3DBLEND_ZERO);
						
							AddNode->SetRandAni((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[3] > 0) ? true : false);
							AddNode->SetRandAniMax(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[2]);
							AddNode->SetRandAniMin(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[3]);
							AddNode->SetCullZbuffer(true);
						
							// switch ani에 관련 된 루틴은 작성해야 함
							AddNode->SetSwitchAni((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1] > 0) ? true : false);
							AddNode->SetAniRad(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0]);
							
							pAddNode = AddNode;

					}
					break;
				case 4: // wave 
					{	
						int layernum = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1] + m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[2];
						int point = 0;
						vector3 target_point(0.0f,0.0f,0.0f);

						CWaveLine *AddNode = new CWaveLine();
						AddNode->SetDevice(CSceneManager::GetDevice());
						layernum  =6;
						m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1] = 4;
						m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[2] = 2;
						
						AddNode->SetLayerNum(layernum);
						AddNode->SetDownLayerNum( m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1]);
						AddNode->SetUpLayerNum( m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[2]);
						
						//point 추가 IParam[3] -> point number
						for(int k=0;k<m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[3];k++) {
							point = k * 3;

							target_point.x = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[point];
							target_point.y = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[point + 1];
							target_point.z = m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[point + 2];

							AddNode->InsertPoint(target_point.x,target_point.y,target_point.z);	
						}
						
						AddNode->CreateTexture(2);
						AddNode->SetTexture(0,"pado.dds");
						//AddNode->SetTexture(1,"water.dds");
						
						AddNode->StoreList();
					
						for(int k=0;k<layernum;k++) {

							if(k< (m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1]))
								AddNode->CreateInterpolation(k,WAVEFWDMOVE);
							else if(k >= (m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1]))
								AddNode->CreateInterpolation(k,WAVEBACKMOVE);
						}
						// vertex 랜덤하게 흔들고 정보세팅 
						AddNode->CreateVertexBuffer();
						//Pick Mode Set
						AddNode->SetPick((m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[0] > 0) ? true : false);
						pAddNode = AddNode;
					}
					break;
				case 5: //water space
					{
					
						CWaterW *AddNode;
						AddNode = new CWaterW(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[0], //width
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_IntParam[1], //height
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[0], // limit height
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[1], // chop
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.x, // center start pos
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_vecCenterPos.z,
											  m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_Param[2], //quad size
											  CSceneManager::GetDevice());	
						
						AddNode->SetTexture("water00.dds","sp.dds");

						pAddNode = AddNode;
	
					
					}
					break;

			}
			if(pAddNode) {
				m_LandscapeEffectNode.push_back(pAddNode);
				m_LandscapeEffectNodeValue.push_back(m_MapLandscapeEffect->m_LandscapeEffect[cEffect].m_nEffectMethod);
			}
			
		}
	
	}// if
}

void CSectorScene::GetPickHousePoly(vector3 vecStart,vector3 vecEnd,std::vector<vector3> &vecPolyList)
{
	long cAddShadowPolySet=0;
	vector3 vecHousePos;
	matrix matInHousePos,matInv,matTrans;
	vector3 vecInHouseStartPos,vecInHouseEndPos;	

	vector3 vecDir=vecEnd-vecStart;
	

	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{	
		
		vecHousePos=m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc();
		vector3 vecHouseInterLens=vecHousePos-vecStart;
		float fHouseInter=vecHouseInterLens.GetLens();		

		float fRad=20.0f;				

		//vector3 vecInter=m_vecPickRayStart-vecObjectPos;

		float fC=(vecHouseInterLens*vecHouseInterLens)-m_HouseObjectNode[cHouse]->m_fRad;
		float fB=vecDir*(vecStart-vecHousePos);

		//vecObjectPos=m_vecPickRayStart-vecObjectPos;
		
		//if(fB*fB-fC>=0.0f)


		//if(fHouseInter< m_HouseObjectNode[cHouse]->m_fRad)
		{		

			matInv.Inverse(m_HouseObjectNode[cHouse]->m_AccumulateTM);

			matInHousePos.Translation(vecEnd);
			matInHousePos=matInHousePos*matInv;	
			vecInHouseEndPos=matInHousePos.GetLoc();

			matInHousePos.Translation(vecStart);
			matInHousePos=matInHousePos*matInv;			
			vecInHouseStartPos=matInHousePos.GetLoc();
			
			std::vector<PolyNode*> CullPolyList;
			if(m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree
				&& m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->IsLoaded())
			{				
				vector3 vecDir=vecInHouseEndPos-vecInHouseStartPos;
				vecDir.Normalize();
				m_HouseObjectNode[cHouse]->m_HouseObject->m_pOctree->CullRay(vecInHouseStartPos,vecDir,10000.0f,CullPolyList);

				for(int cPoly=0;cPoly<(int)CullPolyList.size();cPoly++)
				{
					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[0]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[1]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());

					matTrans.Translation(CullPolyList[cPoly]->m_vecPoly[2]);					
					matTrans=matTrans*m_HouseObjectNode[cHouse]->m_AccumulateTM;
					vecPolyList.push_back(matTrans.GetLoc());
				}
			}
		}
	}	
}

void CSectorScene::UnloadAllData()
{

	m_MapHouse=NULL;
	//m_MapHouse=NULL;
	int DeleteIndex=(int)m_HouseObjectNode.size();
	for(int cHouse=0;cHouse<(int)m_HouseObjectNode.size();cHouse++)
	{		
		//delete m_HouseObjectNode[cHouse];
		CROSSM::SafeDelete(m_HouseObjectNode[cHouse]);
		//CSceneManager::_ReleaseObj(CROSSM::POOL_HOUSEOBJSCENE,m_HouseObjectNode[cHouse]);
		m_HouseObjectNode[cHouse] = NULL;
	}
	m_HouseObjectNode.clear();

	m_MapMesh=NULL;
	
	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{
		CROSSM::SafeDelete(m_ObjectSceneNode[cObject]);
//		CSceneManager::_ReleaseObj(CROSSM::POOL_OBJECTSCENE, m_ObjectSceneNode[cObject]);
		
	}
	m_ObjectSceneNode.clear();

	m_MapWide=NULL;
	m_TreeObjectNode.m_cPlant = 0;
	if(m_TreeObjectNode.m_NormalTreeLeaves[0] != NULL) {
		for(int iTree = 0; iTree < MAX_TREEKIND; iTree++) {
			m_TreeObjectNode.m_NormalTreeLeaves[MAX_TREEKIND] = NULL;

		}
	}
	m_WorldEffectManager.DeleteAllWorldScript();
	
	int i;

	for(i=0;i<(int)m_LandscapeEffectNode.size();i++) {
		if(m_LandscapeEffectNode[i] != NULL) {
			switch(m_LandscapeEffectNodeValue[i]) {
			
			case 1: //fog
				delete (CFogScene *)m_LandscapeEffectNode[i];
				break;
			case 2: // boid
				delete (CBoidGroup *)m_LandscapeEffectNode[i];
				break;
			case 3: // mesh ani
				delete (CGemRender *)m_LandscapeEffectNode[i];
				break;
			case 4: // wave line
				delete (CWaveLine *)m_LandscapeEffectNode[i];
				break;
			case 5: // 격자 water
				delete (CWaterW *)m_LandscapeEffectNode[i];
			}
		}
		
	}
	m_LandscapeEffectNode.clear();

	//DeleteBoidMesh();
	
	if(m_pLightEsf != NULL)
	{
		delete[] m_pLightEsf;
		m_pLightEsf = NULL;

	}

	m_LandscapeEffectNodeValue.clear();
	
	m_MapLandscapeEffect = NULL;

	m_SectorLightManager.DeleteAllLight();

	m_TM._41= -1000000000.0f;
	m_TM._43= -1000000000.0f;

	
	m_MapDungeon = NULL;
	m_MapHeight = NULL;
	m_MapMash = NULL;
	m_MapPlant = NULL;
	m_MapWide = NULL;
	m_MapHouse = NULL;
	m_MapWater = NULL;
	m_MapFall = NULL;
	m_MapMesh = NULL;
	m_MapMustDivideVertex = NULL;
	m_MapEffect = NULL;
	m_MapLandscapeEffect = NULL;

	if(m_pNotCullHouseIndex)
	{
		delete[] m_pNotCullHouseIndex;
		m_pNotCullHouseIndex = NULL;
	}
	m_iNotCullHouseIndex = 0;

}

void CSectorScene::RenderObjectShadow(LPDIRECT3DDEVICE8 pd3dDevice)
{
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
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();

	vector3 vecViewPos=matViewPosition->GetLoc();
	vector3 vecViewPosInter;

	vecViewPosInter;
	float fViewInter;

	matrix matObjectCamera;

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{		
		
		if(m_ObjectSceneNode[cObject]->isCulling()==false)
			continue;
		
		if(m_ObjectSceneNode[cObject]->m_pObject==NULL || !m_ObjectSceneNode[cObject]->m_pObject->IsLoaded())
			continue;		
		/*
		
			*/
		
		

		vecViewPosInter=vecViewPos-m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc();
		fViewInter=vecViewPosInter.GetLens();

		if(MAX_OBJECTVIEWRANBE < fViewInter)
			continue;

		if(MAX_OBJECTDISAPPER < fViewInter)
			continue;

		vector3 vecObjectPos=m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc();

		CHeightFieldScene::CShadowedPoly *pNode;
		pNode=CSceneManager::m_HeightField.GetShadowPolyList(m_ShadowPolyList[cObject]);
		if(pNode==NULL)
			continue;

		//List<vector3> ShadowedPoly;	
		//GetHeightFieldShadowPoly(vecObjectPos,ShadowedPoly);

		TTVertex *pShadowedVertex=new TTVertex[pNode->m_ShadowedPolyList.size()];
		WORD *pShadowedIndices=new WORD[pNode->m_ShadowedPolyList.size()];
		for(int cIndices=0;cIndices<(int)pNode->m_ShadowedPolyList.size();cIndices++)
		{
			pShadowedVertex[cIndices].v=pNode->m_ShadowedPolyList[cIndices];
			pShadowedVertex[cIndices].diff.c=0xff999999;
			pShadowedIndices[cIndices]=cIndices;			
		}
		

		matShadowProj.MakeNoProjection(3.14159f/3.0f,(float)SHADOWSIZE+m_ObjectSceneNode[cObject]->m_fRad*2.0f,(float)SHADOWSIZE+m_ObjectSceneNode[cObject]->m_fRad*2.0f,10.0f,5500.f);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
			
	
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		//if(m_ObjectSceneNode[cObject]->m_isAlpha)
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		}		
		m_pShadowTexture.Begin(pd3dDevice);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->BeginScene();
		matrix matObjectCamera;

		

		matObjectCamera.CameraLookAt(vecObjectPos+vecSunLightDir*550.0f+vector3(0.0,85.0f,0.0f),
			vector3(0.0f,80.0f,0.0f)+vecObjectPos,vector3(0.0f,1.0f,0.0f));

		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matShadowProj);
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);

		

		matrix matWorld;
		matWorld.MakeIdent();

		pd3dDevice->SetTransform(D3DTS_TEXTURE0,matWorld);

		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);		
		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->SetVertexShader(TTVERTEXFVF);	
		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW);

		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)pNode->m_ShadowedPolyList.size(),(int)pNode->m_ShadowedPolyList.size()/3,pShadowedIndices,
			D3DFMT_INDEX16,pShadowedVertex,sizeof(TTVertex));		
		

		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
		//pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW);
		pd3dDevice->SetTransform(D3DTS_WORLD,m_ObjectSceneNode[cObject]->m_AccumulateTM);

		if(CRenderOption::m_AllObjectBump)
		{

			m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);
		}
		else
			m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);
		

		

		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

		if(m_ObjectSceneNode[cObject]->m_isAlpha)
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		}		
		pd3dDevice->EndScene();		
		m_pShadowTexture.End(pd3dDevice);

		////////////////////////////

		pd3dDevice->BeginScene();	
		
		pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		
		
		matrix mat,mat2;
		mat=matShadowProj*matTexScale;		
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;

		pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);
		pd3dDevice->SetTexture(0,m_pShadowTexture.GetTexture());
		pd3dDevice->SetVertexShader(TTVERTEXFVF);	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_ADD);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);


		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);		
		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetTransform(D3DTS_WORLD,matIndent);
		
		pd3dDevice->SetRenderState(D3DRS_ZBIAS,14);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);
		
		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)pNode->m_ShadowedPolyList.size(),(int)pNode->m_ShadowedPolyList.size()/3,pShadowedIndices,
			D3DFMT_INDEX16,pShadowedVertex,sizeof(TTVertex));
		
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		delete [] pShadowedVertex;
		delete [] pShadowedIndices;
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

void CSectorScene::RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_AccumulateTM));
	/*for(int i=0;i<m_HouseObjectNode.num;i++)
	{		
		m_HouseObjectNode[i]->RenderGlare(pd3dDevice);		
	}*/

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{		
		if(m_ObjectSceneNode[cObject]->isCulling()==false)
			continue;
		if(m_ObjectSceneNode[cObject]->m_pObject==NULL || !m_ObjectSceneNode[cObject]->m_pObject->IsLoaded())
			continue;	

		pd3dDevice->SetTransform(D3DTS_WORLD,m_ObjectSceneNode[cObject]->m_TM);

		if(CRenderOption::m_AllObjectBump)
		{			
			/*
			matrix matView,matProjection,matWorld,matWVP;
			pd3dDevice->GetTransform(D3DTS_VIEW,matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,matProjection);				
			pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
			matWVP=matWorld*matView*matProjection;
			matWVP.Transpose();				
			pd3dDevice->SetVertexShaderConstant(16,&matWVP, 4);				

			pd3dDevice->SetPixelShader(m_dwObjectDiffusePixelShader);

			m_ObjectSceneNode[cObject]->m_pObject->m_dwCustomizeShader=m_dwObjectDiffuseVertexShader;											

			m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);			

			pd3dDevice->SetPixelShader(NULL);
			m_ObjectSceneNode[cObject]->m_pObject->m_dwCustomizeShader=0xffffffff;				
			*/

			
			pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
			pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
			pd3dDevice->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX,2);
			pd3dDevice->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX,3);

			pd3dDevice->SetTexture(2,NULL);
			pd3dDevice->SetTexture(3,m_pNormalizeCubeMap);
			
			matrix matView,matProjection,matWorld,matWVP;
			pd3dDevice->GetTransform(D3DTS_VIEW,matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,matProjection);				
			pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
			matWVP=matWorld*matView*matProjection;
			matWVP.Transpose();								

			vector3 vecPos=matWorld.GetLoc();								
			pd3dDevice->SetVertexShaderConstant(16,&matWVP, 4);

			matrix matInv;

			matInv.Inverse(matWorld);
				
			matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
			vector3 vecViewPos=vector3(matViewPosition->_41,matViewPosition->_42,matViewPosition->_43);				

			matrix matViewPos;
			matViewPos.Translation(vecViewPos);

			matViewPos=matViewPos*matInv;								

			float fObjectSpaceViewPosition[4]={matViewPos._41,matViewPos._42,matViewPos._43,0.0f};

			pd3dDevice->SetVertexShaderConstant(35,fObjectSpaceViewPosition,1);

				//////////////////

			matWorld._41=matWorld._42=matWorld._43=0.0f;				
			matInv.Inverse(matWorld);

			vector3 vecLightPos=vector3(1.0f,1.0f,1.0f);
			vecLightPos.Normalize();

			vecLightPos=vecLightPos*10000.0f;

			matrix matLightPos;
			matLightPos.Translation(vecLightPos);			

			matLightPos=matLightPos*matInv;				

			float fObjectSpaceLightPosition[4]={matLightPos._41,matLightPos._42,matLightPos._43,1.0f};				
			pd3dDevice->SetVertexShaderConstant(32,fObjectSpaceLightPosition,1);																		

			pd3dDevice->SetPixelShader(m_dwObjectSpecularPixelShader);  
			
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSW,D3DTADDRESS_CLAMP);					

			m_ObjectSceneNode[cObject]->m_pObject->Render(pd3dDevice);

			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetPixelShader(NULL);
			
		}
	}
}
void CSectorScene::SetBoidMesh() {
	if(!m_bBoidMeshSet) {
		char tmp_buf[256] = {0};

		m_bBoidMeshSet = true;

		m_Boid[0] = new CGemRender;
		m_Boid[1] = new CGemRender;
		m_Boid[2] = new CGemRender;
		
		sprintf(tmp_buf,"%s\\Boid_Gull_T1.GEM",EFFECTPATH);
		m_Boid[0]->LoadGemFile(tmp_buf,CSceneManager::GetDevice());
		m_Boid[0]->SetCullZbuffer(true);
		m_Boid[0]->SetClearBuffer(false);

		memset(tmp_buf,0,sizeof(char) * 256);
		sprintf(tmp_buf,"%s\\Boid_Butterfly_T1.GEM",EFFECTPATH);
		m_Boid[1]->LoadGemFile(tmp_buf,CSceneManager::GetDevice());
		m_Boid[1]->SetCullZbuffer(true);
		m_Boid[1]->SetClearBuffer(false);

		memset(tmp_buf,0,sizeof(char) * 256);
		sprintf(tmp_buf,"%s\\Boid_Fly_T1.GEM",EFFECTPATH);
		m_Boid[2]->LoadGemFile(tmp_buf,CSceneManager::GetDevice());
		m_Boid[2]->SetCullZbuffer(true);
		m_Boid[2]->SetClearBuffer(false);
	}
	
}
void CSectorScene::DeleteBoidMesh() {
	if(m_bBoidMeshSet) {
		
		for(int i=0;i<3;i++) {
			if(m_Boid[i]) {
				delete m_Boid[i];
				m_Boid[i] = NULL;
			}
		}
		m_bBoidMeshSet = false;
	}
}
void CSectorScene::SettingNight(bool bNight)
{	// Night 인가
/*	if(m_bNight == bNight)
		return;*/
/*	int i;

	int iCount = 0;
	for( i = 0; i < m_HouseObjectNode.num; i++ )
	{
		
		m_bNight = m_HouseObjectNode[i]->m_HouseObject->SettingNight(bNight);
		if(m_bNight != bNight)
			iCount++;

	}
	
	if(iCount != 0)
	{
		m_bNight = !bNight;
	}
	else
		m_bNight = bNight;

*/	
}

// 충돌시 true
bool CSectorScene::CheckRayCollisionHouse(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2)
{
	if(m_HouseObjectNode.size() <=0)
		return false;
	D3DXVECTOR3 vecTPos1,vecTPos2;
	D3DXMATRIX matTmp,matInv;
	D3DXVECTOR3 vecStart,vecDir;
//	float fDist;

	matrix matHouseTM;
	for(int i = 0; i < (int)m_HouseObjectNode.size(); i++ )
	{	
		if(!m_HouseObjectNode[i]->m_HouseObject)
			continue;
		if(!m_HouseObjectNode[i]->m_HouseObject->m_pOctree)
			continue;
		if(!m_HouseObjectNode[i]->m_HouseObject->m_pOctree->IsLoaded())
			continue;

		matHouseTM = m_HouseObjectNode[i]->m_AccumulateTM;

		D3DXMatrixIdentity(&matTmp);
		
		matTmp._11 = matHouseTM._11;
		matTmp._12 = matHouseTM._12;
		matTmp._13 = matHouseTM._13;
		matTmp._14 = matHouseTM._14;
		
		matTmp._21 = matHouseTM._21;
		matTmp._22 = matHouseTM._22;
		matTmp._23 = matHouseTM._23;
		matTmp._24 = matHouseTM._24;
		
		matTmp._31 = matHouseTM._31;
		matTmp._32 = matHouseTM._32;
		matTmp._33 = matHouseTM._33;
		matTmp._34 = matHouseTM._34;
		
		matTmp._41 = matHouseTM._41;
		matTmp._42 = matHouseTM._42;
		matTmp._43 = matHouseTM._43;
		matTmp._44 = matHouseTM._44;

		D3DXMatrixInverse(&matInv,NULL,&matTmp);
		D3DXMatrixIdentity(&matTmp);
		
		matTmp._41 = vecPos1.x;
		matTmp._42 = vecPos1.y;
		matTmp._43 = vecPos1.z;
		
		D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
		vecTPos1.x = matTmp._41;
		vecTPos1.y = matTmp._42;
		vecTPos1.z = matTmp._43;

		D3DXMatrixIdentity(&matTmp);
		
		matTmp._41 = vecPos2.x;
		matTmp._42 = vecPos2.y;
		matTmp._43 = vecPos2.z;
		
		D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
		vecTPos2.x = matTmp._41;
		vecTPos2.y = matTmp._42;
		vecTPos2.z = matTmp._43;

		D3DXVECTOR3 vecStart = vecTPos1;
		D3DXVECTOR3 vecDir = vecTPos2 - vecTPos1;
		float fDist = D3DXVec3Length(&vecDir);
		D3DXVec3Normalize(&vecDir,&vecDir);
		
		if(m_HouseObjectNode[i]->m_HouseObject->m_pOctree->CollisionToRay(vecStart,vecDir,fDist))
			return true;


	}
	return false;

}

