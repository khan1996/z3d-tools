// Z3DGeneralChrModel.cpp: implementation of the CZ3DGeneralChrModel class.
//
//////////////////////////////////////////////////////////////////////
//#include "winsock2.h"
//#include "FunctionPerformanceCheck.h"
#include "Z3DGeneralChrModel.h"
#include "RenderOption.h"
#include "BaseDataDefine.h"
#include "SceneManager.h"

#include <io.h>
#include "GMMemory.h"



std::map<Z3DTOK, CZ3DGCMDS*> CZ3DGeneralChrModel::ms_mapTok2GCMDS;


bool CZ3DGeneralChrModel::_BuildCMDSList( const char* szFileSpec )
{
	SimpleString strFullPath( ms_strDataPath, szFileSpec );
	char* szFileNamePart;
	Z3DTOK tokFileName;
	CZ3DGCMDS* pGCMDS;

	_finddata_t fileinfo;
	long lFindHandle = _findfirst( strFullPath, &fileinfo );

	if( -1 == lFindHandle )
	{
		return false;
	}

	do
	{
		szFileNamePart = strrchr( fileinfo.name, '\\' );
		if( NULL == szFileNamePart )
		{
			szFileNamePart = fileinfo.name;				 
		}
		else
		{
			++szFileNamePart;
		}
/*
		if(ms_mapTok2GCMDS.size()>340)
		{
			int i = 0;
		}
*/
		tokFileName = g_TokFileName.Add( szFileNamePart );
		pGCMDS = new CZ3DGCMDS;
		if( false == pGCMDS->Load( fileinfo.name, ms_strDataPath ) )
		{
			SAFE_DELETE( pGCMDS );
			return false;
		}
		if( ms_mapTok2GCMDS.end() != ms_mapTok2GCMDS.find( tokFileName ) )
		{
			SAFE_DELETE( pGCMDS );
			return false;
		}
		ms_mapTok2GCMDS[tokFileName] = pGCMDS;

	} while( 0 == _findnext( lFindHandle, &fileinfo ) );

	_findclose( lFindHandle );
	
	return true;
}


bool CZ3DGeneralChrModel::_ClearCMDSList()
{
	std::map<Z3DTOK, CZ3DGCMDS*>::iterator it;

	for( it = ms_mapTok2GCMDS.begin(); it != ms_mapTok2GCMDS.end(); it++ )
	{
		SAFE_DELETE( it->second );
	}

	return true;
}


void CZ3DGeneralChrModel::_RetrieveCMDSNameList( std::vector<const char*>& r_vecList )
{
	std::map<Z3DTOK, CZ3DGCMDS*>::iterator it;

	for( it = ms_mapTok2GCMDS.begin(); it != ms_mapTok2GCMDS.end(); it++ )
	{
		r_vecList.push_back( g_TokFileName.GetString( it->first ) );
	}
}


