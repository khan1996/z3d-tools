// FullSceneShader.cpp: implementation of the CFullSceneShader class.
//
//////////////////////////////////////////////////////////////////////

#include "FullSceneShader.h"
#include "FullScenePShader.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFullSceneShader::CFullSceneShader(char *strFileName)
{

	m_pPixelShader = new CFullScenePShader(strFileName);
	
}

CFullSceneShader::~CFullSceneShader()
{
	if(m_pPixelShader)
	{
		delete m_pPixelShader;
		m_pPixelShader = NULL;

	}
}
void CFullSceneShader::Apply()
{
	if(m_pPixelShader)
		m_pPixelShader->Apply();

}
void CFullSceneShader::Apply(int i)
{
	if(m_pPixelShader)
		m_pPixelShader->Apply(i);

}
void CFullSceneShader::UnApply()
{
	CSceneManager::GetDevice()->SetPixelShader(0);
}
