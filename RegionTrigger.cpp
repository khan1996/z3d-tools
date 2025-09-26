
#include "RegionTrigger.h"
#include "BinTree.h"
#include <vector>
#include <set>
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <math.h>
#include "GMMemory.h"


#ifdef _DEBUG
	#define LOG_POLYGONTOLINE
	#define LOG_PRINTTREE
#endif



TRIGGERLIST EmptyTriggerList;

//////////////////////////////SLineVector////////////////////////////////////
//

float	SVector2::Magnitude() const
{
	return (float)sqrt( ( x * x ) + ( y * y ) );
}

void	SVector2::Normalize()
{
	float magnitude = Magnitude();

	x /= magnitude;
	y /= magnitude;
}

//////////////////////////////SLineVector////////////////////////////////////
//

SLineVector::SLineVector()
	: a( 0.0f ), b( 0.0f ), c( 0.0f )
{}

SLineVector::SLineVector( float A, float B, float C )
	: a( A ), b( B ), c( C )
{
}

SLineVector::SLineVector( const LINE & line, bool bSwapPos )
{
	SVector2 pt1 = line.first;
	SVector2 pt2 = line.second;

	if( bSwapPos )
	{
		pt1 = ( line.first.x <= line.second.x ? line.first : line.second );
		pt2 = ( line.first.x <= line.second.x ? line.second : line.first );
	}

	a = pt1.y - pt2.y;
	b = pt2.x - pt1.x;
	c = pt1.x * pt2.y - pt2.x * pt1.y;

//	float temp = a * line.first.x + b * line.first.y + c;
//	assert( temp == 0 );
//	temp = a * line.second.x + b * line.second.y + c;
//	assert( temp == 0 );
}

ePos	SLineVector::TestPoint( SVector2 pt, float Epsilon ) const
{
	float r = a * pt.x + b * pt.y + c;

	if( r > Epsilon )
		return POS_FRONT;
	if( r < -Epsilon )
		return POS_BACK;

	return POS_COPLANAR;
}

ePos	SLineVector::TestLine( const LINE & line, float Epsilon ) const
{
	ePos res1 = TestPoint( line.first, Epsilon );
	ePos res2 = TestPoint( line.second, Epsilon );

	if( res1 == POS_FRONT )
	{
		if( res2 == POS_BACK )
			return POS_SPLIT;
		else
			return POS_FRONT;
	}
	else if( res1 == POS_BACK )
	{
		if( res2 == POS_FRONT )
			return POS_SPLIT;
		else
			return POS_BACK;
	}
	else		//coplanar
	{
		if( res2 == POS_FRONT )
			return POS_FRONT;
		else if( res2 == POS_BACK )
			return POS_BACK;
		else
			return POS_COPLANAR;
	}
}

SVector2	SLineVector::SplitPointOfLine( const LINE & l ) const
{
	SLineVector line( l );
	
	SVector2 pos;

	pos.x = ( line.b * c - b * line.c ) / ( line.a * b - a * line.b );
	pos.y = ( line.a * c - a * line.c ) / ( a * line.b - line.a * b );
	
	int iPosX,iPosY;
	iPosX = (int)pos.x;
	iPosY = (int)pos.y;

	pos.x = (float)iPosX;
	pos.y = (float)iPosY;

//	pos.x = (int)pos.x;
//	pos.y = (int)pos.y;

	return pos;
}

void	SLineVector::SplitLine( const LINE & in, LINE & front, LINE & back, float Epsilon ) const
{
	SVector2 pos = SplitPointOfLine( in );

	front.first = ( TestPoint( in.first, Epsilon ) == POS_FRONT ? in.first : in.second );
	front.second = pos;

	back.first = pos;
	back.second = ( TestPoint( in.second, Epsilon ) == POS_BACK ? in.second : in.first );
}


ePos	SLineVector::TestPolygon( const POLYGON & polygon, float Epsilon ) const
{
	bool allfront=true, allback=true;

	for( int i=0; i < (int)polygon.size(); i++ )
	{
		ePos result = TestPoint( polygon[i], Epsilon );

		if( result == POS_BACK )
			allfront = false;
		else if( result == POS_FRONT )
			allback = false;
	}
	if( allfront && !allback )
	{
		return POS_FRONT;
	}
	else if( !allfront && allback )
	{
		return POS_BACK;
	}
	else if( !allfront && !allback )
	{
		return POS_SPLIT;
	}

	return POS_COPLANAR;
}

