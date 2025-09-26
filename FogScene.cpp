#include "FogScene.h"
#include "GMMemory.h"



CFogScene ::CFogScene() {
	m_bPick = false;

	srand(time(0));
	m_ParticleNum = INIT_NUM;
	m_ClosePointNum = 0;

	

	m_FogBuffer = NULL;
	m_FogPointSpriteBuffer = NULL;
	m_BaseTexture = NULL;
	m_LodNum = 0;
	//Lod_Level = NEAR;
	m_SpeedPower = 0.0f;
	m_CenterPoint.x = m_CenterPoint.y = m_CenterPoint.z = 0.0f;
	m_Impress.x = m_Impress.y = m_Impress.z = 0.0f;
	m_Center = 0.0f;
	
	m_Size = 1.0f;

	m_SrcBlendMode = D3DBLEND_SRCALPHA;
	
	m_DstBlendMode = D3DBLEND_INVSRCALPHA;

	m_TextureNum = 0;
	m_Rad = 5.0f;

	m_FadeIn = 1.0f;
	m_FadeOut = 0.0f;
	m_Lod = NEAR_LOD;

	m_Device = CSceneManager::GetDevice();
	m_FadeInSpeed = 1.0f;
	m_pPoint = NULL;
	m_BackFadeIn = 1.0f;

}
CFogScene ::CFogScene(int num) {
	m_BackFadeIn = 1.0f;
	m_pPoint = NULL;
	m_bPick = false;
	m_ParticleNum = num;
	m_ClosePointNum = 0;
	m_FogBuffer = NULL;
	m_FogPointSpriteBuffer = NULL;
	m_BaseTexture = NULL;
	m_LodNum = 0;

	//Lod_Level = NEAR;
	m_SpeedPower = 0.0f;
	m_Rad = 5.0f;
	m_CenterPoint.x = m_CenterPoint.y = m_CenterPoint.z = 0.0f;
	m_Impress.x = m_Impress.y = m_Impress.z = 0.0f;
	m_Center = 0.0f;
	
	m_Size = 1.0f;
	
	m_SrcBlendMode = D3DBLEND_SRCALPHA;
	
	m_DstBlendMode = D3DBLEND_INVSRCALPHA;

	m_Lod = NEAR_LOD;
	m_TextureNum = 0;

	m_FadeIn = 1.0f;
	m_FadeOut = 0.0f;
	
	m_Device = CSceneManager::GetDevice();
}
void CFogScene::Create(vector3 cen) {
	m_Node = new ParticleNode[m_ParticleNum];
	
	CTexture::SetPath(NATURETEXTUREPATH);
	m_BaseTexture=new CTexture;
	//texture image load
	m_BaseTexture->Load(FOGTEX);	
	m_TextureNum++;
	//set fog parameter
	SetCenter(40000.0f);
	SetCenterPos(cen);
	
	SetFlow(cen.y - 200.0f);
	SetGravity(0.0f,0.0f,0.0f);
	vector3 tm;
	tm.z = 0.3f;
	tm.x = 0.6f;
	tm.y = 0.1f;
//	tm.x = tm.y = tm.z = 0.0f;
//	tm.x = 0.3f;
//	tm.y = 0.1f;
	SetSize(5000.0f);
	SetImpress(tm);
	SetPos();
	SetColor(0.8f,0.8f,0.8f,0.5f);
//	SetVertexColor(0.3f,0.3f,0.3f,0.5f,2);
//	SetVertexColor(0.3f,0.3f,0.3f,0.5f,3);
	
	SetEtc();
	SetRad(3000.0f);
	SetSpeed(0.0f,0.0f,0.0f);
	SetSpeedPower(0.0f);
	SetWind(0.0f,0.0f,0.0f);
	Setfadein(1.0f);
	SetFadeInSpeed(0.3f);	
	SetFadeOutSpeed(3.0f);
	CreateVertexBuffer();

}
/*각 vertex 색 달리 설정 
  고라운드 	
  0------1
  |		 |
  |      |
  3------2
 */
void CFogScene::SetVertexColor(float r,float g,float b,float a,int dimension) {
	int i;
	
	for(i=0;i<m_ParticleNum;i++){ 
		m_Node[i].r[dimension] = r;
		m_Node[i].g[dimension] = g;
		m_Node[i].b[dimension] = b;
		m_Node[i].Life = a;
	}
	
}

