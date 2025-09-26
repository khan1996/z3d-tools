// Intersection.cpp: implementation of the CIntersection class.
//
//////////////////////////////////////////////////////////////////////

#include "Intersection.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntersection::CIntersection()
{

}

CIntersection::~CIntersection()
{

}

int CIntersection::PolygonRay(vector3 vecStart, vector3 vecEnd, vector3 *poly,float &fInterLens)
{
	/*
	vector3 vecDir=(vecEnd-vecStart);
	vecDir.Normalize();	
	vector3 vecPoly[3];
	vecPoly[0]=*poly[0];
	vecPoly[1]=*poly[1];
	vecPoly[2]=*poly[2];
	if(PolygonRay2(vecStart,vecDir,vecPoly))
		return 1;
	return 0;
	*/

	///*
	vector3 n=(poly[1]-poly[0])^(poly[2]-poly[0]);
	n.Normalize();
	if((fabs(n.x) <= 0.00000001f) &&
		(fabs(n.y) <= 0.00000001f) &&
		(fabs(n.z) <= 0.00000001f)) {
			fInterLens = 1000000.0f;
			return 0;
		}
	vector3 vecDir=(vecEnd-vecStart);
	vecDir.Normalize();
	float h;
	if(fabs((n.x*vecDir.x + n.y*vecDir.y + n.z*vecDir.z)) < 0.00000001f)
	{
		fInterLens = 1000000.0f;
		return 0;
	}
	else {
		h=( (n.x*poly[0].x+n.y*poly[0].y+n.z*poly[0].z) - (n.x*vecStart.x+n.y*vecStart.y+n.z*vecStart.z) )
				/ (n.x*vecDir.x + n.y*vecDir.y + n.z*vecDir.z);
		fInterLens=h;
	}
	if(h<0.0f)return 0;
	vector3 vecInter=vecEnd-vecStart; 
	if(vecInter.GetLens() < h)
		return 0;

	vector3 vecInterpos=vecStart+vecDir*h;

	
	vector3 vecEgde,vecEdgeNormal;
	float fHalfPlane;
	long pos=0,neg=0;
	vecEgde=poly[0]-poly[1];
	vecEdgeNormal=vecEgde^n;
	vecEdgeNormal.Normalize();
	fHalfPlane=(vecInterpos*vecEdgeNormal)-(poly[1]*vecEdgeNormal);
	if(fHalfPlane >= 0.001f)
		pos++;
	if(fHalfPlane <= -0.001f)
		neg++;
	vecEgde=poly[1]-poly[2];
	vecEdgeNormal=vecEgde^n;
	vecEdgeNormal.Normalize();
	fHalfPlane=(vecInterpos*vecEdgeNormal)-(poly[2]*vecEdgeNormal);

	if(fHalfPlane >= 0.001f)
		pos++;
	if(fHalfPlane <= -0.001f)
		neg++;

	vecEgde=poly[2]-poly[0];
	vecEdgeNormal=vecEgde^n;
	vecEdgeNormal.Normalize();
	fHalfPlane=(vecInterpos*vecEdgeNormal)-(poly[0]*vecEdgeNormal);

	if(fHalfPlane >= 0.001f)
		pos++;
	if(fHalfPlane <= -0.001f)
		neg++;

	if (!pos || !neg)
		return 1;
	return 0;

	/*
	vecFactor[0]=(vecInterpos-poly[0]);vecFactor[0].Normalize();	
	vecFactor[1]=(vecInterpos-poly[1]);vecFactor[1].Normalize();			
	vecFactor[2]=(vecInterpos-poly[2]);vecFactor[2].Normalize();

	fAngle[0]=vecFactor[0]*vecFactor[1];
	fAngle[1]=vecFactor[1]*vecFactor[2];
	fAngle[2]=vecFactor[2]*vecFactor[0];
	fAngle[0]=acosf(fAngle[0]);
	fAngle[1]=acosf(fAngle[1]);
	fAngle[2]=acosf(fAngle[2]);
	/*
	if( fAngle[0] == 3.14159f ||
		fAngle[1] == 3.14159f ||
		fAngle[2] == 3.14159f)
		return 0;
	
	if(fAngle[0]+fAngle[1]+fAngle[2] >= (3.14159f)*1.7f)
		return 1;		
	return 0;
	//*/
}

