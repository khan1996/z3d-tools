// Z3DBladeTrail.h: interface for the CZ3DBladeTrail class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_Z3DBLADETRAIL_H__23EE5981_251D_11D5_A644_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DBLADETRAIL_H__23EE5981_251D_11D5_A644_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DRenderable.h"
#include "Z3DObject.h"
#include "Z3DAttachment.h"
//#include "Z3DManagedObject.h"

#include <vector>


class CZ3DBladeTrail :
	public CZ3DRenderable,
	public CZ3DObject
//	public CZ3DManagedObject
{
public:
	CZ3DBladeTrail( CZ3DAttachment* pWeapon, char* szTex, long lDuration, long lTessel );
	virtual ~CZ3DBladeTrail();

	bool IsCulled()
	{
		return false;
	}

	void Render() {}

	void SecondRender();
	bool IsSecondRenderable()
	{
		return true;
	}

	void SetRGB( BYTE r = 0xFF, BYTE g = 0xFF, BYTE b = 0xFF )
	{
		m_R = r;
		m_G = g;
		m_B = b;
	}

	void SetTexture( const char* szFileName );
	void SetTexture( IDirect3DTexture8* pTexture );


	void Disable()
	{
		SetCull( true );
		SetDisable( true );
	}

	void Enable()
	{
		SetCull( false );
		SetDisable( false );
	}


	void Activate();
	void Stop();


	bool IsActive()
	{
		return m_bActive;
	}

	void UpdateFrame();

protected:
	//vector3 m_vPivot;
	CZ3DAttachment* m_rpWeapon;
	Z3DTexture* m_pTagTexture;

	D3DLVERTEX* m_pVertex;
	WORD* m_pIndex;

	// ��
	BYTE m_R, m_G, m_B;

	// �� ������ �������� ���� ���� ���������� ���� ���
	long m_lDuration;
	long m_lTessel;

	vector3 (*m_paTrace)[2];
	long m_lHeadIndex;
	long m_lLength;

	bool m_bActive;	// ȭ�鿡 ��°����� �����ΰ�?(������ �Ŀ��� ������ ������ �������� ������ true)
	bool m_bInputStopped;// �˱��� �߰��Է��� ���� �����ΰ�?
};



#define Z3D_BOWSTRING_TENSOIN_COUNT 9

class CZ3DBowString :
	public CZ3DRenderable,
	public CZ3DObject
{
public:
	CZ3DBowString( CZ3DAttachment* pWeapon, char* szTex, CZ3DObject* pManipulator );
	virtual ~CZ3DBowString();

	void Render() {}

	void SecondRender();
	bool IsSecondRenderable()
	{
		return true;
	}

	/*void SetRGB( BYTE r = 0xFF, BYTE g = 0xFF, BYTE b = 0xFF )
	{
		m_R = r;
		m_G = g;
		m_B = b;
	}*/

	//void SetTexture( const char* szFileName );

	void Disable()
	{
		SetCull( true );
		SetDisable( true );
	}

	void Enable()
	{
		SetCull( false );
		SetDisable( false );

		vector3 v1, v2;
		m_rpWeapon->GetMarker( v1, v2 );
		//m_vPrevMid = (v1+v2)/2;
	}

	void GripString();
	void ReleaseString();

	bool IsGripped()
	{
		return m_bGripped;
	}

	void UpdateFrame();

protected:
	//vector3 m_vPivot;
	CZ3DAttachment* m_rpWeapon;
	CZ3DObject* m_rpManipulator;
	Z3DTexture* m_pTagTexture;

	D3DLVERTEX* m_pVertex;
	//WORD* m_pIndex;

	// ��
	//BYTE m_R, m_G, m_B;

	//bool m_bNoForce;		// Ȱ������ ������ ���� ���� �����ΰ�?
							// (Ȱ������ ������ �Ŀ��� ������ ���ư��� �������� �� �÷��װ� false��)
	bool m_bGripped;		// Ȱ������ ���� �����ΰ�?
	vector3 m_vTensionDir;	// Ȱ���� ����� ����(�������·κ���)
	//vector3 m_vPrevMid;		// Ȱ���� ����� ���� ������ ��ġ
	//float m_fTensionLength;	// Ȱ���� ����� ����
	long m_lAfterProjection;

	static float ms_fTensionConstant[Z3D_BOWSTRING_TENSOIN_COUNT];
};



#endif // !defined(AFX_Z3DBLADETRAIL_H__23EE5981_251D_11D5_A644_0000E8EB4C69__INCLUDED_)
