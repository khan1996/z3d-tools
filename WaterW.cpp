#include "WaterW.h"
#include "SceneManager.h"
#include "GMMemory.h"

CWaterW ::CWaterW() {
	m_pWaterSpace = NULL;
	m_pWaterChangeValue = NULL;
	m_pWaterCoord = NULL;
	m_nWidthSize = WATERWWIDTH;
	m_nHeightSize = WATERWHEIGHT;
	m_fLimitHeight = 0.0;
	m_nDriftCount = 0;
	m_Device = NULL;
	m_nChopPower = WATERWCHOP;
	m_IndexNum = 0;
	m_ZeroPos.x = m_ZeroPos.y = m_ZeroPos.z = 0.0f;
	m_QuadSize = 1.0f;
    
	CreateWaterWSpace();
	

}
CWaterW ::CWaterW(int size,double lh,float chop,float x,float z,float quadsize,LPDIRECT3DDEVICE8 device) {
	m_pWaterSpace = NULL;
	m_pWaterChangeValue = NULL;
	m_pWaterCoord = NULL;
	m_nWidthSize = size;
	m_nHeightSize = size;

	m_fLimitHeight = lh;
	m_nDriftCount = 0;

	m_nChopPower = chop;
	m_Device = device;
	m_IndexNum = 0;
	m_ZeroPos.x = x;

	m_ZeroPos.z = z;
	m_QuadSize = quadsize;

	CreateWaterWSpace();
	CreateTexture();
	CreateVertexBuffer();

	
}
CWaterW ::CWaterW(int w,int h,double lh,float chop,float x,float z,float quadsize,LPDIRECT3DDEVICE8 device){
	m_pWaterSpace = NULL;
	m_pWaterChangeValue = NULL;
	m_pWaterCoord = NULL;
	m_nWidthSize = w;
	m_nHeightSize = h;
	
	m_fLimitHeight = lh;
	m_nDriftCount = 0;
	m_nChopPower = chop;
	m_Device = device;
	m_IndexNum = 0;
	
	m_ZeroPos.x = x;
	m_ZeroPos.z = z;
	m_QuadSize = quadsize;


	CreateWaterWSpace();
	CreateTexture();
	CreateVertexBuffer();
}
void CWaterW ::SetLimitHeight(double h) {
	m_fLimitHeight = h;
	
}

