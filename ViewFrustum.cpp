// ViewFrustum.cpp: implementation of the CViewFrustum class.
//
//////////////////////////////////////////////////////////////////////

#include "ViewFrustum.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewFrustum::CViewFrustum()
{

}

CViewFrustum::~CViewFrustum()
{

}
void CViewFrustum::Update() {
	
	LPDIRECT3DDEVICE8 device;
	D3DXMATRIX view;
	D3DXMATRIX project;
	D3DXMATRIX clip;

	device = CSceneManager::GetDevice();
	device->GetTransform(D3DTS_VIEW,&view);
	device->GetTransform(D3DTS_PROJECTION,&project);
	D3DXMatrixMultiply(&clip,&view,&project);
	//left plane
	m_Frustum[LEFT_P].a = clip._14 + clip._11;
	m_Frustum[LEFT_P].b = clip._24 + clip._21;
	m_Frustum[LEFT_P].c = clip._34 + clip._31;
	m_Frustum[LEFT_P].d = clip._44 + clip._41;
	//right plane
	m_Frustum[RIGHT_P].a = clip._14 - clip._11;
	m_Frustum[RIGHT_P].b = clip._24 - clip._21;
	m_Frustum[RIGHT_P].c = clip._34 - clip._31;
	m_Frustum[RIGHT_P].d = clip._44 - clip._41;
	//top plane
	m_Frustum[TOP_P].a = clip._14 - clip._12;
	m_Frustum[TOP_P].b = clip._24 - clip._22;
	m_Frustum[TOP_P].c = clip._34 - clip._32;
	m_Frustum[TOP_P].d = clip._44 - clip._42;
	//bottom plane
	m_Frustum[BOTTOM_P].a = clip._14 + clip._12;
	m_Frustum[BOTTOM_P].b = clip._24 + clip._22;
	m_Frustum[BOTTOM_P].c = clip._34 + clip._32;
	m_Frustum[BOTTOM_P].d = clip._44 + clip._42;
	//near plane
	m_Frustum[NEAR_P].a = clip._13;
	m_Frustum[NEAR_P].b = clip._23;
	m_Frustum[NEAR_P].c = clip._33;
	m_Frustum[NEAR_P].d = clip._43;
	//far plane
	m_Frustum[FAR_P].a = clip._14 - clip._13;
	m_Frustum[FAR_P].b = clip._24 - clip._23;
	m_Frustum[FAR_P].c = clip._34 - clip._33;
	m_Frustum[FAR_P].d = clip._44 - clip._43;

	//normalize plane
	for(int i=0;i<6;i++) 
		D3DXPlaneNormalize(&(m_Frustum[i]),&(m_Frustum[i]));


}
bool CViewFrustum::PointInFrustum(float x,float y,float z) {

	for(int i=0;i<6;i++) {
		float fvalue = m_Frustum[i].a * x + m_Frustum[i].b * y + m_Frustum[i].c * z + m_Frustum[i].d;

		if(fvalue <= 0)
			return false; //point not in frustum
	}
	// point in frustum
	return true;

}		
bool CViewFrustum::SphereInFrustum(float x,float y,float z,float rad){

	for(int i=0;i<6;i++) {
		float fvalue = m_Frustum[i].a * x + m_Frustum[i].b * y + m_Frustum[i].c * z + m_Frustum[i].d;

		if(fvalue <= -rad)
			return false; //Sphere not in frustum
	}
	// Sphere in frustum
	return true;

}
bool CViewFrustum::BoxInFrustum(int *vecMin,int *vecMax) {
	for(int i = 0; i < 6; i++ )
	{
		if(m_Frustum[i].a * vecMin[0]  + m_Frustum[i].b * vecMin[1]  + m_Frustum[i].c * vecMin[2]  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax[0] + m_Frustum[i].b * vecMin[1]  + m_Frustum[i].c * vecMin[2]  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin[0]  + m_Frustum[i].b * vecMax[1] + m_Frustum[i].c * vecMin[2]  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax[0] + m_Frustum[i].b * vecMax[1] + m_Frustum[i].c * vecMin[2]  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin[0]  + m_Frustum[i].b * vecMin[1]  + m_Frustum[i].c * vecMax[2] + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax[0] + m_Frustum[i].b * vecMin[1]  + m_Frustum[i].c * vecMax[2] + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin[0]  + m_Frustum[i].b * vecMax[1] + m_Frustum[i].c * vecMax[2] + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax[0] + m_Frustum[i].b * vecMax[1] + m_Frustum[i].c * vecMax[2] + m_Frustum[i].d > 0)  continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// Return a true for the box being inside of the frustum
	return true;

}
bool CViewFrustum::BoxInFrustum(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax) {
	for(int i = 0; i < 6; i++ )
	{
		if(m_Frustum[i].a * vecMin.x  + m_Frustum[i].b * vecMin.y  + m_Frustum[i].c * vecMin.z  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax.x + m_Frustum[i].b * vecMin.y  + m_Frustum[i].c * vecMin.z  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin.x  + m_Frustum[i].b * vecMax.y + m_Frustum[i].c * vecMin.z  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax.x + m_Frustum[i].b * vecMax.y + m_Frustum[i].c * vecMin.z  + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin.x  + m_Frustum[i].b * vecMin.y  + m_Frustum[i].c * vecMax.z + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax.x + m_Frustum[i].b * vecMin.y  + m_Frustum[i].c * vecMax.z + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMin.x  + m_Frustum[i].b * vecMax.y + m_Frustum[i].c * vecMax.z + m_Frustum[i].d > 0)  continue;
		if(m_Frustum[i].a * vecMax.x + m_Frustum[i].b * vecMax.y + m_Frustum[i].c * vecMax.z + m_Frustum[i].d > 0)  continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// Return a true for the box being inside of the frustum
	return true;
}
bool CViewFrustum::BoxInFrustum(RCullingBox *BoundB) {

	BYTE bOutside[8];

	ZeroMemory( &bOutside, sizeof(bOutside) );

	//bound box check in frustum
	for( int iPoint = 0; iPoint < 8; iPoint++) {
        for( int iPlane = 0; iPlane < 6; iPlane++) {
            // bound box 점이 frustum plane 외부에 있을때
			if((m_Frustum[iPlane].a * (BoundB->m_WorldBound[iPoint].x) + 
				m_Frustum[iPlane].b * (BoundB->m_WorldBound[iPoint].y) +
				m_Frustum[iPlane].c * (BoundB->m_WorldBound[iPoint].z) + 
				m_Frustum[iPlane].d) < 0) {
                //외부에 존재 할때 1 bit setting 
				bOutside[iPoint] |= (1 << iPlane);
            }
        }
        // bound box inside frustum.
        if( bOutside[iPoint] == 0 )
            return true;
    }
	return false;
}

