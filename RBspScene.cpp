// RBspScene.cpp: implementation of the RBspScene class.
//
//////////////////////////////////////////////////////////////////////

#include "RBspScene.h"
#include "SceneManager.h"
#include "Sphere.h"
#include "BaseGraphicsLayer.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

D3DXVECTOR3 g_vecCube[8] = {
	D3DXVECTOR3(-50.0f,-50.0f,-50.0f),	// 0
	D3DXVECTOR3(-50.0f,50.0f,-50.0f),		// 1
	D3DXVECTOR3(50.0f,-50.0f,-50.0f),		// 2
	D3DXVECTOR3(50.0f,50.0f,-50.0f),		// 3

	D3DXVECTOR3(-50.0f,-50.0f,50.0f),	// 4
	D3DXVECTOR3(-50.0f,50.0f,50.0f),		// 5
	D3DXVECTOR3(50.0f,-50.0f,50.0f),		// 6
	D3DXVECTOR3(50.0f,50.0f,50.0f),		// 7

};
WORD g_vecCubeIndex[36] = {
	0,1,2,2,1,3,	// 앞면
	6,5,4,7,5,6,	// 뒷면
	1,5,3,3,5,7,	// 윗면
	2,4,0,6,4,2,	// 아랫면	
	2,3,6,6,3,7,	// 오른면
	4,5,0,0,5,1,	// 왼면
};

RBspScene::RBspScene()
{
	memset(m_strBspName,0,sizeof(char) * RBSPBUF_SIZE);
	m_vecStartPos = vector3(0.0f,0.0f,0.0f);
	m_bVisible = false;

	m_pDotVertex = NULL;

	// Leaf Faces
	m_iLeafFacesNum = 0;
	m_pLeafFaces = NULL;
	
	// Plane 
	m_iPlanesNum = 0;
	m_pPlanes = NULL;
	// Leaf Brush
	m_iLeafBrushesNum = 0;
	m_pLeafBrushes = NULL;           // The index into the brush array 
	// Brush 
	m_iBrushesNum = 0;
	m_pBrushes = NULL;
	// Brush Side
	m_iBrusheSidesNum = 0;
	m_pBrusheSides = NULL;
	
		
	// Leaf Node
	m_iLeafsNum = 0;
	m_pLeafs = NULL;
	// Node
	m_iNodesNum = 0;
	m_pNodes = NULL;
	// Lightmap
	m_iLightMapsNum = 0;
	m_pLightMaps = NULL;
	m_pLightTextures = NULL;
	// Texture Def
	m_iTextureDefsNum = 0;
	m_pTextureDefs = NULL;
	m_pTextures = NULL;
	m_pNTextures = NULL;
	m_pFrustum = NULL;

	// Faces
	m_iFacesNum = 0;
	m_pFaces = NULL;
	// Vertexs
	m_iVertsNum = 0;
	m_pBeforeVerts = NULL;			// 원래 bsp 에서 사용하는 vertex class
	m_pVerts = NULL;					// 최적화를 위한 vertex class
	
	m_iVisLeafNodeNum = 0;

	// Triangle List

	// Base Texture Vertex Buffer
	m_pBaseVertexBuffer = NULL;
	// LightMap Vertex Buffer
	m_pLightVertexBuffer = NULL;
	m_pBaseVertexNum = NULL;
	m_pBaseVertexUseNum = NULL;

	// Base Index Buffer
	m_pBaseIndexBuffer = NULL;
	m_pBaseIndexNum = NULL;
	m_pBaseIndexUseNum = NULL;
	
	

	m_pIndexArr.clear();
	m_pVertexArr.clear();
	m_pLightUnit = NULL;

	// Current Camera Position
	m_vecCamera = D3DXVECTOR3(0.0f,0.0f,0.0f);
	
	m_iShaderVersion = RBSPSHADER_GF2;
	m_iRBspLeafIndex = -1;

	m_pVisLeafs = NULL;				// Visible Leaf
	m_iVisLeafs = 0;					// Visible Leaf 갯수

	m_pBVisLeafs = NULL;				// Visible Leaf
	m_iBVisLeafs = 0;					// Visible Leaf 갯수


	m_pLeafExs = NULL;
	m_D3Device = NULL;


	m_bVisEffectBox = true;
	m_bVisSoundBox = true;
	m_bVisAObjectBox = true;
	m_bVisLightBox = true;

	m_vecScale = D3DXVECTOR3(4.0f,4.0f,4.0f);


}

RBspScene::~RBspScene()
{
	int i;

	if(m_pDotVertex != NULL)
	{
		delete[] m_pDotVertex;
		m_pDotVertex = NULL;
	}
	// Leaf Faces
	if(m_pLeafFaces != NULL)
	{
		delete[] m_pLeafFaces;
		m_pLeafFaces = NULL;
	}
	// Plane 
	if(m_pPlanes != NULL)
	{
		delete[] m_pPlanes;
		m_pPlanes = NULL;
	}
	// Leaf Brush
	if(m_pLeafBrushes != NULL)
	{
		delete[] m_pLeafBrushes;
		m_pLeafBrushes = NULL;           // The index into the brush array 
	}
	// Brush 
	if(m_pBrushes != NULL)
	{
		delete[] m_pBrushes;
		m_pBrushes = NULL;
	}
	// Brush Side
	if(m_pBrusheSides != NULL)
	{
		delete[] m_pBrusheSides;
		m_pBrusheSides = NULL;
	}	
	// Leaf Node
	if(m_pLeafs != NULL)
	{
		delete[] m_pLeafs;
		m_pLeafs = NULL;
	}
	if(m_pLeafExs != NULL)
	{
		delete[] m_pLeafExs;
		m_pLeafExs = NULL;

	}
	// Node
	if(m_pNodes != NULL)
	{
		delete[] m_pNodes;
		m_pNodes = NULL;
	}

	// Lightmap
	if(m_pLightMaps != NULL)
	{
		delete[] m_pLightMaps;
		m_pLightMaps = NULL;
	}
	if(m_pLightTextures != NULL)
	{	
		delete[] m_pLightTextures;
		m_pLightTextures = NULL;

	}
	// Texture Def
	if(m_pTextureDefs != NULL)
	{
		delete[] m_pTextureDefs;
		m_pTextureDefs = NULL;
	}
	if(m_pTextures != NULL)
	{
		delete[] m_pTextures;
		m_pTextures = NULL;

	}
	if(m_pNTextures != NULL)
	{	
		delete[] m_pNTextures;
		m_pNTextures = NULL;
	}
	// Faces
	if(m_pFaces != NULL)
	{
		delete[] m_pFaces;
		m_pFaces = NULL;
	}
	if(m_pFrustum != NULL)
	{
		delete m_pFrustum;
		m_pFrustum = NULL;

	}
	// Vertexs
	if(m_pBeforeVerts != NULL)
	{
		delete[] m_pBeforeVerts;
		m_pBeforeVerts = NULL;			// 원래 bsp 에서 사용하는 vertex class
	}
	if(m_pVerts != NULL)
	{
		delete[] m_pVerts;
		m_pVerts = NULL;					// 최적화를 위한 vertex class
	}
	for(i=0;i<m_iTextureDefsNum;i++)
	{
		delete[] m_pIndexArr[i];
		m_pIndexArr[i] = NULL;
		delete[] m_pVertexArr[i];
		m_pVertexArr[i] = NULL;
		m_pLightUnit[i].clear();
	}
	m_pIndexArr.clear();
	m_pVertexArr.clear();
	delete[] m_pLightUnit;

	if(m_pBaseVertexBuffer != NULL)
	{
		for( i = 0; i < m_iTextureDefsNum; i++)
		{
			SAFE_RELEASE(m_pBaseVertexBuffer[i]);
			m_pBaseVertexBuffer[i] = NULL;
		}
		delete[] m_pBaseVertexBuffer;
		m_pBaseVertexBuffer = NULL;
		delete[] m_pBaseVertexNum;
		m_pBaseVertexNum = NULL;
		delete[] m_pBaseVertexUseNum;
		m_pBaseVertexUseNum = NULL;

	}
	if(m_pBaseIndexBuffer != NULL)
	{
		for( i = 0; i < m_iTextureDefsNum; i++)
		{
			SAFE_RELEASE(m_pBaseIndexBuffer[i]);
			m_pBaseIndexBuffer[i] = NULL;
		}
		delete[] m_pBaseIndexBuffer;
		m_pBaseIndexBuffer = NULL;
		delete[] m_pBaseIndexNum;
		m_pBaseIndexNum = NULL;
		delete[] m_pBaseIndexUseNum;
		m_pBaseIndexUseNum = NULL;
	}
	if(m_pVisLeafs != NULL)
	{
		delete[] m_pVisLeafs;
		m_pVisLeafs = NULL;
	}
	if(m_pBVisLeafs != NULL)
	{
		delete[] m_pBVisLeafs;
		m_pBVisLeafs = NULL;
	}

	
}
void RBspScene::WritetoFile(FILE *fp,char *str) {
	assert(fp != NULL);
	assert(str != NULL);

	fwrite((char *)str,sizeof(char),RBSPBUF_SIZE,fp);
	
}
/*
bool RBspScene::LoadBseScene(const char *strFilename)	// Read Converted Quake3 Bsp File
{	
/*	int i;
	if(strFilename != NULL) {
		FILE *fp = fopen(strFilename,"rb");
		if(fp == NULL)
			return false;
		char *strHeader = NULL;
		int iHeaderLength = 0;
		fread((int *)&iHeaderLength,sizeof(int),1,fp);
		strHeader = new char[iHeaderLength + 1];
		fread((char *)strHeader,sizeof(char),iHeaderLength + 1,fp);
		
		if(strcmp("BseFile",strHeader)) {
			MessageBox(NULL,"Load File Not Bse File",strHeader,MB_OK);
			fclose(fp);
			return false;
		}
		int arrNum[11];

		fread((int *)arrNum,sizeof(int),11,fp);
		// VisData Load
		int iAllocSize = 0;

		fread((int *)&(m_VisData.m_iBytesPerCluster),sizeof(int),1,fp);
		fread((int *)&(m_VisData.m_iClustersNum),sizeof(int),1,fp);
		fread((int *)&iAllocSize,sizeof(int),1,fp);
		m_VisData.m_pBitsets = new byte[iAllocSize];
		fread((byte *)(m_VisData.m_pBitsets),sizeof(byte),iAllocSize,fp);
		// Bsp Info Num Setting
		m_iVertsNum = arrNum[0];
		m_iFacesNum = arrNum[1];
		m_iTextureDefsNum = arrNum[2];
		m_iLightMapsNum = arrNum[3];
		m_iNodesNum = arrNum[4];
		m_iLeafsNum = arrNum[5];
		m_iLeafFacesNum = arrNum[6];
		m_iPlanesNum = arrNum[7];
		m_iBrushesNum = arrNum[8];
		m_iBrusheSidesNum = arrNum[9];
		m_iLeafBrushesNum = arrNum[10];
		
		m_pFaces = new RBspFace[m_iFacesNum];
		m_pTextureDefs = new RBspTextureDef[m_iTextureDefsNum];
		m_pLightMaps = new RBspLightMap[m_iLightMapsNum];
		m_pLightTextures = new WBTexture[ m_iLightMapsNum ];
		m_pNodes = new RBspNode[m_iNodesNum];
		m_pLeafs = new RBspLeaf[m_iLeafsNum];
		m_pLeafFaces = new int[m_iLeafFacesNum];
		m_pLeafBrushes = new int[m_iLeafBrushesNum];
		m_pPlanes = new RBspPlane[m_iPlanesNum];
		m_pBrushes = new RBspBrushe[m_iBrushesNum];
		m_pBrusheSides = new RBspBrusheSide[m_iBrusheSidesNum];
		m_pDotVertex = new RBseVertex[m_iVertsNum];
		
		// Base Texture Buffer Create
		m_pBaseVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];
		m_pLightVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];

		m_pBaseVertexNum = new int[m_iTextureDefsNum];
		m_pBaseVertexUseNum = new int[m_iTextureDefsNum];

		m_pBaseIndexBuffer = new LPDIRECT3DINDEXBUFFER8[m_iTextureDefsNum];
		m_pBaseIndexNum = new int[m_iTextureDefsNum];
		m_pBaseIndexUseNum = new int[m_iTextureDefsNum];
		
		m_pLightUnit = new LightUnitType[m_iTextureDefsNum];

		m_pTextures = new WBTexture[ m_iTextureDefsNum ];
		m_pNTextures = new WBTexture[ m_iTextureDefsNum];


		// Face Read
		fread((RBspFace *)m_pFaces,sizeof(RBspFace),m_iFacesNum,fp);
		// Texture Define Read
		fread((RBspTextureDef *)m_pTextureDefs,sizeof(RBspTextureDef),m_iTextureDefsNum,fp);
		//// Load Texture Routine
		for(i = 0; i < m_iTextureDefsNum; i++ )
		{
			m_pBaseVertexBuffer[i] = NULL;
			m_pLightVertexBuffer[i] = NULL;
			m_pBaseVertexNum[i] = 0;
			m_pBaseVertexUseNum[i] = 0;
			m_pBaseIndexBuffer[i] = NULL;
			m_pBaseIndexNum[i] = 0;
			m_pBaseIndexUseNum[i] = 0;

			char strTex[256] = {0};
			strcpy(strTex,m_pTextureDefs[i].m_strName);
			strcat(strTex,".tga");
			if(!m_pTextures[i].LoadTexture(strTex)) {		// tga 파일 로드 시도
				strcpy(strTex,m_pTextureDefs[i].m_strName);
				strcat(strTex,".jpg");
				m_pTextures[i].LoadTexture(strTex);			// 없을시 jpg 로드 시도
			}
			memset(strTex,0,sizeof(char) * 256);
			strcpy(strTex,m_pTextureDefs[i].m_strName);
			strcat(strTex,"_Normal.bmp");
			m_pNTextures[i].LoadTexture(strTex);
		}

		// Lightmap Read
		fread((RBspLightMap *)m_pLightMaps,sizeof(RBspLightMap),m_iLightMapsNum,fp);
		// Setting LightMap
		for( i = 0; i < m_iLightMapsNum; i++ )
		{
			WBTexture::ChangeTextureIntension(m_pLightMaps[i].m_ucLightMap,128 * 128 * 3,20.0f);
			byte m_ucLight[65536];
			int icount = 0;
			int icount2 = 0;
			for(int j=0;j < 128;j++) {
				for(int k=0;k < 128;k++) {
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+2];		//R
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+1];		//G
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount];		//B
					m_ucLight[icount2++] = 0;
					icount +=3;
				}
			}

			m_pLightTextures[i].CreateEmptyTexture(128,128,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED);
			//m_pLightTextures[i].FillTexture(m_pLightMaps[i].m_ucLightMap);
			m_pLightTextures[i].FillTexture(m_ucLight);
		}

		// Node Read
		fread((RBspNode *)m_pNodes,sizeof(RBspNode),m_iNodesNum,fp);
		// Leaf Read
		fread((RBspLeaf *)m_pLeafs,sizeof(RBspLeaf),m_iLeafsNum,fp);
		// Leaf Face Read
		fread((int *)m_pLeafFaces,sizeof(int),m_iLeafFacesNum,fp);
		// Leaf Brushes Read
		fread((int *)m_pLeafBrushes,sizeof(int),m_iLeafBrushesNum,fp);
		// Plane Read
		fread((RBspPlane *)m_pPlanes,sizeof(RBspPlane),m_iPlanesNum,fp);
		// Brushe Read
		fread((RBspBrushe *)m_pBrushes,sizeof(RBspBrushe),m_iBrushesNum,fp);
		// BrusheSide Read
		fread((RBspBrusheSide *)m_pBrusheSides,sizeof(RBspBrusheSide),m_iBrusheSidesNum,fp);
		// Vertex Data Read Read
		fread((RBseVertex *)m_pDotVertex,sizeof(RBseVertex),m_iVertsNum,fp);

		fclose(fp);
		m_FaceRenderBit.ResizeBits(m_iFacesNum);		// Face Render Bit Resize
		// Memory pool Create
		CreateBufferNum2();
		CreateBuffer();

		m_pFrustum = new WBFrustum;

		
		m_pShader = new WBBumpDSShader;
		m_pShadowShader = new RBspShadowShader;
		m_pFogShader = new RLayerFogShader;

	}
	return true;*/
