// X3DEffectMesh.cpp: implementation of the CX3DEffectMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectMesh.h"
#include "EffDebugLog.h"
#include "SceneManager.h"
#include "GMMemory.h"


//#define __EFF_WCREATOR__

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CashNode CX3DEffectMesh::m_lstCash;		// Mesh cash list
//int CX3DEffectMesh::m_nCash = 0;		// Mesh cash Num
//void CX3DEffectMesh::DeleteAllCash();	// Cash ���� delete

CX3DEffectMesh::CX3DEffectMesh()
{
	m_dwSrcBlending = D3DBLEND_SRCALPHA;
	m_dwDestBlending = D3DBLEND_ONE;

	m_bTexAni = FALSE;
	m_moMesh = NULL;
	m_GemRender = NULL;
	m_fRenderFrame = 0.0f;
	m_GemObjectTexNum = NULL;
	m_dwTick = NULL;
	m_bFirst = NULL;
	m_bUpdateFrame = false;
	m_pSmrMesh = NULL;

//	����:m_GemRender = new CGemRender;
}

CX3DEffectMesh::~CX3DEffectMesh()
{
	if(m_moMesh) { delete[] m_moMesh; m_moMesh = NULL; }

	if(m_GemRender)
	{
		SubUseCount(m_GemRender->GetFileName());			// ���� ī���� 1 ����
	//	����:if(m_GemRender) {delete m_GemRender; m_GemRender = NULL;}
		if(m_GemObjectTexNum != NULL)
			delete[] m_GemObjectTexNum;
	}
	if(m_pSmrMesh)
		delete m_pSmrMesh;

	if(m_dwTick != NULL)
		delete[] m_dwTick;
	delete m_bFirst;
}

void CX3DEffectMesh::Create(unsigned long dwStartFrame, unsigned long dwEndFrame)
{
	m_dwStartFrame = dwStartFrame;
	m_dwEndFrame = dwEndFrame;
}

BOOL CX3DEffectMesh::CreateBuffer()
{
	return TRUE;
}

void CX3DEffectMesh::Render(void)
{
	

	DWORD cullmode,zmode;
	m_lpD3DDevice->GetRenderState(D3DRS_CULLMODE,&cullmode);
	m_lpD3DDevice->GetRenderState(D3DRS_ZWRITEENABLE,&zmode);

	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	D3DXVECTOR3 *vecCenter = (D3DXVECTOR3 *)((CX3DEffect *)m_lpLocalEffect)->GetCenter();

	if(m_GemRender)
	{
		m_GemRender->SetScale(m_Scale[0],m_Scale[1],m_Scale[2]);
		m_GemRender->SetEffectPos(*vecCenter);
		// rotation setting
		if(m_QuatSet) {
			D3DXQUATERNION tm_q;
			tm_q.x = m_quatAxis.x;
			tm_q.y = m_quatAxis.y;
			tm_q.z = m_quatAxis.z;
			tm_q.w = m_quatAxis.w;

			m_GemRender->GetAxis(tm_q);
		}

		m_GemRender->SetClearBuffer(m_bClearBuffer);
		if(m_bUpdateFrame) {
			// Cash �� ���� ������ 1 �ذ� 
			// Texture ani �� Random Start Texture Ani �׸��� Random Ani �� ���� m_GemObjectTexNum �迭�� �̿��Ѵ�
			// Gem�� ĳ���� ���� �Ͼ�� ���� �������� �ϳ���.
			// m_GemObjectTexNum[ 0 ~ ObjectNum - 1] : �ؽ��� �ִ� �� ����� mesh�� ���� �ý��� ��ȣ 
			// m_GemObjectTexNun[ObjectNum] : Mesh Before Frame
			// m_GemObjectTexNum[ObjectNum + 1] : Mesh Current Frame
			// m_GemObjectTexNum[ObjectNum + 2] : ���� ��ŸƮ �ִϽÿ� ó�� �ؽ��� ������ ���� �Ǿ��°��� üũ�ϴ� üũ �÷���
			// m_GemObjectTexNum[ObjectNum + 3] : �ؽ��� �ִϽÿ� �ٷ� ���� �ؽ��İ� ���ŵǾ��� ������ ���.
			// Cash �� ���� ������ 2 �ذ�
			// Cash �� �Կ� �־ �������� SetCurrentFrame �� ���־�� �Ѵ�
			// �׷��Ƿ� gem �� update�Ǿ��� ������ bUpdateFrame �� true �� �������־� 
			// Update ���ش�.
			m_GemRender->SetCurrentFrame(m_fRenderFrame,m_GemObjectTexNum,m_dwTick,m_bFirst,m_bUpdateFrame);
			m_bUpdateFrame = false;
		}
		else
			m_GemRender->SetCurrentFrame(m_fRenderFrame,m_GemObjectTexNum,m_dwTick,m_bFirst,m_bUpdateFrame);

		if(m_bVisibility)
			m_GemRender->Render();

	}
	if(m_pSmrMesh)
	{
		D3DXQUATERNION tm_q(0,0,0,1);
		if(m_QuatSet)
		{
			tm_q.x = m_quatAxis.x;
			tm_q.y = m_quatAxis.y;
			tm_q.z = m_quatAxis.z;
			tm_q.w = m_quatAxis.w;
		}

		m_pSmrMesh->Render(CSceneManager::GetDevice(),*vecCenter,D3DXVECTOR3(m_Scale[0],m_Scale[1],m_Scale[2]),tm_q,m_fRenderFrame);
	}

	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE,cullmode);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,zmode);

	
}

