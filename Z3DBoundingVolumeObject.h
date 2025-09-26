// Z3DBoundingVolumeObject.h: interface for the CZ3DBoundingVolumeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DBOUNDINGVOLUMEOBJECT_H__A99E636C_CA7E_4258_A5CC_2794520D5ADE__INCLUDED_)
#define AFX_Z3DBOUNDINGVOLUMEOBJECT_H__A99E636C_CA7E_4258_A5CC_2794520D5ADE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DRenderable.h"
#include "Z3DObject.h"
#include "Z3DCharacterModel.h"

#include "Z3DTYPES.h"


class CZ3DOBB : public CZ3DRenderable
{
public:
	CZ3DOBB() : CZ3DRenderable(false)
	{
	}

	virtual ~CZ3DOBB()
	{
	}
	
	void SetStaticParameter( vector3& vExtent );
	void SetDynamicParameter( vector3& vCenter, matrix& rDir );

	
	bool CheckCollision( const CZ3DOBB& rOBB ) const;

	void Render();
	
	void SecondRender()
	{
	}
	
	
	static void _SetRenderFlag( bool bRender )
	{
		ms_bRender = bRender;
	}

//protected:
	// non-dynamic
	vector3 m_vExtent;

	// need to be update dynamically
	vector3 m_Center;
	vector3 m_aAxis[3];

	
	static D3DSLVERTEX ms_pOutlineVertex[8];
	static WORD ms_pOutlineIndex[24];
	static bool ms_bRender;

};



class CZ3DBoundingVolumeObject : 
	public CZ3DObject, 
	public CZ3DRenderable  
{
public:
	CZ3DBoundingVolumeObject();
	virtual ~CZ3DBoundingVolumeObject();

	CZ3DOBB& GetOBB()
	{
		return m_OBB;
	}

	void UpdateFrame();

	void Render();
	
	void SecondRender()
	{
	}

	void SetChrModel( CZ3DCharacterModel* pChr )
	{
		m_rpChrModel = pChr;
	}

	void SetExtent( const vector3& v )
	{
		m_vExtent = v;
	}

	void SetScale( float f )
	{
		m_fScaleFactor = f;
	}

	
	static void _SetRenderFlag( bool bRender )
	{
		ms_bRender = bRender;
	}

	

protected:
	CZ3DOBB m_OBB;

	CZ3DCharacterModel* m_rpChrModel;

	float m_fScaleFactor;

	vector3 m_vExtent;

	
	static D3DSLVERTEX ms_pOutlineVertex[8];
	static WORD ms_pOutlineIndex[24];

	static bool ms_bRender;
};

#endif // !defined(AFX_Z3DBOUNDINGVOLUMEOBJECT_H__A99E636C_CA7E_4258_A5CC_2794520D5ADE__INCLUDED_)
