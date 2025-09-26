// BspScene.cpp: implementation of the CBspScene class.
//
//////////////////////////////////////////////////////////////////////

#include "BspScene.h"
#include <stdio.h>
#include "SceneManager.h"
#include "RenderOption.h"
#include "GMMemory.h"

//#define	MAX_EXPANDED_AXIS		50

int	originalWidths[MAX_EXPANDED_AXIS];
int	originalHeights[MAX_EXPANDED_AXIS];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBspScene::CBspScene()
{
	m_SurfaceInc = NULL;	

	m_EyeCluster=-1;
	m_Shaders=NULL;
	m_nShader=0;
	m_DrawVerts=NULL;
	m_nDrawVerts=0;
	m_Planes=NULL;
	m_nPlanes=0;

	m_Leafs=NULL;
	m_nLeafs=0;

	m_Nodes=NULL;
	m_nNodes=0;
	
	m_DrawSurfaces=NULL;
	m_nDrawSurfaces=0;
	
	m_LeafSurfaces=NULL;
	m_nLeafSurfaces=0;
	
	m_DrawIndex=NULL;
	m_nDrawIndex=0;

	m_Visibility=NULL;
	m_nVisibility=0;	

	m_SelectLight=-1;
}

CBspScene::~CBspScene()
{	
	if(m_SurfaceInc)
	{
		delete [] m_SurfaceInc;	
		m_SurfaceInc = NULL;
	}

	for(int i=0;i<(int)m_DSurfaceEx.size();i++)
	{
		delete m_DSurfaceEx[i];
	}
	m_DSurfaceEx.clear();

	for(int i=0;i<(int)m_ShaderList.size();i++)
	{
		delete m_ShaderList[i].m_DiffuseTexture;
		delete m_ShaderList[i].m_BumpTexture;
	}
	m_ShaderList.clear();

	for(int i=0;i<m_nDrawSurfaces;i++)
	{
		free(m_SurfaceTest[i]);
		m_SurfaceTest[i] = NULL;
	}

/*
	for(int i=0;i<(int)m_CurveBufferList.size();i++)
	{
		delete [] m_CurveBufferList[i]->m_pVertex;
		delete [] m_CurveBufferList[i]->m_pIndices;
		delete [] m_CurveBufferList[i]->m_pControlPoint;		

		delete m_CurveBufferList[i];
		m_CurveBufferList[i] = NULL;
	}	

	m_CurveBufferList.clear();
*/
}

void CBspScene::Load(char *strFilename)
{
	DHeader *pHeader;
	FILE *fp;
	int nLength;
	void *pBuffer;
	fp=fopen(strFilename,"rb");

	fseek(fp,0,SEEK_END);
	nLength=ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuffer=new char[nLength+1];

	((char *)pBuffer)[nLength]=0;
	fread(pBuffer,nLength,1,fp);
	fclose(fp);
	pHeader=(DHeader*)pBuffer;	
	
	m_nShader=CopyLump(pHeader,LUMP_SHADERS,(void**)&m_Shaders,sizeof(*(m_Shaders)));
	m_nPlanes=CopyLump(pHeader,LUMP_PLANES,(void**)&m_Planes,sizeof(*(m_Planes)));
	m_nNodes=CopyLump(pHeader,LUMP_NODES,(void**)&m_Nodes,sizeof(*(m_Nodes)));
	m_nLeafs=CopyLump(pHeader,LUMP_LEAFS,(void**)&m_Leafs,sizeof(*(m_Leafs)));
	m_nLeafSurfaces=CopyLump(pHeader,LUMP_LEAFSURFACES,(void**)&m_LeafSurfaces,sizeof(m_LeafSurfaces[0]));//LFaces
	m_nDrawVerts=CopyLump(pHeader,LUMP_DRAWVERTS,(void**)&m_DrawVerts,sizeof(BspVertex));
	m_nDrawSurfaces=CopyLump(pHeader,LUMP_SURFACES,(void**)&m_DrawSurfaces,sizeof(DSurface));
	m_nDrawIndex=CopyLump(pHeader,LUMP_DRAWINDEXES,(void**)&m_DrawIndex,sizeof(m_DrawIndex[0]));
	m_nVisibility=CopyLump(pHeader,LUMP_VISIBILITY,(void**)&m_Visibility,1);
	m_nLight=CopyLump(pHeader,LUMP_LIGHT,(void**)&m_Light,sizeof(DLight));	

	float fTemp;

	for(int cLight=0;cLight<m_nLight;cLight++)
	{
		fTemp=m_Light[cLight].m_vecLightPos.y;
		m_Light[cLight].m_vecLightPos.y=m_Light[cLight].m_vecLightPos.z;
		m_Light[cLight].m_vecLightPos.z=fTemp;		
	}

	m_vecMinBox=m_vecMaxBox=m_DrawVerts[0].v;

	for(int cVertex=0;cVertex<m_nDrawVerts;cVertex++)
	{
		m_DrawVerts[cVertex].diff.r=rand()%255;
		m_DrawVerts[cVertex].diff.g=rand()%255;
		m_DrawVerts[cVertex].diff.b=rand()%255;

		if(m_DrawVerts[cVertex].v.x >= m_vecMaxBox.x)
			m_vecMaxBox.x=m_DrawVerts[cVertex].v.x;
		if(m_DrawVerts[cVertex].v.y >= m_vecMaxBox.y)
			m_vecMaxBox.y=m_DrawVerts[cVertex].v.y;
		if(m_DrawVerts[cVertex].v.z >= m_vecMaxBox.z)
			m_vecMaxBox.z=m_DrawVerts[cVertex].v.z;

		if(m_DrawVerts[cVertex].v.x <= m_vecMinBox.x)
			m_vecMinBox.x=m_DrawVerts[cVertex].v.x;
		if(m_DrawVerts[cVertex].v.y <= m_vecMinBox.y)
			m_vecMinBox.y=m_DrawVerts[cVertex].v.y;
		if(m_DrawVerts[cVertex].v.z <= m_vecMinBox.z)
			m_vecMinBox.z=m_DrawVerts[cVertex].v.z;
	}	
	m_vecMax=m_vecMaxBox;
	m_vecMax.y=m_vecMaxBox.z;
	m_vecMax.z=m_vecMaxBox.y;

	m_vecMin=m_vecMinBox;
	m_vecMin.y=m_vecMin.z;
	m_vecMin.z=m_vecMin.y;
	
	m_vecMaxBox=m_vecMax;
	m_vecMinBox=m_vecMin;

	int nLightmap=0;
	for(int cDrawSurface=0;cDrawSurface<m_nDrawSurfaces;cDrawSurface++)
	{
		if(nLightmap < m_DrawSurfaces[cDrawSurface].m_nLightmap)
		{
			nLightmap=m_DrawSurfaces[cDrawSurface].m_nLightmap;
		}		
	}

	m_nLightmap=nLightmap+1;

	m_bLightmapVector=false;
	delete [] pBuffer;	
	InitTrace();
	RenderInit();
}

int CBspScene::CopyLump(DHeader *header, int lump, void **dest, int size)
{
	int len=header->m_Lump[lump].m_Filelen;
	*dest=malloc(len);

	int ofs=header->m_Lump[lump].m_Fileofs;
	memcpy(*dest,(BYTE*)header+ofs,len);

	return len/size;	
}

void CBspScene::Render(LPDIRECT3DDEVICE8 pd3dDevice, vector3 vecViewPos)
{
	//vecViewPos=vector3(56,32,32);
	
	m_Camera.BuildFrustum(CSceneManager::GetCamera()->m_fAspect,3.14159f/3.0f,CSceneManager::GetCamera()->m_fNearPlane,CSceneManager::GetCamera()->m_fFarPlane);

	matrix matHousePos;
	pd3dDevice->GetTransform(D3DTS_WORLD,matHousePos);				
	matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();
	
	matrix matInvTM,matPos;
	matInvTM.Inverse(matHousePos);

	matPos=(*matViewPosition)*matInvTM;

	m_Camera.m_matPosition=matPos;
	m_Camera.MoveFrustum();

	m_vecViewPos=vecViewPos;

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0,  D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	pd3dDevice->SetTextureStageState( 1,  D3DTSS_COLOROP,   D3DTOP_DISABLE);		

    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,0xffdddddd);
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED );		
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);			

	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);    
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE4X);
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
    

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);

	m_EyeCluster=FindCluster(vecViewPos);

	memset(m_SurfaceInc,0,sizeof(int)*m_nDrawSurfaces);
	//memset(m_RenderSurfacesList.nSurfaces,0,sizeof(int)*100);	

	m_bCull=true;

	RenderWalkNode(0);	
	RenderBackend(pd3dDevice);
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	RenderLight(pd3dDevice);
	
	//if(m_bLightmapVector)
	//	RenderLight(pd3dDevice);	
}

int CBspScene::FindCluster(vector3 vecPos)
{
	DNode *pNode;

	int nCluster=-1;
	int nLeaf=-1;

	pNode=&m_Nodes[0];
	
	while(1)
	{
		if(ClassfyPoint(vecPos,pNode->m_nPlane)>0)
		{
			if(pNode->m_Children[0] < 0)
			{
				nLeaf=-(pNode->m_Children[0]+1);
				break;
			}
			else
			{
				pNode=&m_Nodes[pNode->m_Children[0]];
			}
		}
		else
		{
			if(pNode->m_Children[1]<0)
			{
				nLeaf=-(pNode->m_Children[1]+1);
				break;
			}
			else
			{
				pNode=&m_Nodes[pNode->m_Children[1]];
			}
		}
	}
	if(nLeaf>=0)
		nCluster=m_Leafs[nLeaf].m_Cluster;
	return nCluster;	
}

int CBspScene::ClassfyPoint(vector3 p, int nPlane)
{
	DPlane *plane=&m_Planes[nPlane];	
	vector3 test;
	test.x=plane->m_vecNormal.x;
	test.z=plane->m_vecNormal.y;
	test.y=plane->m_vecNormal.z;
	
	if( (p*test) < plane->m_fDist)
		return -1;
	else 
		return 1;
	return -1;
}

void CBspScene::RenderWalkNode(int n)
{
	DNode *pNode=&m_Nodes[n];
	
	/*

	vector3 m_MinBox;
	
	m_MinBox.x=pNode->Min[0];
	m_MinBox.y=pNode->Min[2];
	m_MinBox.z=pNode->Min[1];	
	
	vector3 m_MaxBox;
	
	m_MaxBox.x=pNode->Max[0];
	m_MaxBox.y=pNode->Max[2];
	m_MaxBox.z=pNode->Max[1];	
	if( m_MinBox.x <= m_vecViewPos.x && m_vecViewPos.x <= m_MaxBox.x &&
		m_MinBox.y <= m_vecViewPos.y && m_vecViewPos.y <= m_MaxBox.y &&
		m_MinBox.z <= m_vecViewPos.z && m_vecViewPos.z <= m_MaxBox.z)
		*/
	
	{
	
		if(ClassfyPoint(m_vecViewPos,pNode->m_nPlane)>0)
		{
			if(pNode->m_Children[0]<0)
				RenderWalkLeaf(-(pNode->m_Children[0]+1));
			else 
				RenderWalkNode(pNode->m_Children[0]);
			if(pNode->m_Children[1]<0)
				RenderWalkLeaf(-(pNode->m_Children[1]+1));
			else
				RenderWalkNode(pNode->m_Children[1]);
		}
		else
		{
			if(pNode->m_Children[1]<0)
				RenderWalkLeaf(-(pNode->m_Children[1]+1));
			else
				RenderWalkNode(pNode->m_Children[1]);

			if(pNode->m_Children[0]<0)
				RenderWalkLeaf(-(pNode->m_Children[0]+1));
			else
				RenderWalkNode(pNode->m_Children[0]);
		}	
	}
}

#define BSP_TESTVIS(from,to) \
		(*(m_Visibility->data+(from)*m_Visibility->RowSize+\
		((to)>>3)) & (1<<((to)&7)))

void CBspScene::RenderWalkLeaf(int n)
{
	DLeaf *pLeaf=&m_Leafs[n];
	
	if(m_EyeCluster >= 0)
	{
		if(!BSP_TESTVIS(m_EyeCluster,pLeaf->m_Cluster))
			return;
	}
	
	CViewCamera *pCamera=&m_Camera;
	/*	
	*/

	vector3 m_MinBox;	
	m_MinBox.x=pLeaf->m_min.x;
	m_MinBox.z=pLeaf->m_min.y;
	m_MinBox.y=pLeaf->m_min.z;
	
	vector3 m_MaxBox;	
	m_MaxBox.x=pLeaf->m_max.x;	
	m_MaxBox.z=pLeaf->m_max.y;	
	m_MaxBox.y=pLeaf->m_max.z;		
	
	if(m_bCull)
	{
		
		if( CIntersection::PlaneAABBBox(pCamera->m_vecFrustumTop,-pCamera->m_vecFrustumNormal[0],m_MaxBox,m_MinBox) < 0.0f &&
			CIntersection::PlaneAABBBox(pCamera->m_vecFrustumBottom,-pCamera->m_vecFrustumNormal[1],m_MaxBox,m_MinBox) < 0.0f &&
			CIntersection::PlaneAABBBox(pCamera->m_vecFrustumLeft,-pCamera->m_vecFrustumNormal[2],m_MaxBox,m_MinBox) < 0.0f &&
			CIntersection::PlaneAABBBox(pCamera->m_vecFrustumRight,-pCamera->m_vecFrustumNormal[3],m_MaxBox,m_MinBox) < 0.0f)
		
		{
		
			for(int i=0;i<pLeaf->m_nLeafSurfaces;i++)
			{
				RenderWalkFace(m_LeafSurfaces[i+pLeaf->m_FirstLeafSurface]);
			}
		}
		else
		{
			int a=0;
		}
	}
	else
	{
		for(int i=0;i<pLeaf->m_nLeafSurfaces;i++)
		{
			RenderWalkFace(m_LeafSurfaces[i+pLeaf->m_FirstLeafSurface]);
		}
	}
	
}

