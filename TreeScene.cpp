// TreeScene.cpp: implementation of the CTreeScene class.
//
//////////////////////////////////////////////////////////////////////

#include "TreeScene.h"
#include "SceneManager.h"
#include "RenderOption.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TREESIZEX 1500.0f
#define TREESIZEY 2700.0f

CMeshObject *CTreeScene::m_NormalTreeMesh[MAX_TREEKIND];
CMeshObject *CTreeScene::m_NormalTreeMeshLOD[MAX_TREEKIND];

CMeshObject *CTreeScene::m_NormalTreeLeaves[MAX_TREEKIND];
CMeshObject *CTreeScene::m_NormalTreeTrunk[MAX_TREEKIND];

CTreeScene::CTreeScene()
{	
	m_pHeight=NULL;
	m_ViewPlants = NULL;
	m_iViewPlantsNum = 0;


}

CTreeScene::~CTreeScene()
{	
	if(m_ViewPlants) 
	{
		delete[] m_ViewPlants;
		m_ViewPlants = NULL;
	}

	//m_pPlantVertex->Release();
	//m_pPlantIndices->Release();	
}

void CTreeScene::Create()
{
	//LogMessage("Tree Create");

	//LogMessage("Tree Object Create VertexBuffer& IndexBuffer Create");
	/*
	CSceneManager::GetDevice()->CreateVertexBuffer( MAX_TREE*8*sizeof(SimpleVertex),
													D3DUSAGE_WRITEONLY,SimpleFVF,D3DPOOL_MANAGED,
													&m_pPlantVertex);													
	CSceneManager::GetDevice()->CreateIndexBuffer( MAX_TREE*12*sizeof(WORD),D3DUSAGE_WRITEONLY,
													D3DFMT_INDEX16,D3DPOOL_MANAGED,
													&m_pPlantIndices);
													*/
	//LogMessage("Tree Object Create VertexBuffer& IndexBuffer Create Complete");
	/*
	WORD *pPlantIndices;	
	m_pPlantIndices->Lock(0,MAX_TREE*12*sizeof(WORD),(BYTE**)&pPlantIndices,0);
	for(int cPlant=0;cPlant<MAX_TREE;cPlant++)
	{
		*(pPlantIndices++)=cPlant*8+0;
		*(pPlantIndices++)=cPlant*8+1;
		*(pPlantIndices++)=cPlant*8+2;
		*(pPlantIndices++)=cPlant*8+1;
		*(pPlantIndices++)=cPlant*8+2;
		*(pPlantIndices++)=cPlant*8+3;

		*(pPlantIndices++)=cPlant*8+4;
		*(pPlantIndices++)=cPlant*8+5;
		*(pPlantIndices++)=cPlant*8+6;
		*(pPlantIndices++)=cPlant*8+5;
		*(pPlantIndices++)=cPlant*8+6;
		*(pPlantIndices++)=cPlant*8+7;
	}
	m_pPlantIndices->Unlock();
	*/
	m_fRot=0.0f;
	/*
	if(m_NormalTreeMesh[0]==NULL)
	{
		//LogMessage("Tree Object load normaltree");
		CTexture::SetPath(OBJECTTEXTUREPATH);
		CSceneManager::m_MeshObjectContainer.SetPath(TREEOBJECTPATH);			

		char strName[256];
		for(int i=0;i<MAX_TREEKIND;i++)
		{
			sprintf(strName,"NormalTree%d.R3S",i+1);
			m_NormalTreeMesh[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);		
			if(CRenderOption::m_TreeAnimation)
				m_NormalTreeMesh[i]->ConvertTree();	
			else
				m_NormalTreeMesh[i]->ConvertNormal();
			if(i+1<10)
				sprintf(strName,"tree0%d_lod_fr.R3S",i+1);
			else
				sprintf(strName,"tree%d_lod_fr.R3S",i+1);				

			m_NormalTreeMeshLOD[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);
			if(CRenderOption::m_TreeAnimation)
				m_NormalTreeMeshLOD[i]->ConvertTree();		
			else
				m_NormalTreeMeshLOD[i]->ConvertNormal();

			
		}
	}
	*/
	//LogMessage("Tree Create Complete");
}

