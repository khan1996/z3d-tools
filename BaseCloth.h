#pragma once

#include "d3dx8.h"
#include <vector>
using namespace std; 

enum CLOTH_FORM_TYPE
{
	CFT_QUAD, 
	CFT_TRAPEZOID
};

class CClothProperty
{
public:
	int		m_iWidth; 
	int		m_iHeight; 
	int		m_iRow; 
	int		m_iCol; 
	float	m_fMass; 
	float	m_fGravity; 
	float	m_fSpringTensionConstant; 
	float m_fSpringShearConstant; 
	float	m_fSpringDampingConstant; 
	float	m_fDragCoefficient; 
	float	m_fCollisionToLerance; 
	float	m_fKrestitution; 
	float	m_fFrictionfactor; 
	CLOTH_FORM_TYPE m_FormType; 	

public:
	CClothProperty() {}
	virtual ~CClothProperty() {} 

	void Init( int iWidth, int iHeight, int iRow, int iCol, CLOTH_FORM_TYPE Type )
	{
		m_iWidth = iWidth; 
		m_iHeight = iHeight;  
		m_iRow = iRow;  //세로가 몇칸인가
		m_iCol = iCol;    //가로가 몇칸인가
		m_fMass = 50.0f;  
		m_fGravity = -32.174f; 
		m_fSpringTensionConstant = 400.0f; 
		m_fSpringShearConstant = 400.0f; 
		m_fSpringDampingConstant = 1.0f; 
		m_fDragCoefficient = 0.05f; 
		m_fCollisionToLerance = 0.05f; 
		m_fKrestitution = 0.25f; 
		m_fFrictionfactor = 0.5f; 
		m_FormType = Type; 
	}
};

class CBaseCloth
{
protected:
	struct ParticleRef
	{
		int r;  //행
		int c; //열
	};

	struct Particle
	{
		float						fMass; 
		float						fInvMass; 
		D3DXVECTOR3		vPos; 
		D3DXVECTOR3		vVel; 
		D3DXVECTOR3		vAccel; 
		D3DXVECTOR3		vForce; 
		bool						bLocked; 
		ParticleRef			RC;  
	};

	struct Spring
	{
		ParticleRef	p1; //연결입자 1번
		ParticleRef	p2; //연결입자 2번
		float				k; //탄성계수
		float				d; //감쇠인수
		float				L; //원래길이
	};

	struct CollisionSphere
	{
		float fRadius; 
		D3DXVECTOR3 vPos; 
	};

	typedef vector<Particle*> PARTICLEVECTOR;

public:
	Particle*				m_pParticles; 
	Spring*				m_pSprings; 
	int							m_iNumSpring; 
	int							m_iNumFace; 
	int							m_iNumVertex; 
	float						m_fRStep; 
	float						m_fCStep; 
	
	PARTICLEVECTOR		m_LockedParticleList; 
	CClothProperty				m_Prop; 

protected:
	inline int ROWCOL( int r, int c );

public:
	CBaseCloth( const CClothProperty* pProp ); 
	virtual ~CBaseCloth(void); 

	virtual void Render() = 0; 
	virtual void Update( float dt=0.025f ) = 0; 
	void LockParticle( int iRow, int iCol ); 
	void UnLockParticle( int iRow, int iCol ); 
	inline void SetParticlePos( int iRow, int iCol, const D3DXVECTOR3& vPos ); 
	inline void GetParticlePos( int iRow, int iCol, D3DXVECTOR3& vPos ); 
};

inline int CBaseCloth::ROWCOL( int r, int c )
{
	return (r*(m_Prop.m_iCol+1)+c);
}

inline void CBaseCloth::SetParticlePos( int iRow, int iCol, const D3DXVECTOR3& vPos )
{
	m_pParticles[ROWCOL(iRow,iCol)].vPos = vPos;
}

inline void CBaseCloth::GetParticlePos( int iRow, int iCol, D3DXVECTOR3& vPos )
{
	vPos = m_pParticles[ROWCOL(iRow,iCol)].vPos; 
}


