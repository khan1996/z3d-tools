
#ifndef _AmbienceStruct_H_
#define _AmbienceStruct_H_

#pragma once

#include "STL.h"

/////////////////////////////////////////////////////////////////////////////
//	STL�� string Ŭ������ ��ũ ������ �����Ѽ� ������� ���� ���� ���� ����.


class tString
{
	char *	m_pString;
public:
	tString();
	tString( const char * );
	tString( const tString & );
	~tString();

	tString & operator=( const tString & );

	unsigned int	size();
	const char *	c_str()	{ return m_pString; }
};

//////////////////////////////////////////////////////////////////////////////
//

typedef std::pair<float, tString>		WAVEFILE;
typedef std::list< WAVEFILE >			WAVEFILELIST;			//pair<float, string> => �ð�, ȭ���̸�

struct SSoundAsset
{
	enum eTrigger {
		T_IN = 0,
		T_OUT = 1,
		T_IN_OUT = 2
	};

	tString			m_Name;
	eTrigger		m_eTrigger;
	int				m_iSndBufIndex;
	bool			m_b3DSound;
	WAVEFILELIST *	m_pWaveFileList;

	SSoundAsset();
	SSoundAsset( const SSoundAsset & );
	~SSoundAsset();
	SSoundAsset & operator=( const SSoundAsset & );
};

//////////////////////////////////////////////////////////////////////////////
//

struct SAmbience
{
	int				m_SoundAssetID;
	float			m_fPosX;
	float			m_fPosY;
	float			m_fPosZ;
	float			m_fMinDistance;
	float			m_fMaxDistance;
	int				iSoundBufferIndex;
	long			lObjectSceneID;

	SAmbience();
};


//////////////////////////////////////////////////////////////////////////////
//

#endif