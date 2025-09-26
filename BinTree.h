

#ifndef _BinTree_H_
#define _BinTree_H_

#pragma once

#include <assert.h>
#include <stdexcept>
#include <vector>

using namespace std;

#pragma warning(disable:4786)				//STL의 쓸데없는 Warning을 없애기 위한..

typedef unsigned char byte;

/////////////////////////////////////////////////////////////////////////////
//

int	Power( int x, unsigned y )
{
	int result = 1;
	for( unsigned i = 0; i < y; i++ )
	{
		result *= x;
	}
	return result;
}

//LOGxY에서 소수값을 떼어낸 값이 리턴된다.
int LogInt( unsigned x, unsigned y )
{
	int i = 0;
	while( y >= x )
	{
		i++;
		y /= x;
	}
	return i;
}



/////////////////////////////////////////////////////////////////////////////
//	이진 트리 - 1차원 배열에 저장을 하므로 0 ... n 까지 순서대로 순회를 하면
//				레벨별 순회(level-order-traversal)가 됨.
//				인덱스 0은 캐쉬로 사용

template < class T >
class CBinTree
{
public:
	typedef void (*SAVEFUNC)( FILE *, const T & );
	typedef void (*LOADFUNC)( FILE *, T & );

public:
	////////////////유틸리티 함수들/////////////////////
	//리턴값 0은 에러를 의미함.
	static unsigned		Root()							{ return 1; }
	static unsigned		Parent( unsigned index )		{ return index / 2; }
	static unsigned		LeftChild( unsigned index )		{ return index * 2;	}
	static unsigned		RightChild( unsigned index )	{ return index * 2 + 1; }
	static unsigned		LevelAtIndex( unsigned index )	{ return LogInt( x, index ); }
	static unsigned 	IndexAtLevel( unsigned nLevel )	{ return int( Power( 2, nLevel ) ); }
	////////////////////////////////////////////////////


protected:
	struct SNode
	{
		T		data;													//실제 데이터
		byte	isRed : 1;
		byte	isValid : 1;
		byte	isLeaf : 1;

	};

protected:
	typedef vector<SNode>	DATACONTAINER;

	DATACONTAINER	m_vecTreeData;


protected:
	void		LoadExtraInfo( SNode & node, FILE * fp )
	{
		unsigned char c = 0;

		fread( &c, sizeof( c ), 1, fp );

		node.isRed =	( c & 0x01 ) >> 0;
		node.isValid =	( c & 0x02 ) >> 1;
		node.isLeaf =	( c & 0x04 ) >> 2;
	}

	void		SaveExtraInfo( SNode & node, FILE * fp )
	{
		unsigned char c = 0;

		c = ( node.isRed << 0 ) | ( node.isValid << 1 ) | ( node.isLeaf << 2 );

		assert( ( ( c & 0x01 ) >> 0 ) == node.isRed );
		assert( ( ( c & 0x02 ) >> 1 ) == node.isValid );
		assert( ( ( c & 0x04 ) >> 2 ) == node.isLeaf );

		fwrite( &c, sizeof( c ), 1, fp );
	}


public:
	///////////////////인터페이스////////////////////
	CBinTree()
	{
	}

	~CBinTree()
	{
		Destroy();
	}

	void		Create()
	{
		Destroy();

		m_vecTreeData.push_back( SNode() );								//뿌리노드 추가

		for( unsigned i = 0; i < m_vecTreeData.size(); i++ )			//전부 무효(invalid) 노드로 세팅
		{
			m_vecTreeData[i].isValid = 0;
			m_vecTreeData[i].isLeaf = 1;
		}

		m_vecTreeData[0].isValid = 1;
	}

	void		Create( FILE * fp, LOADFUNC LoadData )
	{
		Destroy();

		if( LoadData == 0 )
			return;

		if( fp == 0 )
			return;

		unsigned size = 0;
		fread( &size, sizeof( unsigned ), 1, fp );

		m_vecTreeData.push_back( SNode() );

		for( unsigned i = 1; i < size; i++ )
		{
			m_vecTreeData.push_back( SNode() );
			assert( m_vecTreeData.size() == ( i + 1 ) );
			LoadData( fp, m_vecTreeData[i].data );
			LoadExtraInfo( m_vecTreeData[i], fp );
		}
	}

