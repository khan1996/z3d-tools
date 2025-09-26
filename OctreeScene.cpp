// OctreeScene.cpp: implementation of the COctreeScene class.
//
//////////////////////////////////////////////////////////////////////

#include "OctreeScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

#define FLOAT_TO_INT(fValue) (*(int *)&(fValue))
#define IS_SIGNBIT(fValue) (FLOAT_TO_INT(fValue) & 0x80000000)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COctreeScene::COctreeScene()
{
	m_pOctree=NULL;
}

COctreeScene::~COctreeScene()
{
	for(int i=0;i<(int)m_PolyList.size();i++)
	{
	/*	if(m_PolyList[i])
		{
			CSceneManager::_ReleaseObj(CROSSM::POOL_POLYNODE,m_PolyList[i]);
			m_PolyList[i] = NULL;
		}*/
		delete m_PolyList[i];
	}
	m_PolyList.clear();

	//CSceneManager::_ReleaseObj(CROSSM::POOL_OCTREEOBJ,m_pOctree);
	

	if(m_pOctree)
	{
		delete m_pOctree;
		m_pOctree = NULL;
	}
}

void COctreeScene::AddPoly(vector3 *vecPoly)
{
	//PolyNode *pNode = (PolyNode *)CSceneManager::_CreateObj(CROSSM::POOL_POLYNODE);
	//pNode->Set(vecPoly);
	//m_PolyList.push_back(pNode);

	PolyNode *pNode=new PolyNode(vecPoly);
	m_PolyList.push_back(pNode);	
	
}

void COctreeScene::GenerateOctree()
{
	for(int cPolyNode=0;cPolyNode<(int)m_PolyList.size();cPolyNode++)
	{
		m_pOctree->m_PolyNodeID.push_back(cPolyNode);		
	}
	m_pOctree->CalcBoundBox(m_PolyList);
	m_pOctree->SplitNode(3,m_PolyList);
	//m_pOctree->CalcBoundBox();
	//m_pOctree->SplitNode(3);
}

void COctreeScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	m_pOctree->Render(pd3dDevice);
}

void COctreeScene::Save(FILE *fp)
{	
	int iCnt = m_PolyList.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cNode=0;cNode<iCnt;cNode++)
	{
		fwrite(m_PolyList[cNode]->m_vecPoly,sizeof(vector3)*3,1,fp);
	}	
	m_pOctree->Save(fp);
}

bool COctreeScene::Load()
{
	int cTotalNode=0;
	vector3 vecPoly[3];
	
	m_pByteDataObj->Read(&cTotalNode,sizeof(int),1);

	if(m_pOctree)
	{
		delete m_pOctree;
		m_pOctree = NULL;
	}
	m_pOctree=new COctree();

	for(int cNode = 0;cNode < cTotalNode;cNode++ )
	{
		m_pByteDataObj->Read(vecPoly,sizeof(vector3)*3,1);
		AddPoly(vecPoly);
	}
	m_pOctree->Load(m_pByteDataObj);
	
	return true;

}
bool COctreeScene::PostLoad()
{
	return true;

}

bool COctreeScene::Unload()
{
	return true;

}

void COctreeScene::Load(FILE *fp)
{
	int cTotalNode=0;
	vector3 vecPoly[3];
	
	if(m_pOctree)
	{
		delete m_pOctree;
		m_pOctree = NULL;
	}
	m_pOctree=new COctree();

	fread(&cTotalNode,sizeof(int),1,fp);
	for(int cNode=0;cNode<cTotalNode;cNode++)
	{
		fread(vecPoly,sizeof(vector3)*3,1,fp);		
		AddPoly(vecPoly);
	}
	m_pOctree->Load(fp);
}

