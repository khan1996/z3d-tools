// Z3DBladeTrail.cpp: implementation of the CZ3DBladeTrail class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable:4786 )
#include "Z3DBladeTrail.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DBladeTrail::CZ3DBladeTrail( CZ3DAttachment* pWeapon, char* szTex,
							   long lDuration, long lTessel )
							   :CZ3DObject(), CZ3DRenderable()//, CZ3DManagedObject()
{
	int i;

	_ASSERT( pWeapon);
	m_rpWeapon = pWeapon;

	m_pTagTexture = g_ContTexture.GetObject( szTex );
	m_pTexture = m_pTagTexture->GetD3dTexture();

	m_lDuration = lDuration;
	m_lTessel = lTessel;

	m_bActive = false;
	m_bInputStopped = true;

	m_paTrace = new vector3[m_lDuration][2];
	m_pVertex = new D3DLVERTEX[2 * ((m_lDuration-1) * (m_lTessel+1) + 1)];
	m_pIndex = new WORD[6 * (m_lDuration-1) * (m_lTessel+1)];

	long lTmp = (m_lDuration-1)*(m_lTessel+1) + 1;
	for( i = 0; i < lTmp; ++i )
	{
		m_pVertex[i*2].u = 0.0f;
		m_pVertex[i*2+1].u = 1.0f;
	}


	for( i = 0; i < (m_lDuration-1) * (m_lTessel+1); ++i )
	{
		m_pIndex[i*6] = i*2;
		m_pIndex[i*6 + 1] = (i+1)*2;
		m_pIndex[i*6 + 2] = i*2 + 1;

		m_pIndex[i*6 + 3] = i*2 + 1;
		m_pIndex[i*6 + 4] = (i+1)*2;
		m_pIndex[i*6 + 5] = (i+1)*2 + 1;

/*
		m_pIndex[i*12 + 6] = i*2;
		m_pIndex[i*12 + 7] = i*2 + 1;
		m_pIndex[i*12 + 8] = (i+1)*2;

		m_pIndex[i*12 + 9] = i*2 + 1;
		m_pIndex[i*12 + 10] = (i+1)*2 + 1;
		m_pIndex[i*12 + 11] = (i+1)*2;
*/
	}

	m_R = m_G = m_B = 0xFF;

	Link( NULL );
}


CZ3DBladeTrail::~CZ3DBladeTrail()
{
	UnLink();

	SAFE_DELETEA( m_pVertex );
	SAFE_DELETEA( m_pIndex );
	SAFE_DELETEA( m_paTrace );

	// Release() 메소드가 아직 없다
//	m_pTagTexture.Release();
}


void CZ3DBladeTrail::SetTexture( const char* szFileName )
{
	Z3DTexture* pTagTexture;

	if( NULL != (pTagTexture = g_ContTexture.GetObject( szFileName )) )
	{
		// Release() 메소드가 아직 없다
//		m_pTagTexture.Release();
		m_pTagTexture = pTagTexture;
		m_pTexture = pTagTexture->GetD3dTexture();
	}
}

void CZ3DBladeTrail::SetTexture( IDirect3DTexture8* pTexture )
{
	m_pTexture = NULL;
	// Release() 메소드가 아직 없다
//	m_pTagTexture.Release();


	if( NULL == pTexture )
	{
		Z3DTexture* pTagTexture;

		if( NULL != ( pTagTexture = g_ContTexture.GetObject( "blade_trail_1.dds" ) ) )
		{
			m_pTagTexture = pTagTexture;
			m_pTexture = m_pTagTexture->GetD3dTexture();
		}
	}
	else
	{
		m_pTexture = pTexture;
	}
}


void CZ3DBladeTrail::Activate()
{
	m_lHeadIndex = 0;
	m_lLength = 0;
	m_bActive = true;
	m_bInputStopped = false;
}


void CZ3DBladeTrail::Stop()
{
	m_bInputStopped = true;
}


