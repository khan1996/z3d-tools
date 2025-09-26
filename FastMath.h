// FastMath.h: interface for the CFastMath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTMATH_H__ED69578B_18C1_42EA_9C5E_888DC38101C2__INCLUDED_)
#define AFX_FASTMATH_H__ED69578B_18C1_42EA_9C5E_888DC38101C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include "MathBase.h"
#define FP_BITS(fp) (*(DWORD *)&(fp))

class CFastMath  
{
	typedef union FastSqrtUnion {
	  float f;
	  unsigned int i;
	} FastSqrtUnion;

	static unsigned int m_FastSqrtTable[0x10000];
	static unsigned short m_FastHeToBi[0x100];
	static long m_HoldRand;

public:	

	// constructor and destructor
	CFastMath();
	virtual ~CFastMath();

	static void Init();
	inline static float FastSqrt(float n) // 빠른 제곱근 연산을 하는 함수.
	{
		if (FP_BITS(n) == 0)
			return 0.0f;                 // check for square root of 0
		FP_BITS(n) = m_FastSqrtTable[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
		return n;
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// converts ascii type of IPX address to hexadecimal format
	// returns the equivalent binary value for an individual character specified in the ascii format
	
	static UCHAR	BiToHe(char cBin);	// SPX, IPX 주소 계산을 위해 사용하는 함수
	static void		AcToHe(char *szDst, char *szSrc, int iCount);	// SPX, IPX 주소 계산을 위해 사용하는 함수

	static char				StrToHex08(char *szSrc);	// convert string to hexa
	static unsigned short	StrToHex16(char *szSrc);	// convert string to hexa
	static DWORD			StrToHex32(char *szSrc);	// convert string to hexa
	static _int64			StrToHex64(char *szSrc);	// convert string to hexa

	static inline void Hex08ToStr(char *szDest, BYTE hex);		// convert hexa to string
	static inline void Hex16ToStr(char *szDest, WORD hex);		// convert hexa to string
	static inline void Hex32ToStr(char *szDest, DWORD hex);		// convert hexa to string
	static inline void Hex64ToStr(char *szDest, DWORD64 hex);	// convert hexa to string

	static char				Atoc(char *szSrc);		// ascii to U8
	static unsigned short	Atos(char *szSrc);		// ascii to U16
	static unsigned int		Atoi(char *szSrc);		// ascii to U32
	static _int64			Atol64(char *szSrc);	// ascii to U64

	static void SRand( unsigned int seed );
	static int Rand( void );

	// 리턴값은 0 ~ nExtent - 1의 범위를 갖는다.
	static DWORD ComplexRandom(int nExtent);
	static DWORD AGLRandom(int nExtent) { return ComplexRandom(nExtent); };

};

inline void CFastMath::Hex08ToStr( char *szDest, BYTE hex )
{
	*((WORD *) szDest) = m_FastHeToBi[ hex ];	szDest += 2;
	*(szDest) = '\0';
}

inline void CFastMath::Hex16ToStr( char *szDest, WORD hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 4) = '\0';
}

inline void CFastMath::Hex32ToStr( char *szDest, DWORD hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 8) = '\0';
}

inline void CFastMath::Hex64ToStr( char *szDest, DWORD64 hex )
{
	LPBYTE pSrc = (LPBYTE) &hex;

#ifdef BIG_ENDIAN
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  8)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 10)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 12)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 14)) = m_FastHeToBi[ *(pSrc++) ];
#else
	*((WORD *) (szDest + 14)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 12)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest + 10)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  8)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  6)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  4)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  2)) = m_FastHeToBi[ *(pSrc++) ];
	*((WORD *) (szDest +  0)) = m_FastHeToBi[ *(pSrc++) ];
#endif
	*(szDest + 16) = '\0';
}

#endif // !defined(AFX_FASTMATH_H__ED69578B_18C1_42EA_9C5E_888DC38101C2__INCLUDED_)