void CBspScene::RenderWalkFace(int n)
{
	DSurface *pSurface=&m_DrawSurfaces[n];
	if(m_SurfaceInc[n]>0)
		return;
	

	CViewCamera *pCamera=&m_Camera;	

	vector3 m_MinBox;	
	vector3 m_MaxBox;	

	m_MinBox=m_DSurfaceEx[n]->m_vecMin;
	m_MaxBox=m_DSurfaceEx[n]->m_vecMax;				
	
	if( CIntersection::PlaneAABBBox(pCamera->m_vecFrustumTop,-pCamera->m_vecFrustumNormal[0],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumBottom,-pCamera->m_vecFrustumNormal[1],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumLeft,-pCamera->m_vecFrustumNormal[2],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumRight,-pCamera->m_vecFrustumNormal[3],m_MaxBox,m_MinBox) < 0.0f)
	{
		m_SurfaceInc[n]=1;
	}
	else
	{
		m_SurfaceInc[n]=2;
	}

	
	if(pSurface->m_nLightmap<0)
		return;
	if(pSurface->m_nShader<0)
		return;
	
	/*
	m_RenderSurfacesList.pDrawSurfaces[pSurface->m_nShader][pSurface->m_nLightmap]
		[m_RenderSurfacesList.nSurfaces[pSurface->m_nShader][pSurface->m_nLightmap]]=n;
	m_RenderSurfacesList.nSurfaces[pSurface->m_nShader][pSurface->m_nLightmap]++;		
	*/
	
}

void CBspScene::RenderInit()
{		
	CTexture::SetPath("c:\\MP-Project\\Texture\\BSP");

	m_SurfaceInc=new int[m_nDrawSurfaces];

	int cSurface=0;	

	for(cSurface=0;cSurface<m_nDrawSurfaces;cSurface++)
	{
		DSurfaceEx *AddNode=new DSurfaceEx;
		AddNode->m_pSurface=&m_DrawSurfaces[cSurface];
		AddNode->m_pShadowVertexBuffer=NULL;

		AddNode->m_vecMax=m_DrawVerts[AddNode->m_pSurface->m_FirstVert].v;
		AddNode->m_vecMin=m_DrawVerts[AddNode->m_pSurface->m_FirstVert].v;

		for(int cVertex=0;cVertex<AddNode->m_pSurface->m_nVerts;cVertex++)
		{
			if(AddNode->m_vecMax.x <= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.x)
				AddNode->m_vecMax.x=m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.x;

			if(AddNode->m_vecMax.y <= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.y)
				AddNode->m_vecMax.y=m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.y;

			if(AddNode->m_vecMax.z <= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.z)
				AddNode->m_vecMax.z=m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.z;

			if(AddNode->m_vecMin.x >= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.x)
				AddNode->m_vecMin.x = m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.x;

			if(AddNode->m_vecMin.y >= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.y)
				AddNode->m_vecMin.y = m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.y;

			if(AddNode->m_vecMin.z >= m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.z)
				AddNode->m_vecMin.z = m_DrawVerts[AddNode->m_pSurface->m_FirstVert+cVertex].v.z;
		}

		vector3 vecTemp;
		vecTemp=AddNode->m_vecMax;
		AddNode->m_vecMax.y=vecTemp.z;
		AddNode->m_vecMax.z=vecTemp.y;

		vecTemp=AddNode->m_vecMin;
		AddNode->m_vecMin.y=vecTemp.z;
		AddNode->m_vecMin.z=vecTemp.y;		

		DSurface *pSurface=&m_DrawSurfaces[cSurface];
		
		//CalcShadowVolume(&m_DrawVerts[pSurface->m_FirstVert],pSurface->m_nVerts,
		//	&m_DrawIndex[pSurface->m_FirstIndex],pSurface->m_nIndex,AddNode);


		m_DSurfaceEx.push_back(AddNode);

		//CalcShadowVolume(&m_DrawVerts[pSurface->m_FirstVert],pSurface->m_nVerts,
					//&m_DrawIndex[pSurface->m_FirstIndex],pSurface->m_nIndex,m_DSurfaceEx[cSurface]);		
		
		CalcShadowVolume(&m_DrawVerts[pSurface->m_FirstVert],pSurface->m_nVerts,
			&m_DrawIndex[pSurface->m_FirstIndex],pSurface->m_nIndex,*m_DSurfaceEx[cSurface]);	

	}

	BspShadowVertex *pVertexData;

	CSceneManager::GetDevice()->CreateVertexBuffer((int)m_SurfaceEdgeVertexList.size()*sizeof(BspShadowVertex),
		D3DUSAGE_WRITEONLY,BSPSHADOWVERTEX,D3DPOOL_MANAGED,&m_pEdgeVertexBuffer);
	
	m_pEdgeVertexBuffer->Lock(0,0,(BYTE**)&pVertexData,0);		

	memcpy(pVertexData,&m_SurfaceEdgeVertexList[0],sizeof(BspShadowVertex)*(int)m_SurfaceEdgeVertexList.size());
	m_pEdgeVertexBuffer->Unlock();

		

	
	for(int i=0;i<m_nShader;i++)
	{
		CTexture *pDiffuseTexture=new CTexture();
		pDiffuseTexture->Load(m_Shaders[i].m_strTextureName);		

		CTexture *pBumpTexture=new CTexture();
		char strBumpTexture[256];
		strcpy(strBumpTexture,m_Shaders[i].m_strTextureName);
		int nLens=strlen(strBumpTexture);
		int cSurface=0;

		strBumpTexture[nLens-4]='_';
		strBumpTexture[nLens-3]='b';
		strBumpTexture[nLens-2]='u';
		strBumpTexture[nLens-1]='m';
		strBumpTexture[nLens-0]='p';
		strBumpTexture[nLens+1]='.';
		strBumpTexture[nLens+2]='d';
		strBumpTexture[nLens+3]='d';
		strBumpTexture[nLens+4]='s';
		strBumpTexture[nLens+5]=0;

		pBumpTexture->Load(strBumpTexture);

		CBSPShader AddShader;		
		AddShader.m_DiffuseTexture=pDiffuseTexture;
		AddShader.m_BumpTexture=pBumpTexture;
		AddShader.m_nPoly=0;
		AddShader.m_pIndicesBuffer=NULL;
		AddShader.m_pVertexBuffer=NULL;
		
		AddShader.m_nVertex=0;
		AddShader.m_nIndices=0;

		DSurface *pSurface=NULL;		

		for( cSurface=0;cSurface<m_nDrawSurfaces;cSurface++)
		{
			pSurface=&m_DrawSurfaces[cSurface];
			if(i==pSurface->m_nShader)
			{
				AddShader.m_nVertex+=pSurface->m_nVerts;
				AddShader.m_nIndices+=pSurface->m_nIndex;
			}
		}

		CSceneManager::GetDevice()->CreateVertexBuffer(AddShader.m_nVertex*sizeof(BspVertex),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,BSPVERTEXFVF,D3DPOOL_DEFAULT,&AddShader.m_pVertexBuffer);
		CSceneManager::GetDevice()->CreateIndexBuffer(AddShader.m_nIndices*sizeof(WORD),D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&AddShader.m_pIndicesBuffer);

		BspVertex *pVertex;

		AddShader.m_pVertexBuffer->Lock(0,0,(BYTE**)&pVertex,D3DLOCK_DISCARD);		

		int nTotalVertex=0;

		for(cSurface=0;cSurface<m_nDrawSurfaces;cSurface++)
		{
			pSurface=&m_DrawSurfaces[cSurface];

			if(i==pSurface->m_nShader)
			{		
				//CalcShadowVolume(&m_DrawVerts[pSurface->m_FirstVert],pSurface->m_nVerts,
					//&m_DrawIndex[pSurface->m_FirstIndex],pSurface->m_nIndex,m_DSurfaceEx[cSurface]);

				for(int cIndices=0;cIndices<pSurface->m_nIndex;cIndices++)
				{
					m_DrawIndex[pSurface->m_FirstIndex+cIndices]=nTotalVertex+m_DrawIndex[pSurface->m_FirstIndex+cIndices];
				}				
				for(int cVertex=0;cVertex<pSurface->m_nVerts;cVertex++)
				{
					pVertex[nTotalVertex].v.x=m_DrawVerts[pSurface->m_FirstVert+cVertex].v.x;
					pVertex[nTotalVertex].v.y=m_DrawVerts[pSurface->m_FirstVert+cVertex].v.z;
					pVertex[nTotalVertex].v.z=m_DrawVerts[pSurface->m_FirstVert+cVertex].v.y;

					pVertex[nTotalVertex].normal.x=m_DrawVerts[pSurface->m_FirstVert+cVertex].normal.x;
					pVertex[nTotalVertex].normal.y=m_DrawVerts[pSurface->m_FirstVert+cVertex].normal.z;
					pVertex[nTotalVertex].normal.z=m_DrawVerts[pSurface->m_FirstVert+cVertex].normal.y;

					pVertex[nTotalVertex].tu=m_DrawVerts[pSurface->m_FirstVert+cVertex].tu;
					pVertex[nTotalVertex].tv=m_DrawVerts[pSurface->m_FirstVert+cVertex].tv;

					pVertex[nTotalVertex].tu1=m_DrawVerts[pSurface->m_FirstVert+cVertex].tu1;
					pVertex[nTotalVertex].tv1=m_DrawVerts[pSurface->m_FirstVert+cVertex].tv1;

					pVertex[nTotalVertex].diff.c=0x0;
					pVertex[nTotalVertex].u.x=m_DrawVerts[pSurface->m_FirstVert+cVertex].u.x;
					pVertex[nTotalVertex].u.y=m_DrawVerts[pSurface->m_FirstVert+cVertex].u.z;
					pVertex[nTotalVertex].u.z=m_DrawVerts[pSurface->m_FirstVert+cVertex].u.y;
					nTotalVertex++;
				}				
			}
		}	
		AddShader.m_pVertexBuffer->Unlock();

		AddShader.m_nDrawIndices=0;
		AddShader.m_pIndices=NULL;
		m_ShaderList.push_back(AddShader);
	}			
	InitShader();
	SurfaceLightArrange();
}

