#pragma once
#include "./CrossMHeader.h"
#include "./resourceobj.h"
#include "./dds.h"

#include <d3d8.h>

#ifndef ReleasePpo
	#define ReleasePpo(ppo) \
		if (*(ppo) != NULL) \
		{ \
			(*(ppo))->Release(); \
			*(ppo) = NULL; \
		} \
		else (VOID)0
#endif

namespace CROSSM {
class CNTexture :
	public CResourceObj
{
public:
	CNTexture(void);
	virtual ~CNTexture(void);
	virtual bool Load();
	virtual bool Unload();
	virtual bool PostLoad();
	
	LPDIRECT3DBASETEXTURE8 GetTexture();
	HRESULT LoadAllMipSurfaces(LPDIRECT3DBASETEXTURE8 ptex, long numMips);

	static void SetPath(char *strPath){strcpy(m_strPath,strPath);};
	static const char*	GetPath(){ return m_strPath; };

	static void Init(LPDIRECT3DDEVICE8	lpDevice);
	static void Close();

	static void NullTextureMode(bool bNullTexture);


protected:
	static char m_strPath[256];
	bool m_bCreateEmpty;
	DWORD m_stage;	
	
	DWORD m_dwValue[2];	// 0 : NumMip, 1 : Depth
	D3DFORMAT	m_fmt;
	bool		m_bInterface;
	
public:
	static LPDIRECT3DDEVICE8 ms_pd3dDevice;

	static int m_SkipMipLevel;

//	char m_strName[TEXTURENAMEBUFFER];	
	long m_dwWidth,m_dwHeight;	


private:
	LPDIRECT3DBASETEXTURE8 m_pddTexture;	

	static IDirect3DTexture8* ms_pNullTexture;
	static bool ms_bNullTextureMode;
};

}