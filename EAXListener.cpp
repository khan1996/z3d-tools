// CEAXListener.cpp: implementation of the CCEAXListener class.
//
//////////////////////////////////////////////////////////////////////

#include "DefineSound.h"
#include "EAXListener.h"
#include "GMMemory.h"


CEAXListener::CEAXListener()
{
	m_pPropertySet					= 0;
	m_Props.lRoom					= EAXLISTENER_DEFAULTROOM;                    
	m_Props.lRoomHF					= EAXLISTENER_DEFAULTROOMHF;                  
	m_Props.flRoomRolloffFactor		= EAXLISTENER_DEFAULTROOMROLLOFFFACTOR;     
	m_Props.flDecayTime				= EAXLISTENER_DEFAULTDECAYTIME;             
	m_Props.flDecayHFRatio			= EAXLISTENER_DEFAULTDECAYHFRATIO;          
	m_Props.lReflections			= EAXLISTENER_DEFAULTREFLECTIONS;             
	m_Props.flReflectionsDelay		= EAXLISTENER_DEFAULTREFLECTIONSDELAY;      
	m_Props.lReverb					= EAXLISTENER_DEFAULTREVERB;                  
	m_Props.flReverbDelay			= EAXLISTENER_DEFAULTREVERBDELAY;           
	m_Props.dwEnvironment			= EAXLISTENER_DEFAULTENVIRONMENT;   
	m_Props.flEnvironmentSize		= EAXLISTENER_DEFAULTENVIRONMENTSIZE;       
	m_Props.flEnvironmentDiffusion	= EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION;  
	m_Props.flAirAbsorptionHF		= EAXLISTENER_DEFAULTAIRABSORPTIONHF;       
	m_Props.dwFlags					= EAXLISTENER_DEFAULTFLAGS;         
}


CEAXListener::~CEAXListener()
{
	Destroy(); 
}


bool CEAXListener::Create( IUnknown* pUnknown )
{
	if(!pUnknown)
		return false;

	HRESULT hr = pUnknown->QueryInterface(IID_IKsPropertySet, (void**)&m_pPropertySet);
	if(FAILED(hr))
		return false;

	DWORD nSupport = 0;
	hr = m_pPropertySet->QuerySupport(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
		&nSupport);
	if((nSupport != (KSPROPERTY_SUPPORT_GET | 
		KSPROPERTY_SUPPORT_SET)))
	{
		SAFE_RELEASE(m_pPropertySet);
		return false;
	}

	hr = m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
		0,
		0,
		(void*)&m_Props,
		sizeof(EAXLISTENERPROPERTIES));
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pPropertySet);
		return false;
	}
	return true;
}


void CEAXListener::Destroy()
{
	SAFE_RELEASE(m_pPropertySet);
}


void CEAXListener::SetRoom(int iRoom)
{
	if(!m_pPropertySet) return;

	m_Props.lRoom = iRoom; 
	
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ROOM,
		0,
		0,
		&iRoom,
		sizeof(int));
}

void CEAXListener::SetRoomHF(int iRoomHF)
{
	if(!m_pPropertySet) return;

	m_Props.lRoomHF = iRoomHF; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ROOMHF,
		0,
		0,
		&iRoomHF,
		sizeof(int));
}

void CEAXListener::SetRoomRolloffFactor(float fRoomRolloffFactor)
{
	if(!m_pPropertySet) return;

	m_Props.flRoomRolloffFactor = fRoomRolloffFactor; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR,
		0,
		0,
		&fRoomRolloffFactor,
		sizeof(float));
}

void CEAXListener::SetDecayTime(float fDecayTime)
{
	if(!m_pPropertySet) return;

	m_Props.flDecayTime = fDecayTime; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_DECAYTIME,
		0,
		0,
		&fDecayTime,
		sizeof(float));
}

void CEAXListener::SetDecayHFRatio(float fDecayHFRatio)
{
	if(!m_pPropertySet) return;

	m_Props.flDecayHFRatio = fDecayHFRatio; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_DECAYHFRATIO,
		0,
		0,
		&fDecayHFRatio,
		sizeof(float));
}

void CEAXListener::SetReflections(int iReflections)
{
	if(!m_pPropertySet) return;

	m_Props.lReflections = iReflections; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_REFLECTIONS,
		0,
		0,
		&iReflections,
		sizeof(int));
}

