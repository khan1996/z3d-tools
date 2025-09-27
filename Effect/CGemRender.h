#ifndef __CGEMRENDER_H__
#define __CGEMRENDER_H__


#include <stdio.h>
#include "texture.h"
#include "BaseDataDefine.h"

#include <d3dx8.h>
#include <d3d8.h>


#define BUF_SIZE 256
#define VOT 1.0f
#define GEM_HEADER "GemDataFile\n"


class CGemRender {
public:	
	void SetClearBuffer(bool b);
	bool m_bClearBuffer;
	class GemTexture {
	public:	
		CTexture *m_Tex;
		int TexNum;
		GemTexture() {
			m_Tex = NULL;
			TexNum = 0;
		}
		~GemTexture() {
			if(m_Tex) {
			/*	if(TexNum >1)
					delete[] m_Tex;
				else*/
				delete[] m_Tex;
			}
		}
	};
	class GemSubFace{
	public:
		int *sub;
		int *sub_mat;
		int sub_num;
		GemSubFace() {
			sub = NULL;
			sub_mat = NULL;
			sub_num = 0;
		}
		~GemSubFace() {
			delete[] sub;
			delete[] sub_mat;
		}
	};
	class GemVertex{
	public:	
		float x,y,z;
		float nx,ny,nz;
		float s,t;
		GemVertex() {
			x = y = z = 0.0f;
			nx = ny = nz = 0.0f;
			s = t = 0.0f;
		}
		~GemVertex() {}
	};	
	class GemRotKey {
	public:
		int frame;
		D3DXVECTOR4 rot;

		GemRotKey() {
			frame = 0;
			rot.x = rot.y = rot.z = rot.w = 0.0f;
		}
		
		~GemRotKey(){
		}
	};
	
	class GemPosKey{
	public:
		int frame;
		bool bez;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 intan;
		D3DXVECTOR3 outtan;
		GemPosKey() {
			frame = 0;
			bez = false;
			pos.x = pos.y = pos.z = 0.0f;
			intan.x = intan.y = intan.z = 0.0f;
			outtan.x = outtan.y = outtan.z = 0.0f;
		}
		~GemPosKey(){
		}
	};
	
	class GemScaleKey{
	public:
		int frame;
		D3DXVECTOR3 scale;
		GemScaleKey() {
			frame = 0;
			scale.x = scale.y = scale.z=  1.0f;
		}
		~GemScaleKey(){
		}
	};
	
	class GemVisKey{
	public:
		int frame;
		float vis;
		GemVisKey(){
			frame = 0;
			vis = 1.0f;
		}
		~GemVisKey(){
		}
	};

	class GemObject {
	public:
		char m_Name[BUF_SIZE];
		char m_ParentName[BUF_SIZE];
		bool m_bParent;
			
		D3DXMATRIX m_TmMatrix;
		
		// vertex info
		D3DXVECTOR2 *m_Coord;
		D3DXVECTOR3 *m_CoordFace;

		D3DXVECTOR3 *m_Vert;
		D3DXVECTOR4 *m_Face;
		D3DXVECTOR3 *m_Fnormal;
		D3DXVECTOR3 *m_Normal;
		int m_MatId;

		int m_Frame;
		int m_CoordNum;
		int m_VertexNum;
		int m_iStartBufferIndex;

		int m_FaceNum;
		// anikey info
		int m_RotKeyNum;
		int m_PosKeyNum;
		int m_ScaleKeyNum;
		int m_VisKeyNum;
		//morphing
		int m_MorphNum;
		// 추가기능
		bool m_Bil1;
		bool m_Bil2;
		bool m_Bil3;
		// multy uv ani
		bool m_Multy;
		bool m_TexAni;
		bool m_RandAni;
		bool m_RandStartTexAni; //시작 프레임이 다른 텍스쳐 애니 flag
		bool m_RandStartSetting; //start 되었는지
		bool m_Zenable;
		bool m_Opacity;
		//사용되는 texture 갯수
		int m_AniTextureNum;
		int m_CurrentAniTexture;
		int m_BeforeAniTexture;

		int m_Red;
		int m_Green;
		int m_Blue;
		// 보간법 선택
		bool m_bH;
		// cullmode on flag
		bool m_bCull;
		
		float m_AniChangeFrame;
		//tex ani start frame
		float m_StartTexAniFrame;
		
		bool m_Zbuffer;
		bool m_bDecal;