void CBspScene::RenderBackend(LPDIRECT3DDEVICE8 pd3dDevice)
{
	DSurface *pSurface;
	//WORD *pIndices;
	//BspVertex *pVertex;
	

	std::vector<DSurface*> CurveRenderList;

	bool bSkyRender=false;
	m_bSkyRender=false;	

	int nRenderSurface=0;	
	int nRenderPoly=0;	

	std::vector<int> LightRenderList;

	for(int nSurface=0;nSurface<m_nDrawSurfaces;nSurface++)
	{
		if(m_SurfaceInc[nSurface]!=1)
			continue;

		pSurface=&m_DrawSurfaces[nSurface];

		if(m_ShaderList[pSurface->m_nShader].m_nDrawIndices==0)
		{
			m_ShaderList[pSurface->m_nShader].m_pIndicesBuffer->Lock(0,
				m_ShaderList[pSurface->m_nShader].m_nIndices*sizeof(WORD),
				(BYTE**)&m_ShaderList[pSurface->m_nShader].m_pIndices,D3DLOCK_DISCARD);
		}		

		for(int cIndices=0;cIndices<pSurface->m_nIndex;cIndices++)
		{
			m_ShaderList[pSurface->m_nShader].m_pIndices[m_ShaderList[pSurface->m_nShader].m_nDrawIndices]
				=m_DrawIndex[pSurface->m_FirstIndex+cIndices];
			m_ShaderList[pSurface->m_nShader].m_nDrawIndices++;
		}		
		nRenderPoly+=pSurface->m_nIndex/3;
		nRenderSurface++;

		for(int cLight=0;cLight<(int)m_DSurfaceEx[nSurface]->m_ApplyLightList.size();cLight++)
		{
			// edith AddUnique 형식 구현
			int iCnt = 0;
			for(int a=0;a<(int)LightRenderList.size();a++)
			{
				if(LightRenderList[a] == m_DSurfaceEx[nSurface]->m_ApplyLightList[cLight])
					iCnt++;
			}
			if(iCnt==0)
				LightRenderList.push_back(m_DSurfaceEx[nSurface]->m_ApplyLightList[cLight]);


			iCnt = 0;
			for(int a=0;a<(int)m_ShaderList[pSurface->m_nShader].m_ApplyLightList.size();a++)
			{
				if(m_ShaderList[pSurface->m_nShader].m_ApplyLightList[a] == m_DSurfaceEx[nSurface]->m_ApplyLightList[cLight])
					iCnt++;
			}
			if(iCnt==0)
				m_ShaderList[pSurface->m_nShader].m_ApplyLightList.push_back(m_DSurfaceEx[nSurface]->m_ApplyLightList[cLight]);
		}		
	}	


	for(int cShader=0;cShader<(int)m_ShaderList.size();cShader++)
	{
		if(m_ShaderList[cShader].m_nDrawIndices)
			m_ShaderList[cShader].m_pIndicesBuffer->Unlock();
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	matrix matView,matProjection,matWorld,matWVP;
	pd3dDevice->GetTransform(D3DTS_VIEW,matView);
	pd3dDevice->GetTransform(D3DTS_PROJECTION,matProjection);				
	pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);
	matWVP=matWorld*matView*matProjection;
	matWVP.Transpose();				

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);		
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	for(int cShader=0;cShader<(int)m_ShaderList.size();cShader++)
	{		
		if(m_ShaderList[cShader].m_nDrawIndices==0)
			continue;

		pd3dDevice->SetTexture(0,m_ShaderList[cShader].m_DiffuseTexture->GetTexture());		
		pd3dDevice->SetVertexShaderConstant(16,&matWVP, 4);		 		
		pd3dDevice->SetVertexShader(m_dwBSPAmbientVertexShader);
		pd3dDevice->SetPixelShader(m_dwBSPAmbientPixelShader);
		
		pd3dDevice->SetStreamSource(0,m_ShaderList[cShader].m_pVertexBuffer,sizeof(BspVertex));
		pd3dDevice->SetIndices(m_ShaderList[cShader].m_pIndicesBuffer,0);

		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_ShaderList[cShader].m_nVertex,0,m_ShaderList[cShader].m_nDrawIndices/3);
	}	

	for(int cLight=0;cLight<(int)LightRenderList.size();cLight++)
	//for(int cLight=0;cLight<1;cLight++)
	{
		pd3dDevice->Clear( 0L, NULL, D3DCLEAR_STENCIL , 0x000000ff, 1.0f, 0x80L );

		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);			
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		pd3dDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
		pd3dDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP );
		pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
		
		pd3dDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
		pd3dDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );			
			
		pd3dDevice->SetPixelShader(NULL);

		pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);		
		pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff0000);

		DLightEx *pLight=&m_LightEx[LightRenderList[cLight]];			

		vector3 vecLightPosition=pLight->m_pLight->m_vecLightPos;			
		static float fRate=0.0f;
		fRate+=0.05f;
		/*
		vecLightPosition.x+=sinf(fRate)*100.0f;
		vecLightPosition.z+=cosf(fRate)*100.0f;
		vecLightPosition.y+=cosf(fRate)*100.0f;	
		*/
		

		float fLightPos[4]={vecLightPosition.x,vecLightPosition.y,vecLightPosition.z,1.0f};

		pd3dDevice->SetVertexShaderConstant(1,&matWVP, 4);			
		pd3dDevice->SetVertexShaderConstant(10,fLightPos,1);		
		pd3dDevice->SetVertexShader(m_dwBSPShadowVertexShader);
			
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		//pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			
		pd3dDevice->SetStreamSource(0,m_pEdgeVertexBuffer,sizeof(BspShadowVertex));
		pd3dDevice->SetIndices(pLight->m_pEdgeIndexBuffer,0);		
		///*
		
		pd3dDevice->SetRenderState( D3DRS_STENCILPASS,D3DSTENCILOP_INCR);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,(int)m_SurfaceEdgeVertexList.size(),0,pLight->m_nEdgePoly/3);

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);

		pd3dDevice->SetRenderState( D3DRS_STENCILPASS,D3DSTENCILOP_DECR);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,(int)m_SurfaceEdgeVertexList.size(),0,pLight->m_nEdgePoly/3);				
		//*/

		//////////////////////////////
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);  			
		pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
		pd3dDevice->SetRenderState( D3DRS_STENCILREF,  0x80 );			

		pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pd3dDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
		pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);



		TraceT trace;
		TraceWorkT tw;	
		vector3 vecCheckViewPos;
		vecCheckViewPos.x=m_vecViewPos.x;			
		vecCheckViewPos.y=m_vecViewPos.z;			
		vecCheckViewPos.z=m_vecViewPos.y;

		vector3 vecCheckLightPos;
		vecCheckLightPos.x=vecLightPosition.x;
		vecCheckLightPos.y=vecLightPosition.z;
		vecCheckLightPos.z=vecLightPosition.y;

		TraceLine(vecCheckViewPos,vecCheckLightPos,&trace,false,&tw);
		if(trace.m_bPassSolid)
			pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_GREATER);
		else
			pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL);	

		float fObjectSpaceLightPosition[4]={vecLightPosition.x,vecLightPosition.y,vecLightPosition.z,1.0f};				
		float fLightSettingValue[4]={1.0f/4000.0f,0.0f,0.0f,0.0f};
		float fObjectSpaceViewPosition[4]={m_Camera.m_matPosition.GetLoc().x,m_Camera.m_matPosition.GetLoc().y,m_Camera.m_matPosition.GetLoc().z,0.0f};		
		float fLightPosAdder[4]={0.5f,0.5f,0.5f,0.5f};


		pd3dDevice->SetVertexShaderConstant(16,&matWVP, 4);		 
		pd3dDevice->SetVertexShaderConstant(32,fObjectSpaceLightPosition,1);
		pd3dDevice->SetVertexShaderConstant(33,fLightSettingValue,1);
		pd3dDevice->SetVertexShaderConstant(35,fObjectSpaceViewPosition,1);
		pd3dDevice->SetVertexShaderConstant(36,fLightPosAdder,1);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);	
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);		
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);	

		for(int cShader=0;cShader<(int)m_ShaderList.size();cShader++)
		{
			if(m_ShaderList[cShader].m_nDrawIndices==0)
				continue;

			int iCnt = 0;
			for(int a=0;a<(int)m_ShaderList[cShader].m_ApplyLightList.size();a++)
			{
				if(m_ShaderList[cShader].m_ApplyLightList[a]==LightRenderList[cLight])
					iCnt++;
			}
			if(iCnt==0)
				continue;

			///////////Diffuse //////////////
			pd3dDevice->SetTexture(0,m_pPointFalloffMap);
			pd3dDevice->SetTexture(1,m_ShaderList[cShader].m_BumpTexture->GetTexture());
			pd3dDevice->SetTexture(2,m_pNormalizeCubeMap);
			pd3dDevice->SetTexture(3,m_ShaderList[cShader].m_DiffuseTexture->GetTexture());

			pd3dDevice->SetVertexShader(m_dwBSPDiffuseVertexShader);
			pd3dDevice->SetPixelShader(m_dwBSPDiffusePixelShader);

			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_MIRROR);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_MIRROR);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSW,D3DTADDRESS_MIRROR);

			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSW,D3DTADDRESS_CLAMP);

			pd3dDevice->SetStreamSource(0,m_ShaderList[cShader].m_pVertexBuffer,sizeof(BspVertex));
			pd3dDevice->SetIndices(m_ShaderList[cShader].m_pIndicesBuffer,0);

			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_ShaderList[cShader].m_nVertex,0,m_ShaderList[cShader].m_nDrawIndices/3);
			///////////Specluar/////////////
			

			pd3dDevice->SetTexture(0,m_ShaderList[cShader].m_BumpTexture->GetTexture());
			pd3dDevice->SetTexture(1,m_pPointFalloffMap);
			pd3dDevice->SetTexture(2,m_pNormalizeCubeMap);
			pd3dDevice->SetTexture(3,m_pNormalizeCubeMap);			

			pd3dDevice->SetVertexShader(m_dwBSPSpecularVertexShader);
			pd3dDevice->SetPixelShader(m_dwBSPSpecularPixelShader);		

			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSU,D3DTADDRESS_MIRROR);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSV,D3DTADDRESS_MIRROR);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSW,D3DTADDRESS_MIRROR);

			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSW,D3DTADDRESS_CLAMP);

			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSW,D3DTADDRESS_CLAMP);		
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_ShaderList[cShader].m_nVertex,0,m_ShaderList[cShader].m_nDrawIndices/3);			
			

			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(0,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(1,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(2,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);

			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
			pd3dDevice->SetTextureStageState(3,D3DTSS_ADDRESSW,D3DTADDRESS_WRAP);					
		}	
	
	}
	
	for(int cShader=0;cShader<(int)m_ShaderList.size();cShader++)
	{
		m_ShaderList[cShader].m_ApplyLightList.clear();
		m_ShaderList[cShader].m_nDrawIndices=0;		
	}

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);			
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);			

}

bool CBspScene::LeafInFrustum(vector3 &vecMax, vector3 &vecMin)
{	
	return true;
}

void CBspScene::CullPoly(vector3 vecMax, vector3 vecMin,std::vector<vector3> &PolyList)
{	
	vector3 vecCenter=(vecMax+vecMin)/2.0f;
	m_EyeCluster=FindCluster(vecCenter);
	memset(m_SurfaceInc,0,sizeof(int)*m_nDrawSurfaces);

	m_vecCenter=vecCenter;
	m_vecMax=vecMax;
	m_vecMin=vecMin;
	CullWalkNodes(0);	
}

void CBspScene::CullWalkNodes(int n)
{
	DNode *pNode=&m_Nodes[n];
	
	vector3 MinNodeBox;
	vector3 MaxNodeBox;	

	MinNodeBox.x=pNode->m_min.x;	
	MinNodeBox.y=pNode->m_min.z;	
	MinNodeBox.z=pNode->m_min.y;	

	MaxNodeBox.x=pNode->m_max.x;	
	MaxNodeBox.y=pNode->m_max.z;	
	MaxNodeBox.z=pNode->m_max.y;	

	if( m_vecMin.x >= MaxNodeBox.x ||
		m_vecMax.x <= MinNodeBox.x ||
		m_vecMin.y >= MaxNodeBox.y ||
		m_vecMax.y <= MinNodeBox.y ||
		m_vecMin.z >= MaxNodeBox.z ||
		m_vecMax.z <= MinNodeBox.z )
	{
		return;
	}		
	if(ClassfyPoint(m_vecCenter,pNode->m_nPlane)>0)
	{
		if(pNode->m_Children[0]<0)
			CullWalkLeaf(-(pNode->m_Children[0]+1));
		else 
			CullWalkNodes(pNode->m_Children[0]);

		if(pNode->m_Children[1]<0)
			CullWalkLeaf(-(pNode->m_Children[1]+1));
		else
			CullWalkNodes(pNode->m_Children[1]);
	}
	else
	{
		if(pNode->m_Children[1]<0)
			CullWalkLeaf(-(pNode->m_Children[1]+1));
		else
			CullWalkNodes(pNode->m_Children[1]);

		if(pNode->m_Children[0]<0)
			CullWalkLeaf(-(pNode->m_Children[0]+1));
		else
			CullWalkNodes(pNode->m_Children[0]);
	}	
}

void CBspScene::CullWalkLeaf(int n)
{
	DLeaf *pLeaf=&m_Leafs[n];
	
	if(m_EyeCluster >= 0)
	{
		if(!BSP_TESTVIS(m_EyeCluster,pLeaf->m_Cluster))
			return;
	}	
	vector3 MinLeafBox;	
	vector3 MaxLeafBox;
	MinLeafBox.x=pLeaf->m_min.x;	
	MinLeafBox.y=pLeaf->m_min.z;	
	MinLeafBox.z=pLeaf->m_min.y;	
	MaxLeafBox.x=pLeaf->m_max.x;
	MaxLeafBox.y=pLeaf->m_max.z;
	MaxLeafBox.z=pLeaf->m_max.y;
	

	if( m_vecMin.x >= MaxLeafBox.x ||
		m_vecMax.x <= MinLeafBox.x ||
		m_vecMin.y >= MaxLeafBox.y ||
		m_vecMax.y <= MinLeafBox.y ||
		m_vecMin.z >= MaxLeafBox.z ||
		m_vecMax.z <= MinLeafBox.z )
	{
		return;
	}	
	int nSurface;
	for(int i=0;i<pLeaf->m_nLeafSurfaces;i++)
	{
		nSurface=m_LeafSurfaces[i+pLeaf->m_FirstLeafSurface];
		if(m_SurfaceInc[nSurface])
		{			
			continue;
		}
		if( m_vecMin.x >= m_pSurfaceMaxBox[nSurface].x ||
			m_vecMax.x <= m_pSurfaceMinBox[nSurface].x ||
			m_vecMin.y >= m_pSurfaceMaxBox[nSurface].z ||
			m_vecMax.y <= m_pSurfaceMinBox[nSurface].z ||
			m_vecMin.z >= m_pSurfaceMaxBox[nSurface].y ||
			m_vecMax.z <= m_pSurfaceMinBox[nSurface].y )		
		{
			m_SurfaceInc[nSurface]=2;
		}
		else
		{
			m_SurfaceInc[nSurface]=1;
		}
		
		//RenderWalkFace(m_LeafSurfaces[i+pLeaf->m_FirstLeafSurface]);
	}
}



void CBspScene::CurveBuffer::CreateCurveBuffer(DSurface *pSurface, BspVertex *pControlPointVertex)
{
	//pSurface->
	m_pSurface=pSurface;
	m_pControlPoint=new vector3[m_pSurface->m_nVerts];

	for(int i=0;i<m_pSurface->m_nVerts;i++)
	{
		m_pControlPoint[i]=pControlPointVertex[m_pSurface->m_FirstVert+i].v;
	}	
	FindSize();	
	
	int len=m_PatchSizeWidth*m_PatchSizeHeight;
	vector3 *Point=new vector3[len];
	vector2 *TextureUV=new vector2[len];
	vector2 *LightmapUV=new vector2[len];
	m_pVertex=new BspVertex[len];
	
	//m_pVertex=new BspVertex[len];

	int StepWidth,StepHeight;
	StepWidth=(m_PatchSizeWidth-1)/(m_pSurface->m_PatchWidth-1);
	StepHeight=(m_PatchSizeHeight-1)/(m_pSurface->m_PatchHeight-1);

	BspVertex *pVert=pControlPointVertex+m_pSurface->m_FirstVert;
	for(int v=0;v<m_PatchSizeHeight;v+=StepHeight)
	{
		for(int u=0;u<m_PatchSizeWidth;u+=StepWidth)
		{
			int p=v*m_PatchSizeWidth+u;
			Point[p]=pVert->v;
			TextureUV[p][0]=pVert->tu;
			TextureUV[p][1]=pVert->tv;
			LightmapUV[p][0]=pVert->tu1;
			LightmapUV[p][1]=pVert->tv1;
			pVert++;
		}
	}	
	int CP[2],Size[2];
	CP[0]=m_pSurface->m_PatchWidth;
	CP[1]=m_pSurface->m_PatchHeight;
	Size[0]=m_PatchSizeWidth;
	Size[1]=m_PatchSizeHeight;

	FillPatch3(CP,Size,Point);	
	FillPatch2(CP,Size,TextureUV);
	FillPatch2(CP,Size,LightmapUV);
	for(int i=0;i<len;i++)
	{
		m_pVertex[i].v.x=Point[i].x;
		m_pVertex[i].v.y=Point[i].z;
		m_pVertex[i].v.z=Point[i].y;
		m_pVertex[i].tu=TextureUV[i][0];
		m_pVertex[i].tv=TextureUV[i][1];
		m_pVertex[i].tu1=LightmapUV[i][0];	
		m_pVertex[i].tv1=LightmapUV[i][1];
	}

	delete [] Point;
	delete [] TextureUV;
	delete [] LightmapUV;

	m_nIndices=(m_PatchSizeWidth-1)*(m_PatchSizeHeight-1)*6;
	m_pIndices=new WORD[m_nIndices];	

	for(int v=0, i = 0;v<m_PatchSizeHeight-1;++v)
	{
		for(int u=0;u<m_PatchSizeWidth-1;++u)
		{
			m_pIndices[i++]=v*m_PatchSizeWidth+u;
			m_pIndices[i++]=(v+1)*m_PatchSizeWidth+u;
			m_pIndices[i++]=v*m_PatchSizeWidth+u+1;
			m_pIndices[i++]=v*m_PatchSizeWidth+u+1;
			m_pIndices[i++]=(v+1)*m_PatchSizeWidth+u;
			m_pIndices[i++]=(v+1)*m_PatchSizeWidth+u+1;
		}

	}	
	
	/*
	FillCurve
	*/
}

const int MAX_MESHLEVEL=10;
const float SUBDIVISIONTOL=5.0f;
#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)

