// CollisionDetection.cpp: implementation of the CCollisionDetection class.
//
//////////////////////////////////////////////////////////////////////

#include <MathUtil.h>
#include <Vector3.h>

#include "CollisionDetection.h"
#include "SceneManager.h"
#include "HeightFieldScene.h"
#include "GMMemory.h"
#include "BaseGraphicsLayer.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100
//Check!!
DWORD ClassifyPoint(vector3 point,vector3 p0,vector3 pN)
{
	vector3 dir=p0-point;
	float d=dir*pN;
	if(d<-0.001f)
		return PLANE_FRONT;
	else
		if(d>0.001f)
			return PLANE_BACKSIDE;
	return ON_PLANE;
}
//Check!!
float IntersectRaySphere(vector3 rO,vector3 rV,vector3 sO,float sR)
{
	vector3 Q=sO-rO;

	float c=Q.GetLens();
	float v=Q*rV;
	float d=sR*sR - (c*c-v*v);

	if(d<0.0f)return (-1.0f);

	return v-sqrtf(d);
}
//Check!!
float IntersectRayPlane(vector3 rOrigin,vector3 rVector,vector3 pOrigin,vector3 pNormal)
{
	float d=-(pNormal*pOrigin);

	float numer=(pNormal*rOrigin)+d;
	float denom=pNormal*rVector;

	if(denom==0.0f)
		return -1.0f;

	return -(numer/denom);
}
//Check!!

bool CheckPointInTriangle(vector3 point,vector3 a,vector3 b,vector3 c)
{
	float fTotal_Angle=0.0f;
	vector3 v1=point-a;
	vector3 v2=point-b;
	vector3 v3=point-c;
	
	v1.Normalize();
	v2.Normalize();
	v3.Normalize();

	fTotal_Angle+=acos(v1*v2);
	fTotal_Angle+=acos(v2*v3);
	fTotal_Angle+=acos(v3*v1);

	if(fabs(fTotal_Angle-2.0f*3.14159f) <= 0.005f)
		return true;
	return false;
}
//Check!!
vector3 ClosestPointOnLine(vector3 &a,vector3 &b,vector3 &p)
{
	vector3 c=p-a;
	vector3 V=b-a;

	float d=V.GetLens();

	V.Normalize();

	float t=V*c;	
	if(t<0.0f)return a;
	if(t>d)return b;

	V=V*t;	
	return a+V;
}
//Check!!
vector3 ClosestPointOnTriangle(vector3 a,vector3 b,vector3 c,vector3 p)
{		
	vector3 Rab=ClosestPointOnLine(a,b,p);
	vector3 Rbc=ClosestPointOnLine(b,c,p);
	vector3 Rca=ClosestPointOnLine(c,a,p);

	vector3 vecForLens;
	vecForLens=p-Rab;
	float dAB=vecForLens.GetLens();

	vecForLens=p-Rbc;
	float dBC=vecForLens.GetLens();

	vecForLens=p-Rca;
	float dCA=vecForLens.GetLens();

	float min=dAB;
	vector3 result =Rab;

	if( dBC < min )
	{
		min=dBC;
		result=Rbc;
	}
	if( dCA < min )
	{
		result=Rca;
	}	
	return result;
}

CCollisionDetection::CCollisionDetection()
{
	m_vecRadius.x=40.0f;
	m_vecRadius.y=85.0f;
	m_vecRadius.z=40.0f;

	m_vecMinMove=vector3(-10000000.0f,0.0f,-10000000.0f);
	m_vecMaxMove=vector3(10000000.0f,0.0f,10000000.0f);
	m_fMinYBound = -123.0f;
	m_fMaxYBound = -123.0f;
    m_bFirstBottom = false;
	m_bFirstScene = true;
	m_bBuild = true;

	m_nLockCount = 0;
}

CCollisionDetection::~CCollisionDetection()
{

}