void CX3DEffectMesh::LoadFile(char *file)
{
	bool m_bGem = true;	
	char strTest1[256];
	strcpy(strTest1, file);

	char strTest2[256];
	strcpy(strTest2, file);

	char *pStr = strrchr(strTest1,'.');
	pStr++;
	*(pStr++) = 'G';
	*(pStr++) = 'E';
	*(pStr) = 'M';

	pStr = strrchr(strTest2,'.');
	pStr++;
	*(pStr++) = 'S';
	*(pStr++) = 'M';
	*(pStr) = 'R';

	if(stricmp(file,strTest1) == 0)
	{
		strcpy(m_strMeshFile, file);	
	}
	else
	{
		strcpy(m_strMeshFile, strTest2);	
		m_bGem =false;
	}
	
	if(m_bGem)
	{


		// Cash Load
		if( ( m_GemRender = LoadCash(file) ) == NULL ) {
			
			m_GemRender = new CGemRender;
			m_GemRender->SetLight(m_bLight);							//Set Vertex Light

	#ifdef __EFF_WCREATOR__
			if(!m_GemRender->LoadGemFile(file,m_lpD3DDevice,m_bVisibility))
				WriteDebug(file);
	#else
			m_GemRender->LoadGemFile(file,m_lpD3DDevice,m_bVisibility);
	#endif

			InputCash(m_GemRender);										//Cash List �ȿ� Mesh�� ���ٸ� Cash List�� input. 

		}
		else {		//Cash load
			if(m_GemRender->m_bDecal)										// Decal�� �̿��Ѱ͵��� Vertex Buffer �� �ʱ�ȭ ���ش�
				m_GemRender->SetInitBuffer();
				
		}
		// Frame Init
		//m_GemRender->SetCurrentFrame(0.0f);

		// Texture Load (����Ʈ ����ɼ��� Ŀ���ִٰ� �������� ĳ���� gem �� texture Load)
		if(m_bVisibility && (m_GemRender->m_Texture == NULL))
			m_GemRender->LoadTexture();

		m_GemRender->SetLight(m_bLight);								//Set Vertex Light								
		m_GemRender->SetMine(m_Mine);
		m_GemRender->SetBlend(m_dwSrcBlending, m_dwDestBlending);
		m_GemRender->SetScale(m_Scale[0],m_Scale[1],m_Scale[2]);
		m_GemRender->SetCash(false);									// Boid �� Ų��

		m_dwObjectNum = m_GemRender->GetObjectNum();
		
		m_GemObjectTexNum = new int[m_dwObjectNum + 4];
	/*	m_dwTick = new DWORD[2];
		m_bFirst = new bool;
		
		*m_bFirst = false;
		memset(m_dwTick,0,sizeof(DWORD) * 2);
	*/
		memset(m_GemObjectTexNum,0,sizeof(int) * (m_dwObjectNum + 4));
		m_GemObjectTexNum[m_dwObjectNum + 2] = -1;						// Rand Start Ani �� ���� �ʱ�ȭ

	}
	else
	{
		m_pSmrMesh = new CSMRObj;
		m_pSmrMesh->SetSrcBlend(m_dwSrcBlending);
		m_pSmrMesh->SetDstBlend(m_dwDestBlending);
		m_pSmrMesh->Load(strTest2);
	
		m_dwObjectNum  = (DWORD)m_pSmrMesh->m_iMeshData;

	}
	m_dwTick = new DWORD[2];
	m_bFirst = new bool;
	
	*m_bFirst = false;
	memset(m_dwTick,0,sizeof(DWORD) * 2);


	m_moMesh = new MeshObject[m_dwObjectNum];
}

