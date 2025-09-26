// WBLightMap.cpp: implementation of the WBLightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "WBLightMap.h"
#include "WBLightMapGenerator.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
float WBLightMapFace::area()
{
	if(m_bSetArea == true)
		return m_fArea;

	int i;
	D3DXVECTOR3 vec1,vec2,vec3;
	float fArea = 0.0f;

//	if(m_iVertsNum < 2)
//		return -1.0f;

	for( i = 2; i < m_iVerts; i++ )
	{
		float fLength = 0.0f;

		vec1 = m_pVerts[i].m_vecPos - m_pVerts[0].m_vecPos;
		vec2 = m_pVerts[i - 1].m_vecPos - m_pVerts[0].m_vecPos;
		D3DXVec3Cross(&vec3,&vec1,&vec2);
		fLength = (D3DXVec3Length(&vec3) * 0.5f);
		fArea += fLength;
	}
	return fArea;

}
void WBLightMapFace::CalcNormal()
{
	if(m_iVerts >= 3) {
		D3DXVECTOR3 vec1,vec2,vec3;

		vec1 = m_pVerts[1].m_vecPos - m_pVerts[0].m_vecPos;
		vec2 = m_pVerts[2].m_vecPos - m_pVerts[0].m_vecPos;
		D3DXVec3Cross(&vec3,&vec1,&vec2);
		D3DXVec3Normalize(&vec3,&vec3);
		m_vecNormal = vec3;
	}
}
int WBLightMapFace::InsidePolygon(D3DXVECTOR3 vecPoint)
{
	if((vecPoint.x >= m_vecMin.x) && (vecPoint.y >= m_vecMin.y )&& (vecPoint.z >= m_vecMin.z)) 
	{
		if((vecPoint.x <= m_vecMax.x) && (vecPoint.y <= m_vecMax.y )&& (vecPoint.z <= m_vecMax.z))
			return 1;
	
	}
	return 0;

}