void COctreeScene::CullSphere(vector3 vecCenter, float fRad,std::vector<PolyNode*> &CullPoly)
{
	std::vector<int> CullID;
	m_pOctree->CullSphere(vecCenter,fRad,m_PolyList,CullID);
	vector3 vecPoly[3];
	vector3 vecRadius=vector3(150.0f,200.0f,150.0);

	vector3 vecMaxRadius=vecCenter+vecRadius;
	vector3 vecMinRadius=vecCenter-vecRadius;	
	for(int cNode=0;cNode<(int)CullID.size();cNode++)
	{
		m_PolyList[CullID[cNode]]->m_used=0;

		vecPoly[0]=m_PolyList[CullID[cNode]]->m_vecPoly[0];
		vecPoly[1]=m_PolyList[CullID[cNode]]->m_vecPoly[1];
		vecPoly[2]=m_PolyList[CullID[cNode]]->m_vecPoly[2];

		if( vecPoly[0].x < vecMinRadius.x &&
			vecPoly[1].x < vecMinRadius.x &&
			vecPoly[2].x < vecMinRadius.x )
			continue;

		if( vecPoly[0].y < vecMinRadius.y &&
			vecPoly[1].y < vecMinRadius.y &&
			vecPoly[2].y < vecMinRadius.y)
			continue;

		if( vecPoly[0].z < vecMinRadius.z &&
			vecPoly[1].z < vecMinRadius.z &&
			vecPoly[2].z < vecMinRadius.z )
			continue;

		if( vecPoly[0].x > vecMaxRadius.x &&
			vecPoly[1].x > vecMaxRadius.x &&
			vecPoly[2].x > vecMaxRadius.x )
			continue;

		if( vecPoly[0].y > vecMaxRadius.y &&
			vecPoly[1].y > vecMaxRadius.y &&
			vecPoly[2].y > vecMaxRadius.y )
			continue;

		if( vecPoly[0].z > vecMaxRadius.z &&
			vecPoly[1].z > vecMaxRadius.z &&
			vecPoly[2].z > vecMaxRadius.z )
			continue;

		CullPoly.push_back(m_PolyList[CullID[cNode]]);
		
	}
}

void COctreeScene::CullFrustum(CViewCamera *pCamera,std::vector<PolyNode*> &CullPoly)
{
	std::vector<int> CullID;
	m_pOctree->CullFrustum(pCamera,m_PolyList,CullID);
	for(int cNode=0;cNode<(int)CullID.size();cNode++)
	{
		CullPoly.push_back(m_PolyList[CullID[cNode]]);
		m_PolyList[CullID[cNode]]->m_used=0;
	}
}

