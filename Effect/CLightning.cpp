#include "CLightning.h"
#include "SceneManager.h"
#include "GMMemory.h"

CLightning ::CLightning() {
	m_EndCount = 0;

	m_Alpha = 1.0f;
	m_bFadeOut = false;
	m_FadeSpeed = 0.001f;

	m_PointList = NULL;
	m_PointNum = 0;
	m_QuadHalfWidth = LIGHTHALFWIDTH;
	m_InsertCount = 0;
	m_RenderCount = 0;
	m_Device = NULL;
	m_Vert = NULL;
	m_VerticalVert = NULL;
	m_VertNum = 0;
	m_Texture = NULL;
	m_TexNum = 0;
	m_ShakeMax = m_QuadHalfWidth;
	m_ShakeMin = 0.0f;

	srand(time(0));
	m_MaxRenderCount = 0;
	m_Color = D3DCOLOR_ARGB(255,255,255,255);
	m_UvAni = 0.0f;
	m_UnitUv = 0.0f;
	m_PlaneValue = 0;
	m_bSetEndEffect = false;
	m_EndUnit = 5;
	m_LightningValue = LV_LIGHTNING;
	m_bCrash = false;
	m_bEndInput = false;

	m_iAlpha = 255;

	tmp_Vertex = NULL;
	tmp_Vertex2 = NULL;
	
	m_VerticalVert2 = NULL;
	tmp_Vertex3 = NULL;

	m_BezierPointList.clear();


}
CLightning ::CLightning(LPDIRECT3DDEVICE8 device) {
	m_EndCount = 0;
	m_Alpha = 1.0f;
	m_bFadeOut = false;
	m_FadeSpeed = 0.001f;

	m_PointList = NULL;
	m_PointNum = 0;
	m_QuadHalfWidth = LIGHTHALFWIDTH;
	m_InsertCount = 0;
	m_RenderCount = 0;
	m_Device = device;
	m_Vert = NULL;
	m_VerticalVert = NULL;
	m_VertNum = 0;
	m_Texture = NULL;
	m_TexNum = 0;
	m_ShakeMax = m_QuadHalfWidth;
	m_ShakeMin = 0.0f;
	srand(time(0));
	m_MaxRenderCount = 0;
	m_Color = D3DCOLOR_ARGB(255,255,255,255);
	m_UvAni = 0.0f;
	m_UnitUv = 0.0f;
	m_PlaneValue = 0;
	m_bSetEndEffect = false;
	m_EndUnit = 5;
	m_LightningValue = LV_LIGHTNING;
	
	m_bCrash = false;
	m_bEndInput = false;

	m_iAlpha = 255;
	
	tmp_Vertex = NULL;
	tmp_Vertex2 = NULL;

	
	m_VerticalVert2 = NULL;
	tmp_Vertex3 = NULL;
	m_BezierPointList.clear();
	

}

CLightning ::CLightning(int num,LPDIRECT3DDEVICE8 device) {
	m_Alpha = 1.0f;
	m_bFadeOut = false;
	m_FadeSpeed = 0.001f;

	m_EndCount = 0;

	m_PointList = NULL;
	m_PointNum = num;
	m_QuadHalfWidth = LIGHTHALFWIDTH;
	m_InsertCount = 0;
	m_RenderCount = 0;
	m_Device = device;
	m_Vert = NULL;
	CreateList(m_PointNum);
	m_VertNum = 0;
	m_VerticalVert = NULL;
	m_Texture = NULL;
	m_TexNum = 0;
	m_ShakeMax = m_QuadHalfWidth;
	m_ShakeMin = 0.0f;

	srand(time(0));
	m_MaxRenderCount = 0;
	m_Color = D3DCOLOR_ARGB(255,255,255,255);
	m_UvAni = 0.0f;
	m_UnitUv = 0.0f;
	m_PlaneValue = 0;
	m_bSetEndEffect = false;
	m_EndUnit = 5;
	m_LightningValue = LV_LIGHTNING;
	m_bCrash = false;
	m_bEndInput = false;

	m_iAlpha = 255;

	tmp_Vertex = NULL;
	tmp_Vertex2 = NULL;

	m_VerticalVert2 = NULL;
	tmp_Vertex3 = NULL;
	m_BezierPointList.clear();
	
}

CLightning::~CLightning() {
	int i;


	if(m_PointList)
		delete[] m_PointList;
	if(m_Vert) {
		for(i=0;i<m_VertNum;i++) {	
			if(CSceneManager::ms_pBufferPools) 
			{

				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_Vert[i]);
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_VerticalVert[i]);
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_VerticalVert2[i]);
			}
			else
			{
				m_Vert[i]->Release();
				m_VerticalVert[i]->Release();
				m_VerticalVert2[i]->Release();
			}
		}
		delete[] m_Vert;
		delete[] m_VerticalVert;
		delete[] m_VerticalVert2;

	}

	if(m_Texture) {
		delete[] m_Texture;
	}
	if(tmp_Vertex)
		delete[] tmp_Vertex;
	if(tmp_Vertex2)
		delete[] tmp_Vertex2;
	if(tmp_Vertex3)
		delete[] tmp_Vertex3;

	if(m_LightningValue == LV_BEZIER) {
		m_BezierPointList.clear();
	}

	
}
void CLightning::CreateList(int num) {
	if(m_PointList)
		delete[] m_PointList;
	m_PointList = new D3DXVECTOR3[num];
	memset(m_PointList,0,sizeof(D3DXVECTOR3) * num);
	m_PointNum = num;
}

