// EAXListener.h: interface for the CEAXListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EAXLISTENER_H__2EAC4DBE_7D26_4C60_8206_B2034CAD2022__INCLUDED_)
#define AFX_EAXLISTENER_H__2EAC4DBE_7D26_4C60_8206_B2034CAD2022__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dsound.h"
#include "eax.h"

class CEAXListener  
{
protected:
	IKsPropertySet*					m_pPropertySet;
	EAXLISTENERPROPERTIES	m_Props; 

public:
	CEAXListener();
	virtual ~CEAXListener();

	bool Create( IUnknown* pUnknown ); 
	void Destroy(); 
	void SetRoom(int iRoom);
	void SetRoomHF(int iRoomHF);
	void SetRoomRolloffFactor(float fRoomRolloffFactor);
	void SetDecayTime(float fDecayTime);
	void SetDecayHFRatio(float fDecayHFRatio);
	void SetReflections(int iReflections);
	void SetReflectionsDelay(float fReflectionsDelay);
	void SetReverb(int iReverb);
	void SetReverbDelay(float fReverbDelay);
	void SetEnvironment(DWORD dwEnvironment);
	void SetEnvironmentSize(float fEnvironmentSize);
	void SetEnvironmentDiffusion(float fEnvironmentDiffusion);
	void SetAirAbsorptionHF(float fAirAbsorption);
	void SetFlags(DWORD dwFlags);
	void SetProperties(const EAXLISTENERPROPERTIES& props);
	void GetProperties(EAXLISTENERPROPERTIES& props);
};

#endif // !defined(AFX_EAXLISTENER_H__2EAC4DBE_7D26_4C60_8206_B2034CAD2022__INCLUDED_)
