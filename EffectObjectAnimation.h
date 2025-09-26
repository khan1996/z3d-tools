// EffectObjectAnimation.h: interface for the CEffectObjectAnimation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTOBJECTANIMATION_H__40A299A4_DF8B_4714_B504_74380E21D329__INCLUDED_)
#define AFX_EFFECTOBJECTANIMATION_H__40A299A4_DF8B_4714_B504_74380E21D329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffectObject.h"
#include "MeshObject.h"
#include <vector>
#include "FileLoad.h"

class CEffectObjectAnimation : public CEffectObject  
{
	class KeyPosition{public:float x,y,z;long keytime;};
	class KeyRotation{public:float x,y,z,w;long keytime;};
	class KeyScale{public:float x,y,z;long keytime;};

	class AnimationPackage
	{
	public:
		std::vector<KeyPosition> m_KeyPosList;
		std::vector<KeyRotation> m_KeyRotList;
		std::vector<KeyScale> m_KeySclList;
	};
	std::vector<AnimationPackage*> m_AnimationPackList;
	std::vector<CMeshObject*> m_pNodeMeshObjectList;
public:
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Load(char *strFilename);
	CEffectObjectAnimation();
	virtual ~CEffectObjectAnimation();

};

#endif // !defined(AFX_EFFECTOBJECTANIMATION_H__40A299A4_DF8B_4714_B504_74380E21D329__INCLUDED_)
