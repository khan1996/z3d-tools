// ViewCamera.cpp: implementation of the CViewCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "ViewCamera.h"
#include "Vertex.h"
#include <d3dx8.h>
#include "GMMemory.h"

#define KeyPressed( key )    HIBYTE( GetAsyncKeyState( key ) )	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewCamera::CViewCamera()
{
	m_CameraMode=0;
	m_matView.MakeIdent();
	m_matPosition.MakeIdent();	
	m_fRotx=m_fRoty=m_fRotz=0.0f;
	m_cFade=-1;
	m_dwTickTime=-1;
}

CViewCamera::~CViewCamera()
{

}

void CViewCamera::BuildFrustum(float fAspect, float fFov,float fNearPlane,float fFarPlane)
{	
	//fFarPlane=1000.0f;
	m_fAspect=fAspect;
	m_fFov=fFov;
	m_fFarPlane=fFarPlane;
	m_fNearPlane=100.0f;

	vector3 vecTopLeft,vecTopRight,vecBottomLeft,vecBottomRight;	
	//float fFrustumHeight=fFarPlane*tanf(fFov*0.5f);
	//float fFrustumWidth=fFrustumHeight*fAspect;

	float fFrustumWidth=fFarPlane*tanf(fFov*0.5f);
	float fFrustumHeight=fFrustumWidth*fAspect;

	//fFrustumHeight*=0.8f;
	//fFrustumWidth*=0.8f;

	vecTopLeft.y=fFrustumHeight;
	vecTopLeft.x=-fFrustumWidth;
	vecTopLeft.z=fFarPlane;

	vecTopRight.y=fFrustumHeight;
	vecTopRight.x=fFrustumWidth;
	vecTopRight.z=fFarPlane;

	vecBottomLeft.y=-fFrustumHeight;
	vecBottomLeft.x=-fFrustumWidth;
	vecBottomLeft.z=fFarPlane;

	vecBottomRight.y=-fFrustumHeight;
	vecBottomRight.x=fFrustumWidth;
	vecBottomRight.z=fFarPlane;

	//m_vecFrustumTop[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumTop[0]=vector3(0.0f,0.0f,0.0f);
	m_vecFrustumTop[1]=vecTopRight;
	m_vecFrustumTop[2]=vecTopLeft;

	//m_vecFrustumBottom[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumBottom[0]=vector3(0.0f,0.0f,0.0f);
	m_vecFrustumBottom[1]=vecBottomLeft;
	m_vecFrustumBottom[2]=vecBottomRight;

	//m_vecFrustumLeft[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumLeft[0]=vector3(0.0f,0.0f,0.0f);
	m_vecFrustumLeft[1]=vecTopLeft;
	m_vecFrustumLeft[2]=vecBottomLeft;	

	//m_vecFrustumRight[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumRight[0]=vector3(0.0f,0.0f,0.0f);
	m_vecFrustumRight[1]=vecBottomRight;
	m_vecFrustumRight[2]=vecTopRight;

	m_vecFrustumNear[0]=vector3(0.0f,0.0f,0.0f)+vector3(0.0f,0.0f,fNearPlane);
	m_vecFrustumNear[1]=vector3(0.0f,100.0f,0.0f)+vector3(0.0f,0.0f,fNearPlane);
	m_vecFrustumNear[2]=vector3(100.0f,0.0f,0.0f)+vector3(0.0f,0.0f,fNearPlane);

}

