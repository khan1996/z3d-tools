#include <d3dx8.h>
#include <d3d8.h>
#include <vector>

namespace SGEngine {
	

	// template function 정의
	template<class _T>
	inline void SafeDelete( _T ptr )
	{
		if( NULL != ptr )
		{
			delete (ptr);
			ptr = NULL;
		}
	}

	template<class _T>
	inline void SafeDeleteA( _T ptr )
	{
		if( NULL != ptr )
		{
			delete[] (ptr);
			ptr = NULL;
		}
	}
class CExportMeshVertex
{
public:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecNormal;
	D3DXVECTOR2 m_vecTexcoord;
	CExportMeshVertex() : m_vecPos(0.0f,0.0f,0.0f),m_vecNormal(0.0f,0.0f,0.0f),m_vecTexcoord(0.0f,0.0f)
	{}
	~CExportMeshVertex(){}
};
enum MESHNODETYPE 
	{
		MESHNODETYPE_DUMY = 0,
		MESHNODETYPE_MATERIAL,		// Material
		MESHNODETYPE_MESHDATA,		// Vertex, Index Data
		MESHNODETYPE_ANINODE,		// Ani Node
		MESHNODETYPE_NUM,
	};

	enum SMRNODE
	{
		SMRNODE_NODE = 0,
		SMRNODE_GROUP,
		SMRNODE_NUM,
	};
	enum SMROBJTYPE
	{
		SMROBJ_MESH = 0,
		SMROBJ_HELPER,
		SMROBJ_BONE,
		SMROBJ_NUM,
	};
	enum SMRMESHTYPE
	{
		SMRMESHTYPE_STATIC = 0,
		SMRMESHTYPE_STATIC_WITHOUTLIGHTMAP,
		SMRMESHTYPE_DYNAMIC,
		SMRMESHTYPE_NUM,
	};

	enum SMRANICONTROLTYPE 
	{
		SMRANICONTROL_BEZIER = 0,
		SMRANICONTROL_LINEAR,
		SMRANICONTROL_TCB,
		SMRANICONTROL_SAMPLE,
		SMRANICONTROL_NUM,


	};

	namespace Core {

		class CMeshHeader
		{
		public:
			CMeshHeader() : m_fVersion(1.0f),m_iNodes(0),m_iType(SMRMESHTYPE_STATIC)
			{
				memset(m_iExtension,0,sizeof(int)*10);

			}
			~CMeshHeader()
			{}
			float m_fVersion;
			int m_iNodes;
			int m_iType;

			int m_iExtension[10];		//  후일을 위한 확장.

			
		};
		class CMeshObjNodeHeader
		{
		public:
			CMeshObjNodeHeader() : m_iNodeType(MESHNODETYPE_DUMY), m_iNodeSize(0)
			{}
			~CMeshObjNodeHeader()
			{}
			int m_iNodeType;
			unsigned long m_iNodeSize;

		};

		class CNodeTMInfo				// MeshTM KeyInfo.
		{
		public:

			D3DXVECTOR3 m_vecTrans;		// Translation
			D3DXVECTOR3 m_vecScale;		// Scale
			D3DXQUATERNION m_qRot;		// Rot
			D3DXQUATERNION m_qScaleAxis;
			float m_fDetermination;

			CNodeTMInfo() : m_vecTrans(0.0f,0.0f,0.0f),m_vecScale(1.0f,1.0f,1.0f),m_qRot(0.0f,0.0f,0.0f,1.0f),
				m_qScaleAxis(0.0f,0.0f,0.0f,1.0f),m_fDetermination(0.0f)
			{}
			~CNodeTMInfo(){}
		};
	
		class CMeshKeyAniNode
		{
		public:
			class CMeshKeyAniTm
			{
			public:
				CMeshKeyAniTm() : m_iFrame(0)
				{

				}
				~CMeshKeyAniTm()
				{

				}
				int m_iFrame;
				CNodeTMInfo m_TmInfo;

			};
			