void CBspScene::CurveBuffer::FindSize()
{
	float *a,*b;
	bool bFound=false;
	for(int v=0;v<m_pSurface->m_PatchHeight;v++)
	{
		for(int u=0;u<m_pSurface->m_PatchWidth-1;u+=2)
		{
			a=(float*)&m_pControlPoint[v*m_pSurface->m_PatchWidth+u];
			b=(float*)&m_pControlPoint[v*m_pSurface->m_PatchWidth+u+2];
			if(!(a==b))
			{
				bFound=true;
				break;
			}
		}
		if(bFound)
			break;
	}

	vector3 test[3];	

	test[0].x=*(a);
	test[0].y=*(a+1);
	test[0].z=*(a+2);
	test[1].x=*(a+3);
	test[1].y=*(a+4);
	test[1].z=*(a+5);
	test[2].x=*(b);
	test[2].y=*(b+1);
	test[2].z=*(b+2);
	
	int level=FindLevel(test);		
	m_PatchSizeWidth=(LEVEL_WIDTH(level)-1)*((m_pSurface->m_PatchWidth-1)/2)+1;    
    
	bFound=false;

	for(int u=0;u<m_pSurface->m_PatchWidth;u++)
	{
		for(int v=0;v<m_pSurface->m_PatchHeight-1;v+=2)
		{
			a=(float*)&m_pControlPoint[v*m_pSurface->m_PatchWidth+u];
			b=(float*)&m_pControlPoint[(v+2)*m_pSurface->m_PatchWidth+u];
			if(!(a==b))
			{
				bFound=true;
				break;
			}
		}
		if(bFound)
			break;
	}	
	

	test[0].x=*(a);
	test[0].y=*(a+1);
	test[0].z=*(a+2);
	
	test[1].x=*(a+m_pSurface->m_PatchWidth*3);
	test[1].y=*(a+m_pSurface->m_PatchWidth*3+1);
	test[1].z=*(a+m_pSurface->m_PatchWidth*3+2);

	test[2].x=*(b);
	test[2].y=*(b+1);
	test[2].z=*(b+2);

	level=FindLevel(test);
    m_PatchSizeHeight=(LEVEL_WIDTH(level)-1)*((m_pSurface->m_PatchHeight-1)/2)+1;    
}

int CBspScene::CurveBuffer::FindLevel(vector3 *v)
{
	int level;
	vector3 a,b,dist;

	for(level=0;level<MAX_MESHLEVEL-1;level++)
	{
		a=(v[0]+v[1])*0.5f;
		b=(v[1]+v[2])*0.5f;
		v[2]=(a+b)*0.5f;
		dist=v[2]-v[1];
		if(dist*dist < SUBDIVISIONTOL*SUBDIVISIONTOL )
			break;
		v[1]=a;
	}
	return level;
}



void CBspScene::CurveBuffer::FillPatchVertex()
{
	
}

void CBspScene::CurveBuffer::FillCurve2(int numcp, int size, int stride, vector2 *p)
{
	int step,halfstep,i,mid;
	vector2 a,b;

	step=(size-1)/(numcp-1);

	while(step>0)
	{
		halfstep=step/2;
		for(i=0;i<size-1;i+=step*2)
		{
			mid=(i+step)*stride;

			a[0]=(p[i*stride][0]+p[mid][0])*0.5f;
			a[1]=(p[i*stride][1]+p[mid][1])*0.5f;

			b[0]=(p[mid][0] + p[(i+step*2)*stride][0] )*0.5f;
			b[1]=(p[mid][1] + p[(i+step*2)*stride][1] )*0.5f;

			p[mid][0]=(a[0]+b[0])*0.5f;
			p[mid][1]=(a[1]+b[1])*0.5f;

			if(halfstep > 0)
			{
				p[(i+halfstep)*stride][0]=a[0];
				p[(i+halfstep)*stride][1]=a[1];

				p[(i+3*halfstep)*stride][0]=b[0];
				p[(i+3*halfstep)*stride][1]=b[1];
			}
		}
		step/=2;
	}	
}

void CBspScene::CurveBuffer::FillCurve3(int numcp, int size, int stride, vector3 *p)
{
	int step,halfstep,i,mid;
	vector3 a,b;

	step = (size-1) / (numcp-1);

	while(step>0)
	{
		halfstep=step/2;
		for(i=0;i<size-1;i+=step*2)
		{
			mid=(i+step)*stride;

			a=(p[i*stride]+p[mid])*0.5f;
			b=(p[mid]+p[(i+step*2)*stride])*0.5f;
			p[mid]=(a+b)*0.5f;
			if(halfstep>0)
			{
				p[(i+halfstep)*stride]=a;
				p[(i+3*halfstep)*stride]=b;
			}
		}
		step/=2;
	}
}

void CBspScene::CurveBuffer::FillPatch2(int *numcp, int *size, vector2 *p)
{
	int step,u,v;

	step=(size[0]-1)/(numcp[0]-1);

	for(u=0;u<size[0];u+=step)
	{
		FillCurve2(numcp[1],size[1],size[0],p+u);
	}

	for(v=0;v<size[1];v++)
	{
		FillCurve2(numcp[0],size[0],1,p+v*size[0]);
	}    
}

void CBspScene::CurveBuffer::FillPatch3(int *numcp, int *size, vector3 *p)
{
	int step,u,v;

	step=(size[0]-1)/(numcp[0]-1);

	for(u=0;u<size[0];u+=step)
	{
		FillCurve3(numcp[1],size[1],size[0],p+u);
	}
	for(v=0;v<size[1];v++)
	{
		FillCurve3(numcp[0],size[0],1,p+v*size[0]);
	}   
}

void CBspScene::CurveBuffer::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetVertexShader(BSPVERTEXFVF);
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,m_PatchSizeHeight*m_PatchSizeWidth,
		 m_nIndices/3,m_pIndices,D3DFMT_INDEX16,m_pVertex,sizeof(BspVertex));
}

void CBspScene::RenderBox(LPDIRECT3DDEVICE8 pd3dDevice,DWORD dwColor)
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
	
	BoxVertex[0].v.x=m_vecMaxBox.x;
	BoxVertex[0].v.y=m_vecMaxBox.y;
	BoxVertex[0].v.z=m_vecMaxBox.z;	

	BoxVertex[1].v.x=m_vecMinBox.x;
	BoxVertex[1].v.y=m_vecMaxBox.y;
	BoxVertex[1].v.z=m_vecMaxBox.z;

	BoxVertex[2].v.x=m_vecMaxBox.x;
	BoxVertex[2].v.y=m_vecMaxBox.y;
	BoxVertex[2].v.z=m_vecMinBox.z;

	BoxVertex[3].v.x=m_vecMinBox.x;
	BoxVertex[3].v.y=m_vecMaxBox.y;
	BoxVertex[3].v.z=m_vecMinBox.z;

	BoxVertex[4].v.x=m_vecMaxBox.x;
	BoxVertex[4].v.y=m_vecMinBox.y;
	BoxVertex[4].v.z=m_vecMaxBox.z;

	BoxVertex[5].v.x=m_vecMinBox.x;
	BoxVertex[5].v.y=m_vecMinBox.y;
	BoxVertex[5].v.z=m_vecMaxBox.z;

	BoxVertex[6].v.x=m_vecMaxBox.x;
	BoxVertex[6].v.y=m_vecMinBox.y;
	BoxVertex[6].v.z=m_vecMinBox.z;

	BoxVertex[7].v.x=m_vecMinBox.x;
	BoxVertex[7].v.y=m_vecMinBox.y;
	BoxVertex[7].v.z=m_vecMinBox.z;
	for(int i=0;i<8;i++)
	{
		BoxVertex[i].diff.c=dwColor;
		BoxVertex[i].spec.c=dwColor;
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

void CBspScene::LoadLightmapVector3()
{	
	m_bLightmapVector=true;

	int i=0;
	/*
	vector3 vecLightmap[128*128];

	vector3 *vecLight;
	DWORD *pTexture;
	color lc;
	for(int i=0;i<m_nLightmap;i++)
	{
		CTexture *AddNode=new CTexture();		
		m_LightmapVectorList.Add(AddNode);

		char strFilename[256];
		sprintf(strFilename,"%s\\%s%d.lmd","c:\\","tower3",i);
		FILE *fp=fopen(strFilename,"rb");
		if(fp==NULL)
			return;
		fread(vecLightmap,sizeof(vector3)*128*128,1,fp);
		fclose(fp);

		vecLight=new vector3[128*128];
		memcpy(vecLight,vecLightmap,sizeof(vector3)*128*128);
		m_vecLightmapList.Add(vecLight);

		AddNode->CreateEmpty(128,128);
		pTexture=(DWORD*)AddNode->Lock();

		for(int ix=0;ix<128;ix++)
		{
			for(int iy=0;iy<128;iy++)
			{
				lc.a=0;
				if(vecLightmap[ix+iy*128].x>255)				
					lc.r=255;
				else
					lc.r=vecLightmap[ix+iy*128].x;

				if(vecLightmap[ix+iy*128].y>255)				
					lc.g=255;
				else
					lc.g=vecLightmap[ix+iy*128].y;

				if(vecLightmap[ix+iy*128].z>255)				
					lc.b=255;
				else
					lc.b=vecLightmap[ix+iy*128].z;

				pTexture[ix+iy*128]=lc.c;
			}
		}		
		AddNode->Unlock();
	}	
	*/

	for(i=0;i<m_nLight;i++)
	{
		m_EditLightList.push_back(m_Light[i]);
	}


	CSceneManager::m_MeshObjectContainer.SetPath(LIGHTOBJECTPATH);	
	m_pLightObject=CSceneManager::m_MeshObjectContainer.GetMeshObject("lamp_1.R3S");

}

void CBspScene::AddLight(vector3 vecLightPos, float fRange,color clr)
{
	
}

void CBspScene::UpdateLightmapVector3(int n)
{
	

}

void CBspScene::RenderLight(LPDIRECT3DDEVICE8 pd3dDevice)
{
	matrix matWorld,matPos;

	pd3dDevice->GetTransform(D3DTS_WORLD,matWorld);	

	pd3dDevice->SetPixelShader(NULL);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);		

	for(int i=0;i<(int)m_EditLightList.size();i++)
	{
		matPos.Translation(m_EditLightList[i].m_vecLightPos);
		matPos=matPos*matWorld;
		pd3dDevice->SetTransform(D3DTS_WORLD,matPos);
		if(m_SelectLight==i)
		{
			m_pLightObject->RenderBox(pd3dDevice);
		}
		m_pLightObject->Render(pd3dDevice);
	}

	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
}

void CBspScene::DeleteLight()
{
	
}

void CBspScene::CullCollisionPoly(vector3 vecPos)
{
	
}

/*
char strBSPDiffusePixelShader[]=	
"ps.1.1\n"
"tex t0\n"// Light Range	
"tex t1\n"// normal
"tex t2\n"// Tagent Light	
"tex t3\n"// base

"dp3_sat r0,t1_bx2,t2_bx2\n"	
"mul r0,r0,t3\n"
"mul_x2 r0,r0,t0.a\n";
*/

char strBSPAmbientPixelShader[]=	
"ps.1.1\n"
"def c0, 0.2f, 0.2f, 0.2f, 0.2f\n"
"tex t0\n"
"mul r0,t0,c0\n";



char strBSPAmbientVertexShader[]=
"vs.1.1\n"
"m4x4 oPos, v0, c16\n"
"mov  oT0.xy, v7.xy\n";

char strBSPDiffusePixelShader[]=	
"ps.1.1\n"
"tex t0\n"// Light Range	
"tex t1\n"// normal
"tex t2\n"// Tagent Light	
"tex t3\n"// base
"dp3_sat r0,t1_bx2,t2_bx2\n"
"mul r0,r0,t3\n"
"mul_x2 r0,r0,t0.a\n";
//"mov r0,t0.a\n";

char strBSPDiffuseVertexShader[]=
"vs.1.1\n"
"m4x4 oPos, v0, c16\n"
"mov  oT1.xy, v7.xy\n"
"mov  oT3.xy, v7.xy\n"

"mov  r8, v9\n"
"mul  r9, r8.yzxw, v3.zxyw\n"
"mad  r9, -r8.zxyw, v3.yzxw, r9\n"

"sub  r1, c32, v0\n"
"dp3  r5.x, r9, r1\n"
"dp3  r5.y, r8, r1\n"
"dp3  r5.z, v3, r1\n"

"mov  oT2.xyz, r5.xyz\n"
"mul  oT0.xyz, c33.x, r1\n";

char strBSPSpecularPixelShader[]=	
"ps.1.1\n"
"tex t0\n"//Normal
"tex t1\n"//Light Space Position
"tex t2\n"//Tangent Light Direction
"tex t3\n"//Tangent Halfway 		
"dp3_sat r0,t0_bx2,t3_bx2\n"//N.H
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r1,r0,r0\n"
"mul r0,r1,r1\n"
"mul r1,r0,r0\n"			
"mul_x2 r0,r1,t1.a\n";	


char strBSPSpecularVertexShader[]=
"vs.1.0\n"
"m4x4 oPos, v0, c16\n"
"mov  oT0.xy, v7.xy\n"   // Texture Coords

"sub  r1, c32, v0\n"        // Light direction
"mul  oT1.xyz, c33.x, r1\n" // Light Space Position

//;r9 = tangent U vector = normal cross tangent V vector
"mov  r8, v9\n"
"mul  r9, r8.yzxw, v3.zxyw\n"
"mad  r9, -r8.zxyw, v3.yzxw, r9\n"

//;r5 = tangent space light vector 
"dp3  r5.x, r9, r1\n"
"dp3  r5.y, r8, r1\n"
"dp3  r5.z, v3, r1\n"

"mov  oT2.xyz, r5.xyz\n" //Tangent Space Light direction to be renormalized by cube map

"sub  r0, c35, v0\n" //Viewer direction
"add  r4.xyz, r0.xyz, r1.xyz\n" //Halfway

//;r4 = tangent space halfway vector 
"dp3  r6.x, r9, r4\n"
"dp3  r6.y, r8, r4\n"
"dp3  r6.z, v3, r4\n"

"mov  oT3.xyz, r6.xyz\n";  // Halfway vector to be renormalized by cube map


//v0 Pos;
//v1 fShadowWeight
//v2 Normal;

//c0 0.0f,0.0f,0.0f,1.0f;
//c1~4 World*View*Projection
//c5~8 World

//c10 LightPos;
//c11 LightRagne,0.0f,0.0f,0.0f;

//*
char strBSPShadowVertexShader[]=
"vs.1.0\n"
"def c0,0.0f,0.0f,0.0f,0.0f"

"sub r0,v0,c10\n"// Light Position - Vertex Position
"dp3 r2.x,r0,r0\n"// Length^2
"rsq r2.y,r2.x\n"// 1/Length
"mul r1,r0,r2.y\n"// LV Normalized 

