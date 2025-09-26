// Z3DGCMDS.h: interface for the CZ3DGCMDS class.

//

//////////////////////////////////////////////////////////////////////



#if !defined(AFX_Z3DGCMDS_H__768AB0C3_2AF6_4B5D_BE26_8CF7E6C92F29__INCLUDED_)
#define AFX_Z3DGCMDS_H__768AB0C3_2AF6_4B5D_BE26_8CF7E6C92F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Z3D General Character Model Description Script


#include "Z3D_GLOBALS.h"
#include "Z3DSkeletonObject.h"
#include "Z3DMultipartPortion.h"
#include "SimpleParser.h"
#include "Z3DMaskedStream.h"
#include "Z3DBoundingVolumeObject.h"

#include <map>
#include <vector>


// TOK의 array. 한 캐릭터에 세팅되는 subpart갯수만큼의 TOK을 가짐
struct Z3DTOKARRAY
{
	long lCount;
	Z3DTOK* pArray;

	void Alloc( long count )
	{
		pArray = new Z3DTOK[count];
		lCount = count;
	}

	void Discard()
	{
		SAFE_DELETEA( pArray );
		lCount = 0;
	}
};


// TOKARRAY와 확률데이터. 한 motion아래 확률로 묶여있는 데이터들의 집합(설명이 더 어렵잖아-_-!)
struct Z3DMOTLET
{
	long lCount;
	Z3DTOKARRAY* pTokArr;
	long* pProbabilityIdx;
	long lProbalilitySum;

	void Alloc( long count )
	{
		pTokArr = new Z3DTOKARRAY[count];
		pProbabilityIdx = new long[count];
		lCount = count;
	}

	~Z3DMOTLET()
	{
		SAFE_DELETEA( pProbabilityIdx );
		for( int i = 0; i < lCount; ++i )
		{
			pTokArr[i].Discard();
		}
		SAFE_DELETEA( pTokArr );
		lCount = 0;
	}
};

struct Z3DMOTIONSHEET
{
	std::map<Z3DTOK, Z3DMOTLET*> mapTok2Motlet;
	Z3DMOTIONSHEET* rpParentSheet;

	~Z3DMOTIONSHEET()
	{
		std::map<Z3DTOK, Z3DMOTLET*>::iterator it;

		for( it = mapTok2Motlet.begin(); it != mapTok2Motlet.end(); it++ )
		{
			SAFE_DELETE( it->second );
		}
	}
};

struct Z3DATTACHMENTHOLDERINFO
{
	Z3DTOK tokSkelPart;
	vector3 vPos;
	quaternion qRot;	
};

struct Z3DATTACHMENTINFO
{
	long lType;

	Z3DTOK tokMeshFile;
	Z3DTOK tokTextureFile;
	Z3DTOK tokTextureFile2;

	Z3DATTACHMENTHOLDERINFO* pHolderInfo;
	long lHolderCount;

	~Z3DATTACHMENTINFO()
	{
		SAFE_DELETEA( pHolderInfo );
	}

	const Z3DATTACHMENTHOLDERINFO* GetAttachmentHolderInfo( long lIdx ) const
	{
		if( !(lIdx < lHolderCount) )
		{
			return pHolderInfo;	// returning index 0
		}

		return &(pHolderInfo[lIdx]);
	}
};

struct Z3DOUTFITINFO
{
	long lCount;
	Z3DTOK tokLinkOutfit;	// 연결되는 outfit의 이름
	Z3DTOK tokLinkSlot;		// 연결되는 outfit이 세팅될 slot이름
	Z3DTOK* pTokMeshName;
	Z3DTOK* pTokTextureName;
	Z3DTOK* pTokTexture2Name;

	void Alloc( long c )
	{
		lCount = c;
		pTokMeshName = new  Z3DTOK[c];
		pTokTextureName = new Z3DTOK[c];
		pTokTexture2Name = new Z3DTOK[c];
	}

