// WBLightMapGenerator.cpp: implementation of the WBLightMapGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "WBLightMapGenerator.h"
#include "WBLightMapBuild.h"
#include "GMMemory.h"

int WBLightMapGenerator::m_iFaces;
vector<WBLightMapFace *> WBLightMapGenerator::m_lstFaces;

int WBLightMapGenerator::m_iLightMapPixel = 32;
int WBLightMapGenerator::m_iLightMapSize = 256;
//int WBLightMapGenerator::m_iLightMapAmb = 16;
int WBLightMapGenerator::m_iLightMapAmb = 14;
int WBLightMapGenerator::m_iLightSamples = 10;
//int WBLightMapGenerator::m_iLightJigg = 60;
int WBLightMapGenerator::m_iLightJigg = 60;
//float WBLightMapGenerator::m_fExposure = 0.09f;
float WBLightMapGenerator::m_fExposure = 5.5f;
int WBLightMapGenerator::m_iShadowSamples = 10;
int WBLightMapGenerator::m_iShadowFull = 0;
float WBLightMapGenerator::m_fShadowDist = 1000.0f;
//float WBLightMapGenerator::m_fShadowFactor = 0.95f;
float WBLightMapGenerator::m_fShadowFactor = 0.85f;

float *WBLightMapGenerator::emm = NULL;
float *WBLightMapGenerator::rad = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int ordenar_por_area_desc(const void *elem1, const void *elem2)
{
	struct reta *r1, *r2;

	r1=*(struct reta **)elem1;
	r2=*(struct reta **)elem2;

	return r2->sizex*r2->sizey-r1->sizex*r1->sizey;
}

retb::retb()
{
	nl=0;
	l=(struct reta **)malloc(0);
}

retb::~retb()
{
	free(l);
}

void retb::add(struct reta *r)
{
	l=(struct reta **)realloc(l, (++nl)*sizeof(struct reta *));
	l[nl-1]=r;
}

int retb::calc(int max_sx, int max_sy)
{
	struct reta r;

	r.sizex=max_sx;
	r.sizey=max_sy;
	r.offsetx=0;
	r.offsety=0;
	r.offsetz=0;

	for(int i=0; i<nl; i++) l[i]->offsetz=-1;

	qsort((void *)l, (size_t)nl, sizeof(struct reta *), ordenar_por_area_desc);

	while(arrumar(&r)) r.offsetz++;

	return r.offsetz;
}

bool retb::arrumar(struct reta *rp)
{
	int i, dsx, dsy;
	struct reta r[2];
	
	if((i=procura(rp->sizex, rp->sizey))==nl) return false; 

	l[i]->offsetx=rp->offsetx;
	l[i]->offsety=rp->offsety;
	l[i]->offsetz=rp->offsetz;

	dsx=rp->sizex-l[i]->sizex;
	dsy=rp->sizey-l[i]->sizey;

	if(dsx*rp->sizey>dsy*rp->sizex)
	{
		r[0].sizex=dsx;
		r[0].sizey=rp->sizey;
		r[0].offsetx=rp->offsetx+l[i]->sizex;
		r[0].offsety=rp->offsety;
		r[0].offsetz=rp->offsetz;
		r[1].sizex=l[i]->sizex;
		r[1].sizey=dsy;
		r[1].offsetx=rp->offsetx;
		r[1].offsety=rp->offsety+l[i]->sizey;
		r[1].offsetz=rp->offsetz;
	}
	else
	{
		r[0].sizex=rp->sizex;
		r[0].sizey=dsy;
		r[0].offsetx=rp->offsetx;
		r[0].offsety=rp->offsety+l[i]->sizey;
		r[0].offsetz=rp->offsetz;
		r[1].sizex=dsx;
		r[1].sizey=l[i]->sizey;
		r[1].offsetx=rp->offsetx+l[i]->sizex;
		r[1].offsety=rp->offsety;
		r[1].offsetz=rp->offsetz;
	}

	for(i=0; i<2; i++) arrumar(&r[i]);

	return true;
}

int retb::procura(int sx, int sy)
{
	int i;

	for(i=0;i<nl;i++)
		if(l[i]->offsetz==-1 &&
			l[i]->sizex<=sx && l[i]->sizey<=sy)
			break;

	return i;
}

WBLightMapGenerator::WBLightMapGenerator()
{
	CleanStatic();

	m_iFaces = 0;
	m_lstFaces.clear();

	m_iLightMaps = 0;
	m_lstLightMaps.clear();

	m_iLightMapTexs = 0;
	m_lstLightMapTexs.clear();

	m_iLights = 0;
	m_lstLights.clear();

	m_iLightMapPixel = 32;
	m_iLightMapSize = 256;
}

