#pragma once

#include "d3dx8.h"
#include "CharacterCape.h"
#include "CharacterLightShadowManager.h"
#include <map>
#include <vector>
using namespace std; 

class CQuadClothResource 
{
	struct ClothVertex
	{
		enum { FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 };
		D3DXVECTOR3 pos; 
		D3DXVECTOR3 n; 
		float u, v; 
	};

private:
	LPDIRECT3DVERTEXBUFFER8	m_pVB; 
	LPDIRECT3DINDEXBUFFER8		m_pIB; 
	ClothVertex*								m_pClothVertex; 
	WORD*										m_pClothIndex; 
	int												m_iNumVertex; 
	int												m_iNumFace; 
	int												m_iCol; 
	int												m_iRow; 

private:
	void CalcNormal( int r, int c, int ar, int ac, int br, int bc ); 
	inline int ROWCOL( int r, int c ) { return (r*(m_iCol+1)+c); }

public:
	CQuadClothResource(); 
	virtual ~CQuadClothResource(); 

	void Init( int Row, int Col ); 
	void Render(); 
	void ShadowRender(); 
	void UpdateLockedVertex(); 
	void SetVertex( int index, D3DXVECTOR3& vPos ); 
	void ComputeNormals(); 
	void SetupCloth( CCharacterCape* pCape ); 
};

inline void CQuadClothResource::SetVertex( int index, D3DXVECTOR3& vPos )
{
	m_pClothVertex[index].pos = vPos; 
}

/*********************************************************/

class CClothMgr
{
public:
	typedef map<CZ3DGeneralChrModel*, CCharacterCape*> CLOTHMAP; 
	friend class CCharacterCape; 

public:
	static CClothMgr*						s_pInstance; 
	CLOTHMAP								m_ClothMap; 
	CQuadClothResource				m_CapeRenderer; 

private:
	CClothMgr(void); 

public:
	virtual ~CClothMgr(void);
	static CClothMgr* _Instance(); 
	static void _Destroy(); 
	void ReleaseAllData(); 

	void Update(); 
	void Render(); 
	void AddCharacterCape( CZ3DGeneralChrModel* pChrModel ); 
	void DeleteCharacterCape( CZ3DGeneralChrModel* pChrModel ); 
	CCharacterCape* GetCharacterCape( CZ3DGeneralChrModel* pChrModel ); 
	void Reset(); 
};