void	SLineVector::Normalize()
{
	float magnitude = (float)sqrt( ( a * a ) + ( b * b ) + ( c * c ) );

	a /= magnitude;
	b /= magnitude;
	c /= magnitude;
}

bool	SLineVector::SplitPolygon( const POLYGON & in, POLYGON & front, POLYGON & back, float Epsilon ) const
{
	if( in.size() == 0 )
		return false;

	front.clear();
	back.clear();

	int thisIndex = in.size() - 1;

	ePos thisRes = TestPoint( in[thisIndex], Epsilon );
	ePos nextRes;

	for( int nextIndex = 0; nextIndex < (int)in.size(); nextIndex++ )
	{
		nextRes = TestPoint( in[nextIndex], Epsilon );

		if( thisRes == POS_FRONT )
		{
			front.push_back( in[thisIndex] );
		}

		if( thisRes == POS_BACK )
		{
			back.push_back( in[thisIndex] );
		}

		if( thisRes == POS_COPLANAR )
		{
			front.push_back( in[thisIndex] );
			back.push_back( in[thisIndex] );
		}

		if( ( thisRes == POS_BACK && nextRes == POS_FRONT ) ||
			( thisRes == POS_FRONT && nextRes == POS_BACK ) )
		{
			SVector2 splitPoint = SplitPointOfLine( LINE( in[thisIndex], in[nextIndex] ) );
			front.push_back( splitPoint );
			back.push_back( splitPoint );
		}
		thisIndex = nextIndex;
		thisRes = nextRes;
	}

	if( front.size() >= 3 && back.size() >= 3 )
	{
		return true;
	}

	return false;
}


//////////////////////////CRegionTrigger::SNode/////////////////////////////////
//

CRegionTrigger::SNode::SNode()
	: m_bNode( 0 ), m_pLine( NULL )
{

}

CRegionTrigger::SNode::SNode( const SNode & rhs )
	: m_bNode( 0 ), m_pLine( NULL )
{
	this->operator=( rhs );
}

CRegionTrigger::SNode &	CRegionTrigger::SNode::operator=( const SNode & rhs )
{
	Destroy();

	m_bNode = rhs.m_bNode;

	if( rhs.m_pTriggerID == NULL )
		return *this;

	if( m_bNode == 0 )
	{
		m_pTriggerID = new TRIGGERLIST( *rhs.m_pTriggerID );
	}
	else if( m_bNode == 1 )
	{
		m_pLineVector = new SLineVector( *rhs.m_pLineVector );
	}
	else
	{
		m_pLine = new LINE( *rhs.m_pLine );
	}

	return *this;
}

CRegionTrigger::SNode::~SNode()
{
	Destroy();
}

void CRegionTrigger::SNode::Create( float a, float b, float c )
{
	Destroy();

	m_bNode = 1;
	m_pLineVector = new SLineVector( a, b, c );
}

void CRegionTrigger::SNode::Create( const TRIGGERLIST & triggers )
{
	Destroy();

	m_bNode = 0;
	m_pTriggerID = new TRIGGERLIST( triggers );
}

void CRegionTrigger::SNode::Create( const SVector2 a, const SVector2 b )
{
	m_bNode = 2;
	m_pLine = new LINE( a, b );
}

