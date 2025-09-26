// Octree.cpp: implementation of the COctree class.
//
//////////////////////////////////////////////////////////////////////

#include "Octree.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MIN_EPSILON  1.0e-4f
#define MAX_EPSILON  1.0e+10f

COctree::COctree()
{
	for(int i=0;i<8;i++)
	{
		m_pChild[i]=NULL;
	}
	m_MaxBox.x=0.0f;
	m_MaxBox.y=0.0f;
	m_MaxBox.z=0.0f;

	m_MinBox.x=0.0f;
	m_MinBox.y=0.0f;
	m_MinBox.z=0.0f;

}

COctree::~COctree()
{
	for(int i=0;i<8;i++)
	{
		if(m_pChild[i])
		{
			delete m_pChild[i];
			//CSceneManager::_ReleaseObj(CROSSM::POOL_OCTREEOBJ,m_pChild[i]);
			m_pChild[i] = NULL;
		}
	}
}
void COctree::Load(CROSSM::CZ3DByteDataObj *pDataObj)
{
	if(pDataObj == NULL)
		return;

	unsigned char HaveChild=0x0;
	pDataObj->Read(&HaveChild,sizeof(unsigned char),1);
	
	for(int i=0;i<8;i++)
	{
		if(HaveChild & 0x1<<i)
		{
			m_pChild[i]=new COctree();
			//m_pChild[i] = (COctree *)CSceneManager::_CreateObj(CROSSM::POOL_OCTREEOBJ);
		}
		else
		{
			m_pChild[i] = NULL;
		}
	}

	pDataObj->Read(&m_MaxBox,sizeof(vector3),1);
	pDataObj->Read(&m_MinBox,sizeof(vector3),1);

	int cPolyList;
	pDataObj->Read(&cPolyList,sizeof(int),1);
	int NodeID;
	for(int i=0;i<cPolyList;i++)
	{
		pDataObj->Read(&NodeID,sizeof(int),1);
		m_PolyNodeID.push_back(NodeID);		
	}
	for(int i=0;i<8;i++)
	{
		if(m_pChild[i])
			m_pChild[i]->Load(pDataObj);
	}	
}
void COctree::Load(FILE *fp)
{
	
	unsigned char HaveChild=0x0;
	fread(&HaveChild,sizeof(unsigned char),1,fp);
	for(int i=0;i<8;i++)
	{
		if(HaveChild & 0x1<<i)
		{
			m_pChild[i]=new COctree();
			//m_pChild[i] = (COctree *)CSceneManager::_CreateObj(CROSSM::POOL_OCTREEOBJ);
		}
	}
	fread(&m_MaxBox,sizeof(vector3),1,fp);
	fread(&m_MinBox,sizeof(vector3),1,fp);

	int cPolyList;
	fread(&cPolyList,sizeof(int),1,fp);
	int NodeID;
	for(int i=0;i<cPolyList;i++)
	{
		fread(&NodeID,sizeof(int),1,fp);
		m_PolyNodeID.push_back(NodeID);		
	}
	for(int i=0;i<8;i++)
	{
		if(m_pChild[i])
			m_pChild[i]->Load(fp);
	}	
}

//DEL void COctree::AddPoly(vector3 *vecPoly[])
//DEL {	
//DEL }

