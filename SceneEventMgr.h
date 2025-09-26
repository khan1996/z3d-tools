#pragma once

#include "d3dx8.h"

class CSceneEventMgr
{
	enum EVENT_BITSET1 //BGM ���̺��̿�
	{
		EB1_DISABLE_CAMP = 1<<7,	 // ������ ������ ���� ���� 
		EB1_UNKNOWN2 = 1<<6,		    
		EB1_UNKNOWN3 = 1<<5,		 
		EB1_UNKNOWN4 = 1<<4,		
		EB1_UNKNOWN5 = 1<<3,		
		EB1_UNKNOWN6 = 1<<2,		
		EB1_UNKNOWN7 = 1<<1,		
		EB1_UNKNOWN8 = 1<<0			
	};

	enum EVENT_BITSET2 //AMB ���̺��̿�
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

	//���� ���� ��ġ�� ������ ������ �ִ°�?
	bool IsEnableStructCamp(); 
	
	//���� �̰��� ������Ƽ���ΰ�?
	bool IsSafetyZone(); 

	//���� �̺�Ʈ ������ �帣�� �ֳ�?
	bool IsPlayingEventMusic(void) { return m_bEventMusic; }

	//�̰��� �����ϸ� bgm�� FadeOut�ϸ� �̺�Ʈ���� �߻�.. ������������ Ogg���Ͽ� 1ä���̾�߸��Ѵ�
	//void PlayEvent3DMusic( char* strFileName, bool bLoop, D3DXVECTOR3& vPos, float fMinDist, float fMaxDist ); 
	//void StopEvent3DMusic( char* strFileName ); 
	void PlayEventMusic( char* strFileName, bool bLoop );
	void StopEventMusic( char* strFileName ); 

	//���� �̰��� �����ΰ�?
	bool IsVillage(); 

	////////////////////////////////////////////////////////////////////////////////
};
