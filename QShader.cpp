// QShader.cpp: implementation of the CQShader class.
//
//////////////////////////////////////////////////////////////////////


#include "QShader.h"
#include "BaseDataDefine.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQShader::CQShader()
{
	m_pTexture=NULL;
}

CQShader::~CQShader()
{

}

void CQShader::LoadTexture()
{
	m_pTexture=new CTexture();
	m_pTexture->SetPath(BSPTEXTUREPATH);	
	int cLens=strlen(m_strTextureName);
	if( m_strTextureName[cLens-4]=='.'&&
		m_strTextureName[cLens-3]=='d'&&
		m_strTextureName[cLens-2]=='d'&&
		m_strTextureName[cLens-1]=='s')
	{
		//char *pDest=strrchr(m_strTextureName,'/');
		//pDest++;   	
		m_pTexture->Load(m_strTextureName);
		return;
	}
	strcat(m_strTextureName,".dds");
	//char *pDest=strrchr(m_strTextureName,'/');
	//pDest++;   	
	m_pTexture->Load(m_strTextureName);
}
