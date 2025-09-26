// Z3DGeneralChrModel.h: interface for the CZ3DGeneralChrModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DGENERALCHRMODEL_H__B38196D0_93FD_49A6_85A7_6E726B0C53A1__INCLUDED_)
#define AFX_Z3DGENERALCHRMODEL_H__B38196D0_93FD_49A6_85A7_6E726B0C53A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DCharacterModel.h"
#include "Z3DGCMDS.h"
#include "Z3DMultipartSkin.h"
#include "Z3DSkeletonObject.h"
#include "Z3DChrEventGenerator.h"
#include "Z3DWeapon.h"
#include "Z3DAniMixer.h"
#include "map"
#include "vector"


enum Z3D_CHARACTER_RENDER_OVERRIDE
{
	Z3DRO_NONE = 0, Z3DRO_STONE
};

struct Z3DTOK_N_GRADE
{
	Z3DTOK tok;
	long lGradeEffectIndex;	// 등급별 이펙트용.  0 = none
	long lGlowIndex;	// glow. 0 = none
};

struct Z3DTOK_OVERLAY_SLOT
{
	long lCount;	// 해당 슬롯의 최대 겹침가능수
	Z3DTOK_N_GRADE* pArray;
	long lUsed;		// 해당 슬롯에 현재 누적된 겹침수


	void Alloc( long count )
	{
		pArray = new Z3DTOK_N_GRADE[count];
		for( int i = 0; i < count; ++i )
		{
			pArray[i].tok = NULL_TOK;
			pArray[i].lGradeEffectIndex = 0;
			pArray[i].lGlowIndex = 0;
		}
		lCount = count;
		lUsed = 0;
	}

	~Z3DTOK_OVERLAY_SLOT()
	{
		SAFE_DELETEA( pArray );
		lCount = 0;
	}
};


struct Z3D_BOUNDING_CYLINDER
{
	vector3 vBottom, vTop;
	float fBottomRadius, fTopRadius;
};


class CZ3DGeneralChrModel : public CZ3DCharacterModel  
{
public:
	CZ3DGeneralChrModel();
	virtual ~CZ3DGeneralChrModel();

	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// 처리되지 않고 쌓여있는 이벤트들을 flush (CharacterLightShadowManager에서 호출)
	void FlushAccumulatedEvents();

	void GetFixedPoint( int nIdx, vector3& v_out  );

	void GetNameTagPosition( vector3& vPos );

	bool IsAlphaUsed()
	{
		if( NULL == m_rpGCMDS )
		{
			return false;
		}

		return m_rpGCMDS->IsAlphaUsed();
	}	


	const CZ3DGCMDS* GetGCMDSInterface() const
	{
		if( this != m_rpThis )
		{
			return NULL;
		}

		return m_rpGCMDS;
	}

	const char* GetGCMDSFileName() const
	{
		if( this != m_rpThis )
		{
			return NULL;
		}

		if( m_rpGCMDS )
		{
			return m_rpGCMDS->GetNameString();
		}

		return NULL;
	}

	float GetScale()
	{
		if( this != m_rpThis )
		{
			return 1.0f;
		}

		return m_fScaleFactor;
	}

	void SetScale( float f );

	float GetEffectScale();

	void SetLODLevel( long l )
	{
		if( this != m_rpThis )
		{
			return;
		}

		if( m_pSkin )
		{
			m_pSkin->SetLODLevel( l );
		}
	}

	void SetDisable( bool bDisabled );

	void SetCull( bool bCulled );
	bool IsCulled()
	{
		if( this != m_rpThis )
		{
			return true;
		}

		return m_pSkin->IsCulled();
	}

//	void Pause( bool bPause )
//	{
//		if( this != m_rpThis )
//		{
//			return;
//		}
//
//		m_bDisabled = bPause;
//	}


	virtual bool Init( const char* szGCMDSname );

	CZ3DObject* GetSkeletonPartObject( const char* szSkeletonPartName );
	CZ3DObject* GetSkeletonPartObject( Z3DTOK tokSkeletonPartName );
	CZ3DObject* GetSkeletonByIndex( int nIDx );

	CZ3DSkeletonObject* GetSkeletonSet( long* pN )
	{
		if( this != m_rpThis )
		{
			return NULL;
		}

		if( NULL == m_rpGCMDS )
		{
			if( pN )
			{
				*pN = 0;
			}
			return NULL;
		}

		if( pN )
		{
			*pN = m_rpGCMDS->GetSkeletonCount();
		}

		return m_pSkeletonObjects;
	}

	bool SetMotionSheet( const char* szSheetName );
	bool SetMotionSheet( Z3DTOK tokSheetName );

	const char* GetCurrentMotionSheetName();

	void SetMotion( const char* szMotionName, unsigned long ulFlag = 0, bool bTransit = true, long lRandomIndex = -1 );
	void SetMotion( Z3DTOK tokMotionName, unsigned long ulFlag = 0, bool bTransit = true , long lRandomIndex = -1 );

