// MapStorage.cpp: implementation of the CMapStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "MapStorage.h"
#include "RenderOption.h"
#include "AmbienceManager.h"
#include "BgmManager.h"
#include "SceneManager.h"
#include "SoundMgr.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
std::vector<CSectorDungeonMap*> CMapStorage::m_DundeonMap;
std::vector<CSectorHeightMap*> CMapStorage::m_HeightMap;
std::vector<CSectorMeshMap*> CMapStorage::m_MeshMap;
std::vector<CSectorPlantMap*> CMapStorage::m_PlantMap;
std::vector<CSectorWideMap*> CMapStorage::m_WideMap;
std::vector<CSectorHouseMap*> CMapStorage::m_HouseMap;
std::vector<CSectorWaterMap*> CMapStorage::m_WaterMap;
std::vector<CSectorFallMap*> CMapStorage::m_FallMap;
std::vector<CInHouseObjectMap*> CMapStorage::m_InHouseMap;
std::vector<CSectorMustDivideVertexMap*> CMapStorage::m_MustDivideVertexMap;
std::vector<CSectorEffectMap*> CMapStorage::m_EffectMap;

long CMapStorage::m_TotalHouseSceneCount;
long CMapStorage::m_TotalInHouseLightSceneCount;
long CMapStorage::m_TotalInHouseObjectSceneCount;
long CMapStorage::m_TotalObjectSceneCount;
*/



CMapStorage::CMapStorage()
{
	m_TotalHouseSceneCount=0;
	m_TotalInHouseLightSceneCount=0;
	m_TotalInHouseObjectSceneCount=0;
	m_TotalObjectSceneCount=0;
	m_TotalLightCount = 0;

	m_LoadMode=0;
	
	memset(m_DundeonMapTable,0,sizeof(CSectorDungeonMap *) * MAXSECTOR);
	memset(m_MeshMapTable,0,sizeof(CSectorMeshMap *) * MAXSECTOR);
	memset(m_PlantMapTable,0,sizeof(CSectorPlantMap *) * MAXSECTOR);
	memset(m_HouseMapTable,0,sizeof(CSectorHouseMap *) * MAXSECTOR);
	memset(m_WaterMapTable,0,sizeof(CSectorWaterMap *) * MAXSECTOR);
	memset(m_FallMapTable,0,sizeof(CSectorFallMap *) * MAXSECTOR);
	memset(m_InHouseMapTable,0,sizeof(CInHouseObjectMap *) * MAXSECTOR);
	memset(m_MustDivideVertexMapTable,0,sizeof(CSectorMustDivideVertexMap *) * MAXSECTOR);
	memset(m_EffectMapTable,0,sizeof(CSectorEffectMap *) * MAXSECTOR);
	memset(m_LightMapTable,0,sizeof(CSectorLightMap *) * MAXSECTOR);
	memset(m_LandscapeEffectMapTable,0,sizeof(CSectorLandscapeEffectMap *) * MAXSECTOR);
	memset(m_HeightMapTable,0,sizeof(CSectorHeightMap *) * MAXSECTOR);
	memset(m_WideMapTable,0,sizeof(CSectorWideMap *) * MAXSECTOR);


	
}

CMapStorage::~CMapStorage()
{
	DeleteAllObject();
}

void CMapStorage::Load(char *strFilename)
{	
	//map search Table 초기화
	memset(m_DundeonMapTable,0,sizeof(CSectorDungeonMap *) * MAXSECTOR);
	memset(m_MeshMapTable,0,sizeof(CSectorMeshMap *) * MAXSECTOR);
	memset(m_PlantMapTable,0,sizeof(CSectorPlantMap *) * MAXSECTOR);
	memset(m_HouseMapTable,0,sizeof(CSectorHouseMap *) * MAXSECTOR);
	memset(m_WaterMapTable,0,sizeof(CSectorWaterMap *) * MAXSECTOR);
	memset(m_FallMapTable,0,sizeof(CSectorFallMap *) * MAXSECTOR);
	memset(m_InHouseMapTable,0,sizeof(CInHouseObjectMap *) * MAXSECTOR);
	memset(m_MustDivideVertexMapTable,0,sizeof(CSectorMustDivideVertexMap *) * MAXSECTOR);
	memset(m_EffectMapTable,0,sizeof(CSectorEffectMap *) * MAXSECTOR);
	memset(m_LightMapTable,0,sizeof(CSectorLightMap *) * MAXSECTOR);
	memset(m_LandscapeEffectMapTable,0,sizeof(CSectorLandscapeEffectMap *) * MAXSECTOR);
	memset(m_HeightMapTable,0,sizeof(CSectorHeightMap *) * MAXSECTOR);
	memset(m_WideMapTable,0,sizeof(CSectorWideMap *) * MAXSECTOR);



	FILE *fp=fopen(strFilename,"rb");		
	
	if(fp==NULL)
		return;

	strcpy(m_strMapFilename,strFilename);

	DeleteAllData();

	char strRead[MAX_NAMEBUFFER];
	fread(strRead,sizeof(char)*MAX_NAMEBUFFER,1,fp);	

	fread(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fread(&m_TotalObjectSceneCount,sizeof(long),1,fp);

	// Height-Value Loading
	int cSavedHeightData=0;
	fread(&cSavedHeightData,sizeof(int),1,fp);	
	if(m_LoadMode==0)
	{
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			fread(&AddNode->m_IndexX,sizeof(int),1,fp);
			fread(&AddNode->m_IndexY,sizeof(int),1,fp);
			
			AddNode->m_pHeightData=new float[SECTORSX*SECTORSY];
			fread(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
			
			m_HeightMap.push_back(AddNode);
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY; 
			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				delete AddNode;
				continue;
			}
			
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}
	else
	{
		float pHeightData[SECTORSX*SECTORSY];
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			fread(&AddNode->m_IndexX,sizeof(int),1,fp);
			fread(&AddNode->m_IndexY,sizeof(int),1,fp);
			AddNode->m_FilePos=ftell(fp);
			fread(&pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);			
			m_HeightMap.push_back(AddNode);		
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				delete AddNode;
				continue;
			}

		
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}
	//Water loading
	int cSavedWaterData=0;
	fread(&cSavedWaterData,sizeof(int),1,fp);
	
	for(int cWaterData=0;cWaterData<cSavedWaterData;cWaterData++)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fWaterPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterHeight,sizeof(float),1,fp);
		fread(&AddNode->m_bWaterRelection,sizeof(bool),1,fp);
		fread(&AddNode->m_WaterColor,sizeof(color),1,fp);
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_WaterMap.push_back(AddNode);
		m_WaterMapTable[iNode] = AddNode;


	}

	//Widemap loading

	int cSavedWideData=0;
	fread(&cSavedWideData,sizeof(int),1,fp);

	for(int cWideData=0;cWideData<cSavedWideData;cWideData++)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(AddNode->m_strWidemapName[0],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		strcpy(AddNode->m_strDetailName[0],"detail.dds");
		

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_WideMap.push_back(AddNode);
		m_WideMapTable[iNode] = AddNode;

	}

	//Fallmap loading

	int cSavedFallData=0;
	fread(&cSavedFallData,sizeof(int),1,fp);

	for(int cFallData=0;cFallData<cSavedFallData;cFallData++)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fFallPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fFallPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fFallRot,sizeof(float),1,fp);
		
		int cSavedFallList;
		fread(&cSavedFallList,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallRightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			fread(&ReadFallColor,sizeof(color),1,fp);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}
	
		m_FallMap.push_back(AddNode);
		m_FallMapTable[iNode] = AddNode;

	}	

	// HouseMap loading

	int cSavedHouseData=0;
	fread(&cSavedHouseData,sizeof(int),1,fp);

	int IndexX, IndexY;
	
	for(int cHouseData=0;cHouseData<cSavedHouseData;cHouseData++)
	{
		fread(&IndexX,sizeof(int),1,fp);
		fread(&IndexY,sizeof(int),1,fp);

		int iNode = IndexX * 11 + IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			continue;
		}

		CSectorHouseMap *AddNode=new CSectorHouseMap();
		int cSavedHouseList=0;		

		AddNode->m_IndexX = IndexX;
		AddNode->m_IndexY = IndexY;

		fread(&cSavedHouseList,sizeof(int),1,fp);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			fread(&ReadHouseTM,sizeof(matrix),1,fp);
			AddNode->m_TM.push_back(ReadHouseTM);
			fread(&ReadHouseID,sizeof(int),1,fp);
			AddNode->m_HouseSceneID.push_back(ReadHouseID);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			ZeroMemory(ReadHouseName, MAX_NAMEBUFFER);
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			ZeroMemory(ReadHouseName, MAX_NAMEBUFFER);
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			ZeroMemory(ReadHouseName, MAX_NAMEBUFFER);
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strInName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			strcpy(ReadHouseName,"");
			AddNode->m_strBspName.push_back(ReadHouseName);
		}
		
		m_HouseMap.push_back(AddNode);
		m_HouseMapTable[iNode] = AddNode;

	}

	int cSavedInHouseData=0;
	fread(&cSavedInHouseData,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		fread(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);		

		strcpy(AddNode->m_strOutName,strlwr(AddNode->m_strOutName));
		strcpy(AddNode->m_strMedName,strlwr(AddNode->m_strMedName));
		strcpy(AddNode->m_strInName,strlwr(AddNode->m_strInName));
		strcpy(AddNode->m_strBspName,"");

		int cSavedObject,cSavedLight;
		fread(&cSavedObject,sizeof(int),1,fp);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			fread(&ReadObjectID,sizeof(long),1,fp);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			fread(&ReadObjectTM,sizeof(matrix),1,fp);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			AddNode->m_isAlpha.push_back(true);
			fread(&ReadLight,sizeof(bool),1,fp);
			AddNode->m_isLight.push_back(true);
		}
		fread(&cSavedLight,sizeof(int),1,fp);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			fread(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			fread(&ReadLightID,sizeof(long),1,fp);
			AddNode->m_LightID.push_back(ReadLightID);
			fread(&ReadLightTM,sizeof(matrix),1,fp);
			AddNode->m_matLightList.push_back(ReadLightTM);
			fread(&ReadLightRange,sizeof(float),1,fp);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			fread(&ReadLightColor,sizeof(color),1,fp);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}		
		m_InHouseMap.push_back(AddNode);
	}
	
	// PlantMap loading
	int cSavedPlantData=0;
	fread(&cSavedPlantData,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<cSavedPlantData;cPlantData++)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);		

		int cSavedSubPlant;
		int ReadKind;
		unsigned char ReadPosX,ReadPosZ;
		//vector3 ReadPos;
		fread(&cSavedSubPlant,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			fread(&ReadKind,sizeof(unsigned char),1,fp);
			fread(&ReadPosX,sizeof(unsigned char),1,fp);
			fread(&ReadPosZ,sizeof(unsigned char),1,fp);
			//fread(&ReadPos,sizeof(vector3),1,fp);
			bool isAlready=false;
			for(int cNowPlant=0;cNowPlant<(int)AddNode->m_PlantKind.size();cNowPlant++)
			{
				if( AddNode->m_PlantPosX[cNowPlant]==ReadPosX &&
					AddNode->m_PlantPosZ[cNowPlant]==ReadPosZ)					
				{
					AddNode->m_PlantKind[cNowPlant]=ReadKind;
					AddNode->m_PlantPosX[cNowPlant]=ReadPosX;
					AddNode->m_PlantPosZ[cNowPlant]=ReadPosZ;
					isAlready=true;
				}
			}
			if(isAlready==false)
			{
				AddNode->m_PlantKind.push_back(ReadKind);
				AddNode->m_PlantPosX.push_back(ReadPosX);
				AddNode->m_PlantPosZ.push_back(ReadPosZ);
			}
			//AddNode->m_vecPlantPos.push_back(ReadPos);
		}

		
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}
		m_PlantMap.push_back(AddNode);
		m_PlantMapTable[iNode] = AddNode;
	}
	
	// MeshMap loading
	int cSavedMeshData=0;
	fread(&cSavedMeshData,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<cSavedMeshData;cMeshData++)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);

		int cSavedObject=0;
		fread(&cSavedObject,sizeof(int),1,fp);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			fread(&ReadObjectSceneID,sizeof(long),1,fp);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fread(&ReadMatrixTM,sizeof(matrix),1,fp);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			fread(&ReadLight,sizeof(bool),1,fp);

			AddNode->m_ObjectSceneID.push_back(ReadObjectSceneID);
			AddNode->m_strObjectName.push_back(ReadObjectName);
			AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(true);
			AddNode->m_isLight.push_back(true);
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		
		m_MeshMap.push_back(AddNode);
		m_MeshMapTable[iNode] = AddNode;
	}
	fclose(fp);
	BugFix_Tree();
	//BugFix_Hangul2English();	
}

void CMapStorage::Save(char *strFilename)
{
	FILE *fp=fopen(strFilename,"wb");
	if(fp==NULL)
		return;
	char strWrite[MAX_NAMEBUFFER];
	sprintf(strWrite,"ZallA-3D Scene Data File Build#1");
	fwrite(strWrite,sizeof(char)*MAX_NAMEBUFFER,1,fp);

	fwrite(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalObjectSceneCount,sizeof(long),1,fp);	

	// Height-Value Saving 
	int iCnt = m_HeightMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cHeightData=0;cHeightData<iCnt;cHeightData++)
	{		
		fwrite(&m_HeightMap[cHeightData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_HeightMap[cHeightData]->m_IndexY,sizeof(int),1,fp);
		fwrite(m_HeightMap[cHeightData]->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
	}

	//Water Saving
	iCnt = m_WaterMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cWaterData=0;cWaterData<iCnt;cWaterData++)
	{
		fwrite(&m_WaterMap[cWaterData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_IndexY,sizeof(int),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterPosX,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterPosY,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeX,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeY,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterHeight,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_bWaterRelection,sizeof(bool),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_WaterColor,sizeof(color),1,fp);
	}

	//Widemap saving
	iCnt = m_WideMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cWideData=0;cWideData<iCnt;cWideData++)
	{
		fwrite(&m_WideMap[cWideData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_WideMap[cWideData]->m_IndexY,sizeof(int),1,fp);
		fwrite(m_WideMap[cWideData]->m_strWidemapName,sizeof(char)*MAX_NAMEBUFFER,1,fp);		
	}

	//Fallmap saving
	iCnt = m_FallMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cFallData=0;cFallData<iCnt;cFallData++)
	{
		fwrite(&m_FallMap[cFallData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_FallMap[cFallData]->m_IndexY,sizeof(int),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallPosX,sizeof(float),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallPosY,sizeof(float),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallRot,sizeof(float),1,fp);
		int iSize = m_FallMap[cFallData]->m_FallHeightList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<iSize;cFallList++)
		{
			fwrite(&m_FallMap[cFallData]->m_FallHeightList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallLeftList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallRightList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallAddXList[cFallList],sizeof(float),1,fp);			
			fwrite(&m_FallMap[cFallData]->m_FallColor[cFallList],sizeof(color),1,fp);
		}
	}

	//HouseMap saving 
	iCnt = m_HouseMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cHouseData=0;cHouseData<iCnt;cHouseData++)
	{
		fwrite(&m_HouseMap[cHouseData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_HouseMap[cHouseData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_HouseMap[cHouseData]->m_TM.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cHouseList=0;cHouseList<iSize;cHouseList++)
		{
			fwrite(&m_HouseMap[cHouseData]->m_TM[cHouseList],sizeof(matrix),1,fp);
			fwrite(&m_HouseMap[cHouseData]->m_HouseSceneID[cHouseList],sizeof(int),1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strOutName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strMedName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strInName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		}
	}
	//InHouseObjectMap saving
	iCnt = m_InHouseMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<iCnt;cInHouseData++)
	{		
		fwrite(m_InHouseMap[cInHouseData]->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		int iSize = m_InHouseMap[cInHouseData]->m_strObjectNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cObject=0;cObject<iSize;cObject++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strObjectNameList[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_ObjectID[cObject],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matObjectList[cObject],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isAlpha[cObject],sizeof(bool),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isLight[cObject],sizeof(bool),1,fp);
		}

		iSize = m_InHouseMap[cInHouseData]->m_strLightNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cLight=0;cLight<iSize;cLight++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strLightNameList[cLight],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightID[cLight],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matLightList[cLight],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_fLightRange[cLight],sizeof(float),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightColor[cLight],sizeof(color),1,fp);
		}		
	}
	// PlantMap saving
	iCnt = m_PlantMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<iCnt;cPlantData++)
	{
		fwrite(&m_PlantMap[cPlantData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_PlantMap[cPlantData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_PlantMap[cPlantData]->m_PlantKind.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<iSize;cSubPlant++)
		{
			fwrite(&m_PlantMap[cPlantData]->m_PlantKind[cSubPlant],sizeof(unsigned char),1,fp);
			//fwrite(&m_PlantMap[cPlantData]->m_vecPlantPos[cSubPlant],sizeof(vector3),1,fp);
			fwrite(&m_PlantMap[cPlantData]->m_PlantPosX[cSubPlant],sizeof(unsigned char),1,fp);
			fwrite(&m_PlantMap[cPlantData]->m_PlantPosZ[cSubPlant],sizeof(unsigned char),1,fp);
		}
	}
	// MeshMap saving
	iCnt = m_MeshMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<iCnt;cMeshData++)
	{
		fwrite(&m_MeshMap[cMeshData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_MeshMap[cMeshData]->m_IndexY,sizeof(int),1,fp);
		int iSize = m_MeshMap[cMeshData]->m_ObjectSceneID.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cObject=0;cObject<iSize;cObject++)
		{
			fwrite(&m_MeshMap[cMeshData]->m_ObjectSceneID[cObject],sizeof(long),1,fp);
			fwrite(m_MeshMap[cMeshData]->m_strObjectName[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_TM[cObject],sizeof(matrix),1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_isAlpha[cObject],sizeof(bool),1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_isLight[cObject],sizeof(bool),1,fp);
		}
	}
	fclose(fp);
}

CSectorDungeonMap* CMapStorage::FindDungeonMap(float fFindx, float fFindy)
{
	return NULL;
}

CSectorHeightMap* CMapStorage::FindHeightMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	/*int iNode = indexx * 11 + indexy; 
	
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_HeightMapTable[iNode])
	{
		if(m_HeightMapTable[iNode]->m_FilePos!=0)
		{
			FILE *fp=fopen(m_strMapFilename,"rb");
			fseek(fp,m_HeightMapTable[iNode]->m_FilePos,SEEK_CUR);
			m_HeightMapTable[iNode]->m_pHeightData=new float[SECTORSX*SECTORSY];
			fread(m_HeightMapTable[iNode]->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
			fclose(fp);
		}

		return m_HeightMapTable[iNode];
	}*/

	for(int cStorage=0;cStorage<(int)m_HeightMap.size();cStorage++)
	{
		if( m_HeightMap[cStorage]->m_IndexX==indexx &&
			m_HeightMap[cStorage]->m_IndexY==indexy)
		{
			if(m_HeightMap[cStorage]->m_FilePos!=0)
			{
				FILE *fp=fopen(m_strMapFilename,"rb");
				fseek(fp,m_HeightMap[cStorage]->m_FilePos,SEEK_CUR);
				m_HeightMap[cStorage]->m_pHeightData=new float[SECTORSX*SECTORSY];
				fread(m_HeightMap[cStorage]->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
				fclose(fp);
			}
			return m_HeightMap[cStorage];
		}
	}
	return NULL;
}
CSectorMeshMap* CMapStorage::FindMeshMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_MeshMapTable[iNode])
	{
		return m_MeshMapTable[iNode];
	}
	/*
	for(int cStorage=0;cStorage<m_MeshMap.num;cStorage++)
	{
		if( m_MeshMap[cStorage]->m_IndexX==indexx &&
			m_MeshMap[cStorage]->m_IndexY==indexy)
		{
			return m_MeshMap[cStorage];
		}
	}*/
	return NULL;
}
CSectorPlantMap* CMapStorage::FindPlantMap(float fFindx, float fFindy)
{	
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_PlantMapTable[iNode])
	{
		return m_PlantMapTable[iNode];
	}
	/*
	for(int cStorage=0;cStorage<m_PlantMap.num;cStorage++)
	{
		if( m_PlantMap[cStorage]->m_IndexX==indexx &&
			m_PlantMap[cStorage]->m_IndexY==indexy)
		{
			return m_PlantMap[cStorage];
		}
	}*/
	return NULL;
}
CSectorWideMap* CMapStorage::FindWideMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_WideMapTable[iNode])
	{
		return m_WideMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_WideMap.num;cStorage++)
	{
		if( m_WideMap[cStorage]->m_IndexX==indexx &&
			m_WideMap[cStorage]->m_IndexY==indexy)
		{
			return m_WideMap[cStorage];
		}
	}*/

	return NULL;
}