void CRegionTrigger::SNode::Create( const LINES & lines, CRegionTrigger & regionTrigger, int thisIndex, float Epsilon )
{
	Destroy();

	if( lines.size() == 0 )
		return;

	int bestIndex = BestSplitLine( lines, Epsilon );
	
	if( bestIndex  < 0 )
	{
		Create( TRIGGERLIST() );
		return;
	}

	m_bNode = 2;
	m_pLine = new LINE( lines[ bestIndex ] );
	SLineVector lineVector( *m_pLine );

	LINES frontList, backList;

	for( int i = 0; i < (int)lines.size(); i++ )
	{
		if( i == bestIndex ) continue;

		ePos res = lineVector.TestLine( lines[i], Epsilon );

		switch( res )
		{
		case POS_FRONT:
			frontList.push_back( lines[i] );
			break;

		case POS_BACK:
			backList.push_back( lines[i] );
			break;

		case POS_SPLIT:
			{
				LINE front, back;
				lineVector.SplitLine( lines[i], front, back, Epsilon );
				frontList.push_back( front );
				backList.push_back( back );
				break;
			}

		case POS_COPLANAR:			
			break;
		}
	}

	int leftIndex = LINETREE::LeftChild( thisIndex );
	SNode & leftNode = regionTrigger.m_pLineTree->SetData( leftIndex );

	if( frontList.size() )
	{
		leftNode.Create( frontList, regionTrigger, leftIndex, Epsilon );
	}
	else
	{
		leftNode.Create( TRIGGERLIST() );
	}
	
	int rightIndex = LINETREE::RightChild( thisIndex );
	SNode & rightNode = regionTrigger.m_pLineTree->SetData( rightIndex );

	if( backList.size() )
	{
		rightNode.Create( backList, regionTrigger, rightIndex, Epsilon );
	}
	else
	{
		rightNode.Create( TRIGGERLIST() );
	}
}

void CRegionTrigger::SNode::Destroy()
{
	if( m_bNode == 0 )
	{
		delete m_pTriggerID;
		m_pTriggerID = NULL;
	}
	else if( m_bNode == 1 )
	{
		delete m_pLineVector;
		m_pLineVector = NULL;
	}
	else
	{
		delete [] m_pLine;
		m_pLine = NULL;
	}
}

void CRegionTrigger::SNode::ToLineVector()
{
	if( m_bNode == 2 )
	{
		SLineVector * pLineVector = new SLineVector( *m_pLine );
		delete [] m_pLine;
		m_pLineVector = pLineVector;
		m_bNode = 1;
	}
}

//////////////////////////////////////////////////////////////////////////////
//

#ifdef LOG_PRINTTREE

ofstream tree_file;

void	PrintToFile( const char * szMsg )
{
	if( tree_file.is_open() )
		tree_file << szMsg;
}

#endif

/////////////////////////////CRegionTrigger////////////////////////////////////
//


CRegionTrigger::CRegionTrigger()
	: m_pLineTree( new LINETREE )
	, m_fEpsilon( 0.01f )
{
}

CRegionTrigger::~CRegionTrigger()
{
	Destroy();
	delete m_pLineTree;
}



void		CRegionTrigger::Create( const TRIGGERREGIONS & regions, float epsilon )
{
	Destroy();

	m_fEpsilon = epsilon;

	LINES lines;
	RegionsToLines( regions, lines );

	m_pLineTree->Create();

	SNode & node = m_pLineTree->SetData( m_pLineTree->Root() );
	node.Create( lines, *this, m_pLineTree->Root(), m_fEpsilon );

	AssignTrigger( regions );


	#ifdef LOG_PRINTTREE
	tree_file.open( "TriggerTree.txt" );
		Print( PrintToFile );
	tree_file.close();
	#endif
}

void	CRegionTrigger::Create( FILE * fp )
{
	Destroy();

	m_pLineTree->Create( fp, LoadNode );
}

void	CRegionTrigger::Destroy()
{
	m_pLineTree->Destroy();
}

void	CRegionTrigger::Save( FILE * fp )
{
	m_pLineTree->Save( fp, SaveNode );
}

unsigned	CRegionTrigger::ChildIndex( SLineVector & splitLine, SVector2 pos, unsigned index )
{
	ePos relativePos = splitLine.TestPoint( pos, m_fEpsilon );

	if( relativePos == POS_FRONT )
		return LINETREE::LeftChild( index );
	else if( relativePos == POS_BACK )
		return LINETREE::RightChild( index );
	else
	{
		//Coplanar일 경우 오차범위를 줄여서 다시 시도
		relativePos = splitLine.TestPoint( pos, m_fEpsilon/100.0f );
		
		if( relativePos == POS_FRONT )
			return LINETREE::LeftChild( index );
		else
			return LINETREE::RightChild( index );
	}
}