//r1 is Light Position - Vertex Position Normal Vector
"dp3 r3.x,r1,v2\n"
//"sge r3.y,c0.x,r3.x\n"
"sge r3.y,c0.x,r3.x\n"
"mul r4.x,r3.y,v1.x\n"
"mul r3,r1,r4.x\n"// LV*fWeight
"add r0,v0,r3\n"// Pos+LV*fWeight
"m4x4 oPos,r0,c1\n";
/*/
/*
char strBSPShadowVertexShader[]=
"vs.1.0\n"
"def c0,0.0f,0.0f,0.0f,0.0f"

"sub r0,v0,c10\n"// Light Position - Vertex Position
"dp3 r2.x,r0,r0\n"// Length^2
"rsq r2.y,r2.x\n"// 1/Length
"mul r1,r0,r2.y\n"// LV Normalized 

//r1 is Light Position - Vertex Position Normal Vector
//"dp3 r3.x,r1,v2\n"
//"sge r3.y,c0.x,r3.x\n"
//"sge r3.y,c0.x,r3.x\n"
//"mul r4.x,r3.y,v1.x\n"
"mov r4.x,v1.x\n"
"mul r3,r1,r4.x\n"// LV*fWeight
"add r0,v0,r3\n"// Pos+LV*fWeight
"m4x4 oPos,r0,c1\n";
*/


/*
// 16 WVP
// 20 WV
// 24 Projection
// 28 Inv WV
// 90 LightRange in x component
// 91 Light Position in View space        
char strBSPShadowVertexShader[]=
"vs.1.0\n"
"def c0,0,0,0,1\n"
"m4x4 r0,v0,c20\n"//World * View Transform Vertex
"m3x3 r3,v2,c28\n"//World * View Transform Normal

"sub r1,r0,c91\n"// Light to Vertex in view space
"dp3 r11.x,r1,r1\n" 
"rsq r11.y,r11.x\n" 
"mul r1,r1,r11.y\n"//normalized Light to Vertex vector

"rcp r11.y,r11.y\n"
"sub r11.z,c90.x,r11.y\n" //light range - len (vLV)
"max r11.z,r11.z,c0.x\n"

"dp3 r10.z,r3,r1\n" // vLV . Normal 
"slt r10.x,r10.z,c0.x\n" // 
"mad r2,r1,r11.z,r0\n"

"m4x4 r3,r2,c24\n"
"m4x4 r0,v0,c16\n"
"sub r10.y,c0.w,r10.x\n"
"mul r1,r3,r10.y\n"
"mad oPos,r0,r10.x,r1\n";
*/



void CBspScene::InitShader()
{
	LPD3DXBUFFER pCode;	

	DWORD dwDecl[] = 
	{
			D3DVSD_STREAM(0),
			D3DVSD_REG(0,   D3DVSDT_FLOAT3), // position
			D3DVSD_REG(3,   D3DVSDT_FLOAT3), // normal        
			D3DVSD_REG(4,	D3DVSDT_D3DCOLOR),//Diffuse Vertex Color
			D3DVSD_REG(7,   D3DVSDT_FLOAT2), // Diffuse Texture
			D3DVSD_REG(8,   D3DVSDT_FLOAT2), // Lightmap Texture
			D3DVSD_REG(9,   D3DVSDT_FLOAT3), // S		
			D3DVSD_END()
	};		

	D3DXAssembleShader(strBSPAmbientPixelShader,strlen(strBSPAmbientPixelShader),0,NULL,&pCode,NULL);										 
	CSceneManager::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwBSPAmbientPixelShader);
	pCode->Release();	

	D3DXAssembleShader(strBSPAmbientVertexShader,strlen(strBSPAmbientVertexShader),0,NULL,&pCode,NULL);				
	CSceneManager::GetDevice()->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwBSPAmbientVertexShader, 0 );
	pCode->Release();	
	
	
	D3DXAssembleShader(strBSPDiffusePixelShader,strlen(strBSPDiffusePixelShader),0,NULL,&pCode,NULL);										 
	CSceneManager::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwBSPDiffusePixelShader);
	pCode->Release();	

	D3DXAssembleShader(strBSPDiffuseVertexShader,strlen(strBSPDiffuseVertexShader),0,NULL,&pCode,NULL);				
	CSceneManager::GetDevice()->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwBSPDiffuseVertexShader, 0 );
	pCode->Release();	

	D3DXAssembleShader(strBSPSpecularPixelShader,strlen(strBSPSpecularPixelShader),0,NULL,&pCode,NULL);										 
	CSceneManager::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwBSPSpecularPixelShader);
	pCode->Release();	

	D3DXAssembleShader(strBSPSpecularVertexShader,strlen(strBSPSpecularVertexShader),0,NULL,&pCode,NULL);				
	CSceneManager::GetDevice()->CreateVertexShader( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwBSPSpecularVertexShader, 0 );
	pCode->Release();	

	DWORD dwShadowDecl[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0,   D3DVSDT_FLOAT3), // position
		D3DVSD_REG(1,   D3DVSDT_FLOAT1), // Weight
		D3DVSD_REG(2,   D3DVSDT_FLOAT3), // Normal;			
		D3DVSD_END()
	};		
	
	D3DXAssembleShader(strBSPShadowVertexShader,strlen(strBSPShadowVertexShader),0,NULL,&pCode,NULL);				
	CSceneManager::GetDevice()->CreateVertexShader( dwShadowDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwBSPShadowVertexShader, 0 );
	pCode->Release();

	D3DXCreateCubeTextureFromFile(CSceneManager::GetDevice(),"c:/NormalizeCubeMap.dds",&m_pNormalizeCubeMap);			
	D3DXCreateVolumeTextureFromFile(CSceneManager::GetDevice(),"c:/PointFalloff.dds",&m_pPointFalloffMap);

}

void CBspScene::SurfaceLightArrange()
{
	for(int cLight=0;cLight<m_nLight;cLight++)
	{
		DLightEx AddLight;
		AddLight.m_pLight=&m_Light[cLight];
		m_LightEx.push_back(AddLight);
	}
	for(int cSurface=0;cSurface<m_nDrawSurfaces;cSurface++)
	{					
		for(int cLight=0;cLight<m_nLight;cLight++)
		{	
			//float fRadius=m_Light[cLight].m_fLightRange;
			float fRadius=1000.0f;
			vector3 vecMaxBox=m_DSurfaceEx[cSurface]->m_vecMax+vector3(fRadius,fRadius,fRadius);
			vector3 vecMinBox=m_DSurfaceEx[cSurface]->m_vecMin-vector3(fRadius,fRadius,fRadius);

			bool bBoxIntersection=false;
			vector3 vecLightPos=m_Light[cLight].m_vecLightPos;

			if( vecMinBox.x <= vecLightPos.x && vecLightPos.x <= vecMaxBox.x &&
				vecMinBox.y <= vecLightPos.y && vecLightPos.y <= vecMaxBox.y &&
				vecMinBox.z <= vecLightPos.z && vecLightPos.z <= vecMaxBox.z )
			{
				m_DSurfaceEx[cSurface]->m_ApplyLightList.push_back(cLight);

				int iCnt = 0;
				for(int a=0;a<(int)m_LightEx[cLight].m_ApplySurface.size();a++)
				{
					if(m_LightEx[cLight].m_ApplySurface[a] == cSurface)
						iCnt++;
				}
				if(iCnt == 0)
					m_LightEx[cLight].m_ApplySurface.push_back(cSurface);
			}
		}				
	}	

	for(int cLight=0;cLight<m_nLight;cLight++)
	{
		int nShadowEdge=0;
		for(int cSurface=0;cSurface<(int)m_LightEx[cLight].m_ApplySurface.size();cSurface++)
		{
			nShadowEdge+=(int)m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.size();
		}		 
		CSceneManager::GetDevice()->CreateIndexBuffer(nShadowEdge*sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_LightEx[cLight].m_pEdgeIndexBuffer);		

		WORD *pIndices;

		m_LightEx[cLight].m_pEdgeIndexBuffer->Lock(0,nShadowEdge*sizeof(WORD),(BYTE**)&pIndices,0);

		

		for(int cSurface=0;cSurface<(int)m_LightEx[cLight].m_ApplySurface.size();cSurface++)
		{
			for(int cIndices=0;cIndices<(int)m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.size();cIndices++)
			{
				pIndices[cIndices]=m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList[cIndices];
			}
			//mcpy(pIndices,&m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.element[0],
			//_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.num);			
			pIndices+=(int)m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.size();

			//nShadowEdge+=m_DSurfaceEx[m_LightEx[cLight].m_ApplySurface[cSurface]]->m_ShadowIndicesList.num;
		}		
		
		m_LightEx[cLight].m_pEdgeIndexBuffer->Unlock();	

		m_LightEx[cLight].m_nEdgePoly=nShadowEdge;

	}
	
}

void AddEdge(int *pEdges,int &nEdges,int v0,int v1)
{
	for(int i=0;i<nEdges;i++)
	{
		if( (pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) ||
			(pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
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
		   
void CBspScene::CalcShadowVolume(BspVertex *pVertex,int nVertex,int *pIndices,int nIndices,DSurfaceEx &Surface)
{	
	int *pEdges=new int[nIndices*6];
    int nEdges=0;
	
	vector3 n;

	int nFace0,nFace1,nFace2;
	
	for(int i=0;i<nIndices/3;i++)
	{
		nFace0=pIndices[i*3+0];
		nFace1=pIndices[i*3+2];
		nFace2=pIndices[i*3+1];

		vector3 vecNormal(pVertex[nFace0].normal.x,pVertex[nFace0].normal.z,pVertex[nFace0].normal.y);
		vector3 vecOrigin(pVertex[nFace0].v.x,pVertex[nFace0].v.z,pVertex[nFace0].v.y);

		
		AddEdge(pEdges,nEdges,nFace0,nFace1);
		AddEdge(pEdges,nEdges,nFace1,nFace2);
		AddEdge(pEdges,nEdges,nFace2,nFace0);				
		/*
		{
			DetectEdge(pEdges,nEdges,nFace0,nFace1,Surface.m_pSurface,vecNormal,vecOrigin);
			DetectEdge(pEdges,nEdges,nFace1,nFace2,Surface.m_pSurface,vecNormal,vecOrigin);
			DetectEdge(pEdges,nEdges,nFace2,nFace0,Surface.m_pSurface,vecNormal,vecOrigin);	
		}
		*/
		
		
	}

	if(nEdges==0)
		return;	

	Surface.m_nShadowVertex=nEdges*6;	

	int nVertexPos;

	for(int cEdge=0;cEdge<nEdges;cEdge++)
	{
		BspShadowVertex v1,v2,v3,v4;
		
		v1.v.x=pVertex[pEdges[2*cEdge+0]].v.x;
		v1.v.y=pVertex[pEdges[2*cEdge+0]].v.z;
		v1.v.z=pVertex[pEdges[2*cEdge+0]].v.y;

		v1.n.x=pVertex[pEdges[2*cEdge+0]].normal.x;
		v1.n.y=pVertex[pEdges[2*cEdge+0]].normal.z;
		v1.n.z=pVertex[pEdges[2*cEdge+0]].normal.y;

		v2.v.x=pVertex[pEdges[2*cEdge+1]].v.x;
		v2.v.y=pVertex[pEdges[2*cEdge+1]].v.z;
		v2.v.z=pVertex[pEdges[2*cEdge+1]].v.y;

		v2.n.x=pVertex[pEdges[2*cEdge+1]].normal.x;
		v2.n.y=pVertex[pEdges[2*cEdge+1]].normal.z;
		v2.n.z=pVertex[pEdges[2*cEdge+1]].normal.y;


		v1.fWeight=v2.fWeight=0.0f;		

		v3.v.x=pVertex[pEdges[2*cEdge+0]].v.x;
		v3.v.y=pVertex[pEdges[2*cEdge+0]].v.z;
		v3.v.z=pVertex[pEdges[2*cEdge+0]].v.y;

		v3.n.x=pVertex[pEdges[2*cEdge+0]].normal.x;
		v3.n.y=pVertex[pEdges[2*cEdge+0]].normal.z;
		v3.n.z=pVertex[pEdges[2*cEdge+0]].normal.y;


		v4.v.x=pVertex[pEdges[2*cEdge+1]].v.x;
		v4.v.y=pVertex[pEdges[2*cEdge+1]].v.z;
		v4.v.z=pVertex[pEdges[2*cEdge+1]].v.y;

		v4.n.x=pVertex[pEdges[2*cEdge+1]].normal.x;
		v4.n.y=pVertex[pEdges[2*cEdge+1]].normal.z;
		v4.n.z=pVertex[pEdges[2*cEdge+1]].normal.y;

		v3.fWeight=v4.fWeight=10000.0f;
		

		nVertexPos=FindShadowEdgeVertex(v1);

		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{ 
			m_SurfaceEdgeVertexList.push_back(v1);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}

		nVertexPos=FindShadowEdgeVertex(v2);
		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{
			m_SurfaceEdgeVertexList.push_back(v2);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}
		nVertexPos=FindShadowEdgeVertex(v3);
		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{
			m_SurfaceEdgeVertexList.push_back(v3);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}

		nVertexPos=FindShadowEdgeVertex(v2);
		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{
			m_SurfaceEdgeVertexList.push_back(v2);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}
		nVertexPos=FindShadowEdgeVertex(v4);
		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{
			m_SurfaceEdgeVertexList.push_back(v4);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}
		nVertexPos=FindShadowEdgeVertex(v3);		
		if(nVertexPos!=-1)
			Surface.m_ShadowIndicesList.push_back(nVertexPos);
		else
		{
			m_SurfaceEdgeVertexList.push_back(v3);
			Surface.m_ShadowIndicesList.push_back((int)m_SurfaceEdgeVertexList.size()-1);
		}
		
	}	
	delete [] pEdges;
}

void CBspScene::DetectEdge(int *pEdges,int &nEdges,int nEdge0, int nEdge1,DSurface *pSrcSurface,vector3 vecPlaneNormal,vector3 vecPlaneOrigin)
{
	//DSurface *pSrcSurface=&m_DrawSurfaces[nSurface];

	vector3 vecEdgeVertex[2];
	vecEdgeVertex[0].x=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge0].v.x;
	vecEdgeVertex[0].y=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge0].v.z;
	vecEdgeVertex[0].z=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge0].v.y;

	vecEdgeVertex[1].x=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge1].v.x;
	vecEdgeVertex[1].y=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge1].v.z;
	vecEdgeVertex[1].z=m_DrawVerts[pSrcSurface->m_FirstVert+nEdge1].v.y;


	float fPlaneDistance[3];
	
	for(int i=0;i<m_nDrawSurfaces;i++)
	{
		DSurface *pTarSurface=&m_DrawSurfaces[i];		

		vector3 vecTarVertex[3];
		for(int cIndices=0;cIndices<pTarSurface->m_nIndex/3;cIndices++)
		{
			vecTarVertex[0].x=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+0] ].v.x;
			vecTarVertex[0].y=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+0] ].v.z;
			vecTarVertex[0].z=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+0] ].v.y;

			vecTarVertex[1].x=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+1] ].v.x;
			vecTarVertex[1].y=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+1] ].v.z;
			vecTarVertex[1].z=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+1] ].v.y;

			vecTarVertex[2].x=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+2] ].v.x;
			vecTarVertex[2].y=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+2] ].v.z;
			vecTarVertex[2].z=m_DrawVerts[ pTarSurface->m_FirstVert + m_DrawIndex[pTarSurface->m_FirstIndex+cIndices*3+2] ].v.y;
			/*
			if(	(vecEdgeVertex[0] == vecTarVertex[0] &&
				 vecEdgeVertex[1] == vecTarVertex[1]) ||

				(vecEdgeVertex[1] == vecTarVertex[0] &&
				 vecEdgeVertex[0] == vecTarVertex[1]) ||

				(vecEdgeVertex[1] == vecTarVertex[1] &&
				 vecEdgeVertex[0] == vecTarVertex[2]) ||

				(vecEdgeVertex[1] == vecTarVertex[1] &&
				 vecEdgeVertex[0] == vecTarVertex[2]) ||

				(vecEdgeVertex[0] == vecTarVertex[2] &&
				 vecEdgeVertex[1] == vecTarVertex[1]) ||

				(vecEdgeVertex[1] == vecTarVertex[2] &&
				 vecEdgeVertex[0] == vecTarVertex[1]) )
				 */

			if(	vecEdgeVertex[0] == vecTarVertex[0] ||
				vecEdgeVertex[0] == vecTarVertex[1] ||
				vecEdgeVertex[0] == vecTarVertex[2] ||

				vecEdgeVertex[1] == vecTarVertex[0] ||
				vecEdgeVertex[1] == vecTarVertex[1] ||
				vecEdgeVertex[1] == vecTarVertex[2] )				
			{				
				fPlaneDistance[0]=CIntersection::PointFromPlane(vecPlaneNormal,vecPlaneOrigin,vecTarVertex[0]);
				fPlaneDistance[1]=CIntersection::PointFromPlane(vecPlaneNormal,vecPlaneOrigin,vecTarVertex[1]);
				fPlaneDistance[2]=CIntersection::PointFromPlane(vecPlaneNormal,vecPlaneOrigin,vecTarVertex[2]);

				if( fPlaneDistance[0] <= -0.0001f ||
					fPlaneDistance[1] <= -0.0001f ||
					fPlaneDistance[2] <= -0.0001f )
				{
					/*
					static int n=0;
					if(n==0 || n==2 || n==3 || n==4)
					*/
					{
						pEdges[nEdges*2+0]=nEdge0;
						pEdges[nEdges*2+1]=nEdge1;
						nEdges++;						
					}
					//n++;
					return;
				}
			}
		}
	}	
}