void CViewCamera::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	/*
	if(m_cFade==-1)
		return;
	m_pVertex[0].v.x=0.0f;m_pVertex[1].v.x=0.0f;
	m_pVertex[2].v.x=800.0f;m_pVertex[3].v.x=800.0f;
	m_pVertex[1].v.y=0.0f;m_pVertex[3].v.y=0.0f;
	m_pVertex[0].v.y=600.0f;m_pVertex[2].v.y=600.0f;		
		
	m_pVertex[0].tu=0.0f;m_pVertex[1].tu=0.0f;m_pVertex[3].tu=1.0f;m_pVertex[2].tu=1.0f;
	m_pVertex[1].tv=0.0f;m_pVertex[3].tv=0.0f;m_pVertex[0].tv=1.0f;m_pVertex[2].tv=1.0f;		
	
	color NowDiffuseColor;
	if(m_FadeInitValue+(m_AdderFadeValue)*m_cFade>255)
	{
		m_cFade=255;
		NowDiffuseColor.c=0xffffffff;
	}
	else if(m_FadeInitValue+(m_AdderFadeValue)*m_cFade<0)
	{
		m_cFade=255;
		NowDiffuseColor.c=0x0;
	}
	else
	{
		NowDiffuseColor.a=m_FadeInitValue+(m_AdderFadeValue)*m_cFade;
		NowDiffuseColor.r=m_FadeInitValue+(m_AdderFadeValue)*m_cFade;
		NowDiffuseColor.g=m_FadeInitValue+(m_AdderFadeValue)*m_cFade;
		NowDiffuseColor.b=m_FadeInitValue+(m_AdderFadeValue)*m_cFade;
	}

	for(int i=0;i<4;i++)
	{
		m_pVertex[i].w=0.1f;m_pVertex[i].v.z=0.1f;
		m_pVertex[i].Specular.c=0x0;
		m_pVertex[i].Diffuse.c=NowDiffuseColor.c;
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);			
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);			
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_pVertex,sizeof(TLVertex));	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	if(m_cFade++>=255)
	{
		m_cFade=-1;
	}
	*/

	/*
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	matrix mat;
	mat.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&mat);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	pd3dDevice->SetVertexShader(LVERTEXFVF);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		

	//Top Render data
	LVertex FrustumVertex[3];
	FrustumVertex[0].v=m_vecFrustumTop[0];
	FrustumVertex[1].v=m_vecFrustumTop[1];
	FrustumVertex[2].v=m_vecFrustumTop[2];

	FrustumVertex[0].spec.c=0x0;
	FrustumVertex[0].diff.c=0xff0000ff;
	FrustumVertex[1].spec.c=0x0;
	FrustumVertex[1].diff.c=0xff0000ff;
	FrustumVertex[2].spec.c=0x0;
	FrustumVertex[2].diff.c=0xff0000ff;	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,1,FrustumVertex,sizeof(LVertex));

	//Top Render data	
	FrustumVertex[0].v=m_vecFrustumBottom[0];
	FrustumVertex[1].v=m_vecFrustumBottom[1];
	FrustumVertex[2].v=m_vecFrustumBottom[2];

	FrustumVertex[0].spec.c=0x0;
	FrustumVertex[0].diff.c=0xffff0000;
	FrustumVertex[1].spec.c=0x0;
	FrustumVertex[1].diff.c=0xffff0000;
	FrustumVertex[2].spec.c=0x0;
	FrustumVertex[2].diff.c=0xffff0000;	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,1,FrustumVertex,sizeof(LVertex));

	//
	FrustumVertex[0].v=m_vecFrustumLeft[0];
	FrustumVertex[1].v=m_vecFrustumLeft[1];
	FrustumVertex[2].v=m_vecFrustumLeft[2];

	FrustumVertex[0].spec.c=0x0;
	FrustumVertex[0].diff.c=0xff000000;
	FrustumVertex[1].spec.c=0x0;
	FrustumVertex[1].diff.c=0xff000000;
	FrustumVertex[2].spec.c=0x0;
	FrustumVertex[2].diff.c=0xff000000;	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,1,FrustumVertex,sizeof(LVertex));

	//
	FrustumVertex[0].v=m_vecFrustumRight[0];
	FrustumVertex[1].v=m_vecFrustumRight[1];
	FrustumVertex[2].v=m_vecFrustumRight[2];

	FrustumVertex[0].spec.c=0x0;
	FrustumVertex[0].diff.c=0xff00ff00;
	FrustumVertex[1].spec.c=0x0;
	FrustumVertex[1].diff.c=0xff00ff00;
	FrustumVertex[2].spec.c=0x0;
	FrustumVertex[2].diff.c=0xff00ff00;	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,1,FrustumVertex,sizeof(LVertex));

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		
	//*/
}