			class CMeshKeyAniRot
			{
			public:
				CMeshKeyAniRot() : m_qRotKey(0.0f,0.0f,0.0f,1.0f),m_iFrame(0)
				{

				}
				~CMeshKeyAniRot()
				{

				}
				D3DXQUATERNION m_qRotKey;
				int	m_iFrame;		

			};
			class CMeshKeyAniPos
			{
				public:
				CMeshKeyAniPos() : m_vecPos(0.0f,0.0f,0.0f),m_iFrame(0)
				{

				}
				~CMeshKeyAniPos()
				{

				}
				D3DXVECTOR3 m_vecPos;
				int m_iFrame;

			};
			class CMeshKeyAniScale
			{
				public:
				CMeshKeyAniScale() : m_iFrame(0),m_vecScale(1.0f,1.0f,1.0f)
				{

				}
				~CMeshKeyAniScale()
				{

				}
				D3DXVECTOR3 m_vecScale;
				int m_iFrame;
			};
			class CMeshKeyAniVis
			{
			public:
				CMeshKeyAniVis() : m_iFrame(0),m_fVisibility(1.0f)
				{

				}
				~CMeshKeyAniVis()
				{

				}
				float m_fVisibility;
				int m_iFrame;

			};

			CMeshKeyAniNode() :m_iRotKeys(0),m_pRotKeys(NULL),m_bRotAni(false),m_iRotType(SMRANICONTROL_BEZIER),
				m_iPosKeys(0),m_pPosKeys(NULL),m_bPosAni(false),m_iPosType(SMRANICONTROL_BEZIER),
				m_iScaleKeys(0),m_pScaleKeys(NULL),m_bScaleAni(false),m_iScaleType(SMRANICONTROL_BEZIER),
				m_iVisKeys(0),m_pVisKeys(NULL),m_bVisAni(false),m_iVisType(SMRANICONTROL_BEZIER),
				m_bTmAni(false),m_iTmKeys(0),m_pTmKeys(NULL)				
			{
			}
			~CMeshKeyAniNode()
			{
				if(m_pRotKeys)
				{
					delete[] m_pRotKeys;
					m_pRotKeys = NULL;
				}

				if(m_pPosKeys)
				{
					delete[] m_pPosKeys;
					m_pPosKeys = NULL;
				}
				
				if(m_pScaleKeys)
				{
					delete[] m_pScaleKeys;
					m_pScaleKeys = NULL;
				}

				if(m_pVisKeys)
				{
					delete[] m_pVisKeys;
					m_pVisKeys = NULL;
				}
				if(m_pTmKeys)
				{
					delete[] m_pTmKeys;
					m_pTmKeys = NULL;
				}
				

			}
			bool m_bRotAni;

			int		m_iRotKeys;
			CMeshKeyAniRot *m_pRotKeys;
			int		m_iRotType;

			bool m_bPosAni;

			int		m_iPosKeys;
			CMeshKeyAniPos *m_pPosKeys;
			int		m_iPosType;

			
			bool m_bScaleAni;

			int		m_iScaleKeys;
			CMeshKeyAniScale *m_pScaleKeys;
			int		m_iScaleType;


			
			bool m_bVisAni;

			int		m_iVisKeys;
			CMeshKeyAniVis *m_pVisKeys;
			int		m_iVisType;
			
			bool m_bTmAni;
			int m_iTmKeys;
			CMeshKeyAniTm *m_pTmKeys;


		};
		class CMeshMaterialInfo
		{
		public:
			CMeshMaterialInfo()
			{
				memset(m_strName,0,sizeof(char) * 256);
			}
			~CMeshMaterialInfo(){}
			bool CheckString(const char *strName)
			{
			
				if(!strcmp(m_strName,strName))
					return true;
				return false;
			}

			char m_strName[256];


		};
		class CMeshObjNode;
		class CMeshSkinData
		{
		public:

			class CMeshSkinRigid 
			{
			public:
				int m_iMatIndex;
				int m_iVertexIndex;
				char m_strNodeName[256];
				int m_iNodeIndex;
				CMeshObjNode *m_pNode;

