#include "Waveline.h"
#include "GMMemory.h"

CWaveLine::CWaveLine() {
	m_Pick = false;
	m_Normal.x = m_Normal.z = 0.0f;
	m_Normal.y = 1.0f;
	m_PointNum = 0;

	m_WaveNum = 0;
	m_UnitInterpol = WAVEUNITVOT;
	m_LayerNum = WAVENUM;

	m_Length = (int)WAVEVECLENGTH;
	m_CreateFrame = WAVETERMFRAME;
	m_Vert = NULL;
	m_Bubble = NULL;

	m_Device = NULL;
	m_ShakeValue.x = WAVESHAKE;
	m_ShakeValue.z = WAVESHAKE;
	m_WaveTex = NULL;
	m_WaveTexNum = 0;
	m_BackupList = NULL;

	m_WaveCenter.x = 0.0f;
	m_WaveCenter.y = 0.0f;
	m_WaveCenter.z = 0.0f;
	m_DownLayerNum = 0;
	m_UpLayerNum = 0;
	m_BeforeTime = timeGetTime();
	m_CurrentTime = m_BeforeTime;
	tcount = 1;
	kcount = 1;
	t_value1 = 0.0f;
	t_value2 = 1.0f; 
	t_value3 = 0.0f;


}
CWaveLine ::CWaveLine(LPDIRECT3DDEVICE8 device) {
	m_Pick = false;
	m_Normal.x = m_Normal.z = 0.0f;
	m_Normal.y = 1.0f;
	m_PointNum = 0;
	m_WaveNum = 0;
	m_UnitInterpol = WAVEUNITVOT;
	m_LayerNum = WAVENUM;
	m_Length = (int)WAVEVECLENGTH;
	m_CreateFrame = WAVETERMFRAME;
	m_Vert = NULL;
	m_Bubble = NULL;

	m_Device = device;
	m_ShakeValue.x = WAVESHAKE;
	m_ShakeValue.z = WAVESHAKE;
	m_WaveTex = NULL;
	m_WaveTexNum = 0;
	m_BackupList = NULL;

	m_WaveCenter.x = 0.0f;
	m_WaveCenter.y = 0.0f;
	m_WaveCenter.z = 0.0f;

	m_DownLayerNum = 0;
	m_UpLayerNum = 0;
	m_BeforeTime = timeGetTime();
	m_CurrentTime = m_BeforeTime;
	
	tcount = 1;
	kcount = 1;
	t_value1 = 0.0f;
	t_value2 = 1.0f; 
	t_value3 = 0.0f;



}
CWaveLine::~CWaveLine() {
	int i;
	if(m_WaveTexNum > 0)
		delete[] m_WaveTex;

	if(m_BackupList)
		delete[] m_BackupList;
	if(m_Vert) {
		for(i=0;i<(m_LayerNum);i++) {
			m_Vert[i]->Release();

		}
		delete[] m_Vert;

	}
	if(m_Bubble) {
		for(i=0;i<m_LayerNum;i++) {
			m_Bubble[i]->Release();
		}
		delete[] m_Bubble;
	}

	if(m_WaveLayer.size()) {
		for(i=0;i<(int)m_WaveLayer.size();i++) {
			delete[] m_WaveLayer[i];
		}
	}
	if(m_WaveBubble.size()) {
		for(i=0;i<(int)m_WaveBubble.size();i++) {
			delete[] m_WaveBubble[i];

		}
	}
	if(m_Wave.size()) {
		for(i=0;i<(int)m_Wave.size();i++) {
			delete[] m_Wave[i];
		}
	}
	m_WaveLayer.clear();
	m_WaveBubble.clear();

	m_Wave.clear();
	m_WaveList.clear();


}
void CWaveLine::CreateTexture(int num) {
	m_WaveTexNum = num;
	m_WaveTex = new CTexture[m_WaveTexNum];

}
void CWaveLine::SetTexture(int num,char *filename) {
	char filepath[256] = {0};

	if(strlen(filename) >0) {
		CTexture::SetPath(EFFECTTEXTUREPATH);	
		m_WaveTex[num].Load(filename);
	}
/*
	if(num) {
		CTexture::SetPath(EFFECTTEXTUREPATH);
		m_WaveTex[0].Load("pado.dds");
		m_WaveTex[1].Load("pado2.dds");
	}
*/
}
void CWaveLine::InsertPoint(float x,float y,float z) {
	CWavePoint tmp_point;
	tmp_point.m_Pos.x = x;
	tmp_point.m_Pos.y = y;
	tmp_point.m_Pos.z = z;
	tmp_point.m_PointIndex = m_PointNum; 
	m_WaveList.push_back(tmp_point);

	m_PointNum++;	
}
void CWaveLine::InsertPoint(D3DXVECTOR3 pos) {

	CWavePoint tmp_point;
	tmp_point.m_Pos = pos;
	tmp_point.m_PointIndex = m_PointNum; 
	m_WaveList.push_back(tmp_point);

	m_PointNum++;

}
void CWaveLine::DeletePoint(float x,float y,float z) {
	int i;
	for(i=0;i<m_PointNum;i++) {
		if((m_WaveList[i].m_Pos.x == x) && (m_WaveList[i].m_Pos.y == y) && (m_WaveList[i].m_Pos.z == z)) {
			m_WaveList.erase(m_WaveList.begin() + i);
			m_PointNum--;

		}
	}
}
void CWaveLine::DeletePoint(D3DXVECTOR3 pos) {
	int i;
	for(i=0;i<m_PointNum;i++) {
		if((m_WaveList[i].m_Pos.x == pos.x) && (m_WaveList[i].m_Pos.y == pos.y) && (m_WaveList[i].m_Pos.z == pos.z)) {
			m_WaveList.erase(m_WaveList.begin() + i);
			m_PointNum--;
		}	
	}
}
void CWaveLine::SetInterpol(float vot) { 
	
	m_UnitInterpol = vot;
	// 영역 체크
	if(m_UnitInterpol >= 1.0f)
		m_UnitInterpol = 1.0f;
	else if(m_UnitInterpol <= 0.0f)
		m_UnitInterpol = 0.0f;

}
void CWaveLine::Render() {
		DWORD cullmode,zwritemode;
		DWORD alpha1,alpha2,light;

		HRESULT hr;
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
			
		m_Device->SetTransform(D3DTS_WORLD,&world);
		m_Device->GetRenderState(D3DRS_CULLMODE,&cullmode);
		m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwritemode);
		hr = m_Device->GetRenderState(D3DRS_SRCBLEND,&alpha1);
		hr = m_Device->GetRenderState(D3DRS_DESTBLEND,&alpha2);
		hr = m_Device->GetRenderState( D3DRS_LIGHTING,&light);

		hr = m_Device->SetRenderState( D3DRS_LIGHTING, FALSE);
		hr = m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		hr = m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);             
		hr = m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  
		hr = m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		hr = m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		if(m_Pick)
			m_Device->SetRenderState(D3DRS_FILLMODE,  D3DFILL_WIREFRAME);

		//m_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
		//m_Device->BeginScene();
	
	/*	hr = m_Device->SetTexture(0,m_WaveTex[1].GetTexture());
		
		
		hr = m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		hr = m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		//hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

					
		//m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MAXANISOTROPY,16);
		hr = m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		hr = m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		// 물거품 들어갈때 들어갈 코드 (물거품은 플렌 전체에 자글자글 하게 넣어주어야 할듯) 
		/*hr = m_Device->SetTexture(1,m_WaveTex[1].GetTexture());
		
		hr = m_Device->SetTextureStageState(1,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		hr = m_Device->SetTextureStageState(1,D3DTSS_MINFILTER,D3DTEXF_LINEAR);

		hr = m_Device->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		hr = m_Device->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
		
		hr = m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
		hr = m_Device->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_DISABLE);
	
		hr = m_Device->SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		hr = m_Device->SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_CURRENT);		
		hr = m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		hr = m_Device->SetTextureStageState(2,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		*/
	/*	for(int i=0;i<m_LayerNum;i++) {
		//	if(i >=7)
		//		continue;
		//	if(i >= m_DownLayerNum)
		//		continue;

			hr = m_Device->SetStreamSource(0,m_Bubble[i],sizeof(CWaveVertex));
			hr = m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
			//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,m_WaveNum - 1);
			hr = m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,((m_WaveNum * 2)-3) + 1);
		}*/
		hr = m_Device->SetTexture(0,m_WaveTex[0].GetTexture());
		
		
		hr = m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		hr = m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		//hr = m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		hr = m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		hr = m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

					
		//m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MAXANISOTROPY,16);
		hr = m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		hr = m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		
		for(int i=0;i<m_LayerNum;i++) {
		//	if(i >=7)
		//		continue;
		//	if(i >= m_DownLayerNum)
		//		continue;

			hr = m_Device->SetStreamSource(0,m_Vert[i],sizeof(CWaveVertex));
			hr = m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
			//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,m_WaveNum - 1);
			hr = m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,((m_WaveNum * 2)-3) + 1);
		}
		

		/////////////////////////////////////////////////plan/////
