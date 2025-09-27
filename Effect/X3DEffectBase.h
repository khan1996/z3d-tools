// X3DEffectBase.h: interface for the CX3DEffectBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTBASE_H__C6A7B2C5_B699_4078_9690_00AF0121FFA9__INCLUDED_)
#define AFX_X3DEFFECTBASE_H__C6A7B2C5_B699_4078_9690_00AF0121FFA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "key.h"
#include "Texture.h"
#include "BaseDataDefine.h"

#define EFFECT_PARTICLE		0
#define EFFECT_BILLBOARD	1
#define EFFECT_CYLINDER		2
#define EFFECT_PLANE		3
#define EFFECT_SPHERE		4
#define EFFECT_MESH			5

class CX3DEffectBase  
{
	protected:
		unsigned long m_dwEffectKind;			// 이펙트 종류
												// 0 : 파티클			1 : 빌보드
												// 2 : 실린더			3 : 플레인
												// 4 : 스피어			5 : 메쉬

		LPDIRECT3DDEVICE8 m_lpD3DDevice;
		CTexture *m_lpTexture;
		char m_strTextureFile[256];
		unsigned long m_dwStartFrame, m_dwEndFrame;

		void *m_lpLocalEffect;
		matrix *m_matLocalView;
		vector3 *m_vecLocalCenter;
		quaternion *m_quatLocalAxis;

		quaternion m_quatAxis;
		quaternion m_TmpQuat;

		bool m_QuatSet;
		vector3 m_vecCenter;
		color m_lColor;

		BOOL m_bTexAni;
		bool m_Mine;

		int m_index;
		float m_Scale[3];
		//world map effect 인지 세팅
		bool m_bWorldEffect;
		int m_StopParticleFrame;

		unsigned long m_dwSrcBlending;
		unsigned long m_dwDestBlending;
		bool m_bLight;
		bool m_bVisibility;

	public:
		CKeyList<VectorKeyList> m_lstCenter;
		CKeyList<QuaternionKeyList> m_lstAxis;
		CKeyList<ColorKeyList> m_lstColor;

	public:
		bool m_bClearBuffer;
		void SetGemClearBuffer(bool b);
		CX3DEffectBase();
		virtual ~CX3DEffectBase();

		void LoadTexture(const char *strFilename);
		LPDIRECT3DBASETEXTURE8 GetTexture(void) { return m_lpTexture->GetTexture(); }
		char *GetTextureFileName(void) { return m_strTextureFile; }

		void SetLocalEffect(void *lpLocalEffect) { m_lpLocalEffect = lpLocalEffect; }
		void *GetLocalEffect(void) { return m_lpLocalEffect; }

		void SetLocalCenter(vector3 *vecLocalCenter) { m_vecLocalCenter = vecLocalCenter; }
		vector3 *GetLocalCenter(void) { return m_vecLocalCenter; }
		void SetLocalAxis(quaternion *quatLocalAxis) { m_quatLocalAxis = quatLocalAxis; }
		quaternion *GetLocalAxis(void) { return m_quatLocalAxis; }

		void SetLocalView(matrix *matLocalView) { m_matLocalView = matLocalView; }
		matrix *GetLocalView(void) { return m_matLocalView; }
		void SetLocalDevice(LPDIRECT3DDEVICE8 lpD3DDevice) { m_lpD3DDevice = lpD3DDevice; }
		LPDIRECT3DDEVICE8 GetLocalDevice(void) { return m_lpD3DDevice; }
		// world map effect 인지 세팅하는 함수
		void SetWorldEffect(bool b) {m_bWorldEffect = b;}
		bool GetWorldEffect() { return m_bWorldEffect;}

		void SetTexAni(BOOL bTexAni) { m_bTexAni = bTexAni; }
		BOOL GetTexAni(void) { return m_bTexAni; }

		void SetColor(color lColor) { m_lColor = lColor; }
		color GetColor(void) { return m_lColor; }
		void SetMine(bool t) { m_Mine = t;}

		void SetAlpha(COLORVALUE cAlpha) { m_lColor.a = cAlpha; }
		COLORVALUE GetAlpha(void) { return m_lColor.a; }
		void SetRColor(COLORVALUE cRColor) { m_lColor.r = cRColor; }
		COLORVALUE GetRColor(void) { return m_lColor.r; }
		void SetGColor(COLORVALUE cGColor) { m_lColor.g = cGColor; }
		COLORVALUE GetGColor(void) { return m_lColor.g; }
		void SetBColor(COLORVALUE cBColor) { m_lColor.b = cBColor; }
		COLORVALUE GetBColor(void) { return m_lColor.b; }

		void SetSrcBlending(unsigned long dwSrcBlending) { m_dwSrcBlending = dwSrcBlending; }
		unsigned long GetSrcBlending(void) { return m_dwSrcBlending; }
		void SetDestBlending(unsigned long dwDestBlending) { m_dwDestBlending = dwDestBlending; }
		unsigned long GetDestBlending(void) { return m_dwDestBlending; }

		void SetStartFrame(unsigned long dwStartFrame) { m_dwStartFrame = dwStartFrame; }
		unsigned long GetStartFrame(void) { return m_dwStartFrame; }
		void SetEndFrame(unsigned long dwEndFrame) { m_dwEndFrame = dwEndFrame; }
		unsigned long GetEndFrame(void) { return m_dwEndFrame; }

		void SetEffectKind(unsigned long dwEffectKind) { m_dwEffectKind = dwEffectKind; }
		unsigned long GetEffectKind(void) { return m_dwEffectKind; }
		//scale set
		void SetEffectScale(float x,float y,float z) { m_Scale[0] = x;m_Scale[1] = y;m_Scale[2] = z;}

		void SetAxis(float fX, float fY, float fZ, float fW) { m_quatAxis.x = fX; m_quatAxis.y = fY; m_quatAxis.z = fZ; m_quatAxis.w = fW; }
		void SetAxis(quaternion &quatAxis) {
											 m_QuatSet = true;
											 m_quatAxis = quatAxis; 
											 m_TmpQuat = m_quatAxis;
											}
		quaternion GetAxis(void) { return m_quatAxis; }
		void SetStopParticle(int t);
		int GetStopParticle();
		void SetCenter(float fX, float fY, float fZ) { m_vecCenter.x = fX; m_vecCenter.y = fY; m_vecCenter.z = fZ; }
		void SetCenter(vector3 &vecCenter) { m_vecCenter = vecCenter; }
		vector3 GetCenter(void) { return m_vecCenter; }
		void SetLight(bool b) { m_bLight = b;}
		void SetVisibility(bool b) { m_bVisibility = b;}

		virtual void Create(unsigned long dwStartFrame, unsigned long dwEndFrame) = 0;
		virtual BOOL CreateBuffer(void) = 0;
		virtual void Render(void) = 0;
		virtual BOOL Interpolation(float fFrame) = 0;
		virtual void Load(FILE *fp, const char *strOriginalPath = NULL) = 0;
		virtual void Save(FILE *fp, const char *strOriginalPath = NULL) = 0;
};

#endif // !defined(AFX_X3DEFFECTBASE_H__C6A7B2C5_B699_4078_9690_00AF0121FFA9__INCLUDED_)