void COctree::SplitNode(int DepthNode,std::vector<PolyNode *> &PolyList)
{
	if(DepthNode==0)
		return;
	float fBoxSizeX,fBoxSizeY,fBoxSizeZ;
	fBoxSizeX=m_MaxBox.x-m_MinBox.x;
	fBoxSizeY=m_MaxBox.y-m_MinBox.y;
	fBoxSizeZ=m_MaxBox.z-m_MinBox.z;
	int BoxMoveX[8]={1,0,1,0,1,0,1,0};
	int BoxMoveY[8]={1,1,1,1,0,0,0,0};
	int BoxMoveZ[8]={1,1,0,0,1,1,0,0};
	vector3 vecBoundBox[8];
	vector3 vecBoundPlaneNormal[6];
	float fDistance[6];
	for(int cNode=0;cNode<8;cNode++)
	{
		m_pChild[cNode]=new COctree();
		//m_pChild[cNode] = (COctree *)CSceneManager::_CreateObj(CROSSM::POOL_OCTREEOBJ);

		for(int cBoxVertex=0;cBoxVertex<8;cBoxVertex++)
		{
			vecBoundBox[cBoxVertex].x=m_MinBox.x+(BoxMoveX[cNode]*fBoxSizeX*0.5f)+(BoxMoveX[cBoxVertex]*fBoxSizeX*0.5f);
			vecBoundBox[cBoxVertex].y=m_MinBox.y+(BoxMoveY[cNode]*fBoxSizeY*0.5f)+(BoxMoveY[cBoxVertex]*fBoxSizeY*0.5f);
			vecBoundBox[cBoxVertex].z=m_MinBox.z+(BoxMoveZ[cNode]*fBoxSizeZ*0.5f)+(BoxMoveZ[cBoxVertex]*fBoxSizeZ*0.5f);
		}
		m_pChild[cNode]->m_MaxBox=vecBoundBox[0];
		m_pChild[cNode]->m_MinBox=vecBoundBox[7];
		
		
		vecBoundPlaneNormal[0]=(vecBoundBox[4]-vecBoundBox[0])^(vecBoundBox[5]-vecBoundBox[4]);		
		vecBoundPlaneNormal[1]=(vecBoundBox[3]-vecBoundBox[2])^(vecBoundBox[7]-vecBoundBox[3]);
		vecBoundPlaneNormal[2]=(vecBoundBox[2]-vecBoundBox[0])^(vecBoundBox[6]-vecBoundBox[2]);
		vecBoundPlaneNormal[3]=(vecBoundBox[1]-vecBoundBox[3])^(vecBoundBox[5]-vecBoundBox[1]);
		vecBoundPlaneNormal[4]=(vecBoundBox[1]-vecBoundBox[0])^(vecBoundBox[3]-vecBoundBox[1]);
		vecBoundPlaneNormal[5]=(vecBoundBox[7]-vecBoundBox[6])^(vecBoundBox[5]-vecBoundBox[7]);			
		
		
		vecBoundPlaneNormal[0].Normalize();
		vecBoundPlaneNormal[1].Normalize();
		vecBoundPlaneNormal[2].Normalize();
		vecBoundPlaneNormal[3].Normalize();
		vecBoundPlaneNormal[4].Normalize();
		vecBoundPlaneNormal[5].Normalize();
		for(int cPoly=0;cPoly<(int)m_PolyNodeID.size();cPoly++)		
		{
			//vector3 *v=&m_PolyList[cPoly]->m_vecPoly[0];
			vector3 *v=&PolyList[m_PolyNodeID[cPoly]]->m_vecPoly[0];
			fDistance[0]=vecBoundPlaneNormal[0]*(*v-vecBoundBox[0]);
			fDistance[1]=vecBoundPlaneNormal[1]*(*v-vecBoundBox[2]);
			fDistance[2]=vecBoundPlaneNormal[2]*(*v-vecBoundBox[0]);
			fDistance[3]=vecBoundPlaneNormal[3]*(*v-vecBoundBox[3]);
			fDistance[4]=vecBoundPlaneNormal[4]*(*v-vecBoundBox[0]);
			fDistance[5]=vecBoundPlaneNormal[5]*(*v-vecBoundBox[6]);
			if( fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f && fDistance[3] > 0.0f &&	fDistance[4] > 0.0f && fDistance[5] > 0.0f)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}
			v=&PolyList[m_PolyNodeID[cPoly]]->m_vecPoly[1];
			//v=&m_PolyList[cPoly]->m_vecPoly[1];
			fDistance[0]=vecBoundPlaneNormal[0]*(*v-vecBoundBox[0]);
			fDistance[1]=vecBoundPlaneNormal[1]*(*v-vecBoundBox[2]);
			fDistance[2]=vecBoundPlaneNormal[2]*(*v-vecBoundBox[0]);
			fDistance[3]=vecBoundPlaneNormal[3]*(*v-vecBoundBox[3]);
			fDistance[4]=vecBoundPlaneNormal[4]*(*v-vecBoundBox[0]);
			fDistance[5]=vecBoundPlaneNormal[5]*(*v-vecBoundBox[6]);
			if( fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f && fDistance[3] > 0.0f &&	fDistance[4] > 0.0f && fDistance[5] > 0.0f)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}
			v=&PolyList[m_PolyNodeID[cPoly]]->m_vecPoly[2];
			//v=&m_PolyList[cPoly]->m_vecPoly[2];
			fDistance[0]=vecBoundPlaneNormal[0]*(*v-vecBoundBox[0]);
			fDistance[1]=vecBoundPlaneNormal[1]*(*v-vecBoundBox[2]);
			fDistance[2]=vecBoundPlaneNormal[2]*(*v-vecBoundBox[0]);
			fDistance[3]=vecBoundPlaneNormal[3]*(*v-vecBoundBox[3]);
			fDistance[4]=vecBoundPlaneNormal[4]*(*v-vecBoundBox[0]);
			fDistance[5]=vecBoundPlaneNormal[5]*(*v-vecBoundBox[6]);
			if( fDistance[0] > 0.0f && fDistance[1] > 0.0f && fDistance[2] > 0.0f && fDistance[3] > 0.0f &&	fDistance[4] > 0.0f && fDistance[5] > 0.0f)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}
			vector3 vecCheckBox[3];
			vecCheckBox[0]=vecBoundBox[0];
			vecCheckBox[1]=vecBoundBox[4];
			vecCheckBox[2]=vecBoundBox[5];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}
			vecCheckBox[0]=vecBoundBox[0];
			vecCheckBox[1]=vecBoundBox[5];
			vecCheckBox[2]=vecBoundBox[1];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[2];
			vecCheckBox[1]=vecBoundBox[3];
			vecCheckBox[2]=vecBoundBox[7];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[2];
			vecCheckBox[1]=vecBoundBox[7];
			vecCheckBox[2]=vecBoundBox[6];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[0];
			vecCheckBox[1]=vecBoundBox[2];
			vecCheckBox[2]=vecBoundBox[6];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[3];
			vecCheckBox[1]=vecBoundBox[1];
			vecCheckBox[2]=vecBoundBox[5];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[3];
			vecCheckBox[1]=vecBoundBox[5];
			vecCheckBox[2]=vecBoundBox[7];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)			
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[0];
			vecCheckBox[1]=vecBoundBox[1];
			vecCheckBox[2]=vecBoundBox[3];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[0];
			vecCheckBox[1]=vecBoundBox[3];
			vecCheckBox[2]=vecBoundBox[2];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[6];
			vecCheckBox[1]=vecBoundBox[7];
			vecCheckBox[2]=vecBoundBox[5];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);				
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

			vecCheckBox[0]=vecBoundBox[6];
			vecCheckBox[1]=vecBoundBox[5];
			vecCheckBox[2]=vecBoundBox[4];
			if(CIntersection::PolygonToPolygon(PolyList[m_PolyNodeID[cPoly]]->m_vecPoly,vecCheckBox)==1)
			//if(CIntersection::PolygonToPolygon(m_PolyList[cPoly]->m_vecPoly,vecCheckBox)==1)
			{
				m_pChild[cNode]->m_PolyNodeID.push_back(m_PolyNodeID[cPoly]);
				//m_pChild[cNode]->m_PolyList.Add(m_PolyList[cPoly]);
				continue;
			}

		}
		if(m_pChild[cNode]->m_PolyNodeID.size()==0)
		{
			delete m_pChild[cNode];
			//CSceneManager::_ReleaseObj(CROSSM::POOL_OCTREEOBJ,m_pChild[cNode]);
			m_pChild[cNode]=NULL;
		}
		else
		{
			m_pChild[cNode]->SplitNode(DepthNode-1,PolyList);
		}

	}			
	if(DepthNode!=0)
	{
		m_PolyNodeID.clear();
		//m_PolyList.num=0;
	}
}