/*	return true;
}*/
bool RBspScene::LoadBspScene(char *strFileName,float fSx,float fSy,float fSz)
{
	
	SetBspScale(fSx,fSy,fSz);
	
	char strBspScene[256];
	memset(strBspScene,0,sizeof(char) * 256);
	strcpy(strBspScene,BSPDATAPATH);
	strcat(strBspScene,strFileName);
	strcpy(m_strBspName,strFileName);
	
	FILE *fp = fopen(strBspScene,"rb");
	if(fp == NULL)
		return false;
	if(!LoadBspHeader(fp))					// Read Quake3 Bsp File Header Load
	{
		fclose(fp);
		return false;
	}
	if(!LoadBspLump(fp))					// Read Quake3 Lump File Load
	{
		fclose(fp);
		return false;
	}
	fclose(fp);
	
	return true;

}
bool RBspScene::LoadBspHeader(FILE *fp)		// Read Quake3 Bsp File Header Load
{
	if(fp == NULL)
		return false;
	
	fread((RBspHeader *)&m_Header,sizeof(RBspHeader),1,fp);
	
	 
	if(m_Header.m_iVersion != 0x2e || strcmp(m_Header.m_strID,"IBSP."))	// Check Quake3 Bsp File	
		return false;
	fread((RBspLump *)m_arrLump,sizeof(RBspLump),TOTAL_LUMP,fp);

	return true;
}
void RBspScene::SetBspScaleLoaded(float x,float y,float z)
{
	int i;

	D3DXVECTOR3 vecBeforeScale = m_vecScale;
	m_vecScale = D3DXVECTOR3(x,y,z);
	D3DXVECTOR3 vecScale = D3DXVECTOR3((x / vecBeforeScale.x),
										(y / vecBeforeScale.y),
										(z / vecBeforeScale.z));

	
	for(i=0;i < m_iVertsNum;i++) 
	{
		/// Scale
		m_pVerts[i].m_vecPos.x *= vecScale.x;
		m_pVerts[i].m_vecPos.y *= vecScale.y;
		m_pVerts[i].m_vecPos.z *= vecScale.z;	
	}

	for(i=0;i < m_iNodesNum; i++) {

		m_pNodes[i].m_vecMinBox[0] *= (vecScale.x);
		m_pNodes[i].m_vecMinBox[1] *= (vecScale.y);
		m_pNodes[i].m_vecMinBox[2] *= (vecScale.z);
		
		m_pNodes[i].m_vecMaxBox[0] *= (vecScale.x);
		m_pNodes[i].m_vecMaxBox[1] *= (vecScale.y);
		m_pNodes[i].m_vecMaxBox[2] *= (vecScale.z);

	}
	for(i=0;i < m_iLeafsNum; i++) {	
		m_pLeafExs[i].m_vecMinBox[0] *= (vecScale.x);
		m_pLeafExs[i].m_vecMinBox[1] *= (vecScale.y);
		m_pLeafExs[i].m_vecMinBox[2] *= (vecScale.z);

		m_pLeafExs[i].m_vecMaxBox[0] *= (vecScale.x);
		m_pLeafExs[i].m_vecMaxBox[1] *= (vecScale.y);
		m_pLeafExs[i].m_vecMaxBox[2] *= (vecScale.z);
	}
	for(i=0;i<m_iPlanesNum;i++) {
		// Scale 은 정형 이어야 한다
		m_pPlanes[i].m_fDistance *= vecScale.x;

	}
	

}
bool RBspScene::LoadBspLump(FILE *fp)		
{
	int i=0;

	// Get Number Lumps
	m_iVertsNum = m_arrLump[	RBSP_VERTICES ].m_iLength / sizeof(RBspVertex);
	m_iFacesNum = m_arrLump[ RBSP_FACES ].m_iLength / sizeof(RBspFace);
	m_iTextureDefsNum = m_arrLump[ RBSP_TEXTURES].m_iLength / sizeof(RBspTextureDef);
	m_iLightMapsNum = m_arrLump[ RBSP_LIGHTMAP ].m_iLength / sizeof(RBspLightMap);
	m_iNodesNum = m_arrLump[ RBSP_NODES ].m_iLength / sizeof(RBspNode);
	
	m_iLeafsNum  = m_arrLump[ RBSP_LEAFS ].m_iLength / sizeof(RBspLeaf);
	m_iLeafFacesNum = m_arrLump[ RBSP_LEAFFACES ].m_iLength / sizeof(int);
	// Brush 
	m_iLeafBrushesNum = m_arrLump[ RBSP_LEAFBRUSHES ].m_iLength / sizeof(int);
	m_iBrushesNum  = m_arrLump[ RBSP_BRUSHES ].m_iLength / sizeof(RBspBrushe);
	m_iBrusheSidesNum = m_arrLump[ RBSP_BRUSHESIDES ].m_iLength / sizeof(RBspBrusheSide);
	
	m_iPlanesNum = m_arrLump[ RBSP_PLANES ].m_iLength / sizeof(RBspPlane);
	
	m_pVerts = new RBspDrawVertex[ m_iVertsNum ];
	m_pBeforeVerts = new RBspVertex[ m_iVertsNum ];

	m_pFaces = new RBspFace[ m_iFacesNum ];

	m_pTextureDefs = new RBspTextureDef[ m_iTextureDefsNum ];
	m_pLightMaps = new RBspLightMap[ m_iLightMapsNum ];
	m_pLightTextures = new CTexture[ m_iLightMapsNum ];
	m_pNodes = new RBspNode[ m_iNodesNum ];
	m_pLeafs = new RBspLeaf[ m_iLeafsNum ];
	m_pLeafExs = new RBspLeafEx[ m_iLeafsNum ];
	m_pVisLeafs = new int[ m_iLeafsNum ];
	m_pBVisLeafs = new int[ m_iLeafsNum ];

	m_pLeafFaces = new int[ m_iLeafFacesNum ];
	m_pPlanes = new RBspPlane[ m_iPlanesNum ];
	m_pTextures = new CTexture[ m_iTextureDefsNum ];
	//m_pNTextures = new CTexture[ m_iTextureDefsNum];

	m_pBrushes = new RBspBrushe[ m_iBrushesNum ];
	m_pBrusheSides = new RBspBrusheSide[ m_iBrusheSidesNum ];
	m_pLeafBrushes = new int[ m_iLeafBrushesNum ];

	// Base Texture Buffer Create
	m_pBaseVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];
	//m_pLightVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];

	m_pBaseVertexNum = new int[m_iTextureDefsNum];
	m_pBaseVertexUseNum = new int[m_iTextureDefsNum];

	m_pBaseIndexBuffer = new LPDIRECT3DINDEXBUFFER8[m_iTextureDefsNum];
	m_pBaseIndexNum = new int[m_iTextureDefsNum];
	m_pBaseIndexUseNum = new int[m_iTextureDefsNum];
	
	m_pLightUnit = new LightUnitType[m_iTextureDefsNum];

	// Vertex Load
	fseek(fp,m_arrLump[ RBSP_VERTICES ].m_iOffset,SEEK_SET);		// Move File Pointer
	fread((RBspVertex *)m_pBeforeVerts,sizeof(RBspVertex),m_iVertsNum,fp);

	for(i=0;i < m_iVertsNum;i++) 
	{
		float tmp = m_pBeforeVerts[i].m_vecPos.y;
		m_pBeforeVerts[i].m_vecPos.y = m_pBeforeVerts[i].m_vecPos.z;
		m_pBeforeVerts[i].m_vecPos.z = tmp;

		m_pVerts[i].m_vecPos = m_pBeforeVerts[i].m_vecPos;
		/// Scale
		m_pVerts[i].m_vecPos.x *= m_vecScale.x;
		m_pVerts[i].m_vecPos.y *= m_vecScale.y;
		m_pVerts[i].m_vecPos.z *= m_vecScale.z;

		m_pVerts[i].m_vecTexcoord = m_pBeforeVerts[i].m_vecTex1;
		m_pVerts[i].m_vecTexcoord2 = m_pBeforeVerts[i].m_vecTex2;
		
		
		m_pVerts[i].m_vecNormal = D3DXVECTOR3(m_pBeforeVerts[i].m_vecNormal.x,
												m_pBeforeVerts[i].m_vecNormal.z,
												m_pBeforeVerts[i].m_vecNormal.y);
		
	}
	delete[] m_pBeforeVerts;
	m_pBeforeVerts = NULL;

	// Face Load
	fseek(fp,m_arrLump[ RBSP_FACES ].m_iOffset,SEEK_SET);			// Move File Pointer
	fread((RBspFace *)m_pFaces,sizeof(RBspFace),m_iFacesNum,fp);
	
	for(i=0;i < m_iFacesNum; i++)										// Face Normal Convert
	{
		float tmp = m_pFaces[i].m_vecNormal.y;
		m_pFaces[i].m_vecNormal.y = m_pFaces[i].m_vecNormal.z;
		m_pFaces[i].m_vecNormal.z = tmp;
	}
	// Texture Def Load
	fseek(fp,m_arrLump[ RBSP_TEXTURES ].m_iOffset,SEEK_SET);			// Move File Pointer
	fread((RBspTextureDef *)m_pTextureDefs,sizeof(RBspTextureDef),m_iTextureDefsNum,fp);
	//// Load Texture Routine
	for(i = 0; i < m_iTextureDefsNum; i++ )
	{
		m_pBaseVertexBuffer[i] = NULL;
		//m_pLightVertexBuffer[i] = NULL;
		m_pBaseVertexNum[i] = 0;
		m_pBaseVertexUseNum[i] = 0;
		m_pBaseIndexBuffer[i] = NULL;
		m_pBaseIndexNum[i] = 0;
		m_pBaseIndexUseNum[i] = 0;

		char strTex[256] = {0};
		strcpy(strTex,m_pTextureDefs[i].m_strName);
		char *cptr = strrchr(strTex,'/');
		if(cptr != NULL)
		{
			cptr++;
			strcat(cptr,".dds");
			CTexture::SetPath(BSPTEXTUREPATH);
			m_pTextures[i].Load(cptr);
		}
		else
		{
		
		}
		/*
		strcat(strTex,".tga");
		if(!m_pTextures[i].LoadTexture(strTex)) {		// tga 파일 로드 시도
			strcpy(strTex,m_pTextureDefs[i].m_strName);
			strcat(strTex,".jpg");
			m_pTextures[i].LoadTexture(strTex);			// 없을시 jpg 로드 시도
		}
		memset(strTex,0,sizeof(char) * 256);
		strcpy(strTex,m_pTextureDefs[i].m_strName);
		strcat(strTex,"_Normal.bmp");
		m_pNTextures[i].LoadTexture(strTex);
		*/
	}
	// LightMap Load
	fseek(fp,m_arrLump[ RBSP_LIGHTMAP ].m_iOffset,SEEK_SET);
	fread((RBspLightMap *)m_pLightMaps,sizeof(RBspLightMap),m_iLightMapsNum,fp);
	
	for( i = 0; i < m_iLightMapsNum; i++ )
	{
		

		CTexture::ChangeTextureIntension(m_pLightMaps[i].m_ucLightMap,128 * 128 * 3,9.0f);
		
		byte m_ucLight[65536];
		int icount = 0;
		int icount2 = 0;
		for(int j=0;j < 128;j++) {
			for(int k=0;k < 128;k++) {
				m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+2];		//R
				m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+1];		//G
				m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount];		//B
				m_ucLight[icount2++] = 0;
				icount +=3;
			}
		}

		m_pLightTextures[i].CreateEmptyTexture(128,128,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED);
	
		//m_pLightTextures[i].FillTexture(m_pLightMaps[i].m_ucLightMap);
		m_pLightTextures[i].FillTexture(m_ucLight);
	}
	
	
	// Node
	fseek(fp,m_arrLump[ RBSP_NODES ].m_iOffset,SEEK_SET);
	fread((RBspNode *)m_pNodes,sizeof(RBspNode),m_iNodesNum,fp);
	for(i=0;i < m_iNodesNum; i++) {
				
		int tmp = m_pNodes[i].m_vecMinBox[1];
		m_pNodes[i].m_vecMinBox[1] = m_pNodes[i].m_vecMinBox[2];
		m_pNodes[i].m_vecMinBox[2] = tmp;
		
		tmp = m_pNodes[i].m_vecMaxBox[1];
		m_pNodes[i].m_vecMaxBox[1] = m_pNodes[i].m_vecMaxBox[2];
		m_pNodes[i].m_vecMaxBox[2] = tmp;
	
		m_pNodes[i].m_vecMinBox[0] *= (int)(m_vecScale.x);
		m_pNodes[i].m_vecMinBox[1] *= (int)(m_vecScale.y);
		m_pNodes[i].m_vecMinBox[2] *= (int)(m_vecScale.z);
		
		m_pNodes[i].m_vecMaxBox[0] *= (int)(m_vecScale.x);
		m_pNodes[i].m_vecMaxBox[1] *= (int)(m_vecScale.y);
		m_pNodes[i].m_vecMaxBox[2] *= (int)(m_vecScale.z);

	}

	// Leaf 
	fseek(fp,m_arrLump[ RBSP_LEAFS ].m_iOffset,SEEK_SET);
	fread((RBspLeaf *)m_pLeafs,sizeof(RBspLeaf),m_iLeafsNum,fp);

 	for(i=0;i < m_iLeafsNum; i++) {
				
		int tmp = m_pLeafs[i].m_vecMinBox[1];
		m_pLeafs[i].m_vecMinBox[1] = m_pLeafs[i].m_vecMinBox[2];
		m_pLeafs[i].m_vecMinBox[2] = tmp;
		
		tmp = m_pLeafs[i].m_vecMaxBox[1];
		m_pLeafs[i].m_vecMaxBox[1] = m_pLeafs[i].m_vecMaxBox[2];
		m_pLeafs[i].m_vecMaxBox[2] = tmp;
		// Leaf Ex conversion
		m_pLeafExs[i].m_iAreaPotal = m_pLeafs[i].m_iAreaPotal;
		m_pLeafExs[i].m_iCluster = m_pLeafs[i].m_iCluster;
		m_pLeafExs[i].m_iLeafBrush = m_pLeafs[i].m_iLeafBrush;
		m_pLeafExs[i].m_iLeafBrushNum = m_pLeafs[i].m_iLeafBrushNum;
		m_pLeafExs[i].m_iLeafFace = m_pLeafs[i].m_iLeafFace;
		m_pLeafExs[i].m_iLeafFaceNum = m_pLeafs[i].m_iLeafFaceNum;
		memcpy(m_pLeafExs[i].m_vecMaxBox,m_pLeafs[i].m_vecMaxBox,sizeof(int) * 3);
		memcpy(m_pLeafExs[i].m_vecMinBox,m_pLeafs[i].m_vecMinBox,sizeof(int) * 3);	
		
		m_pLeafExs[i].m_vecMinBox[0] *= (int)(m_vecScale.x);
		m_pLeafExs[i].m_vecMinBox[1] *= (int)(m_vecScale.y);
		m_pLeafExs[i].m_vecMinBox[2] *= (int)(m_vecScale.z);

		m_pLeafExs[i].m_vecMaxBox[0] *= (int)(m_vecScale.x);
		m_pLeafExs[i].m_vecMaxBox[1] *= (int)(m_vecScale.y);
		m_pLeafExs[i].m_vecMaxBox[2] *= (int)(m_vecScale.z);
	}
	delete[] m_pLeafs;
	m_pLeafs = NULL;

	// Leaf Face
 	fseek(fp,m_arrLump[ RBSP_LEAFFACES ].m_iOffset,SEEK_SET);
	fread((int *)m_pLeafFaces,sizeof(int),m_iLeafFacesNum,fp);
	// Leaf Brushes
 	fseek(fp,m_arrLump[ RBSP_LEAFBRUSHES ].m_iOffset,SEEK_SET);
	fread((int *)m_pLeafBrushes,sizeof(int),m_iLeafBrushesNum,fp);

	// Plane
	fseek(fp,m_arrLump[ RBSP_PLANES ].m_iOffset,SEEK_SET);
	fread((RBspPlane *)m_pPlanes,sizeof(RBspPlane),m_iPlanesNum,fp);
	for(i=0;i<m_iPlanesNum;i++) {
		float tmp = m_pPlanes[i].m_vecNormal.y;
		m_pPlanes[i].m_vecNormal.y = m_pPlanes[i].m_vecNormal.z;
		m_pPlanes[i].m_vecNormal.z = tmp;
		// Scale 은 정형 이어야 한다
		m_pPlanes[i].m_fDistance *= m_vecScale.x;

	}
	

	// Brushe
	fseek(fp,m_arrLump[ RBSP_BRUSHES ].m_iOffset,SEEK_SET);
	fread((RBspBrushe *)m_pBrushes,sizeof(RBspBrushe),m_iBrushesNum,fp);

	// BrusheSide
	fseek(fp,m_arrLump[ RBSP_BRUSHESIDES ].m_iOffset,SEEK_SET);
	fread((RBspBrusheSide *)m_pBrusheSides,sizeof(RBspBrusheSide),m_iBrusheSidesNum,fp);

	// VisData (Cluster)
	fseek(fp,m_arrLump[ RBSP_VISDATA ].m_iOffset,SEEK_SET);
	if( m_arrLump[ RBSP_VISDATA ].m_iLength )
	{
		fread((int *)&(m_VisData.m_iClustersNum),sizeof(int),1,fp);
		fread((int *)&(m_VisData.m_iBytesPerCluster),sizeof(int),1,fp);
		int iAllocSize = m_VisData.m_iBytesPerCluster * m_VisData.m_iClustersNum;
		m_VisData.m_pBitsets = new byte[ iAllocSize ];
		fread((byte *)(m_VisData.m_pBitsets),sizeof(byte),iAllocSize,fp);

	}
	else	  
	{	
		m_VisData.m_pBitsets = NULL;				// Visibility Data Not Exist
	}

	m_FaceRenderBit.ResizeBits(m_iFacesNum);		// Face Render Bit Resize
	// Memory pool Create

	m_D3Device =  CSceneManager::GetDevice();
	
	CreateBufferNum();
	CreateBuffer();
	
	m_pFrustum = new CViewFrustum;


	m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);

	m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	return true;

}
void RBspScene::ClearBufferNum()					// Buffer 관련 Use Number 변수들 초기화
{
	for(int i=0;i < m_iTextureDefsNum;i++) 
	{
		m_pBaseVertexUseNum[i] = 0;
		m_pBaseIndexUseNum[i] = 0;
		m_pLightUnit[i].clear();
	}
}
void RBspScene::CreateBufferNum2() {
/*
	int i;
	
	memset(m_pBaseVertexNum,0,sizeof(int) * m_iTextureDefsNum );
	memset(m_pBaseIndexNum,0,sizeof(int) * m_iTextureDefsNum );

	ClearBufferNum();

	for( i = 0; i < m_iFacesNum; i++ )			// Buffer Size Setting 			
	{		
		int iFaceVertNum  = m_pFaces[i].m_iVertexNum;
		int iBaseTexId = m_pFaces[i].m_iTexId;
		int iLightTexId = m_pFaces[i].m_iLightMapId;

		m_pBaseVertexNum[ iBaseTexId ] += iFaceVertNum;
		m_pBaseIndexNum[ iBaseTexId ] += (iFaceVertNum - 2) * 3;
	}
	//vector push

	for(i=0;i<m_iTextureDefsNum;i++)
	{
		WORD *pWord = NULL;
		RBseVertex *pVertex = NULL;
		
		m_pIndexArr.push_back(pWord);
		m_pVertexArr.push_back(pVertex);

		if(m_pBaseIndexNum[i] > 0)
			m_pIndexArr[i] =  new WORD[m_pBaseIndexNum[i]];
		if(m_pBaseVertexNum[i] > 0)
			m_pVertexArr[i] = new RBseVertex[m_pBaseVertexNum[i]];

	}
*/
}
void RBspScene::CreateBufferNum()
{	
	if(m_iShaderVersion == RBSPSHADER_GF2)
	{

		int i;
		
		memset(m_pBaseVertexNum,0,sizeof(int) * m_iTextureDefsNum );
		memset(m_pBaseIndexNum,0,sizeof(int) * m_iTextureDefsNum );

		ClearBufferNum();

		for( i = 0; i < m_iFacesNum; i++ )			// Buffer Size Setting 			
		{		
			int iFaceVertNum  = m_pFaces[i].m_iVertexNum;
			int iBaseTexId = m_pFaces[i].m_iTexId;
			int iLightTexId = m_pFaces[i].m_iLightMapId;

			m_pBaseVertexNum[ iBaseTexId ] += iFaceVertNum;
			m_pBaseIndexNum[ iBaseTexId ] += (iFaceVertNum - 2) * 3;
		}
		//vector push

		for(i=0;i<m_iTextureDefsNum;i++)
		{
			WORD *pWord = NULL;
			RBspDrawVertex *pVertex = NULL;
			
			m_pIndexArr.push_back(pWord);
			m_pVertexArr.push_back(pVertex);

			if(m_pBaseIndexNum[i] > 0)
				m_pIndexArr[i] =  new WORD[m_pBaseIndexNum[i]];
			if(m_pBaseVertexNum[i] > 0)
				m_pVertexArr[i] = new RBspDrawVertex[m_pBaseVertexNum[i]];

		}
	}
}
// Create BaseTexture Buffer(vertex,index), LightMap Buffer(vertex,index)
void RBspScene::CreateBuffer()
{
	int i;
	HRESULT hr = 0;
	for( i = 0; i < m_iTextureDefsNum; i++ )		// Base Texture's Buffer Create
	{
		
		if(m_pBaseVertexNum[i] > 0 ) {
			if(m_iShaderVersion == RBSPSHADER_GF2) 
			{
				hr = m_D3Device->CreateVertexBuffer(m_pBaseVertexNum[i] * sizeof(RBspDrawVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										dwRBSPUseFVF,D3DPOOL_DEFAULT,&(m_pBaseVertexBuffer[i]));
			}
			else if(m_iShaderVersion == RBSPSHADER_GF3)
			{

/*				hr = m_D3Device->CreateVertexBuffer(m_pBaseVertexNum[i] * sizeof(WBDotVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										0,D3DPOOL_DEFAULT,&(m_pBaseVertexBuffer[i]));

				hr = m_D3Device->CreateVertexBuffer(m_pBaseVertexNum[i] * sizeof(RBspLightVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										0,D3DPOOL_DEFAULT,&(m_pLightVertexBuffer[i]));
			*/
			}
		}
		else {
			m_pBaseVertexBuffer[i] = NULL;
		}
		if(m_pBaseIndexNum[i] > 0) {

			
				hr = m_D3Device->CreateIndexBuffer(m_pBaseIndexNum[i] * sizeof(WORD),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										D3DFMT_INDEX16,D3DPOOL_DEFAULT,&(m_pBaseIndexBuffer[i]));
		}
		else
			m_pBaseIndexBuffer[i] = NULL;
	
	}
}

int RBspScene::ClusterVisibleTest(int iCurrentCluster,int iTestCluster)
{													// iCurrentCluster 에서 iTestCluster 가 보이는지 Test		

	int iReturn = 0;

	if( !(m_VisData.m_pBitsets) || (iCurrentCluster < 0) )
		return 1;
	byte VisSet = m_VisData.m_pBitsets[ (iCurrentCluster * m_VisData.m_iBytesPerCluster) + (iTestCluster / 8) ];
	iReturn = VisSet & ( 1 << ( (iTestCluster) & 7 ) );
	// 1 : visible 0 : not visible
	return iReturn;

}	

int RBspScene::FindWorldLeaf(D3DXVECTOR3 vecPos)
{
	D3DXVECTOR3 vecLocal = WorldToLocalPos(vecPos);
	return FindLeafNode(vecLocal);

}
int RBspScene::FindLeafNode(D3DXVECTOR3 vecPos)
{
	int iChildIndex = 0;

	float fDistance = 0.0f;
	// Leaf 노드는 Front Child Index 와 Back Child Index 가 RBSP_EMPTY(-1) 이다
	while( iChildIndex >= 0 )
	{
		RBspNode rNode = m_pNodes[ iChildIndex ];
		RBspPlane rPlane = m_pPlanes[ rNode.m_iPlaneIndex ];

		// Plane과의 거리 계산 
		fDistance =	((rPlane.m_vecNormal.x * vecPos.x) + 
					(rPlane.m_vecNormal.y * vecPos.y) + 
					(rPlane.m_vecNormal.z * vecPos.z)) - rPlane.m_fDistance;	
		
		// 현제 노드의 앞이면
        if(fDistance >= 0)		
			iChildIndex = rNode.m_iFrontCIndex;				

		else
			iChildIndex = rNode.m_iBackCIndex;


	}
	// Return (same thing as saying:  return -(iChildIndex + 1)).
    // 이번 노드의 index return
	return ~iChildIndex;
		
}
// Slide 없는 collision check
void RBspScene::CheckCollisiontoBrushe2()
{

	//Brushe 순회 
	CheckBrusheNode( 0,0.0f, 1.0f, m_WalkUnit.m_vecBPos, m_WalkUnit.m_vecPos);

	if(m_WalkUnit.m_fFraction == 1.0f)				// Not Collision
	{	
		m_WalkUnit.m_vecOPos = m_WalkUnit.m_vecPos;
	}
	else											// Collision
	{	
		m_WalkUnit.m_vecOPos = m_WalkUnit.m_vecBPos + 
 								(m_WalkUnit.m_fFraction * (m_WalkUnit.m_vecPos - m_WalkUnit.m_vecBPos));
	}

}
void RBspScene::CheckCollisiontoBrushe() 
{
	int i;

	//Brushe 순회 
	CheckBrusheNode( 0,0.0f, 1.0f, m_WalkUnit.m_vecBPos, m_WalkUnit.m_vecPos);

	if(m_WalkUnit.m_fFraction == 1.0f)				// Not Collision
	{	
		m_WalkUnit.m_vecOPos = m_WalkUnit.m_vecPos;
	}
	else											// Collision
	{	

		// Sliding vector 생성
		m_WalkUnit.m_vecSlide = D3DXVECTOR3(0.0f,0.0f,0.0f);
		
		// collision plane normal calc
		for( i = 0; i < m_WalkUnit.m_iCollisionPlanesNum; i++) 
		{
			m_WalkUnit.m_vecSlide += m_pPlanes[(m_WalkUnit.m_iCollisionPlanes[i])].m_vecNormal;		
			
		}
		D3DXVec3Normalize(&(m_WalkUnit.m_vecSlide),&(m_WalkUnit.m_vecSlide));
		// 운동방향 vector 
		D3DXVECTOR3 vecDir = m_WalkUnit.m_vecPos - m_WalkUnit.m_vecBPos;
		vecDir *= -1.0f;
			
		m_WalkUnit.m_vecSlide.y += 15.0f;
		
		m_WalkUnit.m_vecSlide.x += vecDir.x / 6.0f;
		m_WalkUnit.m_vecSlide.z += vecDir.z / 6.0f;
		// 새로운 position 
		m_WalkUnit.m_vecPos = m_WalkUnit.m_vecBPos + m_WalkUnit.m_vecSlide;

	
		m_WalkUnit.m_fFraction = 1.0f;
		m_WalkUnit.m_bOutputStartsOut = true;
		m_WalkUnit.m_bOutputAllSolid = false;
		// 새로운 목표 position 을 가지고 다시 충돌 체크
		CheckBrusheNode( 0,0.0f, 1.0f, m_WalkUnit.m_vecBPos,m_WalkUnit.m_vecPos);

	//if(m_WalkUnit.m_fFraction < 1.0f)
	//	m_WalkUnit.m_fFraction = 0.0f;

	
		m_WalkUnit.m_vecOPos = m_WalkUnit.m_vecBPos + 
 								(m_WalkUnit.m_fFraction * (m_WalkUnit.m_vecPos - m_WalkUnit.m_vecBPos));
	
	}
	
}
/*
void RBspScene::CheckBrusheNode(int iNodeIndex, float fStartFraction, float fEndFraction, D3DXVECTOR3 vecStart,D3DXVECTOR3 vecEnd)
{

	int i;

	if(fStartFraction > m_WalkUnit.m_fFraction)
		return;
	if(iNodeIndex<0)
	{
		RBspLeafEx *pLeaf = &(m_pLeafExs[~iNodeIndex]);
		for( i = 0; i < pLeaf->m_iLeafBrushNum; i++ )
		{
			RBspBrushe *pBrush = &(m_pBrushes[m_pLeafBrushes[pLeaf->m_iLeafBrush + i]]);
			if((pBrush->m_iBrusheSidesNum > 0) && (m_pTextureDefs[pBrush->m_iTexId].m_iContents & 1))
			{
				CheckBrushe(pBrush);
			}
		}
		return;

	}
	RBspNode *pNode = &(m_pNodes[iNodeIndex]);
	RBspPlane *pPlane = &(m_pPlanes[pNode->m_iPlaneIndex]);
	
	float fStartDist = D3DXVec3Dot(&vecStart,&(pPlane->m_vecNormal)) - pPlane->m_fDistance;
	float fEndDist = D3DXVec3Dot(&vecEnd,&(pPlane->m_vecNormal)) - pPlane->m_fDistance;
	
	if(fStartDist >= 0 && fEndDist >= 0)
	{	
		CheckBrusheNode( pNode->m_iFrontCIndex,fStartFraction,fEndFraction,vecStart,vecEnd);

	}
	else if(fStartDist < 0 && fEndDist < 0)
	{
		CheckBrusheNode( pNode->m_iBackCIndex,fStartFraction,fEndFraction,vecStart,vecEnd);
	}
	else
	{	
		int side1, side2;  
		float fraction1, fraction2;	   
		D3DXVECTOR3 vecMiddle;

		if(fStartDist< fEndDist)
		{
			side1 = pNode->m_iBackCIndex;
			side2 = pNode->m_iFrontCIndex;    
			float inverseDistance = 1.0f / (fStartDist - fEndDist); 
			fraction1 = (fStartDist - RBSPEPS) * inverseDistance;	  
			fraction2 = (fStartDist + RBSPEPS) * inverseDistance;	  
		}
		else if(fEndDist < fStartDist)
		{
			side1 = pNode->m_iFrontCIndex;
			side2 = pNode->m_iBackCIndex;
			float inverseDistance = 1.0f / (fStartDist - fEndDist);
			fraction1 = (fStartDist + RBSPEPS) * inverseDistance;
			fraction2 = (fStartDist - RBSPEPS) * inverseDistance;
		}
		else
		{
			side1 = pNode->m_iFrontCIndex;
			side2 = pNode->m_iBackCIndex;

			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}

		if(fraction1 < 0.0f) 
			fraction1 = 0.0f;
		else if(fraction1 > 1.0f) 
			fraction1 = 1.0f;
		if(fraction2 < 0.0f) 
			fraction2 = 0.0f;
		else if(fraction2 > 1.0f) 
			fraction2 = 1.0f;

		vecMiddle = vecStart + (vecEnd - vecStart) * fraction1;

		float fMiddleFraction = fStartFraction + (fEndFraction - fStartFraction) * fraction1;

		CheckBrusheNode(side1,fStartFraction, fMiddleFraction, vecStart, vecMiddle );

		vecMiddle = vecStart + (vecEnd - vecStart) * fraction2;

		fMiddleFraction = fStartFraction + (fEndFraction - fStartFraction) * fraction2;
		CheckBrusheNode(side2,fMiddleFraction, fEndFraction, vecMiddle, vecEnd );

	}
}
void RBspScene::CheckBrushe(RBspScene::RBspBrushe *pBrushe)				// Check Brush for Collision
{
	int i;

	float fStartFraction = -1.0f;	
	float fEndFraction = 1.0f;		
	bool  bStartsOut = false;		
	bool  bEndsOut = false;		
	
	D3DXVECTOR3 vecCollisionNormal;

	for( i = 0; i < pBrushe->m_iBrusheSidesNum; i++ )
	{
		RBspBrusheSide *pBrusheSide = &(m_pBrusheSides[pBrushe->m_iBrusheSide + i]);
		RBspPlane *pPlane = &(m_pPlanes[pBrusheSide->m_iPlaneIndex]);

		float fStartDist = D3DXVec3Dot(&(m_WalkUnit.m_vecBPos),&(pPlane->m_vecNormal)) - (pPlane->m_fDistance + m_WalkUnit.m_fRadius);
		float fEndDist = D3DXVec3Dot(&(m_WalkUnit.m_vecPos),&(pPlane->m_vecNormal)) - (pPlane->m_fDistance + m_WalkUnit.m_fRadius);
		
		if(fStartDist > 0)	
			bStartsOut = true;

		if(fEndDist > 0)	
			bEndsOut = true;

		if (fStartDist > 0 && fEndDist > 0)
		{   
			return;
		}
		if (fStartDist <= 0 && fEndDist <= 0)
		{   
			continue;
		}
		if (fStartDist > fEndDist)
		{  
			float fFraction = (fStartDist - RBSPEPS) / (fStartDist - fEndDist);
			if (fFraction > fStartFraction)
			{
				fStartFraction = fFraction;
				vecCollisionNormal = pPlane->m_vecNormal;
			}
		}
		else
		{   
			float fFraction = (fStartDist + RBSPEPS) / (fStartDist - fEndDist);
			if (fFraction < fEndFraction)
				fEndFraction = fFraction;
		}
	}
	if(bStartsOut == false)
	{
		m_WalkUnit.m_bOutputStartsOut = false;

		if (bEndsOut == false)
			m_WalkUnit.m_bOutputAllSolid = true;
		return;
	}

	if(fStartFraction < fEndFraction)
	{
		if ((fStartFraction > -1.0) && (fStartFraction < m_WalkUnit.m_fFraction))
		{
	
			if(fStartFraction < 0) 
				fStartFraction = 0;
			m_WalkUnit.m_fFraction = fStartFraction;
			m_WalkUnit.m_vecSlide = vecCollisionNormal;

		}
	}

}
*/

void RBspScene::CheckBrusheNode(int iNodeIndex, float fStartFraction, float fEndFraction, D3DXVECTOR3 vecStart,D3DXVECTOR3 vecEnd) 
{
	int i;

	if (fStartFraction > m_WalkUnit.m_fFraction) 
		return;

	if (iNodeIndex < 0)					// leaf node
	{	
		
		// parent node Get
		RBspLeafEx *pLeaf = &(m_pLeafExs[-(iNodeIndex + 1)]);	
		int iStartBrushe = pLeaf->m_iLeafBrush;
		
		
		for (i = 0; i < pLeaf->m_iLeafBrushNum; i++)
		{
			int iBrushe = m_pLeafBrushes[iStartBrushe + i];

			RBspBrushe *pBrushe = &(m_pBrushes[iBrushe]);
			if((pBrushe->m_iBrusheSidesNum > 0) && (m_pTextureDefs[pBrushe->m_iTexId].m_iContents & 1))
				CheckBrushe(pBrushe);

		}
		return;
	}
		
	RBspNode *pNode = &(m_pNodes[iNodeIndex]);
	RBspPlane *pPlane = &(m_pPlanes[pNode->m_iPlaneIndex]);

	float fStartDist,fEndDist,fRadius;
	fStartDist = D3DXVec3Dot(&vecStart,&(pPlane->m_vecNormal)) - pPlane->m_fDistance;
	fEndDist = D3DXVec3Dot(&vecEnd,&(pPlane->m_vecNormal)) - pPlane->m_fDistance;
	fRadius = m_WalkUnit.m_fRadius;

	if( fStartDist >= fRadius && fEndDist >= fRadius)
	{
		// start, end pos 모두 현 노드의 앞면에 존재
		// 앞노드로 내려감
		CheckBrusheNode(pNode->m_iFrontCIndex,fStartFraction,fEndFraction,vecStart,vecEnd);


	}
	else if(fStartDist < -fRadius && fEndDist < -fRadius)
	{
		// Start, End Pos 모두 현 노드의 뒷면에 존재 
		// 뒷노드로 내려감	
		CheckBrusheNode(pNode->m_iBackCIndex,fStartFraction,fEndFraction,vecStart,vecEnd);
	}

	else		// 현 노드  
	{	
		// 두 부분으로 나눈다
		int iSide;
		float fFractionFront,fFractionBack,fMidFraction;
		D3DXVECTOR3 vecMid;
	
		if (fStartDist < fEndDist)
		{
			iSide = RBSP_BACK; // back
			float fInverseDist = 1.0f / (fStartDist - fEndDist);
			fFractionFront = ((fStartDist - fRadius) + RBSPEPS) * fInverseDist;
			fFractionBack = ((fStartDist + fRadius) + RBSPEPS) * fInverseDist;	
		}
		else if (fEndDist < fStartDist)
		{
			iSide = RBSP_FRONT; // front
			float fInverseDist = 1.0f / (fStartDist - fEndDist);
			fFractionFront = ((fStartDist + fRadius) + RBSPEPS) * fInverseDist;
			fFractionBack = ((fStartDist - fRadius) - RBSPEPS) * fInverseDist;

		}
		else
		{
			iSide = RBSP_FRONT; // front
			fFractionFront = 1.0f;
			fFractionBack = 0.0f;
		}
		// 범위 설정
		if (fFractionFront < 0.0f) fFractionFront = 0.0f;
		else if (fFractionFront > 1.0f) fFractionFront = 1.0f;
		
		if (fFractionBack < 0.0f) fFractionBack = 0.0f;
		else if (fFractionBack > 1.0f) fFractionBack = 1.0f;
		// 위에서 나눈 mid fraction을 이용 체크 루틴 돌림
		// 앞부분 체크 
		fMidFraction = fStartFraction + (fEndFraction - fStartFraction) * fFractionFront;
		vecMid = vecStart + (fFractionFront * (vecEnd - vecStart));

		if(iSide == RBSP_FRONT)
			CheckBrusheNode(pNode->m_iFrontCIndex,fStartFraction,fMidFraction,vecStart,vecMid);
		else 
			CheckBrusheNode(pNode->m_iBackCIndex,fStartFraction,fMidFraction,vecStart,vecMid);

		// 뒷부분 체크
		fMidFraction = fStartFraction + (fEndFraction - fStartFraction) * fFractionBack;
		vecMid = vecStart + (fFractionBack * (vecEnd - vecStart));
		
		if(iSide == RBSP_FRONT)
			CheckBrusheNode(pNode->m_iBackCIndex,fMidFraction,fEndFraction,vecMid,vecEnd);
		else 
			CheckBrusheNode(pNode->m_iFrontCIndex,fMidFraction,fEndFraction,vecMid,vecEnd);

	}
}

void RBspScene::CheckBrushe(RBspScene::RBspBrushe *pBrushe)				// Check Brush for Collision
{
	int i,j;

	
	float fStartFraction = -1.0f;
	float fEndFraction = 1.0f;
	bool bStartsOut = false;
	bool bEndsOut = false;
	
	int iStartBrush = pBrushe->m_iBrusheSide;


	for( i = 0; i < pBrushe->m_iBrusheSidesNum; i++)
	{

		RBspBrusheSide *pBrusheSide = &(m_pBrusheSides[ iStartBrush + i ]);
		RBspPlane *pPlane = &(m_pPlanes[ pBrusheSide->m_iPlaneIndex ]);
		RBspTextureDef *pTextureDef = &(m_pTextureDefs[pBrusheSide->m_iTexId]);
		
		if(strstr(pTextureDef->m_strName,"fog_Type1") != NULL)		// 안개 
			continue;

		// 투명 brush skip
  	/*	if((!strcmp("textures/common/caulk",pTextureDef->m_strName)) ||		// textures/common/caulk 
			(!strcmp("textures/common/hint",pTextureDef->m_strName)) ||	// textures/common/hint
			(!strcmp("noshader",pTextureDef->m_strName)))		// NoShader
			continue;
	*/	
		float fStartDist,fEndDist;
		fStartDist = D3DXVec3Dot(&(m_WalkUnit.m_vecBPos),&(pPlane->m_vecNormal)) - (pPlane->m_fDistance + m_WalkUnit.m_fRadius);
		fEndDist = D3DXVec3Dot(&(m_WalkUnit.m_vecPos),&(pPlane->m_vecNormal)) - (pPlane->m_fDistance + m_WalkUnit.m_fRadius);
		
		if (fStartDist > 0.0f)
			bStartsOut = true;
		if (fEndDist > 0.0f)
			bEndsOut = true;

		// Side Check
		if ((fStartDist > 0.0f) && (fEndDist > 0.0f))
		{   // 두 점다 현 brushe 앞에 있다,현 brushe 바깥의 일이다.
			
			return;
		}
		
		if ((fStartDist <= 0.0f) && (fEndDist <= 0.0f))
		{   // 두 점다 현 brushe 뒤에 있다
			continue;
		}
		
		if ( fStartDist > fEndDist )
		{   // line is entering into the brush
			float fFraction = (fStartDist - RBSPEPS) / (fStartDist - fEndDist);

			if (fFraction > fStartFraction) {
				fStartFraction = fFraction;
				// Collision Plane Setting
				//Collision Plane Index		
				
				//m_WalkUnit.m_iCollisionPlane = i;
				bool bExistPlane = false;
				for(j = 0;j < m_WalkUnit.m_iCollisionPlanesNum;j++) 
				{
					if(m_WalkUnit.m_iCollisionPlanes[j] == i)
					{
						bExistPlane = true;
					}
				}	
				if(!bExistPlane && m_WalkUnit.m_iCollisionPlanesNum < 5)  
					m_WalkUnit.m_iCollisionPlanes[ m_WalkUnit.m_iCollisionPlanesNum++ ] = i;
				
					
	
			}
		}
		else
		{   // line is leaving the brush
			float fFraction = (fStartDist + RBSPEPS) / (fStartDist - fEndDist);
			if (fFraction < fEndFraction)
				fEndFraction = fFraction;
		}

	}

	if (bStartsOut == false)
	{
		m_WalkUnit.m_bOutputStartsOut = false;
		if(bEndsOut == false)
			m_WalkUnit.m_bOutputAllSolid = true;
		return;
	}
	if (fStartFraction < fEndFraction)
	{
		if (fStartFraction > -1.0f && fStartFraction < m_WalkUnit.m_fFraction)
		{
			if (fStartFraction < 0.0f)
				fStartFraction = 0.0f;
  			
			m_WalkUnit.m_fFraction = fStartFraction;
		}	
	}
}

void RBspScene::UpdateLeaf(int iLeaf,bool bFrustum,int iCurrentCluster)
{
	
	RBspLeafEx *pLeaf = &(m_pLeafExs[iLeaf]);
	int iFaceCount = 0;
	
	iFaceCount = pLeaf->m_iLeafFaceNum;
	if( iFaceCount <= 0 )		// face 가 없는 노드
		return;
	// 현제 카메라 위치인 leaf 의 클러스터 index  : iCurrentCluster
	// Visible test
	if(!ClusterVisibleTest(iCurrentCluster,pLeaf->m_iCluster))
		return;
	// Frustum cull
	if(bFrustum)
 		if(!m_pFrustum->BoxInFrustum(pLeaf->m_vecMinBox,pLeaf->m_vecMaxBox))
			return;

	while( iFaceCount-- )
	{
		int iFaceIndex = m_pLeafFaces[ pLeaf->m_iLeafFace + iFaceCount ];
		if( m_pFaces[ iFaceIndex ].m_iFaceType != RBSP_POLYGON)		// Polygon 만 렌더
 			continue;
		if( !m_FaceRenderBit.GetBit(iFaceIndex) )// 그려진 face 가 아니면 render
		{
			
			m_iVisLeafNodeNum++;				
			m_FaceRenderBit.SetBit(iFaceIndex);
	
			PushFaceBuffer( iFaceIndex);
			
		}
	}
	
}
void RBspScene::UpdateNode(int iNode,bool bFrustum,int iCurrentCluster) 
{
		
	RBspNode *pNode = &(m_pNodes[iNode]);

	// 상위 노드가 frustum 안에 있으면 그 노드는 무조건 frustum 안에 있다.
	if(bFrustum){
		if(!m_pFrustum->BoxInFrustum(pNode->m_vecMinBox,pNode->m_vecMaxBox)) {		// 이노드는 Frustum 바깥이다.
			return;
		}
		else																	// 이노드는 frustum 안이다.그러므로 자식 노드들은 모두 안이다
			bFrustum = false;

	}
	// a node is an structure that hold a partition plane, all leaf are splited in two
	// groups: FRONT and BACK. plane is the partition plane for this node

	const RBspPlane *pPlane = &(m_pPlanes[ pNode->m_iPlaneIndex ]);

	// 카메라가 현제 평면의 앞이라면 앞에서 부터 그린다 
    if( (pPlane->m_vecNormal.x * m_WalkUnit.m_vecPos.x + pPlane->m_vecNormal.y * m_WalkUnit.m_vecPos.y + pPlane->m_vecNormal.z * m_WalkUnit.m_vecPos.z - pPlane->m_fDistance > 0) ) {
		// Front Child
		if(pNode->m_iFrontCIndex >= 0) 
			UpdateNode((pNode->m_iFrontCIndex),bFrustum,iCurrentCluster);
		else
			UpdateLeaf(~(pNode->m_iFrontCIndex),bFrustum,iCurrentCluster);

		// Back Child
		if(pNode->m_iBackCIndex >= 0)
			UpdateNode((pNode->m_iBackCIndex),bFrustum,iCurrentCluster);
		else
			UpdateLeaf(~(pNode->m_iBackCIndex),bFrustum,iCurrentCluster);

   }
   else //뒷면 부터 그린다  
   {
		if(pNode->m_iBackCIndex >= 0)
			UpdateNode((pNode->m_iBackCIndex),bFrustum,iCurrentCluster);
		else 
			UpdateLeaf(~(pNode->m_iBackCIndex),bFrustum,iCurrentCluster);
		if(pNode->m_iFrontCIndex >= 0)
			UpdateNode((pNode->m_iFrontCIndex),bFrustum,iCurrentCluster);
		else
			UpdateLeaf(~(pNode->m_iFrontCIndex),bFrustum,iCurrentCluster);

   }

}
D3DXVECTOR3 RBspScene::CollisionPoint(D3DXVECTOR3 vecBefore,D3DXVECTOR3 vecNew,float fRad)		// Slide 없는 collision check	
{
	m_WalkUnit.m_vecBPos = vecBefore;
	m_WalkUnit.m_vecPos = vecNew;
	m_WalkUnit.m_fFraction = 1.0f;
	m_WalkUnit.m_bOutputStartsOut = true;
	m_WalkUnit.m_bOutputAllSolid = false;
	m_WalkUnit.m_fRadius = fRad;

	//m_WalkUnit.m_iCollisionPlane = RBSP_EMPTY;
	m_WalkUnit.m_iCollisionPlanesNum = 0;
	
	CheckCollisiontoBrushe2();
	return m_WalkUnit.m_vecOPos;
}
D3DXVECTOR3 RBspScene::CollisionCheck(D3DXVECTOR3 vecBefore,D3DXVECTOR3 vecNew,float fRad)
{
	m_WalkUnit.m_vecBPos = vecBefore;
	m_WalkUnit.m_vecPos = vecNew;
	m_WalkUnit.m_fFraction = 1.0f;
	m_WalkUnit.m_bOutputStartsOut = true;
	m_WalkUnit.m_bOutputAllSolid = false;
	m_WalkUnit.m_fRadius = fRad;

	//m_WalkUnit.m_iCollisionPlane = RBSP_EMPTY;
	m_WalkUnit.m_iCollisionPlanesNum = 0;
	
	CheckCollisiontoBrushe();
	return m_WalkUnit.m_vecOPos;
}
void RBspScene::UpdateScene(D3DXVECTOR3 vecPos,bool bColl)
{
	bool bObjectUpdate = false;

	// Walk Unit Init 
  	m_WalkUnit.m_vecBPos = m_vecCamera;
	m_WalkUnit.m_vecPos = vecPos;
	m_WalkUnit.m_fFraction = 1.0f;
	m_WalkUnit.m_bOutputStartsOut = true;
	m_WalkUnit.m_bOutputAllSolid = false;
	//m_WalkUnit.m_iCollisionPlane = RBSP_EMPTY;
	m_WalkUnit.m_iCollisionPlanesNum = 0;
	
	//m_WalkUnit.m_vecOPos = vecPos;
	if(bColl)
		CheckCollisiontoBrushe();						// Brushe Collision 
	else
		m_WalkUnit.m_vecOPos = m_WalkUnit.m_vecPos;
	//m_vecCamera = m_WalkUnit.m_vecOPos;			// Camera Position Setting
	//CSceneManager::m_ViewCamera->SetVecPosition(vector3(m_vecCamera.x,m_vecCamera.y,m_vecCamera.z));
	//CSceneManager::m_ViewCamera->MoveFrustum();
	m_pFrustum->Update();
	//m_vecCamera = vecPos;							// Camera Position Setting
		
	int iLeafIndex;									// Leaf Node Index
	int iLeafClusterIndex;							// Leaf Cluster Index
	int iLeafsNum = 0;
	int iFaceCount = 0;
	m_FaceRenderBit.ClearBits();
	ClearBufferNum();								// Number Buffer Clear

	iLeafIndex = FindLeafNode(vecPos);
	
	m_iRBspLeafIndex = iLeafIndex;					// World Creator을 위한 leaf index setting

	iLeafClusterIndex = m_pLeafExs[iLeafIndex].m_iCluster;
	m_iVisLeafNodeNum = 0;
	iLeafsNum  = m_iLeafsNum;
	
	m_iBVisLeafs = m_iVisLeafs;									// Backup
	memcpy(m_pBVisLeafs,m_pVisLeafs,sizeof(int) * m_iVisLeafs);	// Backup

	m_iVisLeafs = 0;								// Visible Leaf Zero Setting 
													// Visible Leaf는 나중에 R3S Object 와 Map Effect 에 사용된다
//	UpdateNode(0,true,iLeafClusterIndex);

	while(iLeafsNum--)
	{
		RBspLeafEx *pLeaf = &(m_pLeafExs[iLeafsNum]);
	
		iFaceCount = pLeaf->m_iLeafFaceNum;
		if( iFaceCount <= 0 )		// face 가 없는 노드
			continue;

		if(!ClusterVisibleTest(iLeafClusterIndex,pLeaf->m_iCluster))
			continue;
 		// Leaf Visible Setting
		m_pVisLeafs[m_iVisLeafs] = iLeafsNum;
		m_iVisLeafs++;
		// Frustum cull
//		m_pFrustum->Update();

		if(!m_pFrustum->BoxInFrustum(pLeaf->m_vecMinBox,pLeaf->m_vecMaxBox))
			continue;
		
	
		//
		while( iFaceCount-- )
		{
			int iFaceIndex = m_pLeafFaces[ pLeaf->m_iLeafFace + iFaceCount ];
			if( m_pFaces[ iFaceIndex ].m_iFaceType != RBSP_POLYGON)		// Polygon 만 렌더
 				continue;
			if( !m_FaceRenderBit.GetBit(iFaceIndex) )// 그려진 face 가 아니면 render
			{
				
				m_iVisLeafNodeNum++;				
				m_FaceRenderBit.SetBit(iFaceIndex);
				PushFaceBuffer( iFaceIndex);
				
			}
		}
	
	}
	
	if( m_iVisLeafs != m_iBVisLeafs)
		GenerateR3SObject();
	else
	{
		int iCount = 0;

		for(int iCheck = 0; iCheck < m_iBVisLeafs; iCheck++)
		{
			if(m_pVisLeafs[iCheck] == m_pBVisLeafs[iCheck])
				iCount++;
		}
		if(iCount != m_iBVisLeafs)
			GenerateR3SObject();
		
	}
//  RCollisionDetection::SetTriangle(m_vColTriangle,m_iColTriangle);
  	SetBuffer();
	
}
bool RBspScene::FaceCulling(int iFaceIndex)		// true: Frustum 안에 face 존재 , false frustum Culling 
{

	int iStartIndex = m_pFaces[iFaceIndex].m_iStartVertex;
	int iVertexNum = m_pFaces[iFaceIndex].m_iVertexNum;
	int i;

	for( i = 0; i < iVertexNum; i++ )
	{
		
		if(m_pFrustum->PointInFrustum(m_pDotVertex[iStartIndex + i].m_vecPos.x,
									m_pDotVertex[iStartIndex + i].m_vecPos.y,
									m_pDotVertex[iStartIndex + i].m_vecPos.z))
			return true;		
		

	}
	
	return false;
}
void RBspScene::SetBuffer()
{
	if(m_iShaderVersion == RBSPSHADER_GF3) 
	{
		/*int i,j;
		vector<WBDotVertex *> DotVertex;
		vector<RBspLightVertex *>LightVertex;


		for(i=0;i<m_iTextureDefsNum;i++) {
			WBDotVertex *tmpVert = new WBDotVertex[m_pBaseVertexUseNum[i]];
			RBspLightVertex *tmpLightVert = new RBspLightVertex[m_pBaseVertexUseNum[i]];

			DotVertex.push_back(tmpVert); 
			LightVertex.push_back(tmpLightVert);

			WORD *tmpIndex = new WORD[m_pBaseIndexUseNum[i]];
			
			for(j=0;j<m_pBaseVertexUseNum[i];j++) {
				tmpLightVert[j].m_vecPos = tmpVert[j].m_vecPos = m_pVertexArr[i][j].m_vecPos;
				tmpVert[j].m_vecTexcoord = m_pVertexArr[i][j].m_vecTexcoord;
				tmpLightVert[j].m_vecNormal = tmpVert[j].m_vecNormal = m_pVertexArr[i][j].m_vecNormal;
				tmpLightVert[j].m_vecLightCoord = m_pVertexArr[i][j].m_vecTexcoord2;
				tmpVert[j].m_vecS = m_pVertexArr[i][j].m_vecS;
				tmpVert[j].m_vecSxT = m_pVertexArr[i][j].m_vecSxT;
				tmpVert[j].m_vecT = m_pVertexArr[i][j].m_vecT;
			}
			memcpy(tmpIndex,m_pIndexArr[i],sizeof(WORD) * m_pBaseIndexUseNum[i]);
			// 개량
			// Face Normal Generate
			//CreateFaceNormal(tmpVert,m_pBaseVertexUseNum[i],tmpIndex,m_pBaseIndexUseNum[i]/3);
			// Create S, T, SxT, 마지막인자 true setting 시 노말값 평균 값 사용 (부드러운 쉐이딩을 위해서.)
			//CreateBasis(tmpVert,m_pBaseVertexUseNum[i],tmpIndex,m_pBaseIndexUseNum[i]/3,true);

		}
		// Tangent S,Tangent SxT Calculate
		for(i=0;i<m_iTextureDefsNum; i++) 
		{

			if(m_pBaseVertexUseNum[i] > 0) {
				
				//RBspShaderVertex *tmp = NULL;
				WBDotVertex *tmp = NULL;
				//m_pBaseVertexBuffer[i]->Lock(0,m_pBaseVertexUseNum[i] * sizeof(RBspShaderVertex),(BYTE **)&tmp,D3DLOCK_DISCARD);
				m_pBaseVertexBuffer[i]->Lock(0,m_pBaseVertexUseNum[i] * sizeof(WBDotVertex),(BYTE **)&tmp,D3DLOCK_DISCARD);
				RBspLightVertex *tmpLight = NULL;
				m_pLightVertexBuffer[i]->Lock(0,m_pBaseVertexUseNum[i] * sizeof(RBspLightVertex),(BYTE **)&tmpLight,D3DLOCK_DISCARD);
				//m_pBaseVertexBuffer[i]->Lock(0,m_pBaseVertexUseNum[i] * sizeof(RBspDrawVertex),(BYTE **)&tmp,0);
				/*for(j=0;j<m_pBaseVertexUseNum[i];j++) {
					tmp->m_vecPos = DotVertex[i][j].m_vecPos;
					tmp->m_vecTangentS = DotVertex[i][j].m_vecS;
					tmp->m_vecTangentSxT = DotVertex[i][j].m_vecSxT;
					tmp->m_vecTexcoord = DotVertex[i][j].m_vecTexcoord;
					tmp++;
				}*/
		/*		memcpy(tmp,DotVertex[i],sizeof(WBDotVertex) * m_pBaseVertexUseNum[i]);
				memcpy(tmpLight,LightVertex[i],sizeof(RBspLightVertex) * m_pBaseVertexUseNum[i]);

				m_pBaseVertexBuffer[i]->Unlock();
				m_pLightVertexBuffer[i]->Unlock();

			}
			if(m_pBaseIndexUseNum[i] > 0) {
				WORD *tmp2 = NULL;
				m_pBaseIndexBuffer[i]->Lock(0,m_pBaseIndexUseNum[i] * sizeof(WORD),(BYTE **)&tmp2,D3DLOCK_DISCARD);
				//m_pBaseIndexBuffer[i]->Lock(0,m_pBaseIndexUseNum[i] * sizeof(WORD),(BYTE **)&tmp2,0);
				memcpy(tmp2,m_pIndexArr[i],sizeof(WORD) * m_pBaseIndexUseNum[i]);
				m_pBaseIndexBuffer[i]->Unlock();

			}

		}
		for(i=0;i<DotVertex.size();i++) {
			delete[] DotVertex[i];
			delete[] LightVertex[i];

		}
		DotVertex.clear();
		LightVertex.clear();*/
	}
	else if(m_iShaderVersion == RBSPSHADER_GF2)
	{
		int i;
		for(i=0;i<m_iTextureDefsNum; i++) 
		{
			if(m_pBaseVertexUseNum[i] > 0) {
				RBspDrawVertex *tmp = NULL;
				m_pBaseVertexBuffer[i]->Lock(0,m_pBaseVertexUseNum[i] * sizeof(RBspDrawVertex),(BYTE **)&tmp,D3DLOCK_DISCARD);
				memcpy(tmp,m_pVertexArr[i],sizeof(RBspDrawVertex) * m_pBaseVertexUseNum[i]);
				m_pBaseVertexBuffer[i]->Unlock();
			}
			if(m_pBaseIndexUseNum[i] > 0) {
				WORD *tmp2 = NULL;
				m_pBaseIndexBuffer[i]->Lock(0,m_pBaseIndexUseNum[i] * sizeof(WORD),(BYTE **)&tmp2,D3DLOCK_DISCARD);
				memcpy(tmp2,m_pIndexArr[i],sizeof(WORD) * m_pBaseIndexUseNum[i]);
				m_pBaseIndexBuffer[i]->Unlock();

			}

		}

	}
}

void RBspScene::PushFaceBuffer( int iFaceIndex )	// Face 들을 각 텍스쳐에 해당하는 버퍼로 모은다
{													// Back Face 컬링을 하고 들어간다	
	int i;
	
	int iLightMapId = m_pFaces[iFaceIndex].m_iLightMapId;

	int iTextureId = m_pFaces[iFaceIndex].m_iTexId;
	int iVertexNum = m_pFaces[iFaceIndex].m_iVertexNum;
	int iIndexNum = (iVertexNum - 2) * 3;
	int iStartIndex = m_pFaces[iFaceIndex].m_iStartVertex;
	int iVertCount = m_pBaseVertexUseNum[iTextureId];
	int iIndexCount = m_pBaseIndexUseNum[iTextureId];
	// Base Texture Buffer Setting

	/// Check Face Position
	//D3DXVECTOR3 vec1 = m_pDotVertex[iStartIndex].m_vecPos - m_vecCamera;
	//D3DXVECTOR3 vec2 = m_pDotVertex[iFaceIndex].m_vecNormal;
	//D3DXVECTOR3 vec1 = m_pVerts[iStartIndex].m_vecPos - m_vecCamera;
	//D3DXVECTOR3 vec2 = m_pVerts[iFaceIndex].m_vecNormal;

	// Back Face Culling Code
	/*float fDot = D3DXVec3Dot(&vec1,&vec2);
	if(fDot >=0.8) 
		return;
	*/
	int iCurrentUnit = m_pLightUnit[iTextureId].size();

	// LightMap Id Setting
	if(iCurrentUnit == 0)
	{
		RBspLightUnit tmp;
		tmp.m_iLightId = iLightMapId;
		tmp.m_iStartIndex = 0;
		tmp.m_iStartVertIndex = 0;
		m_pLightUnit[iTextureId].push_back(tmp);	
	}
	else if( (m_pLightUnit[iTextureId][iCurrentUnit - 1].m_iLightId) != iLightMapId)
	{
		RBspLightUnit tmp;
		tmp.m_iLightId = iLightMapId;
		tmp.m_iStartIndex = m_pBaseIndexUseNum[iTextureId];
		tmp.m_iStartVertIndex = m_pBaseVertexUseNum[iTextureId];
		m_pLightUnit[iTextureId].push_back(tmp);
	}
	for( i = 0; i < iVertexNum; i++) 
	{
		//m_pVertexArr[iTextureId][iVertCount++] = m_pDotVertex[iStartIndex + i];
		m_pVertexArr[iTextureId][iVertCount++] = m_pVerts[iStartIndex + i];
		
		if( i < 3) 
		{
			m_pIndexArr[iTextureId][iIndexCount++] = (m_pBaseVertexUseNum[iTextureId] + i);
		}
		else
		{
 			m_pIndexArr[iTextureId][iIndexCount++] = (m_pBaseVertexUseNum[iTextureId]);
			m_pIndexArr[iTextureId][iIndexCount++] = (m_pBaseVertexUseNum[iTextureId] + i-1);
			m_pIndexArr[iTextureId][iIndexCount++] = (m_pBaseVertexUseNum[iTextureId] + i);
		}
	}
	
	m_pBaseVertexUseNum[iTextureId] += iVertexNum;
	m_pBaseIndexUseNum[iTextureId] += iIndexNum;
	/*
	if(!bSkipCollision) {
		if (fStartFraction < fEndFraction)
		{
			if (fStartFraction > -1 && fStartFraction < m_WalkUnit.m_fFraction)
			{
				if (fStartFraction < 0)
					fStartFraction = 0;

  				m_WalkUnit.m_fFraction = fStartFraction;
				if(m_WalkUnit.m_fFraction < 0.8f) {
    					MessageBox(NULL,"Collision","Coll",MB_OK);	
					return;
				}
			}
		}
	
	}
	*/

}
void RBspScene::SetBox(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax)
{
	g_vecCube[0] = D3DXVECTOR3(vecMin.x,vecMin.y,vecMin.z);
	g_vecCube[1] = D3DXVECTOR3(vecMin.x,vecMax.y,vecMin.z);
	g_vecCube[2] = D3DXVECTOR3(vecMax.x,vecMin.y,vecMin.z);
	g_vecCube[3] = D3DXVECTOR3(vecMax.x,vecMax.y,vecMin.z);
	g_vecCube[4] = D3DXVECTOR3(vecMin.x,vecMin.y,vecMax.z);
	g_vecCube[5] = D3DXVECTOR3(vecMin.x,vecMax.y,vecMax.z);
	g_vecCube[6] = D3DXVECTOR3(vecMax.x,vecMin.y,vecMax.z);
	g_vecCube[7] = D3DXVECTOR3(vecMax.x,vecMax.y,vecMax.z);
}

void RBspScene::RenderEvent()
{
	int i;
	int j;

	int iLeafIndex = m_iRBspLeafIndex;
	m_D3Device->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_D3Device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_D3Device->SetVertexShader(D3DFVF_XYZ);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_D3Device->SetTransform(D3DTS_WORLD,&identity);
				
	/*
		RBSPEVENT_OBJECTANI,		// Dynamic Object
		RBSPEVENT_EFFECT,			// Effect
		RBSPEVENT_SOUND,			// Sound
		RBSPEVENT_TOTAL,		

		m_bVisSoundBox = true;
		m_bVisAObjectBox = true;


	*/

	for( i = 0; i < m_iLeafsNum; i++ )
	{
		if(m_bVisEffectBox)							// Effect Box Render
		{
			for(j = 0; j < m_pLeafExs[i].m_iEventNum[RBSPEVENT_EFFECT]; j++)
			{
				// Local to world
			/*	/////////////
				D3DXVECTOR3 vecPos = m_pLeafExs[i].m_lstEffect[j]->m_vecCenter;
				D3DXMATRIX matTmp;
				D3DXMATRIX matInv;

				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
				matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
				matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;
				matInv = matTmp;
				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = vecPos.x;
				matTmp._42 = vecPos.y;
				matTmp._43 = vecPos.z;
				D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
				vecPos.x = matTmp._41;
				vecPos.y = matTmp._42;
				vecPos.z = matTmp._43;						
				
				///////////*/
				D3DXVECTOR3 vecMinBox = D3DXVECTOR3(m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.x -10.0f,
													m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.y -10.0f,
													m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.z -10.0f);
				D3DXVECTOR3 vecMaxBox = D3DXVECTOR3(m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.x +10.0f,
													m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.y +10.0f,
													m_pLeafExs[i].m_lstEffect[j]->m_vecCenter.z +10.0f);
				
				
				SetBox(vecMinBox,vecMaxBox);

				if(m_pLeafExs[i].m_lstEffect[j] == m_pLeafExs[i].m_pPickEventEffect) 
				{
					m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff0000);
					

				}	
				else
				{
					m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xff00ff00);
					
				}
				m_D3Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
					0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
			}
		}
		if(m_bVisLightBox)
		{
			for(j = 0; j < m_pLeafExs[i].m_iLightNum; j++)
			{
		
				// Local to World
				CSphere tmpSphere;
				
				tmpSphere.SetTransparency(40);
				tmpSphere.SetColor(m_pLeafExs[i].m_lstLight[j].m_fRed,
									m_pLeafExs[i].m_lstLight[j].m_fGreen,
									m_pLeafExs[i].m_lstLight[j].m_fBlue);

				tmpSphere.SetRadius(m_pLeafExs[i].m_lstLight[j].m_vecLightRange.x);
				tmpSphere.Create(m_D3Device,10,20);

				/////////////
				/*D3DXVECTOR3 vecPos = m_pLeafExs[i].m_lstLight[j].m_vecLight;
				D3DXMATRIX matTmp;
				D3DXMATRIX matInv;

				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
				matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
				matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;
				matInv = matTmp;
				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = vecPos.x;
				matTmp._42 = vecPos.y;
				matTmp._43 = vecPos.z;
				D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
				vecPos.x = matTmp._41;
				vecPos.y = matTmp._42;
				vecPos.z = matTmp._43;						
				*/
				///////////

				D3DXVECTOR3 vecMinBox = D3DXVECTOR3(m_pLeafExs[i].m_lstLight[j].m_vecLight.x -10.0f,
													m_pLeafExs[i].m_lstLight[j].m_vecLight.y -10.0f,
													m_pLeafExs[i].m_lstLight[j].m_vecLight.z -10.0f);
				D3DXVECTOR3 vecMaxBox = D3DXVECTOR3(m_pLeafExs[i].m_lstLight[j].m_vecLight.x +10.0f,
													m_pLeafExs[i].m_lstLight[j].m_vecLight.y +10.0f,
													m_pLeafExs[i].m_lstLight[j].m_vecLight.z +10.0f);
				
				tmpSphere.SetPosition(m_pLeafExs[i].m_lstLight[j].m_vecLight.x,m_pLeafExs[i].m_lstLight[j].m_vecLight.y,m_pLeafExs[i].m_lstLight[j].m_vecLight.z);
				SetBox(vecMinBox,vecMaxBox);

				if((&m_pLeafExs[i].m_lstLight[j]) == m_pLeafExs[i].m_pPickLight) 
				{
					m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff0000);
									
	
				}	
				else
				{
					m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xff00ff00);
					
				}
				m_D3Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
					0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
				m_D3Device->SetRenderState(D3DRS_FOGENABLE,FALSE);
				tmpSphere.Render();
				m_D3Device->SetRenderState(D3DRS_FOGENABLE,TRUE);

			}
		}
	}

	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,TRUE);
}
void RBspScene::RenderLeafBox()			// Leaf Box render
{
	int i;

	int iLeafIndex = m_iRBspLeafIndex;
	m_D3Device->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_D3Device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_D3Device->SetVertexShader(D3DFVF_XYZ);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,FALSE);

	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	

	for( i = 0; i < m_iLeafsNum; i++ )
	{
		if(iLeafIndex == i) {
			D3DXMATRIX identity;
			D3DXMatrixIdentity(&identity);
			m_D3Device->SetTransform(D3DTS_WORLD,&identity);
			D3DXVECTOR3 vecMinBox = D3DXVECTOR3((float)m_pLeafExs[i].m_vecMinBox[0],
												(float)m_pLeafExs[i].m_vecMinBox[1],
												(float)m_pLeafExs[i].m_vecMinBox[2]);

			D3DXVECTOR3 vecMaxBox = D3DXVECTOR3((float)m_pLeafExs[i].m_vecMaxBox[0],
												(float)m_pLeafExs[i].m_vecMaxBox[1],
												(float)m_pLeafExs[i].m_vecMaxBox[2]);

			SetBox(vecMinBox,vecMaxBox);

		/*	if(iLeafIndex == i) 
			{*/
			m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffff00);
			m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

		/*	}	
			else
			{
				m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xff00ffff);
				m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
			}*/
			m_D3Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
				0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
		}
	}

	m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,TRUE);
	
}
void RBspScene::RenderBspScene()
{
	int i,j;

	int iSize;
	
	int iRenderCount = 0;

//	RenderShadowMap();
	if(m_iShaderVersion == RBSPSHADER_GF3)
	{
/*		WBGraphicLayer::GetDevice()->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
		

		WBGraphicLayer::GetDevice()->SetTextureStageState(1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(1,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

		WBGraphicLayer::GetDevice()->SetTextureStageState(3,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(3,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		WBGraphicLayer::GetDevice()->SetTextureStageState(3,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);

		// Base texture stage
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
		WBGraphicLayer::GetDevice()->SetRenderState( D3DRS_WRAP0, 0 );

		// bump map texture stage
		WBGraphicLayer::GetDevice()->SetTextureStageState(1, D3DTSS_ADDRESSU,	D3DTADDRESS_WRAP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(1, D3DTSS_ADDRESSV,	D3DTADDRESS_WRAP);
		WBGraphicLayer::GetDevice()->SetRenderState( D3DRS_WRAP1, 0 );

		// Illumination texture
		WBGraphicLayer::GetDevice()->SetTextureStageState(2, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(2, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);	
		WBGraphicLayer::GetDevice()->SetTextureStageState(2, D3DTSS_ADDRESSW,	D3DTADDRESS_CLAMP);
		WBGraphicLayer::GetDevice()->SetRenderState( D3DRS_WRAP2, 0);

		WBGraphicLayer::GetDevice()->SetTextureStageState(3, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(3, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);	
		WBGraphicLayer::GetDevice()->SetTextureStageState(3, D3DTSS_ADDRESSW,	D3DTADDRESS_CLAMP);	


		// One Pass
		for( i = 0; i < m_iTextureDefsNum; i++ )
		{
			if(m_pBaseVertexUseNum[i] > 0) {	

				m_pShader->SetTexture(&(m_pTextures[i]),&(m_pNTextures[i]));
				m_pShader->Apply();	
				
				WBGraphicLayer::GetDevice()->SetStreamSource(0,m_pBaseVertexBuffer[i],sizeof(WBDotVertex));
				WBGraphicLayer::GetDevice()->SetIndices(m_pBaseIndexBuffer[i],0);
				
				iSize = m_pLightUnit[i].size();
				
				for(j=0;j < iSize;j++) 
				{

					if(j != (iSize - 1)) {	
						
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pLightUnit[i][j+1].m_iStartVertIndex - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pLightUnit[i][j+1].m_iStartIndex - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
					else {
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pBaseVertexUseNum[i] - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pBaseIndexUseNum[i] - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
				}	
			}
		}

		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		
		// Two Pass (Light Map)
		for( i = 0; i < m_iTextureDefsNum; i++ )
		{
			if(m_pBaseVertexUseNum[i] > 0) {	

				WBGraphicLayer::GetDevice()->SetStreamSource(0,m_pLightVertexBuffer[i],sizeof(RBspLightVertex));
				WBGraphicLayer::GetDevice()->SetIndices(m_pBaseIndexBuffer[i],0);
				
				iSize = m_pLightUnit[i].size();
				
				for(j=0;j < iSize;j++) 
				{

					if(m_pLightUnit[i][j].m_iLightId < 0) {		// Lightmap 없을때
						m_pShadowShader->SetTexture(NULL);
						//m_pShadowShader->SetTexture(NULL,m_pDepthTexture);
					}
					else 
						//m_pShadowShader->SetTexture(&(m_pLightTextures[m_pLightUnit[i][j].m_iLightId]),m_pColorTexture);
						m_pShadowShader->SetTexture(&(m_pLightTextures[m_pLightUnit[i][j].m_iLightId]));
					// Shadow Mapping 
					m_pShadowShader->Apply();	
				
					
					if(j != (iSize - 1)) {	
						
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pLightUnit[i][j+1].m_iStartVertIndex - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pLightUnit[i][j+1].m_iStartIndex - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
					else {
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pBaseVertexUseNum[i] - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pBaseIndexUseNum[i] - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
				}	
			}
		}

		// tree pass (test pass :  layer fog)
		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		//WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		
				
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
		//WBGraphicLayer::GetDevice()->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
		WBGraphicLayer::GetDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		m_pFogShader->Apply();

		for( i = 0; i < m_iTextureDefsNum; i++ )
		{
			if(m_pBaseVertexUseNum[i] > 0) {	

				WBGraphicLayer::GetDevice()->SetStreamSource(0,m_pLightVertexBuffer[i],sizeof(RBspLightVertex));
				WBGraphicLayer::GetDevice()->SetIndices(m_pBaseIndexBuffer[i],0);
				
				iSize = m_pLightUnit[i].size();
				
				for(j=0;j < iSize;j++) 
				{		
					
					if(j != (iSize - 1)) {	
						
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pLightUnit[i][j+1].m_iStartVertIndex - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pLightUnit[i][j+1].m_iStartIndex - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
					else {
						WBGraphicLayer::GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pBaseVertexUseNum[i] - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pBaseIndexUseNum[i] - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
				}	
			}
		}
		WBGraphicLayer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);*/
	}
	else if(m_iShaderVersion == RBSPSHADER_GF2)
	{
		m_D3Device->SetVertexShader(dwRBSPUseFVF);
		
		m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		

		for( i = 0; i < m_iTextureDefsNum; i++ )
		{

			if(m_pBaseVertexUseNum[i] > 0) {	
				m_D3Device->SetTexture(0,m_pTextures[i].GetTexture());	
				m_D3Device->SetStreamSource(0,m_pBaseVertexBuffer[i],sizeof(RBspDrawVertex));
				m_D3Device->SetIndices(m_pBaseIndexBuffer[i],0);
				
				iSize = m_pLightUnit[i].size();
				
				for(j=0;j < iSize;j++) 
				{

					if(m_pLightUnit[i][j].m_iLightId < 0) {		// Lightmap 없을때
						m_D3Device->SetTexture(1,NULL);
						m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);			
					}
					else 
					{
						m_D3Device->SetTexture(1,m_pLightTextures[m_pLightUnit[i][j].m_iLightId].GetTexture());
						m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					}
					if(j != (iSize - 1)) {	
						
						m_D3Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pLightUnit[i][j+1].m_iStartVertIndex - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pLightUnit[i][j+1].m_iStartIndex - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
					else {
						m_D3Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
													m_pLightUnit[i][j].m_iStartVertIndex,(m_pBaseVertexUseNum[i] - m_pLightUnit[i][j].m_iStartVertIndex), m_pLightUnit[i][j].m_iStartIndex, 
													((m_pBaseIndexUseNum[i] - m_pLightUnit[i][j].m_iStartIndex)/ 3));
						iRenderCount++;
					}
				}	
			}
		}// for
		
		RenderR3SObject();

	}
}
// Return value : 총 사용된 light, Arg: PLight 는 배열, iLightNum 은 배열 요소 갯수
int RBspScene::CheckChrLight(D3DXVECTOR3 vecPos,D3DLIGHT8 *pLight,int iLightNum)
{
	D3DXVECTOR3 vecLocalPos = vecPos;
	D3DXMATRIX matTmp;
	D3DXMATRIX matInv;

	D3DXMatrixIdentity(&matTmp);

	matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
	matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
	matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;
	D3DXMatrixInverse(&matInv,NULL,&matTmp);
	D3DXMatrixIdentity(&matTmp);
	matTmp._41 = vecPos.x;
	matTmp._42 = vecPos.y;
	matTmp._43 = vecPos.z;
	D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
	vecLocalPos.x = matTmp._41;
	vecLocalPos.y = matTmp._42;
	vecLocalPos.z = matTmp._43;						


	int iLeaf = FindLeafNode(vecLocalPos);
	
	RBspLeafEx *pLeafEx = &(m_pLeafExs[iLeaf]);
	
	return pLeafEx->CheckInChrLight(vecPos,pLight,iLightNum);
}

