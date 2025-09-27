// X3DEffectBase.cpp: implementation of the CX3DEffectBase class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffectBase.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectBase::CX3DEffectBase()
{
	m_QuatSet = false;
	m_lpTexture = NULL;
	m_index=-1;
	m_Scale[0] = m_Scale[1] = m_Scale[2] = 1.0f;
	strcpy(m_strTextureFile, "");
	m_bWorldEffect = false;
	m_Mine = true;
	m_StopParticleFrame = -1;
	m_bLight = false;
	m_bClearBuffer = true;
}

CX3DEffectBase::~CX3DEffectBase()
{
	if(m_lpTexture) { delete m_lpTexture; m_lpTexture = NULL; }
}
void CX3DEffectBase::SetStopParticle(int t) {
	m_StopParticleFrame = t;
}
int CX3DEffectBase::GetStopParticle() {
	return m_StopParticleFrame;
}
void CX3DEffectBase::LoadTexture(const char *strFilename)
{
	if(m_lpTexture) { delete m_lpTexture; m_lpTexture = NULL; }
	m_lpTexture = new CTexture();
	m_lpTexture->Init(m_lpD3DDevice);

	m_lpTexture->SetPath(EFFECTTEXTUREPATH);
	m_lpTexture->Load((char *)strFilename);
	strcpy(m_strTextureFile, strFilename);
}

void CX3DEffectBase::SetGemClearBuffer(bool b)
{
	m_bClearBuffer = b;

}
