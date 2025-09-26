// ShadowVolume.cpp: implementation of the CShadowVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "ShadowVolume.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowVolume::CShadowVolume()
{

}

CShadowVolume::~CShadowVolume()
{

}


void CShadowVolume::MakeShadowVolumeDirectionLight(vector3 *pVertex, int nVertex, WORD *pIndices, int nIndices,vector3 vecLightDir)
{	
	vector3 vecPoly[3];
	vector3 vecNormal;

	WORD *pEdges=new WORD[nIndices*6];
	int nEdges=0;

	vecLightDir.Normalize();

	for(int i=0;i<nIndices;i++)
	{
		vecPoly[0]=pVertex[pIndices[3*i+0]];
		vecPoly[1]=pVertex[pIndices[3*i+1]];
		vecPoly[2]=pVertex[pIndices[3*i+2]];		

		vecNormal=(vecPoly[2]-vecPoly[1])^(vecPoly[1]-vecPoly[0]);
		vecNormal.Normalize();

		if(vecNormal*vecLightDir >=0.0f)
		{
			AddEdge(pEdges,nEdges,pIndices[3*i+0],pIndices[3*i+1]);
			AddEdge(pEdges,nEdges,pIndices[3*i+1],pIndices[3*i+2]);
			AddEdge(pEdges,nEdges,pIndices[3*i+2],pIndices[3*i+0]);
		}
	}

	vector3 vecVol[4];
	for(int i=0;i<nEdges;i++)
	{
		vecVol[0]=pVertex[pEdges[2*i+0]];
		vecVol[1]=pVertex[pEdges[2*i+1]];

		vecVol[2]=vecVol[0]+vecLightDir*50000.0f;
		vecVol[3]=vecVol[1]+vecLightDir*50000.0f;

		m_ShadowVolumeVertexList.push_back(vecVol[0]);
		m_ShadowVolumeVertexList.push_back(vecVol[1]);
		m_ShadowVolumeVertexList.push_back(vecVol[2]);

		m_ShadowVolumeVertexList.push_back(vecVol[1]);
		m_ShadowVolumeVertexList.push_back(vecVol[3]);
		m_ShadowVolumeVertexList.push_back(vecVol[2]);

	}
	delete [] pEdges;    
}


void CShadowVolume::AddEdge(WORD *pEdges, int &nEdges, WORD v0, WORD v1)
{
	for(int i=0;i<nEdges;i++)
	{
		if( (pEdges[2*i+0]==v0 && pEdges[2*i+1]==v1) ||
			(pEdges[2*i+0]==v1 && pEdges[2*i+1]==v0) )
		{
			if(nEdges>1)
			{
				pEdges[2*i+0]=pEdges[2*(nEdges-1)+0];
				pEdges[2*i+1]=pEdges[2*(nEdges-1)+1];
			}
			nEdges--;
			return;
		}
			 
	}
	pEdges[2*nEdges+0]=v0;
	pEdges[2*nEdges+1]=v1;
    nEdges++;    
}

void CShadowVolume::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetVertexShader(D3DFVF_XYZ);
	if(m_ShadowVolumeVertexList.size()>0)
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,(int)m_ShadowVolumeVertexList.size()/3,&m_ShadowVolumeVertexList[0],sizeof(vector3));
}

void CShadowVolume::Reset()
{
	m_ShadowVolumeVertexList.clear();
}
