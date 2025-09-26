// HeightFieldScene.cpp: implementation of the CHeightFieldScene class.
//
//////////////////////////////////////////////////////////////////////

#include "HeightFieldScene.h"
#include "SceneManager.h"
#include <BaseGraphicsLayer.h>
#include "RenderOption.h"
#include "Bitset.h"
#include "RenderBoundingBox.h"

#include "GMMemory.h"

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

CBitset BitSetEffect; 
CBitset BitSet; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeightFieldScene::CHeightFieldScene()
{
	//LogMessage("Constructor HeightFieldScene");
	m_NowPositionX=-100000000;
	m_NowPositionY=-100000000;
	m_NowTessellatePosX=-100000000;
	m_NowTessellatePosY=-100000000;
	//LogMessage("Constructor HeightFieldScene Complete");
	
	
	// 03.03.04 House and Effect and object Culling flag
	m_bHouseCulling = true;
	m_bObjectCulling = true;				
	m_bEffectCulling = true;

	
	m_SectorScene = NULL;
	m_pSectorSorts = NULL;
	m_iSortsNum = 0;

	UpdateSector = NULL;//=new int[m_LSizeX*m_LSizeY];
	NotUpdateSector = NULL;//=new int[m_LSizeX*m_LSizeY];
	IsCulling = NULL;//=new bool[m_LSizeX*m_LSizeY];
}

CHeightFieldScene::~CHeightFieldScene()
{
	if(UpdateSector)
	{
		delete[] UpdateSector;
		UpdateSector = NULL;
	}
	if(NotUpdateSector)
	{
		delete[] NotUpdateSector;
		NotUpdateSector = NULL;
	}
	if(IsCulling)
	{
		delete[] IsCulling;
		IsCulling = NULL;
	}


	if(m_SectorScene) {
		delete [] m_SectorScene;
		m_SectorScene = NULL;
	}
	if(m_pSectorSorts != NULL)
	{
		delete[] m_pSectorSorts;
		m_pSectorSorts = NULL;
	}

}

void CHeightFieldScene::CheckingNextTerrain()
{	
	vector3 *ViewPos;
	vector3 CharPos;
	if(CSceneManager::m_ViewerMode==0 || CSceneManager::m_ViewerMode==2)
		ViewPos=CSceneManager::GetCamera()->GetPosition();
	else
	{	
		if( CSceneManager::m_CharacterManager.m_CharacterList.empty())
		{
			ViewPos=CSceneManager::GetCamera()->GetPosition();
		}
		else
		{
			if(CSceneManager::m_pCameraScript->ISPlay()) {
				
				D3DXVECTOR3 vecTmpPos = CSceneManager::m_pCameraScript->GetVecPos();
				vector3 vecTmpPos2 = vector3(vecTmpPos.x,vecTmpPos.y,vecTmpPos.z);
				ViewPos = &(vecTmpPos2);

				/*matrix *matPos = CSceneManager::m_ViewCamera->GetMatPosition();
				vector3 vecTmpPos = vector3(matPos->_41,matPos->_42,matPos->_43);
				ViewPos = &(vecTmpPos);
				*/
				//ViewPos=CSceneManager::GetCamera()->GetPosition();
				
			}
			else {
				CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);
				//CSceneManager::m_CharacterList[0]->GetPosition(CharPos.x,CharPos.y,CharPos.z);
				ViewPos=&CharPos;
			}
		}
	}


	int UpdatePositionX=(int)(ViewPos->x/SECTORSIZE);
	int UpdatePositionY=(int)(ViewPos->z/SECTORSIZE);	
	if(ViewPos->x < 0.0f)
		UpdatePositionX--;
	if(ViewPos->z < 0.0f)
		UpdatePositionY--;

	if( m_NowPositionX == UpdatePositionX && m_NowPositionY == UpdatePositionY) 
		return;

	int WillX=(int)(ViewPos->x/SECTORSIZE);
	int WillY=(int)(ViewPos->z/SECTORSIZE);
	int MoveX=m_NowPositionX-WillX;
	int MoveY=m_NowPositionY-WillY;
	/*
	int UpdateSector[LSIZEX*LSIZEY];
	int NotUpdateSector[LSIZEX*LSIZEY];
	*/
