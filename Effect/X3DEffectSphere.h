// X3DEffectSphere.h: interface for the CX3DEffectSphere class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTSPHERE_H__DEFBF05C_A2DA_46B4_968C_1706C124EA75__INCLUDED_)
#define AFX_X3DEFFECTSPHERE_H__DEFBF05C_A2DA_46B4_968C_1706C124EA75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"

class CX3DEffectSphere : public CX3DEffectBase  
{
	protected:
		unsigned long m_dwSegment, m_dwTotalSegment;
		unsigned long m_dwSidePlane;
		unsigned long m_dwPrimitive, m_dwNumVertex;
		float m_fHeightFactor;
		float m_fRadius;

		BOOL m_bUpperVis;
		BOOL m_bUpperUp;
		BOOL m_bUpperTex;
		float m_fUpperFactor;
		BOOL m_bLowerVis;
		BOOL m_bLowerUp;
		BOOL m_bLowerTex;
		float m_fLowerFactor;

		float m_fTileU;
		float m_fTileV;
		float m_fStartU;
		float m_fStartV;
		float m_fTexFrame;

		LPDIRECT3DVERTEXBUFFER8 m_lpVertices;
		LPDIRECT3DVERTEXBUFFER8 m_lpVerticesBlend;
		LPDIRECT3DINDEXBUFFER8 m_lpVerticeIndex;

	public:
		CKeyList<FloatKeyList> m_lstHeightFactor;
		CKeyList<FloatKeyList> m_lstRadius;

		CKeyList<FloatKeyList> m_lstUpperFactor;
		CKeyList<FloatKeyList> m_lstLowerFactor;

		CKeyList<FloatKeyList> m_lstTileU;
		CKeyList<FloatKeyList> m_lstTileV;
		CKeyList<FloatKeyList> m_lstStartU;
		CKeyList<FloatKeyList> m_lstStartV;
		CKeyList<FloatKeyList> m_lstTexFrame;

	public:
		CX3DEffectSphere();
		virtual ~CX3DEffectSphere();

		void SetSegment(unsigned long dwSegment) { m_dwSegment = dwSegment; }
		unsigned long GetSegment(void) { return m_dwSegment; }
		void SetSidePlane(unsigned long dwSidePlane) { m_dwSidePlane = dwSidePlane * 4; }
		unsigned long GetSidePlane(void) { return m_dwSidePlane; }

		void SetUpperVis(BOOL bUpperVis) { m_bUpperVis = bUpperVis; }
		BOOL GetUpperVis(void) { return m_bUpperVis; }
		void SetUpperUp(BOOL bUpperUp) { m_bUpperUp = bUpperUp; }
		BOOL GetUpperUp(void) { return m_bUpperUp; }
		void SetUpperTex(BOOL bUpperTex) { m_bUpperTex = bUpperTex; }
		BOOL GetUpperTex(void) { return m_bUpperTex; }
		void SetLowerVis(BOOL bLowerVis) { m_bLowerVis = bLowerVis; }
		BOOL GetLowerVis(void) { return m_bLowerVis; }
		void SetLowerUp(BOOL bLowerUp) { m_bLowerUp = bLowerUp; }
		BOOL GetLowerUp(void) { return m_bLowerUp; }
		void SetLowerTex(BOOL bLowerTex) { m_bLowerTex = bLowerTex; }
		BOOL GetLowerTex(void) { return m_bLowerTex; }

		void SetRadius(float fRadius) { m_fRadius = fRadius; }
		float GetRadius(void) { return m_fRadius; }
		void SetHeightFactor(float fHeightFactor) { m_fHeightFactor = fHeightFactor; }
		float GetHeightFactor(void) { return m_fHeightFactor; }
		void SetUpperFactor(float fUpperFactor) { m_fUpperFactor = fUpperFactor; }
		float GetUpperFactor(void) { return m_fUpperFactor; }
		void SetLowerFactor(float fLowerFactor) { m_fLowerFactor = fLowerFactor; }
		float GetLowerFactor(void) { return m_fLowerFactor; }

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
		BOOL CreateBuffer();
		void Render(void);
		BOOL Interpolation(float fFrame);
		void Load(FILE *fp, const char *strOriginalPath = NULL);
		void Save(FILE *fp, const char *strOriginalPath = NULL);
};

#endif // !defined(AFX_X3DEFFECTSPHERE_H__DEFBF05C_A2DA_46B4_968C_1706C124EA75__INCLUDED_)
