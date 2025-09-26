// ViewCamera.h: interface for the CViewCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWCAMERA_H__AB40CC30_EEEA_4FA1_96CE_A7CB0153A3CA__INCLUDED_)
#define AFX_VIEWCAMERA_H__AB40CC30_EEEA_4FA1_96CE_A7CB0153A3CA__INCLUDED_

#include "VECTOR.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include <d3d.h>
#include <vector>
#include "Vertex.h"

class CViewCamera  
{	
	vector3 m_vecPosition;	
	long m_CameraMode;	
public:		
	vector3 GetViewTowardVector();
	vector3 GetViewUpVector();
	void EndPlay();
	void Unload();
	// Render for Fade In/Out Func & Memver variable
	TLVertex m_pVertex[4];	
	char m_AdderFadeValue;
	unsigned char m_FadeInitValue;
	int m_cFade;
	void SetFade(char AdderFadeValue,unsigned char FadeInitValue)
	{
		m_AdderFadeValue=AdderFadeValue;
		m_FadeInitValue=FadeInitValue;
		m_cFade=0;
	};
	int GetFadeCount(){return m_cFade;};
	//	
	class CameraNode{
	public:
		vector3 m_vecPos;
		vector3 m_vecTarget;
		vector3 m_vecUp;
		quaternion m_qAngle;
		long m_Time;
	};

	float m_fTotalAniTime;
	void SetTotalAniTime(float fTotalAniTime){m_fTotalAniTime=fTotalAniTime;};
	float GetTotalAniTime(){return m_fTotalAniTime;};
	bool GetAnimateCamera(){return m_isAnimateCamera;};
	std::vector<CameraNode>* GetCameraList(){return &m_CameraAnimateList;};	
	void Save(char *strFilename);
	void Load(char *strFilename);
	void StartPlay();
	void PlayAnimate();
	void LookAt(vector3 vecPos,vector3 vecTarget,vector3 vecUp);
	void InterfaceFreeCamera(int MouseX,int MouseY);
	bool m_isAnimateCamera;
	bool GetIsAnimateCamera(){return m_isAnimateCamera;};
	DWORD m_dwTickTime,m_dwLastUpdateTime;
	float m_fNowAniTime;
	std::vector<CameraNode> m_CameraAnimateList;
	//		
	vector3 m_vecFrustumTop[3];
	vector3 m_vecFrustumBottom[3];
	vector3 m_vecFrustumLeft[3];
	vector3 m_vecFrustumRight[3];
	vector3 m_vecFrustumNormal[4];	
	vector3 m_vecFrustumNear[3];

	vector3 m_vecCenterAxis[2];
	
	float m_fRotx,m_fRoty,m_fRotz;
	float m_fAspect,m_fFov,m_fFarPlane,m_fNearPlane;
	vector3 m_vecCameraPosition;
	matrix m_matView;
	matrix m_matPosition;

	matrix m_matStackView,m_matStackPosition;
	float m_fStackRotx,m_fStackRoty,m_fStackRotz;
	vector3 m_vecStackPos;

	void SaveViewMatrix()
	{
		m_matStackView=m_matView;
		m_matStackPosition=m_matPosition;
		m_fStackRotx=m_fRotx;
		m_fStackRoty=m_fRoty;
		m_fStackRotz=m_fRotz;
		m_vecStackPos=m_vecPosition;
	};
	void LoadViewMatrix()
	{
		m_matView=m_matStackView;
		m_matPosition=m_matStackPosition;
		m_fRotx=m_fStackRotx;
		m_fRoty=m_fStackRoty;
		m_fRotz=m_fStackRotz;		
		m_vecPosition=m_vecStackPos;
		MoveFrustum();
	};	
	void InterfaceCharlook2(int MouseX,int MouseY);
	void InterfaceCharlook(int MouseX,int MouseY,float fCharMove);
	void InterfaceFreelook(int MouseX,int MouseY);
	void MoveFrustum();	
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void BuildFrustum(float fAspect,float fFov,float fNearPlane,float fFarPlane);

	
	vector3 *GetPosition(){return &m_vecPosition;};
	D3DMATRIX* GetMatView(){return (D3DMATRIX*)(&m_matView);};
	matrix* GetMatPosition(){return &m_matPosition;};	
	float GetRotationX(){return m_fRotx;};
	float GetRotationY(){return m_fRoty;};
	float GetRotationZ(){return m_fRotz;};
	void SetRotaitionX(float fRotX){m_fRotx=fRotX;};
	void SetRotaitionY(float fRotY){m_fRoty=fRotY;};
	void SetRotaitionZ(float fRotZ){m_fRotz=fRotZ;};

	void SetVecPosition(vector3 vecPos){m_vecPosition=vecPos;};
	void SetMatView(matrix &mat){m_matView=mat;};
	void SetMatPosition(matrix &mat){m_matPosition=mat;};
	CViewCamera();
	virtual ~CViewCamera();
};

#endif // !defined(AFX_VIEWCAMERA_H__AB40CC30_EEEA_4FA1_96CE_A7CB0153A3CA__INCLUDED_)