void CCollisionDetection::CheckFirstHouseCollision()             // FirstBottom
{

	vector3 p[3];
	vector3 vecNormal;
	vector3 v1,v2;	

	float fHeight=m_HeightField->GetHeight(m_vecSourcePoint);
	if(fHeight+m_vecRadius.y > m_vecSourcePoint.y)
	{
		CCollisionList AddNode;
		AddNode.m_CollisionType=CT_TERRAIN;
		AddNode.m_vecIntersectionPoint=vector3(m_vecSourcePoint.x,fHeight,m_vecSourcePoint.z);
		AddNode.m_fDistance=abs(fHeight-m_vecSourcePoint.y);
		m_CollisionList.push_back(AddNode);
	}	

	
	//*
    //int iAlwaysTest[3];
    int iCurrentSectorIndex = -1;
    int cSector;

	for(cSector=0;cSector<m_HeightField->GetLSizeX()*m_HeightField->GetLSizeY();cSector++)
	{
        bool bCollision=true;

		// alias
		CSectorScene &currentSector = m_HeightField->m_SectorScene[cSector];
		
		if(bCollision)
		{

			for(int cHouse=0;cHouse<(int)currentSector.m_HouseObjectNode.size();cHouse++)
			{
				vector3 vecHousePos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc();
				vector3 vecHouseInterLens=vecHousePos-m_vecSourcePoint;

				// alias
				CHouseObject* pCurrentHouseObject = currentSector.m_HouseObjectNode[cHouse]->m_HouseObject;

				if( (pCurrentHouseObject->m_pOctree &&
					pCurrentHouseObject->m_pOctree->IsLoaded() && 
					 vecHouseInterLens.GetLens() < currentSector.m_HouseObjectNode[cHouse]->m_fRad) ||					 

					(pCurrentHouseObject->m_pOctree &&			
					pCurrentHouseObject->m_pOctree->IsLoaded() &&
					pCurrentHouseObject->m_pOutHouseObject==NULL &&
					 vecHouseInterLens.GetLens() < 1000.0f))
				{				
					matrix matInv;
					matInv.Inverse(currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM);

					matrix matInHousePos;
					matInHousePos.Translation(m_vecSourcePoint);
					matInHousePos=matInHousePos*matInv;
					vector3 vecInHousePos=matInHousePos.GetLoc();

					vector3 vecSource=matInHousePos.GetLoc();										
					matrix matInvRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;				
					matInvRot._41=0.0f;
					matInvRot._42=0.0f;
					matInvRot._43=0.0f;
					matInv.Inverse(matInvRot);
					
					vector3 vecVelocity=m_vecVelocity.Normalized();
					matrix matInHouseRot;
					matInHouseRot.Translation(vecVelocity);
					matInHouseRot=matInHouseRot*matInv;
					vecVelocity=matInHouseRot.GetLoc();

					std::vector<PolyNode*> CollPoly;
					//pCurrentHouseObject->m_pOctree->m_pOctree->CullSphere(vecInHousePos,85.0f,CollPoly);
					pCurrentHouseObject->m_pOctree->CullSphere(vecInHousePos,585.0f,CollPoly);				
					
					vector3 vecCheckDir;
					float fAngle;

					for(int cPoly=0;cPoly<(int)CollPoly.size();cPoly++)
					{
						CollPoly[cPoly]->m_used=0;
						p[0]=(CollPoly[cPoly]->m_vecPoly[0]);
						p[1]=(CollPoly[cPoly]->m_vecPoly[1]);
						p[2]=(CollPoly[cPoly]->m_vecPoly[2]);		
						
						v1=p[1]-p[0];
						v2=p[2]-p[1];
						vecNormal=v1^v2;
						vecNormal.Normalize();
						float fIntersection;					
						// 밑에 검사					
						

						vecCheckDir.x=0.0f;
						vecCheckDir.y=1.0f;
						vecCheckDir.z=0.0f;
						fAngle=vecCheckDir*vecNormal;

						if(vecNormal==vecCheckDir)
							fAngle=1.0f;
						//	fAngle=3.14159f;
						//if(fAngle < 0.0f)
						//	fAngle=-fAngle;



						if(fAngle > 0.5f)
						{
							if(CIntersection::PointFromPlane(vecNormal,p[0],vecSource)>=m_vecRadius.y)
								continue;

							if(CIntersection::PolygonRay(vecSource,vecSource+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													 p,
													 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_BOTTOM;
								m_CollisionList.push_back(AddNode);
								continue;
							}
							
							if(CIntersection::PolygonRay(vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x,
														 vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													 p,
													 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_BOTTOM;
								m_CollisionList.push_back(AddNode);
								continue;
							}
							if(CIntersection::PolygonRay(vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x,
														 vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													 p,
													 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_BOTTOM;
								m_CollisionList.push_back(AddNode);
								continue;
							}
							if(CIntersection::PolygonRay(vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x,
														 vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													 p,
													 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_BOTTOM;
								m_CollisionList.push_back(AddNode);
								continue;
							}
							if(CIntersection::PolygonRay(vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x,
														 vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													 p,
													 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_BOTTOM;
								m_CollisionList.push_back(AddNode);
								continue;
							}
						}
						else
						{
							if(CIntersection::PointFromPlane(vecNormal,p[0],vecSource)>=m_vecRadius.x)
								continue;

							float fRate=vecVelocity*vecNormal;
							//if(fRate>= 0.0f)
							//	continue;

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(0.0f,1.0f,0.0f)*m_vecRadius.y,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,1.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}		

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(0.707f,0.0f,0.707f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.707f,0.0f,0.707f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;

								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();

								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(-0.707f,0.0f,0.707f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(-0.707f,0.0f,0.707f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();

 								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(0.707f,0.0f,-0.707f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.707f,0.0f,-0.707f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(-0.707f,0.0f,-0.707f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(-0.707f,0.0f,-0.707f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(1.0f,0.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}
							
							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(-1.0f,0.0f,0.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,0.0f,1.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}

							if( CIntersection::PolygonRay(vecSource,
														 vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x,
														 p,
														 fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;
								
								matInHousePos.Translation(vecSource+vector3(0.0f,0.0f,-1.0f)*fIntersection);
								matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
								
								matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
								matInHousePos.Translation(vecNormal);
								matInHousePos=matInHousePos*matInHouseRot;
								AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
								AddNode.m_CollisionType=CT_SIDE;
								matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
								matInHousePos.Translation(p[0]);
								matWorldPos=matInHousePos*matWorldPos;
								AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
								m_CollisionList.push_back(AddNode);
								continue;
							}								
						}					
					}
				}
			}
		}
	}		
	//*/
}



void CCollisionDetection::CheckHouseCollision()
{		
	vector3 p[3];
	vector3 vecNormal;
	vector3 v1,v2;	

	float fHeight=m_HeightField->GetHeight(m_vecSourcePoint);
	if(fHeight+m_vecRadius.y > m_vecSourcePoint.y)
	{
		CCollisionList AddNode;
		AddNode.m_CollisionType=CT_TERRAIN;
		AddNode.m_vecIntersectionPoint=vector3(m_vecSourcePoint.x,fHeight,m_vecSourcePoint.z);
		AddNode.m_fDistance=abs(fHeight-m_vecSourcePoint.y);
		m_CollisionList.push_back(AddNode);
	}	

	
    for(int cSector=0;cSector<m_HeightField->GetLSizeX()*m_HeightField->GetLSizeY();cSector++)
	{
		bool bCollision=false;        

		// aliases
		CSectorScene &currentSector = m_HeightField->m_SectorScene[cSector];
		float& currentSectorPosX = currentSector.m_AccumulateTM._41;
		float& currentSectorPosZ = currentSector.m_AccumulateTM._43;

		if(m_CollisionDetectionDetail)
		{
			// detailed collision detection 의 경우 : 중심섹터 상하좌우 포함 총 9섹터 체크
			if (IsPointInRect2D(m_vecSourcePoint.x, m_vecSourcePoint.z,
								currentSectorPosX-SECTORSIZE, m_vecSourcePoint.z-SECTORSIZE,
								SECTORSIZE*3, SECTORSIZE*3))
			{
				bCollision=true;
			}
		}
		else
		{
			// detailed 아닐 경우 : 중심섹터만 체크
			if (IsPointInRect2D(m_vecSourcePoint.x, m_vecSourcePoint.z,
								currentSectorPosX, currentSectorPosZ, SECTORSIZE, SECTORSIZE))
			{
				bCollision=true;
			}

		}

		if(false == bCollision)
		{
			continue;	// 현재 섹터는 범위 체크에서 탈락 -> 충돌처리 건너뜀
		}
		
		// 섹터내 각 하우스에 대해
		for(int cHouse=0;cHouse<(int)currentSector.m_HouseObjectNode.size();cHouse++)
		{
			// alias
			CHouseObject* pCurrentHouseObject = currentSector.m_HouseObjectNode[cHouse]->m_HouseObject;

			// 하우스의 위치기준점
			vector3 vecHousePos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM.GetLoc();
			// 충돌체크점으로부터 하우스위치로의 방향벡터
			vector3 vecHouseInterLens=vecHousePos-m_vecSourcePoint;

if(strstr(currentSector.m_HouseObjectNode[cHouse]->m_strInName,"INNERCASTLE_IN.R3S"))
{
	int a = 10;
}

//			if(vecHouseInterLens.GetLens() < 500.0f)
//			{
//				m_bBuild = false;
//			}
// <-- 일단 이 문제는 아닌거같다. 같은 높이에서도 옆으로 움직이면 사라지는걸 봐선 비슷한 문제인거 같은데
//		사라지고나서 다시 가운데로 돌아와도 다시 안 나타난단 말이지.. 이게 문제-_-
			
			if( (pCurrentHouseObject->m_pOctree &&
				pCurrentHouseObject->m_pOctree->IsLoaded() &&
					vecHouseInterLens.GetLens() < currentSector.m_HouseObjectNode[cHouse]->m_fRad) ||					 

				(pCurrentHouseObject->m_pOctree &&
				pCurrentHouseObject->m_pOctree->IsLoaded() &&
					pCurrentHouseObject->m_pOutHouseObject==NULL &&
					vecHouseInterLens.GetLens() < 1000.0f))
			{	
				
				matrix matInv;
				matInv.Inverse(currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM);

				matrix matInHousePos;
				matInHousePos.Translation(m_vecSourcePoint);
				matInHousePos=matInHousePos*matInv;
				vector3 vecInHousePos=matInHousePos.GetLoc();

				vector3 vecSource=matInHousePos.GetLoc();										
				matrix matInvRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;				
				matInvRot._41=0.0f;
				matInvRot._42=0.0f;
				matInvRot._43=0.0f;
				matInv.Inverse(matInvRot);
				
				vector3 vecVelocity=m_vecVelocity.Normalized();
				matrix matInHouseRot;
				matInHouseRot.Translation(vecVelocity);
				matInHouseRot=matInHouseRot*matInv;
				vecVelocity=matInHouseRot.GetLoc();

				std::vector<PolyNode*> CollPoly;
				//pCurrentHouseObject->m_pOctree->m_pOctree->CullSphere(vecInHousePos,85.0f,CollPoly);
				pCurrentHouseObject->m_pOctree->CullSphere(vecInHousePos,585.0f,CollPoly);				
				
				if(m_bBuild)	// house bound box 안에 있을경우.
				{

					vector3 vecMin;
					vector3 vecMax;

					if(pCurrentHouseObject)
					{
						pCurrentHouseObject->CalcBox(vecMin,vecMax,0);

						if( (vecSource.x >= vecMin.x) && (vecSource.y >= vecMin.y) && (vecSource.z >= vecMin.z) )
						{
							if( (vecSource.x <= vecMax.x) && (vecSource.y <= vecMax.y) && (vecSource.z <= vecMax.z) )
							{		
								m_bBuild = false;
							}

						}

					}

				}
				// 내부씬일때 외부신 컬링. 하드 코딩,'innercastle_in.r3s'
				if(strstr(currentSector.m_HouseObjectNode[cHouse]->m_strInName,"INNERCASTLE_IN.R3S"))
				{
					bool bFlag = false;
					vector3 vecMin;
					vector3 vecMax;
					if(pCurrentHouseObject)
					{
						// Get InHouse BoundBox
						pCurrentHouseObject->CalcBox(vecMin,vecMax,1);

						CrossM::Math::VECTOR3 vSource, vMin, vMax;
						ConvVector3(vSource, vecSource);
						ConvVector3(vMin, vecMin);
						ConvVector3(vMax, vecMax);
						if (CrossM::Math::IsPointInAABB(vSource, vMin, vMax))
						{
							// 내부라면.
							CSceneManager::SetInViewHouseScene(currentSector.m_HouseObjectNode[cHouse]);
							CSceneManager::SetInView(true);
							bFlag = true;
						}

						BaseGraphicsLayer::SetCustomMsg(0, currentSector.m_HouseObjectNode[cHouse]->m_strInName);
						BaseGraphicsLayer::SetCustomMsg(1, currentSector.m_HouseObjectNode[cHouse]->m_strMedName);
						BaseGraphicsLayer::SetCustomMsg(2, currentSector.m_HouseObjectNode[cHouse]->m_strOutName);
					}
					if(!bFlag)
					{
						CSceneManager::SetInViewHouseScene(NULL);
						CSceneManager::SetInView(false);

						BaseGraphicsLayer::SetCustomMsg(0, "");
						BaseGraphicsLayer::SetCustomMsg(1, "");
						BaseGraphicsLayer::SetCustomMsg(2, "");
					}

	
				}
				vector3 vecCheckDir;
				float fAngle;

				for(int cPoly=0;cPoly<(int)CollPoly.size();cPoly++)
				{
					CollPoly[cPoly]->m_used=0;
					p[0]=(CollPoly[cPoly]->m_vecPoly[0]);
					p[1]=(CollPoly[cPoly]->m_vecPoly[1]);
					p[2]=(CollPoly[cPoly]->m_vecPoly[2]);		
					
					v1=p[1]-p[0];
					v2=p[2]-p[1];
					vecNormal=v1^v2;
					vecNormal.Normalize();
					float fIntersection;					
					// 밑에 검사					
					

					vecCheckDir.x=0.0f;
					vecCheckDir.y=1.0f;
					vecCheckDir.z=0.0f;
					fAngle=vecCheckDir*vecNormal;

					if(vecNormal==vecCheckDir)
						fAngle=1.0f;
					//	fAngle=3.14159f;
					//if(fAngle < 0.0f)
					//	fAngle=-fAngle;



					if(fAngle > 0.5f)
					{
						if(CIntersection::PointFromPlane(vecNormal,p[0],vecSource)>=m_vecRadius.y)
							continue;

						if(CIntersection::PolygonRay(vecSource,vecSource+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													p,
													fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_BOTTOM;
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}
						
						if(CIntersection::PolygonRay(vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x,
														vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													p,
													fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_BOTTOM;
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;

							continue;
						}
						if(CIntersection::PolygonRay(vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x,
														vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													p,
													fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_BOTTOM;
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}
						if(CIntersection::PolygonRay(vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x,
														vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													p,
													fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_BOTTOM;
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}
						if(CIntersection::PolygonRay(vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x,
														vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
													p,
													fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,-1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_BOTTOM;
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}
					}
					else
					{
						if(CIntersection::PointFromPlane(vecNormal,p[0],vecSource)>=m_vecRadius.x)
							continue;

						float fRate=vecVelocity*vecNormal;
						//if(fRate>= 0.0f)
						//	continue;

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(0.0f,1.0f,0.0f)*m_vecRadius.y,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,1.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}		

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(0.707f,0.0f,0.707f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.707f,0.0f,0.707f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;

							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();

							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);

							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(-0.707f,0.0f,0.707f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(-0.707f,0.0f,0.707f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();

 							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);

							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(0.707f,0.0f,-0.707f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.707f,0.0f,-0.707f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);

							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(-0.707f,0.0f,-0.707f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(-0.707f,0.0f,-0.707f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(1.0f,0.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}
						
						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(-1.0f,0.0f,0.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,0.0f,1.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}

						if( CIntersection::PolygonRay(vecSource,
														vecSource+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x,
														p,
														fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;
							
							matInHousePos.Translation(vecSource+vector3(0.0f,0.0f,-1.0f)*fIntersection);
							matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
							
							matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
							matInHousePos.Translation(vecNormal);
							matInHousePos=matInHousePos*matInHouseRot;
							AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
							AddNode.m_CollisionType=CT_SIDE;
							matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
							matInHousePos.Translation(p[0]);
							matWorldPos=matInHousePos*matWorldPos;
							AddNode.m_vecPlanePoint=matWorldPos.GetLoc();
							m_CollisionList.push_back(AddNode);
							//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
							m_bBuild = false;
							continue;
						}								
					}					
				}
			}
		}
	}		
	//*/
}

vector3 CCollisionDetection::GetPosition(vector3 vecPos, vector3 vecVelocity,vector3 vecBeforePos,CollisionType &CT,bool ChrCollision)
{
	if( m_nLockCount > 0 )
	{
		return vecPos;
	}

	vector3 vecScaledPosition,vecScaledVelocity;
	vector3 vecFinalPosition;

	//vecVelocity.y=-20.0f;

	m_CollisionDetectionDetail=ChrCollision;
	vecScaledPosition=vecPos;
	vecScaledVelocity=vecVelocity;
	m_vecSourcePoint=vecPos;
	m_vecVelocity=vecVelocity;	 
	
	bool bOutMovePos=false;

//	CSceneManager::m_CharacterManager.SetChrBuildEnable(true);
	m_bBuild = true;

	if(vecPos.x < m_vecMinMove.x)
	{
		vecPos.x=m_vecMinMove.x;
		vecPos.y = CSceneManager::m_HeightField.GetHeight(vecPos) + 30;
		bOutMovePos=true;
	}
	if(vecPos.z < m_vecMinMove.z)
	{
		vecPos.z=m_vecMinMove.z;
		vecPos.y = CSceneManager::m_HeightField.GetHeight(vecPos) + 30;
		bOutMovePos=true;
	}

	if(vecPos.x > m_vecMaxMove.x)
	{
		vecPos.x=m_vecMaxMove.x;
		vecPos.y = CSceneManager::m_HeightField.GetHeight(vecPos) + 30;
		bOutMovePos=true;
	}

	if(vecPos.z > m_vecMaxMove.z)
	{
		vecPos.z=m_vecMaxMove.z;
		vecPos.y = CSceneManager::m_HeightField.GetHeight(vecPos) + 30;
		bOutMovePos=true;
	}
	if(bOutMovePos)
	{
		
		//CSceneManager::m_CharacterManager.SetChrBuildEnable(m_bBuild);

		return vecPos; 
	}
//	vector3 m_vecMinMove,m_vecMaxMove;

	vector3 vecMovePoint=CollideWithWorld(m_vecSourcePoint, m_vecVelocity,vecBeforePos,0,CT,ChrCollision);

	if(CheckSectorWater(vecMovePoint,false))
	{
		CT=CT_WATER;
	}

	if(!CSceneManager::GetInView())
		CheckSectorHeightCollision(CT);
	/*
	m_vecSourcePoint.x=m_vecSourcePoint.x/m_vecRadius.x;
	m_vecSourcePoint.y=m_vecSourcePoint.y/m_vecRadius.y;
	m_vecSourcePoint.z=m_vecSourcePoint.z/m_vecRadius.z;

	m_vecVelocity.x=m_vecVelocity.x/m_vecRadius.x;
	m_vecVelocity.y=m_vecVelocity.y/m_vecRadius.y;
	m_vecVelocity.z=m_vecVelocity.z/m_vecRadius.z;	
	*/
	CT=CT_NONE;

	/*vector3 */vecMovePoint=CollideWithWorld(m_vecSourcePoint, m_vecVelocity,vecBeforePos,0,CT,ChrCollision);
	/*
	if(CT==CT_CHRSIDE)
	{
		return vecPos;
	}
	*/
	/*
	if(CT==CT_TERRAIN)
	{

		List<vector3> vecHeightList;

		m_HeightField->GetHeightFieldShadowPoly(vecMovePoint,vecHeightList);
		
		vector3 v[3];
		vector3 vecNormal,vecCompareNormal;
		float fLens;
		float fAngle;

		vecCompareNormal=vector3(0.0f,1.0f,0.0f);
		

		for(int cPoly=0;cPoly<vecHeightList.num/3;cPoly++)
		{
			v[0]=vecHeightList[cPoly*3+0];
			v[1]=vecHeightList[cPoly*3+1];
			v[2]=vecHeightList[cPoly*3+2];

			if(CIntersection::PolygonRay(vector3(vecMovePoint.x,0.0f,vecMovePoint.z),vector3(vecMovePoint.x,120000.0f,vecMovePoint.z),v,fLens))
			{
				vecNormal=(v[1]-v[0])^(v[2]-v[1]);
				vecNormal.Normalize();
				fAngle=vecNormal*vecCompareNormal;
				fAngle=acosf(fAngle);
				if(fAngle> ((3.14159/180.0f)*35.0f))
				{
					return vecPos;
				}
			}
		}
	}
	*/
	/*
	bool bUpper=false;	
	if(vecPos.y<vecMovePoint.y)
	{
		if(!CheckSectorWater(vecMovePoint,true))
			return vecPos;
	}
	else
	{
		if(!CheckSectorWater(vecMovePoint,false))
			return vecPos;
	}
	*/
	/*
	if(vecMovePoint.y <= 100.0f)
	{
		float fHeight=m_HeightField->GetHeight(vecPos);
		return vector3(vecPos.x,fHeight+m_vecRadius.y,vecPos.z);
		//return vecPos;
	}
		//vecMovePoint.y=100.0f;
		*/

	if(CheckSectorWater(vecMovePoint,false))
	{
		CT=CT_WATER;
	}
	ZoneBoundApply(vecMovePoint,CT);			// Zone Y Bound 적용
	// epsilon 적용.벌벌 현상 수정.
	vector3 vecEpsilon = vecMovePoint - vecBeforePos;
	if(fabs(vecEpsilon.x + vecEpsilon.z) <= 1.0)
		if((vecEpsilon.GetLens() <= 10.0f) &&(CT == CT_NONE || CT == CT_SIDE || CT == CT_BOTTOM))
		{
			//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
			//m_bBuild = false;
			//CSceneManager::m_CharacterManager.SetChrBuildEnable(m_bBuild);
			return vecBeforePos;
		}
/**/
//	CSceneManager::m_CharacterManager.SetChrBuildEnable(m_bBuild);
	return vecMovePoint; 
}

vector3 CCollisionDetection::CollideWithWorld(vector3 vecPos, vector3 vecVelocity,vector3 vecBeforePos,int Depth,CollisionType &CT,bool ChrCollision)
{		
	if((++Depth) == 6)
	{
		return vecBeforePos;
	}	
		

	vecPos+=vecVelocity;
	m_vecVelocity=vecVelocity;

	m_vecSourcePoint=vecPos;
	m_CollisionList.clear();

	float TmpHeight=m_vecRadius.y;
	//m_vecRadius.y*=1.9f;
	if(CSceneManager::m_pBspScene)
		CheckBspCollision();
	else
    {
        if(m_bFirstBottom)
        {
			if(m_bFirstScene)
			{
				CSceneManager::ms_pResourceLoader->Clear();
				m_bFirstScene = false;				// 첫 등장하는 섹터의 Octree 로딩완료 위해.

			}
            CheckFirstHouseCollision();             // FirstBottom

        }
        else
		    CheckHouseCollision();

    }
	if(!CSceneManager::GetInView())
	{
		if(ChrCollision)
		{		
			CheckSectorMeshCollision();
			CheckSectorPlantCollision();		
		}
	}
	//CheckSectorHeightCollision();	
	
	if(ChrCollision)
		CheckCharacterCollision(true);	
	else
		CheckCharacterCollision(false);				
	
	m_vecRadius.y=TmpHeight;
	
	if(m_CollisionList.empty())
	{
		//vector3 vecNewPos=vecPos-vecVelocity;
		//return vecNewPos;
		CT=CT_NONE;
		return vecPos;
	}	
	else
	{
		for(int i=0;i<(int)m_CollisionList.size();i++)
		{			
			if(m_CollisionList[i].m_CollisionType == CT_SIDE)
			{
				/*
				vector3 vecNewPos=vecPos-vecVelocity;
				bWallCollision=true;
				return vecNewPos;
				*/
				
				int MinIndex=i;
				for(int cCollisionList=0;cCollisionList<(int)m_CollisionList.size();cCollisionList++)				
				{
					if(m_CollisionList[cCollisionList].m_fDistance<m_CollisionList[MinIndex].m_fDistance)
					{
						if(m_CollisionList[cCollisionList].m_CollisionType == CT_SIDE)
							MinIndex=cCollisionList;
					}
				}

				vector3 vecVelNormal=vecVelocity;
				vecVelNormal.Normalize();
				vector3 vecMoveDir=(vecVelNormal+m_CollisionList[MinIndex].m_vecPlaneNormal);			

				vector3 vecNewPos=vecPos-vecVelocity;
				
				
				float fBackInter=CIntersection::PointFromPlane(m_CollisionList[MinIndex].m_vecPlaneNormal,
											  m_CollisionList[MinIndex].m_vecPlanePoint,vecPos);
				
				vector3 vecUp=(m_vecRadius.x + 2.0f - fBackInter)*m_CollisionList[MinIndex].m_vecPlaneNormal;				
		// 03.09.08 수정
		/*		if(fBackInter > m_vecRadius.x - 0.01)
					break;
		*/		
				vector3 vecUnit = vector3(0.0f,1.0f,0.0f);
				vector3 vecUnit2 = vecUp;
				vecUnit2.Normalize();
				float fUnit = vecUnit * vecUnit2;
				if(fUnit <= -0.5f)
				{
					//CT=CT_BOTTOM;	
					CT = CT_SIDE;
					return vecBeforePos;

				}
				

				CT=CT_SIDE;				
				//return vecNewPos+(vecPos+vecUp-vecNewPos);
				//vector3 vel=(vecPos+vecUp-vecNewPos);
				vecUp+=vecPos;
				vector3 vecNewVelocity=(vecUp-vecNewPos);
				//vecNewVelocity=vecNewVelocity/2.0f;
				return CollideWithWorld(vecNewPos,vecNewVelocity,vecBeforePos,Depth,CT,ChrCollision);
				//return vecNewPos+vecUp;
				//return CollideWithWorld(vecNewPos,(vecPos+vecUp)-vecNewPos,Depth,bWallCollision);
				//return CollideWithWorld(vecPos,vecUp,Depth,CT);
				//return vecNewPos;
				/*
				m_CollisionList[i].m_vecIntersectionPoint;				
				
				return vecNewPos;				
				*/
			}
			/*
			if(m_CollisionList[i].m_CollisionType == CT_CHRSIDE)
			{
				CT=CT_CHRSIDE;
				return vecPos;
			}
			*/
		}
		float fMinDistance=1000000000.0f;
		int MinIndex=-1;
		for(int i=0;i<(int)m_CollisionList.size();i++)
		{
			if( m_CollisionList[i].m_fDistance<fMinDistance && m_CollisionList[i].m_CollisionType==CT_BOTTOM ||
				m_CollisionList[i].m_fDistance<fMinDistance && m_CollisionList[i].m_CollisionType==CT_TERRAIN)
			{
				fMinDistance=m_CollisionList[i].m_fDistance;
				MinIndex=i;
				CT=m_CollisionList[i].m_CollisionType;
			}
		}
		if(MinIndex==-1)
		{
			vector3 vecNewPos=vecPos-vecVelocity;
			return vecNewPos;			
		}
		m_vecSourcePoint=m_CollisionList[MinIndex].m_vecIntersectionPoint+vector3(0.0f,m_vecRadius.y,0.0f);
		return m_CollisionList[MinIndex].m_vecIntersectionPoint+vector3(0.0f,m_vecRadius.y,0.0f);						
	}	
	
	return m_vecSourcePoint;
}

void CCollisionDetection::CheckSectorMeshCollision()
{
	vector3 vecBoundBox[8];
	vector3 vecPolyTest[3];
	vector3 vecPlaneNormal;
	matrix matTrans;	
	float fIntersection;

	WORD wCollisionIndices[]=
	{
		4,1,0,
		1,4,5,

		5,3,1,
		3,5,7,

		7,2,3,
		2,7,6,		

		6,0,2,
		0,6,4,

		4,5,6,
		5,6,7
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

	//for(int cSector=0;cSector<m_LSizeX*m_LSizeY;cSector++)
	for(int cSector=0;cSector<m_HeightField->GetLSizeX()*m_HeightField->GetLSizeY();cSector++)
	{
		// aliases
		CSectorScene &currentSector = m_HeightField->m_SectorScene[cSector];
		float& currentSectorPosX = currentSector.m_AccumulateTM._41;
		float& currentSectorPosZ = currentSector.m_AccumulateTM._43;

		if( currentSectorPosX <= m_vecSourcePoint.x &&
			m_vecSourcePoint.x < currentSectorPosX+SECTORSIZE &&
			currentSectorPosZ <= m_vecSourcePoint.z &&
			m_vecSourcePoint.z < currentSectorPosZ+SECTORSIZE)
		{
			
			for(int cObject=0;cObject<(int)currentSector.m_ObjectSceneNode.size();cObject++)
			{
				vecLens=currentSector.m_ObjectSceneNode[cObject]->m_TM.GetLoc()-m_vecSourcePoint;
				if(vecLens.GetLens() > 1000.0f)
					continue;
				if(strstr(currentSector.m_ObjectSceneNode[cObject]->m_strObjectName,"ZXC") ||
					strstr(currentSector.m_ObjectSceneNode[cObject]->m_strObjectName,"zxc"))
					continue;
				if(currentSector.m_ObjectSceneNode[cObject]->m_bInit == false)
					continue;

				vecBoundBox[0].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.x;
				vecBoundBox[0].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.y;
				vecBoundBox[0].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.z;

				vecBoundBox[1].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.x;
				vecBoundBox[1].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.y;
				vecBoundBox[1].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.z;

				vecBoundBox[2].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.x;
				vecBoundBox[2].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.y;
				vecBoundBox[2].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.z;

				vecBoundBox[3].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.x;
				vecBoundBox[3].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.y;
				vecBoundBox[3].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.z;

				vecBoundBox[4].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.x;
				vecBoundBox[4].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.y;
				vecBoundBox[4].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.z;

				vecBoundBox[5].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.x;
				vecBoundBox[5].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.y;
				vecBoundBox[5].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.z;

				vecBoundBox[6].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMinBox.x;
				vecBoundBox[6].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.y;
				vecBoundBox[6].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.z;

				vecBoundBox[7].x=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.x;
				vecBoundBox[7].y=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.y;
				vecBoundBox[7].z=currentSector.m_ObjectSceneNode[cObject]->m_vecMaxBox.z;

				for(int i=0;i<8;i++)
				{
					matTrans.Translation(vecBoundBox[i]);
					matTrans=matTrans*currentSector.m_ObjectSceneNode[cObject]->m_AccumulateTM;
					vecBoundBox[i]=matTrans.GetLoc();
				}			

				//
				//vecPolyTest[0]=vecBoundBox[4];
				//vecPolyTest[1]=vecBoundBox[0];
				//vecPolyTest[2]=vecBoundBox[1];			

				vector3 vecVelocity=m_vecVelocity.Normalized();
				vecVelocity.y=0.0f;
				vecVelocity.Normalize();

				for(int cIndices=0;cIndices<8;cIndices++)
				{
					vecPolyTest[0]=vecBoundBox[wCollisionIndices[cIndices*3+0]];
					vecPolyTest[1]=vecBoundBox[wCollisionIndices[cIndices*3+1]];
					vecPolyTest[2]=vecBoundBox[wCollisionIndices[cIndices*3+2]];
					

					vector3 vecNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecNormal.Normalize();

					for(int i=0;i<8;i++)
					{	// 9.26 수정(vector3 관련)
						/*float fRate=vecVelocity*vecNormal;
						if(fRate>= 0.0f)
							continue;*/
						if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
						{
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;						
							AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
							AddNode.m_vecPlanePoint=vecPolyTest[0];
							vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
							vecPlaneNormal.Normalize();
							AddNode.m_vecPlaneNormal=vecPlaneNormal;
							AddNode.m_CollisionType=CT_SIDE;													
							m_CollisionList.push_back(AddNode);
						}
					}
				}

				for(int cIndices=8;cIndices<10;cIndices++)
				{
					vecPolyTest[0]=vecBoundBox[wCollisionIndices[cIndices*3+0]];
					vecPolyTest[1]=vecBoundBox[wCollisionIndices[cIndices*3+1]];
					vecPolyTest[2]=vecBoundBox[wCollisionIndices[cIndices*3+2]];

					if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
												 vecPolyTest,
												 fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;

						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vector3(0.0f,-1.0f,0.0f)*fIntersection;
						
						AddNode.m_vecPlaneNormal=vector3(0.0f,1.0f,0.0f);
						AddNode.m_CollisionType=CT_BOTTOM;
						m_CollisionList.push_back(AddNode);
						continue;
					}
					/*
					if(CIntersection::PolygonRay(m_vecSourcePoint+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x,
												 m_vecSourcePoint+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
												 vecPolyTest,fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;						
						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vector3(1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y;						
						
						AddNode.m_vecPlaneNormal=vector3(0.0f,1.0f,0.0f);
						AddNode.m_CollisionType=CT_BOTTOM;
						m_CollisionList.push_back(AddNode);
						continue;
					}

					if(CIntersection::PolygonRay(m_vecSourcePoint+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x,
												 m_vecSourcePoint+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
												 vecPolyTest,
												 fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;						
						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vector3(-1.0f,0.0f,0.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y;						
						
						AddNode.m_vecPlaneNormal=vector3(0.0f,1.0f,0.0f);
						AddNode.m_CollisionType=CT_BOTTOM;
						m_CollisionList.push_back(AddNode);
						continue;
					}
					if(CIntersection::PolygonRay(m_vecSourcePoint+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x,
												 m_vecSourcePoint+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
												 vecPolyTest,fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;					
						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vector3(0.0f,0.0f,1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y;						
						
						AddNode.m_vecPlaneNormal=vector3(0.0f,1.0f,0.0f);
						AddNode.m_CollisionType=CT_BOTTOM;
						m_CollisionList.push_back(AddNode);
						continue;
					}

					if(CIntersection::PolygonRay(m_vecSourcePoint+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x,
												 m_vecSourcePoint+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y,
												 vecPolyTest,fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vector3(0.0f,0.0f,-1.0f)*m_vecRadius.x+vector3(0.0f,-1.0f,0.0f)*m_vecRadius.y;						
						
						AddNode.m_vecPlaneNormal=vector3(0.0f,1.0f,0.0f);
						AddNode.m_CollisionType=CT_BOTTOM;
						m_CollisionList.push_back(AddNode);
						continue;
					}				
					*/
				}
			}
			break;
		}		
	}
}

void CCollisionDetection::CheckSectorPlantCollision()
{
	vector3 vecInSectorPos;
	int nTreeIndexX,nTreeIndexZ;
	int nCheckTreeIndexX[4],nCheckTreeIndexZ[4];
	vector3 vecBoundBox[8];
	vector3 vecTransBoundBox[8];
	vector3 vecMinBox,vecMaxBox;

	// edith 2008.02.14 나무 충돌 박스
	vecMinBox=vector3(-50.0f,0.0f,-50.0f);
	vecMaxBox=vector3(50.0f,500.0f,50.0f);

	vecBoundBox[0].x=vecMinBox.x;
	vecBoundBox[0].y=vecMinBox.y;
	vecBoundBox[0].z=vecMinBox.z;

	vecBoundBox[1].x=vecMaxBox.x;
	vecBoundBox[1].y=vecMinBox.y;
	vecBoundBox[1].z=vecMinBox.z;

	vecBoundBox[2].x=vecMinBox.x;
	vecBoundBox[2].y=vecMinBox.y;
	vecBoundBox[2].z=vecMaxBox.z;

	vecBoundBox[3].x=vecMaxBox.x;
	vecBoundBox[3].y=vecMinBox.y;
	vecBoundBox[3].z=vecMaxBox.z;

	vecBoundBox[4].x=vecMinBox.x;
	vecBoundBox[4].y=vecMaxBox.y;
	vecBoundBox[4].z=vecMinBox.z;

	vecBoundBox[5].x=vecMaxBox.x;
	vecBoundBox[5].y=vecMaxBox.y;
	vecBoundBox[5].z=vecMinBox.z;

	vecBoundBox[6].x=vecMinBox.x;
	vecBoundBox[6].y=vecMaxBox.y;
	vecBoundBox[6].z=vecMaxBox.z;

	vecBoundBox[7].x=vecMaxBox.x;
	vecBoundBox[7].y=vecMaxBox.y;
	vecBoundBox[7].z=vecMaxBox.z;

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
	
	vector3 vecPolyTest[3];
	vector3 vecPlaneNormal;	
	float fTransY;
	float fIntersection;

	vector3 vecDirection[8];
	vecDirection[0]=vector3(0.707f,0.0f,0.707f);
	vecDirection[1]=vector3(0.707f,0.0f,-0.707f);
	vecDirection[2]=vector3(-0.707f,0.0f,0.707f);
	vecDirection[3]=vector3(-0.707f,0.0f,-0.707f);

	vecDirection[4]=vector3(1.0f,0.0f,0.0f);
	vecDirection[5]=vector3(-1.0f,0.0f,0.0f);
	vecDirection[6]=vector3(0.0f,0.0f,1.0f);
	vecDirection[7]=vector3(0.0f,0.0f,-1.0f);


	//for(int cSector=0;cSector<m_LSizeX*m_LSizeY;cSector++)
	for(int cSector=0;cSector<m_HeightField->GetLSizeX()*m_HeightField->GetLSizeY();cSector++)
	{
		// aliases
		CSectorScene &currentSector = m_HeightField->m_SectorScene[cSector];
		float& currentSectorPosX = currentSector.m_AccumulateTM._41;
		float& currentSectorPosZ = currentSector.m_AccumulateTM._43;

		if( currentSectorPosX <= m_vecSourcePoint.x &&
			m_vecSourcePoint.x < currentSectorPosX+SECTORSIZE &&
			currentSectorPosZ <= m_vecSourcePoint.z &&
			m_vecSourcePoint.z < currentSectorPosZ+SECTORSIZE)
		{
			vecInSectorPos.x=m_vecSourcePoint.x-currentSectorPosX;
			vecInSectorPos.z=m_vecSourcePoint.z-currentSectorPosZ;
			nTreeIndexX=vecInSectorPos.x/LINTERVAL;
			nTreeIndexZ=vecInSectorPos.z/LINTERVAL;
			
			nCheckTreeIndexX[0]=nTreeIndexX;
			nCheckTreeIndexZ[0]=nTreeIndexZ;
			if(nTreeIndexX==SECTORSX-1)
			{
				nCheckTreeIndexX[1]=-1;
				nCheckTreeIndexZ[1]=-1;
			}
			else
			{
				nCheckTreeIndexX[1]=nTreeIndexX+1;
				nCheckTreeIndexZ[1]=nTreeIndexZ;
			}

			if(nTreeIndexZ==SECTORSX-1)
			{
				nCheckTreeIndexX[2]=-1;
				nCheckTreeIndexZ[2]=-1;
			}
			else
			{
				nCheckTreeIndexX[2]=nTreeIndexX;
				nCheckTreeIndexZ[2]=nTreeIndexZ+1;
			}

			if(nTreeIndexX==SECTORSX-1 || nTreeIndexZ==SECTORSX-1)
			{
				nCheckTreeIndexX[3]=-1;
				nCheckTreeIndexZ[3]=-1;
			}
			else
			{				
				nCheckTreeIndexX[3]=nTreeIndexX+1;
				nCheckTreeIndexZ[3]=nTreeIndexZ+1;
			}

			for(int i=0;i<4;i++)
			{				
				if( nCheckTreeIndexX[i]!=-1 &&
					//currentSector.m_TreeObjectNode.m_AlreadyPutPlant[nCheckTreeIndexX[i]+nCheckTreeIndexZ[i]*SECTORSX]!=-1)
					currentSector.m_TreeObjectNode.m_usDetailTree[nCheckTreeIndexX[i]+nCheckTreeIndexZ[i]*SECTORSX]!=0xff)
				{				
					//fTransY=currentSector.m_TreeObjectNode.m_vecPlantPosition[currentSector.m_TreeObjectNode.m_AlreadyPutPlant[nCheckTreeIndexX[i]+nCheckTreeIndexZ[i]*SECTORSX]].y;
					fTransY=currentSector.m_TreeObjectNode.m_pHeight[nCheckTreeIndexX[i]+nCheckTreeIndexZ[i]*SECTORSX];
						//currentSector.m_TreeObjectNode.m_vecPlantPosition[currentSector.m_TreeObjectNode.m_pHeight;
					
					vector3 vecTmp = m_vecSourcePoint - vector3(nCheckTreeIndexX[i]*LINTERVAL+currentSectorPosX,
																			   fTransY,
																			   nCheckTreeIndexZ[i]*LINTERVAL+currentSectorPosZ);
					if(vecTmp.GetLens() < 500.0f)
					{
						//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
						m_bBuild = false;
					}

					// Minotaurs 수정 나무 크기에 따른 충돌 영역
					unsigned short usKID = currentSector.m_TreeObjectNode.m_usDetailTree[nCheckTreeIndexX[i]+nCheckTreeIndexZ[i]*SECTORSX] & 0x3f;

					float fLen = currentSector.m_TreeObjectNode.m_NormalTreeMesh[usKID]->m_MaxBox.x -currentSector.m_TreeObjectNode.m_NormalTreeMesh[usKID]->m_MinBox.x;
					fLen/=LINTERVAL;
					fLen+=3.f;
					fLen*=10.f;
					vecMinBox=vector3(-fLen,0.0f,-fLen);
					vecMaxBox=vector3(fLen,400.0f,fLen);

					vecBoundBox[0].x=vecMinBox.x;
					vecBoundBox[0].z=vecMinBox.z;

					vecBoundBox[1].x=vecMaxBox.x;
					vecBoundBox[1].z=vecMinBox.z;

					vecBoundBox[2].x=vecMinBox.x;
					vecBoundBox[2].z=vecMaxBox.z;

					vecBoundBox[3].x=vecMaxBox.x;
					vecBoundBox[3].z=vecMaxBox.z;

					vecBoundBox[4].x=vecMinBox.x;
					vecBoundBox[4].z=vecMinBox.z;

					vecBoundBox[5].x=vecMaxBox.x;
					vecBoundBox[5].z=vecMinBox.z;

					vecBoundBox[6].x=vecMinBox.x;
					vecBoundBox[6].z=vecMaxBox.z;

					vecBoundBox[7].x=vecMaxBox.x;
					vecBoundBox[7].z=vecMaxBox.z;



					for(int cVertex=0;cVertex<8;cVertex++)
					{
						vecTransBoundBox[cVertex]=vecBoundBox[cVertex]+vector3(nCheckTreeIndexX[i]*LINTERVAL+currentSectorPosX,
																			   fTransY,
																			   nCheckTreeIndexZ[i]*LINTERVAL+currentSectorPosZ);
					}					
					for(int cIndices=0;cIndices<8;cIndices++)
					{
						vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
						vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
						vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
						for(int cDir=0;cDir<8;cDir++)
						{
							if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[cDir]*m_vecRadius.x,vecPolyTest,fIntersection))
							{
								CCollisionList AddNode;
								AddNode.m_fDistance=fIntersection;						
								AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[cDir]*fIntersection;
								AddNode.m_vecPlanePoint=vecPolyTest[0];
								vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
								vecPlaneNormal.Normalize();
								AddNode.m_vecPlaneNormal=vecPlaneNormal;
								AddNode.m_CollisionType=CT_SIDE;													
								m_CollisionList.push_back(AddNode);
							}
						}
					}					
				}
			}		
			break;
		}
	}
}

void CCollisionDetection::CheckCharacterCollision(bool bChrMode)
{	
	vector3 vecLens,vecChrPos,vecPos;
	float fLens;

	vecPos=m_vecSourcePoint;
	vecPos.y -= 85.0f;

	//vecPos.y=0.0f;
	if(bChrMode)
	{
		/*
		CSceneManager::m_CharacterManager.m_CharacterList[0]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		vecChrPos.y=0.0f;
		vecLens=vecPos-vecChrPos;		
		fLens=vecLens.GetLens();
		if(fLens < m_vecRadius.x*2.0f)
		{
			CCollisionList AddNode;
			AddNode.m_fDistance=fLens-m_vecRadius.x;

			AddNode.m_vecPlaneNormal=vecLens;
			AddNode.m_vecPlaneNormal.Normalize();

			AddNode.m_vecPlanePoint=vecChrPos+AddNode.m_vecPlaneNormal*(m_vecRadius.x);
			
			AddNode.m_CollisionType=CT_SIDE;
			m_CollisionList.push_back(AddNode);
		}
		*/

		for(int nChr=1;nChr<(int)CSceneManager::m_CharacterManager.m_CharacterList.size();nChr++)
		{
			if (CSceneManager::m_CharacterManager.m_CharacterList[nChr].m_bHide ||
				CSceneManager::m_CharacterManager.m_CharacterList[nChr].m_bSkipCollision
				) continue;	// Hide 캐릭터 충돌 무시
			if(CSceneManager::m_CharacterManager.m_CharacterList[nChr].m_bCollisionDetectAble)
			{
				CSceneManager::m_CharacterManager.m_CharacterList[nChr].m_pChrmodel->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
				//vecChrPos.y=0.0f;
				vecLens=vecPos-vecChrPos;		
				fLens=vecLens.GetLens();
				if(fLens < m_vecRadius.x*2.0f)
				{
					CCollisionList AddNode;
					/// 우선순위를 밀기 위해 더미값( 2000.0f )를 더해준다
					AddNode.m_fDistance=(fLens-m_vecRadius.x) + 2000.0f;

					AddNode.m_vecPlaneNormal=vecLens;
					AddNode.m_vecPlaneNormal.Normalize();

					AddNode.m_vecPlanePoint= vecChrPos+AddNode.m_vecPlaneNormal*(m_vecRadius.x);
					
					AddNode.m_CollisionType= CT_SIDE ;
					m_CollisionList.push_back(AddNode);
					
				}
				if(fLens <= 500.0f)
				{
					if(CSceneManager::m_CharacterManager.m_CharacterList[nChr].m_pChrmodel->GetNPC())
					{
					//	CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
						m_bBuild = false;
					}
				}

			}
			else
			{
				int a=0;
			}
		}
	}
	else
	{
		//for(int cChr=0;cChr<CSceneManager::m_CharacterManager.m_CharacterList.num;cChr++)
		//{
		//	if(cChr==m_SelfChr)
		//		continue;
		/*
		if(CSceneManager::m_CharacterManager.m_CharacterList.num>0)
		{
			CSceneManager::m_CharacterManager.m_CharacterList[0]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
			vecChrPos.y=0.0f;
			vecLens=vecPos-vecChrPos;		
			fLens=vecLens.GetLens();
			if(fLens < m_vecRadius.x*2.0f)
			{
				CCollisionList AddNode;
				AddNode.m_fDistance=fLens-m_vecRadius.x;

				AddNode.m_vecPlaneNormal=vecLens;
				AddNode.m_vecPlaneNormal.Normalize();

				AddNode.m_vecPlanePoint=vecChrPos+AddNode.m_vecPlaneNormal*(m_vecRadius.x);
				
				AddNode.m_CollisionType=CT_SIDE;
				m_CollisionList.push_back(AddNode);
			}
		}
		*/
	}
		/*			
			vecLens=m_vecSourcePoint-vecChrPos;
			if(vecLens.GetLens() > 500.0f)
				continue;
			for(int i=0;i<8;i++)
			{
				vecTransBoundBox[i]=vecBoundBox[i]+vecChrPos;
			}
			for(int cIndices=0;cIndices<8;cIndices++)
			{
				vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
				vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
				vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
				for(i=0;i<8;i++)
				{
					if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
						AddNode.m_vecPlanePoint=vecPolyTest[0];
						vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
						vecPlaneNormal.Normalize();
						AddNode.m_vecPlaneNormal=vecPlaneNormal;
						AddNode.m_CollisionType=CT_CHRSIDE;													
						m_CollisionList.push_back(AddNode);
					}
				}
			}
		}		
		*/	
	/*
		CSceneManager::m_CharacterManager.m_CharacterList[0]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		vecLens=m_vecSourcePoint-vecChrPos;
		if(vecLens.GetLens() > 500.0f)
			return;
		for(int i=0;i<8;i++)
		{
			vecTransBoundBox[i]=vecBoundBox[i]+vecChrPos;
		}
		for(int cIndices=0;cIndices<8;cIndices++)
		{
			vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
			vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
			vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_CHRSIDE;													
					m_CollisionList.push_back(AddNode);
				}
			}
		}		
	*/

	/*
	if( CIntersection::PolygonRay(vecSource,
								 vecSource+vector3(0.707f,0.0f,0.707f)*m_vecRadius.x,
								 p,
								 fIntersection))
	{
		CCollisionList AddNode;
		AddNode.m_fDistance=fIntersection;
		
		matInHousePos.Translation(vecSource+vector3(0.707f,0.0f,0.707f)*fIntersection);
		matInHousePos=matInHousePos*currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
		AddNode.m_vecIntersectionPoint=matInHousePos.GetLoc();
		
		matrix matInHouseRot=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
		matInHouseRot._41=matInHouseRot._42=matInHouseRot._43=0.0f;
		matInHousePos.Translation(vecNormal);
		matInHousePos=matInHousePos*matInHouseRot;
		AddNode.m_vecPlaneNormal=matInHousePos.GetLoc();
		AddNode.m_CollisionType=CT_SIDE;

		matrix matWorldPos=currentSector.m_HouseObjectNode[cHouse]->m_AccumulateTM;
		matInHousePos.Translation(p[0]);
		matWorldPos=matInHousePos*matWorldPos;
		AddNode.m_vecPlanePoint=matWorldPos.GetLoc();

		m_CollisionList.push_back(AddNode);
		continue;
	}
	*/

	/*
	vector3 vecBoundBox[8],vecTransBoundBox[8];
	vector3 vecPolyTest[3];
	vector3 vecPlaneNormal;
	matrix matTrans;	
	float fIntersection;

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

	vector3 vecMaxBox,vecMinBox;
	vecMaxBox=vector3(-60.0f,0.0f,-60.0f);
	vecMinBox=vector3(60.0f,200.0f,60.f);
	
	vecBoundBox[0].x=vecMinBox.x;
	vecBoundBox[0].y=vecMinBox.y;
	vecBoundBox[0].z=vecMinBox.z;

	vecBoundBox[1].x=vecMaxBox.x;
	vecBoundBox[1].y=vecMinBox.y;
	vecBoundBox[1].z=vecMinBox.z;

	vecBoundBox[2].x=vecMinBox.x;
	vecBoundBox[2].y=vecMinBox.y;
	vecBoundBox[2].z=vecMaxBox.z;

	vecBoundBox[3].x=vecMaxBox.x;
	vecBoundBox[3].y=vecMinBox.y;
	vecBoundBox[3].z=vecMaxBox.z;


	vecBoundBox[4].x=vecMinBox.x;
	vecBoundBox[4].y=vecMaxBox.y;
	vecBoundBox[4].z=vecMinBox.z;

	vecBoundBox[5].x=vecMaxBox.x;
	vecBoundBox[5].y=vecMaxBox.y;
	vecBoundBox[5].z=vecMinBox.z;

	vecBoundBox[6].x=vecMinBox.x;
	vecBoundBox[6].y=vecMaxBox.y;
	vecBoundBox[6].z=vecMaxBox.z;

	vecBoundBox[7].x=vecMaxBox.x;
	vecBoundBox[7].y=vecMaxBox.y;
	vecBoundBox[7].z=vecMaxBox.z;
	
	vector3 vecLens;
	vector3 vecChrPos;
	/*
	if(bChrMode)
	{
		for(int cChr=0;cChr<CSceneManager::m_CharacterManager.m_CharacterList.num;cChr++)
		{
			if(cChr==m_SelfChr)
				continue;
			CSceneManager::m_CharacterManager.m_CharacterList[cChr]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
			vecLens=m_vecSourcePoint-vecChrPos;
			if(vecLens.GetLens() > 500.0f)
				continue;
			for(int i=0;i<8;i++)
			{
				vecTransBoundBox[i]=vecBoundBox[i]+vecChrPos;
			}
			for(int cIndices=0;cIndices<8;cIndices++)
			{
				vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
				vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
				vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
				for(i=0;i<8;i++)
				{
					if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
					{
						vector3 v1=vecPolyTest[1]-vecPolyTest[0];
						vector3 v2=vecPolyTest[2]-vecPolyTest[1];
						vector3 vecNormal=v1^v2;
						vecNormal.Normalize();
						float fAngle=vecDirection[i]*vecNormal;
						fAngle=acosf(fAngle);
						if(fAngle>=(3.14159f/2.0f))
						{						
							CCollisionList AddNode;
							AddNode.m_fDistance=fIntersection;						
							AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
							AddNode.m_vecPlanePoint=vecPolyTest[0];
							vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
							vecPlaneNormal.Normalize();
							AddNode.m_vecPlaneNormal=vecPlaneNormal;
							AddNode.m_CollisionType=CT_CHRSIDE;													
							m_CollisionList.push_back(AddNode);
						}
					}
				}
			}
		}	
		/*
		CSceneManager::m_CharacterManager.m_CharacterList[0]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
		vecLens=m_vecSourcePoint-vecChrPos;
		if(vecLens.GetLens() > 500.0f)
			return;
		for(int i=0;i<8;i++)
		{
			vecTransBoundBox[i]=vecBoundBox[i]+vecChrPos;
		}
		for(int cIndices=0;cIndices<8;cIndices++)
		{
			vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
			vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
			vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_CHRSIDE;													
					m_CollisionList.push_back(AddNode);
				}
			}
		}		
	}
	else
	{		
		for(int cChr=1;cChr<CSceneManager::m_CharacterManager.m_CharacterList.num;cChr++)
		{
			if(cChr==m_SelfChr)
				continue;
			CSceneManager::m_CharacterManager.m_CharacterList[cChr]->GetPosition(vecChrPos.x,vecChrPos.y,vecChrPos.z);
			vecLens=m_vecSourcePoint-vecChrPos;
			if(vecLens.GetLens() > 500.0f)
				continue;
			for(int i=0;i<8;i++)
			{
				vecTransBoundBox[i]=vecBoundBox[i]+vecChrPos;
			}
			for(int cIndices=0;cIndices<8;cIndices++)
			{
				vecPolyTest[0]=vecTransBoundBox[wCollisionIndices[cIndices*3+0]];
				vecPolyTest[1]=vecTransBoundBox[wCollisionIndices[cIndices*3+2]];
				vecPolyTest[2]=vecTransBoundBox[wCollisionIndices[cIndices*3+1]];
				for(i=0;i<8;i++)
				{
					if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
					{
						CCollisionList AddNode;
						AddNode.m_fDistance=fIntersection;						
						AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
						AddNode.m_vecPlanePoint=vecPolyTest[0];
						vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
						vecPlaneNormal.Normalize();
						AddNode.m_vecPlaneNormal=vecPlaneNormal;
						AddNode.m_CollisionType=CT_CHRSIDE;													
						m_CollisionList.push_back(AddNode);
					}
				}
			}
		}		
	}
	*/
}

bool CCollisionDetection::CheckSectorWater(vector3& vecPos,bool bUpper)
{
	for(int cSector=0;cSector<m_HeightField->GetLSizeX()*m_HeightField->GetLSizeY();cSector++)
	{
		// aliases
		CSectorScene &currentSector = m_HeightField->m_SectorScene[cSector];
		float& currentSectorPosX = currentSector.m_AccumulateTM._41;
		float& currentSectorPosZ = currentSector.m_AccumulateTM._43;

		if( currentSectorPosX <= m_vecSourcePoint.x &&
			m_vecSourcePoint.x < currentSectorPosX+SECTORSIZE &&
			currentSectorPosZ <= m_vecSourcePoint.z &&
			m_vecSourcePoint.z < currentSectorPosZ+SECTORSIZE)
		{
			if(currentSector.m_MapWater)
			{				
						
				if(currentSector.m_MapWater->m_fWaterHeight-80 > vecPos.y)
				{
			//		CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
					m_bBuild = false;

					return true;
					/*
					if(bUpper)
						return true;
					return false;
					*/
				}
				else if(currentSector.m_MapWater->m_fWaterHeight-40 > vecPos.y)
				{
					vecPos.y = currentSector.m_MapWater->m_fWaterHeight-80;

					m_bBuild = false;
					return true;
				}

				return false;
				//return true;				
			}
			return false;
			//return true;
		}
	}
	return false;
}

void CCollisionDetection::CheckBspCollision()
{
	
}


void CCollisionDetection::CheckSectorHeightCollision(CollisionType &CT)
{
	std::vector<vector3> vecHeightList;
	m_HeightField->GetHeightFieldShadowPoly(m_vecSourcePoint,vecHeightList);		
	vector3 v[6];
	vector3 vecNormal,vecCompareNormal,vecMove;
	float fLens;
	float fAngle;

	vecCompareNormal=vector3(0.0f,1.0f,0.0f);		

	float fHeight=m_HeightField->GetHeight(m_vecSourcePoint);

	float fMoveFactor;

	if(fHeight+m_vecRadius.y < m_vecSourcePoint.y)
	{		
		return;		
	}
	else
	{
		m_vecSourcePoint.y=fHeight+m_vecRadius.y;
	}

	bool bCheckHeight=false;
	m_bHeightGradient = false;

	for(int cPoly=0;cPoly<(int)vecHeightList.size()/3;cPoly++)
	{
		v[0]=vecHeightList[cPoly*3+0];
		v[1]=vecHeightList[cPoly*3+1];
		v[2]=vecHeightList[cPoly*3+2];
		
		if(CIntersection::PolygonRay(vector3(m_vecSourcePoint.x,0.0f,m_vecSourcePoint.z),vector3(m_vecSourcePoint.x,120000.0f,m_vecSourcePoint.z),v,fLens))
		{
			bCheckHeight=true;

			vecNormal=(v[1]-v[0])^(v[2]-v[1]);
			vecNormal.Normalize();
			fAngle=vecNormal*vecCompareNormal;
			fAngle=acosf(fAngle);
			/*if(fAngle> ((3.14159/180.0f)*35.0f))
			{
				vecMove=vecNormal-vecCompareNormal;
				vecMove.Normalize();
				fMoveFactor=fabsf(vecMove.y);
				vecMove.y=0.0f;
				vecMove=vecMove*fMoveFactor*12.0f;
				m_vecVelocity+=vecMove;
				m_bBuild = false;
			}*/
			
			// edith 높은곳에 못올라가게 기울기 처리
			// 기울기 체크 추가 wizardbug		

			if(CT != CT_WATER)
			{
				if(fAngle> ((3.14159/180.0f)*45.0f))
				{
					vecMove=vecNormal-vecCompareNormal;
					vecMove.Normalize();
					if(vecMove.y > 0.0f)
						vecMove.y *= -1.0f;
					fMoveFactor=fabsf(vecMove.y);
					

					//vecMove.y=0.0f;
					vecMove=vecMove*fMoveFactor*70.0f;	// 잘 굴러떨어지는 로직. 값이 크면 더 많이 떨어진다.
	//				vecMove=vecMove*fMoveFactor*35.0f;
					m_vecVelocity+=vecMove;
					m_vecVelocity.y = -30.0f;	// 값이 적으면 적을수록 떨림이 줄어든다.
	//				m_vecVelocity.y = -8.0f;
					//CSceneManager::m_CharacterManager.SetChrBuildEnable(false);
					m_bBuild = false;
					m_bHeightGradient = true;

				}
			}
		}
		/*

		vecNormal=(v[1]-v[0])^(v[2]-v[1]);
		vecNormal.Normalize();
		fAngle=vecNormal*vecCompareNormal;
		fAngle=acosf(fAngle);
		if(fAngle> ((3.14159/180.0f)*35.0f))
		{

			/*
			v[3]=v[0]+vector3(0.0f,300.0f,0.0f);
			v[4]=v[1]+vector3(0.0f,300.0f,0.0f);
			v[5]=v[2]+vector3(0.0f,300.0f,0.0f);			

			vector3 vecPolyTest[3],vecPlaneNormal;
			float fIntersection;


			vecPolyTest[0]=v[0];
			vecPolyTest[1]=v[1];
			vecPolyTest[2]=v[3];			

			for(int i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}


			vecPolyTest[0]=v[3];
			vecPolyTest[1]=v[4];
			vecPolyTest[2]=v[1];			

			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}

			vecPolyTest[0]=v[1];
			vecPolyTest[1]=v[2];
			vecPolyTest[2]=v[4];			

			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}

			vecPolyTest[0]=v[4];
			vecPolyTest[1]=v[5];
			vecPolyTest[2]=v[2];			

			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}

			vecPolyTest[0]=v[2];
			vecPolyTest[1]=v[0];
			vecPolyTest[2]=v[5];			

			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}

			vecPolyTest[0]=v[5];
			vecPolyTest[1]=v[3];
			vecPolyTest[2]=v[0];			

			for(i=0;i<8;i++)
			{
				if(CIntersection::PolygonRay(m_vecSourcePoint,m_vecSourcePoint+vecDirection[i]*m_vecRadius.x,vecPolyTest,fIntersection))
				{
					CCollisionList AddNode;
					AddNode.m_fDistance=fIntersection;						
					AddNode.m_vecIntersectionPoint=m_vecSourcePoint+vecDirection[i]*fIntersection;
					AddNode.m_vecPlanePoint=vecPolyTest[0];
					vecPlaneNormal=(vecPolyTest[1]-vecPolyTest[0])^(vecPolyTest[2]-vecPolyTest[1]);
					vecPlaneNormal.Normalize();
					AddNode.m_vecPlaneNormal=vecPlaneNormal;
					AddNode.m_CollisionType=CT_SIDE;													
					m_CollisionList.push_back(AddNode);
					return;
				}
			}
		}

		/*
		if(CIntersection::PolygonRay(vector3(vecMovePoint.x,0.0f,vecMovePoint.z),vector3(vecMovePoint.x,120000.0f,vecMovePoint.z),v,fLens))
		{
			vecNormal=(v[1]-v[0])^(v[2]-v[1]);
			vecNormal.Normalize();
			fAngle=vecNormal*vecCompareNormal;
			fAngle=acosf(fAngle);
			if(fAngle> ((3.14159/180.0f)*35.0f))
			{
				return vecPos;
			}
		}
		*/		
	}
	if(bCheckHeight==false)
	{		
	}
}

void CCollisionDetection::CheckInHouseObject(CHouseObject *pHouse,matrix matParent)
{

	vector3 vecBoundBox[8];
	vector3 vecPolyTest[3];
	vector3 vecPlaneNormal;
	matrix matTrans;	

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

	matrix matTM;


}
// Zone Bound func (Zone Info 파일에서 setting)
void  CCollisionDetection::SetZoneBound(float fMin,float fMax) {
	m_fMinYBound = fMin;
	m_fMaxYBound = fMax;
}
void CCollisionDetection::ZoneBoundApply(vector3 &vecPoint,CollisionType &CT) {
	if(m_fMinYBound != -123.0f)
		if(vecPoint.y <= m_fMinYBound) {
			vecPoint.y = m_fMinYBound;
			CT = CT_BOTTOM;					// 바닥으로 setting		
		}
	if(m_fMaxYBound != -123.0f)
		if(vecPoint.y >= m_fMaxYBound)
			vecPoint.y = m_fMaxYBound;

}

void CCollisionDetection::LockDetection()
{
	++m_nLockCount;
}

void CCollisionDetection::UnlockDetection()
{
	if( m_nLockCount > 0 )
	{
		--m_nLockCount;
	}
}

bool CCollisionDetection::IsDetectionLocked()
{
	return ( m_nLockCount > 0);
}

bool CCollisionDetection::IsPointInRect2D(float pointX, float pointY, float minX, float minY, float sizeX, float sizeY)
{
	if (minX <= pointX && pointX < minX + sizeX &&
		minY <= pointY && pointY < minY + sizeY)
	{
		return true;
	}

    return false;
}
