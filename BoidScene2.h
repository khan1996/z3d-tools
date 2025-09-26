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
// 0~1 ������ �� return
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
// ���� �ൿ���� (ȸ��,�޽�,�л�,��������,������ ���󰡱�)
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
	// �Ϲ� ���̵� ����ü
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

		//ȸ�� ��
		//	D3DXVECTOR3 rotaxis;
		float rot[3];

		//speed (fwd�� length)
		float speed;

		// ��ǥ ������
		D3DXVECTOR3 look;
		D3DXVECTOR3 before_look;

		D3DXVECTOR3 fwd;
		D3DXVECTOR3 before_fwd;
		int height;
		// �����
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
	// õ���� ������ �������� Update
	void Update();
	void SetSpace(int s) { Space = s; }
	// ȸ�� :: ��ֹ��̳� �ٸ� ����� ������ �ʰ� ȸ��
	void avoid();
	
	// �浹 ���ϱ� :: ���� �׷��� boid �� ���� �ε����� �ʰ�.
	void separate(BoidUnit *,D3DXVECTOR3 *);
	// �浹���ϱ� :: �����ൿ ���ϰ� ������
	void separate2(BoidUnit *);
	//�������� �浹 ���ϱ�
	void HeightSeparate(BoidUnit *,D3DXVECTOR3 *);

	//���� :: ���� �׷��� boid ��� ���⺤�͸� ���� �����
	void align(BoidUnit *,D3DXVECTOR3 *);
	//���� :: ���� �׷��� boid ��� ��ǥ ��ġ�� ���� �����
	void concenter(BoidUnit *,D3DXVECTOR3 *);
	void randomvec(BoidUnit *,D3DXVECTOR3 *);
	// rot setting
	void RotSet(BoidUnit *);

	// ��踦 ��� boid�� ���� ���� �� ����
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
	// ����� ����(0 : ���� 1: �����ൿ��)
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
	
	// ���� ���� ���� �ϴ� ��Ȳ ����.(���� ������ �ִ� �׷� ����)
	int CloseGroup;
	// ��� ��
	int Space;
	// �������� ���� (��� ���� ����..)
	D3DXVECTOR3 limit;
	// Speed
	D3DXVECTOR3 fwd;
	D3DXVECTOR3 m_SetPos;
	// ���� �׷��� boid �� ������ �浹�� ���� ���� ����� ������
	float BoidBox;

	// �� �׷��� ���� �νİ����� ����
	float SeenRad;
	// �� ���̵� �׷��� �ൿ ����(����,��ü)
	int Pattern;
	// ���� �ൿ���� (ȸ��,�޽�,�л�,��������,������ ���󰡱�)
	int SubPattern;
	// �̸��� �׷� ���� �׷� (0.0f~1.0f)
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

	// Boid�� Mesh Data
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