// 충돌시 1 return
int WBLightMapFace::RayCollision(D3DXVECTOR3 vecStart,D3DXVECTOR3 vecDir,float fLength)
{
/*	
	static D3DXVECTOR3 v[3],n;
	v[0].x = m_pVerts[0].m_vecPos.x + m_vecNormal.x * fLength;
	v[0].y = m_pVerts[0].m_vecPos.y + m_vecNormal.y * fLength;
	v[0].z = m_pVerts[0].m_vecPos.z + m_vecNormal.z * fLength;

	v[1].x = m_pVerts[1].m_vecPos.x + m_vecNormal.x * fLength;
	v[1].y = m_pVerts[1].m_vecPos.y + m_vecNormal.y * fLength;
	v[1].z = m_pVerts[1].m_vecPos.z + m_vecNormal.z * fLength;

	v[2].x = m_pVerts[2].m_vecPos.x + m_vecNormal.x * fLength;
	v[2].y = m_pVerts[2].m_vecPos.y + m_vecNormal.y * fLength;
	v[2].z = m_pVerts[2].m_vecPos.z + m_vecNormal.z * fLength;

	D3DXVec3Cross(&n,&(v[1] - v[0]),&(v[2] - v[0]));
	D3DXVec3Normalize(&n,&n);

	float x = D3DXVec3Dot(&n,&vecDir);
	if(x > -WBSMALL)
		return 0;
	float fDist = (D3DXVec3Dot(&n,&(v[0])) - D3DXVec3Dot(&n,&vecStart)) / x;
	if(fDist < 0)
		return 0;

	D3DXVECTOR3 vec3Vec;
	vec3Vec.x = vecStart.x + vecDir.x * fDist;
	vec3Vec.y = vecStart.y + vecDir.y * fDist;
	vec3Vec.z = vecStart.z + vecDir.z * fDist;

	int i,j,crossings=0,u0,u1;
	n.x=(float)fabs(n.x);
	n.y=(float)fabs(n.y);
	n.z=(float)fabs(n.z);
	if (n.x>n.y && n.x>n.z)
		{ u0=1; u1=2; }
	else 
	if (n.y>n.x && n.y>n.z)
		{ u0=0; u1=2; }
	else 
		{ u0=0; u1=1; }

	v[0][u0]-=vec3Vec[u0];
	v[1][u0]-=vec3Vec[u0];
	v[2][u0]-=vec3Vec[u0];
	v[0][u1]-=vec3Vec[u1];
	v[1][u1]-=vec3Vec[u1];
	v[2][u1]-=vec3Vec[u1];

		
	for( i=0; i<3; i++ )
	{
		j=(i+1) % 3;
		if(((v[i][u1]<-WBSMALL) && (v[j][u1]>=0)) || ((v[j][u1]<-WBSMALL) && (v[i][u1]>=0.0)))
			if((v[i][u0]>=0.0) && (v[j][u0]>=0.0))
				++crossings;
			else
				if((v[i][u0]>=0.0) || (v[j][u0]>=0.0))
					if((v[i][u0]-v[i][u1]*(v[j][u0]-v[i][u0])/(v[j][u1]-v[i][u1]))>WBSMALL)
						++crossings;
	}
	if((crossings%2)==0)
		return 0;
	else
		return 1;

*/
/////////////////////////
	/*
	float fDot = D3DXVec3Dot(&m_vecNormal,&vecDir);
	if(fDot > -WBSMALL)
		return 0;
	float fDist = (D3DXVec3Dot(&m_vecNormal,&m_pVerts[0].m_vecPos) - D3DXVec3Dot(&m_vecNormal,&vecStart)) / fDot;
	if(fDist <0)
		return 0;

	D3DXVECTOR3 vec3Vec;
	vec3Vec.x = vecStart.x + vecDir.x * fDist;
	vec3Vec.y = vecStart.y + vecDir.y * fDist;
	vec3Vec.z = vecStart.z + vecDir.z * fDist;

	for( int i=0;i<m_iVerts;i++ )
	{
		if (((vec3Vec.x-m_pVerts[i].m_vecPos.x)*(m_pEdge[i].x)+
			(vec3Vec.y-m_pVerts[i].m_vecPos.y)*(m_pEdge[i].y)+
			(vec3Vec.z-m_pVerts[i].m_vecPos.z)*(m_pEdge[i].z))>0)
			return 0;
	}
	return 1;	
*/
/*
	float fDot = D3DXVec3Dot(&m_vecNormal,&vecDir);
	if(WB_FPSIGNBIT(fDot) == 0)
		return 0;
	float fDist = (d0 - D3DXVec3Dot(&m_vecNormal,&vecStart)) / fDot;
	
	if(WB_FPSIGNBIT(fDist) || fDist >= fLength)
		return 0;

	D3DXVECTOR3 vec3Vec = vecStart + vecDir * fDist;

	for( int i=0;i<m_iVerts;i++ )
	{
		if (((vec3Vec.x-m_pVerts[i].m_vecPos.x)*(m_pEdge[i].x)+
			(vec3Vec.y-m_pVerts[i].m_vecPos.y)*(m_pEdge[i].y)+
			(vec3Vec.z-m_pVerts[i].m_vecPos.z)*(m_pEdge[i].z))>0)
			return 0;
	}
	return 1;
*/
//	가장 최신
	D3DXVECTOR3 v;
	float d,x;
	int i=0,j=0;
	for( i = 0; i < m_iVerts; i+=3,j+=4 )
	{
		x=D3DXVec3Dot(&(m_pEdge[j]),&vecDir);
		if (WB_FPSIGNBIT(x)==0)
			continue;
		d=(m_pEdgeValue[j] - D3DXVec3Dot(&(m_pEdge[j]),&vecStart))/x;
		if (WB_FPSIGNBIT(d) || d>= fLength)
			continue;

		v= vecStart + vecDir * d;

		if ((v.x-m_pVerts[i].m_vecPos.x)*(m_pEdge[j+1].x)+
			(v.y-m_pVerts[i].m_vecPos.y)*(m_pEdge[j+1].y)+
			(v.z-m_pVerts[i].m_vecPos.z)*(m_pEdge[j+1].z)>0)
			continue;
		if ((v.x-m_pVerts[i+1].m_vecPos.x)*(m_pEdge[j+2].x)+
			(v.y-m_pVerts[i+1].m_vecPos.y)*(m_pEdge[j+2].y)+
			(v.z-m_pVerts[i+1].m_vecPos.z)*(m_pEdge[j+2].z)>0)
			continue;
		if ((v.x-m_pVerts[i+2].m_vecPos.x)*(m_pEdge[j+3].x)+
			(v.y-m_pVerts[i+2].m_vecPos.y)*(m_pEdge[j+3].y)+
			(v.z-m_pVerts[i+2].m_vecPos.z)*(m_pEdge[j+3].z)>0)
			continue;

		return 1;
	}
	return 0;

/*	D3DXVECTOR3 v;
	float d,x;
	int i=0,j=0;
	for( i = 0; i < m_iPlaneVertex; i+=3,j+=4 )
	{
		x=D3DXVec3Dot(&(m_pEdge[j]),&vecDir);
		if (WB_FPSIGNBIT(x)==0)
			continue;
		d=(m_pEdgeValue[j] - D3DXVec3Dot(&(m_pEdge[j]),&vecStart))/x;
		if (WB_FPSIGNBIT(d) || d>= fLength)
			continue;

		v= vecStart + vecDir * d;

		if ((v.x-m_pPlaneVertex[i].x)*(m_pEdge[j+1].x)+
			(v.y-m_pPlaneVertex[i].y)*(m_pEdge[j+1].y)+
			(v.z-m_pPlaneVertex[i].z)*(m_pEdge[j+1].z)>0)
			continue;
		if ((v.x-m_pPlaneVertex[i+1].x)*(m_pEdge[j+2].x)+
			(v.y-m_pPlaneVertex[i+1].y)*(m_pEdge[j+2].y)+
			(v.z-m_pPlaneVertex[i+1].z)*(m_pEdge[j+2].z)>0)
			continue;
		if ((v.x-m_pPlaneVertex[i+2].x)*(m_pEdge[j+3].x)+
			(v.y-m_pPlaneVertex[i+2].y)*(m_pEdge[j+3].y)+
			(v.z-m_pPlaneVertex[i+2].z)*(m_pEdge[j+3].z)>0)
			continue;

		return 1;
	}
	return 0;
*/
/////////////////////////

}

