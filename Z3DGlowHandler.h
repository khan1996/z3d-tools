#pragma once

#include "d3dx8.h"
#include <vector>
#include <list>


struct Z3D_GLOW_KEY
{
	D3DXCOLOR c;
	int nKeyFrame;
};

struct Z3D_GLOW_CONSTANTS
{
	std::vector< Z3D_GLOW_KEY > vecGlowKeyData;
	int nTotalFrame;

	Z3D_GLOW_CONSTANTS()
	{
		nTotalFrame = 0;
	}
};

struct CUSTOM_TLVERTEX
{
	float x, y, z, rhw;
	float u, v;

	CUSTOM_TLVERTEX() {}

	CUSTOM_TLVERTEX( float _x, float _y, float _z, float _rhw, float _u, float _v )
	{
        x = _x; y = _y; z = _z; rhw = _rhw;
		u = _u; v = _v;
	}
};


class CZ3DGlowHandler
{
public:
	CZ3DGlowHandler();
	~CZ3DGlowHandler();

	void SetLevel( int nLevel );
	void ApplySetting( IDirect3DDevice8* pDevice );


	static void _SetRenderViewport( D3DVIEWPORT8& vp );
	static void _SetRenderProjectionMatrix( D3DMATRIX& mat );

	static void _Process();	// 카운터증가 등 프레임당 한번씩 정기적으로 호출

	static bool _Init( IDirect3DDevice8* pDevice, bool bLowSpec = false );
	static void _Close();


	static void _PrepareGlowRender();	// 캐릭터별 glow RTT 전 render target 세팅
	static void _EndGlowRender();	// glow RTT 종료. render target 복귀
	static void _ProcessGlow();	// 캐릭터별 glow RTT 된 texture 로 화면에 glow effect 적용



	static Z3D_GLOW_CONSTANTS& _GetGlowConstantField( int nLevelIndex );
	static void _AddGlowConstantKeyframe( int nLevelndex, D3DXCOLOR& c, int nDuration );

static IDirect3DTexture8* TestGetRenderTexture()
{
	return ms_pRenderTargetTexture;
}


private:
	void FrameMove();

	int m_nLevelIndex;	// 0번은 '효과없음'을 나타내므로, 실제 파라미터 배열 참조는 m_nLevelIndex-1 이다
	int m_nCurrentFrame;

	D3DXCOLOR m_CurrentColor;

	std::list< CZ3DGlowHandler* >::iterator m_itrThis;


	static IDirect3DDevice8* ms_pDevice;
	static IDirect3DTexture8* ms_pRenderTargetTexture;
	static IDirect3DSurface8* ms_pRenderTargetDepth;

	static IDirect3DSurface8* ms_pOldBackBuffer;
	static IDirect3DSurface8* ms_pOldDepthBuffer;


	static Z3D_GLOW_CONSTANTS ms_aGlowParameter[];
	static std::list< CZ3DGlowHandler* > ms_listInstance;

	static CUSTOM_TLVERTEX ms_Vert[4];

	static D3DVIEWPORT8 ms_RenderViewport, ms_SaveViewport;
	static D3DMATRIX ms_RenderProjectionMatrix, ms_SaveProjectionMatrix;

	static bool ms_b16bitMode;

	static bool ms_bSkipGlow;
};