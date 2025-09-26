// Z3DChrEventGenerator.cpp: implementation of the CZ3DChrEventGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DChrEventGenerator.h"
#include "SimpleParser.h"
#include <algorithm>
#include "GMMemory.h"


std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* > CZ3DChrEventGenerator::ms_mapTok2EventData;


bool CZ3DChrEventGenerator::_Load( const char* szFileName )
{
	CSimpleParser parser;
	Z3DTOK tokFilename, tokEvent;

	if( false == parser.OpenFile(szFileName) )
	{
		return false;
	}

	Z3D_EVENT_ELEMENT ee;

	//long lTokenIndex;

	char* szToken = parser.GetNextToken();
	while( NULL != szToken )
	{
		if( 0 == strcmp( ";", szToken ) )
		{
			break;
		}

		tokFilename = g_TokFileName.Add( szToken );
		if( ms_mapTok2EventData.end() != ms_mapTok2EventData.find(tokFilename) )
		{
			return false; // file�� �ߺ�
		}

		std::vector<Z3D_EVENT_ELEMENT>* p_vecEventElement = new std::vector<Z3D_EVENT_ELEMENT>;

		szToken = parser.GetNextToken();
		while( NULL != szToken )
		{
			if( 0 == strcmp( ";", szToken ) )
			{
				break;
			}

			// optional field �� �ʱ�ȭ
			ee.tokSecondaryEvent = NULL_TOK;
			ee.lProb_Denominator = 0;	// �и� 0�̸� Ȯ������ �̺�Ʈ(�׽ù߻�)
			ee.lProb_Numerator = 0;		// �ݵ�� 0���� �ʱ�ȭ �� �ʿ�� ������..��.

			ee.lFrame = aton( szToken );

			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}
			ee.tokEvent = g_TokEventName.Add( szToken );

			if( NULL == (szToken = parser.GetNextToken()) )
			{
				return false;
			}
			if( 0 == strcmp( ":", szToken ) )	// secondary event ����
			{
				szToken = parser.GetNextToken();				
				ee.tokSecondaryEvent = g_TokEventName.Add( szToken );
				szToken = parser.GetNextToken();	// ���� �ʵ带 ����
			}

			if( 0 == strcmp( "/", szToken ) ) // Ȯ�������� ����
			{
				szToken = parser.GetNextToken();				
				ee.lProb_Numerator = aton( szToken );
				parser.GetNextToken();	// skipping ":"
				szToken = parser.GetNextToken();	// ���� ��ū
				ee.lProb_Denominator = aton( szToken );
				szToken = parser.GetNextToken();	// ���� �ʵ带 ����
			}

			p_vecEventElement->push_back( ee );
		}

		std::sort( p_vecEventElement->begin(), p_vecEventElement->end() );
		ms_mapTok2EventData[tokFilename] = p_vecEventElement;

		szToken = parser.GetNextToken();
	}

	return true;
}


void CZ3DChrEventGenerator::_Close()
{
	std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* >::iterator it;

	for( it = ms_mapTok2EventData.begin(); it != ms_mapTok2EventData.end(); it++ )
	{
		SAFE_DELETE( it->second );
	}
}


void CZ3DChrEventGenerator::_RetrieveEventDataList( std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* >* &pMap )
{
	pMap = &ms_mapTok2EventData;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DChrEventGenerator::CZ3DChrEventGenerator()
{
	m_rp_vecEventData = NULL;
	m_tokCurrentAniPack = NULL_TOK;
}

CZ3DChrEventGenerator::~CZ3DChrEventGenerator()
{
}


void CZ3DChrEventGenerator::CheckEvent( float fCurrentFrame, float fPrevFrame,
		std::vector<Z3DTOK> &r_vecEvent, std::vector<Z3DTOK> &r_vecSecondaryEvent )
{
	if( NULL == m_rp_vecEventData )
	{
		return;
	}

	if( fPrevFrame >= fCurrentFrame )
	{
		return;
	}

	if( 0 == m_rp_vecEventData->size() )
	{
		return;
	}

	int lPrevIdx, lCurrentIdx;

	for( lPrevIdx = 0; lPrevIdx < (int)m_rp_vecEventData->size(); ++lPrevIdx )
	{
		if( float((*m_rp_vecEventData)[lPrevIdx].lFrame) > fPrevFrame ) 
		{
			break;			
		}
	}
	for( lCurrentIdx = m_rp_vecEventData->size() - 1; 0 <= lCurrentIdx; --lCurrentIdx )
	{
		if( float((*m_rp_vecEventData)[lCurrentIdx].lFrame) <= fCurrentFrame )
		{
			break;
		}
	}

	if( lPrevIdx > lCurrentIdx )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// ���⼭ �̺�Ʈ�߻��� ���� ���۸� Ŭ�����ϴ°��� �����Ѵ� �ൿ
	//r_vecEvent.clear();
	//r_vecSecondaryEvent.clear();
	for( int i = lPrevIdx; i <= lCurrentIdx; ++i )
	{
		if( (*m_rp_vecEventData)[i].lProb_Denominator )
		{
			// Ȯ�� ó��
			if( (rand() % (*m_rp_vecEventData)[i].lProb_Denominator) < 
				(*m_rp_vecEventData)[i].lProb_Numerator )
			{
				r_vecEvent.push_back( (*m_rp_vecEventData)[i].tokEvent );
				r_vecSecondaryEvent.push_back( (*m_rp_vecEventData)[i].tokSecondaryEvent );
			}
		}
		else
		{
			// Ȯ������( �����ǹ߻� )
			r_vecEvent.push_back( (*m_rp_vecEventData)[i].tokEvent );
			r_vecSecondaryEvent.push_back( (*m_rp_vecEventData)[i].tokSecondaryEvent );
		}
	}
}


bool CZ3DChrEventGenerator::SetTokAniPack( Z3DTOK tokFileName )
{
	std::map< Z3DTOK, std::vector<Z3D_EVENT_ELEMENT>* >::iterator it;
	
	if( ms_mapTok2EventData.end() == (it = ms_mapTok2EventData.find(tokFileName)) )
	{
		m_rp_vecEventData = NULL;
		return false;
	}

	m_rp_vecEventData = it->second;
	return true;
}


Z3DTOK CZ3DChrEventGenerator::GetTokAniPack()
{
	return m_tokCurrentAniPack;
}
