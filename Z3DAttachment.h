// Z3DAttachment.h: interface for the CZ3DAttachment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DATTACHMENT_H__51ABB6E1_1ECB_11D5_A643_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DATTACHMENT_H__51ABB6E1_1ECB_11D5_A643_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DObject.h"
#include "Z3DRenderable.h"
#include "Z3DTypes.h"
#include "Z3DTexture.h"
#include "Z3D_Globals.h"
#include "Z3DGradeEffectHandler.h"
#include "Z3DGlowHandler.h"




class CZ3DAttachment : 
	public CZ3DRenderable, 
	public CZ3DObject  
{
public:
	CZ3DAttachment()
		: CZ3DRenderable(true)
	{
		m_pTexture = NULL; // CZ3DRenderable member
		m_fScaleFactor = 1.0f;

		m_pVertexBuffer = NULL;
		m_lVertexCount = 0;

		m_pIndexBuffer = NULL;
		m_lIndexCount = 0;
	}

	CZ3DAttachment( const vector3 tm_pos, const quaternion tm_dir )
		: CZ3DRenderable(true)
	{
		m_pVertexBuffer = NULL;
		m_lVertexCount = 0;

		m_pIndexBuffer = NULL;
		m_lIndexCount = 0;

		m_pTexture = NULL; // CZ3DRenderable member
		m_Pos = tm_pos;
		m_Dir = tm_dir;
		m_vPivotPos.Identity();
		m_fScaleFactor = 1.0f;
	}
	//CZ3DAttachment( const char* szMesh, const char* szTex, const vector3 tm_pos, const quaternion tm_dir );
	virtual ~CZ3DAttachment();

	bool Set( const char* szMesh, const char* szTex, const char* szTex2 );

	void ApplyTransform();

	void Render();

	void SecondRender() {}

	void RenderShadow( IDirect3DDevice8* pDevice, DWORD vertexShader = 0xFFFFFFFF );
	void RenderGlowShape( IDirect3DDevice8* pDevice );

	void SetScale( float s )
	{
		m_fScaleFactor = s;
	}

	float GetScale()
	{
		return m_fScaleFactor;
	}

	void GetMarker( vector3& v1, vector3& v2 )
	{
		_ASSERT( m_tagAMesh.GetObject() );
		BumpVertex* pV;

		pV = &(m_tagAMesh.GetObject()->m_pVertices[m_tagAMesh.GetObject()->m_wMarker1]);
		v1.x = pV->v.x*m_TM._11 + pV->v.y*m_TM._21 + pV->v.z*m_TM._31 + m_TM._41;
		v1.y = pV->v.x*m_TM._12 + pV->v.y*m_TM._22 + pV->v.z*m_TM._32 + m_TM._42;
		v1.z = pV->v.x*m_TM._13 + pV->v.y*m_TM._23 + pV->v.z*m_TM._33 + m_TM._43;

		pV = &(m_tagAMesh.GetObject()->m_pVertices[m_tagAMesh.GetObject()->m_wMarker2]);
		v2.x = pV->v.x*m_TM._11 + pV->v.y*m_TM._21 + pV->v.z*m_TM._31 + m_TM._41;
		v2.y = pV->v.x*m_TM._12 + pV->v.y*m_TM._22 + pV->v.z*m_TM._32 + m_TM._42;
		v2.z = pV->v.x*m_TM._13 + pV->v.y*m_TM._23 + pV->v.z*m_TM._33 + m_TM._43;
	}

	void SetGradeEffectIndex( int n )
	{
		m_GradeEffectHandler.SetLevel( n );
	}
	
	void SetGlowIndex( int n )
	{
		m_GlowHandler.SetLevel( n );
	}


	
	//static bool _Init( const char* szAMeshHandleFile, const char* szTextureHandleFile );

protected:
	//BumpVertex* m_pVertices;
	IDirect3DVertexBuffer8* m_pVertexBuffer;
	long m_lVertexCount;

	IDirect3DIndexBuffer8* m_pIndexBuffer;
	long m_lIndexCount;

	float m_fScaleFactor;
	H3DAMeshTag m_tagAMesh;
	Z3DTexture* m_pTagTexture;
	Z3DTexture* m_pTagTexture2;

	CZ3DGradeEffectHandler m_GradeEffectHandler;
	CZ3DGlowHandler m_GlowHandler;

};

#endif // !defined(AFX_Z3DATTACHMENT_H__51ABB6E1_1ECB_11D5_A643_0000E8EB4C69__INCLUDED_)