//지정된 지점(pos)에 해당되는 잎(leaf)의 인덱스를 리턴
unsigned	CRegionTrigger::GetLeaf( SVector2 pos )
{
	unsigned index = 1;

	while( index < m_pLineTree->Size() && !m_pLineTree->IsLeaf( index )  )
	{
		const SNode & node = m_pLineTree->GetData( index );
		
		if( node.m_bNode == 1 )
		{
			index = ChildIndex( *node.m_pLineVector, pos, index );
		}
		else if( node.m_bNode == 2 )
		{
			SLineVector splitLine( *node.m_pLine );

			index = ChildIndex( splitLine, pos, index );
		}
		else
		{
			//1과 2 이외의 값은 무효
			return 0xffffffff;
		}
	}
	return index;
}


const TRIGGERLIST &	CRegionTrigger::CheckTrigger( SVector2 pos )
{
	unsigned index = GetLeaf( pos );

	if( !m_pLineTree->IsValid( index ) )
		return EmptyTriggerList;

	const SNode & node = m_pLineTree->GetData( index );

	return *node.m_pTriggerID;
}

void			CRegionTrigger::Print( PRINTFUNC printFunc )
{
	if( m_pLineTree->IsEmpty() )
		return;

	//전위순회를 하며 내용을 출력한다.
	for( LINETREE::iterator_preorder i = m_pLineTree->Begin_preorder(); i != m_pLineTree->End_preorder(); i++ )
	{
		char szBuffer[1024];

		sprintf( szBuffer, "<%d> ", i.GetIndex() );
		printFunc( szBuffer );

		if( i->m_bNode == 0 )
		{
			if( i->m_pTriggerID )
			{
				TRIGGERLIST & tList = *(i->m_pTriggerID);

				if( tList.size() >= 1 )
				{
					sprintf( szBuffer, ", %d", tList[0] );
					printFunc( szBuffer );
				}

				for( int i = 1; i < (int)tList.size(); i++ )
				{
					sprintf( szBuffer, ", %d", tList[i] );
					printFunc( szBuffer );
				}

				printFunc( "\n" );
			}
		}
		else if( i->m_bNode == 1 )
		{
			sprintf( szBuffer, "%fx + %fy + %f = 0\n", i->m_pLineVector->a, i->m_pLineVector->b, i->m_pLineVector->c );
			printFunc( szBuffer );
		}
		else if( i->m_bNode == 2 )
		{
			float x = i->m_pLine->first.x;
			float y = i->m_pLine->first.y;
			float x2 = i->m_pLine->second.x;
			float y2 = i->m_pLine->second.y;

			sprintf( szBuffer, "( %f, %f ) - ( %f, %f )\n", x, y, x2, y2 );
			printFunc( szBuffer );
		}
		else
		{
			assert( false );
		}
	}
}

int			CRegionTrigger::BestSplitLine( const LINES & lines, float Epsilon )
{
	int maxCheck = (int)( lines.size() * 0.1f );
	if( maxCheck == 0 ) maxCheck = 1;

	int bestSplits = 100000;
	int bestIndex = -1;

	for( int i = 0; i < maxCheck; i++ )
	{
		SLineVector currLine( lines[i] );
		int currSplits = 0;

		for( int i2 = 0; i2 < (int)lines.size(); i2++ )
		{
			if( i == i2 ) continue;

			ePos res = currLine.TestLine( lines[i2], Epsilon );
			if( res == POS_SPLIT )
				currSplits++;
		}
		if( currSplits < bestSplits )
		{
			bestSplits = currSplits;
			bestIndex = i;
		}
	}
	return bestIndex;
}

#ifdef LOG_POLYGONTOLINE

void	PrintOneLine( ofstream & file, SVector2 pt1, SVector2 pt2 )
{
	file << pt1.x << ", " << pt1.y << " --- " << pt2.x << ", " << pt2.y << endl;
}

#endif

