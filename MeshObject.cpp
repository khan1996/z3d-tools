// MeshObject.cpp: implementation of the CMeshObject class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshObject.h"
#include "SceneManager.h"
#include "FileLoad.h"
#include "RenderOption.h"
#include <Src/Scene/D3DBufferPools.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECT3DDEVICE8 CMeshObject::m_pd3dDevice;

CMeshObject::CMeshObject()
{
	m_MaxBox.x=-1.0f;
	m_MinBox.x=-1.0f;
	m_isAlreadyLock=false;
	m_dwCustomizeShader=0xffffffff;
}

CMeshObject::~CMeshObject()
{
	int i;
	
	for(i=0;i<(int)m_pLoadedIndices.size();i++)
		delete[] m_pLoadedIndices[i];

	for(i=0;i<(int)m_pLoadedVertex.size();i++)
		delete[] m_pLoadedVertex[i];

	m_pLoadedVertex.clear();
	m_pLoadedIndices.clear();


	for(i=0;i<(int)m_pVertex.size();i++)
		delete m_pVertex[i];

	for(i=0;i<(int)m_pIndices.size();i++)
		delete m_pIndices[i];

	m_pVertex.clear();
	m_pIndices.clear();
	
	if(m_bLoaded == false)
	{
		for(i=0;i<(int)m_pVertexBuffer.size();i++)
			m_pVertexBuffer[i]->Release();

		for(i=0;i<(int)m_pIndicesBuffer.size();i++)
			m_pIndicesBuffer[i]->Release();

		m_pVertexBuffer.clear();
		m_pIndicesBuffer.clear();
	}
	else	// Buffer Pool 이용
	{
		if(CSceneManager::ms_pBufferPools)
		{
			for(i=0;i<(int)m_pVertexBuffer.size();i++)
			{
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);
			}

			for(i=0;i<(int)m_pIndicesBuffer.size();i++)
			{
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,m_pIndicesBuffer[i]);
			}
		}
		else
		{

			for(i=0;i<(int)m_pVertexBuffer.size();i++)
				m_pVertexBuffer[i]->Release();

			for(i=0;i<(int)m_pIndicesBuffer.size();i++)
				m_pIndicesBuffer[i]->Release();

		}
		m_pVertexBuffer.clear();
		m_pIndicesBuffer.clear();
	}

	for(i=0;i<(int)m_pMat.size();i++)
	{
		//m_pMat[i]->DeleteTexture();
		delete m_pMat[i];
	}
	m_pMat.clear();

	for(i=0;i<(int)m_pDetailMap.size();i++)
	{
		if(m_pDetailMap[i])
			delete m_pDetailMap[i];
	}	
	m_pDetailMap.clear();

	for(i=0;i<(int)m_pBumpTexture.size();i++)
	{
		if(m_pBumpTexture[i])
			delete m_pBumpTexture[i];
	}
	m_pBumpTexture.clear();

	/*
	CVertexBuffer *pDelVertexBuffer;
	WORD *pDelIndices;
	CTexture *pDelTexture;
	for(int i=0;i<m_pVertexBuffer.num;i++)
	{
		pDelVertexBuffer=m_pVertexBuffer[i];
		pDelIndices=m_pIndices[i];				
		delete [] pDelIndices;
		delete pDelVertexBuffer;
	}
	for(i=0;i<m_pMat.num;i++)
	{
		pDelTexture=m_pMat[i];
		delete pDelTexture;
	}
	*/
}
bool CMeshObject::Load(Caldron::Base::CCaldronByteDataObj *pDataObj)
{
	
	strcpy(m_strMeshName,m_strName);

	int nObject,nMat,ObjectMethod;
	pDataObj->Read(&nObject,sizeof(int),1);
	pDataObj->Read(&nMat,sizeof(int),1);
	pDataObj->Read(&ObjectMethod,sizeof(int),1);

	m_bRender=false;

	if(ObjectMethod==0)
	{
		m_bCompleteMesh=false;		

		char strTextureName[256];
		for(int i=0;i<nMat;i++)
		{
			pDataObj->Read(strTextureName,sizeof(char)*MAX_NAMEBUFFER,1);
			if(strcmp(strTextureName,"")==0)
				continue;

			m_pMat.push_back(new CTexture());			
			strcpy(m_pMat[i]->m_strName,strTextureName);
		}

		char strObjectName[256];
		int MatRef,cVertex,cIndices;	

		
		vector3 vecNormal;
		vector3 *v[3];
		
		SetShader(LightFVF);

		for(int i=0;i<nObject;i++)
		{
			pDataObj->Read(strObjectName,sizeof(char)*256,1);
			pDataObj->Read(&MatRef,sizeof(long),1);
			pDataObj->Read(&cVertex,sizeof(long),1);
			pDataObj->Read(&cIndices,sizeof(int),1);
		
			LightVertex *pTempLightVertex = new LightVertex[cVertex];
			MultiVertex *pTempVertex=new MultiVertex[cVertex];
			WORD *pTempIndices=new WORD[cIndices*3];

			pDataObj->Read(pTempVertex,sizeof(MultiVertex) * cVertex,1);
			pDataObj->Read(pTempIndices,sizeof(WORD) * cIndices*3,1);
			// CalcBox
			if(m_MaxBox.x==-1.0f && m_MinBox.x==-1.0f)
			{
				m_MinBox.x=m_MaxBox.x=pTempVertex[0].v.x;
				m_MinBox.y=m_MaxBox.y=pTempVertex[0].v.y;
				m_MinBox.z=m_MaxBox.z=pTempVertex[0].v.z;
			}


			for(int cv = 0; cv < cVertex; cv++)
			{
				if(m_MaxBox.x < pTempVertex[cv].v.x)
					m_MaxBox.x=pTempVertex[cv].v.x;
				if(m_MaxBox.y < pTempVertex[cv].v.y)
					m_MaxBox.y=pTempVertex[cv].v.y;
				if(m_MaxBox.z < pTempVertex[cv].v.z)
					m_MaxBox.z=pTempVertex[cv].v.z;

				if(m_MinBox.x > pTempVertex[cv].v.x)
					m_MinBox.x=pTempVertex[cv].v.x;
				if(m_MinBox.y > pTempVertex[cv].v.y)
					m_MinBox.y=pTempVertex[cv].v.y;
				if(m_MinBox.z > pTempVertex[cv].v.z)
					m_MinBox.z=pTempVertex[cv].v.z;

			}
			// Convert Normal
			for(int cIn=0;cIn<cIndices;cIn++)
			{
				v[0]=(vector3*)&pTempVertex[ pTempIndices[cIn*3+0] ].v;
				v[1]=(vector3*)&pTempVertex[ pTempIndices[cIn*3+1] ].v;
				v[2]=(vector3*)&pTempVertex[ pTempIndices[cIn*3+2] ].v;

				vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
				vecNormal.Normalize();
				
				pTempLightVertex[pTempIndices[cIn*3+0]].v=pTempVertex[ pTempIndices[cIn*3+0] ].v;
				pTempLightVertex[pTempIndices[cIn*3+0]].tu=pTempVertex[ pTempIndices[cIn*3+0] ].tu;
				pTempLightVertex[pTempIndices[cIn*3+0]].tv=pTempVertex[ pTempIndices[cIn*3+0] ].tv;
				pTempLightVertex[pTempIndices[cIn*3+0]].spec.c=0x0;
				pTempLightVertex[pTempIndices[cIn*3+0]].n=vecNormal;

				pTempLightVertex[pTempIndices[cIn*3+1]].v=pTempVertex[ pTempIndices[cIn*3+1] ].v;
				pTempLightVertex[pTempIndices[cIn*3+1]].tu=pTempVertex[ pTempIndices[cIn*3+1] ].tu;
				pTempLightVertex[pTempIndices[cIn*3+1]].tv=pTempVertex[ pTempIndices[cIn*3+1] ].tv;
				pTempLightVertex[pTempIndices[cIn*3+1]].spec.c=0x0;
				pTempLightVertex[pTempIndices[cIn*3+1]].n=vecNormal;

				pTempLightVertex[pTempIndices[cIn*3+2]].v=pTempVertex[ pTempIndices[cIn*3+2] ].v;
				pTempLightVertex[pTempIndices[cIn*3+2]].tu=pTempVertex[ pTempIndices[cIn*3+2] ].tu;
				pTempLightVertex[pTempIndices[cIn*3+2]].tv=pTempVertex[ pTempIndices[cIn*3+2] ].tv;
				pTempLightVertex[pTempIndices[cIn*3+2]].spec.c=0x0;
				pTempLightVertex[pTempIndices[cIn*3+2]].n=vecNormal;
				
			}


	
			m_pLoadedVertex.push_back(pTempLightVertex);
			m_pLoadedIndices.push_back(pTempIndices);

			m_nVertex.push_back(cVertex);
			m_nIndices.push_back(cIndices);
			m_pMatRef.push_back(MatRef);

			delete[] pTempVertex;
		}
		
		

	}
	return true;
}
bool CMeshObject::PostLoad()
{
	int i;

	m_bLoaded = true;

	m_bRender=true;

	if(COLLISIONOBJECTVIEW==0 && strstr(m_strMeshName,"collision")!=NULL)
	{
		m_bRender=false;
	}

	LightVertex *pVertexData;
	WORD *pIndicesData;

	int cVertex = 0;
	int cIndices = 0;

	for(i = 0; i < (int)m_pLoadedVertex.size(); i++ )
	{
		cVertex =  m_nVertex[i];
		cIndices = m_nIndices[i];

		LPDIRECT3DVERTEXBUFFER8 pAllocVB;
		LPDIRECT3DINDEXBUFFER8 pAllocIB;


		pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(cVertex*sizeof(LightVertex),LightFVF,false);
		pAllocIB = CSceneManager::ms_pBufferPools->GetIndexBuffer(cIndices*3*sizeof(WORD),D3DFMT_INDEX16,false);
		/*
		CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),NULL,
											MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
		CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),NULL,
										D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);
		*/
//		pAllocVB->Lock(0,sizeof(LightVertex) * cVertex,(BYTE**)&pVertexData,0);
//		pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
		pAllocIB->Lock(0,0,(BYTE**)&pIndicesData,0);
		
		memcpy(pVertexData,m_pLoadedVertex[i],sizeof(LightVertex) * cVertex);
		memcpy(pIndicesData,m_pLoadedIndices[i],sizeof(WORD) * cIndices * 3);

		pAllocVB->Unlock();
		pAllocIB->Unlock();

		m_pVertexBuffer.push_back(pAllocVB);
		m_pIndicesBuffer.push_back(pAllocIB);		

		delete[] m_pLoadedVertex[i];
		delete[] m_pLoadedIndices[i];

	}


	for(i=0;i<(int)m_pLoadedIndices.size();i++)
		delete[] m_pLoadedIndices[i];

	for(i=0;i<(int)m_pLoadedVertex.size();i++)
		delete[] m_pLoadedVertex[i];


	m_pLoadedVertex.clear();
	m_pLoadedIndices.clear();

	CTexture::SetPath(OBJECTTEXTUREPATH);

	TextureLoad();

	return true;

}
/*
bool CMeshObject::Unload()
{
	return true;
}
*/

