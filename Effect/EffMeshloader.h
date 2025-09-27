#ifndef __EFFECTMESH_H__
#define __EFFECTMESH_H__

#define BUF_SIZE 256

#include <vector>
#include "EffAseDefine.h"
#include "texture.h"
#include "BaseDataDefine.h"
#include <d3dx8.h>
#include <d3d8.h>

#define EFF_FILE "GEMDatafile"
#define VOT 1.0f

using namespace std;



class CEffectMesh {
public:
	CEffectMesh();
	~CEffectMesh();

	
	bool LoadWadfile(char *filename,LPDIRECT3DDEVICE8 );
	float Render(LPDIRECT3DDEVICE8 ,D3DXVECTOR3 );
	D3DXMATRIX RotateObject(LPDIRECT3DDEVICE8 ,int object_index,float frame,D3DXMATRIX *w);
	D3DXMATRIX TranslateObject(LPDIRECT3DDEVICE8 d_device,int object_index,float frame,D3DXMATRIX *w);
	D3DXMATRIX ScaleObject(LPDIRECT3DDEVICE8 ,int object_index,float frame,D3DXMATRIX *w);
	int vertex_interpolation(int object_index,float now_frame);
	void Change_Vertexbuffer(int object_index);
	void SetObjectColor(int obj_num,int r,int g,int b,int a);
	void Create_Vertex(int i,LPDIRECT3DDEVICE8	);
	void CreateTexture(LPDIRECT3DDEVICE8 device);
	void CreateAniTexture(LPDIRECT3DDEVICE8 device,int obj_num);
	void LoadSubface(int object_index);
	void SetTexAniFrame(int object_index,float frame);
	void SetStartTexAni(int object_index,float frame);
	void StartTexAni(bool b) {m_texanistart = b;}
	void SetPickColor(int object_index,int r,int g,int b,int a);
	//picking 되어있는 object 의 index
	void SetPickNum(int n) {pick_index = n;}
	void SetNullTexture(bool b) {m_NullTexture = b;}
	float UpdateMesh();
	
	void SetFrame(float f);

	void SetColor(int r,int g,int b,int a) {Color = D3DCOLOR_ARGB(a,r,g,b);}
	// alpha : 0~255
	float GetMaxframe() {return max_frame;}
	int GetObjectNum(){return Model.object_num;}
	void SetAlpha(int );
	void SetLoop(bool b) {m_Loop = b; }
	void StartAni(bool b) {m_Start = b;}
	void SetBlend(DWORD s,DWORD d) {SrcBlend = s; DstBlend = d;}

	bool GetStart() {return m_Start;}
	
	// 프레임 증가율 조정
	void SetVot(float n) {vot = n;}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	in class
///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class WadFace {
	public:
		vector<int> sub;
		int sub_num;
	};
	class WadTexture {
	public:
		vector<LPDIRECT3DTEXTURE8 *> tex;
		int tex_num;
	};
	class custom_vertex{
	public:	
		float x,y,z;
		float nx,ny,nz;
		float s,t;
	};
	class custom_vertex2{
	public:	
		float x,y,z;
	};

	class AseFace{
	public:
		int vertex_index[4];
		int coord_index[3];
	};
	class AseMaterial{
	public:	
		char texture_name[255];
		char texture_file[255];
		BYTE color[3];
		// standard ot multy/sub
		int mat_class;
		AseMaterial *sub;
		int sub_num;
		float fcolor[3];
		int tex_id;
		float u_tile;
		float v_tile;
		float u_offset;
		float v_offset;
	};
	class AseRotKey{
	public:	
		int frame_num;
		float x,y,z,w;
	};
	class AsePosKey{
	public:	
		int frame_num;
		float x,y,z;
	};
	class AseScaleKey{
	public:
		int frame_num;
		float x,y,z;
	};
	class AseMorphObject{
	public:	
		D3DXVECTOR3 *vertex;
		D3DXVECTOR2 *texcoord;
		D3DXVECTOR3 *normal;
		AseFace *face;
		int keynum;
	};

	class AseObject{
	public:
		int material_id;
		int vertex_num;
		int face_num;
		int texcoord_num;
		bool btexture;
		bool bmorph;
		//bilboard mesh
		bool bBil;
		// bilboard2 mesh
		bool bBil2;
		// texture change ani.
		bool bTexani;
		// morphing 시작 프레임
		bool bMorphStart;
		// texture random change ani. 
		bool bRandTex;
		//cull face
		bool bCull;
		// Ani texture num
		int AniTexNum;
		bool bZ;
		

		// 이제 불러져야할 tex index
		int current_tex;
		
		char object_name[255];
		char parent_name[255];
		bool bparent;
		D3DXVECTOR3 *vertex;
		D3DXVECTOR2 *texcoord;
		D3DXVECTOR3 *normal;
		bool bnormal;
		//object 별 RGBA
		DWORD ObjectColor;	
		DWORD ObjectPickColor;

		// ani texture 
		CTexture *anitexture;

		D3DXMATRIX matrix;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot_axis;
		float rot_angle;
		vector<AseRotKey> rot_key;
		vector<AseMorphObject> morph;
		int morph_num;
		vector<AsePosKey> pos_key;
		int pos_keyNum;
			//
		vector<AseScaleKey> scale_key;
		int scale_KeyNum;
		int rot_keyNum;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 scale_axis;
		D3DXQUATERNION *rot_quatkey;
		AseFace *face;
		float texstart_frame;
		float texchange_frame;
	};

	class AseModel {
	public:	
		int object_num;
		int material_num;
		vector<AseObject> pobject;
		vector<AseMaterial> pmaterial;
	};
	AseModel Model;
//////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
	LPDIRECT3DVERTEXBUFFER8 *object;	
	CTexture *texture;
	
	int pick_index;
	int texture_count;
	DWORD Color;
	
	WadFace *object_sub;
	
	custom_vertex *vert;
	D3DXVECTOR3 *vert2;
	float frame;
	float max_frame;
	
	DWORD SrcBlend,DstBlend;
	int vot;
	// ani loop 돌지 안돌지
	bool m_Loop;
	// ani 시작한 상태인지 아닌지..
	bool m_Start;
	bool m_texanistart;
	bool m_NullTexture;
	FILE *wadfile;
};

#endif