//	int *UpdateSector=new int[m_LSizeX*m_LSizeY];
//	int *NotUpdateSector=new int[m_LSizeX*m_LSizeY];
	int CountUpdate=0;

	memset(NotUpdateSector,-1,sizeof(int)*m_LSizeX*m_LSizeY);	
	memset(UpdateSector,-1,sizeof(int)*m_LSizeX*m_LSizeY);	

	int sectorx,sectory;
	int fx,fy;
	int ix,iy;
	memset(m_QuadSector,0,sizeof(m_QuadSector));
	///*
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			sectorx=(int)(m_SectorScene[ix+iy*m_LSizeX].m_TM._41/SECTORSIZE);
			sectory=(int)(m_SectorScene[ix+iy*m_LSizeX].m_TM._43/SECTORSIZE);			
			
			if( (sectorx >= WillX-(int)(m_LSizeX/2) && sectorx <= WillX+(int)(m_LSizeX/2)) &&
				(sectory >= WillY-(int)(m_LSizeY/2) && sectory <= WillY+(int)(m_LSizeY/2))
				)
			{
				fx=sectorx-WillX+(int)(m_LSizeX/2);
				fy=sectory-WillY+(int)(m_LSizeY/2);
				NotUpdateSector[fx+fy*m_LSizeX]=0;							
				m_QuadSector[fx+fy*m_LSizeX]=&m_SectorScene[ix+iy*m_LSizeX];
			}
			else 
				UpdateSector[CountUpdate++]=ix+iy*m_LSizeX;
		}
	}	
	if(CountUpdate != m_LSizeX*m_LSizeY)
	{		
		UpdateSector[CountUpdate]=-1;
	}

	for(int cSector=0;cSector<m_LSizeX*m_LSizeY;cSector++)
	{
		sectorx=(int)(m_SectorScene[cSector].m_TM._41/SECTORSIZE);
		sectory=(int)(m_SectorScene[cSector].m_TM._43/SECTORSIZE);
		
		if( !(UpdatePositionX <= sectorx+1 &&
			UpdatePositionX >= sectorx-1 &&
			UpdatePositionY <= sectory+1 &&
			UpdatePositionY >= sectory-1))
		{
			if(m_SectorScene[cSector].m_ShadowUsed!=-1)
			{
				m_SectorScene[cSector].m_UsedShadowTerrain[m_SectorScene[cSector].m_ShadowUsed]=-1;
				m_SectorScene[cSector].m_ShadowUsed=-1;				
			}									
		}		
	}

	int InnerCount=0;
	int CheckEndUpdate=0;
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{				
			if(NotUpdateSector[ix+iy*m_LSizeX]==-1)
			{				
				CheckEndUpdate=UpdateSector[InnerCount++];				
				if(CheckEndUpdate==-1)
					break;					

				m_SectorScene[CheckEndUpdate].Generate( 
					(ix+WillX-(int)(m_LSizeX/2))*SECTORSIZE,
					(iy+WillY-(int)(m_LSizeY/2))*SECTORSIZE);								

				m_QuadSector[ix+iy*m_LSizeX]=&m_SectorScene[CheckEndUpdate];												
			}			
			if(CheckEndUpdate==-1)break;
		}		
	}
	/*

	for(int cSector=0;cSector<m_LSizeX*m_LSizeY;cSector++)
	{
		sectorx=(int)(m_SectorScene[cSector].m_TM._41/SECTORSIZE);
		sectory=(int)(m_SectorScene[cSector].m_TM._43/SECTORSIZE);
		
		if( !(UpdatePositionX <= sectorx+1 &&
			UpdatePositionX >= sectorx-1 &&
			UpdatePositionY <= sectory+1 &&
			UpdatePositionY >= sectory-1))
		{
			if(m_SectorScene[cSector].m_ShadowUsed!=-1)
			{
				m_SectorScene[cSector].m_UsedShadowTerrain[m_SectorScene[cSector].m_ShadowUsed]=-1;
				m_SectorScene[cSector].m_ShadowUsed=-1;
				m_SectorScene[cSector].GenerateWideMap();
			}									
		}		
	}
	*/
	
	for(int cSector=0;cSector<m_LSizeX*m_LSizeY;cSector++)
	{
		sectorx=(int)(m_SectorScene[cSector].m_TM._41/SECTORSIZE);
		sectory=(int)(m_SectorScene[cSector].m_TM._43/SECTORSIZE);
		
		if( UpdatePositionX <= sectorx+1 &&
			UpdatePositionX >= sectorx-1 &&
			UpdatePositionY <= sectory+1 &&
			UpdatePositionY >= sectory-1)
		{
			if(SHADOWSIZE!=0)
				m_SectorScene[cSector].GenerateShadow(BaseGraphicsLayer::GetDevice());			
			else
				m_SectorScene[cSector].GenerateWideMap();
		}
		else
		{
			if(m_SectorScene[cSector].m_ShadowUsed!=-1)
			{
				m_SectorScene[cSector].m_UsedShadowTerrain[m_SectorScene[cSector].m_ShadowUsed]=-1;
				m_SectorScene[cSector].m_ShadowUsed=-1;
				m_SectorScene[cSector].GenerateWideMap();
			}						
		}
	}
	m_NowPositionX=UpdatePositionX;
	m_NowPositionY=UpdatePositionY;		
/*
	if(UpdateSector) {
		delete [] UpdateSector;
		UpdateSector = NULL;	
	}
	if(NotUpdateSector) {
		delete [] NotUpdateSector;
		NotUpdateSector = NULL;
	}
*/
}

void CHeightFieldScene::Create(int LSizeX,int LSizeY,CMapStorage *pMapStorage)
{	
	//LogMessage("HeightFieldScene Create");
	m_LSizeX=LSizeX;
	m_LSizeY=LSizeY;
	int ix,iy;

	if(m_SectorScene)
	{
		delete[] m_SectorScene;
		m_SectorScene = NULL;
	}

	m_SectorScene=new CSectorScene[m_LSizeX*m_LSizeY];
	//m_QuadSector=new CSectorScene[m_LSizeX*m_LSizeY];	

	if(m_pSectorSorts)
	{
		delete[] m_pSectorSorts;
		m_pSectorSorts = NULL;
	}

	m_pSectorSorts = new SectorSortNode_t[m_LSizeX*m_LSizeY];

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			m_SectorScene[ix+iy*m_LSizeX].Create(pMapStorage);
		}
	}		

	if(UpdateSector)
	{
		delete[] UpdateSector;
		UpdateSector = NULL;
	}
	UpdateSector=new int[m_LSizeX*m_LSizeY];

	if(NotUpdateSector)
	{
		delete[] NotUpdateSector;
		NotUpdateSector = NULL;
	}
	NotUpdateSector=new int[m_LSizeX*m_LSizeY];


	if(IsCulling)
	{
		delete[] IsCulling;
		IsCulling = NULL;
	}
	IsCulling=new bool[m_LSizeX*m_LSizeY];

	m_SectorScene->m_NextTriNode=0;
	m_SectorScene->m_WaterScene.Create();	
	m_GrassSceneManager.Create();	
	//LogMessage("HeightFieldScene Create Complete");
	//m_GrassScene.
}

void CHeightFieldScene::Destroy(void)
{
	for(int ix=0; ix < GetLSizeX(); ix++)
	{
		for(int iy = 0; iy < GetLSizeX(); iy++)
		{
			m_SectorScene[ix + iy * GetLSizeX()].UnloadAllData();
		}
	}

	m_GrassSceneManager.Delete();

}

