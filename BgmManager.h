// BgmManager.h: interface for the CBgmManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGMMANAGER_H__CF4545F4_7FA9_4473_8450_171C8740AE69__INCLUDED_)
#define AFX_BGMMANAGER_H__CF4545F4_7FA9_4473_8450_171C8740AE69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "d3dx8.h"
#include "SectorBGMTrigger.h"

class CSceneEventMgr; 

class CBgmManager  
{
	friend class CSceneEventMgr; 

	struct EventTable
	{
		int nEvent; 
		DWORD dwColor; 
		EventTable() : nEvent(-1), dwColor(0) {}
		void Update() { nEvent = -1; dwColor = 0; }
	};

	enum BGM_STATE
	{
		BS_ZERO = 0, 
		BS_PLACE_IN, 
		BS_PLACE_OUT, 
		BS_BGM_STOP, 
		BS_BGM_PLAY 
	};

	BGM_STATE m_ePlaceStat; 
	BGM_STATE m_eBgmStat; 

protected:
	static CBgmManager*		ms_pInstance; 
	CSectorBGMTrigger			m_Trigger; 
	bool									m_bInPlace; 
	float									m_fBgmVol; 
	float									m_fMasterVolume; 
	bool									m_bActivate; 
	char									m_strCurBgm[256]; 
	bool									m_bBgmTurnEvent; 
	EventTable								m_EventTable; 

	DWORD									m_EventColor;

	BOOL									m_bActBGM;

	//TestMode
	bool m_bTestMode; 
	bool m_bTestShow; 
	LPDIRECT3DTEXTURE8		m_pBGMTexture; 

protected:
	CBgmManager(); 
	virtual ~CBgmManager();

	void GetSectorXYFromPos( int& outX , int& outY, D3DXVECTOR3 inPos ); 
	void GetTextureLockPosFromPos( int& outX , int& outY, D3DXVECTOR3 inPos ); 
	bool GetBGMEventInfo( D3DXVECTOR3 inPos, 
		                  PerSectorTriggerInfo* pinInfo, char*& outStrFile, int& outEventKey, char*& outESFFile, DWORD& outEventColor); 
	void LoadTexture( PerSectorTriggerInfo* pInfo ); 
	void TestScreen( D3DXVECTOR3 Pos ); 

public:
	void PlayBGM( char* strFilename, int EventKey, BOOL bAct = FALSE ); 
	void StopBGM();

	static CBgmManager* _GetInstance(); 
	static void _Destroy();		
	void Create( bool bTestMode=false ); 
	void ReleaseAllData(); 
	void Update( D3DXVECTOR3& Pos, float fSkip, bool bCharSel ); 
	CSectorBGMTrigger* GetBGMTrigger() { return &m_Trigger; }
	void SetVolume( float fVol ) { m_fMasterVolume = fVol; 	}
	void AllStop();
	bool IsSafetyZone( D3DXVECTOR3& Pos ); 
	bool IsCurBgmTurnAmbEvent(); 
	void SetActivate( bool bAct ); 
	bool IsActivated(); 
};

#endif // !defined(AFX_BGMMANAGER_H__CF4545F4_7FA9_4473_8450_171C8740AE69__INCLUDED_)