void CMeshObject::Load(char *strFilename)
{	
	strcpy(m_strMeshName,strFilename);

	CFileLoad FileLoader;
	FileLoader.Load(strFilename);
	// By Belldandy : Load에서 뻑났을때 자연스레 나가는 루틴이 필요 or 정상적으로 실행되는 루틴 필요

	int nObject,nMat,ObjectMethod;	
	FileLoader.GetData(&nObject,sizeof(int));
	FileLoader.GetData(&nMat,sizeof(int));
	FileLoader.GetData(&ObjectMethod,sizeof(int));

	m_bRender=true;

	if(COLLISIONOBJECTVIEW==0 && strstr(strFilename,"collision")!=NULL)
	{
		m_bRender=false;
	}

	if(ObjectMethod==0)
	{
		m_bCompleteMesh=false;		

		

		char strTextureName[256];
		for(int i=0;i<nMat;i++)
		{
			FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);
			if(strcmp(strTextureName,"")==0)
				continue;
			m_pMat.push_back(new CTexture());			
			strcpy(m_pMat[i]->m_strName,strTextureName);
		}
		char strObjectName[256];
		int MatRef,cVertex,cIndices;	
		MultiVertex *pVertexData;
		WORD *pIndicesData;

		for(int i=0;i<nObject;i++)
		{
			FileLoader.GetData(strObjectName,sizeof(char)*256);
			FileLoader.GetData(&MatRef,sizeof(long));
			FileLoader.GetData(&cVertex,sizeof(long));
			FileLoader.GetData(&cIndices,sizeof(int));		

			LPDIRECT3DVERTEXBUFFER8 pAllocVB;
			LPDIRECT3DINDEXBUFFER8 pAllocIB;
			
			MultiVertex *pTempVertex=new MultiVertex[cVertex];
			WORD *pTempIndices=new WORD[cIndices*3];

			CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),NULL,
											 MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
			CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),NULL,
											D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
			pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);

			FileLoader.GetData(pVertexData,sizeof(MultiVertex)*cVertex);
			FileLoader.GetData(pIndicesData,sizeof(WORD)*cIndices*3);
			
			pAllocVB->Unlock();
			pAllocIB->Unlock();

			m_pVertexBuffer.push_back(pAllocVB);
			m_pIndicesBuffer.push_back(pAllocIB);		
			
			m_nVertex.push_back(cVertex);
			m_nIndices.push_back(cIndices);
			m_pMatRef.push_back(MatRef);

			delete [] pTempIndices;
			delete [] pTempVertex;
		}
	}
	else
	{
		if(ObjectMethod==1)
		{
			m_bCompleteMesh=true;			
			char strTextureName[256];
			for(int i=0;i<nMat;i++)
			{
				m_nVertex.push_back(0);
				m_nIndices.push_back(0);
				m_pMatRef.push_back(0);
				FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);

				if(strcmp(strTextureName,"")==0)
					continue;

				m_pMat.push_back(new CTexture());
				strcpy(m_pMat[i]->m_strName,strTextureName);

				if(CRenderOption::m_AllObjectBump)
				{
					int nPos=strlen(strTextureName);

					strTextureName[nPos-4]='_';
					strTextureName[nPos-3]='b';
					strTextureName[nPos-2]='u';
					strTextureName[nPos-1]='m';
					strTextureName[nPos-0]='p';
					strTextureName[nPos+1]='.';
					strTextureName[nPos+2]='d';
					strTextureName[nPos+3]='d';
					strTextureName[nPos+4]='s';
					strTextureName[nPos+5]=0;

					m_pBumpTexture.push_back(new CTexture());

					strcpy(m_pBumpTexture[i]->m_strName,strTextureName);
				}					
			}

			FileLoader.GetData(&m_MaxBox,sizeof(vector3));
			FileLoader.GetData(&m_MinBox,sizeof(vector3));
			int nTotalVertex,nTotalIndices;

			FileLoader.GetData(&nTotalVertex,sizeof(int));
			FileLoader.GetData(&nTotalIndices,sizeof(int));		

			FileLoader.GetData(&m_nIndices[0],sizeof(int)*nObject);		
			FileLoader.GetData(&m_nVertex[0],sizeof(int)*nObject);		
			FileLoader.GetData(&m_pMatRef[0],sizeof(int)*nObject);

			LPDIRECT3DVERTEXBUFFER8 pAllocVB;
			LPDIRECT3DINDEXBUFFER8 pAllocIB;

			CSceneManager::GetDevice()->CreateVertexBuffer(nTotalVertex*sizeof(BumpVertex2),
				D3DUSAGE_WRITEONLY,BUMPVERTEX2FVF,D3DPOOL_MANAGED,&pAllocVB);		
			
			CSceneManager::GetDevice()->CreateIndexBuffer(nTotalIndices*3*sizeof(WORD),
				D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

			BumpVertex2 *pVertexData;
			WORD *pIndicesData;

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
			pAllocIB->Lock(0,nTotalIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);		

			FileLoader.GetData(pVertexData,sizeof(BumpVertex2)*nTotalVertex);
			FileLoader.GetData(pIndicesData,sizeof(WORD)*nTotalIndices*3);

			pAllocVB->Unlock();
			pAllocIB->Unlock();

			m_pVertexBuffer.push_back(pAllocVB);
			m_pIndicesBuffer.push_back(pAllocIB);	

			m_dwShader=BUMPVERTEX2FVF;
		}
		else
		{
			return;
			m_bCompleteMesh=true;
			
			char strTextureName[256];
			for(int i=0;i<nMat;i++)
			{
				m_nVertex.push_back(0);
				m_nIndices.push_back(0);
				m_pMatRef.push_back(0);
				FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);
				if(strcmp(strTextureName,"")==0)
					continue;

				m_pMat.push_back(new CTexture());
				strcpy(m_pMat[i]->m_strName,strTextureName);

				if(CRenderOption::m_AllObjectBump)
				{
					int nPos=strlen(strTextureName);

					strTextureName[nPos-4]='_';
					strTextureName[nPos-3]='b';
					strTextureName[nPos-2]='u';
					strTextureName[nPos-1]='m';
					strTextureName[nPos-0]='p';
					strTextureName[nPos+1]='.';
					strTextureName[nPos+2]='d';
					strTextureName[nPos+3]='d';
					strTextureName[nPos+4]='s';
					strTextureName[nPos+5]=0;

					m_pBumpTexture.push_back(new CTexture());

					strcpy(m_pBumpTexture[i]->m_strName,strTextureName);
				}					
			}

			

			FileLoader.GetData(&m_MaxBox,sizeof(vector3));
			FileLoader.GetData(&m_MinBox,sizeof(vector3));
			int nTotalVertex,nTotalIndices;

			FileLoader.GetData(&nTotalVertex,sizeof(int));
			FileLoader.GetData(&nTotalIndices,sizeof(int));		

			FileLoader.GetData(&m_nIndices[0],sizeof(int)*nObject);		
			FileLoader.GetData(&m_nVertex[0],sizeof(int)*nObject);		
			FileLoader.GetData(&m_pMatRef[0],sizeof(int)*nObject);

			LPDIRECT3DVERTEXBUFFER8 pAllocVB;
			LPDIRECT3DINDEXBUFFER8 pAllocIB;

			CSceneManager::GetDevice()->CreateVertexBuffer(nTotalVertex*sizeof(BumpVertex),
				D3DUSAGE_WRITEONLY,BUMPVERTEXFVF,D3DPOOL_MANAGED,&pAllocVB);		
			
			CSceneManager::GetDevice()->CreateIndexBuffer(nTotalIndices*3*sizeof(WORD),
				D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

			BumpVertex2 *pVertexData;
			WORD *pIndicesData;

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
			pAllocIB->Lock(0,nTotalIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);		

			FileLoader.GetData(pVertexData,sizeof(BumpVertex)*nTotalVertex);
			FileLoader.GetData(pIndicesData,sizeof(WORD)*nTotalIndices*3);			
			
			pAllocVB->Unlock();
			pAllocIB->Unlock();

			m_pVertexBuffer.push_back(pAllocVB);
			m_pIndicesBuffer.push_back(pAllocIB);	

			m_dwShader=BUMPVERTEXFVF;
		}
		
	}

	/*
	FILE *fp;
	fp=fopen(strFilename,"rb");

	int Lens;
	if(fp==NULL)
	{
		char errormsg[256];
		strcpy(errormsg,"CMeshObject:Load , File not found :");
		strcat(errormsg,strFilename);
		throw CSceneLayerError(errormsg);
	}	
	fseek(fp,0L,SEEK_END);
	Lens=ftell(fp);		
	fseek(fp,0L,SEEK_SET);


	int nObject,nMat,TextureSize;	
	fread((void*)(&nObject),sizeof(int),1,fp);
	fread((void*)(&nMat),sizeof(int),1,fp);
	fread((void*)(&TextureSize),sizeof(int),1,fp);

	char strTextureName[256];
	for(int i=0;i<nMat;i++)
	{
		fread(strTextureName,sizeof(char)*256,1,fp);		
		m_pMat.Add(new CTexture());				
		//strcpy(m_pMat[i]->m_strName,strTextureName);
		m_pMat[i]->Load(strTextureName);
	}
	char strObjectName[256];
	int MatRef,cVertex,cIndices;	
	MultiVertex *pVertexData;
	WORD *pIndicesData;
	for(i=0;i<nObject;i++)
	{
		fread(strObjectName,sizeof(char)*256,1,fp);
		fread((void*)(&MatRef),sizeof(long),1,fp);
		fread((void*)(&cVertex),sizeof(long),1,fp);
		fread((void*)(&cIndices),sizeof(int),1,fp);

		LPDIRECT3DVERTEXBUFFER8 pAllocVB;
		LPDIRECT3DINDEXBUFFER8 pAllocIB;
		
		CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
										 MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
		CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

		pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
		pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		fread((void*)pVertexData,sizeof(MultiVertex)*cVertex,1,fp);
		fread((void*)pIndicesData,sizeof(WORD)*cIndices*3,1,fp);
		pAllocVB->Unlock();
		pAllocIB->Unlock();

		m_pVertexBuffer.Add(pAllocVB);
		m_pIndicesBuffer.Add(pAllocIB);		

		m_nVertex.Add(cVertex);
		m_nIndices.Add(cIndices);

		m_pMatRef.Add(MatRef);
	}
	fclose(fp);		
	//Optimize();
	*/
}

