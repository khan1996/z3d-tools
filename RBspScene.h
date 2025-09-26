// RBspScene.h: interface for the RBspScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RBSPSCENE_H__4F5F3941_FEE1_4307_AEBE_CD0CB4E77AC9__INCLUDED_)
#define AFX_RBSPSCENE_H__4F5F3941_FEE1_4307_AEBE_CD0CB4E77AC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "VECTOR.h"
#include "matrix.h"

#include "Texture.h"
#include "RBitSet.h"
#include "ObjectScene.h"
#include <vector>

#include <d3dx8.h>
#include "ViewFrustum.h"

using namespace std;

#define RBSP_EMPTY					-1
#define RBSP_POLYGON				1
#define RBSPBUF_SIZE 50

//#define RBSPEPS 0.01
#define RBSPEPS 0.03125f
#define RBSPBUF_SIZE 50

const DWORD dwRBSPUseFVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) |D3DFVF_TEXCOORDSIZE2(1));
enum RBSPSHADER
{
	RBSPSHADER_GF2,
	RBSPSHADER_GF3
};


enum RBSPSIDES
{
	RBSP_FRONT,
	RBSP_BACK,
};
enum RBSPLIGHT
{
	RBSPLIGHT_POINT,
	RBSPLIGHT_DIRECTION,
	RBSPLIGHT_TOTAL,
};
enum RBSPTRIGGER_TYPE
{
	RBSPTRIGGER_INIT,
	RBSPTRIGGER_SPHERE,
	RBSPTRIGGER_BOX,

};
enum RBSPTRIGGER_EVENT			// Trigger Event
{
	RBSPEVENT_OBJECTANI,		// Dynamic Object
	RBSPEVENT_EFFECT,			// Effect
	RBSPEVENT_SOUND,			// Sound
	RBSPEVENT_TOTAL,		
};
enum RBSPPICK_STATE				// Picking 상태
{
	RBSPPICK_NOT,				// Not Pick
	RBSPPICK_READY,				// Pick Ready( ex > 트리거에 이벤트 집어넣을때의 이벤트 들의 상태
	RBSPPICK_PICKED,			// 실제 Picking 되었을때.


};
// Lump Define
enum RBSPLUMP
{
	RBSP_ENTITIES,				// Stores player/object positions, etc...
	RBSP_TEXTURES,
	RBSP_PLANES,
	RBSP_NODES,
	RBSP_LEAFS,
	RBSP_LEAFFACES,
	RBSP_LEAFBRUSHES,
	RBSP_MODELS,
	RBSP_BRUSHES,
	RBSP_BRUSHESIDES,
	RBSP_VERTICES,
	RBSP_MESHVERTS,
	RBSP_SHADERS,
	RBSP_FACES,
	RBSP_LIGHTMAP,
	RBSP_LIGHTVOLUMES,
	RBSP_VISDATA,
	TOTAL_LUMP,

};
// geforce 3이상 
class RAniObject
{
public:
	RAniObject() 
	{
		vecBox[0].x = vecBox[0].y = vecBox[0].z = 0.0f;
		vecBox[1].x = vecBox[1].y = vecBox[1].z = 0.0f;

	}
	~RAniObject() {}
	D3DXVECTOR3 vecBox[2];

};
class RShadowVolumeScene;
class RShadowVolumeObject;

class RBspScene  
{
public:
////////////////
// Trigger
	class RBspEventBase
	{
	public:
		
		int				m_iPickState;				// Pick Event State
		char			m_strName[ RBSPBUF_SIZE ];	// File Name
		D3DXVECTOR3		m_vecCenter;				// Center Position
		float			m_fDelay;					// Delay Frame
		bool			m_bPlay;					// Play Flag
		bool			m_bStart;					// Start Flag
		bool			m_bLinked;					// Trigger에 Link가 되어 있는지
		DWORD			m_dwStartTime;				// Start Time
		DWORD			m_dwCurrentTime;			// Current Time

		RBspEventBase()
		{
			m_iPickState = RBSPPICK_NOT;
			memset(m_strName,0,sizeof(char) * RBSPBUF_SIZE);
			m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_fDelay = 0.0f;
			m_bPlay = false;
			m_bStart = false;
			m_bLinked = false;
			m_dwStartTime = 0;
			m_dwCurrentTime = 0;

		}
		virtual void Play() = 0;
		virtual void Start() = 0;
	};
	class RBspEventSound : public RBspEventBase
	{
	public:
		RBspEventSound(){
			m_iPickState = RBSPPICK_NOT;
			memset(m_strName,0,sizeof(char) * RBSPBUF_SIZE);
			m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_fDelay = 0.0f;
			m_bPlay = false;
			m_bStart = false;
			m_dwStartTime = 0;
			m_dwCurrentTime = 0;

		}
		virtual void Play();		// Event의 발동
		virtual void Start();		// 실제적인 작동

	};
	class RBspEventEffect : public RBspEventBase
	{
	public:

