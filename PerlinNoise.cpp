// PerlinNoise.cpp: implementation of the CPerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

#include "PerlinNoise.h"
#include <stdlib.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
float CPerlinNoise::m_ExponentArray[MAX_OCTAVES];
float CPerlinNoise::m_fH;
float CPerlinNoise::m_fLacunarity;
float CPerlinNoise::m_fOctaves;
float CPerlinNoise::m_fOffset;
float CPerlinNoise::m_fGain;
float CPerlinNoise::m_fDepth;
int CPerlinNoise::m_P[PERLIN_B + PERLIN_B + 2];
vector3 CPerlinNoise::m_G2[PERLIN_B + PERLIN_B + 2];
int CPerlinNoise::m_RandomSeed[PERLIN_B * 7];

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec.v[i] + PERLIN_NE;\
	b0 = ((int)t) & PERLIN_BM;\
	b1 = (b0+1) & PERLIN_BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;


CPerlinNoise::CPerlinNoise()
{
	//LogMessage("PerlinNode Constructor");
	//LogMessage("PerlinNode Constructor complete");
}

CPerlinNoise::~CPerlinNoise()
{

}

float CPerlinNoise::HybridMultifractal(vector3 vec)
{		
	
		vec.x/=m_fDepth;
	
		vec.y/=m_fDepth;
	
		vec.z/=m_fDepth;
	float value,signal,weight,remainder;
	value=Noise2(vec)+m_fOffset;
	weight=m_fGain*value;

	vec.x*=m_fLacunarity;
	vec.y*=m_fLacunarity;
	vec.z*=m_fLacunarity;	

	int i=1;
	for(;weight>0.001 && i<m_fOctaves;i++)
	{
		if(weight>1.0f) 
			weight=1.0f;
		signal=(Noise2(vec) + m_fOffset) * m_ExponentArray[i];
		value+=weight*signal;
		weight*=m_fGain*signal;
		vec.x*=m_fLacunarity;
		vec.y*=m_fLacunarity;
		vec.z*=m_fLacunarity;
	}
	remainder=m_fOctaves-(int)m_fOctaves;
	if(remainder)
		value+=remainder * Noise2(vec) * m_ExponentArray[i];	
	if(value>=0)
		value=powf(value*150.0f,2.0f);	
	else
		value=-powf(value*150.0f,2.0f);	
	return value;
}

void CPerlinNoise::Init()
{	
	//LogMessage("PerlinNode Init");
	SetNoiseValue(1.0f,6.0f,100.0f,0.6f,1.1f,60000.0f);
	int i=0;
	for(;i<MAX_OCTAVES;i++)
	{
		m_ExponentArray[i]=powf(m_fLacunarity,-i*m_fH);
	}
	LoadSeed("c:\\Noise.val"); 
	int *vrand=m_RandomSeed;

	for(i=0;i<PERLIN_B;i++)
	{
		m_P[i]=i;
		vrand++;
		m_G2[i].x=(float)((*(vrand++) % (PERLIN_B + PERLIN_B)) - PERLIN_B) / PERLIN_B;
		m_G2[i].y=(float)((*(vrand++) % (PERLIN_B + PERLIN_B)) - PERLIN_B) / PERLIN_B;
		m_G2[i].Normalize();
		vrand++;
		vrand++;
		vrand++;
	}

	int k,j;
	while(--i)
	{
		k=m_P[i];
		m_P[i]=m_P[j = *(vrand++)% PERLIN_B];
		m_P[j]=k;
	}
	for(i=0;i<PERLIN_B+2;i++)
	{
		m_P[PERLIN_B+i] = m_P[i];
		m_G2[PERLIN_B+i]=m_G2[i];
	}	
	//LogMessage("PerlinNode Init complete");
}

float CPerlinNoise::Noise2(vector3 vec)
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, sx, sy, a, b, t, u, v;
	register int i, j;	

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = m_P[ bx0 ];
	j = m_P[ bx1 ];

	b00 = m_P[ i + by0 ];
	b10 = m_P[ j + by0 ];
	b01 = m_P[ i + by1 ];
	b11 = m_P[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);
	
	u=rx0*m_G2[b00].x+ry0*m_G2[b00].y;
	v=rx1*m_G2[b10].x+ry0*m_G2[b10].y;	
	a=lerp(sx,u,v);	

	u=rx0*m_G2[b01].x+ry1*m_G2[b01].y;
	v=rx1*m_G2[b11].x+ry1*m_G2[b11].y;
	b = lerp(sx, u, v);	
	return lerp(sy, a, b);	
}

void CPerlinNoise::LoadSeed(char *strFilename)
{
	//FILE *fp=fopen(strFilename,"rb");
	//if(fp==NULL)
	{
		for(int i=0;i<PERLIN_B*7;i++)
			m_RandomSeed[i]=rand();
		//fp=fopen(strFilename,"wb");
		//fwrite(m_RandomSeed,sizeof(int)*(B*7),1,fp);
		//fclose(fp);
		return;
	}
	//fread(m_RandomSeed,sizeof(int)*(B+B+B),1,fp);
	//fclose(fp);
	//return;
}
