// SectorAmbienceMap.cpp: implementation of the CSectorAmbienceMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SoundMgr.h"
#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "SectorAmbienceMap.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct TESTSCREEN_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
	float u, v;
};


CSectorAmbienceMap::CSectorAmbienceMap() : 	m_pAmbTexture(NULL)
{
	m_ResizeTexture.Create( AMB_TEXTURESIZE, AMB_TEXTURESIZE );
}

CSectorAmbienceMap::~CSectorAmbienceMap()
{
	SAFE_RELEASE( m_pAmbTexture ); 
	DeleteScriptData(); 
	DeleteAmbData(); 
}

void CSectorAmbienceMap::WriteScriptData( const char* strFilename )
{
	char strSectorAmb[MAX_PATH]; 
	char strMsg[MAX_PATH]; 
	ZeroMemory( strMsg, sizeof(strMsg) ); 
	ZeroMemory( strSectorAmb, sizeof(strSectorAmb) ); 

	strcpy( strSectorAmb, APP_SECTOR_AMBIENCE ); 

	FILE* fp = fopen(strFilename, "wt");

	int iZoneID = 1;
	int iSID = 1;

	fprintf(fp, "[SET]\r\n");

	int iSetCount = m_SectorAmbList.size();

	fprintf(fp, "AMBIENCE  = %d\r\n", iSetCount);

	fprintf(fp, "\r\n");

	for( int count = 0; count < iSetCount; count++ )
	{
		// [SECTOR_AMBIENCE1]
		fprintf(fp, "[%s%d]\r\n", APP_SECTOR_AMBIENCE, iSID++ ); 

		// ZONE
		fprintf(fp, "ZONE = %d\r\n", iZoneID); 

		// SECTORX = 8
		fprintf(fp, "SECTORX = %d\r\n", m_SectorAmbList[count]->iSectorX);
		// SECTORY = 1
		fprintf(fp, "SECTORY = %d\r\n", m_SectorAmbList[count]->iSectorY);
		// TEXTURE = "C:\\WorkTest\\WorldCreator\\AmbienceTest.bmp"
		fprintf(fp, "TEXTURE = %s\r\n", m_SectorAmbList[count]->strTexFilename);

		// 파일에 저장된 텍스쳐 컨버팅
		sprintf(strMsg, "C:\\z%d_%d_%d.bmp", iZoneID, m_SectorAmbList[count]->iSectorX, m_SectorAmbList[count]->iSectorY);
		SaveColorTable(m_SectorAmbList[count]->ColorTable[0], strMsg);

		//NUM_AMBIENCE = 2
		int inum = m_SectorAmbList[count]->AmbInfoList.size();
		fprintf(fp, "NUM_AMBIENCE = %d\r\n", inum);

		fprintf(fp, "\r\n");

		for(int i = 0; i < inum; i++)
		{
			AmbienceInfo* pAmbInfo = m_SectorAmbList[count]->AmbInfoList[i];

			fprintf(fp, "AMB%d_COLOR = 0x%06x\r\n", i+1, pAmbInfo->dwColor);

//			fprintf(fp, "AMB%d_POINTCOLOR = 0x%x\r\n", i+1, pAmbInfo->dwPointColor);

			int iSchedule = pAmbInfo->ScheduleList.size();
			fprintf(fp, "AMB%d_SCHEDULE = %d\r\n", i+1, iSchedule );

			fprintf(fp, "\r\n");

			//AMB1_SCHEDULE = 2
			for( int j = 0; j < iSchedule; j++ )
			{
				//Create AmbiencePoint Schedule 
				AmbSchedule* pSchedule = pAmbInfo->ScheduleList[j];

				fprintf(fp, "AMB%d_START_SCHEDULE%d = %d\r\n", i+1, j+1, (int)pSchedule->fStartTime );
				fprintf(fp, "AMB%d_END_SCHEDULE%d = %d\r\n", i+1, j+1, (int)pSchedule->fEndTime );

				int iLe = strlen(AMBIENCEPATH);
				sprintf( strMsg, "%s", &pSchedule->strFilename[iLe] );

				fprintf(fp, "AMB%d_SOUND%d = %s\r\n", i+1, j+1, strMsg );
				fprintf(fp, "\r\n");

				int irn = (int)pSchedule->RandomSndList.size();
				int x = 0;
				for( ; x < irn; x++)
				{
					fprintf(fp, "AMB%d_SCHEDULE%d_RANDOM%d = %s\r\n", i+1, j+1, x+1, pSchedule->RandomSndList[x]);
				}

				fprintf(fp, "AMB%d_SCHEDULE%d_RANDOM_CYCLE = %d\r\n", i+1, j+1, x+1, pSchedule->iRandSndCycle);
				fprintf(fp, "\r\n");
			}
		}
	}

	fclose(fp);
}