CSectorHouseMap* CMapStorage::FindHouseMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 

	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_HouseMapTable[iNode])
	{
		return m_HouseMapTable[iNode];
	}
	/*
	for(int cStorage=0;cStorage<m_HouseMap.num;cStorage++)
	{
		if( m_HouseMap[cStorage]->m_IndexX==indexx &&
			m_HouseMap[cStorage]->m_IndexY==indexy)
		{
			return m_HouseMap[cStorage];
		}
	}*/
	return NULL;
}

void CMapStorage::AddHouseMap(float fAddx, float fAddy, float fAddz,char *strOutName,char *strMedName,char *strInName,char *strBspName)
{
	if(strcmp(strOutName,"")==0 && strcmp(strBspName,"")==0 )
		return;

	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);		
	char *strAddInName=new char[MAX_NAMEBUFFER];
	char *strAddMedName=new char[MAX_NAMEBUFFER];
	char *strAddOutName=new char[MAX_NAMEBUFFER];
	char *strAddBspName=new char[MAX_NAMEBUFFER];
	strcpy(strAddInName,strInName);
	strcpy(strAddMedName,strMedName);
	strcpy(strAddOutName,strOutName);
	strcpy(strAddBspName,strBspName);

	matrix tm;
	tm.Translation(vector3(fAddx,fAddy,fAddz));	

	for(int i=0;i<(int)m_HouseMap.size();i++)
	{
		if( m_HouseMap[i]->m_IndexX==indexx &&
			m_HouseMap[i]->m_IndexY==indexy)
		{				
			m_HouseMap[i]->m_strInName.push_back(strAddInName);
			m_HouseMap[i]->m_strMedName.push_back(strAddMedName);
			m_HouseMap[i]->m_strOutName.push_back(strAddOutName);						
			m_HouseMap[i]->m_strBspName.push_back(strAddBspName);
			m_HouseMap[i]->m_TM.push_back(tm);
			m_HouseMap[i]->m_HouseSceneID.push_back(m_TotalHouseSceneCount++);
			return;
		}
	}

	int iNode = indexx * 11 + indexy;
	if(iNode < 0 || iNode >= MAXSECTOR)
	{
		//MessageBox(NULL,"오류","CMapStorage",MB_OK);
		return;
	}

	CSectorHouseMap *AddSectorHouseMap=new CSectorHouseMap;
	AddSectorHouseMap->m_IndexX=indexx;
	AddSectorHouseMap->m_IndexY=indexy;
	AddSectorHouseMap->m_strInName.push_back(strAddInName);
	AddSectorHouseMap->m_strMedName.push_back(strAddMedName);
	AddSectorHouseMap->m_strOutName.push_back(strAddOutName);
	AddSectorHouseMap->m_strBspName.push_back(strAddBspName);

	AddSectorHouseMap->m_TM.push_back(tm);
	AddSectorHouseMap->m_HouseSceneID.push_back(m_TotalHouseSceneCount++);
	m_HouseMap.push_back(AddSectorHouseMap);
		
	m_HouseMapTable[iNode] = AddSectorHouseMap;
}
void CMapStorage::DeleteHouseMap(float fDelx,float fDely,float fDelz,char *filename)
{
	int indexx=(int)(fDelx/SECTORSIZE);
	int indexy=(int)(fDelz/SECTORSIZE);			
	
	for(int i=0;i<(int)m_HouseMap.size();i++)
	{
		if( m_HouseMap[i]->m_IndexX==indexx && m_HouseMap[i]->m_IndexY==indexy)
		{	
			std::vector<int> vecDel;

			for(int cObject=0;cObject<(int)m_HouseMap[i]->m_TM.size();cObject++)
			{
				// edith erase 로직에 문제있을듯.
				if(!strcmp(m_HouseMap[i]->m_strOutName[cObject],filename))
				{
					if(fabs(m_HouseMap[i]->m_TM[cObject]._41 - fDelx) < 1.0f &&
						fabs(m_HouseMap[i]->m_TM[cObject]._42 - fDely) < 1.0f &&
						fabs(m_HouseMap[i]->m_TM[cObject]._43 - fDelz) < 1.0f)
					{
						vecDel.insert(vecDel.begin(), cObject);
					}
				}
			}

			for(int a = 0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				m_HouseMap[i]->m_TM.erase(m_HouseMap[i]->m_TM.begin()+k);

				char *delNode;
				delNode=m_HouseMap[i]->m_strOutName[k];
				m_HouseMap[i]->m_strOutName.erase(m_HouseMap[i]->m_strOutName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				delNode=m_HouseMap[i]->m_strMedName[k];
				m_HouseMap[i]->m_strMedName.erase(m_HouseMap[i]->m_strMedName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				delNode=m_HouseMap[i]->m_strInName[k];
				m_HouseMap[i]->m_strInName.erase(m_HouseMap[i]->m_strInName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				
				delNode=m_HouseMap[i]->m_strBspName[k];
				m_HouseMap[i]->m_strBspName.erase(m_HouseMap[i]->m_strBspName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
			}

			if(m_HouseMap[i]->m_TM.size()==0)
			{
				int iNode = m_HouseMap[i]->m_IndexX * 11 + m_HouseMap[i]->m_IndexY;
				if(iNode < 0 || iNode >= MAXSECTOR)
				{
					//MessageBox(NULL,"오류","CMapStorage",MB_OK);
					return;
				}

				if(m_HouseMap[i]) {
					delete m_HouseMap[i];
					m_HouseMap[i] = NULL;
				}
				m_HouseMap.erase(m_HouseMap.begin()+i);
				m_HouseMapTable[iNode] = NULL;
			}
			return;
		}
	}
}
void CMapStorage::DelHouseMap(float fDelx,float fDely,float fDelz,long HouseSceneID)
{
	int indexx=(int)(fDelx/SECTORSIZE);
	int indexy=(int)(fDelz/SECTORSIZE);			
	
	for(int i=0;i<(int)m_HouseMap.size();i++)
	{
		if( m_HouseMap[i]->m_IndexX==indexx &&
			m_HouseMap[i]->m_IndexY==indexy)
		{	
			std::vector<int> vecDel;

			for(int cObject=0;cObject<(int)m_HouseMap[i]->m_TM.size();cObject++)
			{
				// edith erase 로직에 문제있을듯.
				if(m_HouseMap[i]->m_HouseSceneID[cObject]==HouseSceneID)
				{
					vecDel.insert(vecDel.begin(), cObject);
				}
			}

			for(int a = 0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				m_HouseMap[i]->m_TM.erase(m_HouseMap[i]->m_TM.begin()+k);
				char *delNode;
				delNode=m_HouseMap[i]->m_strOutName[k];
				m_HouseMap[i]->m_strOutName.erase(m_HouseMap[i]->m_strOutName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				delNode=m_HouseMap[i]->m_strMedName[k];
				m_HouseMap[i]->m_strMedName.erase(m_HouseMap[i]->m_strMedName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				delNode=m_HouseMap[i]->m_strInName[k];
				m_HouseMap[i]->m_strInName.erase(m_HouseMap[i]->m_strInName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
				
				delNode=m_HouseMap[i]->m_strBspName[k];
				m_HouseMap[i]->m_strBspName.erase(m_HouseMap[i]->m_strBspName.begin()+k);
				if(delNode) {
					delete [] delNode;
					delNode = NULL;
				}
			}

			if(m_HouseMap[i]->m_TM.size()==0)
			{
				int iNode = m_HouseMap[i]->m_IndexX * 11 + m_HouseMap[i]->m_IndexY;
				if(iNode < 0 || iNode >= MAXSECTOR)
				{
					//MessageBox(NULL,"오류","CMapStorage",MB_OK);
					return;
				}

				if(m_HouseMap[i]) {
					delete m_HouseMap[i];
					m_HouseMap[i] = NULL;
				}
				m_HouseMap.erase(m_HouseMap.begin()+i);
				m_HouseMapTable[iNode] = NULL;
			}
			return;
		}
	}	
}


void CMapStorage::AddHeightMap(float fAddx, float fAddy, float fAddz, float *fHeightData)
{
	if(fHeightData==NULL)
		return;

	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int i=0;i<(int)m_HeightMap.size();i++)
	{
		if( m_HeightMap[i]->m_IndexX == indexx &&
			m_HeightMap[i]->m_IndexY == indexy)
		{
			memcpy(m_HeightMap[i]->m_pHeightData,fHeightData,sizeof(float)*SECTORSX*SECTORSY);
			return;
		}
	}
	CSectorHeightMap *AddHeightMap=new CSectorHeightMap();
	AddHeightMap->m_IndexX=indexx;
	AddHeightMap->m_IndexY=indexy;
	if(m_LoadMode==0)
	{		
		AddHeightMap->m_pHeightData=new float[SECTORSX*SECTORSY];
	}

	memcpy(AddHeightMap->m_pHeightData,fHeightData,sizeof(float)*SECTORSX*SECTORSY);
	m_HeightMap.push_back(AddHeightMap);
}

void CMapStorage::AddWideMap(float fAddx, float fAddy, float fAddz, char *strWideMapName,char *strDetailMapName,char *strWideMapName1,char *strDetailMapName1,char *strWideMapName2,char *strDetailMapName2)
{
	if(strcmp(strWideMapName,"")==0)
		return;

	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int i=0;i<(int)m_WideMap.size();i++)
	{
		if( m_WideMap[i]->m_IndexX == indexx &&
			m_WideMap[i]->m_IndexY == indexy)
		{
			strcpy(m_WideMap[i]->m_strWidemapName[0],strWideMapName);
			strcpy(m_WideMap[i]->m_strWidemapName[1],strWideMapName1);
			strcpy(m_WideMap[i]->m_strWidemapName[2],strWideMapName2);

			strcpy(m_WideMap[i]->m_strDetailName[0],strDetailMapName);
			strcpy(m_WideMap[i]->m_strDetailName[1],strDetailMapName1);
			strcpy(m_WideMap[i]->m_strDetailName[2],strDetailMapName2);

			
			return;
		}
	}
	CSectorWideMap *AddWideMap=new CSectorWideMap();
	AddWideMap->m_IndexX=indexx;
	AddWideMap->m_IndexY=indexy;		

	strcpy(AddWideMap->m_strWidemapName[0],strWideMapName);
	strcpy(AddWideMap->m_strWidemapName[1],strWideMapName1);
	strcpy(AddWideMap->m_strWidemapName[2],strWideMapName2);

	strcpy(AddWideMap->m_strDetailName[0],strDetailMapName);
	strcpy(AddWideMap->m_strDetailName[1],strDetailMapName1);
	strcpy(AddWideMap->m_strDetailName[2],strDetailMapName2);

	m_WideMap.push_back(AddWideMap);
}

void CMapStorage::AddWaterMap(float fAddx, float fAddy, float fAddz,float fWaterHeight,float fWaterSizeX,float fWaterSizeY,float fWaterPosX,float fWaterPosY,color WaterColor,bool bWaterRelection)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int i=0;i<(int)m_WaterMap.size();i++)
	{
		if( m_WaterMap[i]->m_IndexX == indexx &&
			m_WaterMap[i]->m_IndexY == indexy) 
		{
			m_WaterMap[i]->m_fWaterHeight=fWaterHeight;
			m_WaterMap[i]->m_fWaterPosX=fWaterPosX;
			m_WaterMap[i]->m_fWaterPosY=fWaterPosY;
			m_WaterMap[i]->m_fWaterSizeX=fWaterSizeX;
			m_WaterMap[i]->m_fWaterSizeY=fWaterSizeY;
			m_WaterMap[i]->m_bWaterRelection=bWaterRelection;
			m_WaterMap[i]->m_WaterColor=WaterColor;
			return;
		}
	}
	CSectorWaterMap *AddWaterMap=new CSectorWaterMap();
	AddWaterMap->m_IndexX=indexx;
	AddWaterMap->m_IndexY=indexy;
	AddWaterMap->m_fWaterHeight=fWaterHeight;
	AddWaterMap->m_fWaterPosX=fWaterPosX;
	AddWaterMap->m_fWaterPosY=fWaterPosY;
	AddWaterMap->m_fWaterSizeX=fWaterSizeX;
	AddWaterMap->m_fWaterSizeY=fWaterSizeY;
	AddWaterMap->m_bWaterRelection=bWaterRelection;
	AddWaterMap->m_WaterColor=WaterColor;
	m_WaterMap.push_back(AddWaterMap);
}

CSectorWaterMap* CMapStorage::FindWaterMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 

	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_WaterMapTable[iNode])
	{
		return m_WaterMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_WaterMap.num;cStorage++)
	{
		if( m_WaterMap[cStorage]->m_IndexX==indexx &&
			m_WaterMap[cStorage]->m_IndexY==indexy)
		{
			return m_WaterMap[cStorage];
		}
	}*/	
	return NULL;
}

void CMapStorage::AddFallMap(float fAddx, float fAddy,float fAddz,float fFallPosX,float fFallPosY,float fFallRot,std::vector<float> &FallHeight,std::vector<float> &FallLeft,std::vector<float> &FallRight,std::vector<float> &FallAddX,std::vector<color> &WaterFallColor)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);


	for(int i=0;i<(int)m_FallMap.size();i++)
	{
		if( m_FallMap[i]->m_IndexX == indexx && 
			m_FallMap[i]->m_IndexY == indexy)
		{
			m_FallMap[i]->m_fFallPosX=fFallPosX;
			m_FallMap[i]->m_fFallPosY=fFallPosY;
			m_FallMap[i]->m_fFallRot=fFallRot;

			m_FallMap[i]->m_FallHeightList.clear();
			m_FallMap[i]->m_FallLeftList.clear();
			m_FallMap[i]->m_FallRightList.clear();
			m_FallMap[i]->m_FallAddXList.clear();

			for(int cElement=0;cElement<(int)FallHeight.size();cElement++)
			{
				m_FallMap[i]->m_FallHeightList.push_back(FallHeight[cElement]);
				m_FallMap[i]->m_FallLeftList.push_back(FallLeft[cElement]);
				m_FallMap[i]->m_FallRightList.push_back(FallRight[cElement]);
				m_FallMap[i]->m_FallAddXList.push_back(FallAddX[cElement]);
				m_FallMap[i]->m_FallColor.push_back(WaterFallColor[cElement]);
			}			
		}
	}
	
	CSectorFallMap *AddFallMap=new CSectorFallMap();

	AddFallMap->m_IndexX=indexx;
	AddFallMap->m_IndexY=indexy;
	AddFallMap->m_fFallPosX=fFallPosX;
	AddFallMap->m_fFallPosY=fFallPosY;
	AddFallMap->m_fFallRot=fFallRot;

	for(int i=0;i<(int)FallHeight.size();i++)
	{
		AddFallMap->m_FallHeightList.push_back(FallHeight[i]);		
		AddFallMap->m_FallLeftList.push_back(FallLeft[i]);
		AddFallMap->m_FallRightList.push_back(FallRight[i]);
		AddFallMap->m_FallAddXList.push_back(FallAddX[i]);
		AddFallMap->m_FallColor.push_back(WaterFallColor[i]);
	}
	m_FallMap.push_back(AddFallMap);	
}

CSectorFallMap* CMapStorage::FindFallMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_FallMapTable[iNode])
	{
		return m_FallMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_FallMap.num;cStorage++)
	{
		if( m_FallMap[cStorage]->m_IndexX==indexx &&
			m_FallMap[cStorage]->m_IndexY==indexy)
		{
			return m_FallMap[cStorage];
		}
	}*/
	return NULL;
}

void CMapStorage::AddInHouseObject(char *strOutName, char *strMedName, char *strInName,char *strBspName, char *strObjectName, matrix matTM,bool isAlpha,bool isLight)
{
	strOutName=strlwr(strOutName);
	strMedName=strlwr(strMedName);
	strInName=strlwr(strInName);
	strBspName=strlwr(strBspName);

	for(int cHouse=0;cHouse<(int)m_InHouseMap.size();cHouse++)
	{
		if( strcmp(m_InHouseMap[cHouse]->m_strOutName,strOutName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strMedName,strMedName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strInName,strInName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strBspName,strBspName)==0 )
		{
			char *pStrObjectName=new char[MAX_NAMEBUFFER];
			strcpy(pStrObjectName,strObjectName);
			m_InHouseMap[cHouse]->m_strObjectNameList.push_back(pStrObjectName);
			m_InHouseMap[cHouse]->m_matObjectList.push_back(matTM);
			m_InHouseMap[cHouse]->m_isAlpha.push_back(isAlpha);
			m_InHouseMap[cHouse]->m_isLight.push_back(isLight);
			m_InHouseMap[cHouse]->m_ObjectID.push_back(m_TotalInHouseObjectSceneCount++);
			return;
		}
	}
	CInHouseObjectMap *AddInHouseMap=new CInHouseObjectMap();

	char *pStrObjectName=new char[MAX_NAMEBUFFER];
	strcpy(pStrObjectName,strObjectName);
	AddInHouseMap->m_strObjectNameList.push_back(pStrObjectName);
	AddInHouseMap->m_matObjectList.push_back(matTM);
	AddInHouseMap->m_isAlpha.push_back(isAlpha);
	AddInHouseMap->m_isLight.push_back(isLight);

	strcpy(AddInHouseMap->m_strOutName,strOutName);
	strcpy(AddInHouseMap->m_strMedName,strMedName);
	strcpy(AddInHouseMap->m_strInName,strInName);
	strcpy(AddInHouseMap->m_strBspName,strBspName);

	AddInHouseMap->m_ObjectID.push_back(m_TotalInHouseObjectSceneCount++);
	m_InHouseMap.push_back(AddInHouseMap);
}

void CMapStorage::AddInHouseLight(char *strOutName, char *strMedName, char *strInName, char *strBspName, char *strLightName, matrix matTM,float fLightRange,color LightColor)
{
	strOutName=strlwr(strOutName);
	strMedName=strlwr(strMedName);
	strInName=strlwr(strInName);
	strBspName=strlwr(strBspName);

	for(int cHouse=0;cHouse<(int)m_InHouseMap.size();cHouse++)
	{
		if( strcmp(m_InHouseMap[cHouse]->m_strOutName,strOutName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strMedName,strMedName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strInName,strInName)==0 && 
			strcmp(m_InHouseMap[cHouse]->m_strBspName,strBspName)==0 )
		{
			char *pStrLightName=new char[MAX_NAMEBUFFER];
			strcpy(pStrLightName,strLightName);
			m_InHouseMap[cHouse]->m_strLightNameList.push_back(pStrLightName);
			m_InHouseMap[cHouse]->m_matLightList.push_back(matTM);
			m_InHouseMap[cHouse]->m_fLightRange.push_back(fLightRange);
			m_InHouseMap[cHouse]->m_LightColor.push_back(LightColor);
			m_InHouseMap[cHouse]->m_LightID.push_back(m_TotalInHouseLightSceneCount++);
			return;
		}
	}
	CInHouseObjectMap *AddInHouseMap=new CInHouseObjectMap();

	char *pStrObjectName=new char[MAX_NAMEBUFFER];
	strcpy(pStrObjectName,strLightName);
	AddInHouseMap->m_strLightNameList.push_back(pStrObjectName);
	AddInHouseMap->m_matLightList.push_back(matTM);
	AddInHouseMap->m_fLightRange.push_back(fLightRange);
	AddInHouseMap->m_LightColor.push_back(LightColor);
	strcpy(AddInHouseMap->m_strOutName,strOutName);
	strcpy(AddInHouseMap->m_strMedName,strMedName);
	strcpy(AddInHouseMap->m_strInName,strInName);
	strcpy(AddInHouseMap->m_strBspName,strBspName);
	AddInHouseMap->m_LightID.push_back(m_TotalInHouseLightSceneCount++);
	m_InHouseMap.push_back(AddInHouseMap);
}

