#pragma once

#include "d3dx8.h"

class CSceneEventMgr
{
	enum EVENT_BITSET1 //BGM 테이블이용
	{
		EB1_DISABLE_CAMP = 1<<7,	 // 진지를 지을수 없는 지역 
		EB1_UNKNOWN2 = 1<<6,		    
		EB1_UNKNOWN3 = 1<<5,		 
		EB1_UNKNOWN4 = 1<<4,		
		EB1_UNKNOWN5 = 1<<3,		
		EB1_UNKNOWN6 = 1<<2,		
		EB1_UNKNOWN7 = 1<<1,		
		EB1_UNKNOWN8 = 1<<0			
	};

	enum EVENT_BITSET2 //AMB 테이블이용
	{
		EB2_UNKNOWN1 = 1<<7,				
		EB2_UNKNOWN2 = 1<<6,		
		EB2_UNKNOWN3 = 1<<5,		
		EB2_UNKNOWN4 = 1<<4,	
		EB2_UNKNOWN5 = 1<<3,			
		EB2_UNKNOWN6 = 1<<2,			
		EB2_UNKNOWN7 = 1<<1,			
		EB2_UNKNOWN8 = 1<<0		
	};

public:
	bool m_bEventMusic;  
	char m_strEventMusicName[MAX_PATH]; 

public:
	CSceneEventMgr(void);
	virtual ~CSceneEventMgr(void);

	void Update(); 

	//현재 나의 위치에 진지를 지을수 있는가?
	bool IsEnableStructCamp(); 
	
	//현재 이곳은 세이프티존인가?
	bool IsSafetyZone(); 

	//현재 이벤트 음악이 흐르고 있나?
	bool IsPlayingEventMusic(void) { return m_bEventMusic; }

	//이것을 실행하면 bgm이 FadeOut하며 이벤트음이 발생.. 파일의형식은 Ogg파일에 1채널이어야만한다
	//void PlayEvent3DMusic( char* strFileName, bool bLoop, D3DXVECTOR3& vPos, float fMinDist, float fMaxDist ); 
	//void StopEvent3DMusic( char* strFileName ); 
	void PlayEventMusic( char* strFileName, bool bLoop );
	void StopEventMusic( char* strFileName ); 

	//현재 이곳은 마을인가?
	bool IsVillage(); 

	////////////////////////////////////////////////////////////////////////////////
};