void CTreeScene::GeneratePlant(int SeedX,int SeedY,float *pHeightData,float fAvgHeight)
{		
	srand((SeedX+SeedY)/100);		
	int cManySeed=rand()%MAX_SEED;

	int PutSeedX[MAX_SEED],PutSeedY[MAX_SEED];
	int PutRange[MAX_SEED];
	int PutSeedPoss[MAX_SEED];
	//int ix,iy;
	//int iSeed;
	//int InterX,InterY;
	int cPlant=0;

	m_pHeight=pHeightData;
	m_cPlant=0;
	memset(m_usDetailTree,0xff,sizeof(unsigned char)*SECTORSX*SECTORSY);	

	for(int i=0;i<cManySeed;i++)
	{
		PutRange[i]=rand()%MAX_SEEDRANGE;
		PutSeedX[i]=rand()%(SECTORSX-(PutRange[i]*2));
		PutSeedY[i]=rand()%(SECTORSY-(PutRange[i]*2));
		PutSeedX[i]+=PutRange[i];
		PutSeedY[i]+=PutRange[i];
		PutSeedPoss[i]=(rand()%100)+90;
	}	
	/*
	for(ix=0;ix<SECTORSX;ix++)
	{
		for(iy=0;iy<SECTORSY;iy++)
		{
			for(iSeed=0;iSeed<cManySeed;iSeed++)
			{
				InterX=PutSeedX[iSeed]-ix;
				InterY=PutSeedY[iSeed]-iy;
				if(CFastMath::FastSqrt((InterX*InterX)+(InterY*InterY)) <= PutRange[iSeed])
				{
					if(m_usDetailTree[ix+iy*SECTORSX]==0xff)
					{
						if(cPlant>=MAX_TREE)					
							continue;						
						if(PutSeedPoss[iSeed] > (rand()%100))
							continue;						
						if(pHeightData[ix+iy*SECTORSX]>FIELDMIDDLE)
							continue;						
						m_usDetailTree[ix+iy*SECTORSX]=rand()%MAX_TREEKIND;
						cPlant++;										
					}
					break;
				}
			}
		}
	}
	*/
	m_cPlant=cPlant;
	if(m_ViewPlants != NULL)
		delete[] m_ViewPlants;
	m_ViewPlants = new TreeNode_t[cPlant];

}
void CTreeScene::RenderEnv(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(m_cPlant==0)
		return;

	matrix matParent;
	D3DXMATRIX matView;

	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)&matParent);
	pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	


	matrix matPlantPos;	
	vector3 vecPlantPos;
	
	


	matrix matRot;		
	unsigned char usTreeKind;
	if(CRenderOption::m_TreeAnimation)
	{
		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);
		matRot.Rotation(vector3(1.0f,0.0f,1.0f),(sinf(m_fRot))*0.06f);
	}
	m_iViewPlantsNum = 0;

	D3DXVECTOR4 vecViewPos;

//////////////////////
/*	*/
	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
				vecPlantPos.x=ix*LINTERVAL+matParent._41;
				vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
				vecPlantPos.z=iy*LINTERVAL+matParent._43;

				matPlantPos.Translation(vecPlantPos);

				m_AccumulateTM=matPlantPos;
				m_AccumulateTM._42-=10.0f;				

				matRot._41=m_AccumulateTM._41;
				matRot._42=m_AccumulateTM._42;
				matRot._43=m_AccumulateTM._43;

				m_vecMinBox=m_NormalTreeMesh[usTreeKind]->m_MinBox;
				m_vecMaxBox=m_NormalTreeMesh[usTreeKind]->m_MaxBox;
				//Init();
				
				if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
				{
					if(isAABBCulling())
					{	
						D3DXVECTOR3 vecPos = D3DXVECTOR3(vecPlantPos.x,vecPlantPos.y,vecPlantPos.z);
						D3DXVec3Transform(&vecViewPos,&vecPos,&matView);
						m_ViewPlants[m_iViewPlantsNum].fViewDist = vecViewPos.z;
						m_ViewPlants[m_iViewPlantsNum].matRot = matRot;
						m_ViewPlants[m_iViewPlantsNum].matTm = m_AccumulateTM;
						m_ViewPlants[m_iViewPlantsNum].ucKind = usTreeKind;
						m_ViewPlants[m_iViewPlantsNum++].iIndex = (ix+iy*SECTORSX);
						

					}
				}
				else
				{	
					
					if(m_usDetailTree[ix+iy*SECTORSX]&0x40)
					{
						if(isAABBCulling())
						{		
							
							D3DXVECTOR3 vecPos = D3DXVECTOR3(vecPlantPos.x,vecPlantPos.y,vecPlantPos.z);
							D3DXVec3Transform(&vecViewPos,&vecPos,&matView);
							m_ViewPlants[m_iViewPlantsNum].fViewDist = vecViewPos.z;
							m_ViewPlants[m_iViewPlantsNum].matRot = matRot;
							m_ViewPlants[m_iViewPlantsNum].matTm = m_AccumulateTM;
							m_ViewPlants[m_iViewPlantsNum].ucKind = usTreeKind;

							m_ViewPlants[m_iViewPlantsNum++].iIndex = (ix+iy*SECTORSX);

						}
					}						
				}				
			}
		}
	}	


	qsort((void *)m_ViewPlants,(size_t)m_iViewPlantsNum, sizeof(TreeNode_t),Tree_tCompare);

	
	float fFar = CSceneManager::m_WeatherManager.m_InterFogFar + 200.0f;
	vector3 *vecViewPos2=CSceneManager::GetCamera()->GetPosition();
	D3DXVECTOR3 vecDist;
	float fDist;

	if(fFar > 48000.0f)
		fFar = 48000.0f;


	for(int iSortCount = 0; iSortCount < m_iViewPlantsNum; iSortCount++) {
		int iPlantIndex = m_ViewPlants[iSortCount].iIndex; 
	
		
		D3DXVECTOR3 vecTreeCenter = D3DXVECTOR3(m_AccumulateTM._41,
														m_AccumulateTM._42,
														m_AccumulateTM._43);
		vecDist.x = vecViewPos2->x - vecTreeCenter.x;
		vecDist.y = vecViewPos2->y - vecTreeCenter.y;
		vecDist.z = vecViewPos2->z - vecTreeCenter.z;
		fDist = D3DXVec3Length(&vecDist);
		if(fDist > fFar)
		{
			break;
		}
		
		if(m_usDetailTree[iPlantIndex]&0x80) {

			bool bLight = false;


			// character Light 영향 유무
			if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
			{
			

				D3DXVECTOR3 vecPos = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x,
													CSceneManager::m_CharacterLight.Position.y,
													CSceneManager::m_CharacterLight.Position.z);
				
			
				if(D3DXVec3Length(&(vecPos - vecTreeCenter)) <= (CSceneManager::m_CharacterLight.Range + 100.0f))
				{
					pd3dDevice->SetLight(1,&(CSceneManager::m_CharacterLight));
					pd3dDevice->LightEnable(1,TRUE);
					bLight = true;
				}					
			}

			//pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
			if(CRenderOption::m_TreeAnimation)
			{
				pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matRot);
			}
			else
			{
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
			}						
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);
			m_NormalTreeMesh[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);						
		
			if(bLight == true)
			{
				pd3dDevice->LightEnable(1,FALSE);
			}

		}
		else {
			if(m_usDetailTree[iPlantIndex]&0x40) {
			
				if(CRenderOption::m_TreeAnimation)
				{								
					pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
					pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matRot);						
				}
				else
				{
					pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

				if(m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind])
					m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);

				

				if(CRenderOption::m_FullSceneGlare)
				{
					pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
					pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);	
					if(m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind])
						m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);
					pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
				}

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

			}
		}
	}