void CHeightFieldScene::DeleteSectorNotCull() {
	int ix,iy;

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			m_SectorScene[ix+iy*m_LSizeX].DeleteCullHouseIndex();
		}
	}		

}
void CHeightFieldScene::RenderEnv(LPDIRECT3DDEVICE8	pd3dDevice)
{
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	

	

	// WaterReflection Count
	std::vector<float> WaterHeightList;
	int cDrawTerr=0;
	//bool IsCulling[LSIZEX*LSIZEY];
	//bool *IsCulling=new bool[m_LSizeX*m_LSizeY];
	int ix,iy;
	CSceneManager::m_cRenderSector=0;	
	
	vector3 vecMinBox,vecMaxBox;
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{			
				// edith AddUnique 형식 구현
				if(m_SectorScene[ix+iy*m_LSizeX].m_MapWater && m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_bWaterRelection)
				{
					int iCnt=0;
					for(int a=0;a<(int)WaterHeightList.size();a++)
					{
						if(WaterHeightList[a] == m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_fWaterHeight)
							iCnt++;
					}

					if(iCnt==0)
						WaterHeightList.push_back(m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_fWaterHeight);				
				}

				IsCulling[ix+iy*m_LSizeX]=true;				
				CSceneManager::m_cRenderSector++;
			}
			else
			{
				IsCulling[ix+iy*m_LSizeX]=false;
			}
		}
	}
	int nDetailSector=0;
	int DetailSectorRender[9]={0,};

	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	
	//pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	//pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_TerrainLight);

	

	pd3dDevice->LightEnable(0,FALSE);	
	pd3dDevice->LightEnable(1,FALSE);	
	pd3dDevice->LightEnable(2,FALSE);	
	pd3dDevice->LightEnable(3,FALSE);	
	pd3dDevice->LightEnable(4,FALSE);	
	pd3dDevice->LightEnable(5,FALSE);	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);	

	color TerrainColor;
	TerrainColor.a=0xff;
	TerrainColor.r=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.r*255;
	TerrainColor.g=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.g*255;
	TerrainColor.b=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.b*255;
	
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,TerrainColor.c);	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterTerrainColor.c);	

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	
	//pd3dDevice->SetTexture(1,NULL);
	

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{				
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM));								
				m_SectorScene[ix+iy*m_LSizeX].Render(pd3dDevice);
				
			}
		}
	}
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	///*	
    pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
    pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	//*/	

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	matrix matIndent;
	matIndent.MakeIdent();	

	pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matIndent);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);	

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(m_bHouseCulling) {
				if(IsCulling[ix+iy*m_LSizeX])
				{				
					
					m_SectorScene[ix+iy*m_LSizeX].RenderEnvHouseObject(pd3dDevice);
				}
				else 
					m_SectorScene[ix+iy * m_LSizeX].RenderNotCullHouseObject(pd3dDevice);

			}
			else {									// Culling Off 03.03.04 (arena 존 관련)
					m_SectorScene[ix+iy*m_LSizeX].RenderEnvHouseObject(pd3dDevice);
			}
		}
	}	

	D3DMATERIAL8 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	pd3dDevice->SetMaterial( &mtrl );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	if(BaseGraphicsLayer::m_VoodooOption)
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			


	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);


	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
    pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
    pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
	
	for(int cLight=1;cLight<8;cLight++)
	{
		pd3dDevice->LightEnable(cLight,FALSE);
	}
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

	//CSceneManager::m_CharacterManager.RenderEnv(pd3dDevice);
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );				
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 3 );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	


	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);				

	int iBefore = CRenderOption::m_WaterBumpEnvRendering;
	CRenderOption::m_WaterBumpEnvRendering = 0;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{						
				if( m_SectorScene[ix+iy*m_LSizeX].m_MapWater)
				{
					

					if( m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_bWaterRelection )					
						m_SectorScene[ix+iy*m_LSizeX].m_WaterScene.DeterminWaterTexture(pd3dDevice,true);					
					else
						m_SectorScene[ix+iy*m_LSizeX].m_WaterScene.DeterminWaterTexture(pd3dDevice,false);
					
					m_SectorScene[ix+iy*m_LSizeX].RenderWater(pd3dDevice,false);				
				}			
			}
		}
	}
	CRenderOption::m_WaterBumpEnvRendering = iBefore;
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

	if(BaseGraphicsLayer::m_VoodooOption)
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);


	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);



	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{				
				pd3dDevice->LightEnable(0,FALSE);									
				m_SectorScene[ix+iy*m_LSizeX].RenderEnvPlant(pd3dDevice);
			}
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

	pd3dDevice->LightEnable(0,FALSE);	
	pd3dDevice->LightEnable(1,FALSE);	
	pd3dDevice->LightEnable(2,FALSE);	
	pd3dDevice->LightEnable(3,FALSE);	
	pd3dDevice->LightEnable(4,FALSE);	
	pd3dDevice->LightEnable(5,FALSE);	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);	

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
	/*
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	*/
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);	
	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);		
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
	pd3dDevice->LightEnable(0,FALSE);
	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
	pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

	//pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);	
	//pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	
	//pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);	
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

//	delete [] IsCulling;
}

void CHeightFieldScene::SetNationFlag(unsigned char cNation)
{
	for(int ix=0;ix<m_LSizeX;ix++)
	{
		for(int iy=0;iy<m_LSizeY;iy++)
		{
			m_SectorScene[ix+iy*m_LSizeX].SetNationFlag(cNation);
		}
	}
}

void CHeightFieldScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	

	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();

	// 내부 씬일때 외부 컬링
	if(CSceneManager::GetInView())
	{
		CHouseObjectScene *pHouse  = NULL;
		if((pHouse = CSceneManager::GetInViewHouseScene()))
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
				
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			///*	
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
			//*/	

			pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

			matrix matIndent;
			matIndent.MakeIdent();	

			pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matIndent);	
			pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
			pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);	

			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&pHouse->m_AccumulateTM));
			pHouse->Render(pd3dDevice);							
		

		}
			
		CSceneManager::m_CharacterManager.Render(pd3dDevice);
		return;

	}

	
	// WaterReflection Count
	std::vector<float> WaterHeightList;
	int cDrawTerr=0;
	//bool IsCulling[LSIZEX*LSIZEY];
	//bool *IsCulling=new bool[m_LSizeX*m_LSizeY];
	
	static bool bOnce=false; 
	if( !bOnce )
	{
		bOnce = true; 
		BitSet.ResizeBits( m_LSizeX*m_LSizeY ); //by kamzic072
	}
	BitSet.ClearBits(); 

	int ix,iy;
	CSceneManager::m_cRenderSector=0;	
	
	vector3 vecMinBox,vecMaxBox;
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			// 섹터의 bounding box 를 구해 vecMinBox, vecMaxBox 에 저장
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;

			// 섹터의 bounding box 가 카메라의 view frustum 내에 포함되는지 확인
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{
				// edith AddUnique 형식 구현
				if(m_SectorScene[ix+iy*m_LSizeX].m_MapWater && m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_bWaterRelection)
				{
					int iCnt=0;
					for(int a=0;a<(int)WaterHeightList.size();a++)
					{
						if(WaterHeightList[a] == m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_fWaterHeight)
							iCnt++;
					}

					if(iCnt==0)
						WaterHeightList.push_back(m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_fWaterHeight);				
				}

				// bitset 의 해당 섹터 비트 켜고 섹터 숫자 카운트 증가
				BitSet.SetBit( ix+iy*m_LSizeX );
				CSceneManager::m_cRenderSector++;
			}
			else
			{
				//IsCulling[ix+iy*m_LSizeX]=false;
			}
		}
	}
	int nDetailSector=0;
	int DetailSectorRender[9]={0,};


	// weather mgr 의 지형 라이트를 세팅
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_TerrainLight);

