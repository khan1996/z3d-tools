// HouseObject.cpp: implementation of the CHouseObject class.
//
//////////////////////////////////////////////////////////////////////

#include "HouseObject.h"
#include "SceneManager.h"
#include "BaseGraphicsLayer.h"
#include "RenderOption.h"
#include "./OctreeScene.h"
#include "GMMemory.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHouseObject::CHouseObject() : m_pInHouseObject(NULL),m_pMedHouseObject(NULL),m_pOutHouseObject(NULL),m_Lightmap(NULL),m_InLightmap(NULL),
								m_pOctree(NULL),m_isHaveLightmap(false),m_isHaveInLightmap(false),m_bModulate2X(false), m_pFlagHouseObject(NULL)
{

}
CHouseObject::~CHouseObject()
{		
	int i;
	/*** *********/
	// Octree resource 처리 해야함.COctreeScene *m_pOctree;
	if(m_pInHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pInHouseObject);
		m_pInHouseObject = NULL;
	}

	if(m_pMedHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pMedHouseObject);
		m_pMedHouseObject = NULL;
	}

	if(m_pOutHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pOutHouseObject);
		m_pOutHouseObject = NULL;
	}

	if(m_pFlagHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pFlagHouseObject);
		m_pFlagHouseObject = NULL;
	}

	if(m_Lightmap)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_Lightmap);
		m_Lightmap = NULL;

	}
	
	if(m_InLightmap)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_InLightmap);
		m_InLightmap = NULL;

	}
	
	if(m_pOctree)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_OCTREESCENE,m_pOctree);
		m_pOctree = NULL;
	}

	for(i = 0; i < (int)m_pVertex.size(); i++ )
	{
		if(m_pVertex[i])
		{
			delete[] m_pVertex[i];
			m_pVertex[i] = NULL;
		}
	}
	m_pVertex.clear();


	for(i = 0; i < (int)m_pIndices.size(); i++ )
	{
		if(m_pIndices[i])
		{
			delete[] m_pIndices[i];
			m_pIndices[i] = NULL;
		}
	}
	m_pIndices.clear();

	m_nVertex.clear();
	m_nIndices.clear();

}

const long MAX_OBJECT=100;
void CHouseObject::GenerateNewLightmap(int iValue,matrix matSet,matrix matHouseTM,CSectorLight *pLights,int iLights)
{



}
void CHouseObject::GenerateLightmap(matrix matSet,matrix matHouseTM,int LightmapMode)
{	
	
}

void CHouseObject::Render(LPDIRECT3DDEVICE8	pd3dDevice)
{
	/**/D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    pd3dDevice->SetMaterial( &mtrl );

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

	pd3dDevice->SetTexture(2,NULL);
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	
	// 2005.01.10 yundi 새벽 3시에 보였다 안보였다 하는 house 문제 해결
	color tempColor = CSceneManager::m_WeatherManager.m_InterObjectAmbientColor;
	tempColor.a = 0xFF;

	if(m_pOutHouseObject && m_pOutHouseObject->IsLoaded())
	{
		if(m_isHaveLightmap && m_Lightmap->IsLoaded())
		{
	
				pd3dDevice->SetTexture(1,m_Lightmap->GetTexture());

				pd3dDevice->LightEnable(0,FALSE);
				pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);

				pd3dDevice->SetRenderState( D3DRS_AMBIENT, tempColor.c);
				/**/
				pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
				pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, tempColor.c);

				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
				
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );


				if(m_bModulate2X)
					pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
				else
					pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				

				pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
				pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
				////


				//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
				//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

				m_pOutHouseObject->Render(pd3dDevice);

				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

		}
		else
		{
			//m_pOutHouseObject->ConvertNormal();

			pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
			pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
			pd3dDevice->LightEnable(0,TRUE);
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			pd3dDevice->SetTexture(1,NULL);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_pOutHouseObject->Render(pd3dDevice);
		
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

		}

