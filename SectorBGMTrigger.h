// SectorBGMTrigger.h: interface for the CSectorBGMTrigger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORBGMTRIGGER_H__2A07FEDB_6BAC_4C12_A532_4D50FA18E18E__INCLUDED_)
#define AFX_SECTORBGMTRIGGER_H__2A07FEDB_6BAC_4C12_A532_4D50FA18E18E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderTexture.h"
#include <vector>
using namespace std; 

#define ZONE_NUM 15
#define BGM_TEXTURESIZE 128

//==========================
// Define Script Key 
//==========================
#define APP_SET "SET"
#define APP_BGM_TRIGGER "BGM_TRIGGER"
#define KEY_TRIGGER "TRIGGER"
#define KEY_ZONE "ZONE"
#define KEY_SECTORX "SECTORX"
#define KEY_SECTORY "SECTORY"
#define KEY_BGM "BGM"
#define KEY_ESF "ESF"
#define KEY_COLOR "COLOR"
#define KEY_TEXTURE "TEXTURE"
#define KEY_EVENTNUM "EVENTNUM"
#define KEY_EVENTKEY "EVENTKEY"

//===========================
// Define Event Key 
//===========================
//#define EK_BGM_ONCE 0
//#define EK_BGM_LOOP 1
#define EK_BGM_ONCE_SAFE 0 
#define EK_BGM_LOOP_SAFE 1
#define EK_BGM_LOOP_UNSAFE 2
#define EK_BGM_ONCE_UNSAFE 3
#define EK_NOTBGM_SAFE 4
#define EK_BGM_TURN_AMB_SAFE 5
#define EK_BGM_TURN_AMB_UNSAFE 6
#define EK_CAMP_UNCREATE	9
#define EK_ESF_SAFE 10
#define EK_ESF_UNSAFE 11

// 0 한번 플레이 , 세이프티 존 o
// 1 반복 플레이 , 세이프티 존 o
// 2 반복 플레이 , 세이프티 존 x
// 3 한번 플레이 , 세이프티 존 x
// 4 음악 플레이x, 세이프티 존 o
// 5 BGM 1번 플레이, 엠비언스로 패스, 세이프티 존 o
// 6 BGM 1번 플레이, 엠비언스로 패스, 세이프티 존 x
// 9 캠프 안지어짐
// 10 지역이름, 세이프티 존 o
// 11 지역이름, 세이프티 존 x

struct PerSectorTriggerInfo
{
	int iSectorX; 
	int iSectorY;
	char strTexFilename[MAX_PATH];
	int iEventNum; 
	vector<int> EventKeyList; 
	vector<DWORD> BGMColorKeyList; 
	vector<char*> BGMFileList; 
	vector<char*> ESFFileList; 
	DWORD ColorTable[BGM_TEXTURESIZE][BGM_TEXTURESIZE]; 

	void Destroy()
	{
		vector<char*>::iterator iter; 
		for( iter = BGMFileList.begin(); iter != BGMFileList.end(); iter++ )
		{
			delete (*iter); 
		}

		for( iter = ESFFileList.begin(); iter != ESFFileList.end(); iter++ )
		{
			delete (*iter); 
		}

		BGMFileList.clear(); 
		ESFFileList.clear();
		BGMColorKeyList.clear(); 
		EventKeyList.clear(); 
	}; 
};


class CSectorBGMTrigger  
{
public:
	vector<PerSectorTriggerInfo*>	m_ScriptDataList[ZONE_NUM]; 
	vector<PerSectorTriggerInfo*>	m_SectorBgmList; 
	LPDIRECT3DTEXTURE8				m_pBgmTexture; 
	CRenderTexture m_ResizeTexture; 

protected:
	void LoadBgmTexture( char* strFile ); 
	void DeleteScriptData();
	void MakeColorTable( DWORD* pDest, char* strSrcFile ); 

public:
	CSectorBGMTrigger();
	virtual ~CSectorBGMTrigger();

	void DeleteBgmData(); 

	void Create(); 
	void Destroy(); 
	void GetScriptData( const char* strFilename ); 
	void WriteBinData(); 
	void LoadZoneData( int Zone ); 
	void LoadData( char* strFile ); 
	PerSectorTriggerInfo* GetTrigger( int SectorX, int SectorY ); 
};

#endif // !defined(AFX_SECTORBGMTRIGGER_H__2A07FEDB_6BAC_4C12_A532_4D50FA18E18E__INCLUDED_)
