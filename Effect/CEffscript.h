#ifndef __EFFSCRIPT_H__
#define __EFFSCRIPT_H__

#include <stdio.h>
#include <d3dx8.h>
#include <vector>

#include "CEffscriptDefine.h"
#include "X3DEffect.h"
#include "SectorEffectMap.h"
#include "Z3DGeneralChrModel.h"
#include "CLightning.h"
#include "MemoryPool.h"

using namespace std;

typedef vector<CX3DEffect *> EffList;

class RYLCreature;
	
class CEffScript 
{
public:
	
	// Pool 사용시에 메모리 해제가 제대로 안되고 있음.. by Vincent
//	static CFixedPool ms_myPool;
//	inline void* operator new (size_t nSize)	{ return ms_myPool.ALLOC(nSize);				}
//	inline void operator delete (void* pVoid)	{ ms_myPool.FREE(pVoid, sizeof(CEffScript));	}

	// Vertex Light 적용 eff 
	class CEffLight 
	{					
	public:

		char m_strEff[ESBUF_SIZE];

		CEffLight() 
		{
			memset(m_strEff,0,sizeof(char) * ESBUF_SIZE);	
		}

	};

	// 검기 제어 관련 Class
	class CEffWeaponLine
	{				
	public:

		char		m_strTextureName[ESBUF_SIZE];
		CTexture	*m_pLineTexture;	// Texture
		int			m_iColor[3];		// Color
		int			m_iBlendMode;		// Blend Mode

		CEffWeaponLine() 
		{
			memset(m_strTextureName,0,sizeof(char) * ESBUF_SIZE); 
			m_pLineTexture = NULL;
			m_iColor[0] = m_iColor[1] = m_iColor[2] = 255;
			m_iBlendMode = 0;

		}

		~CEffWeaponLine()
		{
			if (m_pLineTexture != NULL) 
			{
				delete m_pLineTexture;
				m_pLineTexture = NULL;				
			}
		}
	};

	// 캐릭터에 붙는 이팩트 
	class CEffChar 
	{					
	public:

		char m_PivotName[ESBUF_SIZE];
		D3DXVECTOR3 vecPos;				// pivot에서의 상대 좌표
		D3DXQUATERNION vecQuat;			// Quaternion
		bool m_bPosUpdate;
		D3DXVECTOR3 m_vecPosBackup;		// 초기 position backup
		bool m_bRotUpdate;
		D3DXQUATERNION m_RotBackup;		// 초기 rotation quaternion backup
		bool m_bFirstUpdate;

