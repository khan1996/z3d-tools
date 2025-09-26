// SectorBGMTrigger.cpp: implementation of the CSectorBGMTrigger class.
//
//////////////////////////////////////////////////////////////////////

#include "SoundMgr.h"
#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "SectorBGMTrigger.h"
#include "GMMemory.h"

struct TESTSCREEN_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
	float u, v;
};

CSectorBGMTrigger::CSectorBGMTrigger()
{
}


CSectorBGMTrigger::~CSectorBGMTrigger()
{

}


void CSectorBGMTrigger::Create()
{
	m_pBgmTexture = NULL; 
	m_ResizeTexture.Create( BGM_TEXTURESIZE, BGM_TEXTURESIZE );
}


void CSectorBGMTrigger::Destroy()
{
	SAFE_RELEASE( m_pBgmTexture ); 
	DeleteScriptData(); 
	DeleteBgmData(); 
}


PerSectorTriggerInfo* CSectorBGMTrigger::GetTrigger( int SectorX, int SectorY )
{
	vector<PerSectorTriggerInfo*>::iterator iter; 

	for( iter = m_SectorBgmList.begin(); 
		 iter != m_SectorBgmList.end(); iter++ )
		 {
			if( (*iter)->iSectorX == SectorX && (*iter)->iSectorY == SectorY )
				return (*iter); 
		 }

	return NULL; 
}


void CSectorBGMTrigger::DeleteScriptData()
{
	vector<PerSectorTriggerInfo*>::iterator iter; 

	for( int Zone = 0; Zone < ZONE_NUM; Zone++ )
	{
		for( iter = m_ScriptDataList[Zone].begin(); 
			 iter != m_ScriptDataList[Zone].end(); iter++ )
		{		
			(*iter)->Destroy(); 	 
			delete (*iter); 
			*iter = NULL; 
		}	
		m_ScriptDataList[Zone].clear(); 
	}
}