		RBspEventEffect()
		{
			m_iPickState = RBSPPICK_NOT;
			memset(m_strName,0,sizeof(char) * RBSPBUF_SIZE);
			m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_fDelay = 0.0f;
			m_bPlay = false;
			m_bStart = false;
			m_dwStartTime = 0;
			m_dwCurrentTime = 0;

		}
		virtual void Play();		// Event의 발동
		virtual void Start();		// 실제적인 작동

	};
typedef vector<RAniObject *> LstAniObject;

	class RBspEventObject : public RBspEventBase 
	{
	public:
		
		int				m_iLoopNum;					// Switching 이후 Ani를 몇번 수행할지
		LstAniObject	m_lstAniObject;				// Ani Object List (본체는 각 Leaf Node에 존재)

		RBspEventObject()
		{
			m_iPickState = RBSPPICK_NOT;
			memset(m_strName,0,sizeof(char) * RBSPBUF_SIZE);
			m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_fDelay = 0.0f;
			m_bPlay = false;
			m_bStart = false;
			m_dwStartTime = 0;
			m_dwCurrentTime = 0;

			m_iLoopNum = 0;
			m_lstAniObject.clear();

		}
		~RBspEventObject()
		{
			m_lstAniObject.clear();

		}
		virtual void Play();		// Event의 발동
		virtual void Start();		// 실제적인 작동

	};
	
	// 일반 R3s 오브젝트
	class RBspR3S 
	{
	public:
		vector3		m_vecPos;
		char		m_strName[RBSPBUF_SIZE];
		matrix		m_matTm;
		bool		m_bAlpha;
		bool		m_bLight;
		int			m_iPickState;				// Pick Event State

		RBspR3S() 
		{
			m_vecPos = vector3(0.0f,0.0f,0.0f);
			memset(m_strName,0,sizeof(char) * RBSPBUF_SIZE);
			m_matTm.MakeIdent();
			m_bAlpha = false;
			m_bLight = false;
			m_iPickState = RBSPPICK_PICKED;
		}
		~RBspR3S() {}
	};
typedef vector<RBspR3S *> LstR3SObject;

typedef vector<RBspEventSound *> LstEventSound;
typedef vector<RBspEventEffect *> LstEventEffect;
typedef vector<RBspEventObject *> LstEventObject;


	class RBspTrigger
	{
	public:
		int			m_iPickState;
		bool		m_bPlay;			// Play Flag	
		int			m_iTriggerValue;	// RBSPTRIGGER_SPHERE or RBSPTRIGGER_BOX
		D3DXVECTOR3 m_vecCenter;		// Trigger Center
		D3DXVECTOR3 m_vecTrigger[2];	// RBSPTRIGGER_SPHERE : vecTrigger[0] -> Sphere Center
										//						vecTrigger[1] -> Sphere Range
										// RBSPTRIGGER_BOX : vecTrigger[0] -> Min Vector
										//					 vecTrigger[1] -> Max Vector

		int m_iEventNum[10];			// 10개의 트리거 이벤트 갯수(현재 이벤트 3개 이용)
		LstEventSound	m_lstSound;		// Event Sound
		LstEventEffect	m_lstEffect;	// Event Effect
		LstEventObject	m_lstObject;	// Event Object
	
	RBspTrigger()
	{
		m_iPickState = RBSPPICK_NOT;
		m_iTriggerValue = RBSPTRIGGER_INIT;
		m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);

		m_vecTrigger[0] = m_vecTrigger[1] = D3DXVECTOR3(0.0f,0.0f,0.0f);
		memset(m_iEventNum,0,sizeof(int) * 10);
		