void COctreeScene::CullRay(vector3 vecStart, vector3 vecDir, float fLens,std::vector<PolyNode*> &CullPoly)
{
	std::vector<int> CullID;
	m_pOctree->CullRay(vecStart,vecDir,fLens,m_PolyList,CullID);
	for(int cNode=0;cNode<(int)CullID.size();cNode++)
	{
		CullPoly.push_back(m_PolyList[CullID[cNode]]);
		m_PolyList[CullID[cNode]]->m_used=0;
	}
		
}
bool COctreeScene::CollisionToRay(const D3DXVECTOR3 &vecStart,const D3DXVECTOR3 &vecDir,const float &fDist)
{

	if(!m_pOctree)	
		return false;	

	static float fNear,fFar;
	static COctree *pStack[64];

	if(m_pOctree->IntersectionRay(vecStart,vecDir,fNear,fFar) == -1)
		return 0;
	
	COctree *pCurrent;
	int iStack = 1;
	int i;
	pStack[0] = m_pOctree;
	while(iStack)
	{
		pCurrent = pStack[--iStack];
		if(pCurrent->m_PolyNodeID.size() == 0)
		{
			for(i = 0; i < 8;i++)
			{
				if(pCurrent->m_pChild[i] && pCurrent->m_pChild[i]->IntersectionRay(vecStart,vecDir,fNear,fFar) != -1)
					pStack[iStack++] = pCurrent->m_pChild[i];

			}
		}
		else
		{
			long iCurrentFaceId;
			for(i = 0; i < (int)pCurrent->m_PolyNodeID.size(); i++)
			{
				iCurrentFaceId = pCurrent->m_PolyNodeID[i];
				if(CollisionTriangleToRay(iCurrentFaceId,vecStart,vecDir,fDist))
				{
				
					return true;
				}
			}
		}
	}
	

	return false;


}
bool COctreeScene::CollisionTriangleToRay(long iFaceIndex,const D3DXVECTOR3 &vecStart,const D3DXVECTOR3 &vecDir,const float &fDist)
{
	D3DXVECTOR3 vecPos[3];
	D3DXVECTOR3 vecTmp1,vecTmp2;

	D3DXVECTOR3 vecNormal;	
	float ffDist;

	vecPos[0] = D3DXVECTOR3(m_PolyList[iFaceIndex]->m_vecPoly[0].x,m_PolyList[iFaceIndex]->m_vecPoly[0].y,m_PolyList[iFaceIndex]->m_vecPoly[0].z);
	vecPos[1] = D3DXVECTOR3(m_PolyList[iFaceIndex]->m_vecPoly[1].x,m_PolyList[iFaceIndex]->m_vecPoly[1].y,m_PolyList[iFaceIndex]->m_vecPoly[1].z);
	vecPos[2] = D3DXVECTOR3(m_PolyList[iFaceIndex]->m_vecPoly[2].x,m_PolyList[iFaceIndex]->m_vecPoly[2].y,m_PolyList[iFaceIndex]->m_vecPoly[2].z);
				
	vecTmp1 = vecPos[1] - vecPos[0];
	vecTmp2 = vecPos[2] - vecPos[0];
	D3DXVec3Cross(&vecNormal,&vecTmp1,&vecTmp2);

	D3DXVec3Normalize(&vecNormal,&vecNormal);
	
	ffDist = D3DXVec3Dot(&vecNormal,&(vecPos[0]));



	float fDot = D3DXVec3Dot(&vecNormal,&vecDir);

	if(IS_SIGNBIT(fDot) == 0)		// 노멀과 레이 방향이 같은 방향이거나 90도 이하일때 (BackFace Check)
		return false;


	float fffDist = (ffDist - D3DXVec3Dot(&vecNormal,&(vecStart))) / fDot;	// ray와 평면의 충돌 체크 (rayOrigin + dir * t 의 t를 구함)

	if(IS_SIGNBIT(fffDist) || (fffDist >= fDist))	//t가 음수이거나 광선의 길이를 넘었을때.	
		return false;
	
	D3DXVECTOR3 vec1 = vecStart + vecDir * fffDist;	//ve1 : intersect point.
	
	// Edge Normal 과 dot 해서 90 넘어가면 외부.

	D3DXVECTOR3 Edges[3];
	int i;	
	for(i = 0; i < 3; i++ )
	{
		D3DXVECTOR3 vecTmp = vecPos[(i + 1) % 3] - vecPos[i];
		D3DXVec3Cross(&(Edges[i]),&vecTmp,&vecNormal);
		D3DXVec3Normalize(&(Edges[i]),&(Edges[i]));

	}

	for(i = 0; i < 3; i++ )
	{
		if ((vec1.x - vecPos[i].x) * Edges[i].x + (vec1.y - vecPos[i].y) * Edges[i].y + (vec1.z - vecPos[i].z)* Edges[i].z > 0)
			return false;
	
	}
	return true;
	
}

void COctreeScene::RenderOctreeTriangle(IDirect3DDevice8* pDevice)
{
	if (m_PolyList.empty())
	{
		return;
	}

	if (m_PolyList.size()*3 != m_vecRenderVertex.size())
	{
		BuildRenderVertex();
	}
	
	DWORD dwLightingState, dwFill;
	pDevice->GetRenderState(D3DRS_LIGHTING, &dwLightingState);
	pDevice->GetRenderState(D3DRS_FILLMODE, &dwFill);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pDevice->LightEnable(0, FALSE);
	pDevice->LightEnable(1, FALSE);
	pDevice->LightEnable(2, FALSE);
	pDevice->LightEnable(3, FALSE);
	pDevice->LightEnable(4, FALSE);
	pDevice->SetTexture(0, NULL);
	pDevice->SetVertexShader(D3DFVF_XYZ);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecRenderVertex.size()/3, &(m_vecRenderVertex[0]), sizeof(vector3));
	pDevice->SetRenderState(D3DRS_FILLMODE, dwFill);
	pDevice->SetRenderState(D3DRS_LIGHTING, dwLightingState);
}

void COctreeScene::BuildRenderVertex()
{
	m_vecRenderVertex.resize(m_PolyList.size()*3);

	// m_vecRenderVertex 에 충분한 공간이 할당되지 못했을 경우를 대비해
	// m_PolyList.num 이 아니라 m_vecRenderVertex.size() 를 이용
	size_t polyCount = m_vecRenderVertex.size()/3;
	for (int i = 0; i < (int)polyCount; ++i)
	{
        m_vecRenderVertex[i*3] = m_PolyList[i]->m_vecPoly[0];
		m_vecRenderVertex[i*3 + 1] = m_PolyList[i]->m_vecPoly[1];
		m_vecRenderVertex[i*3 + 2] = m_PolyList[i]->m_vecPoly[2];
	}
}