WBLightMapGenerator::~WBLightMapGenerator()
{
	int i;

	if(m_iFaces != 0)
	{
		for( i = m_lstFaces.size() - 1; i >= 0; i-- )
		{
			delete m_lstFaces[i];
			m_lstFaces[i] = NULL;
			m_lstFaces.erase(m_lstFaces.begin() + i);
		}
		m_lstFaces.clear();
	}
	if(m_iLightMaps != 0)
	{
		for( i = m_lstLightMaps.size() - 1; i >= 0; i-- )
		{
			delete m_lstLightMaps[i];
			m_lstLightMaps[i] = NULL;
			m_lstLightMaps.erase(m_lstLightMaps.begin() + i);
		}
		m_lstLightMaps.clear();
	}
	if(m_iLightMapTexs != 0)
	{
		for( i = m_lstLightMapTexs.size() - 1; i >= 0; i-- )
		{
			delete m_lstLightMapTexs[i];
			m_lstLightMapTexs[i] = NULL;
			m_lstLightMapTexs.erase(m_lstLightMapTexs.begin() + i);
		}
		m_lstLightMapTexs.clear();
	}
	
	for( i = 0; i < m_iLights; i++ )
	{
		m_lstLights[i] = NULL;
	}
	m_iLights = 0;
	m_lstLights.clear();

}
void WBLightMapGenerator::InputLight(WBLight *pLight)
{
	m_lstLights.push_back(pLight);
	m_iLights++;
}
void WBLightMapGenerator::InputFace(WBLightMapFace *pFace)
{

//	pFace->CalcNormal();
	m_lstFaces.push_back(pFace);
	m_iFaces++;

}


bool WBLightMapGenerator::LightMapGenerate(int iValue)
{
	int i,j,l;//,k;
	vector<int> *lstFaces;
	vector<WBLightMapPlane> lstPlanes;
	vector<int>	lstPlaneFaces;
	
	float fComp = 3.0f;

	if( iValue == 1)
		fComp = 4.71f;

	for( i = 0; i < m_iFaces; i++ ) 
	{
		bool bNew = true;
		for(int j = 0; j < (int)lstPlanes.size(); j++ )
		{
			if(fabs(lstPlanes[j].m_vecNormal.x - m_lstFaces[i]->m_vecNormal.x) < 0.04f)
			{
				if(fabs(lstPlanes[j].m_vecNormal.y - m_lstFaces[i]->m_vecNormal.y) < 0.04f)
				{	
					if(fabs(lstPlanes[j].m_vecNormal.z - m_lstFaces[i]->m_vecNormal.z) < 0.04f)
					{
						float fDistance[3];

						fDistance[0]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[0].m_vecPos)-(lstPlanes[j].m_vecVertex)));
						fDistance[1]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[1].m_vecPos)-(lstPlanes[j].m_vecVertex)));
						fDistance[2]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[2].m_vecPos)-(lstPlanes[j].m_vecVertex)));	
							
						if( fDistance[0] >= -fComp	&& fDistance[0] <= fComp &&
							fDistance[1] >= -fComp	&& fDistance[1] <= fComp &&
							fDistance[2] >= -fComp	&& fDistance[2] <= fComp)
						{
								
							bNew = false;
							break;
						}
					}
				}
			}
			
		/*	if((fabs(lstPlanes[j].m_fDist - m_lstFaces[i]->m_fDist) < 7.0) &&
				(fabs(lstPlanes[j].m_vecNormal.x - m_lstFaces[i]->m_vecNormal.x + 
					lstPlanes[j].m_vecNormal.y - m_lstFaces[i]->m_vecNormal.y +
					lstPlanes[j].m_vecNormal.z - m_lstFaces[i]->m_vecNormal.z) < 0.01))
			{
//				if(m_lstFaces[lstPlaneFaces[j]]->m_iObject == m_lstFaces[i]->m_iObject)
//				{
			/*		for(l = 0; l < 3; l++) 
					{
					int iIndex = m_lstFaces[i]->m_iIndex[l];		
						for( k = 0; k < 3; k++)
						{
							int iTargetIndex = m_lstFaces[lstPlaneFaces[j]]->m_iIndex[k];
							if(iTargetIndex == iIndex)
							{	
								bNew = false;
								break;	
				/*			}
						}
					}
				}
				/*
				for(l = 0; l < 3; l++) 
				{
					int iIndex = m_lstFaces[i]->m_iIndex[l];		
					for( k = 0; k < 3; k++)
					{
						int iTargetIndex = m_lstFaces[lstPlaneFaces[j]]->m_iIndex[k];
						if(iTargetIndex == iIndex)
						{	
							bNew = false;
							break;	
						}
					}
				}
			}*/
		}
		if(bNew == true)
		{
			WBLightMapPlane NewPlane;
			NewPlane.m_fDist = m_lstFaces[i]->m_fDist;
			NewPlane.m_vecNormal = m_lstFaces[i]->m_vecNormal;
			NewPlane.m_vecVertex = m_lstFaces[i]->m_pVerts[0].m_vecPos;
			lstPlanes.push_back(NewPlane);
			lstPlaneFaces.push_back(i);
			
		}
	
	}
	
	lstFaces = new vector<int>[lstPlanes.size()];
	for( i = 0; i < (int)lstPlanes.size(); i++ )
	{
		lstFaces[i].clear();

	}

	for( i = 0; i < m_iFaces; i++ )
	{

		for( j = 0; j < (int)lstPlanes.size(); j++ )
		{
			if(fabs(lstPlanes[j].m_vecNormal.x - m_lstFaces[i]->m_vecNormal.x) < 0.04f)
			{
				if(fabs(lstPlanes[j].m_vecNormal.y - m_lstFaces[i]->m_vecNormal.y) < 0.04f)
				{	
					if(fabs(lstPlanes[j].m_vecNormal.z - m_lstFaces[i]->m_vecNormal.z) < 0.04f)
					{
						float fDistance[3];

						fDistance[0]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[0].m_vecPos)-(lstPlanes[j].m_vecVertex)));
						fDistance[1]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[1].m_vecPos)-(lstPlanes[j].m_vecVertex)));
						fDistance[2]= D3DXVec3Dot(&lstPlanes[j].m_vecNormal, &( (m_lstFaces[i]->m_pVerts[2].m_vecPos)-(lstPlanes[j].m_vecVertex)));	
							
						if( fDistance[0] >= -fComp	&& fDistance[0] <= fComp &&
							fDistance[1] >= -fComp	&& fDistance[1] <= fComp &&
							fDistance[2] >= -fComp	&& fDistance[2] <= fComp)
						{
								
							lstFaces[j].push_back(i);
							break;								
						}
					}
				}
			}

		/*	if((fabs(lstPlanes[j].m_fDist - m_lstFaces[i]->m_fDist) < 7.0) &&
				(fabs(lstPlanes[j].m_vecNormal.x - m_lstFaces[i]->m_vecNormal.x + 
					lstPlanes[j].m_vecNormal.y - m_lstFaces[i]->m_vecNormal.y + 
					lstPlanes[j].m_vecNormal.z - m_lstFaces[i]->m_vecNormal.z) < 0.01))
			{
				if(m_lstFaces[lstPlaneFaces[j]]->m_iObject == m_lstFaces[i]->m_iObject)
				{
				/*	for(l = 0; l < 3; l++) 
					{
					int iIndex = m_lstFaces[i]->m_iIndex[l];		
						for( k = 0; k < 3; k++)
						{
							int iTargetIndex = m_lstFaces[lstPlaneFaces[j]]->m_iIndex[k];
							if(iTargetIndex == iIndex)
							{	
							
								lstFaces[j].push_back(i);
								break;								
					/*		}
						}
					}*/