void CSectorAmbienceMap::GetScriptData( const char* strFilename )
{
	int SecAmbNum, Zone;
	char strSectorAmb[MAX_PATH]; 
	char strMsg[MAX_PATH]; 
	ZeroMemory( strMsg, sizeof(strMsg) ); 
	ZeroMemory( strSectorAmb, sizeof(strSectorAmb) ); 

	strcpy( strSectorAmb, APP_SECTOR_AMBIENCE ); 

	DeleteScriptData(); 

	SecAmbNum = GetPrivateProfileInt( APP_SET, KEY_AMBIENCE, 0, strFilename );

	for( int count = 1; count < SecAmbNum+1; count++ )
	{
		/*** Script Interface (ex) ***

		[SET]
		AMBIENCE = 2

		[SECTOR_AMBIENCE1]
		ZONE = 1
		SECTORX = 8
		SECTORY = 1
		TEXTURE = "C:\\WorkTest\\WorldCreator\\AmbienceTest.bmp"
		NUM_AMBIENCE = 2

		AMB1_COLOR = 0xff
		AMB1_SCHEDULE = 2
		AMB1_POINTCOLOR = 0xffff; 

		AMB1_START_SCHEDULE1 = 5
		AMB1_END_SCHEDULE1 = 6
		AMB1_SOUND1 = "C:\\WorkTest\\WorldCreator\\AmbSound1.wav"

		AMB1_START_SCHEDULE2 = 6
		AMB1_END_SCHEDULE2 = 7
		AMB1_SOUND2 = "C:\\WorkTest\\WorldCreator\\AmbSound2.wav"

		AMB2_COLOR = 0xffff
		AMB2_SCHEDULE = 1
		AMB2_POINTCOLOR = 0xffffff; 

		AMB2_START_SCHEDULE1 = 1
		AMB2_END_SCHEDULE1 = 2
		AMB2_SOUND1 = "C:\\WorkTest\\WorldCreator\\AmbSound2.wav"

		[SECTOR_AMBIENCE2]
		.........

		*/

		SectorAmbience* pSecAmb = new SectorAmbience; 
		sprintf( strSectorAmb, "%s%d", APP_SECTOR_AMBIENCE, count ); 

		//--------------
		// ZONE 
		//--------------
		sprintf( strMsg, "%s, %s", strSectorAmb, KEY_ZONE ); 
		Zone = GetPrivateProfileInt( strSectorAmb, KEY_ZONE, -1, strFilename );
		if( Zone == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK ); 

		//--------------
		// SECTORX  
		//--------------
		sprintf( strMsg, "%s, %s", strSectorAmb, KEY_SECTORX ); 
		pSecAmb->iSectorX = GetPrivateProfileInt( strSectorAmb, KEY_SECTORX, -1, strFilename ); 
		if( pSecAmb->iSectorX == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );

		//--------------
		// SECTORY
		//--------------
		sprintf( strMsg, "%s, %s", strSectorAmb, KEY_SECTORY ); 
		pSecAmb->iSectorY = GetPrivateProfileInt( strSectorAmb, KEY_SECTORY, -1, strFilename ); 
		if( pSecAmb->iSectorY == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );

		//--------------
		// NUM_AMBIENCE
		//--------------
		int AmbNum; 
		sprintf( strMsg, "%s, %s", strSectorAmb, KEY_NUM_AMBIENCE ); 
		AmbNum = GetPrivateProfileInt( strSectorAmb, KEY_NUM_AMBIENCE, -1, strFilename ); 
		if( AmbNum == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );

		//--------------
		// TEXTURE
		//--------------
		char buf[MAX_PATH];
		ZeroMemory( buf, sizeof(buf) ); 
		sprintf( strMsg, "%s, %s", strSectorAmb, KEY_TEXTURE ); 
		GetPrivateProfileString( strSectorAmb, KEY_TEXTURE, "x", buf, sizeof(buf), strFilename );
		if( !strcmp(buf, "x") ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );
		else strcpy( pSecAmb->strTexFilename, buf ); 

		LoadAmbTexture( buf ); 

		for( int i = 0; i < AmbNum; i++ )
		{
			int iSchedule; 

			//Create AmbienceInfo 
			AmbienceInfo* pAmbInfo = new AmbienceInfo; 

			//------------------
			// AMB()_COLOR
			//------------------
			sprintf( buf, "%s%d_%s", KEY_AMB, i+1, KEY_COLOR );
			sprintf( strMsg, "%s, %s", strSectorAmb, buf );
			int temp = (DWORD)GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename );
			if( temp == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );
			else pAmbInfo->dwColor = (DWORD)temp; 

			//------------------
			// AMB()_POINTCOLOR
			//------------------
			sprintf( buf, "%s%d_%s", KEY_AMB, i+1, KEY_POINTCOLOR );
			sprintf( strMsg, "%s, %s", strSectorAmb, buf );
			temp = (DWORD)GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename );
			if( temp == -1 ) { }//MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK ); }

			/* 일단은 쓰이지 않음
			else 
			{
			pAmbInfo->dwPointColor = (DWORD)temp; 

			//Push AmbiencePoint Position 
			RECT rct; 
			D3DLOCKED_RECT PixelData; 					
			SetRect( &rct, 0, 0, AMB_TEXTURESIZE, AMB_TEXTURESIZE ); 
			m_ResizeTexture.GetTexture()->LockRect( 0, &PixelData, &rct, 0 ); 
			for( int y = 0; y < AMB_TEXTURESIZE; y++ )
			{
			for( int x = 0; x < AMB_TEXTURESIZE; x++ )
			{
			DWORD* color = (DWORD*)PixelData.pBits;
			color += x + y*PixelData.Pitch/4;
			DWORD dwtemp = *color & 0x00ffffff; 

			if( dwtemp == pAmbInfo->dwPointColor )
			{
			D3DXVECTOR3* pPos = new D3DXVECTOR3; 
			GetPosFromLockPos( pPos, x, y, pSecAmb->iSectorX, pSecAmb->iSectorY );
			pAmbInfo->AmbPointList.push_back( pPos ); 
			}
			}
			}
			m_ResizeTexture.GetTexture()->UnlockRect(0); 
			}*/

			//----------------
			// AMB()_SCHEDULE 
			//----------------
			sprintf( buf, "%s%d_%s", KEY_AMB, i+1, KEY_SCHEDULE ); 
			sprintf( strMsg, "%s, %s", strSectorAmb, buf );
			iSchedule = GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename ); 
			if( iSchedule == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );

			for( int j = 0; j < iSchedule; j++ )
			{
				//Create AmbiencePoint Schedule 
				AmbSchedule* pSchedule = new AmbSchedule;

				//-------------------------
				// AMB()_START_SCHEDULE() 
				//-------------------------
				sprintf( buf, "%s%d_%s_%s%d", KEY_AMB, i+1, KEY_START, KEY_SCHEDULE, j+1 );
				sprintf( strMsg, "%s, %s", strSectorAmb, buf );
				temp = GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename ); 
				if( temp == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );
				else pSchedule->fStartTime = (float)temp; 

				//-------------------------
				// AMB()_END_SCHEDULE()
				//-------------------------
				sprintf( buf, "%s%d_%s_%s%d", KEY_AMB, i+1, KEY_END, KEY_SCHEDULE, j+1 );
				sprintf( strMsg, "%s, %s", strSectorAmb, buf );
				temp = GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename ); 
				if( temp == -1 ) MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );
				else pSchedule->fEndTime = (float)temp; 

				//-------------------------
				// AMB()_SOUND()
				//-------------------------
				sprintf( buf, "%s%d_%s%d", KEY_AMB, i+1, KEY_SOUND, j+1 );
				sprintf( strMsg, "%s, %s", strSectorAmb, buf );
				GetPrivateProfileString( strSectorAmb, buf, "x", pSchedule->strFilename, 
					sizeof(pSchedule->strFilename), strFilename ); 
				if( !strcmp(pSchedule->strFilename, "x") ) 
					MessageBox( NULL, strMsg, "파싱실패:AMB스크립트", MB_OK );

				//-----------------------------
				// AMB()_SCHEDULE()_RANDOM()
				//-----------------------------
				char TempSndName[MAX_PATH]; 
				for( int rn = 1;;rn++ )
				{
					sprintf( buf, "AMB%d_SCHEDULE%d_RANDOM%d", i+1, j+1, rn ); 					
					GetPrivateProfileString( strSectorAmb, buf, "x", TempSndName, MAX_PATH, strFilename ); 
					if( !strcmp(TempSndName, "x") ) 
					{
						break; 
					}
					else 
					{
						char* pRandSndName; 
						pRandSndName = new char[strlen(TempSndName)+1]; 
						strcpy( pRandSndName, TempSndName ); 
						pSchedule->RandomSndList.push_back(pRandSndName); 
					}
				}
				
				//----------------------------------
				// AMB()_SCHEDULE()_RANDOM_CYCLE
				//----------------------------------
				sprintf( buf, "AMB%d_SCHEDULE%d_RANDOM_CYCLE", i+1, j+1 );
				sprintf( strMsg, "%s, %s", strSectorAmb, buf );
				pSchedule->iRandSndCycle = GetPrivateProfileInt( strSectorAmb, buf, -1, strFilename ); 

				//Push Schedule  
				pAmbInfo->ScheduleList.push_back( pSchedule ); 
			}

			//Push AmbienceInfo 
			pSecAmb->AmbInfoList.push_back( pAmbInfo ); 
		}

		//=====================
		// Add List 
		//=====================
		MakeColorTable( pSecAmb->ColorTable[0], pSecAmb->strTexFilename ); 
		m_ScriptDataList[Zone].push_back( pSecAmb ); 
	}
}