int RBspScene::RBspLeafEx::CheckInChrLight(D3DXVECTOR3 vecPos,D3DLIGHT8 *pLight,int iLightNum)
{
	int i;
	float fMinLength = 0.0f;
	int iMinIndex = -1;
	int iInputNum = 0;

	for( i=0; i < m_iLightNum; i++ )
	{
		D3DXVECTOR3 vecUnit = vecPos - m_lstLight[i].m_vecLight;
		float fLength = D3DXVec3Length(&vecUnit);
		
		if(fLength <= m_lstLight[i].m_vecLightRange.x)
		{	
			if(iInputNum < iLightNum)
			{
				pLight[iInputNum] = m_lstLight[i].m_D3DLight;
				iInputNum++;
			}
		}
	}

	return iInputNum;
}
/*
D3DLIGHT8 *RBspScene::RBspLeafEx::CheckInChrLight(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iMinIndex = -1;

	for( i=0; i < m_iLightNum; i++ )
	{
		D3DXVECTOR3 vecUnit = vecPos - m_lstLight[i].m_vecLight;
		float fLength = D3DXVec3Length(&vecUnit);
		
		if(fLength <= m_lstLight[i].m_vecLightRange.x)
		{	

			if( iMinIndex == -1)
			{
				fMinLength = fLength;
				iMinIndex = i;
			}
			else if(fLength < fMinLength)
			{
				fMinLength = fLength;
				iMinIndex = i;
			}
		}
	}
	if(iMinIndex != -1)
		return &(m_lstLight[iMinIndex].m_D3DLight);

	return NULL;

}*/