void COctree::CalcBoundBox(std::vector<PolyNode *> &PolyList)
{
	float minx,miny,minz,maxx,maxy,maxz;
	if(PolyList.size()==0)
		return;

	maxx=minx=PolyList[0]->m_vecPoly[0].x;
	maxy=miny=PolyList[0]->m_vecPoly[0].y;
	maxz=minz=PolyList[0]->m_vecPoly[0].z;
	
	for(int i=0;i<(int)PolyList.size();i++)
	{
		for(int cv=0;cv<3;cv++)
		{
			if(maxx<PolyList[i]->m_vecPoly[cv].x)
				maxx=PolyList[i]->m_vecPoly[cv].x;		

			if(maxy<PolyList[i]->m_vecPoly[cv].y)
				maxy=PolyList[i]->m_vecPoly[cv].y;

			if(maxz<PolyList[i]->m_vecPoly[cv].z)
				maxz=PolyList[i]->m_vecPoly[cv].z;

			if(minx>PolyList[i]->m_vecPoly[cv].x)
				minx=PolyList[i]->m_vecPoly[cv].x;		

			if(miny>PolyList[i]->m_vecPoly[cv].y)
				miny=PolyList[i]->m_vecPoly[cv].y;

			if(minz>PolyList[i]->m_vecPoly[cv].z)
				minz=PolyList[i]->m_vecPoly[cv].z;
		}
	}
	const float Remain=100.0f;
	m_MaxBox.x=maxx+Remain;
	m_MaxBox.y=maxy+Remain;
	m_MaxBox.z=maxz+Remain;
	m_MinBox.x=minx-Remain;
	m_MinBox.y=miny-Remain;
	m_MinBox.z=minz-Remain;
}

