
#include <stdio.h>
#include "./ByteDataObj.h"

namespace CROSSM {

const long  UINT_SIZE = (long)sizeof(unsigned char);

CByteDataObj::CByteDataObj(void)
{
	m_pBytes = NULL;
	m_lSize = 0;
	m_lReadPos = 0;

}

CByteDataObj::~CByteDataObj(void)
{
	if(m_pBytes) 
	{
		delete[] m_pBytes;
		m_pBytes = NULL;
		m_lSize = 0;
		m_lReadPos = 0;

	}

}
unsigned char *CByteDataObj::GetReadPtr() 
{
	if((m_lReadPos >= m_lSize) || (m_pBytes == NULL)) 
	{
		return NULL;
	}
	
	return &(m_pBytes[m_lReadPos]);

}
bool CByteDataObj::SaveByte(const char *strFileName,long lOffset )
{
	FILE *fp = fopen(strFileName,"wb+");
	if(!fp)
	{
		//CLogMgr::_LogError("CByteDataObj::SaveByte fopen Failed.");
		return false;
	}
	if(lOffset > 0)
	{
		fseek(fp,lOffset,SEEK_SET);
	}
	if(fwrite((void *)m_pBytes,sizeof(unsigned char) * m_lSize,1,fp) != m_lSize)
	{
		//CLogMgr::_LogError("CByteDataObj::SaveByte fwrite Failed.");
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;


}
bool CByteDataObj::LoadByte(const char *strFileName,long lOffset) 
{
	FILE *fp = fopen(strFileName,"rb");
	long lFileSize = 0;

	if(fp == NULL) 
	{
		//CLogMgr::_LogError("CByteDataObj::LoadByte() : %s file not exist\n",strFileName);
		return false;
		
	}
	fseek(fp,0,SEEK_END);
	m_lSize = ftell(fp);
	if(m_lSize <= 0)
	{
		//CLogMgr::_LogError("CByteDataObj::LoadByte() : %s file size Wrong\n",strFileName);
		fclose(fp);
		return false;
	}
	// Offset 적용
	if(m_lSize > lOffset) 
		m_lSize -= lOffset;
	else 
	{
		//CLogMgr::_LogError("CByteDataObj::LoadByte() : %s File Offset지정이 한계를 넘었습니다.\n",strFileName);
		fclose(fp);
		return false;
	
	}

	m_pBytes = new unsigned char[m_lSize];
	if(m_pBytes == NULL) 
	{
		//CLogMgr::_LogError("CByteDataObj::LoadByte() : File Load Buffer's New Fail.\n");
		fclose(fp);
		return false;
	
	}
	fseek(fp,lOffset,SEEK_SET);
	if(fread((unsigned char *)m_pBytes,sizeof(unsigned char),m_lSize,fp) != m_lSize)
	{
		//CLogMgr::_LogError("CByteDataObj::LoadByte() : Load File Fail.\n");
		fclose(fp);
		return false;
	}
	fclose(fp);			
	return true;
	
}
long CByteDataObj::Write(void *ptr,size_t UnitSize,int iNum)
{
	if(ptr == NULL)
		return -1;
	return 0;

}
long CByteDataObj::Read(void *ptr,size_t UnitSize,int iNum)
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
};
