// SectorAmbienceMap.h: interface for the CSectorAmbienceMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORAMBIENCEMAP_H__C1C0A8F8_771B_4D64_814E_0B7F8285632C__INCLUDED_)
#define AFX_SECTORAMBIENCEMAP_H__C1C0A8F8_771B_4D64_814E_0B7F8285632C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderTexture.h"
#include <vector>
#include <string>
using namespace std;
 
#define ZONE_NUM 15
#define AMB_TEXTURESIZE 128
#define AMBDIRECTORY_PATH "c:\\MP-Project\\SOUND\\Amb\\"

//==========================
// Define Script Key 
//==========================
#define APP_SET "SET"
#define APP_SECTOR_AMBIENCE "SECTOR_AMBIENCE"
#define KEY_AMBIENCE "AMBIENCE" 
#define KEY_ZONE "ZONE"
#define KEY_SECTORX "SECTORX"
#define KEY_SECTORY "SECTORY"
#define KEY_TEXTURE "TEXTURE"
#define KEY_NUM_AMBIENCE "NUM_AMBIENCE"
#define KEY_AMB "AMB"
#define KEY_COLOR "COLOR"
#define KEY_SCHEDULE "SCHEDULE"
#define KEY_START "START"
#define KEY_END "END"
#define KEY_SOUND "SOUND"
#define KEY_POINTCOLOR "POINTCOLOR"
#define KEY_RANDOM "RANDOM"

struct AmbSchedule
{
	float fStartTime; 
	float fEndTime; 
	char strFilename[MAX_PATH]; 
	vector<char*> RandomSndList; 
	int iRandSndCycle; //렌덤사운드 시간간격
};

struct AmbienceInfo
{
	DWORD dwColor;
	DWORD dwPointColor; 
	vector<D3DXVECTOR3*> AmbPointList; 
	vector<AmbSchedule*> ScheduleList;  	 
};

struct SectorAmbience
{
	int iSectorX; 
	int iSectorY; 
	vector<AmbienceInfo*> AmbInfoList; 
	char strTexFilename[MAX_PATH]; 
	DWORD ColorTable[AMB_TEXTURESIZE][AMB_TEXTURESIZE]; 
};

class CSectorAmbienceMap  
{
protected:
	vector<SectorAmbience*> m_ScriptDataList[ZONE_NUM]; 
	vector<SectorAmbience*> m_SectorAmbList; 

	LPDIRECT3DTEXTURE8 m_pAmbTexture; 
	CRenderTexture m_ResizeTexture; 

protected:
	void DeleteScriptData(); 
	void LoadAmbTexture( char* strFile ); 
	void GetPosFromLockPos( D3DXVECTOR3* outPos, int inPosX, int inPosY, int SectorX, int SectorY ); 
	void MakeColorTable( DWORD* pDest, char* strSrcFile ); 
	void SaveColorTable( DWORD* pSrce, char* strDestFile ); 

public:
	CSectorAmbienceMap();
	virtual ~CSectorAmbienceMap();

	void DeleteAmbData(); 

	void WriteBinData( const char* strFilePath ); 
	void WriteScriptData( const char* strFilename );	// 한개의 바이너리로된 AMB정보를 Script로 저장한다.
	void LoadData( char* strFile ); 
	void GetScriptData( const char* strFilename );
	SectorAmbience* GetSectorAmbience( int SectorX, int SectorY ); 
};

#endif // !defined(AFX_SECTORAMBIENCEMAP_H__C1C0A8F8_771B_4D64_814E_0B7F8285632C__INCLUDED_)
