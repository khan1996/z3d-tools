// PerlinNoise.h: interface for the CPerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERLINNOISE_H__DF635499_ADD4_4465_901C_9064E518F452__INCLUDED_)
#define AFX_PERLINNOISE_H__DF635499_ADD4_4465_901C_9064E518F452__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <3DMath.h>
#include <stdio.h>
#include "StateLog.h"
const long MAX_OCTAVES=128;

#define PERLIN_B 0x100
#define PERLIN_BM 0xff

#define PERLIN_NE 0x1000
#define PERLIN_NP 12   /* 2^N */
#define PERLIN_NM 0xfff

class CPerlinNoise  
{	
public:

	static void SetNoiseValue(float H,float Lac,float Oct,float Off,float Gain,float Depth)
	{
		m_fH=H;
		m_fLacunarity=Lac;
		m_fOctaves=Oct;
		m_fOffset=Off;
		m_fGain=Gain;		
		m_fDepth=Depth;
	}
	
    static void LoadSeed(char *strFilename);
	static float Noise2(vector3 vec);
	static void Init();
	static float HybridMultifractal(vector3 point);
	
    CPerlinNoise();
	virtual ~CPerlinNoise();

private:

	static float m_fH,m_fLacunarity,m_fOctaves,m_fOffset,m_fGain,m_fDepth;
	static float m_ExponentArray[MAX_OCTAVES];
	static int m_P[PERLIN_B + PERLIN_B + 2];
	static vector3 m_G2[PERLIN_B + PERLIN_B + 2];
	static int m_RandomSeed[PERLIN_B * 7];	
};

#endif // !defined(AFX_PERLINNOISE_H__DF635499_ADD4_4465_901C_9064E518F452__INCLUDED_)