BOOL CX3DEffectMesh::Interpolation(float fFrame)
{
	for(long i = 0; i < (int)m_dwObjectNum; i++)
	{
///////////////////////////////////////////////////////////////////////////////////
		if(!m_moMesh[i].m_lstColor.InterpolationC(fFrame, m_lColor)) return FALSE;
///////////////////////////////////////////////////////////////////////////////////
		// color setting
		if(m_GemRender)
			m_GemRender->SetColor(i,m_lColor.r, m_lColor.g, m_lColor.b);
		//m_GemObjectTexNum[i]++;
	}

	// current frame setting
	m_fRenderFrame = fFrame;
	m_bUpdateFrame = true;
	//m_GemRender->SetCurrentFrame(fFrame);
	return TRUE;
}

void CX3DEffectMesh::Load(FILE *fp, const char *strOriginalPath)
{
	fread(&m_dwSrcBlending, 4, 1, fp);
	fread(&m_dwDestBlending, 4, 1, fp);

	unsigned char len;
	fread(&len, 1, 1, fp);
	if(len)
	{
		char strTemp[MAX_PATH];
		fread(strTemp, len, 1, fp);
		strcpy(m_strMeshFile, strOriginalPath);
		strcat(m_strMeshFile, strTemp);
	} else
	{
		strcpy(m_strMeshFile, "");
	}

	LoadFile(m_strMeshFile);

	for(long i = 0; i < (int)m_dwObjectNum; i++)
	{
		
		m_moMesh[i].m_lstColor.Load(fp, m_lColor);
		fread(&m_moMesh[i].m_fTexFrame, 4, 1, fp);
		if(m_GemRender)
			m_GemRender->SetChangeAniFrame(i,m_moMesh[i].m_fTexFrame);
		
		fread(&m_moMesh[i].m_fStartTexFrame, 4, 1, fp);
		if(m_GemRender)
			m_GemRender->SetStartTexAniFrame(i,m_moMesh[i].m_fStartTexFrame);

	}
}

void CX3DEffectMesh::Save(FILE *fp, const char *strOriginalPath)
{
	fwrite(&m_dwSrcBlending, 4, 1, fp);
	fwrite(&m_dwDestBlending, 4, 1, fp);

	if(strlen(m_strMeshFile))
	{
		char strMesh[MAX_PATH], strTemp[MAX_PATH];
		strcpy(strMesh, m_strMeshFile);
		if(!strncmp(strMesh, strOriginalPath, strlen(strOriginalPath)))
		{
			strcpy(strTemp, &strMesh[strlen(strOriginalPath)]);
		} else {
			MessageBox(NULL, "�޽� ������ �߸�����", "Effect Editor", MB_OK);
			return;
		}

		unsigned char len = strlen(strTemp) + 1;
		fwrite(&len, 1, 1, fp);
		fwrite(strTemp, len, 1, fp);

		for(long i = 0; i < (int)m_dwObjectNum; i++)
		{
			m_moMesh[i].m_lstColor.Save(fp);
			fwrite(&m_moMesh[i].m_fTexFrame, 4, 1, fp);
			fwrite(&m_moMesh[i].m_fStartTexFrame, 4, 1, fp);
		}
	} else
	{
		MessageBox(NULL, "�޽� ������ �߸�����", "Effect Editor", MB_OK);
	}
}

////////////////////////////////// Mesh Cash Func Start //////////////////////////////////