// 근데.. 바로 아래서 라이트를 몽땅 끄는 이유는 뭔겨? -_-
	pd3dDevice->LightEnable(0,FALSE);	
	pd3dDevice->LightEnable(1,FALSE);	
	pd3dDevice->LightEnable(2,FALSE);	
	pd3dDevice->LightEnable(3,FALSE);	
	pd3dDevice->LightEnable(4,FALSE);	
	pd3dDevice->LightEnable(5,FALSE);	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);	


	color TerrainColor;
	TerrainColor.a=0xff;
	TerrainColor.r=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.r*255;
	TerrainColor.g=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.g*255;
	TerrainColor.b=CSceneManager::m_WeatherManager.m_SunLight.Diffuse.b*255;
	
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterTerrainColor.c);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( BitSet.GetBit(ix+iy*m_LSizeX))
			{				
				vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
				vecMaxBox.x=vecMinBox.x+SECTORSIZE;

				vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
				vecMaxBox.z=vecMinBox.z+SECTORSIZE;
// 섹터 바운딩박스 그림. 디버깅용도
// static D3DMATRIX matIdentity =
// {
// 	1, 0, 0, 0,
// 	0, 1, 0, 0,
// 	0, 0, 1, 0,
// 	0, 0, 0, 1
// };
// pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
// vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
// vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;
// CRenderBoundingBox::Render(pd3dDevice, vecMinBox, vecMaxBox);

//pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);


				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM));				

				if( vecMinBox.x <= vecViewPos->x && vecMaxBox.x >= vecViewPos->x &&
					vecMinBox.z <= vecViewPos->z && vecMaxBox.z >= vecViewPos->z )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderDetail(pd3dDevice);
					continue;
				}

				if( (fabsf(vecMinBox.z-vecViewPos->z) <= SECTORSIZE/3.0f || fabsf(vecMaxBox.z-vecViewPos->z) <= SECTORSIZE/3.0f ) &&
					vecMinBox.x <= vecViewPos->x && vecMaxBox.x >= vecViewPos->x )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderDetail(pd3dDevice);
					continue;
				}

				if( (fabsf(vecMinBox.x-vecViewPos->x) <= SECTORSIZE/3.0f || fabsf(vecMaxBox.x-vecViewPos->x) <= SECTORSIZE/3.0f ) &&
					vecMinBox.z <= vecViewPos->z && vecMaxBox.z >= vecViewPos->z )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderDetail(pd3dDevice);
					continue;
				}

				if( (fabsf(vecMinBox.x-vecViewPos->x) <= SECTORSIZE/3.0f ||
					 fabsf(vecMaxBox.x-vecViewPos->x) <= SECTORSIZE/3.0f ) &&
					(fabsf(vecMinBox.z-vecViewPos->z) <= SECTORSIZE/3.0f ||
					 fabsf(vecMaxBox.z-vecViewPos->z) <= SECTORSIZE/3.0f ) )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderDetail(pd3dDevice);
					continue;
				}
				m_SectorScene[ix+iy*m_LSizeX].Render(pd3dDevice);

//pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}
		}
	}
	//*/
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	///*
	// 물에 잠긴 world effect 를 위한 render
	RenderMapEffect();
	//if(WATERREFLECTION && WaterHeightList.num!=0)
	if(0)
	{
		pd3dDevice->EndScene();		
		
		m_SectorScene->m_WaterScene.m_pRelectionTexture.Begin(pd3dDevice);

		for(int cWaterRelection=0;cWaterRelection<(int)WaterHeightList.size();cWaterRelection++)
		{			
			pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,BaseGraphicsLayer::m_ClearColor.c, 1.0f, 0);	
			pd3dDevice->BeginScene();			
			matrix matRelf,matRes;
			matRelf.MakeIdent();
			matRelf._22=-1.0f;
			matRelf._42=WaterHeightList[cWaterRelection]*2.0f;				
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matRelf));	
			pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			CSceneManager::m_WeatherManager.m_SkyScene.RenderCloud(pd3dDevice);
			pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
			for(ix=0;ix<m_LSizeX;ix++)
			{
				for(iy=0;iy<m_LSizeY;iy++)
				{
					if(BitSet.GetBit(ix+iy*m_LSizeX))
					{					
						matRes=matRelf*m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM;				
						pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRes);				
						m_SectorScene[ix+iy*m_LSizeX].Render(pd3dDevice);
					}
				}
			}	
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matRelf));
			//CSceneManager::m_WeatherManager.m_SkyScene.Render(pd3dDevice);
			
			
			
			pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
			pd3dDevice->EndScene();
		}
		
		m_SectorScene->m_WaterScene.m_pRelectionTexture.End(pd3dDevice);	

		m_SectorScene->m_WaterScene.m_pRippleTexture.Begin(pd3dDevice);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);	
		pd3dDevice->BeginScene();

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	
		m_SectorScene->m_WaterScene.RenderRipple(pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
		pd3dDevice->EndScene();
		m_SectorScene->m_WaterScene.m_pRippleTexture.End(pd3dDevice);
		
		pd3dDevice->BeginScene();	
		
	}
	//*/
	// Sector Light Render
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( BitSet.GetBit(ix+iy*m_LSizeX))
			{				
				m_SectorScene[ix+iy*m_LSizeX].RenderSectorLight();
			}
		}
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	///*	
    pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
    pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	//*/	

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	matrix matIndent;
	matIndent.MakeIdent();	

	pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matIndent);	
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);	

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(m_bHouseCulling) {
				if(BitSet.GetBit(ix+iy*m_LSizeX))
				{				
					m_SectorScene[ix+iy*m_LSizeX].RenderHouseObject(pd3dDevice);
				}
				else 
				{
					m_SectorScene[ix+iy * m_LSizeX].RenderNotCullHouseObject(pd3dDevice);
				}

			}
			else
			{									// Culling Off 03.03.04 (arena 존 관련)
					m_SectorScene[ix+iy*m_LSizeX].RenderHouseObject(pd3dDevice);
			}
		}
	}	

	D3DMATERIAL8 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	pd3dDevice->SetMaterial( &mtrl );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	if(BaseGraphicsLayer::m_VoodooOption)
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
	/*
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
	*/
	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);


	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
    pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xee );
    pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	//pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_AmbientColor->c);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
	
	for(int cLight=1;cLight<8;cLight++)
	{
		pd3dDevice->LightEnable(cLight,FALSE);
	}
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(m_bObjectCulling) {				// 03.03.04 아레나
				if(BitSet.GetBit(ix+iy*m_LSizeX))
				{				
					m_SectorScene[ix+iy*m_LSizeX].RenderObject(pd3dDevice);
				}
			}
			else {
				m_SectorScene[ix+iy*m_LSizeX].RenderObject(pd3dDevice);
			}
		}
	}
	// 캐릭터 렌더 : water와의 렌더 순서 위해..	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
    
	CSceneManager::m_CharacterManager.Render(pd3dDevice);


	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );				
	pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );				
	pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
	pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 3 );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	


	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);				
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(BitSet.GetBit(ix+iy*m_LSizeX))
			{						
				if( m_SectorScene[ix+iy*m_LSizeX].m_MapWater)
				{
					if( m_SectorScene[ix+iy*m_LSizeX].m_MapWater->m_bWaterRelection )					
						m_SectorScene[ix+iy*m_LSizeX].m_WaterScene.DeterminWaterTexture(pd3dDevice,true);					
					else
						m_SectorScene[ix+iy*m_LSizeX].m_WaterScene.DeterminWaterTexture(pd3dDevice,false);

					m_SectorScene[ix+iy*m_LSizeX].RenderWater(pd3dDevice,false);				
				}			
			}
		}
	}

	/*


	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);

	//pd3dDevice->SetRenderState( D3DRS_AMBIENT, CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{						
				m_SectorScene[ix+iy*m_LSizeX].RenderGrass(pd3dDevice);				
			}
		}
	}
	*/

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

	if(BaseGraphicsLayer::m_VoodooOption)
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);


	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	//Sector Sorting 
	
	D3DXMATRIX matView;
	
	matrix *pmatView = CSceneManager::m_ViewCamera->GetMatPosition();

	pd3dDevice->GetTransform(D3DTS_VIEW,&matView);

	int Uindexx=(int)((pmatView->_41)/SECTORSIZE);
	int Uindexy=(int)((pmatView->_43)/SECTORSIZE);
	int Sindexx,Sindexy;
	
	int iCurrentSector = 0;

	m_iSortsNum = 0;

	D3DXVECTOR4 vecViewSectorPos;

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( BitSet.GetBit(ix+iy*m_LSizeX))
			{				
				//D3DXVECTOR3 vecSectorTmPos = D3DXVECTOR3(m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41,/*m_TM._41,*/pmatView->_42,m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43/*m_TM._43*/);
				D3DXVECTOR3 vecSectorTmPos = D3DXVECTOR3(m_SectorScene[ix+iy*m_LSizeX].m_TM._41,pmatView->_42,m_SectorScene[ix+iy*m_LSizeX].m_TM._43);

			
				D3DXVec3Transform(&vecViewSectorPos,&vecSectorTmPos,&matView);

				m_pSectorSorts[m_iSortsNum].fViewDist = vecViewSectorPos.z;
				m_pSectorSorts[m_iSortsNum++].iSectorIndex = (ix+iy*m_LSizeX);
				
			}

		}
	}
	
	qsort((void *)m_pSectorSorts,(size_t)m_iSortsNum, sizeof(SectorSortNode_t),Sector_tCompare);

	for(int iSortSector = 0;iSortSector < m_iSortsNum;iSortSector++ )
	{
		CSectorScene *pSort = &(m_SectorScene[m_pSectorSorts[iSortSector].iSectorIndex]);

		Sindexx=(int)(pSort->m_TM._41/SECTORSIZE);
		Sindexy=(int)(pSort->m_TM._43/SECTORSIZE);

		if(!((Uindexx == Sindexx) && (Uindexy == Sindexy))) {
			pd3dDevice->LightEnable(0,FALSE);									
			pSort->RenderPlant(pd3dDevice);
		}
		else {
			iCurrentSector = m_pSectorSorts[iSortSector].iSectorIndex;
		}

	}
	// 현재 current Sector 마지막에 Renderring
	pd3dDevice->LightEnable(0,FALSE);
	m_SectorScene[iCurrentSector].RenderPlant(pd3dDevice);