//}//////////////////////////////////////////////////////////////////////////


	}	//if(m_pOutHouseObject)
	else
	{

	}
	if(m_pMedHouseObject && m_pMedHouseObject->IsLoaded())
	{

//////////////////////////////////////////////////////////////////////////
//if (strstr(m_pMedHouseObject->m_strName, "_qwe"))
//{


		if(m_isHaveLightmap  && m_Lightmap->IsLoaded())
		{
			
			pd3dDevice->SetTexture(1,m_Lightmap->GetTexture());
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

			
			
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
			
			pd3dDevice->LightEnable(0,FALSE);
			pd3dDevice->SetRenderState( D3DRS_AMBIENT, tempColor.c);

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

			///////////// 엠비언트 수정
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, tempColor.c);				
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				
			/////////////

			m_pMedHouseObject->Render(pd3dDevice);

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
		}
		else
		{
			//m_pMedHouseObject->ConvertNormal();
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
			pd3dDevice->LightEnable(0,TRUE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
			
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);		
			// 라이트맵 없는 하우스 알파적용 7.29
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

			pd3dDevice->SetRenderState( D3DRS_AMBIENT, CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
			//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
			
			pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);

			m_pMedHouseObject->Render(pd3dDevice);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
			pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		
		}

//}//////////////////////////////////////////////////////////////////////////


	}	//	if(m_pMedHouseObject)
	if(m_pInHouseObject && m_pInHouseObject->IsLoaded())
	{

//////////////////////////////////////////////////////////////////////////
//if (strstr(m_pInHouseObject->m_strName, "_qwe"))
//{
	
		if(m_isHaveInLightmap  && m_InLightmap->IsLoaded())
		{
			pd3dDevice->SetTexture(1,m_InLightmap->GetTexture());
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			pd3dDevice->LightEnable(0,FALSE);
			pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
			m_pInHouseObject->Render(pd3dDevice);
		}
		else if(m_isHaveLightmap  && m_Lightmap->IsLoaded())
		{

			
			pd3dDevice->SetTexture(1,m_Lightmap->GetTexture());
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			pd3dDevice->LightEnable(0,FALSE);
			pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
			m_pInHouseObject->Render(pd3dDevice);

		}
		else
		{
			//m_pInHouseObject->ConvertNormal();	
			pd3dDevice->LightEnable(0,TRUE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			if(BaseGraphicsLayer::m_VoodooOption)
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			else
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);

			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

			pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
			pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);

			m_pInHouseObject->Render(pd3dDevice);
		}

//}//////////////////////////////////////////////////////////////////////////


	}

	if(m_pFlagHouseObject && m_pFlagHouseObject->IsLoaded())
	{			
		// edith 2009.05.05 9번존 깃발 위치잘못나오는것 왜이렇게 했냐
		matrix matWorld;
		matWorld.MakeIdent();

		matWorld._41 = 237305.0f;
		matWorld._42 = 16055.0f;
		matWorld._43 = 236122.0f;

		pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);

		pd3dDevice->LightEnable(0,TRUE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_pFlagHouseObject->Render(pd3dDevice);
	}
}

	

