#include "BaseDataDefine.h"
#include "SectorDefine.h"
#include "GMMemory.h"

long SHADOWSIZE=256;
long MAXCHRSHADOW=4;
long GROUNDSHADOW=1;
long WATERREFLECTION=1;
long LIGHTMAPRENDER=1;
long CHRTEXTURESIZE=0;
//Init Value File //
char INITVALUEFILE[256]="c:\\MP-Project\\Mp.INI";

char MAPDATAPATH[256]="c:\\MP-Project\\MapData";

char ROOTPATH[256] = "c:\\MP-Project\\";

//Character DataSet Path//

char CHARACTERDATAPATH[256]="c:\\MP-Project\\Character\\Data\\";
char CHARACTERTEXTUREDATAPATH[256]="c:\\MP-Project\\Character\\Data\\tex\\";
char CHARACTERMESHTEX[256]="c:\\MP-Project\\Character\\Data\\meshtex.htable";
char CHARACTERANIPACK[256]="c:\\MP-Project\\Character\\Data\\ani\\APack.ntable";
char CHARACTERLODMESH[256]="c:\\MP-Project\\Character\\Data\\mesh\\LODMesh.ntable";
char CHARACTERTEXPIECE[256]="c:\\MP-Project\\Character\\Data\\tex\\Texpiece.ntable";
char CHARACTERATTACHMENTMESH[256]="c:\\MP-Project\\Character\\Data\\Amesh\\amesh.ntable";
char CHARACTERTEXTURE[256]="c:\\MP-Project\\Character\\Data\\Texture\\texture.ntable";


//Original Texutre Path//
char ORIGINALTEXUTREINTERFACEPATH[256]="c:\\MP-Project\\RawTexture\\Interface";
char ORIGINALTEXUTRELIGHTMAPPATH[256]="c:\\MP-Project\\RawTexture\\Lightmap";
char ORIGINALTEXUTRENATUREPATH[256]="c:\\MP-Project\\RawTexture\\NatureObject";
char ORIGINALTEXTUREOBJECTPATH[256]="c:\\MP-Project\\RawTexture\\Object";
char ORIGINALTEXTURETERRAINPATH[256]="c:\\MP-Project\\RawTexture\\Terrain";
char ORIGINALTEXTUREWIDEPATH[256]="c:\\MP-Project\\RawTexture\\WideTexture";
char ORIGINALTEXTUREFXPATH[256]="c:\\MP-Project\\Texture\\FX";
char ORIGINALTEXTUREEFFECT[256]="c:\\MP-Project\\Texture\\Effect";

//Object Path Define//
char HOUSEOBJECTPATH[256]="c:\\MP-Project\\Objects\\House\\";
char LIGHTOBJECTPATH[256]="c:\\MP-Project\\Objects\\Object\\";
char TREEOBJECTPATH[256]="c:\\MP-Project\\Objects\\NatureObject\\";
char OBJECTPATH[256]="c:\\MP-Project\\Objects\\Object\\";
char OBJECTGEMPATH[256] = "c:\\MP-Project\\Objects\\Gem\\";
char INSTANCEOBJECTPATH[256]="c:\\MP-Project\\Objects\\Instance\\";
//Texture Path Define//
char TERRAINTEXTUREPATH[256]="c:\\MP-project\\Texture\\Terrain";
char OBJECTTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Object";
char NATURETEXTUREPATH[256]="c:\\MP-project\\Texture\\NatureObject";
char WIDETEXTUREPATH[256]="C:\\MP-Project\\Texture\\WideTexture";	
char INTERFACETEXTUREPATH[256]="c:\\MP-Project\\Texture\\Interface";
char NEOINTERFACETEXTUREPATH[ 256 ]="c:\\MP-Project\\Texture\\Interface\\NeoInterface";
char FXTEXTUREPATH[256]="c:\\MP-Project\\Texture\\FX";
char BSPTEXTUREPATH[256]="c:\\MP-Project\\Texture\\BSP";
char BSPDATAPATH[256]="c:\\MP-Project\\MapData\\Bsp\\";

char EFFECTTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Effect\\";
char EFFECTSCRIPTPATH[256] = "c:\\MP-Project\\Effect\\Esf\\";


