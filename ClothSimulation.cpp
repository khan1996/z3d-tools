#include ".\clothsimulation.h"
#include "dxutil.h"
#include <assert.h>

const DWORD CClothSimulation::ClothVertex::FVF = D3DFVF_XYZ|D3DFVF_TEX1;

CClothSimulation::CClothSimulation(void) : m_vWindDirection(D3DXVECTOR3(0.0f,0.0f,0.0f)), m_iWindForce(0), 
m_iNumRow(0), m_iNumCol(0), m_pVB(NULL), m_pIB(NULL), m_pTexture(NULL), m_vVirtualSimulation(D3DXVECTOR3(0,0,0)), 
m_vMoveParticle(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), m_fRotateParticle(0.0f), m_vWorldPos(D3DXVECTOR3(0,0,0)), 
m_vMoveValue(D3DXVECTOR3(0,0,0))
{
}

CClothSimulation::~CClothSimulation(void)
{
	SAFE_RELEASE( m_pVB ); 
	SAFE_RELEASE( m_pIB ); 
	SAFE_RELEASE( m_pTexture ); 
}

void CClothSimulation::Create( LPDIRECT3DDEVICE8 pd3dDevice, int iNumRow, int iNumCol, int iWidth, int iHeight )
{
	assert( iNumRow > 0 ); 
	assert( iNumCol > 0 ); 
	assert( iWidth > 0 ); 
	assert( iHeight > 0 ); 
	assert( pd3dDevice ); 

	m_pd3dDevice = pd3dDevice; 
	m_iNumRow = iNumRow; 
	m_iNumCol = iNumCol; 
	m_iWidth = iWidth; 
	m_iHeight = iHeight; 

	m_CollisionBox.Init(  D3DXVECTOR3(0.0F, 0.0F, 40.0F), D3DXVECTOR3(0.0F, 0.0F, -1.0F), 100, 500, 500, 50 ); 
	D3DXVec3Normalize( &m_CollisionBox.vCollisionNormal, &m_CollisionBox.vCollisionNormal ); 
	m_CollisionBox.vCollisionNormal *= m_CollisionBox.fCollisionConstant; 

	int r, c;
	float f;
	D3DXVECTOR3	vL;
	int count = 0;
	int n = NUMSTRUCTURALSPRINGS;
	int iVertexIndex = 0; 

	for( r = 0; r <= NUMROWS; r++ )
	{
		for( c = 0; c <= NUMCOLUMNS; c++ )
		{
			//파티클 세팅
			if((r == 0) && (c == 0)) 
				f = 1;
			else if((r == NUMROWS) && (c == 0)) 
				f = 2;
			else if((r == 0) && (c == NUMCOLUMNS)) 
				f = 2;
			else if((r == NUMROWS) && (c == NUMCOLUMNS)) 
				f = 1;
			else if(((r == 0) || (r == NUMROWS)) && ((c != 0) && (c != NUMCOLUMNS))) 
				f = 3;
			else
				f = 6;

			m_Particles[r][c].fMass = (f * MASSPERFACE) / 3;
			m_Particles[r][c].fInvMass = 1 / m_Particles[r][c].fMass;
			m_Particles[r][c].vPos.x = c * CSTEP - (float)CLOTHWIDTH/2;
			m_Particles[r][c].vPos.y = (CLOTHHEIGHT - (r * RSTEP)) - (float)CLOTHHEIGHT/2;
			m_Particles[r][c].vPos.z = 0.0f;
			m_Particles[r][c].vVel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_Particles[r][c].vAccel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_Particles[r][c].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_Particles[r][c].bLocked = false; 
		}
	}

	for( r = 0; r <= NUMROWS; r++ )
	{
		for( c = 0; c <= NUMCOLUMNS; c++ )
		{
			//버텍스세팅
			m_ClothVertex[iVertexIndex].pos = m_Particles[r][c].vPos; 
			if( c == 0 ) m_ClothVertex[iVertexIndex].u = 0.0f;
			else m_ClothVertex[iVertexIndex].u = (float)c/(float)NUMCOLUMNS; 
			if( r == 0 ) m_ClothVertex[iVertexIndex].v = 0.0f;
			else m_ClothVertex[iVertexIndex].v = (float)r/(float)NUMROWS;  		
			iVertexIndex++; 

			//스프링세팅
			if(c<NUMCOLUMNS)
			{
				m_StructuralSprings[count].p1.r = r;
				m_StructuralSprings[count].p1.c = c;
				m_StructuralSprings[count].p2.r = r;
				m_StructuralSprings[count].p2.c = c+1;
				m_StructuralSprings[count].k = SPRINGTENSIONCONSTANT;
				m_StructuralSprings[count].d = SPRINGDAMPINGCONSTANT;				
				vL = m_Particles[r][c].vPos - m_Particles[r][c+1].vPos;
				m_StructuralSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(r<NUMROWS)
			{
				m_StructuralSprings[count].p1.r = r;
				m_StructuralSprings[count].p1.c = c;
				m_StructuralSprings[count].p2.r = r+1;
				m_StructuralSprings[count].p2.c = c;
				m_StructuralSprings[count].k = SPRINGTENSIONCONSTANT;
				m_StructuralSprings[count].d = SPRINGDAMPINGCONSTANT;				
				vL = m_Particles[r][c].vPos - m_Particles[r+1][c].vPos;
				m_StructuralSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(r<NUMROWS && c<NUMCOLUMNS)
			{
				m_StructuralSprings[count].p1.r = r;
				m_StructuralSprings[count].p1.c = c;
				m_StructuralSprings[count].p2.r = r+1;
				m_StructuralSprings[count].p2.c = c+1;
				m_StructuralSprings[count].k = SPRINGSHEARCONSTANT;
				m_StructuralSprings[count].d = SPRINGDAMPINGCONSTANT;				
				vL = m_Particles[r][c].vPos - m_Particles[r+1][c+1].vPos;
				m_StructuralSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(c>0 && r<NUMROWS)
			{
				m_StructuralSprings[count].p1.r = r;
				m_StructuralSprings[count].p1.c = c;
				m_StructuralSprings[count].p2.r = r+1;
				m_StructuralSprings[count].p2.c = c-1;
				m_StructuralSprings[count].k = SPRINGSHEARCONSTANT;
				m_StructuralSprings[count].d = SPRINGDAMPINGCONSTANT;				
				vL = m_Particles[r][c].vPos - m_Particles[r+1][c-1].vPos;
				m_StructuralSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
		}
	}

	//인덱스세팅
	iVertexIndex = 0; 
	for( r = 0; r < NUMROWS; r++ )
	{
		for( c = 0; c < NUMCOLUMNS; c++ )
		{
			m_ClothIndex[iVertexIndex++] = c + r*(NUMCOLUMNS+1); 
			m_ClothIndex[iVertexIndex++] = c + r*(NUMCOLUMNS+1) + 1; 
			m_ClothIndex[iVertexIndex++] = c + (r+1)*(NUMCOLUMNS+1);  

			m_ClothIndex[iVertexIndex++] = c + r*(NUMCOLUMNS+1) + 1; 
			m_ClothIndex[iVertexIndex++] = c + (r+1)*(NUMCOLUMNS+1) + 1; 
			m_ClothIndex[iVertexIndex++] = c + (r+1)*(NUMCOLUMNS+1); 
		}
	}

	m_pd3dDevice->CreateVertexBuffer( sizeof(ClothVertex)*NUMVERTICES, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, ClothVertex::FVF, 
		D3DPOOL_DEFAULT, &m_pVB ); 
	m_pd3dDevice->CreateIndexBuffer( sizeof(WORD)*NUMROWS*NUMCOLUMNS*2*3, D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB ); 

	void* pVert;  
	m_pVB->Lock( 0, 0, (BYTE**)&pVert, 0 ); 
	memcpy( pVert, m_ClothVertex, sizeof(ClothVertex)*NUMVERTICES ); 
	m_pVB->Unlock(); 

	void* pIndics; 
	m_pIB->Lock( 0, 0, (BYTE**)&pIndics, 0 ); 
	memcpy( pIndics, m_ClothIndex, sizeof(WORD)*NUMROWS*NUMCOLUMNS*2*3 ); 
	m_pIB->Unlock(); 
	
	D3DXCreateTextureFromFile( m_pd3dDevice, "c:\\picture.jpg", &m_pTexture );
}	

void CClothSimulation::Render()
{
	//D3DXMATRIX matWorld; 
	//D3DXMatrixIdentity( &matWorld ); 
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE ); 
	m_pd3dDevice->SetTexture( 0, m_pTexture ); 
	//m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );	
	//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME ); 
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
	m_pd3dDevice->SetVertexShader( ClothVertex::FVF ); 
	m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(ClothVertex) ); 
	m_pd3dDevice->SetIndices( m_pIB, 0 ); 
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, NUMVERTICES, 0, NUMROWS*NUMCOLUMNS*2 ); 

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE ); 
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); 
}