//					lstFaces[j].push_back(i);
//					break;								
/*				}
			}*/
		}
	}
	// Calc Plane Min Max Point,Area Value
	for( i = 0; i < (int)lstPlanes.size(); i++ )
	{

		WBLightVertex **pPos;
		pPos = new WBLightVertex*[lstFaces[i].size() * 3];

		int iCount = 0;	
		for(j = 0; j < (int)lstFaces[i].size(); j++ )
		{
			pPos[iCount++] = &(m_lstFaces[lstFaces[i][j]]->m_pVerts[0]);
			pPos[iCount++] = &(m_lstFaces[lstFaces[i][j]]->m_pVerts[1]);
			pPos[iCount++] = &(m_lstFaces[lstFaces[i][j]]->m_pVerts[2]);
			
		}
		// Get Min Max Point
		D3DXVECTOR3 vecMin = pPos[0]->m_vecPos;
		D3DXVECTOR3 vecMax = pPos[0]->m_vecPos;
		for( j = 1; j < iCount; j++ )
		{
			if(vecMin.x > pPos[j]->m_vecPos.x)
				vecMin.x = pPos[j]->m_vecPos.x;
			if(vecMin.y > pPos[j]->m_vecPos.y)
				vecMin.y = pPos[j]->m_vecPos.y;
			if(vecMin.z > pPos[j]->m_vecPos.z)
				vecMin.z = pPos[j]->m_vecPos.z;
			
		}
		for( j = 1; j < iCount; j++ )
		{
			if(vecMax.x < pPos[j]->m_vecPos.x)
				vecMax.x = pPos[j]->m_vecPos.x;
			if(vecMax.y < pPos[j]->m_vecPos.y)
				vecMax.y = pPos[j]->m_vecPos.y;
			if(vecMax.z < pPos[j]->m_vecPos.z)
				vecMax.z = pPos[j]->m_vecPos.z;
			
		}
		// Area 
		D3DXVECTOR3 vec1,vec2,vec3;
		float fArea = 0.0f;

		for( j = 0; j < iCount; j+=3 )
		{
			vec1 = pPos[j+1]->m_vecPos - pPos[j]->m_vecPos;
			vec2 = pPos[j+2]->m_vecPos - pPos[j]->m_vecPos;
			D3DXVec3Cross(&vec3,&vec1,&vec2);
			fArea += (D3DXVec3Length(&vec3) * 0.5f);
		}
		/*
		for( j = 2; j < iCount; j+=3 )
		{
			float fLength = 0.0f;

			vec1 = pPos[j]->m_vecPos - pPos[j-2]->m_vecPos;
			vec2 = pPos[j - 1]->m_vecPos - pPos[j-2]->m_vecPos;

			D3DXVec3Cross(&vec3,&vec1,&vec2);
			fLength = (D3DXVec3Length(&vec3) * 0.5f);
			fArea += fLength;
		}
		*/
		//Edge
/*		D3DXVECTOR3 *PlaneEdge = new D3DXVECTOR3[ iCount* 4];
		float *PlaneEdgeValue = new float[ iCount * 4];
		for( j = 0, k = 0; j < iCount; j+=3,k+=4 )
		{

			D3DXVECTOR3 vec1 = pPos[j + 1]->m_vecPos - pPos[j]->m_vecPos;
			D3DXVECTOR3 vec2 = pPos[j + 2]->m_vecPos - pPos[j]->m_vecPos;
			
			D3DXVec3Cross(&(PlaneEdge[k]),&vec1,&vec2);
			D3DXVec3Normalize(&(PlaneEdge[k]),&(PlaneEdge[k]));
			PlaneEdgeValue[k] = D3DXVec3Dot(&(PlaneEdge[k]),&(pPos[j]->m_vecPos));
			
			D3DXVec3Cross(&(PlaneEdge[k + 1]),&(pPos[j + 1]->m_vecPos - pPos[j]->m_vecPos),&(PlaneEdge[k]));
			D3DXVec3Normalize(&(PlaneEdge[k + 1]),&(PlaneEdge[k + 1]));
			
			D3DXVec3Cross(&(PlaneEdge[k + 2]),&(pPos[j + 2]->m_vecPos - pPos[j + 1]->m_vecPos),&(PlaneEdge[k]));
			D3DXVec3Normalize(&(PlaneEdge[k + 2]),&(PlaneEdge[k + 2]));

			D3DXVec3Cross(&(PlaneEdge[k + 3]),&(pPos[j]->m_vecPos - pPos[j + 2]->m_vecPos),&(PlaneEdge[k]));
			D3DXVec3Normalize(&(PlaneEdge[k + 3]),&(PlaneEdge[k + 3]));

		}
*/
		for(j = 0; j < (int)lstFaces[i].size(); j++ )
		{
			m_lstFaces[lstFaces[i][j]]->m_vecMin = vecMin;
			m_lstFaces[lstFaces[i][j]]->m_vecMax = vecMax;
			m_lstFaces[lstFaces[i][j]]->m_bSetArea = true;
			m_lstFaces[lstFaces[i][j]]->m_fArea = fArea;
			m_lstFaces[lstFaces[i][j]]->m_iLightMapId = m_iLightMaps;
			
			m_lstFaces[lstFaces[i][j]]->m_iPlaneVertex = iCount;

/*			m_lstFaces[lstFaces[i][j]]->m_pEdge = new D3DXVECTOR3[ iCount * 4];
			memcpy(m_lstFaces[lstFaces[i][j]]->m_pEdge,PlaneEdge,sizeof(D3DXVECTOR3) * iCount * 4);

			m_lstFaces[lstFaces[i][j]]->m_pEdgeValue = new float[iCount * 4];
			memcpy(m_lstFaces[lstFaces[i][j]]->m_pEdgeValue,PlaneEdgeValue,sizeof(float) * iCount * 4);
			m_lstFaces[lstFaces[i][j]]->m_pPlaneVertex = new D3DXVECTOR3[iCount];
			for(k = 0; k < iCount; k++ )
			{
				m_lstFaces[lstFaces[i][j]]->m_pPlaneVertex[k] = pPos[k]->m_vecPos;
			}
*/		
		}
/*		delete[] PlaneEdge;
		delete[] PlaneEdgeValue;
*/		
		WBLightMap *pLightMap = BuildLightMap(m_lstFaces[lstFaces[i][0]],pPos,(lstFaces[i].size() * 3));
		pLightMap->m_iFaceId = lstFaces[i][0];
		m_lstLightMaps.push_back(pLightMap);
		m_iLightMaps++;
		delete[] pPos;
	}
	for(i = 0; i <  (int)lstPlanes.size(); i++ )
		lstFaces[i].clear();
	delete[] lstFaces;
	lstPlaneFaces.clear();

	/*
	for( i = 0; i < m_iFaces; i++ ) {
		m_lstFaces[i]->m_iLightMapId = m_iLightMaps;
		WBLightMap *pLightMap = BuildLightMap(m_lstFaces[i]);
		pLightMap->m_iFaceId = i;
		m_lstLightMaps.push_back(pLightMap);
		m_iLightMaps++;
	}
	*/
	retb r;
	struct reta *rl=new struct reta[m_iLightMaps];

	for( i=0;i<m_iLightMaps;i++ )
	{
		rl[i].sizex= m_lstLightMaps[i]->sizex;
		rl[i].sizey= m_lstLightMaps[i]->sizey;
		r.add(&rl[i]);
	}

	m_iLightMapTexs = r.calc(m_iLightMapSize, m_iLightMapSize);

	for( i=0;i<m_iLightMaps;i++ )
	{
		m_lstLightMaps[i]->offsetx=rl[i].offsetx;
		m_lstLightMaps[i]->offsety=rl[i].offsety;
		m_lstLightMaps[i]->pic=rl[i].offsetz;
	}

	delete[] rl;

	for( i=0;i<m_iLightMapTexs;i++ ) 
	{
		WBLightMapTex *pTex = new WBLightMapTex(m_iLightMapSize,m_iLightMapSize);
		m_lstLightMapTexs.push_back(pTex);
	}

	for( i=0;i<m_iFaces;i++ )
	{
		if ((m_lstFaces[i]->m_iLightMapId) !=-1)
		{
			l=m_lstFaces[i]->m_iLightMapId;
			for( j=0;j<m_lstFaces[i]->m_iVerts;j++ )
			{
				m_lstFaces[i]->m_pVerts[j].m_vecTexcoord.x = 
				(m_lstFaces[i]->m_pVerts[j].m_vecTexcoord.x * m_lstLightMaps[l]->sizex+
				m_lstLightMaps[l]->offsetx) / m_iLightMapSize;
				
				m_lstFaces[i]->m_pVerts[j].m_vecTexcoord.y = 
				(m_lstFaces[i]->m_pVerts[j].m_vecTexcoord.y * m_lstLightMaps[l]->sizey+
				m_lstLightMaps[l]->offsety) / m_iLightMapSize;
			}
		}
	}
	for( i=0;i<m_iLightMapTexs;i++ )
		memset(m_lstLightMapTexs[i]->bmp,0,m_lstLightMapTexs[i]->bytesxy);

	j=0;
	for( i=0;i<m_iLightMaps;i++ )
	{
		m_lstLightMaps[i]->set_base(m_lstFaces[m_lstLightMaps[i]->m_iFaceId],m_lstLightMapTexs[m_lstLightMaps[i]->pic]);
			memset(m_lstLightMaps[i]->bmp,m_iLightMapAmb,m_lstLightMaps[i]->bytesxy);
		m_lstLightMaps[i]->save(m_lstLightMapTexs[m_lstLightMaps[i]->pic]);
		j+=(m_lstLightMaps[i]->sizex)*(m_lstLightMaps[i]->sizey);
	}
	
	HANDLE hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
	
	COORD cd;	
	DWORD dwWordLens;

	char strObjectInfo[256];
	sprintf(strObjectInfo,"Calc Lighting");

	cd.X=0;
	cd.Y=3;
	WriteConsoleOutputCharacter(hStdOut,strObjectInfo,strlen(strObjectInfo),cd,&dwWordLens);

	ComputeLighting();

	return true;


}
int WBLightMapGenerator::collision_test(D3DXVECTOR3 Pos1,D3DXVECTOR3 Pos2,int iFaceIndex,int k)
{
	
	int i;
	D3DXVECTOR3 vecLine = Pos2 - Pos1;
	float fLength = D3DXVec3Length(&vecLine);
	if(fLength < 0.1f)
		return 0;
	D3DXVec3Normalize(&vecLine,&vecLine);

	for( i = 0; i < m_iFaces; i++ )
	{
	/*	if(i == iFaceIndex)
			continue;
	*/
		if(k == 0) {
			if(m_lstFaces[i]->RayCollision(Pos1,Pos2))
				return 1;
		}
		else {
			if(m_lstFaces[i]->RayCollision(Pos1,vecLine,fLength))
				return 1;
		}
	}
	return 0;

}
void WBLightMapGenerator::ComputeLighting() {
	HANDLE hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
	
	COORD cd;	
	DWORD dwWordLens;

	char strObjectInfo[256];


	int i,j,k;
/*	unsigned char *uc;

	// face Color 엠비 언트 색으로 setting
	for( i=0;i<m_iFaces;i++ ) {
		for( j=0;j < m_lstFaces[i]->m_iVerts;j++ )
		{
			uc=(unsigned char *)&m_lstFaces[i]->m_ucColor;
			uc[0]= (unsigned char)WBLightMapGenerator::m_iLightMapAmb;
			uc[1]= (unsigned char)WBLightMapGenerator::m_iLightMapAmb;
			uc[2]= (unsigned char)WBLightMapGenerator::m_iLightMapAmb;
		}
	}*/
	for( i=0;i<m_iLightMaps;i++ )
	{
		((WBLightMapBuild *)m_lstLightMaps[i])->set_base(0);

		
	}
	for( i=0;i<m_iLights;i++ )
	{


		if (WBLightMapGenerator::m_iLightSamples)
		{
			m_lstLights[i]->m_vecColor *= (1.0f/ WBLightMapGenerator::m_iLightSamples);
			for( j = 0; j < WBLightMapGenerator::m_iLightSamples; j++)
			{

				sprintf(strObjectInfo,"Calc %d / %d Light, Sample : %d / %d",i + 1, m_iLights,j+1,WBLightMapGenerator::m_iLightSamples);

				cd.X=0;
				cd.Y=4;
				WriteConsoleOutputCharacter(hStdOut,strObjectInfo,strlen(strObjectInfo),cd,&dwWordLens);

				D3DXVECTOR3 vecRand;
				
				vecRand = D3DXVECTOR3((WBLightMapGenerator::m_iLightJigg) * RLIGHT_FRAND,(WBLightMapGenerator::m_iLightJigg) * RLIGHT_FRAND,(WBLightMapGenerator::m_iLightJigg) * RLIGHT_FRAND);
				
				// 현 라이트에 영향 받는 leaf node face들을 구한다음 
				// illum 계산후의 상태로 Lightmap 에 기록 한다.
				for( k = 0; k < m_iFaces; k++)
				{
					int iLightId = m_lstFaces[k]->m_iLightMapId;
					
					m_lstLightMaps[iLightId]->illum(m_lstLights[i]->m_vecPos + vecRand,m_lstLights[i]->m_vecColor,m_lstLights[i]->m_fRange,4);
					
				}
				
			}
		
		}
		else
		{
			for( k = 0; k < m_iFaces; k++ )
			{
				int iLightId = m_lstFaces[k]->m_iLightMapId;
				m_lstLightMaps[iLightId]->illum(m_lstLights[i]->m_vecPos,m_lstLights[i]->m_vecColor,m_lstLights[i]->m_fRange,4);
			}
		}
	}

	if(WBLightMapGenerator::m_iShadowSamples)
	{

		for( i=0;i< m_iLightMaps;i++ )
		{

			sprintf(strObjectInfo,"Calc %d / %d Global Shadow",i+1,m_iLightMaps);
			cd.X=0;
			cd.Y=5;
			WriteConsoleOutputCharacter(hStdOut,strObjectInfo,strlen(strObjectInfo),cd,&dwWordLens);


			((WBLightMapBuild *)m_lstLightMaps[i])->compute_bgshadow();
			((WBLightMapBuild *)m_lstLightMaps[i])->Blurring();
		}
	}
	
	

	for( i = 0; i < m_iLightMaps; i++ )
	{
		((WBLightMapBuild *)m_lstLightMaps[i])->save_to_lightmap(m_fExposure);
		

	}
	
	for( i = 0; i < m_iLightMaps; i++ )
	{
		int iPicId = m_lstLightMaps[i]->pic;
		m_lstLightMaps[i]->save(m_lstLightMapTexs[iPicId]);
	}


}
float WBLightMapGenerator::compute_bgshadow(D3DXVECTOR3& ip,D3DXVECTOR3& n)
{

	D3DXVECTOR3 dir,p;
	float dot;
	int i,j;

	j=0;


	for( i=0;i<m_iShadowSamples;i++ )
	{
		

		do 
		{
			dir.x=RLIGHT_FRAND;
			dir.y=RLIGHT_FRAND;
		} while (dir.x*dir.x+dir.y*dir.y>1.0f);
		dir.z=(float)cos(dir.x*RPI2)*(float)cos(dir.y*RPI2);
		D3DXVec3Normalize(&dir,&dir);

		dot=D3DXVec3Dot(&dir,&n);
		if (dot<0) 
		{
			dot=-dot;
			dir *= -1.0f;
		}
		dir+=n*0.2f;
		D3DXVec3Normalize(&dir,&dir);
		if (m_iShadowFull==0 && dir.z<0)
			continue;
		// 충돌 없음	
		if (collision_test(ip+n-dir,ip+dir*m_fShadowDist,-1,1)==0 &&
			collision_test(ip+dir*m_fShadowDist,ip+n+dir,-1,1)==0)
			j++;
	
	}
	return (1.0f-m_fShadowFactor) + m_fShadowFactor*((float)j/m_iShadowSamples);

}
WBLightMap *WBLightMapGenerator::BuildLightMap(WBLightMapFace *pFace,WBLightVertex **ppVerts,int iVerts)
{
	static float quad_uv[8]={ 0,0, 0,1, 1,1, 1,0 };
	D3DXVECTOR3 v,v1,v2;
	WBLightMap *lightmap;
	int sizex, sizey, i=0;
	float f;
	
	if (iVerts==4)
	{
		v1= (ppVerts[1]->m_vecPos) - (ppVerts[0]->m_vecPos);
		v2= (ppVerts[3]->m_vecPos) - (ppVerts[0]->m_vecPos);
		if (fabs(D3DXVec3Dot(&v1,&v2))<0.01f)
		{
			v= (ppVerts[0]->m_vecPos) + v1 + v2 - (ppVerts[2]->m_vecPos);
			if (D3DXVec3Length(&v) < 0.1f)
				i=1; // rect!

		}
	}
	if (i)
	{
		f=D3DXVec3Length(&v2)/m_iLightMapPixel;
		sizex=(int)f;
		if (f-sizex>0.5f) sizex++;
		
		f=D3DXVec3Length(&v1)/m_iLightMapPixel;
		sizey=(int)f;
		if (f-sizey>0.5f) sizey++;

		if (sizex<3) sizex=3;
		if (sizey<3) sizey=3;
		if (sizex>m_iLightMapSize) sizex=m_iLightMapSize;
		if (sizey>m_iLightMapSize) sizey=m_iLightMapSize;

		lightmap=new WBLightMapBuild(-1, -1, 0, 0, sizex, sizey);

		float f1x=(float)(sizex-1)/sizex;
		float f2x=0.5f/sizex;
		float f1y=(float)(sizey-1)/sizey;
		float f2y=0.5f/sizey;

		for( i=0;i<iVerts;i++ )
		{
			ppVerts[i]->m_vecTexcoord.x=quad_uv[2*i]*f1x+f2x;
			ppVerts[i]->m_vecTexcoord.y=quad_uv[2*i+1]*f1y+f2y;
		}
	}
	else
	{
		D3DXVECTOR3 diag;
		int ui,vi;


		i=(fabs(pFace->m_vecNormal.x)>fabs(pFace->m_vecNormal.y))?0:1;
		if( i == 0)
			i=(fabs(pFace->m_vecNormal.x)>fabs(pFace->m_vecNormal.z))?i:2;
		else
			i=(fabs(pFace->m_vecNormal.y)>fabs(pFace->m_vecNormal.z))?i:2;

		if (i==0)
			{ ui=1;	vi=2; }
		else if (i==1)
			{ ui=0;	vi=2; }
		else { ui=0; vi=1; }

		diag= (pFace->m_vecMax) - (pFace->m_vecMin);

		f=diag[ui]/m_iLightMapPixel;
		sizex=(int)f;
		if (f-sizex>0.5f) sizex++;
		f=diag[vi]/m_iLightMapPixel;
		sizey=(int)f;
		if (f-sizey>0.5f) sizey++;

		if (sizex<3) sizex=3;
		if (sizey<3) sizey=3;
		if (sizex>m_iLightMapSize) sizex=m_iLightMapSize;
		if (sizey>m_iLightMapSize) sizey=m_iLightMapSize;
		
		lightmap=new WBLightMapBuild(-1, -1, 0, 0, sizex, sizey);

		float f1x=(float)(sizex-1)/sizex;
		float f2x=0.5f/sizex;
		float f1y=(float)(sizey-1)/sizey;
		float f2y=0.5f/sizey;

		for( i=0;i<iVerts;i++ )
		{
			v=ppVerts[i]->m_vecPos - pFace->m_vecMin;
			if(fabs(diag[ui]) <= 0.000001)
			{
				f = 0.0;

			}
			else 
			{
				f=v[ui]/diag[ui];
			}
			ppVerts[i]->m_vecTexcoord.x=f*f1x+f2x;
			if(fabs(diag[vi]) <= 0.000001)
			{
				f = 0.0f;
			}
			else
			{
				f=v[vi]/diag[vi];
			}
			ppVerts[i]->m_vecTexcoord.y=f*f1y+f2y;
		}
	}

	return lightmap;
}

