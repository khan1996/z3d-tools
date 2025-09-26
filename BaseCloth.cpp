#include ".\basecloth.h"
#include "dxutil.h"
#include "GMMemory.h"

CBaseCloth::CBaseCloth( const CClothProperty* pProp ) : m_pParticles(NULL), m_pSprings(NULL)
{
	m_Prop = *pProp; 

	int r, c;
	float f;
	D3DXVECTOR3	vL;
	int count = 0;
	m_iNumVertex = (m_Prop.m_iRow+1)*(m_Prop.m_iCol+1); 
	m_iNumFace = m_Prop.m_iRow*m_Prop.m_iCol*2; 
	m_iNumSpring = m_Prop.m_iCol*(m_Prop.m_iRow+1) + m_Prop.m_iRow*(m_Prop.m_iCol+1) + m_Prop.m_iCol*m_Prop.m_iRow*2;
	float fMassPerFace = m_Prop.m_fMass / (float)(m_Prop.m_iCol*m_Prop.m_iRow*2); 
	m_fCStep = (float)m_Prop.m_iWidth / (float)m_Prop.m_iCol; 
	m_fRStep = (float)m_Prop.m_iHeight / (float)m_Prop.m_iRow; 

	m_pParticles = new Particle[m_iNumVertex]; 
	m_pSprings = new Spring[m_iNumSpring]; 

	float fBottomLine; 
	D3DXVECTOR3 vLeftTop, vLeftBottom, vRightTop, vRightBottom, vOutLerpPoint, vNewLeftTop, 
		vNewLeftBottom, vNewRightTop, vNewRightBottom;

	if( pProp->m_FormType == CFT_TRAPEZOID )
	{
		fBottomLine = (float)m_Prop.m_iWidth*(float)2.5f; 
		vLeftTop = D3DXVECTOR3( -(float)m_Prop.m_iWidth/2, (float)m_Prop.m_iHeight/2, 0.0f ); 
		vLeftBottom = D3DXVECTOR3( -fBottomLine/2, -(float)m_Prop.m_iHeight/2, 0.0f ); 
		vRightTop = D3DXVECTOR3( (float)m_Prop.m_iWidth/2, (float)m_Prop.m_iHeight/2, 0.0f ); 
		vRightBottom = D3DXVECTOR3( fBottomLine/2, -(float)m_Prop.m_iHeight/2, 0.0f ); 
		vOutLerpPoint, vNewLeftTop, vNewLeftBottom, vNewRightTop, vNewRightBottom; 
	}

	for( r = 0; r <= m_Prop.m_iRow; r++ )
	{
		for( c = 0; c <= m_Prop.m_iCol; c++ )
		{
			if((r == 0) && (c == 0)) 
				f = 1;
			else if((r == m_Prop.m_iRow) && (c == 0)) 
				f = 2;
			else if((r == 0) && (c == m_Prop.m_iCol)) 
				f = 2;
			else if((r == m_Prop.m_iRow) && (c == m_Prop.m_iCol)) 
				f = 1;
			else if(((r == 0) || (r == m_Prop.m_iRow)) && ((c != 0) && (c != m_Prop.m_iCol))) 
				f = 3;
			else
				f = 6;

			m_pParticles[ROWCOL(r,c)].fMass = (f * fMassPerFace) / 3;
			m_pParticles[ROWCOL(r,c)].fInvMass = 1 / m_pParticles[ROWCOL(r,c)].fMass;
			m_pParticles[ROWCOL(r,c)].vVel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_pParticles[ROWCOL(r,c)].vAccel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_pParticles[ROWCOL(r,c)].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_pParticles[ROWCOL(r,c)].bLocked = false; 
			m_pParticles[ROWCOL(r,c)].RC.r = r; 
			m_pParticles[ROWCOL(r,c)].RC.c = c; 

			if( pProp->m_FormType == CFT_TRAPEZOID )
			{
				D3DXVec3Lerp( &vNewLeftTop, &vLeftTop, &vLeftBottom, r ? ((float)r/(float)m_Prop.m_iRow) : 0 ); 
				D3DXVec3Lerp( &vNewRightTop, &vRightTop, &vRightBottom, r ? ((float)r/(float)m_Prop.m_iRow) : 0 ); 
				D3DXVec3Lerp( &vOutLerpPoint, &vNewLeftTop, &vNewRightTop, c ? ((float)c/(float)m_Prop.m_iCol) : 0 ); 
				m_pParticles[ROWCOL(r,c)].vPos = vOutLerpPoint;
			}
			else if( pProp->m_FormType == CFT_QUAD )
			{
				m_pParticles[ROWCOL(r,c)].vPos.x = c * m_fCStep - (float)m_Prop.m_iWidth/2.0f;
				m_pParticles[ROWCOL(r,c)].vPos.y = (m_Prop.m_iHeight - (r * m_fRStep)) - (float)m_Prop.m_iHeight/2.0f;
				m_pParticles[ROWCOL(r,c)].vPos.z = 0.0f;
			}
		}
	}

	for( r = 0; r <= m_Prop.m_iRow; r++ )
	{
		for( c = 0; c <= m_Prop.m_iCol; c++ )
		{
			if(c<m_Prop.m_iCol)
			{
				m_pSprings[count].p1.r = r;
				m_pSprings[count].p1.c = c;
				m_pSprings[count].p2.r = r;
				m_pSprings[count].p2.c = c+1;
				m_pSprings[count].k = m_Prop.m_fSpringTensionConstant;
				m_pSprings[count].d = m_Prop.m_fSpringDampingConstant;				
				vL = m_pParticles[ROWCOL(r,c)].vPos - m_pParticles[ROWCOL(r,c+1)].vPos;
				m_pSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(r<m_Prop.m_iRow)
			{
				m_pSprings[count].p1.r = r;
				m_pSprings[count].p1.c = c;
				m_pSprings[count].p2.r = r+1;
				m_pSprings[count].p2.c = c;
				m_pSprings[count].k = m_Prop.m_fSpringTensionConstant;
				m_pSprings[count].d = m_Prop.m_fSpringDampingConstant;				
				vL = m_pParticles[ROWCOL(r,c)].vPos - m_pParticles[ROWCOL(r+1,c)].vPos;
				m_pSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(r<m_Prop.m_iRow && c<m_Prop.m_iCol)
			{
				m_pSprings[count].p1.r = r;
				m_pSprings[count].p1.c = c;
				m_pSprings[count].p2.r = r+1;
				m_pSprings[count].p2.c = c+1;
				m_pSprings[count].k = m_Prop.m_fSpringShearConstant;
				m_pSprings[count].d = m_Prop.m_fSpringDampingConstant;				
				vL = m_pParticles[ROWCOL(r,c)].vPos - m_pParticles[ROWCOL(r+1,c+1)].vPos;
				m_pSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
			if(c>0 && r<m_Prop.m_iRow)
			{
				m_pSprings[count].p1.r = r;
				m_pSprings[count].p1.c = c;
				m_pSprings[count].p2.r = r+1;
				m_pSprings[count].p2.c = c-1;
				m_pSprings[count].k = m_Prop.m_fSpringShearConstant;
				m_pSprings[count].d = m_Prop.m_fSpringDampingConstant;				
				vL = m_pParticles[ROWCOL(r,c)].vPos - m_pParticles[ROWCOL(r+1,c-1)].vPos;
				m_pSprings[count].L = D3DXVec3Length(&vL); 		
				count++;
			}
		}
	}
}

CBaseCloth::~CBaseCloth(void)
{
	SAFE_DELETE_ARRAY( m_pParticles ); 
	SAFE_DELETE_ARRAY( m_pSprings ); 
}

void CBaseCloth::LockParticle( int iRow, int iCol )
{
	m_pParticles[ROWCOL(iRow,iCol)].bLocked = true; 
	m_LockedParticleList.push_back(&m_pParticles[ROWCOL(iRow,iCol)]); 
}

void CBaseCloth::UnLockParticle( int iRow, int iCol )
{
	m_pParticles[ROWCOL(iRow,iCol)].bLocked = false; 
	
	PARTICLEVECTOR::iterator iter; 
	for( iter = m_LockedParticleList.begin(); iter != m_LockedParticleList.end(); iter++ )
	{
		if( (*iter)->RC.r == iRow && (*iter)->RC.c == iCol )
			m_LockedParticleList.erase(iter); 
	}
}
