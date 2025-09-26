#include "BSPBaseClass.h"
#include <stdio.h>

Poly* BSPBaseData::m_pPolygonArray;
Node *BSPBaseData::m_pNodeArray;
Leaf *BSPBaseData::m_pLeafArray;
Plane *BSPBaseData::m_pPlaneArray;
Portal **BSPBaseData::m_pPortalArray;
BYTE *BSPBaseData::m_pPVSData;

long BSPBaseData::m_nPolygons;
long BSPBaseData::m_nNodes;
long BSPBaseData::m_nLeafs;
long BSPBaseData::m_nPlanes;
long BSPBaseData::m_nPortals;

long BSPBaseData::m_nMaxNodes;
long BSPBaseData::m_nMaxPolygons;	
long BSPBaseData::m_nMaxPlanes;
long BSPBaseData::m_nMaxLeafs;
long BSPBaseData::m_nMaxPortals;

long BSPBaseData::m_BytePerSet;
long BSPBaseData::m_PVSCompressedSize;


BSPBaseData::BSPBaseData()
{
	
	m_pPolygonArray=NULL;
	m_pNodeArray=NULL;
	m_pLeafArray=NULL;
	m_pPlaneArray=NULL;
	m_pPortalArray=NULL;
	m_pPVSData=NULL;

	m_nPolygons=0;
	m_nNodes=0;
	m_nLeafs=0;
	m_nPlanes=0;
	m_nPortals=0;

	m_nMaxNodes=100;
	m_nMaxPolygons=100;	
	m_nMaxPlanes=100;
	m_nMaxLeafs=100;
	m_nMaxPortals=100;
	
}

void BSPBaseData::Create()
{
	m_pPolygonArray=(Poly*)malloc(m_nMaxPolygons*sizeof(Poly));
	m_pNodeArray=(Node*)malloc(m_nMaxNodes*sizeof(Node));
	m_pLeafArray=(Leaf*)malloc(m_nMaxLeafs*sizeof(Leaf));
	m_pPlaneArray=(Plane*)malloc(m_nMaxPlanes*sizeof(Plane));
	m_pPortalArray=(Portal**)malloc(m_nMaxPortals*sizeof(Portal));
	
	ZeroMemory(m_pNodeArray,sizeof(Node)*m_nMaxNodes);
	ZeroMemory(m_pPolygonArray,sizeof(Poly)*m_nMaxPolygons);
	ZeroMemory(m_pLeafArray,sizeof(Leaf)*m_nMaxLeafs);
	ZeroMemory(m_pPlaneArray,sizeof(Plane)*m_nMaxPlanes);
	ZeroMemory(m_pPortalArray,sizeof(Portal)*m_nMaxPortals);	
}

int BSPBaseData::ClassifyPoly(Plane *pPlane, Poly *pPoly)
{
	int Front=0;
	int Back=0;
	int OnPlane=0;
	float fResult;
	vector3 vecPoint1=pPlane->m_vecPointOnPlane;
	for(int i=0;i<pPoly->m_nVertex;i++)
	{
		vector3 vecPoint2=pPoly->m_pVertexList[i].v;
		vector3 vecDir=vecPoint1-vecPoint2;
		fResult=vecDir*pPlane->m_vecNormal;
		if(fResult>0.001f)
		{
			Back++;
		}
		else if(fResult<-0.001f)
		{
			Front++;
		}
		else
		{
			OnPlane++;
			Front++;
			Back++;
		}
	}
	if(OnPlane==pPoly->m_nVertex) return CP_ONPLANE;
	if(Back==pPoly->m_nVertex) return CP_BACK;
	if(Front==pPoly->m_nVertex) return CP_FRONT;
	return CP_SPANNING;
}

