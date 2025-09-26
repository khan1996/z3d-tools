// H3DWeaponTable.h: interface for the CH3DWeaponTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H3DWEAPONTABLE_H__AAFF1DDB_05DD_4D2C_BAA4_F0483EBC4503__INCLUDED_)
#define AFX_H3DWEAPONTABLE_H__AAFF1DDB_05DD_4D2C_BAA4_F0483EBC4503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc.h"
#include "Z3D_CONSTANTS.h"
#include "vector.h"
#include "quaternion.h"

#include <map>


struct H3DWeaponInfo
{
	void Flush()
	{
		SAFE_DELETEA( szTexName );
		SAFE_DELETEA( szMeshName );

		SAFE_DELETEA( szItemName );
	}

	char* szItemName;
	char* szMeshName;
	char* szTexName;

	long lWeaponType;

	Z3D_CHR_SKELETONPART hskel;
	vector3 vecHpos;		// position in hand
	quaternion quatHrot;	// rotation in hand

	Z3D_CHR_SKELETONPART skel;
	vector3 vecPos;
	quaternion quatRot;
};


class CH3DWeaponTable  
{
public:
	CH3DWeaponTable();
	virtual ~CH3DWeaponTable();

	bool Load( const char* szFileName );
	bool GetWeaponInfo( H3DWeaponInfo* &pWi, char* szWeaponName );

protected:
	Z3D_CHR_SKELETONPART GetSkelPartByString( const char* sz );
	bool AddItem( H3DWeaponInfo &wi );

	std::map<char*, H3DWeaponInfo, szi_less> m_mapName2WeaponInfo;
};

#endif // !defined(AFX_H3DWEAPONTABLE_H__AAFF1DDB_05DD_4D2C_BAA4_F0483EBC4503__INCLUDED_)
