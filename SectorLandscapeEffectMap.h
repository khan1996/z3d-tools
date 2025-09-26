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
-m_vecCenterPos : �Ȱ��� �߽�
<float param>
-Param 0 ~ 1 : Fade out / in value (�Ѱ���) 
-Param 2 : fade in speed
-Param 3 : Center ���ߵ�
-Param 4 ~ 6 : ���׻����� ����ġ(�� �������� �󸶳� ������)
-Param 7 ~ 9 : �߷�
-Param 10 : Particle 1���� ũ��
-Param 11 : �Ȱ� ������豸�� ������
-param 12 : y�� ���� �Ѱ���
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
-m_vecCenterPos : �޽� �߽�
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