bool CBspScene::TraceLineCollision(vector3 &vecStart, vector3 vecEnd)
{
	/*
	DNode *pNode;

	int nCluster=-1;
	int nLeaf=-1;

	pNode=&m_Nodes[0];
	
	while(1)
	{
		int nStartPos=ClassfyPoint(vecStart,pNode->m_nPlane);
		int nEndPos=ClassfyPoint(vecEnd,pNode->m_nPlane);

		if(nStartPos == nEndPos)
		{
			if(nStartPos>0)
			{
				if(pNode->m_Children[0] < 0 )
					return true;
				else 
					pNode=&m_Nodes[pNode->m_Children[0]];
			}			
			else
			{
				if(pNode->m_Children[1]<0)
					return true;
				else 
					pNode=&m_Nodes[pNode->m_Children[1]];
			}
		}	
		else
			return false;
	}	
	return false;
	//return RecurTraceLine(0,vecStart,vecEnd);	
	*/
	return RecurTraceLine(0,vecStart,vecEnd);
}

bool CBspScene::RecurTraceLine(int nNode,vector3 &vecStart, vector3 &vecEnd)
{	
	//return false;
	
	DNode *pNode=&m_Nodes[nNode];		
	int nLeaf;	
	
	int nStartPos=ClassfyPoint(vecStart,pNode->m_nPlane);
	int nEndPos=ClassfyPoint(vecEnd,pNode->m_nPlane);
	
	if(nStartPos==nEndPos)
	{
		if(nStartPos>0)
		{
			if(pNode->m_Children[0]<0)			
			{
				nLeaf=-pNode->m_Children[0]-1;
				if(m_Leafs[nLeaf].m_Cluster==-1)
					return true;
				else
					return false;

			}
			else
				return RecurTraceLine(pNode->m_Children[0],vecStart,vecEnd);
		}
		else
		{
			if(pNode->m_Children[1]<0)
			{
				nLeaf=-pNode->m_Children[1]-1;
				if(m_Leafs[nLeaf].m_Cluster==-1)
					return true;
				else
					return false;
			}
			else 
				return RecurTraceLine(pNode->m_Children[1],vecStart,vecEnd);
		}
	}
	else
	{
		DPlane *plane=&m_Planes[pNode->m_nPlane];
		vector3 vecPlaneNormal;
		vecPlaneNormal.x=plane->m_vecNormal.x;
		vecPlaneNormal.y=plane->m_vecNormal.z;
		vecPlaneNormal.z=plane->m_vecNormal.y;

		//float fStartDistance=vecStart*vecPlaneNormal-plane->m_fDist;

		float fStartDistance;

		vector3 vecDir=vecEnd-vecStart;
		vecDir.Normalize();

		fStartDistance=((plane->m_fDist)-(vecStart*vecPlaneNormal))/(vecPlaneNormal*vecDir);
		
		fStartDistance=fabsf(fStartDistance);

		/*
		float h=( (n.x*poly[0].x+n.y*poly[0].y+n.z*poly[0].z) - (n.x*vecStart.x+n.y*vecStart.y+n.z*vecStart.z) )
		     / (n.x*vecDir.x + n.y*vecDir.y + n.z*vecDir.z);
	fInterLens=h;

		*/


		//fStartDistance+=1.0f;

		
		vector3 vecMid;
		//if(nStartPos > 0 )
		fStartDistance-=1.0f;
		vecMid=vecStart+(fStartDistance*vecDir);
		//else
		//	vecMid=vecStart+(fStartDistance*vecPlaneNormal);		

		int nMidPos=ClassfyPoint(vecMid,pNode->m_nPlane);//for debugging
		
		if(RecurTraceLine(nNode,vecStart,vecMid)==true)
			return true;
		
		fStartDistance+=2.0f;

		vecMid=vecStart+(fStartDistance*vecDir);
		/*
		if(nStartPos > 0 )
			vecMid=vecStart-(fStartDistance*vecPlaneNormal);		
		else
			vecMid=vecStart+(fStartDistance*vecPlaneNormal);				
			*/

		nMidPos=ClassfyPoint(vecMid,pNode->m_nPlane);//for debugging
		

		return RecurTraceLine(nNode,vecMid,vecEnd);				
	}
}

void CBspScene::InitTrace()
{
	m_TNodes=(TNode*)malloc( (MAX_TNODES+1) *sizeof(TNode));
	m_TNodes=(TNode*)(((int)m_TNodes+31)&~31);
	m_TNodes_p=m_TNodes;
	MakeTnode(0);	
	InitSurfacesForTesting();
}

void CBspScene::MakeTnode(int nodenum)
{
	TNode *t;
	DPlane *plane;
	int i;
	DNode *node;
	int leafNum;
	
	t=m_TNodes_p++;
	
	node=m_Nodes+nodenum;
	plane=m_Planes+node->m_nPlane;

	t->m_nPlaneNum=node->m_nPlane;
	t->m_Type=PlaneTypeForNormal(plane->m_vecNormal);
	t->m_vecNormal=plane->m_vecNormal;
	t->m_fDist=plane->m_fDist;

	for(i=0;i<2;i++)
	{
		if(node->m_Children[i]<0)
		{
			leafNum=-node->m_Children[i]-1;
			if(m_Leafs[leafNum].m_Cluster==-1)
			{
				t->m_Children[i]=leafNum|(1<<31)|(1<<30);
			}
			else
			{
				t->m_Children[i]=leafNum|(1<<31);
			}
		}
		else
		{
			t->m_Children[i]=m_TNodes_p-m_TNodes;
			MakeTnode(node->m_Children[i]);
		}
	}
}

void CBspScene::TraceLine(vector3 &start, vector3 &stop, TraceT *trace, bool testAll, TraceWorkT *tw)
{
	int r,i,j;
	DLeaf *leaf;
	float oldHitFrac;
	int surfaceNum;
	unsigned char surfaceTested[MAX_MAP_DRAW_SURFS/8];	

	SurfaceTestT *test;	

	trace->m_vecFilter.v[0]=1.0f;
	trace->m_vecFilter.v[1]=1.0f;
	trace->m_vecFilter.v[2]=1.0f;
	
	tw->m_vecStart=start;
	tw->m_vecEnd=stop;
	tw->m_Trace=trace;
	
	tw->m_nOpenLeafs=0;
	trace->m_bPassSolid=false;
	trace->m_fHitFraction=1.0f;
	

	//r = TraceLine_r( 0, start, stop, tw );
	r=RecurTraceLine(0,start,stop,tw);

	if(r && !testAll)
	{
		return;
	}

	memset(surfaceTested,0,(m_nDrawSurfaces+7)/8);

	oldHitFrac=trace->m_fHitFraction;

	for(i=0;i<tw->m_nOpenLeafs;i++)
	{
		leaf=&m_Leafs[tw->m_OpenLeafNumbers[i]];
		for(j=0;j<leaf->m_nLeafSurfaces;j++)
		{
			surfaceNum=m_LeafSurfaces[leaf->m_FirstLeafSurface+j];

			if(surfaceTested[surfaceNum>>3] & (1<<(surfaceNum&7)))
			{
				continue;
			}
			surfaceTested[surfaceNum>>3]|=(1<<(surfaceNum&7));

			test=m_SurfaceTest[surfaceNum];
			if(!test)
				continue;

			TraceAgainstSurface(tw,test);
		}

		if(trace->m_fHitFraction < oldHitFrac)
		{
			trace->m_bPassSolid=true;
			break;
		}
	}

	for(i=0;i<3;i++)
	{
		trace->m_vecHit.v[i]=start.v[i]+(stop.v[i]-start.v[i])*trace->m_fHitFraction;
	}
}

int CBspScene::RecurTraceLine(int node, vector3 &start, vector3 &stop, TraceWorkT *tw)
{
	TNode *tnode;
	float front,back;
	vector3 mid;
	float frac;
	int side;
	int r;

	if(node & (1<<31))
	{
		if(node & (1 << 30))
		{
			tw->m_Trace->m_vecHit=start;
			tw->m_Trace->m_bPassSolid=true;
			return true;
		}
		else
		{
			if(tw->m_nOpenLeafs== MAX_MAP_LEAFS)
			{
				return false;
			}
			tw->m_OpenLeafNumbers[tw->m_nOpenLeafs]= node & ~(3 << 30);
			tw->m_nOpenLeafs++;
			return false;
		}
	}

	tnode=&m_TNodes[node];
	switch(tnode->m_Type)
	{
		case PLANE_X:
			front=start.v[0]-tnode->m_fDist;
			back=stop.v[0]-tnode->m_fDist;
			break;
		case PLANE_Y:
			front=start.v[1]-tnode->m_fDist;
			back=stop.v[1]-tnode->m_fDist;
			break;
		case PLANE_Z:
			front=start.v[2]-tnode->m_fDist;
			back=stop.v[2]-tnode->m_fDist;
			break;
		default:
			front=(start*tnode->m_vecNormal)-tnode->m_fDist;
			back=(stop*tnode->m_vecNormal)-tnode->m_fDist;
			break;

	}	

	if(front>=-TRACE_ON_EPSILON && back>=-TRACE_ON_EPSILON)
	{
		return RecurTraceLine(tnode->m_Children[0],start,stop,tw);
	}

	if(front<TRACE_ON_EPSILON && back<TRACE_ON_EPSILON)
	{
		return RecurTraceLine(tnode->m_Children[1],start,stop,tw);
	}

	side=front<0;
	frac=front/(front-back);

	mid.x=start.x+(stop.x-start.x)*frac;
	mid.y=start.y+(stop.y-start.y)*frac;
	mid.z=start.z+(stop.z-start.z)*frac;

	r=RecurTraceLine(tnode->m_Children[side],start,mid,tw);
	if(r)
	{
		return r;
	}

	return RecurTraceLine(tnode->m_Children[!side],mid,stop,tw);	
}

void CBspScene::TraceAgainstSurface(TraceWorkT *tw, SurfaceTestT *surf)
{
	int i;

	if(SphereCull(tw->m_vecStart,tw->m_vecEnd,surf->m_vecOrigins,surf->m_fRadius))
	{
		return;
	}	
	
	for(i=0;i<surf->m_nFacets;i++)
	{		
		TraceAgainstFacet(tw,surf->m_nShader,surf->m_Facets+i);
	}
}


bool CBspScene::SphereCull(vector3 &start, vector3 &stop, vector3 &origin, float radius)
{
	vector3 v;
	float d;
	vector3 dir;
	float len;
	vector3 on;

	dir=stop-start;

	len=dir.GetLens();
	dir.Normalize();

	v=origin-start;

	d=v*dir;

	if(d>len+radius)
		return true;
	if(d<-radius)
		return true;

	
	on=start+d*dir;

	v=on-origin;
	
	len=v.GetLens();

	if(len>radius)
		return true;
	return false;	
}

