// X3DEffectCylinder.h: interface for the CX3DEffectCylinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTCYLINDER_H__21EB02CB_6CA5_4DA9_9AD2_EBED7AFE8320__INCLUDED_)
#define AFX_X3DEFFECTCYLINDER_H__21EB02CB_6CA5_4DA9_9AD2_EBED7AFE8320__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"

class CX3DEffectCylinder : public CX3DEffectBase  
{
	protected:
		unsigned long m_dwSidePlane, m_dwPrimitive;
		float m_fUpperHeight;
		float m_fLowerHeight;
		float m_fUpperRadius;
		float m_fLowerRadius;

		float m_fTileU;
		float m_fTileV;
		float m_fStartU;
		float m_fStartV;
		float m_fTexFrame;

		LPDIRECT3DVERTEXBUFFER8 m_lpVertices;
		LPDIRECT3DVERTEXBUFFER8 m_lpVerticesBlend;

	public:
		CKeyList<FloatKeyList> m_lstUpperHeight;
		CKeyList<FloatKeyList> m_lstLowerHeight;
		CKeyList<FloatKeyList> m_lstUpperRadius;
		CKeyList<FloatKeyList> m_lstLowerRadius;

		CKeyList<FloatKeyList> m_lstTileU;
		CKeyList<FloatKeyList> m_lstTileV;
		CKeyList<FloatKeyList> m_lstStartU;
		CKeyList<FloatKeyList> m_lstStartV;
		CKeyList<FloatKeyList> m_lstTexFrame;

	public:
		CX3DEffectCylinder();
		virtual ~CX3DEffectCylinder();

		void SetSidePlane(unsigned long dwSidePlane) { m_dwSidePlane = dwSidePlane; }
		unsigned long GetSidePlane(void) { return m_dwSidePlane; }

		void SetUpperHeight(float fUpperHeight) { m_fUpperHeight = fUpperHeight; }
		float GetUpperHeight(void) { return m_fUpperHeight; }
		void SetLowerHeight(float fLowerHeight) { m_fLowerHeight = fLowerHeight; }
		float GetLowerHeight(void) { return m_fLowerHeight; }

		void SetUpperRadius(float fUpperRadius) { m_fUpperRadius = fUpperRadius; }
		float GetUpperRadius(void) { return m_fUpperRadius; }
		void SetLowerRadius(float fLowerRadius) { m_fLowerRadius = fLowerRadius; }
		float GetLowerRadius(void) { return m_fLowerRadius; }

		void SetTileU(float fTileU) { m_fTileU = fTileU; }
		float GetTileU(void) { return m_fTileU; }
		void SetTileV(float fTileV) { m_fTileV = fTileV; }
		float GetTileV(void) { return m_fTileV; }

		void SetStartU(float fStartU) { m_fStartU = fStartU; }
		float GetStartU(void) { return m_fStartU; }
		void SetStartV(float fStartV) { m_fStartV = fStartV; }
		float GetStartV(void) { return m_fStartV; }

		void SetTexFrame(float fTexFrame) { m_fTexFrame = fTexFrame; }
		float GetTexFrame(void) { return m_fTexFrame; }

		void Create(unsigned long dwStartFrame, unsigned long dwEndFrame);
		BOOL CreateBuffer(void);
		void Render(void);
		BOOL Interpolation(float fFrame);
		void Load(FILE *fp, const char *strOriginalPath = NULL);
		void Save(FILE *fp, const char *strOriginalPath = NULL);
};

#endif // !defined(AFX_X3DEFFECTCYLINDER_H__21EB02CB_6CA5_4DA9_9AD2_EBED7AFE8320__INCLUDED_)