void COctree::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	/*
	MultiVertex BoxVertex[8];

	WORD Indices[24];
	WORD *pIndices=Indices;
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
	
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);	
	pd3dDevice->SetVertexShader(MultiFVF);	
	
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,8,12,Indices,D3DFMT_INDEX16,BoxVertex,sizeof(MultiVertex));   	

	for(int cNode=0;cNode<8;cNode++)
	{
		if(m_pChild[cNode]!=NULL)
		{
			m_pChild[cNode]->Render(pd3dDevice);
		}
	}
	*/
}

//DEL void COctree::CullRay(vector3 vecStart, vector3 vecEnd,std::vector<PolyNode*> &PolyList)
//DEL {
//DEL 	/*
//DEL 	int BoxMoveX[8]={1,0,1,0,1,0,1,0};
//DEL 	int BoxMoveY[8]={1,1,1,1,0,0,0,0};
//DEL 	int BoxMoveZ[8]={1,1,0,0,1,1,0,0};		
//DEL 
//DEL 	vector3 vecSourceBox[8];
//DEL 	vector3 vecSourceSize=m_MaxBox-m_MinBox;	
//DEL 
//DEL 	for(int cBoxVertex=0;cBoxVertex<8;cBoxVertex++)
//DEL 	{
//DEL 		vecSourceBox[cBoxVertex].x=m_MinBox.x+(BoxMoveX[cBoxVertex]*vecSourceSize.x);		
//DEL 		vecSourceBox[cBoxVertex].y=m_MinBox.y+(BoxMoveY[cBoxVertex]*vecSourceSize.y);		
//DEL 		vecSourceBox[cBoxVertex].z=m_MinBox.z+(BoxMoveZ[cBoxVertex]*vecSourceSize.z);		
//DEL 	}
//DEL 	vector3 *vecIntersectionPoly[24];
//DEL 	vector3 **pVec=vecIntersectionPoly;
//DEL 	*(pVec++)=&vecSourceBox[0];
//DEL 	*(pVec++)=&vecSourceBox[1];
//DEL 	*(pVec++)=&vecSourceBox[3];
//DEL 	*(pVec++)=&vecSourceBox[2];
//DEL 
//DEL 	*(pVec++)=&vecSourceBox[4];
//DEL 	*(pVec++)=&vecSourceBox[5];
//DEL 	*(pVec++)=&vecSourceBox[7];
//DEL 	*(pVec++)=&vecSourceBox[6];
//DEL 
//DEL 	*(pVec++)=&vecSourceBox[0];
//DEL 	*(pVec++)=&vecSourceBox[1];
//DEL 	*(pVec++)=&vecSourceBox[5];
//DEL 	*(pVec++)=&vecSourceBox[4];
//DEL 
//DEL 	*(pVec++)=&vecSourceBox[2];
//DEL 	*(pVec++)=&vecSourceBox[3];
//DEL 	*(pVec++)=&vecSourceBox[7];
//DEL 	*(pVec++)=&vecSourceBox[6];
//DEL 
//DEL 	*(pVec++)=&vecSourceBox[0];
//DEL 	*(pVec++)=&vecSourceBox[2];
//DEL 	*(pVec++)=&vecSourceBox[6];
//DEL 	*(pVec++)=&vecSourceBox[4];
//DEL 
//DEL 	*(pVec++)=&vecSourceBox[1];
//DEL 	*(pVec++)=&vecSourceBox[3];
//DEL 	*(pVec++)=&vecSourceBox[7];
//DEL 	*(pVec++)=&vecSourceBox[5];
//DEL 	pVec=vecIntersectionPoly;
//DEL 	
//DEL 	float fInter;
//DEL 	if( CIntersection::BoxToRay(vecStart,vecEnd,pVec,fInter) ||
//DEL 		CIntersection::BoxToRay(vecStart,vecEnd,pVec+4,fInter) ||
//DEL 		CIntersection::BoxToRay(vecStart,vecEnd,pVec+8,fInter) ||
//DEL 		CIntersection::BoxToRay(vecStart,vecEnd,pVec+12,fInter) ||
//DEL 		CIntersection::BoxToRay(vecStart,vecEnd,pVec+16,fInter) ||
//DEL 		CIntersection::BoxToRay(vecStart,vecEnd,pVec+20,fInter)	)
//DEL 	{
//DEL 	
//DEL 		for(int cPoly=0;cPoly<m_PolyList.num;cPoly++)
//DEL 		{
//DEL 			if(m_PolyList[cPoly]->m_used==0)
//DEL 			{
//DEL 				PolyList.Add(m_PolyList[cPoly]);		
//DEL 				m_PolyList[cPoly]->m_used++;
//DEL 			}
//DEL 		}
//DEL 		for(int cNode=0;cNode<8;cNode++)
//DEL 		{
//DEL 			if(m_pChild[cNode])
//DEL 				m_pChild[cNode]->CullRay(vecStart,vecEnd,PolyList);
//DEL 		}
//DEL 
//DEL 	//}	
//DEL 	*/
//DEL }

