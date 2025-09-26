// WBLightMapBuild.h: interface for the WBLightMapBuild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBLIGHTMAPBUILD_H__4F1BB2A5_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
#define AFX_WBLIGHTMAPBUILD_H__4F1BB2A5_C09E_11D7_8C7F_0040F41B957B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WBLightMap.h"

class WBLightMapBuild : public WBLightMap  
{
public:
	D3DXVECTOR3 reflectance;
	float pixelarea;
	D3DXVECTOR3 totalenergy;
	float *rad;
	float *emm;
	
	void set_base(int flag);
	void alloc_data(int flag);
	void set_data(int flag,int x,int y,float *f);
	void Blurring();
	void save_to_lightmap(float exposure);

	void illum(D3DXVECTOR3& pos,D3DXVECTOR3& color,float rad,int shadows);
	int emmit_light();
	D3DXVECTOR3 receive_light(D3DXVECTOR3& P,D3DXVECTOR3& N,D3DXVECTOR3& E,int flag=0);
	D3DXVECTOR3 tri_receive_light(WBLightMapFace *f,D3DXVECTOR3& P,D3DXVECTOR3& N,D3DXVECTOR3& E,int flag=0);
	
	void compute_bgshadow();

	
	WBLightMapBuild(int f,int p,int x,int y,int sx,int sy,int bp=3) :
	WBLightMap(f,p,x,y,sx,sy,bp)
	{

		reflectance = D3DXVECTOR3(1.0f,1.0f,1.0f);
		pixelarea=0;
		rad=0;
		emm=0;
		totalenergy = D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	~WBLightMapBuild();

};

#endif // !defined(AFX_WBLIGHTMAPBUILD_H__4F1BB2A5_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
