// GrassScene.cpp: implementation of the CGrassScene class.
//
//////////////////////////////////////////////////////////////////////
#include "SceneManager.h"
#include "GrassScene.h"
#include "RenderOption.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshObject *CGrassScene::m_NormalGrass[5];

CTexture *CGrassScene::m_NormalTexture=NULL;

DWORD CGrassScene::m_dwGrassVertexShader=0xffffffff;

//CTexture *CGrassScene::m_GrassRangeTexture=NULL;


//c8 LowerColor,c9 UpperColor

char strGrassVertexShader[]=
"vs.1.1\n"
"mov a0.x, v3.x\n"

"dp4 r0.x, v0, c[a0.x + 15 + 0]\n"
"dp4 r0.y, v0, c[a0.x + 15 + 1]\n"
"dp4 r0.z, v0, c[a0.x + 15 + 2]\n"
"mov r0.w,c[4].z\n"

"mov a0.x, v3.y\n"

"dp4 r1.x, v0, c[a0.x + 15 + 0]\n"
"dp4 r1.y, v0, c[a0.x + 15 + 1]\n"
"dp4 r1.z, v0, c[a0.x + 15 + 2]\n"
"mov r1.w,c[4].z\n"

"add r2,r0,-r1\n"
"max r2, r2,-r2\n"
"mul r2.z,r2.z,c5\n"
"mov r2.xy,r2.z\n"
"add r2,r2,c6\n"


"mul r3, c8, r2\n"
"add r4, c4,-r2\n"
"mad r2,r4,c9,r3\n"
//"mov r2.yz,r2.x\n"
"mov r8,v8.x\n"
"mul r2,r2,r8\n"
"mov oD0,r2\n"


"mul r0,r0,v1.x\n"
"add r2,c4.z,-v1.x\n"
"mad r2,r1,r2,r0\n"




/*
; Lerp the two positions r0 and r1 into r2
mul r0, r0, v1.x     ; v0 * weight
add r2, c1.x, -v1.x  ; r2 = 1 - weight
mad r2, r1, r2, r0   ; pos = (1-weight)*v1 + v0*weight
*/

//"mul r1.xyz, r1.xyz, v1.x\n"
//"mad r2, r2.xyz, v2.x, r1.xyz\n"
"mov r2.w, c[4].z\n"

"add r2.y,r2.y,v2.x\n"

"dp4 oPos.x, r2, c[0]\n"
"dp4 oPos.y, r2, c[1]\n"
"dp4 oPos.z, r2, c[2]\n"
"dp4 oPos.w, r2, c[3]\n"

"mov oT0.xy, v7\n";


/*
char strGrassVertexShader[]=
"vs.1.1\n"
"m4x4 r0,v0,c4\n"
"m4x4 r1,v0,c8\n"
//Vertex Color
"add r2,r0,-r1\n"
"max r2, r2,-r2\n"
"mul r2.z,r2.z,c3\n"
"add r2,r2,c0\n"
"mov oD0,r2.z\n"
//Vertex Pos
"mul r0, r0, v1.x\n"
"add r2, c1.x, -v1.x\n"
"mad r2, r1, r2, r0\n"
"m4x4 oPos,r2,c12\n"
//Texture 
"mov oT0,v7\n";
//"mul r0,v5,c2\n";
//"mov oD0,c2\n";
*/

CGrassScene::CGrassScene()
{
	m_cGrassElement=0;	
	
	m_vecGrassPos= NULL;
	m_GrassKind= NULL;	
	m_GrassShadow= NULL;
	
	m_pGrassVertexBuffer = NULL;
	m_pGrassIndicesBuffer = NULL;

}

CGrassScene::~CGrassScene()
{
	if(m_vecGrassPos != NULL) { 
		delete[] m_vecGrassPos;
		m_vecGrassPos= NULL;
	}
	if(m_GrassKind != NULL) {
		delete[] m_GrassKind;
		m_GrassKind= NULL;	
	}
	if(m_GrassShadow != NULL) {
		delete[] m_GrassShadow;
		m_GrassShadow= NULL;
	}
	if(m_pGrassVertexBuffer != NULL) {
		m_pGrassVertexBuffer->Release();
		m_pGrassVertexBuffer = NULL;
	}
	if(m_pGrassIndicesBuffer != NULL) {
		m_pGrassIndicesBuffer->Release();
		m_pGrassIndicesBuffer = NULL;
	}

}
void CGrassScene::UnloadData() {

	m_cGrassElement = 0;

}