void CClothSimulation::Update( float dt )
{
	CalculateForce();
	Integrate( dt ); 

	void* pVert;  
	m_pVB->Lock( 0, 0, (BYTE**)&pVert, D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK ); 
	memcpy( pVert, m_ClothVertex, sizeof(ClothVertex)*NUMVERTICES ); 
	m_pVB->Unlock(); 
}

void CClothSimulation::CalculateForce()
{
	int	r, c, i, r1, c1, r2, c2;
	D3DXVECTOR3	dragVector;
	D3DXVECTOR3	f1, f2, d, v;
	float	L, tempValue;
	D3DXVECTOR3 vRandWind; 

	for(r=0; r<=NUMROWS; r++)
	{
		for(c=0; c<=NUMCOLUMNS; c++)
		{		
			m_Particles[r][c].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 

			//gravity
			m_Particles[r][c].vForce.y += (float) (GRAVITY * m_Particles[r][c].fMass);

			// viscous drag
			dragVector = -m_Particles[r][c].vVel;
			D3DXVec3Normalize( &dragVector, &dragVector ); 
			float tempValue = D3DXVec3Length( &m_Particles[r][c].vVel ); 
			m_Particles[r][c].vForce += dragVector * tempValue * tempValue * DRAGCOEFFICIENT;

			// wind
			if( m_iWindForce )
			{
				vRandWind = D3DXVECTOR3( RandNum((int)m_vWindDirection.x), RandNum((int)m_vWindDirection.y), 
					RandNum((int)m_vWindDirection.z) ); 
				D3DXVec3Normalize( &vRandWind, &vRandWind ); 
				m_Particles[r][c].vForce += vRandWind * (rand()%m_iWindForce); 			
			}

			//virtual simulation
			m_Particles[r][c].vForce += m_vVirtualSimulation * 10 * (rand()%5+5); 

			if( !m_Particles[r][c].bLocked )
			{
				m_Particles[r][c].vPos += m_vMoveValue*0.8;
			}
		}
	}

	m_vMoveValue = D3DXVECTOR3(0,0,0); 

	// Process spring forces
	for(i = 0; i<NUMSTRUCTURALSPRINGS; i++)
	{
		r1 = m_StructuralSprings[i].p1.r;
		c1 = m_StructuralSprings[i].p1.c;
		r2 = m_StructuralSprings[i].p2.r;
		c2 = m_StructuralSprings[i].p2.c;

		d = m_Particles[r1][c1].vPos - m_Particles[r2][c2].vPos;
		v = m_Particles[r1][c1].vVel - m_Particles[r2][c2].vVel;
		L = m_StructuralSprings[i].L;

		tempValue = D3DXVec3Length(&d); 

		f1 = -(m_StructuralSprings[i].k * (tempValue - L) + 
			m_StructuralSprings[i].d * ( D3DXVec3Dot(&v, &d) / tempValue )) * ( d / tempValue );
		f2 = -f1;

		if(m_Particles[r1][c1].bLocked == FALSE)
			m_Particles[r1][c1].vForce += f1;

		if(m_Particles[r2][c2].bLocked == FALSE)
			m_Particles[r2][c2].vForce += f2;
	}
}

