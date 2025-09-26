
#ifndef _RegionTrigger_H_
#define _RegionTrigger_H_


#pragma once

#include "STL.h"

/////////////////////////////////////////////////////////////////////////////
//

template < class T >
class CBinTree;

struct SVector2;

typedef vector<int>					TRIGGERLIST;
typedef pair<SVector2,SVector2>		LINE;
typedef vector<LINE>				LINES;
typedef vector<SVector2>			POLYGON;
typedef pair<TRIGGERLIST, POLYGON>	TRIGGERREGION;
typedef vector<TRIGGERREGION>		TRIGGERREGIONS;
typedef void (*PRINTFUNC)( const char * );

typedef struct _iobuf	FILE;
typedef unsigned char	byte;

/////////////////////////////////////////////////////////////////////////////
//

//2D좌표를 나타내는 구조체
struct SVector2
{
	float x, y;

	SVector2()
		: x( 0.0f ), y( 0.0f )
	{}

	SVector2( float X, float Y )
		: x( X ), y( Y )
	{}

	float	Magnitude() const;
	void	Normalize();
};

/////////////////////////////////////////////////////////////////////////////
//

enum ePos
{
	POS_FRONT,
	POS_BACK,
	POS_COPLANAR,
	POS_SPLIT
};


//선분의 방정식을 나타내는 구조체 y = ax + b
struct SLineVector
{
	float a, b, c;

	SLineVector();
	SLineVector( float A, float B, float C );
	SLineVector( const LINE &, bool bSwapPos = true );

	ePos		TestPoint( SVector2 pt, float Epsilon ) const;
	ePos		TestLine( const LINE &, float Epsilon ) const;
	void		Normalize();
	SVector2	SplitPointOfLine( const LINE & l ) const;
	void		SplitLine( const LINE & in, LINE & front, LINE & back, float Epsilon ) const;
	ePos		TestPolygon( const POLYGON &, float Epsilon ) const;
	bool		SplitPolygon( const POLYGON & in, POLYGON & front, POLYGON & back, float Epsilon ) const;
};


/////////////////////////////////////////////////////////////////////////////
//

class CRegionTrigger
{
protected:
	struct SNode
	{
		union
		{
			LINE *			m_pLine;
			SLineVector	*	m_pLineVector;
			TRIGGERLIST	*	m_pTriggerID;
		};
		byte	m_bNode;											//0 = Leaf, 1 = SLineVector, 2 = LINE

		SNode();
		SNode( const SNode & );
		SNode &	operator=( const SNode & );

		~SNode();
		
		void Create( float a, float b, float c );
		void Create( const TRIGGERLIST & triggers );
		void Create( const SVector2 a, const SVector2 b );
		void Create( const LINES &, CRegionTrigger &, int thisIndex, float Epsilon );
		void Destroy();

		void ToLineVector();
	};

	friend struct SNode;

protected:
	static int		BestSplitLine( const LINES &, float Epsilon );
	static void		RegionsToLines( const TRIGGERREGIONS &, LINES & );
	static SVector2	CenterPoint( const POLYGON & polygon );
	static void		SaveNode( FILE * fp, const SNode & );
	static void		LoadNode( FILE * fp, SNode & );

	void			AssignTrigger( const TRIGGERREGIONS & );
	const TRIGGERLIST &	GetTrigger( SVector2 vecPos, const TRIGGERREGIONS & regions );
	void			AssignTrigger( SVector2 vecPos, const TRIGGERLIST & trigger );
	void			MergeRegions( TRIGGERREGIONS &, const TRIGGERREGIONS & );
	void			SplitToTriangle( const TRIGGERREGIONS & regions, TRIGGERREGIONS & out );
	TRIGGERREGIONS	SplitTriggerRegions( const TRIGGERREGIONS & regions, unsigned splitIndex );
	unsigned		ChildIndex( SLineVector & splitLine, SVector2 pos, unsigned index );
	unsigned		GetLeaf( SVector2 pos );

protected:
	//data members
	typedef CBinTree< SNode >	LINETREE;
	
	LINETREE *	m_pLineTree;
	float		m_fEpsilon;


public:
	//--------------인터페이스-------------------//
	CRegionTrigger();
	~CRegionTrigger();
	void	Create( const TRIGGERREGIONS &, float epsilon = 0.01f );

	void	Create( FILE * );
	void	Destroy();

	void	Save( FILE * );


	const TRIGGERLIST &	CheckTrigger( const SVector2 pos );
	void				Print( PRINTFUNC );
	//-------------------------------------------//
};


/////////////////////////////////////////////////////////////////////////////


#endif