void CZ3DBladeTrail::SecondRender()
{
	m_pTexture = m_pTagTexture->GetD3dTexture();

	if( m_bActive && m_lLength > 1 )
	{
		GetDevice()->SetVertexShader( D3DFVF_LVERTEX );
		matrix m;
		m.MakeIdent();
		m._11 = m._22 = m._33 = m_rpWeapon->GetScale();
		m._41 = m_vPivotPos.x;
		m._42 = m_vPivotPos.y;
		m._43 = m_vPivotPos.z;
		GetDevice()->SetTransform( D3DTS_WORLD, m );

//		GetDevice()->SetTexture( 0, NULL );
//		DWORD dwTempShader;
//		GetDevice()->GetPixelShader(&dwTempShader);
//		GetDevice()->SetPixelShader( NULL );
		GetDevice()->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 0, 
			2 * ((m_lTessel+1) * (m_lLength-1) + 1),
			2 * (m_lTessel+1) * (m_lLength-1),
			m_pIndex, D3DFMT_INDEX16, m_pVertex, sizeof(D3DLVERTEX) );
//		GetDevice()->SetPixelShader(dwTempShader);
	}
}

void InterpolateInbetween( D3DLVERTEX* pVo,
						  D3DLVERTEX* pV1, D3DLVERTEX* pV2,
						  D3DLVERTEX* pV3, D3DLVERTEX* pV4,
						  long lTessel )
{
	float u;

	for( int i = 0; i < lTessel; ++i )
	{
		u = float(i+1) / float(lTessel+1);

		D3DXVec3CatmullRom(
			(D3DXVECTOR3*)&(pVo[i*2].x),
			(D3DXVECTOR3*)&(pV1[0].x), (D3DXVECTOR3*)&(pV2[0].x),
			(D3DXVECTOR3*)&(pV3[0].x), (D3DXVECTOR3*)&(pV4[0].x), u );

		D3DXVec3CatmullRom(
			(D3DXVECTOR3*)&(pVo[i*2+1].x),
			(D3DXVECTOR3*)&(pV1[1].x), (D3DXVECTOR3*)&(pV2[1].x),
			(D3DXVECTOR3*)&(pV3[1].x), (D3DXVECTOR3*)&(pV4[1].x), u );
	}
}