void CLightning::DeleteList() {
	if(m_PointList)
		delete[] m_PointList;
}
void CLightning::InsertPoint(float x,float y,float z) {

	m_PointList[m_InsertCount].x = x;
	m_PointList[m_InsertCount].y = y;
	m_PointList[m_InsertCount++].z = z;
	if(m_InsertCount >= m_PointNum)
		m_InsertCount = 0;
}
void CLightning::InsertPoint(D3DXVECTOR3 point) {
	m_PointList[m_InsertCount++] = point;
}
void CLightning::InsertEmptyPoint(float x,float y,float z) {
	int i;

	if(m_InsertCount >=m_PointNum) {

		memcpy(m_PointList,&m_PointList[1],(m_PointNum -1) * sizeof(D3DXVECTOR3));
		m_InsertCount = m_PointNum-1;
	}
	for(i=m_InsertCount;i<m_PointNum;i++) {
		m_PointList[i].x = x;
		m_PointList[i].y = y;
		m_PointList[i].z = z;
	}
	m_InsertCount++;

}
void CLightning::SetEnd() {
	int i;
	for(i=m_PointNum -2; i >=0;i--) {
		m_PointList[i] = m_PointList[i+1];
			
	}
}
void CLightning::SetStartPoint(float x,float y,float z) {
	m_PointList[0].x = x;
	m_PointList[0].y = y;
	m_PointList[0].z = z;
}

void CLightning::SetEndPoint(float x,float y,float z) {
	m_PointList[m_PointNum - 1].x = x;
	m_PointList[m_PointNum - 1].y = y;
	m_PointList[m_PointNum - 1].z = z;

}

