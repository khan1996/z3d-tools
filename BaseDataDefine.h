#ifndef _BASEDATA_H
#define _BASEDATA_H

#pragma once

const long MAX_NAMEBUFFER=256;

extern char INITVALUEFILE[];
extern char MAPDATAPATH[];
extern char ROOTPATH[];

extern char CHARACTERDATAPATH[];
extern char CHARACTERTEXTUREDATAPATH[];
extern char CHARACTERMESHTEX[];
extern char CHARACTERANIPACK[];
extern char CHARACTERLODMESH[];
extern char CHARACTERTEXPIECE[];
extern char CHARACTERATTACHMENTMESH[];
extern char CHARACTERTEXTURE[];

extern char ORIGINALTEXUTREINTERFACEPATH[];
extern char ORIGINALTEXUTRELIGHTMAPPATH[];
extern char ORIGINALTEXUTRENATUREPATH[];
extern char ORIGINALTEXTUREOBJECTPATH[];
extern char ORIGINALTEXTURETERRAINPATH[];
extern char ORIGINALTEXTUREWIDEPATH[];
extern char ORIGINALTEXTUREFXPATH[];
extern char ORIGINALTEXTUREEFFECT[];

//Object Path Define//
extern char HOUSEOBJECTPATH[];
extern char LIGHTOBJECTPATH[];
extern char TREEOBJECTPATH[];
extern char OBJECTPATH[];
extern char EFFECTPATH[];
extern char EFFECTSOUNDSPATH[];
extern char EFFECTSOUNDMPATH[];


extern char INSTANCEOBJECTPATH[];
//Texture Path Define//
extern char TERRAINTEXTUREPATH[];
extern char OBJECTTEXTUREPATH[];
extern char NATURETEXTUREPATH[];
extern char WIDETEXTUREPATH[];
extern char NEOINTERFACETEXTUREPATH[];
extern char INTERFACETEXTUREPATH[];
extern char FXTEXTUREPATH[];
extern char BSPTEXTUREPATH[];
extern char BSPDATAPATH[];
extern char EFFECTTEXTUREPATH[];
extern char EFFECTSCRIPTPATH[];

extern char OBJECTGEMPATH[];
extern char UNSHADOWWIDETEXTUREPATH[];

// LightMap Path Define//
extern char LIGHTMAPTEXTUREPATH[];
//VertexShader Path Define//
extern char TERRAINVERTEXSHADEROATH[];

extern char SHADERPATH[];
//Save Data Path Define//
extern char WEATHERPATH[];
//Sound Data Path Define//
extern char SOUNDFILEPATH[];

//Octree Path Define//
extern char OCTREEPATH[];
//Sector Scene Map Path Define//
extern char SECTORMAPPATH[];
//Font Texture Filename//
extern char FONTFILE[];

extern float LENS_NEAR_HOUSE;
extern float LENS_NEAR_OBJECT;
extern long MOVE_NOTHING;
extern long MOVE_HOUSE;
extern long MOVE_LIGHT;
extern long MOVE_OBJECT;

extern float NEARPLANEVALUE;
extern float LIGHTMAPSIZE;
extern float MAX_ALLOWLIGHTMAPSIZE;
extern float PERLIGHTMAP;

extern long GF3OPTION;
extern long COLLISIONOBJECTVIEW;

// Character Lod Distance
extern float fCHARACTERLODLEVELDIST[];

//Ambience DataPath
extern char AMBIENCEPATH[]; 
//BGM DataPath 
extern char BGMPATH[]; 

/*
//Init Value File //
#define INITVALUEFILE "c:\\MP-Project\\Mp.INI"

//Original Texutre Path//
#define ORIGINALTEXUTREINTERFACEPATH "c:\\MP-Project\\RawTexture\\Interface"
#define ORIGINALTEXUTRELIGHTMAPPATH "c:\\MP-Project\\RawTexture\\Lightmap"
#define ORIGINALTEXUTRENATUREPATH "c:\\MP-Project\\RawTexture\\NatureObject"
#define ORIGINALTEXTUREOBJECTPATH "c:\\MP-Project\\RawTexture\\Object"
#define ORIGINALTEXTURETERRAINPATH "c:\\MP-Project\\RawTexture\\Terrain"
#define ORIGINALTEXTUREWIDEPATH "c:\\MP-Project\\RawTexture\\WideTexture"

//Object Path Define//
#define HOUSEOBJECTPATH "c:\\MP-Project\\Objects\\House\\"
#define LIGHTOBJECTPATH "c:\\MP-Project\\Objects\\Object\\"
#define TREEOBJECTPATH "c:\\MP-Project\\Objects\\NatureObject\\"
#define OBJECTPATH "c:\\MP-Project\\Objects\\Object\\"
//Texture Path Define//
#define TERRAINTEXTUREPATH "c:\\MP-project\\Texture\\Terrain"
#define OBJECTTEXTUREPATH "c:\\MP-Project\\Texture\\Object"
#define NATURETEXTUREPATH "c:\\MP-project\\Texture\\NatureObject"
#define WIDETEXTUREPATH "C:\\MP-Project\\Texture\\WideTexture"		
#define INTERFACETEXTUREPATH "c:\\MP-Project\\Texture\\Interface"

// LightMap Path Define//
#define LIGHTMAPTEXTUREPATH "c:\\MP-Project\\Texture\\Lightmap"
//VertexShader Path Define//
#define TERRAINVERTEXSHADEROATH "c:\\MP-Project\\VertexShader\\LayerFog\\LayerFogTerrain.vsh"
//Save Data Path Define//
#define WEATHERPATH "c:\\MP-Project\\WeatherColorTable.dat"

//Octree Path Define//
#define OCTREEPATH "c:\\MP-Project\\Octree"

//Sector Scene Map Path Define//
#define SECTORMAPPATH "c:\\MP-Project\\Map"
//Font Texture Filename//
#define FONTFILE "Font-H.dds"


const float LENS_NEAR_HOUSE=3000.0f;
const float LENS_NEAR_OBJECT=5000.0f;
const long MOVE_NOTHING=0;
const long MOVE_HOUSE=1;
const long MOVE_LIGHT=2;
const long MOVE_OBJECT=3;

const float NEARPLANEVALUE=3.0f;
const float LIGHTMAPSIZE=32;
const float MAX_ALLOWLIGHTMAPSIZE=50000.0f;
const float PERLIGHTMAP=30.0f;
*/
#endif