void CZ3DBladeTrail::UpdateFrame()
{
	int i;
	long lActualIndex;
	long lActualVertexIndex;
	long lBoundary;

	if( !m_bActive )
	{
		return;
	}

	m_rpWeapon->GetPivot( m_vPivotPos );

	if( m_bInputStopped )
	{
		--m_lLength;
		if( m_lLength < 2 )
		{
			m_bActive = false;
			return;
		}
	}
	else
	{
		if( m_lLength < m_lDuration )
		{
			++m_lLength;
		}

		// 새 궤적위치 입력
		m_rpWeapon->GetMarker( m_paTrace[m_lHeadIndex][0], m_paTrace[m_lHeadIndex][1] );

		// 궤적 버퍼 데이터를 버텍스 버퍼의 control point로
		lBoundary = m_bInputStopped ? (m_lLength+1) : m_lLength;
		for( i = 0; i < lBoundary; ++i )
		{
			lActualIndex = (m_lHeadIndex - i + m_lDuration) % m_lDuration;
			lActualVertexIndex = i * (m_lTessel+1) * 2;

			*((vector3*)&(m_pVertex[lActualVertexIndex].x)) = m_paTrace[lActualIndex][0];
			*((vector3*)&(m_pVertex[lActualVertexIndex+1].x)) = m_paTrace[lActualIndex][1];
		}
	}

	if( m_lTessel )
	{
		if( m_lLength > 1 )
		{
			// 보간처리
			if( 2 == m_lLength )
			{
				lBoundary = m_bInputStopped ? 2 : 1;
				InterpolateInbetween(
					&m_pVertex[1*2],
					&m_pVertex[0 * (m_lTessel+1)*2],
					&m_pVertex[0 * (m_lTessel+1)*2],
					&m_pVertex[1 * (m_lTessel+1)*2],
					&m_pVertex[lBoundary * (m_lTessel+1)*2], m_lTessel );
			}
			else
			{
				lBoundary = m_bInputStopped ? m_lLength : (m_lLength-1);
				InterpolateInbetween(
					&m_pVertex[1*2],
					&m_pVertex[0 * (m_lTessel+1)*2],
					&m_pVertex[0 * (m_lTessel+1)*2],
					&m_pVertex[1 * (m_lTessel+1)*2],
					&m_pVertex[2 * (m_lTessel+1)*2], m_lTessel );
				InterpolateInbetween(
					&m_pVertex[((m_lLength-2)*(m_lTessel+1) + 1)*2],
					&m_pVertex[(m_lLength-3)*(m_lTessel+1)*2],
					&m_pVertex[(m_lLength-2)*(m_lTessel+1)*2],
					&m_pVertex[(m_lLength-1)*(m_lTessel+1)*2],
					&m_pVertex[lBoundary*(m_lTessel+1)*2], m_lTessel );
				for( i = 1; i < m_lLength-2; ++i )
				{
					InterpolateInbetween(
						&m_pVertex[(i*(m_lTessel+1) + 1)*2],
						&m_pVertex[(i-1) * (m_lTessel+1)*2],
						&m_pVertex[  i   * (m_lTessel+1)*2],
						&m_pVertex[(i+1) * (m_lTessel+1)*2],
						&m_pVertex[(i+2) * (m_lTessel+1)*2], m_lTessel );
				}

			}
		}
	}

	// 알파값 세팅(머리/꼬랑지부분 fade out)
	long lTmp = (m_lLength-1)*(m_lTessel+1) + 1;
	lBoundary  = max( lTmp/2, m_lTessel+2 );
	for( i = 0; i < lTmp; ++i )
	{
		m_pVertex[i*2].v = float(i)/lTmp;
		m_pVertex[i*2+1].v = float(i)/lTmp;

		// 꼬랑지
		if( (lTmp - i) < lBoundary )
		{
			D3DCOLOR c = D3DCOLOR_ARGB( 2*(int)(float(lTmp-i)/lBoundary*60.0f + 0.0f) , m_R, m_G, m_B );
			m_pVertex[i*2].c = c;
			m_pVertex[i*2+1].c = c;
		}
		// 머리
		else if( i < lBoundary/2 )
		{
			D3DCOLOR c = D3DCOLOR_ARGB( 2*(int)(float(i)/lBoundary*100.0f + 10.0f) , m_R, m_G, m_B );
			m_pVertex[i*2].c = c;
			m_pVertex[i*2+1].c = c;
		}
		else
		{
			m_pVertex[i*2].c = D3DCOLOR_ARGB( 0xB8, m_R, m_G, m_B );
			m_pVertex[i*2+1].c = D3DCOLOR_ARGB( 0xB8, m_R, m_G, m_B );
		}
	}

	if( !m_bInputStopped )
	{
		m_lHeadIndex = (m_lHeadIndex+1) % m_lDuration;
	}
}


float CZ3DBowString::ms_fTensionConstant[Z3D_BOWSTRING_TENSOIN_COUNT] =
{ 0.0f, -0.03f, 0.03f, -0.04f, 0.07f, -0.1f, 0.3f, 0.7f, 1.0f };

CZ3DBowString::CZ3DBowString( CZ3DAttachment* pWeapon, char* szTex, CZ3DObject* pManipulator )
{
	m_rpManipulator = pManipulator;
	m_rpWeapon = pWeapon;

	m_pTagTexture = g_ContTexture.GetObject( szTex );
	m_pTexture = m_pTagTexture->GetD3dTexture();

	m_pVertex = new D3DLVERTEX[4*7];

	SetSecondRenderBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	for( int i = 0; i < 4; ++i )
	{
		m_pVertex[i].c = D3DCOLOR_ARGB( 0xFF, 0xF, 0xF, 0xF );
	}
	m_pVertex[0].u = m_pVertex[1].u = m_pVertex[2].u = 0.5f;

	m_pVertex[0].v = 0.0f;
	m_pVertex[1].v = 0.5f;
	m_pVertex[2].v = 0.5f;
	m_pVertex[3].v = 1.0f;

	for(int i = 1; i < 7; ++i )
	{
		m_pVertex[i*4] = m_pVertex[0];
		m_pVertex[i*4+1] = m_pVertex[1];
		m_pVertex[i*4+2] = m_pVertex[2];
		m_pVertex[i*4+3] = m_pVertex[3];
	}

	m_bGripped = false;
//	m_bNoForce = true;
//	m_fTensionLength = 0.0f;
	m_lAfterProjection = 0;

	m_vTensionDir.Identity();

	Link( NULL );

	Disable();
}


