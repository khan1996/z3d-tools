// FrameTimer.cpp: implementation of the CFrameTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "FrameTimer.h"
#include "GMMemory.h"

std::vector<float> CFrameTimer::m_fUpdateTimeList;
std::vector<float> CFrameTimer::m_fTimeRemainList;
std::vector<float> CFrameTimer::m_fPerSecondUpdateList;
DWORD CFrameTimer::m_dwTickTime;
DWORD CFrameTimer::m_dwLastUpdateTime;	

	
CFrameTimer g_FrameTimer;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrameTimer::CFrameTimer()
{
	m_dwTickTime = 0xFFFFFFFF;
}

CFrameTimer::~CFrameTimer()
{
	m_fUpdateTimeList.clear();
	m_fTimeRemainList.clear();
	m_fPerSecondUpdateList.clear();
}

void CFrameTimer::Create()
{
	m_dwTickTime = 0xFFFFFFFF;
}

void CFrameTimer::UpdateTime()
{
	if(m_dwTickTime == 0xFFFFFFFF)
	{
		m_dwTickTime=GetTickCount();
		m_dwLastUpdateTime=m_dwTickTime;		
	}	
	DWORD dwOldTickTime=m_dwTickTime;
	m_dwTickTime=GetTickCount();	

	DWORD dwIntervalPreTime=m_dwTickTime-dwOldTickTime;

	for(int i=0;i<(int)m_fUpdateTimeList.size();i++)
	{
		m_fUpdateTimeList[i]=(float)dwIntervalPreTime/(1000.0f/m_fPerSecondUpdateList[i]);
		m_fUpdateTimeList[i]+=m_fTimeRemainList[i];
		m_fTimeRemainList[i]=m_fUpdateTimeList[i]-(int)m_fUpdateTimeList[i];
	}	
}

int CFrameTimer::Regist(float fPerSecondUpdate)
{
	m_fUpdateTimeList.push_back(0.0f);
	m_fTimeRemainList.push_back(0.0f);
	m_fPerSecondUpdateList.push_back(fPerSecondUpdate);
	return (int)m_fUpdateTimeList.size()-1;
}

float CFrameTimer::GetUpdateTimer(int nTimer)
{
	return m_fUpdateTimeList[nTimer];
}

void CFrameTimer::ResetTimer(int nTimer)
{
	m_fUpdateTimeList[nTimer] = 0;
}