CInHouseObjectMap* CMapStorage::FindInHouseMap(char *strOutName, char *strMedName, char *strInName,char *strBspName)
{
	strOutName=strlwr(strOutName);
	strMedName=strlwr(strMedName);
	strInName=strlwr(strInName);
	strBspName=strlwr(strBspName);
	for(int i=0;i<(int)m_InHouseMap.size();i++)
	{		
		if( strcmp(m_InHouseMap[i]->m_strOutName,strOutName)==0 &&
			strcmp(m_InHouseMap[i]->m_strMedName,strMedName)==0 &&
			strcmp(m_InHouseMap[i]->m_strInName,strInName)==0 &&
			strcmp(m_InHouseMap[i]->m_strBspName,strBspName)==0 )
		{
			return m_InHouseMap[i];
		}
	}
	return NULL;
}

void CMapStorage::DeleteAllData()
{
	int i;
	for(i=0;i<(int)m_HouseMap.size();i++)
	{
		if ( m_HouseMap[i] )
		{
			delete m_HouseMap[i];
			m_HouseMap[i] = NULL;	
		}
	}
	m_HouseMap.clear();
	for(i=0;i<(int)m_HeightMap.size();i++)
	{
		if(m_HeightMap[i]) {
			delete m_HeightMap[i];
			m_HeightMap[i] = NULL;
		}
	}
	m_HeightMap.clear();
	for(i=0;i<(int)m_WideMap.size();i++)
	{
		if(m_WideMap[i]) {
			delete m_WideMap[i];
			m_WideMap[i] = NULL;
		}
	}
	m_WideMap.clear();
	for(i=0;i<(int)m_FallMap.size();i++)
	{
		if(m_FallMap[i]) {
			delete m_FallMap[i];
			m_FallMap[i] = NULL;
		}
	}
	m_FallMap.clear();
	for(i=0;i<(int)m_InHouseMap.size();i++)
	{
		if(m_InHouseMap[i]) {
			delete m_InHouseMap[i];
			m_InHouseMap[i] = NULL;
		}
	}
	m_InHouseMap.clear();
	for(i=0;i<(int)m_PlantMap.size();i++)
	{
		if(m_PlantMap[i]) {
			delete m_PlantMap[i];
			m_PlantMap[i] = NULL;
		}
	}
	m_PlantMap.clear();
	for(i=0;i<(int)m_WaterMap.size();i++)
	{
		if(m_WaterMap[i]) {
			delete m_WaterMap[i];
			m_WaterMap[i] = NULL;
		}
	}	
	m_WaterMap.clear();
	for(i=0;i<(int)m_MeshMap.size();i++)
	{
		if(m_MeshMap[i]) {
			delete m_MeshMap[i];
			m_MeshMap[i] = NULL;
		}
	}
	m_MeshMap.clear();
	for(i=0;i<(int)m_EffectMap.size();i++)
	{
		if(m_EffectMap[i]) {
			delete m_EffectMap[i];
			m_EffectMap[i] = NULL;
		}
	}
	m_EffectMap.clear();


}

void CMapStorage::DelInHouseObject(char *strOutName, char *strMedName, char *strInName,long ObjectID)
{
	for(int cHouse=0;cHouse<(int)m_InHouseMap.size();cHouse++)
	{
		if( strcmp(m_InHouseMap[cHouse]->m_strOutName,strOutName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strMedName,strMedName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strInName,strInName)==0)
		{
			std::vector<int> vecDel;

			for(int cObject=0;cObject<(int)m_InHouseMap[cHouse]->m_strObjectNameList.size();cObject++)
			{
				// edith erase 로직에 문제있을듯.
				if(m_InHouseMap[cHouse]->m_ObjectID[cObject]==ObjectID)
				{
					vecDel.insert(vecDel.begin(), cObject);
				}
			}

			for(int a = 0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				delete m_InHouseMap[cHouse]->m_strObjectNameList[k];

				m_InHouseMap[cHouse]->m_strObjectNameList.erase(m_InHouseMap[cHouse]->m_strObjectNameList.begin()+k);
				m_InHouseMap[cHouse]->m_ObjectID.erase(m_InHouseMap[cHouse]->m_ObjectID.begin()+k);
				m_InHouseMap[cHouse]->m_matObjectList.erase(m_InHouseMap[cHouse]->m_matObjectList.begin()+k);
				m_InHouseMap[cHouse]->m_isAlpha.erase(m_InHouseMap[cHouse]->m_isAlpha.begin()+k);
				m_InHouseMap[cHouse]->m_isLight.erase(m_InHouseMap[cHouse]->m_isLight.begin()+k);
			}
		}
	}
}

void CMapStorage::DelInHouseLight(char *strOutName, char *strMedName, char *strInName,long LightID)
{
	for(int cHouse=0;cHouse<(int)m_InHouseMap.size();cHouse++)
	{
		if( strcmp(m_InHouseMap[cHouse]->m_strOutName,strOutName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strMedName,strMedName)==0 &&
			strcmp(m_InHouseMap[cHouse]->m_strInName,strInName)==0)
		{
			std::vector<int> vecDel;

			for(int cLight=0;cLight<(int)m_InHouseMap[cHouse]->m_strLightNameList.size();cLight++)
			{
				// edith erase 로직에 문제있을듯.
				if(m_InHouseMap[cHouse]->m_LightID[cLight]==LightID)
				{
					vecDel.insert(vecDel.begin(), cLight);
				}
			}

			for(int a = 0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				delete m_InHouseMap[cHouse]->m_strLightNameList[k];

				m_InHouseMap[cHouse]->m_LightID.erase(m_InHouseMap[cHouse]->m_LightID.begin()+k);
				m_InHouseMap[cHouse]->m_strLightNameList.erase(m_InHouseMap[cHouse]->m_strLightNameList.begin()+k);					
				m_InHouseMap[cHouse]->m_matLightList.erase(m_InHouseMap[cHouse]->m_matLightList.begin()+k);
				m_InHouseMap[cHouse]->m_fLightRange.erase(m_InHouseMap[cHouse]->m_fLightRange.begin()+k);
				m_InHouseMap[cHouse]->m_LightColor.erase(m_InHouseMap[cHouse]->m_LightColor.begin()+k);					
			}			
			return;
		}
	}	
}

void CMapStorage::AddPlantMap(float fAddx, float fAddy, float fAddz, unsigned char PlantKind)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int cPlant=0;cPlant<(int)m_PlantMap.size();cPlant++)
	{
		if( m_PlantMap[cPlant]->m_IndexX == indexx &&
			m_PlantMap[cPlant]->m_IndexY == indexy)
		{			
			int ix=(fAddx-(indexx*SECTORSIZE))/LINTERVAL;
			int iz=(fAddz-(indexy*SECTORSIZE))/LINTERVAL;

			bool isAlready=false;
			for(int cSubPlant=0;cSubPlant<(int)m_PlantMap[cPlant]->m_PlantKind.size();cSubPlant++)
			{
				if( m_PlantMap[cPlant]->m_PlantPosX[cSubPlant]==ix &&
					m_PlantMap[cPlant]->m_PlantPosZ[cSubPlant]==iz)
				{
					m_PlantMap[cPlant]->m_PlantPosX[cSubPlant]=ix;
					m_PlantMap[cPlant]->m_PlantPosZ[cSubPlant]=iz;
					m_PlantMap[cPlant]->m_PlantKind[cSubPlant]=PlantKind;
					isAlready=true;
					break;
				}
			}		
			if(isAlready==false)
			{
				m_PlantMap[cPlant]->m_PlantPosX.push_back(ix);
				m_PlantMap[cPlant]->m_PlantPosZ.push_back(iz);
				m_PlantMap[cPlant]->m_PlantKind.push_back(PlantKind);
			}
			//m_PlantMap[cPlant]->m_vecPlantPos.push_back(vector3(fAddx-(indexx*SECTORSIZE),fAddy,fAddz-(indexy*SECTORSIZE)));
			return;
		}
	}

	CSectorPlantMap *AddPlantMap=new CSectorPlantMap();
	AddPlantMap->m_IndexX=indexx;
	AddPlantMap->m_IndexY=indexy;

	int ix=(fAddx-(indexx*SECTORSIZE))/LINTERVAL;
	int iz=(fAddz-(indexy*SECTORSIZE))/LINTERVAL;

	AddPlantMap->m_PlantKind.push_back(PlantKind);
	//AddPlantMap->m_vecPlantPos.push_back(vector3(fAddx-(indexx*SECTORSIZE),fAddy,fAddz-(indexy*SECTORSIZE)));
	AddPlantMap->m_PlantPosX.push_back(ix);
	AddPlantMap->m_PlantPosZ.push_back(iz);

	m_PlantMap.push_back(AddPlantMap);		
}

void CMapStorage::DelPlantMap(float fDelx, float fDelY, float fDelz)
{
	int indexx=(int)(fDelx/SECTORSIZE);
	int indexy=(int)(fDelz/SECTORSIZE);

	int Findx,Findz,Delx,Delz;
	for(int cPlant=0;cPlant<(int)m_PlantMap.size();cPlant++)
	{
		std::vector<int> vecDel;

		if( m_PlantMap[cPlant]->m_IndexX == indexx &&
			m_PlantMap[cPlant]->m_IndexY == indexy)
		{
			for(int cInPlant=0;cInPlant<(int)m_PlantMap[cPlant]->m_PlantKind.size();cInPlant++)
			{
				// edith erase 로직에 문제있을듯.
				Delx=m_PlantMap[cPlant]->m_PlantPosX[cInPlant];
				Delz=m_PlantMap[cPlant]->m_PlantPosZ[cInPlant];
				Findx=(int)( (fDelx-(indexx*SECTORSIZE)) /LINTERVAL);
				Findz=(int)( (fDelz-(indexy*SECTORSIZE)) /LINTERVAL);
				if(Delx==Findx && Delz==Findz)
				{
					vecDel.insert(vecDel.begin(), cInPlant);

				}
			}

			for(int a=0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				m_PlantMap[cPlant]->m_PlantKind.erase(m_PlantMap[cPlant]->m_PlantKind.begin()+k);
				m_PlantMap[cPlant]->m_PlantPosX.erase(m_PlantMap[cPlant]->m_PlantPosX.begin()+k);
				m_PlantMap[cPlant]->m_PlantPosZ.erase(m_PlantMap[cPlant]->m_PlantPosZ.begin()+k);
				//m_PlantMap[cPlant]->m_vecPlantPos.DelIndex(cInPlant);
			}	

			return;
		}
	}
}


void CMapStorage::AddMeshMap(float fAddx, float fAddy, float fAddz,char *strObjectName,matrix matTM, bool isAlpha, bool isLight)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int cObject=0;cObject<(int)m_MeshMap.size();cObject++)
	{
		if( m_MeshMap[cObject]->m_IndexX == indexx &&
			m_MeshMap[cObject]->m_IndexY == indexy)
		{
			char *AddObjectName=new char[MAX_NAMEBUFFER];
			strcpy(AddObjectName,strObjectName);
			m_MeshMap[cObject]->m_strObjectName.push_back(AddObjectName);
			m_MeshMap[cObject]->m_TM.push_back(matTM);
			m_MeshMap[cObject]->m_isAlpha.push_back(isAlpha);
			m_MeshMap[cObject]->m_isLight.push_back(isLight);
			m_MeshMap[cObject]->m_ObjectSceneID.push_back(m_TotalObjectSceneCount++);			
			return;
		}
	}

	CSectorMeshMap *AddMeshMap=new CSectorMeshMap();
	AddMeshMap->m_IndexX=indexx;
	AddMeshMap->m_IndexY=indexy;

	char *AddObjectName=new char[MAX_NAMEBUFFER];
	strcpy(AddObjectName,strObjectName);
	AddMeshMap->m_strObjectName.push_back(AddObjectName);
	AddMeshMap->m_TM.push_back(matTM);
	AddMeshMap->m_isAlpha.push_back(isAlpha);
	AddMeshMap->m_isLight.push_back(isLight);
	AddMeshMap->m_ObjectSceneID.push_back(m_TotalObjectSceneCount++);
	m_MeshMap.push_back(AddMeshMap);
}

void CMapStorage::DelMeshMap(float fDelx, float fDely, float fDelz, long ObjectSceneID)
{
	int indexx=(int)(fDelx/SECTORSIZE);
	int indexy=(int)(fDelz/SECTORSIZE);			
	
	for(int i=0;i<(int)m_MeshMap.size();i++)
	{
		if( m_MeshMap[i]->m_IndexX==indexx &&
			m_MeshMap[i]->m_IndexY==indexy)
		{
			std::vector<int> vecDel;

			for(int cObject=0;cObject<(int)m_MeshMap[i]->m_TM.size();cObject++)
			{
				// edith erase 로직에 문제있을듯.
				if(ObjectSceneID==m_MeshMap[i]->m_ObjectSceneID[cObject])
				{
					vecDel.insert(vecDel.begin(), cObject);
				}
			}

			for(int a = 0; a < (int)vecDel.size(); a++)
			{
				int k = vecDel[a];

				m_MeshMap[i]->m_TM.erase(m_MeshMap[i]->m_TM.begin()+k);
				m_MeshMap[i]->m_isAlpha.erase(m_MeshMap[i]->m_isAlpha.begin()+k);
				m_MeshMap[i]->m_isLight.erase(m_MeshMap[i]->m_isLight.begin()+k);
				m_MeshMap[i]->m_ObjectSceneID.erase(m_MeshMap[i]->m_ObjectSceneID.begin()+k);				

				char *delNode;
				delNode=m_MeshMap[i]->m_strObjectName[k];
				delete [] delNode;
				m_MeshMap[i]->m_strObjectName.erase(m_MeshMap[i]->m_strObjectName.begin()+k);
			}

			if( m_MeshMap[i]->m_TM.size()==0)
			{
				delete m_MeshMap[i];
				m_MeshMap.erase(m_MeshMap.begin()+i);
			}
			return;
		}
	}	
}

