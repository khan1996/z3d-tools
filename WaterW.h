#ifndef __CWATERW_H__
#define __CWATERW_H__

#pragma once

#include <d3dx8.h>
#include <d3d8.h>
#include <vector>
#include "texture.h"
#include "BaseDataDefine.h"

using namespace std;
#define WATERWWIDTH 256
#define WATERWHEIGHT 256
#define WATERWCHOP 2
#define WATEREMPTY -1

class CWaterW {
public:
	// 물위를 떠다니는 것들의 정보를 가지고 있는 class
	class CDriftUnit {
	public:
		// 유닛이 영향을 미치는 범위 
		int m_nUnitRange;
		int m_nIndex; // water에 add 시킬때에 주는 고유 인덱스 번호 
		D3DXVECTOR3 m_UnitPos;
		D3DXVECTOR3 m_UnitFwd;

		CDriftUnit(){
			m_nUnitRange = 4;
			m_nIndex = WATEREMPTY;
			m_UnitPos.x = m_UnitPos.y = m_UnitPos.z = 0.0f;
			m_UnitFwd.x = m_UnitFwd.y = m_UnitFwd.z = 0.0f;

		}
		~CDriftUnit(){}

		int SetUnitIndex(int n);

	};

private:
	// water vertex class
	class CWaterVertex{
	public:	
		float x,y,z;
		//normal
		float nx,ny,nz;

/*		DWORD color;
		float s,t;*/
		CWaterVertex() {
			x = y = z = 0.0f;
		//	color = D3DCOLOR_ARGB(255,255,255,255);
		//	s = t = 0.0f;
		}
		~CWaterVertex() {}
	};	
	// 0,0 버텍스의 시작 위치
	D3DXVECTOR3 m_ZeroPos;
	// 쿼드 하나의 사이즈 
	float m_QuadSize;

	double **m_pWaterSpace;
	double **m_pWaterChangeValue;
	double **m_pWaterCoord;
	int m_nWidthSize;
	int m_nHeightSize;

	double m_fLimitHeight;
	float m_nChopPower;
	vector<CDriftUnit> m_DriftUnit;
	int m_nDriftCount;
	int m_IndexNum;
	LPDIRECT3DVERTEXBUFFER8 m_WaterVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_WaterIndexBuffer;

	CTexture m_WaterTexture;
	CTexture m_WaterEnv;
	LPDIRECT3DDEVICE8 m_Device;

    LPDIRECT3DTEXTURE8     m_pSkyTex[6];
    LPDIRECT3DCUBETEXTURE8 m_pSkyCubeTex;

public:

	CWaterW();
	CWaterW(int size,double limitheight,float choppower,float x,float z,float quadsize,LPDIRECT3DDEVICE8 device);
	CWaterW(int width,int height,double limitheight,float choppower,float x,float z,float size,LPDIRECT3DDEVICE8 device);
	bool CreateWaterWSpace();
	void InitWaterW();
	
	void ChangeVertexBuffer();
	void CreateVertexBuffer();
	void CreateTexture();

	void SetDevice(LPDIRECT3DDEVICE8 device) { m_Device = device;}
	bool SetTexture(char *filename,char *envfile);
	void SetZeroPos(float x,float y,float z) {m_ZeroPos.x = x;m_ZeroPos.y = y;m_ZeroPos.z = z;}
	void SetQuadSize(float size) { m_QuadSize = size;}
	
	void SetLimitHeight(double h);
	void SetChopPower(int p) {m_nChopPower = (float)p;}
	
	void AddDriftUnit(CDriftUnit );
	void DeleteDriftUnit(CDriftUnit );
	
	void Render();
	void Update(float x,float y,float z);

	void UpdateWaterW();
	void ComputeWaterW();
	void SetHeight(int x,int y,float height);

	~CWaterW();

};

#endif
