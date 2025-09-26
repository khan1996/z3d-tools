#pragma once

#include "BaseCloth.h"
#include "Z3DGeneralChrModel.h"

#define ADDVALUE_POS 0.95f
#define ADDVALUE_ROT 0.96f
#define ADDVALUE_TOTAL 1.2f

#define CCR_HUMAN_MAN 45
#define CCR_HUMAN_WOMAN 45
#define CCR_AKHAN_MAN 45
#define CCR_AKHAN_WOMAN 45

class CCharacterCape : public CBaseCloth
{
public:
	enum CHARACTER_TYPE
	{
		HUMAN_MAN, 
		HUMAN_WOMAN, 
		AKHAN_MAN, 
		AKHAN_WOMAN, 
		CHARACTER_TYPE_COUNT
	};

public:
	LPD3DXMESH							m_pSphere; 
	D3DXVECTOR3							m_vMoveValue; 
	float											m_fRotateValue; 
	bool											m_bTestMode; 
	CZ3DGeneralChrModel*			m_pChrModel; 
	D3DXVECTOR3							m_vFixedPoint; 
	vector3										m_OldPointPos; 
	vector3										m_OldPointPos2;  
	vector3										m_CurPointPos;  
	vector3										m_CurPointPos2;  
	D3DXVECTOR3							m_vOldPos; 
	D3DXVECTOR3							m_vCurPos; 
	float											m_fOldRot; 
	float											m_fCurRot; 
	LPDIRECT3DTEXTURE8				m_pBackTexture; 
	LPDIRECT3DTEXTURE8				m_pDecalTexture;
	bool											m_bEnable; 

	CollisionSphere						m_CollisionSphere; 
	char											m_strCapeNationalFlag[MAX_PATH]; 
	char											m_strCapeBackground[MAX_PATH]; 
	static float									s_fChrTypeCollisionRadius[CHARACTER_TYPE_COUNT]; 

protected:
	void CalculateForce(); 
	void 	Integrate( float dt ); 
	void MoveLockedParticle( const D3DXVECTOR3& vPos ); 
	void RotateLockedParticle( float fFrameRot ); 

public:
	CCharacterCape(const CClothProperty* pProp, bool bTest = false );
	~CCharacterCape(void);

	virtual void Update( float dt = 0.025f ); 
	virtual void Render(); 
	
	void Create( CHARACTER_TYPE Type, CZ3DGeneralChrModel* pChrModel=NULL ); 
	void RenderShadow(); 
	void DrawCollisionSphere(); 
	void ResetParticle(); 

	//inline 
	void SetMoveValue( const D3DXVECTOR3& vPos );
	void SetRotateValue( float fRot ); 
	void SetCollisionSpherePos( const D3DXVECTOR3& vPos ); 
	void SetCollisionSphereRadius( float fRadius ); 
	float GetCollisionSphereRadius() const; 
	void SetModel( CZ3DGeneralChrModel* pModel ); 
	void SetCapeNationalFlagTexture( const char* strFileName ); 
	void SetCapeBackgroundTexture( const char* strFileName ); 
	void SetEnable( bool bIsEnable ); 
};

/************************************************************************/
/* Inline                                                                                                                 */
/************************************************************************/
inline void CCharacterCape::MoveLockedParticle( const D3DXVECTOR3& vMoveValue )
{
	m_vMoveValue = vMoveValue; 
	for( unsigned int i = 0; i < m_LockedParticleList.size(); i++ )
	{
		m_LockedParticleList[i]->vPos += m_vMoveValue; 
	}
}

inline void CCharacterCape::RotateLockedParticle( float fFrameRot )
{
	m_fRotateValue = fFrameRot; 
}

inline void CCharacterCape::SetMoveValue( const D3DXVECTOR3& vPos )
{
	m_vOldPos = m_vCurPos; 
	m_vCurPos = vPos; 
	m_vMoveValue = m_vCurPos - m_vOldPos; 
}

inline void CCharacterCape::SetRotateValue( float fRot )
{
	m_fOldRot = m_fCurRot; 
	m_fCurRot = fRot; 
	m_fRotateValue = m_fCurRot - m_fOldRot; 
}

inline void CCharacterCape::SetCollisionSpherePos( const D3DXVECTOR3& vPos )
{
	m_CollisionSphere.vPos = vPos; 
}

inline void CCharacterCape::SetCollisionSphereRadius( float fRadius )
{
	m_CollisionSphere.fRadius = fRadius; 
}

inline float CCharacterCape::GetCollisionSphereRadius() const 
{
	return m_CollisionSphere.fRadius; 
}

inline void CCharacterCape::SetModel( CZ3DGeneralChrModel* pModel )
{
	m_pChrModel = pModel; 
}

inline void CCharacterCape::SetCapeBackgroundTexture( const char* strFileName )
{
	strcpy( m_strCapeBackground, strFileName ); 
}

inline void CCharacterCape::SetCapeNationalFlagTexture( const char* strFileName )
{
	strcpy( m_strCapeNationalFlag, strFileName ); 
}

inline void CCharacterCape::SetEnable( bool bIsEnable )
{
	m_bEnable = bIsEnable; 
}