void CClothSimulation::Integrate( float dt )
{
	D3DXVECTOR3	Ae;
	int		r, c;
	int		check = 0;
	int iVertexIndex=0; 
	D3DXMATRIX matRot; 

	for(r=0; r<=NUMROWS; r++)
	{
		for(c=0; c<=NUMCOLUMNS; c++)
		{	
			if( !m_Particles[r][c].bLocked )
			{
				Ae = m_Particles[r][c].vForce * m_Particles[r][c].fInvMass;
				m_Particles[r][c].vAccel = Ae;
				m_Particles[r][c].vVel += Ae * dt;
				m_Particles[r][c].vPos += m_Particles[r][c].vVel * dt; 

				//룽게-쿠타 적분법

				/*
				if( m_Particles[r][c].vPos.x > m_CollisionBox.vMin.x && 
				m_Particles[r][c].vPos.y > m_CollisionBox.vMin.y &&
				m_Particles[r][c].vPos.z > m_CollisionBox.vMin.z &&
				m_Particles[r][c].vPos.x < m_CollisionBox.vMax.x &&
				m_Particles[r][c].vPos.y < m_CollisionBox.vMax.y &&
				m_Particles[r][c].vPos.z < m_CollisionBox.vMax.z ) 
				{ 
				//충돌 계산식을 피하기위한 꽁수
				m_Particles[r][c].vVel += m_CollisionBox.vCollisionNormal*0.2f; 
				}*/ 
			}

			//버텍스세팅

		   //버철시뮬레이션방식
			/*
			m_ClothVertex[iVertexIndex].pos = m_Particles[r][c].vPos - m_vMoveParticle; 
			D3DXMATRIX matRot; 
			D3DXMatrixRotationY( &matRot, -m_fRotateParticle ); 
			D3DXVec3TransformCoord( &m_ClothVertex[iVertexIndex].pos, &m_ClothVertex[iVertexIndex].pos, &matRot ); 

			if( r == 0 && c == 0 ) m_ClothVertex[iVertexIndex].pos = m_vSavePoint2; 
			if( r == 0 && c == 4 ) m_ClothVertex[iVertexIndex].pos = m_vSavePoint; 
			iVertexIndex ++; 
			*/

			//리얼시뮬레이션방식 
			m_ClothVertex[iVertexIndex].pos = m_Particles[r][c].vPos; 
			iVertexIndex ++; 
		}
	}	
}