		m_lstSound.clear();
		m_lstEffect.clear();
		m_lstObject.clear();
		m_bPlay = false;

	}
	~RBspTrigger()
	{
		// 실 데이터는 node가 사라질때 delete 시켜준다
		m_lstSound.clear();
		m_lstEffect.clear();
		m_lstObject.clear();			
	}
		int InputEventSound(RBspEventSound *);
		int InputEventEffect(RBspEventEffect *);
		int InputEventObject(RBspEventObject *);
		bool CheckInTrigger(D3DXVECTOR3 vecPos);
		void Play();

	};
	class RBseVertex{
	public:
		D3DXVECTOR3 m_vecPos;
		D3DXVECTOR3 m_vecNormal;
		D3DXVECTOR2 m_vecTexcoord;
		D3DXVECTOR2 m_vecTexcoord2;
		D3DXVECTOR3 m_vecS;
		D3DXVECTOR3 m_vecT;
		D3DXVECTOR3 m_vecSxT;
	};
	// Bsp Inner Class
	class RBspHeader 
	{
	public:
		char		m_strID[4];		// 'IBSP'
		int			m_iVersion;		// 0x2e (Quake3) 
		RBspHeader()
		{
			memset(m_strID,0,sizeof(char) * 4);
			m_iVersion = 0;
		}
	};
	class RBspLump
	{
	public:
		int			m_iOffset;		// offset from file start 
		int			m_iLength;		// Lump Length
		RBspLump()
		{
			
			m_iOffset = RBSP_EMPTY;
			m_iLength = RBSP_EMPTY;

		}
	};
	class RBspShadowVertex {		// Shadow map Vertex 
	public:
		D3DXVECTOR3 m_vecPos;
		D3DXVECTOR3 m_vecNormal;
		D3DXVECTOR2 m_vecTexcoord;
		RBspShadowVertex() {
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecNormal = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecTexcoord = D3DXVECTOR2(0.0f,0.0f);
		}
	};
	class RBspLightVertex {			// Lighted Vertex
	public:
		D3DXVECTOR3 m_vecPos;			// Position
		D3DXVECTOR3 m_vecNormal;		// Normal
		D3DXVECTOR2 m_vecLightCoord;	// LightMap
		RBspLightVertex() {
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecNormal = D3DXVECTOR3(0.0f,0.0f,0.0f);

			m_vecLightCoord = D3DXVECTOR2(0.0f,0.0f);
		}
	};
		class RBspVertex
	{
	public:
		D3DXVECTOR3 m_vecPos;	
		D3DXVECTOR2 m_vecTex1;	// Texture
		D3DXVECTOR2 m_vecTex2;	// Lightmap
		D3DXVECTOR3 m_vecNormal;
		DWORD	m_dwColor;
		RBspVertex() 
		{
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecNormal = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecTex1 = D3DXVECTOR2(0.0f,0.0f);
			m_vecTex2 = D3DXVECTOR2(0.0f,0.0f);
			m_dwColor = 0xffffffff;

		}

	};
	class RBspDrawVertex {
	public:
		D3DXVECTOR3 m_vecPos;
		D3DXVECTOR3 m_vecNormal;
		D3DXVECTOR2 m_vecTexcoord;
		D3DXVECTOR2 m_vecTexcoord2;
	

		RBspDrawVertex() {
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecTexcoord = D3DXVECTOR2(0.0f,0.0f);
			m_vecTexcoord2 = D3DXVECTOR2(0.0f,0.0f);
			m_vecNormal = D3DXVECTOR3(0.0f,0.0f,0.0f);

		}
	};
	class RBspFace
	{
	public:
		int		m_iTexId;
		int		m_iEffect;
		int		m_iFaceType;						// 1=polygon, 2=patch, 3=mesh, 4=billboard 
		int		m_iStartVertex;
		int		m_iVertexNum;

		int		m_meshVertIndex;					// The index into the first meshvertex 
		int		m_numMeshVerts;						// The number of mesh vertices 

		int		m_iLightMapId;
	
		int		m_iLightMapCorner[2];
		int		m_iLightMapSize[2];
		D3DXVECTOR3 m_vecLightMapPos;			// The 3D origin of lightmap.
		D3DXVECTOR3 m_vecLightMapVec[2];		// The 3D space for s and t unit vectors.
		D3DXVECTOR3 m_vecNormal;
		int		m_iSize[2];							// The bezier patch dimensions. 
		RBspFace()
		{
			m_iTexId = RBSP_EMPTY;
			m_iLightMapId = RBSP_EMPTY;
			m_iEffect = RBSP_EMPTY;
			m_iFaceType = RBSP_POLYGON;
			m_iStartVertex = RBSP_EMPTY;
			m_iVertexNum = RBSP_EMPTY;
			memset(m_iLightMapCorner,0,sizeof(int) * 2);
			memset(m_iLightMapSize,0,sizeof(int) * 2);
			m_vecLightMapPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecLightMapVec[0] = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecLightMapVec[1] = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecNormal = D3DXVECTOR3(0.0f,0.0f,0.0f);
			memset(m_iSize,0,sizeof(int) * 2);

		}
	};
	class RBspTextureDef 
	{
	public:
		char	m_strName[64];
		int		m_iFlags;
		int		m_iContents;
		RBspTextureDef()
		{
			memset(m_strName,0,sizeof(char) * 64);
			m_iFlags = RBSP_EMPTY;
			m_iContents = RBSP_EMPTY;

		}
	};
	class RBspLightMap
	{
	public:
		//byte m_ucLightMap[128][128][3];	// 128 * 128 RGB 
		byte m_ucLightMap[49152];
		
		RBspLightMap()
		{
			memset(m_ucLightMap,0,sizeof(byte) * 49152);
		}
	};
	class RBspNode
	{
	public:
		int		m_iPlaneIndex;
		int		m_iFrontCIndex;		// Front node Child Index 
		int		m_iBackCIndex;		// Back node Child Index
		int		m_vecMinBox[3];	// Bound Box 
		int		m_vecMaxBox[3];
		RBspNode()
		{
			memset(m_vecMinBox,0,sizeof(int) * 3);
			memset(m_vecMaxBox,0,sizeof(int) * 3);
			m_iPlaneIndex = RBSP_EMPTY;
			m_iFrontCIndex = RBSP_EMPTY;
			m_iBackCIndex = RBSP_EMPTY;
			
		}
	};
		class RBspLight{
	public:
		int m_iPickState;
		int	m_iLightValue;			// Light Value : 0 : Point Light 1 : Direction Light
		D3DXVECTOR3 m_vecLight;		// Light Value 에 따른 Vector, Light Value : 0 -> Point Light Pos, 1 -> Light Direction
		D3DXVECTOR3 m_vecLightRange;// Light 범위 
		RShadowVolumeScene *m_pShadowVolume;
		bool	m_bShadowVolume;
		
		//RBspShadowMapScene *m_pShadowMap;
		bool	m_bShadowMap;
		
		float	m_fRed;
		float	m_fGreen;
		float	m_fBlue;

		D3DLIGHT8 m_D3DLight;

		RBspLight()
		{
			m_pShadowVolume = NULL;
/*			m_iPickState = RBSPPICK_NOT;
			m_iLightValue = RBSPLIGHT_POINT;
			m_vecLight =  D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecLightRange = D3DXVECTOR3(0.0f,0.0f,0.0f);
		
			m_bShadowVolume = false;
			m_pShadowVolume = NULL;
			m_bShadowMap = false;
			//m_pShadowMap = NULL;*/
		}
		~RBspLight()
		{
			//if(m_pShadowVolume != NULL)
			//{
			//	delete m_pShadowVolume;
			m_pShadowVolume = NULL;
			//}
		/*	if(m_pShadowVolume != NULL)
			{
				delete m_pShadowVolume;
				m_pShadowVolume = NULL;
			}
			/*if(m_pShadowMap != NULL)
			{
				delete m_pShadowMap;
				m_pShadowMap = NULL;

			}*/
		}
		void CreateShadowVolume()
		{
		/*	m_bShadowVolume = true;
			if(m_pShadowVolume != NULL)
				delete m_pShadowVolume;
			m_pShadowVolume = new RShadowVolumeScene;
			m_pShadowVolume->InitScene();*/
		}
		void CreateShadowMap()
		{
			m_bShadowMap = true;
			/*if(m_pShadowMap != NULL)
				delete m_pShadowMap;
			m_pShadowMap = new RBspShadowMap;*/
		}
		void DeleteAllShadowMap()
		{
			m_bShadowMap = false;
		/*	if(m_pShadowMap != NULL)
			{
				delete m_pShadowMap;
				m_pShadowMap = NULL;

			}*/
		}
		void DeleteAllShadowVolume()
		{
			m_bShadowVolume = false;
		/*	if(m_pShadowVolume != NULL)
			{
				delete m_pShadowVolume;
				m_pShadowVolume = NULL;

			}*/
		}
		void InputShadowVolume(RShadowVolumeObject);
		void RenderShadowVolume();

		void InputShadowMap();


	};

