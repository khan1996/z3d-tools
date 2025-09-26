#pragma once

#include "d3dx8.h"
#include <map>
#include <vector>
using namespace std; 

#define		CLOTHWIDTH	40
#define		CLOTHHEIGHT	40
#define		NUMCOLUMNS	4
#define		NUMROWS		4
#define		NUMVERTICES	((NUMCOLUMNS+1) * (NUMROWS+1))
#define		NUMFACES	((NUMCOLUMNS*NUMROWS) * 2)
#define		CLOTHMASS	50.0f
#define		MASSPERFACE	(CLOTHMASS/(float) NUMFACES)
#define		CSTEP		((float) CLOTHWIDTH / (float) NUMCOLUMNS)
#define		RSTEP		((float) CLOTHHEIGHT / (float) NUMROWS)
#define		NUMSTRUCTURALSPRINGS		(NUMCOLUMNS*(NUMROWS+1) + NUMROWS*(NUMCOLUMNS+1) + NUMCOLUMNS*NUMROWS*2)
#define		GRAVITY					-32.174
#define		SPRINGTENSIONCONSTANT  500.0f
#define		SPRINGSHEARCONSTANT		600.0f //600
#define		SPRINGDAMPINGCONSTANT	1 //2
#define		DRAGCOEFFICIENT			0.05f
#define		COLLISIONTOLERANCE		0.05f
#define		KRESTITUTION			0.25f
#define		FRICTIONFACTOR			0.5f

class CClothSimulation
{
public:
	enum eSIMULATIONTYPE
	{
		NOTHING, 
		MOVE_FRONT, 
		MOVE_BACK, 
		MOVE_RIGHT,
		MOVE_LEFT, 
		ROTATE_FRONT, 
		ROTATE_BACK, 
		ROTATE_RIGHT, 
		ROTATE_LEFT, 
		JUMP_UP, 
		JUMP_DOWN, 
		SWIMMING, 
	};

	struct ClothVertex
	{
		D3DXVECTOR3 pos; 
		float u, v; 
		static const DWORD FVF; 
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
	};

	struct ParticleRef
	{
		int r;  //행
		int c; //열
	};

	struct Spring
	{
		ParticleRef	p1; //연결입자 1번
		ParticleRef	p2; //연결입자 2번
		float				k; //탄성계수
		float				d; //감쇠인수
		float				L; //원래길이
	};
	
	struct	Collision 
	{	
		ParticleRef			p1;
		D3DXVECTOR3		n; //충돌법선
	};

	struct CollisionBox
	{
		D3DXVECTOR3 vMin; 
		D3DXVECTOR3 vMax; 
		D3DXVECTOR3 vCenter; 
		D3DXVECTOR3 vCollisionNormal; 
		float					fCollisionConstant; 
		
		void Init( const D3DXVECTOR3& vPos, const D3DXVECTOR3& vNormal, float fConstant, float fWidth, float fHeight, float fDepth )
		{
			vCenter = vPos; 
			vMin = D3DXVECTOR3( vPos.x-(fWidth/2), vPos.y-(fHeight/2), vPos.z-(fDepth/2) );
			vMax = D3DXVECTOR3( vPos.x+(fWidth/2), vPos.y+(fHeight/2), vPos.z+(fDepth/2) );
			vCollisionNormal = vNormal; 
			fCollisionConstant = fConstant; 
		}
	};

	typedef map<Particle*, ParticleRef> PARTICLEMAP;

public:
	LPDIRECT3DDEVICE8				m_pd3dDevice; 
	LPDIRECT3DVERTEXBUFFER8	m_pVB; 
	LPDIRECT3DINDEXBUFFER8		m_pIB; 
	LPDIRECT3DTEXTURE8				m_pTexture; 

	Particle					m_Particles[NUMROWS+1][NUMCOLUMNS+1];
	Spring					m_StructuralSprings[NUMSTRUCTURALSPRINGS];
	Collision				m_Collisions[NUMVERTICES];
	ClothVertex			m_ClothVertex[NUMVERTICES]; 
	WORD					m_ClothIndex[NUMROWS*NUMCOLUMNS*2*3]; 
	PARTICLEMAP    m_LockedParticleMap; 
	D3DXVECTOR3		m_vWindDirection;  
	int							m_iWindForce; 
	int							m_iNumRow; 
	int							m_iNumCol; 
	int							m_iWidth; 
	int							m_iHeight; 
	int							m_iNumVertex; 
	float						m_fClothMass; 
	CollisionBox			m_CollisionBox; 
	D3DXVECTOR3		m_vVirtualSimulation; 
	
	D3DXVECTOR3		m_vMoveParticle; 
	float						m_fRotateParticle; 
	D3DXVECTOR3    m_vWorldPos; 

	D3DXVECTOR3		m_vSavePoint, m_vSavePoint2; //임시로 만들어논 테스트변수

	D3DXVECTOR3		m_vMoveValue; 

protected:
	void CheckCollision(); 
	void CalculateForce(); 
	void 	Integrate( float dt ); 
	void ResolveCollisions(); 
	int RandNum( int number ); 

public:
	CClothSimulation(void);
	virtual ~CClothSimulation(void);

	void Render(); 
	void Create( LPDIRECT3DDEVICE8 pd3dDevice, int iNumRow, int iNumCol, int iWidth, int iHeight ); 
	void AddCollisionPlane( const D3DXPLANE& Plane ); 
	void Update( float dt = 0.032f ); 
	void LockParticle( int iRow, int iCol ); 
	void UnLockParticle( int iRow, int iCol ); 
	void MoveLockedParticle( const D3DXVECTOR3& vPos ); 
	void RotateLockedParticle( const D3DXVECTOR3& vRot, int CenterRow, int CenterCol ); 
	void SetWindFactor( const D3DXVECTOR3& vDirection, int iForce ); 
	void VirtualSimulation( DWORD dwType ); 
	void SetPosLockParticle( int iRow, int iCol, const D3DXVECTOR3& vPos ); 
	void SetWorldPos( const D3DXVECTOR3& vPos ); 
};
