// NatureParticle.h: interface for the CNatureParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NATUREPARTICLE_H__C5B07131_294F_4F5B_8093_CA560472DB64__INCLUDED_)
#define AFX_NATUREPARTICLE_H__C5B07131_294F_4F5B_8093_CA560472DB64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3dx8.h>
#include "Texture.h"

#define PI		   3.1415926535897932384626433832795f
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180 57.2957795130823208767981548141052f
//#define EPSILON	   1.0e-8
#define SMALL	   1.0e-4f
#define BIG		   1.0e+10f


#define RANDOM_FLOAT		( ( ( float )rand( ) )/RAND_MAX )
#define DEG_TO_RAD( angle )	  ( ( angle )*PIOVER180 )
#define RAD_TO_DEG( radians ) ( ( radians )*PIUNDER180 )

class CNatureParticle  
{
public:
	class CParticleNode {
	public:
		float			m_fLife;
		D3DXVECTOR3		m_vecPos;
		D3DXVECTOR3		m_vecBeforePos;

		D3DXVECTOR3		m_vecVelocity;
		
		D3DXVECTOR3		m_vecSize;
		D3DXVECTOR3		m_vecSizeUnit;
		
		float			m_fMass;
		D3DXVECTOR3		m_vecColor;
		D3DXVECTOR3		m_vecColorUnit;

		float			m_fVisible;
		float			m_fVisibleUnit;

		float			m_fFraction;
	
		float			m_fExtension[3];
		

		CParticleNode() {
			m_fLife = -1.0f;
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecBeforePos = D3DXVECTOR3(0.0f,0.0f,0.0f);

			m_vecVelocity = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecSize = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecSizeUnit = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecColor = D3DXVECTOR3(1.0f,1.0f,1.0f);
			m_vecColorUnit = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_fVisible = 0.0f;
			m_fVisibleUnit =0.0f;
			
			m_fExtension[0] = m_fExtension[1] = m_fExtension[2] = 0.0f;

		}	
		~CParticleNode(){}
	};

	CNatureParticle();
	virtual ~CNatureParticle();

	inline float RangedRandom( float f1, float f2 )
	{	return ( f1+( f2-f1 )*((float)rand()/(float)RAND_MAX) );	}

	void CreateInUseNode(float fVelocityX,float fVelocityY,float fVelocityZ,int RandValue = 0);
	bool InitParticle(int iNodesNum,char *strTexture);
	void Release();
	void Update(float fStep = 1.0f,float fHeight = 0.0f);

	void Explode(float fMag,int iNodesNums);
	void CreateRain(float fMinRangeX,float fMinRangeY,float fMinRangeZ,
					float fMaxRangeX,float fMaxRangeY,float fMaxRangeZ,
					int iDropSpeed,int iNumDrops);
	void CreateChristmasParticle(float fX,float fY,float fZ,int iDropSpeed,int iNumDrops,
		float fRightX,float fRightY,float fRightZ,float fUpX,float fUpY,float fUpZ,bool bForce = false);

	void SetMaxLife(float fLife) { m_fMaxLife = fLife;}
	void SetBasePosition(float fX,float fY,float fZ) { m_vecPos = D3DXVECTOR3(fX,fY,fZ);}
	void SetBaseMass(float fMass) { m_fMass = fMass;}
	
	void SetSize(float fStartWidth,float fStartHeight,
				float fEndWidth,float fEndHeight) {
		m_vecStartSize = D3DXVECTOR3(fStartWidth,0.0f,fStartHeight);
		m_vecEndSize = D3DXVECTOR3(fEndWidth,0.0f,fEndHeight);		
	}
	void SetColor(float fStartColorR,float fStartColorG,float fStartColorB,
				float fEndColorR,float fEndColorG,float fEndColorB) {
		m_vecStartColor = D3DXVECTOR3(fStartColorR,fStartColorG,fStartColorB);
		m_vecEndColor = D3DXVECTOR3(fEndColorR,fEndColorG,fEndColorB);

	}
	void SetFraction(float fFraction) { m_fFraction = fFraction;}
	void SetVisible(float fStartVisible,float fEndVisible) {
		m_fStartVisible = fStartVisible;
		m_fEndVisible = fEndVisible;
	}
	void SetGravity(float fX,float fY,float fZ) { m_vecGravity = D3DXVECTOR3(fX,fY,fZ);}
	int	GetScreenNodesNum() { return m_iScreenNodesNum;}

	virtual void Init(int iNodesNum,char *strTexture) = 0;
	virtual void UpdateBuffer(float fStep = 1.0f) = 0;
	virtual void Render() = 0;
	void SetParticlePosition(BYTE *pImage,DWORD dwWidth,DWORD dwHeight);
	void SetExtUse(bool bFlag) { m_bExtUse = bFlag; m_bExtUse2 = bFlag;}
	void SetEnd(bool bFlag) { m_bEnd = bFlag;}
	bool GetEnd() { return m_bEnd;}

	int				m_iImageParticleNum;
	bool			m_bStart;
	bool			m_bFirst;
	int				m_iExtCount;

protected:
	CParticleNode			*m_pNodes;
	
	int						m_iNodesNum;
	int						m_iScreenNodesNum;

	D3DXVECTOR3				m_vecGravity;
	
	CTexture				*m_pParticleTexture;
	CTexture				*m_pExtTexture;

	float					m_fMaxLife;
	
	D3DXVECTOR3				m_vecPos;
	D3DXVECTOR3				m_vecStartSize;
	D3DXVECTOR3				m_vecEndSize;
	float					m_fMass;
	
	D3DXVECTOR3				m_vecStartColor;
	D3DXVECTOR3				m_vecEndColor;
	
	float					m_fFraction;

	float					m_fStartVisible;
	float					m_fEndVisible;
	
	LPDIRECT3DDEVICE8		m_pDevice;
	int						m_iVertexBufferNum;
	int						m_iIndexBufferNum;
	
	bool					m_bExtUse;
	bool					m_bExtUse2;

	bool					m_bEnd;



	/*	LPDIRECT3DVERTEXBUFFER8	m_pVertexBuffer;
	

	LPDIRECT3DINDEXBUFFER8	m_pIndexBuffer;
	*/
};

#endif // !defined(AFX_NATUREPARTICLE_H__C5B07131_294F_4F5B_8093_CA560472DB64__INCLUDED_)