void CLightning::SetBezierControl(float fControl,float fHeight,float fSpeed,float fAccel,float fLength,float fFade,float fLimit,int axis,int endspeed,int endaccel,int iUv) {	// Bezier 곡선의 control point setting

	m_Bezier.m_fControl = fControl;
	m_Bezier.m_fControlHeight = fHeight;
	m_Bezier.m_fSpeed = fSpeed;
	m_Bezier.m_fAccel = fAccel;
	m_Bezier.m_fStartEndLength = fLength;
	m_Bezier.m_fFade = fFade;
	m_Bezier.m_fLimitLength = fLimit;
	m_Bezier.m_iAxis = axis;
	m_Bezier.m_iEndSpeed = endspeed;
	m_Bezier.m_iEndAccel = endaccel;
	m_Bezier.m_iUvMode = iUv;
	//m_Bezier.m_ft = 0.05f;

}
bool CLightning::SetBezierPoint() {									//Bezier Curve point setting
																	// 보간율은 항상 0.05f 이다.
	bool bReturn  = true;
	
	if(m_InsertCount == 0) {										// Start Position Setting
		m_BezierPointList.push_back(m_PointList[0]);
		m_InsertCount++;
		return bReturn;
	}
	//else if(m_InsertCount == 0)
	//		return;

	D3DXVECTOR3 line_vector;
	float length;
	
	line_vector = m_PointList[m_PointNum -1] - m_PointList[0];
	length = D3DXVec3Length(&line_vector);
	
	D3DXVECTOR3 cross = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f,1.0f,0.0f);

	D3DXVec3Cross(&cross,&line_vector,&up);
	D3DXVec3Normalize(&cross,&cross);
	
	// control setting

	D3DXVECTOR3 bezier_control = m_BezierPointList[0] + (line_vector * m_Bezier.m_fControl);	
	
	D3DXVECTOR3 axis;
	D3DXVECTOR3 axis2;
	D3DXVECTOR3 axis3;

	switch(m_Bezier.m_iAxis) {
	case 0:
		bezier_control += cross * m_Bezier.m_fControlHeight;
		break;
	case 1:
		axis = cross;
		
		axis2 = cross;
		axis3 = cross + up;
		axis2 = axis2 + axis3;

		axis +=axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 2:
		axis = cross;
		axis2 = cross + up;
		axis = axis + axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 3:
		axis = cross;
		axis2 = cross + up;
		axis = axis + axis2;

		axis3 = cross + up;

		axis +=axis3;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 4:
		axis = cross + up;
		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;	

		break;
	case 5:
		axis = cross + up;
		
		axis2 = up;
		axis3 = cross + up;
		axis2 = axis2 + axis3;

		axis += axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 6:
		axis = up;
		axis2 = cross + up;
		axis = axis + axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 7:

		axis = up;
		axis2 = cross + up;
		axis = axis + axis2;
		
		axis3 = up;
		
		axis += axis3;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	case 8:
		axis = up;
		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;	
		break;
	case 9:
		axis = up;

		axis2 = up;
		axis3 = -cross + up;
		axis2 = axis2 + axis3;
		
		axis += axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;
	case 10:
		
		axis = up;
		axis2 = -cross + up;
		axis = axis + axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;
	case 11:
		axis = up;
		axis2 = -cross + up;
		axis = axis + axis2;

		axis3 = -cross + up;

		axis += axis3;
		
		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;
	case 12:
		axis = -cross + up;
		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;	
		break;

	case 13:
		axis = -cross + up;
	
		axis2 = -cross;
		axis3 = -cross + up;
		axis2 = axis2 + axis3;

		axis += axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;
	
	case 14:
		axis = -cross;
		axis2 = -cross + up;
		axis = axis + axis2;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;
	case 15:
		
		axis = -cross;
		axis2 = -cross + up;
		axis = axis + axis2;

		axis3 = -cross;

		axis += axis3;

		D3DXVec3Normalize(&axis,&axis);
		bezier_control += axis * m_Bezier.m_fControlHeight;
		break;

	}

	
	// 현제 리스트에 들어있는 마지막 점
	D3DXVECTOR3 vecLastInput = m_BezierPointList[ m_InsertCount - 1 ];

	D3DXVECTOR3 vecInput;
	D3DXVECTOR3 vecCheck;

	while(bReturn && !m_bEndInput) {
		
		float t = m_Bezier.m_ft;
		float t2 = t * t;
		float fInput = 0.0f;

		vecInput = m_PointList[0] * ((1.0f - t) * (1.0f - t)) + (bezier_control * 2 *(1.0f - t) * t ) + (m_PointList[m_PointNum -1] * t2);	
		vecCheck = vecInput - vecLastInput;

		m_BezierPointList.push_back(vecInput);
		m_InsertCount++;
		
		fInput = D3DXVec3Length(&vecCheck);
		
		
		// Speed setting
		m_Bezier.m_ft += 0.05f;
		
		if(m_Bezier.m_ft >= 1.0f) {

			/////// 마지막점 setting
			float t = m_Bezier.m_ft;
			float t2 = t * t;
			float fInput = 0.0f;

			vecInput = m_PointList[0] * ((1.0f - t) * (1.0f - t)) + (bezier_control * 2 *(1.0f - t) * t ) + (m_PointList[m_PointNum -1] * t2);	
			vecCheck = vecInput - vecLastInput;

			m_BezierPointList.push_back(vecInput);
			m_InsertCount++;

			///////
			m_bEndInput = true;
			m_Bezier.m_ft = 1.0f;
			bReturn = true;

		}
		else if(m_Bezier.m_ft < 0.0f)
			m_Bezier.m_ft = 0.0f;
		
		if( fInput > m_Bezier.m_fSpeed )							// 이동거리가 속도 보다 크다면 이번 추가 끝
			break;

	}
	int iLastIndex = m_BezierPointList.size();

	if(iLastIndex >= 2) {											// 라인 길이 제한 적용
		while((iLastIndex = m_BezierPointList.size()) >= 2) {
			vecLastInput = m_BezierPointList[iLastIndex - 1];
			vecCheck = vecLastInput - m_BezierPointList[0];
			float fLength = D3DXVec3Length(&vecCheck);
			if(fLength > m_Bezier.m_fLimitLength) {					// 제한길이를 넘어섰을때
				if(m_BezierPointList.size() >= 1) {
					m_BezierPointList.erase(m_BezierPointList.begin());
					m_InsertCount--;
				}
			}
			
			else if(m_bEndInput) {									// 충돌했을때 1점씩 지워 나간다
				for(int cEndSpeed = 0; cEndSpeed < m_Bezier.m_iEndSpeed; cEndSpeed++) {
					if(m_BezierPointList.size() >= 1) {
						m_BezierPointList.erase(m_BezierPointList.begin());
						m_InsertCount--;
					}
					else 
						break;
				}
				m_Bezier.m_iEndSpeed += m_Bezier.m_iEndAccel;
				break;
			}
			

			else {
				break;
			}
		}

	}

	m_Bezier.m_fSpeed += m_Bezier.m_fAccel;							// 속도에 가속도 적용

	if(m_bCrash) {	// 충돌한 것은 true
		m_iAlpha -= m_Bezier.m_fFade;
		if(m_iAlpha < 0)
			m_iAlpha = 0;

	}

	return bReturn;

}
/*
void CLightning::SetBezierPoint() {									//Bezier Curve point setting 

	D3DXVECTOR3 line_vector;
	
	float length;
	float unit_length;

	int split_num = m_PointNum -1;

	line_vector = m_PointList[m_PointNum -1] - m_PointList[0];
	length = D3DXVec3Length(&line_vector);
	unit_length = length/(float)split_num;

	D3DXVECTOR3 cross = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f,1.0f,0.0f);

	//D3DXVec3Normalize(&line_vector,&line_vector);

	D3DXVec3Cross(&cross,&line_vector,&up);
	D3DXVec3Normalize(&cross,&cross);

	// control setting

	D3DXVECTOR3 bezier_control = line_vector * m_Bezier.m_fControl;	
	bezier_control += cross * m_Bezier.m_fControlHeight;

	
	float t = m_Bezier.m_ft;
	float t2 = t * t;

	m_PointList[m_InsertCount++] = m_PointList[0] * (1.0f - t2) + (bezier_control * 2 *(1.0f - t) * t ) + (m_PointList[m_PointNum -1] * t2);
	
	// Speed setting
	m_Bezier.m_ft += m_Bezier.m_fSpeed;
	m_Bezier.m_fSpeed += m_Bezier.m_fAccel;
	
	if(m_Bezier.m_ft > 1.0f)
		m_Bezier.m_ft = 1.0f;
	else if(m_Bezier.m_ft < 0.0f)
		m_Bezier.m_ft = 0.0f;
	

	if(m_InsertCount >= m_PointNum -1) {
		D3DXVECTOR3 limit_vec = m_PointList[m_PointNum -2] - m_PointList[0];
		float limit_length = D3DXVec3Length(&limit_vec);

		if(limit_length > m_Bezier.m_fLimitLength) {	//길이가 limit 보다 클때 
			memcpy(m_PointList,&m_PointList[1],(m_PointNum -2) * sizeof(D3DXVECTOR3));	
		}
		else {  // 길이가 limit 보다 작을때	
			memcpy(&m_PointList[1],&m_PointList[2],(m_PointNum -3) * sizeof(D3DXVECTOR3));
		}
		m_InsertCount = m_PointNum-2;
	}

	if(m_bCrash) {	// 충돌한 것은 true
		m_iAlpha -= 10;
		if(m_iAlpha < 0)
			m_iAlpha = 0;

	}
	
}
*/
void CLightning::SetRandPoint() {
	D3DXVECTOR3 line_vector;
	
	float length;
	float unit_length;
	float unit_shake = m_ShakeMax - m_ShakeMin;

	int i;
	int split_num = m_PointNum -1;

	line_vector = m_PointList[m_PointNum -1] - m_PointList[0];
	length = D3DXVec3Length(&line_vector);
	unit_length = length/(float)split_num;

	D3DXVec3Normalize(&line_vector,&line_vector);
	// 시작점과 끝점을 제외한 점들 line 따라 setting
	// 그런뒤 흔들기
	for(i=1;i<m_PointNum - 1;i++) {	
		D3DXVECTOR3 tmp;
		m_PointList[i] = m_PointList[0] + ((unit_length * i) * line_vector);	
		
		//shaking value range : m_ShakeMin ~ m_ShakeMax
		tmp.x = m_ShakeMin + (float)(rand() %(int)((unit_shake) * 1000)) / 1000.0f;
		tmp.y = m_ShakeMin + (float)(rand() %(int)((unit_shake) * 1000)) / 1000.0f;
		tmp.z = m_ShakeMin + (float)(rand() %(int)((unit_shake) * 1000)) / 1000.0f;
		if(rand() % 2)
			tmp.x *= -1.0f;
		if(rand() % 2)
			tmp.y *= -1.0f;
		if(rand() % 2)
			tmp.z *= -1.0f;

		m_PointList[i] +=tmp;

	}
}
void CLightning::CreateTexture(int num) {
	m_TexNum = num;
	m_Texture = new CTexture[m_TexNum];
}
void CLightning::SetTexture(int index,char *filename) {
	CTexture::SetPath(EFFECTTEXTUREPATH);

	if(strlen(filename) >0) {
		m_Texture[index].Load(filename);
	
	}
}
void CLightning::CreateVertexBuffer() {
	int i;
	
	m_Vert = new LPDIRECT3DVERTEXBUFFER8[m_VertNum];
	m_VerticalVert = new LPDIRECT3DVERTEXBUFFER8[m_VertNum];
	m_VerticalVert2 = new LPDIRECT3DVERTEXBUFFER8[m_VertNum];

	for(i=0;i<m_VertNum;i++) {
//		m_Vert[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,true);
//		m_VerticalVert[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,true);
//		m_VerticalVert2[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,true);
		m_Vert[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,false);
		m_VerticalVert[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,false);
		m_VerticalVert2[i] = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,false);
		/*
		m_Device->CreateVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_DEFAULT,&(m_Vert[i]));
		m_Device->CreateVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_DEFAULT,&(m_VerticalVert[i]));
		m_Device->CreateVertexBuffer(m_PointNum * 2 * sizeof(CLightVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_DEFAULT,&(m_VerticalVert2[i]));
		*/
		if(m_LightningValue == 	LV_LIGHTNING)
			SetVertexBuffer(i);
		else if(m_LightningValue == LV_BEZIER) {
			
				SetBezierBuffer(i,m_Bezier.m_iUvMode);
		}
	}

}

