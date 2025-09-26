// H3DOutfitTable.cpp: implementation of the CH3DOutfitTable class.
//
//////////////////////////////////////////////////////////////////////

#include "H3DOutfitTable.h"
#include "SimpleParser.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH3DOutfitTable::CH3DOutfitTable()
{
}

CH3DOutfitTable::~CH3DOutfitTable()
{
	std::map<char*, H3DOutfitElement, szi_less>::iterator it;

	for( it = m_mapName2Outfit.begin(); it != m_mapName2Outfit.end(); it++ )
	{
		it->second.Flush();
	}
}


bool CH3DOutfitTable::Load( const char* szFileName )
{
	CSimpleParser parser;

	if( false == parser.OpenFile( szFileName ) )
	{
		return false;	// file open failed
	}

	int i;

	char* szToken;
	char* szTmp;

	char szItemName[260];
	std::vector<char*> vec_szMeshName;
	std::vector<char*> vec_szTexName;
	long lState = 0;
	/* lState -
		0 : item name read mode
		1 : Mesh name read mode
		2 : Tex name read mode
		3 : Set item name read mode	*/
	H3DOutfitElement oel;
	while( (szToken = parser.GetNextToken()) != NULL )
	{
		switch( lState )
		{
		case 0:
			if( 0 == stricmp( szToken, "Mesh" ) )
			{
				// Mesh name read mode 로 변환
				lState = 1;
			}
			else if( 0 == stricmp( szToken, "Tex" ) )
			{
				// Tex name read mode 로 변환
				lState = 2;
			}
			else if( 0 == stricmp( szToken, "Set" ) )
			{
				// Set item name read mode 로 변환
				lState = 3;
			}
			else
			{
				// 이도저도 아니면 아이템 이름 읽기임.
				strcpy( szItemName, szToken );	// 아이템 이름에 해당하는 문자열 복사
			}
			break;

		case 1:
			if( 0 == stricmp( szToken, ";" ) )
			{
				// Mesh name read mode의 마지막

				// outfit element 의 mesh 필드 값 세팅
				oel.lMeshCount = vec_szMeshName.size();
				oel.pszMeshName = new char*[vec_szMeshName.size()];
				for( i = 0; i < (int)vec_szMeshName.size(); ++i )
				{
					oel.pszMeshName[i] = vec_szMeshName[i];
				}

				// 모드 전환
				lState = 0;
			}
			else
			{
				// mesh name을 임시 vector에 저장
				szTmp = new char[strlen(szToken)+1];
				strcpy( szTmp, szToken );
				vec_szMeshName.push_back( szTmp );
			}
			break;

		case 2:
			if( 0 == stricmp( szToken, ";" ) )
			{
				// Tex name read mode의 마지막

				// outfit element 의 tex 필드 값 세팅
				oel.lTexCount = vec_szTexName.size();
				oel.pszTexName = new char*[vec_szTexName.size()];
				for( i = 0; i < (int)vec_szTexName.size(); ++i )
				{
					oel.pszTexName[i] = vec_szTexName[i];
				}
				oel.szItemName = new char[strlen(szItemName)+1];
				strcpy( oel.szItemName, szItemName );

				// 또한 한 아이템에 대한 읽기의 마지막이므로,
				// 아이템을 목록에 추가하고,
				AddItem( oel );

				// 임시저장용 vector해제
				vec_szMeshName.clear();
				vec_szTexName.clear();

				oel.szSetItemName = NULL;

				// 모드 전환
				lState = 0;
			}
			else
			{
				// mesh name을 임시 vector에 저장
				szTmp = new char[strlen(szToken)+1];
				strcpy( szTmp, szToken );
				vec_szTexName.push_back( szTmp );
			}
			break;

		case 3:
			if( 0 == stricmp( szToken, ";" ) )
			{
				lState = 0;
			}
			else
			{
				oel.szSetItemName = new char[strlen(szToken)+1];
				strcpy( oel.szSetItemName, szToken );
			}

		}
	}

	return true;
}


bool CH3DOutfitTable::AddItem( H3DOutfitElement &outfit )
{
	if( m_mapName2Outfit.end() != m_mapName2Outfit.find( outfit.szItemName ) )
	{
		// 해당 아이템 이름으로 이미 등록된 항목이 있다.
		return false;
	}

	m_mapName2Outfit[outfit.szItemName] = outfit;

	return true;
}


bool CH3DOutfitTable
	::GetOutfit( H3DOutfitElement* &pOutfit, char* szOutfitName )
{
	std::map<char*, H3DOutfitElement, szi_less>::iterator it;

	it = m_mapName2Outfit.find( szOutfitName );
	if( m_mapName2Outfit.end() == it )
	{
		/*outfit.lMeshCount = 0;
		outfit.lTexCount = 0;
		outfit.pszMeshName = NULL;
		outfit.pszTexName = NULL;*/
		pOutfit = NULL;

		return false;
	}

	pOutfit = &(it->second);
	return true;
}