void RBspScene::RBspLeafEx::InputLight(int iLightValue,D3DXVECTOR3 vecLight,D3DXVECTOR3 vecRange,int iRed,int iGreen,int iBlue,bool bVolume,bool bMap)
{
	// Local transform
/*	D3DXVECTOR3 vecLocalPos = vecLight;
	D3DXMATRIX matTmp;
	D3DXMATRIX matInv;

	D3DXMatrixIdentity(&matTmp);

	matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
	matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
	matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;
	D3DXMatrixInverse(&matInv,NULL,&matTmp);
	D3DXMatrixIdentity(&matTmp);
	matTmp._41 = vecLight.x;
	matTmp._42 = vecLight.y;
	matTmp._43 = vecLight.z;
	D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
	vecLocalPos.x = matTmp._41;
	vecLocalPos.y = matTmp._42;
	vecLocalPos.z = matTmp._43;						
	
	// World 좌표 세팅
	

	vecLight = vecLocalPos;*/
	//
	RBspLight PushNode;
	// Point Light or Directional Light
	PushNode.m_iLightValue = iLightValue;
	PushNode.m_vecLight = vecLight;
	PushNode.m_vecLightRange = vecRange;
	PushNode.m_fRed = (float)iRed / 255.0f;
	PushNode.m_fGreen = (float)iGreen / 255.0f;
	PushNode.m_fBlue = (float)iBlue / 255.0f;

	ZeroMemory(&(PushNode.m_D3DLight),sizeof(D3DLIGHT8));
	PushNode.m_D3DLight.Type=D3DLIGHT_POINT;					
	PushNode.m_D3DLight.Attenuation0=0.2f;					
	PushNode.m_D3DLight.Attenuation1=0.005f;
	PushNode.m_D3DLight.Attenuation2=0.0f;
	
	PushNode.m_D3DLight.Diffuse.r = PushNode.m_fRed;
	PushNode.m_D3DLight.Diffuse.g = PushNode.m_fGreen;
	PushNode.m_D3DLight.Diffuse.b = PushNode.m_fBlue;

	PushNode.m_D3DLight.Position.x = PushNode.m_vecLight.x;
	PushNode.m_D3DLight.Position.y = PushNode.m_vecLight.y;
	PushNode.m_D3DLight.Position.z = PushNode.m_vecLight.z;
	PushNode.m_D3DLight.Range= PushNode.m_vecLightRange.x;
	PushNode.m_D3DLight.Falloff = 1.0f;

	m_lstLight.push_back(PushNode);
	if(bVolume)
		m_lstLight[m_iLightNum - 1].CreateShadowVolume();
	if(bMap)
		m_lstLight[m_iLightNum - 1].CreateShadowMap();

	m_iLightNum++;

}
void RBspScene::RBspLeafEx::DeleteAllTrigger()
{
	m_lstTrigger.clear();
	m_iTriggerNum = 0;
}