//DEL void CGrassScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
//DEL {	
//DEL 	if(m_cGrass==0)
//DEL 		return;
//DEL 	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();
//DEL 	vector3 vecViewPos;
//DEL 	vector3 vecViewAbsPos;
//DEL 	vecViewAbsPos=vecViewPos=matViewPosition->GetLoc();	
//DEL 
//DEL 	vector3 vecViewPlant;	
//DEL 	matrix matParent;
//DEL 	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)&matParent);
//DEL 
//DEL 	vecViewPos.x-=matParent._41;
//DEL 	vecViewPos.y-=matParent._42;
//DEL 	vecViewPos.z-=matParent._43;	
//DEL 
//DEL 	vector3 vecSectorStart;
//DEL 	vecSectorStart.x=matParent._41;
//DEL 	vecSectorStart.y=matParent._43;
//DEL 	
//DEL 	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
//DEL 	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
//DEL 	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
//DEL 
//DEL 	pd3dDevice->LightEnable(0,TRUE);		
//DEL 	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);	
//DEL 
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);
//DEL 	/*
//DEL 
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
//DEL 	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);	
//DEL 	*/
//DEL 
//DEL 	//matrix matGrassPos;
//DEL 	//matPlantPos.Translation(vector3(0.0f,0.0f,0.0f));
//DEL 	//pd3dDevice->SetTransform(D3DTS_WORLD,matPlantPos);		
//DEL 	//m_NormalGrassMesh[0]->Render(pd3dDevice);
//DEL 
//DEL 	matrix matGrassPos,matGrassScale;
//DEL 	vector3 vecViewPosInter;
//DEL 	float fPosInter;
//DEL 
//DEL 	/*
//DEL 		vecViewPosInter=vecViewPos-m_ObjectSceneNode[cObject]->m_AccumulateTM.GetLoc();
//DEL 		fViewInter=vecViewPosInter.GetLens();
//DEL 
//DEL 		if(MAX_OBJECTVIEWRANBE < fViewInter)
//DEL 			continue;
//DEL 
//DEL 		if(MAX_OBJECTDISAPPER < fViewInter)
//DEL 		{
//DEL 			float fBlend=(fViewInter-MAX_OBJECTDISAPPER)/(MAX_OBJECTVIEWRANBE-MAX_OBJECTDISAPPER);			
//DEL 			color TextureFactorColor;
//DEL 			TextureFactorColor=color::Interpolation(color(0,0,0,255),color(0,0,0,0),fBlend);
//DEL 			//TextureFactorColor.a=100;
//DEL 			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,TextureFactorColor.c);
//DEL 			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
//DEL 	*/
//DEL 
//DEL 	int ix,iy;
//DEL 	if( 0.0f-LENS_REALPOLYTREE <= vecViewPos.x &&
//DEL 		SECTORSIZE+LENS_REALPOLYTREE >  vecViewPos.x &&
//DEL 		0.0f-LENS_REALPOLYTREE <= vecViewPos.z &&
//DEL 		SECTORSIZE+LENS_REALPOLYTREE >  vecViewPos.z)
//DEL 	{
//DEL 		for(iy=0;iy<SECTORSY;iy++)
//DEL 		{
//DEL 			for(ix=0;ix<SECTORSX;ix++)
//DEL 			{
//DEL 				if(m_GrassKind[ix+iy*SECTORSX]!=-1)
//DEL 				{
//DEL 					if( CFastMath::FastSqrt( (ix*LINTERVAL-vecViewPos.x)*(ix*LINTERVAL-vecViewPos.x) +
//DEL 						(iy*LINTERVAL-vecViewPos.z)*(iy*LINTERVAL-vecViewPos.z)) 
//DEL 						<= 2000.0f)//LENS_REALPOLYTREE)
//DEL 					{	
//DEL 						matGrassPos.Translation(vector3(ix*LINTERVAL,0.0f,iy*LINTERVAL));						
//DEL 
//DEL 						matGrassPos._41+=matParent._41;
//DEL 						matGrassPos._42+=matParent._42;
//DEL 						matGrassPos._43+=matParent._43;
//DEL 						matGrassPos._42=m_pSectorVertex[ix+iy*SECTORSX].v.y;
//DEL 						/*
//DEL 						vector3 vecNormal=m_pSectorVertex[ix+iy*SECTORSX].n;
//DEL 						vecNormal.y=vecNormal.y*3.0f;
//DEL 						vecNormal.Normalize();
//DEL 						vector3 vecRotateAxis=vecNormal^vector3(0.0f,1.0f,0.0f);
//DEL 						float fAngle=vecNormal*vector3(0.0f,1.0f,0.0f);
//DEL 						fAngle=acosf(fAngle);
//DEL 						matrix matRot;
//DEL 						matRot.AxisAngle(vecRotateAxis,-fAngle);
//DEL 						matGrassScale.MakeIdent();
//DEL 						//matGrassPos._11=m_vecGrassScale[ix+iy*SECTORSX].x;
//DEL 						//matGrassPos._22=m_vecGrassScale[ix+iy*SECTORSX].y;
//DEL 						//matGrassPos._33=m_vecGrassScale[ix+iy*SECTORSX].z;
//DEL 						*/
//DEL 						
//DEL 						m_AccumulateTM=matGrassPos;
//DEL 						m_vecMinBox=m_NormalGrassMesh[0]->m_MinBox;
//DEL 						m_vecMaxBox=m_NormalGrassMesh[0]->m_MaxBox;						
//DEL 
//DEL 						Init();
//DEL 						if(isCulling())
//DEL 						{
//DEL 							//matGrassPos=matRot*matGrassPos;
//DEL 							//matGrassPos=matGrassScale*matGrassPos;
//DEL 
//DEL 							vecViewPosInter=vecViewAbsPos-m_matGrass[ix+iy*SECTORSX].GetLoc();
//DEL 							vecViewPosInter.y=0.0f;
//DEL 							fPosInter=vecViewPosInter.GetLens();
//DEL 
//DEL 							if(fPosInter<1500.0f)
//DEL 							{
//DEL 								pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xff000000);
//DEL 								pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
//DEL 								pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
//DEL 								
//DEL 								/*
//DEL 								float fBlend=(fViewInter-MAX_OBJECTDISAPPER)/(MAX_OBJECTVIEWRANBE-MAX_OBJECTDISAPPER);			
//DEL 								color TextureFactorColor;
//DEL 								TextureFactorColor=color::Interpolation(color(0,0,0,255),color(0,0,0,0),fBlend);
//DEL 								pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,TextureFactorColor.c);
//DEL 								pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
//DEL 								*/
//DEL 							}
//DEL 							else
//DEL 							{
//DEL 								float fBlend=(2000.0f-fPosInter)/(500.0f);
//DEL 								fBlend=1.0f-fBlend;
//DEL 								color TextureFactorColor;
//DEL 								TextureFactorColor=color::Interpolation(color(0,0,0,255),color(0,0,0,0),fBlend);
//DEL 								pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,TextureFactorColor.c);
//DEL 								pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//DEL 								//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0x44000000);
//DEL 								pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
//DEL 							}							
//DEL 
//DEL 							pd3dDevice->SetTransform(D3DTS_WORLD,
//DEL 								m_matGrass[ix+iy*SECTORSX]);
//DEL 							m_NormalGrassMesh[0]->Render(pd3dDevice);
//DEL 						}
//DEL 						/*
//DEL 						matGrassPos.Translation(vector3(ix*LINTERVAL,0.0f,iy*LINTERVAL));						
//DEL 						matGrassPos._41+=matParent._41;
//DEL 						matGrassPos._42+=matParent._42;
//DEL 						matGrassPos._43+=matParent._43;
//DEL 						matGrassPos._42=CSceneManager::m_HeightField.GetHeight(matGrassPos.GetLoc());
//DEL 
//DEL 						vector3 v1=vector3(matParent._41+ix*LINTERVAL,0.0f,matParent._43+iy*LINTERVAL);
//DEL 						vector3 v2=vector3(matParent._41+ix*LINTERVAL+LINTERVAL,0.0f,matParent._43+iy*LINTERVAL);
//DEL 						vector3 v3=vector3(matParent._41+ix*LINTERVAL,0.0f,matParent._43+iy*LINTERVAL+LINTERVAL);
//DEL 
//DEL 						v1.y=CSceneManager::m_HeightField.GetHeight(v1);
//DEL 						v2.y=CSceneManager::m_HeightField.GetHeight(v2);
//DEL 						v3.y=CSceneManager::m_HeightField.GetHeight(v3);
//DEL 
//DEL 						vector3 vecNormal=(v3-v1)^(v2-v1);
//DEL 						vecNormal.Normalize();
//DEL 						vector3 vecRotateAxis=vecNormal^vector3(0.0f,1.0f,0.0f);
//DEL 						float fAngle=vecNormal*vector3(0.0f,1.0f,0.0f);
//DEL 						fAngle=acosf(fAngle);
//DEL 						matrix matRot;
//DEL 						matRot.AxisAngle(vecRotateAxis,-fAngle);
//DEL 						
//DEL 						m_AccumulateTM=matGrassPos;
//DEL 						m_vecMinBox=m_NormalGrassMesh[0]->m_MinBox;
//DEL 						m_vecMaxBox=m_NormalGrassMesh[0]->m_MaxBox;
//DEL 						Init();
//DEL 						if(isCulling())
//DEL 						{
//DEL 							matGrassPos=matRot*matGrassPos;
//DEL 							pd3dDevice->SetTransform(D3DTS_WORLD,matGrassPos);
//DEL 							m_NormalGrassMesh[0]->Render(pd3dDevice);
//DEL 						}
//DEL 						*/
//DEL 					}
//DEL 				}
//DEL 			}
//DEL 		}
//DEL 	}
//DEL 	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
//DEL }

//DEL void CGrassScene::Create()
//DEL {
//DEL 	for(int cGrass=0;cGrass<60;cGrass++)
//DEL 	{
//DEL 		m_vecGrassPos[cGrass]=vector3(rand()%1000,0.0f,rand()%1000);
//DEL 	}
//DEL 	
//DEL 	if(m_NormalGrassMesh[0]==NULL)
//DEL 	{
//DEL 		//LogMessage("Glass Object load normaltree");
//DEL 		CTexture::SetPath(OBJECTTEXTUREPATH);
//DEL 		CSceneManager::m_MeshObjectContainer.SetPath(TREEOBJECTPATH);			
//DEL 		m_NormalGrassMesh[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("grass_fr_4060.R3S.R3S");
//DEL 		m_NormalGrassMesh[0]->ConvertNormal();
//DEL 	}
//DEL }