				CMeshSkinRigid() : m_iVertexIndex(0), m_iMatIndex(0),m_pNode(NULL)
				{
					memset(m_strNodeName,0,sizeof(char) * 256);
					m_iNodeIndex = 0;
				}
				~CMeshSkinRigid(){}
			};
			class CMeshBlendData
			{	
			public:
				char m_strNodeName[256];
				int m_iNodeIndex;
				float m_fNodeWeight;
				CMeshObjNode *m_pNode;

				CMeshBlendData() :m_fNodeWeight(0.0f) , m_pNode(NULL)
				{
					memset(m_strNodeName,0,sizeof(char) * 256);
					m_iNodeIndex = 0;
				}
				~CMeshBlendData(){}
			};
			class CMeshSkinBlend
			{
			public:
				int m_iMatIndex;
				int m_iVertexIndex;
				CMeshBlendData *m_pArrBlendData;
				int				m_iBlendData;

				
				CMeshSkinBlend() : m_iVertexIndex(0),m_iMatIndex(0)
				{
					m_pArrBlendData = NULL;
					m_iBlendData = 0;

				}
				~CMeshSkinBlend()
				{
					SafeDeleteA(m_pArrBlendData);
					m_pArrBlendData = NULL;
				}
			};

			CMeshSkinData() : m_iRigid(0),m_iBlend(0),m_pRigid(NULL),m_pBlend(NULL)
			{

			}
			~CMeshSkinData()
			{
				SafeDeleteA(m_pRigid);
				m_pRigid = NULL;
				SafeDeleteA(m_pBlend);
				m_pBlend = NULL;

			}
			void FindSkinRigidData(int iIndex,std::vector<CMeshSkinRigid *> *pArrRigid)
			{
				int i;
				if(pArrRigid)
				{
					for(i = 0; i < m_iRigid; i++)
					{
						if(m_pRigid[i].m_iVertexIndex == iIndex)
						{
							pArrRigid->push_back(&(m_pRigid[i]));
						}
					}
				}

			}
			void FindSkinBlendData(int iIndex,std::vector<CMeshSkinBlend *> *pArrBlend)
			{
				int i;
				if(pArrBlend)
				{
					for(i = 0; i < m_iBlend; i++)
					{
						if(m_pBlend[i].m_iVertexIndex == iIndex)
						{
							pArrBlend->push_back(&(m_pBlend[i]));
						}
					}
				}
			}

			int m_iRigid;
			CMeshSkinRigid *m_pRigid;

			int m_iBlend;
			CMeshSkinBlend *m_pBlend;

		};



		class CMeshObjNode;
		class CMeshCoreData
		{
		public:
			class CMeshSMorphData	// Smart Morphing
			{
			public:
				int m_iModelID;
				char m_strName[256];
				D3DXVECTOR3 *m_pRelativeVertices;
				int			*m_pRelativeIndex;
				int			m_iRelativeNum;
				CMeshSMorphData() : m_iModelID(-1),m_pRelativeVertices(NULL),m_pRelativeIndex(NULL),m_iRelativeNum(0)
				{
					memset(m_strName,0,sizeof(char) * 256);
				}
				~CMeshSMorphData()
				{
					SafeDeleteA(m_pRelativeVertices);
					m_pRelativeVertices = NULL;
					SafeDeleteA(m_pRelativeIndex);
					m_pRelativeIndex = NULL;

				}
				void SetSmartMorphData(CExportMeshVertex *pSrc,CExportMeshVertex *pDest,const int &iVertices,const int &iModelIndex = -1,char *strModelName = NULL);
				
			};
			class CMeshMorphData
			{
			public:
				CMeshMorphData() : m_iFrame(0), m_pVertices(NULL)
				{}
				~CMeshMorphData()
				{
					SafeDeleteA(m_pVertices);
					m_pVertices = NULL;
				}
				int m_iFrame;
				CExportMeshVertex *m_pVertices;



			};
			CMeshCoreData() : m_iType(SMRMESHTYPE_STATIC),m_iVerticesNum(0),m_iIndicesNum(0),
								m_pVertices(NULL),m_pIndices(NULL),m_iMaterialIndex(0),m_pNode(NULL),m_bUpdateData(false),
								m_bMorph(false),m_pMorphData(NULL),m_iMorph(0),
								m_bSMorph(false),m_pSMorph(NULL),m_iSMorph(0),m_pfSMorphInterValue(NULL),
								m_bCullOff(false),m_bZWriteOff(false),m_bZOff(false)
			{
			
				m_bTotalBilboard = false;

				memset(m_strName,0,sizeof(char) * 256);
				m_pDefaultVertices = NULL;
			}
			~CMeshCoreData()
			{
				SafeDeleteA(m_pVertices);
				m_pVertices = NULL;

				SafeDeleteA(m_pDefaultVertices);
				m_pDefaultVertices = NULL;


				SafeDeleteA(m_pIndices);
				m_pIndices = NULL;

			
				SafeDeleteA(m_pMorphData);
				m_pMorphData = NULL;

				SafeDeleteA(m_pSMorph);
				m_pSMorph = NULL;
				SafeDeleteA(m_pfSMorphInterValue);
				m_pfSMorphInterValue = NULL;

			}
		