bool CWaterW ::CreateWaterWSpace() {
	int i;
	if(!m_nWidthSize || !m_nHeightSize) {
		return false;		
	}
	////// mem clear //////
	if(m_pWaterSpace != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterSpace[i];
			m_pWaterSpace[i] = NULL;
		}
		delete[] m_pWaterSpace;
		m_pWaterSpace = NULL;
	}
	if(m_pWaterChangeValue != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterChangeValue[i];
			m_pWaterChangeValue[i] = NULL;
		}
		delete[] m_pWaterChangeValue;
		m_pWaterChangeValue = NULL;
	}
	if(m_pWaterCoord != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterCoord[i];
			m_pWaterCoord[i] = NULL;
		}
		delete[] m_pWaterCoord;
		m_pWaterCoord = NULL;
	}

	//////////////////////////////////////////

	m_pWaterSpace =  new double*[m_nWidthSize];
	m_pWaterChangeValue = new double*[m_nWidthSize];
	m_pWaterCoord = new double*[m_nWidthSize];

	for(i=0;i<m_nWidthSize;i++) {
		m_pWaterSpace[i] = new double[m_nHeightSize];
		m_pWaterChangeValue[i] = new double[m_nHeightSize];
		m_pWaterCoord[i] = new double[m_nHeightSize];
	
	}
	// water 공간 세팅
	InitWaterW();

	return true;
}
void CWaterW ::InitWaterW() {

	int i,j;
	for(i=0;i<m_nWidthSize;i++)
		for(j=0;j<m_nHeightSize;j++){
			m_pWaterSpace[i][j] = 0.0f;
			m_pWaterChangeValue[i][j] = 0.0f;//((double)rand()/(double)RAND_MAX-.2)*1;
		}

}
void CWaterW ::UpdateWaterW() {
	int i,j;
	
	float h;

	for(i=1;i<m_nWidthSize-1;i++) {
		for(j=1;j<m_nHeightSize-1;j++) {
			h = (m_pWaterSpace[j-1][i] + m_pWaterSpace[j+1][i] + m_pWaterSpace[j][i-1] + m_pWaterSpace[j][i+1])/2.0f - m_pWaterChangeValue[j][i];
			
			h -= h/32.0f;
			m_pWaterChangeValue[j][i] = h;
		}
	}
	for(i=0;i<m_nWidthSize;i++) {
		for(j=0;j<m_nHeightSize;j++) {
			float t;
			t = m_pWaterSpace[j][i];
			m_pWaterSpace[j][i] = m_pWaterChangeValue[j][i];
			m_pWaterChangeValue[j][i] = t;

		}
	}


/*
	// 상호 작용 구현
	// 각 버텍스별 영향치 계산 
	for(i=1;i<m_nWidthSize - 1;i++) {
		for(j=1;j<m_nHeightSize - 1;j++) {
			m_pWaterChangeValue[i][j] = m_pWaterChangeValue[i][j] -
										(m_pWaterSpace[i][j] - 
										 (m_pWaterSpace[i][j-1] + 
										  m_pWaterSpace[i][j+1] +
										  m_pWaterSpace[i-1][j-1] + 
										  m_pWaterSpace[i-1][j] + 
										  m_pWaterSpace[i+1][j] + 
										  m_pWaterSpace[i+1][j+1])/6 ) * m_nChopPower;
		}
	}*/
/*
	// 영향치 적용 
	for(i=0;i<m_nWidthSize;i++) {
		for(j=0;j<m_nHeightSize;j++) {

			m_pWaterChangeValue[i][j]=m_pWaterChangeValue[i][j]-_copysign(.01,m_pWaterSpace[i][j]);
			m_pWaterChangeValue[i][j]=m_pWaterChangeValue[i][j]*0.99;
			m_pWaterSpace[i][j]=m_pWaterSpace[i][j]+m_pWaterChangeValue[i][j];
		}
	}
*/
}
void CWaterW ::SetHeight(int x,int y,float height) {
	m_pWaterSpace[y][x] = height;

}
void CWaterW ::ComputeWaterW() {
	int unitnum = m_DriftUnit.size();
	int i;
	// water영역안에 들어와 있는 unit 의 갯수 만큼 업데이트 

	for(i=0;i<unitnum;i++) {
		int x,y;
		x = (int)m_DriftUnit[i].m_UnitPos.x;
		y = (int)m_DriftUnit[i].m_UnitPos.y;

		int unitrange = m_DriftUnit[i].m_nUnitRange;
		
		m_pWaterSpace[y][x] = -0.4f;	
		
		DeleteDriftUnit(m_DriftUnit[i]);
		//m_DriftUnit[i].m_UnitPos.x += m_DriftUnit[i].m_UnitFwd.x;
		//m_DriftUnit[i].m_UnitPos.y += m_DriftUnit[i].m_UnitFwd.y;
		
/*		int unitrange = m_DriftUnit[i].m_nUnitRange;
		D3DXVECTOR3 pos = m_DriftUnit[i].m_UnitPos;
		// 영향의 범위가 water을 벗어났을 때에 영향 영역 수정 
		int xcenter=(int)(pos.x);
		int ycenter=(int)(pos.y)+xcenter/2;
		if((xcenter-unitrange)<0)
			xcenter=unitrange;
		if((xcenter+unitrange)>=m_nWidthSize)
			xcenter=m_nWidthSize-unitrange;
		if((ycenter-unitrange)<0)
			ycenter=unitrange;
		if((ycenter+unitrange)>m_nHeightSize)
			ycenter=m_nHeightSize-unitrange;
		// 영향 주기		
		int b=ycenter-unitrange;int e=ycenter;
		for(i=xcenter-unitrange;i<xcenter+unitrange;i++) {
			for(j=b;j<e;j++) {
				double force;
				D3DXVECTOR3 is(pos.x,pos.y,pos.z);

				if(m_pWaterSpace[i][j]>is.y) {
					force=(m_pWaterSpace[i][j]-is.y)+m_pWaterChangeValue[i][j];
					m_pWaterSpace[i][j]=is.y;
					m_pWaterChangeValue[i][j]=m_pWaterChangeValue[i][j]-force*.5;
				}
			
			}
			if(i<xcenter)
				e++;
			else
				b++;
		} */// for(i=xcenter-unitrange;i<xcenter+unitrange;i++)
	} //for(i=0;i<unitnum;i++)
	unitnum = m_DriftUnit.size();

	for(i=0;i<unitnum;i++) {
		if(((int)m_DriftUnit[i].m_UnitPos.x >=m_nWidthSize) || ((int)m_DriftUnit[i].m_UnitPos.x <0)) { 
			DeleteDriftUnit(m_DriftUnit[i]);
		}
		if(((int)m_DriftUnit[i].m_UnitPos.y >=m_nHeightSize) || ((int)m_DriftUnit[i].m_UnitPos.y <0)) { 
			DeleteDriftUnit(m_DriftUnit[i]);
		
		}
	}
}
void CWaterW ::AddDriftUnit(CDriftUnit tmp_u) {

	tmp_u.SetUnitIndex(m_nDriftCount++);
	m_DriftUnit.push_back(tmp_u);
}
void CWaterW ::DeleteDriftUnit(CDriftUnit tmp_u) {
	int i;
	for(i=0;i<(int)m_DriftUnit.size();i++) {
		if(m_DriftUnit[i].m_nIndex == tmp_u.m_nIndex) {
			m_DriftUnit.erase(m_DriftUnit.begin() + i);
			break;
		}
	}
}
CWaterW ::~CWaterW() {
	int i;
	////// mem clear //////
	if(m_pWaterSpace != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterSpace[i];
			m_pWaterSpace[i] = NULL;
		}
		delete[] m_pWaterSpace;
		m_pWaterSpace = NULL;
	}
	if(m_pWaterChangeValue != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterChangeValue[i];
			m_pWaterChangeValue[i] = NULL;
		}
		delete[] m_pWaterChangeValue;
		m_pWaterChangeValue = NULL;
	}
	if(m_pWaterCoord != NULL) {
		for(i=0;i<m_nWidthSize;i++) {
			delete[] m_pWaterCoord[i];
			m_pWaterCoord[i] = NULL;
		}
		delete[] m_pWaterCoord;
		m_pWaterCoord = NULL;
	}
	
	m_WaterVertexBuffer->Release();
	m_WaterIndexBuffer->Release();

	m_DriftUnit.clear();

}
int CWaterW::CDriftUnit::SetUnitIndex(int n) {
	m_nIndex = n;
	return m_nIndex;

}

