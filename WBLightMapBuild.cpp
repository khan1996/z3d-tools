// WBLightMapBuild.cpp: implementation of the WBLightMapBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "WBLightMapBuild.h"
#include "WBLightMapGenerator.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned char expose(float light,float exposure)
{
	float e=(float)exp(-light*exposure);
	return (unsigned char)((1.0f-e)*255.0f);
}


WBLightMapBuild::~WBLightMapBuild()
{
	delete[] rad;
	delete[] emm;

}
void WBLightMapBuild::set_base(int flag)
{
	alloc_data(flag);
	totalenergy = D3DXVECTOR3(0.0f,0.0f,0.0f);
	if (m_iFaceId != -1)
	{
		float facearea=WBLightMapGenerator::m_lstFaces[m_iFaceId]->area();
		pixelarea=facearea/(sizex*sizey);
	}
	else
		pixelarea=0;
}
void WBLightMapBuild::compute_bgshadow()
{
	
	int x,y;
	float fi=1.0f/sizex,fj=1.0f/sizey,fu,fv;
	D3DXVECTOR3 p,n;
	float *r=rad,bgs;
	
	fv=fj*0.5f;
	
	for( y=0;y<sizey;y++,fv+=fj )
	{
		fu=fi*0.5f;
		for( x=0;x<sizex;x++,fu+=fi )
		{
	

			map_point_local(fu,fv,p,n);
			// world point, normal 
			bgs=WBLightMapGenerator::compute_bgshadow(p,n);
	
			r[0]*=bgs;
			r[1]*=bgs;
			r[2]*=bgs;
	

			r+=3;
		}
	}
}
void WBLightMapBuild::Blurring() 
{
	int i,j,k;
	
	float *r=rad;

	for( i = 0; i < sizey; i++ )
	{
		for( j = 0; j < sizex; j++ )
		{
			float *pCurrent1 = &(r[(i * bytesx) + (j * bytespixel)]);
			float *pCurrent2 = &(r[(i * bytesx) + (j * bytespixel) + 1]);
			float *pCurrent3 = &(r[(i * bytesx) + (j * bytespixel) + 2]);

			float ucSum[3];
			
			ucSum[0] = (*pCurrent1);
			ucSum[1] = (*pCurrent2);
			ucSum[2] = (*pCurrent3);
			
			int iLegal = 0;

			if(i > 0)										// Up Pixel
			{
				ucSum[0] += r[( (i - 1) * bytesx ) + (j * bytespixel)] / 2.0f;
				ucSum[1] += r[( (i - 1) * bytesx ) + (j * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i - 1) * bytesx ) + (j * bytespixel) + 2]/ 2.0f;
				
				iLegal++;
			}
			if(i < (sizey - 1))								// Down Pixel
			{
				ucSum[0] += r[( (i + 1) * bytesx ) + (j * bytespixel)]/ 2.0f;
				ucSum[1] += r[( (i + 1) * bytesx ) + (j * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i + 1) * bytesx ) + (j * bytespixel) + 2]/ 2.0f;
				iLegal++;
			}
			if(j > 0)										// Left
			{
				ucSum[0] += r[( i * bytesx ) + ((j - 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( i * bytesx ) + ((j - 1) * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( i * bytesx ) + ((j - 1) * bytespixel) + 2]/ 2.0f;
				iLegal++;
			}
			if( j < (sizex - 1))							// Right
			{
				ucSum[0] += r[( i * bytesx ) + ((j + 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( i * bytesx ) + ((j + 1) * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( i * bytesx ) + ((j + 1) * bytespixel) + 2]/ 2.0f;
				iLegal++;
			}
			if((i > 0) && (j > 0))							// Up Left
			{
				ucSum[0] += r[( (i - 1)* bytesx ) + ((j - 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( (i - 1)* bytesx ) + ((j - 1) * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i - 1)* bytesx ) + ((j - 1) * bytespixel) + 2]/ 2.0f;
				iLegal++;
			}
			if((i < (sizey -1)) && (j > 0))					// Down Left
			{
				ucSum[0] += r[( (i + 1) * bytesx ) + ((j - 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( (i + 1) * bytesx ) + ((j - 1) * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i + 1) * bytesx ) + ((j - 1) * bytespixel) + 2]/ 2.0f;
				iLegal++;

			}
			if((i > 0) && (j < (sizex - 1)))				//Up Right
			{
				ucSum[0] += r[( (i - 1) * bytesx ) + ((j + 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( (i - 1) * bytesx ) + ((j + 1)* bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i - 1) * bytesx ) + ((j + 1)* bytespixel) + 2]/ 2.0f;
				
				iLegal++;

			}
			if( (i < (sizey - 1)) && ( j < (sizex - 1)))	//Down Right
			{

				ucSum[0] += r[( (i + 1) * bytesx ) + ((j + 1) * bytespixel)]/ 2.0f;
				ucSum[1] += r[( (i + 1) * bytesx ) + ((j + 1) * bytespixel) + 1]/ 2.0f;
				ucSum[2] += r[( (i + 1) * bytesx ) + ((j + 1) * bytespixel) + 2]/ 2.0f;
				iLegal++;
			}

			for(k = 0; k < 3; k++ )
			{
				float iSum = (float)ucSum[k];
				iSum = (float)(iSum / (float)iLegal);
				ucSum[k] = (float)iSum;

			}
				
			(*pCurrent1) = ucSum[0];
			(*pCurrent2) = ucSum[1];
			(*pCurrent3) = ucSum[2];
	
		}
	}
}

void WBLightMapBuild::alloc_data(int flag)
{
	int i,j=sizex*sizey;
	float ambient=WBLightMapGenerator::m_iLightMapAmb/255.0f;
	delete[] rad;
	j*=3;
	rad=new float[j];
	for( i=0;i<j;i++ )
		rad[i]=ambient;
	if (flag)
	{
		delete[] emm;
		j*=3;
		emm=new float[j];
		for( i=0;i<j;i++ )
			emm[i]=ambient;
	}
}

void WBLightMapBuild::set_data(int flag,int x,int y,float *f)
{
	int p=(x+y*sizex)*3;
	if (flag)
	{
		rad[p]=f[0];
		rad[p+1]=f[1];
		rad[p+2]=f[2];
	}	
	else
	{
		emm[p]=f[0];
		emm[p+1]=f[1];
		emm[p+2]=f[2];
	}	
}

void WBLightMapBuild::save_to_lightmap(float exposure)
{

	int i,j,k;
	float *f=rad;

	
	unsigned char *uc=bmp;
	if (exposure==0)
	{
		for( j=0;j<sizey;j++ )
		for( i=0;i<sizex;i++ )
		for( k=0;k<3;k++,f++ )
		{
			int z=(int)(*f*255);
			*(uc++)=z<0?0:z>255?255:z;
		}
	}
	else
	{
		

		for( j=0;j<sizey;j++ ) 
		{
			for( i=0;i<sizex;i++ ) {
				int iNum = 0;
				for( k=0;k<3;k++,f++ ) {
					*(uc)=expose(*f,exposure);
					if(*(uc) < 	(WBLightMapGenerator::m_iLightMapAmb))
						iNum++;
					uc++;	
				}
	
			}
		}
	}
}
// pos : Light Pos, color : Light Color radius : light rad
void WBLightMapBuild::illum(D3DXVECTOR3& pos,D3DXVECTOR3& color,float radius,int shadows)
{
	int i,j;
	
	float dist,dot,r=radius*radius;
	float fi=1.0f/sizex,fj=1.0f/sizey,fu,fv;
	D3DXVECTOR3 p,dir,n;

	float *data=rad;
	fv=fj*0.5f;
	for( j=0;j<sizey;j++,fv+=fj )
	{
	fu=fi*0.5f;
	for( i=0;i<sizex;i++,fu+=fi )
		{
		map_point_local(fu,fv,p,n);

		dir.x=(p.x)-(pos.x);
		dir.y=(p.y)-(pos.y);
		dir.z=(p.z)-(pos.z);
		
		dist = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);

		if (dist>r)
			data+=3;
		else 
			{
			dist=(float)sqrt(dist);
			dir*=(1.0f/dist);
			dot=D3DXVec3Dot(&dir,&n);
			if ((shadows&4) && dot>0)
			{
				data+=3;
				continue;
			}
			if (shadows&8 &&
				!WBLightMapGenerator::collision_test(pos+n+dir,p-dir))
				{
				data+=3;
				continue;
				}
			
			if(WBLightMapGenerator::collision_test(pos+n+dir,p-dir,m_iFaceId,1))
			{
				data+=3;
				continue;

			}
			dist=(1.0f - (dist/radius));
			if (shadows&2)
				dist*=dist;
		
			if (shadows&4)
				dist*=-dot;
		
			// 0.0 이하의 값으로 가는것 방지
			*(data)+=color.x*dist;
			if(*(data) < 0.0f)
				*(data) = 0.0f;
			data++;
			*(data)+=color.y*dist;
			if(*(data) < 0.0f)
				*(data) = 0.0f;
			data++;

			*(data)+=color.z*dist;
			if(*(data) < 0.0f)
				*(data) = 0.0f;
			data++;

			}
		}
	}
}

D3DXVECTOR3 WBLightMapBuild::tri_receive_light(WBLightMapFace *f,D3DXVECTOR3& P,D3DXVECTOR3& N,D3DXVECTOR3& E,int flag)
{
	D3DXVECTOR3 receivedenergy = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 p,n,dir,e;

	//float l2,dot1,dot2;
	
	float *r1=&(WBLightMapGenerator::emm[f->m_iStartIndex*3]);
	float *r2=&(WBLightMapGenerator::rad[f->m_iStartIndex*3]);

	D3DXVECTOR3 reflectance;
/*	if (f->sh!=-1)
		reflectance=g_flyengine->shaders[f->sh]->color;
	else
		reflectance.vec(1);

	int i;
	for( i=0;i<f->nvert;i++,r1+=3,r2+=3 )
	{
		p=f->vert[i+f->vertindx];
		n=f->vert[i+f->vertindx].normal;

		dir=P-p;
		l2=dir.length2();
		if (l2<10.0f)
			continue;

		dir*=1.0f/(float)sqrt(l2);

		dot1=FLY_VECDOT(dir,n);
		if (dot1<=0.01f)
			continue;

		if (l2<1000)
			dot1*=l2/1000.0f;

		if (flag==0)
		{
			dot2=-FLY_VECDOT(dir,N);
			if (dot2<=0.01f)
				continue;

			e=(E*reflectance)*((dot1*dot2)/(l2*FLY_2PI));
		}
		else 
			e=(E*reflectance)*(dot1/(l2*2.0f*FLY_2PI));

		if ((lmshadows&8) &&
			g_flyengine->collision_test(P+N+dir,p+dir,FLY_TYPE_STATICMESH))
			continue;

		r1[0]+=e.x;
		r1[1]+=e.y;
		r1[2]+=e.z;
		r2[0]+=e.x;
		r2[1]+=e.y;
		r2[2]+=e.z;
		receivedenergy+=e;
	}
*/
	return receivedenergy;
}

D3DXVECTOR3 WBLightMapBuild::receive_light(D3DXVECTOR3& P,D3DXVECTOR3& N,D3DXVECTOR3& E,int flag)
{
/*	if (lastupdate==-1)
		return flyVector(0);

	int x,y;
	float fi=1.0f/sizex,fj=1.0f/sizey,fu,fv;
	float *f=emm,*r=rad;
	flyVector p,n,dir,e;
	float l2,dot1,dot2;

	flyVector receivedenergy(0);
	
	((flyEngineBuild *)g_flyengine)->unshotenergy-=totalenergy;

	fv=fj*0.5f;
	for( y=0;y<sizey;y++,fv+=fj )
	{
		fu=fi*0.5f;
		for( x=0;x<sizex;x++,fu+=fi,receivedenergy+=flyVector(f[0]*pixelarea,f[1]*pixelarea,f[2]*pixelarea),f+=3,r+=3 )
		{
			map_point_local(fu,fv,p,n);

			dir=P-p;
			l2=dir.length2();
			if (l2<10.0f)
				continue;

			dir*=1.0f/(float)sqrt(l2);

			dot1=FLY_VECDOT(dir,n);
			if (dot1<=0.01f)
				continue;

			if (l2<1000)
				dot1*=l2/1000.0f;

			if (flag==0)
			{
				dot2=-FLY_VECDOT(dir,N);
				if (dot2<=0.01f)
					continue;

				e=(E*reflectance)*((dot1*dot2)/(l2*FLY_2PI));
			}
			else 
				e=(E*reflectance)*(dot1/(l2*2.0f*FLY_2PI));

			if ((lmshadows&8) &&
				g_flyengine->collision_test(P+N+dir,p+dir,FLY_TYPE_STATICMESH))
				continue;

			f[0]+=e.x;
			f[1]+=e.y;
			f[2]+=e.z;

			r[0]+=e.x;
			r[1]+=e.y;
			r[2]+=e.z;
		}
	}
	
	e.vec(receivedenergy.x-totalenergy.x,receivedenergy.y-totalenergy.y,receivedenergy.z-totalenergy.z);
	totalenergy=receivedenergy;
	((flyEngineBuild *)g_flyengine)->unshotenergy+=totalenergy;

	return e;*/
	return D3DXVECTOR3(0.0f,0.0f,0.0f);

}

int WBLightMapBuild::emmit_light()
{
/*	if (lastupdate==-1)
	{
		((flyEngineBuild *)g_flyengine)->unshotenergy-=totalenergy;
		totalenergy.vec(0,0,0);
		return 1;
	}

	int x,y,i,j,k;
	flyVector p,n,energy;
	
	float fi=1.0f/sizex,fj=1.0f/sizey,fu,fv;

	flyBspNode *node;
	flyMesh *mesh;
	flyFace **face;
	float *f=emm;
	flyLightMapBuild *lmb;
	flyVector transferedenergy(0);

	fv=fj*0.5f;
	for( y=0;y<sizey;y++,fv+=fj )
	{
		fu=fi*0.5f;
		for( x=0;x<sizex;x++,fu+=fi,f[0]=f[1]=f[2]=0,f+=3,cur_emm++ )
		{
			if (_kbhit())
				if (_getch()==27)
					return 0;
			
			map_point_local(fu,fv,p,n);

			energy.vec(pixelarea*f[0],pixelarea*f[1],pixelarea*f[2]);

			node=g_flyengine->find_node(p);
			if (node==0)
				continue;
			j=node->leaf;

			for( i=0;i<g_flyengine->nleaf;i++ )
			if (FLY_GLOBAL_PVS_TEST(j,i) &&
				g_flyengine->leaf[i]->elem.num &&
				g_flyengine->leaf[i]->elem[0]->type==FLY_TYPE_STATICMESH)
				{
					mesh=((flyStaticMesh *)g_flyengine->leaf[i]->elem[0])->objmesh;
					if (mesh)
					{
					face=mesh->faces;
					for( k=0;k<mesh->nf;k++ )
						if (face[k]->lastupdate!=cur_emm)
						if (face[k]->lm!=-1)
						{
							face[k]->lastupdate=cur_emm;
							lmb=(flyLightMapBuild *)g_flyengine->lm[face[k]->lm];
							if (this!=lmb)
								transferedenergy+=lmb->receive_light(p,n,energy);
						}
						else
						if (face[k]->facetype==FLY_FACETYPE_TRIANGLE_MESH)
						{
							face[k]->lastupdate=cur_emm;
							transferedenergy+=tri_receive_light(face[k],p,n,energy);
						}
					}
				}
		}
	}
	float t=Intensity(transferedenergy)/Intensity(totalenergy)*100.0f;
	if (t>100)
		printf("Warning! energy generation %f%%!\n",t);

	((flyEngineBuild *)g_flyengine)->unshotenergy-=totalenergy;
	totalenergy.vec(0,0,0);

	return 1;
	*/
	return 1;

}