void CSectorAmbienceMap::DeleteScriptData()
{
	vector<SectorAmbience*>::iterator iter; 
	vector<AmbienceInfo*>::iterator iter2; 
	vector<AmbSchedule*>::iterator iter3; 
	vector<char*>::iterator iter4; 

	for( int Zone = 0; Zone < ZONE_NUM; Zone++ )
	{
		for( iter = m_ScriptDataList[Zone].begin(); 
			iter != m_ScriptDataList[Zone].end(); iter++ )
		{
			for( iter2 = (*iter)->AmbInfoList.begin(); 
				iter2 != (*iter)->AmbInfoList.end(); iter2++ )
			{
				for( iter3 = (*iter2)->ScheduleList.begin(); 
					iter3 != (*iter2)->ScheduleList.end(); iter3++ )
				{
					for( iter4 = (*iter3)->RandomSndList.begin(); 
						iter4 != (*iter3)->RandomSndList.end(); iter4++ )
					{
						delete [] (*iter4); 
					}
					delete (*iter3);
				}
				delete (*iter2);
			}
			delete (*iter);
		}
	}
}


void CSectorAmbienceMap::LoadAmbTexture( char* strFile )
{
	SAFE_RELEASE(m_pAmbTexture); 
	D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), strFile, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pAmbTexture ); 

		TESTSCREEN_VERTEX pVertex[4]; 
		pVertex[0].x = 0; 
		pVertex[0].y = 0; 
		pVertex[0].z = 0.0f; 
		pVertex[0].rhw = 1.0f; 
		pVertex[0].u = 0.0f; 
		pVertex[0].v = 0.0f; 
		pVertex[0].color = 0x55ffffff; 

		pVertex[1].x = AMB_TEXTURESIZE; 
		pVertex[1].y = 0; 
		pVertex[1].z = 0.0f; 
		pVertex[1].rhw = 1.0f; 
		pVertex[1].u = 1.0f; 
		pVertex[1].v = 0.0f; 
		pVertex[1].color = 0x55ffffff; 

		pVertex[2].x = 0; 
		pVertex[2].y = AMB_TEXTURESIZE; 
		pVertex[2].z = 0.0f; 
		pVertex[2].rhw = 1.0f; 
		pVertex[2].u = 0.0f; 
		pVertex[2].v = 1.0f; 
		pVertex[2].color = 0x55ffffff; 

		pVertex[3].x = AMB_TEXTURESIZE; 
		pVertex[3].y = AMB_TEXTURESIZE; 
		pVertex[3].z = 0.0f; 
		pVertex[3].rhw = 1.0f; 
		pVertex[3].u = 1.0f; 
		pVertex[3].v = 1.0f; 
		pVertex[3].color = 0x55ffffff; 
		
		LPDIRECT3DDEVICE8 pd3dDevice = BaseGraphicsLayer::GetDevice(); 
		pd3dDevice->SetTexture( 0, m_pAmbTexture ); 

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

		m_ResizeTexture.Begin( pd3dDevice ); 
		D3DVIEWPORT8 temp; 
		BaseGraphicsLayer::GetDevice()->GetViewport( &temp ); 
		temp.Height = AMB_TEXTURESIZE; 
		temp.Width = AMB_TEXTURESIZE; 
		BaseGraphicsLayer::GetDevice()->SetViewport( &temp ); 

		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffffff, 1.0f, 0);
		pd3dDevice->BeginScene();
		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(TESTSCREEN_VERTEX) ); 
		pd3dDevice->EndScene(); 
		m_ResizeTexture.End( pd3dDevice ); 

	//Set things back to normal
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE ); 

}


