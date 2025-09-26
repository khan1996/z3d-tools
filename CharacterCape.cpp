#include ".\charactercape.h"
#include "dxutil.h"
#include "SceneManager.h"
#include "ClothMgr.h"
#include "GMMemory.h"

float CCharacterCape::s_fChrTypeCollisionRadius[CCharacterCape::CHARACTER_TYPE_COUNT] = 
{ 
	45.0f, 
	45.0f, 
	75.0f, 
	45.0f 
}; 

CCharacterCape::CCharacterCape(const CClothProperty* pProp, bool bTest ) : CBaseCloth(pProp), 
m_pSphere(NULL), m_vMoveValue(D3DXVECTOR3(0,0,0)), m_fRotateValue(0.0f), m_bTestMode(bTest), 
m_pChrModel(NULL), m_vFixedPoint(D3DXVECTOR3(0,0,0)), m_OldPointPos(vector3(0,0,0)), 
m_OldPointPos2(vector3(0,0,0)), m_CurPointPos(vector3(0,0,0)), m_CurPointPos2(vector3(0,0,0)), 
m_vOldPos(D3DXVECTOR3(0,0,0)), m_vCurPos(D3DXVECTOR3(0,0,0)), m_fOldRot(0), m_fCurRot(0), 
m_pBackTexture(NULL), m_pDecalTexture(NULL), m_bEnable(true)
{
	ZeroMemory( m_strCapeNationalFlag, sizeof(m_strCapeNationalFlag) ); 
	ZeroMemory( m_strCapeBackground, sizeof(m_strCapeBackground) ); 
	D3DXCreateTextureFromFile( CSceneManager::GetDevice(), "c:\\picture2.bmp", &m_pBackTexture );
	D3DXCreateTextureFromFile( CSceneManager::GetDevice(), "c:\\~picture1.bmp", &m_pDecalTexture );
}

CCharacterCape::~CCharacterCape(void)
{
	SAFE_RELEASE( m_pSphere ); 
	SAFE_RELEASE( m_pBackTexture ); 
	SAFE_RELEASE( m_pDecalTexture ); 
}

void CCharacterCape::Create( CHARACTER_TYPE Type, CZ3DGeneralChrModel* pChrModel )
{
	m_pChrModel = pChrModel; 

	m_CollisionSphere.fRadius = s_fChrTypeCollisionRadius[Type];
	m_CollisionSphere.vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
	
	if( m_bTestMode )
		D3DXCreateSphere( CSceneManager::GetDevice(), m_CollisionSphere.fRadius, 10, 10, &m_pSphere, NULL ); 

	LockParticle( 0, 0 );
	LockParticle( 0, 1 );
	LockParticle( 0, 2 );
	LockParticle( 0, 3 );
	LockParticle( 0, 4 );
}