int WBLightMapFace::RayCollision(D3DXVECTOR3 vecStart,D3DXVECTOR3 vecEnd)
{
	//float fDist = -D3DXVec3Dot(&m_vecNormal,&(m_pVerts[0].m_vecPos));
	
	float fValue1 = D3DXVec3Dot(&m_vecNormal,&vecStart) + m_fDist;
	float fValue2 = D3DXVec3Dot(&m_vecNormal,&vecEnd) + m_fDist;

	if((fValue1 * fValue2) <0.0f)
	{
		/// 충돌 point Get
/*		D3DXVECTOR3 vecLine = vecEnd - vecStart;
		D3DXVECTOR3 vecPoint;

		D3DXVec3Normalize(&vecLine, &vecLine);
	
		float Numbers = -(D3DXVec3Dot(&m_vecNormal,&vecStart) + fDist);
		float Denum = D3DXVec3Dot(&m_vecNormal,&vecLine);
		
		if(Denum == 0.0f)
			vecPoint = vecStart;
		else
		{
			float fDist2 = Numbers / Denum;
			vecPoint.x = vecStart.x + vecLine.x * fDist2;
			vecPoint.y = vecStart.y + vecLine.y * fDist2;
			vecPoint.z = vecStart.z + vecLine.z * fDist2;
		}
		// 충돌 포인트가 polygon 안인지 테스트
		if(InsidePolygon(vecPoint))*/
			return 1;
	}
	return 0;

}
void WBLightMapFace::GetMinMax()
{
	int i,j;

	if(m_iVerts <= 0)
		return;
	else
	{
		// Get  Min
		
		D3DXVECTOR3 vecMin = m_pVerts[0].m_vecPos;

		for( i = 1; i < m_iVerts; i++ )
		{
			if(vecMin.x > m_pVerts[i].m_vecPos.x)
				vecMin.x = m_pVerts[i].m_vecPos.x;

			if(vecMin.y > m_pVerts[i].m_vecPos.y)
				vecMin.y = m_pVerts[i].m_vecPos.y;

			if(vecMin.z > m_pVerts[i].m_vecPos.z)
				vecMin.z = m_pVerts[i].m_vecPos.z;
						
		}

		m_vecMin = vecMin;

		// Get Max
		D3DXVECTOR3 vecMax = m_pVerts[0].m_vecPos;
		
		for( i = 1; i < m_iVerts; i++ )
		{
			if(vecMax.x < m_pVerts[i].m_vecPos.x)
				vecMax.x = m_pVerts[i].m_vecPos.x;

			if(vecMax.y < m_pVerts[i].m_vecPos.y)
				vecMax.y = m_pVerts[i].m_vecPos.y;

			if(vecMax.z < m_pVerts[i].m_vecPos.z)
				vecMax.z = m_pVerts[i].m_vecPos.z;
						
		}

		m_vecMax = vecMax;
		
		m_fDist = -(D3DXVec3Dot(&(m_vecNormal),&(m_pVerts[0].m_vecPos)));

		d0 = -(D3DXVec3Dot(&(m_vecNormal),&(m_pVerts[0].m_vecPos)));	
		// Edge 계산
		m_pEdge = new D3DXVECTOR3[m_iVerts * 4];
		m_pEdgeValue = new float[m_iVerts * 4];

		for( i = 0, j = 0; i < m_iVerts; i+=3,j+=4 )
		{
			D3DXVECTOR3 vec1 = m_pVerts[i + 1].m_vecPos - m_pVerts[i].m_vecPos;
			D3DXVECTOR3 vec2 = m_pVerts[i + 2].m_vecPos - m_pVerts[i].m_vecPos;
			
			D3DXVec3Cross(&(m_pEdge[j]),&vec1,&vec2);
			D3DXVec3Normalize(&(m_pEdge[j]),&(m_pEdge[j]));
			m_pEdgeValue[j] = D3DXVec3Dot(&(m_pEdge[j]),&(m_pVerts[i].m_vecPos));
			
			D3DXVec3Cross(&(m_pEdge[j + 1]),&(m_pVerts[i + 1].m_vecPos - m_pVerts[i].m_vecPos),&(m_pEdge[j]));
			D3DXVec3Normalize(&(m_pEdge[j + 1]),&(m_pEdge[j + 1]));
			
			D3DXVec3Cross(&(m_pEdge[j + 2]),&(m_pVerts[i + 2].m_vecPos - m_pVerts[i + 1].m_vecPos),&(m_pEdge[j]));
			D3DXVec3Normalize(&(m_pEdge[j + 2]),&(m_pEdge[j + 2]));

			D3DXVec3Cross(&(m_pEdge[j + 3]),&(m_pVerts[i].m_vecPos - m_pVerts[i + 2].m_vecPos),&(m_pEdge[j]));
			D3DXVec3Normalize(&(m_pEdge[j + 3]),&(m_pEdge[j + 3]));


		}
	}
/*		m_pEdge = new D3DXVECTOR3[m_iVerts];
		m_pEdgeValue = new float[m_iVerts];

		for( i = 0; i < m_iVerts; i++ )
		{
			D3DXVECTOR3 vec1 = m_pVerts[(i + 1) % m_iVerts].m_vecPos - m_pVerts[i].m_vecPos;
			D3DXVec3Cross(&m_pEdge[i],&vec1,&m_vecNormal);
			D3DXVec3Normalize(&m_pEdge[i],&m_pEdge[i]);
			

		}

	}
*/	
	
}
WBLightMap::WBLightMap(int f,int p,int x,int y,int sx,int sy,int bp)
{
	m_iFaceId = f;
	pic=p;
	offsetx=x;
	offsety=y;
	sizex=sx;
	sizey=sy;
	bytespixel=bp;
	bytesx=sx*bp;
	bytesxy=bytesx*sy;
	bmp=new unsigned char[bytesxy];

}

