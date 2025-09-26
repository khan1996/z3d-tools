// SectorLandscapeEffectMap.h: interface for the CSectorLandscapeEffectMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORLANDSCAPEEFFECTMAP_H__ECA32043_D1AE_466D_99C7_E4B5EF14E993__INCLUDED_)
#define AFX_SECTORLANDSCAPEEFFECTMAP_H__ECA32043_D1AE_466D_99C7_E4B5EF14E993__INCLUDED_

#include "3dmath.h"
#include <vector>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/// #1 Vol-Fog ///
/*
<vector3>
-m_vecCenterPos : 안개의 중심
<float param>
-Param 0 ~ 1 : Fade out / in value (한계점) 
-Param 2 : fade in speed
-Param 3 : Center 집중도
-Param 4 ~ 6 : 포그생성시 가중치(그 방향으로 얼마나 퍼질지)
-Param 7 ~ 9 : 중력
-Param 10 : Particle 1개의 크기
-Param 11 : 안개 반응경계구의 반지름
-param 12 : y축 최저 한계점
-param 13 ~ 16 : Quad Red
-Param 17 ~ 20 : Quad GREEN
-Param 21 ~ 24 : Quad BLUE
 <int param>
-param 0 : particle number
-param 1 : fix flag
-param 2 ~ 3 : sector x/y index

<blend>
-param 0 ~ 1 : src /dst blend mode
<char *>
-texture name 
*/
/// #2 Boid
//int 0 : boid number
//int 1 : fix flag
//float 0: move range
//float 1: speed
/// #3 Mesh Morph ///
/*
<vector3>
-m_vecCenterPos : 메쉬 중심
int 0 : mloop
int 1 : m_switch
int 2 : fix flag
int 3 : randani flag
float 0 : m_switchRad;
float 1 : m_Vot;
float 2 : m_RandMax;
float 3 : m_RandMin;
float 4 : m_Xrot;
float 5 : m_Yrot;
float 6 : m_Zrot;


<char param>
-gem file name
*/

class CSectorLandscapeEffectMap  
{	
public:
	class LandscapeEffect 
	{
	public:
		char m_EffName[50];
		int m_nEffectMethod;
		vector3 m_vecCenterPos;
		DWORD m_dwBlend[2];
		float m_Param[50];
		int m_IntParam[20];
		char m_Name[50];
	};
	int m_IndexX,m_IndexY;	
	std::vector<LandscapeEffect> m_LandscapeEffect;
	
	CSectorLandscapeEffectMap();
	virtual ~CSectorLandscapeEffectMap();

};

#endif // !defined(AFX_SECTORLANDSCAPEEFFECTMAP_H__ECA32043_D1AE_466D_99C7_E4B5EF14E993__INCLUDED_)