CZ3DGCMDS* CZ3DGeneralChrModel::_GetGCMDSByName( const char* szFilename )
{
	Z3DTOK tokFileName = g_TokFileName.GetTOK( szFilename );
	if( NULL_TOK == tokFileName )
	{
		return false;
	}
	
	std::map<Z3DTOK, CZ3DGCMDS*>::iterator it;
	if( ms_mapTok2GCMDS.end() == (it = ms_mapTok2GCMDS.find(tokFileName)) )
	{
		return false;
	}
	
	return it->second;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DGeneralChrModel::CZ3DGeneralChrModel()
:	m_fFixHeight(0.0f), m_bFixHeight(false)
{
	m_rpRenderOverrideTexture = NULL;

	m_bPaused = false;
	m_nRenderOverride = Z3DRO_NONE;

	m_rpThis = this;

	m_pSkeletonObjects = NULL;

	m_pSkin = NULL;

	//m_pTokCurrentAniKeyPackFile = NULL;
	m_pTagAniKeyPack = NULL;
	m_pAniHolder = NULL;
	m_pEventGenerator = NULL;

	m_pTagAddonAniKeyPack = NULL;
	m_pAddonAniHolder = NULL;
	m_pAddonEventGenerator = NULL;

	m_fScaleFactor = 1.0f;
	m_fMonsterScriptEffectScale = 1.0f;

	m_rpCurrentMotionSheet = NULL;
	m_tokCurrentMotionSheet = NULL_TOK;

	m_pBoundingCylinder = NULL;

	m_bInitialized = false;

	m_bAniTestMode = false;
	m_bHouse = false;
	m_vecHousePos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	memset(m_strOutName,0,sizeof(char) * 256);
	m_fFixHeight = 0.0f;
	m_bFixHeight = false;
	m_bNPC = false;

}

CZ3DGeneralChrModel::~CZ3DGeneralChrModel()
{
	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator itA;
	for( itA = m_mapAttachmentSlot.begin(); itA != m_mapAttachmentSlot.end(); itA++ )
	{
		Z3D_TOK_N_ATTACHMENT tna = itA->second;

		if( itA->second.pAttachment )
		{
			itA->second.pAttachment->UnLink();
			SAFE_DELETE( itA->second.pAttachment );
		}
	}

	if( m_vec_pBoundingVolumeObject.size() > 0 )
	{
		for( int i = m_vec_pBoundingVolumeObject.size()-1; i >= 0; --i )
		{
			m_vec_pBoundingVolumeObject[i]->UnLink();
			SAFE_DELETE( m_vec_pBoundingVolumeObject[i] );
		}
	}

	if( m_pSkeletonObjects )
	{
		m_pSkeletonObjects[0].UnLink();
	}
	SAFE_DELETEA( m_pSkeletonObjects );
	
	SAFE_DELETE( m_pSkin );

	//SAFE_DELETEA( m_pTokCurrentAniKeyPackFile );
	SAFE_DELETEA( m_pTagAniKeyPack );
	SAFE_DELETEA( m_pAniHolder );
	SAFE_DELETEA( m_pEventGenerator );

	SAFE_DELETEA( m_pTagAddonAniKeyPack );
	SAFE_DELETEA( m_pAddonAniHolder );
	SAFE_DELETEA( m_pAddonEventGenerator );

	std::map<Z3DTOK, Z3DTOK_OVERLAY_SLOT*>::iterator itO;
	for( itO = m_mapOutfitSlot.begin(); itO != m_mapOutfitSlot.end(); itO++ )
	{
		SAFE_DELETE( itO->second );
	}

	SAFE_DELETEA( m_pBoundingCylinder );

	m_rpThis = NULL;
}


bool CZ3DGeneralChrModel::Init( const char* szGCMDSname )
{
	// edith GCMDS 로드 부분?
	if( this != m_rpThis )
	{
		return true;
	}

	if( m_bInitialized )
	{
		return false;	// trying to initialize more than one time on this instance
	}

	Z3DTOK tokFileName = g_TokFileName.GetTOK( szGCMDSname );
	if( NULL_TOK == tokFileName )
	{
		return false;
	}

	std::map<Z3DTOK, CZ3DGCMDS*>::iterator it;
	if( ms_mapTok2GCMDS.end() == (it = ms_mapTok2GCMDS.find(tokFileName)) )
	{
		return false;
	}

	m_rpGCMDS = it->second;

	// allocating skeleton obejct & setting hierarchy
	if( false == m_rpGCMDS->AllocSkeleton( m_pSkeletonObjects ) )
	{
		return false;			
	}

	m_rpGCMDS->AllocBoundingVolumeObject( m_pSkeletonObjects, m_vec_pBoundingVolumeObject );
	for( int i = 0; i < (int)m_vec_pBoundingVolumeObject.size(); ++i )
	{
		m_vec_pBoundingVolumeObject[i]->SetChrModel( this );
	}


	m_pSkin = new CZ3DMultipartSkin( m_rpGCMDS->GetMPT() );
	m_pSkin->SetSkeleton( m_pSkeletonObjects, m_rpGCMDS->GetSkeletonCount() );
	
	m_pSkin->SetSkeletonLocalizer( m_rpGCMDS->GetLocalizerMatrix() );

	//m_pTokCurrentAniKeyPackFile = new Z3DTOK[m_rpGCMDS->GetAniHolderCount()];
	m_pTagAniKeyPack = new H3DAniKeyPackTag[m_rpGCMDS->GetAniHolderCount()];
	m_pAniHolder = new Z3DAniHolder[m_rpGCMDS->GetAniHolderCount()];
	m_pEventGenerator = new CZ3DChrEventGenerator[m_rpGCMDS->GetAniHolderCount()];

	m_pTagAddonAniKeyPack = new H3DAniKeyPackTag[m_rpGCMDS->GetAniHolderCount()];
	m_pAddonAniHolder = new Z3DAniHolder[m_rpGCMDS->GetAniHolderCount()];
	m_pAddonEventGenerator = new CZ3DChrEventGenerator[m_rpGCMDS->GetAniHolderCount()];

	m_AniMixer.Init( m_pSkeletonObjects, m_rpGCMDS->GetLocalizerMatrix(), m_pAniHolder, m_pAddonAniHolder,
			m_rpGCMDS->GetSkeletonCount(), m_rpGCMDS->GetAniHolderCount() );

	const std::vector<Z3DTOK>* p_vecTok;
	p_vecTok = m_rpGCMDS->GetStaticSlotVector();
	for(int i = 0; i < (int)p_vecTok->size(); ++i )
	{
		m_mapStaticSlot[(*p_vecTok)[i]] = NULL_TOK;
	}

	const std::vector<long>* p_vecOverlayCount = m_rpGCMDS->GetOutfitSlotOverlayVector();
	p_vecTok = m_rpGCMDS->GetOutfitSlotVector();
	Z3DTOK_OVERLAY_SLOT* pSlot = NULL;
	for(int i = 0; i < (int)p_vecTok->size(); ++i )
	{
		pSlot = new Z3DTOK_OVERLAY_SLOT;
		pSlot->Alloc( (*p_vecOverlayCount)[i] );
		m_mapOutfitSlot[(*p_vecTok)[i]] = pSlot;
	}

	Z3D_TOK_N_ATTACHMENT tna;
	tna.tokAttachmentName = NULL_TOK;		
	tna.pAttachment = NULL;
	tna.lHolderIndex = 0;
	p_vecTok = m_rpGCMDS->GetAttachmentSlotVector();
	for(int i = 0; i < (int)p_vecTok->size(); ++i )
	{
		m_mapAttachmentSlot[(*p_vecTok)[i]] = tna;
	}
	
	BeginOutfitSetting();
	EndOutfitSetting();

	SetMotionSheet( m_rpGCMDS->GetDefaultMotionSheet() );
	SetMotion( m_rpGCMDS->GetDefaultMotion(), 0, false );


/*(#ifdef _DEBUG
	for( int xxxi = 0; xxxi < m_rpGCMDS->GetAniHolderCount(); xxxi++ )
	{
		if( NULL == m_pAniHolder[xxxi].GetAniKeyPackInterface() )
		{
			MessageBox( NULL, "CZ3DGeneralChrModel::Init() - 뭔가 수상해뵈네잉..", 0, 0 );
		}
	}
#endif*/


	const std::vector<Z3DTOK_SLOT_ATTACHMENT>* p_vecAttachmentDefSetting = 
		m_rpGCMDS->GetAttachmentDefaultSettingVector();

	for(int i = 0; i < (int)p_vecAttachmentDefSetting->size(); ++i )
	{
		SetAttachmentSlot(
			(*p_vecAttachmentDefSetting)[i].tokSlot,
			(*p_vecAttachmentDefSetting)[i].tokAttachment, 0, 0, true );
	}

	const std::vector<Z3D_BOUNDING_CYLINDER_INFO>* pBCList =
									m_rpGCMDS->GetBoundingCylinderVector();
	if( pBCList->size() )
	{
		m_pBoundingCylinder = new Z3D_BOUNDING_CYLINDER[pBCList->size()];
		for(int i = 0; i < (int)pBCList->size(); ++i )
		{
			m_pBoundingCylinder[i].fBottomRadius = (*pBCList)[i].fBottomRadius;
			m_pBoundingCylinder[i].fTopRadius = (*pBCList)[i].fTopRadius;
		}
	}

	m_bInitialized = true;

	// 2003.5.8 추가 - default scale 기능 추가
	SetScale( 1.0f );	// 이렇게 불러주면 SetScale() 내부에서 알아서 해준다.

	m_pSkin->SetTestMode( m_bAniTestMode );

	return true;
}


CZ3DObject* CZ3DGeneralChrModel::GetSkeletonPartObject( const char* szSkeletonPartName )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	if( NULL == m_rpGCMDS )
	{
		return NULL;
	}

	long lIndex = m_rpGCMDS->GetSkeletonPartObjectIdx( szSkeletonPartName );

	if( -1 == lIndex )
	{
		//return NULL;
		return m_pSkeletonObjects;	// actually index 0
	}

	// else
	return &(m_pSkeletonObjects[lIndex]);
}


CZ3DObject* CZ3DGeneralChrModel::GetSkeletonPartObject( Z3DTOK tokSkeletonPartName )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	if( NULL == m_rpGCMDS )
	{
		return NULL;
	}

	long lIndex = m_rpGCMDS->GetSkeletonPartObjectIdx( tokSkeletonPartName );

	if( -1 == lIndex )
	{
		//return NULL;
		return m_pSkeletonObjects;	// actually index 0
	}

	// else
	return &(m_pSkeletonObjects[lIndex]);
}


CZ3DObject* CZ3DGeneralChrModel::GetSkeletonByIndex( int nIDx )
{
	if( NULL == m_rpGCMDS )
	{
		return NULL;
	}
	
	if( nIDx < 0 )
	{
		return &m_pSkeletonObjects[0];
	}
	
	if( nIDx > m_rpGCMDS->GetSkeletonCount() )
	{
		return &m_pSkeletonObjects[0];
	}
	
	return &m_pSkeletonObjects[nIDx];
}


