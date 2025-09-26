// ViewFrustum.h: interface for the CViewFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWFRUSTUM_H__3771772E_0DF5_416F_8B98_F4ECC3232034__INCLUDED_)
#define AFX_VIEWFRUSTUM_H__3771772E_0DF5_416F_8B98_F4ECC3232034__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3dx8.h>

enum WBPLAN { LEFT_P,RIGHT_P,TOP_P,BOTTOM_P,NEAR_P,FAR_P};
enum WBAXIS {X,Y,Z};

class RCullingBox {
public:
	D3DXVECTOR3 m_Pos;
	float m_Rot[3];
	
	D3DXMATRIX m_matWorld; // local 좌표계 box를 world 좌표계로 변화 시키는 matrix
	D3DXVECTOR3 m_LocalBound[8];
	D3DXVECTOR3 m_WorldBound[8];
	bool m_BoxSide;	//in 인지 out 인지.

	RCullingBox() {
		m_Rot[X] = m_Rot[Y] = m_Rot[Z] = 0.0f;
		m_BoxSide = false;
	}
	~RCullingBox(){}
	void ChangeLocalToWorld() {
		// Recompute m_mat, m_vecBoundsWorld, and m_planeBoundsWorld 
        // when the thing's position, orientation, or bounding box has changed
        D3DXMATRIX matRotX,matRotY,matRotZ;
		D3DXMATRIX matTrans;

        D3DXMatrixRotationX( &matRotX, m_Rot[X] );
        D3DXMatrixRotationY( &matRotY, m_Rot[Y] );
		D3DXMatrixRotationZ( &matRotZ, m_Rot[Z] );

        D3DXMatrixTranslation( &matTrans, m_Pos.x, m_Pos.y, m_Pos.z );
        
		m_matWorld = matRotX * matRotZ * matRotY * matTrans;

        // Transform bounding box coords from local space to world space
        for( int i = 0; i < 8; i++ )
            D3DXVec3TransformCoord( &m_WorldBound[i], &m_LocalBound[i], &m_matWorld ); 

	}


};

class CViewFrustum{
	public:
		CViewFrustum();
		~CViewFrustum();
		void Update();

		bool PointInFrustum(float x,float y,float z);
		bool SphereInFrustum(float x,float y,float z,float rad);
		bool BoxInFrustum(RCullingBox *);
		bool BoxInFrustum(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax);
		bool BoxInFrustum(int *vecMin,int *vecMax);
	private:
		D3DXPLANE m_Frustum[6];

};

#endif // !defined(AFX_VIEWFRUSTUM_H__3771772E_0DF5_416F_8B98_F4ECC3232034__INCLUDED_)