/*	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{				
				pd3dDevice->LightEnable(0,FALSE);									
				m_SectorScene[ix+iy*m_LSizeX].RenderPlant(pd3dDevice);
			}
		}
	}
*/
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

	pd3dDevice->LightEnable(0,FALSE);	
	pd3dDevice->LightEnable(1,FALSE);	
	pd3dDevice->LightEnable(2,FALSE);	
	pd3dDevice->LightEnable(3,FALSE);	
	pd3dDevice->LightEnable(4,FALSE);	
	pd3dDevice->LightEnable(5,FALSE);	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);	
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);	

	

	//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);
	// 08.27 막음
	/*pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterFurGrassColor.c);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);	


	
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{				
				pd3dDevice->LightEnable(0,FALSE);					
				//pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	
				m_SectorScene[ix+iy*m_LSizeX].RenderGrass(pd3dDevice);
			}
		}
	}
	*/
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
	/*
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	*/
	pd3dDevice-> SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);	
	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);		
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(BitSet.GetBit(ix+iy*m_LSizeX))
			{					
				m_SectorScene[ix+iy*m_LSizeX].RenderFall(pd3dDevice);
			}
		}
	}
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
	pd3dDevice->LightEnable(0,FALSE);
	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
	pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

	//pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);	
	//pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	
	//pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);	
	pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(BitSet.GetBit(ix+iy*m_LSizeX))
			{
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM));				

				vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
				vecMaxBox.x=vecMinBox.x+SECTORSIZE;

				vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
				vecMaxBox.z=vecMinBox.z+SECTORSIZE;
				
				if( vecMinBox.x <= vecViewPos->x && vecMaxBox.x >= vecViewPos->x &&
					vecMinBox.z <= vecViewPos->z && vecMaxBox.z >= vecViewPos->z )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderLandscapeEffect(pd3dDevice);
					//m_SectorScene[ix+iy*m_LSizeX].UpdateLandscapeEffect();
					continue;
				}

				if( (fabsf(vecMinBox.z-vecViewPos->z) <= SECTORSIZE/2.0f || fabsf(vecMaxBox.z-vecViewPos->z) <= SECTORSIZE/2.0f ) &&
					vecMinBox.x <= vecViewPos->x && vecMaxBox.x >= vecViewPos->x )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderLandscapeEffect(pd3dDevice);
					//m_SectorScene[ix+iy*m_LSizeX].UpdateLandscapeEffect();
				
					continue;
				}

				if( (fabsf(vecMinBox.x-vecViewPos->x) <= SECTORSIZE/2.0f || fabsf(vecMaxBox.x-vecViewPos->x) <= SECTORSIZE/2.0f ) &&
					vecMinBox.z <= vecViewPos->z && vecMaxBox.z >= vecViewPos->z )
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderLandscapeEffect(pd3dDevice);
					//m_SectorScene[ix+iy*m_LSizeX].UpdateLandscapeEffect();
				
					continue;
				}				
				
				if( (fabsf(vecMinBox.x-vecViewPos->x) <= SECTORSIZE/2.0f ||
					 fabsf(vecMaxBox.x-vecViewPos->x) <= SECTORSIZE/2.0f ) &&					 
					(fabsf(vecMinBox.z-vecViewPos->z) <= SECTORSIZE/2.0f ||
					 fabsf(vecMaxBox.z-vecViewPos->z) <= SECTORSIZE/2.0f ) )					 
				{
					m_SectorScene[ix+iy*m_LSizeX].RenderLandscapeEffect(pd3dDevice);
					//m_SectorScene[ix+iy*m_LSizeX].UpdateLandscapeEffect();
				
					continue;
				}				
	
			}
		}
	}
/*	if(IsCulling) {
		delete [] IsCulling;
		IsCulling = NULL;
	}*/

}
void CHeightFieldScene::RenderMapEffect() {

	static bool bOnce=false; 
	if( !bOnce )
	{
		bOnce = true; 
		BitSetEffect.ResizeBits(m_LSizeX*m_LSizeY); //by kamzic072
	}
	BitSetEffect.ClearBits(); 

	//bool *IsCulling=new bool[m_LSizeX*m_LSizeY];
	int ix,iy;
	CSceneManager::m_cRenderSector=0;	
	
	vector3 vecMinBox,vecMaxBox;
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{			
				
				BitSetEffect.SetBit(ix+iy*m_LSizeX); 			
				CSceneManager::m_cRenderSector++;
			}
			else
			{
				//IsCulling[ix+iy*m_LSizeX]=false;
			}
		}
	}


	//Effect
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(m_bEffectCulling) {					// Culling 적용 03.03.04
				//if(IsCulling[ix+iy*m_LSizeX])
				//{
					m_SectorScene[ix+iy*m_LSizeX].RenderEffect(BitSetEffect.GetBit(ix+iy*m_LSizeX) );
				//}

			}
			else {								// Culling 무시 03.03.04 
					m_SectorScene[ix+iy*m_LSizeX].RenderEffect();
			}
		}
	}
/*	if(IsCulling) {
		delete[] IsCulling;
		IsCulling = NULL;
	}*/
}
void CHeightFieldScene::Tessellate()
{
	m_SectorScene->m_NextTriNode=0;
	CSectorScene *Patch;	
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			Patch=m_QuadSector[ix+iy*m_LSizeY];			
			Patch->Reset();
			
			if(ix>0)
				Patch->m_BaseLeft.LeftNeighbor=&m_QuadSector[(ix-1)+iy*m_LSizeX]->m_BaseRight;
			else
				Patch->m_BaseLeft.LeftNeighbor=NULL;
			if(ix<m_LSizeX-1)
				Patch->m_BaseRight.LeftNeighbor=&m_QuadSector[(ix+1)+iy*m_LSizeX]->m_BaseLeft;
			else
				Patch->m_BaseRight.LeftNeighbor=NULL;	
			
			if(iy>0)
				Patch->m_BaseLeft.RightNeighbor=&m_QuadSector[ix+(iy-1)*m_LSizeX]->m_BaseRight;
			else
				Patch->m_BaseLeft.RightNeighbor=NULL;
			if(iy<m_LSizeY-1)
				Patch->m_BaseRight.RightNeighbor=&m_QuadSector[ix+(iy+1)*m_LSizeX]->m_BaseLeft;
			else
				Patch->m_BaseRight.RightNeighbor=NULL;				
		}
	}

	for(int i=0;i<m_LSizeX*m_LSizeY;i++)
		m_SectorScene[i].Tessellate();

	for(int i=0;i<m_LSizeX*m_LSizeY;i++)
		m_SectorScene[i].PrepareRender();	
}

bool CHeightFieldScene::CheckingTessellate()
{
	vector3 *ViewPos;
	vector3 CharPos;
	if(CSceneManager::m_ViewerMode==0 || CSceneManager::m_ViewerMode==2)
		ViewPos=CSceneManager::GetCamera()->GetPosition();
	else
	{		
 		//CSceneManager::m_CharacterList[0]->GetPosition(CharPos.x,CharPos.y,CharPos.z);
		if( CSceneManager::m_CharacterManager.m_CharacterList.empty())
		{
			ViewPos=CSceneManager::GetCamera()->GetPosition();
		}
		else
		{
			if(CSceneManager::m_pCameraScript->ISPlay()) {
				D3DXVECTOR3 vecTmpPos = CSceneManager::m_pCameraScript->GetVecPos();
				vector3 vecTmpPos2 = vector3(vecTmpPos.x,vecTmpPos.y,vecTmpPos.z);
				ViewPos = &(vecTmpPos2);

				/*matrix *matPos = CSceneManager::m_ViewCamera->GetMatPosition();
				vector3 vecTmpPos = vector3(matPos->_41,matPos->_42,matPos->_43);
				ViewPos = &(vecTmpPos);
				*/
				//ViewPos=CSceneManager::GetCamera()->GetPosition();
			}
			else {
				CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(CharPos.x,CharPos.y,CharPos.z);
				ViewPos=&CharPos;
			}
		}
	}

	//vector3 *ViewPos=CSceneManager::GetCamera()->GetPosition();
	if( m_NowTessellatePosX == (int)(ViewPos->x/(SECTORSIZE/FIELDDIVIDE)) &&
		m_NowTessellatePosY == (int)(ViewPos->z/(SECTORSIZE/FIELDDIVIDE)))
		return false;	
	Tessellate();
	m_NowTessellatePosX=(int)(ViewPos->x/(SECTORSIZE/FIELDDIVIDE));
	m_NowTessellatePosY=(int)(ViewPos->z/(SECTORSIZE/FIELDDIVIDE));
	m_GrassSceneManager.UpdateRange();

	return true;
}

