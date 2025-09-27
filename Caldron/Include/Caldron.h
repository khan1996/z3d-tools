/* ********************************************************************* 

  * Caldron Engine 공용헤더파일 
  
	* 파일	: Caldron.h
	* 기능	: Caldron Engine 을 이용하는곳에서 공통적으로 사용하게되는 헤더파일
	* 작성일: 2003.10.23 
	* 작성자: yundi ( 2003.10.23) 
	* 수정자: 
	
********************************************************************** */ 

#pragma once
#pragma warning(disable:4786)
#pragma warning(disable:4251)
#pragma warning(disable:4503)

#include <algorithm>
//#include <d3d9.h>
#include <d3dx8.h>
#include <tchar.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <list>
#include <math.h>
#include <process.h>
#include <mmsystem.h>
//#include <windows.h>
#include <stdio.h>

namespace Caldron
{
	// epsilon 상수
	extern const float MIN_EPSILON;
	extern const float MAX_EPSILON;


	// 상수 선언
	const float FLOAT_RAD = 0.0174532925f;	// radian 값
	extern const float FLOAT_PI;	// pi
	extern const float FLOAT_2PI;
	extern const float FLOAT_PI2;

	extern const int   MAX_STRINGSIZE;
	const int   MAX_RELOADING = 3;	// Resource Loader 에서 Object 로딩 실패시 최대 Reloading 시도 횟수

	#define FLOAT_TO_INT(fValue) (*(int *)&(fValue))
	#define IS_SIGNBIT(fValue) (FLOAT_TO_INT(fValue) & 0x80000000)
	#define FRAND (((rand()%10000)-5000)/5000.0f)

	// template function 정의
	template<class _T>
	inline void SafeDelete( _T ptr )
	{
		if( NULL != ptr )
		{
			delete (ptr);
			ptr = NULL;
		}
	}

	template<class _T>
	inline void SafeDeleteA( _T ptr )
	{
		if( NULL != ptr )
		{
			delete[] (ptr);
			ptr = NULL;
		}
	}

	

	template<class _T>
	inline void SafeRelease( _T ptr )
	{
		if( NULL != ptr )
		{
			ptr->Release();
			ptr = NULL;
		}
	}


	inline bool Succeeded( HRESULT hr )
	{
		return (hr >= 0);
	}

	inline bool Failed( HRESULT hr )
	{
		return (hr < 0);
	}

	inline bool IsZero(float fValue) 
	{
		if((fValue > -(MIN_EPSILON)) && (fValue < MIN_EPSILON))
			return true;
		return false;
	}
	inline float Def2Rad( float fDeg )
	{
		return fDeg*FLOAT_PI/180;
	}
	
	// CProfileMgr 에서 사용
	inline void ProfileGetTicks(_int64 * ticks)
	{
		__asm
		{
			push edx;
			push ecx;
			mov ecx,ticks;
			_emit 0Fh
			_emit 31h
			mov [ecx],eax;
			mov [ecx+4],edx;
			pop ecx;
			pop edx;
		}
	}

	inline float ProfileGetTickRate(void)
	{
		static float _CPUFrequency = -1.0f;
		
		if (_CPUFrequency == -1.0f) {
			__int64 curr_rate = 0;
			::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
			_CPUFrequency = (float)curr_rate;
		} 
		
		return _CPUFrequency;
	}
			
	

	//
	
/* *********************************************************************

  * CBitset
  * 파일 : Bitset.h
  * 기능 : Caldron Engine내 Bitset Class
  * 작성일 : 2003.10.24
  * History : wizardbug ( 2003.10.24)
  
********************************************************************** */	

	class CBitset {
	protected:

		unsigned int	*m_pBits;						// Bit Set ptr
		int				m_iBitSize;								// Bit Set Size

	public:
		CBitset() {
			m_pBits = NULL;
			m_iBitSize = 0;
		}
		~CBitset() {
			if(m_pBits)
			{
				delete[] m_pBits;
				m_pBits = NULL;
			}
		}
		void ResizeBits(int iNewCount) {
			
			m_iBitSize = iNewCount / 32 + 1;		// Get Inteager Size
			if(m_pBits) {
				delete[] m_pBits;
				m_pBits = NULL;
			}
			m_pBits = new unsigned int[m_iBitSize];	// Allocate Bits
			ClearBits();

		}
		void ClearBits() {							// Clear All Bits
			memset(m_pBits,0,sizeof(unsigned int) * m_iBitSize);

		}
		void ClearOneBit(int index) {

			m_pBits[ index >> 5 ] &= ~( 1 << ( index & 31 ) );
		}
		void SetBit(int index) {					// Set Bit		
			m_pBits[ index >> 5 ] |= ( 1 << ( index & 31 ) );

		}
		bool GetBit(int index) {					// Return Bit's Setting : return true or false

			return (m_pBits[ index >> 5 ] & ( 1 << (index & 31 ) )) ? true : false;

		}
	};
/********************************************************************** */
	template <class _T> 
	class CUpdateCounter 
	{ 
	public : 
		CUpdateCounter(int n, int range=1) 
		{ 
			m_cnt = new _T [n]; 
			m_cnt_num = n; 

			m_mask = range - 1; 
			m_current = 0; 

			m_step = m_cnt_num / ((1<<(8*sizeof(_T))) / range - 1) + 1; 
			m_clear = 0; 

			memset(m_cnt, 0, sizeof(_T) * m_cnt_num); 

			Clear(); 
		} 

		~CUpdateCounter() 
		{ 
			delete [] m_cnt; 
		} 

		void Clear() 
		{ 
			ClearBlock(); 

			m_current += (m_mask+1); 
		} 

		void Update(int cnt, int v=0) { 
			m_cnt[cnt] = m_current + v; 
		} 

		int GetValue(int cnt) { 
			return m_cnt[cnt] - m_current; 
		} 

		bool IsUpdated(int cnt) { 
			return (_T)(m_cnt[cnt] - m_current) <= m_mask ? true : false; } 

	private : 
		void ClearBlock() 
		{ 
			int len = m_cnt_num - m_clear > m_step ? m_step : m_cnt_num - m_clear, i; 

			if (len > 0) 
			{ 
				m_cnt[m_clear] = m_current; 

				for(i=1; i<len; i+=i) 
					memcpy(&m_cnt[m_clear+i], &m_cnt[m_clear], (len-i > i ? i : len-i) * sizeof(*m_cnt)); 
			} 

			m_clear = m_clear + len >= m_cnt_num ? 0 : m_clear + len; 
		} 

		_T * m_cnt, m_current, m_mask; 
		int m_cnt_num, m_step, m_clear; 
	} ; 



}