Z3DTOKARRAY* CZ3DGeneralChrModel::GetTokArray( Z3DTOK tokMotionName, long lRandomIndex )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	if( NULL == m_rpCurrentMotionSheet )
	{
		return NULL;
	}

	Z3DMOTLET* pMotlet;
	Z3DMOTIONSHEET* pSheet = const_cast<Z3DMOTIONSHEET*>(m_rpCurrentMotionSheet);
	std::map<Z3DTOK, Z3DMOTLET*>::iterator it;
	while( 1 )
	{
		it = pSheet->mapTok2Motlet.find( tokMotionName );
		if( it != pSheet->mapTok2Motlet.end() )
		{
			// 현재 sheet에서 해당 motion의 motlet을 찾은 경우
			pMotlet = it->second;
			break;
		}
		//else
		pSheet = pSheet->rpParentSheet;
		if( NULL == pSheet )
		{
			return NULL;
		}		
	}
	
	Z3DTOKARRAY* pTokArr;
	if( -1 == lRandomIndex )
	{
		long lMotSeed = rand() % (pMotlet->lProbalilitySum);
		for( int i = 0; i < pMotlet->lCount; ++i )
		{
			// 확률필드가 없는 데이터의 경우
			// lProbalilitySum = 1, lMotSeed = 0 이므로 아래 조건식은 항상 true
			if( lMotSeed < pMotlet->pProbabilityIdx[i] )
			{
				pTokArr = &(pMotlet->pTokArr[i]);
				break;
			}
		}
	}
	else
	{
		pTokArr = &(pMotlet->pTokArr[min( lRandomIndex, (pMotlet->lCount-1) )]);
	}

	return pTokArr;
}


void CZ3DGeneralChrModel::SetMotion( const char* szMotionName, unsigned long ulFlag, bool bTransit, long lRandomIndex )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_rpCurrentMotionSheet )
	{
		return;
	}

	Z3DTOK tokMotionName = g_TokMotionName.GetTOK( szMotionName );
	if( NULL_TOK == tokMotionName )
	{
		return;
	}

	SetMotion( tokMotionName, ulFlag, bTransit, lRandomIndex );
}


void CZ3DGeneralChrModel::SetMotion( Z3DTOK tokMotionName, unsigned long ulFlag, bool bTransit, long lRandomIndex )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_rpCurrentMotionSheet )
	{
		return;
	}

	Z3DTOKARRAY* pTokArr = GetTokArray( tokMotionName, lRandomIndex );
	if( NULL == pTokArr )
	{
		return;
	}

	int i;
	if( 0 == ulFlag )	// flag setting 없음. 모든 ani holder에 setting
	{
		for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); ++i )
		{
			SetAniKeyPack( i, pTokArr->pArray[i], bTransit );
		}
	}
	else	// flag 적용 setting
	{
		for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); ++i )
		{
			if( ulFlag & (0x1 << i) )
			{
				SetAniKeyPack( i, pTokArr->pArray[i], bTransit );
			}
		}
	}
}


void CZ3DGeneralChrModel::AddMotion( const char* szMotionName, unsigned long ulFlag, float fFactorPeak, bool bSustainLast )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_rpCurrentMotionSheet )
	{
		return;
	}

	Z3DTOK tokMotionName = g_TokMotionName.GetTOK( szMotionName );
	if( NULL_TOK == tokMotionName )
	{
		return;
	}

	AddMotion( tokMotionName, ulFlag, fFactorPeak, bSustainLast );
}


void CZ3DGeneralChrModel::AddMotion( Z3DTOK tokMotionName, unsigned long ulFlag, float fFactorPeak, bool bSustainLast )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_rpCurrentMotionSheet )
	{
		return;
	}

	Z3DTOKARRAY* pTokArr = GetTokArray( tokMotionName );
	if( NULL == pTokArr )
	{
		return;
	}

	int i;
	if( 0 == ulFlag )	// flag setting 없음. 모든 ani holder에 setting
	{
		for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); ++i )
		{
			SetAddonAniKeyPack( i, pTokArr->pArray[i], fFactorPeak );
		}
	}
	else	// flag 적용 setting
	{
		for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); ++i )
		{
			if( ulFlag & (0x1 << i) )
			{
				SetAddonAniKeyPack( i, pTokArr->pArray[i], fFactorPeak );
			}
		}
	}

	m_AniMixer.SetAddonFactorPeak( fFactorPeak );
	m_AniMixer.SustainLast( bSustainLast );
}


void CZ3DGeneralChrModel::Act( Z3D_CHR_ACTION action, long lParam1, long lParam2 )
{
	if( this != m_rpThis )
	{
		return;
	}

	switch( action )
	{
	case Z3D_CA_SET_MOTION_SHEET:
		break;

	case Z3D_CA_ACT_MOTION:
		break;

	case Z3D_CA_SET_STATIC_SLOT:
		break;

	case Z3D_CA_BEGIN_OUTFIT_SET:
		break;

	case Z3D_CA_SET_OUTFIT_SLOT:
		break;

	case Z3D_CA_END_OUTFIT_SET:
		break;

	case Z3D_CA_SET_WEAPON:
		break;

	case Z3D_CA_SET_SHIELD:
		break;

	default:
		CZ3DCharacterModel::Act( action, lParam1, lParam2 );
	}
}


bool CZ3DGeneralChrModel::SetAniKeyPack( int n, Z3DTOK tokFileName, bool bTransit )
{
	if( this != m_rpThis )
	{
		return true;
	}

	H3DAniKeyPackTag tagAniKeyPack;
	
	if( NULL_TOK == tokFileName )
	{
		return false;
	}
	if( n >= m_rpGCMDS->GetAniHolderCount() )
	{
		return false;
	}
	const char* szFileName = g_TokFileName.GetString( tokFileName );
	if( NULL == szFileName )
	{
		return false;
	}

	g_ContAniKeyPack.GetObject( tagAniKeyPack, szFileName );
	if( NULL == tagAniKeyPack.GetObject() )
	{
		return false;
	}
	if( tagAniKeyPack == m_pTagAniKeyPack[n] )
	{
		tagAniKeyPack.Release();
		//return false;
		// 이전 것과 같은 동작을 세팅해주더라도 처음부터 다시 플레이하도록 해
		// 해 줘야지 그냥 종료해버리면 안된다.
	}
	else
	{
		m_pTagAniKeyPack[n] = tagAniKeyPack;
	}
	m_pAniHolder[n].SetAniPack( m_pTagAniKeyPack[n].GetObject() );
	m_AniMixer.Swap( n, bTransit );
	m_pEventGenerator[n].SetTokAniPack( tokFileName );
	//m_pTokCurrentAniKeyPackFile = tokFileName;

	return true;
}


bool CZ3DGeneralChrModel::SetAddonAniKeyPack( int n, Z3DTOK tokFileName, float fFactorPeak )
{
	if( this != m_rpThis )
	{
		return true;
	}

	H3DAniKeyPackTag tagAniKeyPack;
	
	if( NULL_TOK == tokFileName )
	{
		return false;
	}
	if( n >= m_rpGCMDS->GetAniHolderCount() )
	{
		return false;
	}
	const char* szFileName = g_TokFileName.GetString( tokFileName );
	if( NULL == szFileName )
	{
		return false;
	}

	g_ContAniKeyPack.GetObject( tagAniKeyPack, szFileName );
	if( NULL == tagAniKeyPack.GetObject() )
	{
		return false;
	}
	if( tagAniKeyPack == m_pTagAddonAniKeyPack[n] )
	{
		tagAniKeyPack.Release();
		//return false;
		// 이전 것과 같은 동작을 세팅해주더라도 처음부터 다시 플레이하도록 해
		// 해 줘야지 그냥 종료해버리면 안된다.
	}
	else
	{
		m_pTagAddonAniKeyPack[n] = tagAniKeyPack;
	}
	m_pAddonAniHolder[n].SetAniPack( m_pTagAddonAniKeyPack[n].GetObject() );
	m_pAddonEventGenerator[n].SetTokAniPack( tokFileName );
	//m_pTokCurrentAniKeyPackFile = tokFileName;

	return true;
}