		CEffChar()
		{
			memset(m_PivotName,0,sizeof(char) * ESBUF_SIZE);
			m_vecPosBackup = vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_RotBackup = vecQuat = D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f);
			m_bPosUpdate = true;
			m_bRotUpdate = true;
			m_bFirstUpdate = false;
		}
	};

	// Bezier strip class
	class CEffBezier 
	{					
	public:

		int m_iAxis;					// Axis value
		char m_strTexture[ESBUF_SIZE];	// Texture Name
		float m_fUv;					// Ani Uv 단위
		float m_fFadeOut;				// Fade Out 단위
		float m_fPlaneSize;				// Plane Size

		float m_fIncreaseSpeed;			// 앞이 늘어나는 속도(비율로 한다)
		float m_fIncreaseAccel;			// 앞이 늘어나는 가속도(비율로 한다)		
		float m_fLimitLength;			// 최고로 늘어날 수 있는 사이즈 
		float m_fStartEndLength;		// 시작점에서 끝점까지의 길이
		int m_nBezierPoint;				// 총 제어점의 갯수 Setting		

		float m_fBaseLength;			// 처음 - 끝 점 사이에서의 기준점의 위치
		float m_fBaseHeight;			// 기준점에서 노멀 백터를 쏠때의 그 수직길이
		unsigned long m_lTick;			// Tick Frame
		int m_iEndSpeed;				// 뒷점(꼬리)의 speed
		int m_iEndAccel;				// 뒷점(꼬리)의 accel
		int m_iUvMode;					// Uv mode

		CLightning *m_pBezierLine;		// Bezier Line의 ptr
		
		CEffBezier() 
		{
			m_iAxis = 0;
			memset(m_strTexture,0,sizeof(char) * ESBUF_SIZE);
			m_fUv = 0.0f;
			m_fFadeOut = 0.1f;
			m_fIncreaseSpeed = 0.03f;
			m_fIncreaseAccel = 0.1f;
			m_fPlaneSize = 10.0f;

			m_fLimitLength = 100.0f;
			m_fStartEndLength = 0.0f;
			m_nBezierPoint = 0;

			m_fBaseLength = 0.5f;
			m_fBaseHeight = 30.0f;
			m_pBezierLine = NULL;
			m_lTick = 0;
			m_iEndSpeed = 0;
			m_iEndAccel = 0;
			m_iUvMode = 0;

		}

		~CEffBezier() 
		{
			if (m_pBezierLine != NULL) 
			{
				delete m_pBezierLine;
				m_pBezierLine = NULL;
			}
		}
	};

	// weapon effect class
	class CEffWeapon 
	{	
	public:

		int m_EffNum;
		char m_EffName[ESBUF_SIZE];

		CEffWeapon() 
		{
			m_EffNum = 0;
			memset(m_EffName,0,sizeof(char) * ESBUF_SIZE);
		}
	};

	class CEffWheel 
	{
	public:

		char m_TexName[ESBUF_SIZE];
		float m_PlaneSize;
		
		CLightning m_WheelBody;
		int m_PointNum;
		
		float m_UnitUv;
		int m_BlendMode; // blendmode	
		int m_PlaneValue; // 1자 or 10자 플랜

		CEffWheel() 
		{
			memset(m_TexName,0,sizeof(char) * 256);
			m_PlaneSize = 10.0f;
			m_PointNum = 0;
			m_UnitUv = 0.0f;
			m_BlendMode = 0; // blendmode
			m_PlaneValue = 0;

		}

		~CEffWheel() {}
	};

	// Lightning 관련 class
	class CEffLightn 
	{ 
	public:

		int m_LnNum;					// lightning 갯수
		char m_TexName[ESBUF_SIZE];		// 사용되는 texture 이름
		float m_PlaneSize;				// plane size
		DWORD m_Color;					// color
		float m_ShakeMin;				// shaking value min(얼마나 흔들릴지)
		float m_ShakeMax;				// shaking value max(얼마나 흔들릴지)
		unsigned long m_UpdateFrame;	// 흘들림이 업데이트 되는 프레임
		bool m_Follow;					// 타겟팅을 따라갈지 안갈지
		int m_StartEffIndex;			// start eff name
		unsigned int m_StartFrame;		// start frame
		int m_EndEffIndex;				// end eff name
		unsigned int m_EndFrame;		// end frame
		CLightning *m_Lightn;
		bool m_bLightnStart;			// lightning 이 시작되었는지 flag
		bool m_bLightnEnd;				// lightning end flag
		int m_LPointNum;				// 분기점 갯수
		unsigned long m_StartTime;
		float m_UnitUv;
		D3DXVECTOR3 m_Pos;
		
		int m_BlendMode;				// blendmode
		float m_RandSize;				// random size
		int m_PlaneValue;				// 1자 or 10자 플랜

		CEffLightn() 
		{
			m_LnNum  = 0;
			memset(m_TexName,0,sizeof(char) * ESBUF_SIZE);
			m_PlaneSize = 10.0f;
			m_Color = D3DCOLOR_ARGB(255,255,255,255);
			m_ShakeMin = 0.0f;
			m_ShakeMax = 10.0f;

			m_UpdateFrame = 30;
			m_Follow = true;
			m_StartFrame = 0;
			m_EndFrame = 0;
			m_StartEffIndex = ESEMPTY;
			m_EndEffIndex = ESEMPTY;
			m_Lightn = NULL;
			m_bLightnStart = false;
			m_bLightnEnd = false;
			m_LPointNum = 0;
			m_StartTime = 0.0;
			m_UnitUv = 0.0f;
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			
			m_BlendMode = 0; // blendmode
			m_RandSize = 0.0f; // random size
			m_PlaneValue = 0;

		}

		~CEffLightn()
		{
			if (m_Lightn != NULL) 
			{
				delete[] m_Lightn;
				m_Lightn = NULL;
			}
	
		}
	};

	class CEffTextureDef 
	{
	public:

		char m_strName[ESBUF_SIZE];

		CEffTextureDef() 
		{
			memset(m_strName,0,sizeof(char) * ESBUF_SIZE);
		}
	};

	// 원형 방사 라이트닝
	class CEffLightn2 
	{					
	public:

		int		m_iMaxLine;				// 최대 라인 갯수
		float	m_fUvAniMax;			// uv ani 최대 단위	
		float	m_fRange;				// 범위
		int		m_iBlend;				// 블랜드 모드
		float	m_fMinSize;				// Min Plane Size
		float	m_fMaxSize;				// Max Plane Size

		CEffLightn *m_pLightn;
		CEffTextureDef *m_pTextureName;	// Texture Name

		int m_iStartEff;
		int m_iStartFrame;
		
		int m_iEndEff;
		int m_iEndFrame;
		
		bool m_bStart;
		bool m_bEnd;
		
		D3DXVECTOR3 m_vecPos;

		unsigned long m_StartTime;

		CEffLightn2() 
		{
			m_iMaxLine = 0;
			m_fUvAniMax = 0.5f;
			m_fRange = 0.0f;
			m_pLightn = NULL;	
			m_pTextureName = NULL;
			
			m_iStartEff = ESEMPTY;
			m_iStartFrame = ESEMPTY;
		
			m_iEndEff = ESEMPTY;
			m_iEndFrame = ESEMPTY;
			m_bStart = false;
			m_bEnd = false;
			m_StartTime = 0;
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_iBlend = 0;
			m_fMinSize = 0.0f;
			m_fMaxSize = 0.0f;
		}

		~CEffLightn2() 
		{
			if (m_pLightn) 
			{
				delete[] m_pLightn;
				m_pLightn = NULL;
			}
			if (m_pTextureName) 
			{
				delete[] m_pTextureName;
				m_pTextureName = NULL;
			}
		}
	};

	// 더미에서 더미로의 라이트닝
	class CEffLightn3 
	{					
	public:

		int		m_iEffChar;
		int		m_iMaxLine;				// 최대 라인 갯수
		float	m_fUvAniMax;			// uv ani 최대 단위	
		float	m_fRange;				// 범위
		int		m_iBlend;				// 블랜드 모드
		float	m_fMinSize;				// Min Plane Size
		float	m_fMaxSize;				// Max Plane Size

		CEffLightn	*m_pLightn;
		CEffChar	*m_pEffChar;
		CEffTextureDef *m_pTextureName;	// Texture Name

		bool m_bStart;
		bool m_bEnd;
		
		unsigned long m_StartTime;

		int m_iStartEff;
		int m_iStartFrame;
		int m_iEndEff;
		int m_iEndFrame;

		CEffLightn3() 
		{
			m_iMaxLine = 0;
			m_fUvAniMax = 0.5f;
			m_fRange = 0.0f;
			m_pLightn = NULL;	
			m_pTextureName = NULL;
			
			m_iStartEff = ESEMPTY;
			m_iStartFrame = ESEMPTY;
		
			m_iEndEff = ESEMPTY;
			m_iEndFrame = ESEMPTY;
			m_bStart = false;
			m_bEnd = false;
			m_StartTime = 0;
		
			m_iBlend = 0;
			m_fMinSize = 0.0f;
			m_fMaxSize = 0.0f;

			m_iEffChar = 0;
			m_pEffChar = NULL;
		}

		~CEffLightn3() 
		{
			if (m_pLightn) 
			{
				delete[] m_pLightn;
				m_pLightn = NULL;
			}
			if (m_pTextureName) 
			{
				delete[] m_pTextureName;
				m_pTextureName = NULL;
			}
			if (m_pEffChar != NULL) 
			{
				delete[] m_pEffChar;
				m_pEffChar = NULL;
			}
		}
	};

	// extension 6 :: 캐릭터 미끌림 
	class CEffExt6
	{ 
	public:

		int m_SlideStartFrame;		// 슬라이드 시작 프레임 
		float m_SlideSpeed; 		// 속도 
		float m_SlideAccel; 		// 가속도
		float m_SlideLength;		// 최대 미끌린 거리 세팅
		float m_LimitSpeed;			// 한계 스피드
		float m_CurrentSlideLength; // 현제 까지 미끌린 거리

		CEffExt6() 
		{	
			m_SlideStartFrame = ESEMPTY;
			m_SlideSpeed = 0.0f;
			m_SlideAccel = 0.0f;
			m_SlideLength = 0.0f;
			m_LimitSpeed = 0.0f;
			m_CurrentSlideLength = 0.0f;
		}

		~CEffExt6() {}
	};

	// extension 5용 plane을 구성하는 vertex class
	class CEffExt5Vertex
	{ 
	public:

		float x,y,z;
		DWORD color;
		float s,t;
		CEffExt5Vertex() 
		{
			x = y = z = 0.0f;
			color = D3DCOLOR_ARGB(255,255,255,255);
			s = t = 0.0f;
		}

		~CEffExt5Vertex() {}
	};

	// extension 5 :: 관련 data 관리 class 
	class CEffExt5
	{ 
	public:
	
		// start, end point
		D3DXVECTOR3 m_PlaneStartPos;
		D3DXVECTOR3 m_PlaneEndPos;
		// 이 라인의 생명이 다했는지(메모리에서 삭제 될지).
		bool m_bExt5End;
		// value setting 유무 플레그
		bool m_bExt5ValueSet;
		CTexture m_PlaneTexture;
		// 십자 플렌
		CEffExt5Vertex m_Plane[8];
		//각 퀴퉁이의 알파
		float m_PlaneAlpha[4];
		D3DXVECTOR3 m_RandValue;
		
		unsigned long m_UpdateTime;
		//확장
	
		char m_TextureName[ESBUF_SIZE];
		float m_LineSize;
		bool m_Rand;

		CEffExt5() 
		{
			m_bExt5End = false;
			m_bExt5ValueSet = false;
			m_PlaneStartPos.x = m_PlaneStartPos.y = m_PlaneStartPos.z = 0.0f;
			m_PlaneEndPos.x = m_PlaneEndPos.y = m_PlaneEndPos.z = 0.0f;	
			for(int i=0;i<4;i++) 
			{
				m_PlaneAlpha[i] = 0.7f;
			}
			m_UpdateTime = 0;
			m_RandValue.x = m_RandValue.y = m_RandValue.z  = 0.0f;
			
			memset(m_TextureName,0,sizeof(char) * ESBUF_SIZE);
			m_LineSize = 1.0f;
			m_Rand = false;	
		}

		~CEffExt5() {}
	};

	class CEffSound {
	public:
		int m_StartFrame;
		char m_Name[ESBUF_SIZE];
		//3d sound distance
		float m_Min;
		float m_Max;
		int m_Target;
		bool m_bPlay;
		CEffSound() {
			m_StartFrame = ESEMPTY;
			memset(m_Name,0,sizeof(char) * ESBUF_SIZE);
			m_bPlay = false;
			m_Min = 1.0f;
			m_Max = 10.0f;
			m_Target = ES;
		}
		~CEffSound() { }
	};

	class CEffSnap{ //snap 관련 struct (아직 까지는 이용하고 있지 않다)
	public:
		int m_SnapStartFrame; 
		char m_PivotName[ESBUF_SIZE];
		char m_EffName[ESBUF_SIZE];
		int m_Loop;
		CEffSnap() {
			m_SnapStartFrame = ESEMPTY;
			memset(m_PivotName,0,sizeof(char) * ESBUF_SIZE);
			memset(m_EffName,0,sizeof(char) * ESBUF_SIZE);
			m_Loop = 0;
			
		}
		~CEffSnap() {}
	};

	class CEffShake {
	public:
		float m_length[3];
		float m_ShakeTime;
		float m_StartTime;
		bool m_Action;
		CEffShake() {
			m_length[0] = m_length[1] = m_length[2] = ESEMPTY;
			m_ShakeTime = 0.0f;
			m_StartTime = 0.0f;
			m_Action = false;
		}
		~CEffShake() {}
	};
	class CEffInfo { // EFf 관련 data info
	public:
		bool m_bFogOff;
		bool m_LoopAction;
		char m_EffName[ESBUF_SIZE];
		float m_Scale;

		// 시작 프레임 setting 기본은 0
		int m_MinFrame;
		// start rotation
		D3DXVECTOR3 m_StartRot;
		bool m_bStartRot;
		//fade in
		int *m_FadeInStart;
		D3DXVECTOR3 *m_FadeIn;
		color *m_FadeColor;	
		int m_FadeInNum;
		//CLight
		int *m_CLightStart;
		color *m_CLightColor;
		float *m_CLightDistance;
		DWORD *m_CLightTime;
		int m_CLightNum;
		bool *m_CLightSwitch;
		int *m_CLightTarget;

		bool *m_FadeInSwitch;
		float m_SpeedValue[3];
		float m_AccelValue[3];
		float m_StartPos[3];

		int m_LoopStart;
		int m_LoopEnd;
		int m_LoopSwitch;
		// camera shaking value
		CEffShake *m_Shake;
		int m_ShakeNum;
		// 히트 이벤트 return 프레임 
		int *m_HitFrame;
		int m_HitFrameNum;
		int m_HitFrameCount;
		// sound value
		CEffSound *m_Sound;
		int m_SoundNum;
		// snap value
		CEffSnap *m_Snap;
		int m_SnapNum;
		int m_SnapCount;

		// 뒤로 미끌림 value
		CEffExt6 *m_Ext6Value;
		int m_Ext6ValueNum; // 미끌림 num
		int m_Ext6ValueCount; // 현제 까지 실행된 count 
		
		bool m_bRotStart;
		bool m_bLight;		// Light y/n
		CEffInfo() {
			

			m_HitFrame = NULL;
			m_HitFrameNum = 0;
			m_HitFrameCount = 0;
			//clight
	
			m_CLightSwitch = NULL;
			m_CLightStart = NULL;
			m_CLightColor = NULL;
			m_CLightDistance = NULL;
			m_CLightTime = NULL;
			m_CLightNum = 0;
			m_CLightTarget = NULL;

			m_LoopAction = false;
			m_Sound = NULL;
			m_SoundNum = 0;
			m_Shake = NULL;
			m_ShakeNum = 0;
			m_FadeInStart = NULL;
			m_FadeIn = NULL;
			m_FadeColor = NULL;	
			// rot 시작했는지
			m_bRotStart = false;
			m_FadeInSwitch = NULL;

			memset(m_EffName,0,sizeof(char) * ESBUF_SIZE);
			m_LoopStart = ESEMPTY;
			m_LoopEnd = ESEMPTY;
			m_LoopSwitch = ESEMPTY;
			m_MinFrame = ESEMPTY;

			m_Scale = 1.0f;
			m_FadeInNum = 0;
			m_SpeedValue[0] = m_SpeedValue[1] = m_SpeedValue[2] = 0.0f;
			m_AccelValue[0] = m_AccelValue[1] = m_AccelValue[2] = 0.0f;
			m_StartPos[0] = m_StartPos[1] = m_StartPos[2] = 0.0f;

			m_StartRot.x = m_StartRot.y = m_StartRot.z = 0.0f;
			m_bStartRot = false;
			m_Ext6Value = NULL;
			m_Ext6ValueNum = 0;
			m_Ext6ValueCount = 0;
			m_Snap = NULL;
			m_SnapNum = 0;
			m_SnapCount = 0;
			m_bLight = false;
			m_bFogOff = false;
			

		}
		~CEffInfo() {
			if (m_FadeInNum >0) {
				if (m_FadeInStart != NULL) {
					delete[] m_FadeInStart;
					m_FadeInStart = NULL;
				}
				if (m_FadeIn != NULL) {
					delete[] m_FadeIn;
					m_FadeIn = NULL;
				}
				if (m_FadeColor != NULL) {
					delete[] m_FadeColor;
					m_FadeColor = NULL;
				}
				if (m_FadeInSwitch != NULL) {
					delete[] m_FadeInSwitch;
					m_FadeInSwitch = NULL;
				}
			}
			if (m_Shake) {
				delete[] m_Shake;
				m_Shake = NULL;
			}
			if (m_Sound) {
				delete[] m_Sound;
				m_Sound = NULL;
			}
			if (m_CLightStart) {
				delete[] m_CLightStart;
				m_CLightStart = NULL;
			}
			if (m_CLightColor) {
				delete[] m_CLightColor;
				m_CLightColor = NULL;
			}
			if (m_CLightDistance) {
				delete[] m_CLightDistance;
				m_CLightDistance = NULL;
			}
			if (m_CLightTime) {
				delete[] m_CLightTime;
				m_CLightTime = NULL;
			}
			if (m_CLightSwitch) {
				delete[] m_CLightSwitch;
				m_CLightSwitch = NULL;
			}
			if (m_CLightTarget) {
				delete[] m_CLightTarget;
				m_CLightTarget = NULL;
			}
			if (m_HitFrame) {
				delete[] m_HitFrame;
				m_HitFrame = NULL;
			}
			if (m_Ext6Value) {
				delete[] m_Ext6Value;
				m_Ext6Value = NULL;
			}
			if (m_Snap) {
				delete[] m_Snap;
				m_Snap = NULL;
			}
		}

	};


	class CEffPlay { // script play 관련 data
	public:
		int *m_StartId;
		int m_StartIdNum;
		int m_StartCount;

		int m_StartFrameNextEff;
		float m_EndPos[3];

		int *m_MidEffId;
		int m_MidEffIdNum;
		int m_MidCount;

		float m_Speed;
		float m_Accel;
		float m_MaxSpeed;
		float m_MinSpeed;

		// effect 궤적 방향 index
		int m_Direct;
		int m_MidEffNum;
		int m_StartMidNextEff;
		
		int *m_EndEffId;
		int m_EndEffIdNum;
		int m_EndCount;
		
		//CEffMidValue *m_MidValue;
		//int m_MidValueNum;

		CEffSnap *m_SnapEff;
		int m_SnapNum;
		
		CEffPlay() {
		//	m_MidValue = NULL;
		//	m_MidValueNum  =0;
			m_Speed = 0.0f;
			m_Accel = 0.0f;

			m_StartId = NULL;
			m_StartIdNum = 0;
			m_StartCount = 0;
			
			m_StartFrameNextEff = ESEMPTY;
			m_EndPos[0] = 0.0f;
			m_EndPos[1] = 0.0f;
			m_EndPos[2] = 0.0f;
			
			m_MidEffId = NULL;
			m_MidEffIdNum = 0;

			m_MidCount = 0;

			m_MaxSpeed = 0.0f;
			m_MinSpeed = 0.0f;

			m_Direct = ESEMPTY;
			m_MidEffNum = ESEMPTY;
			
			m_StartMidNextEff = ESEMPTY;

			m_EndEffId = NULL;
			m_EndEffIdNum = 0;
			m_EndCount = 0;

			m_SnapNum = 0;	
			m_SnapEff = NULL;
		}
		~CEffPlay() {
		//	if (m_MidValue)
		//		delete[] m_MidValue;
			if (m_StartId)
				delete[] m_StartId;
			if (m_MidEffId)
				delete[] m_MidEffId;
			if (m_EndEffId)
				delete[] m_EndEffId;
			if (m_SnapEff) 
				delete[] m_SnapEff;
		}
	};


	FILE *m_ScriptFile;
	char m_FileName[ESBUF_SIZE];

	EffList m_lstEff;

	//character model data
	CZ3DGeneralChrModel *m_StartChr;
	CZ3DGeneralChrModel *m_EndChr;
	float m_EndChrChest; // end chracter의 체스트 높이

	// 스크립트 종류(Zenable 킬것인지 끌것인지..)
	int m_ScriptValue;

	CEffInfo *m_EffList;

	// script 분기 처리
	int m_Weapone;
	int m_Sex;

	unsigned long m_MidShootFrame;
	unsigned long m_MidMoveFrame;
	float m_InterfacePos[2];
	bool m_bInterfacePos;

	CEffPlay m_EffPlayData;
	//start pos setting 했는지..
	bool m_StartPosSet;
	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_MidPos;
	D3DXVECTOR3 m_MidUnit;
	D3DXVECTOR3 m_MidRot;
	bool m_bMidFwd;

	bool m_NotRot;

	D3DXVECTOR3 m_OldMidPos;
	// 캐릭터 앞 벡터
	D3DXVECTOR3 m_ChrFwd;

	float m_MidSpeed;	// 운동중인 mid effect의 속도
	
	D3DXVECTOR3 *m_EndPos;
	//interface effect projection vector
	D3DXVECTOR3 m_Projection;

	LPDIRECT3DDEVICE8 m_Device;
	matrix m_View;
	
	unsigned long m_dwTick;
	unsigned long m_dwOldTick;

	int m_EndPosNum;
	//현제 스크립트 진행도 표시	
	int m_ProcCount;
	// effect 발동 시간(delay time)
	unsigned long m_DelayFrame;
	unsigned long m_TickFrame;

	int m_EffNum;
	int m_CurrentNum;
	//fade out value
	float m_FadeOut[3];
	bool m_FadeOutStart;
	// 누가 effect를 사용한건지
	bool m_Mine;
	// skill effect 인지 
	bool m_bSkill;
	// sub script 이용시
	CEffScript *m_SubScript;
	bool m_SubFirstLoad; // sub 이용한 effect 처음 수행시 미리 sub의 모든 내용 로딩 해놓는다
	int m_SubScriptNum;
	bool m_FileLoad;
	int m_CurrentSubScript;

	int *m_NextScript;
	// 충돌시 true setting
	bool m_CheckCrash;
	bool m_bInterfaceSet;
	
	float m_MulPos;

	unsigned long DelayTick;
	bool DelayStart;
	int DelayFrame;
	int del_count;

	bool m_bWorldEffect;
	float m_WorldAxis[3];

	bool m_bExt1; 				// mid effect가 자기 생명 끝까지 유지
	bool m_bExt2; 				// start effect가 캐릭터 회전에 따라 끝까지 유지
	bool m_bExt3; 				// extension3 aimed shot 용 particle 유지.
	
	bool m_bExt4; 				// extension 4를 위한 flag
	CEffScript *m_Ext4Ptr;		// extension 4 setting 이 적용될 스크립트 ptr
	int m_Ext4Pivot;			// 적용 pivot
	D3DXQUATERNION m_Ext4Quat;	// extension 4 용 quaternion
	D3DXQUATERNION m_Ext4Object;

	bool m_bExt4InfoSet;		// extension4 data setting 되었는지 flag
	bool m_bExt4First;

	bool m_bExt5;				// extension5 flag
	bool m_bExt5ex;				// 확장 유무
	CEffExt5 *m_Ext5Value;
	
	bool m_bExt7; 				// extension 7용 flag : 내부적으로 start와 end position change
	bool m_bExt8; 				// extension 8용 flag : 적용되는 캐릭터의 scale에 따라 effect 크기 적용
	bool m_bExt9; 				// extension 9용 flag : snap effect 가  이 esf 가 해제된 이후에도 메모리에서 삭제 되지 않는다
	bool m_bExt1Crash; 			// extension 1 사용시 mid effect 충돌
	bool m_bExt3Crash; 			// extension 3 사용시 mid effect 충돌

	bool m_bLightning;			// LIGHTNING flag
	
	D3DXVECTOR3 oldfwd_vec;	
	D3DXVECTOR3 fwd_vec;
	D3DXVECTOR3 pos_vec;
	bool m_bCast;				// casting effect 인지
	bool m_bPosSet;				// pos setting 이 적용될지 안될지
	
    RYLCreature*        m_pPlayer;
    unsigned long       m_ulPlayerID;

	CEffLightn *m_Lning;		// lightning
	int m_LningNum;				// lightning number
	bool m_Chant;				// enchent or chent
	
	// 궤적 
	CEffWheel *m_Wheel;
	int m_WheelNum;
	bool m_bWheel;
	bool m_bWheelStart;

	CEffWeapon *m_WeaponEff;
	int m_WeaponEffNum;
	bool m_bWeaponEff;

	bool m_bVisibility;
	
	CEffBezier *m_pBezierMid;	// Middle로 Bezier 스트립을 이용.
	bool m_bBezierMid;
	bool m_bBezierMidSet;

	CEffLightn2 *m_pLightn2; 	// Lightning 2
	bool m_bLightn2;		 	// lightning2 bool 

	CEffLightn3 *m_pLightn3; 	// Lightning 3
	bool m_bLightn3;		 	// Lightning3 bool

	int m_iLight;				// Light 적용되는 eff 갯수
	CEffLight *m_pLight;

	// 시간 개념이 들어 갔을때의 시간 setting
	int m_iStartPlayTime;		// play start time
	int m_iEndPlayTime;			// play end time
	bool m_bPlayTime;			
	bool m_bVisTime;			// 보일 시간인지.
	
	bool m_bSectorCull;			// sector culling

	CEffChar *m_pEffCharacter;
	bool m_bEffCharacter;

	bool m_bFrustumCull;		// Frustum Culling 유/무
	CEffWeaponLine *m_pEffWeaponLine;
	bool m_bEffWeaponLine;

	CEffScript();
	~CEffScript();
	
	bool GetScriptData(char *effect_id);
	bool GetScriptRealBinData(char *bin_name,bool vis = true);
	bool GetScriptBinData(char *bin_name,bool vis = true);
	//chent or enchent 관련 bool setting
	void SetChant(bool t) {m_Chant = t;}
	bool GetChant() {return m_Chant;}

	// world effect setting 
	void SetWorldEffect(bool b) {m_bWorldEffect = b;}
	bool GetWorldEffect() { return m_bWorldEffect;}

	void GetEffNum();
	void GetSnapNum();
	void GetLnNum();
	void GetLn2Num();
	void GetLn3Num();

	void GetBinLightn2();
	
	void GetEffWheelNum();
	void GetEffWeaponNum();
	void GetEffLightNum();
	void GetEffPlayTime();
	void GetEffChar();
	void GetEffWeaponLine();
	void SetDevice(LPDIRECT3DDEVICE8 device);

	// slide character 를 위한 CCreature address setting
    void SetPlayer( RYLCreature* pCreature, unsigned long ulCreatureID )
    {
        if ( pCreature )
        {
            m_pPlayer   = pCreature;
            m_ulPlayerID= ulCreatureID;
        }

        for( int i = 0 ; i < m_SubScriptNum ; ++i ) 
        {
			if ( m_SubScript )
				m_SubScript[ i ].SetPlayer( pCreature, m_ulPlayerID );
		}
    }

	// 캐릭터 모델 세팅
	void SetChr(CZ3DGeneralChrModel *s,CZ3DGeneralChrModel *e);
	
	// chracter fwd vector setting(use start effect)
	void SetChrFwd(float x,float y,float z);

	// interface or normal script
	void SetScriptValue(int n); 

	void SetCast(bool t);
	void SetPosSet(bool t) {m_bPosSet = t;}

	void SetCrash(bool t) {m_CheckCrash = t;}
	bool GetCrash() {return m_CheckCrash;}
	void SetInterfaceSet(bool b) ;
	void GetStartEndNum();
	void GetSubNum();
	void GetExt5ex(char *);
	void GetSubData(char *);
	void GetBinSubData(int index,char *filename);
	void GetEffList(char *);
	void GetEffLoop(char *);
	void GetEffPlay(char *);
	void GetEffSnap(char *);
	void GetFadeOut(char *);
	void GetFadeIn(char *);
	void GetCLight(char *);
	void GetCameraShake(char *);
	void GetSound(char *);
	void GetSlide(char *);
	void GetLightn(char *);
	void GetLightn2(char *);
	void GetLightn3(char *);

	void GetEffWheel(char *);
	void GetEffWeapon(char *);
	void GetEffBezier(char *);
	void GetEffLight(char *);

	void GetHit(char *);

	D3DXQUATERNION GetExt4Quat();
	void WriteConvertFile(char *name);
	//void SetWheelPoint(int index,float x,float y,float z);
	// 누구의 스킬인지 s : 성별 w : 무기 종류
	void SetSkillInfo(int s,int w);
	void SetAllLoopOff();
	
	void SetStartPos(D3DXVECTOR3 );
	void SetStartPos(float x,float y,float z);
	
	void SetMine(bool m);
	bool GetMine() {return m_Mine;}
	// skill effect 일때 setting 
	void SetBSkill(bool m) {m_bSkill = m;}
	bool GetBSkill() { return m_bSkill;}
	void SetRot(D3DXVECTOR3 before,D3DXVECTOR3 now,D3DXVECTOR3 &target);

	void SetMidPos(D3DXVECTOR3 pos) {m_OldMidPos = m_MidPos; m_MidPos = pos;}
	void SetMidPos(float x,float y,float z){m_OldMidPos = m_MidPos;m_MidPos.x = x; m_MidPos.y = y;m_MidPos.z = z;}
	void SetOldMidPos(float x,float y,float z) {m_OldMidPos.x = x;m_OldMidPos.y =y; m_OldMidPos.z = z;}
	void SetMidSpeed(float s) {m_MidSpeed = s;}
	
	void SetEndPos(D3DXVECTOR3 *end,int num);
	void SetEndPos(float ,float ,float );
	void SetEffectRot(float x,float y,float z);
	// 초기 위치에 곱할 값 세팅
	void SetMulPosition(float s){m_MulPos = s;}


	void SetTickFrame(int tick) {m_TickFrame = tick; }
	void PushEff();// script안에 존재하는 effect load
	void PlayFadeIn(int index,int j); //fadein play func
	void PlayCLight(int index,int j);
	void PlayEffSound(int index,int j,float x,float y,float z,float mind,float maxd); //sound play func
	void PlayEffSound(int index,int j);
	void StopEffSound(int index,int j,float x,float y,float z,float mind,float maxd);
	void PlaySlide(int index,int j);
	void PlaySnap(int index,int j);
	
	void PlayTime();				// Play Start End Time


	void PlayLning(int index,int obj_index,float currentframe,D3DXVECTOR3 vecChar);
	void PlayLning2(int index,int obj_index,float currentframe,D3DXVECTOR3 vecChar);
	void PlayLning3(int index,int obj_index,float currentframe,D3DXVECTOR3 vecChar);

	void RenderLightn();
	void RenderLightn2();
	void RenderLightn3();

	void UpdateLning(int index,D3DXVECTOR3 vecChar);
	void UpdateLning2(D3DXVECTOR3 vecChar);
	void UpdateLning3(D3DXVECTOR3 vecChar);

	void PlayWheel();
	void RenderWheel();
	void UpdateWheel(float x = 0.0f,float y = 0.0f,float z = 0.0f,float nx = 0.0f,float ny = 0.0f,float nz = 0.0f,int value = 0);
	
	void PlayWeaponEff();
	void DeleteWeaponEff();

	void PlayWeaponLine();
	void DeleteWeaponLine();
	
	int ProcessEffect(bool bWeatherEffect = true);
	void Render();
	void RenderExt5();

	void SetExt4Ptr(CEffScript *);
	void SetExt4Info(int pivotnum,D3DXQUATERNION rot);
	void SetExt4SelfInfo(D3DXQUATERNION rot); // 자신의 정보 세팅
	void SetExt4Rot(D3DXQUATERNION rot);
	
	void SetVisibility(bool t);
	void SetLight();		// Vertex Light Setting	
	// Bezier Mid's Func
	void SetInitBezier(float startx,float starty,float startz,float endx,float endy,float endz);
	bool GetSectorCull() {return m_bSectorCull;}
	void SetSectorCull(bool b) {m_bSectorCull = b;}

	void SetScale(float fScale);
	void Clone(CEffScript *Clone);
	void GetSubClone(int index,CEffScript *Clone,char *filename);

	// 궤적 strip
	//void RenderStrip(int index);

};