		GemObject *m_Morph;
	

		
		GemVisKey *m_Vis;
		GemScaleKey *m_Scale;
		GemPosKey *m_Pos;
		GemRotKey *m_Rot;
		DWORD m_Color;	
		// vertex buffer 생성시에 필요한 vert pointer
		GemVertex *m_VertexBuffer;
		bool m_bInterpol;

		GemObject() {
			
			m_Multy = false;	
			m_RotKeyNum = m_PosKeyNum = m_ScaleKeyNum = m_VisKeyNum = 0;
			m_Vis = NULL;
			m_Scale = NULL;
			m_Pos = NULL;
			m_Rot = NULL;
			m_Red = m_Green = m_Blue = 255;
			m_bH = false;
			//morphing vertex
			m_Morph = NULL;
			m_MorphNum = 0;
			m_StartTexAniFrame = 0.0f;
			m_Zbuffer = false;
			m_Zenable = false;

			m_bCull = false;
			m_Color = D3DCOLOR_ARGB(255,255,255,255);

			m_MatId = -1;

			m_Frame = -1;
			m_VertexNum  = 0;
			m_FaceNum = 0;
			m_CoordNum  =0;
				
			m_Coord = NULL;
			m_CoordFace = NULL;
				
			m_Vert = NULL;
			m_Face = NULL;
			m_Fnormal = NULL;
			m_Normal = NULL;
			m_bParent = false;
			m_Bil1 = m_Bil2 = m_Bil3 = false;
			m_TexAni = false;
			m_RandAni = false;
			m_RandStartTexAni = false;
			m_RandStartSetting = false;
			m_Opacity = false;
			m_AniChangeFrame = 0.0f;
			m_AniTextureNum = 0;
			m_CurrentAniTexture = 0;
			m_BeforeAniTexture = 0;
			D3DXMatrixIdentity(&m_TmMatrix);
			memset(m_Name,0,sizeof(char)*BUF_SIZE);
			memset(m_ParentName,0,sizeof(char)*BUF_SIZE);
			m_VertexBuffer = NULL;
			m_bInterpol = true;
			m_bDecal = false;
			m_iStartBufferIndex = 0;

		}
		~GemObject() {
			if(m_Scale != NULL)
				delete[] m_Scale;
			if(m_Vis != NULL)
				delete[] m_Vis;
			if(m_Vert != NULL)
				delete[] m_Vert;
			if(m_Face != NULL)
				delete[] m_Face;
			if(m_Coord != NULL)
				delete[] m_Coord;
			if(m_CoordFace != NULL)
				delete[] m_CoordFace;
			if(m_Fnormal != NULL)
				delete[] m_Fnormal;
			if(m_Normal != NULL)
				delete[] m_Normal;
			if(m_Rot)
				delete[] m_Rot;
			if(m_Pos)
				delete[] m_Pos;
			if(m_Morph)
				delete[] m_Morph;
			if(m_VertexBuffer)
				delete[] m_VertexBuffer;

		}
	};
	
	class GemMaterial {
	public:
		char m_TextureName[BUF_SIZE];
		int  m_SubNum;
		GemMaterial *m_Sub;
		GemMaterial() {
			memset(m_TextureName,0,BUF_SIZE);
			m_Sub = NULL;
			m_SubNum = 0;
		}
		~GemMaterial() {
			if(m_SubNum) 
				delete[] m_Sub;
		}
	};	
	CGemRender();
	~CGemRender();
	
	void SetTransMatrix(D3DXMATRIX m) {m_TrMatrix = m;}
	bool CheckPosition(D3DXVECTOR3 center,D3DXVECTOR3 user);
	void SetXrot(float rot) { m_Xrot = rot;}
	void SetYrot(float rot) { m_Yrot = rot;}
	void SetZrot(float rot) { m_Zrot = rot;}


	void ScaleAni(int object_index,D3DXMATRIX &);
	void PosAni(int object_index,D3DXMATRIX &);
	void RotAni(int object_index,D3DXMATRIX &);

	bool LoadGemFile(char *,LPDIRECT3DDEVICE8 ,bool bVisibility = true);
	bool LoadUvFile(char *);

	bool LoadMorph(int );
	void MsgPrint(char *);
	void SetInitBuffer();

	void SetVertexBuffer(int index);
	void SetDecalBuffer(int index);				// Vertex Buffer 에 세팅
	void SetDecalBufferInit(int index);			// Decal Buffer Init

