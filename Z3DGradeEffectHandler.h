#pragma once

#include "d3d8.h"
#include "d3dx8.h"
#include <list>
#include "Z3DTexture.h"


struct Z3D_GRADE_EFFECT_CONSTANTS
{
	D3DXCOLOR c;
	float fPeakValue;	// max 1.0
	int nSignalLength;	// frames
	int nSilentLength;	// frames
};


class CZ3DGradeEffectHandler
{
public:
	CZ3DGradeEffectHandler(void);
	~CZ3DGradeEffectHandler(void);

	void SetLevel( int n );
	void ApplySetting( IDirect3DDevice8* pDevice );
	void DisableSetting( IDirect3DDevice8* pDevice );
	void FrameMove();


	//////////////////////////////////////////////////////////////////////////
	static void SetTextureAniParameter( int nPeriod )
	{
		ms_nTextureAniPeriod = nPeriod;

		ms_nTextureAniCounter = 0;	// conter reset
	}

	static void Process();	// 카운터증가 등 프레임당 한번씩 정기적으로 호출

	static void Init();
	static void Close();

private:
	//////////////////////////////////////////////////////////////////////////
	//
	// Setting inner parameters
	void SetColor( D3DXCOLOR c )
	{
		m_Color = c;
	}

	void SetSignalGraphParameter( float fPeak, int nSignalLength, int nSilentLength )
	{
		m_fPeakValue = fPeak;
		m_nSignalLength = nSignalLength;
		m_nSilentLength = nSilentLength;

		m_nCounter = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	D3DXCOLOR m_Color;

	int m_nCounter;

	float m_fPeakValue; 
	int m_nSignalLength;
	int m_nSilentLength;

	std::list< CZ3DGradeEffectHandler* >::iterator m_itrThis;

	int m_nCurrentLevel;


	//////////////////////////////////////////////////////////////////////////
	static int ms_nTextureAniCounter;
	static int ms_nTextureAniPeriod;

	static Z3D_GRADE_EFFECT_CONSTANTS ms_aGradeEffectParameter[];
	static std::list< CZ3DGradeEffectHandler* > ms_listInstance;

	static Z3DTexture* ms_pEffectTexture;
	static D3DXMATRIX ms_matTextureTransform;
};
