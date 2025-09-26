// Z3DChrEventGenerator.h: interface for the CZ3DChrEventGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DCHREVENTGENERATOR_H__2B89C8C6_9900_43B9_BC56_9FC64A1B20C3__INCLUDED_)
#define AFX_Z3DCHREVENTGENERATOR_H__2B89C8C6_9900_43B9_BC56_9FC64A1B20C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3D_GLOBALS.h"
#include <map>
#include <vector>
#include <utility>


struct Z3D_EVENT_ELEMENT
{
	long lFrame;
	Z3DTOK tokEvent;
	Z3DTOK tokSecondaryEvent;

	// 분수로 나타낸 확률 데이터. 항시발생 이벤트는 denominator가 0
	long lProb_Numerator;
	long lProb_Denominator;

	bool operator < ( const Z3D_EVENT_ELEMENT &op ) const
	{
		return lFrame < op.lFrame;
	}
};

// vector를 쓰자
/*struct Z3D_EVENT_DATA
{
	long lCount;

	Z3D_EVENT_ELEMENT* pData;
	
	~Z3D_EVENT_DATA()
	{
		SAFE_DELETEA( pData );
	}
};*/

class CZ3DChrEventGenerator  
{
public:
	CZ3DChrEventGenerator();
	virtual ~CZ3DChrEventGenerator();

	void CheckEvent( float fCurrentFrame, float fPrevFrame,
		std::vector<Z3DTOK> &r_vecEvent, std::vector<Z3DTOK> &r_vecSecondaryEvent );

	bool SetTokAniPack( Z3DTOK tokFileName );
	Z3DTOK GetTokAniPack();


	static bool _Load( const char* szFileName );
	static void _Close();

	static void _RetrieveEventDataList( std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* >* &pMap );

protected:
	std::vector<Z3D_EVENT_ELEMENT>* m_rp_vecEventData;
	Z3DTOK m_tokCurrentAniPack;
	
	static std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* > ms_mapTok2EventData;
};

#endif // !defined(AFX_Z3DCHREVENTGENERATOR_H__2B89C8C6_9900_43B9_BC56_9FC64A1B20C3__INCLUDED_)