void RBspScene::RBspLeafEx::DeleteAllLight()
{
	m_lstLight.clear();
	m_iLightNum = 0;
}

void RBspScene::RBspLeafEx::DeleteLight(RBspScene::RBspLight *pLight)
{
	int i;
	int iIndex = -1;

	if(pLight == NULL)
		return;
	
	for( i = 0; i < m_iLightNum; i++ )
	{
		if((&m_lstLight[i]) == pLight)
		{
			iIndex = i;
			break;
		}	
	}
	if(iIndex != -1)
	{
		m_lstLight.erase(m_lstLight.begin() + iIndex);
		m_iLightNum--;
	}
}
void RBspScene::RBspLeafEx::DeleteTrigger(RBspScene::RBspTrigger *pTrigger)
{
	int i;
	int iIndex = -1;
	 
	if(pTrigger == NULL)
		return;
	for( i = 0; i < m_iTriggerNum; i++)
	{
		if((&m_lstTrigger[i]) == pTrigger)
		{
			iIndex = i;
			break;
		}
	}
	if(iIndex != -1)
	{
		m_lstTrigger.erase(m_lstTrigger.begin() + iIndex);
		m_iTriggerNum--;

	}
}
void RBspScene::RBspLeafEx::DeleteLight(D3DXVECTOR3 vecLight)
{
	int i;
	for( i = 0; i < m_iLightNum; i++)
	{
		if(((vecLight.x - m_lstLight[i].m_vecLight.x) +
			(vecLight.y - m_lstLight[i].m_vecLight.y) +
			(vecLight.z - m_lstLight[i].m_vecLight.z)) < RBSPEPS)	// 같은점
		{
			m_lstLight.erase(m_lstLight.begin() + i);
			m_iLightNum--;

			return;
		}

	}
}
void RBspScene::RBspLeafEx::DeleteTrigger(D3DXVECTOR3 vecCenter)
{
	int i;
	
	for( i = 0; i < m_iTriggerNum; i++ )
	{
		if(((vecCenter.x - m_lstTrigger[i].m_vecCenter.x) +
			(vecCenter.x - m_lstTrigger[i].m_vecCenter.x) +
			(vecCenter.x - m_lstTrigger[i].m_vecCenter.x)) < RBSPEPS)
		{
			m_lstTrigger.erase(m_lstTrigger.begin() + i);
			m_iTriggerNum--;
			return;

		}
	}
}
void RBspScene::RBspLeafEx::MoveLight(D3DXVECTOR3 vecBeforePos,D3DXVECTOR3 vecNewPos)
{

	int i;
	for( i = 0; i < m_iLightNum; i++)
	{
		if(((vecBeforePos.x - m_lstLight[i].m_vecLight.x) +
			(vecBeforePos.y - m_lstLight[i].m_vecLight.y) +
			(vecBeforePos.z - m_lstLight[i].m_vecLight.z)) < RBSPEPS)	// 같은점
		{
			m_lstLight[i].m_vecLight = D3DXVECTOR3( vecNewPos.x,
													vecNewPos.y,
													vecNewPos.z);
			return;
		}

	}
}

