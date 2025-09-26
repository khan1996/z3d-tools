#ifndef __BOID_H__
#define __BOID_H__

#pragma once

#include <d3dx8.h>
#include "CGemRender.h"
#include <time.h>

//#include "datadefine.h"
//#include " SectorDefine.h"

#define BOIDUNLIMIT -100000
#define BOIDMAXLIMIT 10000000

#define SEENDEFAULT 50.0f
#define FEARFULL 1.0f
#define NOTFEAR 0.0f
//separate
#define BOIDRAD 10.0f
//align
#define ALIGN 0.00005f

#define TEST_SECNUM 0
#define TEST_SECSIZE 800
#define BPI        3.14159f
#define BHALF_PI   (BPI/2)
#define BGRAVITY   9.806650f 
// 0~1 사이의 값 return
#define BRAND() (float) (rand()/(RAND_MAX * 1.0))

//change value
#define BMAXSPEED	0.5f
#define BMIN	0.05f
#define BMAX	0.1f
#define BMAXCHANGE	(BMAXSPEED * BMAX)
//#define BDesiredSpeed	(BMAXSPEED/2)

enum SPACE{ LAND, 
			WATER, 
			FLY};
enum PAT{	BOIDGROUP,
				INDIVIDUAL};
// 서브 행동패턴 (회피,휴식,분산,같은방향,마스터 따라가기)
enum SUBPAT{	AVOID,
				RELEASE,
				SPREAD,
				SAMELOOK,
				MASTER
				};

enum LOD{	NOT,
			MIDDLE,
			FAR
		};
enum ACT{	SLEEP,
			PEACE,
			WALK,
			RUN,
			FLYING
		};



class CBoidGroup{
public:
	// 일반 보이드 구조체
	class BoidUnit {
	public:
		BoidUnit() {
			pos.x = pos.y = pos.z = 0.0f;
			before_pos = pos;
			Live = true;
			rot[0] = rot[1] =rot[2] = 0.0f;
			speed = 0.0f;
			look.x = look.y = look.z = 0.0f;
			before_look.x = before_look.y = before_look.z = 0.0f;
			fwd.x = fwd.y =fwd.z = 0.0f;
			before_fwd.x = before_fwd.y = before_fwd.z = 0.0f;
			vot = 1.0f;
			backup_vot = vot;
			backup_frame  = 0.0f;
			active = 0;
			height = -1;
			frame = 0.0f;
			m_fly = 0;
			start_vot = 0.0f;
		}
		~BoidUnit() {

		}
		bool Live;
		float frame;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 before_pos;

		//회전 축
		//	D3DXVECTOR3 rotaxis;
		float rot[3];

		//speed (fwd의 length)
		float speed;

		// 목표 포지션
		D3DXVECTOR3 look;
		D3DXVECTOR3 before_look;

		D3DXVECTOR3 fwd;
		D3DXVECTOR3 before_fwd;
		int height;
		// 운동상태
		int active;
		float vot;
		float backup_vot;
		float backup_frame;

		float start_vot;
		int m_fly;

	};

	CBoidGroup();
	~CBoidGroup();
	// Boid Mesh Name setting
	void SetBoidName(char *filename,char *path);
	char *GetBoidName();
	
	void RandomRot(BoidUnit *, float );

	// boid Update
	// 천적이 있으면 도망가는 Update
	void Update();
	void SetSpace(int s) { Space = s; }
	// 회피 :: 장애물이나 다른 적들과 만나지 않게 회피
	void avoid();
	
	// 충돌 피하기 :: 같은 그룹의 boid 들 끼리 부딛히지 않게.
	void separate(BoidUnit *,D3DXVECTOR3 *);
	// 충돌피하기 :: 집단행동 안하고 있을때
	void separate2(BoidUnit *);
	//지형과의 충돌 피하기
	void HeightSeparate(BoidUnit *,D3DXVECTOR3 *);

	//정렬 :: 같은 그룹의 boid 들과 방향벡터를 같게 만들기
	void align(BoidUnit *,D3DXVECTOR3 *);
	//응집 :: 같은 그룹의 boid 들과 목표 위치를 같게 만들기
	void concenter(BoidUnit *,D3DXVECTOR3 *);
	void randomvec(BoidUnit *,D3DXVECTOR3 *);
	// rot setting
	void RotSet(BoidUnit *);

	// 경계를 벗어난 boid의 방향 벡터 재 설정
	void Bound(BoidUnit *);

	void SetUnit();
	void SetBoidSpeed(float s) {
		Boid_Speed = s;
	}
	void SetBoidRange(float x,float y,float z,float limit) {
		Boid_Range.x = x;
		Boid_Range.y = y;
		Boid_Range.z = z;
		m_LimitHeight = limit;

	}

	// Boid Render failed : 0 return 
	bool Render(LPDIRECT3DDEVICE8 );
	// 운동상태 설정(0 : 평상시 1: 집단행동시)
	void SetPat(int v) {pat = v;}
	int GetPat() { return pat;}
	void SetUnitVot(float vot);
	void SetPos(float x,float y,float z);
	void CreateMaster();
	float GetHeight(float x,float z);

	void SetMesh(CGemRender *mesh) {BoidMesh = mesh;}
	void SetFwd(D3DXVECTOR3 sp) { fwd = sp; }
	void SetFwd(float x,float y,float z) { fwd.x = x;fwd.y = y;fwd.z = z;}
	void SetFwd();
	void SetPick(bool b) {m_Pick = b;}

	void SetBoidKind(int v) {m_BoidKind = v;}

	void SetUnitNum(int n);	
	void butterfly(BoidUnit &);

	int pat;
private:
	
	// 현재 도망 가야 하는 상황 인지.(가장 가까이 있는 그룹 종류)
	int CloseGroup;
	// 사는 곳
	int Space;
	// 움직임의 제약 (사는 곳에 따라서..)
	D3DXVECTOR3 limit;
	// Speed
	D3DXVECTOR3 fwd;
	D3DXVECTOR3 m_SetPos;
	// 같은 그룹의 boid 들 사이의 충돌을 막기 위한 경계의 반지름
	float BoidBox;

	// 이 그룹이 적을 인식가능한 범위
	float SeenRad;
	// 이 보이드 그룹의 행동 패턴(집단,객체)
	int Pattern;
	// 서브 행동패턴 (회피,휴식,분산,같은방향,마스터 따라가기)
	int SubPattern;
	// 겁많은 그룹 없는 그룹 (0.0f~1.0f)
	float FearValue;
	

	// Boid Unit Number
	int Unit_Num;
	int Lod;
	//sector id
	int SecX,SecY;


	BoidUnit *Units;
	// move range
	D3DXVECTOR3 Boid_Range;
	float m_LimitHeight;

	// speed
	float Boid_Speed;

	// Boid의 Mesh Data
	CGemRender *BoidMesh;
	char m_BoidName[256];
	bool m_Pick;
	unsigned long m_BeforeFrame;
	unsigned long m_CurrentFrame;
	int m_BoidKind;
	bool startup;
	int tick;
};




#endif
