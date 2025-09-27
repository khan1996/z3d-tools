// SMRObj.h: interface for the CSMRObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMROBJ_H__FD33169B_0305_4327_A366_EC1DB3AAD241__INCLUDED_)
#define AFX_SMROBJ_H__FD33169B_0305_4327_A366_EC1DB3AAD241__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "./SMRHeader.h"
#include "texture.h"
#include "BaseDataDefine.h"
class CSMRObj  
{
public:
	CSMRObj() :m_pMaterial(NULL),m_iMaterial(0),
		m_pMeshData(NULL),m_iMeshData(0)
	{
	
		m_pVertexBuffer = NULL;
		m_pIndexBuffer = NULL;
		m_lstTexture.clear();

		m_pRootNode = NULL;
		memset(m_iFrameInfo,0,sizeof(int) * 4);
		
		m_dwSrcBlend = D3DBLEND_SRCALPHA;
		m_dwDstBlend = D3DBLEND_INVSRCALPHA;



	}
	~CSMRObj()
	{

		if(m_pMaterial)
		{
			delete[] m_pMaterial;
			m_pMaterial = NULL;

		}
		if(m_pMeshData)
		{
			delete[] m_pMeshData;
			m_pMeshData = NULL;

		}
		if(m_pVertexBuffer)
		{
			for(int i = 0; i < m_iMeshData; i++ )
			{
				if(m_pVertexBuffer[i])
					m_pVertexBuffer[i]->Release();
			}
			delete[] m_pVertexBuffer;
		}
		if(m_pIndexBuffer)
		{
			for(int i = 0; i < m_iMeshData; i++ )
			{
				if(m_pIndexBuffer[i])
					m_pIndexBuffer[i]->Release();
			}
			delete[] m_pIndexBuffer;
			
		}

		std::vector<CTexture* >::iterator itr = m_lstTexture.begin() ;
		std::vector<CTexture* >::iterator end = m_lstTexture.end() ;
		while ( itr != end )
		{
			if ( NULL != (*itr) )
			{
				delete (*itr) ;
				(*itr) = NULL ;
			}

			++itr ;
		}
		m_lstTexture.clear();

		if(m_pRootNode)
		{
			delete m_pRootNode;
			m_pRootNode = NULL;
		}
	}
	void Render(LPDIRECT3DDEVICE8 lpDevice,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecScale,D3DXQUATERNION quatRot,float fFrame);
	void UpdateTm(SGEngine::Core::CMeshObjNode *pNode,const float &fFrame);
	void UpdateBuffer(const float &fFrame);
	void UpdateSkinning(SGEngine::Core::CMeshObjNode *pNode);
	void SetSkinningLink(SGEngine::Core::CMeshObjNode *pNode);
	SGEngine::Core::CMeshObjNode *FindNode(const char *strName,SGEngine::Core::CMeshObjNode *pNode);
	void Load(const char *strFileName);
	void AddMesh(SGEngine::Core::CMeshCoreData *pMesh,SGEngine::Core::CMeshObjNode *pNode);
	void GetAniNode(SGEngine::Core::CMeshObjNode *pNode,FILE *fp);

	void SetSrcBlend(DWORD dwValue) { m_dwSrcBlend = dwValue;}
	void SetDstBlend(DWORD dwValue) { m_dwDstBlend = dwValue;}

	DWORD GetSrcBlend() { return m_dwSrcBlend;}
	DWORD GetDstBlend() { return m_dwDstBlend;}


	SGEngine::Core::CMeshHeader m_MeshHeader;

	SGEngine::Core::CMeshMaterialInfo *m_pMaterial;
	int m_iMaterial;
	
	SGEngine::Core::CMeshCoreData *m_pMeshData;
	

	

	SGEngine::Core::CMeshObjNode *m_pRootNode;



	LPDIRECT3DVERTEXBUFFER8 *m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 *m_pIndexBuffer;


	std::vector<CTexture *> m_lstTexture;


	int m_iMeshData;
	
	int m_iFrameInfo[4];	
	
	DWORD m_dwSrcBlend;
	DWORD m_dwDstBlend;



};



#endif // !defined(AFX_SMROBJ_H__FD33169B_0305_4327_A366_EC1DB3AAD241__INCLUDED_)