void CMeshObject::CalcBox()
{	
	if(m_bCompleteMesh)
		return;

	MultiVertex *pVertexData;

	if(m_pVertexBuffer.empty())
	{
		throw CSceneLayerError("CMeshObject:CalcBox, This meshobject have not vertex");
	}

	for(int i=0;i<(int)m_pVertexBuffer.size();i++)
	{
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);				
		if(m_MaxBox.x==-1.0f && m_MinBox.x==-1.0f)
		{
			m_MinBox.x=m_MaxBox.x=pVertexData[0].v.x;
			m_MinBox.y=m_MaxBox.y=pVertexData[0].v.y;
			m_MinBox.z=m_MaxBox.z=pVertexData[0].v.z;
			
		}		
		for(int cv=0;cv<m_nVertex[i];cv++)
		{
			if(m_MaxBox.x < pVertexData[cv].v.x)
				m_MaxBox.x=pVertexData[cv].v.x;
			if(m_MaxBox.y < pVertexData[cv].v.y)
				m_MaxBox.y=pVertexData[cv].v.y;
			if(m_MaxBox.z < pVertexData[cv].v.z)
				m_MaxBox.z=pVertexData[cv].v.z;

			if(m_MinBox.x > pVertexData[cv].v.x)
				m_MinBox.x=pVertexData[cv].v.x;
			if(m_MinBox.y > pVertexData[cv].v.y)
				m_MinBox.y=pVertexData[cv].v.y;
			if(m_MinBox.z > pVertexData[cv].v.z)
				m_MinBox.z=pVertexData[cv].v.z;
			pVertexData[cv].diff.a=0xaa;
		}
		m_pVertexBuffer[i]->Unlock();
	}	
}

