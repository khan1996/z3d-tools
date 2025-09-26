#include "snowfall.h"
#include "SceneManager.h"
#include <stdio.h>
#include "GMMemory.h"

/*
#define SDEFAULTNUM 100
#define SMAXSIZE 10.0 //snow Max size
#define SMINSIZE 1.0 // snow Min size

enum CSNOWAXIS{
	CSNOWX,
	CSNOWZ,
};

class CSnowFall{ 
public:
	class CSnowUnit {
	public:
		D3DXVECTOR3 m_Pos;
		float m_Size;
		float m_HeightFieldHigh;
		
		bool m_Life;
		float m_Alpha;
		float m_UnitAlpha;

		DWORD m_Color;

		CSnowUnit() {
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			m_Size = SMINSIZE;
			m_HeightFieldHigh = -1.0f;
			m_Life = true;
			m_Alpha = 1.0f;
			m_UnitAlpha = 0.1f;
			m_Color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
			
		}
		~CSnowUnit() {}
	}
	
	class CSnowVertex {
	public:	
		D3DXVECTOR3 v;
	    D3DCOLOR    color;
	
		CSnowVertex() {
			v.x = v.y = v.z = 0.0f;
			color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);
		}
		~CSnowVertex() {}
	};

	float m_Min[2];
	float m_Max[2];
	float m_StartHeight;
	float m_SnowNum;
	
	D3DXVECTOR3 m_PlayerPos;
	D3DXVECTOR3 m_Gravity;
	D3DXVECTOR3 m_WindSpeed;

	LPDIRECT3DDEVICE8 m_Device;
	SnowUnit *m_Units;
	
	CSnowFall();
	~CSnowFall();
	
	void Init(int num);
	void SetDevice(LPDIRECT3DDEVICE8 device) { m_Device = device;}

	void SetVertexBuffer();
	
	void Render();
	void Update();
	

};
*/