//DEL void CGrassScene::GeneratePlant(int SeedX, int SeedY, float *pHeightData, float fAvgHeight,matrix matParent)
//DEL {		
//DEL 	srand((SeedX+SeedY)/50);		
//DEL 	//int cManySeed=rand()%MAX_SEED;
//DEL 	int cManySeed=19;
//DEL 
//DEL 	int PutSeedX[MAX_SEED],PutSeedY[MAX_SEED];
//DEL 	int PutRange[MAX_SEED];
//DEL 	int PutSeedPoss[MAX_SEED];	
//DEL 	
//DEL 	memset(m_GrassKind,-1,sizeof(char)*SECTORSX*SECTORSY);	
//DEL 	/*
//DEL 	memset(m_GrassKind,0,sizeof(char)*SECTORSX*SECTORSY);	
//DEL 	int ix,iy;
//DEL 	for(ix=0;ix<SECTORSX;ix++)
//DEL 	{
//DEL 		for(iy=0;iy<SECTORSY;iy++)
//DEL 		{			
//DEL 			m_GrassPos[ix+iy*SECTORSX]=pHeightData[ix+iy*SECTORSX];
//DEL 		}
//DEL 	}
//DEL 	//*/
//DEL 	//*
//DEL 	for(int i=0;i<cManySeed;i++)
//DEL 	{
//DEL 		PutRange[i]=rand()%MAX_SEEDRANGE;
//DEL 		PutSeedX[i]=rand()%(SECTORSX-(PutRange[i]*2));
//DEL 		PutSeedY[i]=rand()%(SECTORSY-(PutRange[i]*2));
//DEL 		PutSeedX[i]+=PutRange[i];
//DEL 		PutSeedY[i]+=PutRange[i];
//DEL 		PutSeedPoss[i]=(rand()%100)-100;
//DEL 	}	
//DEL 
//DEL 	int ix,iy,iSeed;
//DEL 	int InterX,InterY;
//DEL 	
//DEL 
//DEL 	int cGrass=0;
//DEL 	int GrassKind;	
//DEL 
//DEL 	matrix matRot;
//DEL 	float fAngle;
//DEL 
//DEL 	for(ix=0;ix<SECTORSX;ix++)
//DEL 	{
//DEL 		for(iy=0;iy<SECTORSY;iy++)
//DEL 		{
//DEL 			for(iSeed=0;iSeed<cManySeed;iSeed++)
//DEL 			{
//DEL 				InterX=PutSeedX[iSeed]-ix;
//DEL 				InterY=PutSeedY[iSeed]-iy;
//DEL 				if(CFastMath::FastSqrt((InterX*InterX)+(InterY*InterY)) <= PutRange[iSeed])
//DEL 				{
//DEL 					if(m_GrassKind[ix+iy*SECTORSX]==-1)
//DEL 					{
//DEL 						if(cGrass>=MAX_TREE)					
//DEL 							continue;						
//DEL 						if(PutSeedPoss[iSeed] > (rand()%100))
//DEL 							continue;																		
//DEL 						//GrassKind=rand()%MAX_GRASSKIND;
//DEL 						GrassKind=0;
//DEL 						m_GrassKind[ix+iy*SECTORSX]=GrassKind;
//DEL 						m_vecGrassScale[ix+iy*SECTORSX].x=1.0f+(rand()%100)*0.01f;
//DEL 						m_vecGrassScale[ix+iy*SECTORSX].y=1.0f+(rand()%100)*0.01f;
//DEL 						m_vecGrassScale[ix+iy*SECTORSX].z=1.0f+(rand()%100)*0.01f;
//DEL 
//DEL 						m_matGrass[ix+iy*SECTORSX].Translation(vector3(ix*LINTERVAL,0.0f,iy*LINTERVAL));
//DEL 						m_matGrass[ix+iy*SECTORSX]._41+=matParent._41;
//DEL 						m_matGrass[ix+iy*SECTORSX]._42+=matParent._42;
//DEL 						m_matGrass[ix+iy*SECTORSX]._43+=matParent._43;
//DEL 						m_matGrass[ix+iy*SECTORSX]._42=m_pSectorVertex[ix+iy*SECTORSX].v.y;
//DEL 						vector3 vecNormal=m_pSectorVertex[ix+iy*SECTORSX].n;
//DEL 						vecNormal.y=vecNormal.y*3.0f;
//DEL 						vecNormal.Normalize();
//DEL 						vector3 vecRotateAxis=vecNormal^vector3(0.0f,1.0f,0.0f);
//DEL 						fAngle=vecNormal*vector3(0.0f,1.0f,0.0f);
//DEL 						fAngle=acosf(fAngle);						
//DEL 						matRot.AxisAngle(vecRotateAxis,-fAngle);					
//DEL 						m_matGrass[ix+iy*SECTORSX]=matRot*m_matGrass[ix+iy*SECTORSX];
//DEL 						/*						
//DEL 						/*
//DEL 						vector3 vecNormal=m_pSectorVertex[ix+iy*SECTORSX].n;
//DEL 						vecNormal.y=vecNormal.y*3.0f;
//DEL 						vecNormal.Normalize();
//DEL 						vector3 vecRotateAxis=vecNormal^vector3(0.0f,1.0f,0.0f);
//DEL 						float fAngle=vecNormal*vector3(0.0f,1.0f,0.0f);
//DEL 						fAngle=acosf(fAngle);
//DEL 						matrix matRot;
//DEL 						matRot.AxisAngle(vecRotateAxis,-fAngle);
//DEL 						matGrassScale.MakeIdent();
//DEL 						//matGrassPos._11=m_vecGrassScale[ix+iy*SECTORSX].x;
//DEL 						//matGrassPos._22=m_vecGrassScale[ix+iy*SECTORSX].y;
//DEL 						//matGrassPos._33=m_vecGrassScale[ix+iy*SECTORSX].z;
//DEL 						*/
//DEL 						
//DEL 						cGrass++;						
//DEL 					}
//DEL 					break;
//DEL 				}
//DEL 			}
//DEL 		}
//DEL 	}		
//DEL 	m_cGrass=cGrass;	
//DEL 	//*/
//DEL }

void CGrassScene::UpdateRange()
{
	vector3 vecChrPos=CSceneManager::m_ViewCamera->m_matPosition.GetLoc();
	int cPos=0;
	float fix,fiy;			

	fix=-LINTERVAL*MAX_GRASSRANGE/2.0f;
	for(int ix=0;ix<MAX_GRASSRANGE;ix++,fix+=LINTERVAL)
	{
		fiy=-LINTERVAL*MAX_GRASSRANGE/2.0f;
		for(int iy=0;iy<MAX_GRASSRANGE;iy++,fiy+=LINTERVAL)
		{
			std::vector<vector3> TerrainPoly;
			CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecChrPos+vector3(fix,0.0f,fiy),TerrainPoly);
			vector3 vecPoly[3];
			float fLens;

			for(int cGlass=0;cGlass<MAX_GRASS;cGlass++)
			{
				for(int cIndices=0;cIndices<(int)TerrainPoly.size()/3;cIndices++)
				{
					vecPoly[0]=TerrainPoly[cIndices*3+0];
					vecPoly[1]=TerrainPoly[cIndices*3+1];
					vecPoly[2]=TerrainPoly[cIndices*3+2];
					
					if(CIntersection::PolygonRay(vector3(m_vecGrassPos[cPos].x+vecChrPos.x,0.0f,m_vecGrassPos[cPos].z+vecChrPos.z),
												 vector3(m_vecGrassPos[cPos].x+vecChrPos.x,100000.0f,m_vecGrassPos[cPos].z+vecChrPos.z),vecPoly,fLens))
					{
						m_vecGrassPos[cPos].y=fLens;
						/*
						m_vecGrassPos[cPos].x=m_vecGrassPos[cPos].x+vecChrPos.x;
						m_vecGrassPos[cPos].y=fLens;
						m_vecGrassPos[cPos].z=m_vecGrassPos[cPos].z+vecChrPos.z;
						*/
						
						//m_vecGrassPos[cPos].x=m_vecGrassPos[cPos].x+vecChrPos.x;
						//m_vecGrassPos[cPos].z=m_vecGrassPos[cPos].z+vecChrPos.z;
						break;
					}
				}
				cPos++;
			}
		}
	}
	/*
	for(int ix=0;ix<5;ix++)
	{
		for(int iy=0;iy<5;iy++)
		{
			m_vecGrassPos[cPos];
			CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecChrPos,TerrainPoly+);

			cPos++;
			/*
			//List<vector3> TerrainPoly;	
			CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecChrPos,TerrainPoly);
			int cIndices;
			vector3 vecPoly[3];
			float fLens;
			for(i=0;i<30;i++)
			{	
				for(cIndices=0;cIndices<TerrainPoly.num/3;cIndices++)
				{
					vecPoly[0]=TerrainPoly[cIndices*3+0];
					vecPoly[1]=TerrainPoly[cIndices*3+1];
					vecPoly[2]=TerrainPoly[cIndices*3+2];
					
					if(CIntersection::PolygonRay(vector3(m_vecGrassPos[i].x,0.0f,m_vecGrassPos[i].z),
												 vector3(m_vecGrassPos[i].x,100000.0f,m_vecGrassPos[i].z),vecPoly,fLens))
					{
						m_vecGrassPos[i].y=fLens;
						break;
					}
				}
			}			

		}
	}
	*/
	/*

	for(int i=0;i<25;i++)
	{
		//List<vector3> TerrainPoly;		
		float fInter=500.0f;
		//CSceneManager::m_HeightField.GetHeightFieldRangePoly(vecChrPos,TerrainPoly,fInter);
		CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecChrPos,TerrainPoly);

		vector3 m_vecGrassPos[30];
		for(int i=0;i<30;i++)
		{
			m_vecGrassPos[i]=vector3(rand()%1000,0.0f,rand()%1000);
			m_vecGrassPos[i]-=vector3(500,0.0f,500);
			m_vecGrassPos[i]+=vector3(vecChrPos.x,0.0f,vecChrPos.z);
		}	
		int cIndices;
		vector3 vecPoly[3];
		float fLens;
		for(i=0;i<30;i++)
		{	
			for(cIndices=0;cIndices<TerrainPoly.num/3;cIndices++)
			{
				vecPoly[0]=TerrainPoly[cIndices*3+0];
				vecPoly[1]=TerrainPoly[cIndices*3+1];
				vecPoly[2]=TerrainPoly[cIndices*3+2];
				
				if(CIntersection::PolygonRay(vector3(m_vecGrassPos[i].x,0.0f,m_vecGrassPos[i].z),
											 vector3(m_vecGrassPos[i].x,100000.0f,m_vecGrassPos[i].z),vecPoly,fLens))
				{
					m_vecGrassPos[i].y=fLens;
					break;
				}
			}
		}
	}
	/*
	//List<vector3> CharPoly;	
	
	float fInter=5000.0f;
	m_HeightField.GetHeightFieldRangePoly(vecChrPos,CharPoly,fInter);		

	TTVertex *pCharVertex=new TTVertex[CharPoly.num];
	WORD *pCharIndices=new WORD[CharPoly.num];
	for(int cIndices=0;cIndices<CharPoly.num;cIndices++)
	{
		pCharVertex[cIndices].v=CharPoly[cIndices];
		pCharVertex[cIndices].diff.c=0xffff0000;		
		pCharVertex[cIndices].v.y+=2.0f;

		pCharIndices[cIndices]=cIndices;
	}
	vector3 vecPoly[3];
	float fLens;
	for(i=0;i<500;i++)
	{	
		for(cIndices=0;cIndices<CharPoly.num/3;cIndices++)
		{
			vecPoly[0]=pCharVertex[cIndices*3+0].v;
			vecPoly[1]=pCharVertex[cIndices*3+1].v;
			vecPoly[2]=pCharVertex[cIndices*3+2].v;
			
			if(CIntersection::PolygonRay(vector3(m_vecGrassPos[i].x,0.0f,m_vecGrassPos[i].z),
										 vector3(m_vecGrassPos[i].x,100000.0f,m_vecGrassPos[i].z),vecPoly,fLens))
			{
				m_vecGrassPos[i].y=fLens;
				break;
			}
		}
	}		
	m_pd3dDevice->SetVertexShader(TTVERTEXFVF);	
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	

	m_pd3dDevice->SetTexture(0,NULL);
	m_pd3dDevice->SetTexture(1,NULL);

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO);
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	matrix matIndent;
	matIndent.MakeIdent();
	m_pd3dDevice->SetTransform(D3DTS_WORLD,matIndent);
	
	m_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,CharPoly.num,CharPoly.num/3,pCharIndices,
			D3DFMT_INDEX16,pCharVertex,sizeof(TTVertex));
		
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	delete [] pCharVertex;
	delete [] pCharIndices;	
	*/
	
}