/*
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	matrix *matView=(matrix*)CSceneManager::GetCamera()->GetMatView();
	matrix matFront;
	matFront.Translation(vector3(0.0f,0.0f,1.0f));
	matFront=matFront*(*matViewPosition);
	vector3 vecRelectionDir;
	vector3 vecViewPos=matViewPosition->GetLoc();
	vecRelectionDir=matFront.GetLoc()-vecViewPos;
	vecRelectionDir.y=-vecRelectionDir.y;
	//vecRelectionDir.y-=(2.0f*(vecRelectionDir.y-vecViewPos.y));	
	//vecViewPos.y=-vecViewPos.y;
	//vecViewPos.y+=3000.0f*2.0f;	
	vector3 vecUp;	
	vecUp.x=matView->_12;
	vecUp.y=-1.0f;//matView->_22;
	vecUp.z=matView->_32;
	vecUp.Normalize();
	/*
	vecRelectionDir.x=matView->_13;
	vecRelectionDir.y=matView->_23;
	vecRelectionDir.z=matView->_33;	
	
	/*
	vecRelectionDir.x=matView->_13;
	vecRelectionDir.y=1.0f;
	vecRelectionDir.z=matView->_33;	
	//
	vecRelectionDir.x=0.0f;
	vecRelectionDir.y=1.0f;
	vecRelectionDir.z=0.0f;
	vecRelectionDir.Normalize();
	//matRelectionView.CameraLookAt(vecViewPos,vecRelectionDir,vecUp);
	//matRelectionView.CameraLookAt(vecViewPos,vecRelectionDir,vecUp);	
	//pd3dDevice->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&matRelectionView);	
	
	matFront.MakeIdent();
	matFront._22=-1.0f;

	matrix temp;
	*/


float CHeightFieldScene::GetHeight(vector3 vecPos)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{	
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{
				return m_SectorScene[ix+iy*m_LSizeX].GetHeight(vecPos);
			}
		}
	}
	return 0.0f;
}

void CHeightFieldScene::GetHeightFieldShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList)
{	
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{
				//m_SectorScene[ix+iy*LSIZEX].GetShadowPoly(vecPos,vecPolyList);
				m_SectorScene[ix+iy*m_LSizeX].GetHeightFieldShadowPoly(vecPos,vecPolyList);
				break;
			}
		}
	}
}

void CHeightFieldScene::GetLight(vector3 vecPos,std::vector<D3DLIGHT8> &LightList)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{
				m_SectorScene[ix+iy*m_LSizeX].GetLight(vecPos,LightList);				
				break;
			}
		}
	}
}

void CHeightFieldScene::GetInHouseShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList,vector3 vecLightPos)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{
				m_SectorScene[ix+iy*m_LSizeX].GetInHouseShadowPoly(vecPos,vecPolyList,vecLightPos);				
				break;
			}
		}
	}
}

void CHeightFieldScene::GetLineIntersectPoly(vector3 vecStart, vector3 vecEnd, std::vector<vector3> &vecPolyList)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)		
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecStart.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecStart.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecStart.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecStart.z)
			{
				m_SectorScene[ix+iy*m_LSizeX].GetLineIntersectPoly(vecStart,vecEnd,vecPolyList);
				break;
			}
		}
	}
}

void CHeightFieldScene::AllObjectCalcLens()
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			m_SectorScene[ix+iy*m_LSizeX].AllObjectCalcLens();
		}
	}
}

void CHeightFieldScene::GetHeightFieldRangePoly(vector3 vecPos,std::vector<vector3> &vecPolyList, float fRange)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{				
				m_SectorScene[ix+iy*m_LSizeX].GetHeightFieldRangePoly(vecPos,vecPolyList,fRange);
				break;
			}
		}
	}
}

char* CHeightFieldScene::GetHeightFieldShadowCollision(vector3 vecPos)
{
	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if( m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41 <= vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41+SECTORSIZE > vecPos.x &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43 <= vecPos.z &&
				m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43+SECTORSIZE > vecPos.z)
			{		
				if(m_SectorScene[ix+iy*m_LSizeX].m_ShadowUsed==-1)
					return NULL;

				return m_SectorScene[ix+iy*m_LSizeX].m_SectorShadowCollision[
					m_SectorScene[ix+iy*m_LSizeX].m_ShadowUsed];		
				
			}
		}
	}
	return NULL;
}

void CHeightFieldScene::Update(float fUpdate)
{
	m_GrassSceneManager.Update(fUpdate/30.0f);

	int ix,iy;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			m_SectorScene[ix+iy*m_LSizeX].m_TreeObjectNode.Update(fUpdate/60.0f);
			
			m_SectorScene[ix+iy*m_LSizeX].UpdateLandscapeEffect();
		}
	}
	float fTime = CSceneManager::GetWeatherTime();
	// 캐릭터 토치
	if((fTime > 20.0f || fTime < 4.0f) && CSceneManager::m_bCharacterLightOp)
		CSceneManager::m_bCharacterLight = true;
	else
		CSceneManager::m_bCharacterLight = false;

	vector3 vecMinBox,vecMaxBox;

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{				
				if(fTime > 19.0f || fTime < 5.0f)
					m_SectorScene[ix+iy*m_LSizeX].SettingNight(true);
				else
					m_SectorScene[ix+iy*m_LSizeX].SettingNight(false);
			}
		}
	}
	



}


void CHeightFieldScene::GenerateSectorSceneObjects( int sectorX, int sectorY )
{
	for( int cSector = 0; cSector < GetLSizeX()*GetLSizeY(); cSector++ )
	{
		int sectorindexx = m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
		int sectorindexy = m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;

		if( sectorX == sectorindexx && sectorY == sectorindexy )
			m_SectorScene[cSector].GenerateObject();
	}
}