void BSPBaseData::SplitPolygon(Poly *pPoly, Plane *pPlane, Poly *pFrontSplit, Poly *pBackSplit)
{
	//LVertex FrontList[40],BackList[40],FirstVertex;
	//vector3 FrontList[40],BackList[40],FirstVertex;

	LVertex FrontList[40],BackList[40],FirstVertex;
	vector3 vecPlaneNormal,vecIntersectionPoint,vecPointOnPlane,vecPointA,vecPointB;
	unsigned long cFront=0,cBack=0,ulCurrentVertex=0;
	float fPercent;

	vecPointOnPlane=pPlane->m_vecPointOnPlane;
	vecPlaneNormal=pPlane->m_vecNormal;

	FirstVertex=pPoly->m_pVertexList[0];
	switch(ClassifyPoint(&FirstVertex.v,pPlane))
	{
		case CP_FRONT:
			FrontList[cFront++]=FirstVertex;
			break;
		case CP_BACK:
			BackList[cBack++]=FirstVertex;
			break;
		case CP_ONPLANE:
			BackList[cBack++]=FirstVertex;
			FrontList[cFront++]=FirstVertex;
			break;		
	}
	for(int i=1;i<pPoly->m_nVertex+1;i++)
	{
		if(i==pPoly->m_nVertex)
			ulCurrentVertex=0;
		else
			ulCurrentVertex=i;
		vecPointA=pPoly->m_pVertexList[i-1].v;
		vecPointB=pPoly->m_pVertexList[ulCurrentVertex].v;
		int Result=ClassifyPoint(&vecPointB,pPlane);
		if(Result==CP_ONPLANE)
		{
			BackList[cBack++]=pPoly->m_pVertexList[ulCurrentVertex];
			FrontList[cFront++]=pPoly->m_pVertexList[ulCurrentVertex];
		}		
		else if(GetIntersect(&vecPointA,&vecPointB,&vecPointOnPlane,&vecPlaneNormal,&vecIntersectionPoint,&fPercent)==true)
		{
			LVertex vecCpy;
			vecCpy.v=vecIntersectionPoint;

			float fDeltaU,fDeltaV,fTexU,fTexV;
			fDeltaU=pPoly->m_pVertexList[ulCurrentVertex].tu-pPoly->m_pVertexList[i-1].tu;
			fDeltaV=pPoly->m_pVertexList[ulCurrentVertex].tv-pPoly->m_pVertexList[i-1].tv;
			fTexU=pPoly->m_pVertexList[i-1].tu+(fDeltaU*fPercent);
			fTexV=pPoly->m_pVertexList[i-1].tv+(fDeltaV*fPercent);
			vecCpy.tu=fTexU;
			vecCpy.tv=fTexV;			

			if(Result==CP_FRONT)
			{
				BackList[cBack++]=vecCpy;
				FrontList[cFront++]=vecCpy;
				if(ulCurrentVertex!=0)
				{
					FrontList[cFront++]=pPoly->m_pVertexList[ulCurrentVertex];
				}
			}
			if(Result==CP_BACK)
			{
				FrontList[cFront++]=vecCpy;
				BackList[cBack++]=vecCpy;
				if(ulCurrentVertex!=0)
				{
					BackList[cBack++]=pPoly->m_pVertexList[ulCurrentVertex];
				}
			}
		}
		else
		{
			if(Result==CP_FRONT)
			{
				if(ulCurrentVertex!=0)
					FrontList[cFront++]=pPoly->m_pVertexList[ulCurrentVertex];
			}
			if(Result==CP_BACK)
			{
				if(ulCurrentVertex!=0)
					BackList[cBack++]=pPoly->m_pVertexList[ulCurrentVertex];
			}
		}
	}
	pFrontSplit->m_nVertex=0;
	pBackSplit->m_nVertex=0;

	pFrontSplit->m_pVertexList=new LVertex[cFront];
	pBackSplit->m_pVertexList=new LVertex[cBack];
	for(i=0;i<(int)cFront;i++)
	{
		pFrontSplit->m_nVertex++;
		pFrontSplit->m_pVertexList[i]=FrontList[i];
	}
	for(i=0;i<(int)cBack;i++)
	{
		pBackSplit->m_nVertex++;
		pBackSplit->m_pVertexList[i]=BackList[i];
	}
	pFrontSplit->m_nIndices=(pFrontSplit->m_nVertex-2)*3;
	pBackSplit->m_nIndices=(pBackSplit->m_nVertex-2)*3;

	pFrontSplit->m_TextureIndex=pPoly->m_TextureIndex;
	pBackSplit->m_TextureIndex=pPoly->m_TextureIndex;

	pFrontSplit->m_pIndices=new WORD[pFrontSplit->m_nIndices];
	pBackSplit->m_pIndices=new WORD[pBackSplit->m_nIndices];

	WORD v0,v1,v2;
	for(i=0;i<pFrontSplit->m_nIndices/3;i++)
	{
		if(i==0)
		{
			v0=0;
			v1=1;
			v2=2;
		}
		else
		{
			v1=v2;
			v2++;
		}
		pFrontSplit->m_pIndices[i*3+0]=v0;
		pFrontSplit->m_pIndices[i*3+1]=v1;
		pFrontSplit->m_pIndices[i*3+2]=v2;
	}
	for(i=0;i<pBackSplit->m_nIndices/3;i++)
	{
		if(i==0)
		{
			v0=0;
			v1=1;
			v2=2;
		}
		else
		{
			v1=v2;
			v2++;
		}
		pBackSplit->m_pIndices[i*3+0]=v0;
		pBackSplit->m_pIndices[i*3+1]=v1;
		pBackSplit->m_pIndices[i*3+2]=v2;
	}
	pFrontSplit->m_vecNormal=pPoly->m_vecNormal;
	pBackSplit->m_vecNormal=pPoly->m_vecNormal;
	/*			
	FrontSplit->Normal=Poly->Normal;
	BackSplit->Normal=Poly->Normal;
	*/
}