void CHouseObject::GenerateOctree()
{	
	GetPolygon();
	m_pOctree=new COctreeScene();
	vector3 pPoly[3];
	for(int i=0;i<(int)m_pVertex.size();i++)
	{
		WORD *pIndices=m_pIndices[i];
		vector3 *pVertex=m_pVertex[i];
		for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
		{
			pPoly[0]=pVertex[pIndices[cIndices*3+0]];
			pPoly[1]=pVertex[pIndices[cIndices*3+1]];
			pPoly[2]=pVertex[pIndices[cIndices*3+2]];			
			m_pOctree->AddPoly(pPoly);
		}
	}	
	
	vector3 vecBoundBox[8];
	vector3 vecPolyTest[3];
	vector3 vecPlaneNormal;
	matrix matTrans;	
	//float fIntersection;

	WORD wCollisionIndices[]=
	{
		4,1,0,
		1,4,5,

		5,3,1,
		3,5,7,

		7,2,3,
		2,7,6,		

		6,0,2,
		0,6,4
	};

	vector3 vecDirection[8];
	vecDirection[0]=vector3(0.707f,0.0f,0.707f);
	vecDirection[1]=vector3(0.707f,0.0f,-0.707f);
	vecDirection[2]=vector3(-0.707f,0.0f,0.707f);
	vecDirection[3]=vector3(-0.707f,0.0f,-0.707f);

	vecDirection[4]=vector3(1.0f,0.0f,0.0f);
	vecDirection[5]=vector3(-1.0f,0.0f,0.0f);
	vecDirection[6]=vector3(0.0f,0.0f,1.0f);
	vecDirection[7]=vector3(0.0f,0.0f,-1.0f);

	vector3 vecLens;
/*
	matrix matTM;
	for(int cObject=0;cObject<m_ObjectList.num;cObject++)
	{		
		matTM=m_ObjectList[cObject]->m_TM;				

		vecBoundBox[0].x=m_ObjectList[cObject]->m_vecMinBox.x;
		vecBoundBox[0].y=m_ObjectList[cObject]->m_vecMinBox.y;
		vecBoundBox[0].z=m_ObjectList[cObject]->m_vecMinBox.z;

		vecBoundBox[1].x=m_ObjectList[cObject]->m_vecMaxBox.x;
		vecBoundBox[1].y=m_ObjectList[cObject]->m_vecMinBox.y;
		vecBoundBox[1].z=m_ObjectList[cObject]->m_vecMinBox.z;

		vecBoundBox[2].x=m_ObjectList[cObject]->m_vecMinBox.x;
		vecBoundBox[2].y=m_ObjectList[cObject]->m_vecMinBox.y;
		vecBoundBox[2].z=m_ObjectList[cObject]->m_vecMaxBox.z;

		vecBoundBox[3].x=m_ObjectList[cObject]->m_vecMaxBox.x;
		vecBoundBox[3].y=m_ObjectList[cObject]->m_vecMinBox.y;
		vecBoundBox[3].z=m_ObjectList[cObject]->m_vecMaxBox.z;

		vecBoundBox[4].x=m_ObjectList[cObject]->m_vecMinBox.x;
		vecBoundBox[4].y=m_ObjectList[cObject]->m_vecMaxBox.y+50.0f;
		vecBoundBox[4].z=m_ObjectList[cObject]->m_vecMinBox.z;

		vecBoundBox[5].x=m_ObjectList[cObject]->m_vecMaxBox.x;
		vecBoundBox[5].y=m_ObjectList[cObject]->m_vecMaxBox.y+50.0f;
		vecBoundBox[5].z=m_ObjectList[cObject]->m_vecMinBox.z;

		vecBoundBox[6].x=m_ObjectList[cObject]->m_vecMinBox.x;
		vecBoundBox[6].y=m_ObjectList[cObject]->m_vecMaxBox.y+50.0f;
		vecBoundBox[6].z=m_ObjectList[cObject]->m_vecMaxBox.z;

		vecBoundBox[7].x=m_ObjectList[cObject]->m_vecMaxBox.x;
		vecBoundBox[7].y=m_ObjectList[cObject]->m_vecMaxBox.y+50.0f;
		vecBoundBox[7].z=m_ObjectList[cObject]->m_vecMaxBox.z;

		for(int i=0;i<8;i++)
		{
			matTrans.Translation(vecBoundBox[i]);
			matTrans=matTrans*matTM;
			vecBoundBox[i]=matTrans.GetLoc();
		}			

		//
		//vecPolyTest[0]=vecBoundBox[4];
		//vecPolyTest[1]=vecBoundBox[0];
		//vecPolyTest[2]=vecBoundBox[1];			
		bool bAlready=false;
		for(int cIndices=0;cIndices<8;cIndices++)
		{
			vecPolyTest[0]=vecBoundBox[wCollisionIndices[cIndices*3+0]];
			vecPolyTest[1]=vecBoundBox[wCollisionIndices[cIndices*3+1]];
			vecPolyTest[2]=vecBoundBox[wCollisionIndices[cIndices*3+2]];			

			m_pOctree->AddPoly(vecPolyTest);
		}

	}
	*/

	m_pOctree->GenerateOctree();		
}



