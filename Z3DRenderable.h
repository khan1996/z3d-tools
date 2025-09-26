// Z3DRenderable.h: interface for the CZ3DRenderable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DRENDERABLE_H__8D329721_B6F6_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DRENDERABLE_H__8D329721_B6F6_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
#include <queue>

#include <d3d8.h>
#include <d3dx8.h>


#include "misc.h"
#include "vector.h"
#include "Z3D_Globals.h"



class CZ3DRenderable  
{
public:
	CZ3DRenderable( bool bIndivdualRender = false )
	{
		m_bIndividualRender = bIndivdualRender;
			
		m_pTexture = NULL;
		/*
			pointer to texture of Renderable instance :
		NULL - no texture

		NULL < m_pTexture < 0xFFFFFFFF - texture change is managed by
			CZ3DRenderable class

		0xFFFFFFFF ( = ~NULL ) - Render method of this instance is reponsible for
			setting texture	(by means of using multiple textured mesh or something)
		*/

		ms_listpObjects.push_back( this );
		m_iterThis = --(ms_listpObjects.end());
		
		m_bCull = false;
		m_lLODLevel = 0;

		m_fTranparency = 1.0f;

		m_SecondRenderSrcBlend = D3DBLEND_SRCALPHA;
		m_SecondRenderDestBlend = D3DBLEND_ONE;
	}

	virtual ~CZ3DRenderable()
	{
		if(!ms_listpObjects.empty())
		{
			ms_listpObjects.erase( m_iterThis );
			m_iterThis = ms_listpObjects.end();
		}
	}

	void SetTransparency( float f )
	{
		m_fTranparency = f;
	}

	float GetTransparency()
	{
		return m_fTranparency;
	}

	IDirect3DTexture8* GetTexturePtr() { return m_pTexture; }
	void SetTexturePtr( IDirect3DTexture8* pTexture )
	{
		_ASSERT( pTexture );

		m_pTexture = pTexture;
	}
	static vector3 m_vecCameraPosition;
	static void SetCameraPosition(vector3 vecCameraPosition){m_vecCameraPosition=vecCameraPosition;};

	virtual bool IsCulled()
	{
		return m_bCull;
	}
	void SetCull( bool bCullFlag )
	{
		m_bCull = bCullFlag;
	}
	
	void SetLODLevel( long l )
	{
		_ASSERTE( l < Z3D_LOD_LEVEL && l >= 0 );
		
		m_lLODLevel = l;
	}

	bool IsRenderIndividually()
	{
		return m_bIndividualRender;
	}

	void SetIndividualRender( bool b )
	{
		m_bIndividualRender = b;
	}

	void RenderIndividual()
	{
		if( m_bCull )
		{
			return;
		}
		
		if( m_pTexture != (IDirect3DTexture8*)0xFFFFFFFF )
		{
			GetDevice()->SetTexture( 0, m_pTexture );
		}
		ms_Material.Diffuse.a = GetTransparency();
		ms_Material.Diffuse.r = 1.0f;
		ms_Material.Diffuse.g = 1.0f;
		ms_Material.Diffuse.b = 1.0f;		

		ms_Material.Ambient.r = 1.0f;
		ms_Material.Ambient.g = 1.0f;
		ms_Material.Ambient.b = 1.0f;		

		GetDevice()->SetMaterial( &ms_Material );
		Render(); 
	}

	void SetSecondRenderBlend( D3DBLEND srcBlend, D3DBLEND destBlend )
	{
		m_SecondRenderSrcBlend = srcBlend;
		m_SecondRenderDestBlend = destBlend;
	}
	
	virtual void Render() = 0;
	virtual void SecondRender() = 0;
	virtual bool IsSecondRenderable()
	{
		return false;	// 기본적으로 안함
	}
	/* second render blend state 적용하면서 제거
	virtual bool IsLightenSecondRender()
	{
		return true;	// 기본적으로 밝아지게...흠냐-_-;
	}*/

	// static functions
	static void Process(); // batch process of whole object to render
	static void SecondProcess();

	static bool _Init(PDIRECT3DDEVICE8 pDevice);
	static bool _Close();

	static IDirect3DDevice8* GetDevice()
	{
		_ASSERT( ms_pDevice );

		return ms_pDevice;
	}

	long GetLODLevel()
	{
		return m_lLODLevel;
	}

protected:
	D3DBLEND m_SecondRenderSrcBlend, m_SecondRenderDestBlend;
	bool m_bIndividualRender;
	bool m_bCull;

	IDirect3DTexture8* m_pTexture;

	std::list<CZ3DRenderable*>::iterator m_iterThis;

	long m_lLODLevel;

	float m_fTranparency;

	static IDirect3DDevice8* ms_pDevice;

	static std::list<CZ3DRenderable*> ms_listpObjects;

	static std::vector<CZ3DRenderable*> ms_vec_pRenderObjects;
	static std::vector<CZ3DRenderable*> ms_vec_pSecondRenderObjects;

//	static unsigned long ms_ulVertexStateBlockHandle;
	static unsigned long ms_ulPixelStateBlockHandle;

	static vector3 ms_vec_CameraPos;

	// 렌더링시 자주 쓰이는 항등행렬-_-;;
	static D3DXMATRIX ms_IdentityD3DMatrix;

	static D3DMATERIAL8 ms_Material;

	// obsolete func.
	int GetLODLevel( float x, float y, float z )
	{
		float fx, fy, fz, fDistSquare;

		fx = ms_vec_CameraPos.x - x;
		fy = ms_vec_CameraPos.y - y;
		fz = ms_vec_CameraPos.z - z;
		fDistSquare = fx*fx + fy*fy + fz*fz;

		// distance-selection code - something like 'switch-case', 'if-else-if-else'..-_-;
		// 0 for most complex, 1 to next..... and n-1 for least complex in 'n-level LOD system'
		if( fDistSquare < 90000.0f ) // < 3m
		{
			return 0;
		}

		if( fDistSquare < 1000000.0f ) // 3m <= x < 10m
		{
			return 1;
		}

		//if( fDistSquare < 3240000.0f ) // 10m <= x < 18m
		if( fDistSquare < 9000000.0f ) // 10m <= x < 30m
		{
			return 2;
		}

		return 3;					// 18m < ~~
	}

	static D3DMATRIX* GetIdentityD3DMatrix()
	{
		return &ms_IdentityD3DMatrix;
	}

};

#endif // !defined(AFX_Z3DRENDERABLE_H__8D329721_B6F6_11D4_AD2B_0000E8EB4C69__INCLUDED_)