typedef vector<RBspLight> LstBspLight;
typedef vector<RBspTrigger> LstTrigger;

	class RBspLeafEx {				// Leaf Node Extension
	public:							// Light List, Shadow Volume List, Shadow Map Data
		int		m_iCluster;
		int		m_iAreaPotal;
		int		m_vecMinBox[3];
		int		m_vecMaxBox[3];
		int		m_iLeafFace;		// Leaf Node Face's First Index in face array
		int		m_iLeafFaceNum;
		int		m_iLeafBrush;
		int		m_iLeafBrushNum;
		
		LstBspLight m_lstLight;						// Light Pos List.
		int		m_iLightNum;						// Light Num
		
		LstTrigger  m_lstTrigger;
		int		m_iTriggerNum;
	
		////////////////// 현 노드가 가지고 있는 이벤트 리스트
		int m_iEventNum[10];			// 10개의 트리거 이벤트 갯수(현재 이벤트 3개 이용)
		
		LstEventSound	m_lstSound;		// Event Sound
		LstEventEffect	m_lstEffect;	// Event Effect
		LstEventObject	m_lstObject;	// Event Object

		LstAniObject	m_lstAniObject;	// Ani Object 본체
		int				m_iAniObjectNum;// Ani Object Number
		
		LstR3SObject	m_lstR3SObject; // R3S Object
		int				m_iR3SObjectNum;

		/// Picking 관련 ptr
		RBspLight		*m_pPickLight;
		RBspTrigger		*m_pPickTrigger;
		RBspEventEffect *m_pPickEventEffect;
		RBspEventSound	*m_pPickEventSound;
		RBspEventObject *m_pPickEventObject;
		RBspR3S			*m_pPickR3SObject;

		RBspLeafEx()
		{
			m_iCluster = RBSP_EMPTY;
			m_iAreaPotal = RBSP_EMPTY;
			memset(m_vecMinBox,0,sizeof(int) * 3);
			memset(m_vecMaxBox,0,sizeof(int) * 3);

			m_iLeafFace = RBSP_EMPTY;
			m_iLeafFaceNum = RBSP_EMPTY;
			m_iLeafBrush = RBSP_EMPTY;
			m_iLeafBrushNum = RBSP_EMPTY;
			
			m_lstLight.clear();
			m_iLightNum = 0;
			
			m_lstTrigger.clear();
			m_iTriggerNum = 0;

			memset(m_iEventNum,0,sizeof(int) * 10);
			
			m_lstSound.clear();
			m_lstEffect.clear();
			m_lstObject.clear();
			
			m_iAniObjectNum  = 0;
			m_lstAniObject.clear();

			m_pPickLight = NULL;
			m_pPickTrigger = NULL;
			m_pPickEventEffect = NULL;
			m_pPickEventSound = NULL;
			m_pPickEventObject = NULL;
			m_pPickR3SObject = NULL;

			m_lstR3SObject.clear(); // R3S Object
			m_iR3SObjectNum = 0;


		}
		~RBspLeafEx()
		{
			int i;
			
			m_lstLight.clear();
			m_lstTrigger.clear();
			
			
			if( static_cast<int>(m_lstSound.size()) >= m_iEventNum[RBSPEVENT_SOUND])
			{
				for( i  = 0 ;i < m_iEventNum[RBSPEVENT_SOUND]; i++)
				{
					delete m_lstSound[i];
					m_lstSound[i] = NULL;
				}
			}
			m_lstSound.clear();
			if( static_cast<int>(m_lstEffect.size()) >= m_iEventNum[RBSPEVENT_EFFECT])
			{
				for( i = 0; i < m_iEventNum[RBSPEVENT_EFFECT]; i++)
				{
					delete m_lstEffect[i];
					m_lstEffect[i] = NULL;
				}
			}
			m_lstEffect.clear();
			
			if( static_cast<int>(m_lstEffect.size()) >= m_iEventNum[RBSPEVENT_OBJECTANI])
			{
				for( i = 0; i < m_iEventNum[RBSPEVENT_OBJECTANI]; i++)
				{
					delete m_lstObject[i];
					m_lstObject[i] = NULL;

				}
			}
			for( i = m_iR3SObjectNum - 1;i >= 0; i--)
			{
				delete m_lstR3SObject[i];
				m_lstR3SObject[i] = NULL;
				m_lstR3SObject.erase(m_lstR3SObject.begin() + i);
			}
			m_lstR3SObject.clear();

			m_lstObject.clear();
			if( static_cast<int>(m_lstEffect.size()) >= m_iAniObjectNum)
			{
				for( i = 0; i < m_iAniObjectNum; i++)
				{
					delete m_lstAniObject[i];
					m_lstAniObject[i] = NULL;
				}
			}
			m_lstAniObject.clear();
		
			m_pPickLight = NULL;
			m_pPickTrigger = NULL;
			m_pPickEventEffect = NULL;
			m_pPickEventSound = NULL;
			m_pPickEventObject = NULL;
			m_pPickR3SObject = NULL;


		}
		
		// Light
		void InputLight(int iLightValue,D3DXVECTOR3 vecLight,D3DXVECTOR3 vecRange,int iRed,int iGreen,int iBlue,bool bShadowVolume,bool bShadowMap);
		void DeleteAllLight();
		// 이 라이트가 날라가면 여기에 귀속되어 있던 쉐도우볼륨과 쉐도우 맵 자체도 날라간다.
		void DeleteLight(D3DXVECTOR3 vecLight);	
		void MoveLight(D3DXVECTOR3 vecBeforePos,D3DXVECTOR3 vecNewPos);
		void DeleteLight(RBspLight *);
		int CheckInChrLight(D3DXVECTOR3 ,D3DLIGHT8 *pLight,int iLightNum);
		
		// Trigger
		void CreateEmptyTrigger(D3DXVECTOR3 vecCenter,int iTriggerValue,D3DXVECTOR3 vecTrigger[2]);
		void PickTriggerInputEffect(RBspEventEffect *);
		void PickTriggerInputObject(RBspEventObject *);
		void PickTriggerInputSound(RBspEventSound *);
		
		void DeleteTrigger(D3DXVECTOR3 vecCenter);
		void DeleteTrigger(RBspTrigger *);
		
		void DeleteAllTrigger();
		// R3S Object
		int InputR3SObject(float fx,float fy,float fz,char *strName,matrix matTm,bool bAlpha,bool bLight);

		// Event Sound
		int InputEventSound(char *strName,D3DXVECTOR3 vecCenter,float fDelay);
		void DeleteEventSound(D3DXVECTOR3 vecCenter);
		void MoveEventSound(D3DXVECTOR3 vecCenter,D3DXVECTOR3 vecEndPos);
		// Event Effect
		int InputEventEffect(char *strName,D3DXVECTOR3 vecCenter,float fDelay);
		void DeleteEventEffect(D3DXVECTOR3 vecCenter);
		void MoveEventEffect(D3DXVECTOR3 vecCenter,D3DXVECTOR3 vecEndPos);
		// Event Object Ani 
		int InputEventObject(RAniObject *,int iLoop,float fDelay);
		void DeleteEventObject(RAniObject *);
		void MoveEventObject(RAniObject *pObject,D3DXVECTOR3 vecLocalMove);		// 상대적인 이동값
		
		void DeleteEventEffect(RBspEventEffect *);
		void DeleteEventSound(RBspEventSound *);
		void DeleteEventObject(RBspEventObject *);

		// Picking 관련
		void PickLight(D3DXVECTOR3 vecPos);
		void PickTrigger(D3DXVECTOR3 vecPos);
		void PickEventEffect(D3DXVECTOR3 vecPos);
		void PickEventSound(D3DXVECTOR3 vecPos);
		void PickEventObject(D3DXVECTOR3 vecPos);
		void PickR3SObject(D3DXVECTOR3 vecPos);


		void UnPickLight();
		void UnPickTrigger();
		void UnPickEventEffect();
		void UnPickEventSound();
		void UnPickEventObject();
		void UnPickR3SObject();
		
		void MovePickLight(D3DXVECTOR3 vecNewPos);
		void MovePickTrigger(D3DXVECTOR3 vecNewPos);
		void MovePickEventEffect(D3DXVECTOR3 vecNewPos);
		void MovePickEvnetSound(D3DXVECTOR3 vecNewPos);
		void MovePickEvnetObject(D3DXVECTOR3 vecNewPos);
		// Delete Picking Unit
		void DeletePickR3SObject();
		void DeletePickEventEffect();
		void DeletePickEventObject();
		void DeletePickEventSound();
		void DeletePickLight();
		void DeletePickTrigger();

		void PickTriggerEdit(D3DXVECTOR3 vecCenter,int iTriggerValue,D3DXVECTOR3 vecTrigger[2]);

		
	};

	class RBspLeaf					 
	{
	public:
		int		m_iCluster;
		int		m_iAreaPotal;
		int		m_vecMinBox[3];
		int		m_vecMaxBox[3];
		int		m_iLeafFace;		// Leaf Node Face's First Index in face array
		int		m_iLeafFaceNum;
		int		m_iLeafBrush;
		int		m_iLeafBrushNum;
		RBspLeaf()
		{
			m_iCluster = RBSP_EMPTY;
			m_iAreaPotal = RBSP_EMPTY;
			memset(m_vecMinBox,0,sizeof(int) * 3);
			memset(m_vecMaxBox,0,sizeof(int) * 3);

			m_iLeafFace = RBSP_EMPTY;
			m_iLeafFaceNum = RBSP_EMPTY;
			m_iLeafBrush = RBSP_EMPTY;
			m_iLeafBrushNum = RBSP_EMPTY;

		}

	};
	class RBspPlane
	{
	public:
		D3DXVECTOR3 m_vecNormal;
		float	m_fDistance;
		RBspPlane() : m_vecNormal(0.0f,0.0f,0.0f), m_fDistance(0.0f)
		{}
	};
	class RBspVisData				// Cluster data for PVS
	{
	public:
		int		m_iClustersNum;
		int		m_iBytesPerCluster;	// The amount of bytes (8 bits) in the cluster's bitset
		byte	*m_pBitsets;		// The array of bytes that holds the cluster bitsets	
		RBspVisData()
		{
			m_iClustersNum = RBSP_EMPTY;
			m_iBytesPerCluster = RBSP_EMPTY;
			m_pBitsets = NULL;
		}
		~RBspVisData()
		{
			if(m_pBitsets)
				delete[] m_pBitsets;
		}
	};
	class RBspLightUnit 
	{
	public:
		int m_iStartIndex;			// Index Buffer 에서의 start pos
		int m_iStartVertIndex;		// Vertex Buffer 에서의 start pos
			
		int m_iLightId;
		RBspLightUnit(){
			m_iStartIndex = 0;
			m_iStartVertIndex = 0;

			m_iLightId = RBSP_EMPTY;
		}
		~RBspLightUnit(){}
	};
	class RBspBrushe 
	{
	public:
		int m_iBrusheSide;          // The starting brush side for the brush 
	    int m_iBrusheSidesNum;		// Number of brush sides for the brush
		int m_iTexId;				// The texture index for the brush	
		RBspBrushe() 
		{
			m_iBrusheSide = RBSP_EMPTY;
			m_iBrusheSidesNum = 0;
			m_iTexId = RBSP_EMPTY;
		}
		~RBspBrushe() {}
	};

	class RBspBrusheSide 
	{
	public:
		int m_iPlaneIndex;           // The plane index
		int m_iTexId;				 // The texture index
		RBspBrusheSide() 
		{
			m_iPlaneIndex = RBSP_EMPTY;
			m_iTexId = RBSP_EMPTY;
		}
		~RBspBrusheSide(){}
	};

	class RBspWalkUnit 
	{
	public:
		D3DXVECTOR3 m_vecBPos;				// Before Position
		D3DXVECTOR3 m_vecPos;				// Current Update Position
		D3DXVECTOR3 m_vecOPos;				// Last Out Position
		D3DXVECTOR3 m_vecSlide;				// Slide Vector
		float		m_fSlideFraction;		// Slide Fraction Value
		int			m_iCollisionPlanes[5];		// Collision Plane		
		//int			m_iCollisionPlane;
		int			m_iCollisionPlanesNum;	// Collision Plane Number(max 5)	 
		float		m_fRadius;				// Radius
		float		m_fFraction;			// Fraction Value
		
		bool		m_bOutputStartsOut;
		bool		m_bOutputAllSolid;

		RBspWalkUnit()
		{
			m_vecBPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecOPos = D3DXVECTOR3(0.0f,0.0f,0.0f);

			m_vecSlide = D3DXVECTOR3(0.0f,0.0f,0.0f);

			m_fRadius = 45.0f;
			
			m_fFraction = 1.0f;
			m_fSlideFraction = 1.0f;
			
			m_bOutputStartsOut = true;
			m_bOutputAllSolid = false;
			//m_iCollisionPlane = RBSP_EMPTY;
			
			m_iCollisionPlanesNum = 0;
			memset(m_iCollisionPlanes,0,sizeof(int) * 5);

		}
	};

	typedef vector<WORD *> IndexArrType ;
	// Geforce3
	//typedef vector<RBseVertex *> VertexArrType ;
	// Geforec2
	typedef vector<RBspDrawVertex *> VertexArrType ;
	typedef vector<RBspLightUnit> LightUnitType;

	typedef vector<RBspLeafEx *> LeafExPtrType;			// Shadow 처리를 위한 Leaf Extension Node Pointer List
														// Update 시에 visible인 leaf Extension Node의 포인터를 넣어 놓고,
														// Render 시에 stencil Buffer Render 한다.	
	
	RBspScene();
	~RBspScene();
	bool LoadBspScene(char *strFileName,float fSx = 4.0f,float fSy = 4.0f,float fSz = 4.0f);
	void RenderScene(bool bBspIn = false,bool bTarget = false);
	void SetZeroPosition(float x,float y,float z);	// 던젼의 원점위치
	void SetBspScale(float x,float y,float z) { m_vecScale = D3DXVECTOR3(x,y,z);}
	// Write String to File
	void WritetoFile(FILE *fp,char *str);
	bool SaveBseFile(char *strFileName);

	void RenderBox(vector3 vecAxis,vector3 vecScale,bool bTarget);
	void RenderEvent();
