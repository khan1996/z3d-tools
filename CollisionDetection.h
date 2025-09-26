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

	// build ��� ����.
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
	vector3 m_vecSourcePoint;	// �浹üũ�� ���ϴ� ��ġ. GetPosition ���� ���� ��ġ������ ����. ���� �����Լ����� ���� �Ķ���� ����.
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

	// 2005.01.11 yundi : Octree �ε��� �浹ó�� ��ø��� ���� �Լ� �߰�
	void LockDetection();
	void UnlockDetection();
	bool IsDetectionLocked();

	// 2005.12.19 yundi : ��� �ߺ��ڵ带 utility function ���� extract method

	/*!
		(pointX, pointY) �� ���� (minX, minY)�� �������� sizeX, sizeY ��
		���μ��� ũ�⸦ ������ �簢�� ���� ���ԵǴ��� üũ.

		��, üũ������ �ּҰ� <= ���� < �ִ밪 ���´�(�̻�/�̸� ��)

		���Ե� ��� true, �׷��� ���� ��� false ��ȯ
	*/
	static bool IsPointInRect2D(float pointX, float pointY, float minX, float minY, float sizeX, float sizeY);


protected:
	int m_nLockCount;
};

#endif // !defined(AFX_COLLISIONDETECTION_H__747BC78B_816E_4F60_A14B_A17C5A7C63B6__INCLUDED_)