bool CZ3DGeneralChrModel::SetStaticSlot( const char* szSlotName, const char* szOutfitName )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( m_rpGCMDS->IsFixed() )
	{
		return false;
	}

	if( NULL == m_pSkin )
	{
		return false;
	}

	Z3DTOK tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	Z3DTOK tokOutfitName = g_TokOutfitName.GetTOK( szOutfitName );
	if( NULL_TOK == tokSlotName || NULL_TOK == tokOutfitName )
	{
		return false;
	}

	std::map<Z3DTOK, Z3DTOK>::iterator it;
	it = m_mapStaticSlot.find( tokSlotName );
	if( m_mapStaticSlot.end() == it )
	{
		return false;
	}

	it->second = tokOutfitName;

	EndOutfitSetting(); // mesh 구성 자체를 완전히 다시 해줘야하므로

	return true;
}


void CZ3DGeneralChrModel::BeginOutfitSetting()
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_pSkin )
	{
		return;
	}

	std::map<Z3DTOK, Z3DTOK_OVERLAY_SLOT*>::iterator it;

	for( it = m_mapOutfitSlot.begin(); it != m_mapOutfitSlot.end(); it++ )
	{
		it->second->lUsed = 0;
		for( int i = 0; i < it->second->lCount; ++i )
		{
			it->second->pArray[i].tok = NULL_TOK;
			it->second->pArray[i].lGradeEffectIndex = 0;
			it->second->pArray[i].lGlowIndex = 0;
		}
	}
}


bool CZ3DGeneralChrModel::SetOutfitSlot( const char* szSlotName, const char* szOutfitName, long lGradeEffectIndex, long lGlowIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( m_rpGCMDS->IsFixed() )
	{
		return false;
	}

	if( NULL == m_pSkin )
	{
		return false;
	}

	Z3DTOK tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	Z3DTOK tokOutfitName = g_TokOutfitName.GetTOK( szOutfitName );
	while( 1 )	// link part 세팅을 위한 반복
	{
		if( NULL_TOK == tokSlotName || NULL_TOK == tokOutfitName )
		{
			return false;
		}

		std::map<Z3DTOK, Z3DTOK_OVERLAY_SLOT*>::iterator it;
		it = m_mapOutfitSlot.find( tokSlotName );	// 해당슬롯명의 슬롯세팅데이터 검색
		if( m_mapOutfitSlot.end() == it )
		{
			return false;
		}
		Z3DTOK_OVERLAY_SLOT* pOS = it->second;

		if( 1 == pOS->lCount ) // 겹침이 없을 경우(slot holder 1개)
		{
			// 덮어쓰기가 가능
			pOS->pArray[0].tok = tokOutfitName;
			pOS->pArray[0].lGradeEffectIndex = lGradeEffectIndex;
			pOS->pArray[0].lGlowIndex = lGlowIndex;
			pOS->lUsed = 1;
		}
		else	// 아닐 때는
		{
			// holder 갯수만큼만 setting 가능하고 덮어쓰기 불가능
			if( pOS->lUsed < pOS->lCount )
			{
				pOS->pArray[pOS->lUsed].tok = tokOutfitName;
				pOS->pArray[pOS->lUsed].lGradeEffectIndex = lGradeEffectIndex;
				pOS->pArray[pOS->lUsed].lGlowIndex = lGlowIndex;
				++(pOS->lUsed);
			}
		}

		// link part setting
		const Z3DOUTFITINFO* pOI = m_rpGCMDS->GetOutfitInfo( tokOutfitName );
		if( NULL == pOI )
		{
			break;
		}
		tokOutfitName = pOI->tokLinkOutfit;
		tokSlotName = pOI->tokLinkSlot;
//		lGradeEffectIndex = 0;	// link part 에는 grade effect 적용안됨
	}

	return true;
}


void CZ3DGeneralChrModel::EndOutfitSetting()
{
	if( this != m_rpThis )
	{
		return;
	}

//DeclareBlockTimingCheck( "Begin", aa );
//DeclareBlockTimingCheck( "Setting", bb );
//DeclareBlockTimingCheck( "Begin+Setting", cc );
//DeclareBlockTimingCheck( "End", dd );

	if( NULL == m_pSkin )
	{
		return;
	}

	int i, j;
	const std::vector<Z3DTOK>* p_vecTok;
	Z3DTOK_OVERLAY_SLOT* pSlot;

//BlockTimingCheckStart(cc);

//BlockTimingCheckStart(aa);
	m_pSkin->BatchOpen();
//BlockTimingCheckStop(aa);

//BlockTimingCheckStart(bb);
	// 기본형(웃통벗고 반바지..같은거) 적용
	ApplyOutfit( m_rpGCMDS->GetDefaultOutfit(), 0, 0 );

	// static slot(얼굴, 헤어스타일) 적용
	p_vecTok = m_rpGCMDS->GetStaticSlotVector();
	for( i = 0; i < (int)p_vecTok->size(); ++i )
	{
		ApplyOutfit( m_mapStaticSlot[(*p_vecTok)[i]], 0, 0 );
	}

	// 옷가지 적용
	p_vecTok = m_rpGCMDS->GetOutfitSlotVector();
	for( i = 0; i < (int)p_vecTok->size(); ++i )
	{
		pSlot = m_mapOutfitSlot[(*p_vecTok)[i]];
		for( j = 0; j < pSlot->lUsed; ++j )
		{
			ApplyOutfit( pSlot->pArray[j].tok, pSlot->pArray[j].lGradeEffectIndex, pSlot->pArray[j].lGlowIndex );
		}
	}
//BlockTimingCheckStop(bb);

	m_pSkin->BatchClose();

}


bool CZ3DGeneralChrModel::SetAttachmentSlot( const char* szSlotName, const char* szAttachmentName, long lGradeEffectIndex, long lGlowIndex, bool bForced )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( !bForced && m_rpGCMDS->IsFixed() )
	{
		return false;
	}

	Z3DTOK tokSlotName, tokAttachmentName;

	tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	if( NULL_TOK == tokSlotName )
	{
		return false;
	}

	if( NULL == szAttachmentName )
	{
		return SetAttachmentSlot( tokSlotName, NULL_TOK, lGradeEffectIndex, lGlowIndex );
	}

	tokAttachmentName = g_TokAttachmentName.GetTOK( szAttachmentName );
	/*if( NULL_TOK == tokAttachmentName )
	{
		return false;
	}*/

	return SetAttachmentSlot( tokSlotName, tokAttachmentName, lGradeEffectIndex, lGlowIndex );
}