void CEAXListener::SetReflectionsDelay(float fReflectionsDelay)
{
	if(!m_pPropertySet) return;

	m_Props.flReflectionsDelay = fReflectionsDelay; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,
		0,
		0,
		&fReflectionsDelay,
		sizeof(float));
}

void CEAXListener::SetReverb(int iReverb)
{
	if(!m_pPropertySet) return;

	m_Props.lReverb = iReverb; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_REVERB,
		0,
		0,
		&iReverb,
		sizeof(int));
}

void CEAXListener::SetReverbDelay(float fReverbDelay)
{
	if(!m_pPropertySet) return;

	m_Props.flReverbDelay = fReverbDelay; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_REVERBDELAY,
		0,
		0,
		&fReverbDelay,
		sizeof(float));
}

void CEAXListener::SetEnvironment(DWORD nEnvironment)
{
	if(!m_pPropertySet) return;

	m_Props.dwEnvironment = nEnvironment; 

	/*===========================
	EAX_ENVIRONMENT_GENERIC
    EAX_ENVIRONMENT_PADDEDCELL
    EAX_ENVIRONMENT_ROOM
    EAX_ENVIRONMENT_BATHROOM
    EAX_ENVIRONMENT_LIVINGROOM
    EAX_ENVIRONMENT_STONEROOM
    EAX_ENVIRONMENT_AUDITORIUM
    EAX_ENVIRONMENT_CONCERTHALL
    EAX_ENVIRONMENT_CAVE
    EAX_ENVIRONMENT_ARENA
    EAX_ENVIRONMENT_HANGAR
    EAX_ENVIRONMENT_CARPETEDHALLWAY
    EAX_ENVIRONMENT_HALLWAY
    EAX_ENVIRONMENT_STONECORRIDOR
    EAX_ENVIRONMENT_ALLEY
    EAX_ENVIRONMENT_FOREST
    EAX_ENVIRONMENT_CITY
    EAX_ENVIRONMENT_MOUNTAINS
    EAX_ENVIRONMENT_QUARRY
    EAX_ENVIRONMENT_PLAIN
    EAX_ENVIRONMENT_PARKINGLOT
    EAX_ENVIRONMENT_SEWERPIPE
    EAX_ENVIRONMENT_UNDERWATER
    EAX_ENVIRONMENT_DRUGGED
    EAX_ENVIRONMENT_DIZZY
    EAX_ENVIRONMENT_PSYCHOTIC
	=============================*/
	
	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ENVIRONMENT,
		0,
		0,
		&nEnvironment,
		sizeof(DWORD));
}

void CEAXListener::SetEnvironmentSize(float fEnvironmentSize)
{
	if(!m_pPropertySet) return;

	m_Props.flEnvironmentSize = fEnvironmentSize; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE,
		0,
		0,
		&fEnvironmentSize,
		sizeof(float));
}

void CEAXListener::SetEnvironmentDiffusion(float fEnvironmentDiffusion)
{
	if(!m_pPropertySet) return;

	m_Props.flEnvironmentDiffusion = fEnvironmentDiffusion; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,
		0,
		0,
		&fEnvironmentDiffusion,
		sizeof(float));
}

void CEAXListener::SetAirAbsorptionHF(float fAirAbsorptionHF)
{
	if(!m_pPropertySet) return;

	m_Props.flAirAbsorptionHF = fAirAbsorptionHF; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF,
		0,
		0,
		&fAirAbsorptionHF,
		sizeof(float));
}

void CEAXListener::SetFlags(DWORD nFlags)
{
	if(!m_pPropertySet) return;

	m_Props.dwFlags = nFlags; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_FLAGS,
		0,
		0,
		&nFlags,
		sizeof(DWORD));
}

void CEAXListener::SetProperties(const EAXLISTENERPROPERTIES& props)
{
	if(!m_pPropertySet) return;

	m_Props = props; 

	m_pPropertySet->Set(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
		0,
		0,
		(void*)&props,
		sizeof(EAXLISTENERPROPERTIES));
}

void CEAXListener::GetProperties(EAXLISTENERPROPERTIES& props)
{
	if(!m_pPropertySet) return;

	/*DWORD nSize;
	m_pPropertySet->Get(
		DSPROPSETID_EAX20_ListenerProperties,
		DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
		0,
		0,
		&props,
		sizeof(EAXLISTENERPROPERTIES),
		&nSize);*/

	props = m_Props; 
}