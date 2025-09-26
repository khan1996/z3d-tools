// RenderOption.h: interface for the CRenderOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDEROPTION_H__D9EA4304_097B_4CF3_96B3_C60B325DD9A0__INCLUDED_)
#define AFX_RENDEROPTION_H__D9EA4304_097B_4CF3_96B3_C60B325DD9A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRenderOption  
{
public:
	static bool Load(HKEY &hReg);
	static bool Save(HKEY &hReg);
	static int m_GrassRendering;
	static int m_GrassAnimationUseVertexShader;
	static int m_GrassRenderRange;
	static int m_RangeGrassRender;
	static int m_FullSceneGlare;
	static int m_FullSceneAnti;
	static int m_FullSceneSpecularGlare;
	static int m_WaterBumpEnvRendering;
	static int m_TerrainMultiDetalTexture;
	static int m_TerrainRange;
	static int m_BuildingLightmap;
	static int m_TreeAnimation;
	static int m_CharacterPerPixelLighting;
	static int m_CharacterPerPixelSpecularLighting;
	static int m_CharacterSelfShadow;
	static int m_CharacterProjectShadowTerrain;
	static int m_CharacterProjectShadowBuilding;
	static int m_DepthOfField;
	static int m_TextureMethod;	
	static int m_ObjectLOD;
	static int m_ObjectProjectShadow;
	static int m_TreeProjectShadow;
	static int m_ScreenFilter;
	static int m_bWeather;
	static int m_bFullSceneEffect;


	static int m_BSPDiffusePerPixel;
	static int m_BSPSpecularPerPixel;

	static int m_AllObjectBump;

	static char m_strBaseGraphicsDataPath[256];
	
	CRenderOption();
	virtual ~CRenderOption();
	
	class ZoneInfo									// � Info Data Load
	{
	public:
		bool m_bZoneInfo;
		int m_iZoneInfo[30];
		float m_fZoneInfo[30];
		ZoneInfo()
		{
			m_bZoneInfo = false;
			
		}
		
	};

	static bool LoadZoneInfo(ZoneInfo *);		// � info File Load
};

#endif // !defined(AFX_RENDEROPTION_H__D9EA4304_097B_4CF3_96B3_C60B325DD9A0__INCLUDED_)