void CSectorAmbienceMap::GetPosFromLockPos( D3DXVECTOR3* outPos, int inPosX, int inPosY, int SectorX, int SectorY )
{
	float Magnification = (float)SECTORSIZE / (float)AMB_TEXTURESIZE; 
	
	outPos->x = (float)SECTORSIZE*SectorX + (float)inPosX * Magnification; 
	outPos->z = (float)SECTORSIZE*SectorY + (float)(AMB_TEXTURESIZE - inPosY) * Magnification; 

	//일단 임시값 설정 
	outPos->y = 0.0f; 
}


SectorAmbience* CSectorAmbienceMap::GetSectorAmbience( int SectorX, int SectorY )
{
	vector<SectorAmbience*>::iterator iter; 
	for( iter = m_SectorAmbList.begin(); 
		 iter != m_SectorAmbList.end(); iter++ )
	{
		if( (*iter)->iSectorX == SectorX && (*iter)->iSectorY == SectorY )
		{
			return (*iter); 
		}
	}

	return NULL; 
}

void CSectorAmbienceMap::SaveColorTable( DWORD* pSrce, char* strDestFile )
{
	DWORD ColorTable[AMB_TEXTURESIZE][AMB_TEXTURESIZE]; 

	memcpy( ColorTable[0], pSrce, sizeof(DWORD)*AMB_TEXTURESIZE*AMB_TEXTURESIZE ); 

	LPDIRECT3DSURFACE8 lpSrcSurface = NULL;
	LPDIRECT3DSURFACE8 lpDestSurface = NULL;
	LPDIRECT3DTEXTURE8 lpLockTexture = NULL; 

	D3DXCreateTextureFromFileEx(BaseGraphicsLayer::GetDevice(), "C:\\Test.bmp", 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &lpLockTexture ); 

	m_ResizeTexture.GetTexture()->GetSurfaceLevel(0,&lpSrcSurface);
	lpLockTexture->GetSurfaceLevel(0,&lpDestSurface);
	D3DXLoadSurfaceFromSurface( lpDestSurface,NULL,NULL,
									lpSrcSurface,NULL,NULL,
									D3DX_FILTER_NONE,0);
	
	RECT rct; 
	D3DLOCKED_RECT PixelData; 					
	SetRect( &rct, 0, 0, AMB_TEXTURESIZE, AMB_TEXTURESIZE ); 
	lpLockTexture->LockRect( 0, &PixelData, &rct, 0 ); 
		
	for( int y = 0; y < AMB_TEXTURESIZE; y++ )
	{
		for( int x = 0; x < AMB_TEXTURESIZE; x++ )
		{
			DWORD* color = (DWORD*)PixelData.pBits;
			color += x + y*PixelData.Pitch/4;
			*color = ColorTable[y][x];
		}
	}
	
	lpLockTexture->UnlockRect(0); 

	D3DXSaveTextureToFile(strDestFile, 
							D3DXIFF_BMP,
							lpLockTexture,
							NULL);

	SAFE_RELEASE(lpLockTexture); 	
}