BSPBaseData::ClassifyPoint(vector3 *vecPoint, Plane *pPlane)
{
	float fResult;
	vector3 vecPointOnPlane=pPlane->m_vecPointOnPlane;
	vector3 vecDir=vecPointOnPlane-*vecPoint;	
	fResult=vecDir*pPlane->m_vecNormal;
	if(fResult<-0.001f)
		return CP_FRONT;
	if(fResult>0.001f)
		return CP_BACK;	
	return CP_ONPLANE;	
}

bool BSPBaseData::GetIntersect(vector3 *vecLineStart, vector3 *vecLineEnd, vector3 *pVertex, vector3 *vecNormal, vector3 *vecIntersection, float *fPercentage)
{
	vector3 vecDirection,vecL1;
	float fLineLength,fDistFromPlane;
	vecDirection=*vecLineEnd-*vecLineStart;
	fLineLength=vecDirection*(*vecNormal);
	if(fabsf(fLineLength)<0.0001f)
	{
		return false;
	}
	vecL1=*pVertex-*vecLineStart;
	fDistFromPlane=vecL1*(*vecNormal);
	*fPercentage=fDistFromPlane/fLineLength;
	if(*fPercentage<0.0f)
	{
		return false;
	}
	else
	{
		if(*fPercentage>1.0f)
		{
			return false;
		}
		else
		{
			*vecIntersection=*vecLineStart+vecDirection*(*fPercentage);
			return true;
		}			
	}	
}

