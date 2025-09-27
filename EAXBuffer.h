// EAXBuffer.h: interface for the CEAXBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EAXBUFFER_H__6BCC188D_ED42_4499_B923_4B61AA73B318__INCLUDED_)
#define AFX_EAXBUFFER_H__6BCC188D_ED42_4499_B923_4B61AA73B318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dsound.h"
#include "eax.h"

class CEAXBuffer  
{
private:
	IKsPropertySet*					m_pPropertySet;
	EAXBUFFERPROPERTIES		m_Props; 

public:
	CEAXBuffer();
	virtual ~CEAXBuffer();

	bool Create( IUnknown* pUnknown ); 
	void Destroy(); 
	void SetDirect(int iDirect);
	void SetDirectHF(int iDirectHF);
	void SetRoom(int iRoom);
	void SetRoomHF(int iRoomHF);
	void SetRoomRolloffFactor(float fRoomRolloffFactor);
	void SetObstruction(int iObstruction);
	void SetObstructionLFRatio(float fObstructionLFRatio);
	void SetOcclusion(int iOcclusion);
	void SetOcclusionLFRatio(float fOcclusionLFRatio);
	void SetOcclusionRoomRatio(float fOcclusionRoomRatio);
	void SetOutsideVolumeHF(int iOutsideVolumeHF);
	void SetAirAbsorptionFactor(float fAirAbsorptionFactor);
	void SetFlags(DWORD dwFlags);
	void SetProperties(const EAXBUFFERPROPERTIES& props);
	void GetProperties(EAXBUFFERPROPERTIES& props);
};

#endif // !defined(AFX_EAXBUFFER_H__6BCC188D_ED42_4499_B923_4B61AA73B318__INCLUDED_)