void CLightning::SetBezierBuffer(int index,int Wheelvalue) {
	int i;
	float s = 0.0f;
	float t = 0.0f;

	D3DXVECTOR3 camera_pos;
	D3DXMATRIX view;
	D3DXMATRIX invview;

	D3DXVECTOR3 up_vec;
	
	D3DXVECTOR3 vector1;
	D3DXVECTOR3 vector2;
	D3DXVECTOR3 vector3;
	D3DXVECTOR3 vector4;
	D3DXVECTOR3 vector5;

	D3DXVECTOR3 tmp;
	D3DXVECTOR3 tmp2;
	D3DXVECTOR3 tmp3;

	int nBuffer = 30;

	int nPoint = m_BezierPointList.size();
	// Buffer 범위 check
	if(nPoint <= 1)
		return;
	if(nPoint > nBuffer)
		return;

	//가로 플랜 	
	if(tmp_Vertex == NULL) {	
		tmp_Vertex = new CLightVertex[nBuffer * 2];
	}
	else
		memset(tmp_Vertex,0,sizeof(CLightVertex) * nBuffer * 2);

	// 세로 플랜
	if(m_PlaneValue > LPLANE_ONE) { // 십자 플랜 이상
		if(tmp_Vertex2 == NULL) {
			tmp_Vertex2 = new CLightVertex[nBuffer * 2];
		}
		else {
			memset(tmp_Vertex2,0,sizeof(CLightVertex) * nBuffer * 2);
		}
		if(tmp_Vertex3 == NULL) {
			tmp_Vertex3 = new CLightVertex[nBuffer * 2];
		}
		else {
			memset(tmp_Vertex3,0,sizeof(CLightVertex) * nBuffer * 2);
		}
		
	}
	m_Device->GetTransform(D3DTS_VIEW,&view);
	D3DXMatrixInverse(&invview,NULL,&view);		
	camera_pos.x = invview._41;
	camera_pos.y = invview._42;
	camera_pos.z = invview._43;
	// up vector
	up_vec.x = 0.0f;
	up_vec.y = 1.0f;
	up_vec.z = 0.0f;

	for(i=0;i<nPoint * 2;i++) {
		/*if(i >= ((nPoint * 2) -2)) { //마지막 제어점일때
			vector1 = m_BezierPointList[i / 2] - m_BezierPointList[ (i - 2) / 2 ];

		}
		else // 다음 제어점 과 현제 점 까지의 백터
			vector1 = m_BezierPointList[(i + 2) / 2] - m_BezierPointList[(i/2)];
		*/
		
		vector1 = m_PointList[m_PointNum - 1] - m_PointList[0];

		if(m_PlaneValue == LPLANE_ONE)
			vector2 = camera_pos - m_BezierPointList[i/2];
		else 
			vector2 = up_vec;// - m_PointList[i/2];
		
		D3DXVec3Normalize(&vector1,&vector1);
		D3DXVec3Normalize(&vector2,&vector2);

		D3DXVec3Cross(&vector3,&vector1,&vector2);
		D3DXVec3Normalize(&vector3,&vector3);
	
	/*	if(m_bBeforeBez) {					// 베지어 곡선이 방향이 반대로 바뀌면서 cross vector의 방향이 뒤집히는 것을 검사
			float fBezDot = D3DXVec3Dot(&m_BeforeBezCross,&vector3);
			if((fBezDot < -1.0f) && (m_Bezier.m_iAxis == 8)) 
				m_bBezInv = true;
			else
				m_bBezInv = false;
			
		}
		else {		// Before Bez Cross Not Setting 
			m_BeforeBezCross = vector3;
			D3DXVec3Normalize(&m_BeforeBezCross,&m_BeforeBezCross);
			m_bBeforeBez = true;
			m_bBezInv = false;
		}
	*/	
		if(m_PlaneValue > LPLANE_ONE) {
				
			vector5 = vector3 + vector2;
			D3DXVec3Normalize(&vector5,&vector5);
			vector4 = vector5;
			
			vector5 = -vector3 + vector2;
			D3DXVec3Normalize(&vector5,&vector5);
			
		}
		
		//vertex 생성
		if(!(i % 2)) {
				
			tmp = m_BezierPointList[i/2] + vector3 * m_QuadHalfWidth;

			if(m_PlaneValue > LPLANE_ONE) {
					tmp2 = m_BezierPointList[i/2] + vector4 * m_QuadHalfWidth;
					tmp3 = m_BezierPointList[i/2] + vector5 * m_QuadHalfWidth;
			}
		}
		else {
			
			tmp = m_BezierPointList[(i-1)/2] - vector3 * m_QuadHalfWidth;
		
			if(m_PlaneValue > LPLANE_ONE) {
					tmp2 = m_BezierPointList[(i-1)/2] - vector4 * m_QuadHalfWidth;
					tmp3 = m_BezierPointList[(i-1)/2] - vector5 * m_QuadHalfWidth;
			}
		}

	
		
		if(m_bFadeOut) {
			m_Color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Alpha);
			m_Alpha -= m_FadeSpeed;
			if(m_Alpha <=0) {
				m_Alpha = 0.0f;
				
			}
		}
		tmp_Vertex[i].x = tmp.x;
		tmp_Vertex[i].y = tmp.y;
		tmp_Vertex[i].z = tmp.z;

		if(m_bCrash) {			//  충돌후
			m_Color = D3DCOLOR_ARGB(m_iAlpha,255,255,255);
		}
		
		tmp_Vertex[i].color = m_Color;

		if(Wheelvalue == 0) {				//Lightning Uv	
			switch(i % 4) {					// texcoord 설정
					
					case 0:
						tmp_Vertex[i].s = 0.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
					case 1:
						tmp_Vertex[i].s = 1.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
					case 2:
						tmp_Vertex[i].s = 0.0f; 
						tmp_Vertex[i].t = (i /2) + m_UvAni; 
						break;
					case 3:
						tmp_Vertex[i].s = 1.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
			}
		}
		else {								//wheel Uv
			switch(i % 4) {					// texcoord 설정
					
					case 0:
						tmp_Vertex[i].s = 0.0f;
						break;
					case 1:
						tmp_Vertex[i].s = 1.0f;
						break;
					case 2:
						tmp_Vertex[i].s = 0.0f; 
						break;
					case 3:
						tmp_Vertex[i].s = 1.0f;
						break;
			}
			tmp_Vertex[i].t = (1.0f/nPoint) * (int)(i/2);

		}
		/////
		if(m_PlaneValue > LPLANE_ONE) {
			tmp_Vertex2[i].x = tmp2.x;
			tmp_Vertex2[i].y = tmp2.y;
			tmp_Vertex2[i].z = tmp2.z;
			tmp_Vertex2[i].color = m_Color;

			tmp_Vertex3[i].x = tmp3.x;
			tmp_Vertex3[i].y = tmp3.y;
			tmp_Vertex3[i].z = tmp3.z;
			tmp_Vertex3[i].color = m_Color;

			
			if(Wheelvalue == 0) {			//Lightning Uv				
				switch(i % 4) {				// texcoord 설정
					
						case 0:
							tmp_Vertex2[i].s = 0.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							tmp_Vertex3[i].s = 0.0f;
							tmp_Vertex3[i].t = (i /2) + m_UvAni;

							break;
						case 1:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							tmp_Vertex3[i].s = 1.0f;
							tmp_Vertex3[i].t = (i /2) + m_UvAni;

							break;
						case 2:
							tmp_Vertex2[i].s = 0.0f; 
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							tmp_Vertex3[i].s = 0.0f; 
							tmp_Vertex3[i].t = (i /2) + m_UvAni;

							break;
						case 3:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							tmp_Vertex3[i].s = 1.0f;
							tmp_Vertex3[i].t = (i /2) + m_UvAni;						
							break;
							
				}
			}
			else {							//wheel Uv
				switch(i % 4) {				// texcoord 설정
						
						case 0:
							tmp_Vertex2[i].s = 0.0f;
							tmp_Vertex3[i].s = 0.0f;

							break;
						case 1:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex3[i].s = 1.0f;
							break;
						case 2:
							tmp_Vertex2[i].s = 0.0f; 
							tmp_Vertex3[i].s = 0.0f;
							break;
						case 3:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex3[i].s = 1.0f;
							break;
				}
				tmp_Vertex2[i].t = (1.0f/nPoint) * (int)(i/2);
				tmp_Vertex3[i].t = (1.0f/nPoint) * (int)(i/2);

			}
		///////
		}


	}

	//set buffer
	VOID *tmp_buffer;
	if(nPoint >= m_PointNum)
		nPoint = m_PointNum;