void CHouseObject::CalcBox(vector3 &MinBox, vector3 &MaxBox,int iCalcObject)
{	
	if(iCalcObject == 0)
	{
	
		if(m_pOutHouseObject)
		{	
			MaxBox=m_pOutHouseObject->m_MaxBox;
			MinBox=m_pOutHouseObject->m_MinBox;	
		}
		if(m_pMedHouseObject)
		{
			if(MaxBox.x < m_pMedHouseObject->m_MaxBox.x)
				MaxBox.x=m_pMedHouseObject->m_MaxBox.x;
			if(MaxBox.y < m_pMedHouseObject->m_MaxBox.y)
				MaxBox.y=m_pMedHouseObject->m_MaxBox.y;
			if(MaxBox.z < m_pMedHouseObject->m_MaxBox.z)
				MaxBox.z=m_pMedHouseObject->m_MaxBox.z;

			if(MinBox.x > m_pMedHouseObject->m_MinBox.x)
				MaxBox.x=m_pMedHouseObject->m_MinBox.x;
			if(MinBox.y > m_pMedHouseObject->m_MinBox.y)
				MinBox.y=m_pMedHouseObject->m_MinBox.y;
			if(MinBox.z > m_pMedHouseObject->m_MinBox.z)
				MinBox.z=m_pMedHouseObject->m_MinBox.z;
			
		}
		if(m_pInHouseObject)
		{
			if(MaxBox.x < m_pOutHouseObject->m_MaxBox.x)
				MaxBox.x=m_pOutHouseObject->m_MaxBox.x;
			if(MaxBox.y < m_pOutHouseObject->m_MaxBox.y)
				MaxBox.y=m_pOutHouseObject->m_MaxBox.y;
			if(MaxBox.z < m_pOutHouseObject->m_MaxBox.z)
				MaxBox.z=m_pOutHouseObject->m_MaxBox.z;

			if(MinBox.x > m_pOutHouseObject->m_MinBox.x)
				MaxBox.x=m_pOutHouseObject->m_MinBox.x;
			if(MinBox.y > m_pOutHouseObject->m_MinBox.y)
				MinBox.y=m_pOutHouseObject->m_MinBox.y;
			if(MinBox.z > m_pOutHouseObject->m_MinBox.z)
				MinBox.z=m_pOutHouseObject->m_MinBox.z;
		}
	}
	else if(iCalcObject == 1)
	{
		if(m_pInHouseObject)
		{
			MaxBox = m_pInHouseObject->m_MaxBox;
			MinBox = m_pInHouseObject->m_MinBox;

		}
	}

}
void CHouseObject::GetPolygon()
{
	int cObject;
	m_pIndices.clear();
	m_nIndices.clear();
	m_pVertex.clear();

	if(m_pOutHouseObject)
	{
		m_pOutHouseObject->AllLockBuffer();	
		for(cObject=0;cObject<m_pOutHouseObject->GetObjectCount();cObject++)
		{	
			vector3 *AddVertex=new vector3[m_pOutHouseObject->m_nVertex[cObject]];			

			MultiVertex *pVertex=(MultiVertex*)m_pOutHouseObject->m_pVertex[cObject];
			for(int cVertex=0;cVertex<m_pOutHouseObject->m_nVertex[cObject];cVertex++)
				AddVertex[cVertex]=pVertex[cVertex].v;			

			m_pVertex.push_back(AddVertex);		
			WORD *AddIndices=new WORD[m_pOutHouseObject->m_nIndices[cObject]*3];
			memcpy(AddIndices,m_pOutHouseObject->m_pIndices[cObject],sizeof(WORD)*3*m_pOutHouseObject->m_nIndices[cObject]);
			m_pIndices.push_back(AddIndices);			
			m_nIndices.push_back(m_pOutHouseObject->m_nIndices[cObject]);
		}
		m_pOutHouseObject->AllUnlockBuffer();
	}	
	if(m_pInHouseObject)
	{
		m_pInHouseObject->AllLockBuffer();
		for(cObject=0;cObject<m_pInHouseObject->GetObjectCount();cObject++)
		{	
			vector3 *AddVertex=new vector3[m_pInHouseObject->m_nVertex[cObject]];

			MultiVertex *pVertex=(MultiVertex*)m_pInHouseObject->m_pVertex[cObject];
			for(int cVertex=0;cVertex<m_pInHouseObject->m_nVertex[cObject];cVertex++)
				AddVertex[cVertex]=pVertex[cVertex].v;

			m_pVertex.push_back(AddVertex);		
			WORD *AddIndices=new WORD[m_pInHouseObject->m_nIndices[cObject]*3];
			memcpy(AddIndices,m_pInHouseObject->m_pIndices[cObject],sizeof(WORD)*3*m_pInHouseObject->m_nIndices[cObject]);
			m_pIndices.push_back(AddIndices);			
			m_nIndices.push_back(m_pInHouseObject->m_nIndices[cObject]);
		}
		m_pInHouseObject->AllUnlockBuffer();
	}
	if(m_pMedHouseObject)
	{
		m_pMedHouseObject->AllLockBuffer();
		for(cObject=0;cObject<m_pMedHouseObject->GetObjectCount();cObject++)
		{	
			vector3 *AddVertex=new vector3[m_pMedHouseObject->m_nVertex[cObject]];

			MultiVertex *pVertex=(MultiVertex*)m_pMedHouseObject->m_pVertex[cObject];
			for(int cVertex=0;cVertex<m_pMedHouseObject->m_nVertex[cObject];cVertex++)
				AddVertex[cVertex]=pVertex[cVertex].v;

			m_pVertex.push_back(AddVertex);		
			WORD *AddIndices=new WORD[m_pMedHouseObject->m_nIndices[cObject]*3];
			memcpy(AddIndices,m_pMedHouseObject->m_pIndices[cObject],sizeof(WORD)*3*m_pMedHouseObject->m_nIndices[cObject]);
			m_pIndices.push_back(AddIndices);			
			m_nIndices.push_back(m_pMedHouseObject->m_nIndices[cObject]);
		}
		m_pMedHouseObject->AllUnlockBuffer();
	}
}