void CMapStorage::SaveSector(char *strFilename,long SectorX,long SectorY)
{
	FILE *fp=fopen(strFilename,"wb");
	if(fp==NULL)
		return;
	char strWrite[MAX_NAMEBUFFER];
	sprintf(strWrite,"ZallA-3D Sector Scene Data File Build#1");
	fwrite(strWrite,sizeof(char)*MAX_NAMEBUFFER,1,fp);

	fwrite(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalObjectSceneCount,sizeof(long),1,fp);		

	bool HaveHeightMap=false;
	for(int cHeightData=0;cHeightData<(int)m_HeightMap.size();cHeightData++)
	{
		if( m_HeightMap[cHeightData]->m_IndexX==SectorX &&
			m_HeightMap[cHeightData]->m_IndexY==SectorY)
		{
			HaveHeightMap=true;
			fwrite(&HaveHeightMap,sizeof(bool),1,fp);
			fwrite(m_HeightMap[cHeightData]->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
			break;
		}
	}
	if(HaveHeightMap==false)
	{
		fwrite(&HaveHeightMap,sizeof(bool),1,fp);
	}

	bool HaveWaterMap=false;
	for(int cWaterData=0;cWaterData<(int)m_WaterMap.size();cWaterData++)
	{
		if( m_WaterMap[cWaterData]->m_IndexX==SectorX &&
			m_WaterMap[cWaterData]->m_IndexY==SectorY)
		{
			HaveWaterMap=true;
			fwrite(&HaveWaterMap,sizeof(bool),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_fWaterPosX,sizeof(float),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_fWaterPosY,sizeof(float),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeX,sizeof(float),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeY,sizeof(float),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_fWaterHeight,sizeof(float),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_bWaterRelection,sizeof(bool),1,fp);
			fwrite(&m_WaterMap[cWaterData]->m_WaterColor,sizeof(color),1,fp);
			break;
		}
	}
	if(HaveWaterMap==false)
	{
		fwrite(&HaveWaterMap,sizeof(bool),1,fp);
	}
	
	bool HaveWideMap=false;

	for(int cWideData=0;cWideData<(int)m_WideMap.size();cWideData++)
	{
		if( m_WideMap[cWideData]->m_IndexX==SectorX &&
			m_WideMap[cWideData]->m_IndexY==SectorY)
		{
			HaveWideMap=true;
			fwrite(&HaveWideMap,sizeof(bool),1,fp);
			fwrite(m_WideMap[cWideData]->m_strWidemapName,sizeof(char)*MAX_NAMEBUFFER,1,fp);		
			break;
		}
	}
	if(HaveWideMap==false)
	{
		fwrite(&HaveWideMap,sizeof(bool),1,fp);
	}

	bool HaveFallMap=false;
	for(int cFallData=0;cFallData<(int)m_FallMap.size();cFallData++)
	{
		if( m_FallMap[cFallData]->m_IndexX==SectorX &&
			m_FallMap[cFallData]->m_IndexY==SectorY)
		{
			HaveFallMap=true;
			fwrite(&HaveFallMap,sizeof(bool),1,fp);
			fwrite(&m_FallMap[cFallData]->m_fFallPosX,sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_fFallPosY,sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_fFallRot,sizeof(float),1,fp);
			int iSize = m_FallMap[cFallData]->m_FallHeightList.size();
			fwrite(&iSize,sizeof(int),1,fp);
			for(int cFallList=0;cFallList<iSize;cFallList++)
			{
				fwrite(&m_FallMap[cFallData]->m_FallHeightList[cFallList],sizeof(float),1,fp);
				fwrite(&m_FallMap[cFallData]->m_FallLeftList[cFallList],sizeof(float),1,fp);
				fwrite(&m_FallMap[cFallData]->m_FallRightList[cFallList],sizeof(float),1,fp);
				fwrite(&m_FallMap[cFallData]->m_FallAddXList[cFallList],sizeof(float),1,fp);			
				fwrite(&m_FallMap[cFallData]->m_FallColor[cFallList],sizeof(color),1,fp);
			}
			break;
		}
	}
	if(HaveFallMap==false)
	{
		fwrite(&HaveFallMap,sizeof(bool),1,fp);
	}

	bool HaveHouseMap=false;
	for(int cHouseData=0;cHouseData<(int)m_HouseMap.size();cHouseData++)
	{
		if( m_HouseMap[cHouseData]->m_IndexX==SectorX &&
			m_HouseMap[cHouseData]->m_IndexY==SectorY)
		{
			HaveHouseMap=true;
			fwrite(&HaveHouseMap,sizeof(bool),1,fp);
			int iSize = m_HouseMap[cHouseData]->m_TM.size();
			fwrite(&iSize,sizeof(int),1,fp);
			for(int cHouseList=0;cHouseList<iSize;cHouseList++)
			{
				matrix matTM=m_HouseMap[cHouseData]->m_TM[cHouseList];
				matTM._41-=SectorX*SECTORSIZE;				
				matTM._43-=SectorY*SECTORSIZE;
				//fwrite(&m_HouseMap[cHouseData]->m_TM[cHouseList],sizeof(matrix),1,fp);
				fwrite(&matTM,sizeof(matrix),1,fp);
				fwrite(&m_HouseMap[cHouseData]->m_HouseSceneID[cHouseList],sizeof(int),1,fp);
				fwrite(m_HouseMap[cHouseData]->m_strOutName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
				fwrite(m_HouseMap[cHouseData]->m_strMedName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
				fwrite(m_HouseMap[cHouseData]->m_strInName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			}
			break;
		}
	}
	if(HaveHouseMap==false)
	{
		fwrite(&HaveHouseMap,sizeof(bool),1,fp);
	}

	bool HavePlantMap=false;

	for(int cPlantData=0;cPlantData<(int)m_PlantMap.size();cPlantData++)
	{
		if( m_PlantMap[cPlantData]->m_IndexX==SectorX &&
			m_PlantMap[cPlantData]->m_IndexY==SectorY)
		{
			HavePlantMap=true;
			fwrite(&HavePlantMap,sizeof(bool),1,fp);
			int iSize = m_PlantMap[cPlantData]->m_PlantKind.size();
			fwrite(&iSize,sizeof(int),1,fp);
			for(int cSubPlant=0;cSubPlant<iSize;cSubPlant++)
			{
				/*
				fwrite(&m_PlantMap[cPlantData]->m_PlantKind[cSubPlant],sizeof(unsigned char),1,fp);
				fwrite(&m_PlantMap[cPlantData]->m_PlantPosX[cSubPlant],sizeof(unsigned char),1,fp);
				fwrite(&m_PlantMap[cPlantData]->m_PlantPosZ[cSubPlant],sizeof(unsigned char),1,fp);
				*/
				fwrite(&m_PlantMap[cPlantData]->m_PlantKind[cSubPlant],sizeof(unsigned char),1,fp);
			//fwrite(&m_PlantMap[cPlantData]->m_vecPlantPos[cSubPlant],sizeof(vector3),1,fp);
				fwrite(&m_PlantMap[cPlantData]->m_PlantPosX[cSubPlant],sizeof(unsigned char),1,fp);
				fwrite(&m_PlantMap[cPlantData]->m_PlantPosZ[cSubPlant],sizeof(unsigned char),1,fp);
				//fwrite(&m_PlantMap[cPlantData]->m_vecPlantPos[cSubPlant],sizeof(vector3),1,fp);
			}
			break;
		}
	}	
	if(HavePlantMap==false)
	{
		fwrite(&HavePlantMap,sizeof(bool),1,fp);
	}

	bool HaveMeshMap=false;

	for(int cMeshData=0;cMeshData<(int)m_MeshMap.size();cMeshData++)
	{
		if( m_MeshMap[cMeshData]->m_IndexX==SectorX&&
			m_MeshMap[cMeshData]->m_IndexY==SectorY)
		{
			HaveMeshMap=true;

			fwrite(&HaveMeshMap,sizeof(bool),1,fp);
			int iSize = m_MeshMap[cMeshData]->m_ObjectSceneID.size();
			fwrite(&iSize,sizeof(int),1,fp);
			for(int cObject=0;cObject<iSize;cObject++)
			{
				fwrite(&m_MeshMap[cMeshData]->m_ObjectSceneID[cObject],sizeof(long),1,fp);
				fwrite(m_MeshMap[cMeshData]->m_strObjectName[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);

				matrix matTM=m_MeshMap[cMeshData]->m_TM[cObject];
				matTM._41-=SectorX*SECTORSIZE;				
				matTM._43-=SectorY*SECTORSIZE;				
				fwrite(&matTM,sizeof(matrix),1,fp);
				//fwrite(&m_MeshMap[cMeshData]->m_TM[cObject],sizeof(matrix),1,fp);
				fwrite(&m_MeshMap[cMeshData]->m_isAlpha[cObject],sizeof(bool),1,fp);
				fwrite(&m_MeshMap[cMeshData]->m_isLight[cObject],sizeof(bool),1,fp);
			}
			break;
		}
	}	
	if(HaveMeshMap==false)
	{
		fwrite(&HaveMeshMap,sizeof(bool),1,fp);
	}
	fclose(fp);
}

void CMapStorage::LoadSector(char *strFilename, long SectorX, long SectorY)
{
	FILE *fp=fopen(strFilename,"rb");
	if(fp==NULL)
		return;
	

	char strRead[MAX_NAMEBUFFER];
	fread(strRead,sizeof(char)*MAX_NAMEBUFFER,1,fp);	

	int ReadJunk;
	fread(&ReadJunk,sizeof(long),1,fp);
	fread(&ReadJunk,sizeof(long),1,fp);
	fread(&ReadJunk,sizeof(long),1,fp);
	fread(&ReadJunk,sizeof(long),1,fp);


	bool HaveHeightData=false;
	fread(&HaveHeightData,sizeof(bool),1,fp);

	if(HaveHeightData)
	{
		CSectorHeightMap *AddNode=new CSectorHeightMap();
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;
		fread(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);				
		AddHeightMap(SectorX*SECTORSIZE,0.0f,SectorY*SECTORSIZE,AddNode->m_pHeightData);
		//m_HeightMap.push_back(AddNode);
		delete AddNode;
	}
	else
	{
		std::vector<int> vecDel;

		for(int cHeight=0;cHeight<(int)m_HeightMap.size();cHeight++)
		{
			if( m_HeightMap[cHeight]->m_IndexX==SectorX &&
				m_HeightMap[cHeight]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cHeight);
			}
		}

		for(int a = 0; a < (int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_HeightMap.erase(m_HeightMap.begin()+k);
		}

	}
	bool HaveWaterData=false;
	fread(&HaveWaterData,sizeof(bool),1,fp);

	if(HaveWaterData)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;
		fread(&AddNode->m_fWaterPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterHeight,sizeof(float),1,fp);
		fread(&AddNode->m_bWaterRelection,sizeof(bool),1,fp);
		fread(&AddNode->m_WaterColor,sizeof(color),1,fp);		
		AddWaterMap(SectorX*SECTORSIZE,0.0f,SectorY*SECTORSIZE,AddNode->m_fWaterHeight,AddNode->m_fWaterSizeX,
			AddNode->m_fWaterSizeY,AddNode->m_fWaterPosX,AddNode->m_fWaterPosY,AddNode->m_WaterColor,AddNode->m_bWaterRelection);
		delete AddNode;
		//m_WaterMap.push_back(AddNode);
	}
	else
	{
		std::vector<int> vecDel;

		for(int cWater=0;cWater<(int)m_WaterMap.size();cWater++)
		{
			if( m_WaterMap[cWater]->m_IndexX==SectorX &&
				m_WaterMap[cWater]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cWater);
			}
		}		

		for(int a=0; a<(int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_WaterMap.erase(m_WaterMap.begin()+k);
		}
	}

	bool HaveWideData=false;
	fread(&HaveWideData,sizeof(bool),1,fp);
	if(HaveWideData)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;
		fread(AddNode->m_strWidemapName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		AddWideMap(SectorX*SECTORSIZE,0.0f,SectorY*SECTORSIZE,AddNode->m_strWidemapName[0],AddNode->m_strDetailName[0],AddNode->m_strWidemapName[1],AddNode->m_strDetailName[1],AddNode->m_strWidemapName[2],AddNode->m_strDetailName[2]);
		delete AddNode;
		//m_WideMap.push_back(AddNode);
	}
	else
	{
		std::vector<int> vecDel;

		for(int cWide=0;cWide<(int)m_WideMap.size();cWide++)
		{
			if( m_WideMap[cWide]->m_IndexX==SectorX &&
				m_WideMap[cWide]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cWide);
			}
		}		

		for(int a=0; a < (int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_WideMap.erase(m_WideMap.begin()+k);
		}
	}

	bool HaveFallData=false;
	fread(&HaveFallData,sizeof(bool),1,fp);
	if(HaveFallData)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();		
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;
		fread(&AddNode->m_fFallPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fFallPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fFallRot,sizeof(float),1,fp);
		
		int cSavedFallList;
		fread(&cSavedFallList,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallRightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			fread(&ReadFallColor,sizeof(color),1,fp);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}
		AddFallMap(SectorX*SECTORSIZE,0.0f,SectorY*SECTORSIZE,AddNode->m_fFallPosX,AddNode->m_fFallPosY,
			AddNode->m_fFallRot,AddNode->m_FallHeightList,AddNode->m_FallLeftList,AddNode->m_FallRightList,
			AddNode->m_FallAddXList,AddNode->m_FallColor);
		delete AddNode;
		//m_FallMap.push_back(AddNode);
	}
	else
	{
		std::vector<int> vecDel;

		for(int cFall=0;cFall<(int)m_FallMap.size();cFall++)
		{
			if( m_FallMap[cFall]->m_IndexX==SectorX &&
				m_FallMap[cFall]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cFall);
			}
		}

		for(int a= 0; a < (int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_FallMap.erase(m_FallMap.begin()+k);
		}
	}

	bool HaveHouseData=false;

	fread(&HaveHouseData,sizeof(bool),1,fp);
	
	if(HaveHouseData)
	{
		CSectorHouseMap *AddNode=new CSectorHouseMap();
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;
		int cSavedHouseList=0;		

		fread(&cSavedHouseList,sizeof(int),1,fp);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{			
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			fread(&ReadHouseTM,sizeof(matrix),1,fp);

			matrix matTM=ReadHouseTM;
			matTM._41+=SectorX*SECTORSIZE;				
			matTM._43+=SectorY*SECTORSIZE;

			//AddNode->m_TM.push_back(ReadHouseTM);
			AddNode->m_TM.push_back(matTM);

			fread(&ReadHouseID,sizeof(int),1,fp);

			AddNode->m_HouseSceneID.push_back(m_TotalHouseSceneCount++);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);			
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strInName.push_back(ReadHouseName);
		}
		int cHouseMap=0;
		for(;cHouseMap<(int)m_HouseMap.size();cHouseMap++)
		{
			if( m_HouseMap[cHouseMap]->m_IndexX==SectorX &&
				m_HouseMap[cHouseMap]->m_IndexY==SectorY)
			{
				break;
			}
		}
		if(cHouseMap==m_HouseMap.size())
			m_HouseMap.push_back(AddNode);
		else
		{
			// 이미있는 섹터인데..
			m_HouseMap[cHouseMap]=AddNode;
		}
	}
	else
	{
		std::vector<int> vecDel;

		for(int cHouse=0;cHouse<(int)m_HouseMap.size();cHouse++)
		{
			if( m_HouseMap[cHouse]->m_IndexX==SectorX &&
				m_HouseMap[cHouse]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cHouse);
			}
		}		

		for(int a= 0; a < (int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_HouseMap.erase(m_HouseMap.begin()+k);
		}
	}

	bool HavePlantData=false;
	fread(&HavePlantData,sizeof(bool),1,fp);
	
	if(HavePlantData)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;

		int cSavedSubPlant;
		int ReadKind;
		vector3 ReadPos;
		unsigned char ReadPosX,ReadPosZ;
		fread(&cSavedSubPlant,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			fread(&ReadKind,sizeof(unsigned char),1,fp);
			//fread(&ReadPos,sizeof(vector3),1,fp);

			fread(&ReadPosX,sizeof(unsigned char),1,fp);
			fread(&ReadPosZ,sizeof(unsigned char),1,fp);
			///*
			AddNode->m_PlantKind.push_back(ReadKind);
			AddNode->m_PlantPosX.push_back(ReadPosX);
			AddNode->m_PlantPosZ.push_back(ReadPosZ);
			//*/
			//AddNode->m_vecPlantPos.push_back(ReadPos);
			/*
			fread(&ReadKind,sizeof(unsigned char),1,fp);
			fread(&ReadPosX,sizeof(unsigned char),1,fp);
			fread(&ReadPosZ,sizeof(unsigned char),1,fp);
			//fread(&ReadPos,sizeof(vector3),1,fp);
			bool isAlready=false;
			for(int cNowPlant=0;cNowPlant<AddNode->m_PlantKind.num;cNowPlant++)
			{
				if( AddNode->m_PlantPosX[cNowPlant]==ReadPosX &&
					AddNode->m_PlantPosZ[cNowPlant]==ReadPosZ)					
				{
					AddNode->m_PlantKind[cNowPlant]=ReadKind;
					AddNode->m_PlantPosX[cNowPlant]=ReadPosX;
					AddNode->m_PlantPosZ[cNowPlant]=ReadPosZ;
					isAlready=true;
				}
			}
			if(isAlready==false)
			{
				AddNode->m_PlantKind.push_back(ReadKind);
				AddNode->m_PlantPosX.push_back(ReadPosX);
				AddNode->m_PlantPosZ.push_back(ReadPosZ);
			}
			*/
		}
		int cPlantMap=0;
		for(;cPlantMap<(int)m_PlantMap.size();cPlantMap++)
		{
			if( m_PlantMap[cPlantMap]->m_IndexX==SectorX &&
				m_PlantMap[cPlantMap]->m_IndexY==SectorY)
			{
				break;
			}
		}
		
		if(cPlantMap==m_PlantMap.size())
			m_PlantMap.push_back(AddNode);
		else
			m_PlantMap[cPlantMap]=AddNode;		
		
	}	
	else
	{
		std::vector<int> vecDel;

		for(int cPlant=0;cPlant<(int)m_PlantMap.size();cPlant++)
		{
			if( m_PlantMap[cPlant]->m_IndexX==SectorX &&
				m_PlantMap[cPlant]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cPlant);
			}
		}

		for(int a= 0; a < (int)vecDel.size(); a++)
		{
			int k = vecDel[a];
			m_PlantMap.erase(m_PlantMap.begin()+k);
		}
	}
	bool HaveMeshData=false;
	fread(&HaveMeshData,sizeof(bool),1,fp);
	if(HaveMeshData)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		AddNode->m_IndexX=SectorX;
		AddNode->m_IndexY=SectorY;

		int cSavedObject=0;
		fread(&cSavedObject,sizeof(int),1,fp);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;		
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			fread(&ReadObjectSceneID,sizeof(long),1,fp);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fread(&ReadMatrixTM,sizeof(matrix),1,fp);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			fread(&ReadLight,sizeof(bool),1,fp);

			AddNode->m_ObjectSceneID.push_back(m_TotalObjectSceneCount++);
			AddNode->m_strObjectName.push_back(ReadObjectName);

			matrix matTM=ReadMatrixTM;
			matTM._41+=SectorX*SECTORSIZE;				
			matTM._43+=SectorY*SECTORSIZE;
			AddNode->m_TM.push_back(matTM);
			//AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(ReadAlpha);
			AddNode->m_isLight.push_back(ReadLight);
		}
		int cMeshMap=0;
		for(;cMeshMap<(int)m_MeshMap.size();cMeshMap++)
		{
			if( m_MeshMap[cMeshMap]->m_IndexX==SectorX &&
				m_MeshMap[cMeshMap]->m_IndexY==SectorY)
			{
				break;
				//m_MeshMap.DelIndex(cMeshMap);
			}
		}
		if(cMeshMap==m_MeshMap.size())
			m_MeshMap.push_back(AddNode);
		else
			m_MeshMap[cMeshMap]=AddNode;
	}	
	else
	{
		std::vector<int> vecDel;

		for(int cMesh=0;cMesh<(int)m_MeshMap.size();cMesh++)
		{
			if( m_MeshMap[cMesh]->m_IndexX==SectorX &&
				m_MeshMap[cMesh]->m_IndexY==SectorY)
			{
				// edith erase 로직에 문제있을듯.
				vecDel.insert(vecDel.begin(), cMesh);
			}
		}

		for(int a= 0; a<(int)vecDel.size();a++)
		{
			int k = vecDel[a];
			m_MeshMap.erase(m_MeshMap.begin()+k);
		}
	}
	fclose(fp);
}

void CMapStorage::OldLoad(char *strFilename)
{
	FILE *fp=fopen(strFilename,"rb");
	if(fp==NULL)
		return;

	DeleteAllData();

	char strRead[MAX_NAMEBUFFER];
	fread(strRead,sizeof(char)*MAX_NAMEBUFFER,1,fp);	

	fread(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fread(&m_TotalObjectSceneCount,sizeof(long),1,fp);

	// Height-Value Loading
	int cSavedHeightData=0;
	fread(&cSavedHeightData,sizeof(int),1,fp);	
	for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
	{
		CSectorHeightMap *AddNode=new CSectorHeightMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(AddNode->m_pHeightData,sizeof(float)*(SECTORSX-1)*(SECTORSY-1),1,fp);
		//m_HeightMap.push_back(AddNode);
	}	
	//Water loading
	int cSavedWaterData=0;
	fread(&cSavedWaterData,sizeof(int),1,fp);
	
	for(int cWaterData=0;cWaterData<cSavedWaterData;cWaterData++)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fWaterPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterHeight,sizeof(float),1,fp);
		fread(&AddNode->m_bWaterRelection,sizeof(bool),1,fp);
		fread(&AddNode->m_WaterColor,sizeof(color),1,fp);		
		m_WaterMap.push_back(AddNode);
	}

	//Widemap loading

	int cSavedWideData=0;
	fread(&cSavedWideData,sizeof(int),1,fp);

	for(int cWideData=0;cWideData<cSavedWideData;cWideData++)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(AddNode->m_strWidemapName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		m_WideMap.push_back(AddNode);
	}

	//Fallmap loading

	int cSavedFallData=0;
	fread(&cSavedFallData,sizeof(int),1,fp);

	for(int cFallData=0;cFallData<cSavedFallData;cFallData++)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fFallPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fFallPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fFallRot,sizeof(float),1,fp);
		
		int cSavedFallList;
		fread(&cSavedFallList,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallRightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			fread(&ReadFallColor,sizeof(color),1,fp);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}
		m_FallMap.push_back(AddNode);
	}	

	// HouseMap loading

	int cSavedHouseData=0;
	fread(&cSavedHouseData,sizeof(int),1,fp);
	
	for(int cHouseData=0;cHouseData<cSavedHouseData;cHouseData++)
	{
		CSectorHouseMap *AddNode=new CSectorHouseMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		int cSavedHouseList=0;		

		fread(&cSavedHouseList,sizeof(int),1,fp);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			fread(&ReadHouseTM,sizeof(matrix),1,fp);
			AddNode->m_TM.push_back(ReadHouseTM);
			fread(&ReadHouseID,sizeof(int),1,fp);
			AddNode->m_HouseSceneID.push_back(ReadHouseID);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);			
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strInName.push_back(ReadHouseName);
		}
		m_HouseMap.push_back(AddNode);
	}

	int cSavedInHouseData=0;
	fread(&cSavedInHouseData,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		fread(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		int cSavedObject,cSavedLight;
		fread(&cSavedObject,sizeof(int),1,fp);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			fread(&ReadObjectID,sizeof(long),1,fp);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			fread(&ReadObjectTM,sizeof(matrix),1,fp);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			AddNode->m_isAlpha.push_back(ReadAlpha);
			fread(&ReadLight,sizeof(bool),1,fp);
			AddNode->m_isLight.push_back(ReadLight);
		}
		fread(&cSavedLight,sizeof(int),1,fp);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			fread(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			fread(&ReadLightID,sizeof(long),1,fp);
			AddNode->m_LightID.push_back(ReadLightID);
			fread(&ReadLightTM,sizeof(matrix),1,fp);
			AddNode->m_matLightList.push_back(ReadLightTM);
			fread(&ReadLightRange,sizeof(float),1,fp);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			fread(&ReadLightColor,sizeof(color),1,fp);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}

		/*
		AddNode->m_strLightNameList.num=0;
		AddNode->m_LightID.num=0;
		AddNode->m_matLightList.num=0;
		AddNode->m_fLightRange.num=0;
		AddNode->m_LightColor.num=0;
		m_TotalInHouseLightSceneCount=0;
		*/
		m_InHouseMap.push_back(AddNode);
	}
	
	// PlantMap loading
	int cSavedPlantData=0;
	fread(&cSavedPlantData,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<cSavedPlantData;cPlantData++)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);		

		int cSavedSubPlant;
		int ReadKind;
		vector3 ReadPos;
		fread(&cSavedSubPlant,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			fread(&ReadKind,sizeof(unsigned char),1,fp);
			fread(&ReadPos,sizeof(vector3),1,fp);
			AddNode->m_PlantKind.push_back(ReadKind);
			//AddNode->m_vecPlantPos.push_back(ReadPos);
		}
		//m_PlantMap.push_back(AddNode);
	}
	
	// MeshMap loading
	int cSavedMeshData=0;
	fread(&cSavedMeshData,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<cSavedMeshData;cMeshData++)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);

		int cSavedObject=0;
		fread(&cSavedObject,sizeof(int),1,fp);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;		
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			fread(&ReadObjectSceneID,sizeof(long),1,fp);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fread(&ReadMatrixTM,sizeof(matrix),1,fp);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			fread(&ReadLight,sizeof(bool),1,fp);

			AddNode->m_ObjectSceneID.push_back(ReadObjectSceneID);
			AddNode->m_strObjectName.push_back(ReadObjectName);
			AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(ReadAlpha);
			AddNode->m_isLight.push_back(ReadLight);
		}
		m_MeshMap.push_back(AddNode);
	}
	fclose(fp);
}

