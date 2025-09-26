// Z3DGCMDS.cpp: implementation of the CZ3DGCMDS class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DGCMDS.h"

#include <stack>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DGCMDS::CZ3DGCMDS()
{
	m_fNameTagBias = 0.0f;
	m_bIsAlphaUsed = false;

	m_fEffectScale = 1.0f;
	m_fDefaultScale = 1.0f;
	m_lSkeletonCount = 0;
	m_lAniHolderCount = 0;
	m_pSkeletonHierarchy = NULL;
	m_pAniHolderIndex = NULL;
	m_pIndexInSubpart = NULL;
	m_pszSkeletonName = NULL;
	m_pLocalizerMatrix = NULL;

	m_TokDefaultOutfit = NULL_TOK;
	m_TokDefaultMotionSheet = NULL_TOK;
	m_TokDefaultMotion = NULL_TOK;

	m_szSkelFileName = NULL;
	m_szFileName = NULL;

	for( int i = 0; i < Z3D_MAX_SHADER_COUNT; ++i )
	{
		m_aszShaderInfoList[i] = NULL;
	}
	
	m_bFixed = false;
}

CZ3DGCMDS::~CZ3DGCMDS()
{
	std::map<Z3DTOK, Z3DMOTIONSHEET*>::iterator itMsheet;
	for( itMsheet = m_mapTok2MotionSheet.begin(); itMsheet != m_mapTok2MotionSheet.end(); itMsheet++ )
	{
		SAFE_DELETE( itMsheet->second );
	}

	std::map<Z3DTOK, Z3DATTACHMENTINFO*>::iterator itWInfo;
	for( itWInfo = m_mapTok2AttachmentInfo.begin(); itWInfo != m_mapTok2AttachmentInfo.end(); itWInfo++ )
	{
		SAFE_DELETE( itWInfo->second );
	}

	std::map<Z3DTOK, Z3DOUTFITINFO*>::iterator itOInfo;
	for( itOInfo = m_mapTok2OutfitInfo.begin(); itOInfo != m_mapTok2OutfitInfo.end(); itOInfo++ )
	{
		SAFE_DELETE( itOInfo->second );
	}

	for( int i = 0; i < Z3D_MAX_SHADER_COUNT; ++i )
	{
		SAFE_DELETEA( m_aszShaderInfoList[i] );
	}

	for(int i = 0; i < (int)m_vecszEffectInfo.size(); ++i )
	{
		//SAFE_DELETE( m_vecEffectInfo[i] );
		SAFE_DELETEA( m_vecszEffectInfo[i] );
	}

	if( m_pszSkeletonName )
	{
		for(int i = 0; i < m_lSkeletonCount; ++i )
		{
			SAFE_DELETEA( m_pszSkeletonName[i] );
		}
		SAFE_DELETEA( m_pszSkeletonName );
	}

	SAFE_DELETEA( m_pSkeletonHierarchy );
	SAFE_DELETEA( m_pAniHolderIndex );
	SAFE_DELETEA( m_pIndexInSubpart );
	SAFE_DELETEA( m_pLocalizerMatrix );
	SAFE_DELETEA( m_szFileName );
	SAFE_DELETEA( m_szSkelFileName );
}


// Loading 관련함수

bool CZ3DGCMDS::ReadSkelFile( const char* szFilename )
{
	FILE* fp;
	if( (fp = fopen( szFilename, "rb" )) == NULL )
	{
		return false;
	}

	long i, j;

	// read skeleton count & aniholder count;
	fread( &i, sizeof(long), 1, fp );
	fread( &j, sizeof(int), 1, fp );

	m_lSkeletonCount = i;
	m_lAniHolderCount = j;
	m_pSkeletonHierarchy = new long[i];
	m_pAniHolderIndex = new long[i];
	m_pIndexInSubpart = new long[i];
	m_pLocalizerMatrix = new matrix[i];

	// read hierarchy info
	for( i = 0; i <  m_lSkeletonCount; ++i )
	{
		fread( &(m_pSkeletonHierarchy[i]), sizeof(int), 1, fp );
	}

	// read localizer matrix data
	for( i = 0; i <  m_lSkeletonCount; ++i )
	{
		fread( &(m_pLocalizerMatrix[i]), sizeof(matrix), 1, fp );
	}

	// read ani holder index & index in subpart
	if( m_lAniHolderCount > 1 )
	{
		for( i = 0; i < m_lSkeletonCount; i++ )
		{
			fread( &(m_pAniHolderIndex[i]), sizeof(int), 1, fp );
			fread( &(m_pIndexInSubpart[i]), sizeof(int), 1, fp );
		}
	}
	else
	{
		for( i = 0; i < m_lSkeletonCount; i++ )
		{
			m_pAniHolderIndex[i] = 0;
			m_pIndexInSubpart[i] = i;
		}
	}

	fclose(fp);

	return true;
}