			void SetTotalBilboard(bool bValue) { m_bTotalBilboard = bValue;}
			bool GetTotalBilboard() { return m_bTotalBilboard;}

			void SetCullOffFlag(bool bValue) { m_bCullOff = bValue;}
			bool GetCullOffFlag() { return m_bCullOff; }

			void SetZWriteOffFlag(bool bValue) { m_bZWriteOff = bValue;}
			bool GetZWriteOffFlag() { return m_bZWriteOff; }

			void SetZOffFlag(bool bValue) { m_bZOff = bValue;}
			bool GetZOffFlag() { return m_bZOff; }


			void CreateBuffer()
			{

				if(m_iIndicesNum > 0)
				{
					SafeDeleteA(m_pIndices);
					m_pIndices = NULL;
					m_pIndices = new WORD[m_iIndicesNum];
				}
				if(m_iVerticesNum > 0)
				{
					SafeDeleteA(m_pVertices);
					m_pVertices = NULL;

					m_pVertices = new CExportMeshVertex[m_iVerticesNum];
				}
			}
			void CreateDefaultBuffer()
			{
				if(m_pDefaultVertices == NULL)
				{
				
					if(m_iVerticesNum > 0)
					{
						m_pDefaultVertices = new CExportMeshVertex[m_iVerticesNum];
						memcpy(m_pDefaultVertices,m_pVertices,sizeof(CExportMeshVertex) * m_iVerticesNum);

					}
				}
			}
			void CopyBuffer(CExportMeshVertex *pData,WORD *pData2)
			{
				if(pData != NULL && m_pVertices != NULL)
				{
					memcpy(m_pVertices,pData,sizeof(CExportMeshVertex) * m_iVerticesNum);
				}
				if(pData2 != NULL && m_pIndices != NULL)
				{
					memcpy(m_pIndices,pData2,sizeof(WORD) * m_iIndicesNum);
				}
			}
			void AddBuffer(CExportMeshVertex *pData,int iData,WORD *pData2,int iData2)
			{
				int i;
				int iIndexStart = m_iVerticesNum;

				if(pData && m_pVertices)
				{
					CExportMeshVertex *pNewData = new CExportMeshVertex[m_iVerticesNum + iData];
					memcpy(pNewData,m_pVertices,sizeof(CExportMeshVertex) * m_iVerticesNum);
					for(i = 0; i < iData; i++ )
					{
						pNewData[m_iVerticesNum + i] = pData[i];
					}
					m_iVerticesNum += iData;
					SafeDeleteA(m_pVertices);
					m_pVertices = pNewData;
				}
				if(pData2 && m_pIndices)
				{
					WORD *pNewData2 = new WORD[iData2 + m_iIndicesNum];
					memcpy(pNewData2,m_pIndices,sizeof(WORD) * m_iIndicesNum);
					for(i = 0; i < iData2; i++ )
					{
						pData2[i] += iIndexStart;
						pNewData2[m_iIndicesNum + i] = pData2[i];

					}
					m_iIndicesNum += iData2;
					SafeDeleteA(m_pIndices);
					m_pIndices = pNewData2;
				}

			}
			void CreateMorphBuffer(int iMorphNums)
			{
				m_bMorph = true;
				m_iMorph = iMorphNums;
				m_pMorphData = new CMeshMorphData[m_iMorph];
			}
			void SetMorphBuffer(int iMorphIndex,int iFrame,int iVertices,CExportMeshVertex *pVertices)
			{
				if(pVertices && iMorphIndex < m_iMorph && m_pMorphData)
				{
					m_pMorphData[iMorphIndex].m_iFrame = iFrame;
					m_pMorphData[iMorphIndex].m_pVertices = new CExportMeshVertex[iVertices];
					memcpy(m_pMorphData[iMorphIndex].m_pVertices,pVertices,sizeof(CExportMeshVertex ) * iVertices);
				}
			}
			void SetSkinMeshData(const int &iVertex,D3DXMATRIX *matTm,D3DXMATRIX *matInvNodeTm);	// rigid
			void SetSkinMeshData(const int &iVertex,CMeshSkinData::CMeshBlendData *pBlend,const int &iBlend);