void CMapStorage::SaveInHouseMap(char *strFilename)
{
	FILE *fp=fopen(strFilename,"wb");
	if(fp==NULL)
		return;

	int iCnt = m_InHouseMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<iCnt;cInHouseData++)
	{		
		fwrite(m_InHouseMap[cInHouseData]->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		int iSize = m_InHouseMap[cInHouseData]->m_strObjectNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cObject=0;cObject<iSize;cObject++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strObjectNameList[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_ObjectID[cObject],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matObjectList[cObject],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isAlpha[cObject],sizeof(bool),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isLight[cObject],sizeof(bool),1,fp);
		}

		iSize = m_InHouseMap[cInHouseData]->m_strLightNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cLight=0;cLight<iSize;cLight++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strLightNameList[cLight],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightID[cLight],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matLightList[cLight],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_fLightRange[cLight],sizeof(float),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightColor[cLight],sizeof(color),1,fp);
		}		
	}
	fclose(fp);
}

void CMapStorage::LoadInHouseMap(char *strFilename)
{
	int cSavedInHouseData=0;
	FILE *fp=fopen(strFilename,"rb");
	fread(&cSavedInHouseData,sizeof(int),1,fp);
	if(cSavedInHouseData==0)
		return;

	m_InHouseMap.clear();

	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		fread(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		strcpy(AddNode->m_strOutName,strlwr(AddNode->m_strOutName));
		strcpy(AddNode->m_strMedName,strlwr(AddNode->m_strMedName));
		strcpy(AddNode->m_strInName,strlwr(AddNode->m_strInName));

		int cSavedObject,cSavedLight;
		fread(&cSavedObject,sizeof(int),1,fp);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			fread(&ReadObjectID,sizeof(long),1,fp);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			fread(&ReadObjectTM,sizeof(matrix),1,fp);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			AddNode->m_isAlpha.push_back(true);
			fread(&ReadLight,sizeof(bool),1,fp);
			AddNode->m_isLight.push_back(true);
		}
		fread(&cSavedLight,sizeof(int),1,fp);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			fread(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			fread(&ReadLightID,sizeof(long),1,fp);
			AddNode->m_LightID.push_back(ReadLightID);
			fread(&ReadLightTM,sizeof(matrix),1,fp);
			AddNode->m_matLightList.push_back(ReadLightTM);
			fread(&ReadLightRange,sizeof(float),1,fp);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			fread(&ReadLightColor,sizeof(color),1,fp);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}
		/*
		AddNode->m_strLightNameList.num=0;
		AddNode->m_LightID.num=0;
		AddNode->m_matLightList.num=0;
		AddNode->m_fLightRange.num=0;
		AddNode->m_LightColor.num=0;
		m_TotalInHouseLightSceneCount=0;
		*/		
		m_InHouseMap.push_back(AddNode);
	}
	fclose(fp);
}

void CMapStorage::BugFix_Tree()
{	

	for(int cPlantMap=0;cPlantMap<(int)m_PlantMap.size();cPlantMap++)
	{
		CSectorPlantMap *PlantMap=m_PlantMap[cPlantMap];
		std::vector<int> DelTreeList;
/*
		for(int cPlant=0;cPlant<(int)PlantMap->m_PlantKind.size();cPlant++)
		{
			//ix=PlantMap->m_vecPlantPos[cPlant].x/LINTERVAL;
			//iy=PlantMap->m_vecPlantPos[cPlant].z/LINTERVAL;
			//if( ix < 0 || ix>= SECTORSX || iy < 0 || iy>=SECTORSY)
			//DelTreeList.push_back(cPlant);
		}
*/
		for(int cPlant=(int)DelTreeList.size()-1;cPlant>=0;cPlant--)
		{
			// edith erase 로직에 문제있을듯.
			PlantMap->m_PlantKind.erase(PlantMap->m_PlantKind.begin()+DelTreeList[cPlant]);
			//PlantMap->m_vecPlantPos.DelIndex(DelTreeList[cPlant]);
		}
	}	
}

std::vector<char *> strHangulList;
std::vector<char *> strEnglishList;

void ConvertH2E(char *strHangle)
{
	strHangle=strlwr(strHangle);
	for(int cHangul=0;cHangul<(int)strHangulList.size();cHangul++)
	{
		if(strcmp(strHangulList[cHangul],strHangle)==0)
		{
			strcpy(strHangle,strEnglishList[cHangul]);
			break;
		}
	}
}

void CMapStorage::BugFix_Hangul2English()
{
	
	FILE *fp=fopen("c:\\convert2.txt","rt");

	int ErrorCheck=0;
	do 
	{
		char *strHangul=new char[256];
		char *strEnglish=new char[256];
		char dummy[256];
		ErrorCheck=fscanf(fp,"%s %s %s",dummy,strHangul,strEnglish);
		if(ErrorCheck!=-1)
		{
			strHangul=strlwr(strHangul);
			strHangulList.push_back(strHangul);
			strEnglish=strlwr(strEnglish);
			strEnglishList.push_back(strEnglish);
		}
	}
	while(ErrorCheck!=-1);	

	for(int cWideData=0;cWideData<(int)m_WideMap.size();cWideData++)
	{
		ConvertH2E(m_WideMap[cWideData]->m_strWidemapName[0]);		
	}
	for(int cHouseData=0;cHouseData<(int)m_HouseMap.size();cHouseData++)
	{
		for(int cHouseList=0;cHouseList<(int)m_HouseMap[cHouseData]->m_HouseSceneID.size();cHouseList++)
		{
			ConvertH2E(m_HouseMap[cHouseData]->m_strOutName[cHouseList]);
			ConvertH2E(m_HouseMap[cHouseData]->m_strMedName[cHouseList]);
			ConvertH2E(m_HouseMap[cHouseData]->m_strInName[cHouseList]);
		}
	}
	for(int cInHouseData=0;cInHouseData<(int)m_InHouseMap.size();cInHouseData++)
	{
		ConvertH2E(m_InHouseMap[cInHouseData]->m_strOutName);
		ConvertH2E(m_InHouseMap[cInHouseData]->m_strMedName);
		ConvertH2E(m_InHouseMap[cInHouseData]->m_strInName);		
		for(int cObject=0;cObject<(int)m_InHouseMap[cInHouseData]->m_strObjectNameList.size();cObject++)
		{
			ConvertH2E(m_InHouseMap[cInHouseData]->m_strObjectNameList[cObject]);
		}
		for(int cLight=0;cLight<(int)m_InHouseMap[cInHouseData]->m_strLightNameList.size();cLight++)
		{
			ConvertH2E(m_InHouseMap[cInHouseData]->m_strLightNameList[cLight]);			
		}
	}
	for(int cMeshData=0;cMeshData<(int)m_MeshMap.size();cMeshData++)
	{
		for(int cObject=0;cObject<(int)m_MeshMap[cMeshData]->m_ObjectSceneID.size();cObject++)
		{
			ConvertH2E(m_MeshMap[cMeshData]->m_strObjectName[cObject]);
		}
	}
}

void CMapStorage::BugFix_Tree2(char *strFilename)
{
	FILE *fp=fopen(strFilename,"rb");
	if(fp==NULL)
		return;

	DeleteAllData();

	char strRead[MAX_NAMEBUFFER];
	fread(strRead,sizeof(char)*MAX_NAMEBUFFER,1,fp);	

	fread(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fread(&m_TotalObjectSceneCount,sizeof(long),1,fp);

	// Height-Value Loading
	int cSavedHeightData=0;
	fread(&cSavedHeightData,sizeof(int),1,fp);	
	for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
	{
		CSectorHeightMap *AddNode=new CSectorHeightMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
		m_HeightMap.push_back(AddNode);
	}	
	//Water loading
	int cSavedWaterData=0;
	fread(&cSavedWaterData,sizeof(int),1,fp);
	
	for(int cWaterData=0;cWaterData<cSavedWaterData;cWaterData++)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fWaterPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterHeight,sizeof(float),1,fp);
		fread(&AddNode->m_bWaterRelection,sizeof(bool),1,fp);
		fread(&AddNode->m_WaterColor,sizeof(color),1,fp);		
		m_WaterMap.push_back(AddNode);
	}

	//Widemap loading

	int cSavedWideData=0;
	fread(&cSavedWideData,sizeof(int),1,fp);

	for(int cWideData=0;cWideData<cSavedWideData;cWideData++)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(AddNode->m_strWidemapName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		m_WideMap.push_back(AddNode);
	}

	//Fallmap loading

	int cSavedFallData=0;
	fread(&cSavedFallData,sizeof(int),1,fp);

	for(int cFallData=0;cFallData<cSavedFallData;cFallData++)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fFallPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fFallPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fFallRot,sizeof(float),1,fp);
		
		int cSavedFallList;
		fread(&cSavedFallList,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallRightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			fread(&ReadFallColor,sizeof(color),1,fp);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}
		m_FallMap.push_back(AddNode);
	}	

	// HouseMap loading

	int cSavedHouseData=0;
	fread(&cSavedHouseData,sizeof(int),1,fp);
	
	for(int cHouseData=0;cHouseData<cSavedHouseData;cHouseData++)
	{
		CSectorHouseMap *AddNode=new CSectorHouseMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		int cSavedHouseList=0;		

		fread(&cSavedHouseList,sizeof(int),1,fp);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			fread(&ReadHouseTM,sizeof(matrix),1,fp);
			AddNode->m_TM.push_back(ReadHouseTM);
			fread(&ReadHouseID,sizeof(int),1,fp);
			AddNode->m_HouseSceneID.push_back(ReadHouseID);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);			
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strInName.push_back(ReadHouseName);
		}
		m_HouseMap.push_back(AddNode);
	}

	int cSavedInHouseData=0;
	fread(&cSavedInHouseData,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		fread(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		strcpy(AddNode->m_strOutName,strlwr(AddNode->m_strOutName));
		strcpy(AddNode->m_strMedName,strlwr(AddNode->m_strMedName));
		strcpy(AddNode->m_strInName,strlwr(AddNode->m_strInName));

		int cSavedObject,cSavedLight;
		fread(&cSavedObject,sizeof(int),1,fp);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			fread(&ReadObjectID,sizeof(long),1,fp);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			fread(&ReadObjectTM,sizeof(matrix),1,fp);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			AddNode->m_isAlpha.push_back(true);
			fread(&ReadLight,sizeof(bool),1,fp);
			AddNode->m_isLight.push_back(true);
		}
		fread(&cSavedLight,sizeof(int),1,fp);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			fread(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			fread(&ReadLightID,sizeof(long),1,fp);
			AddNode->m_LightID.push_back(ReadLightID);
			fread(&ReadLightTM,sizeof(matrix),1,fp);
			AddNode->m_matLightList.push_back(ReadLightTM);
			fread(&ReadLightRange,sizeof(float),1,fp);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			fread(&ReadLightColor,sizeof(color),1,fp);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}
		/*
		AddNode->m_strLightNameList.num=0;
		AddNode->m_LightID.num=0;
		AddNode->m_matLightList.num=0;
		AddNode->m_fLightRange.num=0;
		AddNode->m_LightColor.num=0;
		m_TotalInHouseLightSceneCount=0;
		*/
		m_InHouseMap.push_back(AddNode);
	}
	
	// PlantMap loading
	int cSavedPlantData=0;
	fread(&cSavedPlantData,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<cSavedPlantData;cPlantData++)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);		

		int cSavedSubPlant;
		int ReadKind;
		
		vector3 ReadPos;
		fread(&cSavedSubPlant,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			fread(&ReadKind,sizeof(unsigned char),1,fp);						
			fread(&ReadPos,sizeof(vector3),1,fp);

			int ix=ReadPos.x/LINTERVAL;
			int iy=ReadPos.z/LINTERVAL;			
			AddNode->m_PlantKind.push_back(ReadKind);			
			AddNode->m_PlantPosX.push_back(ix);
			AddNode->m_PlantPosZ.push_back(iy);
		}
		m_PlantMap.push_back(AddNode);
	}
	
	// MeshMap loading
	int cSavedMeshData=0;
	fread(&cSavedMeshData,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<cSavedMeshData;cMeshData++)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);

		int cSavedObject=0;
		fread(&cSavedObject,sizeof(int),1,fp);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			fread(&ReadObjectSceneID,sizeof(long),1,fp);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fread(&ReadMatrixTM,sizeof(matrix),1,fp);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			fread(&ReadLight,sizeof(bool),1,fp);

			AddNode->m_ObjectSceneID.push_back(ReadObjectSceneID);
			AddNode->m_strObjectName.push_back(ReadObjectName);
			AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(true);
			AddNode->m_isLight.push_back(true);
		}
		m_MeshMap.push_back(AddNode);
	}
	fclose(fp);
}

CSectorMustDivideVertexMap* CMapStorage::FindMustDivideVertexMap(float fFindX, float fFindY)
{
	int indexx=(int)(fFindX/SECTORSIZE);
	int indexy=(int)(fFindY/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	
	if(m_MustDivideVertexMapTable[iNode])
	{
		return m_MustDivideVertexMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_MustDivideVertexMap.num;cStorage++)
	{
		if( m_MustDivideVertexMap[cStorage]->m_IndexX==indexx &&
			m_MustDivideVertexMap[cStorage]->m_IndexY==indexy)
		{
			return m_MustDivideVertexMap[cStorage];
		}
	}*/

	return NULL;
}

void CMapStorage::AddMustDivideVertex(CSectorMustDivideVertexMap *pMustDivideVertex)
{
	m_MustDivideVertexMap.push_back(pMustDivideVertex);
}

void CMapStorage::AddSectorLight(float fAddx,float fAddy,float fAddz,float fR,float fG,float fB,float fRad,int iLightSam,int iShadowSam,float fShadowFactor,int iAmb,char *strEffect,float fExpose)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);
	for(int i = 0; i < (int)m_LightMap.size(); i++ )
	{
		if((m_LightMap[i]->m_IndexX == indexx) &&
			(m_LightMap[i]->m_IndexY == indexy))
		{
			CSectorLight pNode;
			pNode.m_fRange = fRad;
			pNode.m_vecColor.x = fR;
			pNode.m_vecColor.y = fG;
			pNode.m_vecColor.z = fB;
			pNode.m_vecPos.x = fAddx;
			pNode.m_vecPos.y = fAddy;
			pNode.m_vecPos.z = fAddz;
			
			pNode.m_iLightSamples = iLightSam;
			pNode.m_iShadowSamples = iShadowSam;
			pNode.m_fShadowFactors = fShadowFactor;
			pNode.m_iAmbient = iAmb;
			pNode.m_fExpose = fExpose;

			strcpy(pNode.m_strEffect,strEffect);


			m_LightMap[i]->m_lstLights.push_back(pNode);
			return;
		}
	}
	CSectorLightMap *pMap = new CSectorLightMap;
	pMap->m_IndexX = indexx;
	pMap->m_IndexY = indexy;

	CSectorLight pNode;
	pNode.m_fRange = fRad;
	pNode.m_vecColor.x = fR;
	pNode.m_vecColor.y = fG;
	pNode.m_vecColor.z = fB;
	pNode.m_vecPos.x = fAddx;
	pNode.m_vecPos.y = fAddy;
	pNode.m_vecPos.z = fAddz;

	pNode.m_iLightSamples = iLightSam;
	pNode.m_iShadowSamples = iShadowSam;
	pNode.m_fShadowFactors = fShadowFactor;
	pNode.m_iAmbient = iAmb;
	pNode.m_fExpose = fExpose;

	strcpy(pNode.m_strEffect,strEffect);

	pMap->m_lstLights.push_back(pNode);
	
	m_LightMap.push_back(pMap);
}
void CMapStorage::DeleteSectorLight(float fAddx,float fAddy,float fAddz)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);
	for(int i = 0; i < (int)m_LightMap.size(); i++ )
	{
		if((m_LightMap[i]->m_IndexX == indexx) &&
		(m_LightMap[i]->m_IndexY == indexy))
		{
			for(int k = 0; k < (int)(m_LightMap[i]->m_lstLights.size()); i++)
			{
				if((m_LightMap[i]->m_lstLights[k].m_vecPos.x == fAddx) && 
					(m_LightMap[i]->m_lstLights[k].m_vecPos.y == fAddy) &&
					(m_LightMap[i]->m_lstLights[k].m_vecPos.z == fAddz))
				{
					m_LightMap[i]->m_lstLights.erase(m_LightMap[i]->m_lstLights.begin() + k);
					return;
				}
			}
		}
	}
}
CSectorLightMap *CMapStorage::FindLightMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_LightMapTable[iNode])
	{
		return m_LightMapTable[iNode];
	}
/*
	for(int i = 0; i < m_LightMap.num; i++)
	{
		if((m_LightMap[i]->m_IndexX == indexx) &&
		(m_LightMap[i]->m_IndexY == indexy))
		{
			return m_LightMap[i];
		}

	}
*/
	return (CSectorLightMap *)NULL;

}

void CMapStorage::AddEffectMap(float fAddx,float fAddy, float fAddz,char *strFilename,float xrot,float yrot,float zrot)
{	
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

//	std::vector<vector3> m_vecEffectPos;
//	std::vector<char*> m_vecEffectName;

	int i=0;
	for(;i<(int)m_EffectMap.size();i++)
	{
		if( m_EffectMap[i]->m_IndexX==indexx &&
			m_EffectMap[i]->m_IndexY==indexy )
		{
			m_EffectMap[i]->m_vecEffectPos.push_back(vector3(fAddx,fAddy,fAddz));
			m_EffectMap[i]->m_vecEffectRot.push_back(vector3(xrot,yrot,zrot));
			char *pAddName = new char[256];
			strcpy(pAddName,strFilename);
			m_EffectMap[i]->m_strEffectName.push_back(pAddName);
			m_EffectMap[i]->Sector_EffectNum++;
			return;
		}
	}
	
	CSectorEffectMap *AddEffectMap=new CSectorEffectMap();
	AddEffectMap->m_IndexX=indexx;
	AddEffectMap->m_IndexY=indexy;

	AddEffectMap->m_vecEffectPos.push_back(vector3(fAddx,fAddy,fAddz));
	AddEffectMap->m_vecEffectRot.push_back(vector3(xrot,yrot,zrot));
	char *pAddName = new char[256];
	strcpy(pAddName,strFilename);

	AddEffectMap->m_strEffectName.push_back(pAddName);
	m_EffectMap.push_back(AddEffectMap);
	m_EffectMap[i]->Sector_EffectNum++;

}
void CMapStorage::DelEffectMap(float fAddx,float fAddy,float fAddz,char *filename) 
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);
	int j;

	for(int cStorage=0;cStorage<(int)m_EffectMap.size();cStorage++)
	{
		if( m_EffectMap[cStorage]->m_IndexX==indexx &&
			m_EffectMap[cStorage]->m_IndexY==indexy)
		{
			for(j=0;j<m_EffectMap[cStorage]->Sector_EffectNum;j++) {
				if(m_EffectMap[cStorage]->m_vecEffectPos[j].x == fAddx &&
				   m_EffectMap[cStorage]->m_vecEffectPos[j].y == fAddy &&
				   m_EffectMap[cStorage]->m_vecEffectPos[j].z == fAddz) 
				{
					m_EffectMap[cStorage]->m_vecEffectPos.erase(m_EffectMap[cStorage]->m_vecEffectPos.begin()+j);
					m_EffectMap[cStorage]->m_strEffectName.erase(m_EffectMap[cStorage]->m_strEffectName.begin()+j);
					m_EffectMap[cStorage]->Sector_EffectNum--;
					return;
				}
			}

		}
	}
	return;
}
CSectorEffectMap *CMapStorage::FindEffectMap(int indexX,int indexY) {

	int iNode = indexX * 11 + indexY; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;


	if(m_EffectMapTable[iNode])
	{
		return m_EffectMapTable[iNode];
	}
	/*
	for(int cStorage=0;cStorage<m_EffectMap.num;cStorage++) {
		if( m_EffectMap[cStorage]->m_IndexX==indexX &&
			m_EffectMap[cStorage]->m_IndexY==indexY) {
			return m_EffectMap[cStorage];
			//return &m_EffectMap.m_Effect[cStorage];
		}
	}*/
	return NULL;
}
CSectorEffectMap *CMapStorage::FindEffectMap(int &index) {
	if(index >= (int)m_EffectMap.size())
		index = 0;
	return m_EffectMap[index];

}
CSectorEffectMap *CMapStorage::FindEffectMap(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;


	if(m_EffectMapTable[iNode])
	{
		return m_EffectMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_EffectMap.num;cStorage++)
	{
		if( m_EffectMap[cStorage]->m_IndexX==indexx &&
			m_EffectMap[cStorage]->m_IndexY==indexy)
		{
			return m_EffectMap[cStorage];
			//return &m_EffectMap.m_Effect[cStorage];
		}
	}*/
	return NULL;
}