	void		Destroy()
	{
		m_vecTreeData.clear();
	}

	void		Save( FILE * fp, SAVEFUNC SaveData )
	{
		if( SaveData == 0 )
			return;

		if( fp == 0 )
			return;

		unsigned size = m_vecTreeData.size();
		fwrite( &size, sizeof( unsigned ), 1, fp );

		for( unsigned i = 1; i < size; i++ )
		{
			SaveData( fp, m_vecTreeData[i].data );
			SaveExtraInfo( m_vecTreeData[i], fp );
		}
	}

	//////////////////////////////////////////////////

	T &			SetData( unsigned index )
	{
		if( index >= m_vecTreeData.size() )
		{
			m_vecTreeData.resize( index + 1 );
		}

		m_vecTreeData[ index ].isValid = true;
		m_vecTreeData[ Parent( index ) ].isLeaf = false;

		return m_vecTreeData[ index ].data;
	}

	const T &	GetData( unsigned index )
	{
		if( index >= m_vecTreeData.size() )
			throw out_of_range( "index out of range(at CBinTree::GetData)" );

		return m_vecTreeData[ index ].data;
	}


	//////////////////////////////////////////////////

	bool			IsEmpty()
	{
		if( m_vecTreeData.empty() )
			return true;

		if( m_vecTreeData[1].isValid == false )
			return true;

		return false;
	}

	bool			IsValid( unsigned index )
	{
		if( index >= m_vecTreeData.size() )
			return false;

		return m_vecTreeData[ index ].isValid == 1;
	}

	bool			IsLeaf( unsigned index )
	{
		return m_vecTreeData[ index ].isLeaf == 1;
	}

	unsigned		Capacity()
	{
		return m_vecTreeData.capacity();
	}

	unsigned		Size()
	{
		return m_vecTreeData.size();
	}

	unsigned		Min( unsigned index )											//index의 자식 노드들 중 최소값인 것을 리턴
	{
		while( LeftChild( index ) < m_vecTreeData.size() && IsValid( LeftChild( index ) ) )
			index = LeftChild( index );

		return index;
	}

	unsigned		Max( unsigned index )											//index의 자식 노드들 중 최대값인 것을 리턴
	{
		while( RightChild( index ) < m_vecTreeData.size() && IsValid( RightChild( index ) ) )
			index = RightChild( index );

		return index;
	}



	//////////////////////////////////////////////////
	class iterator_base
	{
	protected:
		CBinTree *	m_pBinTree;
		unsigned	m_uiCurIndex;

	public:
		iterator_base( CBinTree * pBinTree, unsigned index )
			: m_pBinTree( pBinTree ), m_uiCurIndex( index )
		{
		}

		T &		operator*() const
		{
			return m_pBinTree->m_vecTreeData[m_uiCurIndex].data;
		}


		T *		operator->() const
		{
			return (&**this);
		}

		bool	operator==( iterator_base & rhs ) const
		{
			if( m_pBinTree == rhs.m_pBinTree && m_uiCurIndex == rhs.m_uiCurIndex )
				return true;
			else
				return false;
		}

		bool	operator!=( iterator_base & rhs ) const
		{
			return !operator==( rhs );
		}

		bool	IsValid() const
		{
			if( m_pBinTree != NULL && m_uiCurIndex != 0 )
				return true;
			else
				return false;
		}
		unsigned GetIndex() const
		{
			return m_uiCurIndex;
		}
	};

	//////////////////////////////////////////////////
	//전위순회 - 1 2 4 8 9 5 10 11 3 6 12 13 7 14 15
	class iterator_preorder : public iterator_base
	{
	protected:
		void	Inc()
		{
			//왼쪽이 유효한 동안 왼쪽으로 간다.
			//왼쪽이 무효하면 오른쪽으로 간다.
			//오른쪽도 무효하면 부모노드로 돌아간다.
			//부모 노드가 방문했던 노드이면 또다시 부모노드로 간다.

			if( m_pBinTree->IsValid( LeftChild( m_uiCurIndex ) ) )
				m_uiCurIndex = LeftChild( m_uiCurIndex );
			else if( m_pBinTree->IsValid( RightChild( m_uiCurIndex ) ) )
				m_uiCurIndex = RightChild( m_uiCurIndex );
			else
			{
				unsigned tempIndex;

				while( m_uiCurIndex == RightChild( tempIndex = Parent( m_uiCurIndex ) ) 
					|| !m_pBinTree->IsValid( RightChild( tempIndex ) ) )
				{					
					m_uiCurIndex = tempIndex;
					if( m_uiCurIndex == 0 )
						return;
				}

				m_uiCurIndex = RightChild( tempIndex );
			}
		}

