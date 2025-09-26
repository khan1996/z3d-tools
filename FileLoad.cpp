// FileLoad.cpp: implementation of the CFileLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "FileLoad.h"
#include <windows.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLoad::CFileLoad()
{

}

CFileLoad::~CFileLoad()
{
	if ( m_Data )
	{
		delete[] m_Data;
		m_Data = NULL;
	}
}

void CFileLoad::Load(char *strFilename)
{
	strcpy(m_strFilename,strFilename);
	
	m_Data=0;
	m_Len=0;

	FILE *fp=fopen(strFilename,"rb");
	
	if(fp)
	{
		fseek(fp,0L,SEEK_END);
		m_Len=ftell(fp);
		if(m_Len)
		{
			fseek(fp,0L,SEEK_SET);
			m_Data=(void*)new unsigned char[m_Len];
			if(m_Data)
			{
				int r=fread(m_Data,m_Len,1,fp);
				if(!r)
				{
					if(m_Data) {
						delete[] m_Data;
						m_Data=NULL;
					}
				}
			}
		}
		fclose(fp);
	}
	else
	{		
		MessageBox(NULL,strFilename,0,0);
	}
	m_ReadLoc=(char*)m_Data;
	m_ReadLen=m_Len;  
}

void CFileLoad::GetData(void *pData, size_t size)
{
	memcpy(pData,m_ReadLoc,size);
	m_ReadLoc+=size;
}
