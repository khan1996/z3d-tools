// X3DEffectPlane.h: interface for the CX3DEffectPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTPLANE_H__0FB11223_90D1_47D3_9948_08C94D138879__INCLUDED_)
#define AFX_X3DEFFECTPLANE_H__0FB11223_90D1_47D3_9948_08C94D138879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"

class CX3DEffectPlane : public CX3DEffectBase  
{
	protected:
		float m_fWidth;
		float m_fHeight;

		float m_fTileU;
		float m_fTileV;
		float m_fStartU;
		float m_fStartV;
		float m_fTexFrame;

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

	public:
		CX3DEffectPlane();
		virtual ~CX3DEffectPlane();

		void SetWidth(float fWidth) { m_fWidth = fWidth; }
		float GetWidth(void) { return m_fWidth; }
		void SetHeight(float fHeight) { m_fHeight = fHeight; }
		float GetHeight(void) { return m_fHeight; }

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

#endif // !defined(AFX_X3DEFFECTPLANE_H__0FB11223_90D1_47D3_9948_08C94D138879__INCLUDED_)