/////////////////////
	bool LoadBspHeader(FILE *);
	bool LoadBspLump(FILE *);
	bool LoadBseScene(const char *strFileName,float fx = 4.0f,float fy = 4.0f,float fz = 4.0f);

	void CreateBufferNum();			// Buffer Size Setting
	void ClearBufferNum();			// Buffer Size Init
	void CreateBuffer();			// LightMap, BaseTexture Vertex,Index Buffer Create

	void CreateBufferNum2();			// Buffer Size Setting


	void RenderBspScene();
	void SetBuffer();

	void CheckCollisiontoBrushe();	// Brushe Collision Check
	void CheckCollisiontoBrushe2(); // Slide  없는 충돌
	// Check Node Brush
	void CheckBrusheNode(int iNodeIndex, float fStartFraction, float fEndFraction, D3DXVECTOR3 vecStart,D3DXVECTOR3 vecEnd);
	// Check Brush for Collision
	void CheckBrushe(RBspBrushe *pBrushe);

	void UpdateScene(D3DXVECTOR3 vecPos,bool bColl = true);
	void UpdateLeafBox(float x,float y,float z);

	int FindLeafNode(D3DXVECTOR3 vecPos);
	int FindWorldLeaf(D3DXVECTOR3 vecPos);	// World Position 으로 leaf 알아내기
	int ClusterVisibleTest(int iCurrentCluster,int iTestCluster);
	bool FaceCulling(int iFaceIndex);
	void PushFaceBuffer( int iFaceIndex );
	
	void UpdateNode(int iNode,bool bFrustum,int iCurrentCluster);
	void UpdateLeaf(int iLeaf,bool bFrustum,int iCurrentCluster);
	void RenderLeafBox();

	void SetBox(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax);
	int	 GetLeafIndex() {return m_iRBspLeafIndex;}
	
	int CheckChrLight(D3DXVECTOR3 vecPos,D3DLIGHT8 *pLight,int iLightNum);

	//D3DLIGHT8 *CheckChrLight(D3DXVECTOR3 vecPos);		// 캐릭터가 bsp 라이트 범위 안인가 Check
	D3DXVECTOR3 CollisionCheck(D3DXVECTOR3 vecBefore,D3DXVECTOR3 vecNew,float fRad);
	// Slide 없는 충돌
	D3DXVECTOR3 CollisionPoint(D3DXVECTOR3 vecBefore,D3DXVECTOR3 vecNew,float fRad);

	void SetBspScaleLoaded(float x,float y,float z);
	
	D3DXVECTOR3 WorldToLocalPos(D3DXVECTOR3 vecWorld);	// World Position 을 Local position 으로..
	D3DXVECTOR3 LocalToWorldPos(D3DXVECTOR3 vecLocal);	// Local Position 을 World Position 으로..
	void GenerateR3SObject();							// Generate R3S Object
	void DynamicR3SLoad();
	void RenderR3SObject();
	/////

	RBseVertex		*m_pDotVertex;

	// Leaf Faces
	int				m_iLeafFacesNum;
	int				*m_pLeafFaces;
	
	// Plane 
	int				m_iPlanesNum;
	RBspPlane		*m_pPlanes;
	// Leaf Brush
	int				m_iLeafBrushesNum;
	int				*m_pLeafBrushes;           // The index into the brush array 
	// Brush 
	int				m_iBrushesNum;
	RBspBrushe		*m_pBrushes;
	// Brush Side
	int				m_iBrusheSidesNum;
	RBspBrusheSide	*m_pBrusheSides;
	
		
	// Leaf Node
	int				m_iLeafsNum;
	RBspLeaf		*m_pLeafs;
	RBspLeafEx		*m_pLeafExs;
	int				*m_pVisLeafs;				// Visible Leaf
	int				m_iVisLeafs;					// Visible Leaf 갯수

	int				*m_pBVisLeafs;				// Visible Leaf
	int				m_iBVisLeafs;					// Visible Leaf 갯수


	// Node
	int				m_iNodesNum;
	RBspNode		*m_pNodes;
	// Lightmap
	int				m_iLightMapsNum;
	RBspLightMap	*m_pLightMaps;
	CTexture		*m_pLightTextures;
	// Texture Def
	int				m_iTextureDefsNum;
	RBspTextureDef	*m_pTextureDefs;
	CTexture		*m_pTextures;
	CTexture		*m_pNTextures;

	// Faces
	int				m_iFacesNum;
	RBspFace		*m_pFaces;
	// Vertexs
	int				m_iVertsNum;
	RBspVertex		*m_pBeforeVerts;			// 원래 bsp 에서 사용하는 vertex class
	RBspDrawVertex	*m_pVerts;					// 최적화를 위한 vertex class
	
	RBspLump		m_arrLump[TOTAL_LUMP];
	RBspHeader		m_Header;
	// Visible (Cluster)
	RBspVisData		m_VisData;
	// Face Render Bin
	RBitset			m_FaceRenderBit;
	int				m_iVisLeafNodeNum;
	
	CViewFrustum	*m_pFrustum;

	// Triangle List

	// Base Texture Vertex Buffer
	LPDIRECT3DVERTEXBUFFER8 *m_pBaseVertexBuffer;
	// LightMap Vertex Buffer
	LPDIRECT3DVERTEXBUFFER8 *m_pLightVertexBuffer;
	int				*m_pBaseVertexNum;
	int				*m_pBaseVertexUseNum;

	// Base Index Buffer
	LPDIRECT3DINDEXBUFFER8 *m_pBaseIndexBuffer;
	int				*m_pBaseIndexNum;
	int				*m_pBaseIndexUseNum;
	
	

	IndexArrType		m_pIndexArr;
	VertexArrType		m_pVertexArr;
	LightUnitType		*m_pLightUnit;
	
	// Current Camera Position
	D3DXVECTOR3			m_vecCamera;
	// Walk Unit Instance
	RBspWalkUnit		m_WalkUnit;
	
	
	int					m_iShaderVersion;
	
	std::vector<CObjectScene*> m_ObjectSceneNode;

////////////////////
	char m_strBspName[RBSPBUF_SIZE];
	vector3 m_vecStartPos;
	vector3 m_vecZeroPos;
	D3DXVECTOR3 m_vecScale;

	bool	m_bVisible;

	int		m_iRBspLeafIndex;
	LPDIRECT3DDEVICE8 m_D3Device;
	
	bool	m_bVisEffectBox;
	bool	m_bVisSoundBox;
	bool	m_bVisAObjectBox;
	bool	m_bVisLightBox;
};

#endif // !defined(AFX_RBSPSCENE_H__4F5F3941_FEE1_4307_AEBE_CD0CB4E77AC9__INCLUDED_)
