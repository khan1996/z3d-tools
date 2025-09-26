// CollisionDetection.h: interface for the CCollisionDetection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISIONDETECTION_H__747BC78B_816E_4F60_A14B_A17C5A7C63B6__INCLUDED_)
#define AFX_COLLISIONDETECTION_H__747BC78B_816E_4F60_A14B_A17C5A7C63B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include "HeightFieldScene.h"

enum	CollisionType	
{
	CT_BOTTOM = 0,
	CT_SIDE,
	CT_TERRAIN,
	CT_CHRSIDE,
	CT_NONE, 
	CT_WATER
};
#define COLLEPSILON 0.000001f


class CCollisionDetection  
{
	class CCollisionList 
	{
	public:
		float m_fDistance;
		vector3 m_vecIntersectionPoint;		
		vector3 m_vecPlaneNormal;
		vector3 m_vecPlanePoint;
		CollisionType m_CollisionType;
	};
public:	
	vector3 m_vecMinMove,m_vecMaxMove;

	float m_fMinYBound,m_fMaxYBound;
    bool  m_bFirstBottom;   // First Bottom.
	bool  m_bFirstScene;

	// build 계산 관련.
	bool m_bBuild;



	bool m_CollisionDetectionDetail;
	void CheckInHouseObject(CHouseObject *pHouse,matrix matParent);
	void CheckSectorHeightCollision(CollisionType &CT);
	void CheckBspCollision();
	bool CheckSectorWater(vector3& vecPos,bool bUpper);
	void CheckCharacterCollision(bool bChrMode);
	void CheckSectorPlantCollision();
	void CheckSectorMeshCollision();
	std::vector<CCollisionList> m_CollisionList;
	vector3 CollideWithWorld(vector3 vecPos,vector3 vecVelocity,vector3 vecBefore,int Depth,CollisionType &CT,bool ChrCollision);
	vector3 GetPosition(vector3 vecPos,vector3 vecVelocity,vector3 vecBefore,CollisionType &CT,bool ChrCollision);
	void CheckHouseCollision();
    void CheckFirstHouseCollision();

	vector3 m_vecVelocity;
	vector3 m_vecSourcePoint;	// 충돌체크를 원하는 위치. GetPosition 에서 받은 위치값으로 세팅. 이후 내부함수들을 위한 파라미터 역할.
	vector3 m_vecRadius;
	vector3 m_vecLastSafePosition;
	bool m_bStuck;
	vector3 m_vecStuckPlaneNormal;
	float m_fStuckDistance;
	bool m_bFoundCollision;	
	matrix m_matInHouse;
	bool m_bHeightGradient;

	int m_SelfChr;
	CHeightFieldScene *m_HeightField;
	CCollisionDetection();
	void SetZoneBound(float min,float max);				// Zone Bound Y value
	void ZoneBoundApply(vector3 &vecPoint,CollisionType &);
    void SetFirstBottom(bool bEnable) {m_bFirstBottom = bEnable; }
	bool GetBuildEnable() { return m_bBuild;}
	bool GetHeightGradient() { return m_bHeightGradient;}

	virtual ~CCollisionDetection();

	// 2005.01.11 yundi : Octree 로딩중 충돌처리 잠시멈춤 관련 함수 추가
	void LockDetection();
	void UnlockDetection();
	bool IsDetectionLocked();

	// 2005.12.19 yundi : 몇몇 중복코드를 utility function 으로 extract method

	/*!
		(pointX, pointY) 의 점이 (minX, minY)를 기준으로 sizeX, sizeY 의
		가로세로 크기를 가지는 사각형 내에 포함되는지 체크.

		단, 체크범위는 최소값 <= 포함 < 최대값 형태다(이상/미만 임)

		포함될 경우 true, 그렇지 않을 경우 false 반환
	*/
	static bool IsPointInRect2D(float pointX, float pointY, float minX, float minY, float sizeX, float sizeY);


protected:
	int m_nLockCount;
};

#endif // !defined(AFX_COLLISIONDETECTION_H__747BC78B_816E_4F60_A14B_A17C5A7C63B6__INCLUDED_)
