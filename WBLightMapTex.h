// WBLightMapTex.h: interface for the WBLightMapTex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WBLIGHTMAPTEX_H__4F1BB2A6_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
#define AFX_WBLIGHTMAPTEX_H__4F1BB2A6_C09E_11D7_8C7F_0040F41B957B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WBLightMapTex  
{
public:
	WBLightMapTex(int sx,int sy,int bp=3);
	virtual ~WBLightMapTex();
	unsigned char *bmp;	//!< the light maps bitmap
	int sizex,			//!< x size of the picture (power of 2)
		sizey;			//!< y size of the picture (power of 2)
	int bytesx,			//!< number of bytes per line (sizex*bytespixel)
		bytesxy;		//!< total bytes of the bitmap (bytesx*sizey)
	int bytespixel;		//!< the light map bytes per pixel (3: RGB, 4: RGBA)

};

#endif // !defined(AFX_WBLIGHTMAPTEX_H__4F1BB2A6_C09E_11D7_8C7F_0040F41B957B__INCLUDED_)