bool CZ3DGCMDS::ReadSkeyFile( const char* szFilename )
{
	CSimpleParser parser;
	long i, j;

	if( false == parser.OpenFile( szFilename ) )
	{
		return false;
	}

	if( 0 != stricmp( "SKELETON_KEY_DATA", parser.GetNextToken() ) )
	{
		return false;	// invalid header
	}

	// 이미 할당된 데이터들의 해제
	if( m_pszSkeletonName )
	{
		for( i = 0; i < m_lSkeletonCount; ++i )
		{
			SAFE_DELETEA( m_pszSkeletonName[i] );
		}
	}
	SAFE_DELETEA( m_pSkeletonHierarchy );
	SAFE_DELETEA( m_pAniHolderIndex );
	SAFE_DELETEA( m_pIndexInSubpart );
	SAFE_DELETEA( m_pszSkeletonName );
	SAFE_DELETEA( m_pLocalizerMatrix );


	char* szToken;
	
	while( (szToken =  parser.GetNextToken()) != NULL )
	{
		if( 0 == stricmp( "skeletoncount", szToken ) )
		{
			szToken = parser.GetNextToken();
			m_lSkeletonCount = aton( szToken );
		}
		else if( 0 == stricmp( "skeleton", szToken ) )
		{
			m_pSkeletonHierarchy = new long[m_lSkeletonCount];
			m_pAniHolderIndex = new long[m_lSkeletonCount];
			m_pIndexInSubpart = new long[m_lSkeletonCount];
			m_pszSkeletonName = new char*[m_lSkeletonCount];

			szToken = parser.GetNextToken();
			while( 0 != stricmp( szToken, ";" ) )
			{
				i = aton(szToken);
				if( i >= m_lSkeletonCount )
				{
					// skip remainder of 4 tokens
					for( j = 0; j < 4; ++j )
					{
						parser.GetNextToken();
					}

					szToken = parser.GetNextToken();
					continue;
				}

				m_pSkeletonHierarchy[i] = aton( parser.GetNextToken() );
				m_pAniHolderIndex[i] = aton( parser.GetNextToken() );
				m_pIndexInSubpart[i] = aton( parser.GetNextToken() );
				szToken = parser.GetNextToken();
				m_pszSkeletonName[i] = new char[strlen(szToken)+1];
				strcpy( m_pszSkeletonName[i], szToken );

				szToken = parser.GetNextToken();
			}			
		}
		else if( 0 == stricmp( "aniholdercount", szToken ) )
		{
			szToken = parser.GetNextToken();
			m_lAniHolderCount = aton( szToken );
		}
		else if( 0 == stricmp( "TMInverse", szToken ) )
		{
			m_pLocalizerMatrix = new matrix[m_lSkeletonCount];

			szToken = parser.GetNextToken();
			while( 0 != stricmp( szToken, ";" ) )
			{
				i = aton(parser.GetNextToken());	// skip "matrix" token
				if( i >= m_lSkeletonCount )
				{
					// skip remainder of 12 tokens
					for( j = 0; j < 12; ++j )
					{
						parser.GetNextToken();
					}
					szToken = parser.GetNextToken();
					continue;
				}

				m_pLocalizerMatrix[i]._11 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._12 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._13 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._14 = 0.0f;
				
				m_pLocalizerMatrix[i]._21 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._22 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._23 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._24 = 0.0f;
				
				m_pLocalizerMatrix[i]._31 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._32 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._33 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._34 = 0.0f;
				
				m_pLocalizerMatrix[i]._41 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._42 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._43 = atof( parser.GetNextToken() );
				m_pLocalizerMatrix[i]._44 = 1.0f;

				szToken = parser.GetNextToken();
			}
		}
		else if( 0 == stricmp( "BoundingVolume", szToken ) )
		{
			Z3D_BOUNDINGVOLUME_INFO bvi;
			vector3 vPos;
			quaternion qRot;
			vector3 vSize;
			vector3 vTmp;
			
			szToken = parser.GetNextToken();
			while( 0 != stricmp( szToken, ";" ) )
			{
				if( 0 == stricmp( "OBB", szToken ) ||
					0 == stricmp( "CYLINDER", szToken ) ||
					0 == stricmp( "SPHERE", szToken ) )
				{
					if( 0 == stricmp( "OBB", szToken ) )
					{
						bvi.eBVType = Z3D_BOUNDINGVOLUME_INFO::Z3DBV_OBB;
					}
					else if ( 0 == stricmp( "CYLINDER", szToken ) )
					{
						bvi.eBVType = Z3D_BOUNDINGVOLUME_INFO::Z3DBV_CYLINDER;
					}
					else if ( 0 == stricmp( "SPHERE", szToken ) )
					{
						bvi.eBVType = Z3D_BOUNDINGVOLUME_INFO::Z3DBV_SPHERE;
					}

					bvi.lSkelIndex = aton( parser.GetNextToken() );
					
					szToken = parser.GetNextToken();
					while( 0 != stricmp( szToken, ";" ) )
					{
						if( 0 == stricmp( "pos", szToken ) )
						{
							vPos.x = atof( parser.GetNextToken() );
							vPos.y = atof( parser.GetNextToken() );
							vPos.z = atof( parser.GetNextToken() );
						}
						else if( 0 == stricmp( "rot", szToken ) )
						{
							qRot.x = atof( parser.GetNextToken() );
							qRot.y = atof( parser.GetNextToken() );
							qRot.z = atof( parser.GetNextToken() );
							qRot.w = atof( parser.GetNextToken() );
						}
						else if( 0 == stricmp( "size", szToken ) )
						{
							vSize.x = atof( parser.GetNextToken() );
							vSize.y = atof( parser.GetNextToken() );
							vSize.z = atof( parser.GetNextToken() );
						}

						szToken = parser.GetNextToken();
					}

					
					// calc
					if( Z3D_BOUNDINGVOLUME_INFO::Z3DBV_OBB == bvi.eBVType )
					{
						z3d::VectorRotate( vTmp, vector3(0,0.5f*vSize.y,0), qRot );

						bvi.vOrigin = vPos + vTmp;
						bvi.vExtent = 0.5f*vSize;
						bvi.qRot = qRot;
					}
					else if( Z3D_BOUNDINGVOLUME_INFO::Z3DBV_CYLINDER == bvi.eBVType )
					{
					}
					else if( Z3D_BOUNDINGVOLUME_INFO::Z3DBV_SPHERE == bvi.eBVType )
					{
					}

					m_vecBoundingVolumeInfo.push_back( bvi ); 

					szToken = parser.GetNextToken();
				}
			}
		}
	}

	parser.Close();

	return true;
}


bool CZ3DGCMDS::ReadSkeletonPartInfo( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		tok = g_TokSkeletonPartName.Add( szToken );
		szToken = rParser.GetNextToken();
		if( NULL == szToken )
		{
			return false;
		}
		m_mapTok2SkelpartIdx[tok] = aton( szToken );
	}
	
	return true;
}


bool CZ3DGCMDS::ReadBoundingCylinderInfo( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;

	Z3D_BOUNDING_CYLINDER_INFO bci;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		if( NULL == szToken )
		{
			return false;
		}
		bci.lBottomIdx = aton(szToken);


		szToken = rParser.GetNextToken();
		if( NULL == szToken )
		{
			return false;
		}
		bci.lTopIdx = aton(szToken);

		szToken = rParser.GetNextToken();
		if( NULL == szToken )
		{
			return false;
		}
		bci.fBottomRadius = atof(szToken);

		szToken = rParser.GetNextToken();
		if( NULL == szToken )
		{
			return false;
		}
		bci.fTopRadius = atof(szToken);

		m_vecBoundingCylinderInfo.push_back( bci );
	}
	
	return true;
}


bool CZ3DGCMDS::ReadMotionSheet( CSimpleParser &rParser )
{
	int i;
	char* szToken;
	Z3DMOTIONSHEET* pCurrentSheet;
	std::stack<Z3DMOTIONSHEET*> stackParentSheet;
		
	Z3DTOK tok;

	pCurrentSheet = NULL;
	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( "sheet", szToken ) )
		{
			szToken = rParser.GetNextToken();
			if( NULL == szToken )
			{
				return false;
			}

			stackParentSheet.push( pCurrentSheet );
			pCurrentSheet = new Z3DMOTIONSHEET;
			pCurrentSheet->rpParentSheet = stackParentSheet.top();

			tok = g_TokSheetName.Add( szToken );
			if( m_mapTok2MotionSheet.end() != m_mapTok2MotionSheet.find( tok ) )
			{
				return false;	// motion sheet 이름 중복
			}
			m_mapTok2MotionSheet[tok] = pCurrentSheet;
		}
		else if( 0 == stricmp( ";", szToken) )
		{
			if( stackParentSheet.empty() )
			{
				break;	// "motionsheet" 토큰과 대응되는 ";", sheet 읽기 끝
			}

			pCurrentSheet = stackParentSheet.top();
			stackParentSheet.pop();
		}
		else
		{
			Z3DTOK tokMotionName = g_TokMotionName.Add( szToken );
			Z3DMOTLET* pMotlet = new Z3DMOTLET;

			Z3DTOKARRAY ta;
			Z3DTOK tokMotionFile;
			ta.Alloc( m_lAniHolderCount );
			for( i = 0; i < m_lAniHolderCount; ++i )
			{
				szToken = rParser.GetNextToken();
				if( NULL == szToken )
				{
					return false;
				}
				if( 0 == stricmp("NULL", szToken) )
				{
					tokMotionFile = NULL_TOK;
				}
				else
				{
					tokMotionFile = g_TokFileName.Add( szToken );
				}
				ta.pArray[i] = tokMotionFile;
			}
			
			if( NULL == (szToken = rParser.GetNextToken()) )
			{
				return false;
			}
			if( 0 == stricmp( ";", szToken ) )	// 한줄짜리의 끝
			{
				pMotlet->Alloc( 1 );
				pMotlet->lProbalilitySum = 1;
				pMotlet->pProbabilityIdx[0] = 1;
				pMotlet->pTokArr[0] = ta;
			}
			else	// 여러줄짜리
			{
				std::vector<Z3DTOKARRAY> vecTa;
				std::vector<long> vecProbabilityFactor;
				vecProbabilityFactor.push_back( aton(szToken) );
				vecTa.push_back( ta );

				while( szToken = rParser.GetNextToken() )
				{
					if( NULL == szToken )
					{
						return false;
					}

					
					if( 0 == stricmp( ";", szToken ) )	// 여러줄읽기 끝
					{
						pMotlet->Alloc( vecTa.size() );
						for( i = 0; i < pMotlet->lCount; ++i )
						{
							pMotlet->pTokArr[i] = vecTa[i];
							pMotlet->pProbabilityIdx[i] = vecProbabilityFactor[i];
						}
						pMotlet->lProbalilitySum = vecProbabilityFactor.back();
						break;	// MOTLET 작성완료
					}
					else
					{
						ta.Alloc( m_lAniHolderCount );
						for( i = 0; i < m_lAniHolderCount; ++i )
						{
							if( NULL == szToken )
							{
								return false;
							}
							if( 0 == stricmp("NULL", szToken) )
							{
								tokMotionFile = NULL_TOK;
							}
							else
							{
								tokMotionFile = g_TokFileName.Add( szToken );
							}
							tokMotionFile = g_TokFileName.Add( szToken );
							ta.pArray[i] = tokMotionFile;
							szToken = rParser.GetNextToken();
						}
						vecTa.push_back( ta );
						// 확률비율 읽기
						if( NULL == szToken )
						{
							return false;
						}
						vecProbabilityFactor.push_back( aton(szToken) + vecProbabilityFactor.back() );
					}
				}
			}
			if( pCurrentSheet->mapTok2Motlet.end() !=
				pCurrentSheet->mapTok2Motlet.find( tokMotionName ) )
			{
				return false;	// 한 sheet 안에 같은 이름의 motion이 중복됨
			}
			pCurrentSheet->mapTok2Motlet[tokMotionName] = pMotlet;
		}
	}

	return true;
}