//////////////////////
	
/*
	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
				vecPlantPos.x=ix*LINTERVAL+matParent._41;
				vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
				vecPlantPos.z=iy*LINTERVAL+matParent._43;

				matPlantPos.Translation(vecPlantPos);

				m_AccumulateTM=matPlantPos;
				m_AccumulateTM._42-=10.0f;				

				matRot._41=m_AccumulateTM._41;
				matRot._42=m_AccumulateTM._42;
				matRot._43=m_AccumulateTM._43;

				m_vecMinBox=m_NormalTreeMesh[usTreeKind]->m_MinBox;
				m_vecMaxBox=m_NormalTreeMesh[usTreeKind]->m_MaxBox;
				//Init();
				
				if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
				{
					if(isAABBCulling())
					{	
						bool bLight = false;

						// character Light 영향 유무
						if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
						{
						

							D3DXVECTOR3 vecPos = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x,
																CSceneManager::m_CharacterLight.Position.y,
																CSceneManager::m_CharacterLight.Position.z);
							
							D3DXVECTOR3 vecTreeCenter = D3DXVECTOR3(m_AccumulateTM._41,
																	m_AccumulateTM._42,
																	m_AccumulateTM._43);

							if(D3DXVec3Length(&(vecPos - vecTreeCenter)) <= (CSceneManager::m_CharacterLight.Range + 100.0f))
							{
								pd3dDevice->SetLight(1,&(CSceneManager::m_CharacterLight));
								pd3dDevice->LightEnable(1,TRUE);
								bLight = true;
							}					
						}

						//pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
						if(CRenderOption::m_TreeAnimation)
						{
							pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_AccumulateTM);
							pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRot);
						}
						else
						{
							pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
						}
							

						//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);	
						//if(usTreeKind==0)
						if(0)
						{	
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);	
							m_NormalTreeTrunk[usTreeKind]->Render(pd3dDevice);
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);							
							m_NormalTreeLeaves[usTreeKind]->Render(pd3dDevice);							
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
						}
						else
						{
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);
							m_NormalTreeMesh[usTreeKind]->Render(pd3dDevice);						
						}
						if(bLight == true)
						{
							pd3dDevice->LightEnable(1,FALSE);
						}
					}
				}
				else
				{	
					
					if(m_usDetailTree[ix+iy*SECTORSX]&0x40)
					{
						if(isAABBCulling())
						{		
							
							if(CRenderOption::m_TreeAnimation)
							{								
								pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_AccumulateTM);
								pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRot);						
							}
							else
							{
								pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
							}

							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

							if(m_NormalTreeMeshLOD[usTreeKind])
								m_NormalTreeMeshLOD[usTreeKind]->Render(pd3dDevice);

							

							if(CRenderOption::m_FullSceneGlare)
							{
								pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
								pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);	
								if(m_NormalTreeMeshLOD[usTreeKind])
									m_NormalTreeMeshLOD[usTreeKind]->Render(pd3dDevice);
								pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
							}

							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
						}
					}						
				}				
			}
		}
	}	*/