void COctree::CullSphere(vector3 vecCenter, float fRad,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID)
{	
	vector3 vecBoundCenter=m_MinBox+(m_MaxBox-m_MinBox)/2.0f;
	vector3 vecDiagol=m_MaxBox-vecBoundCenter;
	float fBoundRad=vecDiagol.GetLens();
	vector3 vecInter=vecCenter-vecBoundCenter;
	float fInter=vecInter.GetLens();
	if( fInter < fRad+fBoundRad)
	{
		for(int cPoly=0;cPoly<(int)m_PolyNodeID.size();cPoly++)
		{
			if(PolyList[m_PolyNodeID[cPoly]]->m_used==0)
			{
				PolyID.push_back(m_PolyNodeID[cPoly]);				
				PolyList[m_PolyNodeID[cPoly]]->m_used++;
			}
		}
		for(int cNode=0;cNode<8;cNode++)
		{
			if(m_pChild[cNode])
				m_pChild[cNode]->CullSphere(vecCenter,fRad,PolyList,PolyID);
		}
	}	
}

void COctree::Save(FILE *fp)
{	
	unsigned char HaveChild=0x0;
	for(int i=0;i<8;i++)
	{
		if(m_pChild[i])
		{
			HaveChild|=(0x01<<i);
		}
	}	
	fwrite(&HaveChild,sizeof(unsigned char),1,fp);
	fwrite(&m_MaxBox,sizeof(vector3),1,fp);
	fwrite(&m_MinBox,sizeof(vector3),1,fp);
	//fwrite(&m_PolyList.num,sizeof(int),1,fp);

	int iCnt = m_PolyNodeID.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int i=0;i<(int)m_PolyNodeID.size();i++)
	{		
		fwrite(&m_PolyNodeID[i],sizeof(int),1,fp);
		//fwrite(m_PolyList[i]->m_vecPoly,sizeof(vector3)*3,1,fp);
	}
	for(int i=0;i<8;i++)
	{
		if(m_pChild[i])
		{
			m_pChild[i]->Save(fp);
		}
	}	
}

