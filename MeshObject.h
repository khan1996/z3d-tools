// MeshObject.h: interface for the CMeshObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHOBJECT_H__6734D99C_DC1E_4886_B686_DB6FF091A3E8__INCLUDED_)
#define AFX_MESHOBJECT_H__6734D99C_DC1E_4886_B686_DB6FF091A3E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Vertex.h>
#include <vector>
#include "SceneException.h"
#include <texture.h>
#include <3DMath.h>
#include <src/base/LoadedObj.h>


class CMeshObject : public Caldron::Base::CLoadedObj
{		
	static LPDIRECT3DDEVICE8 m_pd3dDevice;
public:	
	void MeshReload();
	char m_strMeshName[256];
	void ConvertTree();
	void ConvertDetail();
	void TextureLoad();
	void Optimize();
	void ConvertMulti();
	void ConvertNormal();
	void Save(char *strFilename);
	void SetShader(DWORD Shader){m_dwShader=Shader;};
	DWORD m_dwShader;
	DWORD m_dwCustomizeShader;	

	bool m_isAlreadyLock;
	vector3 m_MaxBox,m_MinBox;		
	bool m_bCompleteMesh;
	void CopyBuffer();
	void AllUnlockBuffer();
	void AllLockBuffer();

	int m_nTotalVertex;
	int m_nTotalIndices;

	std::vector<LPDIRECT3DVERTEXBUFFER8> m_pVertexBuffer;
	std::vector<LPDIRECT3DINDEXBUFFER8> m_pIndicesBuffer;
	std::vector<CTexture*> m_pMat;
	std::vector<CTexture*> m_pDetailMap;

	std::vector<CTexture*> m_pBumpTexture;
	std::vector<DWORD> m_pMatRef;
	

	std::vector<WORD *> m_pIndices;
	std::vector<void *> m_pVertex;

	std::vector<long> m_nVertex;
	std::vector<long> m_nIndices;

	std::vector<WORD *> m_pLoadedIndices;
	std::vector<LightVertex *> m_pLoadedVertex;


	bool m_bRender;

	long GetObjectCount(){return (long)m_nVertex.size();};
	void RenderBox(LPDIRECT3DDEVICE8 pd3dDevice);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void CalcBox();
	void Load(char *strFilename);
	CMeshObject();
	virtual ~CMeshObject();
	
	virtual bool Load(Caldron::Base::CCaldronByteDataObj *);
	virtual bool PostLoad();
	//virtual bool Unload();
};

#endif // !defined(AFX_MESHOBJECT_H__6734D99C_DC1E_4886_B686_DB6FF091A3E8__INCLUDED_)