void CCharacterCape::Render()
{
	if( !m_bEnable ) return; 

	CClothMgr::_Instance()->m_CapeRenderer.SetupCloth( this ); 
	LPDIRECT3DDEVICE8 pd3dDevice = CSceneManager::GetDevice(); 
	pd3dDevice->SetTexture( 0, m_pDecalTexture ); 
	pd3dDevice->LightEnable( 0, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//----------------------------------------------------------------------------
	/*
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD ); 
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE ); 
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	*/
	//----------------------------------------------------------------------------

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	CClothMgr::_Instance()->m_CapeRenderer.Render(); 
}

void CCharacterCape::Update( float dt )
{
	if( !m_bEnable ) return; 

	MoveLockedParticle( m_vMoveValue ); 
	RotateLockedParticle( m_fRotateValue ); 

	if( m_pChrModel )
	{
		vector3 CurPos = m_pChrModel->GetPosition();
		float fCurRot = m_pChrModel->GetDirection(); 

		m_OldPointPos = m_CurPointPos; 
		m_OldPointPos2 = m_CurPointPos2; 
		m_pChrModel->GetFixedPoint( 1, m_CurPointPos ); //오른쪽어깨.
		m_pChrModel->GetFixedPoint( 0, m_CurPointPos2 ); //왼쪽어깨.

		m_CurPointPos += CurPos; 
		m_CurPointPos2 += CurPos; 

		vector3 tempPP, tempPP2, tempMV; 
		tempPP = m_CurPointPos - CurPos; 
		tempPP2 = m_CurPointPos2 - CurPos;  
		tempMV = (tempPP+tempPP2)*0.5f + CurPos; 

		vector3 CSPoint; 
		m_pChrModel->GetFixedPoint( 2, CSPoint ); 
		CSPoint += CurPos; 

		SetCollisionSpherePos( D3DXVECTOR3(CSPoint.x, CSPoint.y, CSPoint.z) ); 
		SetMoveValue( D3DXVECTOR3(tempMV.x, tempMV.y, tempMV.z) ); 
		SetRotateValue( fCurRot ); 
		SetParticlePos( 0, 0, D3DXVECTOR3(m_CurPointPos2.x, m_CurPointPos2.y, m_CurPointPos2.z) ); 

		D3DXVECTOR3 vLerpPoint; 
		D3DXVec3Lerp( &vLerpPoint, &D3DXVECTOR3(m_CurPointPos2.x, m_CurPointPos2.y, m_CurPointPos2.z), 
			&D3DXVECTOR3(m_CurPointPos.x, m_CurPointPos.y, m_CurPointPos.z), 0.25 ); 
		SetParticlePos( 0, 1, vLerpPoint ); 
		D3DXVec3Lerp( &vLerpPoint, &D3DXVECTOR3(m_CurPointPos2.x, m_CurPointPos2.y, m_CurPointPos2.z), 
			&D3DXVECTOR3(m_CurPointPos.x, m_CurPointPos.y, m_CurPointPos.z), 0.5 ); 
		SetParticlePos( 0, 2, vLerpPoint ); 

		m_vFixedPoint = vLerpPoint; 

		D3DXVec3Lerp( &vLerpPoint, &D3DXVECTOR3(m_CurPointPos2.x, m_CurPointPos2.y, m_CurPointPos2.z), 
			&D3DXVECTOR3(m_CurPointPos.x, m_CurPointPos.y, m_CurPointPos.z), 0.75 ); 
		SetParticlePos( 0, 3, vLerpPoint ); 
		SetParticlePos( 0, 4, D3DXVECTOR3(m_CurPointPos.x, m_CurPointPos.y, m_CurPointPos.z) ); 
	}

	CalculateForce();
	Integrate( dt ); 

	CalculateForce();
	Integrate( dt ); 

	CalculateForce();
	Integrate( dt ); 

	CalculateForce();
	Integrate( dt ); 

	//튀는현상 혹은 부적절한 현상 제거부분
	D3DXVECTOR3 vDist = m_pParticles[ROWCOL(0, 0)].vPos - m_pParticles[ROWCOL(1, 0)].vPos; 
	if( D3DXVec3Length( &vDist ) > 50 )
	{
		ResetParticle(); 
	}
}

void CCharacterCape::CalculateForce()
{
	int	r, c, i, r1, c1, r2, c2;
	D3DXVECTOR3	dragVector;
	D3DXVECTOR3	f1, f2, d, v;
	float	L, tempValue;
	D3DXVECTOR3 vDist; 
	D3DXMATRIX matWorld, matTrans, matRot;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vAxis;

	m_fRotateValue *= ADDVALUE_ROT; 

	for(r=0; r<=m_Prop.m_iRow; r++)
	{
		for(c=0; c<=m_Prop.m_iCol; c++)
		{		
			if( !m_pParticles[ROWCOL(r,c)].bLocked )
			{
				m_pParticles[ROWCOL(r,c)].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 

				//gravity
				m_pParticles[ROWCOL(r,c)].vForce.y += (float)(m_Prop.m_fGravity * m_pParticles[ROWCOL(r,c)].fMass);

				// viscous drag
				dragVector = -m_pParticles[ROWCOL(r,c)].vVel;
				D3DXVec3Normalize( &dragVector, &dragVector ); 
				float tempValue = D3DXVec3Length( &m_pParticles[ROWCOL(r,c)].vVel ); 
				m_pParticles[ROWCOL(r,c)].vForce += dragVector * tempValue * tempValue * m_Prop.m_fDragCoefficient;

				//충돌
				vDist = m_pParticles[ROWCOL(r,c)].vPos - m_CollisionSphere.vPos; 
				if( D3DXVec3Length(&vDist) < m_CollisionSphere.fRadius )
				{
					D3DXVec3Normalize( &vDist, &vDist ); 
					vDist = vDist*600; 
					m_pParticles[ROWCOL(r,c)].vForce += vDist; 
				}

				m_pParticles[ROWCOL(r,c)].vPos += m_vMoveValue * ADDVALUE_POS;
				D3DXMatrixRotationY( &matRot, m_fRotateValue );
				vAxis = (m_pParticles[ROWCOL(0,0)].vPos + m_pParticles[ROWCOL(0,4)].vPos)/2;
				vTemp = m_pParticles[ROWCOL(r,c)].vPos - vAxis;
				D3DXMatrixTranslation( &matTrans, vTemp.x, vTemp.y, vTemp.z );
				D3DXMatrixMultiply( &matWorld, &matTrans, &matRot );
				vPos = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
				D3DXVec3TransformCoord( &vPos, &vPos, &matWorld );
				m_pParticles[ROWCOL(r,c)].vPos += vPos - vTemp;
			}
		}
	}

	m_vMoveValue = D3DXVECTOR3(0,0,0); 
	m_fRotateValue = 0; 

	// Process spring forces
	for(i = 0; i<m_iNumSpring; i++)
	{
		r1 = m_pSprings[i].p1.r;
		c1 = m_pSprings[i].p1.c;
		r2 = m_pSprings[i].p2.r;
		c2 = m_pSprings[i].p2.c;

		d = m_pParticles[ROWCOL(r1,c1)].vPos - m_pParticles[ROWCOL(r2,c2)].vPos;
		v = m_pParticles[ROWCOL(r1,c1)].vVel - m_pParticles[ROWCOL(r2,c2)].vVel;
		L = m_pSprings[i].L;

		tempValue = D3DXVec3Length(&d); 

		f1 = -(m_pSprings[i].k * (tempValue - L) + 
			m_pSprings[i].d * ( D3DXVec3Dot(&v, &d) / tempValue )) * ( d / tempValue );
		f2 = -f1;

		if(m_pParticles[ROWCOL(r1,c1)].bLocked == FALSE)
			m_pParticles[ROWCOL(r1,c1)].vForce += f1;

		if(m_pParticles[ROWCOL(r2,c2)].bLocked == FALSE)
			m_pParticles[ROWCOL(r2,c2)].vForce += f2;
	}
}

void CCharacterCape::	Integrate( float dt )
{
	int iVertexIndex=0; 
	D3DXVECTOR3 vDist; 
	
	for( int r=0; r<=m_Prop.m_iRow; r++)
	{
		for(int c=0; c<=m_Prop.m_iCol; c++)
		{	
			if( !m_pParticles[ROWCOL(r,c)].bLocked )
			{
				m_pParticles[ROWCOL(r,c)].vAccel = m_pParticles[ROWCOL(r,c)].vForce * m_pParticles[ROWCOL(r,c)].fInvMass;
				m_pParticles[ROWCOL(r,c)].vVel += m_pParticles[ROWCOL(r,c)].vAccel * dt;
				m_pParticles[ROWCOL(r,c)].vPos += m_pParticles[ROWCOL(r,c)].vVel * dt * ADDVALUE_TOTAL; 

				//충돌방식1
				/*
				vDist = m_pParticles[ROWCOL(r,c)].vPos - m_CollisionSphere.vPos; 
				if( D3DXVec3Length(&vDist) < m_CollisionSphere.fRadius )
				{
					m_pParticles[ROWCOL(r,c)].vPos = vOldPos; 
					D3DXVec3Normalize( &vDist, &vDist ); 
					Vn = D3DXVec3Dot(&vDist, &m_pParticles[ROWCOL(r,c)].vVel) * vDist;
					Vt = m_pParticles[ROWCOL(r,c)].vVel - Vn;
					m_pParticles[ROWCOL(r,c)].vVel = (-(m_Prop.m_fKrestitution+1) * Vn) + (m_Prop.m_fFrictionfactor*Vt);
				}*/
				
				//충돌방식2
				vDist = m_pParticles[ROWCOL(r,c)].vPos - m_CollisionSphere.vPos; 
				if( D3DXVec3Length(&vDist) < m_CollisionSphere.fRadius )
				{
					D3DXVec3Normalize( &vDist, &vDist ); 
					vDist *= m_CollisionSphere.fRadius; 
					m_pParticles[ROWCOL(r,c)].vPos = m_CollisionSphere.vPos + vDist; 
					m_pParticles[ROWCOL(r,c)].vVel = D3DXVECTOR3(0,0,0); 
				}
			}
		}
	}
}

void CCharacterCape::ResetParticle()
{
	D3DXMATRIX matWorld, matTrans, matRot; 
	D3DXMatrixRotationY( &matRot, m_pChrModel->GetDirection() ); 
	D3DXMatrixTranslation( &matTrans, m_vFixedPoint.x, m_vFixedPoint.y, m_vFixedPoint.z ); 
	matWorld = matRot*matTrans; 

	for( int r = 0; r <= m_Prop.m_iRow; r++ )
	{
		for( int c = 0; c <= m_Prop.m_iCol; c++ )
		{
			if( !m_pParticles[ROWCOL(r,c)].bLocked )
			{
				m_pParticles[ROWCOL(r,c)].vPos.x = c * m_fCStep - (float)m_Prop.m_iWidth/2.0f;
				m_pParticles[ROWCOL(r,c)].vPos.y = 0.0f; 
				m_pParticles[ROWCOL(r,c)].vPos.z = r * m_fRStep; 
				m_pParticles[ROWCOL(r,c)].vVel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
				m_pParticles[ROWCOL(r,c)].vAccel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
				m_pParticles[ROWCOL(r,c)].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
				D3DXVec3TransformCoord( &m_pParticles[ROWCOL(r,c)].vPos, &m_pParticles[ROWCOL(r,c)].vPos, &matWorld );
			}
		}
	}
}

void CCharacterCape::DrawCollisionSphere()
{
	if( !m_bTestMode ) return; 

	D3DXMATRIX matWorld; 
	CSceneManager::GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ); 
	D3DXMatrixTranslation( &matWorld, m_CollisionSphere.vPos.x, m_CollisionSphere.vPos.y, m_CollisionSphere.vPos.z ); 
	CSceneManager::GetDevice()->SetTransform( D3DTS_WORLD, &matWorld ); 
	m_pSphere->DrawSubset(0); 
	CSceneManager::GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ); 
}

void CCharacterCape::RenderShadow()
{
	if( !m_bEnable ) return; 

	CClothMgr::_Instance()->m_CapeRenderer.Render(); 
}