			void SetUpdateFlag(bool bFlag) { m_bUpdateData = bFlag;}
			void UpdateMeshMorph(const float &fFrame);
			
			void UpdateMeshSmartMorph();
			void SetSmartMorphInterValue(const float &fValue,const int &iIndex);
			int GetSmartMorphNum() { return m_iSMorph;}

			CMeshObjNode *m_pNode;

			char m_strName[256];
			int m_iType;
			int m_iVerticesNum;
			int m_iIndicesNum;
			int m_iMaterialIndex;

			bool m_bUpdateData;


			CExportMeshVertex *m_pVertices;
			CExportMeshVertex *m_pDefaultVertices;	//skinning 을 위한 default vertices.

			WORD			 *m_pIndices;

			bool m_bMorph;
			CMeshMorphData *m_pMorphData;
			int m_iMorph;

			bool m_bSMorph;
			CMeshSMorphData *m_pSMorph;
			int m_iSMorph;
			float *m_pfSMorphInterValue;
			bool			m_bTotalBilboard;

			bool			m_bCullOff;
			bool			m_bZWriteOff;
			bool			m_bZOff;

			


		};

	
		class CMeshObjNode
		{

		public:
			CMeshObjNode() : m_pParent(0),m_pChild(0),m_iChild(0),m_iParentID(-1),m_iNodeID(-1),
				m_iNodeType(SMRNODE_NODE),m_iObjectType(SMROBJ_MESH),m_iMeshType(SMRMESHTYPE_STATIC),m_pAniNode(NULL),
				m_bSkinMorph(false),m_pSkinData(NULL)
			{
				memset(m_strName,0,sizeof(char) * 256);
				m_iMeshData = 0;
				m_pArrMeshData.clear();

				m_bAniNode = false;
				D3DXMatrixIdentity(&m_matNodeTm);
				D3DXMatrixIdentity(&m_matInvNodeTm);
				
				D3DXMatrixIdentity(&m_matLocalTm);
				
				
				m_fVisibility = 1.0f;

				m_bWorldTM = false;

			}
			~CMeshObjNode()
			{

				SafeDeleteA(m_pChild);
				m_pChild = NULL;


				SafeDelete(m_pAniNode);
				m_pAniNode = NULL;
				
				SafeDelete(m_pSkinData);
				m_pSkinData = NULL;

			}
			void SetParentNode(CMeshObjNode *pNode,const int &iParentID)
			{
				m_pParent = pNode;
				m_iParentID = iParentID;
			}
			CMeshObjNode *GetParentNode()
			{
				return m_pParent;
			}
			int GetParentNodeID()
			{
				return m_iParentID;

			}
			int GetChildNum()
			{
				return m_iChild;
			}
			CMeshObjNode *GetChildNode(int iNum)
			{
				if(iNum < m_iChild)
				{	
					return &(m_pChild[iNum]);
				}	
				return NULL;
			}
			void SetNodeName(const char *strName) { strcpy(m_strName,strName);}
			bool IsNodeSameName(const char *strName) 
			{ 
				if(!strcmp(m_strName,strName)) return true;
				return false;
			}
			char *GetNodeName() { return m_strName;}

