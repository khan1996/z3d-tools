// EffectObjectAnimation.cpp: implementation of the CEffectObjectAnimation class.
//
//////////////////////////////////////////////////////////////////////
#include "SceneManager.h"
#include "EffectObjectAnimation.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectObjectAnimation::CEffectObjectAnimation()
{

}

CEffectObjectAnimation::~CEffectObjectAnimation()
{
	int i;
	for(i = 0; i < (int)m_pNodeMeshObjectList.size(); ++i)
	{
		delete m_pNodeMeshObjectList[i];
	}
	m_pNodeMeshObjectList.clear();

	for(i = 0; i < (int)m_AnimationPackList.size(); ++i)
	{
		delete m_AnimationPackList[i];
	}
	m_AnimationPackList.clear();
	

}

void CEffectObjectAnimation::Load(char *strFilename)
{

	//z3d::MatrixToD3DMATRIX
	/*
	FILE *fp;
	int nObject=0;
	fp=fopen(strFilename,"wb");
	DWORD dwRenderingType=0;
	fwrite(&dwRenderingType,sizeof(DWORD),1,fp);
	nObject=Index;
	
	fwrite(&HaveMat,sizeof(HaveMat),1,fp);
	for(int cMtrl=0;cMtrl<HaveMat;cMtrl++)
	{
		fwrite((void*)(MatName[cMtrl]),sizeof(char)*256,1,fp);
	}
	for(int cObject=0;cObject<nObject;cObject++)
	{
		fwrite((void*)(&cSaveVertex[cObject]),sizeof(int),1,fp);		
		fwrite((void*)(&cSaveFace[cObject]),sizeof(int),1,fp);
		fwrite((void*)(&cSaveKeyPosition[cObject]),sizeof(int),1,fp);
		fwrite((void*)(SaveVertex[cObject]),sizeof(Vertex)*cSaveVertex[cObject],1,fp);
		fwrite((void*)(SaveFace[cObject]),sizeof(WORD)*cSaveFace[cObject]*3,1,fp);		
		fwrite((void*)(SaveKeyPosition[cObject]),sizeof(KeyPosition)*cSaveKeyPosition[cObject],1,fp);
	}
	fclose(fp);
	*/
	///*
	DWORD dwRenderingType=0;
	CFileLoad FileLoader;
	FileLoader.Load(strFilename);
	FileLoader.GetData(&dwRenderingType,sizeof(DWORD));

	CMeshObject *pAddMesh=new CMeshObject();
	int nMat;
	char strTextureName[256];
	FileLoader.GetData(&nMat,sizeof(int));
	for(int i=0;i<nMat;i++)
	{
		FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);
		pAddMesh->m_pMat.push_back(new CTexture());
		strcpy(pAddMesh->m_pMat[i]->m_strName,strTextureName);
	}	
	CTexture::SetPath(OBJECTTEXTUREPATH);
	pAddMesh->TextureLoad();
	int cVertex=0,cIndices=0,nKeyPos,nKeyRot;
	LPDIRECT3DVERTEXBUFFER8 pAllocVB;
	LPDIRECT3DINDEXBUFFER8 pAllocIB;

	MultiVertex *pVertexData;
	WORD *pIndicesData;

	FileLoader.GetData(&cVertex,sizeof(int));	
	FileLoader.GetData(&cIndices,sizeof(int));
	FileLoader.GetData(&nKeyPos,sizeof(int));
	FileLoader.GetData(&nKeyRot,sizeof(int));
	CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
									 MultiFVF,D3DPOOL_MANAGED,&pAllocVB);	
	CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),D3DUSAGE_WRITEONLY,
									D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);	
	pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);	
	pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
	FileLoader.GetData(pVertexData,sizeof(MultiVertex)*cVertex);
	FileLoader.GetData(pIndicesData,sizeof(WORD)*cIndices*3);

	pAllocVB->Unlock();
	pAllocIB->Unlock();

	pAddMesh->m_pVertexBuffer.push_back(pAllocVB);
	pAddMesh->m_pIndicesBuffer.push_back(pAllocIB);		

	pAddMesh->m_nVertex.push_back(cVertex);
	pAddMesh->m_nIndices.push_back(cIndices);
	pAddMesh->m_pMatRef.push_back(0);

	AnimationPackage *AniPack=new AnimationPackage;
	KeyPosition KeyPos;
	for(int cKeyPos=0;cKeyPos<nKeyPos;cKeyPos++)
	{
		FileLoader.GetData(&KeyPos,sizeof(KeyPosition));
		AniPack->m_KeyPosList.push_back(KeyPos);
	}	
	KeyRotation KeyRot;
	for(int cKeyRot=0;cKeyRot<nKeyRot;cKeyRot++)
	{
		FileLoader.GetData(&KeyRot,sizeof(KeyRotation));		
		AniPack->m_KeyRotList.push_back(KeyRot);
	}
	/*
	quaternion qtAccumlate,qtTemp;
	qtAccumlate.x=AniPack->m_KeyRotList[0].x;
	qtAccumlate.y=AniPack->m_KeyRotList[0].y;
	qtAccumlate.z=AniPack->m_KeyRotList[0].z;
	qtAccumlate.w=AniPack->m_KeyRotList[0].w;
	for(cKeyRot=1;cKeyRot<nKeyRot;cKeyRot++)
	{
		//qtAccumlate*=AniPack->m_KeyRotList[cKeyRot];
		qtTemp.x=AniPack->m_KeyRotList[cKeyRot].x;
		qtTemp.y=AniPack->m_KeyRotList[cKeyRot].y;
		qtTemp.z=AniPack->m_KeyRotList[cKeyRot].z;
		qtTemp.w=AniPack->m_KeyRotList[cKeyRot].w;
		qtAccumlate*=qtTemp;
		//qtAccumlate=qtTemp*qtAccumlate;
		AniPack->m_KeyRotList[cKeyRot].x=qtAccumlate.x;
		AniPack->m_KeyRotList[cKeyRot].y=qtAccumlate.y;
		AniPack->m_KeyRotList[cKeyRot].z=qtAccumlate.z;
		AniPack->m_KeyRotList[cKeyRot].w=qtAccumlate.w;
	}
	for(cKeyRot=0;cKeyRot<nKeyRot;cKeyRot++)
		AniPack->m_KeyRotList[cKeyRot].w=-AniPack->m_KeyRotList[cKeyRot].w;
		*/

	m_AnimationPackList.push_back(AniPack);
	pAddMesh->m_dwShader=MultiFVF;
	m_pNodeMeshObjectList.push_back(pAddMesh);

	/*
	char strTextureName[256];
	for(int i=0;i<nMat;i++)
	{
		FileLoader.GetData(strTextureName,sizeof(char)*MAX_NAMEBUFFER);
		if(strcmp(strTextureName,"")==0)
			continue;
		m_pMat.Add(new CTexture());
		strcpy(m_pMat[i]->m_strName,strTextureName);
	}
	char strObjectName[256];
	int MatRef,cVertex,cIndices;	
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	for(i=0;i<nObject;i++)
	{
		FileLoader.GetData(strObjectName,sizeof(char)*256);
		FileLoader.GetData(&MatRef,sizeof(long));
		FileLoader.GetData(&cVertex,sizeof(long));
		FileLoader.GetData(&cIndices,sizeof(int));		

		LPDIRECT3DVERTEXBUFFER8 pAllocVB;
		LPDIRECT3DINDEXBUFFER8 pAllocIB;

		CSceneManager::GetDevice()->CreateVertexBuffer(cVertex*sizeof(MultiVertex),D3DUSAGE_WRITEONLY,
										 MultiFVF,D3DPOOL_MANAGED,&pAllocVB);
		CSceneManager::GetDevice()->CreateIndexBuffer(cIndices*3*sizeof(WORD),D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,D3DPOOL_MANAGED,&pAllocIB);

		pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
		pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);

		FileLoader.GetData(pVertexData,sizeof(MultiVertex)*cVertex);
		FileLoader.GetData(pIndicesData,sizeof(WORD)*cIndices*3);

		pAllocVB->Unlock();
		pAllocIB->Unlock();

		m_pVertexBuffer.Add(pAllocVB);
		m_pIndicesBuffer.Add(pAllocIB);		

		m_nVertex.Add(cVertex);
		m_nIndices.Add(cIndices);
		m_pMatRef.Add(MatRef);
	}
	*/
}

