// FullScenePShader.cpp: implementation of the CFullScenePShader class.
//
//////////////////////////////////////////////////////////////////////

#include "FullScenePShader.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	
char Brightness[] =  
"ps.1.1\n"
"tex t0\n"
"add r0, t0, c0\n";

char Black[] =
"ps.1.1\n"
"tex t0\n"
"dp3 r0, t0, c0\n"
"mov r0.a, c2.a\n";
 
char Edges[] = 
"ps.1.1\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"
"mov_x2 r0, t0\n"
"sub r0, r0, t1\n"
"sub_x2 r0, r0, t2\n";

char Negative[] = 
"ps.1.1\n"
"tex t0\n"
"mov r0, 1-t0\n"
"mov r0.a, c2.a\n";

char Sepia[] = 
"ps.1.1\n"
"def c1, 0.3, 0.6, 0.1, 1.0\n"
"def c2, 0.9, 0.7, 0.3, 1.0\n"
"def c3, 0.5, 0.5, 0.5, 1.0\n"
"tex t0\n"
"dp3 r0, t0, c1\n"
"mul r0, r0, c2\n"
"mov r0.a, c2.a\n";

char Edge2[] =
"ps.1.1\n"
"def c1, 0.3f, 0.59f, 0.11f, 0.0f\n"
"def c4, 1.0f, 1.0f, 1.0f, 1.0f\n"
"tex t0\n"
"tex t1\n"
"tex t2\n"
"tex t3\n"
"dp3    r0.rgba, t0, c1\n"		// convert t0 color to luminance, store in r0.a
"dp3    r1.rgba, t2, c1\n"		// convert t2 color to luminance, store in r1.a
"dp3    r0.rgb, t1, c1\n"		// convert t1 color to luminance, store in r0.rgb
"dp3    r1.rgb, t3, c1\n"		// convert t3 color to luminance, store in r1.rgb
"sub_x4 r0, r0, r1\n"		// take both differences   (and keep oversaturating the colors)
"mul_x4 r0, r0, r0\n"		// square both differences (instead of abs())
"sub_sat    r0.rgb, 1-r0, r0.a\n"	// invert and add the 2 components:
"mul r0, r0, c4\n";

char Edge3[] =
"ps.1.1\n"
"def c1, 0.3f, 0.59f, 0.11f, 0.0f\n"    
"tex t0\n"
"tex t1\n"
"tex t2\n"
"tex t3\n"
"dp3    r0.rgba, t0, c1\n"		// convert t0 color to luminance, store in r0.a
"dp3    r1.rgba, t2, c1\n"		// convert t2 color to luminance, store in r1.a
"dp3    r0.rgb, t1, c1\n"		// convert t1 color to luminance, store in r0.rgb
"dp3    r1.rgb, t3, c1\n"		// convert t3 color to luminance, store in r1.rgb
"sub_x4 r0, r0, r1\n"		    // take both differences   (and keep oversaturating the colors)
"mul_x2 r0, r0, r0\n"		    // square both differences (instead of abs())
"sub_sat    r0, 1-r0, r0.a\n"	// invert and add the 2 components:
"mul_x2_sat r0, r0, r1\n";	    // and multiply with luminance for final result 

char Sharpen[] =
"ps.1.1\n"
"tex t0\n"          // this is the center sample
"tex t1\n"
"tex t2\n"
"tex t3\n"
"mov     r0, t0\n"
"sub     r0, r0,   t1\n"
"add     r0, r0,   t0\n"
"sub     r0, r0,   t2\n"
"add     r0, r0,   t0\n"
"sub     r0, r0,   t3\n"
"add_sat r0, r0,   t0\n"
"mov r0.a, c2.a\n";