void CWaterW::Render() {

	ChangeVertexBuffer();
	DWORD zmode,alphamode,cullmode;

	D3DXMATRIX view,world,project;
	D3DXMATRIX beforeworld;
	D3DXMatrixIdentity(&beforeworld);
	m_Device->GetTransform(D3DTS_WORLD,&world);
	m_Device->SetTransform(D3DTS_WORLD,&beforeworld);
	
	m_Device->GetTransform(D3DTS_VIEW,&view);
	m_Device->GetTransform(D3DTS_PROJECTION,&project);

	m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zmode);
	m_Device->GetRenderState(D3DRS_ALPHABLENDENABLE,&alphamode);
	m_Device->GetRenderState(D3DRS_CULLMODE,&cullmode);
//////////////////
 
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);             
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  
	m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	m_Device->SetRenderState(D3DRS_LIGHTING,TRUE);
	
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);             
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);  
	m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	  

///0//////	
	
	//m_Device->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2);
	//m_Device->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL);
	m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
	m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
			
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);	

	DWORD env_alpha;
	env_alpha = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);

	m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,env_alpha);


	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);		
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
/////////
    
	// Turn on texture-coord generation for cubemapping
    m_Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
    m_Device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );

    // Render the object with the environment-mapped body
    m_Device->SetTexture( 0, m_pSkyCubeTex);
  
  