void CSectorAmbienceMap::MakeColorTable( DWORD* pDest, char* strSrcFile )
{
	DWORD ColorTable[AMB_TEXTURESIZE][AMB_TEXTURESIZE]; 

	LoadAmbTexture( strSrcFile ); 

	LPDIRECT3DSURFACE8 lpSrcSurface = NULL;
	LPDIRECT3DSURFACE8 lpDestSurface = NULL;
	LPDIRECT3DTEXTURE8 lpLockTexture = NULL; 

	D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), strSrcFile, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &lpLockTexture ); 

	m_ResizeTexture.GetTexture()->GetSurfaceLevel(0,&lpSrcSurface);
	lpLockTexture->GetSurfaceLevel(0,&lpDestSurface);
	D3DXLoadSurfaceFromSurface( lpDestSurface,NULL,NULL,
									lpSrcSurface,NULL,NULL,
									D3DX_FILTER_NONE,0);
	
	RECT rct; 
	D3DLOCKED_RECT PixelData; 					
	SetRect( &rct, 0, 0, AMB_TEXTURESIZE, AMB_TEXTURESIZE ); 
	lpLockTexture->LockRect( 0, &PixelData, &rct, 0 ); 
		
	for( int y = 0; y < AMB_TEXTURESIZE; y++ )
	{
		for( int x = 0; x < AMB_TEXTURESIZE; x++ )
		{
			DWORD* color = (DWORD*)PixelData.pBits;
			color += x + y*PixelData.Pitch/4;
			ColorTable[y][x] = *color; 
		}
	}
	
	lpLockTexture->UnlockRect(0); 
	memcpy( pDest, ColorTable[0], sizeof(DWORD)*AMB_TEXTURESIZE*AMB_TEXTURESIZE ); 

	SAFE_RELEASE(lpLockTexture); 
}