char saturation[] =
"ps.1.1\n"
"def c1, 0.2125, 0.7154, 0.0721, 0.0\n"
"def c2, 1.0, 0.0, 0.0, 0.4\n"
"def c4, 0.8, 0.8, 0.8, 1.0\n"
"tex t0\n"
"dp3 r1, t0, c1\n"
"sub r0, t0, r1\n"
"mad_sat r0, r0, c2.wwww, r1\n"
"sub r0, r0, c3\n"
"mul r1, r0, c4\n"
"add r0, r0, r1\n"
"add r0, r0, c3\n"
"mov r0.a, c4.a\n";

char saturation2[] =
"ps.1.1\n"
"def c1, 0.2125, 0.7154, 0.0721, 0.0\n"
"def c2, 1.0, 0.0, 0.0, 0.4\n"
"def c4, 0.8, 0.8, 0.8, 1.0\n"
"tex t0\n"
"dp3 r1, t0, c1\n"
"sub r0, t0, r1\n"
"mad_sat r0, r0, c2.wwww, r1\n"
"sub r0, r0, c3\n"
"mul r1, r0, c4\n"
"add r0, r0, r1\n"
"add r0, r0, c3\n"
"mov r0.a, c4.a\n";


CFullScenePShader::CFullScenePShader(char *strFileName)
{
	memset(m_strName,0,sizeof(char) * 256);
	strcpy(m_strName,strFileName);
	/*
	char strTmp[256] = {0};

	sprintf(strTmp,"%s%s",SHADERFILEPATH,strFileName);*/
	if(strstr("Brightness",strFileName))
		CreatePixelShaderFromBuffer(Brightness);
	if(strstr("Black",strFileName))
		CreatePixelShaderFromBuffer(Black);
	if(strstr("Edges",strFileName))
		CreatePixelShaderFromBuffer(Edges);
	if(strstr("Negative",strFileName))
		CreatePixelShaderFromBuffer(Negative);
	if(strstr("Sepia",strFileName))
		CreatePixelShaderFromBuffer(Sepia);
	if(strstr("Edge2",strFileName))
		CreatePixelShaderFromBuffer(Edge2);
	if(strstr("Edge3",strFileName))
		CreatePixelShaderFromBuffer(Edge3);
	if(strstr("Sharpen",strFileName))
		CreatePixelShaderFromBuffer(Sharpen);
	if(strstr("saturation",strFileName))
		CreatePixelShaderFromBuffer(saturation);
	if(strstr("saturation2",strFileName))
		CreatePixelShaderFromBuffer(saturation);

}

