
#include "AmbienceStruct.h"

#include <list>
#include <vector>
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////////////
//

tString::tString()
	: m_pString( NULL )
{
}

tString::tString( const char * szString )
{
	m_pString = new char[ strlen( szString ) + 1 ];
	strcpy( m_pString, szString );
}

tString::tString( const tString & rhs )
	: m_pString( NULL )
{
	this->operator=( rhs.m_pString );
}

tString::~tString()
{
	if(m_pString) {
		delete[] m_pString;
		m_pString = NULL;
	}
}

tString & tString::operator=( const tString & rhs )
{
	if(m_pString) {
		delete[] m_pString;
		m_pString = NULL;
	}
	if( rhs.m_pString == NULL )
	{
		m_pString = NULL;
	}
	else
	{
		m_pString = new char[ strlen( rhs.m_pString ) + 1 ];
		strcpy( m_pString, rhs.m_pString );
	}

	return *this;
}

unsigned int	tString::size()
{
	if( m_pString )
		return strlen( m_pString );
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////////
//


SSoundAsset::SSoundAsset()
	: m_eTrigger( T_IN_OUT )
	, m_iSndBufIndex( -1 )
	, m_b3DSound( true )
	, m_Name( "이름 없음" )
	, m_pWaveFileList( new WAVEFILELIST )
{}

SSoundAsset::SSoundAsset( const SSoundAsset & rhs )
	: m_eTrigger( rhs.m_eTrigger )
	, m_iSndBufIndex( rhs.m_iSndBufIndex )
	, m_b3DSound( rhs.m_b3DSound )
	, m_Name( rhs.m_Name )
	, m_pWaveFileList( new WAVEFILELIST( *rhs.m_pWaveFileList ) )
{
}

SSoundAsset::~SSoundAsset()
{
	if(m_pWaveFileList != NULL) {
		delete m_pWaveFileList;
		m_pWaveFileList = NULL;
	}
}

SSoundAsset & SSoundAsset::operator =( const SSoundAsset & rhs )
{
	m_eTrigger = rhs.m_eTrigger;
	m_iSndBufIndex = rhs.m_iSndBufIndex;
	m_b3DSound = rhs.m_b3DSound;
	m_Name = rhs.m_Name;
	
	if( m_pWaveFileList )
		*m_pWaveFileList = *rhs.m_pWaveFileList;
	else
		m_pWaveFileList = new WAVEFILELIST( *rhs.m_pWaveFileList );

	return *this;
}


//////////////////////////////////////////////////////////////////////////////
//


SAmbience::SAmbience()
	: m_SoundAssetID( 0 )
	, m_fPosX( 0.0f )
	, m_fPosY( 0.0f )
	, m_fPosZ( 0.0f )
	, m_fMinDistance( 0.0f )
	, m_fMaxDistance( 0.0f )
	, iSoundBufferIndex( -1 )
	, lObjectSceneID( 0 )
{}


//////////////////////////////////////////////////////////////////////////////