	void AddMotion( const char* szMotionName, unsigned long ulFlag = 0, float fFactorPeak = 0.5f, bool bSustainLast = false );
	void AddMotion( Z3DTOK tokMotionName, unsigned long ulFlag = 0, float fFactorPeak = 0.5f, bool bSustainLast = false  );

	float GetMotionCurrentFrame( int n )
	{
		if( this != m_rpThis )
		{
			return 0.0f;
		}

		if( n < m_rpGCMDS->GetAniHolderCount() )
		{
			return m_pAniHolder[n].GetCurrentFrame();
		}

		return 0.0f;
	}

	float GetAddonMotionCurrentFrame( int n )
	{
		if( this != m_rpThis )
		{
			return 0.0f;
		}

		if( n < m_rpGCMDS->GetAniHolderCount() )
		{
			return m_pAddonAniHolder[n].GetCurrentFrame();
		}

		return 0.0f;
	}

	void SetMotionCurrentFrame( int n, float fFrame )
	{
		if( this != m_rpThis )
		{
			return;
		}

		if( n < m_rpGCMDS->GetAniHolderCount() )
		{
			m_pAniHolder[n].SetCurrentFrame( fFrame );
		}
	}

	bool SetStaticSlot( const char* szSlotName, const char* szOutfitName );
	void BeginOutfitSetting();
	bool SetOutfitSlot( const char* szSlotName, const char* szOutfitName, long lGradeEffectIndex = 0, long lGlowIndex = 0 );
	void EndOutfitSetting();

	bool SetAttachmentSlot( const char* szSlotName, const char* szAttachmentName, long lGradeEffectIndex = 0, long lGlowIndex = 0, bool bForced = false );
	bool SetAttachmentSlot( Z3DTOK tokSlotName, Z3DTOK tokAttachmentName, long lGradeEffectIndex, long lGlowIndex, bool bForced = false );

	bool SetAttachmentHolder( const char* szSlotName, long lIndex );
	bool SetAttachmentHolder( Z3DTOK tokSlotName, long lIndex );

	long GetAttachmentHolder( const char* szSlotName );
	long GetAttachmentHolder( Z3DTOK tokSlotName );

	CZ3DWeapon* GetAttachmentObject( const char* szSlotName );
	CZ3DWeapon* GetAttachmentObject( Z3DTOK tokSlotName );
	

	bool IsActionFinished(int n)
	{
		if( this != m_rpThis )
		{
			return true;
		}

		//_ASSERT( n < m_rpGCMDS->GetAniHolderCount() );
		if( !(n < m_rpGCMDS->GetAniHolderCount()) )
		{
			return true;	// 동작이 시작한적도 없으니 당근 끝나있음으로 리턴
		}

		return m_pAniHolder[n].IsOverflow();
	}

	bool IsAddonActionFinished(int n)
	{
		if( this != m_rpThis )
		{
			return true;
		}

		//_ASSERT( n < m_rpGCMDS->GetAniHolderCount() );
		if( !(n < m_rpGCMDS->GetAniHolderCount()) )
		{
			return true;	// 동작이 시작한적도 없으니 당근 끝나있음으로 리턴
		}

		return m_pAddonAniHolder[n].IsOverflow();
	}