int CIntersection::PolygonToPolygon(vector3 *vecPoly1, vector3 *vecPoly2)
{
	vector3 vecPolyNormal1=(vecPoly1[1]-vecPoly1[0])^(vecPoly1[2]-vecPoly1[1]);
	vector3 vecPolyNormal2=(vecPoly2[1]-vecPoly2[0])^(vecPoly2[2]-vecPoly2[1]);
	vecPolyNormal1.Normalize();
	vecPolyNormal2.Normalize();

	float fDistance[3];
	fDistance[0]= vecPolyNormal1*(vecPoly2[0]-vecPoly1[0]);
	fDistance[1]= vecPolyNormal1*(vecPoly2[1]-vecPoly1[0]);
	fDistance[2]= vecPolyNormal1*(vecPoly2[2]-vecPoly1[0]);
	if( (fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f) )
	{
		return -1;
	}
	if( (fDistance[0] < 0.0f && fDistance[1] < 0.0f && fDistance[2] < 0.0f) )
	{
		return -2;
	}

	fDistance[0]= vecPolyNormal2*(vecPoly1[0]-vecPoly2[0]);
	fDistance[1]= vecPolyNormal2*(vecPoly1[1]-vecPoly2[0]);
	fDistance[2]= vecPolyNormal2*(vecPoly1[2]-vecPoly2[0]);

	if( (fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f) ||
		(fDistance[0] < 0.0f && fDistance[1] < 0.0f && fDistance[2] < 0.0f))
		return 0;	
	
	float fIntersection;
	vector3 vecLens;
	if(PolygonRay(vecPoly2[0],vecPoly2[1],vecPoly1,fIntersection)==1)
	{
		vecLens=vecPoly2[0]-vecPoly2[1];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	if(PolygonRay(vecPoly2[1],vecPoly2[2],vecPoly1,fIntersection)==1)
	{
		vecLens=vecPoly2[1]-vecPoly2[2];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	if(PolygonRay(vecPoly2[2],vecPoly2[0],vecPoly1,fIntersection)==1)
	{
		vecLens=vecPoly2[2]-vecPoly2[0];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(vecPoly1[0],vecPoly1[1],vecPoly2,fIntersection)==1)
	{
		vecLens=vecPoly1[0]-vecPoly1[1];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(vecPoly1[1],vecPoly1[2],vecPoly2,fIntersection)==1)
	{
		vecLens=vecPoly1[1]-vecPoly1[2];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(vecPoly1[2],vecPoly1[0],vecPoly2,fIntersection)==1)
	{
		vecLens=vecPoly1[2]-vecPoly1[0];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	return 0;
}

int CIntersection::BoxToRay(vector3 vecStart, vector3 vecEnd, vector3 *poly[], float &fIntersection)
{
	vector3 n=(*poly[1]-*poly[0])^(*poly[2]-*poly[0]);
	n.Normalize();
	vector3 vecDir=(vecEnd-vecStart);
	vecDir.Normalize();
	float h=( (n.x*poly[0]->x+n.y*poly[0]->y+n.z*poly[0]->z) - (n.x*vecStart.x+n.y*vecStart.y+n.z*vecStart.z) )
		     / (n.x*vecDir.x + n.y*vecDir.y + n.z*vecDir.z);
	fIntersection=h;
	if(h<0.0f)return 0;

	vector3 vecInterpos=vecStart+vecDir*h;
	vector3 vecFactor[4];
	float fAngle[4];

	vecFactor[0]=(vecInterpos-*poly[0]);vecFactor[0].Normalize();	
	vecFactor[1]=(vecInterpos-*poly[1]);vecFactor[1].Normalize();			
	vecFactor[2]=(vecInterpos-*poly[2]);vecFactor[2].Normalize();
	vecFactor[3]=(vecInterpos-*poly[3]);vecFactor[3].Normalize();


	fAngle[0]=vecFactor[0]*vecFactor[1];
	fAngle[1]=vecFactor[1]*vecFactor[2];
	fAngle[2]=vecFactor[2]*vecFactor[3];
	fAngle[3]=vecFactor[3]*vecFactor[0];
	fAngle[0]=acosf(fAngle[0]);
	fAngle[1]=acosf(fAngle[1]);
	fAngle[2]=acosf(fAngle[2]);
	fAngle[3]=acosf(fAngle[3]);

	if( fAngle[0] == 3.14159f ||
		fAngle[1] == 3.14159f ||
		fAngle[2] == 3.14159f ||
		fAngle[3] == 3.14159f)
		return 0;
	if(fAngle[0]+fAngle[1]+fAngle[2]+fAngle[3] >= (3.14159f)*2.0f)
		return 1;		
	return 0;
}

bool CIntersection::PlanePoint(vector3 *pPlane, vector3 &vecPoint)
{
	vector3 vecPolyNormal=(pPlane[1]-pPlane[0])^(pPlane[2]-pPlane[1]);	
	
	if(vecPolyNormal*(vecPoint-pPlane[0]) > 0.0f)
		return true;
	return false;
}

int CIntersection::PolygonQuad(vector3 *vecPoly, vector3 *vecQuad)
{
	vector3 vecPolyNormal1=(vecPoly[1]-vecPoly[0])^(vecPoly[2]-vecPoly[1]);
	vector3 vecPolyNormal2=(vecQuad[1]-vecQuad[0])^(vecQuad[2]-vecQuad[1]);
	vecPolyNormal1.Normalize();
	vecPolyNormal2.Normalize();

	float fDistance[4];
	fDistance[0]= vecPolyNormal1*(vecQuad[0]-vecPoly[0]);
	fDistance[1]= vecPolyNormal1*(vecQuad[1]-vecPoly[0]);
	fDistance[2]= vecPolyNormal1*(vecQuad[2]-vecPoly[0]);
	fDistance[3]= vecPolyNormal1*(vecQuad[3]-vecPoly[0]);
	if( (fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f && fDistance[3] > 0.0f ) )
	{
		return -1;
	}
	if( (fDistance[0] < 0.0f && fDistance[1] < 0.0f && fDistance[2] < 0.0f && fDistance[3] > 0.0f ) )
	{
		return -2;
	}

	fDistance[0]= vecPolyNormal2*(vecPoly[0]-vecQuad[0]);
	fDistance[1]= vecPolyNormal2*(vecPoly[1]-vecQuad[0]);
	fDistance[2]= vecPolyNormal2*(vecPoly[2]-vecQuad[0]);

	if( (fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f) ||
		(fDistance[0] < 0.0f && fDistance[1] < 0.0f && fDistance[2] < 0.0f))
		return 0;	
	
	float fIntersection;
	vector3 vecLens;

	if(BoxToRay(vecPoly[0],vecPoly[1],&vecQuad,fIntersection)==1)
	{
		vecLens=vecPoly[0]-vecPoly[1];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(BoxToRay(vecPoly[1],vecPoly[2],&vecQuad,fIntersection)==1)
	{
		vecLens=vecPoly[1]-vecPoly[2];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	if(BoxToRay(vecPoly[2],vecPoly[0],&vecQuad,fIntersection)==1)
	{
		vecLens=vecPoly[2]-vecPoly[0];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	return 0;
	/*	
	if(PolygonRay(*vecPoly2[2],*vecPoly2[0],vecPoly1,fIntersection)==1)
	{
		vecLens=*vecPoly2[2]-*vecPoly2[0];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(*vecPoly1[0],*vecPoly1[1],vecPoly2,fIntersection)==1)
	{
		vecLens=*vecPoly1[0]-*vecPoly1[1];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(*vecPoly1[1],*vecPoly1[2],vecPoly2,fIntersection)==1)
	{
		vecLens=*vecPoly1[1]-*vecPoly1[2];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}

	if(PolygonRay(*vecPoly1[2],*vecPoly1[0],vecPoly2,fIntersection)==1)
	{
		vecLens=*vecPoly1[2]-*vecPoly1[0];
		if(vecLens.GetLens()>=fIntersection)
			return 1;
	}
	return 0;
	*/
}

float CIntersection::PolygonRay2(vector3 vecStart, vector3 vecEnd, vector3 *poly,float &fInterLens)
{
	vector3 n=(poly[1]-poly[0])^(poly[2]-poly[0]);
	n.Normalize();
	vector3 vecDir=(vecEnd-vecStart);
	vecDir.Normalize();
	float h=( (n.x*poly[0].x+n.y*poly[0].y+n.z*poly[0].z) - (n.x*vecStart.x+n.y*vecStart.y+n.z*vecStart.z) )
		     / (n.x*vecDir.x + n.y*vecDir.y + n.z*vecDir.z);
	fInterLens=h;
	if(h<0.0f)return 0;
	vector3 vecInter=vecEnd-vecStart; 
	if(vecInter.GetLens() < h)
		return 0;

	///*
	vector3 vecInterpos=vecStart+vecDir*h;
	vector3 vecFactor[3];
	float fAngle[3];

	vecFactor[0]=(vecInterpos-poly[0]);vecFactor[0].Normalize();	
	vecFactor[1]=(vecInterpos-poly[1]);vecFactor[1].Normalize();			
	vecFactor[2]=(vecInterpos-poly[2]);vecFactor[2].Normalize();

	fAngle[0]=vecFactor[0]*vecFactor[1];
	fAngle[1]=vecFactor[1]*vecFactor[2];
	fAngle[2]=vecFactor[2]*vecFactor[0];
	fAngle[0]=acosf(fAngle[0]);
	fAngle[1]=acosf(fAngle[1]);
	fAngle[2]=acosf(fAngle[2]);
	/*
	if( fAngle[0] == 3.14159f ||
		fAngle[1] == 3.14159f ||
		fAngle[2] == 3.14159f)
		return 0;
		*/
	return fAngle[0]+fAngle[1]+fAngle[2];	
}

int CIntersection::PlaneSphere(vector3 &vecLoc, float &fRad, vector3 *pPoly)
{	
	float x1,y1,z1,x2,y2,z2,x3,y3,z3;

	float x4,y4,z4;

	x1=pPoly[1].x-pPoly[0].x;
	y1=pPoly[1].y-pPoly[0].y;
	z1=pPoly[1].z-pPoly[0].z;
 
	x2=pPoly[2].x-pPoly[1].x;
	y2=pPoly[2].y-pPoly[1].y;
	z2=pPoly[2].z-pPoly[1].z;

	x3=y1*z2-z1*y2;
	y3=z1*x2-x1*z2;
	z3=x1*y2-y1*x2;
	

	float invMag=1.0f/CFastMath::FastSqrt(x3*x3+y3*y3+z3*z3);
	x3=x3*invMag;	
	y3=y3*invMag;	
	z3=z3*invMag;	

	x4=vecLoc.x-pPoly[0].x;
	y4=vecLoc.y-pPoly[0].y;
	z4=vecLoc.z-pPoly[0].z;
	
	float fDistance=x3*x4+y3*y4+z3*z4;
	float fAbsDistance=fDistance>0.0f ? fDistance : -fDistance;
	if(fRad >= fAbsDistance)
	{
		return 1;
	}
	else
	{
		if(fDistance>0.0f)
			return 0;
		else
			return -1;
	}
	/*
	vector3 vecPolyNormal=(pPoly[1]-pPoly[0])^(pPoly[2]-pPoly[1]);
	vecPolyNormal.Normalize();
	
	float fDistance=vecPolyNormal*(vecLoc-pPoly[0]);
	float fAbsDis=fabs(fDistance);
	if(fRad >= fAbsDis)
	{
		return 1;
	}
	else
	{
		if(fDistance>=0.0f)
			return 0;
		else
			return -1;
	}
	*/
}

int CIntersection::SplitPolygonPolygon(vector3 *pPolySour, vector3 *pPolyDest, vector3 *pPolyResult)
{
	vector3 vecPolySourNormal=(pPolySour[1]-pPolySour[0])^(pPolySour[2]-pPolySour[1]);
	vecPolySourNormal.Normalize();

	float fDistance[3];
	fDistance[0]=vecPolySourNormal*(pPolyDest[0]-pPolySour[0]);
	fDistance[1]=vecPolySourNormal*(pPolyDest[1]-pPolySour[0]);
	fDistance[2]=vecPolySourNormal*(pPolyDest[2]-pPolySour[0]);
	
	if( (fDistance[0] >= 0.0f && fDistance[1] >= 0.0f && fDistance[2] >= 0.0f) )
	{
		pPolyResult[0]=pPolyDest[0];
		pPolyResult[1]=pPolyDest[1];
		pPolyResult[2]=pPolyDest[2];
		return 1;
	}
	if( (fDistance[0] <= 0.0f && fDistance[1] <= 0.0f && fDistance[2] <= 0.0f) )
	{
		return 0;
	}
	float fIntersect;
	vector3 vecFirstInter,vecSecondInter;
	int FrontVector[2]={-1,-1};
	int BackVector[2]={-1,-1};
	int cFront=0;
	int cBack=0;
	for(int i=0;i<3;i++)
	{
		if(fDistance[i] >= 0.0f)
		{
			FrontVector[cFront]=0;
			cFront++;
		}
		else
		{
			BackVector[cBack]=0;
			cBack++;
		}
	}	
	
	if(cFront==2)
	{
		PolygonRay(pPolyDest[FrontVector[0]],pPolyDest[BackVector[0]],pPolySour,fIntersect);
		vecFirstInter=pPolyDest[BackVector[0]]-pPolyDest[FrontVector[0]];
		vecFirstInter.Normalize();
		vecFirstInter=pPolyDest[FrontVector[0]]+vecFirstInter*fIntersect;		

		PolygonRay(pPolyDest[FrontVector[1]],pPolyDest[BackVector[0]],pPolySour,fIntersect);		
		vecSecondInter=pPolyDest[BackVector[0]]-pPolyDest[FrontVector[1]];
		vecSecondInter.Normalize();
		vecSecondInter=pPolyDest[FrontVector[1]]+vecFirstInter*fIntersect;

		pPolyResult[0]=pPolyDest[FrontVector[0]];
		pPolyResult[1]=vecFirstInter;
		pPolyResult[2]=vecSecondInter;

		pPolyResult[3]=vecFirstInter;
		pPolyResult[4]=vecSecondInter;
		pPolyResult[5]=pPolyDest[FrontVector[1]];
		return 2;
	}
	else
	{
		PolygonRay( pPolyDest[FrontVector[0]],pPolyDest[BackVector[0]],pPolySour,fIntersect);
		vecFirstInter=pPolyDest[BackVector[0]]-pPolyDest[FrontVector[0]];
		vecFirstInter.Normalize();
		vecFirstInter=pPolyDest[FrontVector[0]]+vecFirstInter*fIntersect;

		PolygonRay( pPolyDest[FrontVector[0]],pPolyDest[BackVector[1]],pPolySour,fIntersect);
		vecSecondInter=pPolyDest[BackVector[0]]-pPolyDest[FrontVector[1]];
		vecSecondInter.Normalize();
		vecSecondInter=pPolyDest[FrontVector[0]]+vecFirstInter*fIntersect;

		pPolyResult[0]=pPolyDest[FrontVector[0]];
		pPolyResult[1]=vecFirstInter;
		pPolyResult[2]=vecSecondInter;
		return 1;
	}	
	return 0;
	/*
	float fIntersect;
	vector3 vecFirstInter,vecSecondInter;
	if( fDistance[0]==0.0f)
	{
		if(fDistance[1] > 0.0f)
		{
			PolygonRay(pPolyDest[1],pPolyDest[2],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[2]-pPolyDest[1];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[1]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[0];
			pPolyResult[1]=pPolyDest[1];
			pPolyResult[2]=vecFirstInter;			
		}
		else
		{
			PolygonRay(pPolyDest[2],pPolyDest[1],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[1]-pPolyDest[2];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[2]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[0];
			pPolyResult[1]=vecFirstInter;
			pPolyResult[2]=pPolyDest[2];
		}
		return 1;
	}
	if( fDistance[1]==0.0f)
	{
		if(fDistance[2] > 0.0f)
		{
			PolygonRay(pPolyDest[2],pPolyDest[0],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[0]-pPolyDest[2];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[2]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[1];
			pPolyResult[1]=vecFirstInter;
			pPolyResult[2]=pPolyDest[2];
		}
		else
		{
			PolygonRay(pPolyDest[0],pPolyDest[2],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[2]-pPolyDest[0];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[0]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[1];
			pPolyResult[1]=vecFirstInter;
			pPolyResult[2]=pPolyDest[0];
		}
		return 1;
	}
	if( fDistance[2]==0.0f)
	{
		if(fDistance[0] > 0.0f)
		{
			PolygonRay(pPolyDest[0],pPolyDest[1],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[1]-pPolyDest[0];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[0]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[2];
			pPolyResult[1]=vecFirstInter;
			pPolyResult[2]=pPolyDest[0];
		}
		else
		{
			PolygonRay(pPolyDest[1],pPolyDest[0],&pPolySour,fIntersect);
			vecFirstInter=pPolyDest[0]-pPolyDest[1];
			vecFirstInter.Normalize();
			vecFirstInter=pPolyDest[1]+vecFirstInter*fIntersect;
			pPolyResult[0]=pPolyDest[2];
			pPolyResult[1]=pPolyDest[0];
			pPolyResult[2]=vecFirstInter;
		}
		return 1;
	}
	
	if(
	*/
}
// return-value 0 is not intersect
// return-value 1 is Sphere in polygon
// return-value 2 is 
int CIntersection::PolygonSphere(vector3 &vecCenter, float fRad, vector3 *pPoly,vector3 *vecIntersect,int &cIntersect)
{	
	vector3 vecPolyNormal=(pPoly[1]-pPoly[0])^(pPoly[2]-pPoly[1]);
	vecPolyNormal.Normalize();
	
	float fDistance=vecPolyNormal*(vecCenter-pPoly[0]);
	float fAbsDis=fabs(fDistance);
	vector3 vecPolyInter;
	
	if(fRad >= fAbsDis)
	{
		vecPolyNormal=-vecPolyNormal;
		float fInter;		
		///*
		if(PolygonRay(vecCenter,vecCenter+vecPolyNormal*fRad,pPoly,fInter)==1)
		{
			*vecIntersect=vecCenter+vecPolyNormal*fInter;
			vecIntersect++;
			cIntersect++;
			return 3;
		}
		//*/
		/*		
		if(PolygonRay(vecCenter,vecCenter+vector3(0.0f,-1.0f,0.0f)*fRad*10.0f,pTestPoly,fInter)==1)
		{
			//if(vecPolyNormal.y > 0.2f )
			{
				*vecIntersect=vecCenter+vecPolyNormal*fInter;
				vecIntersect++;
				cIntersect++;
				return 1;
			}
		}
		*/
		/*
		vecPolyInter=pPoly[0]-vecCenter;
		if(vecPolyInter.GetLens() <= fRad)
		{
			*vecIntersect=pPoly[0];
			vecIntersect++;
			cIntersect++;
			return 1;
		}
			
		vecPolyInter=pPoly[1]-vecCenter;
		if(vecPolyInter.GetLens() <= fRad)
		{
			*vecIntersect=pPoly[1];
			vecIntersect++;
			cIntersect++;
			return 1;
		}
		vecPolyInter=pPoly[2]-vecCenter;
		if(vecPolyInter.GetLens() <= fRad)
		{
			*vecIntersect=pPoly[2];
			vecIntersect++;
			cIntersect++;
			return 1;
		}

		float fMaxInter=0.0f;
		float b,c;	
		float fInterD=0;
		vector3 vecOriginCenter;

		vecPolyInter=pPoly[1]-pPoly[0];
		fMaxInter=vecPolyInter.GetLens();
		vecPolyInter.Normalize();
		
		vecOriginCenter=pPoly[0]-vecCenter;
		b=vecPolyInter*(vecOriginCenter);
		c=vecOriginCenter*vecOriginCenter-(fRad*fRad);
		if((b*b-c)>0.0f)			
		{
			fInterD=-b-sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[0]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
			fInterD=-b+sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[0]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
		}

		vecPolyInter=pPoly[2]-pPoly[1];
		fMaxInter=vecPolyInter.GetLens();
		vecPolyInter.Normalize();		
		
		vecOriginCenter=pPoly[1]-vecCenter;
		b=vecPolyInter*(vecOriginCenter);
		c=vecOriginCenter*vecOriginCenter-(fRad*fRad);
		if((b*b-c)>0.0f)			
		{
			fInterD=-b-sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[1]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
			fInterD=-b+sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[1]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
		}

		vecPolyInter=pPoly[0]-pPoly[2];
		fMaxInter=vecPolyInter.GetLens();
		vecPolyInter.Normalize();

		vecOriginCenter=pPoly[2]-vecCenter;
		b=vecPolyInter*(vecOriginCenter);
		c=vecOriginCenter*vecOriginCenter-(fRad*fRad);
		if((b*b-c)>0.0f)
		{
			fInterD=-b-sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[2]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
			fInterD=-b+sqrtf(b*b-c);
			if(fInterD >= 0.0f && fInterD <= fMaxInter)
			{
				*vecIntersect=pPoly[2]+fInterD*vecPolyInter;
				vecIntersect++;
				cIntersect++;
			}
		}
		if(cIntersect!=0)
			return 2;

		if(PolygonRay(vecCenter,vecCenter+vecPolyNormal*fRad,pTestPoly,fInter)==1)
		{
			*vecIntersect=vecCenter+vecPolyNormal*fInter;
			vecIntersect++;
			cIntersect++;
			return 3;
		}
		*/
		return 0;
	}	
	return 0;
}

float CIntersection::PlaneAABBBox(vector3 *vecPlane, vector3 &vecNormal, vector3 &vecMaxBox, vector3 &vecMinBox)
{
	vector3 vecNearPoint;
	if(vecNormal.x > 0.0f)
	{
		if(vecNormal.y > 0.0f)
		{
			if(vecNormal.z > 0.0f)
			{
				vecNearPoint.x=vecMinBox.x;vecNearPoint.y=vecMinBox.y;vecNearPoint.z=vecMinBox.z;
			}
			else
			{
				vecNearPoint.x=vecMinBox.x;vecNearPoint.y=vecMinBox.y;vecNearPoint.z=vecMaxBox.z;
			}
		}
		else
		{
			if(vecNormal.z > 0.0f)
			{
				vecNearPoint.x=vecMinBox.x;vecNearPoint.y=vecMaxBox.y;vecNearPoint.z=vecMinBox.z;
			}
			else
			{
				vecNearPoint.x=vecMinBox.x;vecNearPoint.y=vecMaxBox.y;vecNearPoint.z=vecMaxBox.z;
			}
		}
	}
	else
	{
		if(vecNormal.y > 0.0f)
		{
			if(vecNormal.z > 0.0f)
			{
				vecNearPoint.x=vecMaxBox.x;vecNearPoint.y=vecMinBox.y;vecNearPoint.z=vecMinBox.z;
			}
			else
			{
				vecNearPoint.x=vecMaxBox.x;vecNearPoint.y=vecMinBox.y;vecNearPoint.z=vecMaxBox.z;
			}
		}
		else
		{
			if(vecNormal.z > 0.0f)
			{
				vecNearPoint.x=vecMaxBox.x;vecNearPoint.y=vecMaxBox.y;vecNearPoint.z=vecMinBox.z;
			}
			else
			{
				vecNearPoint.x=vecMaxBox.x;vecNearPoint.y=vecMaxBox.y;vecNearPoint.z=vecMaxBox.z;
			}
		}
	}	
	return vecNormal*(vecNearPoint-vecPlane[0]);	
}

float CIntersection::PointFromPlane(vector3 *pPlane, vector3 &vecPoint)
{
	vector3 vecPolyNormal=(pPlane[1]-pPlane[0])^(pPlane[2]-pPlane[1]);	
	vecPolyNormal.Normalize();
	
	return vecPolyNormal*(vecPoint-pPlane[0]);
}

float CIntersection::PointFromPlane(vector3 &pNormal, vector3 &pPlaneOrigin, vector3 &vecPoint)
{
	return pNormal*(vecPoint-pPlaneOrigin);
}

int CIntersection::PlaneSphereNormal(vector3 &vecPos, float &fRad, vector3 &vecNormal, vector3 &vecPoly)
{
	float x4,y4,z4;
	x4=vecPos.x-vecPoly.x;
	y4=vecPos.y-vecPoly.y;
	z4=vecPos.z-vecPoly.z;

	float fDistance=vecNormal.x*x4+vecNormal.y*y4+vecNormal.z*z4;

	float fAbsDistance=fDistance>0.0f ? fDistance : -fDistance;
	if(fRad >= fAbsDistance)
	{
		return 1;
	}
	else
	{
		if(fDistance>0.0f)
			return 0;
		else
			return -1;
	}	
}
