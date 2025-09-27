// X3DEffectBillboard.h: interface for the CX3DEffectBillboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTBILLBOARD_H__C83DF58D_552F_430C_8BBD_7274114C7B44__INCLUDED_)
#define AFX_X3DEFFECTBILLBOARD_H__C83DF58D_552F_430C_8BBD_7274114C7B44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"

class CX3DEffectBillboard : public CX3DEffectBase
{
	protected:
		float m_fWidth;
		float m_fHeight;

		float m_fTileU;
		float m_fTileV;
		float m_fStartU;
		float m_fStartV;
		float m_fTexFrame;

		BOOL m_bAxisAligned;

		LPDIRECT3DVERTEXBUFFER8 m_lpVertices;
		LPDIRECT3DVERTEXBUFFER8 m_lpVerticesBlend;

	public:
		CKeyList<FloatKeyList> m_lstWidth;
		CKeyList<FloatKeyList> m_lstHeight;

		CKeyList<FloatKeyList> m_lstTileU;
		CKeyList<FloatKeyList> m_lstTileV;
		CKeyList<FloatKeyList> m_lstStartU;
		CKeyList<FloatKeyList> m_lstStartV;
		CKeyList<FloatKeyList> m_lstTexFrame;

		// 내부 Interpolation 용 변수
		vector3 m_vecEyeT, m_vecAxisT, m_vecWidthT;

	public:
		CX3DEffectBillboard();
		virtual ~CX3DEffectBillboard();

		void SetWidth(float fWidth) { m_fWidth = fWidth; }
		float GetWidth(void) { return m_fWidth; }
		void SetHeight(float fHeight) { m_fHeight = fHeight; }
		float GetHeight(void) { return m_fHeight; }

		void SetStartU(float fStartU) { m_fStartU = fStartU; }
		float GetStartU(void) { return m_fStartU; }
		void SetStartV(float fStartV) { m_fStartV = fStartV; }
		float GetStartV(void) { return m_fStartV; }

		void SetTileU(float fTileU) { m_fTileU = fTileU; }
		float GetTileU(void) { return m_fTileU; }
		void SetTileV(float fTileV) { m_fTileV = fTileV; }
		float GetTileV(void) { return m_fTileV; }

		void SetAxisAligned(BOOL bAxisAligned) { m_bAxisAligned = bAxisAligned; }
		BOOL GetAxisAligned(void) { return m_bAxisAligned; }

		void SetTexFrame(float fTexFrame) { m_fTexFrame = fTexFrame; }
		float GetTexFrame(void) { return m_fTexFrame; }

		void Create(unsigned long dwStartFrame, unsigned long dwEndFrame);
		BOOL CreateBuffer(void);
		void Render(void);
		BOOL Interpolation(float fFrame);
		void Load(FILE *fp, const char *strOriginalPath = NULL);
		void Save(FILE *fp, const char *strOriginalPath = NULL);
};

#endif // !defined(AFX_X3DEFFECTBILLBOARD_H__C83DF58D_552F_430C_8BBD_7274114C7B44__INCLUDED_)