			int GetNodeID() { return m_iNodeID;}
			void SetNodeID(const int &iNodeID) { m_iNodeID = iNodeID;}
			void SetType(const int &iNodeType,const int &iObjType,const int &iMeshType)
			{
				m_iNodeType = iNodeType;
				m_iObjectType = iObjType;
				m_iMeshType = iMeshType;
			}
			int GetNodeType() { return m_iNodeType;}
			int GetObjectType() { return m_iObjectType;}
			int GetMeshType() { return m_iMeshType;}
			

			CMeshCoreData *GetMeshData(const int &iMat)
			{
				for(unsigned int i = 0; i < m_pArrMeshData.size(); i++ )
				{
					if(m_pArrMeshData[i]->m_iMaterialIndex == iMat)
						return m_pArrMeshData[i];
				}
				return NULL;
			}
			void SetSkinMeshData(const int &iMat,const int &iVertex,CMeshSkinData::CMeshBlendData *pBlend,const int &iBlend)
			{
			
				for(unsigned int i = 0; i < m_pArrMeshData.size(); i++ )
				{
					if(m_pArrMeshData[i]->m_iMaterialIndex == iMat)
						m_pArrMeshData[i]->SetSkinMeshData(iVertex,pBlend,iBlend);

				}


			}
			void SetSkinMeshData(const int &iMat,const int &iVertex,D3DXMATRIX *matTm,D3DXMATRIX *matInvNodeTm)
			{

				for(unsigned int i = 0; i < m_pArrMeshData.size(); i++ )
				{
					if(m_pArrMeshData[i]->m_iMaterialIndex == iMat)
						m_pArrMeshData[i]->SetSkinMeshData(iVertex,matTm,matInvNodeTm);

				}

			}
			void AddMeshData(CMeshCoreData *pMesh)
			{
				m_pArrMeshData.push_back(pMesh);
				pMesh->m_pNode = this;

				m_iMeshData++;

			}
			void SetChildNum(const int &iNum) { 
				m_iChild = iNum;
				SafeDeleteA(m_pChild);
				if(iNum > 0)
				{
					m_pChild = new CMeshObjNode[m_iChild];
					for(int i = 0; i <m_iChild; i++ )
					{
						m_pChild[i].SetParentNode(this,m_iNodeID);
					}
				}
			}
			
			void SetAniNode(CMeshKeyAniNode *pNode)
			{
				m_pAniNode = pNode;
				m_bAniNode = true;
			}
			CMeshKeyAniNode *GetAniNode()
			{
				if(m_bAniNode)
					return m_pAniNode;
				return NULL;

			}
			bool ISAniNode()
			{
				return m_bAniNode;

			}
			void SetSkinData(CMeshSkinData *pData)
			{
				m_pSkinData = pData;
				m_bSkinMorph =true;

			}
			CMeshSkinData *GetSkinData()
			{
				if(m_bSkinMorph)
					return m_pSkinData;
				return NULL;
			}
			bool ISSkinData()
			{
				return m_bSkinMorph;

			}
			void SetWorldTMFlag(bool bFlag) { m_bWorldTM = bFlag;}
			bool GetWorldTMFlag() { return m_bWorldTM;}

			void UpdateTm(const float &fFrame,D3DXMATRIX *matParent = NULL);
			void UpdateSkinning();


			CMeshObjNode *	m_pParent;
			int				m_iParentID;

			CMeshObjNode *	m_pChild;
			int				m_iChild;

			D3DXMATRIX	m_matNodeTm;
			D3DXMATRIX	m_matInvNodeTm;

			D3DXMATRIX	m_matLocalTm;
			


			CNodeTMInfo m_DefaultNodeTm;
			float		m_fVisibility;

		protected:

			
			char			m_strName[256];
			int				m_iNodeID;
			
			int				m_iNodeType;
			int				m_iObjectType;
			int				m_iMeshType;
			bool			m_bWorldTM;


			CMeshKeyAniNode *m_pAniNode;
			bool			m_bAniNode;

			bool			m_bSkinMorph;
			CMeshSkinData *m_pSkinData;



			std::vector<CMeshCoreData *> m_pArrMeshData;	// loading 할때 채워진다.
			int			m_iMeshData;

		


			
		};
	}

}