bool CZ3DGCMDS::ReadStaticSlot( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		tok = g_TokSlotName.Add( szToken );
		// 중복검사
		if( m_vecTokStaticSlot.end() !=
			std::find(
				m_vecTokStaticSlot.begin(), m_vecTokStaticSlot.end(), tok )
		)
		{
			return false;
		}
		m_vecTokStaticSlot.push_back( tok );
	}

	return true;
}


bool CZ3DGCMDS::ReadOutfitSlot( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		tok = g_TokSlotName.Add( szToken );
		// 중복검사
		if( m_vecTokOutfitSlot.end() !=
			std::find(
				m_vecTokOutfitSlot.begin(), m_vecTokOutfitSlot.end(), tok )
		)
		{
			return false;
		}
		m_vecTokOutfitSlot.push_back( tok );


		if( NULL == (szToken = rParser.GetNextToken()) )
		{
			return false;
		}
		m_vecOutfitSlotOverlayCount.push_back( aton(szToken) );
	}

	return true;
}


bool CZ3DGCMDS::ReadAttachementSlot( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		tok = g_TokSlotName.Add( szToken );
		// 중복검사
		if( m_vecTokAttachmentSlot.end() !=
			std::find(
				m_vecTokAttachmentSlot.begin(), m_vecTokAttachmentSlot.end(), tok )
		)
		{
			return false;
		}
		m_vecTokAttachmentSlot.push_back( tok );
	}

	return true;
}


bool CZ3DGCMDS::ReadAttachmentSet( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;
	Z3DATTACHMENTINFO* pAInfo;
	std::vector<Z3DATTACHMENTHOLDERINFO> vecHolder;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		if( 0 == stricmp( "set", szToken ) )
		{
			if( NULL == ( szToken = rParser.GetNextToken() ) )
			{
				return false;
			}
			Z3DTOK tokName = g_TokAttachmentName.Add( szToken );
			pAInfo = new Z3DATTACHMENTINFO;
			// 값이 들어가지 않을수 있는 필드 초기화
			pAInfo->tokTextureFile2 = NULL_TOK;
			vecHolder.clear();

			szToken = rParser.GetNextToken();
			while( NULL != szToken )
			{
				if( 0 == stricmp( "type", szToken ) )
				{
					szToken = rParser.GetNextToken();
					pAInfo->lType = aton( szToken );

					szToken = rParser.GetNextToken();
				}
				else if( 0 == stricmp( "mesh", szToken ) )
				{
					szToken = rParser.GetNextToken();
					pAInfo->tokMeshFile = g_TokFileName.Add( szToken );

					szToken = rParser.GetNextToken();
				}
				else if( 0 == stricmp( "tex", szToken ) )
				{
					szToken = rParser.GetNextToken();
					pAInfo->tokTextureFile = g_TokFileName.Add( szToken );
					szToken = rParser.GetNextToken();
					if( 0 == stricmp( ":", szToken ) )
					{
						szToken = rParser.GetNextToken();
						pAInfo->tokTextureFile2 = g_TokFileName.Add( szToken );
						szToken = rParser.GetNextToken();
					}
				}
				else if( 0 == stricmp( "holder", szToken ) )
				{
					if( false == ReadAttachmentHolder( rParser, vecHolder ) )
					{
						return false;
					}

					szToken = rParser.GetNextToken();
				}
				else if( 0 == stricmp( ";", szToken ) )
				{
					if( m_mapTok2AttachmentInfo.end() != m_mapTok2AttachmentInfo.find( tokName ) )
					{
						return false;	// 무기 중복
					}
					if( 0 == (pAInfo->lHolderCount = vecHolder.size()) )
					{
						return false;
					}
					pAInfo->pHolderInfo = new Z3DATTACHMENTHOLDERINFO[pAInfo->lHolderCount];
					for( int i = 0; i < pAInfo->lHolderCount; ++i )
					{
						pAInfo->pHolderInfo[i] = vecHolder[i];
					}
					m_mapTok2AttachmentInfo[tokName] = pAInfo;
					break;
				}
			}
		}
	}

	return true;
}


bool CZ3DGCMDS::ReadAttachmentHolder( CSimpleParser &rParser, std::vector<Z3DATTACHMENTHOLDERINFO> &r_vecHolder )
{
	char* szToken;
	Z3DTOK tok;
	Z3DATTACHMENTHOLDERINFO holder;

	while( szToken = rParser.GetNextToken() )
	{
		if( NULL == szToken )
		{
			return false;
		}

		if( 0 == stricmp( ";", szToken ) )
		{
			r_vecHolder.push_back( holder );
			break;
		}
		else if( 0 == stricmp( "skel", szToken ) )
		{
			szToken = rParser.GetNextToken();
			holder.tokSkelPart = g_TokSkeletonPartName.Add( szToken );
		}
		else if( 0 == stricmp( "pos", szToken ) )
		{
			holder.vPos.x = atof( rParser.GetNextToken() );
			holder.vPos.y = atof( rParser.GetNextToken() );
			holder.vPos.z = atof( rParser.GetNextToken() );
		}
		else if( 0 == stricmp( "rot", szToken ) )
		{
			holder.qRot.x = atof( rParser.GetNextToken() );
			holder.qRot.y = atof( rParser.GetNextToken() );
			holder.qRot.z = atof( rParser.GetNextToken() );
			holder.qRot.w = atof( rParser.GetNextToken() );
		}
	}

	return true;
}


