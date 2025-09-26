// BaseGraphic.h: interface for the BaseGraphic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEGRAPHIC_H__D2B16AD2_C324_44D0_A581_93528EA232C2__INCLUDED_)
#define AFX_BASEGRAPHIC_H__D2B16AD2_C324_44D0_A581_93528EA232C2__INCLUDED_

#include "VECTOR.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>

#include "ViewCamera.h"
#include "list.h"
//#include <Utility/Exception.h>
#include "Texture.h"
#include "EnumD3D.h"
#include "D3DFont.h"
#include "StateLog.h"

class BaseGraphicsLayer  
{		
	static HWND m_hWnd;		
    static LPDIRECT3DDEVICE8 m_pd3dDevice;    
	static LPDIRECT3D8 m_pD3D;
    bool m_bWindowed;		
	bool m_bEditorMode;
	//Projection Parameter//	
	float m_fFov,m_fNear,m_fFar;
	//NS//
	CTexture m_NsTexture;	
	DWORD m_dwStartRemainTextureMemory;
	DWORD m_dwAbleTextureMemory;
	
public :
	static int m_iGraphicCards;

public:	
	static bool ProjectiveTextureMapping(vector3 vecPoint,float &fTu,float &fTv);
	void RectFlip(std::vector<RECT> &RectList);
	static long m_lScreenSx,m_lScreenSy;	
	static CD3DFont* m_pFont;              // Font for drawing text
	static D3DPRESENT_PARAMETERS m_d3dpp;
	RECT m_rcWindowBounds;
	RECT m_rcWindowClient;
	static bool m_VoodooOption;

	static HWND GethWnd(){return m_hWnd;};


	void ManyTransformVector(vector3 *t,vector3 *result,float *w);
	static bool TransformVector(vector3 &t,vector3 &vecResult,float &w);
	static bool TransformVector(const vector3 &vSource, const matrix& mTransform, vector3 &vResult,float &w);
	void ShowState();
	void PrefareRender();
	void Flip();
	void GetPickPoint(long lScreenX,long lScreenY,float &fsX,float &fsY,float &fsZ);

	static void DrawText(float x, float y, DWORD dwColor, char*szText);
	static CViewCamera		m_ViewCamera;
	static color			m_ClearColor;
	

	void Create(HWND hWnd,bool bWindowed,bool Editor,long screenx,long screeny, long screenwidth, long screenheight);
	BaseGraphicsLayer();
	virtual ~BaseGraphicsLayer();
	static LPDIRECT3DDEVICE8 GetDevice(){return m_pd3dDevice;};	
	static int GetGraphicCard() { return m_iGraphicCards;}

	static void SetCustomMsg(size_t index, const char* sz);

	
	static char m_aszCustomMsg[10][300];
};

#endif // !defined(AFX_BASEGRAPHIC_H__D2B16AD2_C324_44D0_A581_93528EA232C2__INCLUDED_)
