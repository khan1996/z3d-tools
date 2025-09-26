// RBspSceneManager.h: interface for the RBspSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RBSPSCENEMANAGER_H__64ADEA70_AAD1_466D_A048_04FFA818E581__INCLUDED_)
#define AFX_RBSPSCENEMANAGER_H__64ADEA70_AAD1_466D_A048_04FFA818E581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "RBspScene.h"
#include <vector>

using namespace std;

class RBspSceneManager  
{
public:

	RBspSceneManager();
	~RBspSceneManager();
	
	int InsertBspScene(char *strFileName,float fScale);
	int InsertBseScene(char *strFileName,float fScale);

	RBspScene *SearchBspScene(char *strFileMame);
	int	DeleteBspScene(char *strFileName);
	void SelectBspScene(char *strFileName);
	void Render();
	void SetPickIndex(char *strFileName);
	void BspSceneOut();
	void UpdateBsp();
	void ToggleViewLeafBox();

	vector<RBspScene *> m_lstBspScene;
	int					m_iBspSceneNum;
	int					m_iCurrentSceneIndex;
	int					m_iPickIndex;
	bool				m_bViewLeafBox;

	RBspScene			*m_pCurrentBspScene;

	
};

#endif // !defined(AFX_RBSPSCENEMANAGER_H__64ADEA70_AAD1_466D_A048_04FFA818E581__INCLUDED_)