bool CHouseObject::LoadOut(char *strOutName)
{
	if(m_pOutHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pOutHouseObject);
		
	}
	m_pOutHouseObject = (CROSSM::CNMesh *)CSceneManager::_GetResource(CROSSM::RESOURCE_MESH,strOutName);

	return true;
}
bool CHouseObject::LoadIn(char *strInName)
{
	if(m_pInHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pInHouseObject);
		
	}
	m_pInHouseObject = (CROSSM::CNMesh *)CSceneManager::_GetResource(CROSSM::RESOURCE_MESH,strInName);
	return true;
}

bool CHouseObject::LoadMed(char *strMedName)
{
	if(m_pMedHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pMedHouseObject);
	}
	m_pMedHouseObject = (CROSSM::CNMesh *)CSceneManager::_GetResource(CROSSM::RESOURCE_MESH,strMedName);
	return true;

}

bool CHouseObject::LoadFlag(char *strFlagName)
{
	if(m_pFlagHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH, m_pFlagHouseObject);
	}
	m_pFlagHouseObject = (CROSSM::CNMesh *)CSceneManager::_GetResource(CROSSM::RESOURCE_MESH, strFlagName);
	return true;
}

bool CHouseObject::LoadOctree(char *strOct)
{
	//m_pOctree
	if(m_pOctree)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_OCTREESCENE,m_pOctree);	
	}
	m_pOctree = (COctreeScene *)CSceneManager::_GetResource(CROSSM::RESOURCE_OCTREESCENE,strOct);

	return true;

}
bool CHouseObject::LoadLightmap(char *strName)
{
	if(m_Lightmap)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_Lightmap);
	}

	m_Lightmap = (CROSSM::CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,strName);
	m_isHaveLightmap = true;
	return true;

}
bool CHouseObject::LoadInLightmap(char *strName)
{
	if(m_InLightmap)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_InLightmap);
	}

	m_InLightmap = (CROSSM::CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,strName);
	m_isHaveInLightmap = true;
	return true;


}
void CHouseObject::ReleaseOut()
{
	if(m_pOutHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pOutHouseObject);
		m_pOutHouseObject = NULL;

	}

}
void CHouseObject::ReleaseIn()
{
	if(m_pInHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pInHouseObject);
		m_pInHouseObject = NULL;

	}
}
void CHouseObject::ReleaseMed()
{
	if(m_pMedHouseObject)
	{

		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pMedHouseObject);
		m_pMedHouseObject = NULL;

	}

}

void CHouseObject::ReleaseFlag()
{
	if(m_pFlagHouseObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH, m_pFlagHouseObject);
		m_pFlagHouseObject = NULL;

	}
}

void CHouseObject::ReleaseOctree()
{
	if(m_pOctree)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_OCTREESCENE,m_pOctree);
		m_pOctree = NULL;
	}

}


void CHouseObject::RenderOctree(IDirect3DDevice8* pDevice)
{
	if (m_pOctree)
	{
		m_pOctree->RenderOctreeTriangle(pDevice);
	}
}