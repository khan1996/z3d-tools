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
			// m_nLockCount �� 0�� �ƴ� ��� ��� �޽����� ����� �� �ִ�.
		}

		void Lock()
		{
			// thread safe code �ʿ�������?
			++m_nLockCount;
		}
		
		void Unlock()
		{
			// thread safe code �ʿ�������?
			if( m_nLockCount > 0 )
			{
				// ���� lock count �� ���� ��� �޽��� ����Ҽ� ������
				--m_nLockCount;
			}
		}

		bool IsLocked()
		{
			return (m_nLockCount>0);
		}

		// VB �� vertex ���� upload �� ���
		virtual void UploadVertexData( void* p ) = 0;
		// IB �� index ���� upload �� ���
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

		// VB �� vertex ���� upload �� ���
		void UploadVertexData( void* p )
		{
			if( 0 == m_vecVertexData.size() )
			{
				return;
			}

			MoveMemory( p, &(m_vecVertexData[0]), sizeof(_T) * m_vecVertexData.size() );
		}

		// IB �� index ���� upload �� ���
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



		// vertex ���� ����(����Ȯ��)
		void SetVertexCount( int n )
		{
			m_vecVertexData.resize( n );
		}

		// �ش� index ���� ����(���� Ȯ��)
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
