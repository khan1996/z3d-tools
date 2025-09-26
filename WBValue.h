/* ************************************************
	WBValue :: WBEngine's value type define
************************************************** */

#ifndef __WBVALUE_H__
#define __WBVALUE_H__

#pragma once 

#include <d3dx8.h>
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
//#include "WBLog.h"
#include "RBitSet.h"

#define WBRAD 0.0174532925f
#define WORD_MAX 65536
#define FEPSILON 1e-12


//enum WBPLAN { LEFT_P,RIGHT_P,TOP_P,BOTTOM_P,NEAR_P,FAR_P};
//enum WBAXIS {X,Y,Z};

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#define FAILED(Status) ((HRESULT)(Status)<0)

#define MIN( p, q )  ( ( ( p )<( q ) )?( p ):( q ) )
#define MAX( p, q )  ( ( ( p )<( q ) )?( q ):( p ) )
#define RANDOM_FLOAT		( ( ( float )rand( ) )/RAND_MAX )


const float FINFINITY = FLT_MAX;
const double DINFINITY = DBL_MAX;

inline DWORD FtoDW( float f ) { return *((DWORD*)&f); }

#endif