class CEffCacheObj {
public:
	std::vector<CEffScript *>	m_lstDatas;
	int							m_iDatasNum;
	CEffCacheObj() : m_iDatasNum(0)
	{
		m_lstDatas.clear();
	}
	CEffCacheObj(CEffScript *pObj)  : m_iDatasNum(0)
	{
		m_lstDatas.clear();
		m_lstDatas.push_back(pObj);
		m_iDatasNum++;
	}
	void Add_Back(CEffScript *pObj)
	{
		m_lstDatas.push_back(pObj);
		m_iDatasNum++;
	}
	~CEffCacheObj() {
		if (m_iDatasNum > 0)
		{
			for(int i=0;i < m_iDatasNum; i++) 
			{
				if (m_lstDatas[i]) 
				{
					if (m_lstDatas[i])
					{
						delete m_lstDatas[i];
					}
					m_lstDatas[i] = NULL;

				}
			}
		}
		m_iDatasNum =  0;
		m_lstDatas.clear();
	}

		
			
};

typedef std::map<unsigned long , CEffCacheObj *> EFFCACHETABLE;
typedef EFFCACHETABLE::value_type EFFCACHETABLEOBJ;
typedef EFFCACHETABLE::iterator EFFCACHEITER;
/* *********************************************************************  

* CBaseCacheMgr

* 파일 : BaseCacheMgr.h 
* 기능 : Caldron엔진 내부에서 사용되는 모든 CacheMgr들의 어미 class
* history : 
	2004.01.16 wizardbug

********************************************************************** */  
const int DEFAULT_EFFMAXCACHENUM = 100;
class CEffectCacheMgr
{
protected:
	
	unsigned long GetHashID(const char *strFileName)
	{
		unsigned long ulHashId = 0;

		int iLength = (int)strlen(strFileName);

		for(int i=0;i < iLength; i++) {
			ulHashId += (( i + 1) * strFileName[i]);
		}	
		return ulHashId;


	}

	EFFCACHETABLE m_CacheTable;
	int	m_iCachesNum;
	bool m_bFirst;
	bool m_bInitLoadScript;


public:
	CEffectCacheMgr(void) : m_iCachesNum(0),m_bFirst(true),m_bInitLoadScript(false)
	{
		m_CacheTable.clear();
	}
	~CEffectCacheMgr(void)
	{
		if (m_iCachesNum > 0) 
		{
			for(EFFCACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
			{
				if (Itr->second != NULL) 
				{
					if (Itr->second)
					{
					
						delete (Itr->second);
					}
					Itr->second = NULL;

				}
				
				Itr++;
			}
			m_CacheTable.clear();
			m_iCachesNum = 0;

		}

	}
	CEffScript *GetData(const char *strFileName);
	
	CEffScript *LoadData(const char *strFileName);
	void InitLoad();


};

#endif