void CViewCamera::MoveFrustum()
{
	float fFrustumHeight=m_fFarPlane*tanf(m_fFov*0.5f);
	float fFrustumWidth=fFrustumHeight*(1.0f/m_fAspect);

	vector3 vecTopLeft,vecTopRight,vecBottomLeft,vecBottomRight,vecCenterAxisX,vecCenterAxisY,vecCenter;

	matrix matHeight,matWidth;

	matHeight.Translation(vector3(fFrustumWidth,fFrustumHeight,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecTopRight=matHeight.GetLoc();

	matHeight.Translation(vector3(-fFrustumWidth,fFrustumHeight,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecTopLeft=matHeight.GetLoc();

	matHeight.Translation(vector3(fFrustumWidth,-fFrustumHeight,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecBottomRight=matHeight.GetLoc();

	matHeight.Translation(vector3(-fFrustumWidth,-fFrustumHeight,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecBottomLeft=matHeight.GetLoc();


	matHeight.Translation(vector3(fFrustumWidth,0.0f,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecCenterAxisX=matHeight.GetLoc();

	matHeight.Translation(vector3(0.0f,fFrustumHeight,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecCenterAxisY=matHeight.GetLoc();

	matHeight.Translation(vector3(0.0f,0.0f,m_fFarPlane));
	matHeight=matHeight*m_matPosition;
	vecCenter=matHeight.GetLoc();

	m_vecFrustumTop[0]=m_matPosition.GetLoc();
	m_vecFrustumTop[1]=vecCenter;
	m_vecFrustumTop[2]=vecCenterAxisY;

	m_vecCenterAxis[0]=(m_vecFrustumTop[1]-m_vecFrustumTop[0])^(m_vecFrustumTop[2]-m_vecFrustumTop[1]);
	m_vecCenterAxis[0].Normalize();
	m_vecCenterAxis[0]=-m_vecCenterAxis[0];

	m_vecFrustumTop[0]=m_matPosition.GetLoc();
	m_vecFrustumTop[1]=vecCenter;
	m_vecFrustumTop[2]=vecCenterAxisX;

	m_vecCenterAxis[1]=(m_vecFrustumTop[1]-m_vecFrustumTop[0])^(m_vecFrustumTop[2]-m_vecFrustumTop[1]);
	m_vecCenterAxis[1].Normalize();
	m_vecCenterAxis[1]=-m_vecCenterAxis[1];


	m_vecFrustumTop[0]=m_matPosition.GetLoc();
	m_vecFrustumTop[1]=vecTopRight;
	m_vecFrustumTop[2]=vecTopLeft;

	m_vecFrustumNormal[0]=(m_vecFrustumTop[1]-m_vecFrustumTop[0])^(m_vecFrustumTop[2]-m_vecFrustumTop[1]);
	m_vecFrustumNormal[0].Normalize();

	m_vecFrustumBottom[0]=m_matPosition.GetLoc();
	m_vecFrustumBottom[1]=vecBottomLeft;
	m_vecFrustumBottom[2]=vecBottomRight;
	m_vecFrustumNormal[1]=(m_vecFrustumBottom[1]-m_vecFrustumBottom[0])^(m_vecFrustumBottom[2]-m_vecFrustumBottom[1]);
	m_vecFrustumNormal[1].Normalize();

	m_vecFrustumLeft[0]=m_matPosition.GetLoc();
	m_vecFrustumLeft[1]=vecTopLeft;
	m_vecFrustumLeft[2]=vecBottomLeft;
	m_vecFrustumNormal[2]=(m_vecFrustumLeft[1]-m_vecFrustumLeft[0])^(m_vecFrustumLeft[2]-m_vecFrustumLeft[1]);
	m_vecFrustumNormal[2].Normalize();
	
	m_vecFrustumRight[0]=m_matPosition.GetLoc();
	m_vecFrustumRight[1]=vecBottomRight;
	m_vecFrustumRight[2]=vecTopRight;
	m_vecFrustumNormal[3]=(m_vecFrustumRight[1]-m_vecFrustumRight[0])^(m_vecFrustumRight[2]-m_vecFrustumRight[1]);
	m_vecFrustumNormal[3].Normalize();

	/*
	m_vecFrustumNear[1]=;
	m_vecFrustumNear[2]=;
	*/
	
	
	

	matrix matNear[3];
	matNear[0].Translation(vector3(0.0f,0.0f,0.0f)+vector3(0.0f,0.0f,m_fNearPlane));
	matNear[1].Translation(vector3(100.0f,0.0f,0.0f)+vector3(0.0f,0.0f,m_fNearPlane));
	matNear[2].Translation(vector3(0.0f,100.0f,0.0f)+vector3(0.0f,0.0f,m_fNearPlane));	
	
	matNear[0]=matNear[0]*m_matPosition;
	matNear[1]=matNear[1]*m_matPosition;
	matNear[2]=matNear[2]*m_matPosition;

	m_vecFrustumNear[0]=matNear[0].GetLoc();
	m_vecFrustumNear[1]=matNear[1].GetLoc();
	m_vecFrustumNear[2]=matNear[2].GetLoc();


	/*
	vector3 vecTopLeft,vecTopRight,vecBottomLeft,vecBottomRight;	
	

	vecTopLeft.y=fFrustumHeight;
	vecTopLeft.x=-fFrustumWidth;
	vecTopLeft.z=fFarPlane;

	vecTopRight.y=fFrustumHeight;
	vecTopRight.x=fFrustumWidth;
	vecTopRight.z=fFarPlane;

	vecBottomLeft.y=-fFrustumHeight;
	vecBottomLeft.x=-fFrustumWidth;
	vecBottomLeft.z=fFarPlane;

	vecBottomRight.y=-fFrustumHeight;
	vecBottomRight.x=fFrustumWidth;
	vecBottomRight.z=fFarPlane;

	m_vecFrustumTop[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumTop[1]=vecTopRight;
	m_vecFrustumTop[2]=vecTopLeft;

	m_vecFrustumBottom[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumBottom[1]=vecBottomLeft;
	m_vecFrustumBottom[2]=vecBottomRight;

	m_vecFrustumLeft[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumLeft[1]=vecTopLeft;
	m_vecFrustumLeft[2]=vecBottomLeft;
	
	m_vecFrustumRight[0]=vector3(0.0f,0.0f,-1000.0f);
	m_vecFrustumRight[1]=vecBottomRight;
	m_vecFrustumRight[2]=vecTopRight;
	*/

	/*
	matrix matPlaneTrans;
	matPlaneTrans.Translation(m_vecFrustumTop[0]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumTop[0]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumTop[1]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumTop[1]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumTop[2]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumTop[2]=matPlaneTrans.GetLoc();
	//

	
	matPlaneTrans.Translation(m_vecFrustumBottom[0]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumBottom[0]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumBottom[1]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumBottom[1]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumBottom[2]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumBottom[2]=matPlaneTrans.GetLoc();

	//
	
	matPlaneTrans.Translation(m_vecFrustumLeft[0]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumLeft[0]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumLeft[1]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumLeft[1]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumLeft[2]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumLeft[2]=matPlaneTrans.GetLoc();
	//

	matPlaneTrans.Translation(m_vecFrustumRight[0]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumRight[0]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumRight[1]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumRight[1]=matPlaneTrans.GetLoc();

	
	matPlaneTrans.Translation(m_vecFrustumRight[2]);
	matPlaneTrans=m_matPosition*matPlaneTrans;
	m_vecFrustumRight[2]=matPlaneTrans.GetLoc();	
	*/
}

void CViewCamera::InterfaceFreelook(int MouseX, int MouseY)
{
	if(m_isAnimateCamera)
	{
		PlayAnimate();
		return;
	}
	D3DXMATRIX *matPosition=(D3DXMATRIX*)GetMatPosition();

	float fDeltaY=MouseX/300.0f;
	float fDeltaX=MouseY/300.0f;

	D3DXVECTOR3 vecT(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecR(0.0f, 0.0f, 0.0f);

	D3DXMATRIX matT, matR;
	D3DXQUATERNION qR;		

	if(KeyPressed(VK_MENU))
		return;
	
	if(KeyPressed(VK_SHIFT))
	{
		if(KeyPressed('A') || KeyPressed(VK_LEFT)) vecT.x -= 341.0f; // Slide Left			
		if(KeyPressed('D') || KeyPressed(VK_RIGHT)) vecT.x += 341.0f; // Slide Right
		if(KeyPressed(VK_DOWN)) vecT.y += 341.0f; // Slide Down
		if(KeyPressed(VK_UP)) vecT.y -= 341.0f; // Slide Up
		if(KeyPressed('W')) vecT.z += 341.0f; // Move Forward
		if(KeyPressed('S')) vecT.z -= 341.0f; // Move Backward	
		if(KeyPressed(VK_NUMPAD4)) fDeltaY+=0.008f;
		if(KeyPressed(VK_NUMPAD6)) fDeltaY-=0.008f;
		if(KeyPressed(VK_NUMPAD8)) fDeltaX-=0.008f;
		if(KeyPressed(VK_NUMPAD2)) fDeltaX+=0.008f;
	}
	else
	{
		if(KeyPressed('A') || KeyPressed(VK_LEFT) )  vecT.x -= 1.0f; // Slide Left
		if(KeyPressed('D') || KeyPressed(VK_RIGHT)) vecT.x += 1.0f; // Slide Right
		if(KeyPressed(VK_DOWN))                                       vecT.y += 1.0f; // Slide Down
		if(KeyPressed(VK_UP))                                         vecT.y -= 1.0f; // Slide Up
		if(KeyPressed('W'))
			vecT.z += 1.0f; // Move Forward
		if(KeyPressed('S'))
			vecT.z -= 1.0f; // Move Backward	

		if(KeyPressed(VK_NUMPAD4)) fDeltaY+=0.008f;
		if(KeyPressed(VK_NUMPAD6)) fDeltaY-=0.008f;
		if(KeyPressed(VK_NUMPAD8)) fDeltaX-=0.008f;
		if(KeyPressed(VK_NUMPAD2)) fDeltaX+=0.008f;
	}	
	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);
	D3DXMatrixMultiply(matPosition, &matT, matPosition);
	
	float fRotationX=GetRotationX();
	float fRotationY=GetRotationY();
	
	D3DXQuaternionRotationYawPitchRoll(&qR, 0.0f,-fRotationX ,0.0f);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(matPosition, &matR, matPosition);

	fRotationX-=fDeltaX;
	fRotationY-=fDeltaY;
	
	D3DXQuaternionRotationYawPitchRoll(&qR,-fDeltaY,fRotationX, 0.0f);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(matPosition, &matR, matPosition);	

	SetRotaitionX(fRotationX);
	SetRotaitionY(fRotationY);
	
	SetVecPosition(vector3(GetMatPosition()->_41,GetMatPosition()->_42,GetMatPosition()->_43));	


	/*
	matrix matView;
	float fViewHeight=70000;
	matView.CameraLookAt(vector3(30000.0f/2.0f,fViewHeight,30000.0f/2.0f),vector3(30000.0f/2.0f,0.0f,30000.0f/2.0f),vector3(0.0f,0.0f,1.0f));
	//*/
	
	matrix *matPos=GetMatPosition();
	matrix matInv;
	matInv.Inverse(*matPos);
	
	SetMatView(matInv);	
	//matPos->Inverse(matView);
	//SetMatView(matView);
	if(KeyPressed(VK_SPACE))
	{
		
	}
	else
	{
		MoveFrustum();
	}
}

void CViewCamera::InterfaceCharlook(int MouseX, int MouseY, float fCharMove)
{	
	if(m_isAnimateCamera)
	{
		PlayAnimate();
		return;
	}
	
	/*
	D3DXMATRIX *matPosition=(D3DXMATRIX*)GetMatPosition();
	float fRoty=(float)MouseX/300.0f;
	float fRotx=(float)MouseY/300.0f;

	D3DXVECTOR3 vecT(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecR(-fRotx,-fRoty, 0.0f);	
	
	//if(KeyPressed('W'))
	vecT.z += fCharMove;

	D3DXMATRIX matT, matR;
	D3DXQUATERNION qR;	
	D3DXQuaternionRotationYawPitchRoll(&qR, 0, -m_fRotx, 0);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(m_matExtraPosition, &matR, m_matExtraPosition);	
	
	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);
	D3DXMatrixMultiply(m_matExtraPosition, &matT, m_matExtraPosition);	

	m_fRotx+=vecR.x;
	m_fRoty+=vecR.y;
	m_fRotz+=vecR.z;
	
	D3DXQuaternionRotationYawPitchRoll(&qR, vecR.y, m_fRotx, vecR.z);

	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(m_matExtraPosition, &matR, m_matExtraPosition);					

	D3DXMatrixTranslation(&matT,0.0f,0.0f,-350.0f);
	
	D3DXMatrixMultiply(matPosition, &matT, m_matExtraPosition);		
	
	SetVecPosition(vector3(GetMatPosition()->_41,GetMatPosition()->_42,GetMatPosition()->_43));	
	
	matrix *matPos=GetMatPosition();
	matrix matInv;
	matInv.Inverse(*matPos);	
	SetMatView(matInv);
	MoveFrustum();	
	*/
}

void CViewCamera::InterfaceCharlook2(int MouseX, int MouseY)
{	
	if(m_isAnimateCamera)
	{
		PlayAnimate();
		return;
	}	
	
	m_fRotx+=(float)MouseY/300.0f;
	m_fRoty+=(float)MouseX/300.0f;	
	if(m_fRotx > 3.14159f/2.0f-0.3f)
		m_fRotx=3.14159f/2.0f-0.3f;
	if(m_fRotx < -3.14159f/2.0f+0.7f)
		m_fRotx=-3.14159f/2.0f+0.7f;
	//DXMATRIX matT, matR;
	matrix matRotation,matResult;
	D3DXQUATERNION qR;

	matResult.Translation(vector3(0.0f,0.0f,-1.0f));
	D3DXQuaternionRotationYawPitchRoll(&qR,m_fRoty,m_fRotx,0.0f);
	D3DXMatrixRotationQuaternion(matRotation,&qR);		
	matResult=matResult*matRotation;	
	m_vecCameraPosition=matResult.GetLoc();	


	/*
	m_fRotx=3.1415f;
	m_fRoty=3.1415f;
	matrix matRotY,matRotX,matResult;
	matRotY.YRotation(m_fRoty);
	matRotX.XRotation(m_fRotx);
	matResult.Translation(vector3(1.0f,0.0f,0.0f));
	matResult=matResult*matRotY;
	vector3 vecRotY=matRotY.GetLoc();
	vecRotY^
	*/
}

void CViewCamera::InterfaceFreeCamera(int MouseX, int MouseY)
{
	if(m_isAnimateCamera)
	{
		PlayAnimate();
		return;
	}
	//InterfaceFreelook(-MouseX,-MouseY);
}

void CViewCamera::LookAt(vector3 vecPos, vector3 vecTarget, vector3 vecUp)
{
/*	m_matView.CameraLookAt(vecPos,vecTarget,vecUp);
	m_matPosition.Inverse(m_matView);
	MoveFrustum();*/
		
	vector3 vecNewUp = vector3(0.0f,1.0f,0.0f);
	

	D3DXMatrixLookAtLH((D3DXMATRIX *)(&m_matView),(D3DXVECTOR3 *)(&vecPos),(D3DXVECTOR3 *)&vecTarget,(D3DXVECTOR3 *)&vecNewUp);
	
	m_matPosition.Inverse(m_matView);
	MoveFrustum();


	D3DXVECTOR3* pZBasis = (D3DXVECTOR3*) &m_matPosition._31;

    m_fRoty   = atan2f( pZBasis->x, pZBasis->z );
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_fRotx = -atan2f( pZBasis->y, fLen );
	m_fRotz = 0.0f;
	m_vecPosition = vector3(m_matPosition._41,m_matPosition._42,m_matPosition._43);
	//m_matView=m_matStackView;
	//m_matPosition=m_matStackPosition;
}

void CViewCamera::PlayAnimate()
{
	return;

	if(m_dwTickTime==-1)
	{
		m_dwTickTime=GetTickCount();
		m_dwLastUpdateTime=m_dwTickTime;
		return ;
	}	
	DWORD dwOldTickTime=m_dwTickTime;
	m_dwTickTime=GetTickCount();

	m_fNowAniTime+=(float)(m_dwTickTime-dwOldTickTime);
	
	float fNode=(m_fNowAniTime/m_fTotalAniTime)*100.0f;
	bool bNodeFound=false;
	int cNode=1;
	for(;cNode<(int)m_CameraAnimateList.size();cNode++)
	{
		if( m_CameraAnimateList[cNode-1].m_Time <= fNode &&
			fNode <=m_CameraAnimateList[cNode].m_Time)
		{
			bNodeFound=true;
			break;
		}
	}

	float fInter;

	if(bNodeFound==false)
	{
		m_isAnimateCamera=false;
		m_dwTickTime=-1;
		//return;
		cNode=(int)m_CameraAnimateList.size()-1;
		fInter=1.0f;
		if((int)m_CameraAnimateList.size()==0)
			return;		
	}
	else
	{
		fInter=(fNode-m_CameraAnimateList[cNode-1].m_Time)/(m_CameraAnimateList[cNode].m_Time-m_CameraAnimateList[cNode-1].m_Time);
	}

	vector3 vecNowCameraPosition,vecCameraInter;
	vecCameraInter=m_CameraAnimateList[cNode].m_vecPos-m_CameraAnimateList[cNode-1].m_vecPos;	
	vecNowCameraPosition=m_CameraAnimateList[cNode-1].m_vecPos+fInter*vecCameraInter;	

	D3DXQUATERNION qResult,q1,q2;
	q1.x=m_CameraAnimateList[cNode-1].m_qAngle.x;
	q1.y=m_CameraAnimateList[cNode-1].m_qAngle.y;
	q1.z=m_CameraAnimateList[cNode-1].m_qAngle.z;
	q1.w=m_CameraAnimateList[cNode-1].m_qAngle.w;

	q2.x=m_CameraAnimateList[cNode].m_qAngle.x;
	q2.y=m_CameraAnimateList[cNode].m_qAngle.y;
	q2.z=m_CameraAnimateList[cNode].m_qAngle.z;
	q2.w=m_CameraAnimateList[cNode].m_qAngle.w;

	matrix matRot;

	D3DXQuaternionSlerp(&qResult,&q1,&q2,fInter);
	D3DXMatrixRotationQuaternion(matRot,&qResult);
	
	matrix matViewPos;
	matViewPos.Translation(vecNowCameraPosition);

	matViewPos=matRot*matViewPos;

	m_matView.Inverse(matViewPos);
	m_matPosition=matViewPos;
	MoveFrustum();

	m_vecPosition.x=m_matPosition._41;
	m_vecPosition.y=m_matPosition._42;
	m_vecPosition.z=m_matPosition._43;
}

void CViewCamera::StartPlay()
{
	m_isAnimateCamera=true;
	m_fNowAniTime=0.0f;
	m_dwTickTime=-1;
}

void CViewCamera::Load(char *strFilename)
{
	CameraNode AddNode;
	FILE *fp=fopen(strFilename,"r");
	if(fp==NULL)
		return;

	while(fscanf(fp,"%d %f %f %f, %f %f %f %f \n",&AddNode.m_Time,
												&AddNode.m_vecPos.x,
											   &AddNode.m_vecPos.y,
											   &AddNode.m_vecPos.z,
											   &AddNode.m_qAngle.x,
											   &AddNode.m_qAngle.y,
											   &AddNode.m_qAngle.z,
											   &AddNode.m_qAngle.w)==8)
	{
		m_CameraAnimateList.push_back(AddNode);
	}

	
}

void CViewCamera::Save(char *strFilename)
{
	FILE *fp=fopen(strFilename,"w");
	for(int i=0;i<(int)m_CameraAnimateList.size();i++)
	{
		fprintf(fp,"%d %f %f %f, %f %f %f %f \n",
			m_CameraAnimateList[i].m_Time,
			m_CameraAnimateList[i].m_vecPos.x,
			m_CameraAnimateList[i].m_vecPos.y,
			m_CameraAnimateList[i].m_vecPos.z,
			m_CameraAnimateList[i].m_qAngle.x,
			m_CameraAnimateList[i].m_qAngle.y,
			m_CameraAnimateList[i].m_qAngle.z,
			m_CameraAnimateList[i].m_qAngle.w);
	}
	fclose(fp);
}

void CViewCamera::Unload()
{
	m_CameraAnimateList.clear();
}

void CViewCamera::EndPlay()
{
	m_isAnimateCamera=false;
}

vector3 CViewCamera::GetViewTowardVector()
{
	D3DXMATRIX *matPosition=(D3DXMATRIX*)GetMatPosition();	
	D3DXMATRIX matT,matResult;

	D3DXVECTOR3 vecT(0.0f, 0.0f, 1.0f);	
	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);	

	D3DXMatrixMultiply(&matResult, &matT, matPosition);	

	vector3 vecOld,vecNew;
	vecOld=vector3(matPosition->_41,matPosition->_42,matPosition->_43);
	vecNew=vector3(matResult._41,matResult._42,matResult._43);
	
	return vecNew-vecOld;
}

vector3 CViewCamera::GetViewUpVector()
{
	D3DXMATRIX *matPosition=(D3DXMATRIX*)GetMatPosition();	
	D3DXMATRIX matT,matResult;

	D3DXVECTOR3 vecT(0.0f, 1.0f, 0.0f);	
	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);	

	D3DXMatrixMultiply(&matResult, &matT, matPosition);	

	vector3 vecOld,vecNew;
	vecOld=vector3(matPosition->_41,matPosition->_42,matPosition->_43);
	vecNew=vector3(matResult._41,matResult._42,matResult._43);
	
	return vecNew-vecOld;
}