//////////////////////


	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matParent);
	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
}
void CTreeScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	if(m_cPlant==0)
		return;

	matrix matParent;
	D3DXMATRIX matView;

	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)&matParent);
	pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	


	matrix matPlantPos;	
	vector3 vecPlantPos;
	
	
	/*
	if(GF3OPTION)
	{
		matrix matProjection,matHeightView,matTexScale,matInv,matView,mat,mat2,matTex;
		matProjection.MakeProjection(3.14159f/3.0f,1.0f,50,100000.0f);
		float fViewHeight=(SECTORSIZE/2.0f)/tanf(3.14159f/6.0f);

		matrix matWorld;
		pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
		float fCloudMoveX=(int)(matWorld._41/SECTORSIZE);
		float fCloudMoveZ=(int)(matWorld._43/SECTORSIZE);		

		matHeightView.CameraLookAt(vector3(SECTORSIZE/2.0f+fCloudMoveX*SECTORSIZE,-fViewHeight,SECTORSIZE/2.0f+fCloudMoveZ*SECTORSIZE),
			vector3(SECTORSIZE/2.0f+fCloudMoveX*SECTORSIZE,0.0f,SECTORSIZE/2.0f+fCloudMoveZ*SECTORSIZE),
			vector3(0.0f,0.0f,-1.0f));

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
		
		mat=matProjection*matTexScale;
		mat2=matHeightView*mat;   
		matTex=matInv*mat2;

		matrix matTrans;
		matTrans.MakeIdent();
		matTrans._11=0.11f;
		matTrans._22=0.11f;				

		

		matTrans._31=CSectorScene::m_fCloudMove+fCloudMoveX*0.11f;
		matTrans._32=CSectorScene::m_fCloudMove+fCloudMoveZ*0.11f;					
		matTex=matTex*matTrans;

		pd3dDevice->SetTransform(D3DTS_TEXTURE1,(D3DMATRIX*)&matTex);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTexture(1,CSectorScene::m_CloudShadow.GetTexture());
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);			
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);		

	}			
	*/

	matrix matRot;		
	unsigned char usTreeKind;
	if(CRenderOption::m_TreeAnimation)
	{
		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);
		matRot.Rotation(vector3(1.0f,0.0f,1.0f),(sinf(m_fRot))*0.06f);
	}
	m_iViewPlantsNum = 0;

	D3DXVECTOR4 vecViewPos;