void BSPBaseData::CalculateBox(BoundingBox *pBox,Poly *pPolyList)
{
	Poly *pPoly=pPolyList;
	while(pPoly!=NULL)
	{
		for(int i=0;i<pPoly->m_nVertex;i++)
		{
			if(pPoly->m_pVertexList[i].v.x<pBox->m_vecBoxMin.x)
				pBox->m_vecBoxMin.x=pPoly->m_pVertexList[i].v.x;
			if(pPoly->m_pVertexList[i].v.y<pBox->m_vecBoxMin.y)
				pBox->m_vecBoxMin.y=pPoly->m_pVertexList[i].v.y;
			if(pPoly->m_pVertexList[i].v.z<pBox->m_vecBoxMin.z)
				pBox->m_vecBoxMin.z=pPoly->m_pVertexList[i].v.z;
			
			if(pPoly->m_pVertexList[i].v.x>pBox->m_vecBoxMax.x)
				pBox->m_vecBoxMax.x=pPoly->m_pVertexList[i].v.x;
			if(pPoly->m_pVertexList[i].v.y>pBox->m_vecBoxMax.y)
				pBox->m_vecBoxMax.y=pPoly->m_pVertexList[i].v.y;
			if(pPoly->m_pVertexList[i].v.z>pBox->m_vecBoxMax.z)
				pBox->m_vecBoxMax.z=pPoly->m_pVertexList[i].v.z;
		}
		pPoly=pPoly->m_pNext;
	}	
}

void BSPBaseData::DeletePolygon(Poly *pPoly)
{
	delete [] pPoly->m_pVertexList;
	delete [] pPoly->m_pIndices;
	delete pPoly;
}

void BSPBaseData::IncreaseNumberOfNodes()
{
	m_nNodes++;
	if(m_nNodes==m_nMaxNodes)
	{
		m_nMaxNodes+=100;
		m_pNodeArray=(Node*)realloc(m_pNodeArray,m_nMaxNodes*sizeof(Node));
	}	
}

void BSPBaseData::IncreaseNumberOfPolygons()
{
	m_nPolygons++;
	if(m_nPolygons==m_nMaxPolygons)
	{
		m_nMaxPolygons+=100;
		m_pPolygonArray=(Poly*)realloc(m_pPolygonArray,m_nMaxPolygons*sizeof(Poly));
	}	
}

void BSPBaseData::IncreaseNumberOfPlanes()
{
	m_nPlanes++;
	if(m_nPlanes==m_nMaxPlanes)
	{
		m_nMaxPlanes+=100;
		m_pPlaneArray=(Plane*)realloc(m_pPlaneArray,m_nMaxPlanes*sizeof(Plane));
	}	
}

void BSPBaseData::IncreaseNumberOfLeafs()
{
	m_nLeafs++;
	if(m_nLeafs==m_nMaxLeafs)
	{
		m_nMaxLeafs+=100;
		m_pLeafArray=(Leaf*)realloc(m_pLeafArray,m_nMaxLeafs*sizeof(Leaf));
	}	
}

void BSPBaseData::IncreaseNumberOfPortals()
{
	m_nPortals++;
	if(m_nPortals==m_nMaxPortals)
	{
		m_nMaxPortals+=100;
		m_pPortalArray=(Portal**)realloc(m_pPortalArray,m_nMaxPortals*sizeof(Portal*));
	}	
}

void BSPBaseData::DeletePortal(Portal *pPortal)
{
	/*
	delete []Portal->VertexList;
	delete []Portal->Indices;
	delete Portal;
	*/
	delete [] pPortal->m_pVertexList;
	delete [] pPortal->m_pIndices;
	delete pPortal;
}