//	m_Vert[index]->Lock(0,m_PointNum* sizeof(CLightVertex) * 2,(BYTE **)&tmp_buffer,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
	m_Vert[index]->Lock(0,0,(BYTE **)&tmp_buffer,0);
	
	memcpy(tmp_buffer,tmp_Vertex,2 * nPoint * sizeof(CLightVertex));

	m_Vert[index]->Unlock();
//	delete[] tmp_Vertex;
//	tmp_Vertex = NULL;

	if(m_PlaneValue > LPLANE_ONE) {
		VOID *tmp_buffer2;
//		m_VerticalVert[index]->Lock(0,m_PointNum * sizeof(CLightVertex) * 2,(BYTE **)&tmp_buffer2,D3DLOCK_DISCARD  | D3DLOCK_NOSYSLOCK);
		m_VerticalVert[index]->Lock(0,0,(BYTE **)&tmp_buffer2,0);
		
		memcpy(tmp_buffer2,tmp_Vertex2,2 * nPoint * sizeof(CLightVertex));

		m_VerticalVert[index]->Unlock();
//		delete[] tmp_Vertex2;
//		tmp_Vertex2 = NULL;

		VOID *tmp_buffer3;
//		m_VerticalVert2[index]->Lock(0,m_PointNum * sizeof(CLightVertex) * 2,(BYTE **)&tmp_buffer3,D3DLOCK_DISCARD  | D3DLOCK_NOSYSLOCK);
		m_VerticalVert2[index]->Lock(0,0,(BYTE **)&tmp_buffer3,0);
		
		memcpy(tmp_buffer3,tmp_Vertex3,2 * nPoint * sizeof(CLightVertex));

		m_VerticalVert2[index]->Unlock();
//		delete[] tmp_Vertex3;
//		tmp_Vertex3 = NULL;

	}
	m_MaxRenderCount = ((nPoint * 2)-3) + 1;
	
}
void CLightning::SetVertexBuffer(int index,int Wheelvalue) {

	int i;
	float s = 0.0f;
	float t = 0.0f;

	D3DXVECTOR3 camera_pos;
	D3DXMATRIX view;
	D3DXMATRIX invview;

	D3DXVECTOR3 up_vec;
	
	D3DXVECTOR3 vector1;
	D3DXVECTOR3 vector2;
	D3DXVECTOR3 vector3;
	D3DXVECTOR3 vector4;

	D3DXVECTOR3 tmp;
	D3DXVECTOR3 tmp2;
	//가로 플랜 	
	if(tmp_Vertex == NULL)
		tmp_Vertex = new CLightVertex[m_PointNum * 2];
	// 세로 플랜
	if(m_PlaneValue > LPLANE_ONE) { // 십자 플랜 이상
		if(tmp_Vertex2 == NULL)
			tmp_Vertex2 = new CLightVertex[m_PointNum * 2];
	}
	m_Device->GetTransform(D3DTS_VIEW,&view);
	D3DXMatrixInverse(&invview,NULL,&view);		
	camera_pos.x = invview._41;
	camera_pos.y = invview._42;
	camera_pos.z = invview._43;
	// up vector
	up_vec.x = 0.0f;
	up_vec.y = 1.0f;
	up_vec.z = 0.0f;

	for(i=0;i<m_PointNum * 2;i++) {
		if(i >= ((m_PointNum *2) -2)) { //마지막 제어점일때
			/*D3DXVECTOR3 tmp_v = m_PointList[i/2] - m_PointList[(i-2)/2];
			vector1 = (m_PointList[i/2] * 1.1f) - m_PointList[i/2];*/
			vector1 = m_PointList[i/2] - m_PointList[(i-2)/2];

		}
		else // 다음 제어점 과 현제 점 까지의 백터
			vector1 = m_PointList[(i + 2)/2] - m_PointList[i/2];

		if(m_PlaneValue == LPLANE_ONE)
			vector2 = camera_pos - m_PointList[i/2];
		else 
			vector2 = up_vec;// - m_PointList[i/2];
		
		D3DXVec3Normalize(&vector1,&vector1);
		D3DXVec3Normalize(&vector2,&vector2);

		D3DXVec3Cross(&vector3,&vector1,&vector2);
		D3DXVec3Normalize(&vector3,&vector3);
		if(m_PlaneValue > LPLANE_ONE) {
			D3DXVec3Cross(&vector4,&vector1,&vector3);
			D3DXVec3Normalize(&vector4,&vector4);
		}
		
		//vertex 생성
		if(!(i % 2)) {
			tmp = m_PointList[i/2] + vector3 * m_QuadHalfWidth;
			if(m_PlaneValue > LPLANE_ONE)
				tmp2 = m_PointList[i/2] + vector4 * m_QuadHalfWidth;
		}
		else {
			tmp = m_PointList[(i-1)/2] - vector3 * m_QuadHalfWidth;
			if(m_PlaneValue > LPLANE_ONE)
				tmp2 = m_PointList[(i-1)/2] - vector4 * m_QuadHalfWidth;
		}
		
		if(m_bFadeOut) {
			m_Color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Alpha);
			m_Alpha -= m_FadeSpeed;
			if(m_Alpha <=0) {
				m_Alpha = 0.0f;
				
			}
		}
		tmp_Vertex[i].x = tmp.x;
		tmp_Vertex[i].y = tmp.y;
		tmp_Vertex[i].z = tmp.z;

		if(m_bCrash) {			//  충돌후
			m_Color = D3DCOLOR_ARGB(m_iAlpha,255,255,255);
		}
		
		tmp_Vertex[i].color = m_Color;

		if(Wheelvalue == 0) {				//Lightning Uv	
			switch(i % 4) {					// texcoord 설정
					
					case 0:
						tmp_Vertex[i].s = 0.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
					case 1:
						tmp_Vertex[i].s = 1.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
					case 2:
						tmp_Vertex[i].s = 0.0f; 
						tmp_Vertex[i].t = (i /2) + m_UvAni; 
						break;
					case 3:
						tmp_Vertex[i].s = 1.0f;
						tmp_Vertex[i].t = (i /2) + m_UvAni;
						break;
			}
		}
		else {								//wheel Uv
			switch(i % 4) {					// texcoord 설정
					
					case 0:
						tmp_Vertex[i].s = 0.0f;
						break;
					case 1:
						tmp_Vertex[i].s = 1.0f;
						break;
					case 2:
						tmp_Vertex[i].s = 0.0f; 
						break;
					case 3:
						tmp_Vertex[i].s = 1.0f;
						break;
			}
			tmp_Vertex[i].t = (1.0f/m_PointNum) * (int)(i/2);

		}
		/////
		if(m_PlaneValue > LPLANE_ONE) {
			tmp_Vertex2[i].x = tmp2.x;
			tmp_Vertex2[i].y = tmp2.y;
			tmp_Vertex2[i].z = tmp2.z;
			tmp_Vertex2[i].color = m_Color;
			if(Wheelvalue == 0) {			//Lightning Uv				
				switch(i % 4) {				// texcoord 설정
					
						case 0:
							tmp_Vertex2[i].s = 0.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							break;
						case 1:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							break;
						case 2:
							tmp_Vertex2[i].s = 0.0f; 
							tmp_Vertex2[i].t = (i /2) + m_UvAni; 
							break;
						case 3:
							tmp_Vertex2[i].s = 1.0f;
							tmp_Vertex2[i].t = (i /2) + m_UvAni;
							break;
							
				}
			}
			else {							//wheel Uv
				switch(i % 4) {				// texcoord 설정
						
						case 0:
							tmp_Vertex2[i].s = 0.0f;
							break;
						case 1:
							tmp_Vertex2[i].s = 1.0f;
							break;
						case 2:
							tmp_Vertex2[i].s = 0.0f; 
							break;
						case 3:
							tmp_Vertex2[i].s = 1.0f;
							break;
				}
				tmp_Vertex2[i].t = (1.0f/m_PointNum) * (int)(i/2);

			}
		///////
		}


	}

	//set buffer
	VOID *tmp_buffer;
