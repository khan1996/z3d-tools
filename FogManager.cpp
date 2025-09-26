
#include "FogManager.h"
#include "GMMemory.h"

/*************************************************************************************************************

  CFogManager class

***************************************************************************************************************/


CFogManager :: CFogManager() {
	int m_FogNum = 0;
}	
CFogManager ::~CFogManager() {
	
	for(int i = 0;i<m_FogNum;i++) 
		delete m_FogUnit[i];

	m_FogUnit.clear();
}

// return value : update 된 particle 객체 갯수
// arg 1 ,2 : 검색 섹터 좌표
int CFogManager::UpdateFog(int secx,int secy) {
	
	int i;														/* close person 함수 추가*/
	int count = 0;												/* SECTORSIZE 가 기록되어있는 SectorDefine.h include */													
																//Z3D Sceneclass 안에 있다.
	vector3 center;
	vector3 pos;
//	int indexx,indexy;

	for(i=0;i<m_FogNum;i++) {
		center = m_FogUnit[i]->GetCenter();
		//indexx = (int)(center.x/SECTORSIZE);
		//indexy = (int)(center.z/SECTORSIZE);
	// Sector 안의 것만 rendering		
	//	if(secx == indexx && secy == indexy) {
		//get person pos;
			m_FogUnit[i]->Update(pos.x,pos.y,pos.z);
			count++;
	//	}
	}
	return count;
}

int CFogManager::UpdateFog(int num) {
	D3DXVECTOR3 pos;
	//get person pos
	m_FogUnit[num]->Update(pos.x,pos.y,pos.z);
	return num;
}

int CFogManager::Render(int num) {
	m_FogUnit[num]->Render();
	return num;
}
int CFogManager::Render() {

	int i;
	int count = 0;
	vector3 center;
//	secter index
//	int indexx,indexy;

	for(i=0;i<m_FogNum;i++) {
		center = m_FogUnit[i]->GetCenter();
		//indexx = (int)(center.x/SECTORSIZE);
		//indexy = (int)(center.z/SECTORSIZE);
		//if(secx == indexx && secy == indexy) {
			m_FogUnit[i]->Render();
			count++;
		//}
	}
	return count;

}
int CFogManager::SetFog(int num) {

/*	m_FogUnit[num]->SetCenter(30.0f);
	m_FogUnit[num]->SetCenterPos(0.0f,0.0f,10.0f);
	m_FogUnit[num]->SetColor(1.0f,1.0f,1.0f,1.0f);
	m_FogUnit[num]->SetFlow(-2.0f);
	m_FogUnit[num]->SetGravity(0.0f,0.0f,0.0f);
	D3DXVECTOR3 tm;
	tm.z = 0.3f;
	tm.x = 0.6f;
	tm.y = 0.1f;
//	tm.x = tm.y = tm.z = 0.0f;
//	tm.x = 0.3f;
//	tm.y = 0.1f;
	m_FogUnit[num]->SetSize(5.0f);
	m_FogUnit[num]->SetImpress(tm);
	m_FogUnit[num]->SetPos();
	m_FogUnit[num]->SetColor(1.0f,1.0f,1.0f,1.0f);
	m_FogUnit[num]->SetEtc();
	m_FogUnit[num]->SetRad(5.0f);
	m_FogUnit[num]->SetSpeed(0.0f,0.0f,0.0f);
	m_FogUnit[num]->SetSpeedPower(0.0f);
	m_FogUnit[num]->SetWind(0.0f,0.0f,0.0f);
	m_FogUnit[num]->CreateTextureBuffer(2);

	m_FogUnit[num]->SetTexture(0,"white.dds");
	m_FogUnit[num]->SetTexture(1,"white32.dds");
	m_FogUnit[num]->Setfadein(1.0f);
	m_FogUnit[num]->CreateVertexBuffer();*/
	//m_FogUnit[num]->Create();

	return 0;

}
void CFogManager::Insert_Fog() {
	CFogScene *tmp;
	
	m_FogUnit.push_back(tmp);
	m_FogUnit[m_FogNum] = new CFogScene();

///	m_FogUnit[m_FogNum]->Create();

	m_FogNum++;
}
int CFogManager::FindFog(float x,float y,float z) {
	vector3 center;
	for(int i=0;i<m_FogNum;i++) {
		center = m_FogUnit[i]->GetCenter();
		if(center.x == x && center.y == y && center.z == z)
			return i;
	}
	return -1;
}

// 게임에 추가 되면서 살아날 코드
/*void CFogManager::CalculateLod() {
	int i;
	vector3 f_pos;
	
	// cameram get
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	vector3 vecViewPos=matViewPosition->GetLoc();

	vecViewPos.x-=m_AccumulateTM._41;
	vecViewPos.z-=m_AccumulateTM._43;

	float fLens=0.0f;
	
	if( 0.0f-fLens_FarTree <= vecViewPos.x &&
		SECTORSIZE+fLens_FarTree >  vecViewPos.x &&
		0.0f-fLens_FarTree <= vecViewPos.z &&
		SECTORSIZE+fLens_FarTree >  vecViewPos.z) {	
		for(i=0;i<Fog_Num;i++) {

			f_pos.x = Fog_Unit[i]->Center_Point.x - vecViewPos.x;
			f_pos.y = Fog_Unit[i]->Center_Point.y - vecViewPos.y;
			f_pos.z = Fog_Unit[i]->Center_Point.z - vecViewPos.z;
			fLens=CFastMath::FastSqrt((f_pos.x*f_pos.x)+(f_pos.y*f_pos.y)+(f_pos.z*f_pos.z));
		
			if(fLens <=LOD_NEAR)
				Fog_Unit[i]->Lod = NEAR_LOD;
			else if(fLens <=LOD_FAR)
				Fog_Unit[i]->Lod = FAR_LOD;
			else 
				Fog_Unit[i]->Lod = NOT_SCENE;
		}

	}
}
*/