//////////////////////
/*	*/
	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
				vecPlantPos.x=ix*LINTERVAL+matParent._41;
				vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
				vecPlantPos.z=iy*LINTERVAL+matParent._43;

				matPlantPos.Translation(vecPlantPos);

				m_AccumulateTM=matPlantPos;
				m_AccumulateTM._42-=10.0f;				

				matRot._41=m_AccumulateTM._41;
				matRot._42=m_AccumulateTM._42;
				matRot._43=m_AccumulateTM._43;

				m_vecMinBox=m_NormalTreeMesh[usTreeKind]->m_MinBox;
				m_vecMaxBox=m_NormalTreeMesh[usTreeKind]->m_MaxBox;
				//Init();
				
				if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
				{
					if(isAABBCulling())
					{	
						D3DXVECTOR3 vecPos = D3DXVECTOR3(vecPlantPos.x,vecPlantPos.y,vecPlantPos.z);
						D3DXVec3Transform(&vecViewPos,&vecPos,&matView);
						m_ViewPlants[m_iViewPlantsNum].fViewDist = vecViewPos.z;
						m_ViewPlants[m_iViewPlantsNum].matRot = matRot;
						m_ViewPlants[m_iViewPlantsNum].matTm = m_AccumulateTM;
						m_ViewPlants[m_iViewPlantsNum].ucKind = usTreeKind;
						m_ViewPlants[m_iViewPlantsNum++].iIndex = (ix+iy*SECTORSX);
						

					}
				}
				else
				{	
					
					if(m_usDetailTree[ix+iy*SECTORSX]&0x40)
					{
						if(isAABBCulling())
						{		
							
							D3DXVECTOR3 vecPos = D3DXVECTOR3(vecPlantPos.x,vecPlantPos.y,vecPlantPos.z);
							D3DXVec3Transform(&vecViewPos,&vecPos,&matView);
							m_ViewPlants[m_iViewPlantsNum].fViewDist = vecViewPos.z;
							m_ViewPlants[m_iViewPlantsNum].matRot = matRot;
							m_ViewPlants[m_iViewPlantsNum].matTm = m_AccumulateTM;
							m_ViewPlants[m_iViewPlantsNum].ucKind = usTreeKind;

							m_ViewPlants[m_iViewPlantsNum++].iIndex = (ix+iy*SECTORSX);

						}
					}						
				}				
			}
		}
	}	


	qsort((void *)m_ViewPlants,(size_t)m_iViewPlantsNum, sizeof(TreeNode_t),Tree_tCompare);


	for(int iSortCount = 0; iSortCount < m_iViewPlantsNum; iSortCount++) {
		int iPlantIndex = m_ViewPlants[iSortCount].iIndex; 
	
		if(m_usDetailTree[iPlantIndex]&0x80) {

			bool bLight = false;

			// character Light 영향 유무
			if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
			{
			

				D3DXVECTOR3 vecPos = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x,
													CSceneManager::m_CharacterLight.Position.y,
													CSceneManager::m_CharacterLight.Position.z);
				
				D3DXVECTOR3 vecTreeCenter = D3DXVECTOR3(m_AccumulateTM._41,
														m_AccumulateTM._42,
														m_AccumulateTM._43);

				if(D3DXVec3Length(&(vecPos - vecTreeCenter)) <= (CSceneManager::m_CharacterLight.Range + 100.0f))
				{
					pd3dDevice->SetLight(1,&(CSceneManager::m_CharacterLight));
					pd3dDevice->LightEnable(1,TRUE);
					bLight = true;
				}					
			}

			//pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
			if(CRenderOption::m_TreeAnimation)
			{
				pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matRot);
			}
			else
			{
				pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
			}						
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);
			m_NormalTreeMesh[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);						
		
			if(bLight == true)
			{
				pd3dDevice->LightEnable(1,FALSE);
			}

		}
		else {
			if(m_usDetailTree[iPlantIndex]&0x40) {
			
				if(CRenderOption::m_TreeAnimation)
				{								
					pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
					pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matRot);						
				}
				else
				{
					pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_ViewPlants[iSortCount].matTm);
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

				if(m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind])
					m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);

				

				if(CRenderOption::m_FullSceneGlare)
				{
					pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
					pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);	
					if(m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind])
						m_NormalTreeMeshLOD[m_ViewPlants[iSortCount].ucKind]->Render(pd3dDevice);
					pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
				}

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

			}
		}
	}

//////////////////////
	
/*
	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
				vecPlantPos.x=ix*LINTERVAL+matParent._41;
				vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
				vecPlantPos.z=iy*LINTERVAL+matParent._43;

				matPlantPos.Translation(vecPlantPos);

				m_AccumulateTM=matPlantPos;
				m_AccumulateTM._42-=10.0f;				

				matRot._41=m_AccumulateTM._41;
				matRot._42=m_AccumulateTM._42;
				matRot._43=m_AccumulateTM._43;

				m_vecMinBox=m_NormalTreeMesh[usTreeKind]->m_MinBox;
				m_vecMaxBox=m_NormalTreeMesh[usTreeKind]->m_MaxBox;
				//Init();
				
				if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
				{
					if(isAABBCulling())
					{	
						bool bLight = false;

						// character Light 영향 유무
						if(CSceneManager::m_bCharacterLight)		// 캐릭터 밤용 light 켰을때
						{
						

							D3DXVECTOR3 vecPos = D3DXVECTOR3(CSceneManager::m_CharacterLight.Position.x,
																CSceneManager::m_CharacterLight.Position.y,
																CSceneManager::m_CharacterLight.Position.z);
							
							D3DXVECTOR3 vecTreeCenter = D3DXVECTOR3(m_AccumulateTM._41,
																	m_AccumulateTM._42,
																	m_AccumulateTM._43);

							if(D3DXVec3Length(&(vecPos - vecTreeCenter)) <= (CSceneManager::m_CharacterLight.Range + 100.0f))
							{
								pd3dDevice->SetLight(1,&(CSceneManager::m_CharacterLight));
								pd3dDevice->LightEnable(1,TRUE);
								bLight = true;
							}					
						}

						//pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
						if(CRenderOption::m_TreeAnimation)
						{
							pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_AccumulateTM);
							pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRot);
						}
						else
						{
							pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
						}
							

						//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);	
						//if(usTreeKind==0)
						if(0)
						{	
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);	
							m_NormalTreeTrunk[usTreeKind]->Render(pd3dDevice);
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x0);							
							m_NormalTreeLeaves[usTreeKind]->Render(pd3dDevice);							
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
						}
						else
						{
							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);
							m_NormalTreeMesh[usTreeKind]->Render(pd3dDevice);						
						}
						if(bLight == true)
						{
							pd3dDevice->LightEnable(1,FALSE);
						}
					}
				}
				else
				{	
					
					if(m_usDetailTree[ix+iy*SECTORSX]&0x40)
					{
						if(isAABBCulling())
						{		
							
							if(CRenderOption::m_TreeAnimation)
							{								
								pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_AccumulateTM);
								pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRot);						
							}
							else
							{
								pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
							}

							pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

							if(m_NormalTreeMeshLOD[usTreeKind])
								m_NormalTreeMeshLOD[usTreeKind]->Render(pd3dDevice);

							

							if(CRenderOption::m_FullSceneGlare)
							{
								pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
								pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);	
								if(m_NormalTreeMeshLOD[usTreeKind])
									m_NormalTreeMeshLOD[usTreeKind]->Render(pd3dDevice);
								pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
							}

							pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
						}
					}						
				}				
			}
		}
	}	*/
