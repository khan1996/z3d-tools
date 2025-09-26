// InitValue.h: interface for the CInitValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITVALUE_H__902DD191_CD4A_4EB3_BEFE_33EBF57D1257__INCLUDED_)
#define AFX_INITVALUE_H__902DD191_CD4A_4EB3_BEFE_33EBF57D1257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitValue  
{
public:
	void Init();
	bool Load(HKEY &hReg);
	bool Save(HKEY &hReg);

	long m_lScreenX,m_lScreenY,m_lScreenColorDepth;	
	long m_nAdapter,m_nDevice,m_nMode;
	long m_CharShadow,m_MapShadow,m_BuildShadow;
	long m_TerrainDetail,m_CharDetail,m_ViewRange;
	long m_WaterReflection,m_Lensflare,m_TreeDetail;
	long m_MaxTextureSize;
	CInitValue();
	virtual ~CInitValue();	
};

#endif // !defined(AFX_INITVALUE_H__902DD191_CD4A_4EB3_BEFE_33EBF57D1257__INCLUDED_)
