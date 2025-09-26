// AmbienceManager.h: interface for the CAmbienceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMBIENCEMANAGER_H__0F58FE74_1039_42DF_94B3_9CCEAC652943__INCLUDED_)
#define AFX_AMBIENCEMANAGER_H__0F58FE74_1039_42DF_94B3_9CCEAC652943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SectorAmbienceMap.h"
#include "RenderTexture.h"

#include <vector>
using namespace std; 

class CD3DFont; 
class CSceneEventMgr; 

class CAmbienceManager  
{
	friend class CSceneEventMgr; 

protected:
	static CAmbienceManager*		ms_pInstance; 
	CSectorAmbienceMap*			m_pSectorAmbMap; 

	bool m_bFadeIn; 
	bool m_bFadeOut; 
	float m_fFadeInVolume; 
	float m_fFadeOutVolume; 
	char m_strFadeInFileName[MAX_PATH]; 
	char m_strFadeOutFileName[MAX_PATH]; 
	char m_strOldFile[MAX_PATH]; 
	bool m_bAmbPlay; 
	char m_strAmbPlayFile[MAX_PATH];
	float m_fAmbVolume; 
	float m_fMasterVolume; 
	AmbSchedule* m_pCurSchedule; 

	//TestMode
	bool m_bTestMode; 
	bool m_bTestShow; 
	CD3DFont* m_ptFont; 
	LPDIRECT3DTEXTURE8			m_pAmbTexture; 
	LPDIRECT3DTEXTURE8			m_pArrowTexture; 	
	LPD3DXSPRITE				m_pMouseArrow; 
	
protected:
	CAmbienceManager();
	virtual ~CAmbienceManager();
	void LoadTexture( SectorAmbience* ); 
	void GetTextureLockPos( int& outX, int& outY, D3DXVECTOR3 inPos ); 
	bool GetAmbienceInfo( SectorAmbience* pinInfo, D3DXVECTOR3& inPos, 
						  char*& outStrFile, D3DVECTOR& outPos, float& outVol, DWORD& outMode ); 
	void TestScreen(D3DXVECTOR3 Pos); 
	void RandomPlaySound(); 

public:
	static CAmbienceManager* _GetInstance(); 
	static void _Destroy(); 
	void Create( bool bTestMode=false ); 
	void ReleaseAllData(); 
	void Update( D3DXVECTOR3& Pos, float fSkip, bool bCharSel ); 
	void PlayAmbience( char* strFile, D3DVECTOR vPos, float fVol, DWORD dwMode ); 
	void SetTestScreen( bool bShow ) { m_bTestShow = bShow; };  
	CSectorAmbienceMap* GetAmbienceMap() { return m_pSectorAmbMap; }; 
	void AllStop(); 
	void SetVolume( float fVol ) { m_fMasterVolume = fVol; }
};

#endif // !defined(AFX_AMBIENCEMANAGER_H__0F58FE74_1039_42DF_94B3_9CCEAC652943__INCLUDED_)
