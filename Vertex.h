#ifndef _VERTEX_H
#define _VERTEX_H

#pragma once 

#include "3DMath.h"
#include <d3d.h>


const long MultiFVF=D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 ;

class MultiVertex{
public:
	vector3 v;
	color diff;
	color spec;
	float tu,tv;
	float tu1,tv1;	
};

//const long SectorFVF=D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_SPECULAR| D3DFVF_TEX2 ;
const long SectorFVF=D3DFVF_XYZ | D3DFVF_TEX1 ;

class SectorVertex{
public:
	vector3 v;
	//vector3 n;
	//color spec;
	float tu,tv;
	//float tu1,tv1;
};

const long LightFVF=D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_SPECULAR| D3DFVF_TEX1 ;

class LightVertex{
public:
	vector3 v;
	vector3 n;
	color spec;
	float tu,tv;	
};

const long DetailFVF=D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_TEX2 ;

class DetailVertex
{
public:
	vector3 v;
	vector3 n;
	float tu,tv;
	float tu1,tv1;
};

const long SimpleFVF=D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_SPECULAR |D3DFVF_TEX2;
class SimpleVertex {
public:
	vector3 v;
	color diff;
	color spec;
	float tu,tv;
	float tu1,tv1;
};

const long TLVERTEXFVF=D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
class TLVertex
{
public:
	vector3 v;
	float w;
	color Diffuse;
	color Specular;
	float tu,tv;	
};

const long TLVERTEX2FVF=D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2;
class TLVertex2
{
public:
	vector3 v;
	float w;
	color Diffuse;
	color Specular;
	float tu,tv;	
	float tu1,tv1;	
};


const long LVERTEXFVF=D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_SPECULAR |D3DFVF_TEX1;
class LVertex 
{
public:
	vector3 v;
	color diff;
	color spec;
	float tu,tv;
};

const long TTVERTEXFVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
class TTVertex
{
public:
	vector3 v;
	color diff;
};

const long BSPVERTEXFVF=D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2| D3DFVF_TEXCOORDSIZE3(1);
class BspVertex
{
public:
	vector3 v;
	vector3 normal;
	color diff;	
	float tu,tv;
	float tu1,tv1;
	vector3 u;
};

const long BSPSHADOWVERTEX=D3DFVF_XYZB1|D3DFVF_NORMAL;
class BspShadowVertex
{
public:
	vector3 v;
	float fWeight;
	vector3 n;
};

const long GRASSVERTEXFVF=D3DFVF_XYZB1|D3DFVF_DIFFUSE|D3DFVF_TEX1;

class GrassVertex
{
public:
	vector3 v;		
	float fWeight;	
	color diff;
	float tu,tv;	
};

const long TREEVERTEXFVF=D3DFVF_XYZB1|D3DFVF_NORMAL|D3DFVF_TEX1;

class TreeVertex
{
public:
	vector3 v;
	float fWeight;
	vector3 n;
	float tu,tv;
};

class Dot3Vertex
{
public:
	vector3 v;
	vector3 Normal;
	vector3 Diffuse;
	float tu,tv;
	vector3 s;
	vector3 t;
	vector3 SxT;

};

const long BUMPVERTEXFVF=D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1);

class BumpVertex 
{
public:
	vector3 v;
	vector3 n;
	float tu,tv;
	vector3 u;
};

const long BUMPVERTEX2FVF=D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE3(2);

class BumpVertex2
{
public:
	vector3 v;
	vector3 n;
	float tu,tv;
	float tu1,tv1;
	vector3 u;
};
#endif