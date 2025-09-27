// X3DEffect.cpp: implementation of the CX3DEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectParticle.h"
#include "X3DEffectBillboard.h"
#include "X3DEffectCylinder.h"
#include "X3DEffectPlane.h"
#include "X3DEffectSphere.h"
#include "X3DEffectMesh.h"
#include "EffDebugLog.h"
#include "GMMemory.h"

//#define __EFF_WCREATOR__

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffect::CX3DEffect()
{
	m_dwMinFrame = 0.0f;
	m_fFrame = 0.0f;
	m_fBeforeFrame = 0.0f;
	m_bImportCenter = FALSE;
	m_lpCenter = NULL;
	m_bImportAxis = FALSE;
	m_lpAxis = NULL;
	m_dwKindMove = 0;
	m_dwCount = 0;
	m_SectorX = -1;
	m_SectorY = -1;
	map_effect = false;
	m_StartEffect = false;
	m_dwFrameTick = 33;
	m_MoveState = false;
	m_Mine = true;
	m_GemScale[0]  = m_GemScale[1] = m_GemScale[2] = 1.0f;
	m_bLight = false;
	
}

CX3DEffect::~CX3DEffect()
{
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		delete pEffect;
		pEffect = NULL;
	}

	m_lstEffect.clear();	

	if(!m_bImportCenter) if(m_lpCenter) { delete m_lpCenter; m_lpCenter = NULL; }
	if(m_lpAxis) { delete m_lpAxis; m_lpAxis = NULL; }
}
void CX3DEffect::SetScale(float x,float y,float z) {

	m_GemScale[0] = x;m_GemScale[1] = y;m_GemScale[2] = z;
	if(m_lstEffect.size() > 0) {
		list<CX3DEffectBase *>::iterator it;
		CX3DEffectBase *pEffect;

		for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
		{
			pEffect = (CX3DEffectBase *)(*it);
			if(pEffect != NULL) 
				pEffect->SetEffectScale(x,y,z);
		}
	}
}
unsigned long CX3DEffect::AddEffect(CX3DEffectBase *pNewEffect)
{
	pNewEffect->SetLocalDevice(m_lpD3DDevice);
	pNewEffect->SetLocalView(m_lpViewMatrix);
	pNewEffect->SetLocalEffect((void *)this);
	pNewEffect->SetLocalCenter(m_lpCenter);
	pNewEffect->SetLocalAxis(m_lpAxis);

	m_lstEffect.push_back(pNewEffect);

	return m_lstEffect.size() - 1;
}

BOOL CX3DEffect::Interpolation(unsigned long dwTick, float fFrame)
{
	
	if((m_fFrame > m_dwMaxFrame) && !m_bLoop)
		return FALSE;

	switch(m_dwKindMove)
	{
		case 1:
			if(m_dwCount == 30)
				return FALSE;
			m_lpCenter->x += m_vecPower.x;
			m_lpCenter->y += m_vecPower.y;
			m_lpCenter->z += m_vecPower.z;
			m_dwCount++;
			break;

		case 2:
			break;
	}
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	if(-1.0f == fFrame)
	{
		for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
		{
			pEffect = (CX3DEffectBase *)(*it);
			
			if(!pEffect->Interpolation(m_fFrame))
				return FALSE;
		}
		m_fBeforeFrame = m_fFrame;
		m_fFrame += ((dwTick / m_dwFrameTick) * m_fIncFrame);
		if(m_bLoop && (m_fFrame >= m_dwMaxFrame)) {
			m_fFrame -= m_dwMaxFrame; 
			m_fFrame += m_dwMinFrame;
		}
	} else
	{
		for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
		{
			pEffect = (CX3DEffectBase *)(*it);
			
			if(!pEffect->Interpolation(fFrame))
				return FALSE;
		}
		m_fBeforeFrame = m_fFrame;

		m_fFrame = fFrame;
	}
	return TRUE;
}

void CX3DEffect::Render(void)
{
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	if(m_StartEffect) {
		for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
		{
			pEffect = (CX3DEffectBase *)(*it);
			pEffect->Render();
		}
	}
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
}

