// HouseObjectNode.h: interface for the CHouseObjectNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEOBJECTNODE_H__DF571E5D_B6EA_4B04_8378_47CAFBC9F04B__INCLUDED_)
#define AFX_HOUSEOBJECTNODE_H__DF571E5D_B6EA_4B04_8378_47CAFBC9F04B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "HouseObject.h"

class CImposterScene; 

class CHouseObjectScene : public CSceneNode
{	
public:
	bool m_SelfLoadOut,m_SelfLoadMed,m_SelfLoadIn,m_SelfLoadInObject;
	
	long m_HouseSceneID;
	CHouseObject *m_HouseObject;	

	char m_strInName[MAX_NAMEBUFFER];
	char m_strMedName[MAX_NAMEBUFFER];
	char m_strOutName[MAX_NAMEBUFFER];	
	char m_strFlag[MAX_NAMEBUFFER];

	bool m_bOctree;
	bool m_bNotCull;
	bool m_bSkip;
	bool m_bMedCull;
	bool m_bInit;

	bool m_bFlag;

	CImposterScene* m_pImposterScene; 

	CHouseObjectScene();
	virtual ~CHouseObjectScene();
	
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	
	void SetSkipFlag(bool bSkip) { m_bSkip = bSkip;}
	bool GetSkipFlag() { return m_bSkip;}

	void SetFlag(bool bFlag)	{ m_bFlag = bFlag;	}
	bool GetFlag(void)			{ return m_bFlag;	}
};

/*

class CHouseObjectScene : public CSceneNode
{	
public:
	void RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void DeleteBspPosition();
	void UpdateBspPosition();
	bool m_SelfLoadOut,m_SelfLoadMed,m_SelfLoadIn,m_SelfLoadInObject;
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	long m_HouseSceneID;
	CHouseObject *m_HouseObject;		
	char m_strInName[MAX_NAMEBUFFER];
	char m_strMedName[MAX_NAMEBUFFER];
	char m_strOutName[MAX_NAMEBUFFER];
	char m_strBSPName[MAX_NAMEBUFFER];
	bool m_bOctree;
	bool m_bNotCull;
	bool m_bSkip;
	bool m_bMedCull;

	CHouseObjectScene();
	virtual ~CHouseObjectScene();
	
	void SetSkipFlag(bool bSkip) { m_bSkip = bSkip;}
	bool GetSkipFlag() { return m_bSkip;}

};
*/

#endif // !defined(AFX_HOUSEOBJECTNODE_H__DF571E5D_B6EA_4B04_8378_47CAFBC9F04B__INCLUDED_)
