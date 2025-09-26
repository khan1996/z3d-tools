// ConvertTexture.h: interface for the CConvertTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTTEXTURE_H__CA884184_2931_4713_89E5_356F999EF196__INCLUDED_)
#define AFX_CONVERTTEXTURE_H__CA884184_2931_4713_89E5_356F999EF196__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Texture.h"

class CConvertTexture : public CTexture  
{	
public:
	void SaveDDS(char *strFilename,int LowerSave,int MaxTextureSize);
	LPDIRECT3DTEXTURE8 m_pddNewTexture;
	void SaveDDS(char *strFilename);
	void GenerateMipMaps(bool bGenMipmap);
	void Compress(D3DFORMAT fmtTo);

	int GetMipmapCount(){return m_pddTexture->GetLevelCount();};
	CConvertTexture();
	virtual ~CConvertTexture();

private:
};

#endif // !defined(AFX_CONVERTTEXTURE_H__CA884184_2931_4713_89E5_356F999EF196__INCLUDED_)
