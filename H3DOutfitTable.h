// H3DOutfitTable.h: interface for the CH3DOutfitTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H3DOUTFITTABLE_H__145CF134_819E_4958_8C2D_7413F1D28089__INCLUDED_)
#define AFX_H3DOUTFITTABLE_H__145CF134_819E_4958_8C2D_7413F1D28089__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc.h"

#include <map>
#include <vector>


struct H3DOutfitElement
{
	void Flush()
	{
		int i;
		for( i = 0; i < lMeshCount; ++i )
		{
			SAFE_DELETEA( pszMeshName[i] );
		}
		SAFE_DELETEA( pszMeshName );

		for( i = 0; i < lTexCount; ++i )
		{
			SAFE_DELETEA( pszTexName[i] );
		}
		SAFE_DELETEA( pszTexName );

		SAFE_DELETEA( szSetItemName );
		SAFE_DELETEA( szItemName );
	}

	H3DOutfitElement()
	{
		szSetItemName = NULL;	// 이 아이템은 설정되지 않을수도 있으니 초기화해준다.
	}

	char* szItemName;
	char* szSetItemName;
	long lMeshCount;
	long lTexCount;
	char** pszMeshName;
	char** pszTexName;
};


class CH3DOutfitTable  
{
public:
	CH3DOutfitTable();
	~CH3DOutfitTable();

	bool Load( const char* szFileName );
	bool GetOutfit( H3DOutfitElement* &pOutfit, char* szOutfitName );

protected:
	bool AddItem( H3DOutfitElement &outfit );

	std::map<char*, H3DOutfitElement, szi_less> m_mapName2Outfit;
	//std::vector<char*> m_vec_szOutfitItemName;
};

#endif // !defined(AFX_H3DOUTFITTABLE_H__145CF134_819E_4958_8C2D_7413F1D28089__INCLUDED_)