void CBspScene::TraceAgainstFacet(TraceWorkT *tr, int nShader, CFaceT *facet)
{
	int j;
	float d1,d2,d,f;
	vector3 point;
	float dist;
	
	if(facet->m_nBoundaries < 3)
		return;

	dist=facet->m_fSurface[3];
	vector3 vecSurface(facet->m_fSurface[0],facet->m_fSurface[1],facet->m_fSurface[2]);

	d1=(tr->m_vecStart*vecSurface)-dist;
	if(d1>-1 && d1<1)
		return;
	
	d2=(tr->m_vecEnd*vecSurface)-dist;
	if(d2>-1 && d2<1)
	{
		return;
	}
	
	f=(d1-ON_EPSILON)/(d1-d2);
	if(f<=0)
		return;
	
	if(f>=tr->m_Trace->m_fHitFraction)
		return;
	
	point=tr->m_vecStart+f*(tr->m_vecEnd-tr->m_vecStart);

	for(j=0;j<facet->m_nBoundaries;j++)
	{
		dist=facet->m_fBoundaries[j][3];

		vector3 vecBound(facet->m_fBoundaries[j][0],facet->m_fBoundaries[j][1],facet->m_fBoundaries[j][2]);
		d=point*vecBound;
		if(d>dist+ON_EPSILON)
			break;
	}
	
	if(j!=facet->m_nBoundaries)
		return;
	tr->m_Trace->m_vecFilter=vector3(0.0f,0.0f,0.0f);

	tr->m_Trace->m_fHitFraction=f;
}

void CBspScene::InitSurfacesForTesting()
{
	int i,j;
	DSurface *dsurf;
	SurfaceTestT *test;

	BspVertex *dvert;

	for(i=0;i<m_nDrawSurfaces;i++)
	{
		dsurf=&m_DrawSurfaces[i];

		if(!dsurf->m_nIndex && !dsurf->m_PatchWidth)
			continue;

		test=(SurfaceTestT*)malloc(sizeof(*test));
		m_SurfaceTest[i]=test;
		//test->m_vecMaxs=test->m_vecMins=vector3(0.0f,0.0f,0.0f);
		test->m_vecMaxs=vector3(-99999,-99999,-99999);
		test->m_vecMins=vector3(99999,99999,99999);

		dvert=&m_DrawVerts[dsurf->m_FirstVert];
		for(j=0;j<dsurf->m_nVerts;j++,dvert++)
		{
			AddPointToBounds(dvert->v,test->m_vecMaxs,test->m_vecMins);
		}

		vector3 vecOrigin=test->m_vecMaxs+test->m_vecMins;
		vecOrigin=0.5f*vecOrigin;

		vector3 vecTemp=test->m_vecMaxs-vecOrigin;
		test->m_fRadius=vecTemp.GetLens();	
		test->m_vecOrigins=vecOrigin;
		if(dsurf->m_SurfaceType==0)//Planar
		{
			FacetsForTriangleSurface(dsurf,dsurf->m_nShader,test);
		}
		else if(dsurf->m_SurfaceType==1)
		{
			FacetsForPatch(dsurf,dsurf->m_nShader,test);
		}
	}
	
}

void CBspScene::FacetsForTriangleSurface(DSurface *dsurf, int nShader, SurfaceTestT *test)
{
	int i;
	BspVertex *v1,*v2,*v3,*v4;
	int count;

	int i1,i2,i3,i4,i5,i6;

	test->m_bPatch=false;
	test->m_nFacets=dsurf->m_nIndex/3;
	test->m_Facets=(CFaceT*)malloc(sizeof(test->m_Facets[0])*test->m_nFacets);
	test->m_nShader=nShader;
	
	count=0;
	for(i=0;i<test->m_nFacets;i++)
	{
		i1=m_DrawIndex[dsurf->m_FirstIndex+i*3];
		i2=m_DrawIndex[dsurf->m_FirstIndex+i*3+1];
		i3=m_DrawIndex[dsurf->m_FirstIndex+i*3+2];

		v1=&m_DrawVerts[dsurf->m_FirstVert+i1];
		v2=&m_DrawVerts[dsurf->m_FirstVert+i2];
		v3=&m_DrawVerts[dsurf->m_FirstVert+i3];

		if(i!=test->m_nFacets-1)
		{
			i4=m_DrawIndex[dsurf->m_FirstIndex+i*3+3];
			i5=m_DrawIndex[dsurf->m_FirstIndex+i*3+4];
			i6=m_DrawIndex[dsurf->m_FirstIndex+i*3+5];

			if(i4==i3 && i5==i2)
			{
				v4=&m_DrawVerts[dsurf->m_FirstVert+i6];				
				if(CM_GenerateFacetFor4Points(&test->m_Facets[count],v1,v2,v4,v3))
				{
					count++;
					i++;
					continue;
				}
			}
		}
		if(CM_GenerateFacetFor3Points(&test->m_Facets[count],v1,v2,v3))
			count++;
	}

	test->m_nFacets=count;	
}

bool CBspScene::CM_GenerateFacetFor3Points(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c)
{
	if(!PlaneFromPoints(f->m_fSurface,a->v,b->v,c->v))
	{
		f->m_nBoundaries=0;
		return false;
	}
	
	f->m_nBoundaries=3;

	CM_GenerateBoundaryForPoints(f->m_fBoundaries[0],f->m_fSurface,a->v,b->v);
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[1],f->m_fSurface,b->v,c->v);
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[2],f->m_fSurface,c->v,a->v);
	
	f->m_vecPoint[0]=a->v;
	f->m_vecPoint[1]=b->v;
	f->m_vecPoint[2]=c->v;	

	TextureMatrixFromPoints( f, a, b, c );

	return true;
}

bool CBspScene::PlaneFromPoints(float *plane, vector3 &a, vector3 &b, vector3 &c)
{
	vector3 d1,d2;

	d1=b-a;
	d2=c-a;
	
	vector3 vecNormal;
	vecNormal=d2^d1;

	if(vecNormal.GetLens()==0)
	{
		return false;
	}
	vecNormal.Normalize();
	
	plane[0]=vecNormal.x;
	plane[1]=vecNormal.y;
	plane[2]=vecNormal.z;


	plane[3]=a*vecNormal;
	return true;
}

void CBspScene::CM_GenerateBoundaryForPoints(float boundary[], float plane[], vector3 &a, vector3 &b)
{
	vector3 d1;

	d1=b-a;

	vector3 vecNormal,vecPlane,vecBoundary;	
	vecBoundary=vector3(boundary[0],boundary[1],boundary[2]);
	vecPlane=vector3(plane[0],plane[1],plane[2]);

	vecNormal=vecPlane^d1;
	vecNormal.Normalize();
	vecBoundary=vecNormal;

	boundary[3]=a*vecBoundary;

	boundary[0]=vecBoundary.x;
	boundary[1]=vecBoundary.y;
	boundary[2]=vecBoundary.z;	
}

void CBspScene::TextureMatrixFromPoints(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c)
{
	int			i, j;
	float		t;
	float		m[3][4];
	float		s;

	// This is an incredibly stupid way of solving a three variable equation
	for ( i = 0 ; i < 2 ; i++ ) {

		m[0][0] = a->v.v[0];
		m[0][1] = a->v.v[1];
		m[0][2] = a->v.v[2];
		if(i==0)		
			m[0][3] = a->tu;
		else
			m[0][3] = a->tv;

		m[1][0] = b->v.v[0];
		m[1][1] = b->v.v[1];
		m[1][2] = b->v.v[2];

		if(i==0)		
			m[1][3] = b->tu;
		else
			m[1][3] = b->tv;		

		m[2][0] = c->v.v[0];
		m[2][1] = c->v.v[1];
		m[2][2] = c->v.v[2];

		if(i==0)
			m[2][3]=c->tu;
		else
			m[2][3]=c->tv;		

		if ( fabs(m[1][0]) > fabs(m[0][0]) && fabs(m[1][0]) > fabs(m[2][0]) ) {
			for ( j = 0 ; j < 4 ; j ++ ) {
				t = m[0][j];
				m[0][j] = m[1][j];
				m[1][j] = t;
			}
		} else if ( fabs(m[2][0]) > fabs(m[0][0]) && fabs(m[2][0]) > fabs(m[1][0]) ) {
			for ( j = 0 ; j < 4 ; j ++ ) {
				t = m[0][j];
				m[0][j] = m[2][j];
				m[2][j] = t;
			}
		}

		s = 1.0 / m[0][0];
		m[0][0] *= s;
		m[0][1] *= s;
		m[0][2] *= s;
		m[0][3] *= s;

		s = m[1][0];
		m[1][0] -= m[0][0] * s;
		m[1][1] -= m[0][1] * s;
		m[1][2] -= m[0][2] * s;
		m[1][3] -= m[0][3] * s;

		s = m[2][0];
		m[2][0] -= m[0][0] * s;
		m[2][1] -= m[0][1] * s;
		m[2][2] -= m[0][2] * s;
		m[2][3] -= m[0][3] * s;

		if ( fabs(m[2][1]) > fabs(m[1][1]) ) {
			for ( j = 0 ; j < 4 ; j ++ ) {
				t = m[1][j];
				m[1][j] = m[2][j];
				m[2][j] = t;
			}
		}

		s = 1.0 / m[1][1];
		m[1][0] *= s;
		m[1][1] *= s;
		m[1][2] *= s;
		m[1][3] *= s;

		s = m[2][1];
		m[2][0] -= m[1][0] * s;
		m[2][1] -= m[1][1] * s;
		m[2][2] -= m[1][2] * s;
		m[2][3] -= m[1][3] * s;

		s = 1.0 / m[2][2];
		m[2][0] *= s;
		m[2][1] *= s;
		m[2][2] *= s;
		m[2][3] *= s;

		f->m_fTextureMatrix[i][2] = m[2][3];
		f->m_fTextureMatrix[i][1] = m[1][3] - f->m_fTextureMatrix[i][2] * m[1][2];
		f->m_fTextureMatrix[i][0] = m[0][3] - f->m_fTextureMatrix[i][2] * m[0][2] - f->m_fTextureMatrix[i][1] * m[0][1];

		f->m_fTextureMatrix[i][3] = 0;
		
	}
}

#define	PLANAR_EPSILON	0.1

bool CBspScene::CM_GenerateFacetFor4Points(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c, BspVertex *d)
{
	float dist;
	int i;
	float plane[4];

	if(!PlaneFromPoints(f->m_fSurface,a->v,b->v,c->v))
	{
		f->m_nBoundaries=0;
		return false;
	}
	vector3 vecSurface(f->m_fSurface[0],f->m_fSurface[1],f->m_fSurface[2]);
	dist=(d->v*vecSurface)-f->m_fSurface[3];

	if(fabsf(dist) > PLANAR_EPSILON)
	{
		f->m_nBoundaries=0;
		return false;
	}
	f->m_nBoundaries=4;
	
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[0],f->m_fSurface,a->v,b->v);
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[1],f->m_fSurface,b->v,c->v);
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[2],f->m_fSurface,c->v,d->v);
	CM_GenerateBoundaryForPoints(f->m_fBoundaries[3],f->m_fSurface,d->v,a->v);

	f->m_vecPoint[0]=a->v;
	f->m_vecPoint[1]=b->v;
	f->m_vecPoint[2]=c->v;
	f->m_vecPoint[3]=d->v;

	for(i=1;i<4;i++)
	{
		if(!PlaneFromPoints(plane,f->m_vecPoint[i],f->m_vecPoint[(i+1)%4],f->m_vecPoint[(i+2)%4]))
		{
			f->m_nBoundaries=0;
			return false;
		}
		vecSurface=vector3(f->m_fSurface[0],f->m_fSurface[1],f->m_fSurface[2]);
		vector3 vecPlane(plane[0],plane[1],plane[2]);
		if((vecSurface*vecPlane) < 0.9f)
		{
			f->m_nBoundaries=0;
			return false;
		}
	}

	TextureMatrixFromPoints(f,a,b,c);

	return true;	
}

void CBspScene::FacetsForPatch(DSurface *dsurf, int nShader, SurfaceTestT *test)
{
	int i,j;
	BspVertex *v1,*v2,*v3,*v4;
	int count;
	SMesh srcMesh,*subdivided,*mesh;

	srcMesh.m_nWidth=dsurf->m_PatchWidth;
	srcMesh.m_nHeight=dsurf->m_PatchHeight;
	srcMesh.m_pVerts=&m_DrawVerts[dsurf->m_FirstVert];

	mesh=SubdivideMesh(srcMesh,8,999);
	PutMeshOnCurve(*mesh);		
	MakeMeshNormals(*mesh);
	subdivided=RemoveLinearMeshColumnsRows(mesh);	
	FreeMesh(mesh);

	test->m_bPatch=true;
	test->m_nFacets=(subdivided->m_nWidth-1)*(subdivided->m_nHeight-1)*2;
	test->m_Facets=(CFaceT*)malloc(sizeof(test->m_Facets[0])*test->m_nFacets);
	test->m_nShader=-1;	

	count=0;
	
	for(i=0;i<subdivided->m_nWidth-1;i++)
	{
		for(j=0;j<subdivided->m_nHeight-1;j++)
		{
			v1=subdivided->m_pVerts+j*subdivided->m_nWidth+i;
			v2=v1+1;
			v3=v1+subdivided->m_nWidth+1;
			v4=v1+subdivided->m_nWidth;

			if(CM_GenerateFacetFor4Points(&test->m_Facets[count],v1,v4,v3,v2))
			{
				count++;
			}
			else
			{
				if(CM_GenerateFacetFor3Points(&test->m_Facets[count],v1,v4,v3))
					count++;
				if(CM_GenerateFacetFor3Points(&test->m_Facets[count],v1,v3,v2))
					count++;
			}
		}
	}
	test->m_nFacets=count;
	FreeMesh(subdivided);	
}