void CX3DEffectMesh::InputCash(CGemRender *pMesh) {				// Cash �ȿ� mesh data input
	if(m_lstCash.m_Cash.size() > EFF_MAXCASH) {
		DeleteCash();
	}
	_CashNode *pNode;
	pNode = new _CashNode;
	pNode->m_pMesh = pMesh;

	pNode->m_nUseCount = 1;
	m_lstCash.m_Cash.push_back(pNode);
	m_lstCash.m_nCash++;	
/*
	if(m_lstCash.size() > EFF_MAXCASH) {
		DeleteCash();
	}
	_CashNode *pNode;
	pNode = new _CashNode;
	pNode->m_pMesh = pMesh;

	pNode->m_nUseCount = 1;
	m_lstCash.push_back(pNode);

	m_nCash++;	
*/
}
void CX3DEffectMesh::SubUseCount(char *strFilename) {			//Cash Node�� ���� ī���� ���� 

	if(strFilename == NULL) 
		return;
	for(int i=0;i<(int)m_lstCash.m_Cash.size();i++) {
		if(strstr(m_lstCash.m_Cash[i]->m_pMesh->GetFileName(),strFilename)) {
			m_lstCash.m_Cash[i]->m_nUseCount--;
		}	
	}
/*	if(strFilename == NULL) 
		return;
	for(int i=0;i<m_lstCash.size();i++) {
		if(strstr(m_lstCash[i]->m_pMesh->GetFileName(),strFilename)) {
			m_lstCash[i]->m_nUseCount--;
		}	
	}
*/
}
CGemRender *CX3DEffectMesh::LoadCash(char *strFilename) {		// Cash ���� mesh load

	if(strFilename == NULL) 
		return NULL;

	for(int i=0;i<(int)(m_lstCash.m_Cash.size());i++) {
		if(!strcmp(m_lstCash.m_Cash[i]->m_pMesh->GetFileName(),strFilename)) {
		
			m_lstCash.m_Cash[i]->m_nUseCount++;
			return m_lstCash.m_Cash[i]->m_pMesh;
		}	
	}

	return NULL;

/*
	if(strFilename == NULL) 
		return NULL;

	for(int i=0;i<m_lstCash.size();i++) {
		if(strstr(m_lstCash[i]->m_pMesh->GetFileName(),strFilename)) {
		
			m_lstCash[i]->m_nUseCount++;
			return m_lstCash[i]->m_pMesh;
		}	
	}

	return NULL;
*/
/*

	std::map<char*,CGemRender *>::iterator it;

	it = m_lstCash.find( const_cast<char*>(strFilename));
	
	if( m_lstCash.end() == it )
	{
		return NULL;
	}
	
	return it->second;
*/
}
void CX3DEffectMesh::DeleteCash() {								// Cash ���� mesh Data delete

	for(int i=0;i<(int)(m_lstCash.m_Cash.size());i++) {
		if(!m_lstCash.m_Cash[i]->m_nUseCount) {
			delete m_lstCash.m_Cash[i];
			m_lstCash.m_Cash[i] = NULL;	
			m_lstCash.m_Cash.erase(m_lstCash.m_Cash.begin() + i);
			m_lstCash.m_nCash--;
			break;
		}	
	}
	/*
	for(int i=0;i<m_lstCash.size();i++) {
	if(!m_lstCash[i]->m_nUseCount) {
		delete m_lstCash[i];
		m_lstCash[i] = NULL;	
		m_lstCash.erase(&(m_lstCash[i]));
		m_nCash--;
		break;
	}	
	
	*/
}
/*
void CX3DEffectMesh::DeleteAllCash() {	// Cash list ���� delete

	for(int i=0;i<m_lstCash.m_Cash.size();i++) {
		delete m_lstCash.m_Cash[i];
		m_lstCash.m_Cash[i] = NULL;				
	}
	m_lstCash.m_Cash.clear();
	m_lstCash.m_nCash = 0;

/*	
	for(int i=0;i<m_lstCash.size();i++) {
		delete m_lstCash[i];
		m_lstCash[i] = NULL;				
	}
	m_lstCash.clear();
	m_nCash = 0;
*/	
//}
////////////////////////////////// Mesh Cash Func End //////////////////////////////////