//	m_Vert[index]->Lock(0,m_PointNum * sizeof(CLightVertex) * 2,(BYTE **)&tmp_buffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK );
	m_Vert[index]->Lock(0,0,(BYTE **)&tmp_buffer, 0 );
	
	memcpy(tmp_buffer,tmp_Vertex,2 * m_PointNum * sizeof(CLightVertex));

	m_Vert[index]->Unlock();
	//delete[] tmp_Vertex;

	if(m_PlaneValue > LPLANE_ONE) {
		VOID *tmp_buffer2;
//		m_VerticalVert[index]->Lock(0,m_PointNum * sizeof(CLightVertex) * 2,(BYTE **)&tmp_buffer2,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
		m_VerticalVert[index]->Lock(0,0,(BYTE **)&tmp_buffer2,0);
		
		memcpy(tmp_buffer2,tmp_Vertex2,2 * m_PointNum * sizeof(CLightVertex));

		m_VerticalVert[index]->Unlock();
		//delete[] tmp_Vertex2;
	}
	m_MaxRenderCount = ((m_PointNum * 2)-3) + 1;

}
// fade out 완료 되었으면 true return 
bool CLightning::GetEnd() {
	if(m_Alpha <= 0.0f)
		return true;
	return false;
}
void CLightning::Render2(int blendmode) {
	int k;

	if(	m_bSetEndEffect == true) { // wheel을 사용하는 esf의 middle effect가 끝났을때
		if(m_InsertCount >= 1) {
			for(k = 0;k<m_PointNum / m_EndUnit;k++) // 지금까지의 제어점을 마지막 점으로 setting 
				InsertEmptyPoint(m_PointList[m_InsertCount - 1].x,m_PointList[m_InsertCount - 1].y,m_PointList[m_InsertCount - 1].z);
		}
		SetVertexBuffer(0,1);
	}

	D3DXMATRIX beforeworld;
	DWORD zwrite,alphablend;
	DWORD cullmode;

	m_Device->GetTransform(D3DTS_WORLD,&beforeworld);
	m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwrite);
	m_Device->GetRenderState(D3DRS_ALPHABLENDENABLE,&alphablend);
	m_Device->GetRenderState(D3DRS_CULLMODE,&cullmode);

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	m_Device->SetTransform(D3DTS_WORLD,&world);

	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);   
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  	
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	if(blendmode ==0) { // src alpha, dst one
		m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	}
	else if(blendmode == 1){ // src alpha,dst invsrc_alpha
		m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	}
	
	m_Device->SetTexture(0,m_Texture[0].GetTexture());

	m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);

	m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	//m_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

	for(int i=0;i<m_VertNum;i++) {
		m_Device->SetStreamSource(0,m_Vert[i],sizeof(CLightVertex));
		m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
			//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,(m_PointNum * 2) - 1);
		if((m_InsertCount -1)>0)
			m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,(m_InsertCount-1) * 2);
		
		
		if(m_PlaneValue > LPLANE_ONE) {	// 십자 플랜 이상일때 (십자, X...)
			m_Device->SetStreamSource(0,m_VerticalVert[i],sizeof(CLightVertex));
			
				//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,(m_PointNum * 2) - 1);
			if((m_InsertCount -1)>0)
				m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,(m_InsertCount-1) * 2);

		}
	}
	m_Device->SetTransform(D3DTS_WORLD,&beforeworld);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zwrite);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,alphablend);
	m_Device->SetRenderState(D3DRS_CULLMODE,cullmode);

}
void CLightning::Render(int blendmode) {
		
		D3DXMATRIX world;
		D3DXMATRIX src_world;
		DWORD zwrite,alphablend;

		D3DXMatrixIdentity(&world);
		
		m_Device->GetTransform(D3DTS_WORLD,&src_world);
		m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwrite);
		m_Device->GetRenderState(D3DRS_ALPHABLENDENABLE,&alphablend);

		m_Device->SetTransform(D3DTS_WORLD,&world);
		
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);             
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		if(blendmode ==0) { // src alpha, dst one
			m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		}
		else if(blendmode == 1){ // src alpha,dst invsrc_alpha
			m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		}

		//m_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);

		m_Device->SetTexture(0,m_Texture[0].GetTexture());
	
		m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		//m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MAXANISOTROPY,16);

			
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

		m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		for(int i=0;i<m_VertNum;i++) {
			m_Device->SetStreamSource(0,m_Vert[i],sizeof(CLightVertex));
			m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
			//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,(m_PointNum * 2) - 1);
			
			
			if(m_LightningValue == LV_BEZIER) {
				if((m_InsertCount -1)>0)
					m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,(m_InsertCount-1) * 2);

			}
			else
				if(m_RenderCount > 0)
					m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,m_RenderCount);
			
			
			
			if(m_PlaneValue > LPLANE_ONE) {	// 십자 플랜 이상일때 (십자, X...)
				m_Device->SetStreamSource(0,m_VerticalVert[i],sizeof(CLightVertex));
			
				if(m_LightningValue == LV_BEZIER) {
					if((m_InsertCount -1)>0) {
						m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,(m_InsertCount-1) * 2);

						m_Device->SetStreamSource(0,m_VerticalVert2[i],sizeof(CLightVertex));
						m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,(m_InsertCount-1) * 2);
					
					}
				}
				else 
					if(m_RenderCount > 0)
					//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,(m_PointNum * 2) - 1);
						m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,m_RenderCount);
			}
		}
		m_Device->SetTransform(D3DTS_WORLD,&src_world);
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zwrite);
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,alphablend);
}
D3DXVECTOR3 CLightning::GetBezierLastPos() {
	int iSize = m_BezierPointList.size();
	if(iSize > 0)
		return m_BezierPointList[iSize - 1];
	else
		return D3DXVECTOR3(0.0f,0.0f,0.0f);
}
bool CLightning::UpdateLightning(D3DXVECTOR3 &pos,bool bCrash) {
	int i;
	bool bBezierReturn = false;

	m_RenderCount++;
	
	m_UvAni += m_UnitUv;
	
	m_bCrash = bCrash;				// 충돌 유무

	for(i=0;i<m_VertNum;i++) {
		
		if(m_LightningValue == LV_LIGHTNING) {
			SetRandPoint();
			SetVertexBuffer(i);
		}
		else if(m_LightningValue == LV_BEZIER) {
			
			bBezierReturn = SetBezierPoint();

			SetBezierBuffer(i,m_Bezier.m_iUvMode);
			
			int nPoint = m_BezierPointList.size();
			if(nPoint > 1) {
				pos.x = m_BezierPointList[nPoint - 1].x;
				pos.y = m_BezierPointList[nPoint - 1].y;
				pos.z = m_BezierPointList[nPoint - 1].z;
			}
			else {
				pos.x = m_PointList[0].x;
				pos.y = m_PointList[0].y;
				pos.z = m_PointList[0].z;

			}
		}

	}
	if(m_LightningValue == LV_LIGHTNING) {
		if(m_RenderCount > m_MaxRenderCount) {
			m_RenderCount--;
		
			return true;
		}
		return false;
	}
	else if(m_LightningValue == LV_BEZIER){
		return bBezierReturn;	
	}
	return false;
}
void CLightning::SetColor() {
	m_Color = D3DCOLOR_ARGB(255,
							rand() % 256,
							rand() % 256,
							rand() % 256);

}