int RBspScene::RBspLeafEx::InputEventSound(char *strName,D3DXVECTOR3 vecCenter,float fDelay)
{
	m_iEventNum[ RBSPEVENT_SOUND]++;
	RBspEventSound *pNode = new RBspEventSound;
	strncpy(pNode->m_strName,strName,sizeof(char) * (RBSPBUF_SIZE - 1));
	pNode->m_fDelay = fDelay;
	pNode->m_vecCenter = vecCenter;
	m_lstSound.push_back(pNode);

	return m_iEventNum[ RBSPEVENT_SOUND];

}
void RBspScene::RBspLeafEx::DeletePickEventEffect()
{
	if(m_pPickEventEffect == NULL)
		return;
	else
	{
		int i;
		for( i = 0; i < m_iEventNum[RBSPEVENT_EFFECT]; i++ )
		{
			if(m_pPickEventEffect == m_lstEffect[i])
			{
				delete m_lstEffect[i];
				m_lstEffect[i] = NULL;
				m_iEventNum[RBSPEVENT_EFFECT]--;
				m_lstEffect.erase(m_lstEffect.begin() + i);
				m_pPickEventEffect = NULL;
				break;
			}
		}
	}
}
void RBspScene::RBspLeafEx::DeletePickEventObject()
{
	if(m_pPickEventObject == NULL)
		return;
	else
	{
		int i;
		for( i = 0; i < m_iEventNum[RBSPEVENT_OBJECTANI]; i++ )
		{
			if(m_pPickEventObject == m_lstObject[i])
			{
				delete m_lstObject[i];
				m_lstObject[i] = NULL;
				m_iEventNum[RBSPEVENT_OBJECTANI]--;
				m_lstObject.erase(m_lstObject.begin() + i);
				m_pPickEventObject = NULL;
				break;

			}
		}
	}
}
void RBspScene::RBspLeafEx::DeletePickEventSound()
{
	if(m_pPickEventSound == NULL)
		return;
	else
	{
		int i;
		for( i = 0; i < m_iEventNum[RBSPEVENT_SOUND]; i++ )
		{
			if(m_pPickEventSound == m_lstSound[i])
			{
				delete m_lstSound[i];
				m_lstSound[i] = NULL;
				m_iEventNum[RBSPEVENT_SOUND]--;
				m_lstSound.erase(m_lstSound.begin() + i);
				m_pPickEventSound = NULL;
				break;

			}
		}
	}

}
void RBspScene::RBspLeafEx::DeletePickLight()
{
	if(m_pPickLight == NULL)
		return;
	else
	{
		int i;
		for( i = 0; i < m_iLightNum; i++ )
		{
			if(m_pPickLight == (&m_lstLight[i]))
			{
				m_lstLight.erase(m_lstLight.begin() + i);
				m_iLightNum--;
				m_pPickLight = NULL;
				return;
			}
		}
	}
}
void RBspScene::RBspLeafEx::DeletePickTrigger()
{
	if(m_pPickTrigger == NULL)
		return;
	else
	{
		int i;
		for( i = 0 ; i < m_iTriggerNum; i++ )
		{
			if(m_pPickTrigger == (&m_lstTrigger[i]))
			{
				m_lstTrigger.erase(m_lstTrigger.begin() + i);
				m_iTriggerNum--;
				m_pPickTrigger = NULL;
				return;
			}
		}
	}

}
void RBspScene::RBspLeafEx::DeletePickR3SObject()
{
	if(m_pPickR3SObject == NULL)
		return;
	else
	{
		int i;
		for( i = 0; i < m_iR3SObjectNum; i++ )
		{
			if(m_pPickR3SObject == m_lstR3SObject[i])
			{
				delete (m_lstR3SObject[i]);
				m_lstR3SObject[i] = NULL;
				m_lstR3SObject.erase(m_lstR3SObject.begin() + i);
				m_iR3SObjectNum--;
				return;

			}
		}
	}
}
void RBspScene::RBspLeafEx::DeleteEventSound(D3DXVECTOR3 vecCenter)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_SOUND ]; i++)
	{
		
		if(((vecCenter.x - m_lstSound[i]->m_vecCenter.x) + 
			(vecCenter.y - m_lstSound[i]->m_vecCenter.y) + 
			(vecCenter.z - m_lstSound[i]->m_vecCenter.z)) < RBSPEPS) 
		{
			delete m_lstSound[i];
			m_lstSound.erase(m_lstSound.begin() + i);
			m_iEventNum[ RBSPEVENT_SOUND ]--;
			return;

		}
	}
}
void RBspScene::RBspLeafEx::MoveEventSound(D3DXVECTOR3 vecCenter,D3DXVECTOR3 vecEndPos)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_SOUND ]; i++ )
	{	
		if(((vecCenter.x - m_lstSound[i]->m_vecCenter.x) + 
			(vecCenter.y - m_lstSound[i]->m_vecCenter.y) + 
			(vecCenter.z - m_lstSound[i]->m_vecCenter.z)) < RBSPEPS) 
		{
			m_lstSound[i]->m_vecCenter = vecEndPos;
			return;

		}

	}
}
// R3S Object Input
int RBspScene::RBspLeafEx::InputR3SObject(float fX,float fY,float fZ,char *strName,matrix matTm,bool bAlpha,bool bLight)
{
	RBspR3S *pNode = new RBspR3S;
	pNode->m_vecPos = vector3(fX,fY,fZ);
	strcpy((pNode->m_strName),strName);
	pNode->m_matTm = matTm;
	pNode->m_bAlpha = bAlpha;
	pNode->m_bLight = bLight;

	m_lstR3SObject.push_back(pNode);
	m_iR3SObjectNum++;
	
	return m_iR3SObjectNum;

}
int RBspScene::RBspLeafEx::InputEventEffect(char *strName,D3DXVECTOR3 vecCenter,float fDelay)
{
	// Local transform
/*	D3DXVECTOR3 vecLocalPos = vecCenter;
	D3DXMATRIX matTmp;
	D3DXMATRIX matInv;

	D3DXMatrixIdentity(&matTmp);

	matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
	matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
	matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;
	D3DXMatrixInverse(&matInv,NULL,&matTmp);
	D3DXMatrixIdentity(&matTmp);
	matTmp._41 = vecCenter.x;
	matTmp._42 = vecCenter.y;
	matTmp._43 = vecCenter.z;
	D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
	vecLocalPos.x = matTmp._41;
	vecLocalPos.y = matTmp._42;
	vecLocalPos.z = matTmp._43;						

	vecCenter = vecLocalPos;
*/	//

	m_iEventNum[ RBSPEVENT_EFFECT]++;
	RBspEventEffect *pNode = new RBspEventEffect;
	strncpy(pNode->m_strName,strName,sizeof(char) * (RBSPBUF_SIZE - 1));
	pNode->m_fDelay = fDelay;
	pNode->m_vecCenter = vecCenter;
	m_lstEffect.push_back(pNode);

	return m_iEventNum[ RBSPEVENT_EFFECT];

}
void RBspScene::RBspLeafEx::DeleteEventEffect(D3DXVECTOR3 vecCenter)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_EFFECT ]; i++)
	{
		
		if(((vecCenter.x - m_lstEffect[i]->m_vecCenter.x) + 
			(vecCenter.y - m_lstEffect[i]->m_vecCenter.y) + 
			(vecCenter.z - m_lstEffect[i]->m_vecCenter.z)) < RBSPEPS) 
		{
			delete m_lstEffect[i];
			m_lstEffect.erase(m_lstEffect.begin() + i);
			m_iEventNum[ RBSPEVENT_EFFECT ]--;
			return;

		}
	}
}
void RBspScene::RBspLeafEx::MoveEventEffect(D3DXVECTOR3 vecCenter,D3DXVECTOR3 vecEndPos)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_EFFECT ]; i++ )
	{	
		if(((vecCenter.x - m_lstEffect[i]->m_vecCenter.x) + 
			(vecCenter.y - m_lstEffect[i]->m_vecCenter.y) + 
			(vecCenter.z - m_lstEffect[i]->m_vecCenter.z)) < RBSPEPS) 
		{
			m_lstEffect[i]->m_vecCenter = vecEndPos;
			return;

		}

	}

}
int RBspScene::RBspLeafEx::InputEventObject(RAniObject *pObject,int iLoop,float fDelay)
{
	m_iEventNum[ RBSPEVENT_OBJECTANI]++;
	RBspEventObject *pNode = new RBspEventObject;
	pNode->m_fDelay = fDelay;
	pNode->m_iLoopNum = iLoop;
	pNode->m_lstAniObject.push_back(pObject);
	m_lstObject.push_back(pNode);

	return m_iEventNum[ RBSPEVENT_OBJECTANI];

}
void RBspScene::RBspLeafEx::DeleteEventObject(RAniObject *pObject)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_OBJECTANI]; i++ )
	{
		if(m_lstObject[i]->m_lstAniObject[0] == pObject)
		{
			delete m_lstObject[i];
			m_lstObject.erase(m_lstObject.begin() + i);
			m_iEventNum[ RBSPEVENT_OBJECTANI]--;
			return;

		}
	}
}

void RBspScene::RBspLeafEx::MoveEventObject(RAniObject *pObject,D3DXVECTOR3 vecLocalMove) 		// 상대적인 이동값
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_OBJECTANI]; i++ )
	{
		if(m_lstObject[i]->m_lstAniObject[0] == pObject)
		{
			// RAniObject 객체 move
			// m_lstObject[i]->m_lstAniObject[0]->Move(vecLocalMove);
			return;

		}
	}
}
void RBspScene::RBspLeafEx::DeleteEventEffect(RBspEventEffect *pEffect)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_EFFECT ]; i++ )
	{
		if(m_lstEffect[i] == pEffect)
		{
			delete m_lstEffect[i];
			m_lstEffect[i] = NULL;
			m_lstEffect.erase(m_lstEffect.begin() + i);
			m_iEventNum[ RBSPEVENT_EFFECT ]--;
			break;
		}
	}
}
void RBspScene::RBspLeafEx::DeleteEventSound(RBspEventSound *pSound)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_SOUND ]; i++ )
	{
		if(m_lstSound[i] == pSound)
		{
			delete m_lstSound[i];
			m_lstSound[i] = NULL;
			m_lstSound.erase(m_lstSound.begin() + i);
			m_iEventNum[ RBSPEVENT_SOUND ]--;
			break;
		}
	}
}
void RBspScene::RBspLeafEx::DeleteEventObject(RBspEventObject *pObject)
{
	int i;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_OBJECTANI ]; i++ )
	{
		delete m_lstObject[i];
		m_lstObject[i] = NULL;
		m_lstObject.erase(m_lstObject.begin() + i);
		m_iEventNum[ RBSPEVENT_OBJECTANI ]--;
		break;

	}
}

// Picking
void RBspScene::RBspLeafEx::PickR3SObject(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;

	for( i = 0; i < m_iR3SObjectNum; i++ )
	{
		D3DXVECTOR3 vecObject = D3DXVECTOR3(m_lstR3SObject[i]->m_vecPos.x,m_lstR3SObject[i]->m_vecPos.y,m_lstR3SObject[i]->m_vecPos.z);

		D3DXVECTOR3 vecTmp = vecObject - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if(i == 0)
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if(fMinLength > fTmp) 
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
	}
	if(iPickIndex == -1)
		m_pPickR3SObject = NULL;
	else
	{
		m_pPickR3SObject = m_lstR3SObject[iPickIndex];
		m_lstR3SObject[iPickIndex]->m_iPickState = RBSPPICK_PICKED;
	}
}
void RBspScene::RBspLeafEx::PickLight(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;

	for( i = 0; i < m_iLightNum; i++ )
	{
		D3DXVECTOR3 vecTmp = m_lstLight[i].m_vecLight - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if(i == 0)
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if(fMinLength > fTmp) 
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
	}
	if(iPickIndex == -1)
		m_pPickLight = NULL;
	else
	{
		m_pPickLight = &m_lstLight[iPickIndex];
		m_pPickLight->m_iPickState = RBSPPICK_PICKED;
	}
}
void RBspScene::RBspLeafEx::PickTrigger(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;
	for( i = 0; i < m_iTriggerNum; i++ )
	{
		D3DXVECTOR3 vecTmp = m_lstTrigger[i].m_vecCenter - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if(i == 0)
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if(fMinLength > fTmp) 
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
	}
	if(iPickIndex == -1)
		m_pPickTrigger = NULL;
	else
	{
		m_pPickTrigger = &m_lstTrigger[iPickIndex];
		m_pPickTrigger->m_iPickState = RBSPPICK_PICKED;
	}
}
void RBspScene::RBspLeafEx::PickEventEffect(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_EFFECT ]; i++)
	{
		D3DXVECTOR3 vecTmp = m_lstEffect[i]->m_vecCenter - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if( i == 0 )
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if(fMinLength > fTmp)
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
	}
	if(iPickIndex == -1)
		m_pPickEventEffect = NULL;
	else 
	{
		m_pPickEventEffect = m_lstEffect[iPickIndex];
		m_pPickEventEffect->m_iPickState = RBSPPICK_PICKED;

	}
}
void RBspScene::RBspLeafEx::PickEventSound(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;
	for( i = 0; i < m_iEventNum[ RBSPEVENT_SOUND ]; i++ )
	{
		D3DXVECTOR3 vecTmp = m_lstSound[i]->m_vecCenter - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if( i == 0 )
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if( fMinLength > fTmp)
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
	}
	if(iPickIndex == -1)
		m_pPickEventSound = NULL;
	else
	{
		m_pPickEventSound = m_lstSound[iPickIndex];
		m_pPickEventSound->m_iPickState = RBSPPICK_PICKED;
	}		
}
void RBspScene::RBspLeafEx::PickEventObject(D3DXVECTOR3 vecPos)
{
	int i;
	float fMinLength = 0.0f;
	int iPickIndex = -1;
	
	for( i = 0; i < m_iEventNum[ RBSPEVENT_OBJECTANI ]; i++ )
	{
		D3DXVECTOR3 vecTmp = m_lstObject[i]->m_vecCenter - vecPos;
		float fTmp = D3DXVec3Length(&vecTmp);
		if( i == 0 )
		{
			fMinLength = fTmp;
			iPickIndex = i;
		}
		else if( fMinLength > fTmp)
		{
			fMinLength = fTmp;
			iPickIndex = i;

		}
	}
	if(iPickIndex == -1)
		m_pPickEventObject = NULL;
	else
	{
		m_pPickEventObject = m_lstObject[iPickIndex];
		m_pPickEventObject->m_iPickState = RBSPPICK_PICKED;
	}

}

void RBspScene::RBspLeafEx::UnPickLight()
{
	int i;
	m_pPickLight = NULL;
	for( i = 0; i < m_iLightNum; i++ )
	{
		m_lstLight[i].m_iPickState = RBSPPICK_READY;
	}
}
void RBspScene::RBspLeafEx::UnPickTrigger()
{
	int i;
	m_pPickTrigger = NULL;
	for( i = 0; i < m_iTriggerNum; i++ )
	{
		m_lstTrigger[i].m_iPickState = RBSPPICK_READY;
	}
}
void RBspScene::RBspLeafEx::UnPickEventEffect()
{
	int i;
	m_pPickEventEffect = NULL;
	for( i = 0; i < m_iEventNum[RBSPEVENT_EFFECT]; i++ )
	{
		m_lstEffect[i]->m_iPickState = RBSPPICK_READY;
	}
}
void RBspScene::RBspLeafEx::UnPickEventSound()
{
	int i;
	m_pPickEventObject = NULL;
	for( i = 0; i < m_iEventNum[RBSPEVENT_SOUND]; i++ )
	{
		m_lstSound[i]->m_iPickState = RBSPPICK_READY;
	}
}
void RBspScene::RBspLeafEx::UnPickEventObject()
{
	int i;
	m_pPickEventObject = NULL;
	for( i = 0; i < m_iEventNum[RBSPEVENT_OBJECTANI]; i++ )
	{
		m_lstObject[i]->m_iPickState = RBSPPICK_READY;
	}
}

void RBspScene::RBspLeafEx::UnPickR3SObject()
{
	int i;
	m_pPickR3SObject = NULL;


	for( i = 0; i < m_iR3SObjectNum; i++ )
	{
		m_lstR3SObject[i]->m_iPickState = RBSPPICK_READY;	
	}
}
void RBspScene::RBspLeafEx::MovePickLight(D3DXVECTOR3 vecNewPos)
{
	if(m_pPickLight != NULL)
		m_pPickLight->m_vecLight = vecNewPos;

}
void RBspScene::RBspLeafEx::MovePickTrigger(D3DXVECTOR3 vecNewPos)
{
	if(m_pPickTrigger != NULL)
		m_pPickTrigger->m_vecCenter = vecNewPos;

}
void RBspScene::RBspLeafEx::PickTriggerEdit(D3DXVECTOR3 vecCenter,int iTriggerValue,D3DXVECTOR3 vecTrigger[2])
{
	if(m_pPickTrigger != NULL)
	{
		m_pPickTrigger->m_vecCenter = vecCenter;
		m_pPickTrigger->m_iTriggerValue = iTriggerValue;
		m_pPickTrigger->m_vecTrigger[0] = vecTrigger[0];
		m_pPickTrigger->m_vecTrigger[1] = vecTrigger[1];
	}
}
void RBspScene::RBspLeafEx::MovePickEventEffect(D3DXVECTOR3 vecNewPos)
{
	if(m_pPickEventEffect != NULL)
		m_pPickEventEffect->m_vecCenter = vecNewPos;
	
}
void RBspScene::RBspLeafEx::MovePickEvnetSound(D3DXVECTOR3 vecNewPos)
{
	if(m_pPickEventSound != NULL)
		m_pPickEventSound->m_vecCenter = vecNewPos;

}
void RBspScene::RBspLeafEx::MovePickEvnetObject(D3DXVECTOR3 vecNewPos)
{
	if(m_pPickEventObject != NULL)
		m_pPickEventObject->m_vecCenter = vecNewPos;

}
// Trigger
void RBspScene::RBspLeafEx::CreateEmptyTrigger(D3DXVECTOR3 vecCenter,int iTriggerValue,D3DXVECTOR3 vecTrigger[2])
{
	RBspTrigger Node;
	
	Node.m_vecCenter = vecCenter;				// Trigger Center
	Node.m_iTriggerValue = iTriggerValue;		// Trigger 종류 ( 0 : Sphere, 1 : Box )
	Node.m_vecTrigger[0] = vecTrigger[0];		// Trigger 종류에 따른 경계값
	Node.m_vecTrigger[1] = vecTrigger[1];		// (Sphere : 0 -> Center, 1 -> Range )
												// (Box	: 0 -> Center, 1 -> Min, Max Value)

	m_lstTrigger.push_back(Node);
	m_iTriggerNum++;
		
	m_pPickTrigger = &(m_lstTrigger[m_iTriggerNum - 1]);	// 추가된후에 Pick Trigger로 세팅하여 다음작업 수행가능하게.

}
void RBspScene::RBspLeafEx::PickTriggerInputEffect(RBspEventEffect *pEffect)
{
	if(m_pPickTrigger == NULL || pEffect == NULL)
		return;
	
	m_pPickTrigger->InputEventEffect(pEffect);
	// Link 표시
	pEffect->m_bLinked = true;

}
void RBspScene::RBspLeafEx::PickTriggerInputObject(RBspEventObject *pObject)
{
	if(m_pPickTrigger == NULL || pObject == NULL)
		return;
	m_pPickTrigger->InputEventObject(pObject);
	// Link 표시
	pObject->m_bLinked = true;
}
void RBspScene::RBspLeafEx::PickTriggerInputSound(RBspEventSound *pSound)
{
	if(m_pPickTrigger == NULL || pSound == NULL)
		return;
	m_pPickTrigger->InputEventSound(pSound);
	// Link 표시
	pSound->m_bLinked = true;
}

