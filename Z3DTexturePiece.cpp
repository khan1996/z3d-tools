// SimpleParser.cpp: implementation of the CSimpleParser class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DTexturePiece.h"
#include "GMMemory.h"


inline static int GetBPP( D3DFORMAT format )
{
	if( D3DFMT_A8R8G8B8 == format ||
		D3DFMT_X8R8G8B8 == format ||
		D3DFMT_X8L8V8U8 == format ||
		D3DFMT_Q8W8V8U8 == format ||
		D3DFMT_V16U16 == format ||
		D3DFMT_W11V11U10 == format ||
		D3DFMT_A2W10V10U10 == format )
		return 32;

	if( D3DFMT_R5G6B5   == format ||
		D3DFMT_X1R5G5B5 == format ||
		D3DFMT_A1R5G5B5 == format ||
		D3DFMT_A4R4G4B4 == format ||
		D3DFMT_A8R3G3B2 == format ||
		D3DFMT_X4R4G4B4 == format ||
		D3DFMT_V8U8 == format ||
		D3DFMT_L6V5U5 == format )
		return 16;

	if( D3DFMT_R3G3B2 == format || D3DFMT_A8 == format )
		return 8;

	if( D3DFMT_R8G8B8 == format )
		return 24;

	return -1; // no proper format
}


bool Z3DTexturePiece::Load( const char* szFileName )
{
	FILE* fp;

	if( (fp = fopen( szFileName, "rb" )) == NULL )
		return false;
	
	fread( &wId, sizeof(WORD), 1, fp );
	fread( &format, sizeof(D3DFORMAT), 1, fp );
	fread( &sLODCount, sizeof(short), 1, fp );
	fread( &sX, sizeof(short), 1, fp );
	fread( &sY, sizeof(short), 1, fp );
	fread( &sWidth, sizeof(short), 1, fp );
	fread( &sHeight, sizeof(short), 1, fp );

	int bpp = GetBPP(format) / 8; // 'byte per pixel'
	int count, x, y;

	count = 0;
	x = sWidth;
	y = sHeight;
	for( int i = 0; i < sLODCount; i++ )
	{
		count += (x * y * bpp);
		x >>= 1;
		y >>= 1;
	}
	pBitmap = new BYTE[count];

	x = sWidth;
	y = sHeight;
	BYTE* p = pBitmap;
	for(int i = 0; i < sLODCount; i++ )
	{
		count = x * y * bpp;
		fread( p, count, 1, fp );
		p += count;
		x >>= 1;
		y >>= 1;
	}

	fclose(fp);

	return true;
}


bool Z3DTexturePiece::Blt2Texture( IDirect3DTexture8* pTex )
{
	_ASSERT( pTex );

	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc( 0, &desc );
	int nLODLimit = min((int)pTex->GetLevelCount(), sLODCount);

	int bpp = GetBPP(format);

	if( GetBPP(desc.Format) != bpp )
		return false;

	bpp /= 8; // now 'bit per pixel' turns to 'byte per pixel'
	short x = sX, y = sY, w = sWidth, h = sHeight;
	RECT rect;
	D3DLOCKED_RECT lockrect;
	BYTE* pSrc = pBitmap;
	BYTE* pDest;
	int i, j, count;

	for( i = 0; i < Z3DTexture::_GetDetailLevel(); i++ )
	{
		pSrc += w * h * bpp; // skip piece of data for one level of mipmap
		nLODLimit = max( 1, nLODLimit-1 );
		
		x /= 2;
		y /= 2;
		w /= 2;
		h /= 2;
	}

	for( i = 0; i < nLODLimit; i++ )
	{
		 rect.left = x;
		 rect.top = y;
		 rect.right = x+w;
		 rect.bottom =  y+h;

		if( D3DERR_INVALIDCALL == pTex->LockRect( i, &lockrect, &rect, D3DLOCK_NOSYSLOCK ) )
			break;

		pDest = (BYTE*)lockrect.pBits;
		count = w * bpp;
		for( j = 0; j < h; j++ )
		{
			memcpy( pDest, pSrc, count );
			pDest += lockrect.Pitch;
			pSrc += count;
		}

		pTex->UnlockRect(i);

		x /= 2;
		y /= 2;
		w /= 2;
		h /= 2;
	}

	return true;
}