void		CRegionTrigger::RegionsToLines( const TRIGGERREGIONS & regions, LINES & line )
{
	//-------기록화일(log) 작성--------//
	#ifdef LOG_POLYGONTOLINE
		ofstream file( "log_lines.txt" );

		if( !file.is_open() )
			return;
	#endif
	//---------------------------------//

	for( unsigned i = 0; i < regions.size(); i++ )
	{
		const POLYGON & polygon = regions[i].second;

		if( polygon.size() >= 2 )
		{
			#ifdef LOG_POLYGONTOLINE
				PrintOneLine( file, polygon[0], polygon[1] );
			#endif

			line.push_back( LINE( polygon[0], polygon[1] ) );

			for( unsigned n = 2; n < polygon.size(); n++ )
			{
				#ifdef LOG_POLYGONTOLINE
					PrintOneLine( file, polygon[n-2], polygon[n] );	
				#endif

				line.push_back( LINE( polygon[n-2], polygon[n] ) );
			}

			#ifdef LOG_POLYGONTOLINE
				PrintOneLine( file, polygon[polygon.size()-2], polygon[polygon.size()-1] );	
			#endif

			line.push_back( LINE( polygon[polygon.size()-2], polygon[polygon.size()-1] ) );
		}
	}


/*
	for( int i = 0; i < regions.size(); i++ )
	{
		const POLYGON & polygon = regions[i].second;

		SVector2 thisPoint = polygon[ polygon.size() - 1 ];

		for( int i2 = 0; i2 < polygon.size(); i2++ )
		{
			SVector2 nextPoint = polygon[i2];

			line.push_back( LINE( thisPoint, nextPoint ) );

			#ifdef LOG_POLYGONTOLINE
				file << thisPoint.x << ", " << thisPoint.y << " --- " << nextPoint.x << ", " << nextPoint.y << endl;
			#endif

			thisPoint = nextPoint;			
		}
	}
*/
}

void		CRegionTrigger::AssignTrigger( const TRIGGERREGIONS & regions )
{
	//잎인 노드들의 인덱스를 모두 얻어온다.
/*	vector<unsigned> leafs;

	unsigned size = m_pLineTree->Size();
	for( unsigned i = 0; i < size; i++ )
	{
		if( m_pLineTree->IsValid( i ) && m_pLineTree->IsLeaf( i ) )
		{
			leafs.push_back( i );
		}
	}

	vector<SVector2> KeyPos;

	for( i = 0; i < leafs.size(); i++ )
	{
		unsigned parent = LINETREE::Parent( leafs[i] );
		if( m_pLineTree->IsValid( parent ) )
		{
			const SNode & node = m_pLineTree->GetData( parent );
			if( node.m_bNode == 2 )
			{
				//선의 중점을 구하고, 중점에서 대상 공간의 방향으로 약간 이동한 점을 구해서
				//그 점으로 트리거를 할당한다.
				SVector2 & pt1 = node.m_pLine->first;
				SVector2 & pt2 = node.m_pLine->second;
				SVector2 ptCenter( ( pt1.x + pt2.x ) / 2, ( pt1.y + pt2.y ) / 2 );

				SLineVector lineVec( *node.m_pLine );
				SVector2 dirVec( lineVec.a, lineVec.b );
				dirVec.Normalize();
				
				//i가 오른쪽 자식이었을 경우에
				if( LINETREE::LeftChild( parent ) != leafs[i] )
				{
					//-a, -b벡터
					KeyPos.push_back( SVector2( ptCenter.x - dirVec.x, ptCenter.y - dirVec.y ) );
				}
				else	//i가 왼쪽 자식이었을 경우에
				{
					//a, b벡터
					KeyPos.push_back( SVector2( ptCenter.x + dirVec.x, ptCenter.y + dirVec.y ) );
				}
			}
		}
	}
	
	for( i = 0; i < KeyPos.size(); i++ )
	{
		const TRIGGERLIST & trigger = GetTrigger( KeyPos[i], regions );
		AssignTrigger( KeyPos[i], trigger );
	}


*/	
	TRIGGERREGIONS tempRegions;
	SplitToTriangle( regions, tempRegions );
	TRIGGERREGIONS SplittedRegions = SplitTriggerRegions( tempRegions, 1 );

	for( unsigned i = 0; i < SplittedRegions.size(); i++ )
	{
		SVector2 ptCenter = CenterPoint( SplittedRegions[i].second );
		const TRIGGERLIST & triggers = SplittedRegions[i].first;

		unsigned index = GetLeaf( ptCenter );

		if( m_pLineTree->IsValid( index ) )
		{
			SNode & node = m_pLineTree->SetData( index );
			TRIGGERLIST & tList = *node.m_pTriggerID;
			
			for( unsigned i2 = 0; i2 < triggers.size(); i2++ )
			{
				if( tList.end() == find( tList.begin(), tList.end(), triggers[i2] ) )
					node.m_pTriggerID->push_back( triggers[i2] );
			}
		}
	}
}