	void SetStartFrame(float s);
	
	void SetCash(bool b) { m_bCash = b;}

	void Render();
	void RenderObject(int object_index);

	void Update();
	void Update(D3DXVECTOR3 center,D3DXVECTOR3 user);

	void UnUpdate();
	bool VertexInterpolation(int index);
	void LoadSubFace(int object_index);
	void LoadTexture();
	// set zbuffer cullface all object
	void SetCullZbuffer(bool );
	// my effect setting
	void SetMine(bool t); 

	void LoadTexAni(int object_index);
	bool VisAni(int object_index);
	void CreateAniTm(int index,D3DXMATRIX &,D3DXMATRIX &);
	void SetChangeAniFrame(int object_index,float f);
	void SetStartTexAniFrame(int object_index,float f);
	int GetMaxFrame() {return (m_EndF / m_UnitF);}
	// effect scale setting func
	void SetScale(float x,float y,float z);
	void SetEffectPos(float x,float y,float z);
	void SetEffectPos(D3DXVECTOR3 pos);
	void SetCurrentFrame(float f);
	void SetCurrentFrame(float f,int *,DWORD *,bool *,bool update = false);
	
	void SetSrcBlend(DWORD s) {m_SrcBlend = s;}
	void SetDstBlend(DWORD d) {m_DstBlend = d;}
	void SetBlend(DWORD s,DWORD d) { m_SrcBlend = s; m_DstBlend = d;}
	void SetColor(int object_index,int r,int g,int b);	
	void SetNullTexture(bool b) {m_bNullTexture = b;}
	void SetPickObject(int index);
	void SetVot(float v) {m_Vot = v;}
	void SetLoop(bool b) {m_bLoop = b;}
	int GetObjectNum() {return m_ObjectNum;}
	void SetStartTexAni(bool b) {m_bTexAni = b;}
	void SetAxis(D3DXMATRIX & );
	void GetAxis(float ,float ,float ,float );
	void GetAxis(D3DXQUATERNION tmp) {m_AxisSet = true; m_Axis = tmp;}
	// ANI MESH USE
	void SetRandAni(bool b) {m_RandAni = b;}
	void SetRandAniMax(float s) {m_RandAniMax = s;}
	void SetRandAniMin(float s) {m_RandAniMin = s;}
	void SetSwitchAni(bool b) {m_SwitchAni = b;}
	void SetAniRad(float r) {m_AniRad = r;}
	// light 적용 mesh
	void SetLight(bool b) {m_bLight = b;}
	char *GetFileName() {return m_FileName;}

	FILE *m_GemFile;
	//multy uv
	int m_bMulty;
	
	int m_StartF;
	int m_EndF;
	int m_UnitF;
	int m_ObjectNum;
	int m_PickObject;

	int m_MatNum;
	bool m_bNullTexture;
	bool m_bLoop;
	bool m_bCash;

	bool m_bTexAni;
	bool m_Mine;
	bool m_bDecal;

	DWORD m_SrcBlend,m_DstBlend;

	float m_CurrentF;
	LPDIRECT3DDEVICE8 m_Device;
	D3DXMATRIX m_Scale;
	D3DXMATRIX m_TrMatrix;
	D3DXVECTOR3 m_Pos;
	D3DXQUATERNION global;
	D3DXQUATERNION m_Axis;
	bool m_AxisSet;
	//ANI MESH	
	float m_Xrot,m_Yrot,m_Zrot;
	bool m_RandAni;
	float m_RandAniMax;
	float m_RandAniMin;
	float m_AniRad;
	bool m_SwitchAni;
	bool m_StartSwitch;
	GemMaterial *m_Mtl;
	

	GemObject *m_Object;
	GemSubFace *m_Sub;

//	LPDIRECT3DVERTEXBUFFER8 *m_ObjectVertex;
	LPDIRECT3DVERTEXBUFFER8 m_lpVertexBuffer;
	int m_iTotalVertexNums;
	bool m_bDynamic;
	VOID *m_VertexLockPtr;


	GemTexture *m_Texture;
	GemTexture *m_TexAni;

	int m_TexNum;
	float m_Vot;

	//// vertex buffer 생성시에 필요한 vert pointer
	//GemVertex *m_Vert;	
	int VbufferNum;
	// multy uv ani
	CGemRender *m_Multy;
	bool m_bLight;
	char m_FileName[256];

};


#endif