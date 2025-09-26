// SectorLight.h: interface for the CSectorLight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORLIGHT_H__E238E34F_4657_45DB_9BCD_FC99B926BE46__INCLUDED_)
#define AFX_SECTORLIGHT_H__E238E34F_4657_45DB_9BCD_FC99B926BE46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx8.h>
#include <d3d8.h>
#include <vector>
#include "CEffScript.h"


using namespace std;

class CSectorLight				// LightMap Generation 용 sector light class
{
public:
	CSectorLight();
	~CSectorLight();
	
	D3DXVECTOR3			m_vecPos;
	D3DXVECTOR3			m_vecColor;
	float				m_fRange;
	int					m_iShadowSamples;
	int					m_iLightSamples;
	float				m_fShadowFactors;
	int					m_iAmbient;
	char				m_strEffect[256];
	float				m_fExpose;



};

class CSectorLightMap 
{
public:
	int					 m_IndexX,m_IndexY;
	vector<CSectorLight> m_lstLights;

	CSectorLightMap();
	~CSectorLightMap();
	void Delete(float x,float y,float z);
	void Move(float x,float y,float z,float nx,float ny,float nz);

};
class CSectorLightManager
{
public:
	CSectorLightManager();
	~CSectorLightManager();
	void InputLight(D3DXVECTOR3 vecPos,D3DXVECTOR3 vecColor,float fRad,char *strEffect,int iLight,int iShadow,float fFactor,int iAmb,float fExpose);
	void DeletePickLight();
	void DeleteAllLight();

	void RenderLight();
	void RenderTerrainLight();

	int PickLight(D3DXVECTOR3 vecPos);
	void SelectLight(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax,D3DXMATRIX matTm);		// Bound Box 영역과 영향을 미치는 light 모으기
	void SelectObjLight(float fx,float fy,float fz);

	vector<CSectorLight *>	m_lstLights;
	int						m_iLights;
	CSectorLight			*m_pPickLight;

	CSectorLight			*m_pSelectLights;
	int						m_iSelectLights;
	int						m_IndexX,m_IndexY;

	vector<CSectorLight >	m_lstSelectObjLights;
	int						m_iSelectObjLights;
	CEffScript				*m_pLightEsf;

	
};

#endif // !defined(AFX_SECTORLIGHT_H__E238E34F_4657_45DB_9BCD_FC99B926BE46__INCLUDED_)
