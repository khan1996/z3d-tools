// WBLightMap.h: interface for the WBLightMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBLIGHTMAP_H__4F1BB2A4_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
#define AFX_WBLIGHTMAP_H__4F1BB2A4_C09E_11D7_8C7F_0040F41B957B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WBValue.h"
#include "WBLightMapTex.h"

#define WB_FPBITS(fp)		(*(int *)&(fp))
//! returns the absolute value of a floating point number in bitwise form
#define WB_FPABSBITS(fp)	(WB_FPBITS(fp)&0x7FFFFFFF)
//! returns the signal bit of a floating point number
#define WB_FPSIGNBIT(fp)	(WB_FPBITS(fp)&0x80000000)
//! returns the absolute value of a floating point number
#define WB_FPABS(fp)		(*((int *)&fp)=WB_FPABSBITS(fp))
#define WBSMALL 1.0e-4f

class WBLightVertex {
public:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecColor;
	D3DXVECTOR2 m_vecTexcoord;

	WBLightVertex() {
		m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_vecColor = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_vecTexcoord = D3DXVECTOR2(0.0f,0.0f);
	}
};
class WBLightMapPlane {
public:
	float		m_fDist;
	D3DXVECTOR3 m_vecNormal;
	D3DXVECTOR3 m_vecVertex;

};

class WBLightMapFace {
public:
	bool m_bSetArea;
	float m_fArea;
	
	int m_iObject;

	int m_iVerts;
	int m_iStartIndex;
	
	int m_iIndex[3];

	int m_iLightMapId;

	int			m_iPlaneVertex;
	D3DXVECTOR3 *m_pPlaneVertex;
	D3DXVECTOR3 *m_pEdge;
	float		*m_pEdgeValue;

	float d0;
	float m_fDist;

	WBLightVertex *m_pVerts;
	D3DXVECTOR3 m_vecNormal;
	D3DXVECTOR3 m_vecMin;
	D3DXVECTOR3 m_vecMax;
	unsigned char m_ucColor[3];

	WBLightMapFace() {
		m_bSetArea = false;

		m_iVerts = 0;
		m_iStartIndex = 0;

		m_iLightMapId = -1;
		m_pVerts = NULL;
		m_pEdgeValue = NULL;
		m_pEdge = NULL;
		m_pPlaneVertex = NULL;
		m_iPlaneVertex = 0;



	}
	~WBLightMapFace() {
		if(m_pVerts != NULL)
			delete[] m_pVerts;
		if(m_pEdge != NULL)
			delete[] m_pEdge;
		if(m_pEdgeValue != NULL)
			delete[] m_pEdgeValue;
		if(m_pPlaneVertex != NULL)
			delete[] m_pPlaneVertex;

	}
	float area();
	void GetMinMax();
	int RayCollision(D3DXVECTOR3 vecStart,D3DXVECTOR3 vecDir,float fLength);
	int RayCollision(D3DXVECTOR3 vecStart,D3DXVECTOR3 vecEnd);
	int InsidePolygon(D3DXVECTOR3 vecPoint);
	void CalcNormal();

};

class WBLightMap  
{
public:
	WBLightMap(int f,int p,int x,int y,int sx,int sy,int bp=3);
	~WBLightMap();
	int m_iFaceId;
	D3DXVECTOR3		v0,
					v1,
					v2,
					normal;
	float det;		
	float uv[3][2];	
	
	int pic;			//!< flyLightMapPic index
	int offsetx,		//!< x offset into the light map picture
		offsety;		//!< y offset into the light map picture
	int sizex,			//!< x size of the light map (no need for being power of 2)
		sizey;			//!< y size of the light map (no need for being power of 2)
	unsigned char *bmp;	//!< light map bitmap
	int bytespixel,		//!< number of bytes per pixel (3: RGB, 4: RGBA)
		bytesx,			//!< number of bytes per line (sizex*bytespixel)
		bytesxy;		//!< total bytes of the bitmap (bytesx*sizey)
	D3DXVECTOR3	d0,		//!< base value used for illumination calculations
				d1,		//!< base value used for illumination calculations
				d2;		//!< base value used for illumination calculations

	
	void set_base(WBLightMapFace *f,WBLightMapTex *lmp,D3DXVECTOR3& pos=D3DXVECTOR3(0,0,0));
	
	void map_point(float u, float v, D3DXVECTOR3 &point); 
	
	void map_point_local(float u, float v, D3DXVECTOR3 &p,D3DXVECTOR3 &n);
	virtual void illum(D3DXVECTOR3& pos,D3DXVECTOR3& color,float radius,int shadows);
	
	void load(WBLightMapTex *lmp);
	//! Save the light map to flyLightMapPic
	void save(WBLightMapTex *lmp);



};

#endif // !defined(AFX_WBLIGHTMAP_H__4F1BB2A4_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