void CSectorAmbienceMap::WriteBinData( const char* strFilePath )
{
	vector<SectorAmbience*>::iterator iter_SecAmb; 
	vector<AmbienceInfo*>::iterator iter_AmbInfo; 
	vector<AmbSchedule*>::iterator iter_AmbSchle; 
	vector<D3DXVECTOR3*>::iterator iter_Point; 

	FILE* fp; 
	char strFile[MAX_PATH];
	ZeroMemory( strFile, sizeof(strFile) ); 


	for( int Zone = 0; Zone < ZONE_NUM; Zone++ )
	{
		if( m_ScriptDataList[Zone].size() == 0 ) continue; 

		sprintf( strFile, "%s\\Zone%d.amb", strFilePath, Zone ); 

		fp = fopen( strFile, "wb" ); 

		if( fp == NULL )
		{
			MessageBox( NULL, "Ambience파일을 쓰는데 실패!!", "파일에러", MB_OK ); 
		}

		int SecAmbNum = m_ScriptDataList[Zone].size(); 
		fwrite( &SecAmbNum, sizeof(int), 1, fp );

		for( iter_SecAmb = m_ScriptDataList[Zone].begin(); 
			iter_SecAmb != m_ScriptDataList[Zone].end(); iter_SecAmb++ )
		{
			fwrite( &((*iter_SecAmb)->iSectorX), sizeof(int), 1, fp ); 
			fwrite( &((*iter_SecAmb)->iSectorY), sizeof(int), 1, fp ); 
			(*iter_SecAmb)->strTexFilename[MAX_PATH-1] = NULL; 
			fwrite( (*iter_SecAmb)->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			fwrite( (*iter_SecAmb)->ColorTable[0], sizeof(DWORD), AMB_TEXTURESIZE*AMB_TEXTURESIZE, fp ); 

			int AmbInfoNum = (*iter_SecAmb)->AmbInfoList.size();
			fwrite( &AmbInfoNum, sizeof(int), 1, fp ); 

			for( iter_AmbInfo = (*iter_SecAmb)->AmbInfoList.begin(); 
				iter_AmbInfo != (*iter_SecAmb)->AmbInfoList.end(); iter_AmbInfo++ )
			{
				fwrite( &((*iter_AmbInfo)->dwColor), sizeof(DWORD), 1, fp ); 
				fwrite( &((*iter_AmbInfo)->dwPointColor), sizeof(DWORD), 1, fp ); 

				int AmbPointNum = (*iter_AmbInfo)->AmbPointList.size(); 
				fwrite( &AmbPointNum, sizeof(int), 1, fp ); 

				for( iter_Point = (*iter_AmbInfo)->AmbPointList.begin(); 
					iter_Point != (*iter_AmbInfo)->AmbPointList.end(); iter_Point++ )
				{
					fwrite( *iter_Point, sizeof(D3DXVECTOR3), 1, fp );
				}

				int SchleNum = (*iter_AmbInfo)->ScheduleList.size(); 
				fwrite( &SchleNum, sizeof(int), 1, fp ); 

				for( iter_AmbSchle = (*iter_AmbInfo)->ScheduleList.begin(); 
					iter_AmbSchle != (*iter_AmbInfo)->ScheduleList.end(); iter_AmbSchle++ )
				{
					fwrite( &(*iter_AmbSchle)->fStartTime, sizeof(float), 1, fp ); 
					fwrite( &(*iter_AmbSchle)->fEndTime, sizeof(float), 1, fp ); 
					fwrite( (*iter_AmbSchle)->strFilename, sizeof(char), MAX_PATH, fp ); 

					//렌덤사운드기록추가 
					int nRandSndNum = (*iter_AmbSchle)->RandomSndList.size(); 
					fwrite( &nRandSndNum, sizeof(int), 1, fp ); 
					for( int rn = 0; rn < nRandSndNum; rn++ )
					{
						char* strRndSnd = (*iter_AmbSchle)->RandomSndList[rn]; 
						int nNameLen = strlen( strRndSnd ); 
						fwrite( &nNameLen, 1, sizeof(int), fp ); 
						fwrite( strRndSnd, 1, nNameLen, fp ); 
					}
					//렌덤사운드 주기추가
					fwrite( &(*iter_AmbSchle)->iRandSndCycle, sizeof(int), 1, fp ); 

				}
			}
		}

		fclose( fp ); 
	}
}

void CSectorAmbienceMap::LoadData( char* strFile )
{
//	GetScriptData("C:\\릴사운드스크립트\\Test.txt");
//	WriteBinData("C:\릴사운드스크립트");

	FILE* fp; 
	fp = fopen( strFile, "rb" ); 
	if( fp == NULL )
	{
		MessageBox( NULL, "Ambience파일을 여는데 실패!!", "파일에러", MB_OK ); 
		return; 
	}

	DeleteAmbData(); 

	int SecAmbNum;  
	fread( &SecAmbNum, sizeof(int), 1, fp );

	for( int j = 0; j < SecAmbNum; j++ )
	{
		SectorAmbience* pSecAmb = new SectorAmbience; 

		fread( &(pSecAmb->iSectorX), sizeof(int), 1, fp ); 
		fread( &(pSecAmb->iSectorY), sizeof(int), 1, fp ); 
		fread( pSecAmb->strTexFilename, sizeof(char), MAX_PATH, fp ); 
		fread( pSecAmb->ColorTable[0], sizeof(DWORD), AMB_TEXTURESIZE*AMB_TEXTURESIZE, fp ); 

		int AmbInfoNum; 
		fread( &AmbInfoNum, sizeof(int), 1, fp ); 

		for( int q = 0; q < AmbInfoNum; q++ ) 
		{
			AmbienceInfo* pAmbInfo = new AmbienceInfo; 
			fread( &(pAmbInfo->dwColor), sizeof(DWORD), 1, fp ); 
			fread( &(pAmbInfo->dwPointColor), sizeof(DWORD), 1, fp ); 

			int AmbPointNum;  
			fread( &AmbPointNum, sizeof(int), 1, fp ); 

			for( int w = 0; w < AmbPointNum; w++ ) 
			{
				D3DXVECTOR3* pPoint = new D3DXVECTOR3; 
				fread( pPoint, sizeof(D3DXVECTOR3), 1, fp );
				pAmbInfo->AmbPointList.push_back( pPoint ); 
			}

			int SchleNum;  
			fread( &SchleNum, sizeof(int), 1, fp ); 

			for( int p = 0; p < SchleNum; p++ ) 
			{
				AmbSchedule* pSchedule = new AmbSchedule; 
				fread( &pSchedule->fStartTime, sizeof(float), 1, fp ); 
				fread( &pSchedule->fEndTime, sizeof(float), 1, fp ); 
				char name[MAX_PATH]; 
				fread( name, sizeof(char), MAX_PATH, fp );
				sprintf( pSchedule->strFilename, "%s%s", AMBIENCEPATH, name );

				//렌덤사운드 로드추가 
				int nRandSndNum = 0;   
				fread( &nRandSndNum, sizeof(int), 1, fp ); 
				for( int rn = 0; rn < nRandSndNum; rn++ )
				{
					int nNameLen;  
					fread( &nNameLen, 1, sizeof(int), fp ); 
					char* strRndSnd = new char[nNameLen+1];
					memset( strRndSnd, 0, nNameLen+1 ); 
					fread( strRndSnd, 1, nNameLen, fp ); 
					pSchedule->RandomSndList.push_back( strRndSnd ); 
				}
				//렌덤사운드 주기 로드추가
				fread( &pSchedule->iRandSndCycle, sizeof(int), 1, fp ); 

				pAmbInfo->ScheduleList.push_back( pSchedule ); 
			}
			pSecAmb->AmbInfoList.push_back( pAmbInfo );
		}

		m_SectorAmbList.push_back( pSecAmb ); 
	}

	fclose( fp ); 	

//	WriteScriptData("C:\\Test.txt");
}


void CSectorAmbienceMap::DeleteAmbData()
{
	vector<SectorAmbience*>::iterator iter_sec; 
	vector<AmbienceInfo*>::iterator iter_amb; 
	vector<AmbSchedule*>::iterator iter_sche; 
	vector<D3DXVECTOR3*>::iterator iter_vec; 
	vector<char*>::iterator iter_str; 

	for( iter_sec = m_SectorAmbList.begin(); 
		iter_sec != m_SectorAmbList.end(); iter_sec++ )
	{
		for( iter_amb = (*iter_sec)->AmbInfoList.begin(); 
			iter_amb != (*iter_sec)->AmbInfoList.end(); iter_amb++ )
		{
			for( iter_sche = (*iter_amb)->ScheduleList.begin(); 
				iter_sche != (*iter_amb)->ScheduleList.end(); iter_sche++ )
			{
				for( iter_str = (*iter_sche)->RandomSndList.begin(); 
					iter_str != (*iter_sche)->RandomSndList.end(); iter_str++ )
				{
					delete [] (*iter_str); 
				}
				delete (*iter_sche);
			}
			for( iter_vec = (*iter_amb)->AmbPointList.begin(); 
				iter_vec != (*iter_amb)->AmbPointList.end(); iter_vec++ )
			{
				delete (*iter_vec); 
			}
			delete (*iter_amb);
		}
		delete (*iter_sec);
	}

	m_SectorAmbList.clear(); 
}


