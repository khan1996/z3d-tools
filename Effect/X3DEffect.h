// X3DEffect.h: interface for the CX3DEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECT_H__36D22C0B_3861_4B57_B539_4E4BFAD198C1__INCLUDED_)
#define AFX_X3DEFFECT_H__36D22C0B_3861_4B57_B539_4E4BFAD198C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)  // don't warn about browse name overflow.

#include <list>

#include "z3dmath.h"
#include "X3DEffectBase.h"

using namespace std;

class CX3DEffect
{
	protected:
		list<CX3DEffectBase *> m_lstEffect;
		LPDIRECT3DDEVICE8 m_lpD3DDevice;
		matrix *m_lpViewMatrix;

		unsigned long m_dwFrameTick;
		float m_fFrame;
		float m_fBeforeFrame;

		float m_fIncFrame;
		unsigned long m_dwMaxFrame;
		unsigned long m_dwMinFrame;
		// 이펙트 이동 중인지
		bool m_MoveState;

		BOOL m_bLoop;
		unsigned long m_dwKindMove;
		vector3 m_vecDest;
		vector3 m_vecPower;
		vector3 m_vecEForce;
		unsigned long m_dwCount;
		unsigned long m_dwOldTick;

		BOOL m_bImportCenter;
		vector3 *m_lpCenter;
		BOOL m_bImportAxis;
		quaternion *m_lpAxis;
		color m_lFlash;
		//scale value
		float m_GemScale[3];
		// effect가 시작 되었는지
		bool m_StartEffect;
		bool m_Mine;

		CKeyList<ColorKeyList> m_lstFlash;
		bool m_bLight;

	public:
		bool map_effect;
		int m_SectorX;
		int m_SectorY;

		CX3DEffect();
		virtual ~CX3DEffect();
		void SetMine(bool b); 

		void SetCenter(vector3 *pCenter) { m_bImportCenter = TRUE; m_lpCenter = pCenter; }
		void SetCenter(float fX, float fY, float fZ)
		{
			m_bImportCenter = FALSE;
			if(!m_lpCenter) m_lpCenter = new vector3;
			m_lpCenter->x = fX;
			m_lpCenter->y = fY;
			m_lpCenter->z = fZ;
		}
		vector3 *GetCenter(void) { return m_lpCenter; }

		void SetAxis(quaternion *pAxis);
		void SetAxis(float fAxisX, float fAxisY, float fAxisZ);
		
		quaternion *GetAxis(void) { return m_lpAxis; }


		void SetFlash(unsigned long dwTick, color lColor)
		{
			SetKey(dwTick, m_lstFlash, lColor);
		}
		BOOL GetFlash(unsigned long dwTick, color &lColor)
		{
			return m_lstFlash.Find(dwTick, lColor);
		}
		void SetDevice(LPDIRECT3DDEVICE8 lpD3DDevice) { m_lpD3DDevice = lpD3DDevice; }
		LPDIRECT3DDEVICE8 GetDevice(void) { return m_lpD3DDevice; }
		void SetViewMatrix(matrix *pViewMatrix) { m_lpViewMatrix = pViewMatrix; }
		matrix *GetViewMatrix(void) { return m_lpViewMatrix; }


		void SetFrameTick(unsigned long dwFrameTick) { m_dwFrameTick = dwFrameTick; }
		unsigned long GetFrameTick(void) { return m_dwFrameTick; }
		void SetTick(unsigned long dwTick) { m_dwOldTick = dwTick; }
		unsigned long GetTick(void) { return m_dwOldTick; }
		void SetMaxFrame(unsigned long dwMaxFrame) { m_dwMaxFrame = dwMaxFrame - 1; }
		void SetMinFrame(unsigned long dwMinFrame) {m_dwMinFrame = dwMinFrame;}
		void SetStopParticle(int t);
		
		unsigned long GetMaxFrame(void) { return m_dwMaxFrame; }
		void SetFrame(float fFrame) { m_fBeforeFrame = m_fFrame; m_fFrame = fFrame; }
		float GetBeforeFrame(void) { return m_fBeforeFrame;}
		float GetFrame(void) { return m_fFrame; }
		void SetIncFrame(float fIncFrame) { m_fIncFrame = fIncFrame; }
		float GetIncFrame(void) { return m_fIncFrame; }
		void SetLoop(BOOL bLoop);
		BOOL GetLoop(void) { return m_bLoop; }
		//effect 시작
		void SetEffectStart(bool t) {m_StartEffect = t;}
		bool GetEffectStart() { return m_StartEffect;}

		unsigned long AddEffect(CX3DEffectBase *pNewEffect);
		CX3DEffectBase *GetEffect(unsigned long dwEffectNumber);
		BOOL DeleteEffect(unsigned long dwEffectNumber);
		void SetEffectSetting(void);

		void SetFreezeMove(void) { m_dwKindMove = 0; }
		void SetDirectMove(vector3 &vecDest, vector3 &vecPower, vector3 &vecEForce);
		void SetMoveState(bool t){ m_MoveState = t;}
		bool GetMoveState() {return m_MoveState;}
		void SetScale(float x,float y,float z); //{m_GemScale[0] = x;m_GemScale[1] = y;m_GemScale[2] = z;}

		void Render(void);
		void SetWorldEffect(bool t);
		void SetLight(bool b) { m_bLight = b;}

		BOOL Interpolation(unsigned long dwTick, float fFrame = -1.0f);

		void Load(const char *strFilePath, const char *strFileName,bool bLoad = true);
		void Save(const char *strFilePath, const char *strFileName);
};

#endif // !defined(AFX_X3DEFFECT_H__36D22C0B_3861_4B57_B539_4E4BFAD198C1__INCLUDED_)
