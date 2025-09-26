#ifndef SECTORDEFINE_H
#define SECTORDEFINE_H

#pragma once

// Character Texture Size//
extern long CHRTEXTURESIZE;
// Max Character Projection Shadow count//
extern long MAXCHRSHADOW;
// HeightField & ROAM Data define....//
extern float DETAILGROUND;
extern long GROUNDSHADOW;
extern long SHADOWSIZE;
extern long LIGHTMAPRENDER;
extern long WATERREFLECTION;
extern long LENSFLARE;
const float LINTERVAL=492.3076f;
const int SECTORSX=65,SECTORSY=65;
const int SECTORSIZE=(int)((SECTORSX-1)*LINTERVAL);

const long LSIZEX=7; 
const long LSIZEY=7;

//extern int SECTORSX;
//extern int SECTORSY;
//extern int SECTORSIZE;
extern float DETAILTEXTUREDEPTH;
extern float FIELDHIGH;
extern float FIELDMIDDLE;
extern float FIELDLOW;
extern long ROAMBUFFERSIZE;
extern float FIELDDIVIDE;

const long PATCH=100;
const long VARIANCE_DEPTH=9;
const long POOL_SIZE=128000;
extern float TERRAINLOD;

//House Data define.......


extern float MAX_HOUSEVIEWRANGE;
extern float MAX_HOUSEDISAPPER;
extern float MAX_OUTVIEWRANGE;
extern float MAX_MEDVIEWRANGE;
extern float MAX_INVIEWRANGE;


extern float MAX_HOUSECOLLISION;

//Object Data define
extern float MAX_OBJECTVIEWRANBE;
extern float MAX_OBJECTDISAPPER;
//Nature Data define......//


const long MAX_SEED=20;
extern long MAX_SEEDRANGE;
extern float LENS_REALPOLYTREE;
const long MAX_TREE=SECTORSX*SECTORSY;
const long MAX_TREEKIND=30;


extern long SIZE_FOGTEXTURE;

extern float fFogHeight;
extern float fFogXRange;
extern float fFogYRange;
extern float fFogAddHeight;

extern float MAX_GRASS;
extern int MAX_GRASSRANGE;

extern float fFogMax;
extern float fFogMin;

extern float fSky_Height;
extern float fCloud_Height;

const long MAX_CLOUD=3;


//Sun Scene Data define..
extern float fSun_Length;
extern float fSun_FrontEye;
const long MAX_FLARE=7;

//Fall Scene Data Define ...
extern long MAX_FALLPARTICLE;

//Water Scene Data Define..
const long MAX_WATERTEXTURE=63;

/*
extern long LSIZEX=7;
extern long LSIZEY=7;
extern float LINTERVAL=500.0f;
extern int SECTORSX=64,SECTORSY=64;
extern int SECTORSIZE=(int)((SECTORSX-1)*LINTERVAL);
extern float DETAILTEXTUREDEPTH=50.0f;
extern float FIELDHIGH=13000.0f;
extern float FIELDMIDDLE=8000.0f;
extern float FIELDLOW=0.0f;
extern long PATCH=100;
extern long ROAMBUFFERSIZE=13;
enum HEIGHTSAVEMODE { LOWERDETAIL,HIGHDETAIL };
extern float FIELDDIVIDE=20.0f;
#define VARIANCE_DEPTH 9
#define POOL_SIZE 245000

//House Data define.......


extern float MAX_HOUSEVIEWRANGE=80000.0f;
extern float MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-4000.0f;
extern float MAX_OUTVIEWRANGE=15000.0f;
extern float MAX_MEDVIEWRANGE=15000.0f;
extern float MAX_INVIEWRANGE=3000.0f;


extern float MAX_HOUSECOLLISION=3000.0f;

//Object Data define
extern float MAX_OBJECTVIEWRANBE=10000.0f;
extern float MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-1000.0f;
//Nature Data define......//

extern long MAX_TREE=SECTORSX*SECTORSY;
extern long MAX_SEED=20;
extern long MAX_SEEDRANGE=30;
extern float LENS_REALPOLYTREE=5000.0f;
extern long MAX_TREEKIND=16;


extern long SIZE_FOGTEXTURE=64;

extern float fFogHeight=3000.0f;
extern float fFogXRange=5000.0f;
extern float fFogYRange=1000.0f;
extern float fFogAddHeight=100.0f;

extern float fFogMax=20000.0f;
extern float fFogMin=-1000.0f;

extern float fSky_Height=120000.0f;
extern float fCloud_Height=50000.0f;

extern long MAX_CLOUD=3;
//Sun Scene Data define..
extern float fSun_Length=200000.0f;
extern float fSun_FrontEye=10000.0f;
extern long MAX_FLARE=7;

//Fall Scene Data Define ...
extern long MAX_FALLPARTICLE=1000;

//Water Scene Data Define..
extern long MAX_WATERTEXTURE=63;
*/
#endif
