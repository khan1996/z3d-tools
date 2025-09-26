#if !defined(__MISC_H__)
#define __MISC_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#pragma warning( disable : 4005 )



//#define _ASSERT(p) ((void)(p))

#include <windows.h>
#include <d3d8.h>
#include <math.h>

#ifdef _DEBUG
	#define _ASSERT(p) ((void)(p))	
	#define _ASSERTE(p) ((void)(p))	
#endif 

#ifndef _DEBUG
	#define _ASSERT(p) ((void)(p))	
	#define _ASSERTE(p) ((void)(p))	
#endif 

#define FLOAT_EPSILON 1.0e-5f
#define FLOAT_PHI 3.1415926535f
#define FLOAT_DEG_1 (FLOAT_PHI / 180.0f)
#define FLOAT_DEG(d) ((d)*FLOAT_DEG_1)


// Miscellaneous functions & macros
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(p)  { if(p) { delete [](p);     (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif


template< class _T >
struct ptr_greater
{
	bool operator()( const _T p1, const _T p2) const
	{
		return (*p1) < (*p2);
	}
};


struct szi_less
{
	bool operator () ( const char* sz1, const char* sz2 ) const
	{
		return (stricmp( sz1, sz2 ) < 0);
	}
};


struct VECTOR4D
{
	float x, y, z, w;
};


struct VECTOR3D
{
	float x, y, z;
};


struct VECTOR2D
{
	float x, y;
};

inline BOOL IsEquallF( float f1, float f2 )
{
	float f;

	f = f1 - f2;
	if( f < 0 ) f = -f;

	// Returns true if two float number is equall(differs small enough)
	return f < FLOAT_EPSILON;
}

inline BOOL IsEquallStr( const char* str1, const char* str2 )
{
	int i = 0;

	while( str1[i] && str2[i] )
	{
		if( str1[i] != str2[i] ) return FALSE;
		i++;
	}

	return TRUE;
}

inline bool CompareHeader( const char* szHeaderToken, const char* szString )
{
	size_t n = strlen( szHeaderToken );

	if( strnicmp( szHeaderToken, szString, n ) == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool CheckProbability( long l_prob_numerator, long l_prob_denominator )
{
	return (rand() % l_prob_denominator) < l_prob_numerator;
}

/* Disabled Swap functions because of STL swap function templates.
inline void Swap( long &l1, long &l2 )
{
	l1 ^= l2 ^= l1 ^= l2;
}


inline void Swap( float &f1, float &f2 )
{
	float f;
	f = f1;
	f1 = f2;
	f2 = f1;
}


inline void Swap( int &i1, int &i2 )
{
	i1 ^= i2 ^= i1 ^= i2;
}


inline void Swap( WORD &w1, WORD &w2 )
{
	w1 ^= w2 ^= w1 ^= w2;
}
*/


inline void WRONG_WAY(const char* sz)
{
	//_ASSERT(!sz);
}


struct D3DVERTEX
{
	D3DVERTEX() {}
	D3DVERTEX( float _x, float _y, float _z, float _nx, float _ny, float _nz, float _tu, float _tv)
	{
		x = _x;
		y = _y;
		z = _z;

		nx = _nx;
		ny = _ny;
		nz = _nz;

		tu = _tu;
		tv = _tv;
	}


	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
};

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)


// simple LVERTEX, just for line colored drawing
struct D3DSLVERTEX
{
	D3DSLVERTEX() {}
	D3DSLVERTEX( float _x, float _y, float _z, D3DCOLOR _c)
	{
		x = _x;
		y = _y;
		z = _z;

		c = _c;
	}

	float x, y, z;
	D3DCOLOR c;
};

#define D3DFVF_SLVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE )


// LVERTEX, as you can figure out...
struct D3DLVERTEX
{
	D3DLVERTEX() {}
	D3DLVERTEX( float _x, float _y, float _z, D3DCOLOR _c, float _u, float _v )
	{
		x = _x;
		y = _y;
		z = _z;
		
		c = _c;

		u = _u;
		v = _v;
	}
	
	float x, y, z;
	D3DCOLOR c;
	float u, v;
};

#define D3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


/*typedef float FLOAT2[2];
typedef float FLOAT3[3];
typedef float FLOAT4[4];
typedef BYTE BYTE2[2];
typedef BYTE BYTE3[3];
typedef BYTE BYTE4[4];
typedef char CHR2[2];
typedef char CHR3[3];
typedef char CHR4[4];
typedef int INT2[2];
typedef int INT3[3];
typedef int INT4[4];*/

struct INT3
{
	union
	{
		int _0, _1, _2;
		int a[3];
	};
};
/*template<class _T>
struct A2
{
	union
	{
		_T _0, _1;
		_T a[2];

	};
};*/


inline long GetSizeByBank( long lActualSize, long lBankSize )
{
	float fFactor = float(lActualSize) / lBankSize;
	fFactor = ceilf( fFactor );
	return long(lBankSize * fFactor);
}



// define 'aton()' func. similar to 'atoi()' but recognizes hexadecimal number
#define aton(a) ( (int)strtol( (a), NULL, 0 ) )

#define range_rand(base, range) (((float)rand()/RAND_MAX)*(range) + (base))

#endif // !defined(__MISC_H__)
