
/* *********************************************************************  
  
  * CByteDataObj  
  
  * ���� : ByteDataObj.h  
  * ��� : Caldron Engine�� CResourceMgr ���� �о���̴� ���ҽ����� ���� �Ǵ� �⺻ ����.
           CLoadedObj�� ����� �� ���ҽ� ���� obj ���� Load ��ƾ���� �� ���ҽ��� �˸��� ���·� ��ȯ �ε� �ȴ�.

  * �ۼ��� : 2004.01.06  
  * history : 
      wizardbug ( 2004.01.06)  
  
********************************************************************** */  


#include "CaldronByteDataObj.h"
#include <windows.h>
#include <stdio.h>
#include "GMMemory.h"

namespace Caldron {
	namespace Base {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaldronByteDataObj::CCaldronByteDataObj()
{
	m_pBytes = NULL;
	m_lSize = 0;
	m_lReadPos = 0;

}

CCaldronByteDataObj::~CCaldronByteDataObj()
{
	if(m_pBytes) 
	{
		delete[] m_pBytes;
		m_pBytes = NULL;
		m_lSize = 0;
		m_lReadPos = 0;

	}
}
unsigned char *CCaldronByteDataObj::GetReadPtr() 
{
	if((m_lReadPos >= m_lSize) || (m_pBytes == NULL)) 
	{
		return NULL;
	}
	
	return &(m_pBytes[m_lReadPos]);

}
bool CCaldronByteDataObj::LoadByte(char *strFileName,long lOffset) 
{
	FILE *fp = fopen(strFileName,"rb");
	long lFileSize = 0;

	if(fp == NULL) 
	{
		return false;
		
	}
	fseek(fp,0,SEEK_END);
	m_lSize = ftell(fp);
	if(m_lSize <= 0)
	{
	
		fclose(fp);
		return false;
	}
	// Offset ����
	if(m_lSize > lOffset) 
		m_lSize -= lOffset;
	else 
	{
	
		fclose(fp);
		return false;
	
	}

	m_pBytes = new unsigned char[m_lSize];
	if(m_pBytes == NULL) 
	{
	
		fclose(fp);
		return false;
	
	}
	fseek(fp,lOffset,SEEK_SET);
	if(fread((unsigned char *)m_pBytes,sizeof(unsigned char),m_lSize,fp) != m_lSize)
	{
	
		fclose(fp);
		return false;
	}
	fclose(fp);			
	return true;
	
}

long CCaldronByteDataObj::Read(void *ptr,size_t UnitSize,int iNum)
{
	if(ptr == NULL)
		return -1;
	long lCurrentReaded = ((long)(UnitSize) * iNum) / UINT_SIZE;
	if(m_lSize >= lCurrentReaded + m_lReadPos)
	{
		memcpy(ptr,&(m_pBytes[m_lReadPos]), (size_t)(UINT_SIZE * lCurrentReaded));
		m_lReadPos += lCurrentReaded;
	}
	else 
	{
		return -1;

	}
	return m_lReadPos;
	
}
}}