	~Z3DOUTFITINFO()
	{
		SAFE_DELETEA( pTokMeshName );
		SAFE_DELETEA( pTokTextureName );
		SAFE_DELETEA( pTokTexture2Name );
	}
};


struct Z3DTOK_SLOT_ATTACHMENT
{
	Z3DTOK tokSlot;
	Z3DTOK tokAttachment;
};


struct Z3D_BOUNDING_CYLINDER_INFO
{
	long lBottomIdx, lTopIdx;
	float fBottomRadius, fTopRadius;
};


#define Z3D_MAX_SHADER_COUNT	6
//#define Z3D_MAX_SHADER_CONSTANT_COUNT	10

/*struct Z3D_SHADER_PARAMETER_INFO
{
	Z3D_SHADER_PARAMETER_INFO()
	{
		for( int i = 0; i < Z3D_MAX_SHADER_CONSTANT_COUNT; ++i )
		{
			afConstants[i] = 0.0f;
		}
	}

	float GetParam( long lIdx ) const
	{
		if( lIdx < Z3D_MAX_SHADER_CONSTANT_COUNT )
		{
			return afConstants[lIdx];
		}
		else
		{
			return 0.0f;
		}
	}

	float afConstants[Z3D_MAX_SHADER_CONSTANT_COUNT];
};*/


struct Z3D_EFFECT_INFO
{
	char* szESFName;
	char* szSkelName;
	//vector3 vPos;
	//quaternion qRot;

	Z3D_EFFECT_INFO()
	{
		szESFName = NULL;
		szSkelName = NULL;
	}

	~Z3D_EFFECT_INFO()
	{
		SAFE_DELETEA( szSkelName );
		SAFE_DELETEA( szESFName );
	}
};


struct Z3D_BOUNDINGVOLUME_INFO
{
	enum Z3D_BOUNDINGVOLUME_TYPE{ Z3DBV_OBB, Z3DBV_CYLINDER, Z3DBV_SPHERE } eBVType;

	long lSkelIndex;	// index to skeleton this BV sticked to

	vector3 vOrigin;	// origin spot relative to its parent object(origin coordinate in local space)
	vector3 vExtent;	// 육면체 길이의 절반(중점으로부터의 크기)
	//vector3 avAxes[3];
	quaternion qRot;
};

class CZ3DGCMDS  
{
public:
	CZ3DGCMDS();
	virtual ~CZ3DGCMDS();

	const char* GetNameString() const
	{
		return m_szFileName;
	}

	bool Load( const char* szFileName, const char* szPath = NULL );
	bool Save( const char* szFileName = NULL, const char* szPath = NULL ) const;
	
	long GetSkeletonPartObjectIdx( const char* szSkeletonPartName );
	long GetSkeletonPartObjectIdx( Z3DTOK tokSkeletonPartName );

	const matrix* GetLocalizerMatrix()
	{
		return m_pLocalizerMatrix;
	}

	long GetSkeletonCount() const
	{
		return m_lSkeletonCount;
	}

	const long* GetSkeletonHierarchy() const
	{
		return m_pSkeletonHierarchy;
	}

	long GetAniHolderCount() const
	{
		return m_lAniHolderCount;
	}

	bool AllocSkeleton( CZ3DSkeletonObject* &pSkeleton );

	Z3D_MULTIPART_PORTION_TYPE GetMPT()
	{
		return m_MPT;
	}

	const std::vector<Z3DTOK>* GetStaticSlotVector()
	{
		return &m_vecTokStaticSlot;
	}

	const std::vector<Z3DTOK>* GetOutfitSlotVector()
	{
		return &m_vecTokOutfitSlot;
	}

	const std::vector<long>* GetOutfitSlotOverlayVector()
	{
		return &m_vecOutfitSlotOverlayCount;
	}

	const std::vector<Z3DTOK>* GetAttachmentSlotVector()
	{
		return &m_vecTokAttachmentSlot;
	}

