#ifndef __WADLOADER_H__
#define __WADLOADER_H__

#define BUF_SIZE 255

#include <vector>
#include "asedefine.h"
#include "texture.h"
#include "BaseDataDefine.h"
#include <d3dx8.h>
#include <d3d8.h>

#define WAD_FILE "GEMDatafile"
#define VOT 1.0f
#define ANIRAD 1000.0f

using namespace std;



class CMeshMorph {
public:
	CMeshMorph();
	~CMeshMorph();

	
	bool LoadWadfile(char *filename,LPDIRECT3DDEVICE8 );
	int Render(LPDIRECT3DDEVICE8 ,D3DXVECTOR3 );
	D3DXMATRIX RotateObject(LPDIRECT3DDEVICE8 ,int object_index,float frame,D3DXMATRIX *w);
	D3DXMATRIX TranslateObject(LPDIRECT3DDEVICE8 d_device,int object_index,float frame,D3DXMATRIX *w);
	void vertex_interpolation(int object_index,float now_frame);
	void Change_Vertexbuffer(int object_index);
	void Create_Vertex(int i,LPDIRECT3DDEVICE8	);
	void CreateTexture(LPDIRECT3DDEVICE8 device);
	void LoadSubface(int object_index);
	void SetNullTexture(bool b) {m_NullTexture = b;}
	int UpdateMesh(D3DXVECTOR3 ,D3DXVECTOR3 );
	
	void SetXrot(float rot) { m_xrot = rot;}
	void SetYrot(float rot) { m_yrot = rot;}
	void SetZrot(float rot) { m_zrot = rot;}

	void SetAniRad(float r) {anirad = r;}

	void SetFrame(float f) {frame = f;}

	void SetColor(DWORD );
	// alpha : 0~255
	int GetMaxframe() {return max_frame;}
	void SetAlpha(int );
	void SetLoop(bool b) {m_Loop = b; }
	void StartAni(bool b) {m_Start = b;}
	void SetSwitchAni(bool b) {m_switchani = b;}
	void SetBlend(DWORD s,DWORD d) {SrcBlend = s; DstBlend = d;}
	
	void SetRandAni(bool b) {m_RandAni = b;}
	void SetRandAniMax(float s) {m_RandMax = s;}
	void SetRandAniMin(float s) {m_RandMin = s;}

	bool CheckPosition(D3DXVECTOR3 ,D3DXVECTOR3 );

	bool GetStart() {return m_Start;}

	// ������ ������ ����
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
	class AseScaleKey{
		int frame_num;
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
		char object_name[255];
		char parent_name[255];
		bool bparent;
		D3DXVECTOR3 *vertex;
		D3DXVECTOR2 *texcoord;
		D3DXVECTOR3 *normal;
		bool bnormal;


		D3DXMATRIX matrix;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot_axis;
		float rot_angle;
		vector<AseRotKey> rot_key;
		vector<AseMorphObject> morph;
		int morph_num;
		vector<AsePosKey> pos_key;
		int pos_keyNum;
		int rot_keyNum;
		int scale_keyNum;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 scale_axis;
		vector<AseScaleKey> scale_key;
		D3DXQUATERNION *rot_quatkey;
		AseFace *face;

	};

	class AseModel {
	public:	
		int object_num;
		int material_num;
		int effect_num;

		vector<AseObject> pobject;
		vector<AseMaterial> pmaterial;
		vector<D3DXVECTOR3> pEffectPos;

	};
	AseModel Model;
//////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
	LPDIRECT3DVERTEXBUFFER8 *object;	
	CTexture *texture;

	int texture_count;
	DWORD Color;
	
	WadFace *object_sub;
	
	custom_vertex *vert;
	D3DXVECTOR3 *vert2;
	float frame;
	int max_frame;
	
	DWORD SrcBlend,DstBlend;
	float vot;
	//rotation degree
	float m_xrot,m_yrot,m_zrot;
	// switch ani
	float anirad;
	// switch ani �̿��� �޽����� �ƴ���
	bool m_switchani;
	// random ani
	bool m_RandAni;
	float m_RandMax;
	float m_RandMin;
	// ani loop ���� �ȵ���
	bool m_Loop;
	// ani ������ �������� �ƴ���..
	bool m_Start;
	bool m_NullTexture;
	
	FILE *wadfile;
};

#endif