//////////////////////

	//pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	/*
	if(GF3OPTION)
	{
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
	}
	*/

	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
	pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matParent);
	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
}

void CTreeScene::GeneratePlant(CSectorPlantMap *PlantMap,float *pHeightData)
{	
	int ix,iy;
	m_pHeight=pHeightData;
	m_cPlant=0;
	memset(m_usDetailTree,0xff,sizeof(unsigned char)*SECTORSX*SECTORSY);
	int cPlant=0;
	for(;cPlant<(int)PlantMap->m_PlantKind.size();cPlant++)
	{
		ix=PlantMap->m_PlantPosX[cPlant];
		iy=PlantMap->m_PlantPosZ[cPlant];
		if( ix < 0 || ix>= SECTORSX || iy < 0 || iy>=SECTORSY)
			continue;
		m_usDetailTree[ix+iy*SECTORSX]=PlantMap->m_PlantKind[cPlant];
		m_cPlant++;
	}	
	for(int i=0;i<(int)m_ShadowPolyList.size();i++)
	{		
		CSceneManager::m_HeightField.DeleteShadowPolyList(m_ShadowPolyList[i]);
	}	
	m_ShadowPolyList.clear();
	
	if(m_ViewPlants != NULL)
		delete[] m_ViewPlants;

	m_ViewPlants = new TreeNode_t[cPlant];
}

void CTreeScene::CalcObjectLens()
{
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	vector3 vecViewPos=matViewPosition->GetLoc();

	vecViewPos.x-=m_AccumulateTM._41;
	vecViewPos.z-=m_AccumulateTM._43;

	if(CRenderOption::m_TreeProjectShadow)
	{
		for(int i=0;i<(int)m_ShadowPolyList.size();i++)
		{	
			if(m_ShadowPolyList[i]!=-1)		
				CSceneManager::m_HeightField.DeleteShadowPolyList(m_ShadowPolyList[i]);
		}
		m_ShadowPolyList.clear();
	}
	

	float fLens=0.0f;
	WORD usTree;

	float fLens_FarTree=90000.0f;
	
	if( 0.0f-fLens_FarTree <= vecViewPos.x &&
		SECTORSIZE+fLens_FarTree >  vecViewPos.x &&
		0.0f-fLens_FarTree <= vecViewPos.z &&
		SECTORSIZE+fLens_FarTree >  vecViewPos.z)	
	{
		for(int ix=0;ix<SECTORSX;ix++)
		{
			for(int iy=0;iy<SECTORSY;iy++)
			{
				if(m_usDetailTree[ix+iy*SECTORSX] !=0xff)
				{
					//m_usDetailTree[ix+iy*SECTORSX]=
					//		m_usDetailTree[ix+iy*SECTORSX]|0x80;
					/*
					float fLens=CFastMath::FastSqrt(
					(((ix*LINTERVAL+m_AccumulateTM._41)-vecViewPos.x)*((ix*LINTERVAL+m_AccumulateTM._41)-vecViewPos.x))+
					(((iy*LINTERVAL+m_AccumulateTM._43)-vecViewPos.z)*((iy*LINTERVAL+m_AccumulateTM._43)-vecViewPos.z))
					);
					if(fLens<=LENS_REALPOLYTREE)
						m_usDetailTree[ix+iy*SECTORSX]=m_usDetailTree[ix+iy*SECTORSX]|0x80;
					else
						m_usDetailTree[ix+iy*SECTORSX]=m_usDetailTree[ix+iy*SECTORSX]&0x7f;
						*/

					usTree=m_usDetailTree[ix+iy*SECTORSX]&0x3f;

					fLens=CFastMath::FastSqrt((ix*LINTERVAL-vecViewPos.x)*(ix*LINTERVAL-vecViewPos.x)+
										(iy*LINTERVAL-vecViewPos.z)*(iy*LINTERVAL-vecViewPos.z));
					if( fLens <= LENS_REALPOLYTREE)
					{
						m_usDetailTree[ix+iy*SECTORSX]=usTree|0x80;

						vector3 vecTreePos=m_AccumulateTM.GetLoc();
						vecTreePos.x+=ix*LINTERVAL;
						vecTreePos.z+=iy*LINTERVAL;
						if(CRenderOption::m_TreeProjectShadow)
							m_ShadowPolyList.push_back(CSceneManager::m_HeightField.AddShadowPoly(vecTreePos,1));

					}
					else if( fLens < fLens_FarTree)
					{
						m_usDetailTree[ix+iy*SECTORSX]=usTree|0x40;
						if(CRenderOption::m_TreeProjectShadow)
							m_ShadowPolyList.push_back(-1);
					}					
					else
					{
						m_usDetailTree[ix+iy*SECTORSX]=usTree;
						if(CRenderOption::m_TreeProjectShadow)
							m_ShadowPolyList.push_back(-1);
					}					
				}
			}
		}
	}	
}