	const std::vector<Z3DTOK_SLOT_ATTACHMENT>* GetAttachmentDefaultSettingVector()
	{
		return &m_vecDefaultAttachment;
	}

	const std::vector<Z3D_BOUNDING_CYLINDER_INFO>* GetBoundingCylinderVector()
	{
		return &m_vecBoundingCylinderInfo;
	}

	const Z3DOUTFITINFO* GetOutfitInfo( Z3DTOK tokOutfitName );
	const Z3DATTACHMENTINFO* GetAttachmentInfo( Z3DTOK tokAttachmentName );

	Z3DMOTIONSHEET* GetMotionsheet( Z3DTOK tokSheetName );


	bool IsFixed()
	{
		return m_bFixed;
	}

	bool IsShaderParameterAvailable( long lIdx )
	{
		if( lIdx >= Z3D_MAX_SHADER_COUNT )
		{
			return false;
		}

		if( NULL == m_aszShaderInfoList[lIdx] )
		{
			return false;
		}

		return true;
	}

	const char* GetShaderParameterInfo( long lIdx )
	{
		if( lIdx >= Z3D_MAX_SHADER_COUNT )
		{
			return NULL;
		}

		return m_aszShaderInfoList[lIdx];
	}

	//long GetEffectInfoList( const Z3D_EFFECT_INFO** &rppEffectInfo );
	long GetEffectInfoList( const char** &rpszEffectInfo );

	float GetSizeFactor();	// 캐릭터의 대략적인 키를 구하는 함수

	const char* GetSkeletonName( int idx );

	const char* GetFileName()
	{
		return m_szFileName;
	}

	// 편집용 retrieval / modification 함수군
	const char* GetSkeyFileName();
	void SetSkeyFilename( const char* szFileName );

	long GetMeshType();
	void SetMeshType( long lMeshType );

	void RetrieveSkelpartList( std::map<Z3DTOK, long>* &prSkelpartList );
	void RetrieveMotionSheetList( std::map<Z3DTOK, Z3DMOTIONSHEET*>* &prMotionsheetList );
	void RetrieveOutfitList( std::map<Z3DTOK, Z3DOUTFITINFO*>* &prOutfitList );
	void RetrieveAttachmentList( std::map<Z3DTOK, Z3DATTACHMENTINFO*>* &prAttachmentList );

	void RetrieveStaticSlot( std::vector<Z3DTOK>* &prStaticSlot );
	void RetrieveOutfitSlot( std::vector<Z3DTOK>* &prOutfitSlot, std::vector<long>* &prOSOverlapCount );
	void RetrieveAttachmentSlot( std::vector<Z3DTOK>* &prAttachmentSlot );

	Z3DTOK GetDefaultOutfit();
	void SetDefaultOutfit( Z3DTOK tokDefaultOutfit );

	Z3DTOK GetDefaultMotionSheet();
	void SetDefaultMotionSheet( Z3DTOK tok );
	
	Z3DTOK GetDefaultMotion();
	void SetDefaultMotion( Z3DTOK tok );
	//void GetDefaultMotion( Z3DTOK &rtokMotionSheet, Z3DTOK &rtokMotion );
	//void SetDefaultMotion( Z3DTOK tokMotionSheet, Z3DTOK tokMotion );

	float GetDefaultScale();
	void SetDefaultScale( float f );

	void RetrieveDefaultAttachmentList( std::vector<Z3DTOK_SLOT_ATTACHMENT>* &prDefaultAttachmentList );

	//void RetrieveEffectInfoList( std::vector<Z3D_EFFECT_INFO*>* &prEffectInfoList );
	void RetrieveEffectInfoList( std::vector<char*>* &prEffectInfoList );

	float GetEffectScale();
	void SetEffectScale( float f );
	
	Z3DBlend2Vertex& GetFixedPoint( int nIdx );