void CMeshObject::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	if(!m_bRender)
		return;
	if(m_dwCustomizeShader!=0xffffffff)
	{		
		pd3dDevice->SetVertexShader(m_dwCustomizeShader);

		if(m_dwShader==BUMPVERTEX2FVF)
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[0],sizeof(BumpVertex2));
		if(m_dwShader==BUMPVERTEXFVF)
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[0],sizeof(BumpVertex));
		pd3dDevice->SetIndices(m_pIndicesBuffer[0],0);
		int nVertexOffset,nIndicesOffset;
		nVertexOffset=nIndicesOffset=0;

		for(int i=0;i<(int)m_nVertex.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());			
			if(CRenderOption::m_AllObjectBump && m_pBumpTexture.size()!=0)
			{
				pd3dDevice->SetTexture(1,m_pBumpTexture[m_pMatRef[i]]->GetTexture());
			}

			if(m_nVertex[i]==0 || m_nIndices[i]==0)
				continue;

			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,nVertexOffset,
											 m_nVertex[i],
											 nIndicesOffset,
											 m_nIndices[i]);
			nVertexOffset+=m_nVertex[i];
			nIndicesOffset+=m_nIndices[i]*3;
		}		

		return;
	}
	else
		pd3dDevice->SetVertexShader(m_dwShader);
	
	if(m_dwShader==MultiFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(MultiVertex));
			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);   

			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}	
		return;
	}
	if(m_dwShader==LightFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(LightVertex));
			
			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);		
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);   

			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}
		return;
	}
	if(m_dwShader==DetailFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());
			
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(DetailVertex));			
			
			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);		
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);
			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}
		return;
	}
	if(m_dwShader==TREEVERTEXFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(LightVertex));			

			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);        
			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}
		return;
	}
	if(m_dwShader==BUMPVERTEX2FVF)
	{
		pd3dDevice->SetStreamSource(0,m_pVertexBuffer[0],sizeof(BumpVertex2));
		pd3dDevice->SetIndices(m_pIndicesBuffer[0],0);
		int nVertexOffset,nIndicesOffset;
		nVertexOffset=nIndicesOffset=0;

		for(int i=0;i<(int)m_nVertex.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());			
			if(m_nVertex[i]==0 || m_nIndices[i]==0)
				continue;
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,nVertexOffset,
											 m_nVertex[i],
											 nIndicesOffset,
											 m_nIndices[i]);
			nVertexOffset+=m_nVertex[i];
			nIndicesOffset+=m_nIndices[i]*3;
		}	
		return;
	}

	if(m_dwShader==BUMPVERTEXFVF)
	{		
		pd3dDevice->SetStreamSource(0,m_pVertexBuffer[0],sizeof(BumpVertex));
		pd3dDevice->SetIndices(m_pIndicesBuffer[0],0);
		int nVertexOffset,nIndicesOffset;
		nVertexOffset=nIndicesOffset=0;

		for(int i=0;i<(int)m_nVertex.size();i++)		
		{
			pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());			
			if(m_nVertex[i]==0 || m_nIndices[i]==0)
				continue;
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,nVertexOffset,
											 m_nVertex[i],
											 nIndicesOffset,
											 m_nIndices[i]);
			nVertexOffset+=m_nVertex[i];
			nIndicesOffset+=m_nIndices[i]*3;
		}	
		return;
	}
}

