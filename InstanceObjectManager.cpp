// InstanceObjectManager.cpp: implementation of the CInstanceObjectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "InstanceObjectManager.h"
#include "SceneManager.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstanceObjectManager::CInstanceObjectManager()
{
	m_TotalInstanceObjectID=0;
}

CInstanceObjectManager::~CInstanceObjectManager()
{
	DeleteAllObject();
}

///// Minotaurs 추가
void CInstanceObjectManager::DeleteAllObject()
{
	m_InstanceObjectList.clear();
}

void CInstanceObjectManager::InstanceObjectRotation(int InstanceObjectID,float fDist)
{
	for(int i=0;i<(int)m_InstanceObjectList.size();i++)
	{
		if(m_InstanceObjectList[i].m_ID==InstanceObjectID)
		{
			m_InstanceObjectList[i].m_fAngle += fDist;
		}
	}
}

int CInstanceObjectManager::AddInstanceObject(char *strObjectName, vector3 vecPos, float fAngle)
{	
	CSceneManager::m_MeshObjectContainer.SetPath(INSTANCEOBJECTPATH);	
	InstanceObjectNode AddNode;
	AddNode.m_ID=m_TotalInstanceObjectID;
	AddNode.m_Color.c=0x0;
	if(strcmp(strObjectName,"")!=0)
	{
		AddNode.m_pMeshObject=CSceneManager::m_MeshObjectContainer.GetMeshObject(strObjectName);
		AddNode.m_pMeshObject->ConvertNormal();
	}
	else
	{
		AddNode.m_pMeshObject=NULL;
	}
	AddNode.m_vecPosition=vecPos;
	AddNode.m_fAngle = fAngle;
	strcpy(AddNode.m_strMeshName,strObjectName);
	m_InstanceObjectList.push_back(AddNode);
	m_TotalInstanceObjectID++;
	return AddNode.m_ID;
}

void CInstanceObjectManager::DeleteInstanceObject(int InstanceObjectID)
{
	for(int i=0;i<(int)m_InstanceObjectList.size();i++)
	{
		if(m_InstanceObjectList[i].m_ID==InstanceObjectID)
		{
			CSceneManager::m_MeshObjectContainer.DeleteMeshObject(m_InstanceObjectList[i].m_strMeshName);			

			m_InstanceObjectList.erase(m_InstanceObjectList.begin()+i);
			break;
		}
	}
}