bool CZ3DGCMDS::ReadOutfitSet( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;
	Z3DTOK tokLink;
	Z3DTOK tokLinkSlot;


	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		if( 0 == stricmp( "set", szToken ) )
		{
			std::vector<Z3DTOK> vecTokMeshName;
			std::vector<Z3DTOK> vecTokTextureName;
			std::vector<Z3DTOK> vecTokTexture2Name;

			if( NULL == ( szToken = rParser.GetNextToken() ) )
			{
				return false;
			}
			Z3DTOK tokName = g_TokOutfitName.Add( szToken );
			if( m_mapTok2OutfitInfo.end() != m_mapTok2OutfitInfo.find(tokName) )
			{
				return false;	// 복장 명칭 중복
			}

			szToken = rParser.GetNextToken();
			if( 0 == stricmp( "link", szToken ) )
			{
				if( NULL == (szToken = rParser.GetNextToken()) )
				{
					return false;
				}
				tokLink = g_TokOutfitName.Add( szToken );

				if( NULL == (szToken = rParser.GetNextToken()) )
				{
					return false;
				}
				tokLinkSlot = g_TokSlotName.GetTOK( szToken );

				szToken = rParser.GetNextToken();
			}
			else
			{
				tokLink = NULL_TOK;
				tokLinkSlot = NULL_TOK;
			}

			while( 0 != stricmp( ";", szToken ) )
			{
				if( 0 == stricmp( "NULL", szToken ) )
				{
					tok = NULL_TOK;
				}
				else
				{
					tok = g_TokFileName.Add( szToken );
				}
				vecTokMeshName.push_back( tok );

				szToken = rParser.GetNextToken();
				if( 0 == stricmp( "NULL", szToken ) )
				{
					tok = NULL_TOK;
				}
				else
				{
					tok = g_TokFileName.Add( szToken );
				}
				vecTokTextureName.push_back( tok );

				szToken = rParser.GetNextToken();
				if( 0 == stricmp( ":", szToken ) )
				{
					szToken = rParser.GetNextToken();
					if( 0 == stricmp( "NULL", szToken ) )
					{
						tok = NULL_TOK;
					}
					else
					{
						tok = g_TokFileName.Add( szToken );
					}
					szToken = rParser.GetNextToken();
				}
				else
				{
					tok = NULL_TOK;
				}
				vecTokTexture2Name.push_back( tok );
			}

			if( 0 == vecTokMeshName.size() )
			{
				return false;
			}
			
			Z3DOUTFITINFO* pOInfo = new Z3DOUTFITINFO;
			pOInfo->Alloc( vecTokMeshName.size() );
			for( int i = 0; i < pOInfo->lCount; ++i )
			{
				pOInfo->pTokMeshName[i] = vecTokMeshName[i];
				pOInfo->pTokTextureName[i] = vecTokTextureName[i];
				pOInfo->pTokTexture2Name[i] = vecTokTexture2Name[i];
			}
			pOInfo->tokLinkOutfit = tokLink;
			pOInfo->tokLinkSlot = tokLinkSlot;
			m_mapTok2OutfitInfo[tokName] = pOInfo;
		}
	}

	if( m_mapTok2OutfitInfo.size() > 1 )
	{
		m_bFixed = false;
	}
	else
	{
		m_bFixed = true;
	}

	return true;
}


bool CZ3DGCMDS::ReadDefaultAttachment( CSimpleParser &rParser )
{
	char* szToken;
	Z3DTOK tok;
	Z3DTOK_SLOT_ATTACHMENT TSA;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		tok = g_TokSlotName.GetTOK( szToken );
		if( tok == NULL_TOK )
		{
			return false;	//	등록되지 않은 슬롯
		}
		TSA.tokSlot = tok;

		if( NULL == ( szToken = rParser.GetNextToken() ) )
		{
			return false;
		}
		tok = g_TokAttachmentName.GetTOK( szToken );
		if( tok == NULL_TOK )
		{
			return false;	//	등록되지 않은 attachment item
		}
		TSA.tokAttachment = tok;
		
		m_vecDefaultAttachment.push_back( TSA );
	}

	return true;
}


bool CZ3DGCMDS::ReadShaderInfo( CSimpleParser &rParser )
{
	char* szToken;
	long lIndex;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		if( 0 == stricmp( "shader", szToken ) )
		{
			szToken = rParser.GetNextToken();
			lIndex = aton(szToken);

			if( lIndex < Z3D_MAX_SHADER_COUNT )
			{
				SAFE_DELETEA( m_aszShaderInfoList[lIndex] );
				
				szToken = rParser.GetNextToken();
				m_aszShaderInfoList[lIndex] = new char[strlen(szToken)+1];
				strcpy( m_aszShaderInfoList[lIndex], szToken );
			}
			else
			{
				// skip text content
				szToken = rParser.GetNextToken();
			}
		}
	}

	return true;
}


bool CZ3DGCMDS::ReadEffectInfo( CSimpleParser &rParser )
{
	char* szToken;
	//Z3D_EFFECT_INFO* pEi;

	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}

		/*if( 0 == stricmp( "effect", szToken ) )
		{
			pEi = new Z3D_EFFECT_INFO;

			szToken = rParser.GetNextToken();
			pEi->szSkelName = new char[strlen(szToken) + 1];
			strcpy( pEi->szSkelName, szToken );
			
			szToken = rParser.GetNextToken();
			pEi->szESFName = new char[strlen(szToken) + 1];
			strcpy( pEi->szESFName, szToken );

			szToken = rParser.GetNextToken();
			pEi->vPos.x = atof( szToken );
			szToken = rParser.GetNextToken();
			pEi->vPos.y = atof( szToken );
			szToken = rParser.GetNextToken();
			pEi->vPos.z = atof( szToken );

			szToken = rParser.GetNextToken();
			pEi->qRot.x = atof( szToken );
			szToken = rParser.GetNextToken();
			pEi->qRot.y = atof( szToken );
			szToken = rParser.GetNextToken();
			pEi->qRot.z = atof( szToken );
			szToken = rParser.GetNextToken();
			pEi->qRot.w = atof( szToken );

			m_vecEffectInfo.push_back( pEi );
		}*/
		char* szTemp = new char[strlen(szToken)+1];
		strcpy( szTemp, szToken );
		m_vecszEffectInfo.push_back(szTemp);
	}
	
	return true;
}


bool CZ3DGCMDS::ReadFixedPoint( CSimpleParser &rParser )
{
	char* szToken;
	long lIndex;
	Z3DBlend2Vertex vTmp;
	
	while( szToken = rParser.GetNextToken() )
	{
		if( 0 == stricmp( ";", szToken ) )
		{
			break;
		}
		
		if( 0 == stricmp( "point", szToken ) )
		{
			szToken = rParser.GetNextToken();
			lIndex = aton(szToken);
			
			while( 1 )
			{
				szToken = rParser.GetNextToken();
				
				if( 0 == stricmp( ";", szToken ) )
				{
					break;
				}
				else if( 0 == stricmp( "position", szToken ) )
				{
					szToken = rParser.GetNextToken();
					vTmp.pos.x = atof( szToken );
					
					szToken = rParser.GetNextToken();
					vTmp.pos.y = atof( szToken );
					
					szToken = rParser.GetNextToken();
					vTmp.pos.z = atof( szToken );
				}
				else if( 0 == stricmp( "bone", szToken ) )
				{
					szToken = rParser.GetNextToken();
					vTmp.mtrx_id[0] = aton( szToken );
					
					szToken = rParser.GetNextToken();
					vTmp.mtrx_id[1] = aton( szToken );
				}
				else if( 0 == stricmp( "factor", szToken ) )
				{
					szToken = rParser.GetNextToken();
					vTmp.bfactor = atof( szToken );
				}
			}
			
			if( lIndex <= 2 || lIndex >= 0 )
			{
				m_aFixedPoint[lIndex] = vTmp;
			}
		}
	}
	
	return true;
}