CSnowFall::CSnowFall() {
	
	srand(timeGetTime());

	m_Units = NULL;
	m_Size[CSNOWX] = m_Size[CSNOWZ] = 0.0f;
	
	m_UnitSize[0] = SMINSIZE;
	m_UnitSize[1] = SMAXSIZE;

	m_Color[0] = 1.0f;
	m_Color[1] = 1.0f;
	m_Color[2] = 1.0f;

	m_StartHeight  = 0.0f;
	m_SnowNum = 0;
	m_PlayerPos.x = m_PlayerPos.y = m_PlayerPos.z = 0.0f;
	m_Gravity.x = m_Gravity.y = m_Gravity.z = 0.0f;
	m_WindSpeed.x = m_WindSpeed.y = m_WindSpeed.z = 0.0f;
	m_STexture = NULL;
	m_bValueSet = false;

}
CSnowFall::~CSnowFall() {

	if(m_Units != NULL) {
		delete[] m_Units;
	}
	if(m_SnowNum)
		m_VBuffer->Release();
	
	if(m_STexture != NULL)
		delete m_STexture;

}
void CSnowFall::Render() {
	
	if(m_bValueSet == false)
		return;
	HRESULT hr;
	DWORD lightmode,zwritemode;
	DWORD blendmode,srcblend,dstblend;
	DWORD cullmode;
	//backup state
	m_Device->GetRenderState(D3DRS_LIGHTING,&lightmode);
	m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwritemode);
	m_Device->GetRenderState(D3DRS_ALPHABLENDENABLE,&blendmode);
	m_Device->GetRenderState(D3DRS_SRCBLEND,&srcblend);
	m_Device->GetRenderState(D3DRS_DESTBLEND,&dstblend);
	m_Device->GetRenderState(D3DRS_CULLMODE,&cullmode);

	m_Device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	m_Device->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	hr = m_Device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(SMINSIZE) );
	/*
	hr = m_Device->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(2.0f) );
	hr = m_Device->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(1.00f) );
	hr = m_Device->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.00f) );
	*/

	m_Device->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    m_Device->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    m_Device->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

	hr = m_Device->SetRenderState( D3DRS_POINTSIZE,     FtoDW(4.0f) );
	//hr = m_Device->SetRenderState( D3DRS_POINTSIZE,     FtoDW(8.0f) );
	m_Device->SetTexture(0,m_STexture->GetTexture());
	

	////////////////////////////////////////////////	
	m_Device->SetRenderState( D3DRS_LIGHTING, FALSE);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);		
	
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  
	m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);  

	//m_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	

	m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

	m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	//// point sprite 

	m_Device->SetStreamSource( 0, m_VBuffer, sizeof(CSnowVertex));
	m_Device->SetVertexShader( D3DFVF_POINTVERTEX );
			
	m_Device->DrawPrimitive( D3DPT_POINTLIST, 0,m_SnowNum);
	////
	hr = m_Device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
	m_Device->SetRenderState( D3DRS_POINTSIZE,   FtoDW(1.0f) );
	m_Device->SetRenderState( D3DRS_POINTSPRITEENABLE,FALSE);
	m_Device->SetRenderState( D3DRS_POINTSCALEENABLE,FALSE);
	
	m_Device->SetRenderState(D3DRS_LIGHTING,lightmode);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zwritemode);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,blendmode);
	m_Device->SetRenderState(D3DRS_SRCBLEND,srcblend);
	m_Device->SetRenderState(D3DRS_DESTBLEND,dstblend);
	m_Device->SetRenderState(D3DRS_CULLMODE,cullmode);


}
void CSnowFall::SetVertexBuffer() {
	
	int i;
	CSnowVertex *point_vertex = NULL;
	point_vertex = new CSnowVertex[m_SnowNum];
	
	for(i=0;i<m_SnowNum;i++) {
		point_vertex[i].v = m_Units[i].m_Pos;
		point_vertex[i].color = m_Units[i].m_Color;
	}

	VOID *tmp;
	m_VBuffer->Lock(0,m_SnowNum * sizeof(CSnowVertex),(BYTE **)&tmp ,0);
	
	memcpy(tmp,point_vertex,sizeof(CSnowVertex) * m_SnowNum);  
	
	m_VBuffer->Unlock();
	
	delete[] point_vertex;
	point_vertex =  NULL;

}
void CSnowFall::SetWorldValue(float gx,float gy,float gz,float wx,float wy,float wz,char *filename) {

	m_Gravity.x = gx;
	m_Gravity.y = gy;
	m_Gravity.z = gz;

	m_WindSpeed.x = wx;
	m_WindSpeed.y = wy;
	m_WindSpeed.z = wz;
	if(filename != NULL) {
	
		CTexture::SetPath(EFFECTTEXTUREPATH);
		m_STexture = new CTexture;
		m_STexture->Load(filename);
		
	}
	
}
void CSnowFall::Init(LPDIRECT3DDEVICE8 device,int num,float x,float y,float z,float xsize,float zsize,float height) {
	m_bValueSet = true;
	SetDevice(device);

	m_SnowNum = num;

	m_Device->CreateVertexBuffer( m_SnowNum * sizeof(CSnowVertex),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		D3DFVF_POINTVERTEX, D3DPOOL_DEFAULT, &m_VBuffer);

	
	m_PlayerPos.x = x;
	m_PlayerPos.y = y;
	m_PlayerPos.z = z;
	m_StartHeight = height;
	
	m_Size[CSNOWX] = xsize;
	m_Size[CSNOWZ] = zsize;

	m_Units = new CSnowUnit[m_SnowNum];

	for(int i=0;i<m_SnowNum;i++) {
		SetUnitInfo(i);
	}
	SetVertexBuffer();	
}
void CSnowFall::SetColor(float r,float g,float b) {
	m_Color[0] = r;
	m_Color[1] = g;
	m_Color[2] = b;

}
void CSnowFall::SetUnitInfo(int index) {
	if(!m_Units[index].m_Life) {	
		float tmp;
		float tmp_size;
		//pos setting
		m_Units[index].m_Pos.y = m_StartHeight;
		float tmp_height =  m_StartHeight - m_PlayerPos.y;
		// 눈송이 생성 높이 : 카메라 높이 - tmp_height/2.0f ~ 카메라 높이 + tmp_height
		m_Units[index].m_Pos.y -= (rand() % (int)(tmp_height + (tmp_height / 2.0f)));

		m_Units[index].m_Pos.x = m_PlayerPos.x;
		tmp = (float)(rand() % ((int)m_Size[CSNOWX] * 100)) / 100.0f;
		if(rand() %2) {
			tmp *= -1.0f;
		}
		m_Units[index].m_Pos.x += tmp;

		m_Units[index].m_Pos.z = m_PlayerPos.z;
		tmp = (float)(rand() % (int)(m_Size[CSNOWZ] * 100.0f)) / 100.0f;
		if(rand() %2) {
			tmp *= -1.0f;
		}
		m_Units[index].m_Pos.z += tmp;
		//size setting
		tmp_size =  m_UnitSize[1] - m_UnitSize[0];
		tmp = (float)(rand() % (int)(tmp_size * 100.0f)) / 100.0f;
		
		m_Units[index].m_Size = m_UnitSize[0] + tmp; //unit size setting
		m_Units[index].m_Life = true;
		
		m_Units[index].m_Alpha = (float)(rand() % 1000) / 1000.0f + 0.3f;
		if(m_Units[index].m_Alpha > 1.0f)
			m_Units[index].m_Alpha = 1.0f;

		m_Units[index].m_UnitAlpha = (float)(rand() % 1000) / 100000.0f + 0.001f;
		m_Units[index].m_RGB[0] =  m_Color[0];
		m_Units[index].m_RGB[1] =  m_Color[1];
		m_Units[index].m_RGB[2] =  m_Color[2];
		m_Units[index].m_Color = D3DCOLOR_COLORVALUE(m_Color[0],m_Color[1],m_Color[2],m_Units[index].m_Alpha);
	}
	
}
// Minotaurs 적당이 수정;;
void CSnowFall::Update(float playerx,float playery,float playerz,float xsize,float zsize,float cheight,bool bReset) {
	int i;
	if(bReset)
		m_bValueSet = bReset;
	if((m_bValueSet == false) || (m_Units == NULL))
		return;
	m_Size[CSNOWX] = xsize;
	m_Size[CSNOWZ] = zsize;
	m_StartHeight = cheight;
	m_PlayerPos.x = playerx;
	m_PlayerPos.y = playery;
	m_PlayerPos.z = playerz;

	
	int n = 0;
	for(i = 0;i<m_SnowNum;i++) { 
			//////////////
		if(m_Units[i].m_Life == false)	//resetting
		{
			if(bReset)
				SetUnitInfo(i);
			else
				++n;
		}
		else //// update///
		{
			m_Units[i].m_Alpha -= m_Units[i].m_UnitAlpha;

			m_Units[i].m_Pos +=m_Gravity;

			if(!(rand() % CSNOWRAND)) {
				m_Units[i].m_Pos +=m_WindSpeed;	

			}
			vector3 tmp_pos = vector3(m_Units[i].m_Pos.x,m_Units[i].m_Pos.y,m_Units[i].m_Pos.z);

			m_Units[i].m_HeightFieldHigh = CSceneManager::m_HeightField.GetHeight(tmp_pos);
			
			if(m_Units[i].m_HeightFieldHigh > m_Units[i].m_Pos.y) // 지면에 다았을때  
				m_Units[i].m_Life = false;
			
			if(m_Units[i].m_Alpha <=0.0f) { // alpha value 0
				m_Units[i].m_Alpha = 0.0f;
				m_Units[i].m_Life = false;
			}
			else {
				m_Units[i].m_Color = D3DCOLOR_COLORVALUE(m_Units[i].m_RGB[0],m_Units[i].m_RGB[1],m_Units[i].m_RGB[2],m_Units[i].m_Alpha);
			}
			if((fabs(m_PlayerPos.x - m_Units[i].m_Pos.x) > m_Size[CSNOWX]) || (fabs(m_PlayerPos.z - m_Units[i].m_Pos.z) > m_Size[CSNOWZ])) 
				m_Units[i].m_Life = false; // 범위를 벗어난 눈
		}
	
	}
	// 더이상 새로 생성되는 넘이 없으면 m_bValueSet = false
	if(n == m_SnowNum)
	{
		m_bValueSet = false;
		return;
	}
	//vertex buffer setting
	SetVertexBuffer();
}