WBLightMap::~WBLightMap()
{
	delete[] bmp; 
}
void WBLightMap::set_base(WBLightMapFace *f,WBLightMapTex *lmp,D3DXVECTOR3& pos)
{
	v0 = f->m_pVerts[0].m_vecPos + pos;
	v1 = f->m_pVerts[1].m_vecPos - f->m_pVerts[0].m_vecPos;
	v2 = f->m_pVerts[2].m_vecPos - f->m_pVerts[0].m_vecPos;
	uv[0][0]=(f->m_pVerts[0].m_vecTexcoord.x);
	uv[0][1]=(f->m_pVerts[0].m_vecTexcoord.y);
	uv[1][0]=(f->m_pVerts[1].m_vecTexcoord.x)-(f->m_pVerts[0].m_vecTexcoord.x);
	uv[1][1]=(f->m_pVerts[1].m_vecTexcoord.y)-(f->m_pVerts[0].m_vecTexcoord.y);
	uv[2][0]=(f->m_pVerts[2].m_vecTexcoord.x)-(f->m_pVerts[0].m_vecTexcoord.x);
	uv[2][1]=(f->m_pVerts[2].m_vecTexcoord.y)-(f->m_pVerts[0].m_vecTexcoord.y);
	det=(uv[1][0]*uv[2][1])-(uv[2][0]*uv[1][1]);
	normal=f->m_vecNormal;
	D3DXVECTOR3 p1,p2;
	map_point((float)(offsetx)/lmp->sizex,(float)(offsety)/lmp->sizey,d0);
	map_point((float)(offsetx+sizex)/lmp->sizex,(float)(offsety)/lmp->sizey,p1);
	map_point((float)(offsetx)/lmp->sizex,(float)(offsety+sizey)/lmp->sizey,p2);
	d1=p1-d0;
	d2=p2-d0;

}