CFogScene::~CFogScene() {

	if(m_FogBuffer != NULL)
		m_FogBuffer->Release();
	
	if(m_FogPointSpriteBuffer != NULL) 
		m_FogPointSpriteBuffer->Release();

	if(m_Node != NULL) {
		delete[] m_Node;
		m_Node = NULL;	
	}
/*	if(m_pVertex != NULL) {
		delete[] m_pVertex;
		m_pVertex = NULL;
	}
	if(m_pPoint != NULL) {
		delete[] m_pPoint;
		m_pPoint = NULL;
	}*/
	if(m_BaseTexture != NULL) { 
		delete m_BaseTexture;
		m_BaseTexture = NULL;
	}
}

void CFogScene::SetWind(float x,float y,float z) {
	m_Wind[0] = x;
	m_Wind[1] = y;
	m_Wind[2] = z;
}
void CFogScene::SetFlow(float flow_height,int index) {
	m_Node[index].Flow = flow_height;
}
void CFogScene::SetFlow(float flow_height) {
	int i;
	for(i=0;i<m_ParticleNum;i++) 
		m_Node[i].Flow = flow_height;
}
void CFogScene::SetHeight(float height,int index) {
	m_Node[index].Y = height;
}
void CFogScene::SetHeight(float height) {
	int i;
	for(i=0;i<m_ParticleNum;i++)
		m_Node[i].Y = height;

}
void CFogScene::SetColor(float r,float g,float b,float a) {
	int i;
	for(i=0;i<m_ParticleNum;i++){ 
		m_Node[i].r[0] = m_Node[i].r[1] = m_Node[i].r[2] = m_Node[i].r[3] = r;
		m_Node[i].g[0] = m_Node[i].g[1] = m_Node[i].g[2] = m_Node[i].g[3] = g;
		m_Node[i].b[0] = m_Node[i].b[1] = m_Node[i].b[2] = m_Node[i].b[3] = b;
		m_Node[i].Life = a;
	}

}
void CFogScene::SetColor(float r,float g,float b,float a,int p_index) {
		m_Node[p_index].r[0] = m_Node[p_index].r[1] = m_Node[p_index].r[2] = m_Node[p_index].r[3] = r;
		m_Node[p_index].g[0] = m_Node[p_index].g[1] = m_Node[p_index].g[2] = m_Node[p_index].g[3] = g;
		m_Node[p_index].b[0] = m_Node[p_index].b[1] = m_Node[p_index].b[2] = m_Node[p_index].b[3] = b;	
		m_Node[p_index].Life = a;
}

// 중력
void CFogScene ::SetGravity(float x,float y,float z) {
	
	m_Gravity[0] = x;
	m_Gravity[1] = y;
	m_Gravity[2] = z;


}
void CFogScene::SetSpeedPower(float power) {
	m_SpeedPower = power;
}

void CFogScene::SetImpress(vector3 imp) {
	m_Impress = imp;
}
void CFogScene::SetImpress(float x,float y,float z) {
	m_Impress.x = x;
	m_Impress.y = y;
	m_Impress.z = z;
}

void CFogScene::SetCenterPos(vector3 center) {
	m_CenterPoint = center;
}
void CFogScene ::SetPos() {
	int i;
	for(i=0;i<m_ParticleNum;i++) {
		m_Node[i].X = m_CenterPoint.x + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * (m_Impress.x + 0.1f)));
		m_Node[i].Y = m_CenterPoint.y + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * (m_Impress.y + 0.1f)));+350.0f;
		m_Node[i].Z = m_CenterPoint.z + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * (m_Impress.z + 0.1f)));

		m_Node[i].BackX = m_Node[i].X;
		m_Node[i].BackY = m_Node[i].Y;
		m_Node[i].BackZ = m_Node[i].Z;
	}
}
void CFogScene ::SetPos(int node_num) {
	m_Node[node_num].X = m_CenterPoint.x + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * m_Impress.x));
	m_Node[node_num].Y = m_CenterPoint.y + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * m_Impress.y));
	m_Node[node_num].Z = m_CenterPoint.z + (((float)(rand()%1000) - 500.0f)/1000.0f * (m_Center * m_Impress.z));
	
	m_Node[node_num].BackX = m_Node[node_num].X;
	m_Node[node_num].BackY = m_Node[node_num].Y;
	m_Node[node_num].BackZ = m_Node[node_num].Z;

}
void CFogScene ::SetPos(int node_num,float x,float y,float z) {
	m_Node[node_num].X = x;
	m_Node[node_num].Y = y;
	m_Node[node_num].Z = z;
	
	m_Node[node_num].BackX = m_Node[node_num].X;
	m_Node[node_num].BackY = m_Node[node_num].Y;
	m_Node[node_num].BackZ = m_Node[node_num].Z;

}
void CFogScene ::SetCenterPos(float x,float y,float z) {
	m_CenterPoint.x = x;
	m_CenterPoint.y = y;
	m_CenterPoint.z = z;
	
}