WBLightMap *WBLightMapGenerator::BuildLightMap(WBLightMapFace *pFace)
{

	static float quad_uv[8]={ 0,0, 0,1, 1,1, 1,0 };
	D3DXVECTOR3 v,v1,v2;
	WBLightMap *lightmap;
	int sizex, sizey, i=0;
	float f;
	
	if (pFace->m_iVerts==4)
	{
		v1= (pFace->m_pVerts[1].m_vecPos) - (pFace->m_pVerts[0].m_vecPos);
		v2= (pFace->m_pVerts[3].m_vecPos) - (pFace->m_pVerts[0].m_vecPos);
		if (fabs(D3DXVec3Dot(&v1,&v2))<0.01f)
		{
			v= (pFace->m_pVerts[0].m_vecPos) + v1 + v2 - (pFace->m_pVerts[2].m_vecPos);
			if (D3DXVec3Length(&v) < 0.1f)
				i=1; // rect!

		}
	}
	if (i)
	{
		f=D3DXVec3Length(&v2)/m_iLightMapPixel;
		sizex=(int)f;
		if (f-sizex>0.5f) sizex++;
		
		f=D3DXVec3Length(&v1)/m_iLightMapPixel;
		sizey=(int)f;
		if (f-sizey>0.5f) sizey++;

		if (sizex<3) sizex=3;
		if (sizey<3) sizey=3;
		if (sizex>m_iLightMapSize) sizex=m_iLightMapSize;
		if (sizey>m_iLightMapSize) sizey=m_iLightMapSize;

		lightmap=new WBLightMapBuild(-1, -1, 0, 0, sizex, sizey);

		float f1x=(float)(sizex-1)/sizex;
		float f2x=0.5f/sizex;
		float f1y=(float)(sizey-1)/sizey;
		float f2y=0.5f/sizey;

		for( i=0;i<pFace->m_iVerts;i++ )
		{
			pFace->m_pVerts[i].m_vecTexcoord.x=quad_uv[2*i]*f1x+f2x;
			pFace->m_pVerts[i].m_vecTexcoord.y=quad_uv[2*i+1]*f1y+f2y;
		}
	}
	else
	{
		D3DXVECTOR3 diag;
		int ui,vi;


		i=(fabs(pFace->m_vecNormal.x)>fabs(pFace->m_vecNormal.y))?0:1;
		if( i == 0)
			i=(fabs(pFace->m_vecNormal.x)>fabs(pFace->m_vecNormal.z))?i:2;
		else
			i=(fabs(pFace->m_vecNormal.y)>fabs(pFace->m_vecNormal.z))?i:2;

		if (i==0)
			{ ui=1;	vi=2; }
		else if (i==1)
			{ ui=0;	vi=2; }
		else { ui=0; vi=1; }

		diag= (pFace->m_vecMax) - (pFace->m_vecMin);

		f=diag[ui]/m_iLightMapPixel;
		sizex=(int)f;
		if (f-sizex>0.5f) sizex++;
		f=diag[vi]/m_iLightMapPixel;
		sizey=(int)f;
		if (f-sizey>0.5f) sizey++;

		if (sizex<3) sizex=3;
		if (sizey<3) sizey=3;
		if (sizex>m_iLightMapSize) sizex=m_iLightMapSize;
		if (sizey>m_iLightMapSize) sizey=m_iLightMapSize;
		
		lightmap=new WBLightMapBuild(-1, -1, 0, 0, sizex, sizey);

		float f1x=(float)(sizex-1)/sizex;
		float f2x=0.5f/sizex;
		float f1y=(float)(sizey-1)/sizey;
		float f2y=0.5f/sizey;

		for( i=0;i<pFace->m_iVerts;i++ )
		{
			v=pFace->m_pVerts[i].m_vecPos - pFace->m_vecMin;
			if(fabs(diag[ui]) <= 0.000001)
			{
				f = 0.0;

			}
			else 
			{
				f=v[ui]/diag[ui];
			}
			pFace->m_pVerts[i].m_vecTexcoord.x=f*f1x+f2x;
			if(fabs(diag[vi]) <= 0.000001)
			{
				f = 0.0f;
			}
			else
			{
				f=v[vi]/diag[vi];
			}
			pFace->m_pVerts[i].m_vecTexcoord.y=f*f1y+f2y;
		}
	}

	return lightmap;

}
void WBLightMapGenerator::CleanStatic()
{
	int i;
	for( i = 0 ; i < m_iFaces; i++ )
	{
		delete m_lstFaces[i];
		m_lstFaces[i] = NULL;
		
	}
	m_lstFaces.clear();
	m_iFaces = 0;
	
	delete[] emm;
	emm = NULL;
	delete[] rad;
	rad = NULL;

}