char UNSHADOWWIDETEXTUREPATH[256]="c:\\MP-Project\\Texture\\UnShadowWideTexture";

// LightMap Path Define//
char LIGHTMAPTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Lightmap";
//VertexShader Path Define//
char TERRAINVERTEXSHADEROATH[256]="c:\\MP-Project\\VertexShader\\LayerFog\\LayerFogTerrain.vsh";
char SHADERPATH[256]="c:\\MP-Project\\Shader\\";
//Save Data Path Define//
char WEATHERPATH[256]="c:\\MP-Project\\WeatherColorTable.dat";

//Octree Path Define//
char OCTREEPATH[256]="c:\\MP-Project\\Octree";
//Sector Scene Map Path Define//
char SECTORMAPPATH[256]="c:\\MP-Project\\Map";
//Font Texture Filename//
char FONTFILE[256]="Font-H.dds";
char EFFECTPATH[256] = "c:\\MP-Project\\Effect\\";

//Sound Path//
char SOUNDFILEPATH[256]="c:\\MP-Project\\Sound\\";
char EFFECTSOUNDSPATH[256] = "c:\\MP-Project\\Sound\\Effect\\Ssnd\\";
char EFFECTSOUNDMPATH[256] = "c:\\MP-Project\\Sound\\Effect\\Msnd\\";

float DETAILGROUND=5.0f;

float LENS_NEAR_HOUSE=30000.0f;
float LENS_NEAR_OBJECT=5000.0f;
long MOVE_NOTHING=0;
long MOVE_HOUSE=1;
long MOVE_LIGHT=2;
long MOVE_OBJECT=3;

float NEARPLANEVALUE=3.0f;
float LIGHTMAPSIZE=32;
float MAX_ALLOWLIGHTMAPSIZE=50000.0f;
float PERLIGHTMAP=30.0f;

float DETAILTEXTUREDEPTH=75.0f;
float FIELDHIGH=13000.0f;
float FIELDMIDDLE=8000.0f;
float FIELDLOW=0.0f;

long ROAMBUFFERSIZE=6;
enum HEIGHTSAVEMODE { LOWERDETAIL,HIGHDETAIL };
float FIELDDIVIDE=64.0f;


//House Data define.......

///*
float MAX_HOUSEVIEWRANGE=80000.0f;
float MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-4000.0f;
float MAX_OUTVIEWRANGE=15000.0f;
float MAX_MEDVIEWRANGE=10000.0f;
float MAX_INVIEWRANGE=4200.0f;
//*/
/*
float MAX_HOUSEVIEWRANGE=15000.0f;
float MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-4000.0f;
float MAX_OUTVIEWRANGE=15000.0f;
float MAX_MEDVIEWRANGE=15000.0f;
float MAX_INVIEWRANGE=3000.0f;
//*/


float MAX_HOUSECOLLISION=30000.0f;

//Object Data define
float MAX_OBJECTVIEWRANBE=10000.0f;
float MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-1000.0f;
//Nature Data define......//

long MAX_SEEDRANGE=30;
float LENS_REALPOLYTREE=5000.0f;

// Grass Data define........//
float MAX_GRASS=100;
int MAX_GRASSRANGE=19;

long SIZE_FOGTEXTURE=64;

float fFogHeight=3000.0f;
float fFogXRange=5000.0f;
float fFogYRange=1000.0f;
float fFogAddHeight=100.0f;


float TERRAINLOD = 10000.f;

float fFogMax=20000.0f;
float fFogMin=-1000.0f;

float fSky_Height=120000.0f;
float fCloud_Height=50000.0f;


//Sun Scene Data define..
float fSun_Length=200000.0f;
float fSun_FrontEye=10000.0f;
//Lens Flare Data Define..
long LENSFLARE=4;

//Fall Scene Data Define ...
long MAX_FALLPARTICLE=1000;

long GF3OPTION=0;
long COLLISIONOBJECTVIEW=0;

float fCHARACTERLODLEVELDIST[3] = {1700.0f,3000.0f,4500.0f};

//Ambience DataPath
char AMBIENCEPATH[256] = "c:\\MP-Project\\SOUND\\Amb\\"; 
//BGM DataPath 
char BGMPATH[256] = "c:\\MP-Project\\SOUND\\Bgm\\"; 
