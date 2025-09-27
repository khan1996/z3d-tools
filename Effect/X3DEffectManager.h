// X3DEffectManager.h: interface for the CX3DEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTMANAGER_H__4DAA8921_D30C_4F05_B138_89DFFC19B449__INCLUDED_)
#define AFX_X3DEFFECTMANAGER_H__4DAA8921_D30C_4F05_B138_89DFFC19B449__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)  // don't warn about browse name overflow.

#include <d3d8.h>
#include <d3dx8.h>
#include <vector>

#include "X3DEffect.h"
#include "X3DEffectSphere.h"
#include "SectorEffectMap.h"
#include "CEffscript.h"
#include "ViewFrustum.h"

#define DEFAULTLIMITSCRIPT 5

using namespace std;

typedef unsigned long XEFFECT;
typedef vector<CX3DEffect *> EffectList;
typedef vector<XEFFECT> EffectHandleList;
typedef vector<CEffScript *> EffectScriptList;
typedef vector<CLightning *> LightningList;

class CX3DEffectManager  
{
	protected:
		LPDIRECT3DDEVICE8 m_lpD3DDevice;
		matrix m_matView;
		matrix m_matWorld;	
	
		EffectHandleList m_lstExist;
		EffectHandleList m_lstFree;
		
		// 배경 script list
		EffectScriptList m_lstWScript;
		// Interface script list
		EffectScriptList m_lstIScript;
		
		//Lightning list
		LightningList m_lstLightning;

		unsigned long m_dwTick;
		unsigned long m_dwOldTick;
		int m_EffScriptLimitNum;
		int m_EffScriptLimitCount;

		CViewFrustum *m_pFrustum;

		
	public:	
		EffectList m_lstEffect;
		// skill script list
		EffectScriptList m_lstScript;

		XEFFECT AddEffect(CX3DEffect *lpNewEffect);

		CEffScript *AddEffScript(CEffScript *);
		int AddWorldEffScript(CEffScript *);
		CEffScript *AddInterfaceScript(CEffScript *);
		void AddLightning(CLightning *);
		
		// play script :return index num
		int ProcessScript(int index);
		bool ProcessWScript(int index,bool bCull = true);
		bool ProcessInterfaceScript(int index);
		bool ProcessLightning(int index);

		bool CheckScript(int index,CEffScript *);
		bool CheckNullScript(CEffScript *t);
		bool CheckNullInterfaceScript(CEffScript *t);
		bool CheckInterfaceScript(int index,CEffScript *t);
		
		int GetScriptNum();
		int GetWScriptNum();
		int GetLightningNum();

		char *GetEsfName(int index);
		
		int GetInterfaceScriptNum();
		void SetEffScriptLimitNum(int n) {m_EffScriptLimitNum = n;}
		int GetEffScriptLimitNum() {return m_EffScriptLimitNum;}


		void RenderScript();
		void RenderWorldScript(bool bCull = true);
		void RenderLightning();

		void RenderInterfaceScript();

		void DeleteEndScript(int i);
		void DeleteEndScript(CEffScript *del);
		void DeleteWScript(int i);
		void DeleteLightning(int i);
		
		void DeleteInterfaceScript(int i);
		void DeleteInterfaceScript(CEffScript *del);

		void DeleteEffect(XEFFECT hEffect);
		BOOL IsLive(XEFFECT hEffect);

		void Render();
		void Render(int num);
		void UpdateFrame(void);
		CX3DEffectManager();
		~CX3DEffectManager();

		void DeleteAllEffect(void);
		void DeleteAllEffectScript();
		void DeleteAllInterfaceScript();
		void DeleteAllLightning(); 

		// 월드 스크립트 
		void DeleteAllWorldScript();
		void SetDevice(LPDIRECT3DDEVICE8 pD3DDevice) { m_lpD3DDevice = pD3DDevice; }
		LPDIRECT3DDEVICE8 GetDevice(void) { return m_lpD3DDevice; }

		matrix *GetViewMatrix(void) { return &m_matView; }
};

#endif // !defined(AFX_X3DEFFECTMANAGER_H__4DAA8921_D30C_4F05_B138_89DFFC19B449__INCLUDED_)
