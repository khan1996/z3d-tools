// NatureParticle.cpp: implementation of the CNatureParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "NatureParticle.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNatureParticle::CNatureParticle() {
	m_pNodes = NULL;
	
	m_iNodesNum = 0;
	m_iScreenNodesNum = 0;

	m_pParticleTexture = NULL;
	m_pExtTexture = NULL;

	m_bExtUse = false;
	m_bExtUse2 = false;

	m_bEnd = false;
	m_bStart = false;
	m_bFirst = true;
	m_iExtCount = 0;

	m_pDevice = NULL;
}

CNatureParticle::~CNatureParticle() {
	Release();
}
void CNatureParticle::Release() {
	if(m_pNodes != NULL) {
		delete[] m_pNodes;
		m_pNodes = NULL;
	}
	if(m_pParticleTexture != NULL) {
		delete m_pParticleTexture;
		m_pParticleTexture = NULL;
	} 
	if(m_pExtTexture != NULL) { 
		delete m_pExtTexture;
		m_pExtTexture = NULL;
	}
}

void CNatureParticle::CreateInUseNode(float fVelocityX,float fVelocityY,float fVelocityZ,int iRandValue) {

	int iEndNode;
	int i;

	iEndNode= -1;
	
	for(i = 0; i < m_iNodesNum; i++ ) {
		if(m_pNodes[i].m_fLife <= 0.0f) {
			iEndNode = i;
			break;

		}
	}
	if(iEndNode == -1)
		return;
	m_pNodes[iEndNode].m_fLife = m_fMaxLife * RANDOM_FLOAT;
	
	m_pNodes[iEndNode].m_vecPos = m_vecPos;

	m_pNodes[iEndNode].m_vecBeforePos = m_vecPos;

	m_pNodes[iEndNode].m_vecVelocity = D3DXVECTOR3(fVelocityX,fVelocityY,fVelocityZ);
	//const float ColorValue[3] = { 0.1f,0.6f,1.0f};

	//float fColor = ColorValue[rand() % 3];

	//float fColor = (300 + (float)(rand() % 700)) / 1000.0f;
	
	m_pNodes[iEndNode].m_vecColor = m_vecStartColor;
	
	m_pNodes[iEndNode].m_vecColorUnit = (m_vecEndColor - m_vecStartColor);
	m_pNodes[iEndNode].m_vecColorUnit/= m_pNodes[iEndNode].m_fLife;
//	m_pNodes[iEndNode].m_vecColor = D3DXVECTOR3(fColor,fColor,fColor);
//	m_pNodes[iEndNode].m_vecColorUnit= D3DXVECTOR3(0.0f,0.0f,0.0f);


	m_pNodes[iEndNode].m_fVisible = m_fStartVisible;
	m_pNodes[iEndNode].m_fVisibleUnit = (m_fEndVisible - m_fStartVisible) / m_pNodes[iEndNode].m_fLife;
	
	m_pNodes[iEndNode].m_fFraction = m_fFraction;
	m_pNodes[iEndNode].m_fMass = m_fMass;
	
	m_pNodes[iEndNode].m_vecSize = m_vecStartSize;
	m_pNodes[iEndNode].m_vecSizeUnit = (m_vecEndSize - m_vecStartSize);
	m_pNodes[iEndNode].m_vecSizeUnit /= m_pNodes[iEndNode].m_fLife;

	if(iRandValue) {
		
		m_pNodes[iEndNode].m_vecColorUnit = D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

}
bool CNatureParticle::InitParticle(int iNodesNum,char *strTexture) {
	
	srand(time(0));


	m_pDevice = CSceneManager::GetDevice();

	if(m_pParticleTexture != NULL) {
		delete m_pParticleTexture;
		m_pParticleTexture = NULL;
	}
	m_pParticleTexture = new CTexture;
	m_pExtTexture = new CTexture;

	CTexture::SetPath(EFFECTTEXTUREPATH);
	
	m_pParticleTexture->Load(strTexture);
	m_pExtTexture->Load("CParticle2.dds");

	m_pNodes = new CParticleNode[iNodesNum];
	m_iNodesNum = iNodesNum;
	
	m_iVertexBufferNum = m_iNodesNum * 4;
	m_iIndexBufferNum = m_iNodesNum * 6;

	//m_pDevice->CreateVertexBuffer();
	//m_pDevice->CreateIndexBuffer();

	return true;

}
// RGB 24 Bit Texture Position Setting 
void CNatureParticle::SetParticlePosition(BYTE *pImage,DWORD dwWidth,DWORD dwHeight) {
	

	int i,j;
	int iCount = 0;

	int iTotalPosNum = dwWidth * dwHeight;
	int iDiv = (iTotalPosNum / m_iNodesNum);
	
	if(iDiv <= 0)
		iDiv = 1;
	
	iDiv *= 4;

	BYTE *pTmp = pImage;

	for(i = 0; i < (int)dwHeight; i++ ) {
		for(j = 0; j < (int)dwWidth; j++ ) {
			
			if((pTmp[0] != 255) || (pTmp[1] != 255) || (pTmp[2] != 255)) {
				m_pNodes[iCount].m_fExtension[0] = (j - ((int)dwWidth / 2)) * 10.0f;
				m_pNodes[iCount].m_fExtension[1] = (i - ((int)dwHeight / 2)) * 10.0f;
				m_pNodes[iCount].m_fExtension[2] = 0.0f;				
				iCount++;

			}

			//pTmp += iDiv;
			pTmp += 4;
		}
	}
	
	m_iImageParticleNum = iCount;
	
}
void CNatureParticle::Update(float fStep,float fHeight) {
	D3DXVECTOR3 vecUpdate;
	int iCount = 0;

	for(int i = 0; i < m_iNodesNum; i++) {
		m_pNodes[i].m_fLife -= 1.0f;
		if(m_pNodes[i].m_fLife > 0.0f) {
			vecUpdate = m_pNodes[i].m_vecVelocity * m_pNodes[i].m_fMass;
			m_pNodes[i].m_vecPos += vecUpdate;
			m_pNodes[i].m_vecColor += m_pNodes[i].m_vecColorUnit;
			m_pNodes[i].m_vecSize += m_pNodes[i].m_vecSizeUnit;
			m_pNodes[i].m_fVisible += m_pNodes[i].m_fVisibleUnit;

			m_pNodes[i].m_vecVelocity *= (1.0f - m_pNodes[i].m_fFraction);
			m_pNodes[i].m_vecVelocity += m_vecGravity;
	
			if(m_bExtUse) { 
				m_pNodes[i].m_vecPos.x += m_pNodes[i].m_fExtension[0] * 0.1f;
				m_pNodes[i].m_vecPos.y -= m_pNodes[i].m_fExtension[1] * 0.1f;
				
			}
			else if(fHeight > 0.0f){
				m_pNodes[i].m_vecPos.y = m_pNodes[i].m_vecBeforePos.y + fHeight;
			}
			else {
				m_pNodes[i].m_vecPos.x += m_pNodes[i].m_fExtension[0] * 0.04f;
				m_pNodes[i].m_vecPos.y -= m_pNodes[i].m_fExtension[1] * 0.04f;

				m_pNodes[i].m_vecPos.x += ((float)(rand() % 100) / 10.0f) - 5.0f;//m_pNodes[i].m_fExtension[0] * 0.1f;
				m_pNodes[i].m_vecPos.z += ((float)(rand() % 100) / 10.0f) - 5.0f;//m_pNodes[i].m_fExtension[1] * 0.1f;
				
				m_pNodes[i].m_vecPos.y -= (vecUpdate.y / 2.0f);	
			}
		


		}
		else {
			iCount++;
		}
	}
	if(m_bExtUse) {
		m_iExtCount++;
	}
	if(iCount >= (m_iNodesNum - 50)) {
		if(!m_bStart && m_bExtUse2)
			m_bEnd = true;
		
		m_bExtUse = false;
		m_bExtUse2 = false;
	
	}

	if(m_iExtCount >= 25)
		m_bExtUse = false;


}

/*void CNatureParticle::Render() {


}
*/
void CNatureParticle::Explode(float fMag,int iNodesNums) {

	float fYaw;
	float fPitch;

	while((--iNodesNums)>0) {
		fYaw  = RANDOM_FLOAT*PI*2.0f;
		fPitch= DEG_TO_RAD( RANDOM_FLOAT*( rand( )%360 ) );

		CreateInUseNode( ( cosf( fPitch ) )*( fMag*RANDOM_FLOAT ),
						( sinf( fPitch )*cosf( fYaw ) )*( fMag*RANDOM_FLOAT ),
						( sinf( fPitch )*sinf( fYaw ) )*( fMag*RANDOM_FLOAT ) );
	}
}
void CNatureParticle::CreateChristmasParticle(float fX,float fY,float fZ,int iDropSpeed,int iNumDrops,float fRightX,float fRightY,float fRightZ,float fUpX,float fUpY,float fUpZ,bool bForce) {

	D3DXVECTOR3 vecBasePos;
	int i;
//	vecBasePos= m_vecPos;


	for( i=0; i<iNumDrops; i++ )
	{
	
		SetBasePosition( fX,fY,fZ );
		if(bForce) {
			m_pNodes[i].m_fLife = -1.0f;
		}
		CreateInUseNode( 0.0f, -iDropSpeed, 0.0f ,1);
	}
/*	D3DXVECTOR3 vecRight(fRightX,fRightY,fRightZ);

	vecRight.x *= m_pNodes[i].m_fExtension[0];
	vecRight.y *= -m_pNodes[i].m_fExtension[1];
	vecRight.z = 0.0f;//m_pNodes[i].m_fExtension[0];

	D3DXVECTOR3 vecUp(fUpX,fUpY,fUpZ);

	vecUp.x *= m_pNodes[i].m_fExtension[0];
	vecUp.y *= -m_pNodes[i].m_fExtension[1];
	vecUp.z = 0.0f;//m_pNodes[i].m_fExtension[0];

	for( i = 0; i < m_iImageParticleNum; i++ ) {
		m_pNodes[i].m_fExtension[0] += vecRight.x;
		m_pNodes[i].m_fExtension[0] += vecUp.x;

		m_pNodes[i].m_fExtension[1] += vecRight.y;
		m_pNodes[i].m_fExtension[1] += vecUp.y;

		m_pNodes[i].m_fExtension[2] += vecRight.z;
		m_pNodes[i].m_fExtension[2] += vecUp.z;
		
	}
*/

//	m_vecPos= vecBasePos;

}
void CNatureParticle::CreateRain(float fMinRangeX,float fMinRangeY,float fMinRangeZ,
				float fMaxRangeX,float fMaxRangeY,float fMaxRangeZ,
				int iDropSpeed,int iNumDrops) {
	

	D3DXVECTOR3 vecBasePos;
	int i;
	vecBasePos= m_vecPos;

	for( i=0; i<iNumDrops; i++ )
	{
	
		SetBasePosition( RangedRandom( fMinRangeX, fMaxRangeX ),
							 RangedRandom( fMinRangeY, fMaxRangeY ),
							 RangedRandom( fMinRangeZ, fMaxRangeZ ) );

		CreateInUseNode( 0.0f, -iDropSpeed, 0.0f );
	}


	m_vecPos= vecBasePos;
}