const TRIGGERLIST &	CRegionTrigger::GetTrigger( SVector2 vecPos, const TRIGGERREGIONS & regions )
{
	static TRIGGERLIST triggerList;

	for( unsigned i = 0; i < regions.size(); i++ )
	{
		const POLYGON & poly = regions[i].second;

		SVector2 pt1;
		bool in = true;

		if( !poly.empty() )
			pt1 = poly[ poly.size() - 1 ];

		for( unsigned j = 0; j < poly.size(); j++ )
		{
			SVector2 pt2 = poly[j];

			SLineVector lineVec( LINE( pt1, pt2 ), false );
			
			//도형의 모든 모서리선들과 점을 비교해서 하나라도 밖에 있으면 점이 도형 밖에 있는 것임
			if( lineVec.TestPoint( vecPos, m_fEpsilon ) == POS_BACK )
			{
				in = false;
				break;
			}

			pt1 = pt2;
		}

		if( in )
		{
			//점이 도형 안에 있을 때 도형의 트리거 리스트를 모두 triggerList로 복사한다.
			for( unsigned k = 0; k < regions[i].first.size(); k++ )
			{
				triggerList.push_back( regions[i].first[k] );
			}
		}
	}
	
	return triggerList;
}

void			CRegionTrigger::AssignTrigger( SVector2 vecPos, const TRIGGERLIST & trigger )
{
	unsigned index = GetLeaf( vecPos );

	if( m_pLineTree->IsValid( index ) )
	{
		SNode & node = m_pLineTree->SetData( index );
		TRIGGERLIST & tList = *node.m_pTriggerID;

		for( unsigned i = 0; i < trigger.size(); i++ )
		{
			if( tList.end() == find( tList.begin(), tList.end(), trigger[i] ) )
				node.m_pTriggerID->push_back( trigger[i] );
		}
	}
}

SVector2	CRegionTrigger::CenterPoint( const POLYGON & polygon )
{
	double xTotal = 0.0, yTotal = 0.0;

	for( unsigned i = 0; i < polygon.size(); i++ )
	{
		xTotal += polygon[i].x;
		yTotal += polygon[i].y;
	}

	return SVector2( (float)(xTotal / polygon.size()), (float)(yTotal / polygon.size()) );
}

void		CRegionTrigger::MergeRegions( TRIGGERREGIONS & out, const TRIGGERREGIONS & in )
{
	for( int i = 0; i < (int)in.size(); i++ )
	{
		out.push_back( in[i] );
	}
}


void		CRegionTrigger::SplitToTriangle( const TRIGGERREGIONS & regions, TRIGGERREGIONS & out )
{
	for( unsigned i = 0; i < regions.size(); i++ )
	{
		const POLYGON & poly = regions[i].second;

		for( unsigned n = 0; n < poly.size() - 2; n++ )
		{
			out.push_back( TRIGGERREGION() );
				
			TRIGGERREGION & t = out.back();

			t.first = regions[i].first;

			if( ( n + 1 ) % 2 )		//n + 1이 홀수일 때
			{
				SVector2 temp = poly[n];
				t.second.push_back( temp );

				temp = poly[n+1];
				t.second.push_back( temp );

				temp = poly[n+2];
				t.second.push_back( temp );
			}
			else
			{
				SVector2 temp = poly[n+1];
				t.second.push_back( temp );

				temp = poly[n];
				t.second.push_back( temp );

				temp = poly[n+2];
				t.second.push_back( temp );
			}
		}
	}
}