void CTreeScene::RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice, CRenderTexture &pShadowTexture)
{
	pd3dDevice->EndScene();		    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	
	
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

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
	
	

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
	vecSunLightDir.y-=0.3f;
	vecSunLightDir.Normalize();
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();

	vector3 vecViewPos=matViewPosition->GetLoc();
	vector3 vecViewPosInter;

	vecViewPosInter;
	//float fViewInter;

	

	matrix matRot;		
	unsigned char usTreeKind;
	if(CRenderOption::m_TreeAnimation)
	{
		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);
		matRot.Rotation(vector3(1.0f,0.0f,1.0f),(sinf(m_fRot))*0.06f);
	}

	vector3 vecPlantPos;

	matrix matParent,matPlantPos;
	pd3dDevice->GetTransform(D3DTS_WORLD,matParent);


	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

	m_AccumulateTM.Translation(vector3(0.0f,0.0f,0.0f));

	bool bTreeKindRender[MAX_TREEKIND];
	bool bTreeRender[SECTORSX*SECTORSY];

	memset(bTreeKindRender,false,sizeof(bool)*MAX_TREEKIND);
	
	int cShadowList=0;
	for(int ix=0;ix<SECTORSX;ix++)
	{
		for(int iy=0;iy<SECTORSY;iy++)
		{
			if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
			{
				usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
				vector3 vecPlantPos;

				vecPlantPos.x=ix*LINTERVAL+matParent._41;
				vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
				vecPlantPos.z=iy*LINTERVAL+matParent._43;

				matPlantPos.Translation(vecPlantPos);

				m_AccumulateTM=matPlantPos;
				m_AccumulateTM._42-=10.0f;

				matRot._41=m_AccumulateTM._41;
				matRot._42=m_AccumulateTM._42;
				matRot._43=m_AccumulateTM._43;
				
				m_vecMinBox=m_NormalTreeMesh[usTreeKind]->m_MinBox;
				m_vecMaxBox=m_NormalTreeMesh[usTreeKind]->m_MaxBox;

				m_vecMinBox.x*=2.0f;
				m_vecMinBox.z*=2.0f;

				m_vecMaxBox.x*=2.0f;
				m_vecMaxBox.z*=2.0f;				
				if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
				{
					
					if(isAABBCulling())
					{
						bTreeKindRender[usTreeKind]=true;
						bTreeRender[ix+iy*SECTORSX]=true;
					}
					else
					{
						bTreeRender[ix+iy*SECTORSX]=false;
					}
				}
			}
			else
			{
				bTreeRender[ix+iy*SECTORSX]=false;
			}
		}
	}	

	matrix matObjectCamera;			


	for(int cTree=0;cTree<MAX_TREEKIND;cTree++)
	{
		if(bTreeKindRender[cTree]==false)
			continue;

		m_vecMinBox=m_NormalTreeMesh[cTree]->m_MinBox;
		m_vecMaxBox=m_NormalTreeMesh[cTree]->m_MaxBox;
		Init();

		vector3 vecCenter=m_vecMaxBox+m_vecMinBox;
		vecCenter=vecCenter*0.5f;		

		D3DXMatrixOrthoLH(matShadowProj,m_fRad*1.0f,m_fRad*1.0f,10.0f, 100000.0f);	

		m_AccumulateTM.Translation(vector3(0.0f,0.0f,0.0f));
		matRot._41=matRot._42=matRot._43=0.0f;

		pShadowTexture.Begin(pd3dDevice);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->BeginScene();		

		matObjectCamera.CameraLookAt(vector3(0.0f,vecCenter.y,0.0f)-vecSunLightDir*1550.0f,
			vector3(0.0f,vecCenter.y,0.0f),vector3(0.0f,1.0f,0.0f));

		pd3dDevice->SetTransform(D3DTS_VIEW,matObjectCamera);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,matShadowProj);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

		pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xff000000);

		matrix matWorld;
		matWorld.MakeIdent();							

		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);								

		if(CRenderOption::m_TreeAnimation)
		{
			pd3dDevice->SetTransform(D3DTS_WORLD1,(D3DMATRIX*)&m_AccumulateTM);
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matRot);
		}
		else
		{
			pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);
		}	
		//

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xaa);			
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);		
		
		m_NormalTreeMesh[cTree]->Render(pd3dDevice);		
		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

		pd3dDevice->EndScene();		
		pShadowTexture.End(pd3dDevice);
		
		cShadowList=0;

		for(int ix=0;ix<SECTORSX;ix++)
		{
			for(int iy=0;iy<SECTORSY;iy++)
			{
				if(m_usDetailTree[ix+iy*SECTORSX]!=0xff)
				{
					usTreeKind=m_usDetailTree[ix+iy*SECTORSX] & 0x3f;
					vecPlantPos.x=ix*LINTERVAL+matParent._41;
					vecPlantPos.y=m_pHeight[ix+iy*SECTORSX];
					vecPlantPos.z=iy*LINTERVAL+matParent._43;

					matPlantPos.Translation(vecPlantPos);

					m_AccumulateTM=matPlantPos;
					m_AccumulateTM._42-=10.0f;				

					matRot._41=m_AccumulateTM._41;
					matRot._42=m_AccumulateTM._42;
					matRot._43=m_AccumulateTM._43;			

					
					
					if(m_usDetailTree[ix+iy*SECTORSX]&0x80)
					{
						if(usTreeKind==cTree && bTreeRender[ix+iy*SECTORSX]==true)						
						{
							//List<vector3> ShadowedPoly;							
							//CSceneManager::m_HeightField.GetHeightFieldRangePoly(vecPlantPos,ShadowedPoly,3000.0f);
							CHeightFieldScene::CShadowedPoly *pNode;

							pNode=CSceneManager::m_HeightField.GetShadowPolyList(m_ShadowPolyList[cShadowList]);
							if(pNode==NULL)
								continue;

							if(pNode->m_ShadowedPolyList.empty())
								continue;

							TTVertex *pShadowedVertex=new TTVertex[pNode->m_ShadowedPolyList.size()];
							WORD *pShadowedIndices=new WORD[pNode->m_ShadowedPolyList.size()];
							for(int cIndices=0;cIndices<(int)pNode->m_ShadowedPolyList.size();cIndices++)
							{
								pShadowedVertex[cIndices].v=pNode->m_ShadowedPolyList[cIndices];
								pShadowedVertex[cIndices].diff.c=0xff999999;
								pShadowedIndices[cIndices]=cIndices;			
							}					 

							pd3dDevice->BeginScene();	
			
							pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		

							//matObjectCamera.CameraLookAt(vecPlantPos+vecSunLightDir*1550.0f,
							//	vecPlantPos,vector3(0.0f,1.0f,0.0f));

							matObjectCamera.CameraLookAt(vecPlantPos+vector3(0.0f,vecCenter.y,0.0f)-vecSunLightDir*1550.0f,
								vecPlantPos+vector3(0.0f,vecCenter.y,0.0f),vector3(0.0f,1.0f,0.0f));
							

							matrix mat,mat2;
							mat=matShadowProj*matTexScale;		
							mat2=matObjectCamera*mat;
							matTex=matInv*mat2;

							pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
							pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);

							pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
							pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

							pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
							pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

							pd3dDevice->SetTexture(0,pShadowTexture.GetTexture());
							pd3dDevice->SetVertexShader(TTVERTEXFVF);	

							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);		

							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_ADD);
							pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);


							pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
							pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
							pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);		
							//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);		

							pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
							matTex.MakeIdent();
							pd3dDevice->SetTransform(D3DTS_WORLD,matTex);
							pd3dDevice->SetTransform(D3DTS_WORLD1,matTex);
							pd3dDevice->SetRenderState(D3DRS_ZBIAS,14);

							pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);

							pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)pNode->m_ShadowedPolyList.size(),(int)pNode->m_ShadowedPolyList.size()/3,pShadowedIndices,
								D3DFMT_INDEX16,pShadowedVertex,sizeof(TTVertex));		

							
							pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);

							pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);

							pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
							delete [] pShadowedVertex;
							delete [] pShadowedIndices;
							pd3dDevice->EndScene();														
						}
					}
					cShadowList++;

				}
			}
		}		
	}

	pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
	pd3dDevice->SetTransform(D3DTS_VIEW,matView);

	//////////////////
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,TRUE);	
	matrix matWorld;
	matWorld.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD1,matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
	pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);

	pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->BeginScene();

}

void CTreeScene::Update(float fUpdate)
{
	m_fRot+=fUpdate;
}

int Tree_tCompare(const void *arg1,const void *arg2) {
	
	if(((TreeNode_t *)arg1)->fViewDist < ((TreeNode_t *)arg2)->fViewDist)
		return 1;
	else if(((TreeNode_t *)arg1)->fViewDist > ((TreeNode_t *)arg2)->fViewDist)
		return -1;
	return 0;

}