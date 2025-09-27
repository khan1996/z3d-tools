#ifndef __CLIGHTNING_H__
#define __CLIGHTNING_H__
#include <d3dx8.h>
#include <d3d8.h>
#include <time.h>
#include <vector>

#include "texture.h"
#include "BaseDataDefine.h"


#define LIGHTHALFWIDTH 10.0f 
// lightning 을 표현 하는 plane 방식 
enum PLANE {
	LPLANE_ONE, // one plane
	LPLANE_CROSS, //십자 plane
	LPLANE_X,	// X자 plane
};
enum LIGHTNINGVALUE {	// 어떤종류 인지 : 0 라이트닝, 궤적, 1 베지어 곡선 
	LV_LIGHTNING,
	LV_BEZIER,

};

using namespace std;

class CLightning {
public:
	class CLightVertex {
	public:	
		float x,y,z;
		DWORD color;
		float s,t;
		CLightVertex() {
			x = y = z = 0.0f;
			color = D3DCOLOR_ARGB(255,255,255,255);
			s = t = 0.0f;
		}
		~CLightVertex() {}

	};
	class CBezierInfo {				// Bezier Line 관련 Data
	public:
		float m_fControl;
		float m_fControlHeight;
		float m_fSpeed;	
		float m_fAccel;
		float m_fStartEndLength;
		float m_fFade;
		float m_fLimitLength;
		float m_ft;
		int m_iAxis;
		int m_iEndSpeed;
		int m_iEndAccel;
		int m_iUvMode;	
	
		CBezierInfo() {
					
			m_fControl = 0.5;
			m_fControlHeight = 30.0f;
			m_fSpeed = 0.0f;	
			m_fAccel = 0.0f;
			m_fStartEndLength = 0.0f;
			m_fFade = 0.0f;
			m_fLimitLength = 0.0f;
			m_ft = 0.05f;
			m_iAxis = 0;
			
			m_iEndSpeed = 0;
			m_iEndAccel = 0;
			m_iUvMode = 0;


		}

	};
	// Bezier Point List
	vector<D3DXVECTOR3> m_BezierPointList;

	// 기준점 및 렌덤점 포인터
	D3DXVECTOR3 *m_PointList;
	int m_EndCount;

	int m_PointNum;
	// 한 퀴드의 width
	float m_QuadHalfWidth;
	// insert 된 point
	int m_InsertCount;
	// render 된 count
	int m_RenderCount;
	int m_MaxRenderCount;
	
	LPDIRECT3DDEVICE8 m_Device;
	//가로 플랜 
	LPDIRECT3DVERTEXBUFFER8 *m_Vert;
	// 세로 플랜
	LPDIRECT3DVERTEXBUFFER8 *m_VerticalVert;
	// 세로 플랜 2
	LPDIRECT3DVERTEXBUFFER8 *m_VerticalVert2;	
	int m_VertNum;
	CTexture *m_Texture;
	int m_TexNum;
	//shake value 최소치 
	float m_ShakeMin;
	//shake value 최대치
	float m_ShakeMax;

	DWORD m_Color;
	float m_Alpha;

	bool m_bFadeOut;
	float m_FadeSpeed;
	// uv ani 를 위한 증가치
	float m_UvAni;
	// 업데이트시 증가되는 uv 단위양
	float m_UnitUv;
	// 십자, 1자 플랜 인지 결정
	int m_PlaneValue;
	// 이팩트 end setting
	bool m_bSetEndEffect;
	// 줄어드는 속도
	int m_EndUnit;
	
	int m_LightningValue;

	// Bezier Value
	CBezierInfo m_Bezier;
	bool m_bCrash;				// 충돌한 light beam 인가?
	bool m_bEndInput;

	int m_iAlpha;				// 충돌한 light beam alpha value		
	
	CLightVertex *tmp_Vertex;
	CLightVertex *tmp_Vertex2;
	CLightVertex *tmp_Vertex3;


	CLightning(LPDIRECT3DDEVICE8 );
	CLightning(int ,LPDIRECT3DDEVICE8 );
	CLightning();
	
	~CLightning();
	//wheel이 세팅되어 있는 esf 에서 wheel 이 더이상 업데이트 되지 않을때 setting 
	// render2에 영향을 줌
	void SetEffectEnd(bool s,int end) {m_bSetEndEffect =  s; m_EndUnit = end;}

	void SetUnitUv(float s) {m_UnitUv = s;}

	void SetSize(float s) {m_QuadHalfWidth = s;}

	void SetDevice(LPDIRECT3DDEVICE8 device) {m_Device = device;}
	
	void SetShakeValue(float smin,float smax) {m_ShakeMin = smin;
												m_ShakeMax = smax;}

	void CreateList(int );
	void DeleteList();

	void InsertPoint(float ,float ,float );
	void InsertPoint(D3DXVECTOR3 );
	void InsertEmptyPoint(float ,float ,float );
	void SetStartPoint(float ,float ,float );
	void SetVertNum(int n) {m_VertNum = n;}
	void SetFadeStart(bool t) {m_bFadeOut = t;}
	void SetFadeSpeed(float s) { m_FadeSpeed = s;}
	bool GetEnd();
	void SetEnd();
	void SetEndPoint(float ,float ,float );
	void SetRandPoint();
	bool SetBezierPoint();					//Bezier Curve Point Setting 
	
	void SetColor(DWORD c) {m_Color = c;}
	void SetColor();
	void SetPlaneValue(int k) { m_PlaneValue = k;}

	void CreateTexture(int );
	void SetTexture(int ,char *);

	void CreateVertexBuffer();
	void SetVertexBuffer(int ,int wheelvalue = 0 );
	void SetBezierBuffer(int ,int wheelvalue = 0 );

	void Render(int blendmode);
	void SetCrash(bool b) { m_bCrash = b;}	// Crash Check Setting Func 

	// wheel (궤적 그리는 루틴)
	void Render2(int blendmode);

	bool UpdateLightning(D3DXVECTOR3 &pos,bool bCrash);

	void SetLightningValue(int t) { m_LightningValue = t;}
	void SetBezierControl(float control,float height,float speed,float accel,float length,
							float fade,float limitlength,int axis,int endspeed,int endaccel,int uv);

	D3DXVECTOR3 GetBezierLastPos();



};


#endif
