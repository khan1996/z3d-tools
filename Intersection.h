// Intersection.h: interface for the CIntersection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERSECTION_H__2C579563_8B13_41AA_BD24_5DF348664376__INCLUDED_)
#define AFX_INTERSECTION_H__2C579563_8B13_41AA_BD24_5DF348664376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "3DMath.h"


class CIntersection  
{
public:
	static int PlaneSphereNormal(vector3 &vecPos,float &fRad,vector3 &vecNormal,vector3 &vecPoly);
	static float PointFromPlane(vector3 &pNormal,vector3 &pPlaneOrigin,vector3 &vecPoint);
	static float PointFromPlane(vector3 *pPlane,vector3 &vecPoint);
	static float PlaneAABBBox(vector3 *vecPlane,vector3 &vecNormal,vector3 &vecMaxBox,vector3 &vecMinBox);
	static int PolygonSphere(vector3 &vecCenter,float fRad,vector3 *pPoly,vector3 *vecIntersect,int &cIntersect);
	static int SplitPolygonPolygon(vector3 *pPolySour,vector3 *pPolyDest,vector3 *pPolyResult);
	static int PlaneSphere(vector3 &vecLoc,float &fRad,vector3 *pPoly);
	static float PolygonRay2(vector3 vecStart, vector3 vecEnd, vector3 *poly,float &fInterLens);
	static int PolygonQuad(vector3 *vecPoly,vector3 *vecQuad);
	static bool PlanePoint(vector3 *pPlane,vector3 &vecPoint);
	static int BoxToRay(vector3 vecStart,vector3 vecEnd,vector3 *poly[4],float &fIntersection);
	static int PolygonToPolygon(vector3 *vecPoly1,vector3 *vecPoly2);
	static int PolygonRay(vector3 vecStart, vector3 vecEnd, vector3 *poly,float &fInterLens);
	CIntersection();
	virtual ~CIntersection();
};

#endif // !defined(AFX_INTERSECTION_H__2C579563_8B13_41AA_BD24_5DF348664376__INCLUDED_)
