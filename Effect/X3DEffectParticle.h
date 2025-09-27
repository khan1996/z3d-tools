// X3DEffectParticle.h: interface for the CX3DEffectParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTPARTICLE_H__0750B6D9_11E2_41A9_BFEF_D92F0F1E0CC5__INCLUDED_)
#define AFX_X3DEFFECTPARTICLE_H__0750B6D9_11E2_41A9_BFEF_D92F0F1E0CC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"

typedef struct _Particle
{
	vector3 vecPos;
	vector3 vecVel;
	float fLifetime;
	float fMaxLife;
	float fTexFrame;

	float fRotation;
	unsigned long dwRotationCount;
	float fWidth;
	float fHeight;
	color lColor;
} Particle, *LPParticle;

class CX3DEffectParticle : public CX3DEffectBase
{
	protected:
		unsigned long m_dwMaxAmount;
		unsigned long m_dwDrawAmount;
		float m_fAlpha;
		float m_fPower;
		float m_fAngle;
		vector3 m_vecEForce;
		quaternion m_quatDir;

		float m_fPowerSeed;
		float m_fLifetime;
		float m_fLifetimeSeed;

		float m_fRotation;
		float m_fTexSpeed;

// 볼륨
		unsigned long m_dwVolumeType;		// 0 : None, 1 : Square, 2 : Circle
		float m_fVolX;
		float m_fVolY;
		float m_fVolZ;
		float m_fRadius;
		float m_fInnerRadius;

// 파티클 정보 / 버퍼
		LPParticle m_lpVerticeInfo;
		LPDIRECT3DVERTEXBUFFER8 m_lpVertices;
		LPDIRECT3DVERTEXBUFFER8 m_lpVerticesBlend;
		LPDIRECT3DINDEXBUFFER8 m_lpVerticeIndex;

	public:
		CKeyList<FloatKeyList> m_lstAmount;
		CKeyList<FloatKeyList> m_lstAlpha;
		CKeyList<FloatKeyList> m_lstPower;
		CKeyList<FloatKeyList> m_lstAngle;
		CKeyList<VectorKeyList> m_lstEForce;
		CKeyList<QuaternionKeyList> m_lstDirection;

// Scenario용 리스트
		CKeyList<FloatKeyList> m_lstWidth;
		CKeyList<FloatKeyList> m_lstHeight;

	public:
		CX3DEffectParticle();
		virtual ~CX3DEffectParticle();

		void SetVolumeNone(void);
		void SetVolumeSquare(float fVolX, float fVolY, float fVolZ);
		void SetVolumeCircle(float fRadius, float fInnerRadius);

		void SetPowerSeed(float fPowerSeed) { m_fPowerSeed = fPowerSeed; }
		void SetLifetime(float fLifetime) { m_fLifetime = fLifetime; }
		void SetLifetimeSeed(float fLifetimeSeed) { m_fLifetimeSeed = fLifetimeSeed; }
		void SetRotation(float fRotation) { m_fRotation = fRotation; }
		void SetTexSpeed(float fTexSpeed) { m_fTexSpeed = fTexSpeed; }

		vector3 GetEForce(void) { return m_vecEForce; }
		unsigned char GetMainAlpha(void) { return (unsigned char)(m_fAlpha * 255); }
		float GetAmount(float fFrame) { float temp; m_lstAmount.Interpolation(fFrame, temp); return temp;}
		float GetLifetime(void) { return m_fLifetime; }
		float GetPower(void) { return m_fPower; }
		float GetAngle(void) { return m_fAngle; }
		quaternion GetDirection(void) { return m_quatDir; }
		float GetWidth(float fFrame) { float temp; m_lstWidth.Interpolation(fFrame, temp); return temp; }
		float GetHeight(float fFrame) { float temp; m_lstHeight.Interpolation(fFrame, temp); return temp; }

		unsigned char GetAlpha(float fFrame) { color temp; m_lstColor.InterpolationC(fFrame, temp); return temp.a; }
		unsigned char GetRColor(float fFrame) { color temp; m_lstColor.InterpolationC(fFrame, temp); return temp.r; }
		unsigned char GetGColor(float fFrame) { color temp; m_lstColor.InterpolationC(fFrame, temp); return temp.g; }
		unsigned char GetBColor(float fFrame) { color temp; m_lstColor.InterpolationC(fFrame, temp); return temp.b; }

		BOOL CreateParticle(LPParticle lpParticle);

		void Create(unsigned long dwStartFrame, unsigned long dwEndFrame);
		BOOL CreateBuffer(void);
		void Render(void);
		BOOL Interpolation(float fFrame);
		void Load(FILE *fp, const char *strOriginalPath = NULL);
		void Save(FILE *fp, const char *strOriginalPath = NULL);

		BOOL ParticleInterpolation(unsigned long dwNumber);
};

#endif // !defined(AFX_X3DEFFECTPARTICLE_H__0750B6D9_11E2_41A9_BFEF_D92F0F1E0CC5__INCLUDED_)
