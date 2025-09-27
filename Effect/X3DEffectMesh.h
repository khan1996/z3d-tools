// X3DEffectMesh.h: interface for the CX3DEffectMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X3DEFFECTMESH_H__AE1D13FE_1406_41C3_B13A_98825BC9C07C__INCLUDED_)
#define AFX_X3DEFFECTMESH_H__AE1D13FE_1406_41C3_B13A_98825BC9C07C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "X3DEffectBase.h"
#include "CGemRender.h"
#include "SMRObj.h"

#include <vector>
//#include <map>

#define EFF_MAXCASH	60

using namespace std;

class _CashNode {						// Mesh Cash List node class
		public:
			CGemRender *m_pMesh;		// Mesh Data
			int m_nUseCount;			// 이 노드를 이용하는 effect의 갯수(0이 되면 list 에서 삭제 가능)
			_CashNode() {
				m_pMesh = NULL;
				m_nUseCount = 0;
			}
			~_CashNode() {
				if(m_pMesh != NULL) {
					delete m_pMesh;
					m_pMesh = NULL;
				}
			}
};
class CashNode {
public:

	vector<_CashNode *> m_Cash;
	int m_nCash;
	CashNode() {
		m_Cash.clear();
		m_nCash = 0;
	}
	~CashNode() {
		for(int i=0;i< (int)m_Cash.size();i++) {
			delete m_Cash[i];
			m_Cash[i] = NULL;				
		}
		m_Cash.clear();
		m_nCash = 0;

	}
};

//typedef vector<_CashNode *> CashNode;

//typedef std::map<char* ,CGemRender *> CashNode;

typedef struct _MeshObject
{
	CKeyList<ColorKeyList> m_lstColor;
	float m_fStartTexFrame;
	float m_fTexFrame;
} MeshObject;

class CX3DEffectMesh : public CX3DEffectBase  
{
	protected:

		bool m_bUpdateFrame;
		
		char m_strMeshFile[MAX_PATH];
	//	CEffectMesh *Mesh;
		CGemRender *m_GemRender;
		CSMRObj *m_pSmrMesh;

		float m_fRenderFrame;							// Render 되어야 할 Frame
		static CashNode m_lstCash;						// Mesh cash list
	//	static int m_nCash;								// Mesh cash Num
		int		   *m_GemObjectTexNum;					// Gem Object Texture Num
		DWORD		*m_dwTick;
		bool		*m_bFirst;
	public:
		unsigned long m_dwObjectNum;
		MeshObject *m_moMesh;

	public:
		CX3DEffectMesh();
		virtual ~CX3DEffectMesh();

		// Mesh Cash Func
		CGemRender *LoadCash(char *strFilename);		// Cash 안의 mesh load
		void InputCash(CGemRender *);					// Cash 안에 mesh data input
		void DeleteCash();								// Cash 안의 mesh Data delete
		//static void DeleteAllCash();					// Cash 전부 delete
		void SubUseCount(char *strFilename);			// Cash 참조 카운터 1 감소 

		unsigned long GetMaxFrame(void) 
		{
			if(m_GemRender)
				return (unsigned long)m_GemRender->GetMaxFrame(); 
			else if(m_pSmrMesh)
			{
				return (unsigned long)m_pSmrMesh->m_iFrameInfo[1]; 
			}
		}

		void SetSrcBlending(unsigned long dwSrcBlending)
		{
			m_dwSrcBlending = dwSrcBlending;
			if(m_GemRender)
			{
	//			Mesh->SetBlend(m_dwSrcBlending, m_dwDestBlending);
				m_GemRender->SetSrcBlend(m_dwSrcBlending);
			}
			else if(m_pSmrMesh)
			{
				m_pSmrMesh->SetSrcBlend(m_dwSrcBlending);
			}

		}
		void SetDestBlending(unsigned long dwDestBlending)
		{
			m_dwDestBlending = dwDestBlending;
			if(m_GemRender)
			{

				//			Mesh->SetBlend(m_dwSrcBlending, m_dwDestBlending);
				m_GemRender->SetDstBlend(m_dwDestBlending);

			}
			else if(m_pSmrMesh)
			{
				m_pSmrMesh->SetDstBlend(m_dwDestBlending);
			}
		}

		void SetTexFrame(unsigned long dwObjectNum, float fTexFrame)
		{
			m_moMesh[dwObjectNum].m_fTexFrame = fTexFrame;
//			Mesh->SetTexAniFrame(dwObjectNum, fTexFrame);
			if(m_GemRender)
			{
				m_GemRender->SetChangeAniFrame(dwObjectNum, fTexFrame);
			}
		}
		float GetTexFrame(unsigned long dwObjectNum) { return m_moMesh[dwObjectNum].m_fTexFrame; }

		void SetStartTexFrame(unsigned long dwObjectNum, float fStartTexFrame)
		{
			m_moMesh[dwObjectNum].m_fStartTexFrame = fStartTexFrame;
			if(m_GemRender)
			{

				m_GemRender->SetStartTexAniFrame(dwObjectNum,fStartTexFrame);
	//			Mesh->SetStartTexAni(dwObjectNum, fStartTexFrame);
			}
		}
		float GetStartTexFrame(unsigned long dwObjectNum) { return m_moMesh[dwObjectNum].m_fStartTexFrame; }

		void SetObjectColor(unsigned long dwObjectNum)
		{
//			Mesh->SetPickColor(dwObjectNum, r, g, b, a);
//			Mesh->SetPickNum(dwObjectNum);
			if(m_GemRender)
			{

				m_GemRender->SetPickObject(dwObjectNum);
			}
		}
		void Create(unsigned long dwStartFrame, unsigned long dwEndFrame);
		BOOL CreateBuffer();
		void Render(void);
		BOOL Interpolation(float fFrame);
		
		void LoadFile(char *file);

		void Load(FILE *fp, const char *strOriginalPath = NULL);
		void Save(FILE *fp, const char *strOriginalPath = NULL);
};

#endif // !defined(AFX_X3DEFFECTMESH_H__AE1D13FE_1406_41C3_B13A_98825BC9C07C__INCLUDED_)
