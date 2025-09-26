// WBLightMapTex.cpp: implementation of the WBLightMapTex class.
//
//////////////////////////////////////////////////////////////////////

#include "WBLightMapTex.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WBLightMapTex::WBLightMapTex(int sx,int sy,int bp)
{
	bytespixel=bp;
	sizex=sx; 
	sizey=sy; 
	bytesx=sx*bytespixel;
	bytesxy=bytesx*sizey;
	bmp=new unsigned char [bytesxy];
}

WBLightMapTex::~WBLightMapTex()
{
	delete[] bmp;
}