void CEffectObjectAnimation::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	static int KeyFrame=0;
	KeyFrame+=30;
	if(KeyFrame>16000)
		KeyFrame=0;
	int Interval=m_AnimationPackList[0]->m_KeyPosList[1].keytime-m_AnimationPackList[0]->m_KeyPosList[0].keytime;	
	float fInterTime=(float)KeyFrame/(float)Interval;

	vector3 vecInterPos;
	vecInterPos.x=m_AnimationPackList[0]->m_KeyPosList[1].x-m_AnimationPackList[0]->m_KeyPosList[0].x;
	vecInterPos.y=m_AnimationPackList[0]->m_KeyPosList[1].y-m_AnimationPackList[0]->m_KeyPosList[0].y;
	vecInterPos.z=m_AnimationPackList[0]->m_KeyPosList[1].z-m_AnimationPackList[0]->m_KeyPosList[0].z;		
	//vecInterPos/=(float)Interval;
	vector3 vecAniPos;
	vecAniPos.x=m_AnimationPackList[0]->m_KeyPosList[0].x+vecInterPos.x*fInterTime;
	vecAniPos.y=m_AnimationPackList[0]->m_KeyPosList[0].y+vecInterPos.y*fInterTime;
	vecAniPos.z=m_AnimationPackList[0]->m_KeyPosList[0].z+vecInterPos.z*fInterTime;

	quaternion qtFirst,qtEnd,qtNow;
	qtFirst.x=m_AnimationPackList[0]->m_KeyRotList[0].x;
	qtFirst.y=m_AnimationPackList[0]->m_KeyRotList[0].y;
	qtFirst.z=m_AnimationPackList[0]->m_KeyRotList[0].z;
	qtFirst.w=m_AnimationPackList[0]->m_KeyRotList[0].w;
	qtEnd.x=m_AnimationPackList[0]->m_KeyRotList[1].x;
	qtEnd.y=m_AnimationPackList[0]->m_KeyRotList[1].y;
	qtEnd.z=m_AnimationPackList[0]->m_KeyRotList[1].z;
	qtEnd.w=m_AnimationPackList[0]->m_KeyRotList[1].w;
	z3d::QuaternionSlerp(qtNow,qtFirst,qtEnd,fInterTime);	
	//D3DMath_MatrixFromQuaternion(rotmat,x,y,z,w);

	D3DXQUATERNION qNow,qFirst,qEnd;
	qFirst.x=qtFirst.x;
	qFirst.y=qtFirst.y;
	qFirst.z=qtFirst.z;
	qFirst.w=qtFirst.w;

	qEnd.x=qtEnd.x;
	qEnd.y=qtEnd.y;
	qEnd.z=qtEnd.z;
	qEnd.w=qtEnd.w;
	D3DXQuaternionSlerp(&qNow,&qFirst,&qEnd,fInterTime);
	
	qtNow.x=qNow.x;
	qtNow.y=qNow.y;
	qtNow.z=qNow.z;
	qtNow.w=qNow.w;
	
	
	matrix matRot;
	//z3d::MatrixRotationQuaternion(matRot,qtNow);
	D3DXMatrixRotationQuaternion(matRot,&qNow);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	

	matrix matInit;
	//matInit.MakeIdent();
	matInit.Translation(vecAniPos);
	matInit=matRot*matInit;
	pd3dDevice->SetTransform(D3DTS_WORLD,matInit);
	//pd3dDevice->SetTransform(D3DTS_WORLD,matRot);
	for(int cObject=0;cObject<(int)m_pNodeMeshObjectList.size();cObject++)
	{
		m_pNodeMeshObjectList[cObject]->Render(pd3dDevice);
	}
}
