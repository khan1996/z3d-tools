// H3DWeaponTable.cpp: implementation of the CH3DWeaponTable class.
//
//////////////////////////////////////////////////////////////////////

#include "H3DWeaponTable.h"
#include "SimpleParser.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH3DWeaponTable::CH3DWeaponTable()
{
}


CH3DWeaponTable::~CH3DWeaponTable()
{
	std::map<char*, H3DWeaponInfo, szi_less>::iterator it;

	for( it = m_mapName2WeaponInfo.begin(); it != m_mapName2WeaponInfo.end(); it++ )
	{
		it->second.Flush();
	}
}


Z3D_CHR_SKELETONPART CH3DWeaponTable::GetSkelPartByString( const char* sz )
{
	if( 0 == stricmp( "HEAD", sz) )
	{
		return Z3D_SP_HEAD;
	}
	else if( 0 == stricmp( "NECK", sz) )
	{
		return Z3D_SP_NECK;
	}
	else if( 0 == stricmp( "CHEST", sz) )
	{
		return Z3D_SP_CHEST;
	}
	else if( 0 == stricmp( "WAIST", sz) )
	{
		return Z3D_SP_WAIST;
	}
	else if( 0 == stricmp( "PELVIS", sz) )
	{
		return Z3D_SP_PELVIS;
	}
	else if( 0 == stricmp( "R_HAND", sz) )
	{
		return Z3D_SP_R_HAND;
	}
	else if( 0 == stricmp( "R_FOREARM", sz) )
	{
		return Z3D_SP_R_FOREARM;
	}
	else if( 0 == stricmp( "R_UPPERARM", sz) )
	{
		return Z3D_SP_R_UPPERARM;
	}
	else if( 0 == stricmp( "R_THIGH", sz) )
	{
		return Z3D_SP_R_THIGH;
	}
	else if( 0 == stricmp( "R_CALF", sz) )
	{
		return Z3D_SP_R_CALF;
	}
	else if( 0 == stricmp( "R_FOOT", sz) )
	{
		return Z3D_SP_R_FOOT;
	}
	else if( 0 == stricmp( "L_HAND", sz) )
	{
		return Z3D_SP_L_HAND;
	}
	else if( 0 == stricmp( "L_FOREARM", sz) )
	{
		return Z3D_SP_L_FOREARM;
	}
	else if( 0 == stricmp( "L_UPPERARM", sz) )
	{
		return Z3D_SP_L_UPPERARM;
	}
	else if( 0 == stricmp( "L_THIGH", sz) )
	{
		return Z3D_SP_L_THIGH;
	}
	else if( 0 == stricmp( "L_CALF", sz) )
	{
		return Z3D_SP_L_CALF;
	}
	else if( 0 == stricmp( "L_FOOT", sz) )
	{
		return Z3D_SP_L_FOOT;
	}

	return Z3D_SP_NONE;
}


bool CH3DWeaponTable::Load( const char* szFileName )
{
	CSimpleParser parser;

	if( false == parser.OpenFile( szFileName ) )
	{
		return false;	// file open failed
	}

	H3DWeaponInfo wi;
	char* szToken;

	long lState;	// 0 : reading weapon name, 1 : reading weapon info

	lState = 0;
	while( (szToken = parser.GetNextToken()) != NULL )
	{
		switch( lState )
		{
		case 0:
			wi.szItemName = new char[strlen(szToken)+1];
			strcpy( wi.szItemName, szToken );
			lState = 1;
			break;

		case 1:
			if( 0 == stricmp( "Mesh", szToken ) )
			{
				szToken = parser.GetNextToken();
				wi.szMeshName = new char[strlen(szToken)+1];
				strcpy( wi.szMeshName, szToken );
			}
			else if( 0 == stricmp( "Tex", szToken ) )
			{
				szToken = parser.GetNextToken();
				wi.szTexName = new char[strlen(szToken)+1];
				strcpy( wi.szTexName, szToken );
			}
			else if( 0 == stricmp( "Type", szToken ) )
			{
				wi.lWeaponType = aton( parser.GetNextToken() );
				if( wi.lWeaponType > 9 )
				{
					return false;
				}
			}
			else if( 0 == stricmp( "HSkel", szToken ) )
			{
				wi.hskel = GetSkelPartByString( parser.GetNextToken() );
			}
			else if( 0 == stricmp( "HPos", szToken ) )
			{
				wi.vecHpos.x = (float)atof( parser.GetNextToken() );
				wi.vecHpos.y = (float)atof( parser.GetNextToken() );
				wi.vecHpos.z = (float)atof( parser.GetNextToken() );
			}
			else if( 0 == stricmp( "HRot", szToken ) )
			{
				wi.quatHrot.x = (float)atof( parser.GetNextToken() );
				wi.quatHrot.y = (float)atof( parser.GetNextToken() );
				wi.quatHrot.z = (float)atof( parser.GetNextToken() );
				wi.quatHrot.w = (float)atof( parser.GetNextToken() );
			}
			else if( 0 == stricmp( "Skel", szToken ) )
			{
				wi.skel = GetSkelPartByString( parser.GetNextToken() );
			}
			else if( 0 == stricmp( "Pos", szToken ) )
			{
				wi.vecPos.x = (float)atof( parser.GetNextToken() );
				wi.vecPos.y = (float)atof( parser.GetNextToken() );
				wi.vecPos.z = (float)atof( parser.GetNextToken() );
			}
			else if( 0 == stricmp( "Rot", szToken ) )
			{
				wi.quatRot.x = (float)atof( parser.GetNextToken() );
				wi.quatRot.y = (float)atof( parser.GetNextToken() );
				wi.quatRot.z = (float)atof( parser.GetNextToken() );
				wi.quatRot.w = (float)atof( parser.GetNextToken() );
			}
			else if( 0 == stricmp( ";", szToken ) )
			{
				AddItem( wi );
				lState = 0;
			}
			break;
		}
		
	}
	
	return true;
}


bool CH3DWeaponTable::AddItem( H3DWeaponInfo &wi )
{
	if( m_mapName2WeaponInfo.end() != m_mapName2WeaponInfo.find( wi.szItemName ) )
	{
		// 해당 아이템 이름으로 이미 등록된 항목이 있다.
		return false;
	}

	m_mapName2WeaponInfo[wi.szItemName] = wi;

	return true;
}


bool CH3DWeaponTable::GetWeaponInfo( H3DWeaponInfo* &pWi, char* szWeaponName )
{
	std::map<char*, H3DWeaponInfo, szi_less>::iterator it;

	it = m_mapName2WeaponInfo.find( szWeaponName );
	if( m_mapName2WeaponInfo.end() == it )
	{
		return false;
	}

	pWi = &(it->second);
	return true;
}