//////////////
	m_Device->SetStreamSource(0,m_WaterVertexBuffer,sizeof(CWaterVertex));
	m_Device->SetIndices(m_WaterIndexBuffer,0);

	m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);
	m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_nWidthSize * m_nHeightSize,0,(m_IndexNum / 3));
	
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zmode);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,alphamode);  
	m_Device->SetRenderState(D3DRS_CULLMODE,cullmode);
    // Restore the render states
    m_Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
    m_Device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );


	m_Device->SetTransform(D3DTS_WORLD,&world);

}

void CWaterW::CreateVertexBuffer() {

	WORD *VertIndex;
	int i,j;

	int count = 0;

	m_Device->CreateVertexBuffer((m_nWidthSize) * (m_nHeightSize) * sizeof(CWaterVertex),0,
								D3DFVF_XYZ | D3DFVF_NORMAL,D3DPOOL_DEFAULT,&(m_WaterVertexBuffer));
	VertIndex = new WORD[(m_nWidthSize-2) * (m_nHeightSize-2) * 6];
	
	for(i=1;i<(m_nWidthSize -1);i++) {
		for(j=1;j<(m_nHeightSize-1);j++) {
			VertIndex[count++] = (m_nWidthSize * (i)) + j-1;
			VertIndex[count++] = (m_nWidthSize * (i-1)) + j-1;
			VertIndex[count++] = (m_nWidthSize * (i)) + j;
			VertIndex[count++] = (m_nWidthSize * (i)) + j;
			VertIndex[count++] = (m_nWidthSize * (i-1)) + j-1;
			VertIndex[count++] = (m_nWidthSize * (i-1)) + j;
			//vertIndex[count++] = 1,0 (i,j-1)
			//vertIndex[count++] = 0,0 (i-1,j-1)
			//vertIndex[count++] = 1,1 (i,j)
			//vertIndex[count++] = 1,1 (i,j)
			//vertIndex[count++] = 0,0 (i-1,j-1)
			//vertIndex[count++] = 0,1 (i-1,j)
			
			

		}
	}
	m_IndexNum  = count;
	
	m_Device->CreateIndexBuffer((m_nWidthSize-2) * (m_nHeightSize-2) * 6 * sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&(m_WaterIndexBuffer));
	VOID *tmp_buffer;
	m_WaterIndexBuffer->Lock(0,(m_nWidthSize-2) * (m_nHeightSize-2) * 6 * sizeof(WORD),(BYTE **)&tmp_buffer,0);

	memcpy(tmp_buffer,VertIndex,(m_nWidthSize-2) * (m_nHeightSize-2) * 6 * sizeof(WORD));

	m_WaterIndexBuffer->Unlock();
	delete[] VertIndex;

}
void CWaterW::ChangeVertexBuffer() {
	int i,j;
	int count = 0;

	CWaterVertex *m_WaterV = new CWaterVertex[(m_nHeightSize) * (m_nWidthSize)];
	
	for(i=0;i<(m_nWidthSize);i++) {
		for(j=0;j<(m_nHeightSize);j++) {
			//quad setting
			//left triangle
			m_WaterV[count].x = m_ZeroPos.x + ((float)(j) * m_QuadSize);
			m_WaterV[count].y = m_fLimitHeight + m_pWaterSpace[j][i];
			m_WaterV[count].z = m_ZeroPos.z + ((float)(i)* m_QuadSize);
			//normal
			D3DXVECTOR3 tmp1;
			D3DXVECTOR3 tmp2;
			if((i-1 >=0) && (j-1 >=0)) {
				tmp1.x = 0;
				tmp1.y = m_pWaterSpace[j][i-1] - m_pWaterSpace[j][i];
				tmp1.z = (i-1) - i;
			
			
				D3DXVec3Normalize(&tmp1,&tmp1);
				tmp2.x = (j-1) -j;
				tmp2.y = m_pWaterSpace[j-1][i] - m_pWaterSpace[j][i];
				tmp2.z = 0;
				D3DXVec3Normalize(&tmp2,&tmp2);
				D3DXVECTOR3 normal;
				D3DXVec3Cross(&normal,&tmp1,&tmp2);
				D3DXVec3Normalize(&normal,&normal);
				
				m_WaterV[count].nx = normal.x;
				m_WaterV[count].ny = normal.y;
				m_WaterV[count].nz = normal.z;
			}
			else {
				m_WaterV[count].nx = 0.0f;
				m_WaterV[count].ny = 1.0f;
				m_WaterV[count].nz = 0.0f;
			}
			count++;				

			
		}
	}

	VOID *tmp_buffer;

	m_WaterVertexBuffer->Lock(0,(m_nHeightSize)*(m_nWidthSize)*sizeof(CWaterVertex),(BYTE **)&tmp_buffer,0);
	
	memcpy(tmp_buffer,m_WaterV,((m_nHeightSize) * (m_nWidthSize) * sizeof(CWaterVertex)));

	m_WaterVertexBuffer->Unlock();
	delete[] m_WaterV;
	
}
bool CWaterW::SetTexture(char *filename,char *envfilename) {
	CTexture::SetPath(EFFECTTEXTUREPATH);

	if(strlen(filename) >0) {
		m_WaterTexture.Load(filename);

	}
	if(strlen(envfilename) >0) {
		m_WaterEnv.Load(envfilename);

		return true;
	}
	return false;	
}
// Water Scene 전체 업데이트 
void CWaterW ::Update(float x,float y,float z) {
	int x_index = (int)( (x - m_ZeroPos.x)/m_QuadSize);
	int y_index = (int) ((z - m_ZeroPos.z)/m_QuadSize);
	int flag = 0;

	if(!(x == 0 && y == 0 && z==0))  
		if(x_index>= 0 && y_index >=0) 
			if(x_index <m_nWidthSize && y_index <m_nHeightSize) { 
				SetHeight(y_index,x_index,m_nChopPower);
				flag = 1;
			}
		
	if(!flag) {
		InitWaterW();			
	}
	UpdateWaterW();


//	ComputeWaterW();
}
void CWaterW ::CreateTexture() {

    
    char* szSkyTex[6] = {
        "c:\\lobbyxpos.bmp","c:\\lobbyxneg.bmp","c:\\lobbyypos.bmp","c:\\lobbyyneg.bmp","c:\\lobbyzneg.bmp","c:\\lobbyzpos.bmp"
	};

    for(int i = 0; i < 6; i++) {
        
        D3DXCreateTextureFromFileEx(m_Device,szSkyTex[i], D3DX_DEFAULT, D3DX_DEFAULT, 
            1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
            0, NULL, NULL, &m_pSkyTex[i]);
    }

    D3DXCreateCubeTexture(m_Device, 128, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, &m_pSkyCubeTex);
   
    for(int i = 0; i < 6; i++) {
		if(m_pSkyTex[i]) {
                IDirect3DSurface8 *pSrc;
                IDirect3DSurface8 *pDest;

                m_pSkyTex[i]->GetSurfaceLevel(0, &pSrc);
                m_pSkyCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &pDest);

                if(pSrc && pDest)
                    D3DXLoadSurfaceFromSurface(pDest, NULL, NULL, pSrc, NULL, NULL, D3DX_DEFAULT, 0);

                pDest->Release();
                pSrc->Release();
        }
    }

    D3DXFilterCubeTexture(m_pSkyCubeTex, NULL, 0, D3DX_DEFAULT);    

}