void CFogScene::SetSpeed(float x,float y,float z) {
	int i;
	m_Speed[0] = x;
	m_Speed[1] = y;
	m_Speed[2] = z;
	
	for(i=0;i<m_ParticleNum;i++){ 
		m_Node[i].Xi = x;
		m_Node[i].Yi = y;
		m_Node[i].Zi = z;
	}
}

void CFogScene::SetSpeed(float x,float y,float z,int index) {
		m_Node[index].Xi = x;
		m_Node[index].Yi = y;
		m_Node[index].Zi = z;
}
void CFogScene::SetSpeed() {
	int i;
	for(i=0;i<m_ParticleNum;i++) {
		if(m_SpeedPower != 0.0f) {
			m_Node[i].Xi =((float)(rand() %(int)m_SpeedPower) -m_SpeedPower/2.0f) *0.001f;
			m_Node[i].Yi =((float)(rand() %(int)m_SpeedPower) -m_SpeedPower/2.0f) *0.001f;
			m_Node[i].Zi =((float)(rand() %(int)m_SpeedPower) -m_SpeedPower/2.0f) *0.001f;
		}
	}
}
// Particle Center 와의 집중도
void CFogScene ::SetCenter(float Cvalue) {
	m_Center = Cvalue;
}

void CFogScene::CreateVertexBuffer() {

//	int i;
//	int count = 0;


	m_Device->CreateVertexBuffer( m_ParticleNum * sizeof(POINTVERTEX),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		D3DFVF_POINTVERTEX, D3DPOOL_DEFAULT, &m_FogPointSpriteBuffer);
	
	m_Device->CreateVertexBuffer(m_ParticleNum*sizeof(ParticleVertex) * 6,0,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_MANAGED,&m_FogBuffer);

	/*
	vector3 right;
	vector3 up;
	D3DXMATRIX m_World;

	m_pVertex = new ParticleVertex[m_ParticleNum * 6];
	
	m_Device->GetTransform(D3DTS_VIEW,&m_World);
	
	right.x = m_World._11;
	right.y = m_World._21;
	right.z = m_World._31;
	
	up.x = m_World._12;
	up.y = m_World._22;
	up.z = m_World._32;

	if(m_Lod == NEAR_LOD) {
		for(i=0;i<m_ParticleNum;i++) {
			//0
			m_pVertex[count].x = m_Node[i].X + (-right.x + up.x)*(m_Size);
			m_pVertex[count].y = m_Node[i].Y + (-right.y + up.y)*(m_Size);
			m_pVertex[count].z = m_Node[i].Z + (-right.z + up.z)*(m_Size);
			m_pVertex[count].tx = 0.0f;
			m_pVertex[count].ty = 0.0f;
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[0],m_Node[i].g[0],m_Node[i].b[0],m_Node[i].Life);
			//1
			m_pVertex[count].x = m_Node[i].X + (right.x + up.x)*(m_Size);
			m_pVertex[count].y = m_Node[i].Y + (right.y + up.y)*(m_Size);
			m_pVertex[count].z = m_Node[i].Z + (right.z + up.z)*(m_Size);
			m_pVertex[count].tx = 1.0f;
			m_pVertex[count].ty = 0.0f;
		
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[1],m_Node[i].g[1],m_Node[i].b[1],m_Node[i].Life);

			//2
			m_pVertex[count].x = m_Node[i].X + (right.x - up.x)*(m_Size);
			m_pVertex[count].y = m_Node[i].Y + (right.y - up.y)*(m_Size);
			m_pVertex[count].z = m_Node[i].Z + (right.z - up.z)*(m_Size);
			m_pVertex[count].tx = 1.0f;
			m_pVertex[count].ty = 1.0f;
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[2],m_Node[i].g[2],m_Node[i].b[2],m_Node[i].Life);

			//0
			m_pVertex[count].x = m_Node[i].X + (-right.x + up.x)*(m_Size);
			m_pVertex[count].y = m_Node[i].Y + (-right.y + up.y)*(m_Size);
			m_pVertex[count].z = m_Node[i].Z + (-right.z + up.z)*(m_Size);
			m_pVertex[count].tx = 0.0f;
			m_pVertex[count].ty = 0.0f;
		
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[0],m_Node[i].g[0],m_Node[i].b[0],m_Node[i].Life);
		
			//2
			m_pVertex[count].x = m_Node[i].X + (right.x - up.x)*(m_Size);
			m_pVertex[count].y = m_Node[i].Y + (right.y - up.y)*(m_Size);
			m_pVertex[count].z = m_Node[i].Z + (right.z - up.z)*(m_Size);
			m_pVertex[count].tx = 1.0f;
			m_pVertex[count].ty = 1.0f;
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[2],m_Node[i].g[2],m_Node[i].b[2],m_Node[i].Life);

			//3
			m_pVertex[count].x = m_Node[i].X + (right.x + up.x)*(-m_Size);
			m_pVertex[count].y = m_Node[i].Y + (right.y + up.y)*(-m_Size);
			m_pVertex[count].z = m_Node[i].Z + (right.z + up.z)*(-m_Size);
			m_pVertex[count].tx = 0.0f;
			m_pVertex[count].ty = 1.0f;
			m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[3],m_Node[i].g[3],m_Node[i].b[3],m_Node[i].Life);

		}
	}

	m_Device->CreateVertexBuffer(m_ParticleNum*sizeof(ParticleVertex) * 6,0,
								D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,D3DPOOL_MANAGED,&m_FogBuffer);
		
	VOID *tmp;
	m_FogBuffer->Lock(0,m_ParticleNum*sizeof(ParticleVertex) * 6,
					(BYTE **)&tmp,0);
	memcpy(tmp,m_pVertex,m_ParticleNum*sizeof(ParticleVertex) * 6);
		
	m_FogBuffer->Unlock();
		
	delete[] m_pVertex;
	m_pVertex = NULL;

	*/
}
void CFogScene::ChangeVertexBuffer() {
	int i;
	int count = 0;
	int count2 = 0;

	vector3 right;
	vector3 up;
	D3DXMATRIX m_World;
	
	if(m_ClosePointNum >0) 
		m_pVertex = new ParticleVertex[m_ClosePointNum * 6];
	m_pPoint = new POINTVERTEX[m_ParticleNum - m_ClosePointNum];
	

	m_Device->GetTransform(D3DTS_VIEW,&m_World);
	
	right.x = m_World._11;
	right.y = m_World._21;
	right.z = m_World._31;
	
	up.x = m_World._12;
	up.y = m_World._22;
	up.z = m_World._32;

	// Lod near 
	if(m_Lod == NEAR_LOD) {
		for(i=0;i<m_ParticleNum;i++) {
			//0
			if(m_Node[i].m_bClose) {
				m_pVertex[count].x = m_Node[i].X + (-right.x + up.x)*(m_Size);
				m_pVertex[count].y = m_Node[i].Y + (-right.y + up.y)*(m_Size);
				m_pVertex[count].z = m_Node[i].Z + (-right.z + up.z)*(m_Size);
				m_pVertex[count].tx = 0.0f;
				m_pVertex[count].ty = 0.0f;
				
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[0],m_Node[i].g[0],m_Node[i].b[0],m_Node[i].Life);
				//1
				m_pVertex[count].x = m_Node[i].X + (right.x + up.x)*(m_Size);
				m_pVertex[count].y = m_Node[i].Y + (right.y + up.y)*(m_Size);
				m_pVertex[count].z = m_Node[i].Z + (right.z + up.z)*(m_Size);
				m_pVertex[count].tx = 1.0f;
				m_pVertex[count].ty = 0.0f;
			
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[1],m_Node[i].g[1],m_Node[i].b[1],m_Node[i].Life);

				//2
				m_pVertex[count].x = m_Node[i].X + (right.x - up.x)*(m_Size);
				m_pVertex[count].y = m_Node[i].Y + (right.y - up.y)*(m_Size);
				m_pVertex[count].z = m_Node[i].Z + (right.z - up.z)*(m_Size);
				m_pVertex[count].tx = 1.0f;
				m_pVertex[count].ty = 1.0f;
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[2],m_Node[i].g[2],m_Node[i].b[2],m_Node[i].Life);

				//0
				m_pVertex[count].x = m_Node[i].X + (-right.x + up.x)*(m_Size);
				m_pVertex[count].y = m_Node[i].Y + (-right.y + up.y)*(m_Size);
				m_pVertex[count].z = m_Node[i].Z + (-right.z + up.z)*(m_Size);
				m_pVertex[count].tx = 0.0f;
				m_pVertex[count].ty = 0.0f;
			
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[0],m_Node[i].g[0],m_Node[i].b[0],m_Node[i].Life);
			
				//2
				m_pVertex[count].x = m_Node[i].X + (right.x - up.x)*(m_Size);
				m_pVertex[count].y = m_Node[i].Y + (right.y - up.y)*(m_Size);
				m_pVertex[count].z = m_Node[i].Z + (right.z - up.z)*(m_Size);
				m_pVertex[count].tx = 1.0f;
				m_pVertex[count].ty = 1.0f;
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[2],m_Node[i].g[2],m_Node[i].b[2],m_Node[i].Life);

				//3
				m_pVertex[count].x = m_Node[i].X + (right.x + up.x)*(-m_Size);
				m_pVertex[count].y = m_Node[i].Y + (right.y + up.y)*(-m_Size);
				m_pVertex[count].z = m_Node[i].Z + (right.z + up.z)*(-m_Size);
				m_pVertex[count].tx = 0.0f;
				m_pVertex[count].ty = 1.0f;
				m_pVertex[count++].Color = D3DCOLOR_COLORVALUE(m_Node[i].r[3],m_Node[i].g[3],m_Node[i].b[3],m_Node[i].Life);
			} // m_bclose == true;
			else {
				m_pPoint[count2].v.x = m_Node[i].X; 
				m_pPoint[count2].v.y = m_Node[i].Y; 
				m_pPoint[count2].v.z = m_Node[i].Z;
				m_pPoint[count2++].color = D3DCOLOR_COLORVALUE(m_Node[i].r[0],m_Node[i].g[0],m_Node[i].b[0],m_Node[i].Life);
			
			}
		}
	}
	if(m_ClosePointNum) {
		VOID *tmp;
		m_FogBuffer->Lock(0,m_ClosePointNum * sizeof(ParticleVertex) * 6,
						(BYTE **)&tmp,0);
		memcpy(tmp,m_pVertex,m_ClosePointNum * sizeof(ParticleVertex) * 6);
			
		m_FogBuffer->Unlock();
			
		delete[] m_pVertex;
		m_pVertex = NULL;
	}
	//point sprite

	VOID *tmp2;
	m_FogPointSpriteBuffer->Lock(0,(m_ParticleNum - m_ClosePointNum) * sizeof(POINTVERTEX),(BYTE **)&tmp2 ,0);
	
	memcpy(tmp2,m_pPoint,sizeof(POINTVERTEX) * (m_ParticleNum - m_ClosePointNum));  
	
	m_FogPointSpriteBuffer->Unlock();
	
	delete[] m_pPoint;
	m_pPoint = NULL;

}

