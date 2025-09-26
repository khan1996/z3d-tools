#ifndef __CWAVELINE_H__
#define __CWAVELINE_H__

#pragma once

#include <vector>
#include "texture.h"
#include "BaseDataDefine.h"
#include <d3dx8.h>
#include <d3d8.h>


using namespace std;

#define WAVENUM 25
#define BACKWAVE 3
// 시작 라인과 backwave의 거리 
#define WAVEBACKLENGTH 90.0f
#define WAVEFWDLENGTH -1800.0f
#define WAVEUNITVOT 0.2f
#define WAVEVECLENGTH 1000.0f
#define WAVETERMFRAME 300
#define WAVESHAKE 15.0f;
// plane texture 반복 거리
#define PLANEWRAP 250000.0f

// 파도 전체를 몇등분하여 알파값을 줄지..
#define WAVESLIP 12

enum WaveType {
	WAVEFWDMOVE,
	WAVEBACKMOVE,
	WAVEPLANE,
	WAVEPLANE2,
};


class CWaveLine {

	class CWaveVertex{
	public:	
		float x,y,z;
		DWORD color;
		float s,t;
		CWaveVertex() {
			x = y = z = 0.0f;
			color = D3DCOLOR_ARGB(255,255,255,255);
			s = t = 0.0f;
		}
		~CWaveVertex() {}
	};	

	class CWavePoint{
	public:
		int m_WaveValue;
		// 포지션
		D3DXVECTOR3 m_Pos;
		// speed
		float m_Speed;
		// 가속도
		float m_Aspeed;
		// forward vector [ 실제 이동거리  = FwdVec * Speed]
		D3DXVECTOR3 m_FwdVec;
		// alpha
		float m_Alpha;
		// alpha 증감치
		float m_UnitAlpha;
		// 보간 점 까지 추가된 후의 위치 인덱스 
		int m_PointIndex;
		bool m_lie;

		CWavePoint() {
			m_WaveValue = WAVEFWDMOVE;
			m_lie = false;
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			m_Speed = 1.0f;
			m_Aspeed = 0.01f;
			m_FwdVec.x = m_FwdVec.y = m_FwdVec.z = 0.0f;
			m_Alpha =  0.0f;
			m_UnitAlpha = 0.0f;
			m_PointIndex =  -1;
		}
		~CWavePoint() {
		}

	};
public:
	// up vector
	D3DXVECTOR3 m_Normal;
	// quad point list
	vector<CWavePoint> m_WaveList;
	CWavePoint *m_BackupList;
	vector<CWaveVertex *> m_WaveLayer;
	vector<CWaveVertex *> m_WaveBubble;

	// quad line point
	int m_PointNum;

	// 보간 점까지 추가된 point list
	vector<CWavePoint *>m_Wave;
	// 보간점 추가된 point number
	int m_WaveNum;
	// 보간 정도 (보간점의 간격)
	float m_UnitInterpol;
	// 점들을 생성시에 얼마나 흔들어 줄지
	D3DXVECTOR3 m_ShakeValue;
	// 점들을 늘리게 할때 이용되는 점
	D3DXVECTOR3 m_WaveCenter;

	int m_DownLayerNum;
	int m_UpLayerNum;

	// quad strip Number
	int m_LayerNum;
	// quad 사이즈(앞 벡터 length)
	int m_Length;
	// quad strip 간의 생성 프레임 차
	int m_CreateFrame;
	
	LPDIRECT3DVERTEXBUFFER8 *m_Vert;
	LPDIRECT3DVERTEXBUFFER8 *m_Bubble;

	LPDIRECT3DDEVICE8 m_Device;
	CTexture *m_WaveTex;
	int m_WaveTexNum;
	
	bool m_Pick;

	unsigned long m_BeforeTime;
	unsigned long m_CurrentTime;
	int tcount;
	int kcount;

	float t_value1;
	float t_value2; 
	float t_value3;

	CWaveLine();
	CWaveLine(LPDIRECT3DDEVICE8 );

	~CWaveLine();
	void Render();
	void Update();
	//Insert Point
	void InsertPoint(D3DXVECTOR3 );
	void InsertPoint(float x,float y,float z);
	//Delete 
	void DeletePoint(D3DXVECTOR3 );
	void DeletePoint(float x,float y,float z);
	
	void SetDownLayerNum(int num) {m_DownLayerNum = num;}
	void SetUpLayerNum(int num) {m_UpLayerNum = num;}

	void SetDevice(LPDIRECT3DDEVICE8 de) {m_Device = de; }
	void SetInterpol(float vot);
	void CreateTexture(int num);
	void SetTexture(int num,char *);
	void SetVertexBuffer(int i,int type);
	void SetBubbleBuffer(int i,int type);

	//원본 vertex list 저장
	void StoreList();
	//list 되돌림
	void LoadList();

	void SetNormal(float x,float y,float z) { m_Normal.x = x;m_Normal.y = y;m_Normal.z = z; }
	void SetNormal(D3DXVECTOR3 n) { m_Normal = n;}
	void SetCreateFrame(int cf) {m_CreateFrame = cf;}
	void SetLayerNum(int lnum) { m_LayerNum  = lnum;}
	void SetQuadLength(float l) { m_Length = (int)l;}
	void SetPick(bool b) {m_Pick = b;}

	//wave other info setting
	void SetWavePointSetting(CWavePoint *pt);
	// 보간점 생성
	void CreateInterpolation(int index,int type);
	void CreateVertexBuffer();
	void ResetLayer(int index,int type);
	// true 이면 늘어나기 시작 false 이면 아무 변화 없음
	bool Test_Vector(CWavePoint ,int type);
	void MoveVertex(CWavePoint *);
};


#endif