void CMeshObject::RenderBox(LPDIRECT3DDEVICE8 pd3dDevice)
{
	LPDIRECT3DVERTEXBUFFER8 pBoxVB;
	MultiVertex *BoxVertex;	

	LPDIRECT3DINDEXBUFFER8 pBoxIB;
	WORD *pIndices;

	CSceneManager::GetDevice()->CreateVertexBuffer(8*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
										 MultiFVF,D3DPOOL_MANAGED,&pBoxVB);
	CSceneManager::GetDevice()->CreateIndexBuffer(24*sizeof(WORD),D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,D3DPOOL_MANAGED,&pBoxIB);			

	pBoxVB->Lock(0,0,(BYTE**)&BoxVertex,0);
	pBoxIB->Lock(0, 24*sizeof(WORD),(BYTE**)&pIndices,0);
	BoxVertex[0].v.x=m_MaxBox.x;
	BoxVertex[0].v.y=m_MaxBox.y;
	BoxVertex[0].v.z=m_MaxBox.z;	

	BoxVertex[1].v.x=m_MinBox.x;
	BoxVertex[1].v.y=m_MaxBox.y;
	BoxVertex[1].v.z=m_MaxBox.z;

	BoxVertex[2].v.x=m_MaxBox.x;
	BoxVertex[2].v.y=m_MaxBox.y;
	BoxVertex[2].v.z=m_MinBox.z;

	BoxVertex[3].v.x=m_MinBox.x;
	BoxVertex[3].v.y=m_MaxBox.y;
	BoxVertex[3].v.z=m_MinBox.z;

	BoxVertex[4].v.x=m_MaxBox.x;
	BoxVertex[4].v.y=m_MinBox.y;
	BoxVertex[4].v.z=m_MaxBox.z;

	BoxVertex[5].v.x=m_MinBox.x;
	BoxVertex[5].v.y=m_MinBox.y;
	BoxVertex[5].v.z=m_MaxBox.z;

	BoxVertex[6].v.x=m_MaxBox.x;
	BoxVertex[6].v.y=m_MinBox.y;
	BoxVertex[6].v.z=m_MinBox.z;

	BoxVertex[7].v.x=m_MinBox.x;
	BoxVertex[7].v.y=m_MinBox.y;
	BoxVertex[7].v.z=m_MinBox.z;
	for(int i=0;i<8;i++)
	{
		BoxVertex[i].diff.c=0xffffffff;
		BoxVertex[i].spec.c=0xffffffff;
	}
	
	*(pIndices++)=0;
	*(pIndices++)=1;
	*(pIndices++)=1;
	*(pIndices++)=3;
	*(pIndices++)=3;
	*(pIndices++)=2;
	*(pIndices++)=2;
	*(pIndices++)=0;	

	*(pIndices++)=4;
	*(pIndices++)=5;
	*(pIndices++)=5;
	*(pIndices++)=7;
	*(pIndices++)=7;
	*(pIndices++)=6;
	*(pIndices++)=6;
	*(pIndices++)=4;

	*(pIndices++)=0;
	*(pIndices++)=4;
	*(pIndices++)=1;
	*(pIndices++)=5;
	*(pIndices++)=3;
	*(pIndices++)=7;
	*(pIndices++)=2;
	*(pIndices++)=6;

	pBoxVB->Unlock();
	pBoxIB->Unlock();
	
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);	
	pd3dDevice->SetVertexShader(MultiFVF);	
		
	pd3dDevice->SetStreamSource(0,pBoxVB,sizeof(MultiVertex));
	pd3dDevice->SetIndices(pBoxIB,0);		
	pd3dDevice->DrawIndexedPrimitive(D3DPT_LINELIST,
									 0,8,
									 0,12);      
	pBoxVB->Release();
	pBoxIB->Release();
}

void CMeshObject::AllLockBuffer()
{		
	if(m_bCompleteMesh)
		return;
	if(m_isAlreadyLock==true)
		return;
	m_isAlreadyLock=true;
	if(m_dwShader==LightFVF)
	{
		LightVertex *pVertexData;
		m_pVertex.clear();

		for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		{
			m_pVertexBuffer[cVB]->Lock(0,0,(BYTE**)&pVertexData,NULL);
			m_pVertex.push_back(pVertexData);
		}
	}	
	else
	{
		MultiVertex *pVertexData;
		m_pVertex.clear();
		for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		{
			m_pVertexBuffer[cVB]->Lock(0,0,(BYTE**)&pVertexData,NULL);
			m_pVertex.push_back(pVertexData);
		}
	}
	WORD *pIndicesData;
	m_pIndices.clear();
	for(int cIB=0;cIB<(int)m_pIndicesBuffer.size();cIB++)
	{		
		m_pIndicesBuffer[cIB]->Lock(0,m_nIndices[cIB]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pIndices.push_back(pIndicesData);
	}
}

void CMeshObject::AllUnlockBuffer()
{
	if(m_bCompleteMesh)
		return;

	for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		m_pVertexBuffer[cVB]->Unlock();		

	for(int cIB=0;cIB<(int)m_pIndicesBuffer.size();cIB++)
		m_pIndicesBuffer[cIB]->Unlock();

	m_pVertex.clear();
	m_pIndices.clear();
	m_isAlreadyLock=false;
}

void CMeshObject::CopyBuffer()
{
	if(m_bCompleteMesh)
		return;
	MultiVertex *pVertexData=NULL;
	MultiVertex *pCopyVertex=NULL;

	WORD *pIndicesData=NULL;
	WORD *pCopyIndices=NULL;

	m_pVertex.clear();
	m_pIndices.clear();

	for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
	{
		pCopyVertex=new MultiVertex[m_nVertex[cVB]];
		m_pVertexBuffer[cVB]->Lock(0,0,(BYTE**)&pVertexData,NULL);
		memcpy(pCopyVertex,pVertexData,sizeof(MultiVertex)*m_nVertex[cVB]);
		m_pVertexBuffer[cVB]->Unlock();
		m_pVertex.push_back(pCopyVertex);		
	}

	for(int cIB=0;cIB<(int)m_pIndicesBuffer.size();cIB++)
	{
		pCopyIndices=new WORD[m_nIndices[cIB]*3*sizeof(WORD)];
		m_pIndicesBuffer[cIB]->Lock(0,m_nIndices[cIB]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		memcpy(pCopyIndices,pIndicesData,m_nIndices[cIB]*3*sizeof(WORD));
		m_pIndicesBuffer[cIB]->Unlock();
		m_pIndices.push_back(pCopyIndices);
	}
}

void CMeshObject::Save(char *strFilename)
{
	bool bConvert=false;
	if(m_dwShader!=MultiFVF)
	{
		bConvert=true;
		ConvertMulti();
	}
	FILE *fp;
	int TextureSize=0;
	fp=fopen(strFilename,"wb");
	int iCnt = m_pVertexBuffer.size();
	fwrite(&iCnt,sizeof(int),1,fp);

	iCnt = m_pMat.size();
	fwrite(&iCnt,sizeof(int),1,fp);	

	fwrite(&TextureSize,sizeof(int),1,fp);
	char strTextureName[256];

	for(int i=0;i<(int)m_pMat.size();i++)
	{
		sscanf(m_pMat[i]->m_strName,"%s",strTextureName);
		fwrite(strTextureName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
	}
	AllLockBuffer();

	char strObjectName[MAX_NAMEBUFFER]="";
	for(int i=0;i<(int)m_pVertexBuffer.size();i++)
	{
		fwrite(strObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(&m_pMatRef[i],sizeof(long),1,fp);
		fwrite(&m_nVertex[i],sizeof(long),1,fp);
		fwrite(&m_nIndices[i],sizeof(int),1,fp);
		fwrite(m_pVertex[i],sizeof(MultiVertex)*m_nVertex[i],1,fp);
		fwrite(m_pIndices[i],sizeof(WORD)*m_nIndices[i]*3,1,fp);
	}	
	fclose(fp);
	AllUnlockBuffer();
	if(bConvert)
	{
		ConvertNormal();
	}
}

void CMeshObject::ConvertNormal()
{	
	if(m_bCompleteMesh)
		return;
	if(m_dwShader==LightFVF)
		return;
	SetShader(LightFVF);
	LightVertex *pVertexNormalData;
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	vector3 vecNormal;
	vector3 *v[3];
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

		CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(LightVertex),D3DUSAGE_WRITEONLY,
													LightFVF,D3DPOOL_MANAGED,&pAllocVB);

		pAllocVB->Lock(0,0,(BYTE**)&pVertexNormalData,0);
		m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);

		for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
		{
			v[0]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+0] ].v;
			v[1]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+1] ].v;
			v[2]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+2] ].v;

			vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
			vecNormal.Normalize();

			pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexData[ pIndicesData[cIndices*3+2] ].v;
			
			pVertexNormalData[pIndicesData[cIndices*3+0]].v=pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tu=pVertexData[ pIndicesData[cIndices*3+0] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tv=pVertexData[ pIndicesData[cIndices*3+0] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+0]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+0]].n=vecNormal;


			pVertexNormalData[pIndicesData[cIndices*3+1]].v=pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tu=pVertexData[ pIndicesData[cIndices*3+1] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tv=pVertexData[ pIndicesData[cIndices*3+1] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+1]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+1]].n=vecNormal;

			pVertexNormalData[pIndicesData[cIndices*3+2]].v=pVertexData[ pIndicesData[cIndices*3+2] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tu=pVertexData[ pIndicesData[cIndices*3+2] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tv=pVertexData[ pIndicesData[cIndices*3+2] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+2]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+2]].n=vecNormal;
			
		}

