// Z3DTexturePiece.h: interface for the Z3DTexturePiece class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DTEXTUREPIECE_H__C52E1602_D5D1_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DTEXTUREPIECE_H__C52E1602_D5D1_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <stdio.h>
#include "misc.h"

#include "Z3Dtexture.h"


#define Z3D_MIPMAP_COUNT 10


struct Z3DTexturePiece  
{
	WORD wId;

	D3DFORMAT format;

	short sLODCount;

	short sX;
	short sY;

	short sWidth;
	short sHeight;

	BYTE* pBitmap;

	Z3DTexturePiece()
	{
		format = D3DFMT_UNKNOWN;

		sLODCount = 0;

		sX = 0;
		sY = 0;

		sWidth = 0;
		sHeight = 0;

		pBitmap = NULL;
	}
	
	~Z3DTexturePiece()
	{
		SAFE_DELETE(pBitmap);
	}

	int GetId()
	{
		return wId;
	}


	bool Load( const char* szFileName );
	bool Blt2Texture( IDirect3DTexture8* pTex );
};


#endif // !defined(AFX_Z3DTEXTUREPIECE_H__C52E1602_D5D1_11D4_AD2B_0000E8EB4C69__INCLUDED_)