Poly* BSPBaseData::LoadMWM(char *strFilename)
{
	FILE *fp;
	Poly *pRoot=NULL;
	Poly *pChild=NULL;
	WORD cPoly,nVertex;
	int i,b;
	LVertex xyzBuffer[50];
	WORD TextureIndex;
	DWORD uselessinfo;
	fp=fopen(strFilename,"rb");
	fread(&cPoly,sizeof(WORD),1,fp);
	
	for(i=0;i<cPoly;i++)
	{
		fread(&nVertex,sizeof(WORD),1,fp);
		for(b=0;b<nVertex;b++)
		{
			fread(&xyzBuffer[b].v.x,sizeof(float),1,fp);
			fread(&xyzBuffer[b].v.y,sizeof(float),1,fp);
			fread(&xyzBuffer[b].v.z,sizeof(float),1,fp);
			fread(&uselessinfo,sizeof(DWORD),1,fp);
			fread(&xyzBuffer[b].diff.c,sizeof(DWORD),1,fp);
			fread(&xyzBuffer[b].spec.c,sizeof(DWORD),1,fp);
			fread(&xyzBuffer[b].tu,sizeof(float),1,fp);
			fread(&xyzBuffer[b].tv,sizeof(float),1,fp);			
		}
		fread(&TextureIndex,sizeof(WORD),1,fp);				
		if(i==0)
		{
			pRoot=AddPolygon(NULL,&xyzBuffer[0],nVertex);
			pRoot->m_TextureIndex=TextureIndex;
			pChild=pRoot;
		}
		else
		{
			pChild=AddPolygon(pChild,&xyzBuffer[0],nVertex);
			pChild->m_TextureIndex=TextureIndex;
		}
	}
	fclose(fp);
	
	return pRoot;
}

Poly* BSPBaseData::AddPolygon(Poly *pParent, LVertex *Vertices, WORD nVertex)
{	
	Poly *pChild=new Poly;
	pChild->m_nVertex=nVertex;
	pChild->m_nIndices=(nVertex-2)*3;
	pChild->m_pNext=NULL;
	pChild->m_bBeenUsedAsSplitter=false;

	pChild->m_pVertexList=new LVertex[pChild->m_nVertex];
	pChild->m_pIndices=new WORD[pChild->m_nIndices];

	for(int i=0;i<nVertex;i++)
	{
		pChild->m_pVertexList[i]=Vertices[i];
	}

	WORD v0,v1,v2;
	for(i=0;i<pChild->m_nIndices/3;i++)
	{
		if(i==0)
		{
			v0=0;
			v1=1;
			v2=2;
		}
		else
		{
			v1=v2;
			v2++;
		}
		pChild->m_pIndices[i*3+0]=v0;
		pChild->m_pIndices[i*3+1]=v1;
		pChild->m_pIndices[i*3+2]=v2;
	}
	vector3 vec0=pChild->m_pVertexList[0].v;
	vector3 vec1=pChild->m_pVertexList[1].v;
	vector3 vec2=pChild->m_pVertexList[pChild->m_nVertex-1].v;
	vector3 vecNormal=(vec1-vec0)^(vec2-vec0);
	vecNormal.Normalize();
	pChild->m_vecNormal=vecNormal;

	if(pParent!=NULL)
	{
		pParent->m_pNext=pChild;
	}
	return pChild;
	/*		
	
	// generate polygon normal
	D3DXVECTOR3 * vec0=(D3DXVECTOR3 *) &Child->VertexList[0];
	D3DXVECTOR3 * vec1=(D3DXVECTOR3 *) &Child->VertexList[1];
	D3DXVECTOR3 * vec2=(D3DXVECTOR3 *) &Child->VertexList[Child->NumberOfVertices-1];// the last vert
	
	
	D3DXVECTOR3 edge1=(*vec1)-(*vec0);
	D3DXVECTOR3 edge2=(*vec2)-(*vec0);
	D3DXVec3Cross(&Child->Normal,&edge1,&edge2);
	D3DXVec3Normalize(&Child->Normal,&Child->Normal);
	
	
	if (Parent!=NULL)
	{
		Parent->Next=Child;
	}
	
	return Child;
	*/
}


