// BoidScene.h: interface for the CBoidScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOIDSCENE_H__C74FEBE3_6C82_40E8_8862_3914033E05E0__INCLUDED_)
#define AFX_BOIDSCENE_H__C74FEBE3_6C82_40E8_8862_3914033E05E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include "MeshObject.h"

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
#define rnd()  (((FLOAT)rand() ) / RAND_MAX)

class CBoidScene  
{
	class BoidNode
	{
	public:
		vector3 m_vecLoc;
		vector3 m_vecDir;
		vector3 m_vecDeltaPos;
		vector3 m_vecDeltaDir;
		int m_DeltaCnt;
		float m_fSpeed;
		float m_fYaw,m_fPitch,m_fRoll,m_fDYaw;
		color m_color;
		float *m_fDist;
		matrix m_matLocal;
		float m_fAni;
	};	
	CMeshObject *m_pBoidObject[2];

public:
	void DeleteBoid();

	int m_nBoids;
	int m_BoidObject;

	float m_InfluenceRadius;
	float m_InfluenceRadiusSquared;
	float m_CollisionFraction;
	float m_InvCollisionFraction;
	float m_NormalSpeed;
	float m_AngleTweak;
	float m_PitchToSpeedRatio;

	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void UpdateFlock();
	void Update(float fUpdate);
	vector3 m_vecGoal;
	void Create(int nBoid,int BoidObject);
	BoidNode *m_pBoids;
	CBoidScene();
	virtual ~CBoidScene();

};

#endif // !defined(AFX_BOIDSCENE_H__C74FEBE3_6C82_40E8_8862_3914033E05E0__INCLUDED_)