CX3DEffectBase *CX3DEffect::GetEffect(unsigned long dwEffectNumber)
{
	list<CX3DEffectBase *>::iterator it;
	unsigned long dwEffectCount = 0;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		if(dwEffectCount == dwEffectNumber) { return (CX3DEffectBase *)(*it); }
		dwEffectCount++;
	}

	return NULL;
}

BOOL CX3DEffect::DeleteEffect(unsigned long dwEffectNumber)
{
	list<CX3DEffectBase *>::iterator it;
	unsigned long dwEffectCount = 0;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		if(dwEffectCount == dwEffectNumber)
		{
			pEffect = (CX3DEffectBase *)(*it);
			m_lstEffect.erase(it);
			delete pEffect;
			pEffect = NULL;
			return TRUE;
		}
		dwEffectCount++;
	}

	return FALSE;
}
void CX3DEffect::SetAxis(quaternion *pAxis) { 
	m_bImportAxis = TRUE; 
	if(!m_lpAxis) m_lpAxis = new quaternion;

	m_lpAxis->x = pAxis->x;
	m_lpAxis->y = pAxis->y;
	m_lpAxis->z = pAxis->z;
	m_lpAxis->w = pAxis->w;


	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetAxis(*pAxis);
	}
}
void CX3DEffect::SetAxis(float fAxisX, float fAxisY, float fAxisZ){

	m_bImportAxis = TRUE;
	
	if(!m_lpAxis) m_lpAxis = new quaternion;
	m_lpAxis->Identity();
	m_lpAxis->Pitch(FLOAT_DEG(fAxisX - (((unsigned long)(fAxisX / 360)) * 360.0f)));
	m_lpAxis->Yaw(FLOAT_DEG(fAxisY - (((unsigned long)(fAxisY / 360)) * 360.0f)));
	m_lpAxis->Roll(FLOAT_DEG(fAxisZ - (((unsigned long)(fAxisZ / 360)) * 360.0f)));
	
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetAxis(*m_lpAxis);
	}
}
		

void CX3DEffect::SetEffectSetting(void)
{
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetLocalDevice(m_lpD3DDevice);
		pEffect->SetLocalView(m_lpViewMatrix);
	}
}
void CX3DEffect::SetMine(bool b) { 
	m_Mine = b;
	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetMine(m_Mine);
	}
}
void CX3DEffect::Load(const char *strFilePath, const char *strFileName,bool bLoad)
{
	FILE *fp;
	char strFile[MAX_PATH];
	strcpy(strFile, strFilePath);
	strcat(strFile, strFileName);
	fp = fopen(strFile, "rb");
	if(!fp) {
#ifdef __EFF_WCREATOR__
		WriteDebug(strFile);								// File Converting용 Log File Write 루틴
#endif
		return;
	}

	CX3DEffectBase *lpEffect;
	unsigned short Size, Kind;
	unsigned long i, dwStartFrame, dwEndFrame;
	unsigned char len;
	char strTextureFile[MAX_PATH], strTemp[MAX_PATH];

	fread(&m_dwMaxFrame, 4, 1, fp);
	fread(&m_dwFrameTick, 4, 1, fp);
	fread(&m_fIncFrame, 4, 1, fp);
	fread(&Size, 2, 1, fp);
	for(i = 0; i < Size; i++)
	{
		bool bParticle = false;

		fread(&Kind, 2, 1, fp);

		switch(Kind)
		{
			case EFFECT_PARTICLE:
				lpEffect = new CX3DEffectParticle;
				bParticle = true;
				break;

			case EFFECT_BILLBOARD:
				lpEffect = new CX3DEffectBillboard;
				break;

			case EFFECT_CYLINDER:
				lpEffect = new CX3DEffectCylinder;
				break;

			case EFFECT_PLANE:
				lpEffect = new CX3DEffectPlane;
				break;

			case EFFECT_SPHERE:
				lpEffect = new CX3DEffectSphere;
				break;

			case EFFECT_MESH:
				lpEffect = new CX3DEffectMesh;
				break;

			default:
				MessageBox(NULL, "에러", "에러", MB_OK);
				fclose(fp);
				return;
		}

		fread(&len, 1, 1, fp);
		if(len)
		{
			char *t_ptr =  NULL;
			fread(strTemp, len, 1, fp);
			t_ptr = strrchr(strTemp,'\\');
			if(t_ptr != NULL) {
				t_ptr++;
				strcpy(strTextureFile,t_ptr);
			}
			else {
				strcpy(strTextureFile,strTemp);
			}
			//strcpy(strTextureFile, strFilePath);
			//strcat(strTextureFile, strTemp);

		} else
		{
			strcpy(strTextureFile, "");
		}

		AddEffect(lpEffect);
		if(bLoad)
			if(strTextureFile) lpEffect->LoadTexture(strTextureFile);
		fread(&dwStartFrame, 4, 1, fp);
		fread(&dwEndFrame, 4, 1, fp);
		lpEffect->SetVisibility(bLoad);
		lpEffect->Create(dwStartFrame, dwEndFrame);
		lpEffect->SetEffectKind(Kind);
		lpEffect->SetEffectScale(m_GemScale[0],m_GemScale[1],m_GemScale[2]);
		lpEffect->SetLight(m_bLight);
		lpEffect->Load(fp, strFilePath);
		if(bLoad || bParticle)
			lpEffect->CreateBuffer();
		// 스케일 세팅
		
	}

	fclose(fp);
}