void CHeightFieldScene::RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice)
{
	int ix,iy;

	vector3 vecMinBox,vecMaxBox;
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{					
				if(CRenderOption::m_ObjectProjectShadow)
					m_SectorScene[ix+iy*m_LSizeX].RenderObjectShadow(pd3dDevice);			
				pd3dDevice->SetTransform(D3DTS_WORLD,m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM);
				if(CRenderOption::m_TreeProjectShadow)
					m_SectorScene[ix+iy*m_LSizeX].m_TreeObjectNode.RenderShadow(pd3dDevice,CSectorScene::m_pShadowTexture);
				
			}			
		}
	}
	//m_GrassSceneManager.RenderShadow(pd3dDevice,CSectorScene::m_pShadowTexture);
}

int CHeightFieldScene::AddShadowPoly(vector3 vecPos,int nMethod)
{
	static int nID=0;
	CShadowedPoly AddNode;
	AddNode.m_vecShadowPos=vecPos;
	AddNode.m_nGatherMethod=nMethod;
	AddNode.m_nShadowID=nID;
	AddNode.m_bActive=true;
	m_ShadowedVertexList.push_back(AddNode);		
	nID++;
	return nID-1;
}

CHeightFieldScene::CShadowedPoly* CHeightFieldScene::GetShadowPolyList(int nID)
{
	for(int i=0;i<(int)m_ShadowedVertexList.size();i++)
	{
		if(m_ShadowedVertexList[i].m_nShadowID==nID)
		{
			return &m_ShadowedVertexList[i];
		}
	}
	return NULL;
}

void CHeightFieldScene::DeleteShadowPolyList(int nID)
{
	for(int i=0;i<(int)m_ShadowedVertexList.size();i++)
	{
		if(m_ShadowedVertexList[i].m_nShadowID==nID)
		{
			m_ShadowedVertexList.erase(m_ShadowedVertexList.begin()+i);
			return;
		}
	}
	
}

void CHeightFieldScene::RebuildShadowPoly()
{
	for(int i=0;i<(int)m_ShadowedVertexList.size();i++)
	{
		m_ShadowedVertexList[i].m_ShadowedPolyList.clear();

		if(m_ShadowedVertexList[i].m_nGatherMethod==0)
		{
			GetHeightFieldShadowPoly(m_ShadowedVertexList[i].m_vecShadowPos,
				m_ShadowedVertexList[i].m_ShadowedPolyList);
		}
		else
		{
			GetHeightFieldRangePoly(m_ShadowedVertexList[i].m_vecShadowPos,
				m_ShadowedVertexList[i].m_ShadowedPolyList,3000.0f);
		}
	}
}

void CHeightFieldScene::RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
//	bool *IsCulling=new bool[m_LSizeX*m_LSizeY];
	int ix,iy;
	CSceneManager::m_cRenderSector=0;	
	
	vector3 vecMinBox,vecMaxBox;
	
	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			vecMinBox.x=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMinBox.y;
			vecMaxBox.y=m_SectorScene[ix+iy*m_LSizeX].m_vecMaxBox.y;	

			vecMinBox.z=m_SectorScene[ix+iy*m_LSizeX].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{				
				IsCulling[ix+iy*m_LSizeX]=true;								
			}
			else
			{
				IsCulling[ix+iy*m_LSizeX]=false;
			}
		}
	}

	for(ix=0;ix<m_LSizeX;ix++)
	{
		for(iy=0;iy<m_LSizeY;iy++)
		{
			if(IsCulling[ix+iy*m_LSizeX])
			{				
				m_SectorScene[ix+iy*m_LSizeX].RenderGlare(pd3dDevice);
			}
		}
	}	
/*
	if(IsCulling) {
		delete [] IsCulling;
		IsCulling = NULL;
	}
*/
}

void CHeightFieldScene::InitNowPos() {

	m_NowPositionX=-100000000;
	m_NowPositionY=-100000000;
	m_NowTessellatePosX=-100000000;
	m_NowTessellatePosY=-100000000;

}

int Sector_tCompare(const void *arg1,const void *arg2) {
	
	if(((SectorSortNode_t *)arg1)->fViewDist < ((SectorSortNode_t *)arg2)->fViewDist)
		return 1;
	else if(((SectorSortNode_t *)arg1)->fViewDist > ((SectorSortNode_t *)arg2)->fViewDist)
		return -1;
	return 0;

}

bool CHeightFieldScene::CheckRayCollision(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2)
{

	int iMinX,iMaxX,iMinY,iMaxY;

	int iSectorX1,iSectorY1,iSectorX2,iSectorY2;

	iSectorX1 = (int)(vecPos1.x / SECTORSIZE);
	iSectorY1 = (int)(vecPos1.z / SECTORSIZE);

	iSectorX2 = (int)(vecPos2.x / SECTORSIZE);
	iSectorY2 = (int)(vecPos2.z / SECTORSIZE);

	if(iSectorX1 > iSectorX2)
	{
		iMinX = iSectorX2;
		iMaxX = iSectorX1;
	}
	else
	{
		iMinX = iSectorX1;
		iMaxX = iSectorX2;

	}

	if(iSectorY1 > iSectorY2)
	{
		iMinY = iSectorY2;
		iMaxY = iSectorY1;
	}
	else
	{
		iMinY = iSectorY1;
		iMaxY = iSectorY2;
	}


	
	int iIndex = -1;
	for(int i = iMinX; i <= iMaxX; i++ )
	{
		for(int j = iMinY; j <= iMaxY; j++ )
		{

			if((iIndex = GetSectorIndex(i,j)) != -1)
			{
				if(m_SectorScene[iIndex].CheckRayCollisionHouse(vecPos1,vecPos2))
					return true;
				
			}
		}
	}
	return false;

}
int CHeightFieldScene::GetSectorIndex(const int iSX,const int iSY)
{

	int iX,iY;
	for(int ix=0;ix<m_LSizeX;ix++)
	{
		for(int iy=0;iy<m_LSizeY;iy++)
		{
			int iIndex = ix+iy*m_LSizeX;
			iX=(int)((m_SectorScene[iIndex].m_TM._41)/SECTORSIZE);
			iY=(int)((m_SectorScene[iIndex].m_TM._43)/SECTORSIZE);
			if(iSX == iX && iSY == iY)
				return iIndex;
		}
	}		

	return -1;

}