void CGrassScene::Create()
{	
	
	m_vecGrassPos=new vector3[MAX_GRASS];
	m_GrassKind=new unsigned char[MAX_GRASS];	
	m_GrassShadow=new unsigned char[MAX_GRASS];	
	
	CSceneManager::GetDevice()->CreateIndexBuffer(MAX_GRASS*12*sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pGrassIndicesBuffer);
	
	
	WORD *pIndices;
	m_pGrassIndicesBuffer->Lock(0,MAX_GRASS*12*sizeof(WORD),(BYTE**)&pIndices,0);
	
	for(int i=0;i<MAX_GRASS;i++)
	{
		pIndices[i*12+0]=i*8+0;
		pIndices[i*12+1]=i*8+1;
		pIndices[i*12+2]=i*8+2;

		pIndices[i*12+3]=i*8+1; 
		pIndices[i*12+4]=i*8+2;
		pIndices[i*12+5]=i*8+3;

		pIndices[i*12+6]=i*8+4;
		pIndices[i*12+7]=i*8+5;
		pIndices[i*12+8]=i*8+6;

		pIndices[i*12+9]=i*8+5;
		pIndices[i*12+10]=i*8+6;
		pIndices[i*12+11]=i*8+7;
	}
	m_pGrassIndicesBuffer->Unlock();

	if(CRenderOption::m_GrassAnimationUseVertexShader)
	{
		CSceneManager::GetDevice()->CreateVertexBuffer(MAX_GRASS*8*sizeof(GrassExtVertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_pGrassVertexBuffer);
		if(m_dwGrassVertexShader==0xffffffff)
		{
			LPD3DXBUFFER pCode;	
			D3DXAssembleShader(strGrassVertexShader,strlen(strGrassVertexShader),0,NULL,&pCode,NULL);				
		
			DWORD dwDecl[] =
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG(0,D3DVSDT_FLOAT3), // position
				D3DVSD_REG(1,D3DVSDT_FLOAT1), // Weight0
				D3DVSD_REG(2,D3DVSDT_FLOAT1), // Weight1
				D3DVSD_REG(3,D3DVSDT_FLOAT2), // index0,index1
				D3DVSD_REG(7,D3DVSDT_FLOAT2), // tex			
				D3DVSD_REG(8,D3DVSDT_FLOAT1), // Color rate
				D3DVSD_END()
			};
			

			CSceneManager::GetDevice()->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwGrassVertexShader, 0 );
			pCode->Release();		
		}
	}
	else
	{
		CSceneManager::GetDevice()->CreateVertexBuffer(MAX_GRASS*8*sizeof(GrassVertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_pGrassVertexBuffer);
	}
	/*
	
										 */

	/*
	m_GrassPosVertex=new TTVertex[MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE*2];
	m_GrassPosIndices=new WORD[MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE*2];
	m_vecGrassPos=new vector3[MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE];
	m_vecRealGrassPos=new vector3[MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE];
	int cPos=0;
	float fix,fiy;

	fix=-LINTERVAL*MAX_GRASSRANGE/2.0f;
	for(int ix=0;ix<MAX_GRASSRANGE;ix++,fix+=LINTERVAL)
	{
		fiy=-LINTERVAL*MAX_GRASSRANGE/2.0f;
		for(int iy=0;iy<MAX_GRASSRANGE;iy++,fiy+=LINTERVAL)
		{
			for(int cGlass=0;cGlass<MAX_GRASS;cGlass++)
			{
				m_vecGrassPos[cPos]=vector3(fix+rand()%(int)LINTERVAL,0.0f,fiy+rand()%(int)LINTERVAL);
				cPos++;
			}
		}
	}	
	for(int i=0;i<MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE;i++)
	{
		m_GrassPosIndices[i*2+0]=i*2+0;
		m_GrassPosIndices[i*2+1]=i*2+1;
	}

	CSceneManager::m_MeshObjectContainer.SetPath(TREEOBJECTPATH);			
	m_NormalGrass=CSceneManager::m_MeshObjectContainer.GetMeshObject("grass_fr_4060.R3S");
	*/
}

void CGrassScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	if(m_cGrassElement==0)
		return;
	CViewCamera *pCamera=CSceneManager::GetCamera();			
	vector3 *ViewPos;
	ViewPos=CSceneManager::GetCamera()->GetPosition();
	
	int PosX=(int)(ViewPos->x/LINTERVAL);
	int PosY=(int)(ViewPos->z/LINTERVAL);
	
	//fRot=4.14f;	
	

	matrix matRot;

	float fBias=0.0f;
	//pd3dDevice->SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	//pd3dDevice->SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,0);

	
	
	if( CIntersection::PlaneAABBBox(pCamera->m_vecFrustumTop,-pCamera->m_vecFrustumNormal[0],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumBottom,-pCamera->m_vecFrustumNormal[1],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumLeft,-pCamera->m_vecFrustumNormal[2],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumRight,-pCamera->m_vecFrustumNormal[3],m_MaxBox,m_MinBox) < 0.0f)
	{	
		int Range=(m_GrassPosX-PosX)*(m_GrassPosX-PosX)+(m_GrassPosY-PosY)*(m_GrassPosY-PosY);
		if(Range > MAX_GRASSRANGE*MAX_GRASSRANGE)
			return;
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);	

		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);		

		
		if(GF3OPTION)		
		{
			
		}		

		
		/*
		if(GF3OPTION)		
		{
			matrix matProjection,matHeightView,matTexScale,matInv,matView,mat,mat2,matTex;
			matProjection.MakeProjection(3.14159f/3.0f,1.0f,50,100000.0f);
			float fViewHeight=(SECTORSIZE/2.0f)/tanf(3.14159f/6.0f);
			
			float fCloudMoveX=(int)(m_MinBox.x/SECTORSIZE);
			float fCloudMoveZ=(int)(m_MinBox.z/SECTORSIZE);		

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

			pd3dDevice->SetTransform(D3DTS_TEXTURE2,(D3DMATRIX*)&matTex);
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

			pd3dDevice->SetTexture(2,CSectorScene::m_CloudShadow.GetTexture());
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE);			
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT);		

		}
		*/
		

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		//pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);


		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

		pd3dDevice->SetTexture(1,NULL);
		matrix matPos;	
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x10);

		matPos.MakeIdent();
		//pd3dDevice->SetTransform(D3DTS_WORLD1,matPos);
		//pd3dDevice->SetTransform(D3DTS_WORLD1,matPos);		
		
		//matRot.Rotation(vector3(1.0f,0.0f,0.0f),sinf(fRot)*0.003f);
		
		float frate=sinf(m_fGrassRot);
		
		if(frate>0.98f)
			frate=0.98f;
		if(frate<-0.98f)
			frate=-0.98f;
		
		
		//matRot.Rotation(vector3(1.0f,0.0f,0.0f),frate*0.003f);
		vector3 vecRotAxis;
		vecRotAxis=vector3(1.0f,0.0f,0.0f);		
		vecRotAxis.Normalize();
		matRot.Rotation(vecRotAxis,frate*0.008f);		
		matRot._41=m_AccumulateTM._41;		
		matRot._43=m_AccumulateTM._43;

		pd3dDevice->SetTransform(D3DTS_WORLD,matRot);
		pd3dDevice->SetTransform(D3DTS_WORLD1,m_AccumulateTM);

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);  
		
		CTexture *pTex = NULL;
		if(CSceneManager::m_WeatherManager.m_bSubTable)
		{
			
			pTex = CSceneManager::m_WeatherManager.GetCurrentSubGrassTexture();
			if(pTex)
			{
				pd3dDevice->SetTexture(0,pTex->GetTexture());
			}
			else
			{
				pd3dDevice->SetTexture(0,m_NormalTexture->GetTexture());
			}
		}
		else
			pd3dDevice->SetTexture(0,m_NormalTexture->GetTexture());


		//pd3dDevice->SetTexture(0,m_NormalTexture->GetTexture());

		
		pd3dDevice->SetIndices(m_pGrassIndicesBuffer,0);

		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);								
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);	

		pd3dDevice->SetVertexShader(GRASSVERTEXFVF);			
			
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		
		if(CRenderOption::m_GrassAnimationUseVertexShader)
		{   
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

			pd3dDevice->SetStreamSource(0,m_pGrassVertexBuffer,sizeof(GrassExtVertex));
			/*
			matrix matView,matProj,matViewProj;
			pd3dDevice->GetTransform(D3DTS_VIEW,matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);

			matViewProj=matView*matProj;
			matViewProj.Transpose();

			matrix matWorld0=matRot;
			matrix matWorld1=m_AccumulateTM;

			matWorld0.Transpose();
			matWorld1.Transpose();			
			
			pd3dDevice->SetVertexShaderConstant(4,&matWorld0,4);
			pd3dDevice->SetVertexShaderConstant(8,&matWorld1,4);
			pd3dDevice->SetVertexShaderConstant(12,&matViewProj,4);		     
			
			D3DXVECTOR4 vZero(0.4f,0.4f,0.4f,0.4f);        
			pd3dDevice->SetVertexShaderConstant(  0, &vZero,     1 );
			D3DXVECTOR4 vOne(1,1,1,1);        
			pd3dDevice->SetVertexShaderConstant(  1, &vOne,     1 );	

			float fColorRate=(frate*0.5f)+0.5f;
			D3DXVECTOR4 vRate(fColorRate,fColorRate,fColorRate,fColorRate);
			pd3dDevice->SetVertexShaderConstant(  2, &vRate,     1 );	

			float fScale=0.04f;
			D3DXVECTOR4 vScale(fScale,fScale,fScale,fScale);
			pd3dDevice->SetVertexShaderConstant(  3, &vScale,     1 );	
			*/
			
			matrix matView,matProj,matViewProj;
			pd3dDevice->GetTransform(D3DTS_VIEW,matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);
			
			matViewProj=m_AccumulateTM*matView*matProj;

			matViewProj.Transpose();

			pd3dDevice->SetVertexShaderConstant(0, &matViewProj,4);	

			D3DXVECTOR4 vZero(1.0f,1.0f,1.0f,1.0f);        
			pd3dDevice->SetVertexShaderConstant(4, &vZero,1);	

			
			//D3DXVECTOR4 vCRate(-1.44125,1.77187e-007,0.0121164,4983.58);
			//D3DXVECTOR4 vCRate(0.0121164,0.0121164,0.0121164,0.0121164);
			D3DXVECTOR4 vCRate(0.016f,0.016f,0.016f,0.016f);
			//D3DXVECTOR4 vCRate(1,1,1,1);

			pd3dDevice->SetVertexShaderConstant(5, &vCRate,1);			

			//D3DXVECTOR4 vCRate2(0.1736,0,-1.43,0.5f);
			D3DXVECTOR4 vCRate2(0.3f,0.3f,0.3f,4.5f);
			//D3DXVECTOR4 vCRate2(0.5f,0.5f,0.5f,0.5f);
			pd3dDevice->SetVertexShaderConstant(6, &vCRate2,1);			
			

			D3DXVECTOR4 vCRate3(10000.0f,10000.0f,10000.0f,10000.0f);
			pd3dDevice->SetVertexShaderConstant(7, &vCRate3,1);				
			

			
			float fUpperColor[4]={CSceneManager::m_WeatherManager.m_InterGrassColor.r/255.0f,
								  CSceneManager::m_WeatherManager.m_InterGrassColor.g/255.0f,
								  CSceneManager::m_WeatherManager.m_InterGrassColor.b/255.0f,1.0f};

			float fLowerColor[4]={fUpperColor[0]*100.0f/255.0f,
								  fUpperColor[1]*100.0f/255.0f,
								  fUpperColor[2]*100.0f/255.0f,1.0f};

			pd3dDevice->SetVertexShaderConstant(8,fLowerColor,1);
			pd3dDevice->SetVertexShaderConstant(9,fUpperColor,1);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			/*
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,1*8,
												 0,1*4);		
												 */

			pd3dDevice->SetVertexShader(m_dwGrassVertexShader);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);				
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

			

			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				

		}
		else
		{
			//////////////////
			pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwAlphaFactor);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);	
			//////////////////

			pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterGrassColor.c);	

			pd3dDevice->SetStreamSource(0,m_pGrassVertexBuffer,sizeof(GrassVertex));
			pd3dDevice->SetVertexShader(GRASSVERTEXFVF);
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);		

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			/*
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);		
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);
												 */

			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

		}
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);

		CSceneManager::m_cRenderPolygon+=m_cGrassElement*4;
		CSceneManager::m_cRenderVertex+=m_cGrassElement*8;		

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
		
		
		{
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
			pd3dDevice->SetTexture(2,NULL);

			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		}		
	}
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	fBias=0.0f;
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,0);
	

	//pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	/*
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetVertexShader(TTVERTEXFVF);	
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE*2,MAX_GRASS*MAX_GRASSRANGE*MAX_GRASSRANGE,m_GrassPosIndices,D3DFMT_INDEX16,
		m_GrassPosVertex,sizeof(TTVertex));
		*/
	
}