void CMapStorage::DeleteAllObject()
{
	int i;
	for(i=0;i<(int)m_HouseMap.size();i++)
	{
		delete m_HouseMap[i];
	}
	for(i=0;i<(int)m_HeightMap.size();i++)
	{
		delete m_HeightMap[i];
	}
	for(i=0;i<(int)m_WideMap.size();i++)
	{
		delete m_WideMap[i];
	}
	for(i=0;i<(int)m_FallMap.size();i++)
	{
		delete m_FallMap[i];
	}
	for(i=0;i<(int)m_InHouseMap.size();i++)
	{
		delete m_InHouseMap[i];
	}
	for(i=0;i<(int)m_PlantMap.size();i++)
	{
      	delete m_PlantMap[i];
	}
	for(i=0;i<(int)m_MeshMap.size();i++)
	{
		delete m_MeshMap[i];
	}
	for(i=0;i<(int)m_WaterMap.size();i++)
	{
		delete m_WaterMap[i];
	}
	for(i=0;i<(int)m_EffectMap.size();i++)
	{
		delete m_EffectMap[i];
	}
	for(i=0;i<(int)m_LandscapeEffectMap.size();i++) {
		delete m_LandscapeEffectMap[i];
	}
	for(i = 0; i < (int)m_LightMap.size(); i++ )
	{
		delete m_LightMap[i];

	}

	m_LightMap.clear();
	m_HouseMap.clear();
	m_HeightMap.clear();	
	m_WideMap.clear();
	m_FallMap.clear();
	m_InHouseMap.clear();
	m_PlantMap.clear();
	m_MeshMap.clear();
	m_WaterMap.clear();
	m_EffectMap.clear();
	m_LandscapeEffectMap.clear();
}
/*
void CMapStorage::AddSoundMap(float fAddx, float fAddy, float fAddz, char *strFilename, float fMinRange,float fMaxRange)
{
	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int i=0;i<m_SoundMap.num;i++)
	{
		if( m_SoundMap[i]->m_IndexX == indexx &&
			m_SoundMap[i]->m_IndexY == indexy)
		{
			char *pFilename=new char[256];
			strcpy(pFilename,strFilename);
			m_SoundMap[i]->m_strSoundFilenameList.push_back(pFilename);
			m_SoundMap[i]->m_SoundPosList.push_back(vector3(fAddx,fAddy,fAddz));
			m_SoundMap[i]->m_SoundMinRangeList.push_back(fMinRange);
			m_SoundMap[i]->m_SoundMaxRangeList.push_back(fMaxRange);
			return;
		}
	}

	CSectorSoundMap *AddNode=new CSectorSoundMap();
	AddNode->m_IndexX=indexx;
	AddNode->m_IndexY=indexy;
	AddNode->m_SoundPosList.push_back(vector3(fAddx,fAddy,fAddz));
	AddNode->m_SoundMinRangeList.push_back(fMinRange);
	AddNode->m_SoundMaxRangeList.push_back(fMaxRange);

	char *pFilename=new char[256];
	strcpy(pFilename,strFilename);
	AddNode->m_strSoundFilenameList.push_back(pFilename);
	m_SoundMap.push_back(AddNode);

	/*
	if(fHeightData==NULL)
		return;

	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);

	for(int i=0;i<m_HeightMap.num;i++)
	{
		if( m_HeightMap[i]->m_IndexX == indexx &&
			m_HeightMap[i]->m_IndexY == indexy)
		{
			memcpy(m_HeightMap[i]->m_pHeightData,fHeightData,sizeof(float)*SECTORSX*SECTORSY);
			return;
		}
	}
	CSectorHeightMap *AddHeightMap=new CSectorHeightMap();
	AddHeightMap->m_IndexX=indexx;
	AddHeightMap->m_IndexY=indexy;
	if(m_LoadMode==0)
	{		
		AddHeightMap->m_pHeightData=new float[SECTORSX*SECTORSY];
	}

	memcpy(AddHeightMap->m_pHeightData,fHeightData,sizeof(float)*SECTORSX*SECTORSY);
	m_HeightMap.push_back(AddHeightMap);
	*/
/*
}

CSectorSoundMap* CMapStorage::FindSoundMap(float fFindx, float fFindy)
{	
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);

	for(int cStorage=0;cStorage<m_SoundMap.num;cStorage++)
	{
		if( m_SoundMap[cStorage]->m_IndexX==indexx &&
			m_SoundMap[cStorage]->m_IndexY==indexy)
		{
			return m_SoundMap[cStorage];
		}
	}
	return NULL;
}

void CMapStorage::SaveSoundMap(char *strSoundMapFilename)
{
	FILE *fp=fopen(strSoundMapFilename,"wb");
	fwrite(&m_SoundMap.num,sizeof(int),1,fp);
	for(int cStorage=0;cStorage<m_SoundMap.num;cStorage++)
	{
		fwrite(&m_SoundMap[cStorage]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_SoundMap[cStorage]->m_IndexY,sizeof(int),1,fp);
		fwrite(&m_SoundMap[cStorage]->m_SoundMaxRangeList.num,sizeof(int),1,fp);
		for(int i=0;i<m_SoundMap[cStorage]->m_SoundMaxRangeList.num;i++)
		{
			fwrite(m_SoundMap[cStorage]->m_strSoundFilenameList[i],sizeof(char)*256,1,fp);
			fwrite(&m_SoundMap[cStorage]->m_SoundPosList[i],sizeof(vector3),1,fp);
			fwrite(&m_SoundMap[cStorage]->m_SoundMaxRangeList[i],sizeof(float),1,fp);
			fwrite(&m_SoundMap[cStorage]->m_SoundMinRangeList[i],sizeof(float),1,fp);			
		}
	}
}

void CMapStorage::LoadSoundMap(char *strSoundMapFilename)
{
	FILE *fp=fopen(strSoundMapFilename,"rb");
	if(fp==NULL)
		return;
	int nStorage=0;
	fread(&nStorage,sizeof(int),1,fp);
	for(int cStorage=0;cStorage<nStorage;cStorage++)
	{
		CSectorSoundMap *AddNode=new CSectorSoundMap();
		AddNode->m_IndexY;
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		int c;
		fread(&c,sizeof(int),1,fp);
		for(int i=0;i<c;i++)
		{
			char *strName=new char[256];
			fread(strName,sizeof(char)*256,1,fp);
			AddNode->m_strSoundFilenameList.push_back(strName);

			vector3 vecPos;
			fread(&vecPos,sizeof(vector3),1,fp);
			AddNode->m_SoundPosList.push_back(vecPos);
			float fRange;
			fread(&fRange,sizeof(float),1,fp);
			AddNode->m_SoundMaxRangeList.push_back(fRange);
			fread(&fRange,sizeof(float),1,fp);
			AddNode->m_SoundMinRangeList.push_back(fRange);			
		}
		m_SoundMap.push_back(AddNode);
	}
}
*/
void CMapStorage::SaveGF3(char *strFilename)
{
	FILE *fp=fopen(strFilename,"wb");
	if(fp==NULL)
		return;
	char strWrite[MAX_NAMEBUFFER];
	sprintf(strWrite,"ZallA-3D Scene Data File Build#2");
	fwrite(strWrite,sizeof(char)*MAX_NAMEBUFFER,1,fp);

	fwrite(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fwrite(&m_TotalObjectSceneCount,sizeof(long),1,fp);	

	// Height-Value Saving 
	int iCnt = m_HeightMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cHeightData=0;cHeightData<iCnt;cHeightData++)
	{		
		fwrite(&m_HeightMap[cHeightData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_HeightMap[cHeightData]->m_IndexY,sizeof(int),1,fp);
		fwrite(m_HeightMap[cHeightData]->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
	}

	//Water Saving
	iCnt = m_WaterMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cWaterData=0;cWaterData<iCnt;cWaterData++)
	{
		fwrite(&m_WaterMap[cWaterData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_IndexY,sizeof(int),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterPosX,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterPosY,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeX,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterSizeY,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_fWaterHeight,sizeof(float),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_bWaterRelection,sizeof(bool),1,fp);
		fwrite(&m_WaterMap[cWaterData]->m_WaterColor,sizeof(color),1,fp);
	}

	//Widemap saving
	iCnt = m_WideMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cWideData=0;cWideData<iCnt;cWideData++)
	{
		fwrite(&m_WideMap[cWideData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_WideMap[cWideData]->m_IndexY,sizeof(int),1,fp);
		fwrite(m_WideMap[cWideData]->m_strWidemapName[0],sizeof(char)*MAX_NAMEBUFFER,1,fp);		
		fwrite(m_WideMap[cWideData]->m_strDetailName[0],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_WideMap[cWideData]->m_strWidemapName[1],sizeof(char)*MAX_NAMEBUFFER,1,fp);		
		fwrite(m_WideMap[cWideData]->m_strDetailName[1],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_WideMap[cWideData]->m_strWidemapName[2],sizeof(char)*MAX_NAMEBUFFER,1,fp);		
		fwrite(m_WideMap[cWideData]->m_strDetailName[2],sizeof(char)*MAX_NAMEBUFFER,1,fp);
	}

	//Fallmap saving
	iCnt = m_FallMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cFallData=0;cFallData<iCnt;cFallData++)
	{
		fwrite(&m_FallMap[cFallData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_FallMap[cFallData]->m_IndexY,sizeof(int),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallPosX,sizeof(float),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallPosY,sizeof(float),1,fp);
		fwrite(&m_FallMap[cFallData]->m_fFallRot,sizeof(float),1,fp);

		int iSize = m_FallMap[cFallData]->m_FallHeightList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<iSize;cFallList++)
		{
			fwrite(&m_FallMap[cFallData]->m_FallHeightList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallLeftList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallRightList[cFallList],sizeof(float),1,fp);
			fwrite(&m_FallMap[cFallData]->m_FallAddXList[cFallList],sizeof(float),1,fp);			
			fwrite(&m_FallMap[cFallData]->m_FallColor[cFallList],sizeof(color),1,fp);
		}
	}

	//HouseMap saving 
	iCnt = m_HouseMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cHouseData=0;cHouseData<iCnt;cHouseData++)
	{
		fwrite(&m_HouseMap[cHouseData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_HouseMap[cHouseData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_HouseMap[cHouseData]->m_TM.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cHouseList=0;cHouseList<iSize;cHouseList++)
		{
			fwrite(&m_HouseMap[cHouseData]->m_TM[cHouseList],sizeof(matrix),1,fp);
			fwrite(&m_HouseMap[cHouseData]->m_HouseSceneID[cHouseList],sizeof(int),1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strOutName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strMedName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strInName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(m_HouseMap[cHouseData]->m_strBspName[cHouseList],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		}
	}
	//InHouseObjectMap saving
	iCnt = m_InHouseMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<iCnt;cInHouseData++)
	{		
		fwrite(m_InHouseMap[cInHouseData]->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fwrite(m_InHouseMap[cInHouseData]->m_strBspName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		int iSize = m_InHouseMap[cInHouseData]->m_strObjectNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cObject=0;cObject<iSize;cObject++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strObjectNameList[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_ObjectID[cObject],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matObjectList[cObject],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isAlpha[cObject],sizeof(bool),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_isLight[cObject],sizeof(bool),1,fp);
		}

		iSize = m_InHouseMap[cInHouseData]->m_strLightNameList.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cLight=0;cLight<iSize;cLight++)
		{
			fwrite(m_InHouseMap[cInHouseData]->m_strLightNameList[cLight],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightID[cLight],sizeof(long),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_matLightList[cLight],sizeof(matrix),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_fLightRange[cLight],sizeof(float),1,fp);
			fwrite(&m_InHouseMap[cInHouseData]->m_LightColor[cLight],sizeof(color),1,fp);
		}		
	}
	// PlantMap saving
	iCnt = m_PlantMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<iCnt;cPlantData++)
	{
		fwrite(&m_PlantMap[cPlantData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_PlantMap[cPlantData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_PlantMap[cPlantData]->m_PlantKind.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<iSize;cSubPlant++)
		{
			fwrite(&m_PlantMap[cPlantData]->m_PlantKind[cSubPlant],sizeof(unsigned char),1,fp);
			//fwrite(&m_PlantMap[cPlantData]->m_vecPlantPos[cSubPlant],sizeof(vector3),1,fp);
			fwrite(&m_PlantMap[cPlantData]->m_PlantPosX[cSubPlant],sizeof(unsigned char),1,fp);
			fwrite(&m_PlantMap[cPlantData]->m_PlantPosZ[cSubPlant],sizeof(unsigned char),1,fp);
		}
	}
	// MeshMap saving
	iCnt = m_MeshMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<iCnt;cMeshData++)
	{
		fwrite(&m_MeshMap[cMeshData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_MeshMap[cMeshData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_MeshMap[cMeshData]->m_ObjectSceneID.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cObject=0;cObject<iSize;cObject++)
		{
			fwrite(&m_MeshMap[cMeshData]->m_ObjectSceneID[cObject],sizeof(long),1,fp);
			fwrite(m_MeshMap[cMeshData]->m_strObjectName[cObject],sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_TM[cObject],sizeof(matrix),1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_isAlpha[cObject],sizeof(bool),1,fp);
			fwrite(&m_MeshMap[cMeshData]->m_isLight[cObject],sizeof(bool),1,fp);
		}
	}

	iCnt = m_EffectMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cEffectData=0;cEffectData<iCnt;cEffectData++)
	{
		fwrite(&m_EffectMap[cEffectData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_EffectMap[cEffectData]->m_IndexY,sizeof(int),1,fp);
		
		int iSize = m_EffectMap[cEffectData]->m_vecEffectPos.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cEffect=0;cEffect<iSize;cEffect++)
		{
			fwrite(&m_EffectMap[cEffectData]->m_vecEffectPos[cEffect],sizeof(vector3),1,fp);
			fwrite(&m_EffectMap[cEffectData]->m_vecEffectRot[cEffect],sizeof(vector3),1,fp);
			fwrite(m_EffectMap[cEffectData]->m_strEffectName[cEffect],sizeof(char)*256,1,fp);
		}
	}
	// landscapeeffect save
	iCnt = m_LandscapeEffectMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int cLandscapeEffectData = 0;cLandscapeEffectData<iCnt;cLandscapeEffectData++) 
	{
		fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_IndexX,sizeof(int),1,fp);
		fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_IndexY,sizeof(int),1,fp);

		int iSize = m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect.size();
		fwrite(&iSize,sizeof(int),1,fp);
		for(int cLand = 0;cLand <iSize;cLand++) 
		{
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_nEffectMethod,sizeof(int),1,fp);
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_vecCenterPos,sizeof(vector3),1,fp);
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_Name,sizeof(char)* 50,1,fp);
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_Param,sizeof(float) * 50,1,fp);					
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_IntParam,sizeof(int) * 20,1,fp);
			fwrite(&m_LandscapeEffectMap[cLandscapeEffectData]->m_LandscapeEffect[cLand].m_dwBlend,sizeof(DWORD) * 2,1,fp);
		}
	}
	// Sector Light Save
	iCnt = m_LightMap.size();
	fwrite(&iCnt,sizeof(int),1,fp);
	for(int iLightMap = 0; iLightMap < iCnt; iLightMap++)
	{
		int iNum = m_LightMap[iLightMap]->m_lstLights.size();

		fwrite((int *)&(m_LightMap[iLightMap]->m_IndexX),sizeof(int),1,fp);
		fwrite((int *)&(m_LightMap[iLightMap]->m_IndexY),sizeof(int),1,fp);
		fwrite((int *)&(iNum),sizeof(int),1,fp);

		for(int iLight = 0; iLight < (int)(m_LightMap[iLightMap]->m_lstLights.size()); iLight++)
		{
			fwrite((CSectorLight *)&(m_LightMap[iLightMap]->m_lstLights[iLight]),sizeof(CSectorLight),1,fp);
		}
	}
	fclose(fp);
}

void CMapStorage::Read( void* tBuf, INT len, const char* sBuf, LONG& seek )
{
	CopyMemory(tBuf, &sBuf[seek], len);
	seek += len;
}

void CMapStorage::ConvertData(char * data, LONG len)
{
	char * enckey = "cjsgkdmlTlqkfshaemfdmlwktlsdjqtsmsgkfn";

	if ( len )
	{
		char a;
		int counter = 0;
		for(LONG i = 256; i < len; ++i)
		{
			a = data[i];

			if (counter == ((sizeof(enckey) / sizeof(char)) - 1))
			{
				counter = 0;
			}

			a = ((a - counter) ^ enckey[counter]);
			data[i] = a;
			counter++;
		}
	}
}

void CMapStorage::LoadGF3Mem(char *strFilename)
{
	FILE *fp=fopen(strFilename,"rb");		
	
	if(fp==NULL)
		return;

	fseek(fp, 0, SEEK_END);
	fpos_t pos = 0;
	fgetpos(fp, &pos);
	fseek(fp, 0, SEEK_SET);

	char* pMem = new char[pos];
	fread(pMem, pos, 1, fp);

	fclose(fp);

	// 컨버팅
	ConvertData(pMem, pos);

	//map search Table 초기화
	memset(m_DundeonMapTable,0,sizeof(CSectorDungeonMap *) * MAXSECTOR);
	memset(m_MeshMapTable,0,sizeof(CSectorMeshMap *) * MAXSECTOR);
	memset(m_PlantMapTable,0,sizeof(CSectorPlantMap *) * MAXSECTOR);
	memset(m_HouseMapTable,0,sizeof(CSectorHouseMap *) * MAXSECTOR);
	memset(m_WaterMapTable,0,sizeof(CSectorWaterMap *) * MAXSECTOR);
	memset(m_FallMapTable,0,sizeof(CSectorFallMap *) * MAXSECTOR);
	memset(m_InHouseMapTable,0,sizeof(CInHouseObjectMap *) * MAXSECTOR);
	memset(m_MustDivideVertexMapTable,0,sizeof(CSectorMustDivideVertexMap *) * MAXSECTOR);
	memset(m_EffectMapTable,0,sizeof(CSectorEffectMap *) * MAXSECTOR);
	memset(m_LightMapTable,0,sizeof(CSectorLightMap *) * MAXSECTOR);
	memset(m_LandscapeEffectMapTable,0,sizeof(CSectorLandscapeEffectMap *) * MAXSECTOR);
	memset(m_HeightMapTable,0,sizeof(CSectorHeightMap *) * MAXSECTOR);
	memset(m_WideMapTable,0,sizeof(CSectorWideMap *) * MAXSECTOR);

	strcpy(m_strMapFilename,strFilename);

	DeleteAllData();

	LONG seek = 0;

	char strRead[MAX_NAMEBUFFER];
	ZeroMemory(strRead, sizeof(char)*MAX_NAMEBUFFER);
	Read(strRead, sizeof(char)*MAX_NAMEBUFFER, pMem, seek);	

	Read(&m_TotalHouseSceneCount, sizeof(long), pMem, seek);	
	Read(&m_TotalInHouseLightSceneCount, sizeof(long), pMem, seek);	
	Read(&m_TotalInHouseObjectSceneCount, sizeof(long), pMem, seek);	
	Read(&m_TotalObjectSceneCount, sizeof(long), pMem, seek);	

	// Height-Value Loading
	int cSavedHeightData=0;
	Read(&cSavedHeightData, sizeof(int), pMem, seek);	
	if(m_LoadMode==0)
	{
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);	
			Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);	

			AddNode->m_pHeightData=new float[SECTORSX*SECTORSY];
			Read(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,pMem, seek);	

			if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
				AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
			{
				delete AddNode;
				continue;
			}

			m_HeightMap.push_back(AddNode);
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				delete AddNode;
				continue;
			}
			
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}
	else
	{
		//float pHeightData[SECTORSX*SECTORSY];
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);	
			Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);	
	
			AddNode->m_pHeightData=new float[SECTORSX*SECTORSY];
			Read(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,pMem, seek);	

			if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
				AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
			{
				delete AddNode;
				continue;
			}

			/*AddNode->m_FilePos=ftell(fp);
			Read(&pHeightData,sizeof(float)*SECTORSX*SECTORSY,pMem, seek);			
			*/
			m_HeightMap.push_back(AddNode);
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;

			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				delete AddNode;
				
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				continue;
			}
			
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}

	//Water loading
	int cSavedWaterData=0;
	Read(&cSavedWaterData,sizeof(int),pMem, seek);
	
	for(int cWaterData=0;cWaterData<cSavedWaterData;cWaterData++)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);
		Read(&AddNode->m_fWaterPosX,sizeof(float),pMem, seek);
		Read(&AddNode->m_fWaterPosY,sizeof(float),pMem, seek);
		Read(&AddNode->m_fWaterSizeX,sizeof(float),pMem, seek);
		Read(&AddNode->m_fWaterSizeY,sizeof(float),pMem, seek);
		Read(&AddNode->m_fWaterHeight,sizeof(float),pMem, seek);
		Read(&AddNode->m_bWaterRelection,sizeof(bool),pMem, seek);
		Read(&AddNode->m_WaterColor,sizeof(color),pMem, seek);		
		
		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}
	
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_WaterMap.push_back(AddNode);
		m_WaterMapTable[iNode] = AddNode;
	}

	//Widemap loading