CFullScenePShader::~CFullScenePShader()
{

}
void CFullScenePShader::Apply()
{
	CSceneManager::GetDevice()->SetPixelShader(m_dwPixelShader);
	SetupPixelShaderConstants(0);

}
void CFullScenePShader::Apply(int i)
{
	CSceneManager::GetDevice()->SetPixelShader(m_dwPixelShader);
	SetupPixelShaderConstants(1);

}
void CFullScenePShader::SetupPixelShaderConstants(int i)
{
	
	

	// For Bright,Contrast
	if(strstr(m_strName,"Brightness")) {
		float fDelta = 0.3f;
		CSceneManager::GetDevice()->SetPixelShaderConstant(0, &D3DXVECTOR4(fDelta,fDelta,fDelta, 1.0f),1);
	}
	/*
	if(strstr(m_strName,"Contrast.psh")) {	
		//float Delta = 0.2f;
		CSceneManager::GetDevice()->SetPixelShaderConstant(0, &D3DXVECTOR4(-CSceneManager::m_vecLifeColor.x, -CSceneManager::m_vecLifeColor.x, -CSceneManager::m_vecLifeColor.x, 1.0f),1);
	}*/
	if(strstr(m_strName,"Black")) {
		/*D3DXVECTOR3 vecColor(1.0f,1.0f,1.0f);
		
		vecColor.x = CSceneManager::m_vecLifeColor.x * 0.5f;
		vecColor.y = CSceneManager::m_vecLifeColor.y;
		vecColor.z = CSceneManager::m_vecLifeColor.z * 0.16f;


		if(vecColor.x > 1.0f) vecColor.x = 1.0f;
		if(vecColor.y > 1.0f) vecColor.y = 1.0f;
		if(vecColor.z > 1.0f) vecColor.z = 1.0f;

		if(vecColor.x < 0.0f) vecColor.x = 0.0f;
		if(vecColor.y < 0.0f) vecColor.y = 0.0f;
		if(vecColor.z < 0.0f) vecColor.z = 0.0f;
		*/
		D3DXVECTOR3 vecColor(0.5f,1.0f,0.16f);

		CSceneManager::GetDevice()->SetPixelShaderConstant(0, &D3DXVECTOR4(vecColor.x, vecColor.y, vecColor.z, 1.0f),1);
	}
/**/
	if(i == 1)	//Blur Image ¿ë RGB -> Sepia 
	{
		CSceneManager::GetDevice()->SetPixelShaderConstant(2, &D3DXVECTOR4(0.9f, 0.7f, 0.3f,0.4f),1);

	}
	else 
	{
		CSceneManager::GetDevice()->SetPixelShaderConstant(2, &D3DXVECTOR4(0.9f, 0.7f, 0.3f,1.0f),1);
	}
/*	if(strstr(m_strName,"Edge3.psh")) {
		CSceneManager::GetDevice()->SetPixelShaderConstant(1, &D3DXVECTOR4(0.3f, 0.59f, 0.11f, 0.0f ),1);
	}*/
	/*
	if(strstr(m_strName,"Levels")) {	
		D3DXVECTOR3 vecColor(1.0f,1.0f,1.0f);
		vecColor.x = CSceneManager::m_vecLifeColor.x * 0.5f;
		vecColor.y = CSceneManager::m_vecLifeColor.y;
		vecColor.z = CSceneManager::m_vecLifeColor.z * 0.16f;

		if(vecColor.x > 1.0f) vecColor.x = 1.0f;
		if(vecColor.y > 1.0f) vecColor.y = 1.0f;
		if(vecColor.z > 1.0f) vecColor.z = 1.0f;

		if(vecColor.x < 0.0f) vecColor.x = 0.0f;
		if(vecColor.y < 0.0f) vecColor.y = 0.0f;
		if(vecColor.z < 0.0f) vecColor.z = 0.0f;



		//float Delta = 0.2f;
		CSceneManager::GetDevice()->SetPixelShaderConstant(1, &D3DXVECTOR4(vecColor.y,0.0f,0.0f,0.0f),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(2, &D3DXVECTOR4(0.0f,vecColor.y,0.0f,0.0f),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(3, &D3DXVECTOR4(0.0f,0.0f,vecColor.y,0.0f),1);
	}*/

	if(strstr(m_strName,"saturation")) {
		
		CSceneManager::GetDevice()->SetPixelShaderConstant(1, &D3DXVECTOR4(0.2125f, 0.7154f, 0.0721f, 0.0f),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(2, &D3DXVECTOR4(1.0f, 0.5f, 0.8f, 0.4f),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(3, &D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f),1);
		if(i == 1)
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(0.8f,0.8f,0.8f,0.4f),1);
		else
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(0.8f,0.8f,0.8f,1.0f),1);
	
	}
	if(strstr(m_strName,"saturation2")) {
		
		CSceneManager::GetDevice()->SetPixelShaderConstant(1, &D3DXVECTOR4(0.2125f, 0.7154f, 0.0721f, 0.0f),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(2, &D3DXVECTOR4(1.0f, 0.5f, 0.8f, (CSceneManager::m_fLife * 3.0f/1.0f)),1);
		CSceneManager::GetDevice()->SetPixelShaderConstant(3, &D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f),1);
		if(i == 1)
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(0.0f,0.0f,0.0f,0.4f),1);
		else
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f),1);
	
	}
	if(strstr(m_strName,"Edge2")) {
		if(i == 0)
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(1.0f, 1.0f, 1.0f,1.0f),1);				
		else
			CSceneManager::GetDevice()->SetPixelShaderConstant(4, &D3DXVECTOR4(1.0f, 1.0f, 1.0f,0.4f),1);				
	}
}