void CInstanceObjectManager::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	//pd3dDevice->CaptureStateBlock(oldStateBlock);	
	//pd3dDevice->ApplyStateBlock(m_ObjectRenderStateBlock);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);  

	
	pd3dDevice->LightEnable(0,TRUE);
	
	pd3dDevice->LightEnable(1,FALSE);
	
	pd3dDevice->LightEnable(2,FALSE);
	
	pd3dDevice->LightEnable(3,FALSE);
	
	pd3dDevice->LightEnable(4,FALSE);
	
	pd3dDevice->LightEnable(5,FALSE);
	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);



	/////
	pd3dDevice->LightEnable(0,TRUE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);

	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	D3DMATERIAL8 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	pd3dDevice->SetMaterial( &mtrl );

	pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);	

	matrix matTrans, matRotation;
	matRotation.MakeIdent();

	CSceneNode SceneNode;
	SceneNode.m_fRad=200.0f;
	float w;
	vector3 vecResult;
	D3DLIGHT8 ColorLight=CSceneManager::m_WeatherManager.m_SunLight;

	for(int cObject=0;cObject<(int)m_InstanceObjectList.size();++cObject)
	{		
		matTrans.Translation(m_InstanceObjectList[cObject].m_vecPosition);

		///// Minotaurs 추가
		matRotation.Rotation(vector3(0.f,1.f,0.f),m_InstanceObjectList[cObject].m_fAngle);//Minotaurs instanceObject
		matTrans = matRotation * matTrans;

		SceneNode.m_AccumulateTM=matTrans;
		if(SceneNode.isCulling())
		{
			if(m_InstanceObjectList[cObject].m_Color.c!=0x0)
			{
				ColorLight.Diffuse.r=(float)m_InstanceObjectList[cObject].m_Color.r/255.0f;
				ColorLight.Diffuse.g=(float)m_InstanceObjectList[cObject].m_Color.g/255.0f;;
				ColorLight.Diffuse.b=(float)m_InstanceObjectList[cObject].m_Color.b/255.0f;;				
				pd3dDevice->SetRenderState(D3DRS_AMBIENT,m_InstanceObjectList[cObject].m_Color.c);
				pd3dDevice->SetLight(0,&ColorLight);
			}
			else
			{
				//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);
				//pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);

				pd3dDevice->SetRenderState(D3DRS_AMBIENT,0xffcccccc);
				pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
			}
			pd3dDevice->SetTransform(D3DTS_WORLD,matTrans);
			BaseGraphicsLayer::TransformVector(m_InstanceObjectList[cObject].m_vecPosition,vecResult,w);
			m_InstanceObjectList[cObject].m_ptMousePick.x=(int)vecResult.x;
			m_InstanceObjectList[cObject].m_ptMousePick.y=(int)vecResult.y;
			if(m_InstanceObjectList[cObject].m_pMeshObject)
				m_InstanceObjectList[cObject].m_pMeshObject->Render(pd3dDevice);
		}
		else
		{
			m_InstanceObjectList[cObject].m_ptMousePick.x=m_InstanceObjectList[cObject].m_ptMousePick.y=-1;
		}
	}	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	//pd3dDevice->ApplyStateBlock(oldStateBlock);
}

void CInstanceObjectManager::Create(LPDIRECT3DDEVICE8 pd3dDevice)
{
/*	pd3dDevice->CreateStateBlock(D3DSBT_ALL,&m_ObjectRenderStateBlock);

	pd3dDevice->BeginStateBlock();
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
	
	pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);  

	
	pd3dDevice->LightEnable(0,TRUE);
	
	pd3dDevice->LightEnable(1,FALSE);
	
	pd3dDevice->LightEnable(2,FALSE);
	
	pd3dDevice->LightEnable(3,FALSE);
	
	pd3dDevice->LightEnable(4,FALSE);
	
	pd3dDevice->LightEnable(5,FALSE);
	
	pd3dDevice->LightEnable(6,FALSE);	
	pd3dDevice->LightEnable(7,FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	/*
	pd3dDevice->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_AmbientColor->c);
	*/
//	pd3dDevice->EndStateBlock(&m_ObjectRenderStateBlock);
}

int CInstanceObjectManager::MouseOnCheckInstanceObject(int px, int py)
{
	for(int i=0;i<(int)m_InstanceObjectList.size();i++)
	{
		if(m_InstanceObjectList[i].m_ptMousePick.x!=-1)
		{
			if( m_InstanceObjectList[i].m_ptMousePick.x > px-30 &&
				m_InstanceObjectList[i].m_ptMousePick.x < px+30 &&
				m_InstanceObjectList[i].m_ptMousePick.y > py-30 &&
				m_InstanceObjectList[i].m_ptMousePick.y < py+30)
			{
				return m_InstanceObjectList[i].m_ID;
			}
		}
	}
	return -1;
}

void CInstanceObjectManager::SetInstanceObjectColor(int ID, color Color)
{
	for(int i=0;i<(int)m_InstanceObjectList.size();i++)
	{
		if(m_InstanceObjectList[i].m_ID==ID)
		{
			m_InstanceObjectList[i].m_Color=Color;
			break;
		}
	}
}

void CInstanceObjectManager::SetAllObjectColor(color Color)
{
	for(int i=0;i<(int)m_InstanceObjectList.size();i++)
	{	
		m_InstanceObjectList[i].m_Color=Color;		
	}
}
