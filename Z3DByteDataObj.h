#pragma once

#include "CrossMHeader.h"

namespace CROSSM {

	class CZ3DByteDataObj
	{
	public:
		CZ3DByteDataObj(void);
		~CZ3DByteDataObj(void);
		
		bool	LoadByte(const char *strFileName,long lOffset = 0 );
		bool	SaveByte(const char *strFileName,long lOffset = 0 );

		long	GetByteSize() { return m_lSize; }
		
		long	GetReadPos() { return m_lReadPos;}
		void	SetReadPos(long lPos) { m_lReadPos = lPos;}
		
		unsigned char *GetReadPtr();
		
		long	Read(void *ptr,size_t UnitSize,int iNum);
		long	Write(void *ptr,size_t UnitSize,int iNum);
	
	protected:

		unsigned char	*m_pBytes;
		long		   	m_lSize;
		long			m_lReadPos;


	};
}