//	FILE* tFp = fopen("C:\\ZoneInfo.txt", "wt");

	int cSavedWideData=0;
	Read(&cSavedWideData,sizeof(int),pMem, seek);

	for(int cWideData=0;cWideData<cSavedWideData;cWideData++)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);

		Read(AddNode->m_strWidemapName[0],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strDetailName[0],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);

		Read(AddNode->m_strWidemapName[1],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strDetailName[1],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);

		Read(AddNode->m_strWidemapName[2],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strDetailName[2],sizeof(char)*MAX_NAMEBUFFER,pMem, seek);

		if(CRenderOption::m_TerrainMultiDetalTexture)
		{			

		}
		else
		{
			strcpy(AddNode->m_strDetailName[0],"detail.dds");
			strcpy(AddNode->m_strWidemapName[1],"");
			strcpy(AddNode->m_strDetailName[1],"");
			strcpy(AddNode->m_strWidemapName[2],"");
			strcpy(AddNode->m_strDetailName[2],"");
		}
		
		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

//		fprintf(tFp, "%d, %d, %d, %s\r\n", iNode, AddNode->m_IndexX, AddNode->m_IndexY, AddNode->m_strWidemapName[0]);

		m_WideMap.push_back(AddNode);
		m_WideMapTable[iNode] = AddNode;

	}

//	fclose(tFp);

	//Fallmap loading

	int cSavedFallData=0;
	Read(&cSavedFallData,sizeof(int),pMem, seek);

	for(int cFallData=0;cFallData<cSavedFallData;cFallData++)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);
		Read(&AddNode->m_fFallPosX,sizeof(float),pMem, seek);
		Read(&AddNode->m_fFallPosY,sizeof(float),pMem, seek);
		Read(&AddNode->m_fFallRot,sizeof(float),pMem, seek);
		
		int cSavedFallList;
		Read(&cSavedFallList,sizeof(int),pMem, seek);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			Read(&ReadFallData,sizeof(float),pMem, seek);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			Read(&ReadFallData,sizeof(float),pMem, seek);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			Read(&ReadFallData,sizeof(float),pMem, seek);
			AddNode->m_FallRightList.push_back(ReadFallData);
			Read(&ReadFallData,sizeof(float),pMem, seek);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			Read(&ReadFallColor,sizeof(color),pMem, seek);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}
		
		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}
		m_FallMap.push_back(AddNode);
		m_FallMapTable[iNode] = AddNode;
	}	

	// HouseMap loading

	int cSavedHouseData=0;
	Read(&cSavedHouseData,sizeof(int),pMem, seek);
	
	int IndexX, IndexY;

	for(int cHouseData=0;cHouseData<cSavedHouseData;cHouseData++)
	{
		Read(&IndexX,sizeof(int),pMem, seek);
		Read(&IndexY,sizeof(int),pMem, seek);

		CSectorHouseMap *AddNode=new CSectorHouseMap();
		int cSavedHouseList=0;		

		AddNode->m_IndexX = IndexX;
		AddNode->m_IndexY = IndexY;

		Read(&cSavedHouseList,sizeof(int),pMem, seek);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			Read(&ReadHouseTM,sizeof(matrix),pMem, seek);
			AddNode->m_TM.push_back(ReadHouseTM);
			Read(&ReadHouseID,sizeof(int),pMem, seek);
			AddNode->m_HouseSceneID.push_back(ReadHouseID);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			Read(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);			
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			Read(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			Read(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			AddNode->m_strInName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			Read(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			AddNode->m_strBspName.push_back(ReadHouseName);
		}
		
		if(IndexX < 0 || IndexX > 11 ||
			IndexY < 0 || IndexY > 11 )
		{
			delete AddNode;
			continue;
		}

		int iNode = IndexX * 11 + IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_HouseMap.push_back(AddNode);
		m_HouseMapTable[iNode] = AddNode;
	}

	int cSavedInHouseData=0;
	Read(&cSavedInHouseData,sizeof(int),pMem, seek);
	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		Read(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
		Read(AddNode->m_strBspName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);

		strcpy(AddNode->m_strOutName,strlwr(AddNode->m_strOutName));
		strcpy(AddNode->m_strMedName,strlwr(AddNode->m_strMedName));
		strcpy(AddNode->m_strInName,strlwr(AddNode->m_strInName));
		strcpy(AddNode->m_strBspName,strlwr(AddNode->m_strBspName));

		int cSavedObject,cSavedLight;
		Read(&cSavedObject,sizeof(int),pMem, seek);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			Read(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			Read(&ReadObjectID,sizeof(long),pMem, seek);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			Read(&ReadObjectTM,sizeof(matrix),pMem, seek);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			Read(&ReadAlpha,sizeof(bool),pMem, seek);
			AddNode->m_isAlpha.push_back(true);
			Read(&ReadLight,sizeof(bool),pMem, seek);
			AddNode->m_isLight.push_back(true);
		}
		Read(&cSavedLight,sizeof(int),pMem, seek);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			Read(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			Read(&ReadLightID,sizeof(long),pMem, seek);
			AddNode->m_LightID.push_back(ReadLightID);
			Read(&ReadLightTM,sizeof(matrix),pMem, seek);
			AddNode->m_matLightList.push_back(ReadLightTM);
			Read(&ReadLightRange,sizeof(float),pMem, seek);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			Read(&ReadLightColor,sizeof(color),pMem, seek);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}		
		m_InHouseMap.push_back(AddNode);
				
	}
	
	// PlantMap loading
	int cSavedPlantData=0;
	Read(&cSavedPlantData,sizeof(int),pMem, seek);
	for(int cPlantData=0;cPlantData<cSavedPlantData;cPlantData++)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);		


		int cSavedSubPlant;
		int ReadKind;
		unsigned char ReadPosX,ReadPosZ;
		//vector3 ReadPos;
		Read(&cSavedSubPlant,sizeof(int),pMem, seek);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			Read(&ReadKind,sizeof(unsigned char),pMem, seek);
			Read(&ReadPosX,sizeof(unsigned char),pMem, seek);
			Read(&ReadPosZ,sizeof(unsigned char),pMem, seek);
			//Read(&ReadPos,sizeof(vector3),pMem, seek);
			bool isAlready=false;
			for(int cNowPlant=0;cNowPlant<(int)AddNode->m_PlantKind.size();cNowPlant++)
			{
				if( AddNode->m_PlantPosX[cNowPlant]==ReadPosX &&
					AddNode->m_PlantPosZ[cNowPlant]==ReadPosZ)					
				{
					AddNode->m_PlantKind[cNowPlant]=ReadKind;
					AddNode->m_PlantPosX[cNowPlant]=ReadPosX;
					AddNode->m_PlantPosZ[cNowPlant]=ReadPosZ;
					isAlready=true;
				}
			}
			if(isAlready==false)
			{
				AddNode->m_PlantKind.push_back(ReadKind);
				AddNode->m_PlantPosX.push_back(ReadPosX);
				AddNode->m_PlantPosZ.push_back(ReadPosZ);
			}
			//AddNode->m_vecPlantPos.push_back(ReadPos);
		}

				
		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_PlantMap.push_back(AddNode);
		m_PlantMapTable[iNode] = AddNode;
	}
	
	// MeshMap loading
	int cSavedMeshData=0;
	Read(&cSavedMeshData,sizeof(int),pMem, seek);
	for(int cMeshData=0;cMeshData<cSavedMeshData;cMeshData++)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);

		int cSavedObject=0;
		Read(&cSavedObject,sizeof(int),pMem, seek);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			Read(&ReadObjectSceneID,sizeof(long),pMem, seek);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			Read(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,pMem, seek);
			Read(&ReadMatrixTM,sizeof(matrix),pMem, seek);
			Read(&ReadAlpha,sizeof(bool),pMem, seek);
			Read(&ReadLight,sizeof(bool),pMem, seek);

			AddNode->m_ObjectSceneID.push_back(ReadObjectSceneID);
			AddNode->m_strObjectName.push_back(ReadObjectName);
			AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(true);
			AddNode->m_isLight.push_back(true);
		}

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}
				
		
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_MeshMap.push_back(AddNode);
		m_MeshMapTable[iNode] = AddNode;

	}

	int cSavedEffectData=0;
	Read(&cSavedEffectData,sizeof(int),pMem, seek);
	for(int cEffectData=0;cEffectData<cSavedEffectData;cEffectData++)
	{
		CSectorEffectMap *AddNode=new CSectorEffectMap();
		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);
		int nEffect;
		Read(&nEffect,sizeof(int),pMem, seek);

		vector3 vecEffectPos;
		vector3 vecEffectRot;

		char* strEffectName;
		for(int cEffect=0;cEffect<nEffect;cEffect++)
		{			
			Read(&vecEffectPos,sizeof(vector3),pMem, seek);
			Read(&vecEffectRot,sizeof(vector3),pMem, seek);
			
			strEffectName=new char[256];
			Read(strEffectName,sizeof(char)*256,pMem, seek);

			AddNode->m_vecEffectPos.push_back(vecEffectPos);
			AddNode->m_strEffectName.push_back(strEffectName);
			AddNode->m_vecEffectRot.push_back(vecEffectRot);
			
		}
		
		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}
				
		
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_EffectMap.push_back(AddNode);
		m_EffectMapTable[iNode] = AddNode;
	}
	//landscape effect
	int cSaveLandscapeEffectData = 0;
	Read((int *)&cSaveLandscapeEffectData,sizeof(int),pMem, seek);
	for(int cLandscape = 0;cLandscape<cSaveLandscapeEffectData;cLandscape++) {
		CSectorLandscapeEffectMap *AddNode = new CSectorLandscapeEffectMap();
		CSectorLandscapeEffectMap ::LandscapeEffect *tmpLandscape;

		Read(&AddNode->m_IndexX,sizeof(int),pMem, seek);
		Read(&AddNode->m_IndexY,sizeof(int),pMem, seek);
		int nLandscapeEffect;
		Read(&nLandscapeEffect,sizeof(int),pMem, seek);
		
		for(int cLEffect =0;cLEffect<nLandscapeEffect;cLEffect++) {
			tmpLandscape = new CSectorLandscapeEffectMap ::LandscapeEffect();

			Read(&(tmpLandscape->m_nEffectMethod),sizeof(int),pMem, seek);
			Read(&(tmpLandscape->m_vecCenterPos),sizeof(vector3),pMem, seek);
			Read(tmpLandscape->m_Name,sizeof(char)* 50,pMem, seek);
			Read(tmpLandscape->m_Param,sizeof(float)* 50,pMem, seek);
			Read(tmpLandscape->m_IntParam,sizeof(int)* 20,pMem, seek);
			Read(tmpLandscape->m_dwBlend,sizeof(DWORD)*2,pMem, seek);

			AddNode->m_LandscapeEffect.push_back(*tmpLandscape);	
			delete tmpLandscape;

			//AddNode->m_LandscapeEffect.push_back
		}

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11 )
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

			
		m_LandscapeEffectMap.push_back(AddNode);
		m_LandscapeEffectMapTable[iNode] = AddNode;

	}
	// Sector Light Load
	if(feof(fp) == 0)
	{
		int iLightMapNum;
		Read((int *)&iLightMapNum,sizeof(int),pMem, seek);
		if(iLightMapNum >0 && iLightMapNum < 100)
		{
			for(int iLightMap = 0; iLightMap < iLightMapNum; iLightMap++)
			{
				CSectorLightMap *pAddNode = new CSectorLightMap;

				Read((int *)&(pAddNode->m_IndexX),sizeof(int),pMem, seek);
				Read((int *)&(pAddNode->m_IndexY),sizeof(int),pMem, seek);
				
				int iLightNum;
				Read((int *)&iLightNum,sizeof(int),pMem, seek);

				for(int iLight = 0; iLight < iLightNum; iLight++)
				{
					CSectorLight Node;
					Read((CSectorLight *)&(Node),sizeof(CSectorLight),pMem, seek);
					pAddNode->m_lstLights.push_back(Node);
				}
				
				if(pAddNode->m_IndexX < 0 || pAddNode->m_IndexX > 11 ||
					pAddNode->m_IndexY < 0 || pAddNode->m_IndexY > 11 )
				{
					delete pAddNode;
					continue;
				}
			
				int iNode = pAddNode->m_IndexX * 11 + pAddNode->m_IndexY;
				if(iNode < 0 || iNode >= MAXSECTOR)
				{
					//MessageBox(NULL,"오류","CMapStorage",MB_OK);
					delete pAddNode;
					continue;
				}

				m_LightMap.push_back(pAddNode);
				m_LightMapTable[iNode] = pAddNode;

			}
		}
	}

	if(pMem)
	{
		delete []pMem;
	}

	BugFix_Tree();
	//BugFix_Hangul2English();	

	//======================
	// SoundMap 
	//======================
	CAmbienceManager::_GetInstance()->AllStop(); 
	CAmbienceManager::_GetInstance()->GetAmbienceMap()->DeleteAmbData();

	CBgmManager::_GetInstance()->AllStop(); 
	CBgmManager::_GetInstance()->GetBGMTrigger()->DeleteBgmData();

	char Temp[MAX_PATH]; 
	char* token = strtok( strFilename, "Z" ); 
	token = strtok( NULL, "." ); 
	if( strcmp(token, "one3") == 0 ) return;    //배틀그라운드는 제외 
    if( strcmp(token, "one10") == 0 ) return;   //배틀그라운드 서버군은 제외 
    if( strcmp(token, "one11") == 0 ) return;   //배틀그라운드 서버군은 제외 
    if( strcmp(token, "one13") == 0 ) return;   //배틀그라운드 서버군은 제외 
	if( strcmp(token, "one15") == 0 ) return;	//캐릭터 선택화면은 제외
    if( strcmp(token, "one100") == 0 || strcmp(token, "one14") == 0 ) return;  // 100번존(감금존) 제외
	if( strcmp(token, "one16") == 0 ) return;

	sprintf( Temp, "%sZ%s.amb", AMBIENCEPATH, token ); 
//	CAmbienceManager::_GetInstance()->AllStop(); 
	CAmbienceManager::_GetInstance()->GetAmbienceMap()->LoadData( Temp );
	sprintf( Temp, "%sZ%s.bgm", BGMPATH, token );
//	CBgmManager::_GetInstance()->AllStop(); 
	CBgmManager::_GetInstance()->GetBGMTrigger()->LoadData( Temp );

	//LoadMapOutHouse();
}

void CMapStorage::LoadGF3(char *strFilename)
{
	FILE *fp=fopen(strFilename,"rb");		
	
	if(fp==NULL)
		return;

	//map search Table 초기화
	memset(m_DundeonMapTable,0,sizeof(CSectorDungeonMap *) * MAXSECTOR);
	memset(m_MeshMapTable,0,sizeof(CSectorMeshMap *) * MAXSECTOR);
	memset(m_PlantMapTable,0,sizeof(CSectorPlantMap *) * MAXSECTOR);
	memset(m_HouseMapTable,0,sizeof(CSectorHouseMap *) * MAXSECTOR);
	memset(m_WaterMapTable,0,sizeof(CSectorWaterMap *) * MAXSECTOR);
	memset(m_FallMapTable,0,sizeof(CSectorFallMap *) * MAXSECTOR);
	memset(m_InHouseMapTable,0,sizeof(CInHouseObjectMap *) * MAXSECTOR);
	memset(m_MustDivideVertexMapTable,0,sizeof(CSectorMustDivideVertexMap *) * MAXSECTOR);
	memset(m_EffectMapTable,0,sizeof(CSectorEffectMap *) * MAXSECTOR);
	memset(m_LightMapTable,0,sizeof(CSectorLightMap *) * MAXSECTOR);
	memset(m_LandscapeEffectMapTable,0,sizeof(CSectorLandscapeEffectMap *) * MAXSECTOR);
	memset(m_HeightMapTable,0,sizeof(CSectorHeightMap *) * MAXSECTOR);
	memset(m_WideMapTable,0,sizeof(CSectorWideMap *) * MAXSECTOR);


	strcpy(m_strMapFilename,strFilename);

	DeleteAllData();

	char strRead[MAX_NAMEBUFFER];
	fread(strRead,sizeof(char)*MAX_NAMEBUFFER,1,fp);	

	fread(&m_TotalHouseSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseLightSceneCount,sizeof(long),1,fp);
	fread(&m_TotalInHouseObjectSceneCount,sizeof(long),1,fp);
	fread(&m_TotalObjectSceneCount,sizeof(long),1,fp);

	// Height-Value Loading
	int cSavedHeightData=0;
	fread(&cSavedHeightData,sizeof(int),1,fp);	
	if(m_LoadMode==0)
	{
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			fread(&AddNode->m_IndexX,sizeof(int),1,fp);
			fread(&AddNode->m_IndexY,sizeof(int),1,fp);
			AddNode->m_pHeightData=new float[SECTORSX*SECTORSY];
			fread(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
			
			if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
				AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
			{
				delete AddNode;
				continue;
			}

			m_HeightMap.push_back(AddNode);
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				delete AddNode;
				continue;
			}
			
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}
	else
	{
		//float pHeightData[SECTORSX*SECTORSY];
		for(int cHeightData=0;cHeightData<cSavedHeightData;cHeightData++)
		{
			CSectorHeightMap *AddNode=new CSectorHeightMap();
			fread(&AddNode->m_IndexX,sizeof(int),1,fp);
			fread(&AddNode->m_IndexY,sizeof(int),1,fp);
			
			
			AddNode->m_pHeightData=new float[SECTORSX*SECTORSY];
			fread(AddNode->m_pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);
		

			if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
				AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
			{
				delete AddNode;
				continue;
			}

			/*AddNode->m_FilePos=ftell(fp);
			fread(&pHeightData,sizeof(float)*SECTORSX*SECTORSY,1,fp);			
			*/
			m_HeightMap.push_back(AddNode);
			/*
			int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;

			if(iNode < 0 || iNode >= MAXSECTOR)
			{
				delete AddNode;
				
				//MessageBox(NULL,"오류","CMapStorage",MB_OK);
				continue;
			}
			
			m_HeightMapTable[iNode] = AddNode;
			*/
		}
	}
	//Water loading
	int cSavedWaterData=0;
	fread(&cSavedWaterData,sizeof(int),1,fp);
	
	for(int cWaterData=0;cWaterData<cSavedWaterData;cWaterData++)
	{
		CSectorWaterMap *AddNode=new CSectorWaterMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fWaterPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeX,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterSizeY,sizeof(float),1,fp);
		fread(&AddNode->m_fWaterHeight,sizeof(float),1,fp);
		fread(&AddNode->m_bWaterRelection,sizeof(bool),1,fp);
		fread(&AddNode->m_WaterColor,sizeof(color),1,fp);		
		


		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}
				
	
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_WaterMap.push_back(AddNode);
		m_WaterMapTable[iNode] = AddNode;
	}

	//Widemap loading