bool CZ3DGeneralChrModel::SetAttachmentSlot( Z3DTOK tokSlotName, Z3DTOK tokAttachmentName, long lGradeEffectIndex, long lGlowIndex, bool bForced )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( !bForced && m_rpGCMDS->IsFixed() )
	{
		return false;
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;

	it = m_mapAttachmentSlot.find( tokSlotName );
	if( m_mapAttachmentSlot.end() == it )
	{
		return false;
	}

	// attachment 이름이 null : 현재 attachment 삭제
	if( NULL_TOK == tokAttachmentName )
	{
		it->second.tokAttachmentName = NULL_TOK;
		SAFE_DELETE( it->second.pAttachment );
		return true;
	}

	const Z3DATTACHMENTINFO* pAInfo = m_rpGCMDS->GetAttachmentInfo( tokAttachmentName );
	if( NULL == pAInfo )
	{
		return false;
	}

	if( it->second.tokAttachmentName == tokAttachmentName )
	{
		// 이전에 세팅된것과 같은 attachment
		if( it->second.pAttachment )
		{
			it->second.pAttachment->SetGradeEffectIndex( lGradeEffectIndex );
			it->second.pAttachment->SetGlowIndex( lGlowIndex );
		}

		return false;
	}

	const char* szMeshName = g_TokFileName.GetString( pAInfo->tokMeshFile );
	const char* szTexName = g_TokFileName.GetString( pAInfo->tokTextureFile );
	const char* szTexName2 = g_TokFileName.GetString( pAInfo->tokTextureFile2 );
	if( !(szMeshName && szTexName) )
	{
		return false;
	}

	//SAFE_DELETE( it->second.pAttachment );
	if( NULL == it->second.pAttachment )
	{
		it->second.pAttachment = new CZ3DWeapon( GetSkeletonPartObject("R_HAND") );
		it->second.pAttachment->SetScale( m_fScaleFactor );
		it->second.pAttachment->SetGradeEffectIndex( lGradeEffectIndex );
		it->second.pAttachment->SetGlowIndex( lGlowIndex );
	}

	char strBumpTextureName[256] = { 0, };
	if( NULL == szTexName2 && CRenderOption::m_TextureMethod && CRenderOption::m_CharacterPerPixelLighting)
	{
		sprintf(strBumpTextureName, "%sTexture\\%s", CHARACTERDATAPATH,szTexName);

		int nStrLens=strlen(strBumpTextureName);
		strBumpTextureName[nStrLens-4]='_';
		strBumpTextureName[nStrLens-3]='b';
		strBumpTextureName[nStrLens-2]='u';
		strBumpTextureName[nStrLens-1]='m';
		strBumpTextureName[nStrLens-0]='p';
		strBumpTextureName[nStrLens+1]='.';
		strBumpTextureName[nStrLens+2]='d';
		strBumpTextureName[nStrLens+3]='d';
		strBumpTextureName[nStrLens+4]='s';
		strBumpTextureName[nStrLens+5]=0;

		FILE *fp=fopen(strBumpTextureName,"rb");
		if(fp==NULL)
			strcpy(strBumpTextureName,"");
		else
		{
			sprintf(strBumpTextureName, "%s", szTexName);

			nStrLens=strlen(strBumpTextureName);
			strBumpTextureName[nStrLens-4]='_';
			strBumpTextureName[nStrLens-3]='b';
			strBumpTextureName[nStrLens-2]='u';
			strBumpTextureName[nStrLens-1]='m';
			strBumpTextureName[nStrLens-0]='p';
			strBumpTextureName[nStrLens+1]='.';
			strBumpTextureName[nStrLens+2]='d';
			strBumpTextureName[nStrLens+3]='d';
			strBumpTextureName[nStrLens+4]='s';
			strBumpTextureName[nStrLens+5]=0;					
			fclose( fp );
		}
		it->second.pAttachment->Set( szMeshName, szTexName, strBumpTextureName );
	}
	else
	{
		it->second.pAttachment->Set( szMeshName, szTexName, szTexName2 );
	}

	const Z3DATTACHMENTHOLDERINFO* pholderInfo =
					pAInfo->GetAttachmentHolderInfo(it->second.lHolderIndex);
	it->second.pAttachment->Link(
				GetSkeletonPartObject(pholderInfo->tokSkelPart) );
	it->second.pAttachment->SetPositionLocal( pholderInfo->vPos );
	it->second.pAttachment->SetDirectionLocal( pholderInfo->qRot );

	if( 5 ==  pAInfo->lType )	// type 5 == bow
	{
		it->second.pAttachment->SetBowMode( true );
	}
	else
	{
		it->second.pAttachment->SetBowMode( false );
	}


	it->second.tokAttachmentName = tokAttachmentName;

	return true;
}


bool CZ3DGeneralChrModel::SetAttachmentHolder(  const char* szSlotName, long lIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

	Z3DTOK tokSlotName;

	tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	if( NULL_TOK == tokSlotName )
	{
		return false;
	}

	return SetAttachmentHolder( tokSlotName, lIndex );
}


bool CZ3DGeneralChrModel::SetAttachmentHolder(  Z3DTOK tokSlotName, long lIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;

	it = m_mapAttachmentSlot.find( tokSlotName );
	if( m_mapAttachmentSlot.end() == it )
	{
		return false;
	}

	if( NULL == it->second.pAttachment )
	{
		return true;
	}

	const Z3DATTACHMENTINFO* pAInfo = m_rpGCMDS->GetAttachmentInfo( it->second.tokAttachmentName );
	if( NULL == pAInfo )
	{
		return false;	// 이쪽이 실행될 일은 없음..
	}

	const Z3DATTACHMENTHOLDERINFO* pholderInfo = pAInfo->GetAttachmentHolderInfo(lIndex);
	it->second.pAttachment->Link(
				GetSkeletonPartObject(pholderInfo->tokSkelPart) );
	it->second.pAttachment->SetPositionLocal( pholderInfo->vPos );
	it->second.pAttachment->SetDirectionLocal( pholderInfo->qRot );
	it->second.lHolderIndex = lIndex;

	return false;	
}


long CZ3DGeneralChrModel::GetAttachmentHolder( const char* szSlotName )
{
	if( this != m_rpThis )
	{
		return 0;
	}

	Z3DTOK tokSlotName;

	tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	if( NULL_TOK == tokSlotName )
	{
		return -1;
	}

	return GetAttachmentHolder( tokSlotName );
}


long CZ3DGeneralChrModel::GetAttachmentHolder( Z3DTOK tokSlotName )
{
	if( this != m_rpThis )
	{
		return 0;
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;

	it = m_mapAttachmentSlot.find( tokSlotName );
	if( m_mapAttachmentSlot.end() == it )
	{
		return -1;
	}

	return it->second.lHolderIndex;
}


CZ3DWeapon* CZ3DGeneralChrModel::GetAttachmentObject( const char* szSlotName )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	Z3DTOK tokSlotName;

	tokSlotName = g_TokSlotName.GetTOK( szSlotName );
	if( NULL_TOK == tokSlotName )
	{
		return NULL;
	}

	return GetAttachmentObject( tokSlotName );
}


CZ3DWeapon* CZ3DGeneralChrModel::GetAttachmentObject( Z3DTOK tokSlotName )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;

	it = m_mapAttachmentSlot.find( tokSlotName );
	if( m_mapAttachmentSlot.end() == it )
	{
		return NULL;
	}

	return it->second.pAttachment;
}