#ifndef _DEBUG
		// smoothing
		int *iTmp = new int[m_nVertex[i]];
		int iCount;

		for(int iVert = 0; iVert < m_nVertex[i]; iVert++)
		{
			memset(iTmp,0,sizeof(int) * m_nVertex[i]);
			iCount = 0;

			vector3 vecPos = pVertexNormalData[iVert].v;
			vector3 vecNormal = vector3(0.0f,0.0f,0.0f);
			for(int j = 0; j < m_nVertex[i]; j++ )
			{
				if(pVertexNormalData[j].v.x == pVertexNormalData[iVert].v.x &&
					pVertexNormalData[j].v.y == pVertexNormalData[iVert].v.y &&
					pVertexNormalData[j].v.z == pVertexNormalData[iVert].v.z)
				{
					if(D3DXVec3Dot((D3DXVECTOR3 *)(&pVertexNormalData[j].n),(D3DXVECTOR3 *)(&pVertexNormalData[iVert].n)) >= 0.5f)
					{

						iTmp[iCount++] = j;
						vecNormal += pVertexNormalData[j].n;
					}
				}
			}
			vecNormal.Normalize();
			for(int iT = 0; iT < iCount; iT++)
			{
				pVertexNormalData[iTmp[iT]].n = vecNormal;
			}

			
		}
		delete[] iTmp;
		iTmp = NULL;
#endif
		
		pAllocVB->Unlock();		
		m_pIndicesBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Release();
		m_pVertexBuffer[i]=pAllocVB;
	}
}

void CMeshObject::ConvertMulti()
{
	if(m_bCompleteMesh)
		return;
	if(MultiFVF==m_dwShader)
		return;
	if(LightFVF==m_dwShader)
	{
		SetShader(MultiFVF);
		LightVertex *pVertexNormalData;
		MultiVertex *pVertexData;
		
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

			CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
														MultiFVF,D3DPOOL_MANAGED,&pAllocVB);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);		
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
			{
				pVertexData[cVertex].v=pVertexNormalData[cVertex].v;
				pVertexData[cVertex].tu=pVertexNormalData[cVertex].tu;
				pVertexData[cVertex].tv=pVertexNormalData[cVertex].tv;
				pVertexData[cVertex].spec.c=0x0;
				pVertexData[cVertex].diff.c=0xffffffff;			
			}		
			pAllocVB->Unlock();		
			m_pVertexBuffer[i]->Unlock();
			m_pVertexBuffer[i]->Release();
			m_pVertexBuffer[i]=pAllocVB;
		}
	}
	else
	{
		SetShader(MultiFVF);
		DetailVertex *pVertexNormalData;
		MultiVertex *pVertexData;
		
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

			CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
														MultiFVF,D3DPOOL_MANAGED,&pAllocVB);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);		
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
			{
				
				pVertexData[cVertex].v=pVertexNormalData[cVertex].v;
				pVertexData[cVertex].tu=pVertexNormalData[cVertex].tu;
				pVertexData[cVertex].tv=pVertexNormalData[cVertex].tv;				
				pVertexData[cVertex].spec.c=0x0;
				pVertexData[cVertex].diff.c=0xffffffff;							
			}		
			pAllocVB->Unlock();		
			m_pVertexBuffer[i]->Unlock();
			m_pVertexBuffer[i]->Release();
			m_pVertexBuffer[i]=pAllocVB;
		}
	}
}

void CMeshObject::Optimize()
{
	if(m_bCompleteMesh)
		return;
	DWORD dwShaderTemp=m_dwShader;
	if(m_dwShader!=MultiFVF)	
		ConvertMulti();

	const float fEll=0.0001f;
	CopyBuffer();
	for(int i=0;i<(int)m_nIndices.size();i++)
	{
		MultiVertex *pOPVertexData=new MultiVertex[m_nVertex[i]];
		MultiVertex *pOldVertexData=(MultiVertex *)m_pVertex[i];
		WORD *pOPIndicesData=new WORD[m_nIndices[i]*3];
		int cNowAdd=0;
		for(int cIndices=0;cIndices<m_nIndices[i]*3;cIndices++)
		{
			MultiVertex CompareVertex=pOldVertexData[m_pIndices[i][cIndices]];
			bool isAlready=false;
			int cVertex=0;

			for(;cVertex<cNowAdd;cVertex++)
			{
				if(dwShaderTemp==MultiFVF)
				{
					if( fabs( CompareVertex.v.x - pOPVertexData[cVertex].v.x ) <= fEll &&
						fabs( CompareVertex.v.y - pOPVertexData[cVertex].v.y ) <= fEll &&
						fabs( CompareVertex.v.z - pOPVertexData[cVertex].v.z ) <= fEll &&
						fabs( CompareVertex.tu - pOPVertexData[cVertex].tu ) <= fEll &&
						fabs( CompareVertex.tv - pOPVertexData[cVertex].tv ) <= fEll &&
						fabs( CompareVertex.tu1 - pOPVertexData[cVertex].tu1) <= fEll &&
						fabs( CompareVertex.tv1 - pOPVertexData[cVertex].tv1) <= fEll)
					{
						isAlready=true;
						break;
					}
				}
				else
				{
					if( fabs( CompareVertex.v.x - pOPVertexData[cVertex].v.x ) <= fEll &&
						fabs( CompareVertex.v.y - pOPVertexData[cVertex].v.y ) <= fEll &&
						fabs( CompareVertex.v.z - pOPVertexData[cVertex].v.z ) <= fEll &&
						fabs( CompareVertex.tu - pOPVertexData[cVertex].tu ) <= fEll &&
						fabs( CompareVertex.tv - pOPVertexData[cVertex].tv ) <= fEll )						
					{
						isAlready=true;
						break;
					}
				}
			}
			if(isAlready)
			{
				pOPIndicesData[cIndices]=cVertex;
			}
			else
			{
				pOPVertexData[cVertex]=CompareVertex;
				pOPIndicesData[cIndices]=cVertex;
				cNowAdd++;
			}
		} 
		m_nVertex[i]=cNowAdd;

		LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

		MultiVertex *pVertexData;
		CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
														MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
		pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
		memcpy(pVertexData,pOPVertexData,sizeof(MultiVertex)*m_nVertex[i]);
		pAllocVB->Unlock();
		m_pVertexBuffer[i]->Release();
		m_pVertexBuffer[i]=pAllocVB;		

		WORD *pIndicesData;
		m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		memcpy(pIndicesData,pOPIndicesData,sizeof(WORD)*m_nIndices[i]*3);
		m_pIndicesBuffer[i]->Unlock();		
	}
	if(dwShaderTemp!=MultiFVF)
		ConvertNormal();
}

