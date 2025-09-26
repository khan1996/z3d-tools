// InitValue.cpp: implementation of the CInitValue class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "InitValue.h"
#include "BaseDataDefine.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInitValue::CInitValue()
{
	m_nAdapter=m_nDevice=m_nMode=0;
	m_CharShadow=m_MapShadow=m_BuildShadow=0;
	m_TerrainDetail=m_CharDetail=m_ViewRange=0;
	m_WaterReflection=m_Lensflare=m_TreeDetail=0;
}

CInitValue::~CInitValue()
{

}

bool CInitValue::Save(HKEY &hReg)
{
	if(!hReg) return false;

	long lResult;
//	long lResult = RegDeleteValue(hReg, "InitValue");
//	if(lResult != ERROR_SUCCESS) return false;

	long lRegValue[16];
	lRegValue[0] = m_lScreenX;
	lRegValue[1] = m_lScreenY;
	lRegValue[2] = m_lScreenColorDepth;
	lRegValue[3] = m_nAdapter;
	lRegValue[4] = m_nDevice;
	lRegValue[5] = m_nMode;
	lRegValue[6] = m_CharShadow;
	lRegValue[7] = m_MapShadow;
	lRegValue[8] = m_BuildShadow;
	lRegValue[9] = m_TerrainDetail;
	lRegValue[10] = m_CharDetail;
	lRegValue[11] = m_ViewRange;
	lRegValue[12] = m_WaterReflection;
	lRegValue[13] = m_Lensflare;
	lRegValue[14] = m_TreeDetail;
	lRegValue[15] = m_MaxTextureSize;
	lResult = RegSetValueEx(hReg, "InitValue", 0, REG_BINARY, (LPBYTE)&lRegValue, sizeof(lRegValue));
	if(lResult == ERROR_SUCCESS) return true;

	return false;
/*	FILE *fp=fopen(INITVALUEFILE,"wb");
	fwrite(this,sizeof(*this),1,fp);
	fclose(fp);*/
}

bool CInitValue::Load(HKEY &hReg)
{
	if(!hReg) return false;

	long lRegValue[16];
	unsigned long dwReadLens = sizeof(lRegValue);
	long lResult = RegQueryValueEx(hReg, "InitValue", 0, NULL, (LPBYTE)&lRegValue, &dwReadLens);
	if(lResult != ERROR_SUCCESS) return false;
	m_lScreenX = lRegValue[0];
	m_lScreenY = lRegValue[1];
	m_lScreenColorDepth = lRegValue[2];
	m_nAdapter = lRegValue[3];
	m_nDevice = lRegValue[4];
	m_nMode = lRegValue[5];
	m_CharShadow = lRegValue[6];
	m_MapShadow = lRegValue[7];
	m_BuildShadow = lRegValue[8];
	m_TerrainDetail = lRegValue[9];
	m_CharDetail = lRegValue[10];
	m_ViewRange = lRegValue[11];
	m_WaterReflection = lRegValue[12];
	m_Lensflare = lRegValue[13];
	m_TreeDetail = lRegValue[14];
	m_MaxTextureSize = lRegValue[15];
	
	return true;
/*	FILE *fp=fopen(INITVALUEFILE,"rb");
	fread(this,sizeof(*this),1,fp);
	fclose(fp);*/
}

void CInitValue::Init()
{

}