void CSectorBGMTrigger::GetScriptData( const char* strFilename )
{
	int TriggerNum, Zone;
	char strTrigger[MAX_PATH]; 
	char strMsg[MAX_PATH]; 
	ZeroMemory( strMsg, sizeof(strMsg) ); 
	ZeroMemory( strTrigger, sizeof(strTrigger) ); 

	strcpy( strTrigger, APP_BGM_TRIGGER ); 
	
	DeleteScriptData(); 

	TriggerNum = GetPrivateProfileInt( APP_SET, KEY_TRIGGER, 0, strFilename );
	
	for( int count = 1; count < TriggerNum+1; count++ )
	{
		/*** Script Interface (ex) ***
		
		[SET]
		TRIGGER = 2

		[BGM_TRIGGER1]
		ZONE = 1 
		SECTORX = 8
		SECTORY = 1
		EVENTNUM = 2
		TEXTURE = "C:\\WorkTest\\WorldCreator\\8_1.bmp"

		COLOR1 = 0x0000ff; 
		BGM1 = "C:\\WorkTest\\WorldCreator\\music.ogg"
		EVENTKEY1 = 0

		COLOR2 = 0xffffff; 
		BGM2 = "C:\\WorkTest\\WorldCreator\\Believe.ogg"
		EVENTKEY2 = 1


		[BGM_TRIGGER2]
		.........
		
		*/
		
		PerSectorTriggerInfo* pInfo = new PerSectorTriggerInfo; 
		ZeroMemory( pInfo, sizeof(pInfo) ); 
		sprintf( strTrigger, "%s%d", APP_BGM_TRIGGER, count ); 

		//-----------
		// ZONE 
		//-----------
		sprintf( strMsg, "%s, %s", strTrigger, KEY_ZONE ); 
		Zone = GetPrivateProfileInt( strTrigger, KEY_ZONE, -1, strFilename );
		if( Zone == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK ); 
	
		//-----------
		// SECTORX  
		//-----------
		sprintf( strMsg, "%s, %s", strTrigger, KEY_SECTORX ); 
		pInfo->iSectorX = GetPrivateProfileInt( strTrigger, KEY_SECTORX, -1, strFilename ); 
		if( pInfo->iSectorX == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK );
		
		//-----------
		// SECTORY
		//-----------
		sprintf( strMsg, "%s, %s", strTrigger, KEY_SECTORY ); 
		pInfo->iSectorY = GetPrivateProfileInt( strTrigger, KEY_SECTORY, -1, strFilename ); 
		if( pInfo->iSectorY == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK );
		
		//-----------
		// EVENTNUM
		//-----------
		sprintf( strMsg, "%s, %s", strTrigger, KEY_EVENTNUM ); 
		pInfo->iEventNum = GetPrivateProfileInt( strTrigger, KEY_EVENTNUM, -1, strFilename ); 
		if( pInfo->iEventNum == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK );
		
		//-----------
		// TEXTURE
		//-----------
		sprintf( strMsg, "%s, %s", strTrigger, KEY_TEXTURE ); 
		GetPrivateProfileString( strTrigger, KEY_TEXTURE, "x", pInfo->strTexFilename, 
			sizeof(pInfo->strTexFilename), strFilename );
		if( !strcmp(pInfo->strTexFilename, "x") ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK );
		
		for( int i = 1; i < pInfo->iEventNum+1; i++ )
		{
			char buf[MAX_PATH]; 
			ZeroMemory( buf, sizeof(buf) ); 
			sprintf( buf, "%s%d", KEY_COLOR, i ); 
			
			//-------------
			// Color 
			//-------------
			DWORD dwColor; 
			int temp; 
			sprintf( strMsg, "%s, %s", strTrigger, buf ); 
			temp = GetPrivateProfileInt( strTrigger, buf, -1, strFilename ); 
			if( temp == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK );  
			else dwColor = (DWORD)temp; 
			pInfo->BGMColorKeyList.push_back( dwColor ); 

			//-------------
			// BGM
			//-------------
			sprintf( buf, "%s%d", KEY_BGM, i ); 
			char* strBgm;
			strBgm = new char[MAX_PATH]; 
			sprintf( strMsg, "%s, %s", strTrigger, buf ); 
			GetPrivateProfileString( strTrigger, buf, "x", strBgm, MAX_PATH, strFilename ); 
			if( !strcmp(strBgm, "x") ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK ); 
			pInfo->BGMFileList.push_back( strBgm ); 

			char* strEsf;
			strEsf = new char[MAX_PATH]; 
			strncpy(strEsf, strBgm, strlen(strBgm)-3);
			strcpy(strEsf, "esf");
			pInfo->ESFFileList.push_back( strEsf ); 

			//-------------
			// EVENTKEY
			//-------------
			sprintf( buf, "%s%d", KEY_EVENTKEY, i ); 
			int EventKey; 
			sprintf( strMsg, "%s, %s", strTrigger, buf ); 
			EventKey = GetPrivateProfileInt( strTrigger, buf, -1, strFilename ); 
			if( EventKey == -1 ) MessageBox( NULL, strMsg, "파싱실패:BGM스크립트", MB_OK ); 
			pInfo->EventKeyList.push_back( EventKey );  
		}
		
		//=====================
		// Add Trigger 
		//=====================
		MakeColorTable( pInfo->ColorTable[0], pInfo->strTexFilename );
		m_ScriptDataList[Zone].push_back( pInfo ); 
	}
}


