// Sound3D.cpp: implementation of the CSound3D class.
//
//////////////////////////////////////////////////////////////////////

#include "DefineSound.h"
#include "Sound.h"
#include "Sound3D.h"
#include "SoundMgr.h"
#include "GMMemory.h"

CSound3D::CSound3D() : m_pSound( NULL ), 
										 m_ppDS3DBuffer( NULL )
{
}


CSound3D::~CSound3D()
{
	Destroy(); 
}


bool CSound3D::Create( const char* strFileName, DWORD dwNumBuffers )
{
	m_pSound = new CSound; 
	m_pSound->m_bSound3D = true; 
	
	if( !m_pSound->Create( strFileName, dwNumBuffers ) )
	{
		Destroy(); 
		return false; 
	}
	
	m_ppDS3DBuffer = new LPDIRECTSOUND3DBUFFER8[dwNumBuffers];
	
	for( DWORD i = 0; i < dwNumBuffers; i++ )
	{
		m_ppDS3DBuffer[i] = Get3DSoundBufferInterface( i ); 
	}

	//=============================
	// Create EAX 
	//=============================
//	if( CSoundMgr::_GetInstance()->GetInitProp()->m_bUseEAX )
//		m_EAXBuffer.Create( m_pDS3DBuffer ); 

	return true ;
}


void CSound3D::Destroy()
{
	for( DWORD i = 0; i < m_pSound->m_dwNumBuffers; i++ )
	{
		SAFE_RELEASE( m_ppDS3DBuffer[i] ); 
		SAFE_DELETE( m_ppDS3DBuffer[i] );
	}

	SAFE_DELETE_ARRAY( m_ppDS3DBuffer ) ;
	SAFE_DELETE( m_pSound ); 
}


LPDIRECTSOUND3DBUFFER8 CSound3D::Get3DSoundBufferInterface( DWORD dwIndex )
{
	LPDIRECTSOUND3DBUFFER8 pDS3DBuffer;
	HRESULT hr = m_pSound->m_ppDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer8, (void**)&pDS3DBuffer );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, "m_pSound->m_ppDSBuffer[dwIndex]->QueryInterface( IID_IDirectSound3DBuffer8, (void**)&pDS3DBuffer )", 
			"CSound3D::Get3DSoundBufferInterface", MB_OK ); 
		return NULL; 
	}
	
	return pDS3DBuffer;
}


void CSound3D::SetConeAngles( DWORD dwInside, DWORD dwOutside )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetConeAngles( dwInside, dwOutside, DS3D_IMMEDIATE ); 
}


void CSound3D::SetConeOrientation( D3DVECTOR& vOrientation )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetConeOrientation( vOrientation.x, 
									   vOrientation.y, vOrientation.z, DS3D_IMMEDIATE ); 
}


void CSound3D::SetConeOutsideVolume( float fVolume )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetConeOutsideVolume( LinearToLogVol(fVolume), DS3D_IMMEDIATE ); 
}


void CSound3D::SetMaxDistance( float fMaxDist )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetMaxDistance( fMaxDist, DS3D_IMMEDIATE );  
}


void CSound3D::SetMinDistance( float fMinDist )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetMinDistance( fMinDist, DS3D_IMMEDIATE ); 
}


void CSound3D::SetMode( DWORD dwMode )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetMode( dwMode, DS3D_IMMEDIATE ); 
}


void CSound3D::SetPosition( D3DVECTOR& vPos )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetPosition( vPos.x, vPos.y, vPos.z, DS3D_IMMEDIATE ); 
}


void CSound3D::SetVelocity( const D3DVECTOR& vVel )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->SetVelocity( vVel.x, vVel.y, vVel.z, DS3D_IMMEDIATE ); 
}


void CSound3D::GetConeAngles( DWORD& dwInside, DWORD& dwOutside )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetConeAngles( &dwInside, &dwOutside ); 
}


void CSound3D::GetConeOrientation( D3DVECTOR& vOrientation )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetConeOrientation( &vOrientation ); 
}


void CSound3D::GetConeOutsideVolume( float& fVolume )
{
	LPLONG V = NULL ;  
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetConeOutsideVolume( V ); 
	fVolume = LogToLinearVol( *V );
}


void CSound3D::GetMaxDistance( float& fMaxDist )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetMaxDistance( &fMaxDist ); 
}


void CSound3D::GetMinDistance( float& fMinDist )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetMinDistance( &fMinDist ); 
}


void CSound3D::GetMode( DWORD& dwMode )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetMode( &dwMode ); 
}


void CSound3D::GetPosition( D3DVECTOR& vPos )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetPosition( &vPos ); 
}


void CSound3D::GetVelocity( D3DVECTOR& vVel )
{
	m_ppDS3DBuffer[m_pSound->m_dwLatestPlayIndex]->GetVelocity( &vVel ); 
}


