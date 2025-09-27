// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#include "DefineSound.h"
#include "SoundMgr.h"
#include "Listener.h"
#include "GMMemory.h"

CListener::CListener( LPDIRECTSOUND8 pDS ) : m_pListener(NULL), m_pDSBuffer(NULL)
{
	Create( pDS ); 
}


CListener::~CListener()
{
	Destory(); 
}


void CListener::Create( LPDIRECTSOUND8 pDS )
{
	DSBUFFERDESC        dsbdesc;
   LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
   dsbdesc.dwSize = sizeof(DSBUFFERDESC);
   dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
    
	if( FAILED(pDS->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)) )
	{
        MessageBox( NULL, "FAILED(pDS->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL))", 
			"CListener::Create", MB_OK ); 
		SAFE_RELEASE( pDSBPrimary );
		Destory(); 
	    return; 
	}

    if( FAILED(pDSBPrimary->QueryInterface(IID_IDirectSound3DListener8, 
                                                  (VOID**)&m_pListener)) )
    {
        SAFE_RELEASE( pDSBPrimary );
		MessageBox( NULL, "FAILED(pDSBPrimary->QueryInterface", 
			"CListener::Create", MB_OK ); 
		SAFE_RELEASE( pDSBPrimary ); 
		Destory(); 
		return; 
    }

	m_Prop.dwSize = sizeof(DS3DLISTENER);
	m_pListener->GetAllParameters( &m_Prop ); 
	SAFE_RELEASE( pDSBPrimary );

	//=============================
	// EAX Listener
	//============================= 
	if( CSoundMgr::_GetInstance()->GetInitProp()->m_bUseEAX == false ) return; 

	WAVEFORMATEX wf;
    memset(&wf, 0, sizeof(WAVEFORMATEX)); 
    wf.wFormatTag = WAVE_FORMAT_PCM; 
    wf.nChannels = 1; 
    wf.nSamplesPerSec = 22050; 
    wf.nBlockAlign = 2; 
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign; 
    wf.wBitsPerSample = 16; 

	DSBUFFERDESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_LOCHARDWARE | DSBCAPS_CTRL3D;
	desc.lpwfxFormat = &wf;
	desc.dwBufferBytes = 1024;
	
	LPDIRECTSOUNDBUFFER pDSBuffer =	NULL;
	if( FAILED(pDS->CreateSoundBuffer(&desc, &pDSBuffer, NULL)) )
	{
		MessageBox( NULL, "Not Use EAX!", "Fail!", MB_OK ); 
		return;
	}

	if( FAILED(pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDSBuffer)) )
	{
		MessageBox( NULL, "FAILED(pDSBuffer->QueryInterface", 
			"CListener::Create", MB_OK ); 
		return; 
	}

	SAFE_RELEASE(pDSBuffer);

	//Success
	m_EAXListener.Create( m_pDSBuffer ); 
}


void CListener::Destory()
{
	SAFE_RELEASE( m_pDSBuffer ); 
	SAFE_RELEASE( m_pListener ); 
}


void CListener::Update()
{
	m_pListener->SetAllParameters( &m_Prop, DS3D_IMMEDIATE ); 
}


void CListener::SetDistanceFactor( float fDistanceFactor )
{
	m_Prop.flDistanceFactor = fDistanceFactor; 
}


void CListener::SetDopplerFactor( float fDopplerFactor )
{
	m_Prop.flDopplerFactor = fDopplerFactor; 
}


void CListener::SetOrientation( D3DVECTOR& vFront, D3DVECTOR& vTop )
{
	memcpy( &m_Prop.vOrientFront, &vFront, sizeof(D3DVECTOR) );
	memcpy( &m_Prop.vOrientTop, &vTop, sizeof(D3DVECTOR) );
}


void CListener::SetPosition( D3DVECTOR& vPos )
{
	memcpy( &m_Prop.vPosition, &vPos, sizeof(D3DVECTOR) ); 
}


void CListener::SetRolloffFactor( float fRolloffFactor )
{
	m_Prop.flRolloffFactor = fRolloffFactor; 
}


void CListener::SetVelocity( D3DVECTOR& vVel )
{
	memcpy( &m_Prop.vVelocity, &vVel, sizeof(D3DVECTOR) ); 
}


void CListener::GetDistanceFactor( float& fDistanceFactor )
{
	fDistanceFactor = m_Prop.flDistanceFactor; 
}


void CListener::GetDopplerFactor( float& fDopplerFactor )
{
	fDopplerFactor = m_Prop.flDopplerFactor; 
}


void CListener::GetOrientation( D3DVECTOR& vFront, D3DVECTOR& vTop )
{
	memcpy( &vFront, &m_Prop.vOrientFront, sizeof(D3DVECTOR) );
	memcpy( &vTop, &m_Prop.vOrientTop, sizeof(D3DVECTOR) );
}


void CListener::GetPosition( D3DVECTOR& vPos )
{
	memcpy( &vPos, &m_Prop.vPosition, sizeof(D3DVECTOR) );
}


void CListener::GetRolloffFactor( float& fRolloffFactor )
{
	fRolloffFactor = m_Prop.flRolloffFactor;
}


void CListener::GetVelocity( D3DVECTOR& vVel )
{
	memcpy( &vVel, &m_Prop.vVelocity, sizeof(D3DVECTOR) );
}


