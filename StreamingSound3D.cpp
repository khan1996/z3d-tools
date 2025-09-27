// StreamingSound3D.cpp: implementation of the CStreamingSound3D class.
//
//////////////////////////////////////////////////////////////////////

#include "DefineSound.h"
#include "StreamingSound.h"
#include "StreamingSound3D.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamingSound3D::CStreamingSound3D() : m_pStreamingSound(NULL), 
																		 m_pDS3DBuffer(NULL)
{
	ZeroMemory( &m_Prop, sizeof(DS3DBUFFER) );
}

CStreamingSound3D::~CStreamingSound3D()
{
	Destroy(); 
}


bool CStreamingSound3D::Create( char* strFileName )
{
	m_pStreamingSound = new CStreamingSound; 
	m_pStreamingSound->m_bSound3D = true; 
	if( !m_pStreamingSound->Create( strFileName ) )
	{
		Destroy(); 
		return false; 
	}

	m_pStreamingSound->m_pDSBuffer->QueryInterface( IID_IDirectSound3DBuffer8, (VOID**) &m_pDS3DBuffer ); 
	m_Prop.dwSize = sizeof(DS3DBUFFER); 
	m_pDS3DBuffer->GetAllParameters( &m_Prop ); 	
	return true; 
}
	
	
void CStreamingSound3D::Destroy()
{
	SAFE_RELEASE( m_pDS3DBuffer ); 
	SAFE_DELETE( m_pStreamingSound ); 
}
	

void CStreamingSound3D::SetConeAngles( DWORD dwInside, DWORD dwOutside )
{
	m_pDS3DBuffer->SetConeAngles( dwInside, dwOutside, DS3D_IMMEDIATE ); 
	m_Prop.dwInsideConeAngle = dwInside;
	m_Prop.dwOutsideConeAngle = dwOutside; 
}


void CStreamingSound3D::SetConeOrientation( D3DVECTOR& vOrientation )
{
	m_pDS3DBuffer->SetConeOrientation( vOrientation.x, 
									   vOrientation.y, 
									   vOrientation.z, DS3D_IMMEDIATE ); 
	memcpy( &m_Prop.vConeOrientation, &vOrientation, sizeof(D3DVECTOR) );
}


void CStreamingSound3D::SetConeOutsideVolume( float fVolume )
{
	m_pDS3DBuffer->SetConeOutsideVolume( LinearToLogVol(fVolume), DS3D_IMMEDIATE ); 
	m_Prop.lConeOutsideVolume = LinearToLogVol(fVolume); 
}


void CStreamingSound3D::SetMaxDistance( float fMaxDist )
{
	m_pDS3DBuffer->SetMaxDistance( fMaxDist, DS3D_IMMEDIATE ); 
	m_Prop.flMaxDistance = fMaxDist; 
}


void CStreamingSound3D::SetMinDistance( float fMinDist )
{
	m_pDS3DBuffer->SetMinDistance( fMinDist, DS3D_IMMEDIATE ); 
	m_Prop.flMinDistance = fMinDist; 
}


void CStreamingSound3D::SetMode( DWORD dwMode )
{
	m_pDS3DBuffer->SetMode( dwMode, DS3D_IMMEDIATE ); 
	m_Prop.dwMode = dwMode; 
}


void CStreamingSound3D::SetPosition( D3DVECTOR& vPos )
{
	m_pDS3DBuffer->SetPosition( vPos.x, vPos.y, vPos.z, DS3D_IMMEDIATE ); 
	memcpy( &m_Prop.vPosition, &vPos, sizeof(D3DVECTOR) ); 
}


void CStreamingSound3D::SetVelocity( const D3DVECTOR& vVel )
{
	m_pDS3DBuffer->SetVelocity( vVel.x, vVel.y, vVel.z, DS3D_IMMEDIATE ); 
	memcpy( &m_Prop.vVelocity, &vVel, sizeof(D3DVECTOR) ); 
}


void CStreamingSound3D::GetConeAngles( DWORD& dwInside, DWORD& dwOutside )
{
	dwInside = m_Prop.dwInsideConeAngle; 
	dwOutside = m_Prop.dwOutsideConeAngle; 
}


void CStreamingSound3D::GetConeOrientation( D3DVECTOR& vOrientation )
{
	memcpy( &vOrientation, &m_Prop.vConeOrientation, sizeof(D3DVECTOR) );
}


void CStreamingSound3D::GetConeOutsideVolume( float& fVolume )
{
	fVolume = LogToLinearVol( m_Prop.lConeOutsideVolume );
}


void CStreamingSound3D::GetMaxDistance( float& fMaxDist )
{
	fMaxDist = m_Prop.flMaxDistance; 
}


void CStreamingSound3D::GetMinDistance( float& fMinDist )
{
	fMinDist = m_Prop.flMinDistance; 
}


void CStreamingSound3D::GetMode( DWORD& dwMode )
{
	dwMode = m_Prop.dwMode; 
}


void CStreamingSound3D::GetPosition( D3DVECTOR& vPos )
{
	memcpy( &vPos, &m_Prop.vPosition, sizeof(D3DVECTOR) ); 
}


void CStreamingSound3D::GetVelocity( D3DVECTOR& vVel )
{
	memcpy( &vVel, &m_Prop.vVelocity, sizeof(D3DVECTOR) );
}