TRIGGERREGIONS	CRegionTrigger::SplitTriggerRegions( const TRIGGERREGIONS & regions, unsigned splitIndex )
{
	if( regions.size() == 0 )
		return regions;

	if( !m_pLineTree->IsValid( splitIndex ) || m_pLineTree->IsLeaf( splitIndex ) )
		return regions;

	SNode & tempNode = m_pLineTree->SetData( splitIndex );
	//tempNode.ToLineVector();
	//const SLineVector & splitLine = *tempNode.m_pLineVector;
	SLineVector splitLine( *tempNode.m_pLine );

	TRIGGERREGIONS frontList, backList;

	for( int i = 0; i < (int)regions.size(); i++ )
	{
		ePos res = splitLine.TestPolygon( regions[i].second, m_fEpsilon );

		switch( res )
		{
		case POS_FRONT:
			frontList.push_back( regions[i] );
			break;

		case POS_BACK:
			backList.push_back( regions[i] );
			break;

		case POS_SPLIT:
			{
				POLYGON front, back;
				splitLine.SplitPolygon( regions[i].second, front, back, m_fEpsilon );
				frontList.push_back( TRIGGERREGION( regions[i].first, front ) );
				backList.push_back( TRIGGERREGION( regions[i].first, back ) );
				break;
			}
		case POS_COPLANAR:												//이 경우는 나올 수 없음.
			break;
		}
	}

	TRIGGERREGIONS result1 = SplitTriggerRegions( frontList, LINETREE::LeftChild( splitIndex ) );
	TRIGGERREGIONS result2 = SplitTriggerRegions( backList, LINETREE::RightChild( splitIndex ) );

	MergeRegions( result1, result2 );

	return result1;
}

void			CRegionTrigger::SaveNode( FILE * fp, const SNode & node )
{	
	if( node.m_pLine == 0 )
	{
		//빈 노드라는 표시로 3을 넣음
		byte b = 3;
		fwrite( &b, sizeof( byte ), 1, fp );
	}
	else
	{
		fwrite( &node.m_bNode, sizeof( byte ), 1, fp );

		if( node.m_bNode == 0 )
		{
			unsigned nTrigger = node.m_pTriggerID->size();

			fwrite( &nTrigger, sizeof( unsigned ), 1, fp );

			for( unsigned i = 0; i < nTrigger; i++ )
			{
				fwrite( &node.m_pTriggerID->at( i ), sizeof( int ), 1, fp );
			}
		}
		else if( node.m_bNode == 1 )
		{
			fwrite( &node.m_pLineVector->a, sizeof( float ), 1, fp );
			fwrite( &node.m_pLineVector->b, sizeof( float ), 1, fp );
			fwrite( &node.m_pLineVector->c, sizeof( float ), 1, fp );
		}
		else if( node.m_bNode == 2 )
		{
			fwrite( &node.m_pLine->first, sizeof( SVector2 ), 1, fp );
			fwrite( &node.m_pLine->second, sizeof( SVector2 ), 1, fp );
		}
		else
		{
			assert( false );
		}
	}


	//디버그를 위한 표식
	char temp = '#';
	fwrite( &temp, 1, 1, fp );
}

void			CRegionTrigger::LoadNode( FILE * fp, SNode & node )
{
	node.Destroy();

	byte bNode;

	int vectorsize = sizeof( LINE );

	fread( &bNode, sizeof( byte ), 1, fp );

	if( bNode == 0 )
	{
		unsigned nTrigger = 0;
		
		fread( &nTrigger, sizeof( unsigned ), 1, fp );

		TRIGGERLIST tlist;

		for( unsigned i = 0; i < nTrigger; i++ )
		{			
			int t;
			fread( &t, sizeof( int ), 1, fp );
			tlist.push_back( t );
		}

		node.Create( tlist );
	}
	else if( bNode == 1 )
	{
		float a, b, c;
		fread( &a, sizeof( float ), 1, fp );
		fread( &b, sizeof( float ), 1, fp );
		fread( &c, sizeof( float ), 1, fp );

		node.Create( a, b, c );
	}
	else if( bNode == 2 )
	{
		SVector2 a, b;
		fread( &a, sizeof( SVector2 ), 1, fp );
		fread( &b, sizeof( SVector2 ), 1, fp );

		node.Create( a, b );
	}
	else if( bNode == 3 )
	{
	}
	else
	{
		assert( false );
	}

	char c;
	fread( &c, 1, 1, fp );
	assert( c == '#' );
}

/////////////////////////////////////////////////////////////////////////////