		void	Dec()
		{
			//아직 구현 안했으므로 호출하면 바로 end가 되어서 끝나도록 해놨음.
			m_uiCurIndex = 0;
		}


	public:
		iterator_preorder( CBinTree * pBinTree, unsigned index )
			: iterator_base( pBinTree, index )
		{
		}

		iterator_preorder & operator++()
		{
			Inc();
			return (*this);
		}

		iterator_preorder	operator++(int)
		{
			iterator_preorder tmp = *this;
			Inc();
			return tmp;
		}

		iterator_preorder &	operator--()
		{
			Dec();
			return (*this);
		}

		iterator_preorder	operator--(int)
		{
			iterator_preorder tmp = *this;
			Dec();
			return tmp;
		}
	};

	//////////////////////////////////////////////////
	//중위순회 - 8 4 9 2 10 5 11 1 12 6 13 3 14 7 15
	class iterator_inorder
	{
	protected:
		void	Inc()
		{
			//Leftmost를 시작으로 한다.
			if( m_pBinTree->IsValid( RightChild( m_uiCurIndex ) ) )
				m_uiCurIndex = Min( RightChild( m_uiCurIndex ) );
			else
			{
				unsigned tempIndex;

				while( m_uiCurIndex == RightChild( tempIndex = Parent( m_uiCurIndex ) ) )
					m_uiCurIndex = tempIndex;

				m_uiCurIndex = tempIndex;
			}
		}

		void	Dec()
		{
			if( m_pBinTree->IsValid( LeftChild( m_uiCurIndex ) ) )
				m_uiCurIndex = Max( LeftChild( m_uiCurIndex ) );
			else
			{
				unsigned tempIndex;

				while( m_uiCurIndex == LeftChild( tempIndex = Parent( m_uiCurIndex ) ) )
					m_uiCurIndex = tempIndex;

				m_uiCurIndex = tempIndex;
			}
		}


	public:
		iterator_inorder( CBinTree * pBinTree, unsigned index )
			: iterator_base( pBinTree, index )
		{
		}

		iterator_inorder &	operator++()
		{
			Inc(); 
			return (*this);
		}

		iterator_inorder	operator++(int)
		{
			iterator_inorder tmp = *this;
			Inc();
			return tmp;
		}

		iterator_inorder &	operator--()
		{
			Dec();
			return (*this);
		}

		iterator_inorder	operator--(int)
		{
			iterator_inorder tmp = *this;
			Dec();
			return tmp;
		}
	};

	//////////////////////////////////////////////////
	//레벨별 순회 - 0~n까지 차례대로 순회하면 됨.
	class iterator_levelorder
	{
	public:
		iterator_levelorder( CBinTree * pBinTree, unsigned index )
			: iterator_base( pBinTree, index )
		{
		}

		iterator_levelorder &	operator++()
		{
			m_uiCurIndex++;
			return (*this );
		}

		iterator_levelorder		operator++(int)
		{
			iterator_levelorder tmp = *this;
			m_uiCurIndex++;
			return tmp;
		}

		iterator_levelorder	&	operator--()
		{
			m_uiCurIndex--;
			return (*this);
		}

		iterator_levelorder		operator--(int)
		{
			iterator_levelorder tmp = *this;
			m_uiCurIndex--;
			return tmp;
		}
	};


	iterator_preorder	Begin_preorder()			{ return iterator_preorder( this, 1 ); }
	iterator_levelorder	Begin_levelorder()			{ return iterator_levelorder( this, 1 ); }

	iterator_preorder	End_preorder()				{ return iterator_preorder( this, 0 ); }
	iterator_levelorder	End_levelorder()			{ return iterator_levelorder( this, m_TreeData.size() ); }

	friend class iterator_base;
};



#endif