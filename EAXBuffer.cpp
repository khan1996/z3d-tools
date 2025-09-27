// CEAXBuffer.cpp: implementation of the CCEAXBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "DefineSound.h"
#include "EAXBuffer.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CEAXBuffer::CEAXBuffer()
{
	m_pPropertySet					= 0;
	m_Props.lDirect					= EAXBUFFER_DEFAULTDIRECT;                
	m_Props.lDirectHF				= EAXBUFFER_DEFAULTDIRECTHF;              
	m_Props.lRoom					= EAXBUFFER_DEFAULTROOM;                  
	m_Props.lRoomHF					= EAXBUFFER_DEFAULTROOMHF;                
	m_Props.flRoomRolloffFactor		= EAXBUFFER_DEFAULTROOMROLLOFFFACTOR;   
	m_Props.lObstruction			= EAXBUFFER_DEFAULTOBSTRUCTION;           
	m_Props.flObstructionLFRatio	= EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO;  
	m_Props.lOcclusion				= EAXBUFFER_DEFAULTOCCLUSION;             
	m_Props.flOcclusionLFRatio		= EAXBUFFER_DEFAULTOCCLUSIONLFRATIO;    
	m_Props.flOcclusionRoomRatio	= EAXBUFFER_DEFAULTOCCLUSIONROOMRATIO;  
	m_Props.lOutsideVolumeHF		= EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF;       
	m_Props.flAirAbsorptionFactor	= EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR; 
	m_Props.dwFlags					= EAXBUFFER_DEFAULTFLAGS;      
}

CEAXBuffer::~CEAXBuffer()
{
	Destroy();
}

bool CEAXBuffer::Create( IUnknown* pUnknown )
{
	if(!pUnknown)
		return false;
	HRESULT hr = pUnknown->QueryInterface(IID_IKsPropertySet, (void**)&m_pPropertySet);
	if(FAILED(hr))
		return false;

	DWORD nSupport = 0;
	hr = m_pPropertySet->QuerySupport(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
		&nSupport);
	if((nSupport != (KSPROPERTY_SUPPORT_GET | 
		KSPROPERTY_SUPPORT_SET)))
	{
		SAFE_RELEASE(m_pPropertySet);
		return false;
	}

	hr = m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
		0,
		0,
		(void*)(&m_Props),
		sizeof(EAXBUFFERPROPERTIES));
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pPropertySet);
		return false;
	}
	return true;
}

void CEAXBuffer::Destroy()
{
	SAFE_RELEASE(m_pPropertySet);
}

void CEAXBuffer::SetDirect(int iDirect)
{
	m_Props.lDirect = iDirect;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_DIRECT,
		0,
		0,
		&iDirect,
		sizeof(int));
}

void CEAXBuffer::SetDirectHF(int iDirectHF)
{
	m_Props.lDirectHF = iDirectHF;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_DIRECTHF,
		0,
		0,
		&iDirectHF,
		sizeof(int));
}

void CEAXBuffer::SetRoom(int iRoom)
{
	m_Props.lRoom = iRoom;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ROOM,
		0,
		0,
		&iRoom,
		sizeof(int));
}

void CEAXBuffer::SetRoomHF(int iRoomHF)
{
	m_Props.lRoomHF = iRoomHF;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ROOMHF,
		0,
		0,
		&iRoomHF,
		sizeof(int));
}

void CEAXBuffer::SetRoomRolloffFactor(float fRoomRolloffFactor)
{
	m_Props.flRoomRolloffFactor = fRoomRolloffFactor;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ROOMROLLOFFFACTOR,
		0,
		0,
		&fRoomRolloffFactor,
		sizeof(float));
}

void CEAXBuffer::SetObstruction(int iObstruction)
{
	m_Props.lObstruction = iObstruction;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OBSTRUCTION,
		0,
		0,
		&iObstruction,
		sizeof(int));
}

void CEAXBuffer::SetObstructionLFRatio(float fObstructionLFRatio)
{
	m_Props.flObstructionLFRatio = fObstructionLFRatio;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO,
		0,
		0,
		&fObstructionLFRatio,
		sizeof(float));
}

void CEAXBuffer::SetOcclusion(int iOcclusion)
{
	m_Props.lOcclusion = iOcclusion;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OCCLUSION,
		0,
		0,
		&iOcclusion,
		sizeof(int));
}

void CEAXBuffer::SetOcclusionLFRatio(float fOcclusionLFRatio)
{
	m_Props.flOcclusionLFRatio = fOcclusionLFRatio;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO,
		0,
		0,
		&fOcclusionLFRatio,
		sizeof(float));
}

void CEAXBuffer::SetOcclusionRoomRatio(float fOcclusionRoomRatio)
{
	m_Props.flOcclusionRoomRatio = fOcclusionRoomRatio;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO,
		0,
		0,
		&fOcclusionRoomRatio,
		sizeof(float));
}

void CEAXBuffer::SetOutsideVolumeHF(int iOutsideVolumeHF)
{
	m_Props.lOutsideVolumeHF = iOutsideVolumeHF;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_OUTSIDEVOLUMEHF,
		0,
		0,
		&iOutsideVolumeHF,
		sizeof(int));
}

void CEAXBuffer::SetAirAbsorptionFactor(float fAirAbsorptionFactor)
{
	m_Props.flAirAbsorptionFactor = fAirAbsorptionFactor;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_AIRABSORPTIONFACTOR,
		0,
		0,
		&fAirAbsorptionFactor,
		sizeof(float));
}

void CEAXBuffer::SetFlags(DWORD nFlags)
{
	m_Props.dwFlags = nFlags;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_FLAGS,
		0,
		0,
		&nFlags,
		sizeof(DWORD));
}

void CEAXBuffer::SetProperties(const EAXBUFFERPROPERTIES& props)
{
	m_Props = props;
	if(!m_pPropertySet) return;
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_BufferProperties,
		DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
		0,
		0,
		(void*)(&props),
		sizeof(EAXBUFFERPROPERTIES));
}

void CEAXBuffer::GetProperties(EAXBUFFERPROPERTIES& props)
{
	/*
	DWORD nSize;
	if(m_pPropertySet)
	{
		m_pPropertySet->Get(
			DSPROPSETID_EAX20_BufferProperties,
			DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
			0,
			0,
			&m_Props,
			sizeof(EAXBUFFERPROPERTIES),
			&nSize);
	}*/

	props = m_Props;
}