void CGrassScene::Generate(float fx, float fz,int cGrassElement)
{
	if(CRenderOption::m_GrassAnimationUseVertexShader)
	{
		std::vector<vector3> TerrainPoly;

		CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vector3(fx+LINTERVAL/2.0f,0.0f,fz+LINTERVAL/2.0f),TerrainPoly);
		char *pShadowCollision=CSceneManager::m_HeightField.GetHeightFieldShadowCollision(vector3(fx+LINTERVAL/2.0f,0.0f,fz+LINTERVAL/2.0f));	

		vector3 vecStart,vecEnd;
		vecStart.y=0.0f;
		vecEnd.y=100000.0f;
		float fLens;
		m_cGrassElement=cGrassElement;

		m_MinBox=vector3(fx,0.0f,fz);
		m_MaxBox=vector3(fx+LINTERVAL,0.0f,fz+LINTERVAL);

		float fMax,fMin;
		fMax=-10000000.0f;
		fMin=10000000.0f;

		//float fInSectorPosX,fInSectorPosY;
		float fInterVal=SECTORSIZE/(float)SHADOWSIZE;

		GrassExtVertex *pVertex;
		m_pGrassVertexBuffer->Lock(0,0,(BYTE**)&pVertex,0);	

		vector3 vecGrassPos;

		vector3 vecAbsPos=vector3(fx,0.0f,fz);
		m_AccumulateTM.Translation(vecAbsPos);
		
		m_fGrassRot=(rand()%1000)*0.001f;

		int cGrassReal=0;
		float fRate;

		float fGrassTextureU,fGrassTextureV;

		float fGrassHeight,fGrassWidth;
		int nGrassKind;
		for(int cGrass=0;cGrass<cGrassElement;cGrass++)
		{
			float fABSPosX=vecGrassPos.x=vecEnd.x=vecStart.x=(rand()%(int)LINTERVAL)+fx;
			float fABSPosZ=vecGrassPos.z=vecEnd.z=vecStart.z=(rand()%(int)LINTERVAL)+fz;
			vecGrassPos.y=0.0f;
			

			int px=(int)(vecGrassPos.x/SECTORSIZE);
			int pz=(int)(vecGrassPos.z/SECTORSIZE);
			float fPosx=vecGrassPos.x-(px*SECTORSIZE);
			float fPosz=vecGrassPos.z-(pz*SECTORSIZE);		
			
			if(fPosx<0.0f || fPosz<0.0f)
				continue;

			if(pShadowCollision==NULL)
				continue;
			

			if( (pShadowCollision[(int)(fPosx/fInterVal)+(int)(fPosz/fInterVal)*SHADOWSIZE] & 0xf0)!=0x80 )
				continue;
			

			
			for(int cIndices=0;cIndices<(int)TerrainPoly.size()/3;cIndices++)
			{			
				if(CIntersection::PolygonRay(vecStart,vecEnd,&TerrainPoly[cIndices*3],fLens))
				{			
					fRate=(rand()%100)/100.0f;					

					vecGrassPos.y=10000.0f;					
					if(fLens>fMax)
						fMax=fLens;
					if(fLens<fMin)
						fMin=fLens;

					nGrassKind=rand()%4;					

					switch(nGrassKind)
					{						
						case 0:						
						case 1:						
						case 2:
						case 3:
						case 4:						
							fGrassHeight=80.0f;
							fGrassWidth=40.0f;
							break;
					}					
					//fGrassHeight*=1.0f+(rand()%100)/100.0f;					
					
					fGrassTextureV=(nGrassKind/2)*0.5f;
					fGrassTextureU=(nGrassKind%2)*0.5f;

					fRate=sinf(vecGrassPos.x*0.003f);
					/*
					class GrassExtVertex 
{
	vector3 v;
	float fw0,fw1;
	float index0,index1;
	float tu,tv;	
};
*/
					for(int i=0;i<8;i++)
					{
						int nPos=(int)((fABSPosX)/200.0f);
						pVertex[cGrassReal*8+i].index0=4*(nPos%19)+4;
						pVertex[cGrassReal*8+i].index1=0;

						//vecGrassPos.y=10000.0f;

						pVertex[cGrassReal*8+i].fw1=fLens-10000.0f;
					}
					fRate=1.0f;

					pVertex[cGrassReal*8+0].v=vecGrassPos+vector3(-fGrassWidth,fGrassHeight,0.0f)-vecAbsPos;
					pVertex[cGrassReal*8+0].fw0=fRate;					
					
					pVertex[cGrassReal*8+1].v=vecGrassPos+vector3(-fGrassWidth,0.0f,0.0f)-vecAbsPos;				
					pVertex[cGrassReal*8+1].fw0=0.0f;
					
					
					pVertex[cGrassReal*8+2].v=vecGrassPos+vector3(fGrassWidth,fGrassHeight,0.0f)-vecAbsPos;
					pVertex[cGrassReal*8+2].fw0=fRate;
					
					

					pVertex[cGrassReal*8+3].v=vecGrassPos+vector3(fGrassWidth,0.0f,0.0f)-vecAbsPos;
	
					pVertex[cGrassReal*8+3].fw0=0.0f;
					

					pVertex[cGrassReal*8+0].tu=fGrassTextureU+0.01f;
					pVertex[cGrassReal*8+0].tv=fGrassTextureV+0.01f;

					pVertex[cGrassReal*8+1].tu=fGrassTextureU+0.01f;
					pVertex[cGrassReal*8+1].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+2].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+2].tv=fGrassTextureV+0.01f;

					pVertex[cGrassReal*8+3].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+3].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+4].v=vecGrassPos+vector3(0.0f,fGrassHeight,-fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+4].fw0=fRate;
					
					pVertex[cGrassReal*8+5].v=vecGrassPos+vector3(0.0f,0.0f,-fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+5].fw0=0.0f;
					
					pVertex[cGrassReal*8+6].v=vecGrassPos+vector3(0.0f,fGrassHeight,fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+6].fw0=fRate;
					
					pVertex[cGrassReal*8+7].v=vecGrassPos+vector3(0.0f,0.0f,fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+7].fw0=0.0f;					

					pVertex[cGrassReal*8+4].tu=fGrassTextureU+0.01f;
					pVertex[cGrassReal*8+4].tv=fGrassTextureV+0.01f;

					pVertex[cGrassReal*8+5].tu=fGrassTextureU+0.01f;
					pVertex[cGrassReal*8+5].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+6].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+6].tv=fGrassTextureV+0.01f;

					pVertex[cGrassReal*8+7].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+7].tv=fGrassTextureV+0.5f;		
					
					if(pShadowCollision!=NULL)
					{			
						fPosx/=fInterVal;
						fPosz/=fInterVal;
						if((pShadowCollision[(int)fPosx+((int)fPosz)*SHADOWSIZE]&0x0f)==0x1)
						{
							pVertex[cGrassReal*8+0].fColorRate=0.7f;
							pVertex[cGrassReal*8+1].fColorRate=0.7f;
							pVertex[cGrassReal*8+2].fColorRate=0.7f;
							pVertex[cGrassReal*8+3].fColorRate=0.7f;
							pVertex[cGrassReal*8+4].fColorRate=0.7f;
							pVertex[cGrassReal*8+5].fColorRate=0.7f;
							pVertex[cGrassReal*8+6].fColorRate=0.7f;
							pVertex[cGrassReal*8+7].fColorRate=0.7f;

							//m_GrassShadow[cGrass]=1;
							/*
							pVertex[cGrassReal*8+0].diff.c=0xff888888;
							pVertex[cGrassReal*8+1].diff.c=0xff888888;
							pVertex[cGrassReal*8+2].diff.c=0xff888888;
							pVertex[cGrassReal*8+3].diff.c=0xff888888;
							pVertex[cGrassReal*8+4].diff.c=0xff888888;
							pVertex[cGrassReal*8+5].diff.c=0xff888888;
							pVertex[cGrassReal*8+6].diff.c=0xff888888;
							pVertex[cGrassReal*8+7].diff.c=0xff888888;
							*/
						}
						else
						{
							pVertex[cGrassReal*8+0].fColorRate=1.0f;
							pVertex[cGrassReal*8+1].fColorRate=1.0f;
							pVertex[cGrassReal*8+2].fColorRate=1.0f;
							pVertex[cGrassReal*8+3].fColorRate=1.0f;
							pVertex[cGrassReal*8+4].fColorRate=1.0f;
							pVertex[cGrassReal*8+5].fColorRate=1.0f;
							pVertex[cGrassReal*8+6].fColorRate=1.0f;
							pVertex[cGrassReal*8+7].fColorRate=1.0f;

							//m_GrassShadow[cGrass]=0;
							/*
							pVertex[cGrassReal*8+0].diff.c=0xffffffff;
							pVertex[cGrassReal*8+1].diff.c=0xffffffff;
							pVertex[cGrassReal*8+2].diff.c=0xffffffff;
							pVertex[cGrassReal*8+3].diff.c=0xffffffff;
							pVertex[cGrassReal*8+4].diff.c=0xffffffff;
							pVertex[cGrassReal*8+5].diff.c=0xffffffff;
							pVertex[cGrassReal*8+6].diff.c=0xffffffff;
							pVertex[cGrassReal*8+7].diff.c=0xffffffff;
							*/
						}			
					}
					else
					{
						pVertex[cGrassReal*8+0].fColorRate=1.0f;
						pVertex[cGrassReal*8+1].fColorRate=1.0f;
						pVertex[cGrassReal*8+2].fColorRate=1.0f;
						pVertex[cGrassReal*8+3].fColorRate=1.0f;
						pVertex[cGrassReal*8+4].fColorRate=1.0f;
						pVertex[cGrassReal*8+5].fColorRate=1.0f;
						pVertex[cGrassReal*8+6].fColorRate=1.0f;
						pVertex[cGrassReal*8+7].fColorRate=1.0f;

						/*
						pVertex[cGrassReal*8+0].diff.c=0xffffffff;
						pVertex[cGrassReal*8+1].diff.c=0xffffffff;
						pVertex[cGrassReal*8+2].diff.c=0xffffffff;
						pVertex[cGrassReal*8+3].diff.c=0xffffffff;
						pVertex[cGrassReal*8+4].diff.c=0xffffffff;
						pVertex[cGrassReal*8+5].diff.c=0xffffffff;
						pVertex[cGrassReal*8+6].diff.c=0xffffffff;
						pVertex[cGrassReal*8+7].diff.c=0xffffffff;
						//m_GrassShadow[cGrass]=0;
						*/
					}
					
					cGrassReal++;
					break;
				}
			}		
		}

		m_pGrassVertexBuffer->Unlock();

		m_cGrassElement=cGrassReal;

		m_MinBox.y=fMin;
		m_MaxBox.y=fMax;
	}
	else
	{
		std::vector<vector3> TerrainPoly;

		CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vector3(fx+LINTERVAL/2.0f,0.0f,fz+LINTERVAL/2.0f),TerrainPoly);
		char *pShadowCollision=CSceneManager::m_HeightField.GetHeightFieldShadowCollision(vector3(fx+LINTERVAL/2.0f,0.0f,fz+LINTERVAL/2.0f));	

		vector3 vecStart,vecEnd;
		vecStart.y=0.0f;
		vecEnd.y=100000.0f;
		float fLens;
		m_cGrassElement=cGrassElement;

		m_MinBox=vector3(fx,0.0f,fz);
		m_MaxBox=vector3(fx+LINTERVAL,0.0f,fz+LINTERVAL);

		float fMax,fMin;
		fMax=-10000000.0f;
		fMin=10000000.0f;

		//float fInSectorPosX,fInSectorPosY;
		float fInterVal=SECTORSIZE/(float)SHADOWSIZE;

		m_dwAlphaFactor = 0x00ffffff;
		m_dwAlphaUnit = 0x00500000;
		m_dwLimit = 0x00000000;

		GrassVertex *pVertex;
		m_pGrassVertexBuffer->Lock(0,0,(BYTE**)&pVertex,0);	

		vector3 vecGrassPos;

		vector3 vecAbsPos=vector3(fx,0.0f,fz);
		m_AccumulateTM.Translation(vecAbsPos);
		
		m_fGrassRot=(rand()%1000)*0.001f;

		int cGrassReal=0;
		float fRate;

		float fGrassTextureU,fGrassTextureV;

		float fGrassHeight,fGrassWidth;
		int nGrassKind;
		for(int cGrass=0;cGrass<cGrassElement;cGrass++)
		{
			vecGrassPos.x=vecEnd.x=vecStart.x=(rand()%(int)LINTERVAL)+fx;
			vecGrassPos.z=vecEnd.z=vecStart.z=(rand()%(int)LINTERVAL)+fz;
			vecGrassPos.y=0.0f;
			

			int px=(int)(vecGrassPos.x/SECTORSIZE);
			int pz=(int)(vecGrassPos.z/SECTORSIZE);
			float fPosx=vecGrassPos.x-(px*SECTORSIZE);
			float fPosz=vecGrassPos.z-(pz*SECTORSIZE);		
			
			if(fPosx<0.0f || fPosz<0.0f)
				continue;

			if(pShadowCollision==NULL)
				continue;
			

			if( (pShadowCollision[(int)(fPosx/fInterVal)+(int)(fPosz/fInterVal)*SHADOWSIZE] & 0xf0)!=0x80 )
				continue;
			

			
			for(int cIndices=0;cIndices<(int)TerrainPoly.size()/3;cIndices++)
			{			
				if(CIntersection::PolygonRay(vecStart,vecEnd,&TerrainPoly[cIndices*3],fLens))
				{			

					fRate=(rand()%100)/100.0f;
					vecGrassPos.y=fLens;
					//m_vecGrassPos[cGrass].y=fLens;
					if(fLens>fMax)
						fMax=fLens;
					if(fLens<fMin)
						fMin=fLens;

					nGrassKind=rand()%4;					

					switch(nGrassKind)
					{						
						case 0:						
						case 1:						
						case 2:
						case 3:
							fGrassHeight=80.0f;
							fGrassWidth=60.0f;
							break;
					}					
					/*
					fGrassHeight*=1.8f;
					fGrassWidth*=1.8f;
					//*/
					fGrassHeight*=1.0f+(rand()%100)/100.0f;
					
					fGrassTextureV=(nGrassKind/2)*0.5f;
					fGrassTextureU=(nGrassKind%2)*0.5f;

					fRate=sinf(vecGrassPos.x*0.003f);

					pVertex[cGrassReal*8+0].v=vecGrassPos+vector3(-fGrassWidth,fGrassHeight,0.0f)-vecAbsPos;
					pVertex[cGrassReal*8+0].fWeight=fRate;
					pVertex[cGrassReal*8+1].v=vecGrassPos+vector3(-fGrassWidth,0.0f,0.0f)-vecAbsPos;				
					pVertex[cGrassReal*8+1].fWeight=0.0f;
					pVertex[cGrassReal*8+2].v=vecGrassPos+vector3(fGrassWidth,fGrassHeight,0.0f)-vecAbsPos;
					pVertex[cGrassReal*8+2].fWeight=fRate;
					pVertex[cGrassReal*8+3].v=vecGrassPos+vector3(fGrassWidth,0.0f,0.0f)-vecAbsPos;
					pVertex[cGrassReal*8+3].fWeight=0.0f;

					pVertex[cGrassReal*8+0].tu=fGrassTextureU + 0.01f;
					pVertex[cGrassReal*8+0].tv=fGrassTextureV + 0.01f;

					pVertex[cGrassReal*8+1].tu=fGrassTextureU  + 0.01f;
					pVertex[cGrassReal*8+1].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+2].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+2].tv=fGrassTextureV  + 0.01f;

					pVertex[cGrassReal*8+3].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+3].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+4].v=vecGrassPos+vector3(0.0f,fGrassHeight,-fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+4].fWeight=fRate;
					pVertex[cGrassReal*8+5].v=vecGrassPos+vector3(0.0f,0.0f,-fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+5].fWeight=0.0f;
					pVertex[cGrassReal*8+6].v=vecGrassPos+vector3(0.0f,fGrassHeight,fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+6].fWeight=fRate;
					pVertex[cGrassReal*8+7].v=vecGrassPos+vector3(0.0f,0.0f,fGrassWidth)-vecAbsPos;
					pVertex[cGrassReal*8+7].fWeight=0.0f;

					pVertex[cGrassReal*8+4].tu=fGrassTextureU  + 0.01f;
					pVertex[cGrassReal*8+4].tv=fGrassTextureV  + 0.01f;

					pVertex[cGrassReal*8+5].tu=fGrassTextureU  + 0.01f;
					pVertex[cGrassReal*8+5].tv=fGrassTextureV+0.5f;

					pVertex[cGrassReal*8+6].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+6].tv=fGrassTextureV  + 0.01f;

					pVertex[cGrassReal*8+7].tu=fGrassTextureU+0.5f;
					pVertex[cGrassReal*8+7].tv=fGrassTextureV+0.5f;		
					
					if(pShadowCollision!=NULL)
					{			
						fPosx/=fInterVal;
						fPosz/=fInterVal;
						if((pShadowCollision[(int)fPosx+((int)fPosz)*SHADOWSIZE]&0x0f)==0x1)
						{
							//m_GrassShadow[cGrass]=1;

							pVertex[cGrassReal*8+0].diff.c=0xff888888;
							pVertex[cGrassReal*8+1].diff.c=0xff888888;
							pVertex[cGrassReal*8+2].diff.c=0xff888888;
							pVertex[cGrassReal*8+3].diff.c=0xff888888;
							pVertex[cGrassReal*8+4].diff.c=0xff888888;
							pVertex[cGrassReal*8+5].diff.c=0xff888888;
							pVertex[cGrassReal*8+6].diff.c=0xff888888;
							pVertex[cGrassReal*8+7].diff.c=0xff888888;
						}
						else
						{
							//m_GrassShadow[cGrass]=0;

							pVertex[cGrassReal*8+0].diff.c=0xffffffff;
							pVertex[cGrassReal*8+1].diff.c=0xffffffff;
							pVertex[cGrassReal*8+2].diff.c=0xffffffff;
							pVertex[cGrassReal*8+3].diff.c=0xffffffff;
							pVertex[cGrassReal*8+4].diff.c=0xffffffff;
							pVertex[cGrassReal*8+5].diff.c=0xffffffff;
							pVertex[cGrassReal*8+6].diff.c=0xffffffff;
							pVertex[cGrassReal*8+7].diff.c=0xffffffff;
						}			
					}
					else
					{
						pVertex[cGrassReal*8+0].diff.c=0xffffffff;
						pVertex[cGrassReal*8+1].diff.c=0xffffffff;
						pVertex[cGrassReal*8+2].diff.c=0xffffffff;
						pVertex[cGrassReal*8+3].diff.c=0xffffffff;
						pVertex[cGrassReal*8+4].diff.c=0xffffffff;
						pVertex[cGrassReal*8+5].diff.c=0xffffffff;
						pVertex[cGrassReal*8+6].diff.c=0xffffffff;
						pVertex[cGrassReal*8+7].diff.c=0xffffffff;
						//m_GrassShadow[cGrass]=0;
					}
					
					cGrassReal++;
					break;
				}
			}		
		}

		m_pGrassVertexBuffer->Unlock();

		m_cGrassElement=cGrassReal;

		m_MinBox.y=fMin;
		m_MaxBox.y=fMax;
	}
	
	/*
	vector3 vecChrPos=CSceneManager::m_ViewCamera->m_matPosition.GetLoc();
	int cPos=0;
	float fix,fiy;			

	fix=-LINTERVAL*MAX_GRASSRANGE/2.0f;
	for(int ix=0;ix<MAX_GRASSRANGE;ix++,fix+=LINTERVAL)
	{
		fiy=-LINTERVAL*MAX_GRASSRANGE/2.0f;
		for(int iy=0;iy<MAX_GRASSRANGE;iy++,fiy+=LINTERVAL)
		{
			//List<vector3> TerrainPoly;
			CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecChrPos+vector3(fix,0.0f,fiy),TerrainPoly);
			vector3 vecPoly[3];
			float fLens;

			for(int cGrass=0;cGrass<MAX_GRASS;cGrass++)
			{
				for(int cIndices=0;cIndices<TerrainPoly.num/3;cIndices++)
				{
					vecPoly[0]=TerrainPoly[cIndices*3+0];
					vecPoly[1]=TerrainPoly[cIndices*3+1];
					vecPoly[2]=TerrainPoly[cIndices*3+2];
					
					if(CIntersection::PolygonRay(vector3(m_vecGrassPos[cPos].x+vecChrPos.x,0.0f,m_vecGrassPos[cPos].z+vecChrPos.z),
												 vector3(m_vecGrassPos[cPos].x+vecChrPos.x,100000.0f,m_vecGrassPos[cPos].z+vecChrPos.z),vecPoly,fLens))
					{
						m_vecGrassPos[cPos].y=fLens;

						m_vecRealGrassPos[cPos].x=m_vecGrassPos[cPos].x+vecChrPos.x;
						m_vecRealGrassPos[cPos].y=fLens;
						m_vecRealGrassPos[cPos].z=m_vecGrassPos[cPos].z+vecChrPos.z;
						
						//m_vecGrassPos[cPos].x=m_vecGrassPos[cPos].x+vecChrPos.x;
						//m_vecGrassPos[cPos].z=m_vecGrassPos[cPos].z+vecChrPos.z;
						break;
					}
				}
				cPos++;
			}
		}
	}
	*/
}

