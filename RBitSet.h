// BitSet.h: interface for the CBitSet class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __RBITSET_H__
#define __RBITSET_H__

#include <windows.h>

class RBitset {
protected:

	unsigned int *m_pBits;						// Bit Set ptr
	int m_iBitSize;								// Bit Set Size

public:
	RBitset() {
		m_pBits = NULL;
		m_iBitSize = 0;
	}
	~RBitset() {
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


#endif 