void CZ3DGeneralChrModel::ApplyOutfit( Z3DTOK tokOutfitName, long lGradeEffectIndex, long lGlowIndex )
{
	if( this != m_rpThis )
	{
		return;
	}

//DeclareBlockTimingCheck("Never being called", aa);
//DeclareBlockTimingCheck("Getstrings", bb);
//DeclareBlockTimingCheck("Bump filename mangling", cc);
//DeclareBlockTimingCheck("Setting", dd);

	if( NULL_TOK == tokOutfitName )
	{
		return;
	}

	const Z3DOUTFITINFO* pOI = m_rpGCMDS->GetOutfitInfo( tokOutfitName );
	if( NULL == pOI )
	{
		return;
	}

	int i;
	const char* szMeshName;
	const char* szTextureName;
	//const char* szTexture2Name;
	if( Z3D_MPT_TEXPIECE == m_rpGCMDS->GetMPT() )
	{
//BlockTimingCheckStart(aa);
		for( i = 0; i < pOI->lCount; i++ )
		{
			szMeshName = g_TokFileName.GetString( pOI->pTokMeshName[i] );
			if( szMeshName )
			{
				m_pSkin->SetMesh( szMeshName );
			}

			szTextureName = g_TokFileName.GetString( pOI->pTokTextureName[i] );

			char strBumpTextureName[256];
			FILE *fp=NULL;

			if(CRenderOption::m_TextureMethod && CRenderOption::m_CharacterPerPixelLighting)
			{
				sprintf(strBumpTextureName, "%sTexture\\", CHARACTERDATAPATH,szTextureName);
			}			
			
			//szTexture2Name = g_TokFileName.GetString( pOI->pTokTexture2Name[i] );
			/*if( NULL_TOK == pOI->pTokTexture2Name[i] )
			{
				szTexture2Name = NULL;
			}
			else
			{
				szTexture2Name = g_TokFileName.GetString( pOI->pTokTexture2Name[i] );
			}*/
			
			if( szTextureName )
			{
				m_pSkin->SetTex( szTextureName, strBumpTextureName);
			}
			/*
			if( szTextureName )
			{
				m_pSkin->SetTex( szTextureName, szTexture2Name );
			}
			*/
		}
//BlockTimingCheckStop(aa);
	}
	else	// Z3D_MPT_TEXTURE
	{
		for( i = 0; i < pOI->lCount; i++ )
		{
//BlockTimingCheckStart(bb);
			szMeshName = g_TokFileName.GetString( pOI->pTokMeshName[i] );
			szTextureName = g_TokFileName.GetString( pOI->pTokTextureName[i] );
//BlockTimingCheckStop(bb);

//BlockTimingCheckStart(cc);
			char strBumpTextureName[256] = { 0, };
			
			if(CRenderOption::m_TextureMethod && CRenderOption::m_CharacterPerPixelLighting)
			{
				sprintf(strBumpTextureName, "%sTexture\\%s", CHARACTERDATAPATH,szTextureName);

				int nStrLens=strlen(strBumpTextureName);
				strBumpTextureName[nStrLens-4]='_';
				strBumpTextureName[nStrLens-3]='b';
				strBumpTextureName[nStrLens-2]='u';
				strBumpTextureName[nStrLens-1]='m';
				strBumpTextureName[nStrLens-0]='p';
				strBumpTextureName[nStrLens+1]='.';
				strBumpTextureName[nStrLens+2]='d';
				strBumpTextureName[nStrLens+3]='d';
				strBumpTextureName[nStrLens+4]='s';
				strBumpTextureName[nStrLens+5]=0;

				FILE *fp=fopen(strBumpTextureName,"rb");
				if(fp==NULL)
					strcpy(strBumpTextureName,"");
				else
				{
					sprintf(strBumpTextureName, "%s", szTextureName);

					nStrLens=strlen(strBumpTextureName);
					strBumpTextureName[nStrLens-4]='_';
					strBumpTextureName[nStrLens-3]='b';
					strBumpTextureName[nStrLens-2]='u';
					strBumpTextureName[nStrLens-1]='m';
					strBumpTextureName[nStrLens-0]='p';
					strBumpTextureName[nStrLens+1]='.';
					strBumpTextureName[nStrLens+2]='d';
					strBumpTextureName[nStrLens+3]='d';
					strBumpTextureName[nStrLens+4]='s';
					strBumpTextureName[nStrLens+5]=0;					
					fclose( fp );
				}
			}			
//BlockTimingCheckStop(cc);

			//szTexture2Name = g_TokFileName.GetString( pOI->pTokTexture2Name[i] );

			if( szMeshName )
			{
				//m_pSkin->SetMeshTexture( szMeshName, szTextureName, szTexture2Name );
//BlockTimingCheckStart(dd);
				m_pSkin->SetMeshTexture(szMeshName,szTextureName,strBumpTextureName, lGradeEffectIndex, lGlowIndex );
//BlockTimingCheckStop(dd);
			}
		}
	}
}


void CZ3DGeneralChrModel::FrameMove()
{
	if( this != m_rpThis )
	{
		return;
	}

/*#ifdef _DEBUG
	for( int xxxi = 0; xxxi < m_rpGCMDS->GetAniHolderCount(); xxxi++ )
	{
		if( NULL == m_pAniHolder[xxxi].GetAniKeyPackInterface() )
		{
			MessageBox( NULL, "AniKeyPack 데이터 손상(this pointer = NULL", 0, 0 );
		}
	}
#endif*/

	if( m_bPaused )
	{
		return;
	}

	
	int i;
	vector3 vd;


	if( NULL == m_pAniHolder )
	{
		return;
	}

	if( m_bAniTestMode )
	{
		for( i = 0; i < m_rpGCMDS->GetSkeletonCount(); ++i )
		{
			m_pSkeletonObjects[i].SetPositionLocal( 0, 0, 0 );
			m_pSkeletonObjects[i].SetDirectionLocal( 0, 0, 0, 1 );
		}

		return;
	}
		
	/*for( i = 0; i < m_rpGCMDS->GetSkeletonCount(); i++ )
	{
		m_pSkeletonObjects[i].ApplyAniKey( m_pAniHolder );
	}*/
	m_AniMixer.Mix();
	/*for( i = 0; i < m_rpGCMDS->GetSkeletonCount(); i++ )
	{
		m_pSkeletonObjects[i].SetPositionLocal( 0, 0, 0 );
		m_pSkeletonObjects[i].SetDirectionLocal( 0, 0, 0, 1 );
	}*/


	// Bip01 오브젝트와 pelvis 오브젝트간의 x-z거리를 무시한다
	/*	m_pSkeletonObjects[1].GetPositionLocal( vd );
		vd.x = 0;
		vd.z = 0;
		m_pSkeletonObjects[1].SetPositionLocal( vd );
	*/
	// 캐릭터모델의 위치를 skin과 attachment 의 pivot으로..
	if( m_pSkin )
	{
		m_pSkin->SetPivot( m_Pos );
	}


	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->SetPivot( m_Pos );
		}
	}

	quaternion q1, q2;
	z3d::QuaternionAxisAngle( q1, 0, 1.0f, 0, m_fDir );
	//m_pSkeletonObjects[0].Rotate( q );
	m_pSkeletonObjects[0].GetDirectionLocal( q2 );
	m_pSkeletonObjects[0].SetDirectionLocal( q2*q1 );
	m_pSkeletonObjects[0].GetPositionLocal( vd );
	z3d::VectorRotate( vd, vd, q1 );
	m_pSkeletonObjects[0].SetPositionLocal( vd );
	

	
	// event 체크
	
	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// 매 호출당 이벤트 체크 버퍼를 클리어에서 FlushAccumulated..() 함수가
	// 호출될때만 클리어로 변경
	//m_vec_tokEventStorage.clear();
	//m_vec_tokSecondaryEventStorage.clear();
	for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); ++i )
	{
		m_pEventGenerator[i].CheckEvent(
			m_pAniHolder[i].GetCurrentFrame(),
			m_pAniHolder[i].GetPrevFrame(),
			m_vec_tokEventStorage, m_vec_tokSecondaryEventStorage );

		m_pAddonEventGenerator[i].CheckEvent(
			m_pAddonAniHolder[i].GetCurrentFrame(),
			m_pAddonAniHolder[i].GetPrevFrame(),
			m_vec_tokEventStorage, m_vec_tokSecondaryEventStorage );
	}

	for( i = 0; i < m_rpGCMDS->GetAniHolderCount(); i++ )
	{
		m_pAniHolder[i].Progress();
		m_pAddonAniHolder[i].Progress();
	}
}


