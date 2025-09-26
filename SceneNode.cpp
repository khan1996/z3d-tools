// SceneNode.cpp: implementation of the CSceneNode class.
//
//////////////////////////////////////////////////////////////////////

#include "SceneNode.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneNode::CSceneNode()
{
	m_pChild=m_pNext=0;
}

CSceneNode::~CSceneNode()
{

}

bool CSceneNode::isCulling()
{		
	/*
	vector3 vecInter=m_vecMaxBox-m_vecMinBox;

	matrix matTrans;
	
	vector3 vecMiddlePos=m_vecMinBox+m_vecMaxBox;
	vecMiddlePos*=0.5f;
	vector3 vecRad=m_vecMaxBox-vecMiddlePos;
	float fRad=vecRad.GetLens();
	
	matTrans.Translation(vecMiddlePos);
	matTrans=m_AccumulateTM*matTrans;
	vecMiddlePos=matTrans.GetLoc();	
	*/	
	/*
	matrix matTrans;
	matTrans.Translation(m_vecMiddle);
	matTrans=m_AccumulateTM*matTrans;
	vector3 vecPos=matTrans.GetLoc();
	*/
	/*
	CSceneManager::GetCamera()->m_vecFrustumNormal[0];	
	m_vecFrustumLeft;
	CSceneManager::GetCamera()->m_vecFrustumNormal[1];
	m_vecFrustumRight;
	CSceneManager::GetCamera()->m_vecFrustumNormal[2];

	CSceneManager::GetCamera()->m_vecFrustumTop
		CSceneManager::GetCamera()->m_vecFrustumBottom
		*/


	vector3 vecPos;//=m_AccumulateTM.GetLoc();
	vecPos.x=m_AccumulateTM._41;
	vecPos.y=m_AccumulateTM._42;
	vecPos.z=m_AccumulateTM._43;
	/*

	CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[2],CSceneManager::GetCamera()->m_vecFrustumLeft[0]);
	CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumLeft);	

	CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[3],CSceneManager::GetCamera()->m_vecFrustumRight[0]);
	CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumRight);

	CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[0],CSceneManager::GetCamera()->m_vecFrustumTop[0]);
	CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumTop);

	CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[1],CSceneManager::GetCamera()->m_vecFrustumBottom[0]);
	CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumBottom);
	*/

	if( CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[2],CSceneManager::GetCamera()->m_vecFrustumLeft[0])<0 )	
		return false;		
	if( CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[3],CSceneManager::GetCamera()->m_vecFrustumRight[0])<0)
		return false;	
	if( CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[0],CSceneManager::GetCamera()->m_vecFrustumTop[0]) < 0)
		return false;	
	if( CIntersection::PlaneSphereNormal(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumNormal[1],CSceneManager::GetCamera()->m_vecFrustumBottom[0]) < 0)
		return false;						
	return true;
	/*


	if( CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumLeft)<0 )	
		return false;	
	if( CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumRight) < 0)
		return false;
	if( CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumTop) < 0)
		return false;
	if( CIntersection::PlaneSphere(vecPos,m_fRad,CSceneManager::GetCamera()->m_vecFrustumBottom) < 0)
		return false;				
	return true;
	*/
}
void CSceneNode::Init()
{
	if( m_vecMaxBox.GetLens() >= m_vecMinBox.GetLens() )	
		m_fRad=m_vecMaxBox.GetLens();	
	else	
		m_fRad=m_vecMinBox.GetLens();	
	/*
	vector3 vecInter=m_vecMaxBox-m_vecMinBox;
	m_vecMiddle=m_vecMinBox+m_vecMaxBox;
	m_vecMiddle*=0.5f;
	vector3 vecRad=m_vecMaxBox-m_vecMiddle;
	m_fRad=vecRad.GetLens();	
	*/
}

bool CSceneNode::isAABBCulling()
{
	vector3 vecPos=m_AccumulateTM.GetLoc();
	vector3 vecMaxBox=m_vecMaxBox+vecPos;
	vector3 vecMinBox=m_vecMinBox+vecPos;

	if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
	{
		return true;
	}
	return false;
}