//
/*
void RBspScene::RBspLight::InputShadowVolume(RShadowVolumeObject Object)
{
	if(!m_bShadowVolume)
		CreateShadowVolume();
	
	RShadowVolumeEdge *pEdges = new RShadowVolumeEdge;
	pEdges->CreateEdgeFromObject(D3DXVECTOR3(0.0f,1.0f,0.0f),&Object,2);
	m_pShadowVolume->InputVolume(pEdges);
}*/
void RBspScene::RBspLight::RenderShadowVolume()
{
	if(!m_bShadowVolume)
		return;
	if(m_pShadowVolume == NULL)
	{
		m_bShadowVolume = false;
		return;
	}
	// Render Stencil Buffer( 화면에 Masking 마지막에 한번 해준다)
//	m_pShadowVolume->RenderStencil();

}
void RBspScene::RBspLight::InputShadowMap()
{

}

int RBspScene::RBspTrigger::InputEventEffect(RBspEventEffect *pEffect) 
{
	m_iEventNum[RBSPEVENT_EFFECT]++;
	m_lstEffect.push_back(pEffect);
	return m_iEventNum[RBSPEVENT_EFFECT];

}

int RBspScene::RBspTrigger::InputEventSound(RBspEventSound *pSound) 
{
	m_iEventNum[RBSPEVENT_SOUND]++;
	m_lstSound.push_back(pSound);
	return m_iEventNum[RBSPEVENT_SOUND];

}

int RBspScene::RBspTrigger::InputEventObject(RBspEventObject *pObject) 
{
	m_iEventNum[ RBSPEVENT_OBJECTANI ]++;
	m_lstObject.push_back(pObject);
	return m_iEventNum[ RBSPEVENT_OBJECTANI];

}
// vecPos가 trigger 안에 있다면 true return
bool RBspScene::RBspTrigger::CheckInTrigger(D3DXVECTOR3 vecPos) 
{

	vecPos -= m_vecCenter;

	if(m_iTriggerValue == RBSPTRIGGER_SPHERE)			// Sphere
	{
		D3DXVECTOR3 vecTmp = vecPos - m_vecTrigger[0];	// m_vecTrigger[0] : Sphere Center Pos
		float fRange = D3DXVec3Length(&vecTmp);
		if(fRange <= m_vecTrigger[1].x) {					// m_vecTrigger[1].x == m_vecTrigger[1].y == m_vecTrigger[1].z == Sphere Radius
			m_bPlay = true;
			Play();
			return true;
		}
	}
	else if(m_iTriggerValue == RBSPTRIGGER_BOX)			// Box
	{													// m_vecTrigger[0] : Min Box, m_vecTrigger[1] : Max Box
		if((vecPos.x >= m_vecTrigger[0].x) && (vecPos.y >= m_vecTrigger[0].y) && (vecPos.z >= m_vecTrigger[0].z) &&
		   (vecPos.x <= m_vecTrigger[1].x) && (vecPos.y <= m_vecTrigger[1].y) && (vecPos.z <= m_vecTrigger[1].z))
		{
			m_bPlay = true;
			Play();
			return true;
		}
	}
	m_bPlay = false;
	return false;
}
// Trigger 동작시에 트리거에 연결된 Event들 동작 func
// Delay나 다른 세부 동작제어는 각기의 event class 내부에서 이루어진다.
void RBspScene::RBspTrigger::Play()	
{
	if(m_bPlay == true)
	{	
		int i;
		for( i = 0; i < m_iEventNum[RBSPEVENT_EFFECT]; i++) {	// Effect
			m_lstEffect[i]->Play();
		}
		for( i = 0; i < m_iEventNum[RBSPEVENT_SOUND]; i++) {	// Sound
			m_lstSound[i]->Play();
		}
		for( i = 0; i < m_iEventNum[RBSPEVENT_OBJECTANI]; i++) {// Object Ani
			m_lstObject[i]->Play();
		}
	}
}
void RBspScene::RBspEventEffect::Play()		// Effect Play Func
{
	if(m_bPlay == false) 
	{
		m_bPlay = true;	
		m_dwStartTime = timeGetTime();
	}
	else{	// start 된뒤
		m_dwCurrentTime = timeGetTime();
		if((m_dwCurrentTime - m_dwStartTime) / 30 > (DWORD)m_fDelay)
			Start();
		
	}
}
void RBspScene::RBspEventObject::Play()		// Ani Object Play Func
{
	if(m_bPlay == false)
	{
		m_bPlay = true;
		m_dwStartTime = timeGetTime();
	}
	else{
		m_dwCurrentTime = timeGetTime();
		if((m_dwCurrentTime - m_dwStartTime) / 30 > (DWORD)m_fDelay)
			Start();

	}
}
void RBspScene::RBspEventSound::Play()		// Sound Play Func
{
	if(m_bPlay == false)
	{
		m_bPlay = true;
		m_dwStartTime = timeGetTime();
	}
	else{
		m_dwCurrentTime = timeGetTime();
		if((m_dwCurrentTime - m_dwStartTime) / 30 > (DWORD)m_fDelay)
			Start();

	}
}

void RBspScene::RBspEventEffect::Start()	//실제적인 작동
{
	m_bStart = true;

}
void RBspScene::RBspEventObject::Start()	//실제적인 작동
{
	m_bStart = true;
}
void RBspScene::RBspEventSound::Start()	//실제적인 작동
{
	m_bStart = true;
}
void RBspScene::UpdateLeafBox(float x,float y,float z)
{
	int i;
	for( i = 0; i < m_iLeafsNum; i++)
	{
		m_pLeafExs[i].m_vecMinBox[0] += x;
		m_pLeafExs[i].m_vecMinBox[1] += y;
		m_pLeafExs[i].m_vecMinBox[2] += z;
		// Max		
		m_pLeafExs[i].m_vecMaxBox[0] += x;
		m_pLeafExs[i].m_vecMaxBox[1] += y;
		m_pLeafExs[i].m_vecMaxBox[2] += z;
	}

}
void RBspScene::SetZeroPosition(float x,float y,float z)
{
	int i;
	m_vecZeroPos = vector3(x,y,z);
	m_bVisible = true;
	for( i = 0; i < m_iLeafsNum; i++)
	{
		D3DXMATRIX box;
		D3DXMATRIX axis;
		// Min
		D3DXMatrixIdentity(&box);
		D3DXMatrixIdentity(&axis);

		axis._41 = m_vecZeroPos.x;
		axis._42 = m_vecZeroPos.y;
		axis._43 = m_vecZeroPos.z;

		box._41 = m_pLeafExs[i].m_vecMinBox[0];
		box._42 = m_pLeafExs[i].m_vecMinBox[1];
		box._43 = m_pLeafExs[i].m_vecMinBox[2];
		
		D3DXMatrixMultiply(&box,&box,&axis);
		
		m_pLeafExs[i].m_vecMinBox[0] = box._41;
		m_pLeafExs[i].m_vecMinBox[1] = box._42;
		m_pLeafExs[i].m_vecMinBox[2] = box._43;
		// Max
		D3DXMatrixIdentity(&box);
		
		box._41 = m_pLeafExs[i].m_vecMaxBox[0];
		box._42 = m_pLeafExs[i].m_vecMaxBox[1];
		box._43 = m_pLeafExs[i].m_vecMaxBox[2];
		
		D3DXMatrixMultiply(&box,&box,&axis);
		
		m_pLeafExs[i].m_vecMaxBox[0] = box._41;
		m_pLeafExs[i].m_vecMaxBox[1] = box._42;
		m_pLeafExs[i].m_vecMaxBox[2] = box._43;
	}

}
void RBspScene::RenderBox(vector3 vecAxis,vector3 vecScale,bool bTarget)
{

		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);
		identity._41 = vecAxis.x;
		identity._42 = vecAxis.y;
		identity._43 = vecAxis.z;

		identity._11 = vecScale.x;
		identity._22 = vecScale.y;
		identity._33 = vecScale.z;

		m_D3Device->SetRenderState(D3DRS_LIGHTING,FALSE);
		m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		if(bTarget == false)
			m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xff0000ff);
		else
			m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffff00);
		m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

		m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		m_D3Device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		m_D3Device->SetTransform(D3DTS_WORLD,&identity);
		m_D3Device->SetVertexShader(D3DFVF_XYZ);


		m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		m_D3Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
			0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
		m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
		m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
}
void RBspScene::RenderScene(bool bBspIn,bool bTarget)
{

	if(m_bVisible) 
	{
		if(bBspIn) 
		{
			m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

			D3DXMATRIX identity;
			D3DXMatrixIdentity(&identity);
			identity._41 = m_vecZeroPos.x;
			identity._42 = m_vecZeroPos.y;
			identity._43 = m_vecZeroPos.z;
			/*
			identity._11 = vecScale.x;
			identity._22 = vecScale.y;
			identity._33 = vecScale.z;
			*/
			m_D3Device->SetTransform(D3DTS_WORLD,&identity);

			m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			/*
			m_D3Device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			m_D3Device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

			m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
			m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
			*/

			RenderBspScene();
		}
		else			// 내부에 있는 bsp가 아니다
		{
			SetBox(D3DXVECTOR3(-50.0f,-50.0f,-50.0f),D3DXVECTOR3(50.0f,50.0f,50.0f));
			//상자표시
			RenderBox(m_vecZeroPos,vector3(10.0f,10.0f,10.0f),bTarget);
		}
	}
}
bool RBspScene::LoadBseScene(const char *strFilename,float fx,float fy,float fz)	// Read Bsp Extension File
{	
	int i,j;
	
	SetBspScale(fx,fy,fz);

	char strBuf[RBSPBUF_SIZE] = {0};
	FILE *fp = NULL;
	fp = fopen(strFilename,"rb");
	if(fp == NULL) 
	{
		MessageBox(NULL,"File Load Failed",strFilename,MB_OK);
		return false;
	}
	else
	{
		fread((char *)strBuf,sizeof(char) * RBSPBUF_SIZE,1,fp);
		if(strcmp(strBuf,"BseFile"))
		{
			fclose(fp);
			MessageBox(NULL,"Not Bse File",strBuf,MB_OK);
			return false;
			
		}
		else
			strcpy(m_strBspName,strFilename);
		// Data Num Read
		int arrNum[11];
		fread((int *)arrNum,sizeof(int),11,fp);
		
		m_iVertsNum = arrNum[0];
		m_iFacesNum = arrNum[1];
		m_iTextureDefsNum = arrNum[2];
		m_iLightMapsNum = arrNum[3];
		m_iNodesNum = arrNum[4];
		m_iLeafsNum = arrNum[5];
		m_iLeafFacesNum = arrNum[6];
		m_iPlanesNum = arrNum[7];
		m_iBrushesNum = arrNum[8];
		m_iBrusheSidesNum = arrNum[9];
		m_iLeafBrushesNum = arrNum[10];

		m_pVerts = new RBspDrawVertex[m_iVertsNum];
		m_pFaces = new RBspFace[m_iFacesNum];
		m_pTextureDefs = new RBspTextureDef[ m_iTextureDefsNum ];
		m_pLightMaps = new RBspLightMap[ m_iLightMapsNum ];
		m_pLightTextures = new CTexture[ m_iLightMapsNum ];
		m_pNodes = new RBspNode[ m_iNodesNum ];
		m_pLeafExs = new RBspLeafEx[ m_iLeafsNum ];

		m_pLeafFaces = new int[ m_iLeafFacesNum ];
		m_pPlanes = new RBspPlane[ m_iPlanesNum ];
		m_pTextures = new CTexture[ m_iTextureDefsNum ];
		m_pBrushes = new RBspBrushe[ m_iBrushesNum ];
		m_pBrusheSides = new RBspBrusheSide[ m_iBrusheSidesNum ];
		m_pLeafBrushes = new int[ m_iLeafBrushesNum ];

		// Base Texture Buffer Create
		m_pBaseVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];
		//m_pLightVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iTextureDefsNum];

		m_pBaseVertexNum = new int[m_iTextureDefsNum];
		m_pBaseVertexUseNum = new int[m_iTextureDefsNum];

		m_pBaseIndexBuffer = new LPDIRECT3DINDEXBUFFER8[m_iTextureDefsNum];
		m_pBaseIndexNum = new int[m_iTextureDefsNum];
		m_pBaseIndexUseNum = new int[m_iTextureDefsNum];
		
		m_pLightUnit = new LightUnitType[m_iTextureDefsNum];
		// Vis data Load
		fread((int *)&(m_VisData.m_iBytesPerCluster),sizeof(int),1,fp);
		fread((int *)&(m_VisData.m_iClustersNum),sizeof(int),1,fp);
		int iAllocSize = 0;
		fread((int *)&iAllocSize,sizeof(int),1,fp);
		m_VisData.m_pBitsets = new byte[iAllocSize];
		fread((byte *)m_VisData.m_pBitsets,sizeof(byte),iAllocSize,fp);
		// face Read
		fread((RBspFace *)m_pFaces,sizeof(RBspFace),m_iFacesNum,fp);
		// Texture Define Read
		fread((RBspTextureDef *)m_pTextureDefs,sizeof(RBspTextureDef),m_iTextureDefsNum,fp);
		// LightMap Read
		fread((RBspLightMap *)m_pLightMaps,sizeof(RBspLightMap),m_iLightMapsNum,fp);
////////////////////
		for(i = 0; i < m_iTextureDefsNum; i++ )
		{
			m_pBaseVertexBuffer[i] = NULL;
			//m_pLightVertexBuffer[i] = NULL;
			m_pBaseVertexNum[i] = 0;
			m_pBaseVertexUseNum[i] = 0;
			m_pBaseIndexBuffer[i] = NULL;
			m_pBaseIndexNum[i] = 0;
			m_pBaseIndexUseNum[i] = 0;

			char strTex[256] = {0};
			strcpy(strTex,m_pTextureDefs[i].m_strName);
			char *cptr = strrchr(strTex,'/');
			if(cptr != NULL)
			{
				cptr++;
				strcat(cptr,".dds");
				CTexture::SetPath(BSPTEXTUREPATH);
				m_pTextures[i].Load(cptr);
			}
			else
			{
			
			}
		}
		// LightMap Load
		
		for( i = 0; i < m_iLightMapsNum; i++ )
		{
						
			byte m_ucLight[65536];
			int icount = 0;
			int icount2 = 0;
			for(int j=0;j < 128;j++) {
				for(int k=0;k < 128;k++) {
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+2];		//R
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount+1];		//G
					m_ucLight[icount2++] = m_pLightMaps[i].m_ucLightMap[icount];		//B
					m_ucLight[icount2++] = 0;
					icount +=3;
				}
			}

			m_pLightTextures[i].CreateEmptyTexture(128,128,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED);
		
			//m_pLightTextures[i].FillTexture(m_pLightMaps[i].m_ucLightMap);
			m_pLightTextures[i].FillTexture(m_ucLight);
		}
