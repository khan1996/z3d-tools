// Z3DCharacterModel.cpp: implementation of the CZ3DCharacterModel class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DCharacterModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "SimpleParser.h"
#include "GMMemory.h"


// static members
std::list<CZ3DCharacterModel*> CZ3DCharacterModel::ms_list_pModel;
SimpleString CZ3DCharacterModel::ms_strDataPath;


bool CZ3DCharacterModel::_Init( const char* szDataPath )
{
	//_ASSERT( pStub );
	//ms_rpStub = pStub;
	int l = strlen( szDataPath );

	if( '\\' == szDataPath[l-1] || '/' == szDataPath[l-1] )
	{
		ms_strDataPath( szDataPath );
	}
	else
	{
		ms_strDataPath( szDataPath, "/" );
	}

	/*g_ContOutfitTable.SetFilePath( szDataPath );
	g_ContWeaponTable.SetFilePath( szDataPath );

	// preloading desired outfit tables
	H3DOutfitTableTag tagTmp;
	if( false == g_ContOutfitTable.GetObject( tagTmp, "PC_MAN.outfit" ) )
	{
		return false;
	}
	if( false == g_ContOutfitTable.GetObject( tagTmp, "PC_WOMAN.outfit" ) )
	{
		return false;
	}*/

	CZ3DGradeEffectHandler::Init();

	return true;
}

//void CZ3DCharacterModel::Close();



