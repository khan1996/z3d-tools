// ShadowVolume.h: interface for the CShadowVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWVOLUME_H__6B48C03A_7A32_4848_8BB8_2EFAF79067F0__INCLUDED_)
#define AFX_SHADOWVOLUME_H__6B48C03A_7A32_4848_8BB8_2EFAF79067F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDataDefine.h"
#include "3DMath.h"
#include <vector>
#include "MeshObject.h"

class CShadowVolume  
{
	std::vector<vector3> m_ShadowVolumeVertexList;	    
public:	
	void Reset();
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void AddEdge(WORD* pEdges,int &nEdges,WORD v0,WORD v1);
	void MakeShadowVolumeDirectionLight(vector3 *pVertex,int nVertex,WORD *pIndices,int nIndices,vector3 vecLightDir);
	CShadowVolume();
	virtual ~CShadowVolume();

};

#endif // !defined(AFX_SHADOWVOLUME_H__6B48C03A_7A32_4848_8BB8_2EFAF79067F0__INCLUDED_)