void CGrassScene::Update(float fUpdate)
{
	m_fGrassRot+=fUpdate;
	m_dwLimit += m_dwAlphaUnit >> 16;

	if(m_dwLimit > 0xff00)
		m_dwAlphaFactor = 0xffffffff;
	else {
		m_dwAlphaFactor += m_dwAlphaUnit;
		m_dwAlphaUnit += 0x00100000;
	}
	
}

void CGrassScene::RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice,CRenderTexture &pShadowTexture)
{
	if(m_cGrassElement==0)
		return;

	CViewCamera *pCamera=CSceneManager::GetCamera();			

	vector3 *ViewPos;
	ViewPos=CSceneManager::GetCamera()->GetPosition();
	
	int PosX=(int)(ViewPos->x/LINTERVAL);
	int PosY=(int)(ViewPos->z/LINTERVAL);

	if( CIntersection::PlaneAABBBox(pCamera->m_vecFrustumTop,-pCamera->m_vecFrustumNormal[0],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumBottom,-pCamera->m_vecFrustumNormal[1],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumLeft,-pCamera->m_vecFrustumNormal[2],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumRight,-pCamera->m_vecFrustumNormal[3],m_MaxBox,m_MinBox) < 0.0f)
	{	
		int Range=(m_GrassPosX-PosX)*(m_GrassPosX-PosX)+(m_GrassPosY-PosY)*(m_GrassPosY-PosY);
		if(Range > MAX_GRASSRANGE*MAX_GRASSRANGE)
			return;	
	}
	else
	{
		return;
	}
	
	vector3 vecCenter=m_MaxBox+m_MinBox;
	vector3 vecObjectPos=vecCenter*0.5f;
	std::vector<vector3> ShadowedPoly;	
	CSceneManager::m_HeightField.GetHeightFieldShadowPoly(vecObjectPos,ShadowedPoly);

	if(ShadowedPoly.empty())
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
	//float fViewInter;

	vecSunLightDir.y-=0.47f;
	vecSunLightDir.Normalize();

	
	

	matrix matObjectCamera;			
		

		TTVertex *pShadowedVertex=new TTVertex[ShadowedPoly.size()];
		WORD *pShadowedIndices=new WORD[ShadowedPoly.size()];
		for(int cIndices=0;cIndices<(int)ShadowedPoly.size();cIndices++)
		{
			pShadowedVertex[cIndices].v=ShadowedPoly[cIndices];
			pShadowedVertex[cIndices].diff.c=0xff777777;
			pShadowedIndices[cIndices]=cIndices;			
		}
		
		//matShadowProj.MakeNoProjection(3.14159f/3.0f,(float)SHADOWSIZE+5.0f*2.0f,(float)SHADOWSIZE+5.0f*2.0f,10.0f,5500.f);

		D3DXMatrixOrthoLH(matShadowProj,500.0f*1.8f,500.0f*1.8f,10.0f, 100000.0f);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		
	
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		pShadowTexture.Begin(pd3dDevice);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->BeginScene();
		//matrix matObjectCamera;		

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
		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);				
		pd3dDevice->SetVertexShader(TTVERTEXFVF);	
		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);

		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)ShadowedPoly.size(),(int)ShadowedPoly.size()/3,pShadowedIndices,
			D3DFMT_INDEX16,pShadowedVertex,sizeof(TTVertex));		
		

		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);		
		//pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);						

		//////////////////////////////

		Render(pd3dDevice);

		//////////////////////////////
		
		pd3dDevice->EndScene();		
		pShadowTexture.End(pd3dDevice);

		////////////////////////////

		pd3dDevice->BeginScene();	
		
		pd3dDevice->SetTransform(D3DTS_VIEW,CSceneManager::m_ViewCamera->GetMatView());		
		
		matrix mat,mat2;
		mat=matShadowProj*matTexScale;		
		mat2=matObjectCamera*mat;
		matTex=matInv*mat2;

		pd3dDevice->SetTransform(D3DTS_PROJECTION,matOldProj);
		pd3dDevice->SetTransform(D3DTS_TEXTURE0,matTex);
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

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetTransform(D3DTS_WORLD,matIndent);
		
		pd3dDevice->SetRenderState(D3DRS_ZBIAS,14);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW);
		
		pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,(int)ShadowedPoly.size(),(int)ShadowedPoly.size()/3,pShadowedIndices,
			D3DFMT_INDEX16,pShadowedVertex,sizeof(TTVertex));
		
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
		delete [] pShadowedVertex;
		delete [] pShadowedIndices;
		pd3dDevice->EndScene();				
	
	/*
	if(m_cGrassElement==0)
		return;
	CViewCamera *pCamera=CSceneManager::GetCamera();			
	vector3 *ViewPos;
	ViewPos=CSceneManager::GetCamera()->GetPosition();
	
	int PosX=(int)(ViewPos->x/LINTERVAL);
	int PosY=(int)(ViewPos->z/LINTERVAL);
	
	//fRot=4.14f;	
	

	matrix matRot;

	float fBias=0.0f;		
	
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);			

		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTexture(1,NULL);

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);				

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

		pd3dDevice->SetTexture(1,NULL);
		matrix matPos;	
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x10);

		matPos.MakeIdent();	
		
		float frate=sinf(m_fGrassRot);
		
		if(frate>0.98f)
			frate=0.98f;
		if(frate<-0.98f)
			frate=-0.98f;	
		
		vector3 vecRotAxis;
		vecRotAxis=vector3(1.0f,0.0f,0.0f);		
		vecRotAxis.Normalize();
		matRot.Rotation(vecRotAxis,frate*0.008f);		
		matRot._41=m_AccumulateTM._41;		
		matRot._43=m_AccumulateTM._43;
		pd3dDevice->SetTransform(D3DTS_WORLD,matRot);
		pd3dDevice->SetTransform(D3DTS_WORLD1,m_AccumulateTM);

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);  
		pd3dDevice->SetTexture(0,m_NormalTexture->GetTexture());

		
		pd3dDevice->SetIndices(m_pGrassIndicesBuffer,0);

		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);								
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffffffff);	

		pd3dDevice->SetVertexShader(GRASSVERTEXFVF);			
			
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		
		if(CRenderOption::m_GrassAnimationUseVertexShader)
		{   
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	

			pd3dDevice->SetStreamSource(0,m_pGrassVertexBuffer,sizeof(GrassExtVertex));			
			
			matrix matView,matProj,matViewProj;
			pd3dDevice->GetTransform(D3DTS_VIEW,matView);
			pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);
			
			matViewProj=m_AccumulateTM*matView*matProj;

			matViewProj.Transpose();

			pd3dDevice->SetVertexShaderConstant(0, &matViewProj,4);	

			D3DXVECTOR4 vZero(1.0f,1.0f,1.0f,1.0f);        
			pd3dDevice->SetVertexShaderConstant(4, &vZero,1);	

			
			
			D3DXVECTOR4 vCRate(0.016f,0.016f,0.016f,0.016f);
			

			pd3dDevice->SetVertexShaderConstant(5, &vCRate,1);			

			
			D3DXVECTOR4 vCRate2(0.3,0.3,0.3,4.5f);
			
			pd3dDevice->SetVertexShaderConstant(6, &vCRate2,1);			
			

			D3DXVECTOR4 vCRate3(10000.0f,10000.0f,10000.0f,10000.0f);
			pd3dDevice->SetVertexShaderConstant(7, &vCRate3,1);	

			float fLowerColor[4]={100.0f/255.0f,102.0f/255.0f,115.0f/255.0f,1.0f};
			float fUpperColor[4]={255.0f/255.0f,255.0f/255.0f,255.0f/255.0f,1.0f};

			pd3dDevice->SetVertexShaderConstant(8,fLowerColor,1);
			pd3dDevice->SetVertexShaderConstant(9,fUpperColor,1);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);		

			pd3dDevice->SetVertexShader(m_dwGrassVertexShader);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);				
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x50);	
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				

		}
		else
		{
			pd3dDevice->SetStreamSource(0,m_pGrassVertexBuffer,sizeof(GrassVertex));
			pd3dDevice->SetVertexShader(GRASSVERTEXFVF);
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
												 0,m_cGrassElement*8,
												 0,m_cGrassElement*4);		

			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				
		}
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);				
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);

		CSceneManager::m_cRenderPolygon+=m_cGrassElement*4;
		CSceneManager::m_cRenderVertex+=m_cGrassElement*8;		

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
		
		
		{
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
			pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );	
			pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
			pd3dDevice->SetTexture(2,NULL);

			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		}		
	}
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	fBias=0.0f;
	pd3dDevice->SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
	pd3dDevice->SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,0);	
	*/

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->BeginScene();
}