// arg1 : LOD level
void CFogScene::Render() {
	LRESULT rt;
	// 이부분이 삭제. point sprite use.
	ChangeVertexBuffer();

	if(m_Lod) {
		float size = 1.0f;
		if(m_Size > 1000.0f) {
			size += ((m_Size - 1000.0f)/1000.0f);
		}
		if(!m_bPick)
			m_Device->SetTexture(0,m_BaseTexture->GetTexture());
		else {	
			m_Device->SetTexture(0,NULL);
			m_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		}
		// point sprite setting ////////////////////////
		m_Device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		m_Device->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		
		HRESULT hr;
		hr = m_Device->SetRenderState( D3DRS_POINTSIZE,     FtoDW(10.0f * size) );
		hr = m_Device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.00f) );
	
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(2.0f) );
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(1.00f) );
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.00f) );
		/*
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00001f) );
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
		hr = m_Device->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.00f) );
		*/
		////////////////////////////////////////////////	
		m_Device->SetRenderState( D3DRS_LIGHTING, FALSE);
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);		
		
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  
		m_Device->SetRenderState(D3DRS_SRCBLEND,m_SrcBlendMode);
		m_Device->SetRenderState(D3DRS_DESTBLEND,m_DstBlendMode);
		m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);  
		//m_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
		//m_Device->BeginScene();
	
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

		m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		//// point sprite 

		m_Device->SetStreamSource( 0, m_FogPointSpriteBuffer, sizeof(POINTVERTEX));
		m_Device->SetVertexShader( D3DFVF_POINTVERTEX );
				
		rt = m_Device->DrawPrimitive( D3DPT_POINTLIST, 0,m_ParticleNum - m_ClosePointNum);
		////
		hr = m_Device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		m_Device->SetRenderState( D3DRS_POINTSIZE,   FtoDW(1.0f) );
		m_Device->SetRenderState( D3DRS_POINTSPRITEENABLE,FALSE);
		m_Device->SetRenderState( D3DRS_POINTSCALEENABLE,FALSE);
	
		// 일반 quad
		if(m_ClosePointNum) {
			m_Device->SetStreamSource(0,m_FogBuffer,sizeof(ParticleVertex));
			m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			
			rt = m_Device->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_ClosePointNum*2);	
		}
		// end setting 
		//m_Device->EndScene();
		
		if(m_bPick)
			m_Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);  
		m_Device->SetRenderState( D3DRS_LIGHTING, TRUE);
	}

	
}
void CFogScene::SetEtc() {
	int i;
	for(i=0;i<m_ParticleNum;i++) {
		m_Node[i].bFade = false;
		m_Node[i].bFadeIn = false;
		m_Node[i].Fade = 0.01f;
		
	//	Node[i].Fade = (float)(rand()%100)/100000.0f + 0.005;
	}
}
// update node
void CFogScene::Update(float x,float y,float z) {
	int i;
	ClosePerson(x,y,z);
	
	for(i=0;i<m_ParticleNum;i++) {

		if(m_Node[i].Life <=m_FadeOut) {
			m_Node[i].Active = false;
			m_Node[i].bFadeIn = true;	
			m_Node[i].bFade = false;
			m_Node[i].Life = m_FadeOut;
			
		}
		
		//fade out
		if(m_Node[i].bFade == true) {
			m_Node[i].Life -= m_Node[i].Fade*m_FadeOutSpeed;
			if(m_Node[i].Life < m_FadeOut)
				m_Node[i].Life = m_FadeOut;
		}
		// fade in and out off 
		if(m_Node[i].bFade == false && m_Node[i].bFadeIn == false) {
			m_Node[i].BackX = m_Node[i].X;
			m_Node[i].BackY = m_Node[i].Y;
			m_Node[i].BackZ = m_Node[i].Z;
		}

		// wind 적용
		m_Node[i].X += m_Node[i].Xi + m_Wind[0];
		
		//if(Node[i].Xi >0)
		//	MessageBox(NULL,"test","test",MB_OK);
		m_Node[i].Y += m_Wind[1];
		m_Node[i].Z += m_Wind[2];

		//flow 밑으로는 안나가게.
		if(m_Node[i].Flow <= m_Node[i].Y + m_Node[i].Yi)
			m_Node[i].Y += m_Node[i].Yi;
			
		m_Node[i].Z += m_Node[i].Zi;
		
		// 중력 적용
		m_Node[i].X += m_Gravity[0];
		m_Node[i].Y += m_Gravity[1];
		m_Node[i].Z += m_Gravity[2];
		
		
		if(m_Node[i].Active == false) {
			
			m_Node[i].X = m_Node[i].BackX;
			m_Node[i].Y = m_Node[i].BackY;
			m_Node[i].Z = m_Node[i].BackZ;
			m_Node[i].Xi = 0.0f;
			m_Node[i].Yi = 0.0f;
			m_Node[i].Zi = 0.0f;
			m_Node[i].bFadeIn = true;
			m_Node[i].bFade = false;
		
			//Node[i].Life = 1.0f;
		}
		//fade in
		if(m_Node[i].bFadeIn == true) {
			m_Node[i].Life +=m_Node[i].Fade * m_FadeInSpeed;
		
			if(m_Node[i].Life > m_FadeIn) {
			
				m_Node[i].Life = m_FadeIn;
				m_Node[i].Active = true;
				m_Node[i].bFadeIn = false;
				m_Node[i].bFade = false;
			}
	
		}

	}
//	ChangeVertexBuffer();
}
void CFogScene::Resetting() {
	int i;
	for(i=0;i<m_ParticleNum;i++) {
		if(m_Node[i].Active == false) {

		}
	}
}
void CFogScene::ClosePerson(vector3 pos) {
	int i;
	float tmp[3];
	float unit[3];
	float length;
	static vector3 before(0.0f,0.0f,0.0f);

	for(i=0;i<m_ParticleNum;i++) {
		tmp[0] = m_Node[i].BackX - pos.x;
		tmp[1] = m_Node[i].BackY - pos.y;
		tmp[2] = m_Node[i].BackZ - pos.z;
		
		length = (float)sqrt((tmp[0] * tmp[0]) + (tmp[1] * tmp[1]) + (tmp[2] * tmp[2]));
		unit[0] = tmp[0] / length;
		unit[1] = tmp[1] / length;
		unit[2] = tmp[2] / length;
	
		if(m_Rad > length && before.x != pos.x && before.y != pos.y && before.z != pos.z) {
			float Density = 0.66f;
			/*if(m_Node[i].Life > m_FadeOut) {
				// 조금 하해지게..
				Setfadein(0.7f);
				m_Node[i].Xi +=unit[0] * 0.01f;
				m_Node[i].bFade = true;
				//Node[i].Yi +=unit[1];
				m_Node[i].Zi +=unit[2]*0.01f;
			}*/
			m_Device->SetRenderState(D3DRS_FOGENABLE, TRUE);
 
		    // fog 칼라를 설정한다.
			m_Device->SetRenderState(D3DRS_FOGCOLOR,D3DCOLOR_COLORVALUE(m_Node[0].r[0],m_Node[0].g[0],m_Node[0].b[0],m_Node[0].Life));
    
			m_Device->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_EXP);
			m_Device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
    
		}
		else {
			m_Device->SetRenderState(D3DRS_FOGENABLE,FALSE);
			before = pos;
			// 완전히 하해지게..--;
			Setfadein(1.0f);

		}
		
	}
	
}