SMesh* CBspScene::RemoveLinearMeshColumnsRows(SMesh *in)
{
	int i,j,k;
	float len,maxLength;
	vector3 proj,dir;
	SMesh out;

	BspVertex expand[MAX_EXPANDED_AXIS][MAX_EXPANDED_AXIS];
	out.m_nWidth=in->m_nWidth;
	out.m_nHeight=in->m_nHeight;

	for(i=0;i<in->m_nWidth;i++)
	{
		for(j=0;j<in->m_nHeight;j++)
		{
			expand[j][i]=in->m_pVerts[j*in->m_nWidth+i];
		}
	}
	for(j=1;j<out.m_nWidth-1;j++)
	{
		maxLength=0.0f;
		for(i=0;i<out.m_nHeight;i++)
		{
			ProjectPointOntoVector(expand[i][j].v,expand[i][j-1].v,expand[i][j+1].v,proj);
			dir=expand[i][j].v-proj;
			len=dir.GetLens();
			if(len>maxLength)
				maxLength=len;
		}
		if(maxLength<0.1f)
		{
			out.m_nWidth--;
			for(i=0;i<out.m_nHeight;i++)
			{
				for(k=j;k<out.m_nWidth;k++)
				{
					expand[i][k]=expand[i][k+1];
				}
			}
			for(k=j;k<out.m_nWidth;k++)
			{
				originalWidths[k]=originalWidths[k+1];
			}
			j--;
		}
	}

	for(j=1;j<out.m_nHeight-1;j++)
	{
		maxLength=0.0f;
		for(i=0;i<out.m_nWidth;i++)
		{
			ProjectPointOntoVector(expand[j][i].v,expand[j-1][i].v,expand[j+1][i].v,proj);
			dir=expand[j][i].v-proj;
			len=dir.GetLens();
			if(len>maxLength)
				maxLength=len;
		}
		if(maxLength<0.1)
		{
			out.m_nHeight--;
			for(i=0;i<out.m_nWidth;i++)
			{
				for(k=j;k<out.m_nHeight;k++)
				{
					expand[k][i]=expand[k+1][i];
				}
			}
			for(k=j;k<out.m_nHeight;k++)
			{
				originalHeights[k]=originalHeights[k+1];
			}
			j--;
		}
	}

	out.m_pVerts=&expand[0][0];
	for(i=1;i<out.m_nHeight;i++)
	{
		memmove(&out.m_pVerts[i*out.m_nWidth],expand[i],out.m_nWidth*sizeof(BspVertex));
	}
	return CopyMesh(&out);	
}

int CBspScene::PlaneTypeForNormal(vector3 p)
{
	if(p.x==1.0f || p.x==-1.0f)
		return PLANE_X;
	if(p.y==1.0f || p.y==-1.0f)
		return PLANE_Y;
	if(p.z==1.0f || p.z==-1.0f)
		return PLANE_Z;
	return PLANE_NON_AXIAL;	
}

void CBspScene::AddPointToBounds(vector3 &v, vector3 &max, vector3 &min)
{
	float val;
	for(int i=0;i<3;i++)
	{
		val=v.v[i];
		if(val<min.v[i])
			min.v[i]=val;
		if(val>max.v[i])
			max.v[i]=val;
	}	
}

SMesh* CBspScene::SubdivideMesh(SMesh in, float maxError, float minLength)
{
	int i,j,k,l;
	BspVertex prev,next,mid;
	vector3 prevxyz,nextxyz,midxyz;
	vector3 delta;
	float len;
	SMesh out;
	BspVertex expand[MAX_EXPANDED_AXIS][MAX_EXPANDED_AXIS];	

	out.m_nWidth=in.m_nWidth;
	out.m_nHeight=in.m_nHeight;	

	for(i=0;i<in.m_nWidth;i++)
	{
		for(j=0;j<in.m_nHeight;j++)
		{
			expand[j][i]=in.m_pVerts[j*in.m_nWidth+i];
		}
	}	

	for(i=0;i<in.m_nHeight;i++)
	{
		originalHeights[i]=i;
	}
	for(i=0;i<in.m_nWidth;i++)
	{
		originalWidths[i]=i;
	}

	for(j=0;j+2<out.m_nWidth;j+=2)
	{
		for(i=0;i<out.m_nHeight;i++)
		{
			for(l=0;l<3;l++)
			{
				prevxyz.v[l]=expand[i][j+1].v.v[l]-expand[i][j].v.v[l];
				nextxyz.v[l]=expand[i][j+2].v.v[l]-expand[i][j+1].v.v[l];
				midxyz.v[l]=(expand[i][j].v.v[l]+expand[i][j+1].v.v[l]*2+expand[i][j+2].v.v[l])*0.25f;
			}

			if( prevxyz.GetLens() > minLength ||
				nextxyz.GetLens() > minLength )
				break;

			delta=expand[i][j+1].v-midxyz;
			len=delta.GetLens();
			if(len>maxError)
				break;
		}

		if(out.m_nWidth+2 >=MAX_EXPANDED_AXIS)
			break;
		if(i==out.m_nHeight)
			continue;

		out.m_nWidth+=2;

		for(k=out.m_nWidth-1;k>j+3;k--)
		{
			originalWidths[k]=originalWidths[k-2];
		}
		originalWidths[j+3]=originalWidths[j+1];
		originalWidths[j+2]=originalWidths[j+1];
		originalWidths[j+1]=originalWidths[j];

		for(i=0;i<out.m_nHeight;i++)
		{
			LerpDrawVert(&expand[i][j],&expand[i][j+1],&prev);
			LerpDrawVert(&expand[i][j+1],&expand[i][j+2],&next);
			LerpDrawVert(&prev,&next,&mid);

			for(k=out.m_nWidth-1;k>j+3;k--)
			{
				expand[i][k]=expand[i][k-2];					
			}
			expand[i][j+1]=prev;
			expand[i][j+2]=mid;
			expand[i][j+3]=next;			
		}
		j-=2;
	}	

	

	for(j=0;j+2<out.m_nHeight;j+=2)
	{
		for(i=0;i<out.m_nWidth;i++)
		{
			for(l=0;l<3;l++)
			{
				prevxyz.v[l]=expand[j+1][i].v.v[l]-expand[j][i].v.v[l];
				nextxyz.v[l]=expand[j+2][i].v.v[l]-expand[j+1][i].v.v[l];
				midxyz.v[l]=(expand[j][i].v.v[l] + expand[j+1][i].v.v[l]*2+ expand[j+2][i].v.v[l])*0.25f;				
			}
			if( prevxyz.GetLens() > minLength ||
				nextxyz.GetLens() > minLength )
				break;

			delta=expand[j+1][i].v-midxyz;
			len=delta.GetLens();
			if(len>maxError)
				break;

		}

		if(out.m_nHeight+2>=MAX_EXPANDED_AXIS)
			break;

		if(i==out.m_nWidth)
			continue;

		out.m_nHeight+=2;

		for(k=out.m_nHeight-1;k>j+3;k--)
			originalHeights[k]=originalHeights[k-2];

		originalHeights[j+3]=originalHeights[j+1];
		originalHeights[j+2]=originalHeights[j+1];
		originalHeights[j+1]=originalHeights[j];

		for(i=0;i<out.m_nWidth;i++)
		{
			LerpDrawVert(&expand[j][i],&expand[j+1][i],&prev);
			LerpDrawVert(&expand[j+1][i],&expand[j+2][i],&next);
			LerpDrawVert(&prev,&next,&mid);

			for(k=out.m_nHeight-1;k>j+3;k--)
				expand[k][i]=expand[k-2][i];
			expand[j+1][i]=prev;
			expand[j+2][i]=mid;
			expand[j+3][i]=next;			
		}
		j-=2;
	}
	

	out.m_pVerts=&expand[0][0];	
	
	for(i=1;i<out.m_nHeight;i++)
	{
		memmove(&out.m_pVerts[i*out.m_nWidth],expand[i],out.m_nWidth*sizeof(BspVertex));				
	}
	return CopyMesh(&out);		
}

void CBspScene::PutMeshOnCurve(SMesh in)
{
	int i,j,l;
	float prev,next;

	for(i=0;i<in.m_nWidth;i++)
	{
		for(j=1;j<in.m_nHeight;j+=2)
		{
			for(l=0;l<3;l++)
			{
				prev=(in.m_pVerts[j*in.m_nWidth+i].v.v[l]+in.m_pVerts[(j+1)*in.m_nWidth+i].v.v[l])*0.5f;
				next=(in.m_pVerts[j*in.m_nWidth+i].v.v[l]+in.m_pVerts[(j-1)*in.m_nWidth+i].v.v[l])*0.5f;
				in.m_pVerts[j*in.m_nWidth+i].v.v[l]=(prev+next)*0.5f;

			}
		}
	}
	/*
	int		i, j, l;
	float	prev, next;

	// put all the aproximating points on the curve
	for ( i = 0 ; i < in.width ; i++ ) {
		for ( j = 1 ; j < in.height ; j += 2 ) {
			for ( l = 0 ; l < 3 ; l++ ) {
				prev = ( in.verts[j*in.width+i].xyz[l] + in.verts[(j+1)*in.width+i].xyz[l] ) * 0.5;
				next = ( in.verts[j*in.width+i].xyz[l] + in.verts[(j-1)*in.width+i].xyz[l] ) * 0.5;
				in.verts[j*in.width+i].xyz[l] = ( prev + next ) * 0.5;
			}
		}
	}

	for ( j = 0 ; j < in.height ; j++ ) {
		for ( i = 1 ; i < in.width ; i += 2 ) {
			for ( l = 0 ; l < 3 ; l++ ) {
				prev = ( in.verts[j*in.width+i].xyz[l] + in.verts[j*in.width+i+1].xyz[l] ) * 0.5;
				next = ( in.verts[j*in.width+i].xyz[l] + in.verts[j*in.width+i-1].xyz[l] ) * 0.5;
				in.verts[j*in.width+i].xyz[l] = ( prev + next ) * 0.5;
			}
		}
	}
	*/

	for(j=0;j<in.m_nHeight;j++)
	{
		for(i=1;i<in.m_nWidth;i+=2)
		{
			for(l=0;l<3;l++)
			{
				prev=(in.m_pVerts[j*in.m_nWidth+i].v.v[l]+in.m_pVerts[j*in.m_nWidth+i+1].v.v[l])*0.5f;
				next=(in.m_pVerts[j*in.m_nWidth+i].v.v[l]+in.m_pVerts[j*in.m_nWidth+i-1].v.v[l])*0.5f;
				in.m_pVerts[j*in.m_nWidth+i].v.v[l]=(prev+next)*0.5f;
			}
		}
	}	
}

int	neighbors[8][2] = {
	{0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}, {-1,-1}, {-1,0}, {-1,1}
};

void CBspScene::MakeMeshNormals(SMesh in)
{
	int i,j,k,dist;
	vector3 normal;
	vector3 sum;
	int count;
	vector3 base;
	vector3 delta;
	int x,y;
	BspVertex *dv;
	vector3 around[8],temp;
	bool good[8];
	bool wrapWidth,wrapHeight;
	float len;

	wrapWidth=false;
	for(i=0;i<in.m_nHeight;i++)
	{
		delta=in.m_pVerts[i*in.m_nWidth].v-in.m_pVerts[i*in.m_nWidth+in.m_nWidth-1].v;
		len=delta.GetLens();
		if(len>1.0f)
			break;
	}
	if(i==in.m_nHeight)
	{
		wrapWidth=true;
	}
	wrapHeight=false;
	for(i=0;i<in.m_nWidth;i++)
	{
		delta=in.m_pVerts[i].v-in.m_pVerts[i+(in.m_nHeight-1)*in.m_nWidth].v;
		len=delta.GetLens();
		if(len>1.0f)
			break;
	}
	if(i==in.m_nWidth)
	{
		wrapHeight=true;
	}
	for(i=0;i<in.m_nWidth;i++)
	{
		for(j=0;j<in.m_nHeight;j++)
		{
			count=0;
			dv=&in.m_pVerts[j*in.m_nWidth+i];
			base=dv->v;
			for(k=0;k<8;k++)
			{
				around[k]=vector3(0.0f,0.0f,0.0f);
				good[k]=false;

				for(dist=1;dist<=3;dist++)
				{
					x=i+neighbors[k][0]*dist;
					y=j+neighbors[k][1]*dist;
					if(wrapWidth)
					{
						if(x<0)
							x=in.m_nWidth-1+x;
						else if(x>=in.m_nWidth)
							x=1+x-in.m_nWidth;
					}
					if(wrapHeight)
					{
						if(y<0)
							y=in.m_nHeight-1+y;
						else if(y>=in.m_nHeight)
							y=1+y-in.m_nHeight;
					}

					if(x<0 || x>=in.m_nWidth || y<0 || y>=in.m_nHeight)
						break;
					temp=in.m_pVerts[y*in.m_nWidth+x].v-base;
					//temp.Normalize()
					if(temp.Mag()==0)
						continue;
					else
					{
						good[k]=true;
						temp.Normalize();
						around[k]=temp;
						break;
					}
				}
			}
			sum=vector3(0.0f,0.0f,0.0f);
			for(k=0;k<8;k++)
			{
				if(!good[k] || !good[(k+1)&7])
					continue;
				normal=around[(k+1)&7]^around[k];
				if(normal.Mag()==0)
					continue;
				normal.Normalize();
				sum=sum+normal;
				count++;
			}
			if(count==0)
			{
				count=1;
			}
			dv->normal=sum.Normalized();
		}
	}	
}

void CBspScene::FreeMesh(SMesh *m)
{
	delete [] m->m_pVerts;
	delete m;	
}

void CBspScene::ProjectPointOntoVector(vector3 &point, vector3 &vStart, vector3 &vEnd, vector3 &vProj)
{
	vector3 pVec,vec;

	pVec=point-vStart;
	vec=vEnd-vStart;
	vec.Normalize();
	float f=pVec*vec;
	
	vProj=vStart+vec*f;	
}

SMesh* CBspScene::CopyMesh(SMesh *mesh)
{
	SMesh *out;
	//int size;

	out=new SMesh;
	out->m_nWidth=mesh->m_nWidth;
	out->m_nHeight=mesh->m_nHeight;

	//size=out->m_nWidth*out->m_nHeight*sizeof(*out->m_pVerts);
	
	out->m_pVerts=new BspVertex[out->m_nWidth*out->m_nHeight];

	memcpy(out->m_pVerts,mesh->m_pVerts,sizeof(BspVertex)*out->m_nWidth*out->m_nHeight);	

	return out;
}

void CBspScene::LerpDrawVert(BspVertex *a, BspVertex *b, BspVertex *out)
{
	out->v.v[0]=0.5f*(a->v.v[0]+b->v.v[0]);
	out->v.v[1]=0.5f*(a->v.v[1]+b->v.v[1]);
	out->v.v[2]=0.5f*(a->v.v[2]+b->v.v[2]);

	out->tu=0.5f*(a->tu+b->tu);
	out->tv=0.5f*(a->tv+b->tv);

	out->tu1=0.5f*(a->tu1+b->tu1);
	out->tv1=0.5f*(a->tv1+b->tv1);		
}

int CBspScene::FindShadowEdgeVertex(BspShadowVertex Vertex)
{
	for(int i=0;i<(int)m_SurfaceEdgeVertexList.size();i++)
	{
		if( m_SurfaceEdgeVertexList[i].v==Vertex.v && 
			m_SurfaceEdgeVertexList[i].n==Vertex.n &&
			fabsf(m_SurfaceEdgeVertexList[i].fWeight-Vertex.fWeight) <= 0.01f)
		{
			return i;
		}
	}
	return -1;
}