void CMeshObject::TextureLoad()
{
	for(int i=0;i<(int)m_pMat.size();i++)
	{			
		//if(m_nVertex[i]==0 || m_nIndices[i]==0)
		//	continue;
		m_pMat[i]->Load(m_pMat[i]->m_strName);	

		if(CRenderOption::m_AllObjectBump && m_pBumpTexture.size()!=0)
		{
			m_pBumpTexture[i]->Load(m_pBumpTexture[i]->m_strName);			
		}
		
	}	
}

void CMeshObject::ConvertDetail()
{
	if(m_bCompleteMesh)
		return;
	if(m_dwShader==DetailFVF)
		return;
	if(m_dwShader==MultiFVF)
	{
		MultiVertex *pVertexData;
		WORD *pIndicesData;
		DetailVertex *pVertexNormalData;
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

			CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(DetailVertex),D3DUSAGE_WRITEONLY,
														DetailFVF,D3DPOOL_MANAGED,&pAllocVB);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);

			vector3 *v[3];

			vector3 vecNormal;

			for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
			{
				v[0]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+0] ].v;
				v[1]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+1] ].v;
				v[2]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+2] ].v;

				vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
				vecNormal.Normalize();			
				
				pVertexNormalData[pIndicesData[cIndices*3+0]].v=pVertexData[ pIndicesData[cIndices*3+0] ].v;
				pVertexNormalData[pIndicesData[cIndices*3+0]].tu=pVertexData[ pIndicesData[cIndices*3+0] ].tu;
				pVertexNormalData[pIndicesData[cIndices*3+0]].tv=pVertexData[ pIndicesData[cIndices*3+0] ].tv;				
				pVertexNormalData[pIndicesData[cIndices*3+0]].tu1=pVertexData[ pIndicesData[cIndices*3+0] ].tu*30.0f;
				pVertexNormalData[pIndicesData[cIndices*3+0]].tv1=pVertexData[ pIndicesData[cIndices*3+0] ].tv*30.0f;
				pVertexNormalData[pIndicesData[cIndices*3+0]].n=vecNormal;

				pVertexNormalData[pIndicesData[cIndices*3+1]].v=pVertexData[ pIndicesData[cIndices*3+1] ].v;
				pVertexNormalData[pIndicesData[cIndices*3+1]].tu=pVertexData[ pIndicesData[cIndices*3+1] ].tu;
				pVertexNormalData[pIndicesData[cIndices*3+1]].tv=pVertexData[ pIndicesData[cIndices*3+1] ].tv;
				pVertexNormalData[pIndicesData[cIndices*3+1]].tu1=pVertexData[ pIndicesData[cIndices*3+1] ].tu*30.0f;
				pVertexNormalData[pIndicesData[cIndices*3+1]].tv1=pVertexData[ pIndicesData[cIndices*3+1] ].tv*30.0f;				
				pVertexNormalData[pIndicesData[cIndices*3+1]].n=vecNormal;

				pVertexNormalData[pIndicesData[cIndices*3+2]].v=pVertexData[ pIndicesData[cIndices*3+2] ].v;
				pVertexNormalData[pIndicesData[cIndices*3+2]].tu=pVertexData[ pIndicesData[cIndices*3+2] ].tu;
				pVertexNormalData[pIndicesData[cIndices*3+2]].tv=pVertexData[ pIndicesData[cIndices*3+2] ].tv;				
				pVertexNormalData[pIndicesData[cIndices*3+2]].tu1=pVertexData[ pIndicesData[cIndices*3+2] ].tu*30.0f;
				pVertexNormalData[pIndicesData[cIndices*3+2]].tv1=pVertexData[ pIndicesData[cIndices*3+2] ].tv*30.0f;
				pVertexNormalData[pIndicesData[cIndices*3+2]].n=vecNormal;
				
			}
			pAllocVB->Unlock();		
			m_pIndicesBuffer[i]->Unlock();
			m_pVertexBuffer[i]->Unlock();
			m_pVertexBuffer[i]->Release();
			m_pVertexBuffer[i]=pAllocVB;
		}
	}
	else
	{		
		LightVertex *pVertexNormalData;
		DetailVertex *pVertexData;	
		
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;

			CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
														MultiFVF,D3DPOOL_MANAGED,&pAllocVB);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);		
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
			{
				pVertexData[cVertex].v=pVertexNormalData[cVertex].v;
				pVertexData[cVertex].tu=pVertexNormalData[cVertex].tu;
				pVertexData[cVertex].tv=pVertexNormalData[cVertex].tv;
				pVertexData[cVertex].n=pVertexNormalData[cVertex].n;
				pVertexData[cVertex].tu1=pVertexNormalData[cVertex].tu*10.0f;
				pVertexData[cVertex].tv1=pVertexNormalData[cVertex].tv*10.0f;
			}		
			pAllocVB->Unlock();		
			m_pVertexBuffer[i]->Unlock();
			m_pVertexBuffer[i]->Release();
			m_pVertexBuffer[i]=pAllocVB;
		}
	}	
	SetShader(DetailFVF);
	//DetailTextureLoad();	
}

//DEL void CMeshObject::DetailTextureLoad()
//DEL {	
//DEL 	char strDetailName[256];
//DEL 	char strFullName[256];
//DEL 	int nlens;
//DEL 	FILE *fp=NULL;
//DEL 	CTexture::SetPath(OBJECTTEXTUREPATH);
//DEL 
//DEL 	for(int i=0;i<m_pMat.num;i++)
//DEL 	{
//DEL 		strcpy(strDetailName,m_pMat[i]->m_strName);
//DEL 		nlens=strlen(strDetailName);
//DEL 		strDetailName[nlens-4]='\0';
//DEL 		strcat(strDetailName,"_detail.dds");
//DEL 		sprintf(strFullName,"%s\\%s",OBJECTTEXTUREPATH,strDetailName);
//DEL 		fp=fopen(strFullName,"rb");
//DEL 		if(fp)
//DEL 		{
//DEL 			fclose(fp);
//DEL 			CTexture *AddNode=new CTexture();
//DEL 			strcpy(AddNode->m_strName,strDetailName);
//DEL 			AddNode->Load(AddNode->m_strName);
//DEL 			m_pDetailMap.Add(AddNode);
//DEL 		}
//DEL 		else
//DEL 		{
//DEL 			m_pDetailMap.Add(NULL);
//DEL 		}
//DEL 	}
//DEL 	m_bDetailTexture=true;
//DEL }