/*		hr = m_Device->SetTexture(0,m_WaveTex[1].GetTexture());
	
		m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		//m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_ANISOTROPIC);
		//m_Device->SetTextureStageState(0,D3DTSS_MAXANISOTROPY,16);

			
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		//m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);//D3DTOP_MODULATE);

		m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		m_Device->SetStreamSource(0,m_Vert[m_LayerNum],sizeof(CWaveVertex));
		m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
		//m_Device->DrawPrimitive(D3DPT_LINESTRIP,0,m_WaveNum - 1);
		hr = m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);	
	*/	/////////////////////////////////////////////////////////////////////
	
	//	m_Device->EndScene();
		if(m_Pick)
			m_Device->SetRenderState(D3DRS_FILLMODE,  D3DFILL_SOLID);
		m_Device->SetRenderState(D3DRS_CULLMODE,cullmode);
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zwritemode);
		hr = m_Device->SetRenderState(D3DRS_SRCBLEND,alpha1);
		hr = m_Device->SetRenderState(D3DRS_DESTBLEND,alpha2);
		hr = m_Device->SetRenderState( D3DRS_LIGHTING,light);
		
}
void CWaveLine::ResetLayer(int index,int type) {
	//delete[] (m_Wave[index]);
	
//	m_Wave.erase(&(m_Wave[index]));
//	CWavePoint *tmp;
//	tmp = new CWavePoint;
//	m_Wave.push_back(tmp);
//	int test = m_Wave.size();
//	CreateInterpolation(m_Wave.size() - 1,type);
	CreateInterpolation(index,type);

}
void CWaveLine::MoveVertex(CWaveLine::CWavePoint *point) {
	int i;


	for(i=0;i<m_WaveNum * 2; i++) {
		if(point[i].m_WaveValue == WAVEFWDMOVE) {
			point[i].m_Pos -= ((point[i].m_FwdVec) * (point[i].m_Speed) * 6.2f);
		
		}
		else if(point[i].m_WaveValue == WAVEBACKMOVE)
			point[i].m_Pos -= ((point[i].m_FwdVec) * (point[i].m_Speed) * 1.0f);

		point[i].m_Speed += point[i].m_Aspeed;
		
		if(!(i %2)){ // 앞의 점들 
			point[i].m_lie = Test_Vector(point[i],point[i].m_WaveValue);

			if(point[i].m_lie) {
			
					point[i].m_Pos -= (point[i].m_FwdVec)* (6.0f);
					
					if(point[i].m_UnitAlpha <0.0f)
						point[i].m_Alpha += point[i].m_UnitAlpha * 5.0f;
					else  {
						point[i].m_UnitAlpha *= (-1.0f);
						point[i].m_Alpha += point[i].m_UnitAlpha * 5.0f;
					}
					// 주위의 점들도 앞으로 조금식 당긴다
					if((i-2)>=0) {
						point[i-2].m_Pos -= (point[i-2].m_FwdVec)* (3.5f);
						
						if(point[i-2].m_UnitAlpha <0.0f)
							point[i-2].m_Alpha += point[i-2].m_UnitAlpha * 2.5f;
						else  {
							point[i-2].m_UnitAlpha *= (-1.0f);
							point[i-2].m_Alpha += point[i-2].m_UnitAlpha * 2.5f;
						}
					}
					//
					if((i+2)<=((m_WaveNum *2) -1)) {
						point[i+2].m_Pos -= (point[i+2].m_FwdVec)* (3.5f);
						
						if(point[i+2].m_UnitAlpha <0.0f)
							point[i+2].m_Alpha += point[i+2].m_UnitAlpha * 2.5f;
						else  {
							point[i+2].m_UnitAlpha *= (-1.0f);
							point[i+2].m_Alpha += point[i+2].m_UnitAlpha * 2.5f;
						}

					}
			
				//


			}
		}
		else {
			//point[i].m_Alpha  = 0.0f;
		}

	}

}
void CWaveLine::Update() {
	int i,j;
	int loop;

	int f_count = 0;
	int b_count = 0;
	unsigned long frameskip = 0;

	m_CurrentTime = timeGetTime();
	if(m_CurrentTime - m_BeforeTime >= 30) {
		frameskip = (m_CurrentTime - m_BeforeTime) / 30;
		tcount+= frameskip;
		m_BeforeTime = m_CurrentTime;
		for(loop = 0;loop <(int)frameskip;loop++) {
			int reset_index = -1;

			/// 1을 k로 		
			//for(j=0;j<kcount;j++) {
			for(j=0;j<m_LayerNum;j++) {
				MoveVertex(m_Wave[j]);
				f_count = 0;
				b_count = 0;
				for(i=0;i<m_WaveNum * 2;i++) {
				
				
					if((m_Wave[j][i].m_WaveValue) == WAVEFWDMOVE) {
						if(m_Wave[j][i].m_UnitAlpha > 0.0f) 
							m_Wave[j][i].m_Alpha +=m_Wave[j][i].m_UnitAlpha*1.5f;
						if(m_Wave[j][i].m_Alpha >= 0.9f) {
							m_Wave[j][i].m_Alpha = 0.9f;
							m_Wave[j][i].m_UnitAlpha *= (-1.0f);
						}
						else if(m_Wave[j][i].m_Alpha <= 0.0f) {
					
							f_count++;
							if(m_Wave[j][i].m_Alpha <= 0.0f)
								m_Wave[j][i].m_Alpha = 0.0f;
							
						}
					}//if
					else {
						
						m_Wave[j][i].m_Alpha +=(m_Wave[j][i].m_UnitAlpha * 1.2f);
						if(m_Wave[j][i].m_Alpha >= 0.3f) {
							m_Wave[j][i].m_Alpha = 0.3f;
							m_Wave[j][i].m_UnitAlpha *= (-1.0f);
						}
						else if(m_Wave[j][i].m_Alpha <= 0.0f) {
							b_count++;
							if(m_Wave[j][i].m_Alpha <= 0.0f)
								m_Wave[j][i].m_Alpha = 0.0f;
							
						}
					}//else
					
				} // for
			
				if(f_count >=(m_WaveNum)) {
					reset_index = j;
					//ResetLayer(j,WAVEFWDMOVE);
				}
				//if(b_count == (m_WaveNum * 2)) {
				if(b_count == (m_WaveNum)) {
					reset_index = j;
					//ResetLayer(j,WAVEBACKMOVE);
				}
					

			}
			for(i=0;i<kcount;i++) {
			//for(i=0;i<m_LayerNum;i++) {		
				if(m_Wave[i][0].m_WaveValue == WAVEFWDMOVE) {
					SetVertexBuffer(i,WAVEFWDMOVE);
					//SetBubbleBuffer(i,WAVEFWDMOVE);
				}
				else { 
					SetVertexBuffer(i,WAVEBACKMOVE);
					//SetBubbleBuffer(i,WAVEBACKMOVE);

				}
			}
			
			
			//SetVertexBuffer(m_LayerNum,WAVEPLANE2);
	
			if(reset_index != -1) {
				ResetLayer(reset_index,m_Wave[reset_index][0].m_WaveValue);
			}	
		}//for(loop = 0;loop <(m_CurrentTime - m_BeforeTime) / 40.0f;loop++)
		
	}// if
	if((tcount > (m_CreateFrame))) {
		kcount++;
		//tcount++;
		tcount = 0;
	}
	if(kcount > m_LayerNum) {
		kcount = kcount -1;
	}
	
}
void CWaveLine::CreateVertexBuffer() {
	int i;
	CWaveVertex *tmp;
	//CWaveVertex *tmp2;

	m_Vert = new LPDIRECT3DVERTEXBUFFER8[m_LayerNum];
	//m_Bubble = new LPDIRECT3DVERTEXBUFFER8[m_LayerNum];

	for(i=0;i<m_LayerNum;i++) {
	
		m_Device->CreateVertexBuffer(m_WaveNum * 2 * sizeof(CWaveVertex),0,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_MANAGED,&(m_Vert[i]));
		/*m_Device->CreateVertexBuffer(m_WaveNum * 2 * sizeof(CWaveVertex),0,
							    D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_MANAGED,&(m_Bubble[i]));
		*/
		m_WaveLayer.push_back(tmp);
		//m_WaveBubble.push_back(tmp2);

		m_WaveLayer[i] = new CWaveVertex[m_WaveNum * 2];
		//m_WaveBubble[i] = new CWaveVertex[m_WaveNum * 2];

		if(i < m_DownLayerNum) {
			SetVertexBuffer(i,WAVEFWDMOVE);
		//	SetBubbleBuffer(i,WAVEFWDMOVE);
		}
		else{
			SetVertexBuffer(i,WAVEBACKMOVE);
		//	SetBubbleBuffer(i,WAVEBACKMOVE);
		}
	}
	// 배경 plane
/*	m_Device->CreateVertexBuffer(m_WaveNum * 2 * sizeof(CWaveVertex),0,
							D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_MANAGED,&(m_Vert[m_LayerNum]));
		
	m_WaveLayer.push_back(tmp);
	m_WaveLayer[m_LayerNum] = new CWaveVertex[4];
	SetVertexBuffer(m_LayerNum,WAVEPLANE);
*/	
}
bool CWaveLine::Test_Vector(CWaveLine::CWavePoint point,int type) {
	if(point.m_lie) {
		return true;
	}
	else {
		float length;	
		float tmp_length;

		int index = 0;
		D3DXVECTOR3 tmp_point;
		tmp_point = m_WaveList[0].m_Pos - point.m_Pos;
		length = D3DXVec3Length(&tmp_point);
		
		for(int i=0;i<m_PointNum;i++) {
			tmp_point = m_WaveList[i].m_Pos - point.m_Pos;
			tmp_length = D3DXVec3Length(&tmp_point);
			
			if(tmp_length < length) {
				length = tmp_length;
				index = i;
			}
				
		
		}

		D3DXVECTOR3 tmp_vec = m_WaveList[index].m_Pos - point.m_Pos;
		length = D3DXVec3Length(&tmp_vec);
		if(type == WAVEFWDMOVE) {
			if(length <((-WAVEFWDLENGTH) - 550.0f))
				return true;
		}
		else if(type == WAVEBACKMOVE) {
				return false;
		}
	}
	return false;
}
void CWaveLine::SetBubbleBuffer(int i,int type) {
	
	CWavePoint *t_point;
	t_point = new CWavePoint[m_WaveNum * 2];

	
	memcpy(t_point,m_Wave[i],sizeof(CWavePoint) * m_WaveNum * 2);

//	SetVertexInfo(t_point);
	for(int j=0;j<m_WaveNum * 2;j++) {
				
		m_WaveBubble[i][j].x = t_point[j].m_Pos.x;
		m_WaveBubble[i][j].y = t_point[j].m_Pos.y;
		m_WaveBubble[i][j].z = t_point[j].m_Pos.z;
	
		switch(j % 4) { // texcoord 설정
		
			case 0:
				m_WaveBubble[i][j].s = 0.0f ;
				m_WaveBubble[i][j].t = 0.0f ;
				break;
			case 1:
				m_WaveBubble[i][j].s = 1.0f ;
				m_WaveBubble[i][j].t = 0.0f ;
				break;
			case 2:
				m_WaveBubble[i][j].s = 0.0f ;
				m_WaveBubble[i][j].t = 1.0f ;
				break;
			case 3:
				m_WaveBubble[i][j].s = 1.0f ;
				m_WaveBubble[i][j].t = 1.0f ;
				break;

	
		}
		//alpha setting 
		m_WaveBubble[i][j].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,(t_point[j].m_Alpha / 2.5f));
	
		if((j % 2)!=0) {
				m_WaveBubble[i][j].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,0.0f);
		}
	}
	m_WaveBubble[i][0].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
	m_WaveBubble[i][1].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