bool CZ3DGCMDS::Load( const char* szFileName, const char* szPath )
{
	// edith GCMDS 파일 실제로드하는 부분
	CSimpleParser parser;
	Z3DTOK tok;

	//if( false == parser.OpenFile( szFileName ) && szPath )
	if( szPath )
	{
		SimpleString nname( szPath, szFileName );

		if( false == parser.OpenFile( nname ) )
		{
			return false;
		}
	}
	else
	{
		if( false == parser.OpenFile( szFileName ) )
		{
			return false;
		}
	}

	char* szToken;
	char szTmp[260];

	while( (szToken =  parser.GetNextToken()) != NULL )
	{
		if( 0 == stricmp( "skel", szToken ) )
		{
			szToken = parser.GetNextToken();
			if( szPath )
			{
				sprintf( szTmp, "%s%s", szPath, szToken );
			}
			else
			{
				strcpy( szTmp, szToken );
			}

			if( false == ReadSkelFile( szTmp ) )
			{
				return false;
			}

			// szToken 에는 path name 을 붙이기 전의 순수한 skel 파일명이 담겨있음
			m_szSkelFileName = new char[strlen(szToken)+1];
			strcpy( m_szSkelFileName, szToken );
		}
		else if( 0 == stricmp( "skey", szToken ) )
		{
			szToken = parser.GetNextToken();
			if( szPath )
			{
				sprintf( szTmp, "%s%s", szPath, szToken );
			}
			else
			{
				strcpy( szTmp, szToken );
			}
			
			if( false == ReadSkeyFile( szTmp ) )
			{
				return false;
			}

			// szToken 에는 path name 을 붙이기 전의 순수한 skel 파일명이 담겨있음
			m_szSkelFileName = new char[strlen(szToken)+1];
			strcpy( m_szSkelFileName, szToken );
		}
		else if( 0 == stricmp( "meshtype", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}
			long lType = aton( szToken );
			if( 0 == lType )
			{
				m_MPT = Z3D_MPT_TEXPIECE;
			}
			else
			{
				m_MPT = Z3D_MPT_TEXTURE;
			}
		}
		else if( 0 == stricmp( "skelpart", szToken ) )
		{
			if( false == ReadSkeletonPartInfo( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "boundingcylinder", szToken ) )
		{
			if( false == ReadBoundingCylinderInfo( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "motionsheet", szToken ) )
		{
			if( false == ReadMotionSheet( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "staticslot", szToken ) )
		{
			if( false == ReadStaticSlot( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "outfitslot", szToken ) )
		{
			if( false == ReadOutfitSlot( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "attachmentslot", szToken ) )
		{
			if( false == ReadAttachementSlot( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "attachmentset", szToken ) )
		{
			if( false == ReadAttachmentSet( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "outfitset", szToken ) )
		{
			if( false == ReadOutfitSet( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "default_outfit", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}

			if( NULL_TOK == (m_TokDefaultOutfit = g_TokOutfitName.GetTOK( szToken )) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "default_motion", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}

			if( NULL_TOK == (m_TokDefaultMotionSheet = g_TokSheetName.GetTOK( szToken )) )
			{
				return false;
			}

			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}

			if( NULL_TOK == (m_TokDefaultMotion = g_TokMotionName.GetTOK( szToken )) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "default_attachment", szToken ) )
		{
			if( false == ReadDefaultAttachment( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "default_scale", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}

			m_fDefaultScale = atof(szToken);
		}
		else if( 0 == stricmp( "shaderinfo", szToken ) )
		{
			if( false == ReadShaderInfo( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "effectinfo", szToken ) )
		{
			if( false == ReadEffectInfo( parser ) )
			{
				return false;
			}
		}
		else if( 0 == stricmp( "effect_scale", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}
			
			m_fEffectScale = atof(szToken);
		}
		else if( 0 == stricmp( "alpha_used", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}

			if( 0 == stricmp( "true", szToken ) )
			{
				m_bIsAlphaUsed = true;
			}
			else
			{
				m_bIsAlphaUsed = false;
			}
		}
		else if( 0 == stricmp( "nametag_bias", szToken ) )
		{
			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}
			m_fNameTagBias = atof( szToken );
		}

	}

	m_szFileName = new char[strlen(szFileName) + 1];
	strcpy( m_szFileName, szFileName );
	return true;
}


bool CZ3DGCMDS::Save( const char* szFileName, const char* szPath ) const
{
	CZ3DMaskedStream mstrm;
	Z3DTOK tok;
	int i;

	if( NULL == m_szFileName )
	{
		return false;	// GCMDS not loaded yet for this instance
	}

	if( NULL == szFileName )
	{
		szFileName = m_szFileName;
	}

	if( szPath )
	{
		SimpleString nname( szPath, szFileName );
		
		if( false == mstrm.Open( nname ) )
		{
			return false;
		}
	}
	else
	{		if( false == mstrm.Open( szFileName ) )
		{
			return false;
		}
	}
	

	// skey
	mstrm.Printf( "skey \"%s\"\n\n\n", m_szSkelFileName );

	// meshtype (almost obsolete)
	mstrm.Printf( "meshtype %d\t// 0 = TEXPIECE, 1 = TEXTURE\n\n\n", (m_MPT == Z3D_MPT_TEXTURE) ? 1 : 0 );

	// skelpart
	if( m_mapTok2SkelpartIdx.size() > 0 )
	{
		std::map<Z3DTOK, long>::const_iterator it;

		mstrm.Printf( "skelpart\n" );
		
		for( it = m_mapTok2SkelpartIdx.begin(); it != m_mapTok2SkelpartIdx.end(); ++it )
		{
			mstrm.Printf( "\t%s %d\n", g_TokSkeletonPartName.GetString( it->first ), it->second );
		}

		mstrm.Printf( ";\n\n\n" );
	}

	// boundingcylinder
	if( m_vecBoundingCylinderInfo.size() )
	{
		mstrm.Printf( "boundingcylinder\n" );

		for( i = 0; i < (int)m_vecBoundingCylinderInfo.size(); ++i )
		{
			mstrm.Printf( "\t%d\t%d\t%4.1f\t%4.1f\n",
				m_vecBoundingCylinderInfo[i].lBottomIdx,
				m_vecBoundingCylinderInfo[i].lTopIdx,
				m_vecBoundingCylinderInfo[i].fBottomRadius,
				m_vecBoundingCylinderInfo[i].fTopRadius );
		}
		
		mstrm.Printf( ";\n\n\n" );
	}

	// motionsheet
	if( false == WriteMotionSheet( mstrm ) )
	{
		return false;
	}

	// staticslot
	if( m_vecTokStaticSlot.size() )
	{
		mstrm.Printf( "staticslot\n" );
	
		for( i = 0; i < (int)m_vecTokStaticSlot.size(); ++i )
		{
			mstrm.Printf( "\t%s", g_TokSlotName.GetString( m_vecTokStaticSlot[i] ) );			
		}
		
		mstrm.Printf( ";\n\n\n" );
	}

	// outfitslot
	if( m_vecTokOutfitSlot.size() )
	{
		mstrm.Printf( "outfitslot\n" );
		
		for( i = 0; i < (int)m_vecTokOutfitSlot.size(); ++i )
		{
			mstrm.Printf( "\t%s\t%d\n",
				g_TokSlotName.GetString( m_vecTokOutfitSlot[i] ), m_vecOutfitSlotOverlayCount[i] );
		}
		
		mstrm.Printf( ";\n\n\n" );
	}
	
	// attachmentslot
	if( m_vecTokAttachmentSlot.size() )
	{
		mstrm.Printf( "attachmentslot\n" );
		
		for( i = 0; i < (int)m_vecTokAttachmentSlot.size(); ++i )
		{
			mstrm.Printf( "\t%s", g_TokSlotName.GetString( m_vecTokAttachmentSlot[i] ) );			
		}
		
		mstrm.Printf( ";\n\n\n" );
	}

	// attachmentset
	if( m_mapTok2AttachmentInfo.size() )
	{
		std::map<Z3DTOK, Z3DATTACHMENTINFO*>::const_iterator it;

		mstrm.Printf( "attachmentset\n" );

		for( it = m_mapTok2AttachmentInfo.begin(); it != m_mapTok2AttachmentInfo.end(); ++it )
		{
			mstrm.Printf( "\n\tset\t\"%s\"\n", g_TokAttachmentName.GetString( it->first ) );
			mstrm.Printf( "\t\ttype\t%d\n", it->second->lType );
			mstrm.Printf( "\t\tmesh\t\"%s\"\n", g_TokFileName.GetString( it->second->tokMeshFile ) );
			mstrm.Printf( "\t\ttex\t\t\"%s\"\n", g_TokFileName.GetString( it->second->tokTextureFile ) );

			for( i = 0; i < it->second->lHolderCount; ++i )
			{
				mstrm.Printf( "\t\tholder\n" );

				mstrm.Printf( "\t\t\tskel\t%s\n",
					g_TokSkeletonPartName.GetString( it->second->pHolderInfo[i].tokSkelPart ) );
				mstrm.Printf( "\t\t\tpos\t%8.4f\t%8.4f\t%8.4f\n",
					it->second->pHolderInfo[i].vPos.x,
					it->second->pHolderInfo[i].vPos.y,
					it->second->pHolderInfo[i].vPos.z );
				mstrm.Printf( "\t\t\trot\t%f\t%f\t%f\t%f\n",
					it->second->pHolderInfo[i].qRot.x,
					it->second->pHolderInfo[i].qRot.y,
					it->second->pHolderInfo[i].qRot.z,
					it->second->pHolderInfo[i].qRot.w );					

				mstrm.Printf( "\t\t;\n" );
			}

			mstrm.Printf( "\t;\n\n" );
		}

		mstrm.Printf( ";\n\n\n" );
	}

	// outfitset
	if( m_mapTok2OutfitInfo.size() )
	{
		std::map<Z3DTOK, Z3DOUTFITINFO*>::const_iterator it;
		
		mstrm.Printf( "outfitset\n" );

		for( it = m_mapTok2OutfitInfo.begin(); it != m_mapTok2OutfitInfo.end(); ++it )
		{
			mstrm.Printf( "\tset\t\"%s\"", g_TokOutfitName.GetString( it->first ) );
			if( NULL_TOK == it->second->tokLinkSlot )
			{
				mstrm.Printf( "\n" );
			}
			else
			{
				mstrm.Printf( "\tlink \"%s\" %s\n",
					g_TokOutfitName.GetString( it->second->tokLinkOutfit ),					
					g_TokSlotName.GetString( it->second->tokLinkSlot ) );
			}

			for( i = 0; i < it->second->lCount; ++i )
			{
				if( NULL_TOK == it->second->pTokTextureName[i] )
				{
					mstrm.Printf( "\t\t\"%s\"\tNULL\n", 
						g_TokFileName.GetString( it->second->pTokMeshName[i] ) );
				}
				else
				{
					mstrm.Printf( "\t\t\"%s\"\t\"%s\"\n",
						g_TokFileName.GetString( it->second->pTokMeshName[i] ),
						g_TokFileName.GetString( it->second->pTokTextureName[i] ) );
				}
			}

			mstrm.Printf( "\t;\n" );
		}

		mstrm.Printf( ";\n\n\n" );
	}

	// default_outfit
	mstrm.Printf( "default_outfit\t\"%s\"\n\n\n",
		g_TokOutfitName.GetString(m_TokDefaultOutfit) );

	// default_motion
	mstrm.Printf( "default_motion\t%s\t%s\n\n\n",
		g_TokSheetName.GetString( m_TokDefaultMotionSheet ),
		g_TokMotionName.GetString( m_TokDefaultMotion ) );
		
	// default_attachment
	if( m_vecDefaultAttachment.size() )
	{
		mstrm.Printf( "default_attachment\n" );

		for( i = 0; i < (int)m_vecDefaultAttachment.size(); ++i )
		{
			mstrm.Printf( "\t%s\t\"%s\"\n",
				g_TokSlotName.GetString( m_vecDefaultAttachment[i].tokSlot ),
				g_TokAttachmentName.GetString( m_vecDefaultAttachment[i].tokAttachment ) );
		}
		
		mstrm.Printf( ";\n\n\n" );
	}

	// default_scale
	if( 1.0f != m_fDefaultScale )
	{
		mstrm.Printf( "default_scale %f\n\n\n", m_fDefaultScale );
	}
	
	// shaderinfo
	for( i = 0; i < Z3D_MAX_SHADER_COUNT; ++i )
	{
		if( NULL != m_aszShaderInfoList[i] )
		{
			break;
		}
	}
	if( i < Z3D_MAX_SHADER_COUNT )	// shader 정보가 하나라도 있을 경우(위 루프 중간에서 빠져나옴)
	{
		mstrm.Printf( "shaderinfo\n" );
		
		for( i = 0; i < Z3D_MAX_SHADER_COUNT; ++i )
		{
			if( NULL != m_aszShaderInfoList[i] )
			{
				mstrm.Printf( "\tshader\t%d\t\"%s\"\n", i, m_aszShaderInfoList[i] );
			}
		}
				
		mstrm.Printf( ";\n\n\n" );
	}

	// effectinfo
	if( m_vecszEffectInfo.size() )
	{
		mstrm.Printf( "effectinfo\n" );

		for( i = 0; i < (int)m_vecszEffectInfo.size(); ++i )
		{
			mstrm.Printf( "\t\"%s\"\n", m_vecszEffectInfo[i] );
		}
		
		mstrm.Printf( ";\n\n\n" );
	}
	
	// effect_scale
	if( 1.0f != m_fEffectScale )
	{
		mstrm.Printf( "effect_scale %f\n\n\n", m_fEffectScale );
	}

	return true;
}


static const char* MakeIndent( int n )
{
	static char szBuffer[100];

	if( n > 99 ) n = 99;
	int i = 0;
	for(; i < n; ++i )
	{
		szBuffer[i] = '\t';
	}
	szBuffer[i] = '\0';

	return szBuffer;
}

#include <list>
#include "GMMemory.h"

static bool DumpSheet( CZ3DMaskedStream &rMstrm,
					  std::list< std::pair< Z3DTOK,Z3DMOTIONSHEET* > > listMotionSheet,
					  std::pair< Z3DTOK, Z3DMOTIONSHEET* > currentSheet, int nIndentCount )
{
	std::map< Z3DTOK, Z3DMOTLET* >::iterator itMotlet;
	std::list< std::pair< Z3DTOK,Z3DMOTIONSHEET* > >::iterator itMotionSheet;
	std::pair< Z3DTOK, Z3DMOTIONSHEET* > childSheet;
	Z3DMOTLET* pMotlet = NULL;
	int i, j;
	const char* szFileName;
	
	// motion sheet name
	rMstrm.Printf( "\n%ssheet %s\n", MakeIndent( nIndentCount ),
		g_TokSheetName.GetString(currentSheet.first) );
	
	for( itMotlet = currentSheet.second->mapTok2Motlet.begin();
	itMotlet != currentSheet.second->mapTok2Motlet.end(); ++itMotlet )
	{
		// motion name
		rMstrm.Printf( "%s%s\n", MakeIndent( nIndentCount+1 ),
			g_TokMotionName.GetString( itMotlet->first ) );
		
		pMotlet = itMotlet->second;
		
		// 단일 동작
		if( 1 == pMotlet->lCount )
		{
			rMstrm.Printf( "%s", MakeIndent( nIndentCount+1 ) );
			for( i = 0; i < pMotlet->pTokArr->lCount; ++i )
			{
				if( NULL_TOK == pMotlet->pTokArr->pArray[i] )
				{
					szFileName = "NULL";
				}
				else
				{
					szFileName = g_TokFileName.GetString( pMotlet->pTokArr->pArray[i] );
				}

				rMstrm.Printf( "\t%s", szFileName );
			}
			rMstrm.Printf( ";\n\n" );
		}
		// 다중 동작(random)
		else
		{
			long lPrevProbability = 0;
			
			for( i = 0; i < pMotlet->lCount; ++i )
			{
				rMstrm.Printf( "%s", MakeIndent( nIndentCount+1 ) );
				for( j = 0; j < pMotlet->pTokArr[i].lCount; ++j )
				{
					if( NULL_TOK == pMotlet->pTokArr->pArray[j] )
					{
						szFileName = "NULL";
					}
					else
					{
						szFileName = g_TokFileName.GetString( pMotlet->pTokArr->pArray[j] );
					}
					
					rMstrm.Printf( "\t%s", szFileName );
				}
				rMstrm.Printf( "\t%d", pMotlet->pProbabilityIdx[i] - lPrevProbability );
				lPrevProbability = pMotlet->pProbabilityIdx[i];
				
				if( i == pMotlet->lCount-1 )	// last item ?
				{
					rMstrm.Printf( ";\n\n" );
				}
				else
				{
					rMstrm.Printf( "\n" );
				}
			}
		}
	}

	for( itMotionSheet = listMotionSheet.begin();
	itMotionSheet != listMotionSheet.end(); ++itMotionSheet )
	{
		if( currentSheet.second == itMotionSheet->second->rpParentSheet )
		{
			DumpSheet( rMstrm, listMotionSheet, *itMotionSheet, nIndentCount+1 );
		}
	}

	rMstrm.Printf( "%s;\n", MakeIndent( nIndentCount ),
		g_TokSheetName.GetString(currentSheet.first) );

	return true;
}


bool CZ3DGCMDS::WriteMotionSheet( CZ3DMaskedStream &rMstrm ) const
{
	std::map< Z3DTOK, Z3DMOTIONSHEET* >::const_iterator it;
	std::list< std::pair< Z3DTOK,Z3DMOTIONSHEET* > > listMotionSheet;
	std::list< std::pair< Z3DTOK,Z3DMOTIONSHEET* > >::iterator itMotionSheet;


	std::pair< Z3DTOK, Z3DMOTIONSHEET* > rootSheet;

	for( it = m_mapTok2MotionSheet.begin(); it != m_mapTok2MotionSheet.end(); ++it )
	{
		listMotionSheet.push_back( *it );
	}

	if( 0 == listMotionSheet.size() )
	{
		return false;	// no motion sheet (but it's not possible)
	}
	

	rMstrm.Printf( "motionsheet\n" );
	
	int nIndentCount = 1;

	// search for the root sheet
	rootSheet.second = NULL;
	for( itMotionSheet = listMotionSheet.begin();
		itMotionSheet != listMotionSheet.end(); ++itMotionSheet )
	{
		if( NULL == itMotionSheet->second->rpParentSheet )
		{
			rootSheet = *itMotionSheet;
			break;
		}
	}
	if( NULL == rootSheet.second )
	{
		return false;	// no root sheet ( it's not possible, either )
	}

	DumpSheet( rMstrm, listMotionSheet, rootSheet, 1 );

	rMstrm.Printf( ";\n\n\n" );
	
	return true;
}


// 이용 관련 함수

long CZ3DGCMDS::GetSkeletonPartObjectIdx( const char* szSkeletonPartName )
{
	Z3DTOK tok = g_TokSkeletonPartName.GetTOK( szSkeletonPartName );
	if( NULL_TOK == tok )
	{
		return -1;
	}

	return GetSkeletonPartObjectIdx( tok );
}


long CZ3DGCMDS::GetSkeletonPartObjectIdx( Z3DTOK tokSkeletonPartName )
{
	std::map<Z3DTOK, long>::iterator it;

	if( m_mapTok2SkelpartIdx.end() == ( it = m_mapTok2SkelpartIdx.find(tokSkeletonPartName) ) )
	{
		return -1;
	}

	return it->second;
}


bool CZ3DGCMDS::AllocSkeleton( CZ3DSkeletonObject* &pSkeleton )
{
	pSkeleton = new CZ3DSkeletonObject[m_lSkeletonCount];

	for( long i = 0; i < m_lSkeletonCount; ++i )
	{
		if( -1 == m_pSkeletonHierarchy[i] )
		{
			pSkeleton[i].Link( NULL );
		}
		else
		{
			pSkeleton[i].Link( &(pSkeleton[m_pSkeletonHierarchy[i]]) );
		}

		pSkeleton[i].GetTM()->MakeIdent();
		pSkeleton[i].SetIndex( m_pAniHolderIndex[i], m_pIndexInSubpart[i] );
	}

	return true;	
}

const Z3DOUTFITINFO* CZ3DGCMDS::GetOutfitInfo( Z3DTOK tokOutfitName )
{
	std::map<Z3DTOK, Z3DOUTFITINFO*>::iterator it;

	it = m_mapTok2OutfitInfo.find( tokOutfitName );
	if( it == m_mapTok2OutfitInfo.end() )
	{
		return NULL;
	}
	//else
	return it->second;
}


const Z3DATTACHMENTINFO* CZ3DGCMDS::GetAttachmentInfo( Z3DTOK tokAttachmentName )
{
	std::map<Z3DTOK, Z3DATTACHMENTINFO*>::iterator it;

	it = m_mapTok2AttachmentInfo.find( tokAttachmentName );
	if( it == m_mapTok2AttachmentInfo.end() )
	{
		return NULL;
	}
	//else
	return it->second;
}


Z3DMOTIONSHEET* CZ3DGCMDS::GetMotionsheet( Z3DTOK tokSheetName )
{
	std::map<Z3DTOK, Z3DMOTIONSHEET*>::iterator it;

	it = m_mapTok2MotionSheet.find( tokSheetName );
	if( m_mapTok2MotionSheet.end() == it )
	{
		return NULL;
	}

	return it->second;
}


long CZ3DGCMDS::GetEffectInfoList( const char** &rpszEffectInfo )
{
	if( 0 == m_vecszEffectInfo.size() )
	{
		rpszEffectInfo = NULL;
		return 0;
	}

	rpszEffectInfo = const_cast<const char**>( &(m_vecszEffectInfo[0]) );
	return m_vecszEffectInfo.size();
}


float CZ3DGCMDS::GetSizeFactor()
{
	long lIdx = GetSkeletonPartObjectIdx( "HEAD" );
	if( -1 == lIdx )
	{
		return 0.0f;
	}

	float fRet = 0.0f;
	
	fRet += m_pLocalizerMatrix[lIdx]._41 * m_pLocalizerMatrix[lIdx]._41;
	fRet += m_pLocalizerMatrix[lIdx]._42 * m_pLocalizerMatrix[lIdx]._42;
	fRet += m_pLocalizerMatrix[lIdx]._43 * m_pLocalizerMatrix[lIdx]._43;

	return sqrtf( fRet );
}




// 편집용 retrieval / modification 함수군
const char* CZ3DGCMDS::GetSkeyFileName()
{
	return m_szSkelFileName;
}

void CZ3DGCMDS::SetSkeyFilename( const char* szFileName )
{
	if( false == ReadSkeyFile( szFileName ) )
	{
		return;
	}

	SAFE_DELETEA( m_szSkelFileName );
	m_szSkelFileName = new char[strlen(szFileName)+1];
	strcpy( m_szSkelFileName, szFileName );
}

long CZ3DGCMDS::GetMeshType()
{
	if( Z3D_MPT_TEXPIECE == m_MPT )
	{
		return 0;
	}
	
	if( Z3D_MPT_TEXTURE == m_MPT )
	{
		return 1;
	}

	return 1;
}

void CZ3DGCMDS::SetMeshType( long lMeshType )
{
	if( 0 == lMeshType )
	{
		m_MPT = Z3D_MPT_TEXPIECE;
	}

	if( 1 == lMeshType )
	{
		m_MPT = Z3D_MPT_TEXTURE;
	}
}

void CZ3DGCMDS::RetrieveSkelpartList( std::map<Z3DTOK, long>* &prSkelpartList )
{
	prSkelpartList = &m_mapTok2SkelpartIdx;
}

void CZ3DGCMDS::RetrieveMotionSheetList( std::map<Z3DTOK, Z3DMOTIONSHEET*>* &prMotionsheetList )
{
	prMotionsheetList = &m_mapTok2MotionSheet;
}

void CZ3DGCMDS::RetrieveOutfitList( std::map<Z3DTOK, Z3DOUTFITINFO*>* &prOutfitList )
{
	prOutfitList = &m_mapTok2OutfitInfo;
}

void CZ3DGCMDS::RetrieveAttachmentList( std::map<Z3DTOK, Z3DATTACHMENTINFO*>* &prAttachmentList )
{
	prAttachmentList = &m_mapTok2AttachmentInfo;
}

void CZ3DGCMDS::RetrieveStaticSlot( std::vector<Z3DTOK>* &prStaticSlot )
{
	prStaticSlot = &m_vecTokStaticSlot;
}

void CZ3DGCMDS::RetrieveOutfitSlot( std::vector<Z3DTOK>* &prOutfitSlot, std::vector<long>* &prOSOverlapCount )
{
	prOutfitSlot = &m_vecTokOutfitSlot;
	prOSOverlapCount = &m_vecOutfitSlotOverlayCount;
}

void CZ3DGCMDS::RetrieveAttachmentSlot( std::vector<Z3DTOK>* &prAttachmentSlot )
{
	prAttachmentSlot = &m_vecTokAttachmentSlot;
}

Z3DTOK CZ3DGCMDS::GetDefaultOutfit()
{
	return m_TokDefaultOutfit;
}

void CZ3DGCMDS::SetDefaultOutfit( Z3DTOK tokDefaultOutfit )
{
	m_TokDefaultOutfit = tokDefaultOutfit;
}

Z3DTOK CZ3DGCMDS::GetDefaultMotion()
{
	return m_TokDefaultMotion;
}

void CZ3DGCMDS::SetDefaultMotion( Z3DTOK tok )
{
	m_TokDefaultMotion = tok;
}

/*void CZ3DGCMDS::GetDefaultMotion( Z3DTOK &rtokMotionSheet, Z3DTOK &rtokMotion )
{
	rtokMotionSheet = m_TokDefaultMotionSheet;
	rtokMotion = m_TokDefaultMotion;
}

void CZ3DGCMDS::SetDefaultMotion( Z3DTOK tokMotionSheet, Z3DTOK tokMotion )
{
	m_TokDefaultMotionSheet = tokMotionSheet;
	m_TokDefaultMotion = tokMotion;
}*/

Z3DTOK CZ3DGCMDS::GetDefaultMotionSheet()
{
	return m_TokDefaultMotionSheet;
}

void CZ3DGCMDS::SetDefaultMotionSheet( Z3DTOK tok )
{
	m_TokDefaultMotionSheet = tok;
}

float CZ3DGCMDS::GetDefaultScale()
{
	return m_fDefaultScale;
}

void CZ3DGCMDS::SetDefaultScale( float f )
{
	if( f > 0.0f )
	{
		m_fDefaultScale = f;
	}
}

void CZ3DGCMDS::RetrieveDefaultAttachmentList( std::vector<Z3DTOK_SLOT_ATTACHMENT>* &prDefaultAttachmentList )
{
	prDefaultAttachmentList = &m_vecDefaultAttachment;
}

void CZ3DGCMDS::RetrieveEffectInfoList( std::vector<char*>* &prEffectInfoList )
{
	prEffectInfoList = &m_vecszEffectInfo;
}


const char* CZ3DGCMDS::GetSkeletonName( int idx )
{
	if( idx >= m_lSkeletonCount )
	{
		return NULL;
	}

	if( NULL == m_pszSkeletonName )
	{
		return NULL;
	}

	return m_pszSkeletonName[idx];
}


float CZ3DGCMDS::GetEffectScale()
{
	return m_fEffectScale;
}

void CZ3DGCMDS::SetEffectScale( float f )
{
	if( f > 0.0f )
	{
		m_fEffectScale = f;
	}
}


void CZ3DGCMDS::AllocBoundingVolumeObject( CZ3DSkeletonObject* pSkels, std::vector<CZ3DBoundingVolumeObject*>& vec_pBV )
{
	CZ3DBoundingVolumeObject* pBVO;

	for( int i = 0; i < (int)m_vecBoundingVolumeInfo.size(); ++i )
	{
		pBVO = new CZ3DBoundingVolumeObject;

		if( m_vecBoundingVolumeInfo[i].lSkelIndex < m_lSkeletonCount )
		{
			pBVO->Link( &(pSkels[m_vecBoundingVolumeInfo[i].lSkelIndex]) );
		}
		else
		{
			pBVO->Link( NULL );
		}

		pBVO->SetPositionLocal( m_vecBoundingVolumeInfo[i].vOrigin );
		pBVO->SetDirectionLocal( m_vecBoundingVolumeInfo[i].qRot );
		pBVO->SetExtent( m_vecBoundingVolumeInfo[i].vExtent );

		vec_pBV.push_back( pBVO );
	}
}


int CZ3DGCMDS::BoundingVolumeIndex2SkelIndex( int nBVIndex )
{
	if( nBVIndex >=  (int)m_vecBoundingVolumeInfo.size() )
	{
		return 0;	// not exactly proper, but for safety
	}

	return m_vecBoundingVolumeInfo[nBVIndex].lSkelIndex;
}


Z3DBlend2Vertex& CZ3DGCMDS::GetFixedPoint( int nIdx )
{
	if( nIdx > 2 )
	{
		nIdx = 2;
	}
	
	return m_aFixedPoint[nIdx];
}