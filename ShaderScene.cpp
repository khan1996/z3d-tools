
#include "ShaderScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

/* ***********************
CVertexShader
************************* */
CVertexShader::~CVertexShader()
{

}
void CVertexShader::CreateVertexShader(char* filename, DWORD* vertexShaderDeclaration)
{
	// Create vertex VertexShader
	LPD3DXBUFFER VertexShadercode=NULL;

	CShaderScene::PreprocessAndAssembleShaderFromFile2(filename,	NULL, &VertexShadercode);
	CSceneManager::GetDevice()->CreateVertexShader(vertexShaderDeclaration,
 		(DWORD*)VertexShadercode->GetBufferPointer(),	&m_dwVertexShader, 0);

	SAFE_RELEASE(VertexShadercode);
}

/* ***********************
CPixelShader
************************* */

CPixelShader::~CPixelShader()
{

}

void CPixelShader::CreatePixelShaderFromFile(char* filename)
{
	// Create pixel shader
	LPD3DXBUFFER shadercode=NULL;
	CShaderScene::PreprocessAndAssembleShaderFromFile2(filename, NULL,	&shadercode);

	CSceneManager::GetDevice()->CreatePixelShader(	(DWORD*)shadercode->GetBufferPointer(), 
		&m_dwPixelShader);

	SAFE_RELEASE(shadercode);
}

void CPixelShader::CreatePixelShaderFromBuffer(char *string)
{
	// Create pixel shader
	LPD3DXBUFFER shadercode=NULL;
	CShaderScene::PreprocessAndAssembleShaderFromBuffer(string, strlen(string), NULL,	&shadercode);

	CSceneManager::GetDevice()->CreatePixelShader(	(DWORD*)shadercode->GetBufferPointer(), 
		&m_dwPixelShader);

	SAFE_RELEASE(shadercode);
}


/* ***********************
CShderScene
************************* */

CShaderScene::CShaderScene() {
	m_pDevice = CSceneManager::GetDevice();
	

	m_bGetState = false;
	m_bSetState = false;
	memset(m_strName,0,sizeof(char) * 256);


}
CShaderScene::~CShaderScene() {

}
HRESULT CShaderScene::PreprocessAndAssembleShaderFromBuffer(LPCVOID pSrcData, UINT SrcDataLen,
														LPD3DXBUFFER *ppConstants,LPD3DXBUFFER *ppCode) {
	LPD3DXBUFFER* ppErrors=NULL;
    HRESULT hResult=D3DXAssembleShader(pSrcData, SrcDataLen, 0,  ppConstants, ppCode, ppErrors);
	return hResult;

}

HRESULT CShaderScene::PreprocessAndAssembleShaderFromFile2(LPCSTR szFile,LPD3DXBUFFER *ppConstants, LPD3DXBUFFER *ppCode) {


	LPD3DXBUFFER* ppErrors=NULL;
	HRESULT hResult = D3DXAssembleShaderFromFile(szFile, 0, ppConstants, ppCode, ppErrors);
	return hResult;

}

void CShaderScene::ApplyOldState() {
	if((m_pDevice != NULL) && m_bGetState) {
		m_pDevice->SetRenderState(D3DRS_ZENABLE,m_bOldEnable[ZBUFFER]);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,m_bOldEnable[ZWRITE]);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,m_bOldEnable[ALPHABLEND]);

		m_pDevice->SetRenderState(D3DRS_CULLMODE,m_dwOldCullFace);

		if(m_bEnable[ALPHABLEND] == true) {
			m_pDevice->SetRenderState(D3DRS_SRCBLEND,m_dwOldSrcBlend);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND,m_dwOldDestBlend);
		}
		
	}
}
void CShaderScene::ApplyState() {
	if(m_bSetState = true) {
		if(m_pDevice != NULL) {
			m_pDevice->SetRenderState(D3DRS_ZENABLE,m_bEnable[ZBUFFER]);
			m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,m_bEnable[ZWRITE]);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,m_bEnable[ALPHABLEND]);

			m_pDevice->SetRenderState(D3DRS_CULLMODE,m_dwCullFace);

			if(m_bEnable[ALPHABLEND] == true) {
				m_pDevice->SetRenderState(D3DRS_SRCBLEND,m_dwSrcBlend);
				m_pDevice->SetRenderState(D3DRS_DESTBLEND,m_dwDestBlend);
			}
			
		}
	}
	D3DXVec4Normalize(&m_vecLightDir,&m_vecLightDir);
	m_pDevice->SetVertexShaderConstant(21, m_vecLightDir, 1);

}
void CShaderScene::SetState(int iFlag,bool bEnable,DWORD dwValue1,DWORD dwValue2) {
	m_bEnable[ZBUFFER] = bEnable;

	switch(iFlag) {
	case ALPHABLEND:
		m_dwSrcBlend = dwValue1;
		m_dwDestBlend = dwValue2;
		break;
	case CULLMODE:
		m_dwCullFace = dwValue1;
		break;
	default:
		break;

	};
	m_bSetState = true;
}
void CShaderScene::GetState() {
	if(m_pDevice != NULL) {
		DWORD dwValue;

		m_pDevice->GetRenderState(D3DRS_ZENABLE ,&dwValue);
		m_bOldEnable[ZBUFFER] = (dwValue == 1) ? true : false;
		m_pDevice->GetRenderState(D3DRS_ZWRITEENABLE ,&dwValue);
		m_bOldEnable[ZWRITE] = (dwValue == 1) ? true : false;
		m_pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE ,&dwValue);
		m_bOldEnable[ALPHABLEND] = (dwValue == 1) ? true : false;

		m_pDevice->GetRenderState(D3DRS_CULLMODE,&m_dwOldCullFace);
		m_pDevice->GetRenderState(D3DRS_SRCBLEND,&m_dwOldSrcBlend);
		m_pDevice->GetRenderState(D3DRS_DESTBLEND,&m_dwOldDestBlend);

		m_bGetState = true;

	}
}
void CShaderScene::Create(char *strShader) {
	strcpy(m_strName,strShader);	
}
void CShaderScene::SetLightDirection(D3DXVECTOR3 vecDir) {
	m_vecLightDir = D3DXVECTOR4(vecDir.x,vecDir.y,vecDir.z,1.0f);
}
void CShaderScene::SetLightPos(D3DXVECTOR3 vecPos) {
	m_vecLightPos = D3DXVECTOR4(vecPos.x,vecPos.y,vecPos.z,1.0f);
}

