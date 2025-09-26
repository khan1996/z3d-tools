
#ifndef _Sphere_H_
#define _Sphere_H_

#pragma once 

struct IDirect3DVertexBuffer8;
struct IDirect3DDevice8;

////////////////////////////////////////////////////////////////////////////
//

class CSphere
{
	IDirect3DDevice8 *			m_pd3dDevice;
	IDirect3DVertexBuffer8 *	m_pVB;
	unsigned					m_uiRings, m_uiSegments;					//구의 위도, 경도의 개수

	float						m_fRed, m_fGreen, m_fBlue;
	unsigned					m_uiSphereVertices;
	unsigned char				m_ucTransparency;
public:
	CSphere();
	~CSphere();

	void	Create( IDirect3DDevice8 *, unsigned rings, unsigned segments );
	void	Destroy();

	void	SetPosition( float x, float y, float z );
	void	SetTransparency( unsigned char t )			{ m_ucTransparency = t; }
	void	SetRadius( float radius )					{ m_fRadius = radius; }
	void	SetColor( float r, float g, float b );
	void	Render();
	float						m_fPosX, m_fPosY, m_fPosZ, m_fRadius;
};

////////////////////////////////////////////////////////////////////////////

#endif

