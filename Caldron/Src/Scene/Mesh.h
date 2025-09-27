#pragma once

#pragma warning( disable : 4786 )
#include "../../include/Caldron.h"


namespace Caldron
{
namespace Scene
{

	class CBaseMesh
	{
	public:
		CBaseMesh()
		{
			m_nLockCount = 0;
		}
		
		virtual ~CBaseMesh()
		{
			// m_nLockCount 가 0이 아닐 경우 경고 메시지를 띄워줄 수 있다.
		}

		void Lock()
		{
			// thread safe code 필요할지도?
			++m_nLockCount;
		}
		
		void Unlock()
		{
			// thread safe code 필요할지도?
			if( m_nLockCount > 0 )
			{
				// 음수 lock count 에 대한 경고 메시지 출력할수 있을듯
				--m_nLockCount;
			}
		}

		bool IsLocked()
		{
			return (m_nLockCount>0);
		}

		// VB 로 vertex 내용 upload 시 사용
		virtual void UploadVertexData( void* p ) = 0;
		// IB 로 index 내용 upload 시 사용
		virtual void UploadIndexData( void* p ) = 0;

		// 
		virtual int GetVertexCount() = 0;
		virtual int GetIndexCount() = 0;


		virtual int GetVertexSize() = 0;
		virtual DWORD GetFVF() = 0;


		bool IsIndexed()
		{
			return m_bIndexed;
		}


	protected:
		int m_nLockCount;
		bool m_bIndexed;
	};


	template <class _T>
	class CMesh : public CBaseMesh
	{
	public:
		CMesh()
		{
		}

		~CMesh()
		{
		}

		// VB 로 vertex 내용 upload 시 사용
		void UploadVertexData( void* p )
		{
			if( 0 == m_vecVertexData.size() )
			{
				return;
			}

			MoveMemory( p, &(m_vecVertexData[0]), sizeof(_T) * m_vecVertexData.size() );
		}

		// IB 로 index 내용 upload 시 사용
		void UploadIndexData( void* p )
		{
			if( 0 == m_vecIndexData.size() )
			{
				return;
			}

			MoveMemory( p, &(m_vecIndexData[0]), sizeof(_T) * m_vecIndexData.size() );
		}


		//
		int GetVertexCount()
		{
			return m_vecVertexData.size();
		}
		//
		int GetIndexCount()
		{
			return m_vecIndexData.size();
		}


		int GetVertexSize()
		{
			return sizeof( _T );
		}

		DWORD GetFVF()
		{
			return _T::GetFVF();
		}



		// vertex 갯수 설정(공간확보)
		void SetVertexCount( int n )
		{
			m_vecVertexData.resize( n );
		}

		// 해당 index 갯수 설정(공간 확보)
		void SetIndexCount( int n )
		{
			m_vecIndexData.resize( n );

			if( 0 == n )
			{
				m_bIndexed = false;
			}
			else
			{
				m_bIndexed = true;
			}
		}


		//
		_T* GetVertexWritePtr()
		{
			if( IsLocked() )
			{
				return NULL;
			}

			if( 0 == m_vecVertexData.size() )
			{
				return NULL;
			}

			return &(m_vecVertexData[0]);
		}

		WORD* GetIndexWritePtr()
		{
			if( IsLocked() )
			{
				return NULL;
			}

			if( 0 == m_vecIndexData.size() )
			{
				return NULL;
			}

			return &(m_vecIndexData[0]);
		}


	private:
		std::vector< _T > m_vecVertexData;
		std::vector< WORD > m_vecIndexData;
	};


}
}