	void AllocBoundingVolumeObject( CZ3DSkeletonObject* pSkels, std::vector<CZ3DBoundingVolumeObject*>& vec_pBV );
	int BoundingVolumeIndex2SkelIndex( int nBVIndex );
	std::vector<Z3D_BOUNDINGVOLUME_INFO>& GetBoundingVolumeInfoVector()
	{
		return m_vecBoundingVolumeInfo;
	}

	bool IsAlphaUsed()
	{
		return m_bIsAlphaUsed;
	}
	
	float GetNameTagBias()
	{
		return m_fNameTagBias;
	}





protected:
	long m_lSkeletonCount;
	long m_lAniHolderCount;
	long* m_pSkeletonHierarchy;
	long* m_pAniHolderIndex;
	long* m_pIndexInSubpart;
	char** m_pszSkeletonName;
	matrix* m_pLocalizerMatrix;
	std::vector<Z3D_BOUNDINGVOLUME_INFO> m_vecBoundingVolumeInfo;



	Z3D_MULTIPART_PORTION_TYPE m_MPT;

	std::map<Z3DTOK, long> m_mapTok2SkelpartIdx;
	std::map<Z3DTOK, Z3DMOTIONSHEET*> m_mapTok2MotionSheet;

	std::vector<Z3DTOK> m_vecTokStaticSlot;
	std::vector<Z3DTOK> m_vecTokOutfitSlot;
	std::vector<long> m_vecOutfitSlotOverlayCount;
	std::vector<Z3DTOK> m_vecTokAttachmentSlot;

	std::vector<Z3D_BOUNDING_CYLINDER_INFO> m_vecBoundingCylinderInfo;

	std::map<Z3DTOK, Z3DOUTFITINFO*> m_mapTok2OutfitInfo;
	std::map<Z3DTOK, Z3DATTACHMENTINFO*> m_mapTok2AttachmentInfo;
	bool m_bFixed;	// outfit이 하나밖에 없는 model은 fixed model로 취급되어 outfit과 attachment 변경이 제한됨
	
	Z3DTOK m_TokDefaultOutfit;
	Z3DTOK m_TokDefaultMotionSheet;
	Z3DTOK m_TokDefaultMotion;

	std::vector<Z3DTOK_SLOT_ATTACHMENT> m_vecDefaultAttachment;

	float m_fDefaultScale;
	float m_fEffectScale;

	char* m_szSkelFileName;
	char* m_szFileName;

	char* m_aszShaderInfoList[Z3D_MAX_SHADER_COUNT];

	//std::vector<Z3D_EFFECT_INFO*> m_vecEffectInfo;
	std::vector<char*> m_vecszEffectInfo;
	
	Z3DBlend2Vertex m_aFixedPoint[3];

	bool m_bIsAlphaUsed;
	
	float m_fNameTagBias;


	bool ReadSkelFile( const char* szFilename );
	bool ReadSkeyFile( const char* szFilename );
	bool ReadSkeletonPartInfo( CSimpleParser &rParser );
	bool ReadBoundingCylinderInfo( CSimpleParser &rParser );
	bool ReadMotionSheet( CSimpleParser &rParser );
	bool ReadStaticSlot( CSimpleParser &rParser );
	bool ReadOutfitSlot( CSimpleParser &rParser );
	bool ReadAttachementSlot( CSimpleParser &rParser );
	bool ReadAttachmentSet( CSimpleParser &rParser );
	bool ReadAttachmentHolder( CSimpleParser &rParser, std::vector<Z3DATTACHMENTHOLDERINFO> &r_vecHolder );
	bool ReadOutfitSet( CSimpleParser &rParser );
	bool ReadDefaultAttachment( CSimpleParser &rParser );
	bool ReadShaderInfo( CSimpleParser &rParser );
	bool ReadEffectInfo( CSimpleParser &rParser );
	bool ReadFixedPoint( CSimpleParser &rParser );
	bool WriteMotionSheet( CZ3DMaskedStream &rMstrm ) const;
};


#endif // !defined(AFX_Z3DGCMDS_H__768AB0C3_2AF6_4B5D_BE26_8CF7E6C92F29__INCLUDED_)