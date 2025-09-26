#ifndef __WBFOG_H__
#define __WBFOG_H__

#pragma once

#include "texture.h"
#include "SceneNode.h"
#include "SceneManager.h"
#include "3dmath.h"

#include <d3dx8.h>
#include <time.h>
#include <vector>
#define INIT_NUM 50

//Lod Size
#define LOD_NEAR 2000.0f
#define LOD_FAR 60000.0f

#define ALPHA_MAX 0xffffffff
#define ALPHA_MID 0x80808080

#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

#define FOGTEX "vfog.dds"
//LOD Define
enum LODLEVEL{
	NOT_SCENE,
	NEAR_LOD,
	FAR_LOD
};


using namespace std;

class CFogScene : public CSceneNode{
public:
	CFogScene( );
	CFogScene(int );
	void Create(vector3 );

	// particle's system center
	void SetCenterPos(float x,float y,float z);
	void SetCenterPos(vector3 );
	//particle node pos
	void SetPos();
	void SetPos(int );
	void SetPos(int ,float ,float ,float );

	// 집중도
	void SetCenter(float );
	void Render();

	void SetPick(bool b) {m_bPick = b;}

	// 최저 계수
	void SetFlow(float );
	
	void SetFlow(float ,int );
	// color set
	void SetColor(float ,float ,float ,float );
	void SetColor(float ,float ,float ,float ,int p_index);
	// 세부 컬러(버텍스 갯수)
	void SetVertexColor(float ,float ,float ,float ,int dimension);

	// gravity set
	void SetGravity(float ,float ,float );
	// speed set
	void SetSpeed(float ,float ,float );
	void SetSpeed(float ,float ,float ,int );
	void SetSpeed();
	//bfade set...
	void SetEtc();

	// set height
	void SetHeight(float ,int );
	void SetHeight(float );

	void SetWind(float ,float ,float );
	void SetImpress(vector3 imp);
	void SetImpress(float ,float ,float );
	
	void SetSpeedPower(float );
	void Update(float ,float ,float );
	void ClosePerson(vector3 );
	void ClosePerson(float ,float ,float );

	// parameter resetting.
	void Resetting();
	
	void ChangeVertexBuffer();

	void SetSize(float );
	void SetBlendMode(DWORD ,DWORD );

	void SetRad(float r) { m_Rad = r;}
	void CreateVertexBuffer();
	vector3 GetCenter() {return m_CenterPoint;}
	
	void Setfadeout(float );
	void Setfadein(float );
	void SetBackFadeIn(float f);
	void SetFadeInSpeed(float s) {m_FadeInSpeed = s;}
	void SetFadeOutSpeed(float s) {m_FadeOutSpeed = s;}
	DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
	~CFogScene();

private:
	class POINTVERTEX {
	public:	
		D3DXVECTOR3 v;
	    D3DCOLOR    color;
	
		POINTVERTEX() {
			v.x = v.y = v.z = 0.0f;
			color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
		}
		~POINTVERTEX() {}
	};

	// 파티클 기본 노드 class 
	class ParticleNode{

	public:		
		bool Active; // really life
		float Life; //Alpha value
		float Fade; // Alpha speed
		bool bFade; // start fade y/n
		bool bFadeIn;//fade in y/n
		bool m_bClose;

		// 최대 fadein/out value


		//RGB
		float r[4];
		float g[4];
		float b[4];

		//Position
		float X;
		float Y;
		float Z;
		// Before Position
		float BackX;
		float BackY;
		float BackZ;
		// speed
		float Xi;
		float Yi;
		float Zi;
		// sector position
		int Sector_X;
		int Sector_Y;
		// limit value in y axis
		float Flow;
		ParticleNode() {
			m_bClose = false;

		}
		~ParticleNode() {}
	};
	//render 시 사용되는 vertex 정보 class
	class ParticleVertex{
	
	public:
		float x,y,z;
		DWORD Color;
		float tx,ty;
		ParticleVertex() {
			x = y = z = 0.0f;
			Color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
			tx = ty = 0.0f;
		}
		~ParticleVertex() {}
	};

	// 내부 변수
	float m_FadeOut;
	float m_FadeIn;	
	// fade in speed
	float m_FadeInSpeed;
	float m_FadeOutSpeed;

	// 바람 계수 (평소 운동 속도 조절) 
	float m_Wind[3];
	//center 집중도
	float m_Center;
	//가중치
	vector3 m_Impress;


	// speed vector = direction vector * Speed_Power
	float m_SpeedPower;
	//중력
	float m_Gravity[3];
	//speed
	float m_Speed[3];
	
	int m_ParticleNum;
	int m_ClosePointNum;

	int m_LodLevel;
	int m_LodNum;
	// particle size
	float m_Size;
	
	DWORD m_SrcBlendMode;
	DWORD m_DstBlendMode;


	// fog system center.
	vector3 m_CenterPoint;
	ParticleVertex *m_pVertex;
	POINTVERTEX *m_pPoint;

	ParticleNode *m_Node;
	//LPDIRECT3DTEXTURE8 *m_TextureId;
	CTexture *m_BaseTexture;
	int m_TextureNum;
	LPDIRECT3DDEVICE8 m_Device;
	LPDIRECT3DVERTEXBUFFER8 m_FogBuffer;
	LPDIRECT3DVERTEXBUFFER8 m_FogPointSpriteBuffer;
	float m_BackFadeIn;
	float m_Rad;
	// picking 시 쓰이는 것
	bool m_bPick;
	int m_Lod;
};

class CVfogArg{	
	public:
		float	m_FadeIn;
		float	m_FadeOut;
		float	m_FadeInSpeed;
		float	m_A1;
		float	m_A2;
		float	m_A3;
		float	m_A4;
		float	m_B1;
		float	m_B2;
		float	m_B3;
		float	m_B4;
		int		m_Center;
		int		m_Num;	
		float	m_G1;
		float	m_G2;
		float	m_G3;
		float	m_G4;
		float	m_Gx;
		float	m_Gy;
		float	m_Gz;
		float	m_ImX;
		float	m_ImY;
		float	m_ImZ;
		float	m_R1;
		float	m_R2;
		float	m_R3;
		float	m_R4;
		float	m_Rad;
		int		m_Size;
};

#endif