void CClothSimulation::LockParticle( int iRow, int iCol )
{
	m_Particles[iRow][iCol].bLocked = true; 
	
	ParticleRef ref;
	ref.r = iRow; 
	ref.c = iCol; 
	m_LockedParticleMap.insert( PARTICLEMAP::iterator::value_type(&m_Particles[iRow][iCol], ref) ); 
}

void CClothSimulation::UnLockParticle( int iRow, int iCol )
{
	PARTICLEMAP::iterator iter; 
	m_Particles[iRow][iCol].bLocked = false; 
	iter = m_LockedParticleMap.find( &m_Particles[iRow][iCol] ); 
	if( iter != m_LockedParticleMap.end() )
	{
		m_LockedParticleMap.erase( iter ); 
	}
}

void CClothSimulation::MoveLockedParticle( const D3DXVECTOR3& vMoveValue )
{
	D3DXVECTOR3 TempValue = vMoveValue; 
	D3DXMATRIX matRot; 
	D3DXMatrixRotationY( &matRot, m_fRotateParticle ); 
	D3DXVec3TransformCoord( &TempValue, &TempValue, &matRot ); 

	PARTICLEMAP::iterator iter; 
	for( iter = m_LockedParticleMap.begin(); iter != m_LockedParticleMap.end(); iter++ )
	{
		m_Particles[iter->second.r][iter->second.c].vPos += TempValue;
	}

	m_vMoveParticle += TempValue; 

	m_vMoveValue = TempValue; 

	//충돌박스이동
	m_CollisionBox.vCenter += vMoveValue; 
	m_CollisionBox.vMin += vMoveValue; 
	m_CollisionBox.vMax += vMoveValue; 
}