void CFogScene::ClosePerson(float x,float y,float z){
	int i;
	float tmp[3] = {0.0f,0.0f,0.0f};
	float unit[3] = {0.0f,0.0f,0.0f};
	float length = 0.0f;
	float Density = 0.66f;
	// 사람이 한사람일때 만이다.. 여러명이면.. 각자 상태 플레그로 움직임을 체크하여야 할것
	// 전의 움직임
	static float before[3] = {0.0f,0.0f,0.0f};
	int f=0;
	m_ClosePointNum = 0;

	for(i=0;i<m_ParticleNum;i++) {
		tmp[0] = m_Node[i].BackX - x;
	//	tmp[1] = m_Node[i].BackY - y;
		tmp[2] = m_Node[i].BackZ - z;
		
		length = (float)sqrt((tmp[0] * tmp[0]) /*+ (tmp[1] * tmp[1])*/ + (tmp[2] * tmp[2]));
		unit[0] = tmp[0] / length;
		unit[1] = tmp[1] / length;
		unit[2] = tmp[2] / length;
		
		if(m_Rad > length && (before[0] != x || before[1] != y || before[2] != z)) {
			if(m_Node[i].Life > m_FadeOut) {
							
				m_Node[i].Xi +=unit[0]*0.01f;
				m_Node[i].bFade = true;
				//Node[i].Yi +=unit[1]*5.0f;
				m_Node[i].Zi +=unit[2]*0.01f;
			}
			else {
				
			//	f--;
			
				before[0] = x;
				before[1] = y;
				before[2] = z;
			}
		}
		if(m_Rad >length) {
			f++;
		}
		if(m_Rad + 7500.0f>length) {
			// point sprite -> bilboard fog settting 
			m_ClosePointNum++;
			m_Node[i].m_bClose = true;
		}
		else {
			m_Node[i].m_bClose = false;
		}

	}
	// 내가 안게속에 있을때.
	if(f>0) {
		// 조금 하해지게..
		Setfadein(m_BackFadeIn / 2.0f);
	}
	//내가 안게속에 없을때..
	if(f==0) {
		
		// 완전히 하해지게..--;
		Setfadein(m_BackFadeIn);
	}
}
	
void CFogScene::SetSize(float fsize) {
	m_Size = fsize;
}
void CFogScene::SetBlendMode(DWORD src,DWORD dst) {

	m_SrcBlendMode = src;

	m_DstBlendMode = dst;
}
void CFogScene::Setfadein(float f) {
	m_FadeIn = f;
}
void CFogScene::Setfadeout(float f) {
	m_FadeOut = f;
}
void CFogScene::SetBackFadeIn(float f) {
	m_BackFadeIn = f;

}