void WBLightMap::map_point(float u, float v, D3DXVECTOR3 &point)
{
	u-=uv[0][0];
	v-=uv[0][1];
	if(fabs(det) <= 0.00001)
	{
		point = v0;
	}
	else {

		point=
			v0+
			v1*((u*uv[2][1]-uv[2][0]*v)/det)+
			v2*((uv[1][0]*v-u*uv[1][1])/det);
	}
}

void WBLightMap::map_point_local(float u, float v, D3DXVECTOR3 &p,D3DXVECTOR3 &n)
{
	static D3DXVECTOR3 pn[2];
	if(m_iFaceId != -1)
	{
		p=d0+d1*u+d2*v; 
		n=normal;
	}
}

void WBLightMap::illum(D3DXVECTOR3& pos,D3DXVECTOR3& color,float radius,int shadows)
{
	int i,j,k;
	unsigned char *uc;
	
	float fi=1.0f/sizex,fj=1.0f/sizey,fu,fv;
	float dist,dot,r=radius*radius;
	D3DXVECTOR3 p,n,dir;

	fv=fj*0.5f;
	for( j=0;j<sizey;j++,fv+=fj )
	{
	fu=fi*0.5f;
	uc=&bmp[j*bytesx];
	for( i=0;i<sizex;i++,fu+=fi )
		{
		map_point_local(fu,fv,p,n);
		dir=p-pos;
		dist=(dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);

		if (dist>r)
			uc+=3;
		else 
			{
			dist=(float)sqrt(dist);
			dir*=1.0f/dist;
			dot=D3DXVec3Dot(&dir,&normal);
			if ((shadows&1) && dot>0)
				{
				uc+=3;
				continue;
				}
			if (shadows&8 &&
				WBLightMapGenerator::collision_test(pos,p-dir))
				{
				uc+=3;
				continue;
				}
			dist=(1.0f-dist/radius);
			if (shadows&2)
				dist*=dist;
			if (shadows&4)
				dist*=-dot;
			dist*=255.0;

			k=(int)(color.x*dist)+(int)(*uc);
			*(uc++)=k>255?255:k;

			k=(int)(color.y*dist)+(int)(*uc);
			*(uc++)=k>255?255:k;

			k=(int)(color.z*dist)+(int)(*uc);
			*(uc++)=k>255?255:k;
			}
		}
	}

}

void WBLightMap::load(WBLightMapTex *lmp)
{
	int i,j=sizex*bytespixel;
	for( i=0;i<sizey;i++ )
		memcpy(
			&bmp[i*bytesx],
			&lmp->bmp[(i+offsety)*lmp->bytesx+offsetx*lmp->bytespixel],
			j);

}

void WBLightMap::save(WBLightMapTex *lmp)
{
	int i,j=sizex*bytespixel;
	for( i=0;i<sizey;i++ )
		memcpy(
			&lmp->bmp[(i+offsety)*lmp->bytesx+offsetx*lmp->bytespixel],
			&bmp[i*bytesx],
			j);

}
