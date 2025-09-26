// Z3DAniKeyPack.cpp: implementation of the Z3DAniKeyPack class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DAniKeyPack.h"

#include <stdio.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

bool Z3DAniKeyPack::Load( char* szFilename )
{
	// load animation key pack( set of animation keys ) data
	FILE* fp;

	if( (fp = fopen( szFilename, "rb" ) ) == NULL )
		return false; // file open error

	fread( &m_wId, sizeof(WORD), 1, fp );
	fread( &m_sAniControllerCount, sizeof(short), 1, fp );
	fread( &m_sAniLength, sizeof(short), 1, fp );

	m_pAniControllers = new Z3DAnimationController[m_sAniControllerCount];

	for( int i = 0; i < m_sAniControllerCount; i++ )
	{
		Z3DAnimationController& rCtrler = m_pAniControllers[i];
		// TM inverse 성분은 .skel 파일로 옮겨감 - .Apack에서 삭제
		//fread( &rCtrler.m_TMInvPos, sizeof(vector3), 1, fp );
		//fread( &rCtrler.m_TMInvRot, sizeof(quaternion), 1, fp );

		fread( &rCtrler.m_sPosKeyCount, sizeof(short), 1, fp );
		fread( &rCtrler.m_sRotKeyCount, sizeof(short), 1, fp );

		rCtrler.m_pPosKeys = new Z3DPosKey[rCtrler.m_sPosKeyCount];
		rCtrler.m_pRotKeys = new Z3DRotKey[rCtrler.m_sRotKeyCount];

		fread( rCtrler.m_pPosKeys, sizeof(Z3DPosKey), rCtrler.m_sPosKeyCount, fp );
		/*for( int k = 0; k < rCtrler.m_sPosKeyCount; k++ )
		{
			fread( &(rCtrler.m_pPosKeys[k].tick), sizeof(short), 1, fp );
			fread( &(rCtrler.m_pPosKeys[k].pos.x), sizeof(float), 1, fp );
			fread( &(rCtrler.m_pPosKeys[k].pos.y), sizeof(float), 1, fp );
			fread( &(rCtrler.m_pPosKeys[k].pos.z), sizeof(float), 1, fp );
		}*/

		fread( rCtrler.m_pRotKeys, sizeof(Z3DRotKey), rCtrler.m_sRotKeyCount, fp );
		/*for( k = 0; k < rCtrler.m_sPosKeyCount; k++ )
		{
			fread( &(rCtrler.m_pRotKeys[k].tick), sizeof(short), 1, fp );
			fread( &(rCtrler.m_pRotKeys[k].rot.x), sizeof(float), 1, fp );
			fread( &(rCtrler.m_pRotKeys[k].rot.y), sizeof(float), 1, fp );
			fread( &(rCtrler.m_pRotKeys[k].rot.z), sizeof(float), 1, fp );
			fread( &(rCtrler.m_pRotKeys[k].rot.w), sizeof(float), 1, fp );
		}*/
	}

	fclose(fp);
	return true;
}


bool Z3DAniKeyPack::Save( char* szFileName )
{
	FILE* fp;

	if( 0 >= m_sAniLength )
	{
		return false;
	}
	if( 0 >= m_sAniControllerCount )
	{
		return false;
	}
	
	if( (fp = fopen( szFileName, "wb" ) ) == NULL )
		return false; // file open error

	fwrite( &m_wId, sizeof(WORD), 1, fp );
	fwrite( &m_sAniControllerCount, sizeof(short), 1, fp );
	fwrite( &m_sAniLength, sizeof(short), 1, fp );

	for( int i = 0; i < m_sAniControllerCount; i++ )
	{
		Z3DAnimationController& rCtrler = m_pAniControllers[i];

		fwrite( &rCtrler.m_sPosKeyCount, sizeof(short), 1, fp );
		fwrite( &rCtrler.m_sRotKeyCount, sizeof(short), 1, fp );

		fwrite( rCtrler.m_pPosKeys, sizeof(Z3DPosKey), rCtrler.m_sPosKeyCount, fp );

		fwrite( rCtrler.m_pRotKeys, sizeof(Z3DRotKey), rCtrler.m_sRotKeyCount, fp );
	}

	fclose(fp);
	return true;
}


bool Z3DAniKeyPack::GetDisplacement( vector3 &v_disp, long lControllerIndex )
{
	if( lControllerIndex >= m_sAniControllerCount )
	{
		return false;
	}

	Z3DAnimationController &rAniController = m_pAniControllers[lControllerIndex];

	long lLastIndex = rAniController.m_sPosKeyCount - 1;
	v_disp = rAniController.m_pPosKeys[lLastIndex].pos - rAniController.m_pPosKeys[0].pos;
	return true;
}