//	m_WaveBubble[i][2].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
//	m_WaveBubble[i][3].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
	m_WaveBubble[i][(m_WaveNum * 2)-2].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
	m_WaveBubble[i][(m_WaveNum * 2)-1].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
//	m_WaveBubble[i][(m_WaveNum * 2)-4].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
//	m_WaveBubble[i][(m_WaveNum * 2)-3].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);

	VOID *tmp_buffer;
	m_Bubble[i]->Lock(0,m_WaveNum * sizeof(CWaveVertex) * 2,(BYTE **)&tmp_buffer,0);
	
	memcpy(tmp_buffer,m_WaveBubble[i],2 * m_WaveNum * sizeof(CWaveVertex));

	m_Bubble[i]->Unlock();

	delete[] t_point;
}
void CWaveLine::SetVertexBuffer(int i,int type) {
	
	CWavePoint *t_point;
	t_point = new CWavePoint[m_WaveNum * 2];

	if(type == WAVEPLANE) {
		D3DXVECTOR3 plane_length;
		float plane_width;
		m_WaveLayer[i][0].x = m_Wave[0][0].m_Pos.x+(m_Wave[0][0].m_FwdVec.x) * (m_Length *2.0f);
		m_WaveLayer[i][0].y = m_Wave[0][0].m_Pos.y+(m_Wave[0][0].m_FwdVec.y) * (m_Length *2.0f);
		m_WaveLayer[i][0].z = m_Wave[0][0].m_Pos.z+(m_Wave[0][0].m_FwdVec.z) * (m_Length *2.0f);
		m_WaveLayer[i][0].s = 0.0f ;
		m_WaveLayer[i][0].t = t_value1;
		m_WaveLayer[i][0].color = D3DCOLOR_ARGB(120,255,255,255);

		m_WaveLayer[i][1].x = m_Wave[0][1].m_Pos.x -(m_Wave[0][0].m_FwdVec.x) * m_Length;
		m_WaveLayer[i][1].y = m_Wave[0][1].m_Pos.y -(m_Wave[0][0].m_FwdVec.y) * m_Length;
		m_WaveLayer[i][1].z = m_Wave[0][1].m_Pos.z -(m_Wave[0][0].m_FwdVec.z) * m_Length;
		m_WaveLayer[i][1].s = t_value2 ;
		m_WaveLayer[i][1].t = t_value1;
		m_WaveLayer[i][1].color = D3DCOLOR_ARGB(120,255,255,255);

		m_WaveLayer[i][2].x = m_Wave[0][m_WaveNum*2 - 2].m_Pos.x+(m_Wave[0][m_WaveNum *2 -2].m_FwdVec.x) * (m_Length *2.0f);
		m_WaveLayer[i][2].y = m_Wave[0][m_WaveNum*2 - 2].m_Pos.y+(m_Wave[0][m_WaveNum *2- 2].m_FwdVec.y) * (m_Length *2.0f);
		m_WaveLayer[i][2].z = m_Wave[0][m_WaveNum*2 - 2].m_Pos.z+(m_Wave[0][m_WaveNum *2- 2].m_FwdVec.z) * (m_Length *2.0f);
		m_WaveLayer[i][2].s = 0.0f ;
		m_WaveLayer[i][2].t = t_value2;
		m_WaveLayer[i][2].color = D3DCOLOR_ARGB(120,255,255,255);
		
		m_WaveLayer[i][3].x = m_Wave[0][m_WaveNum*2 - 1].m_Pos.x - (m_Wave[0][m_WaveNum*2- 2].m_FwdVec.x) * m_Length;
		m_WaveLayer[i][3].y = m_Wave[0][m_WaveNum*2 - 1].m_Pos.y - (m_Wave[0][m_WaveNum*2- 2].m_FwdVec.y) * m_Length;
		m_WaveLayer[i][3].z = m_Wave[0][m_WaveNum*2 - 1].m_Pos.z - (m_Wave[0][m_WaveNum*2- 2].m_FwdVec.z) * m_Length;
		m_WaveLayer[i][3].s = t_value2;
		m_WaveLayer[i][3].t = t_value2;
		m_WaveLayer[i][3].color = D3DCOLOR_ARGB(120,255,255,255);

		plane_length.x = m_WaveLayer[i][2].x - m_WaveLayer[i][0].x;
		plane_length.y = m_WaveLayer[i][2].y - m_WaveLayer[i][0].y;
		plane_length.z = m_WaveLayer[i][2].z - m_WaveLayer[i][0].z;
		plane_width = D3DXVec3Length(&plane_length);
		//tiling 해주는 부분 
		t_value2 += plane_width / PLANEWRAP;
		t_value3 = t_value2;
		m_WaveLayer[i][2].t = t_value3;
		m_WaveLayer[i][3].t = t_value3;
		

	}
	else if(type == WAVEPLANE2) {
		m_WaveLayer[i][0].s = t_value1;
		m_WaveLayer[i][0].t = 0.0f ;
		m_WaveLayer[i][0].color = D3DCOLOR_ARGB(120,255,255,255);

		m_WaveLayer[i][1].s = t_value2;
		m_WaveLayer[i][1].t = 0.0f;
		m_WaveLayer[i][1].color = D3DCOLOR_ARGB(120,255,255,255);

		m_WaveLayer[i][2].s = t_value1;
		m_WaveLayer[i][2].t = t_value3;
		m_WaveLayer[i][2].color = D3DCOLOR_ARGB(120,255,255,255);
		
		m_WaveLayer[i][3].s = t_value2 ;
		m_WaveLayer[i][3].t = t_value3;
		m_WaveLayer[i][3].color = D3DCOLOR_ARGB(120,255,255,255);
		
		t_value1 +=0.01f;
		t_value2 +=0.01f;
		/*if(t_value1 >1.0f)
			t_value1 -=1.0f;
		if(t_value2 >1.0f)
			t_value2 -=1.0f;
*/

	}
	else {
		if(type == WAVEBACKMOVE) 
			printf("");

		memcpy(t_point,m_Wave[i],sizeof(CWavePoint) * m_WaveNum * 2);

	//	SetVertexInfo(t_point);
		for(int j=0;j<m_WaveNum * 2;j++) {
					
			m_WaveLayer[i][j].x = t_point[j].m_Pos.x;
			m_WaveLayer[i][j].y = t_point[j].m_Pos.y;
			m_WaveLayer[i][j].z = t_point[j].m_Pos.z;
		
			switch(j % 4) { // texcoord 설정
			
				case 0:
					m_WaveLayer[i][j].s = 0.0f ;
					m_WaveLayer[i][j].t = 0.0f ;
					break;
				case 1:
					m_WaveLayer[i][j].s = 1.0f ;
					m_WaveLayer[i][j].t = 0.0f ;
					break;
				case 2:
					m_WaveLayer[i][j].s = 0.0f ;
					m_WaveLayer[i][j].t = 1.0f ;
					break;
				case 3:
					m_WaveLayer[i][j].s = 1.0f ;
					m_WaveLayer[i][j].t = 1.0f ;
					break;

		
			}
			m_WaveLayer[i][j].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,t_point[j].m_Alpha);
			if((j % 2)!=0) {
				float t_alpha = ((float)t_point[j].m_Alpha / 2.0f);
				if(t_alpha >= 0.0f)
					m_WaveLayer[i][j].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,t_alpha);
				else 
					m_WaveLayer[i][j].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,0.0f);
			}
		}
		m_WaveLayer[i][0].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
		m_WaveLayer[i][1].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
	//	m_WaveLayer[i][2].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
	//	m_WaveLayer[i][3].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
		m_WaveLayer[i][(m_WaveNum * 2)-2].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
		m_WaveLayer[i][(m_WaveNum * 2)-1].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.0f);
	//	m_WaveLayer[i][(m_WaveNum * 2)-4].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);
	//	m_WaveLayer[i][(m_WaveNum * 2)-3].color = D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,0.2f);

	}
	if(type == WAVEPLANE || type == WAVEPLANE2) {
		VOID *tmp_buffer;
		m_Vert[i]->Lock(0,sizeof(CWaveVertex) * 4,(BYTE **)&tmp_buffer,0);
		
		memcpy(tmp_buffer,m_WaveLayer[i],4 * sizeof(CWaveVertex));

		m_Vert[i]->Unlock();

	}
	else {
		VOID *tmp_buffer;
		m_Vert[i]->Lock(0,m_WaveNum * sizeof(CWaveVertex) * 2,(BYTE **)&tmp_buffer,0);
		
		memcpy(tmp_buffer,m_WaveLayer[i],2 * m_WaveNum * sizeof(CWaveVertex));

		m_Vert[i]->Unlock();
	}
	delete[] t_point;
}
void CWaveLine::StoreList() {
	int i;
	CWavePoint *tmp_layer = NULL;
	if(m_BackupList)
		delete[] m_BackupList;
	m_BackupList = new CWavePoint[m_PointNum];

	for(i=0;i<m_PointNum;i++) {
		memcpy(&(m_BackupList[i]),&(m_WaveList[i]),sizeof(CWavePoint));
	}
	for(i=0;i<m_LayerNum;i++) {
		m_Wave.push_back(tmp_layer);
	}

}
void CWaveLine::LoadList() {
	if(m_PointNum)
		m_WaveList.clear();
	for(int i=0;i<m_PointNum;i++) {
		m_WaveList.push_back(m_BackupList[i]);	
	}

}
void CWaveLine::CreateInterpolation(int index,int type) {
	
	int i;
	int count = 0;
	
	CWavePoint *tmp_point = NULL;
	
	// 기준 point 사이의 보간점 갯수 세기
	int k = (int)(1.0f/m_UnitInterpol);
	k -= 1;
	// 원본 리스트 Load
	LoadList();
	
	// 보간점 포함된 wave point 버퍼 생성
	m_WaveNum = k * (m_PointNum - 1) + m_PointNum;
	//m_Wave.push_back(tmp_layer);
	if(m_Wave[index] != NULL)
		delete[]  m_Wave[index];

	m_Wave[index] = new CWavePoint[m_WaveNum];

	tmp_point = new CWavePoint[m_WaveNum];
	
	// Wave point 에서 가장 먼 점 setting
	m_WaveCenter = m_WaveList[0].m_Pos;

	if(type == WAVEFWDMOVE) {
		for(i=0;i<m_PointNum;i++) {	
			// pos 흔들기	
			D3DXVECTOR3 tmp;
			tmp.x = (float)(rand() %(int)((m_ShakeValue.x) * 1000)) / 1000.0f;
			tmp.z = (float)(rand() %(int)((m_ShakeValue.z) * 1000)) / 1000.0f;
				
			if(rand() % 2)
				tmp.x *= -1.0f;
			if(rand() % 2)
				tmp.z *= -1.0f;
				//10
			m_WaveList[i].m_Pos.x += tmp.x * 20.0f;
			m_WaveList[i].m_Pos.z += tmp.z * 20.0f;
		
			
		}

		for(i=0;i<m_PointNum;i++) {
			

			// catmull rom 보간 
			D3DXVECTOR3 p1;
			D3DXVECTOR3 p2;
			D3DXVECTOR3 p3;
			D3DXVECTOR3 p4;
							
			if(i==0) {
				p1 = m_WaveList[i].m_Pos;
				p2 = m_WaveList[i].m_Pos;
			}
			else {
				p1 = m_WaveList[i-1].m_Pos;
				p2 = m_WaveList[i].m_Pos;
			}
			if((i+1) >= (m_PointNum-1)) {	
				p3 = m_WaveList[i+1].m_Pos;
				p4 = m_WaveList[i+1].m_Pos;
			}
			else {
				p3 = m_WaveList[i+1].m_Pos;	
				p4 = m_WaveList[i+2].m_Pos;
			}
		
			m_Wave[index][count].m_Pos = p2;
			
			
			m_Wave[index][count].m_PointIndex = count++;
			// 마지막 점 일 경우는 넣지 않는다
			if(i != (m_PointNum -1 )) {
				// Catmull rom spline
				for(int j= 1;j<=k;j++) {
					float t = m_UnitInterpol * j;
					float t2 = t * t;
					float t3 = t*t*t;

					m_Wave[index][count].m_Pos = p1 * ((-0.5f)*t3 + t2 + (-0.5f)*t) +
										p2 * ((1.5f)*t3 + (-2.5f)*t2 + 1.0f) +
										p3 * ((-1.5f)*t3 + (2.0f)*t2 + (0.5f)*t) +
										p4 * ((0.5f)*t3 + (-0.5f)*t2);
				///// 점 흔들기
					D3DXVECTOR3 tmp;
					tmp.x = (float)(rand() %(int)((m_ShakeValue.x) * 1000)) / 1000.0f;
					tmp.z = (float)(rand() %(int)((m_ShakeValue.z) * 1000)) / 1000.0f;
							
					if(rand() % 2)
						tmp.x *= -1.0f;
					if(rand() % 2)
						tmp.z *= -1.0f;
							
					m_Wave[index][count].m_Pos.x += tmp.x * 2.0f;
					m_Wave[index][count].m_Pos.z += tmp.z * 2.0f;
				////
					m_Wave[index][count].m_PointIndex = count++;
				}
			}
			
		}
		
		//fwd vector get
		D3DXVECTOR3 right;
		D3DXVECTOR3 tmp = m_Wave[index][m_WaveNum - 1].m_Pos - m_Wave[index][0].m_Pos;
		D3DXVec3Normalize(&right,&tmp);
		
		float wavefwdlength = WAVEFWDLENGTH;
		float sub_tmp = 0.0f;
			
		// 거리 렌덤 세팅
		sub_tmp = (float)(rand() % (int)(wavefwdlength / 2));
		if(rand() %2) {
			sub_tmp *= -1.0f;
		}
		wavefwdlength += sub_tmp;

		for(i=0;i<m_WaveNum;i++) {
			
			// fwd vector get
			D3DXVec3Cross(&(m_Wave[index][i].m_FwdVec),&right,&m_Normal);
			D3DXVec3Normalize(&(m_Wave[index][i].m_FwdVec),&(m_Wave[index][i].m_FwdVec));
			
			m_Wave[index][i].m_Pos.x += (-m_Wave[index][i].m_FwdVec.x) * wavefwdlength;
			//m_Wave[index][i].m_Pos.y += (m_Wave[index][i].m_FwdVec.y) * BACKLENGTH;
			m_Wave[index][i].m_Pos.z += (-m_Wave[index][i].m_FwdVec.z) * wavefwdlength;
			
			// other value seting
			//SetWavePointSetting(&(m_Wave[index][i]));
			
		}

		memcpy(tmp_point,m_Wave[index],sizeof(CWavePoint) * m_WaveNum);
		delete[] m_Wave[index];
		m_Wave[index] = new CWavePoint[m_WaveNum * 2];
	
		float t_ual;
		
		for(i=0;i<m_WaveNum * 2; i++) {
			m_Wave[index][i].m_Alpha = 0.0f;
			m_Wave[index][i].m_WaveValue = type;

			//unit alpha setting
			if(((m_WaveNum * 2) / WAVESLIP) > 0) {
				if(!(i % ((m_WaveNum * 2) / WAVESLIP))) {
				
					t_ual = (rand() % 1000) / 100000.0f;
					if(t_ual == 0.0f) {
						t_ual = 0.0001f;	
					}
			
				}
			}
			if(!(i % 2))
				m_Wave[index][i] = tmp_point[(i/2)];
			else {
				m_Wave[index][i] = tmp_point[(i-1)/2]; 
				m_Wave[index][i].m_Pos.x +=  (((tmp_point[(i-1)/2].m_FwdVec.x)) * m_Length / 1.3f);
			//	m_Wave[index][i].m_Pos.y +=  ((-(tmp_point[(i-1)/2].m_FwdVec.y)) * m_Length);
				m_Wave[index][i].m_Pos.z +=  (((tmp_point[(i-1)/2].m_FwdVec.z)) * m_Length /1.3f);
			}	
			m_Wave[index][i].m_UnitAlpha = t_ual;
		}
	


	}// if
	else if(type == WAVEBACKMOVE) {
		for(i=0;i<m_PointNum;i++) {	
			// pos 흔들기	
			D3DXVECTOR3 tmp;
			tmp.x = (float)(rand() %(int)((m_ShakeValue.x) * 1000)) / 1000.0f;
			tmp.z = (float)(rand() %(int)((m_ShakeValue.z) * 1000)) / 1000.0f;
				
			if(rand() % 2)
				tmp.x *= -1.0f;
			if(rand() % 2)
				tmp.z *= -1.0f;
				
			m_WaveList[i].m_Pos.x += tmp.x * 10.0f;
			m_WaveList[i].m_Pos.z += tmp.z * 10.0f;
		
			
		}

		for(i=0;i<m_PointNum;i++) {
		

			// catmull rom 보간 
			D3DXVECTOR3 p1;
			D3DXVECTOR3 p2;
			D3DXVECTOR3 p3;
			D3DXVECTOR3 p4;
							
			if(i==0) {
				p1 = m_WaveList[i].m_Pos;
				p2 = m_WaveList[i].m_Pos;
			}
			else {
				p1 = m_WaveList[i-1].m_Pos;
				p2 = m_WaveList[i].m_Pos;
			}
			if((i+1) >= (m_PointNum-1)) {	
				p3 = m_WaveList[i+1].m_Pos;
				p4 = m_WaveList[i+1].m_Pos;
			}
			else {
				p3 = m_WaveList[i+1].m_Pos;	
				p4 = m_WaveList[i+2].m_Pos;
			}
		
			m_Wave[index][count].m_Pos = p2;
		
			m_Wave[index][count].m_PointIndex = count++;
			// 마지막 점 일 경우는 넣지 않는다
			if(i != (m_PointNum -1 )) {
				// Catmull rom spline
				for(int j= 1;j<=k;j++) {
					float t = m_UnitInterpol * j;
					float t2 = t * t;
					float t3 = t*t*t;

					m_Wave[index][count].m_Pos = p1 * ((-0.5f)*t3 + t2 + (-0.5f)*t) +
										p2 * ((1.5f)*t3 + (-2.5f)*t2 + 1.0f) +
										p3 * ((-1.5f)*t3 + (2.0f)*t2 + (0.5f)*t) +
										p4 * ((0.5f)*t3 + (-0.5f)*t2);
					m_Wave[index][count].m_PointIndex = count++;
				}
			}
			
		}

		//fwd vector get
		D3DXVECTOR3 right;
		D3DXVECTOR3 tmp = m_Wave[index][m_WaveNum - 1].m_Pos - m_Wave[index][0].m_Pos;
		D3DXVec3Normalize(&right,&tmp);
			
		for(i=0;i<m_WaveNum;i++) {

			// fwd vector get
			D3DXVec3Cross(&(m_Wave[index][i].m_FwdVec),&right,&m_Normal);
			D3DXVec3Normalize(&(m_Wave[index][i].m_FwdVec),&(m_Wave[index][i].m_FwdVec));

			m_Wave[index][i].m_Pos.x += (m_Wave[index][i].m_FwdVec.x) * WAVEBACKLENGTH;
			//m_Wave[index][i].m_Pos.y += (m_Wave[index][i].m_FwdVec.y) * BACKLENGTH;
			m_Wave[index][i].m_Pos.z += (m_Wave[index][i].m_FwdVec.z) * WAVEBACKLENGTH;
			
			// other value seting
			//SetWavePointSetting(&(m_Wave[index][i]));
			
		}

		memcpy(tmp_point,m_Wave[index],sizeof(CWavePoint) * m_WaveNum);
		delete[] m_Wave[index];
		m_Wave[index] = new CWavePoint[m_WaveNum * 2];
		float t_ual;
		
		for(i=0;i<m_WaveNum * 2; i++) {
			
			m_Wave[index][i].m_Alpha = 0.0f;
			//unit alpha setting
			if(((m_WaveNum * 2) / WAVESLIP) >0) {
				if(!(i % ((m_WaveNum * 2) / WAVESLIP))) {
				
					t_ual = (float)(rand() % 1000) / 100000.0f;
					if(t_ual == 0.0f)
						t_ual += 0.002f;
						
				}
			}
			
			if(!(i % 2))
				m_Wave[index][i] = tmp_point[(i/2)];
			else {
				m_Wave[index][i] = tmp_point[(i-1)/2]; 
				m_Wave[index][i].m_Pos.x -=  (((tmp_point[(i-1)/2].m_FwdVec.x)) * (m_Length/2.0f));
			//	m_Wave[index][i].m_Pos.y +=  (((tmp_point[(i-1)/2].m_FwdVec.y)) * m_Length);
				m_Wave[index][i].m_Pos.z -=  (((tmp_point[(i-1)/2].m_FwdVec.z)) * (m_Length/2.0f));
			}	
			m_Wave[index][i].m_UnitAlpha = t_ual;
		}
		for(i=0;i<m_WaveNum * 2;i++) {
			m_Wave[index][i].m_WaveValue = type;
			m_Wave[index][i].m_FwdVec *=(-1.0f);
		}
		
	}// else if
	if(tmp_point) {
		delete[] tmp_point;
	
	}
}
void CWaveLine::SetWavePointSetting(CWaveLine::CWavePoint *pt) {
	pt->m_UnitAlpha = (rand() % 100000) / 1000000.0f + 0.05f;

}