void BSPBaseData::CopyBspData(BSPBaseData *pData)
{
	m_nMaxPolygons=pData->m_nMaxPolygons;
	m_nMaxNodes=pData->m_nMaxNodes;
	m_nMaxLeafs=pData->m_nMaxLeafs;
	m_nMaxPlanes=pData->m_nMaxPlanes;
	m_nMaxPortals=pData->m_nMaxPortals;

	m_nPolygons=pData->m_nPolygons;
	m_nNodes=pData->m_nNodes;
	m_nLeafs=pData->m_nLeafs;
	m_nPlanes=pData->m_nPlanes;
	m_nPortals=pData->m_nPortals;

	m_pPolygonArray=(Poly*)malloc(m_nMaxPolygons*sizeof(Poly));
	m_pNodeArray=(Node*)malloc(m_nMaxNodes*sizeof(Node));
	m_pLeafArray=(Leaf*)malloc(m_nMaxLeafs*sizeof(Leaf));
	m_pPlaneArray=(Plane*)malloc(m_nMaxPlanes*sizeof(Plane));
	m_pPortalArray=(Portal**)malloc(m_nMaxPortals*sizeof(Portal));

	memcpy(m_pPolygonArray,pData->m_pPolygonArray,sizeof(Poly)*m_nMaxPolygons);
	memcpy(m_pNodeArray,pData->m_pNodeArray,sizeof(Node)*m_nMaxNodes);
	memcpy(m_pLeafArray,pData->m_pLeafArray,sizeof(Leaf)*m_nMaxLeafs);
	memcpy(m_pPlaneArray,pData->m_pPlaneArray,sizeof(Plane)*m_nMaxPlanes);
	memcpy(m_pPortalArray,pData->m_pPortalArray,sizeof(Portal)*m_nMaxPortals);
}

void BSPBaseData::SplitPortal(Portal *pPortal, Plane *pPlane, Portal *pFrontSplit, Portal *pBackSplit)
{	
	SplitPolygon((Poly*)pPortal,pPlane,(Poly*)pFrontSplit,(Poly*)pBackSplit);
	pFrontSplit->m_nLeafs=pPortal->m_nLeafs;
	pBackSplit->m_nLeafs=pPortal->m_nLeafs;	

	memcpy(pFrontSplit->m_LeafOwnerArray,pPortal->m_LeafOwnerArray,sizeof(long)*pPortal->m_nLeafs);
	memcpy(pBackSplit->m_LeafOwnerArray,pPortal->m_LeafOwnerArray,sizeof(long)*pPortal->m_nLeafs);	
}

void BSPBaseData::Save(char *strFilename,List<CQShader*> &ShaderList)
{
	FILE *fp=fopen(strFilename,"wb");

	fwrite(&ShaderList.num,sizeof(int),1,fp);
	for(int i=0;i<ShaderList.num;i++)
	{
		fwrite(ShaderList[i]->GetTextureName(),sizeof(char)*256,1,fp);
	}

	m_nNodes++;
	fwrite(&m_nNodes,sizeof(long),1,fp);

	for(i=0;i<m_nNodes;i++)
	{
		fwrite(&m_pNodeArray[i].m_bLeaf,sizeof(unsigned char),1,fp);
		fwrite(&m_pNodeArray[i].m_Plane,sizeof(unsigned long),1,fp);
		fwrite(&m_pNodeArray[i].m_Front,sizeof(unsigned long),1,fp);
		fwrite(&m_pNodeArray[i].m_Back,sizeof(unsigned long),1,fp);
	}
	
	fwrite(&m_nPlanes,sizeof(long),1,fp);
	fwrite(m_pPlaneArray,sizeof(Plane),m_nPlanes,fp);

	fwrite(&m_nLeafs,sizeof(long),1,fp);
	for(i=0;i<m_nLeafs;i++)
	{
		fwrite(&m_pLeafArray[i].m_StartPolygon,sizeof(long),1,fp);
		fwrite(&m_pLeafArray[i].m_EndPolygon,sizeof(long),1,fp);
		fwrite(&m_pLeafArray[i].m_PVSIndex,sizeof(long),1,fp);
		fwrite(&m_pLeafArray[i].m_BoundingBox,sizeof(BoundingBox),1,fp);
	}
	
	fwrite(&m_nPolygons,sizeof(long),1,fp);

	for(i=0;i<m_nPolygons;i++)
	{
		fwrite(&m_pPolygonArray[i].m_nVertex,sizeof(WORD),1,fp);
		fwrite(m_pPolygonArray[i].m_pVertexList,sizeof(LVertex),m_pPolygonArray[i].m_nVertex,fp);
		fwrite(&m_pPolygonArray[i].m_nIndices,sizeof(WORD),1,fp);
		fwrite(m_pPolygonArray[i].m_pIndices,sizeof(WORD),m_pPolygonArray[i].m_nIndices,fp);
		fwrite(&m_pPolygonArray[i].m_vecNormal,sizeof(vector3),1,fp);
		m_pPolygonArray[i].m_TextureIndex=0;
		fwrite(&m_pPolygonArray[i].m_TextureIndex,sizeof(DWORD),1,fp);
	}
	fwrite(&m_PVSCompressedSize,sizeof(long),1,fp);
	fwrite(m_pPVSData,sizeof(BYTE),m_PVSCompressedSize,fp);
	fclose(fp);
}

