#ifndef __SNOWFALL_H__
#define __SNOWFALL_H__

#pragma once

#include <d3dx8.h>
#include <d3d8.h>
#include "texture.h"
#include "BaseDataDefine.h"

#define SDEFAULTNUM 100
#define SMAXSIZE 50.0 //snow Max size
#define SMINSIZE 1.0 // snow Min size
#define CSNOWRAND 5

#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

enum CSNOWAXIS {
	CSNOWX,
	CSNOWZ,
};

class CSnowFall{ 
public:
	class CSnowUnit {
	public:
		D3DXVECTOR3 m_Pos;
		float m_Size;
		float m_HeightFieldHigh;
		
		bool m_Life;
		float m_Alpha;
		float m_RGB[3];
		float m_UnitAlpha;

		DWORD m_Color;

		CSnowUnit() {
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			m_Size = SMINSIZE;
			m_HeightFieldHigh = -1.0f;
			m_Life = false;
			m_Alpha = 1.0f;
			m_UnitAlpha = 0.1f;
			m_RGB[0] = m_RGB[1] = m_RGB[2] = 1.0f;

			m_Color = D3DCOLOR_COLORVALUE(m_RGB[0],m_RGB[1],m_RGB[2],m_Alpha);
			
		}
		~CSnowUnit() {}
	};
	
	class CSnowVertex {
	public:	
		D3DXVECTOR3 v;
	    D3DCOLOR    color;
	
		CSnowVertex() {
			v.x = v.y = v.z = 0.0f;
			color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
		}
		~CSnowVertex() {}
	};

	float m_Size[2];
	float m_UnitSize[2];

	float m_StartHeight;
	int m_SnowNum;
	
	D3DXVECTOR3 m_PlayerPos;
	D3DXVECTOR3 m_Gravity;
	D3DXVECTOR3 m_WindSpeed;
	float m_Color[3];

	LPDIRECT3DDEVICE8 m_Device;
	LPDIRECT3DVERTEXBUFFER8 m_VBuffer;
	CTexture *m_STexture;

	CSnowUnit *m_Units;
	bool m_bValueSet;

	CSnowFall();
	~CSnowFall();
	
	void Init(LPDIRECT3DDEVICE8 device,int num,float x,float y,float z,float xsize,float zsize,float height);
	void SetColor(float r,float g,float b);
	void SetUnitInfo(int num);
		
	void SetWorldValue(float gx,float gy,float gz,float wx,float wy,float wz,char *texture);

	void SetDevice(LPDIRECT3DDEVICE8 device) { m_Device = device;}
	void SetVertexBuffer();

	void Render();
	void Update(float x,float y,float z,float xsize,float zsize,float height,bool bReset);
	DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

};


#endif