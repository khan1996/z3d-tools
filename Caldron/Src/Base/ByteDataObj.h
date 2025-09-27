// ByteDataObj.h: interface for the CByteDataObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYTEDATAOBJ_H__A71C88C6_A81A_4696_9922_802A8B1C119A__INCLUDED_)
#define AFX_BYTEDATAOBJ_H__A71C88C6_A81A_4696_9922_802A8B1C119A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



namespace Caldron {
	namespace Base {

		const long  UINT_SIZE = (long)sizeof(unsigned char);

/* *********************************************************************  
  
  * CByteDataObj  
  
  * 파일 : ByteDataObj.h  
  * 기능 : Caldron Engine내 CResourceMgr 에서 읽어들이는 리소스들이 저장 되는 기본 형태.
           CLoadedObj를 상속한 각 리소스 들의 obj 들의 Load 루틴에서 각 리소스에 알맞은 형태로 변환 로드 된다.

  * 작성일 : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  


class CByteDataObj  
{
public:
	CByteDataObj();
	virtual ~CByteDataObj();
	
	bool	LoadByte(char *strFileName,long lOffset = 0 );
	long	GetByteSize() { return m_lSize; }
	long	GetReadPos() { return m_lReadPos;}
	void	SetReadPos(long lPos) { m_lReadPos = lPos;}
	unsigned char *GetReadPtr();
	long	Read(void *ptr,size_t UnitSize,int iNum);

	unsigned char	*m_pBytes;
	long		   	m_lSize;
	long			m_lReadPos;

};

}}

#endif // !defined(AFX_BYTEDATAOBJ_H__A71C88C6_A81A_4696_9922_802A8B1C119A__INCLUDED_)