void CZ3DGeneralChrModel::FlushAccumulatedEvents()
{
	m_vec_tokEventStorage.clear();
	m_vec_tokSecondaryEventStorage.clear();
}


bool CZ3DGeneralChrModel::CheckEvent( const char* szEventName, Z3DTOK* p_tokSecondaryEvent )
{
	if( this != m_rpThis )
	{
		return true;
	}

	Z3DTOK tok = g_TokEventName.GetTOK( szEventName );
	if( NULL_TOK == tok )
	{
		return false;
	}

	std::vector<Z3DTOK>::iterator it;

	if( m_vec_tokEventStorage.end() == 
		( it = std::find(	m_vec_tokEventStorage.begin(),
					m_vec_tokEventStorage.end(), tok ) ) )
	{
		return false;
	}

	if( p_tokSecondaryEvent )
	{
		*p_tokSecondaryEvent =
			m_vec_tokSecondaryEventStorage[it - m_vec_tokEventStorage.begin()];
	}

	*it = NULL_TOK;	// clear checked event;
	return true;
}


bool CZ3DGeneralChrModel::SetMotionSheet( const char* szSheetName )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( NULL == szSheetName )
	{
		return false;
	}
	Z3DTOK tok = g_TokSheetName.GetTOK( szSheetName );
	if( NULL_TOK == tok )
	{
		return false;
	}

	return SetMotionSheet( tok );
}


bool CZ3DGeneralChrModel::SetMotionSheet( Z3DTOK tokSheetName )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( NULL == m_rpGCMDS )
	{
		return false;
	}

	const Z3DMOTIONSHEET* pSheet = m_rpGCMDS->GetMotionsheet( tokSheetName );
	if( NULL == pSheet )
	{
		return false;
	}

	m_rpCurrentMotionSheet = pSheet;
	m_tokCurrentMotionSheet = tokSheetName;

	return true;
}


const char* CZ3DGeneralChrModel::GetCurrentMotionSheetName()
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	return g_TokSheetName.GetString( m_tokCurrentMotionSheet );
}


void CZ3DGeneralChrModel::PreRender()
{
	if( this != m_rpThis )
	{
		return;
	}

	if( m_pSkin )
	{
		m_pSkin->ApplyAnimation();
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->ApplyTransform();
		}
	}
}


void CZ3DGeneralChrModel::Render()
{
	if( this != m_rpThis )
	{
		return;
	}

	if( Z3DRO_NONE == m_nRenderOverride )
	{
		if( m_pSkin )
		{
			m_pSkin->RenderIndividual();
		}

		std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
		for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
		{
			if( it->second.pAttachment )
			{
				it->second.pAttachment->RenderIndividual();
			}
		}
	}
	else
	{
		m_pSkin->GetDevice()->SetTexture( 0, m_rpRenderOverrideTexture->GetD3dTexture() );

		if( m_pSkin )
		{
			m_pSkin->RenderShadow( m_pSkin->GetDevice(), 0xFFFFFFFF, 0 );
		}

		std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
		for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
		{
			if( it->second.pAttachment )
			{
				it->second.pAttachment->RenderShadow( m_pSkin->GetDevice() );
			}
		}
	}
}


void CZ3DGeneralChrModel::DrawShadow( IDirect3DDevice8* pDevice, DWORD vertexShader )
{
	if( this != m_rpThis )
	{
		return;
	}

	_ASSERT( pDevice );
	
	if( m_pSkin )
	{
		m_pSkin->RenderShadow( pDevice, vertexShader );
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->RenderShadow( pDevice, vertexShader );
		}
	}
}


void CZ3DGeneralChrModel::RenderGlowShape( IDirect3DDevice8* pDevice )
{
	if( this != m_rpThis )
	{
		return;
	}

	_ASSERT( pDevice );

	if( IsCulled() )
	{
		return;
	}

	if( m_pSkin )
	{
		m_pSkin->RenderGlowShape( pDevice );
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->RenderGlowShape( pDevice );
		}
	}
}


void CZ3DGeneralChrModel::GetAniDataDisplacement( vector3 &v_disp, int n )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( !(n < m_rpGCMDS->GetAniHolderCount()) )
	{
		return;
	}

	if( NULL == m_pTagAniKeyPack[n].GetObject() )
	{
		v_disp.Identity();
		return;
	}

	Z3DAniKeyPack* pPack = m_pTagAniKeyPack[n].GetObject();
	if( false == pPack->GetDisplacement( v_disp, 0 ) )
	{
		v_disp.Identity();
	}
}


long CZ3DGeneralChrModel::GetAniDataLength( int n )
{
	if( this != m_rpThis )
	{
		return 0;
	}

	if( !(n < m_rpGCMDS->GetAniHolderCount()) )
	{
		return 0L;
	}

	if( NULL == m_pTagAniKeyPack[n].GetObject() )
	{
		return 0L;
	}

	Z3DAniKeyPack* pPack = m_pTagAniKeyPack[n].GetObject();

	return pPack->GetAniLength();
}


void CZ3DGeneralChrModel::SetDisable( bool bDisabled )
{
	if( this != m_rpThis )
	{
		return;
	}

	CZ3DCharacterModel::SetDisable( bDisabled );

	SetCull( bDisabled );
}


void CZ3DGeneralChrModel::SetCull( bool bCulled )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( m_pSkeletonObjects )	// init 의 증거
	{
		m_pSkeletonObjects[0].SetDisable( bCulled );
	}

	if( m_pSkin )
	{
		m_pSkin->SetCull( bCulled );
	}

	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->SetCull( bCulled );
		}
	}
}


void CZ3DGeneralChrModel::SetScale( float f )
{
	if( this != m_rpThis )
	{
		return;
	}

	float fDefaultScale = 1.0f;

	if( m_rpGCMDS )
	{
		fDefaultScale = m_rpGCMDS->GetDefaultScale();
	}

	m_fScaleFactor = f * fDefaultScale;
	if( m_pSkin )
	{
		m_pSkin->SetScale( m_fScaleFactor );
	}

	// attachment 관련 설정
	std::map<Z3DTOK, Z3D_TOK_N_ATTACHMENT>::iterator it;
	for( it = m_mapAttachmentSlot.begin(); it != m_mapAttachmentSlot.end(); it++ )
	{
		if( it->second.pAttachment )
		{
			it->second.pAttachment->SetScale( m_fScaleFactor );
		}
	}


	for( int i = 0; i < (int)m_vec_pBoundingVolumeObject.size(); ++i )
	{
		m_vec_pBoundingVolumeObject[i]->SetScale( f * fDefaultScale );
	}
}


