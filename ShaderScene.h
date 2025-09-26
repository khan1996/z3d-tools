#ifndef __CSHADERSCENE_H__
#define __CSHADERSCENE_H__

#pragma once

#include "ShaderConstants.h"				// Shader 상수 define header
#include <windows.h>
#include <d3dx8.h>
#include <d3d8.h>
/* ***************************************************

  Vertex Shader, Pixel Shader, Shader
  Base Class.
*************************************************** */
enum ENABLEFLAG {
	ZBUFFER,
	ZWRITE,
	ALPHABLEND,
	CULLMODE,
	ENABLEFLAGNUM,
};
class CVertexShader {

protected:
	void						CreateVertexShader(char* fileName, DWORD* vertexShaderDeclaration);
	DWORD						m_dwVertexShader;

public:
	
	CVertexShader() : m_dwVertexShader(0){}
	virtual ~CVertexShader();

	virtual void				Apply() = 0;
	virtual int					GetVertexStride() const = 0;
	virtual DWORD				GetVertexFormat() const = 0;
	virtual void				SetupVertexShaderConstants() = 0;

};

class CPixelShader  
{
protected:
	
	void						CreatePixelShaderFromFile(char* fileName);
	void						CreatePixelShaderFromBuffer(char *shaderStr);

	DWORD						m_dwPixelShader;

public:
	CPixelShader() : m_dwPixelShader(0){}
	virtual ~CPixelShader();
	virtual void				Apply() = 0;
	virtual void				SetupPixelShaderConstants() = 0;
};


class CShaderScene {						// Shader 한 단위의 Base Class
protected:
	char						m_strName[256];

	LPDIRECT3DDEVICE8			m_pDevice;

	DWORD						m_dwSrcBlend;
	DWORD						m_dwDestBlend;
	DWORD						m_dwCullFace;

	bool						m_bEnable[ENABLEFLAGNUM];

	DWORD						m_dwOldSrcBlend;
	DWORD						m_dwOldDestBlend;
	DWORD						m_dwOldCullFace;
	
	bool						m_bOldEnable[ENABLEFLAGNUM];
	
	bool						m_bGetState;
	bool						m_bSetState;

	D3DXVECTOR4					m_vecLightDir;
	D3DXVECTOR4					m_vecLightPos;


public:
	
	virtual void				Apply() = 0;
	virtual void				UnApply() = 0;

	CShaderScene();
	virtual ~CShaderScene();
	
	void						SetState(int iFlag,bool bEnable,DWORD dwValue1 = 0x00000000,DWORD dwValue2 = 0x00000000);
	void						GetState();

	void						ApplyState();
	void						ApplyOldState();
	
	char *						GetShaderName() { return m_strName;}

	static HRESULT				PreprocessAndAssembleShaderFromBuffer(		// Ascii로 된 Shader를 Bin으로 컴파일 
																	  LPCVOID pSrcData,
																	  UINT SrcDataLen,
																	  LPD3DXBUFFER* ppConstants,
																	  LPD3DXBUFFER* ppCode);
	
	static HRESULT				PreprocessAndAssembleShaderFromFile2(		// 파일에서 Shader를 Bin으로 컴파일
																		LPCSTR          szFile,
																		LPD3DXBUFFER*   ppConstants, 
																		LPD3DXBUFFER*   ppCode);
	void						Create(char *strShader);
	
	void						SetLightDirection(D3DXVECTOR3 vecDir);
	void						SetLightPos(D3DXVECTOR3 vecPos);


	
};




#endif