void CX3DEffect::Save(const char *strFilePath, const char *strFileName)
{
	FILE *fp;
	char strFile[MAX_PATH];
	strcpy(strFile, strFilePath);
	strcat(strFile, strFileName);
	fp = fopen(strFile, "wb");
	if(!fp) return;

	list<CX3DEffectBase *>::iterator it;
	CX3DEffectBase *pEffect;

	unsigned short Size = m_lstEffect.size(), Kind;
	unsigned char len;
	unsigned long StartFrame, EndFrame;

	fwrite(&m_dwMaxFrame, 4, 1, fp);
	fwrite(&m_dwFrameTick, 4, 1, fp);
	fwrite(&m_fIncFrame, 4, 1, fp);
	fwrite(&Size, 2, 1, fp);
	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);

		Kind = (unsigned short)pEffect->GetEffectKind();
		StartFrame = pEffect->GetStartFrame();
		EndFrame = pEffect->GetEndFrame();
		len = strlen(pEffect->GetTextureFileName()) + 1;

		fwrite(&Kind, 2, 1, fp);
		if(len == 1)
		{
			len = 0;
			fwrite(&len, 1, 1, fp);
		} else
		{
			fwrite(&len, 1, 1, fp);
			fwrite(pEffect->GetTextureFileName(), len, 1, fp);
		}
		fwrite(&StartFrame, 4, 1, fp);
		fwrite(&EndFrame, 4, 1, fp);

		pEffect->Save(fp);
	}

	fclose(fp);
}

void CX3DEffect::SetDirectMove(vector3 &vecDest, vector3 &vecPower, vector3 &vecEForce)
{
	m_dwKindMove = 1;
	m_vecDest = vecDest;
	m_vecPower = (m_vecDest - *m_lpCenter) / 30.0f;
	m_vecEForce = vecEForce;
}
void CX3DEffect::SetLoop(BOOL bLoop) { 
	m_bLoop = bLoop; 
	list<CX3DEffectBase *>::iterator it;
	
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetGemClearBuffer(!bLoop);				// clear buffer setting
	}

}
void CX3DEffect::SetWorldEffect(bool t) {
		
	list<CX3DEffectBase *>::iterator it;
	
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetWorldEffect(t);
	}

}
void CX3DEffect::SetStopParticle(int t) {
		
	list<CX3DEffectBase *>::iterator it;
	
	CX3DEffectBase *pEffect;

	for(it = m_lstEffect.begin(); it != m_lstEffect.end(); it++)
	{
		pEffect = (CX3DEffectBase *)(*it);
		pEffect->SetStopParticle(t);
	}

}