void CSectorBGMTrigger::WriteBinData()
{
	FILE* fp; 
	char strFile[MAX_PATH]; 
	ZeroMemory( strFile, sizeof(strFile) ); 
	vector<PerSectorTriggerInfo*>::iterator iter; 

	for( int Zone = 0; Zone < ZONE_NUM; Zone++ )
	{
		if( m_ScriptDataList[Zone].size() == 0 ) continue; 

		sprintf( strFile, "%sZone%d.bgm", BGMPATH, Zone ); 

		fp = fopen( strFile, "wb" ); 
		
		if( fp == NULL )
		{
			MessageBox( NULL, "BGMTrigger파일을 쓰는데 실패!!", "파일에러", MB_OK ); 
		}
	
		int TriggerNum = m_ScriptDataList[Zone].size();   
		fwrite( &TriggerNum, sizeof(int), 1, fp ); 
		
		for( iter = m_ScriptDataList[Zone].begin(); iter != m_ScriptDataList[Zone].end(); iter++ )
		{
			fwrite( &((*iter)->iSectorX), sizeof(int), 1, fp ); 
			fwrite( &((*iter)->iSectorY), sizeof(int), 1, fp ); 
			(*iter)->strTexFilename[MAX_PATH-1] = NULL; 
			fwrite( (*iter)->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			fwrite( &((*iter)->iEventNum), sizeof(int), 1, fp ); 
			fwrite( (*iter)->ColorTable[0], sizeof(DWORD), BGM_TEXTURESIZE*BGM_TEXTURESIZE, fp ); 

			for( int n = 0; n < (int)((*iter)->EventKeyList.size()); n++ )
			{
				fwrite( &((*iter)->EventKeyList[n]), sizeof(int), 1, fp ); 
			}
			for( int n = 0; n < (int)((*iter)->BGMColorKeyList.size()); n++ )
			{
				fwrite( &((*iter)->BGMColorKeyList[n]), sizeof(DWORD), 1, fp ); 
			}
			for( int n = 0; n < (int)((*iter)->BGMFileList.size()); n++ )
			{
				(*iter)->BGMFileList[n][MAX_PATH-1] = NULL; 
				fwrite( (*iter)->BGMFileList[n], sizeof(char), MAX_PATH, fp ); 
			}
		}

		fclose( fp );
	}
}


void CSectorBGMTrigger::DeleteBgmData()
{
	vector<PerSectorTriggerInfo*>::iterator iter; 

	for( iter = m_SectorBgmList.begin(); 
		 iter != m_SectorBgmList.end(); iter++ )
	{		
		(*iter)->Destroy(); 	 
		delete (*iter); 
		*iter = NULL; 
	}	
	
	m_SectorBgmList.clear(); 
}


void CSectorBGMTrigger::LoadData( char* strFile )
{
	FILE* fp; 
	vector<PerSectorTriggerInfo*>::iterator iter; 
	fp = fopen( strFile, "rb" );	
	if( fp == NULL )
	{
		MessageBox( NULL, "BGM파일을 여는데 실패!!", "파일에러", MB_OK ); 
		return; 
	}

	DeleteBgmData(); 

	int TriggerNum;  
	fread( &TriggerNum, sizeof(int), 1, fp ); 

	int iSectorX, iSectorY, iEventNum;
		
	for( int j = 0; j < TriggerNum; j++ )
	{
		fread( &(iSectorX), sizeof(int), 1, fp ); 
		fread( &(iSectorY), sizeof(int), 1, fp ); 

		PerSectorTriggerInfo* pTrigger = GetTrigger( iSectorX, iSectorY );
		if(pTrigger) // 이미 있는 트리거다
		{
			// 다시로드. 이미 했지만.. 결국 똑같은 데이타다.
			fread( pTrigger->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			
			// 이벤트 넘버는 나중에 추가된것이기 때문에 +로 증가시켜준다.
			fread( &(iEventNum), sizeof(int), 1, fp ); 
			pTrigger->iEventNum += iEventNum;

			fread( pTrigger->ColorTable[0], sizeof(DWORD), BGM_TEXTURESIZE*BGM_TEXTURESIZE, fp ); 

			// 추가한다.
			int temp; 
			DWORD temp2; 
			for( int n = 0; n < iEventNum; n++ )
			{ 
				fread( &temp, sizeof(int), 1, fp ); 
				pTrigger->EventKeyList.push_back( temp ); 
			}
			for( int n = 0; n < iEventNum; n++ )
			{
				fread( &temp2, sizeof(DWORD), 1, fp ); 
				pTrigger->BGMColorKeyList.push_back( temp2 ); 
			}
			for( int n = 0; n < iEventNum; n++ )
			{
				char* path = new char[MAX_PATH];  
				char file[MAX_PATH]; 
				fread( file, sizeof(char), MAX_PATH, fp );
				sprintf( path, "%s%s", BGMPATH, file ); 
				pTrigger->BGMFileList.push_back( path ); 	

				path = new char[MAX_PATH];  
				memset(path, 0, MAX_PATH);
				strncpy(path, file, strlen(file)-3 ); 
				strcat(path, "esf");
				pTrigger->ESFFileList.push_back( path ); 
			}

		}
		else
		{
			//Create Trigger 
			PerSectorTriggerInfo* pTrigger = new PerSectorTriggerInfo; 

			pTrigger->iSectorX = iSectorX;
			pTrigger->iSectorY = iSectorY;
//			fread( &(pTrigger->iSectorX), sizeof(int), 1, fp ); 
//			fread( &(pTrigger->iSectorY), sizeof(int), 1, fp ); 

			fread( pTrigger->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			fread( &(pTrigger->iEventNum), sizeof(int), 1, fp ); 
			fread( pTrigger->ColorTable[0], sizeof(DWORD), BGM_TEXTURESIZE*BGM_TEXTURESIZE, fp ); 
				
			int temp; 
			DWORD temp2; 
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{ 
				fread( &temp, sizeof(int), 1, fp ); 
				pTrigger->EventKeyList.push_back( temp ); 
			}
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{
				fread( &temp2, sizeof(DWORD), 1, fp ); 
				pTrigger->BGMColorKeyList.push_back( temp2 ); 
			}
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{
				char* path = new char[MAX_PATH];  
				char file[MAX_PATH]; 
				fread( file, sizeof(char), MAX_PATH, fp );
				sprintf( path, "%s%s", BGMPATH, file ); 
				pTrigger->BGMFileList.push_back( path ); 	

				path = new char[MAX_PATH];  
				memset(path, 0, MAX_PATH);
				if(strlen(file) > 3)
				{
					strncpy(path, file, strlen(file)-3 ); 
					strcat(path, "esf");
				}
				pTrigger->ESFFileList.push_back( path ); 
			}
			
			m_SectorBgmList.push_back( pTrigger ); 
		}
	}

	fclose( fp );
}


void CSectorBGMTrigger::LoadZoneData( int Zone )
{
	FILE* fp; 
	char strFile[MAX_PATH]; 
	vector<PerSectorTriggerInfo*>::iterator iter; 

	sprintf( strFile, "c:\\MP-Project\\SOUND\\Bgm\\Bgm_Zone%d", Zone ); 
	
	fp = fopen( strFile, "rb" );	

	if( fp == NULL )
	{
		MessageBox( NULL, "BGMTrigger파일을 읽는데 실패!!", "파일에러", MB_OK ); 
	}

	DeleteBgmData(); 

	int TriggerNum;  
	fread( &TriggerNum, sizeof(int), 1, fp ); 
		
	int iSectorX, iSectorY, iEventNum;

	for( int j = 0; j < TriggerNum; j++ )
	{
		fread( &(iSectorX), sizeof(int), 1, fp ); 
		fread( &(iSectorY), sizeof(int), 1, fp ); 

		PerSectorTriggerInfo* pTrigger = GetTrigger( iSectorX, iSectorY );
		if(pTrigger) // 이미 있는 트리거다
		{
			// 다시로드. 이미 했지만.. 결국 똑같은 데이타다.
			fread( pTrigger->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			
			// 이벤트 넘버는 나중에 추가된것이기 때문에 +로 증가시켜준다.
			fread( &(iEventNum), sizeof(int), 1, fp ); 
			pTrigger->iEventNum += iEventNum;

			fread( pTrigger->ColorTable[0], sizeof(DWORD), BGM_TEXTURESIZE*BGM_TEXTURESIZE, fp ); 

			// 추가한다.
			int temp; 
			DWORD temp2; 
			for( int n = 0; n < iEventNum; n++ )
			{ 
				fread( &temp, sizeof(int), 1, fp ); 
				pTrigger->EventKeyList.push_back( temp ); 
			}
			for( int n = 0; n < iEventNum; n++ )
			{
				fread( &temp2, sizeof(DWORD), 1, fp ); 
				pTrigger->BGMColorKeyList.push_back( temp2 ); 
			}
			for( int n = 0; n < iEventNum; n++ )
			{
				char* path = new char[MAX_PATH];  
				char file[MAX_PATH]; 
				fread( file, sizeof(char), MAX_PATH, fp );
				sprintf( path, "%s%s", BGMPATH, file ); 
				pTrigger->BGMFileList.push_back( path ); 	

				path = new char[MAX_PATH];  
				memset(path, 0, MAX_PATH);
				strncpy(path, file, strlen(file)-3 ); 
				strcat(path, "esf");
				pTrigger->ESFFileList.push_back( path ); 
			}
		}
		else
		{
			//Create Trigger 
			PerSectorTriggerInfo* pTrigger = new PerSectorTriggerInfo; 
				
			pTrigger->iSectorX = iSectorX;
			pTrigger->iSectorY = iSectorY;
//			fread( &(pTrigger->iSectorX), sizeof(int), 1, fp ); 
//			fread( &(pTrigger->iSectorY), sizeof(int), 1, fp ); 

			fread( pTrigger->strTexFilename, sizeof(char), MAX_PATH, fp ); 
			fread( &(pTrigger->iEventNum), sizeof(int), 1, fp ); 
			fread( pTrigger->ColorTable[0], sizeof(DWORD), BGM_TEXTURESIZE*BGM_TEXTURESIZE, fp ); 
				
			int temp; 
			DWORD temp2; 
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{ 
				fread( &temp, sizeof(int), 1, fp ); 
				pTrigger->EventKeyList.push_back( temp ); 
			}
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{
				fread( &temp2, sizeof(DWORD), 1, fp ); 
				pTrigger->BGMColorKeyList.push_back( temp2 ); 
			}
			for( int n = 0; n < pTrigger->iEventNum; n++ )
			{
				char* buf = new char[MAX_PATH];  
				fread( buf, sizeof(char), MAX_PATH, fp ); 
				pTrigger->BGMFileList.push_back( buf ); 	

				char* path = new char[MAX_PATH];  
				memset(path, 0, MAX_PATH);
				strncpy(path, buf, strlen(buf)-3 ); 
				strcat(path, "esf");
				pTrigger->ESFFileList.push_back( path ); 
			}
			
			m_SectorBgmList.push_back( pTrigger ); 
		}
	}

	fclose( fp );
}


void CSectorBGMTrigger::LoadBgmTexture( char* strFile )
{
	SAFE_RELEASE( m_pBgmTexture ); 

	D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), strFile, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pBgmTexture ); 

		TESTSCREEN_VERTEX pVertex[4]; 
		pVertex[0].x = 0; 
		pVertex[0].y = 0; 
		pVertex[0].z = 0.0f; 
		pVertex[0].rhw = 1.0f; 
		pVertex[0].u = 0.0f; 
		pVertex[0].v = 0.0f; 
		pVertex[0].color = 0x55ffffff; 

		pVertex[1].x = 128.0f; 
		pVertex[1].y = 0; 
		pVertex[1].z = 0.0f; 
		pVertex[1].rhw = 1.0f; 
		pVertex[1].u = 1.0f; 
		pVertex[1].v = 0.0f; 
		pVertex[1].color = 0x55ffffff; 

		pVertex[2].x = 0; 
		pVertex[2].y = 128.0f; 
		pVertex[2].z = 0.0f; 
		pVertex[2].rhw = 1.0f; 
		pVertex[2].u = 0.0f; 
		pVertex[2].v = 1.0f; 
		pVertex[2].color = 0x55ffffff; 

		pVertex[3].x = 128.0f; 
		pVertex[3].y = 128.0f; 
		pVertex[3].z = 0.0f; 
		pVertex[3].rhw = 1.0f; 
		pVertex[3].u = 1.0f; 
		pVertex[3].v = 1.0f; 
		pVertex[3].color = 0x55ffffff; 
		
		LPDIRECT3DDEVICE8 pd3dDevice = BaseGraphicsLayer::GetDevice(); 
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
		pd3dDevice->SetTexture( 0, m_pBgmTexture ); 



		m_ResizeTexture.Begin( pd3dDevice ); 
		D3DVIEWPORT8 temp; 
		BaseGraphicsLayer::GetDevice()->GetViewport( &temp ); 
		temp.Height = 128; 
		temp.Width = 128; 
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


void CSectorBGMTrigger::MakeColorTable( DWORD* pDest, char* strSrcFile )
{
	DWORD ColorTable[BGM_TEXTURESIZE][BGM_TEXTURESIZE]; 

	LoadBgmTexture( strSrcFile ); 
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
	SetRect( &rct, 0, 0, BGM_TEXTURESIZE, BGM_TEXTURESIZE ); 
	lpLockTexture->LockRect( 0, &PixelData, &rct, 0 ); 

	for( int y = 0; y < BGM_TEXTURESIZE; y++ )
	{
		for( int x = 0; x < BGM_TEXTURESIZE; x++ )
		{
			DWORD* color = (DWORD*)PixelData.pBits;
			color += x + y*PixelData.Pitch/4;
			ColorTable[y][x] = *color; 
		}
	}
	
	lpLockTexture->UnlockRect(0); 
	memcpy( pDest, ColorTable[0], sizeof(DWORD)*BGM_TEXTURESIZE*BGM_TEXTURESIZE ); 
	SAFE_RELEASE(lpLockTexture); 
}