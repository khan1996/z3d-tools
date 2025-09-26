// WBLightMapGenerator.h: interface for the WBLightMapGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBLIGHTMAPGENERATOR_H__4F1BB2A3_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
#define AFX_WBLIGHTMAPGENERATOR_H__4F1BB2A3_C09E_11D7_8C7F_0040F41B957B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WBValue.h"
#include "WBLightmap.h"

#include <vector>
using namespace std;

#define RPI2 1.57079632679f

#define RLIGHT_FRAND (((rand()%10000)-5000)/5000.0f)


class WBLight {
public:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecColor;
	float		m_fRange;
	WBLight()
	{
		m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_vecColor = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_fRange = 0.0f;

	}
	~WBLight(){}
};

struct reta
{
	int sizex, sizey;
	int offsetx, offsety, offsetz;
};


class retb
{
public:
	retb();
	virtual ~retb();
	void add(struct reta *r);
	int calc(int max_sx, int max_sy);

private:
	bool arrumar(struct reta *r);
	int procura(int sx, int sy);

	struct reta **l;
	int nl;
};


class WBLightMapGenerator {
public:
	static void CleanStatic();

	static int collision_test(D3DXVECTOR3 ,D3DXVECTOR3 ,int iFace = -1,int k = 0);
	static float compute_bgshadow(D3DXVECTOR3& ip,D3DXVECTOR3& n);
	void InputLight(WBLight *);
	void InputFace(WBLightMapFace *);

	bool LightMapGenerate(int ivalue);
	WBLightMap *BuildLightMap(WBLightMapFace *pFace,WBLightVertex **ppVerts,int iVerts);

	WBLightMap *BuildLightMap(WBLightMapFace *pFace);
	void ComputeLighting();

	WBLightMapGenerator();
	~WBLightMapGenerator();
	
	static int m_iFaces;
	static vector<WBLightMapFace *> m_lstFaces;
	
	int m_iLightMaps;
	vector<WBLightMap *> m_lstLightMaps;

	int m_iLightMapTexs;
	vector<WBLightMapTex *> m_lstLightMapTexs;

	int m_iLights;
	vector<WBLight *>m_lstLights;

	static int m_iLightMapPixel;
	static int m_iLightMapSize;
	static int m_iLightMapAmb;
	static int m_iLightSamples;
	static int m_iLightJigg;
	static int m_iShadowSamples;
	static int m_iShadowFull;
	static float m_fShadowDist;
	static float m_fShadowFactor;


	static float *emm;
	static float *rad;
	static float m_fExposure;


};

#endif // !defined(AFX_WBLIGHTMAPGENERATOR_H__4F1BB2A3_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
