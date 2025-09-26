// ChristmasParticle.h: interface for the CChristmasParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHRISTMASPARTICLE_H__24078282_01D5_4099_A4CF_1B76DA1E6890__INCLUDED_)
#define AFX_CHRISTMASPARTICLE_H__24078282_01D5_4099_A4CF_1B76DA1E6890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NatureParticle.h"
#include <sys/types.h>
#include <sys/timeb.h>

const DWORD dwRainParticleVertexDecl = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));

class CRainParticleVertex {
public:
	D3DXVECTOR3 m_vecPos;
	DWORD		m_dwColor;
	D3DXVECTOR2 m_vecTexcoord;
	
};


const DWORD dwChristmasParticleVertexDecl = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

class CChristmasParticleVertex {
public:
	D3DXVECTOR3 m_vecPos;
	DWORD		m_dwColor;
	
};

class CTexture;
class CChristmasParticle : public CNatureParticle  
{
public:
	CChristmasParticle();
	virtual ~CChristmasParticle();
	virtual void Init(int iNodesNum,char *strTexture);
	virtual void UpdateBuffer(float fStep = 1.0f);
	virtual void Render();
	void	SetStart(bool bFlag) { m_bStart = bFlag;}
	void	SetCenter(float fX,float fY,float fZ) { m_vecCenter = D3DXVECTOR3(fX,fY,fZ);}
	DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
	float						m_fHeight;
//	bool						m_bStart;

	D3DXVECTOR3					m_vecCenter;
	D3DXVECTOR3					m_vecUp;
	D3DXVECTOR3					m_vecRight;
	DWORD						m_dwStartTime;

protected:
	LPDIRECT3DVERTEXBUFFER8		m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8		m_pIndexBuffer;
	LPDIRECT3DDEVICE8			m_pDevice;
	CTexture					*m_pImageTexture;
	bool						m_bPointSprite;



};

#endif // !defined(AFX_CHRISTMASPARTICLE_H__24078282_01D5_4099_A4CF_1B76DA1E6890__INCLUDED_)