	void SetAlphaLevel( float f )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_pSkin->SetTransparency( f );
	}

	bool SetAnimationSpeed( float f, long lIndex = -1 )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		if( -1 == lIndex )
		{
			for( int i = 0; i < m_rpGCMDS->GetAniHolderCount(); i++ )
			{
				m_pAniHolder[i].SetSpeedFactor( f );
				m_pAddonAniHolder[i].SetSpeedFactor( f );
			}
		}
		else
		{
			if( !(lIndex < m_rpGCMDS->GetAniHolderCount()) )
			{
				return false;
			}

			m_pAniHolder[lIndex].SetSpeedFactor( f );
			m_pAddonAniHolder[lIndex].SetSpeedFactor( f );
		}

		return true;
	}

	bool CheckEvent( const char* szEventName, Z3DTOK* p_tokSecondaryEvent = NULL );
	bool CheckCollision( const CZ3DOBB& rOBB );

	virtual void Act( Z3D_CHR_ACTION action, long lParam1 = 0, long lParam2 = 0 );

	virtual void FrameMove();

	void Render();
	virtual void DrawShadow( IDirect3DDevice8* pDevice, DWORD vertexShader = 0xFFFFFFFF );
	void RenderGlowShape( IDirect3DDevice8* pDevice );

	void PreRender();

	void GetAniDataDisplacement( vector3 &v_disp, int n );
	long GetAniDataLength( int n );

	void GetBoundingBox( vector3& r_vmin, vector3& r_vmax );

	const CZ3DMultipartSkin* GetSkinInterface()
	{
		if( this != m_rpThis )
		{
			return NULL;
		}

		return m_pSkin;
	}

	long GetBoundingCylinderCount()
	{
		if( this != m_rpThis )
		{
			return 0;
		}

		return static_cast<long>(m_rpGCMDS->GetBoundingCylinderVector()->size());
	}

	const Z3D_BOUNDING_CYLINDER* GetBoundingCylinderList();


	bool IsShaderParameterAvailable( long lIdx );
	const char* GetShaderParameterInfo( long lIdx );

	long GetEffectInfoList( const char** &rpszEffectInfo );

	float GetSizeFactor()
	{
		if( this != m_rpThis )
		{
			return 1.0f;
		}

		if( m_rpGCMDS )
		{
			return m_rpGCMDS->GetSizeFactor();
		}

		return 0.0f;
	}

	void SetAniTestMode( bool b )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_bAniTestMode = b;
		if( m_pSkin )
		{
			m_pSkin->SetTestMode( b );
		}
	}

	bool HasBoundingVolumeInfo();


	static bool _BuildCMDSList( const char* szFileSpec = "*.GCMDS" );
	static bool _ClearCMDSList();
	static void _RetrieveCMDSNameList( std::vector<const char*>& r_vecList );
	static CZ3DGCMDS* _GetGCMDSByName( const char* szFilename );
	//wizardbug
	void SetHouse(float x,float y,float z,char *strName);
	bool GetHouse(D3DXVECTOR3 &vecPos,char *strName);
	void SetHouseSkip(bool bSkip);

	bool GetFixHeight() { return m_bFixHeight; }
	void SetFixHeight(bool bFix) { m_bFixHeight = bFix; }
	float m_fFixHeight;
	void SetNPC(bool bFlag) { m_bNPC = bFlag;}
	bool GetNPC() { return m_bNPC;}


	void Pause( bool b )
	{
		m_bPaused = b;
	}
	
	bool IsPaused()
	{
		return m_bPaused;
	}

	void SetRenderOverride( int n );


protected:
	Z3DTexture* m_rpRenderOverrideTexture;	// texture for rendering override
	int m_nRenderOverride;	// rendering override flag ( none/stone/... )

	bool m_bPaused;

	//wizardbug
	bool m_bFixHeight;
	bool m_bNPC;

	CZ3DGeneralChrModel* m_rpThis;

	std::vector<int> m_vec_nCollisionSkeletonIndex;

	std::vector< CZ3DBoundingVolumeObject* > m_vec_pBoundingVolumeObject;

	bool m_bAniTestMode;
	bool m_bInitialized;
	CZ3DAniMixer m_AniMixer;

	float m_fScaleFactor;
	float m_fMonsterScriptEffectScale;
	std::vector<Z3DTOK> m_vec_tokEventStorage;
	std::vector<Z3DTOK> m_vec_tokSecondaryEventStorage;

	CZ3DGCMDS* m_rpGCMDS;
	const Z3DMOTIONSHEET* m_rpCurrentMotionSheet;
	Z3DTOK m_tokCurrentMotionSheet;

	CZ3DSkeletonObject* m_pSkeletonObjects;
	CZ3DMultipartSkin* m_pSkin;

	H3DAniKeyPackTag* m_pTagAniKeyPack;
	Z3DAniHolder* m_pAniHolder;
	CZ3DChrEventGenerator* m_pEventGenerator;

	H3DAniKeyPackTag* m_pTagAddonAniKeyPack;
	Z3DAniHolder* m_pAddonAniHolder;
	CZ3DChrEventGenerator* m_pAddonEventGenerator;

	std::map<Z3DTOK, Z3DTOK> m_mapStaticSlot;
	std::map<Z3DTOK, Z3DTOK_OVERLAY_SLOT*> m_mapOutfitSlot;

	Z3D_BOUNDING_CYLINDER* m_pBoundingCylinder;

	struct Z3D_TOK_N_ATTACHMENT
	{
		Z3DTOK tokAttachmentName;
		CZ3DWeapon* pAttachment;
		long lHolderIndex;
	};
	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT> m_mapAttachmentSlot;


	bool SetAniKeyPack( int n, Z3DTOK tokFileName, bool bTransit );
	bool SetAddonAniKeyPack( int n, Z3DTOK tokFileName, float fFactorPeak );

	void ApplyOutfit( Z3DTOK tokOutfitName, long lGradeEffectIndex, long lGlowIndex );

	Z3DTOKARRAY* GetTokArray( Z3DTOK tokMotionName, long lRandomIndex = -1 );


	static std::map<Z3DTOK, CZ3DGCMDS*> ms_mapTok2GCMDS;
	// wizardbug
	bool m_bHouse;
	D3DXVECTOR3 m_vecHousePos;
	char m_strOutName[256];

};

#endif // !defined(AFX_Z3DGENERALCHRMODEL_H__B38196D0_93FD_49A6_85A7_6E726B0C53A1__INCLUDED_)