//	FILE* tFp = fopen("C:\\ZoneInfo.txt", "wt");

	int cSavedWideData=0;
	fread(&cSavedWideData,sizeof(int),1,fp);

	for(int cWideData=0;cWideData<cSavedWideData;cWideData++)
	{
		CSectorWideMap *AddNode=new CSectorWideMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);

		fread(AddNode->m_strWidemapName[0],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strDetailName[0],sizeof(char)*MAX_NAMEBUFFER,1,fp);

		fread(AddNode->m_strWidemapName[1],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strDetailName[1],sizeof(char)*MAX_NAMEBUFFER,1,fp);

		fread(AddNode->m_strWidemapName[2],sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strDetailName[2],sizeof(char)*MAX_NAMEBUFFER,1,fp);

		if(CRenderOption::m_TerrainMultiDetalTexture)
		{			

		}
		else
		{
			strcpy(AddNode->m_strDetailName[0],"detail.dds");
			strcpy(AddNode->m_strWidemapName[1],"");
			strcpy(AddNode->m_strDetailName[1],"");
			strcpy(AddNode->m_strWidemapName[2],"");
			strcpy(AddNode->m_strDetailName[2],"");
		}
		

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

//		fprintf(tFp, "%d, %d, %d, %s\r\n", iNode, AddNode->m_IndexX, AddNode->m_IndexY, AddNode->m_strWidemapName[0]);

		m_WideMap.push_back(AddNode);
		m_WideMapTable[iNode] = AddNode;

	}

//	fclose(tFp);

	//Fallmap loading

	int cSavedFallData=0;
	fread(&cSavedFallData,sizeof(int),1,fp);

	for(int cFallData=0;cFallData<cSavedFallData;cFallData++)
	{
		CSectorFallMap *AddNode=new CSectorFallMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		fread(&AddNode->m_fFallPosX,sizeof(float),1,fp);
		fread(&AddNode->m_fFallPosY,sizeof(float),1,fp);
		fread(&AddNode->m_fFallRot,sizeof(float),1,fp);
		
		int cSavedFallList;
		fread(&cSavedFallList,sizeof(int),1,fp);
		for(int cFallList=0;cFallList<cSavedFallList;cFallList++)
		{
			float ReadFallData;
			color ReadFallColor;
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallHeightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallLeftList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallRightList.push_back(ReadFallData);
			fread(&ReadFallData,sizeof(float),1,fp);
			AddNode->m_FallAddXList.push_back(ReadFallData);
			fread(&ReadFallColor,sizeof(color),1,fp);
			AddNode->m_FallColor.push_back(ReadFallColor);
		}


		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}
		m_FallMap.push_back(AddNode);
		m_FallMapTable[iNode] = AddNode;
	}	

	// HouseMap loading

	int cSavedHouseData=0;
	fread(&cSavedHouseData,sizeof(int),1,fp);
	
	int IndexX, IndexY;

	for(int cHouseData=0;cHouseData<cSavedHouseData;cHouseData++)
	{
		fread(&IndexX,sizeof(int),1,fp);
		fread(&IndexY,sizeof(int),1,fp);

		CSectorHouseMap *AddNode=new CSectorHouseMap();
		int cSavedHouseList=0;		

		AddNode->m_IndexX = IndexX;
		AddNode->m_IndexY = IndexY;

		fread(&cSavedHouseList,sizeof(int),1,fp);

		for(int cHouseList=0;cHouseList<cSavedHouseList;cHouseList++)
		{
			int ReadHouseID;
			char *ReadHouseName;
			matrix ReadHouseTM;			
			fread(&ReadHouseTM,sizeof(matrix),1,fp);
			AddNode->m_TM.push_back(ReadHouseTM);
			fread(&ReadHouseID,sizeof(int),1,fp);
			AddNode->m_HouseSceneID.push_back(ReadHouseID);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);			
			AddNode->m_strOutName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strMedName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strInName.push_back(ReadHouseName);

			ReadHouseName=new char[MAX_NAMEBUFFER];
			fread(ReadHouseName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strBspName.push_back(ReadHouseName);
		}
		

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}

		int iNode = IndexX * 11 + IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			continue;
		}

		m_HouseMap.push_back(AddNode);
		m_HouseMapTable[iNode] = AddNode;
	}

	int cSavedInHouseData=0;
	fread(&cSavedInHouseData,sizeof(int),1,fp);
	for(int cInHouseData=0;cInHouseData<cSavedInHouseData;cInHouseData++)
	{
		CInHouseObjectMap *AddNode=new CInHouseObjectMap();
		
		fread(AddNode->m_strOutName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strMedName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strInName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
		fread(AddNode->m_strBspName,sizeof(char)*MAX_NAMEBUFFER,1,fp);

		strcpy(AddNode->m_strOutName,strlwr(AddNode->m_strOutName));
		strcpy(AddNode->m_strMedName,strlwr(AddNode->m_strMedName));
		strcpy(AddNode->m_strInName,strlwr(AddNode->m_strInName));
		strcpy(AddNode->m_strBspName,strlwr(AddNode->m_strBspName));

		int cSavedObject,cSavedLight;
		fread(&cSavedObject,sizeof(int),1,fp);
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			matrix ReadObjectTM;
			bool ReadAlpha,ReadLight;
			long ReadObjectID;
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strObjectNameList.push_back(ReadObjectName);	
			
			fread(&ReadObjectID,sizeof(long),1,fp);
			AddNode->m_ObjectID.push_back(ReadObjectID);

			fread(&ReadObjectTM,sizeof(matrix),1,fp);
			AddNode->m_matObjectList.push_back(ReadObjectTM);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			AddNode->m_isAlpha.push_back(true);
			fread(&ReadLight,sizeof(bool),1,fp);
			AddNode->m_isLight.push_back(true);
		}
		fread(&cSavedLight,sizeof(int),1,fp);
		for(int cLight=0;cLight<cSavedLight;cLight++)
		{
			matrix ReadLightTM;
			float ReadLightRange;
			color ReadLightColor;
			long ReadLightID;
			char *ReadLightName=new char[MAX_NAMEBUFFER];
			fread(ReadLightName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			AddNode->m_strLightNameList.push_back(ReadLightName);

			fread(&ReadLightID,sizeof(long),1,fp);
			AddNode->m_LightID.push_back(ReadLightID);
			fread(&ReadLightTM,sizeof(matrix),1,fp);
			AddNode->m_matLightList.push_back(ReadLightTM);
			fread(&ReadLightRange,sizeof(float),1,fp);
			AddNode->m_fLightRange.push_back(ReadLightRange);
			fread(&ReadLightColor,sizeof(color),1,fp);
			AddNode->m_LightColor.push_back(ReadLightColor);
		}		
		m_InHouseMap.push_back(AddNode);
				
	}
	
	// PlantMap loading
	int cSavedPlantData=0;
	fread(&cSavedPlantData,sizeof(int),1,fp);
	for(int cPlantData=0;cPlantData<cSavedPlantData;cPlantData++)
	{
		CSectorPlantMap *AddNode=new CSectorPlantMap();		
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);		

		int cSavedSubPlant;
		int ReadKind;
		unsigned char ReadPosX,ReadPosZ;
		//vector3 ReadPos;
		fread(&cSavedSubPlant,sizeof(int),1,fp);
		for(int cSubPlant=0;cSubPlant<cSavedSubPlant;cSubPlant++)
		{
			fread(&ReadKind,sizeof(unsigned char),1,fp);
			fread(&ReadPosX,sizeof(unsigned char),1,fp);
			fread(&ReadPosZ,sizeof(unsigned char),1,fp);
			//fread(&ReadPos,sizeof(vector3),1,fp);
			bool isAlready=false;
			for(int cNowPlant=0;cNowPlant<(int)AddNode->m_PlantKind.size();cNowPlant++)
			{
				if( AddNode->m_PlantPosX[cNowPlant]==ReadPosX &&
					AddNode->m_PlantPosZ[cNowPlant]==ReadPosZ)					
				{
					AddNode->m_PlantKind[cNowPlant]=ReadKind;
					AddNode->m_PlantPosX[cNowPlant]=ReadPosX;
					AddNode->m_PlantPosZ[cNowPlant]=ReadPosZ;
					isAlready=true;
				}
			}
			if(isAlready==false)
			{
				AddNode->m_PlantKind.push_back(ReadKind);
				AddNode->m_PlantPosX.push_back(ReadPosX);
				AddNode->m_PlantPosZ.push_back(ReadPosZ);
			}
			//AddNode->m_vecPlantPos.push_back(ReadPos);
		}


		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}
				
	
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_PlantMap.push_back(AddNode);
		m_PlantMapTable[iNode] = AddNode;
	}
	
	// MeshMap loading
	int cSavedMeshData=0;
	fread(&cSavedMeshData,sizeof(int),1,fp);
	for(int cMeshData=0;cMeshData<cSavedMeshData;cMeshData++)
	{
		CSectorMeshMap *AddNode=new CSectorMeshMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);

		int cSavedObject=0;
		fread(&cSavedObject,sizeof(int),1,fp);
		int ReadObjectSceneID;
		matrix ReadMatrixTM;
		bool ReadAlpha,ReadLight;
		for(int cObject=0;cObject<cSavedObject;cObject++)
		{
			fread(&ReadObjectSceneID,sizeof(long),1,fp);
			char *ReadObjectName=new char[MAX_NAMEBUFFER];
			fread(ReadObjectName,sizeof(char)*MAX_NAMEBUFFER,1,fp);
			fread(&ReadMatrixTM,sizeof(matrix),1,fp);
			fread(&ReadAlpha,sizeof(bool),1,fp);
			fread(&ReadLight,sizeof(bool),1,fp);

			AddNode->m_ObjectSceneID.push_back(ReadObjectSceneID);
			AddNode->m_strObjectName.push_back(ReadObjectName);
			AddNode->m_TM.push_back(ReadMatrixTM);
			AddNode->m_isAlpha.push_back(true);
			AddNode->m_isLight.push_back(true);
		}

				

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}
		
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_MeshMap.push_back(AddNode);
		m_MeshMapTable[iNode] = AddNode;

	}

	int cSavedEffectData=0;
	fread(&cSavedEffectData,sizeof(int),1,fp);
	for(int cEffectData=0;cEffectData<cSavedEffectData;cEffectData++)
	{
		CSectorEffectMap *AddNode=new CSectorEffectMap();
		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		int nEffect;
		fread(&nEffect,sizeof(int),1,fp);

		vector3 vecEffectPos;
		vector3 vecEffectRot;

		char* strEffectName;
		for(int cEffect=0;cEffect<nEffect;cEffect++)
		{			
			fread(&vecEffectPos,sizeof(vector3),1,fp);
			fread(&vecEffectRot,sizeof(vector3),1,fp);
			
			strEffectName=new char[256];
			fread(strEffectName,sizeof(char)*256,1,fp);

			AddNode->m_vecEffectPos.push_back(vecEffectPos);
			AddNode->m_strEffectName.push_back(strEffectName);
			AddNode->m_vecEffectRot.push_back(vecEffectRot);
			
		}
		

		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}
				
		
		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

		m_EffectMap.push_back(AddNode);
		m_EffectMapTable[iNode] = AddNode;
	}
	//landscape effect
	int cSaveLandscapeEffectData = 0;
	fread((int *)&cSaveLandscapeEffectData,sizeof(int),1,fp);
	for(int cLandscape = 0;cLandscape<cSaveLandscapeEffectData;cLandscape++) {
		CSectorLandscapeEffectMap *AddNode = new CSectorLandscapeEffectMap();
		CSectorLandscapeEffectMap ::LandscapeEffect *tmpLandscape;

		fread(&AddNode->m_IndexX,sizeof(int),1,fp);
		fread(&AddNode->m_IndexY,sizeof(int),1,fp);
		int nLandscapeEffect;
		fread(&nLandscapeEffect,sizeof(int),1,fp);
		
		for(int cLEffect =0;cLEffect<nLandscapeEffect;cLEffect++) {
			tmpLandscape = new CSectorLandscapeEffectMap ::LandscapeEffect();

			fread(&(tmpLandscape->m_nEffectMethod),sizeof(int),1,fp);
			fread(&(tmpLandscape->m_vecCenterPos),sizeof(vector3),1,fp);
			fread(tmpLandscape->m_Name,sizeof(char)* 50,1,fp);
			fread(tmpLandscape->m_Param,sizeof(float)* 50,1,fp);
			fread(tmpLandscape->m_IntParam,sizeof(int)* 20,1,fp);
			fread(tmpLandscape->m_dwBlend,sizeof(DWORD)*2,1,fp);

			AddNode->m_LandscapeEffect.push_back(*tmpLandscape);	
			delete tmpLandscape;

			//AddNode->m_LandscapeEffect.push_back
		}


		if(AddNode->m_IndexX < 0 || AddNode->m_IndexX > 11 ||
			AddNode->m_IndexY < 0 || AddNode->m_IndexY > 11)
		{
			delete AddNode;
			continue;
		}

		int iNode = AddNode->m_IndexX * 11 + AddNode->m_IndexY;
		if(iNode < 0 || iNode >= MAXSECTOR)
		{
			//MessageBox(NULL,"오류","CMapStorage",MB_OK);
			delete AddNode;
			continue;
		}

			
		m_LandscapeEffectMap.push_back(AddNode);
		m_LandscapeEffectMapTable[iNode] = AddNode;

	}
	// Sector Light Load
	if(feof(fp) == 0)
	{
		int iLightMapNum;
		fread((int *)&iLightMapNum,sizeof(int),1,fp);
		if(iLightMapNum >0 && iLightMapNum < 100)
		{
			for(int iLightMap = 0; iLightMap < iLightMapNum; iLightMap++)
			{
				CSectorLightMap *pAddNode = new CSectorLightMap;

				fread((int *)&(pAddNode->m_IndexX),sizeof(int),1,fp);
				fread((int *)&(pAddNode->m_IndexY),sizeof(int),1,fp);
				
				int iLightNum;
				fread((int *)&iLightNum,sizeof(int),1,fp);

				for(int iLight = 0; iLight < iLightNum; iLight++)
				{
					CSectorLight Node;
					fread((CSectorLight *)&(Node),sizeof(CSectorLight),1,fp);
					pAddNode->m_lstLights.push_back(Node);
				}
				
			

				if(pAddNode->m_IndexX < 0 || pAddNode->m_IndexX > 11 ||
					pAddNode->m_IndexY < 0 || pAddNode->m_IndexY > 11)
				{
					delete pAddNode;
					continue;
				}

				int iNode = pAddNode->m_IndexX * 11 + pAddNode->m_IndexY;
				if(iNode < 0 || iNode >= MAXSECTOR)
				{
					//MessageBox(NULL,"오류","CMapStorage",MB_OK);
					delete pAddNode;
					continue;
				}

				m_LightMap.push_back(pAddNode);
				m_LightMapTable[iNode] = pAddNode;

			}
		}
	}
	fclose(fp);
	BugFix_Tree();
	//BugFix_Hangul2English();	

	//======================
	// SoundMap 
	//======================
	CAmbienceManager::_GetInstance()->AllStop(); 
	CAmbienceManager::_GetInstance()->GetAmbienceMap()->DeleteAmbData();

	CBgmManager::_GetInstance()->AllStop(); 
	CBgmManager::_GetInstance()->GetBGMTrigger()->DeleteBgmData();

	char Temp[MAX_PATH]; 
	char* token = strtok( strFilename, "Z" ); 
	token = strtok( NULL, "." ); 
	if( strcmp(token, "one3") == 0 ) return;    //배틀그라운드는 제외 
    if( strcmp(token, "one10") == 0 ) return;   //배틀그라운드 서버군은 제외 
    if( strcmp(token, "one11") == 0 ) return;   //배틀그라운드 서버군은 제외 
    if( strcmp(token, "one13") == 0 ) return;   //배틀그라운드 서버군은 제외 
	if( strcmp(token, "one15") == 0 ) return;	//캐릭터 선택화면은 제외
    if( strcmp(token, "one100") == 0 || strcmp(token, "one14") == 0 ) return;  // 100번존(감금존) 제외
	if( strcmp(token, "one16") == 0 ) return;

	sprintf( Temp, "%sZ%s.amb", AMBIENCEPATH, token ); 
//	CAmbienceManager::_GetInstance()->AllStop(); 
	CAmbienceManager::_GetInstance()->GetAmbienceMap()->LoadData( Temp );
	sprintf( Temp, "%sZ%s.bgm", BGMPATH, token );
//	CBgmManager::_GetInstance()->AllStop(); 
	CBgmManager::_GetInstance()->GetBGMTrigger()->LoadData( Temp );

	//LoadMapOutHouse();
}

void CMapStorage::DeleteLandscapeEffect(CSectorLandscapeEffectMap* pmap){
	int del_map = 0;
	for(del_map = 0;del_map < (int)m_LandscapeEffectMap.size();del_map++) 
	{
		if(m_LandscapeEffectMap[del_map] == pmap) 
		{
			m_LandscapeEffectMap.erase(m_LandscapeEffectMap.begin()+del_map);
			return;
		}
	}
}
void CMapStorage::AddLandscapeEffect(float fAddx, float fAddy, float fAddz,int nEffectMethod,DWORD dwBlend[2],float Param[50],int IntParam[20],char *strTextureName,char *eff)
{
	/*

	*/

	int indexx=(int)(fAddx/SECTORSIZE);
	int indexy=(int)(fAddz/SECTORSIZE);
	bool new_sector = false;

	fAddy+=100.0f;
	
	for(int i=0;i<(int)m_LandscapeEffectMap.size();i++)
	{
		if( m_LandscapeEffectMap[i]->m_IndexX == indexx &&
			m_LandscapeEffectMap[i]->m_IndexY == indexy )
		{
			CSectorLandscapeEffectMap::LandscapeEffect AddEffectNode;
			AddEffectNode.m_vecCenterPos=vector3(fAddx,fAddy,fAddz);
			AddEffectNode.m_nEffectMethod=nEffectMethod;
			AddEffectNode.m_dwBlend[0]=dwBlend[0];
			AddEffectNode.m_dwBlend[1]=dwBlend[1];
			memcpy(AddEffectNode.m_Param,Param,sizeof(float)*50);
			memcpy(AddEffectNode.m_IntParam,IntParam,sizeof(int)*20);
			if(strTextureName != NULL)
				strncpy(AddEffectNode.m_Name,strTextureName,49);
			
		//	strcpy(AddEffectNode.m_EffName,eff);
			m_LandscapeEffectMap[i]->m_LandscapeEffect.push_back(AddEffectNode);
			new_sector = true;
		}
	}
	if(!new_sector) {
		CSectorLandscapeEffectMap *AddLandscapeEffect=new CSectorLandscapeEffectMap();
		AddLandscapeEffect->m_IndexX=indexx;
		AddLandscapeEffect->m_IndexY=indexy;

		CSectorLandscapeEffectMap::LandscapeEffect AddEffectNode;
		AddEffectNode.m_vecCenterPos=vector3(fAddx,fAddy,fAddz);
		AddEffectNode.m_nEffectMethod=nEffectMethod;
		AddEffectNode.m_dwBlend[0]=dwBlend[0];
		AddEffectNode.m_dwBlend[1]=dwBlend[1];
		memcpy(AddEffectNode.m_Param,Param,sizeof(float)*50);
		memcpy(AddEffectNode.m_IntParam,IntParam,sizeof(int)*20);
		if(strTextureName != NULL) 
			strncpy(AddEffectNode.m_Name,strTextureName,49);
	//	strcpy(AddEffectNode.m_EffName,eff);

		AddLandscapeEffect->m_LandscapeEffect.push_back(AddEffectNode);
		m_LandscapeEffectMap.push_back(AddLandscapeEffect);
	}
}

CSectorLandscapeEffectMap* CMapStorage::FindLandscapeEffect(float fFindx, float fFindy)
{
	int indexx=(int)(fFindx/SECTORSIZE);
	int indexy=(int)(fFindy/SECTORSIZE);
	int iNode = indexx * 11 + indexy; 
	
	if(iNode <0 || iNode >= MAXSECTOR)
		return NULL;

	if(m_LandscapeEffectMapTable[iNode])
	{
		return m_LandscapeEffectMapTable[iNode];
	}
/*
	for(int cStorage=0;cStorage<m_LandscapeEffectMap.num;cStorage++)
	{
		if( m_LandscapeEffectMap[cStorage]->m_IndexX==indexx && 
			m_LandscapeEffectMap[cStorage]->m_IndexY==indexy)
		{
			return m_LandscapeEffectMap[cStorage];
		}
	}
*/
	return NULL;	
}