void CMeshObject::ConvertTree()
{
	SetShader(TREEVERTEXFVF);
	TreeVertex *pVertexNormalData;	
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	vector3 vecNormal;
	vector3 *v[3];
	float fMaxY,fMinY;
	fMaxY=-100000000000.0f;
	fMinY=100000000000.0f;
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);
		for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
		{
			if(fMaxY<pVertexData[cVertex].v.y)			
				fMaxY=pVertexData[cVertex].v.y;
			if(fMinY>pVertexData[cVertex].v.y)
				fMinY=pVertexData[cVertex].v.y;
			
		}
		m_pVertexBuffer[i]->Unlock();
	}

	float fSize=fMaxY-fMinY;	
	float fRate;//,fEnd;
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

		CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(TreeVertex),D3DUSAGE_WRITEONLY,
													TREEVERTEXFVF,D3DPOOL_MANAGED,&pAllocVB);

		pAllocVB->Lock(0,0,(BYTE**)&pVertexNormalData,0);
		m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);

		for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
		{
			v[0]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+0] ].v;
			v[1]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+1] ].v;
			v[2]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+2] ].v;

			vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
			vecNormal.Normalize();

			pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexData[ pIndicesData[cIndices*3+2] ].v;
			
			pVertexNormalData[pIndicesData[cIndices*3+0]].v=pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tu=pVertexData[ pIndicesData[cIndices*3+0] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tv=pVertexData[ pIndicesData[cIndices*3+0] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+0]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+0]].v.y-fMinY)/fSize;
			pVertexNormalData[pIndicesData[cIndices*3+0]].fWeight=fRate*fRate;
			/*
			if(fRate<0.5)
				pVertexNormalData[pIndicesData[cIndices*3+0]].fWeight=0.0f;
			else
			{
				fEnd=(fRate-0.5f)*2.0f;
				*/
				//pVertexNormalData[pIndicesData[cIndices*3+0]].fWeight=fEnd*fEnd;
			//}


			pVertexNormalData[pIndicesData[cIndices*3+1]].v=pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tu=pVertexData[ pIndicesData[cIndices*3+1] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tv=pVertexData[ pIndicesData[cIndices*3+1] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+1]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+1]].v.y-fMinY)/fSize;			
			pVertexNormalData[pIndicesData[cIndices*3+1]].fWeight=fRate*fRate;

			pVertexNormalData[pIndicesData[cIndices*3+2]].v=pVertexData[ pIndicesData[cIndices*3+2] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tu=pVertexData[ pIndicesData[cIndices*3+2] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tv=pVertexData[ pIndicesData[cIndices*3+2] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+2]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+2]].v.y-fMinY)/fSize;			
			pVertexNormalData[pIndicesData[cIndices*3+2]].fWeight=fRate*fRate;
			
		}
		// smoothing
		int *iTmp = new int[m_nVertex[i]];
		int iCount;

		for(int iVert = 0; iVert < m_nVertex[i]; iVert++)
		{
			memset(iTmp,0,sizeof(int) * m_nVertex[i]);
			iCount = 0;

			vector3 vecPos = pVertexNormalData[iVert].v;
			vector3 vecNormal = vector3(0.0f,0.0f,0.0f);
			for(int j = 0; j < m_nVertex[i]; j++ )
			{
				if(pVertexNormalData[j].v.x == pVertexNormalData[iVert].v.x &&
					pVertexNormalData[j].v.y == pVertexNormalData[iVert].v.y &&
					pVertexNormalData[j].v.z == pVertexNormalData[iVert].v.z)
				{
					iTmp[iCount++] = j;
					vecNormal += pVertexNormalData[j].n;

				}
			}
			vecNormal.Normalize();
			for(int iT = 0; iT < iCount; iT++)
			{
				pVertexNormalData[iTmp[iT]].n = vecNormal;
			}

			
		}
		delete[] iTmp;
		iTmp = NULL;

		//

		pAllocVB->Unlock();
		m_pIndicesBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Release();
		m_pVertexBuffer[i]=pAllocVB;
	}
}

void CMeshObject::MeshReload()
{
	m_pVertex.clear();
	m_pIndices.clear();
	for(int i=0;i<(int)m_pVertexBuffer.size();i++)
		m_pVertexBuffer[i]->Release();

	m_pVertexBuffer.clear();
	m_nVertex.clear();

	for(int i=0;i<(int)m_pIndicesBuffer.size();i++)
		m_pIndicesBuffer[i]->Release();	

	m_pIndicesBuffer.clear();
	m_nIndices.clear();

	CFileLoad FileLoader;
	FileLoader.Load(m_strMeshName);

	int nObject,nMat,TextureSize;	
	FileLoader.GetData(&nObject,sizeof(int));
	FileLoader.GetData(&nMat,sizeof(int));
	FileLoader.GetData(&TextureSize,sizeof(int));

	m_bRender=true;
	

	char strTextureName[256];
	for(int i=0;i<nMat;i++)
	{
		FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);		
		
	}
	char strObjectName[256];
	int MatRef,cVertex,cIndices;	
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	for(int i=0;i<nObject;i++)
	{
		FileLoader.GetData(strObjectName,sizeof(char)*256);
		FileLoader.GetData(&MatRef,sizeof(long));
		FileLoader.GetData(&cVertex,sizeof(long));
		FileLoader.GetData(&cIndices,sizeof(int));		

		LPDIRECT3DVERTEXBUFFER8 pAllocVB;
		LPDIRECT3DINDEXBUFFER8 pAllocIB;
		
		MultiVertex *pTempVertex=new MultiVertex[cVertex];
		WORD *pTempIndices=new WORD[cIndices*3];

		CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
										 MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
		CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

		pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
		pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);

		FileLoader.GetData(pVertexData,sizeof(MultiVertex)*cVertex);
		FileLoader.GetData(pIndicesData,sizeof(WORD)*cIndices*3);
		
		pAllocVB->Unlock();
		pAllocIB->Unlock();

		m_pVertexBuffer.push_back(pAllocVB);
		m_pIndicesBuffer.push_back(pAllocIB);		
		
		m_nVertex.push_back(cVertex);
		m_nIndices.push_back(cIndices);
		m_pMatRef.push_back(MatRef);

		delete [] pTempIndices;
		delete [] pTempVertex;
	}
	if(m_dwShader!=MultiFVF)
	{
		m_dwShader=MultiFVF;
		ConvertNormal();
	}
}
