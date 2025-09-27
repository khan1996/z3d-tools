// Listener.h: interface for the CListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENER_H__FE18A847_15A9_41C1_AD4D_E368D6D1A346__INCLUDED_)
#define AFX_LISTENER_H__FE18A847_15A9_41C1_AD4D_E368D6D1A346__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>
#include "EAXListener.h"

class CListener  
{
protected:
	LPDIRECTSOUND3DLISTENER8	m_pListener; 
	LPDIRECTSOUNDBUFFER8			m_pDSBuffer; 
	DS3DLISTENER								m_Prop;
	CEAXListener								m_EAXListener; 

protected:
	void Create( LPDIRECTSOUND8 pDS ); 
	void Destory();

public:
	CListener( LPDIRECTSOUND8 pDS );
	virtual ~CListener();

	void Update();  
	CEAXListener* GetEAX() { return &m_EAXListener; }; 
	
	void SetPosition(D3DVECTOR& vPos);
	void GetPosition(D3DVECTOR& vPos);

	void SetVelocity(D3DVECTOR& vVel);
	void GetVelocity(D3DVECTOR& vVel);

	void SetOrientation(D3DVECTOR& vFront, D3DVECTOR& vTop);
	void GetOrientation(D3DVECTOR& vFront, D3DVECTOR& vTop);

	void SetDistanceFactor(float fDistanceFactor);
	void GetDistanceFactor(float& fDistanceFactor);

	void SetDopplerFactor(float fDopplerFactor);
	void GetDopplerFactor(float& fDopplerFactor);

	void SetRolloffFactor(float fRolloffFactor);
	void GetRolloffFactor(float& fRolloffFactor);
};

#endif // !defined(AFX_LISTENER_H__FE18A847_15A9_41C1_AD4D_E368D6D1A346__INCLUDED_)