void BSPBaseData::Load(char *strFilename)
{
	FILE *fp=fopen(strFilename,"rb");

	fread(&m_nNodes,sizeof(long),1,fp);
	m_pNodeArray=(Node*)malloc(m_nNodes*sizeof(Node));
	for(int i=0;i<m_nNodes;i++)
	{
		fread(&m_pNodeArray[i].m_bLeaf,sizeof(unsigned char),1,fp);
		fread(&m_pNodeArray[i].m_Plane,sizeof(unsigned long),1,fp);
		fread(&m_pNodeArray[i].m_Front,sizeof(unsigned long),1,fp);
		fread(&m_pNodeArray[i].m_Back,sizeof(unsigned long),1,fp);
	}
	fread(&m_nPlanes,sizeof(long),1,fp);
	m_pPlaneArray=(Plane*)malloc(m_nPlanes*sizeof(Plane));
	fread(m_pPlaneArray,sizeof(Plane),m_nPlanes,fp);
	
	fread(&m_nLeafs,sizeof(long),1,fp);
	m_pLeafArray=(Leaf*)malloc(m_nLeafs*sizeof(Leaf));

	for(i=0;i<m_nLeafs;i++)
	{
		fread(&m_pLeafArray[i].m_StartPolygon,sizeof(long),1,fp);
		fread(&m_pLeafArray[i].m_EndPolygon,sizeof(long),1,fp);
		fread(&m_pLeafArray[i].m_PVSIndex,sizeof(long),1,fp);
		fread(&m_pLeafArray[i].m_BoundingBox,sizeof(BoundingBox),1,fp);
	}
	
	fread(&m_nPolygons,sizeof(long),1,fp);
	m_pPolygonArray=(Poly*)malloc(m_nPolygons*sizeof(Poly));
	for(i=0;i<m_nPolygons;i++)
	{
		fread(&m_pPolygonArray[i].m_nVertex,sizeof(WORD),1,fp);
		m_pPolygonArray[i].m_pVertexList=new LVertex[m_pPolygonArray[i].m_nVertex];
		fread(m_pPolygonArray[i].m_pVertexList,sizeof(LVertex),m_pPolygonArray[i].m_nVertex,fp);
		fread(&m_pPolygonArray[i].m_nIndices,sizeof(WORD),1,fp);
		m_pPolygonArray[i].m_pIndices=new WORD[m_pPolygonArray[i].m_nIndices];
		fread(m_pPolygonArray[i].m_pIndices,sizeof(WORD),m_pPolygonArray[i].m_nIndices,fp);
		fread(&m_pPolygonArray[i].m_vecNormal,sizeof(vector3),1,fp);
		fread(&m_pPolygonArray[i].m_TextureIndex,sizeof(DWORD),1,fp);
	}
	fread(&m_PVSCompressedSize,sizeof(long),1,fp);
	m_pPVSData=(BYTE*)malloc(m_PVSCompressedSize*sizeof(BYTE));
	fread(m_pPVSData,sizeof(BYTE),m_PVSCompressedSize,fp);
	fclose(fp);
}