float CZ3DGeneralChrModel::GetEffectScale()
{
	if( this != m_rpThis )
	{
		return 1.0f;
	}

	return this->GetScale() * m_rpGCMDS->GetEffectScale();
}


void CZ3DGeneralChrModel::GetBoundingBox( vector3& r_vmin, vector3& r_vmax )
{
	if( this != m_rpThis )
	{
		return;
	}

	if( NULL == m_pSkin )
	{
		r_vmin = m_Pos;
		r_vmax = m_Pos;
		return;
	}

	m_pSkin->GetBoundingBox( r_vmin, r_vmax );
}


const Z3D_BOUNDING_CYLINDER* CZ3DGeneralChrModel::GetBoundingCylinderList()
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	if( NULL == m_rpGCMDS )
	{
		return NULL;
	}

	const std::vector<Z3D_BOUNDING_CYLINDER_INFO>& rBCIList =
									*(m_rpGCMDS->GetBoundingCylinderVector());
	for( int i = 0; i < (int)rBCIList.size(); ++i )
	{
		m_pBoundingCylinder[i].vBottom.x = m_pSkeletonObjects[rBCIList[i].lBottomIdx].GetTM()->_41;
		m_pBoundingCylinder[i].vBottom.y = m_pSkeletonObjects[rBCIList[i].lBottomIdx].GetTM()->_42;
		m_pBoundingCylinder[i].vBottom.z = m_pSkeletonObjects[rBCIList[i].lBottomIdx].GetTM()->_43;

		m_pBoundingCylinder[i].vTop.x = m_pSkeletonObjects[rBCIList[i].lTopIdx].GetTM()->_41;
		m_pBoundingCylinder[i].vTop.y = m_pSkeletonObjects[rBCIList[i].lTopIdx].GetTM()->_42;
		m_pBoundingCylinder[i].vTop.z = m_pSkeletonObjects[rBCIList[i].lTopIdx].GetTM()->_43;
	}
		
	return m_pBoundingCylinder;
}


bool CZ3DGeneralChrModel::IsShaderParameterAvailable( long lIdx )
{
	if( this != m_rpThis )
	{
		return true;
	}

	if( NULL == m_rpGCMDS )
	{
		return false;
	}

	return m_rpGCMDS->IsShaderParameterAvailable( lIdx );
}


const char* CZ3DGeneralChrModel::GetShaderParameterInfo( long lIdx )
{
	if( this != m_rpThis )
	{
		return NULL;
	}

	if( NULL == m_rpGCMDS )
	{
		return NULL;
	}

	return m_rpGCMDS->GetShaderParameterInfo( lIdx );
}


long CZ3DGeneralChrModel::GetEffectInfoList( const char** &rpszEffectInfo )
{
	if( this != m_rpThis )
	{
		return 0;
	}

	if( NULL == m_rpGCMDS )
	{
		rpszEffectInfo = NULL;
		return 0;
	}

	return m_rpGCMDS->GetEffectInfoList( rpszEffectInfo );
}


void CZ3DGeneralChrModel::GetFixedPoint( int nIdx, vector3& v_out  )
{
	if( this != m_rpThis )
	{
		return;
	}

	Z3DBlend2Vertex vTmp;

	if( 0 == nIdx )
	{
		vTmp.pos.x = -16.87133f;
		vTmp.pos.y = 147.946f;
		vTmp.pos.z = 12.17191f;
		vTmp.mtrx_id[0] = 14;
		vTmp.mtrx_id[1] = 12;
		vTmp.bfactor = 0.36006f;
	}
	else
	{
		vTmp.pos.x = 16.87138f;
		vTmp.pos.y = 147.946f;
		vTmp.pos.z = 12.17191f;
		vTmp.mtrx_id[0] = 20;
		vTmp.mtrx_id[1] = 12;
		vTmp.bfactor = 0.360064f;
	}
	
	GetSkinInterface()->GetProcessedVertex( vTmp, v_out );
}


bool CZ3DGeneralChrModel::CheckCollision( const CZ3DOBB& rOBB )
{
	m_vec_nCollisionSkeletonIndex.clear();

	bool bCollision = false;
	
	for( int i = 0; i < (int)m_vec_pBoundingVolumeObject.size(); ++i )
	{
		if( true == m_vec_pBoundingVolumeObject[i]->GetOBB().CheckCollision( rOBB ) )
		{
			bCollision = true;
			m_vec_nCollisionSkeletonIndex.push_back(
				m_rpGCMDS->BoundingVolumeIndex2SkelIndex( i ) );
		}
		
	}

	return bCollision;
}


bool CZ3DGeneralChrModel::HasBoundingVolumeInfo()
{
	if( m_rpGCMDS )
	{
		if( m_rpGCMDS->GetBoundingVolumeInfoVector().size() > 0 )
		{
			return true;
		}
	}
	
	return false;
}
void CZ3DGeneralChrModel::SetHouse(float x,float y,float z,char *strName)
{
	m_bHouse = true;
	m_vecHousePos.x = x;
	m_vecHousePos.y = y;
	m_vecHousePos.z = z;
	
	strcpy(m_strOutName,strName);


}
bool CZ3DGeneralChrModel::GetHouse(D3DXVECTOR3 &vecPos,char *strName)
{
	if(!m_bHouse)
	{
		vecPos.x = vecPos.z = vecPos.y = 0.0f;
		return false;

	}
	vecPos.x = m_vecHousePos.x;
	vecPos.y = m_vecHousePos.y;
	vecPos.z = m_vecHousePos.z;
	strcpy(strName,m_strOutName);
	return true;

}
void CZ3DGeneralChrModel::SetHouseSkip(bool bSkip)
{
	if(m_bHouse)
	{
		CSceneManager::SetSkipHouse(m_vecHousePos.x,m_vecHousePos.y,m_vecHousePos.z,bSkip);

	}
}


void CZ3DGeneralChrModel::GetNameTagPosition( vector3& vPos )
{
	CZ3DObject* pObj = GetSkeletonPartObject( "HEAD" );

	// vPos 값 초기화
	pObj->GetPosition( vPos );
	vPos.y += m_rpGCMDS->GetNameTagBias();

	vPos *= GetScale() * m_rpGCMDS->GetEffectScale();

	vPos.y = vPos.y * 1.1f + 50.0f;

	vPos += m_Pos;

	if( m_bFixHeight )
	{
		vPos.y += m_fFixHeight;
	}
}


void CZ3DGeneralChrModel::SetRenderOverride( int n )
{
	if( m_nRenderOverride == n )
	{
		return;
	}

	// m_rpRenderOverrideTexture 의 release..이나 현재는 없음

	switch( n )
	{
	case Z3DRO_STONE:
		m_rpRenderOverrideTexture = g_ContTexture.GetObject( "RO_stone.dds" );
		break;

	case Z3DRO_NONE:
	default:
		// m_rpRenderOverrideTexture 의 release..이나 현재는 없음
		break;
	}

	m_nRenderOverride = n;
}