void CClothSimulation::RotateLockedParticle( const D3DXVECTOR3& vRot, int CenterRow, int CenterCol  )
{
	D3DXMATRIX matWorld, matRot, matTrans; 
	D3DXMatrixRotationYawPitchRoll( &matRot, vRot.y, vRot.x, vRot.z ); 
	D3DXVECTOR3 vTemp; 
	D3DXVECTOR3 vPos;  

	m_fRotateParticle += vRot.y; 

	PARTICLEMAP::iterator iter; 
	for( iter = m_LockedParticleMap.begin(); iter != m_LockedParticleMap.end(); iter++ )
	{
		if( iter->second.r == CenterRow && iter->second.c == CenterCol ) continue; 

		vTemp = m_Particles[iter->second.r][iter->second.c].vPos - m_Particles[CenterRow][CenterCol].vPos; 
		D3DXMatrixTranslation( &matTrans, vTemp.x, vTemp.y, vTemp.z ); 
		D3DXMatrixMultiply( &matWorld, &matTrans, &matRot ); 
		vPos = D3DXVECTOR3(0.0F, 0.0F, 0.0F); 
		D3DXVec3TransformCoord( &vPos, &vPos, &matWorld ); 
		m_Particles[iter->second.r][iter->second.c].vPos += vPos - vTemp; 
	}

	//충돌박스회전
	/*
	vTemp = m_CollisionBox.vMin - m_Particles[CenterRow][CenterCol].vPos; 
	D3DXMatrixTranslation( &matTrans, vTemp.x, vTemp.y, vTemp.z ); 
	D3DXMatrixMultiply( &matWorld, &matTrans, &matRot ); 
	vPos = D3DXVECTOR3(0.0F, 0.0F, 0.0F); 
	D3DXVec3TransformCoord( &vPos, &vPos, &matWorld ); 
	m_CollisionBox.vMin += vPos - vTemp; 
	
	vTemp = m_CollisionBox.vMax - m_Particles[CenterRow][CenterCol].vPos; 
	D3DXMatrixTranslation( &matTrans, vTemp.x, vTemp.y, vTemp.z ); 
	D3DXMatrixMultiply( &matWorld, &matTrans, &matRot ); 
	vPos = D3DXVECTOR3(0.0F, 0.0F, 0.0F); 
	D3DXVec3TransformCoord( &vPos, &vPos, &matWorld ); 
	m_CollisionBox.vMax += vPos - vTemp; 

	//충돌법선회전
	D3DXVec3TransformCoord( &m_CollisionBox.vCollisionNormal, &m_CollisionBox.vCollisionNormal, &matRot ); 
	D3DXVec3Normalize( &m_CollisionBox.vCollisionNormal, &m_CollisionBox.vCollisionNormal ); 
	m_CollisionBox.vCollisionNormal *= m_CollisionBox.fCollisionConstant; 
	*/
}

void CClothSimulation::SetWindFactor( const D3DXVECTOR3& vDirection, int iForce )
{
	m_vWindDirection = vDirection; 
	m_iWindForce = iForce; 
}

void CClothSimulation::AddCollisionPlane( const D3DXPLANE& Plane )
{
	
}

