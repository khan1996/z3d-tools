// FrameTimer.h: interface for the CFrameTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMETIMER_H__6C017430_DEBB_4372_B200_70D57C02B061__INCLUDED_)
#define AFX_FRAMETIMER_H__6C017430_DEBB_4372_B200_70D57C02B061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3d8.h>
#include <vector>

class CFrameTimer  
{
public:
	static float GetUpdateTimer(int nTimer);
	static void ResetTimer(int nTimer);
	static int Regist(float fPerSecondUpdate);
	static void UpdateTime();	

	static std::vector<float> m_fUpdateTimeList;
	static std::vector<float> m_fTimeRemainList;
	static std::vector<float> m_fPerSecondUpdateList;
	static DWORD m_dwTickTime;
	static DWORD m_dwLastUpdateTime;	
	static void Create();
	CFrameTimer();
	virtual ~CFrameTimer();
};

#endif // !defined(AFX_FRAMETIMER_H__6C017430_DEBB_4372_B200_70D57C02B061__INCLUDED_)
