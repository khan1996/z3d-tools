#pragma once
#include "resourceobj.h"
#include "vector.h"
#include "NTexture.h"
#include "vertex.h"
#include "../Caldron/Src/Scene/D3DBufferPools.h"

#include <vector>


namespace CROSSM {


class CNMesh :
	public CResourceObj
{
public:
	CNMesh(void);
	virtual ~CNMesh(void);
	virtual bool Load();
	virtual bool Unload();
	virtual bool PostLoad();
	void MeshReload(){}
	
	void ConvertTree();
	void ConvertMulti();
	void ConvertNormal();


	void AllUnlockBuffer();
	void AllLockBuffer();

	long GetObjectCount(){return (int)m_nVertex.size();};
	void RenderName(IDirect3DDevice8* pDevice);
	void RenderBox(LPDIRECT3DDEVICE8 pd3dDevice);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void CalcBox();
	void SetShader(DWORD Shader){m_dwShader=Shader;};
	void SetPostConvert(DWORD Shader) { m_dwConvertShader = Shader; };

	void Save(char *strName){}


	
	int m_iObjectMethod;	// object Vertex Type, 0 : MulTiVertex, 3 : LightVertex.

	DWORD m_dwShader;
	DWORD m_dwConvertShader;

	bool m_isAlreadyLock;
	vector3 m_MaxBox,m_MinBox;		
	bool m_bCompleteMesh;

	int m_nTotalVertex;
	int m_nTotalIndices;
	
	bool m_bRender;

	struct BoundingBoxMinMax
	{
		vector3 vBoxMin, vBoxMax;
	};

	std::vector<BoundingBoxMinMax> m_vecMeshPieceBoundingBoxInfo;
	std::vector<LPDIRECT3DVERTEXBUFFER8> m_pVertexBuffer;
	std::vector<LPDIRECT3DINDEXBUFFER8> m_pIndicesBuffer;
	std::vector<CNTexture*> m_pMat;
	std::vector<char *> m_pMatName;

	std::vector<DWORD> m_pMatRef;
	

	std::vector<WORD *> m_pIndices;
	std::vector<void *> m_pVertex;

	std::vector<long> m_nVertex;
	std::vector<long> m_nIndices;

	std::vector<WORD *> m_pLoadedIndices;
	std::vector<LightVertex *> m_pLoadedVertex;
	



};

}