void CClothSimulation::VirtualSimulation( DWORD dwType )
{
	float fStep = 0.8f; 

	switch( dwType )
	{
	case NOTHING:
		m_vVirtualSimulation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
		break; 

	case MOVE_FRONT:
		//m_vVirtualSimulation += D3DXVECTOR3( 0.0f, 0.0f, 0.1f ); 
		//MoveLockedParticle( D3DXVECTOR3(0.0f, sinf(float(GetTickCount()))*1.5f, 0.0f) ); 
		MoveLockedParticle( D3DXVECTOR3(0.0f, 0.0f, -fStep) ); 
		break; 

	case MOVE_BACK:
		//m_vVirtualSimulation += D3DXVECTOR3( 0.0f, 0.0f, -0.1f ); 
		//MoveLockedParticle( D3DXVECTOR3(0.0f, sinf(float(GetTickCount()))*1.5f, 0.0f) ); 
		MoveLockedParticle( D3DXVECTOR3(0.0f, 0.0f, fStep) ); 
		break; 

	case MOVE_RIGHT:
		//m_vVirtualSimulation += D3DXVECTOR3( 0.1f, 0.0f, 0.0f ); 
		//MoveLockedParticle( D3DXVECTOR3(0.0f, sinf(float(GetTickCount()))*1.5f, 0.0f) ); 
		MoveLockedParticle( D3DXVECTOR3(-fStep, 0.0f, 0.0f) ); 		
		break; 

	case MOVE_LEFT:
		//m_vVirtualSimulation += D3DXVECTOR3( -0.1f, 0.0f, 0.0f ); 
		//MoveLockedParticle( D3DXVECTOR3(0.0f, sinf(float(GetTickCount()))*1.5f, 0.0f) ); 
		MoveLockedParticle( D3DXVECTOR3(fStep, 0.0f, 0.0f) ); 		
		break; 

	case ROTATE_RIGHT:
		RotateLockedParticle( D3DXVECTOR3(0.0f, 0.05f, 0.0f), 0, 2 ); 
		break; 

	case ROTATE_LEFT:
		RotateLockedParticle( D3DXVECTOR3(0.0f, -0.05f, 0.0f), 0, 2 );
		break; 

	default:
		break; 
	}

	if( m_vVirtualSimulation.x > 2.0f ) m_vVirtualSimulation.x = 2.0f; 
	if( m_vVirtualSimulation.x < -2.0f ) m_vVirtualSimulation.x = -2.0f; 
	if( m_vVirtualSimulation.y > 1.0f ) m_vVirtualSimulation.y = 1.0f; 
	if( m_vVirtualSimulation.y < -1.0f ) m_vVirtualSimulation.y = -1.0f; 
	if( m_vVirtualSimulation.z > 1.0f ) m_vVirtualSimulation.z = 1.0f; 
	if( m_vVirtualSimulation.z < -0.5f ) m_vVirtualSimulation.z = -0.5f; 
}

int CClothSimulation::RandNum( int number )
{
	int rnd=1; 
	if( number < 0 )
	{
		rnd = -number;
		if( rnd == 0 ) rnd = 1; 
		rnd = -rand()%rnd; 
	}
	else
	{
		if( rnd == 0 ) rnd = 1; 
		rnd = rand()%rnd; 
	}

	return rnd; 
}

void CClothSimulation::SetPosLockParticle( int iRow, int iCol, const D3DXVECTOR3& vPos )
{
	//	if( m_Particles[iRow][iCol].bLocked )
	{
		//m_Particles[iRow][iCol].vPos = vPos; 

		if( iRow == 0 && iCol == 0 ) m_vSavePoint = vPos; 
		if( iRow == 0 && iCol == 4 ) m_vSavePoint2 = vPos; 
	}
}

void CClothSimulation::SetWorldPos( const D3DXVECTOR3& vPos )
{
	m_vWorldPos = vPos; 

	for( int r = 0; r <= NUMROWS; r++ )
	{
		for( int c = 0; c <= NUMCOLUMNS; c++ )
		{
			if( !m_Particles[r][c].bLocked )
				m_Particles[r][c].vPos += m_vWorldPos; 
		}
	}
}