CZ3DBowString::~CZ3DBowString()
{
	UnLink();

	SAFE_DELETEA( m_pVertex );

	// Release() 메소드가 아직 없다
//	m_tagTexture.Release();
}


void CZ3DBowString::SecondRender()
{
	GetDevice()->SetVertexShader( D3DFVF_LVERTEX );
	matrix m;
	m.MakeIdent();
	m._11 = m._22 = m._33 = m_rpWeapon->GetScale();
	m._41 = m_vPivotPos.x;
	m._42 = m_vPivotPos.y;
	m._43 = m_vPivotPos.z;
	GetDevice()->SetTransform( D3DTS_WORLD, m );

	GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, 2*7, m_pVertex, sizeof(D3DLVERTEX) );
}

void CZ3DBowString::GripString()
{
	vector3 v1, v2;

	m_bGripped = true;
//	m_bNoForce = false;
	m_lAfterProjection = 0;

	m_rpWeapon->GetMarker( v1, v2 );
//	m_vPrevMid = (v1+v2)/2;
}


void CZ3DBowString::ReleaseString()
{
	m_bGripped = false;

	vector3 v1, v2, v3;

	m_rpWeapon->GetMarker( v1, v2 );
	m_rpManipulator->GetPosition( v3 );
	m_vTensionDir = (v3 - ((v1+v2)/2));
//	m_vPrevMid = (v1+v2)/2;
	
	// 활시위가 가운데까지 돌아가는데 Z3D_BOWSTRING_TENSOIN_COUNT 프레임(index 0 포함이므로 -1)
	m_lAfterProjection = Z3D_BOWSTRING_TENSOIN_COUNT-1;	
}


void CZ3DBowString::UpdateFrame()
{
	vector3 v1, v2, v3;

	m_rpWeapon->GetPivot( m_vPivotPos );

	m_rpWeapon->GetMarker( v1, v2 );

	*((vector3*)&(m_pVertex[0].x)) = v1;
	*((vector3*)&(m_pVertex[3].x)) = v2;


	if( m_bGripped )
	{
		m_rpManipulator->GetPosition( v3 );
	}
	else
	{
		if( m_lAfterProjection > 0 )
		{
			v3 = ((v1 + v2)/2) + (ms_fTensionConstant[m_lAfterProjection]*m_vTensionDir);
			--m_lAfterProjection;
		}
		else
		{
			v3 = (v1 + v2)/2;
		}

	}
	*((vector3*)&(m_pVertex[1].x)) = v3;
	*((vector3*)&(m_pVertex[2].x)) = v3;

	int i, j;
	float fShift = 0.1f / m_rpWeapon->GetScale();

	j = 1;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x + fShift;
		m_pVertex[j*4 + i].y = m_pVertex[i].y;
		m_pVertex[j*4 + i].z = m_pVertex[i].z;
	}

	j = 2;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x - fShift;
		m_pVertex[j*4 + i].y = m_pVertex[i].y;
		m_pVertex[j*4 + i].z = m_pVertex[i].z;
	}

	j = 3;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x;
		m_pVertex[j*4 + i].y = m_pVertex[i].y + fShift;
		m_pVertex[j*4 + i].z = m_pVertex[i].z;
	}

	j = 4;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x;
		m_pVertex[j*4 + i].y = m_pVertex[i].y - fShift;
		m_pVertex[j*4 + i].z = m_pVertex[i].z;
	}

	j = 5;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x;
		m_pVertex[j*4 + i].y = m_pVertex[i].y;
		m_pVertex[j*4 + i].z = m_pVertex[i].z + fShift;
	}

	j = 6;
	for( i = 0; i < 4; ++i )
	{
		m_pVertex[j*4 + i].x = m_pVertex[i].x;
		m_pVertex[j*4 + i].y = m_pVertex[i].y;
		m_pVertex[j*4 + i].z = m_pVertex[i].z - fShift;
	}
}