////////////////////

		// Node
		fread((RBspNode *)m_pNodes,sizeof(RBspNode),m_iNodesNum,fp);
		// LeafExs
		for( i = 0; i < m_iLeafsNum; i++ )
		{
			m_pLeafExs[i].m_pPickEventEffect = NULL;
			m_pLeafExs[i].m_pPickEventObject = NULL;
			m_pLeafExs[i].m_pPickEventSound = NULL;
			m_pLeafExs[i].m_pPickLight = NULL;
			m_pLeafExs[i].m_pPickTrigger = NULL;
			memset(m_pLeafExs[i].m_iEventNum,0,sizeof(int) * 10);

			int iEventNum[10];
		
			fread((int *)iEventNum,sizeof(int),10,fp);
			int iArr[10];
			fread((int *)iArr,sizeof(int),10,fp);
			
			int iObjectAni = iEventNum[RBSPEVENT_OBJECTANI];
			int iEffect = iEventNum[RBSPEVENT_EFFECT];
			int iSound = iEventNum[RBSPEVENT_SOUND];
			int iLight = iArr[0];
			int iTrigger = iArr[1];
			int iAniObject = iArr[2];
			int iR3SObject = iArr[3];

			
			for(j = 0; j < iObjectAni; j++) 
			{
			
			}
			for(j = 0; j < iEffect; j++)
			{
				RBspEventEffect pNode;
				fread((RBspEventEffect *)&pNode,sizeof(RBspEventEffect),1,fp);
				m_pLeafExs[i].InputEventEffect(pNode.m_strName,pNode.m_vecCenter,pNode.m_fDelay);
			}
			for( j = 0; j < iSound; j++)
			{
				RBspEventSound pNode;
				fread((RBspEventSound *)&pNode,sizeof(RBspEventSound),1,fp);
				m_pLeafExs[i].InputEventSound(pNode.m_strName,pNode.m_vecCenter,pNode.m_fDelay);
			}
			for( j = 0; j < iLight; j++)
			{	
				RBspLight pNode;
				fread((RBspLight *)&pNode,sizeof(RBspLight),1,fp);
				
				m_pLeafExs[i].InputLight(pNode.m_iLightValue,pNode.m_vecLight,pNode.m_vecLightRange,(int)(pNode.m_fRed * 255.0f),(int)(pNode.m_fGreen * 255.0f),(int)(pNode.m_fBlue * 255.0f),false,false);


			}
			for( j = 0; j < iTrigger; j++ )
			{

			}
			for( j = 0; j < iAniObject; j++ )
			{

			}
			for( j = 0; j < iR3SObject; j++ )
			{
				RBspR3S pNode;
				fread((RBspR3S *)&pNode,sizeof(RBspR3S),1,fp);
				m_pLeafExs[i].InputR3SObject(pNode.m_vecPos.x,pNode.m_vecPos.y,pNode.m_vecPos.z,pNode.m_strName,pNode.m_matTm,pNode.m_bAlpha,pNode.m_bLight);

			}
			int iBspArr[12];
			fread((int *)iBspArr,sizeof(int),12,fp);

			m_pLeafExs[i].m_iAreaPotal = iBspArr[0];
			m_pLeafExs[i].m_iCluster = iBspArr[1]; 
			m_pLeafExs[i].m_iLeafBrush = iBspArr[2];
			m_pLeafExs[i].m_iLeafBrushNum = iBspArr[3];
			m_pLeafExs[i].m_iLeafFace = iBspArr[4];
			m_pLeafExs[i].m_iLeafFaceNum = iBspArr[5];
			m_pLeafExs[i].m_vecMaxBox[0] = iBspArr[6];
			m_pLeafExs[i].m_vecMaxBox[1] = iBspArr[7];
			m_pLeafExs[i].m_vecMaxBox[2] = iBspArr[8];
			
			m_pLeafExs[i].m_vecMinBox[0] = iBspArr[9];
			m_pLeafExs[i].m_vecMinBox[1] = iBspArr[10];
			m_pLeafExs[i].m_vecMinBox[2] = iBspArr[11];
					

		}

		// Leaf Face
		fread((int *)m_pLeafFaces,sizeof(int),m_iLeafFacesNum,fp);
		// Leaf Brushes
		fread((int *)m_pLeafBrushes,sizeof(int),m_iLeafBrushesNum,fp);
		// Plane
		fread((RBspPlane *)m_pPlanes,sizeof(RBspPlane),m_iPlanesNum,fp);
		// Brushe
		fread((RBspBrushe *)m_pBrushes,sizeof(RBspBrushe),m_iBrushesNum,fp);
		// BrusheSide
		fread((RBspBrusheSide *)m_pBrusheSides,sizeof(RBspBrusheSide),m_iBrusheSidesNum,fp);

		// Vertex Write
		fread((RBspDrawVertex *)m_pVerts,sizeof(RBspDrawVertex),m_iVertsNum,fp);
		//Scale
		fread((D3DXVECTOR3 *)&m_vecScale,sizeof(D3DXVECTOR3),1,fp);
		// ZeroPos
		fread((D3DXVECTOR3 *)&m_vecZeroPos,sizeof(D3DXVECTOR3),1,fp);
		
		fclose(fp);

		m_FaceRenderBit.ResizeBits(m_iFacesNum);		// Face Render Bit Resize
		// Memory pool Create

		m_D3Device =  CSceneManager::GetDevice();
	
		CreateBufferNum();
		CreateBuffer();
	
		m_pFrustum = new CViewFrustum;


		m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);

		m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		m_D3Device->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	}
	return true;

}
bool RBspScene::SaveBseFile(char *strFilename)
{
	int i,j;
	char strBuf[RBSPBUF_SIZE] = {0};

	FILE *fp = NULL;
	fp = fopen(strFilename,"wb");
	if(fp == NULL) {
		MessageBox(NULL,"File Create Failed",strFilename,MB_OK);
		return false;
	}
	else {
		memset(strBuf,0,sizeof(char) * RBSPBUF_SIZE);
		strcpy(strBuf,"BseFile");
		WritetoFile(fp,strBuf);

		// Data Num Write
		int arrNum[11];
		arrNum[0] = m_iVertsNum;		
		arrNum[1] = m_iFacesNum;		//
		arrNum[2] = m_iTextureDefsNum;	//
		arrNum[3] = m_iLightMapsNum;	//
		arrNum[4] = m_iNodesNum;		//
		arrNum[5] = m_iLeafsNum;		//
		arrNum[6] = m_iLeafFacesNum;	//
		arrNum[7] = m_iPlanesNum;		//
		arrNum[8] = m_iBrushesNum;		//
		arrNum[9] = m_iBrusheSidesNum;	//
		arrNum[10] = m_iLeafBrushesNum;	//
		fwrite((int *)arrNum,sizeof(int),11,fp);
		// VisData Write
		int iAllocSize = m_VisData.m_iBytesPerCluster * m_VisData.m_iClustersNum;
		fwrite((int *)&(m_VisData.m_iBytesPerCluster),sizeof(int),1,fp);
		fwrite((int *)&(m_VisData.m_iClustersNum),sizeof(int),1,fp);
		fwrite((int *)&iAllocSize,sizeof(int),1,fp);
		fwrite((byte *)(m_VisData.m_pBitsets),sizeof(byte),iAllocSize,fp);
		
		// face write
		fwrite((RBspFace *)m_pFaces,sizeof(RBspFace),m_iFacesNum,fp);
		// Texture Define write
		fwrite((RBspTextureDef *)m_pTextureDefs,sizeof(RBspTextureDef),m_iTextureDefsNum,fp);
		// Lightmap write
		fwrite((RBspLightMap *)m_pLightMaps,sizeof(RBspLightMap),m_iLightMapsNum,fp);
		// Node
		fwrite((RBspNode *)m_pNodes,sizeof(RBspNode),m_iNodesNum,fp);
		// LeafExs
		
		for( i = 0; i < m_iLeafsNum; i++ )
		{
			m_pLeafExs[i].m_pPickEventEffect = NULL;
			m_pLeafExs[i].m_pPickEventObject = NULL;
			m_pLeafExs[i].m_pPickEventSound = NULL;
			m_pLeafExs[i].m_pPickLight = NULL;
			m_pLeafExs[i].m_pPickTrigger = NULL;
			m_pLeafExs[i].m_pPickR3SObject = NULL;

			fwrite((int *)m_pLeafExs[i].m_iEventNum,sizeof(int),10,fp);
			int iArr[10];
			
			int iObjectAni = m_pLeafExs[i].m_iEventNum[RBSPEVENT_OBJECTANI];
			int iEffect = m_pLeafExs[i].m_iEventNum[RBSPEVENT_EFFECT];
			int iSound = m_pLeafExs[i].m_iEventNum[RBSPEVENT_SOUND];
			int iLight = m_pLeafExs[i].m_iLightNum;
			int iTrigger = m_pLeafExs[i].m_iTriggerNum;
			int iAniObject = m_pLeafExs[i].m_iAniObjectNum;
			int iR3SObjectNum = m_pLeafExs[i].m_iR3SObjectNum;


			iArr[0] = iLight;
			iArr[1] = iTrigger;
			iArr[2] = iAniObject;
			iArr[3] = iR3SObjectNum;

			fwrite((int *)iArr,sizeof(int),10,fp);

			for(j = 0; j < iObjectAni; j++) 
			{
			
			}
			for(j = 0; j < iEffect; j++)
			{
				fwrite((RBspEventEffect *)(m_pLeafExs[i].m_lstEffect[j]),sizeof(RBspEventEffect),1,fp);
			}
			for( j = 0; j < iSound; j++)
			{
				fwrite((RBspEventSound *)(m_pLeafExs[i].m_lstSound[j]),sizeof(RBspEventSound),1,fp);
			}
			for( j = 0; j < iLight; j++)
			{	
				m_pLeafExs[i].m_lstLight[j].m_pShadowVolume = NULL;
				fwrite((RBspLight *)&(m_pLeafExs[i].m_lstLight[j]),sizeof(RBspLight),1,fp);
			}
			for( j = 0; j < iTrigger; j++ )
			{

			}
			for( j= 0; j < iAniObject; j++)
			{

			}
			for( j = 0; j < iR3SObjectNum; j++ )
			{
				fwrite((RBspR3S *)(m_pLeafExs[i].m_lstR3SObject[j]),sizeof(RBspR3S),1,fp);

			}
			// Delete
			/*for( j = iObjectAni - 1; j >= 0; j-- )
			{
				delete m_pLeafExs[i].m_lstObject[j];
				m_pLeafExs[i].m_lstObject.erase(&(m_pLeafExs[i].m_lstObject[j]));
			}
			m_pLeafExs[i].m_lstObject.clear();
			for( j = iEffect - 1; j >= 0; j-- )
			{
				delete m_pLeafExs[i].m_lstEffect[j];
				m_pLeafExs[i].m_lstEffect.erase(&(m_pLeafExs[i].m_lstEffect[j]));
			}
			m_pLeafExs[i].m_lstEffect.clear();
			for( j= iSound - 1; j >= 0; j-- )
			{
				delete m_pLeafExs[i].m_lstSound[j];
				m_pLeafExs[i].m_lstSound.erase(&(m_pLeafExs[i].m_lstSound[j]));
			}
			m_pLeafExs[i].m_lstSound.clear();
			for( j = iLight -1 ; j >= 0; j-- )
			{	
				m_pLeafExs[i].m_lstLight.erase(&(m_pLeafExs[i].m_lstLight[j]));
			}
			m_pLeafExs[i].m_lstLight.clear();
			for( j = iTrigger -1 ; j >= 0; j--)
			{
				m_pLeafExs[i].m_lstTrigger.erase(&(m_pLeafExs[i].m_lstTrigger[j]));	
			}
			m_pLeafExs[i].m_lstTrigger.clear();

			for( j= iAniObject - 1; j >= 0; j--)
			{
				delete m_pLeafExs[i].m_lstAniObject[j];
				m_pLeafExs[i].m_lstAniObject.erase(&(m_pLeafExs[i].m_lstAniObject[j]));
			}
			m_pLeafExs[i].m_lstAniObject.clear();
		
			m_pLeafExs[i].m_iEventNum[RBSPEVENT_OBJECTANI] = 0;
			m_pLeafExs[i].m_iEventNum[RBSPEVENT_EFFECT] = 0;
			m_pLeafExs[i].m_iEventNum[RBSPEVENT_SOUND] = 0;
			m_pLeafExs[i].m_iLightNum = 0;
			m_pLeafExs[i].m_iTriggerNum = 0 ;
			m_pLeafExs[i].m_iAniObjectNum = 0;*/
			//////
			int iBspArr[12];
			iBspArr[0] = m_pLeafExs[i].m_iAreaPotal;
			iBspArr[1] = m_pLeafExs[i].m_iCluster;
			iBspArr[2] = m_pLeafExs[i].m_iLeafBrush;
			iBspArr[3] = m_pLeafExs[i].m_iLeafBrushNum;
			iBspArr[4] = m_pLeafExs[i].m_iLeafFace;
			iBspArr[5] = m_pLeafExs[i].m_iLeafFaceNum;
			iBspArr[6] = m_pLeafExs[i].m_vecMaxBox[0];
			iBspArr[7] = m_pLeafExs[i].m_vecMaxBox[1];
			iBspArr[8] = m_pLeafExs[i].m_vecMaxBox[2];
			
			iBspArr[9] = m_pLeafExs[i].m_vecMinBox[0];
			iBspArr[10] = m_pLeafExs[i].m_vecMinBox[1];
			iBspArr[11] = m_pLeafExs[i].m_vecMinBox[2];
			
			fwrite((int *)iBspArr,sizeof(int),12,fp);
		}

		
		// Leaf Face
		fwrite((int *)m_pLeafFaces,sizeof(int),m_iLeafFacesNum,fp);
		// Leaf Brushes
		fwrite((int *)m_pLeafBrushes,sizeof(int),m_iLeafBrushesNum,fp);
		// Plane
		fwrite((RBspPlane *)m_pPlanes,sizeof(RBspPlane),m_iPlanesNum,fp);
		// Brushe
		fwrite((RBspBrushe *)m_pBrushes,sizeof(RBspBrushe),m_iBrushesNum,fp);
		// BrusheSide
		fwrite((RBspBrusheSide *)m_pBrusheSides,sizeof(RBspBrusheSide),m_iBrusheSidesNum,fp);

		// Vertex Write
		fwrite((RBspDrawVertex *)m_pVerts,sizeof(RBspDrawVertex),m_iVertsNum,fp);
		//Scale
		fwrite((D3DXVECTOR3 *)&m_vecScale,sizeof(D3DXVECTOR3),1,fp);
		// ZeroPos
		fwrite((D3DXVECTOR3 *)&m_vecZeroPos,sizeof(D3DXVECTOR3),1,fp);

			
		fclose(fp);

		
	}
	return true;
}
D3DXVECTOR3 RBspScene::WorldToLocalPos(D3DXVECTOR3 vecWorld)	// World Position 을 Local position 으로..
{

	D3DXVECTOR3 vecPos = vecWorld;
	D3DXMATRIX matTmp;
	D3DXMATRIX matInv;

	D3DXMatrixIdentity(&matTmp);

	matTmp._41 = m_vecZeroPos.x;
	matTmp._42 = m_vecZeroPos.y;
	matTmp._43 = m_vecZeroPos.z;

	D3DXMatrixInverse(&matInv,NULL,&matTmp);
	D3DXMatrixIdentity(&matTmp);
	
	matTmp._41 = vecPos.x;
	matTmp._42 = vecPos.y;
	matTmp._43 = vecPos.z;
	
	D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
	vecPos.x = matTmp._41;
	vecPos.y = matTmp._42;
	vecPos.z = matTmp._43;						
	
	return vecPos;


}
D3DXVECTOR3 RBspScene::LocalToWorldPos(D3DXVECTOR3 vecLocal)	// Local Position 을 World Position 으로..
{
	D3DXVECTOR3 vecPos = vecLocal;
	D3DXMATRIX matTmp;
	D3DXMATRIX matInv;

	D3DXMatrixIdentity(&matTmp);
	matTmp._41 = m_vecZeroPos.x;
	matTmp._42 = m_vecZeroPos.y;
	matTmp._43 = m_vecZeroPos.z;
	matInv = matTmp;
	D3DXMatrixIdentity(&matTmp);
	matTmp._41 = vecPos.x;
	matTmp._42 = vecPos.y;
	matTmp._43 = vecPos.z;
	D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
	vecPos.x = matTmp._41;
	vecPos.y = matTmp._42;
	vecPos.z = matTmp._43;						
	
	return vecPos;

}
void RBspScene::RenderR3SObject()				// R3S Object Render
{

	D3DMATERIAL8 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.6f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.6f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.6f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.6f;
	m_D3Device->SetMaterial( &mtrl );

	m_D3Device->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);	
	m_D3Device->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	m_D3Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_D3Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    m_D3Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	m_D3Device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	if(BaseGraphicsLayer::m_VoodooOption)
		m_D3Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	else
		m_D3Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);			
	
	m_D3Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );	
	m_D3Device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_D3Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );	

	m_D3Device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_D3Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	m_D3Device->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
    m_D3Device->SetRenderState( D3DRS_ALPHAREF,        0xee );
    m_D3Device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	m_D3Device->SetTexture(1,NULL);


//	m_D3Device->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
	m_D3Device->SetRenderState(D3DRS_AMBIENT,CSceneManager::m_WeatherManager.m_InterAmbientColor.c);
	
	m_D3Device->SetRenderState( D3DRS_LIGHTING,TRUE);
	
	D3DXMATRIX matBackup;

	m_D3Device->GetTransform(D3DTS_WORLD,&matBackup);

	for(int cObject = 0; cObject < (int)m_ObjectSceneNode.size(); cObject++)
	{
		//////////////////// Light
	
		for(int cLight=0;cLight<8;cLight++)
		{
			m_D3Device->LightEnable(cLight,FALSE);
		}


		D3DLIGHT8 pCurrentLight[4];
		// 최대 4개 까지

		D3DXVECTOR3 vecPos = D3DXVECTOR3(m_ObjectSceneNode[cObject]->m_TM._41,m_ObjectSceneNode[cObject]->m_TM._42,m_ObjectSceneNode[cObject]->m_TM._43);
		int iLightNum = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->CheckChrLight(vecPos,pCurrentLight,4);

		if(iLightNum > 0)
		{
			
			for(int cLNum = 1; cLNum < iLightNum + 1; cLNum++) {
				m_D3Device->SetLight(cLNum,&(pCurrentLight[cLNum - 1]));
				m_D3Device->LightEnable(cLNum,TRUE);
			}
		}
		else 
		{
			m_D3Device->LightEnable(0,TRUE);
			D3DLIGHT8 light;
			light.Type        = D3DLIGHT_POINT;
			light.Attenuation0=0.5f;					
			light.Attenuation1=0.0f;	

			light.Diffuse.r   = CSceneManager::m_WeatherManager.m_InterCharacterLight.r/255.0f;
			light.Diffuse.g   = CSceneManager::m_WeatherManager.m_InterCharacterLight.g/255.0f;
			light.Diffuse.b   = CSceneManager::m_WeatherManager.m_InterCharacterLight.b/255.0f;		
			vector3 vecLight(-1.0f,-1.0f,-1.0f);
			vecLight.Normalize();
			light.Position.x   = light.Direction.x =vecLight.x;
			light.Position.y   = light.Direction.y =vecLight.y + 10000.0f;
			light.Position.z   = light.Direction.z =vecLight.z;
			light.Range        = 0.0f;
		
			m_D3Device->SetLight(0,&light);

		}

		///////////////////

		D3DXVECTOR3 vecMin = D3DXVECTOR3(m_ObjectSceneNode[cObject]->m_vecMinBox.x,
										m_ObjectSceneNode[cObject]->m_vecMinBox.y,
										m_ObjectSceneNode[cObject]->m_vecMinBox.z);
		D3DXVECTOR3 vecMax = D3DXVECTOR3(m_ObjectSceneNode[cObject]->m_vecMaxBox.x,
										m_ObjectSceneNode[cObject]->m_vecMaxBox.y,
										m_ObjectSceneNode[cObject]->m_vecMaxBox.z);

		// vecMin
		matrix matTm = m_ObjectSceneNode[cObject]->m_TM;
		matrix matPos;
		matPos.MakeIdent();
		matPos._41 = vecMin.x;
		matPos._42 = vecMin.y;
		matPos._43 = vecMin.z;
		
		matPos = matPos * matTm;
		
		vecMin.x = matPos._41;
		vecMin.y = matPos._42;
		vecMin.z = matPos._43;
		// vecMax
		matTm = m_ObjectSceneNode[cObject]->m_TM;
		matPos.MakeIdent();

		matPos._41 = vecMax.x;
		matPos._42 = vecMax.y;
		matPos._43 = vecMax.z;
		
		matPos = matPos * matTm;
		
		vecMax.x = matPos._41;
		vecMax.y = matPos._42;
		vecMax.z = matPos._43;

		if(!m_pFrustum->BoxInFrustum(vecMin,vecMax))
			continue;
		
		m_D3Device->SetTransform(D3DTS_WORLD,m_ObjectSceneNode[cObject]->m_TM);

		if(CSceneManager::m_PickObjectScene==m_ObjectSceneNode[cObject])
		{
			if(CSceneManager::m_bIsPickObjectMoving==MOVE_OBJECT )
				m_ObjectSceneNode[cObject]->m_pObject->RenderBox(m_D3Device);
			else
			{
				m_ObjectSceneNode[cObject]->m_pObject->Render(m_D3Device);
				m_ObjectSceneNode[cObject]->m_pObject->RenderBox(m_D3Device);
			}
		}		
		else
		{
			m_ObjectSceneNode[cObject]->m_pObject->Render(m_D3Device);
		}

	}
	m_D3Device->SetTransform(D3DTS_WORLD,&matBackup);
	m_D3Device->SetRenderState( D3DRS_LIGHTING,FALSE);
	
}
void RBspScene::DynamicR3SLoad()
{/*
	CSceneManager::m_ObjectContainer.DeleteAllObject();

	for(int cObject=0;cObject<m_ObjectSceneNode.num;cObject++)
	{
		if(m_ObjectSceneNode[cObject]->m_pObject==NULL)
		{					
			m_ObjectSceneNode[cObject]->m_pObject=
				CSceneManager::m_ObjectContainer.GetObject(
				m_ObjectSceneNode[cObject]->m_strObjectName,
				m_ObjectSceneNode[cObject]->m_isAlpha,
				m_ObjectSceneNode[cObject]->m_isLight);

			m_ObjectSceneNode[cObject]->m_vecMaxBox=m_ObjectSceneNode[cObject]->m_pObject->m_MaxBox;
			m_ObjectSceneNode[cObject]->m_vecMinBox=m_ObjectSceneNode[cObject]->m_pObject->m_MinBox;			
			m_ObjectSceneNode[cObject]->Init();

		}				
	}*/
}
void RBspScene::GenerateR3SObject()
{

	int i;
	int j;

	int iTotalAdd = 0;

	for(int cObject=0;cObject<(int)m_ObjectSceneNode.size();cObject++)
	{
		delete m_ObjectSceneNode[cObject];		
	}
	
	m_ObjectSceneNode.clear();

	CSceneManager::m_PickObjectScene = NULL;
	if(m_iVisLeafs)
	{
		for( i = 0; i < m_iVisLeafs; i++ )
		{
			int iLeafIndex = m_pVisLeafs[i];
			RBspLeafEx *pLeaf = &(m_pLeafExs[iLeafIndex]);
			for( j= 0; j < pLeaf->m_iR3SObjectNum; j++ )
			{
				RBspR3S *pR3SInfo = pLeaf->m_lstR3SObject[j];
				CObjectScene *AddObjectScene = new CObjectScene();
				AddObjectScene->m_TM = pR3SInfo->m_matTm;
				AddObjectScene->m_AccumulateTM = pR3SInfo->m_matTm;
				AddObjectScene->m_pObject = NULL;

				strcpy(AddObjectScene->m_strObjectName,pR3SInfo->m_strName);
				AddObjectScene->m_isAlpha = pR3SInfo->m_bAlpha;
				AddObjectScene->m_isLight = pR3SInfo->m_bLight;
				AddObjectScene->m_isShadowed=false;
				
				AddObjectScene->m_ObjectID = iTotalAdd++;
				//AddObjectScene->Init();
				m_ObjectSceneNode.push_back(AddObjectScene);
				// picking 처리
				if(pLeaf->m_lstR3SObject[j]->m_iPickState == RBSPPICK_PICKED)
					CSceneManager::m_PickObjectScene = AddObjectScene;
				
			}
		}

	}
	DynamicR3SLoad();
}