void COctree::CullFrustum(CViewCamera *pCamera,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID)
{
	if( CIntersection::PlaneAABBBox(pCamera->m_vecFrustumTop,-pCamera->m_vecFrustumNormal[0],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumBottom,-pCamera->m_vecFrustumNormal[1],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumLeft,-pCamera->m_vecFrustumNormal[2],m_MaxBox,m_MinBox) < 0.0f &&
		CIntersection::PlaneAABBBox(pCamera->m_vecFrustumRight,-pCamera->m_vecFrustumNormal[3],m_MaxBox,m_MinBox) < 0.0f)
	{
		for(int cPoly=0;cPoly<(int)m_PolyNodeID.size();cPoly++)
		{
			if(PolyList[m_PolyNodeID[cPoly]]->m_used==0)
			{
				PolyID.push_back(m_PolyNodeID[cPoly]);
				PolyList[m_PolyNodeID[cPoly]]->m_used++;
			}
		}
		for(int cNode=0;cNode<8;cNode++)
		{
			if(m_pChild[cNode])
				m_pChild[cNode]->CullFrustum(pCamera,PolyList,PolyID);
		}
	}	
}

void COctree::CullRay(vector3 vecStart,vector3 vecDir, float fLens,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID)
{
	vector3 vecEnd=vecStart+vecDir*fLens;
	if(vecDir.x >= 0.0f)	
	{
		if(m_MaxBox.x < vecStart.x)
			return;
		if(m_MinBox.x > vecEnd.x)
			return;	
		/*
		fRate=(m_MinBox.x-vecStart.x)/vecDir.x;
		if(fRate<0.0f)
			vecStart=vecStart+fRate*vecDir;		

		fRate=(m_MaxBox.x-vecEnd.x)/vecDir.x;
		if(fRate>0.0f)
			vecEnd=vecEnd-fRate*vecDir;			
			*/
	}
	else
	{
		if(m_MaxBox.x < vecEnd.x)
			return;
		if(m_MinBox.x > vecStart.x)
			return;
		/*
		fRate=(m_MinBox.x-vecEnd.x)/vecDir.x;
		if(fRate<0.0f)
			vecStart=vecEnd+fRate*vecDir;		

		fRate=(m_MaxBox.x-vecStart.x)/vecDir.x;
		if(fRate>0.0f)
			vecEnd=vecStart-fRate*vecDir;		
			*/
	}		
	if(vecDir.y >= 0.0f)
	{
		if(m_MaxBox.y < vecStart.y)
			return;
		if(m_MinBox.y > vecEnd.y)
			return;	
		/*
		fRate=(m_MinBox.y-vecStart.x)/vecDir.y;
		if(fRate<0.0f)
			vecStart=vecStart+fRate*vecDir;		

		fRate=(m_MaxBox.y-vecEnd.y)/vecDir.y;
		if(fRate>0.0f)
			vecEnd=vecEnd-fRate*vecDir;			
			*/
	}
	else
	{
		if(m_MaxBox.y < vecEnd.y)
			return;
		if(m_MinBox.y > vecStart.y)
			return;
		/*
		fRate=(m_MinBox.y-vecEnd.y)/vecDir.y;
		if(fRate<0.0f)
			vecStart=vecEnd+fRate*vecDir;		

		fRate=(m_MaxBox.y-vecStart.y)/vecDir.x;
		if(fRate>0.0f)
			vecEnd=vecStart-fRate*vecDir;		
			*/
	}
	if(vecDir.z>=0.0f)
	{
		if(m_MaxBox.z < vecStart.z)
			return;
		if(m_MinBox.z > vecEnd.z)	
			return;	
	}
	else
	{
		if(m_MaxBox.z < vecEnd.z)
			return;
		if(m_MinBox.z > vecStart.z)
			return;
	}

	for(int cPoly=0;cPoly<(int)m_PolyNodeID.size();cPoly++)
	{
		if(PolyList[m_PolyNodeID[cPoly]]->m_used==0)
		{
			PolyID.push_back(m_PolyNodeID[cPoly]);
			PolyList[m_PolyNodeID[cPoly]]->m_used++;
		}
	}

	for(int cNode=0;cNode<8;cNode++)
	{		
		if(m_pChild[cNode])
			m_pChild[cNode]->CullRay(vecStart,vecDir,fLens,PolyList,PolyID);			
	}
}
int	COctree::IntersectionRay(const D3DXVECTOR3 &vecRayOrigin,const D3DXVECTOR3 &vecRayDir,float &fNear,float &fFar)
{
	fNear = -MAX_EPSILON;
	fFar = MAX_EPSILON;
	
	float t1,t2,t;
	int iReturnValue = -1;

	int i,j;
	for(i = 0; i < 3; i++)
	{
		float fMinValue,fMaxValue;
		switch(i)
		{
		case 0:
			fMinValue = m_MinBox.x;
			fMaxValue = m_MaxBox.x;
			break;
		case 1:
			fMinValue = m_MinBox.y;
			fMaxValue = m_MaxBox.y;
			break;
		case 2:
			fMinValue = m_MinBox.z;
			fMaxValue = m_MaxBox.z;
			break;

		};
		if(vecRayDir[i] > -MIN_EPSILON && vecRayDir[i] < MIN_EPSILON)
		{
			
			if(vecRayOrigin[i] < fMinValue || vecRayOrigin[i] > fMaxValue)
				return -1;
		}
		else
		{
			
			t1=(fMinValue-vecRayOrigin[i])/vecRayDir[i];
			t2=(fMaxValue-vecRayOrigin[i])/vecRayDir[i];
			if (t1>t2)
			{ 
				t=t1; t1=t2; t2=t; 
				j=3+i;
			}
			else
				j=i;
			if (t1>fNear)
			{
				fNear=t1;
				iReturnValue=j;
			}
			if (t2<fFar)
				fFar=t2;
			if (fNear>fFar || fFar<MIN_EPSILON)
				return -1;
		}
	}
	if (fNear>fFar || fFar<MIN_EPSILON)
		return -1;

	return iReturnValue;
}
