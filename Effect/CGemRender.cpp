#include "CGemRender.h"
#include "SceneManager.h"
#include "GMMemory.h"

CGemRender::CGemRender() {
	m_bLight = false;

	m_AxisSet = false;
	m_AniRad = 0.0f;
	m_StartSwitch = false;
	m_SwitchAni = false;
	m_RandAni =  false;
	m_RandAniMax = 0.0f;
	m_RandAniMin = 0.0f;
	m_Xrot = m_Yrot = m_Zrot = 0.0f;
	m_StartF = 0;
	m_EndF = 0;
	m_UnitF = 0;
	m_ObjectNum = 0;
	m_CurrentF = 0.0f;

	m_MatNum = 0;
	m_bMulty = 0;	
	m_GemFile = NULL;
	m_Mtl = NULL;
	m_Object = NULL;
	//m_ObjectVertex = NULL;
	//m_Vert = NULL;
	m_lpVertexBuffer = NULL;
	m_iTotalVertexNums = 0;
	m_bDynamic = false;
	m_VertexLockPtr = NULL;


	m_Device = NULL;
	VbufferNum = 0;
	m_Sub = NULL;
	m_Texture = NULL;
	m_TexNum = 0;
	m_TexAni = NULL;
	D3DXMatrixIdentity(&m_Scale);
	m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
	m_Multy = NULL;
	m_SrcBlend = D3DBLEND_SRCALPHA;
	m_DstBlend = D3DBLEND_ONE;
	m_bNullTexture = false;
	m_PickObject = -1;
	m_Vot = VOT;
	m_bLoop = false;
	m_bTexAni = false;
	D3DXMatrixIdentity(&m_TrMatrix);
	//axis quaternion
	global.x = m_Axis.x = 0.0f;
	global.y =m_Axis.y = 0.0f;
	global.z =m_Axis.z = 0.0f;
	global.w =m_Axis.w = 1.0f;
	m_Mine = true;
	memset(m_FileName,0,sizeof(char) * 256);
	m_bCash = true;
	m_bClearBuffer = true;
	m_bDecal = false;

}

CGemRender::~CGemRender() {
	if(m_Multy != NULL)
		delete m_Multy;

	if(m_GemFile != NULL)
		fclose(m_GemFile);
	if(m_Mtl != NULL)
		delete[] m_Mtl;
	if(m_Object != NULL)
		delete[] m_Object;
/*	if(m_ObjectVertex != NULL){
		for(int i=0;i<VbufferNum;i++) {
			if(m_ObjectVertex[i] != NULL)
				m_ObjectVertex[i]->Release();
		}
		delete[] m_ObjectVertex;
	}*/
	if(m_lpVertexBuffer != NULL)
	{
		if(CSceneManager::ms_pBufferPools)
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_lpVertexBuffer);
		else
			m_lpVertexBuffer->Release();

		m_lpVertexBuffer = NULL;

	}
	if(m_Sub) {
		delete[] m_Sub;
	}
	if(m_Texture) {
		delete[] m_Texture;
	}
	if(m_TexAni) {
		delete[] m_TexAni;
	}

/*	if(m_Vert) {
		delete[] m_Vert;
		m_Vert = NULL;
	}*/

}
void CGemRender::SetMine(bool t) {
	m_Mine = t;
}
void CGemRender::MsgPrint(char *str){
	MessageBox(NULL,str,"GemRender",MB_OK);
}
bool CGemRender::LoadMorph(int object_index) {
	int i;
	int morph_num = m_Object[object_index].m_MorphNum;
	char buffer[BUF_SIZE] = {0};
	char *token = NULL;

	m_Object[object_index].m_Morph = new GemObject[morph_num];
	
	for(i=0;i<morph_num;i++) {
			
		fread(buffer,sizeof(char) * BUF_SIZE,1,m_GemFile);
		token = strtok(buffer,"\n ");
		int cnum = atoi(token);
		token = strtok(NULL,"\n ");
		int fnum = atoi(token);
		token = strtok(NULL,"\n ");
		int vnum = atoi(token);
		m_Object[object_index].m_Morph[i].m_CoordNum = cnum;
		m_Object[object_index].m_Morph[i].m_FaceNum = fnum;
		m_Object[object_index].m_Morph[i].m_VertexNum = vnum;

		fread(&(m_Object[object_index].m_Morph[i].m_Frame),sizeof(int),1,m_GemFile);
		if(cnum) {
			m_Object[object_index].m_Morph[i].m_Coord = new D3DXVECTOR2[cnum];
			fread(m_Object[object_index].m_Morph[i].m_Coord,sizeof(D3DXVECTOR2),cnum,m_GemFile);
		}
		if(fnum) {
			m_Object[object_index].m_Morph[i].m_Face = new D3DXVECTOR4[fnum];
			m_Object[object_index].m_Morph[i].m_CoordFace = new D3DXVECTOR3[fnum];
			fread(m_Object[object_index].m_Morph[i].m_CoordFace,sizeof(D3DXVECTOR3),fnum,m_GemFile);
			fread(m_Object[object_index].m_Morph[i].m_Face,sizeof(D3DXVECTOR4),fnum,m_GemFile);	
		}
		if(vnum) {
			m_Object[object_index].m_Morph[i].m_Normal = new D3DXVECTOR3[vnum];
			m_Object[object_index].m_Morph[i].m_Vert = new D3DXVECTOR3[vnum];
			fread(m_Object[object_index].m_Morph[i].m_Normal,sizeof(D3DXVECTOR3),vnum,m_GemFile);
			fread(m_Object[object_index].m_Morph[i].m_Vert,sizeof(D3DXVECTOR3),vnum,m_GemFile);
		}
	
	}
	return true;	
}
bool CGemRender::LoadUvFile(char *filename){
	m_Multy->LoadGemFile(filename,m_Device);
	return true;

}
void CGemRender::GetAxis(float x,float y,float z,float w) {
	m_Axis.x = x;
	m_Axis.y = y;
	m_Axis.z = z;
	m_Axis.w = w;


}
void CGemRender::SetAxis(D3DXMATRIX &world) {
	
	D3DXMATRIX tmp_matrix;
	D3DXMatrixIdentity(&tmp_matrix);
	
	D3DXMatrixRotationQuaternion(&tmp_matrix,&m_Axis);
	D3DXMatrixMultiply(&world,&world,&tmp_matrix);

}
bool CGemRender::LoadGemFile(char *filename,LPDIRECT3DDEVICE8 device,bool bVisibility) {
	
	char buffer[BUF_SIZE] = {0};
	char msg[BUF_SIZE] = {0};
	char *token = NULL;
	int i,j;
	int sub_mat = 0;
	
	strcpy(m_FileName,filename);

	m_Device = device;
	CTexture::SetPath(EFFECTTEXTUREPATH);

	m_GemFile = fopen(filename,"rb");
	if(!m_GemFile)
		return false;

	// gem fil check
	memset(buffer,0,sizeof(char) * BUF_SIZE);
	fread(buffer,sizeof(char)*BUF_SIZE,1,m_GemFile);
	
	if(strcmp(buffer,GEM_HEADER) != 0) {
		sprintf(msg,"%s is Not Gem File",filename);
		MsgPrint(msg);
		return false;
	}
	// scene info load
	memset(buffer,0,sizeof(char) * BUF_SIZE);
	fread(buffer,sizeof(char)*BUF_SIZE,1,m_GemFile);
	
	token = strtok(buffer,"\n ");
	m_StartF = atoi(token);
	token = strtok(NULL,"\n ");
	m_EndF = atoi(token);
	token = strtok(NULL,"\n ");
	m_UnitF = atoi(token);
	token = strtok(NULL,"\n ");
	m_ObjectNum = atoi(token);
	token = strtok(NULL,"\n ");
	m_MatNum  = atoi(token);
	m_Mtl = new GemMaterial[m_MatNum];

	//material load
	for(i=0;i<m_MatNum;i++) {
		fread(&(m_Mtl[i]),sizeof(GemMaterial),1,m_GemFile);
		sub_mat = m_Mtl[i].m_SubNum;

		if(sub_mat) {
			m_Mtl[i].m_Sub = new GemMaterial[sub_mat];
			for(j=0;j<sub_mat;j++) {
				fread(&(m_Mtl[i].m_Sub[j]),sizeof(GemMaterial),1,m_GemFile);
			}
		}
	}
	//object load
	m_Object = new GemObject[m_ObjectNum];
	//subface space create
	m_Sub = new GemSubFace[m_ObjectNum];

	for(i=0;i<m_ObjectNum;i++) {
		float row[3] = {0.0f,0.0f,0.0f};

		//object number info load
		memset(buffer,0,sizeof(char) * BUF_SIZE);
		fread(buffer,sizeof(char)*BUF_SIZE,1,m_GemFile);
		token = strtok(buffer,"\n ");
		m_Object[i].m_bParent = (atoi(token)) > 0 ? true : false;
		token = strtok(NULL,"\n ");
		m_Object[i].m_CoordNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_FaceNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_Frame = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_MatId = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_MorphNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_PosKeyNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_RotKeyNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_ScaleKeyNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_VertexNum = atoi(token);
		token = strtok(NULL,"\n ");
		m_Object[i].m_VisKeyNum = atoi(token);
		//object name info load
		fread(m_Object[i].m_Name,sizeof(char) * BUF_SIZE,1,m_GemFile);
		//확장 기능 세팅
		if(strchr(m_Object[i].m_Name,'B')) {
			m_Object[i].m_Bil1 = true;
		}
		if(strchr(m_Object[i].m_Name,'C')) {
			m_Object[i].m_Bil2 = true;
		}
		if(strchr(m_Object[i].m_Name,'Y')) {
			m_Object[i].m_Bil3 = true;
		}
		if(strchr(m_Object[i].m_Name,'Z')) {
			m_Object[i].m_Zbuffer = true;	
		}
		if(strchr(m_Object[i].m_Name,'K')) {
			m_Object[i].m_Zenable = true;
		}
		if(strchr(m_Object[i].m_Name,'M')) {
			m_Object[i].m_Multy = true;
		}
		if(strchr(m_Object[i].m_Name,'T')) {
			m_Object[i].m_TexAni = true;
			char *ptr = strchr(m_Object[i].m_Name,'0');
			m_Object[i].m_AniTextureNum = atoi(ptr);
		}
		if(strchr(m_Object[i].m_Name,'R')) {
			m_Object[i].m_RandAni = true;
			char *ptr = strchr(m_Object[i].m_Name,'0');
			m_Object[i].m_AniTextureNum = atoi(ptr);
		}
		if(strchr(m_Object[i].m_Name,'E')) {
			if(strchr(m_Object[i].m_Name,'X')) {	
				m_Object[i].m_RandStartTexAni = true;
				char *ptr = strchr(m_Object[i].m_Name,'0');
				m_Object[i].m_AniTextureNum = atoi(ptr);
			}
		}
	
		if(strchr(m_Object[i].m_Name,'H')) {
			m_Object[i].m_bH = true;
			
		}
		if(strchr(m_Object[i].m_Name,'W')) {
			m_Object[i].m_bCull = true;
			
		}
		if(strchr(m_Object[i].m_Name,'O')) {
			m_Object[i].m_Opacity = true;
			
		}
		if(strchr(m_Object[i].m_Name,'J')) {
			m_Object[i].m_bDecal = true;
			m_bDecal = true;
		}
		if(m_Object[i].m_bParent)
			fread(m_Object[i].m_ParentName,sizeof(char) * BUF_SIZE,1,m_GemFile);
		//matrix load
		//row1
		fread(row,sizeof(float) * 3,1,m_GemFile);
		m_Object[i].m_TmMatrix._11 = row[0];
		m_Object[i].m_TmMatrix._12 = row[1];
		m_Object[i].m_TmMatrix._13 = row[2];
		//row2
		fread(row,sizeof(float) * 3,1,m_GemFile);
		m_Object[i].m_TmMatrix._21 = row[0];
		m_Object[i].m_TmMatrix._22 = row[1];
		m_Object[i].m_TmMatrix._23 = row[2];
		//row3
		fread(row,sizeof(float) * 3,1,m_GemFile);
		m_Object[i].m_TmMatrix._31 = row[0];
		m_Object[i].m_TmMatrix._32 = row[1];
		m_Object[i].m_TmMatrix._33 = row[2];
		//row4
		fread(row,sizeof(float) * 3,1,m_GemFile);
		m_Object[i].m_TmMatrix._41 = row[0];
		m_Object[i].m_TmMatrix._42 = row[1];
		m_Object[i].m_TmMatrix._43 = row[2];		
		//object vertex load
		
		if(m_Object[i].m_CoordNum) { 
			m_Object[i].m_Coord = new D3DXVECTOR2[(m_Object[i].m_CoordNum)];
			fread(m_Object[i].m_Coord,sizeof(D3DXVECTOR2),m_Object[i].m_CoordNum,m_GemFile);
		}
		if(m_Object[i].m_FaceNum) {
			m_Object[i].m_Face = new D3DXVECTOR4[m_Object[i].m_FaceNum];
			m_Object[i].m_CoordFace = new D3DXVECTOR3[m_Object[i].m_FaceNum];
			m_Object[i].m_Fnormal = new D3DXVECTOR3[m_Object[i].m_FaceNum];

			fread(m_Object[i].m_CoordFace,sizeof(D3DXVECTOR3),m_Object[i].m_FaceNum,m_GemFile);
			fread(m_Object[i].m_Face,sizeof(D3DXVECTOR4),m_Object[i].m_FaceNum,m_GemFile);
		}
		if(m_Object[i].m_VertexNum) {
			m_Object[i].m_Vert = new D3DXVECTOR3[(m_Object[i].m_VertexNum)];
			m_Object[i].m_Normal = new D3DXVECTOR3[(m_Object[i].m_VertexNum)];
			fread(m_Object[i].m_Normal,sizeof(D3DXVECTOR3),m_Object[i].m_VertexNum,m_GemFile);
			fread(m_Object[i].m_Vert,sizeof(D3DXVECTOR3),m_Object[i].m_VertexNum,m_GemFile);
		}
		if(m_Object[i].m_PosKeyNum) {
			m_Object[i].m_Pos = new GemPosKey[(m_Object[i].m_PosKeyNum)];
			fread(m_Object[i].m_Pos,sizeof(GemPosKey),m_Object[i].m_PosKeyNum,m_GemFile);
		}
		if(m_Object[i].m_RotKeyNum) {
			m_Object[i].m_Rot = new GemRotKey[(m_Object[i].m_RotKeyNum)];
			fread(m_Object[i].m_Rot,sizeof(GemRotKey),m_Object[i].m_RotKeyNum,m_GemFile);
		}
		if(m_Object[i].m_ScaleKeyNum) {
			m_Object[i].m_Scale = new GemScaleKey[(m_Object[i].m_ScaleKeyNum)];
			fread(m_Object[i].m_Scale,sizeof(GemScaleKey),m_Object[i].m_ScaleKeyNum,m_GemFile);
		}
		if(m_Object[i].m_VisKeyNum) {
			m_Object[i].m_Vis = new GemVisKey[(m_Object[i].m_VisKeyNum)];
			fread(m_Object[i].m_Vis,sizeof(GemVisKey),m_Object[i].m_VisKeyNum,m_GemFile);
		}
		//morphing data load
		if(m_Object[i].m_MorphNum) {
			LoadMorph(i);
		}
		if(m_Object[i].m_MatId >=0)
			if(m_Mtl[m_Object[i].m_MatId].m_SubNum)
				LoadSubFace(i);

	}
	//m_ObjectVertex = new LPDIRECT3DVERTEXBUFFER8[m_ObjectNum];
	VbufferNum  = m_ObjectNum;
	
	m_TexAni = new GemTexture[m_ObjectNum];
	m_iTotalVertexNums = 0;
	bool bDynamic = false;

	for(i = 0;i < m_ObjectNum; i++ )
	{
		int face_num = m_Object[i].m_FaceNum;
		m_Object[i].m_iStartBufferIndex = m_iTotalVertexNums;

		if(face_num > 0)
		{
			m_iTotalVertexNums += face_num * 3;
		}
		if(m_Object[i].m_MorphNum)
		{
			bDynamic = true;

		}

	}
//	m_bDynamic = bDynamic;
bDynamic = m_bDynamic = false;

	if(m_iTotalVertexNums > 0)
	{
		m_lpVertexBuffer = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_iTotalVertexNums * sizeof(GemVertex),D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,bDynamic);
	}
	else
	{
		m_lpVertexBuffer = NULL;

	}
	for(i =0; i < m_ObjectNum; i++ )
	{
		SetVertexBuffer(i);
		// TexAni or RandAni
		if(m_Object[i].m_TexAni || m_Object[i].m_RandAni || m_Object[i].m_RandStartTexAni)
			LoadTexAni(i);

	}
	/*
	for(i=0;i<m_ObjectNum;i++) {
		int face_num = m_Object[i].m_FaceNum;
		HRESULT hr = 0;

		if(m_Object[i].m_MorphNum) { 
			if(face_num > 0) {
				hr = m_Device->CreateVertexBuffer(face_num * sizeof(GemVertex) * 3,D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,D3DPOOL_DEFAULT,&(m_ObjectVertex[i]));
			}
			else {
				m_ObjectVertex[i] = NULL;		
			}
		}
		else {
			if(face_num > 0) {
				hr = m_Device->CreateVertexBuffer(face_num * sizeof(GemVertex) * 3,D3DUSAGE_WRITEONLY,
										D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,D3DPOOL_MANAGED,&(m_ObjectVertex[i]));
			}
			else {
				m_ObjectVertex[i] = NULL;
			}
		}
		if(hr<0)
			m_ObjectVertex[i] = NULL;

		SetVertexBuffer(i);
		// TexAni or RandAni
		if(m_Object[i].m_TexAni || m_Object[i].m_RandAni || m_Object[i].m_RandStartTexAni)
			LoadTexAni(i);
		
	}*/
	//multy uv flag read
	fread((int *)&m_bMulty,sizeof(int),1,m_GemFile);
	//Face Normal use
	for(int fn = 0;fn<m_ObjectNum;fn++) {

		fread((D3DXVECTOR3 *)m_Object[fn].m_Fnormal,sizeof(D3DXVECTOR3),m_Object[fn].m_FaceNum,m_GemFile);
//		03.09.09 수정
//		SetVertexBuffer(fn);
	}
	if(m_GemFile) {
		fclose(m_GemFile);
		m_GemFile = NULL;
	}
	//multy UV ani use
	if(m_bMulty) {
		m_Multy = new CGemRender();

		char tmp_buf[BUF_SIZE] = {0};
		strcpy(tmp_buf,filename);
		char *ptr = strrchr(tmp_buf,'.');
		*(ptr) = 'u';
		ptr++;
		*(ptr) = 'v';
		ptr++;
		*(ptr) = '.';
		ptr++;
		*(ptr) = 'g';
		ptr++;
		*(ptr) = 'e';
		ptr++;
		*(ptr) = 'm';
		ptr++;
		*(ptr) = '\0';

		LoadUvFile(tmp_buf);

	}
	if(bVisibility)
		LoadTexture();
	return true;	
}
//tex ani 나 rand ani 
void CGemRender::LoadTexAni(int object_index) {
	int texnum = m_Object[object_index].m_AniTextureNum;
	int i;
	int mtl_id = m_Object[object_index].m_MatId;

	char buf[BUF_SIZE] = {0};
	char filename[BUF_SIZE]={0};
	char path[BUF_SIZE] = {0};
	
	char *ptr = NULL;
	
	memcpy(buf,m_Mtl[mtl_id].m_TextureName,sizeof(char)*BUF_SIZE);
	ptr = strrchr(buf,'\\');
	ptr++;
	strcpy(filename,ptr);
	*(ptr) = '\0';
	strcpy(path,buf);

	
	if(texnum) {
		m_TexAni[object_index].TexNum = texnum;
		m_TexAni[object_index].m_Tex = new CTexture[texnum];
		
	
		for(i=0;i <texnum;i++) {
			int length = strlen(filename);
			//십자리
			if(i / 10)
				filename[length - 6] = '0'+ i/10;
			else
				filename[length - 6] = '0';
			if(i % 10)
				filename[length - 5] = '0'+ (i % 10);
			else
				filename[length - 5] = '0';

			memset(buf,0,sizeof(char)*BUF_SIZE);
			//strcpy(buf,path);
			strcat(buf,filename);
			
			m_TexAni[object_index].m_Tex[i].Load(buf);
		}
	}
}

void CGemRender::SetCullZbuffer(bool t) {
	int i;
	for(i=0;i<m_ObjectNum;i++) {
		m_Object[i].m_bCull = t;
		m_Object[i].m_Zbuffer = t;
	}

}
void CGemRender::SetEffectPos(float x,float y,float z) {
	m_Pos.x = x;
	m_Pos.y = y;
	m_Pos.z = z;
	
}
// TexNum : Effect Mesh 마다 다른 current Frame 이나 현제 프레임 같은것을 기억하고 있는 list
// CX3DEffectMesh 에 존재
// TexNum[ 0 ~ ObjectNum - 1] : 텍스쳐 애니 를 사용한 mesh의 현재 택스쳐 번호 
// TexNun[ObjectNum] : Mesh Before Frame
// TexNum[ObjectNum + 1] : Mesh Current Frame
// TexNum[ObjectNum + 2] : 렌덤 스타트 애니시에 처음 텍스쳐 프레임 세팅 되었는가를 체크하는 체크 플래그
// TexNum[ObjectNum + 3] : 텍스쳐 애니시에 바로 전에 텍스쳐가 갱신되었던 프레임 기억.

void CGemRender::SetCurrentFrame(float f,int *TexNum,DWORD *dwTick,bool *bFirst,bool bUpdate) {

	bool bObjectNum3 = false;
	bool bObjectNum2 = false;

	if((*bFirst) == false) {
		TexNum[m_ObjectNum] = 0;
		(*bFirst) = true;
	}
	else
		TexNum[m_ObjectNum] = TexNum[m_ObjectNum +1];
	
	m_CurrentF = f;

	double tmp_value = m_CurrentF * m_UnitF;

	if(m_bLoop) {
		if(tmp_value >= m_EndF) {
			m_CurrentF = (float)m_StartF;
			tmp_value = m_StartF;
		}
	}
	else if(tmp_value >= m_EndF) {
		m_CurrentF = (float)m_EndF / (float)m_UnitF;
		tmp_value = m_EndF;
	}
	
	TexNum[m_ObjectNum + 1] = m_CurrentF;
	
	if(m_bLoop || tmp_value <= m_EndF) {				// 루핑이거나 끝프레임에 도착하지 않았을때 
		for(int i = 0;i<m_ObjectNum;i++) {
			if(m_Object[i].m_RandAni) {									// Randow Texture flag
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame) {		// 현재 프레임이  시작 프레임 보다 클때
					// 현재 프레임(m_CurrentF)에서 전의 업데이트 프레임(TexNum[m_ObjectNum + 3 ])을 뺀것이 Ani Change Frame 보다 크다면
					if(bUpdate && (((int)fabs(m_CurrentF - TexNum[m_ObjectNum + 3]) >= (int)m_Object[i].m_AniChangeFrame))) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						// Randow
						TexNum[i] = m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
						// 업데이트 갱신 flag true
						bObjectNum3 = true;
						

					}
					else {
						m_Object[i].m_CurrentAniTexture = TexNum[i];
					}
				}
				
			}
			else if(m_Object[i].m_TexAni) {								// Normal Texture Ani Flag
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame) {		// 현재 프레임이  시작 프레임 보다 클때
					// 현재 프레임(m_CurrentF)에서 전의 업데이트 프레임(TexNum[m_ObjectNum + 3 ])을 뺀것이 Ani Change Frame 보다 크다면	
					if(bUpdate && (((int)(fabs(m_CurrentF - TexNum[m_ObjectNum + 3])) >= (int)m_Object[i].m_AniChangeFrame))) {
						// 업데이트 갱신 flag true
						bObjectNum3 = true;
						
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						m_Object[i].m_CurrentAniTexture = TexNum[i];
						//TexNum[i]++;
						// 현재 업데이트 될 텍스쳐의 갯수에 따른 업데이트
						TexNum[i]+= (int)fabs(m_CurrentF - TexNum[m_ObjectNum + 3]) / (int)m_Object[i].m_AniChangeFrame;

						// Looping 이나 world effect(m_bClearBuffer == false) 의 경우 처리
						if(/*(m_bLoop || !m_bClearBuffer) && */(m_Object[i].m_AniTextureNum <= TexNum[i]/*m_Object[i].m_CurrentAniTexture*/)) {
							m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
							m_Object[i].m_CurrentAniTexture = 0;
							TexNum[i] = 0;
							//(*bFirst) = false;
						}
					
					}
					else {	// 현재 업데이트 시에는 업데이트 없음

						if(m_Object[i].m_AniTextureNum <= TexNum[i]) {
							//if(!m_bClearBuffer)
								TexNum[i] = 0;
							//else 
							//	TexNum[i] = m_Object[i].m_AniTextureNum - 1;
						}
						
						m_Object[i].m_CurrentAniTexture = TexNum[i];
					}
				}
					
			}
			else if(m_Object[i].m_RandStartTexAni) {			// Random Start Texture Ani
			
				//가장 처음 update시 시작프레임 setting
				// 첫번째 Update 후에 TexNum[m_ObjectNum + 2]  = 0 setting 해 준다
				if(TexNum[m_ObjectNum + 2] < 0) { 
					// Random Texture Start 번호
					bObjectNum2 = true;
					m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
					TexNum[i] = m_Object[i].m_CurrentAniTexture;
					

				}
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame) {	// 현재 프레임이  시작 프레임 보다 클때
					// 현재 프레임(m_CurrentF)에서 전의 업데이트 프레임(TexNum[m_ObjectNum + 3 ])을 뺀것이 Ani Change Frame 보다 크다면	
					if(bUpdate && (((int)fabs(m_CurrentF - TexNum[m_ObjectNum + 3]) >= (int)m_Object[i].m_AniChangeFrame))) {
						// 갱신 flag true setting
						bObjectNum3 = true;

						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						m_Object[i].m_CurrentAniTexture = TexNum[i];
						// 현재 업데이트 될 텍스쳐의 갯수에 따른 업데이트
						TexNum[i]+= (int)fabs(m_CurrentF - TexNum[m_ObjectNum + 3]) / (int)m_Object[i].m_AniChangeFrame;
						
						// Looping 이나 world effect(m_bClearBuffer == false) 의 경우 처리
						if(/*(m_bLoop || !m_bClearBuffer) && */(m_Object[i].m_AniTextureNum <= m_Object[i].m_CurrentAniTexture)) {
							m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
		
							m_Object[i].m_CurrentAniTexture = 0;
							TexNum[i] = 0;
						}
						
					}
					else {				// Texture Ani Update 없음
						if(m_Object[i].m_AniTextureNum <= TexNum[i]) {
							//if(!m_bClearBuffer)
							TexNum[i] = 0;
							
						}

						m_Object[i].m_CurrentAniTexture = TexNum[i];
					}
				}
			}
		}
	}
	if(bObjectNum3 == true)					// 이번 update시 Texture Ani가 Update 되었다면.
		TexNum[m_ObjectNum + 3] = m_CurrentF;
	if(bObjectNum2 == true)					// Random Start Ani 초기 texture Number Setting 되었다면
		TexNum[m_ObjectNum + 2] = 0;		// Rand Start Ani setting 표시
	
}
void CGemRender::SetCurrentFrame(float f){
	

	m_CurrentF = f;

	double tmp_value = m_CurrentF * m_UnitF;

	if(m_bLoop) {
		if(tmp_value >= m_EndF) {
			m_CurrentF = (float)m_StartF;
		}
	}
	else if(tmp_value >= m_EndF) {
		m_CurrentF = (float)m_EndF / (float)m_UnitF;
	}
	
	if(m_bLoop || tmp_value < m_EndF) {				// 루핑이거나 끝프레임에 도착하지 않았을때 
		for(int i = 0;i<m_ObjectNum;i++) {
			if(m_Object[i].m_RandAni) {
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
					if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame) % (int)m_Object[i].m_AniChangeFrame)) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
					}
						
			}
			else if(m_Object[i].m_TexAni) {
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
					if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame)% (int)m_Object[i].m_AniChangeFrame)) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						(m_Object[i].m_CurrentAniTexture)++;
					
					}
					if(m_Object[i].m_AniTextureNum <= m_Object[i].m_CurrentAniTexture) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						m_Object[i].m_CurrentAniTexture = 0;
					}
			}
			else if(m_Object[i].m_RandStartTexAni) {
				if(!m_Object[i].m_RandStartSetting) { //가장 처음 update시 시작프레임 setting
					m_Object[i].m_RandStartSetting = true;
					m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
				}
				if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
					if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame)% (int)m_Object[i].m_AniChangeFrame)) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						(m_Object[i].m_CurrentAniTexture)++;
					
					}
					if(m_Object[i].m_AniTextureNum <= m_Object[i].m_CurrentAniTexture) {
						m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
						m_Object[i].m_CurrentAniTexture = 0;
					}

			}
		}
	}
}
void CGemRender::SetStartFrame(float s) {
	m_StartF = (int)s;
}
void CGemRender::SetColor(int object_index,int r,int g,int b) {
	m_Object[object_index].m_Red = r;
	m_Object[object_index].m_Green = g;
	m_Object[object_index].m_Blue = b;
	m_Object[object_index].m_Color = D3DCOLOR_ARGB(255,r,g,b);
	
}
void CGemRender::SetEffectPos(D3DXVECTOR3 pos) {
	m_Pos = pos;
}
void CGemRender::SetPickObject(int index) {
	if(index <m_ObjectNum)
		m_PickObject = index;
}
void CGemRender::SetScale(float x,float y,float z) {
	m_Scale._11 = x;
	m_Scale._22 = y;
	m_Scale._33 = z;
}

//texture ani chage frame set
void CGemRender::SetChangeAniFrame(int object_index,float f) {
	m_Object[object_index].m_AniChangeFrame = f;
}
void CGemRender::SetStartTexAniFrame(int object_index,float f){
	m_Object[object_index].m_StartTexAniFrame = f;
}
void CGemRender::LoadSubFace(int object_index) { // sub material 사용한 object의 처리
	int i,j,k=0;
	int tmp = 0;
	int num = 0;

	int mat_num = m_Mtl[(m_Object[object_index].m_MatId)].m_SubNum;
	int face_num = m_Object[object_index].m_FaceNum;
	
	//잘못된 mat num 수정
	for(i=0;i<face_num;i++) {
		if(mat_num <=(int)m_Object[object_index].m_Face[i].w)
			m_Object[object_index].m_Face[i].w = 0;
	}
	
	// object sub mat 별로 분리
	for(i=0;i<face_num;i++) {
		if(i == 0)	{
			j = (int)m_Object[object_index].m_Face[i].w;
			num++;
		}
		else if(j != (int)m_Object[object_index].m_Face[i].w) {
			num++;
			j = (int)m_Object[object_index].m_Face[i].w;
		}
	}	

	m_Sub[object_index].sub_num = num;
	m_Sub[object_index].sub = new int[num];
	m_Sub[object_index].sub_mat = new int[num];
	memset(m_Sub[object_index].sub,0,sizeof(int) * num);
	memset(m_Sub[object_index].sub_mat,0,sizeof(int) * num);

	// 실제 갯수 얻기
	for(i=0;i<face_num;i++) {
		if(i == 0) {
			m_Sub[object_index].sub[tmp]++;
			m_Sub[object_index].sub_mat[tmp] = (int)m_Object[object_index].m_Face[i].w;
			j= (int)m_Object[object_index].m_Face[i].w;
		}
		else if(j !=(int)m_Object[object_index].m_Face[i].w) {
			tmp++;
			m_Sub[object_index].sub[tmp]++;
			m_Sub[object_index].sub_mat[tmp] = (int)m_Object[object_index].m_Face[i].w;
			j = (int)m_Object[object_index].m_Face[i].w;
		}
		else 
			m_Sub[object_index].sub[tmp]++;
		
	}

}
void CGemRender::LoadTexture() {
	int i,j;
	int tex_count = 0;

	m_Texture = new GemTexture[m_MatNum];
	char *ptr = NULL;
	for(i=0;i<m_MatNum;i++) {
		//sub material 없는 경우
		if(!m_Mtl[i].m_SubNum) {	
			m_Texture[i].m_Tex = new CTexture[1];
			ptr = strrchr(m_Mtl[i].m_TextureName,'\\');
			ptr++;


			m_Texture[i].m_Tex->Load(ptr);
			m_Texture[i].TexNum++;
		}
		else {
			m_Texture[i].m_Tex = new CTexture[(m_Mtl[i].m_SubNum)];
			for(j=0;j<m_Mtl[i].m_Sub[j].m_SubNum;j++) {
				ptr = strrchr(m_Mtl[i].m_TextureName,'\\');	
				ptr++;
				m_Texture[i].m_Tex[j].Load(ptr);
				m_Texture[i].TexNum++;
			
			}
		}
							
	}

}
bool CGemRender::VertexInterpolation(int object_index) {
	
	float now_frame = m_CurrentF * m_UnitF;

	D3DXVECTOR3 tmp(0.0f,0.0f,0.0f);
	D3DXVECTOR3 move_vector(0.0f,0.0f,0.0f);
	D3DXVECTOR3 next_f(0.0f,0.0f,0.0f);
	D3DXVECTOR3 before_f(0.0f,0.0f,0.0f);
	
	D3DXVECTOR2 next_ft(0.0f,0.0f);
	D3DXVECTOR2 before_ft(0.0f,0.0f);
	D3DXVECTOR2 tmp_t(0.0f,0.0f);

	D3DXVECTOR3 next_n(0.0f,0.0f,0.0f);
	D3DXVECTOR3 before_n(0.0f,0.0f,0.0f);
	D3DXVECTOR3 tmp_n(0.0f,0.0f,0.0f);
	int next_frame = 0;
	int before_frame = 0;
	int i,j;
	double interpol = 0.0;

	//if(!m_Object[object_index].m_Morph[0].m_FaceNum || !m_Object[object_index].m_Morph[0].m_VertexNum)
	//	return false;
	// 지금 현제 프레임의 morph 데이터 위치 찾기
	
	for(i=0;i<m_Object[object_index].m_MorphNum;i++) {
		if((float)(m_Object[object_index].m_Morph[i].m_Frame) >= now_frame) {
			next_frame = i;
			before_frame = i-1;
			break;
		}
	}
	if(i !=0 && (i < (m_Object[object_index].m_MorphNum))) {		
		

		interpol = (double)(now_frame - (float)m_Object[object_index].m_Morph[before_frame].m_Frame) / (double)((float)m_Object[object_index].m_Morph[next_frame].m_Frame - m_Object[object_index].m_Morph[before_frame].m_Frame);
	
		if(interpol <0.0f)
			interpol = 0.0f;
		else if(interpol >1.0f)
			interpol = 1.0f;
		//coord load
		for(j=0;j<m_Object[object_index].m_CoordNum;j++) {
			if(!m_Object[object_index].m_Morph[next_frame].m_Coord)
					return false;
			//texcoord
			memcpy(&next_ft,&(m_Object[object_index].m_Morph[next_frame].m_Coord[j]),sizeof(D3DXVECTOR2));
			memcpy(&before_ft,&(m_Object[object_index].m_Morph[before_frame].m_Coord[j]),sizeof(D3DXVECTOR2));
			
			tmp_t.x = next_ft.x - before_ft.x;
			tmp_t.y = next_ft.y - before_ft.y;
			
			m_Object[object_index].m_Coord[j].x = before_ft.x + tmp_t.x * (float)interpol;
			m_Object[object_index].m_Coord[j].y = before_ft.y + tmp_t.y * (float)interpol;
		
		}
		//vertex load	
		for(j=0;j<m_Object[object_index].m_VertexNum;j++) {
			//vertex
			if(!m_Object[object_index].m_Morph[next_frame].m_Vert)
				return false;
			
			if(!m_Object[object_index].m_Morph[next_frame].m_Normal)
				return false;
			
			memcpy(&next_f,&(m_Object[object_index].m_Morph[next_frame].m_Vert[j]),sizeof(D3DXVECTOR3));
			memcpy(&before_f,&(m_Object[object_index].m_Morph[before_frame].m_Vert[j]),sizeof(D3DXVECTOR3));
			
			//normal
			memcpy(&next_n,&(m_Object[object_index].m_Morph[next_frame].m_Normal[j]),sizeof(D3DXVECTOR3));
			memcpy(&before_n,&(m_Object[object_index].m_Morph[before_frame].m_Normal[j]),sizeof(D3DXVECTOR3));	
			
			tmp_n.x = next_n.x - before_n.x;
			tmp_n.y = next_n.y - before_n.y;
			tmp_n.z = next_n.z - before_n.z;
			
			
		
			
			m_Object[object_index].m_Normal[j].x = before_n.x + tmp_n.x * (float)interpol;
			m_Object[object_index].m_Normal[j].y = before_n.y + tmp_n.y * (float)interpol;
			m_Object[object_index].m_Normal[j].z = before_n.z + tmp_n.z * (float)interpol;
			//////////////////////
			
		
			if(m_Object[object_index].m_bH) {
				////linear interpolation.
				tmp.x = next_f.x - before_f.x;
				tmp.y = next_f.y - before_f.y;
				tmp.z = next_f.z - before_f.z;
				
				m_Object[object_index].m_Vert[j].x = before_f.x + tmp.x * (float)interpol;
				m_Object[object_index].m_Vert[j].y = before_f.y + tmp.y * (float)interpol;
				m_Object[object_index].m_Vert[j].z = before_f.z + tmp.z * (float)interpol;
			
			/*	//hermite interpol
				m0 = (((1.0f + 1.7f)/2.0f) * ((p2 - p1) + (p3 - p2)));
				m1 = (((1.0f + 1.7f)/2.0f) * ((p3 - p2) + (p4 - p3)));

				m_Object[object_index].m_Vert[j] = (((2 * t3) - (3 * t2) + 1)* p2) + 
												   ((t3 - (2 * t2) + t) * m0) + 
												   ((t3 - t2) * m1) +
												   (((-(2 * t3)) + (3 * t2)) * p3);	
			*/
					
			}
			else {
				D3DXVECTOR3 p1(0.0f,0.0f,0.0f);
				D3DXVECTOR3 p2(0.0f,0.0f,0.0f);
				D3DXVECTOR3 p3(0.0f,0.0f,0.0f);
				D3DXVECTOR3 p4(0.0f,0.0f,0.0f);
				D3DXVECTOR3 m0(0.0f,0.0f,0.0f);
				D3DXVECTOR3 m1(0.0f,0.0f,0.0f);

					
				if(i==1) {
					p1 = m_Object[object_index].m_Morph[before_frame].m_Vert[j];
					p2 = m_Object[object_index].m_Morph[before_frame].m_Vert[j];
				}
				else {
					p1 = m_Object[object_index].m_Morph[before_frame -1].m_Vert[j];
					p2 = before_f;
				}
				if((i+1 == (m_Object[object_index].m_MorphNum)) || !(m_Object[object_index].m_Morph[next_frame + 1].m_Vert)) {	
					p3 = next_f;
					p4 = next_f;
				}
				else {
					p3 = next_f;	
					p4 = m_Object[object_index].m_Morph[next_frame + 1].m_Vert[j];
				}
				double t = interpol;
				double t2 = t * t;
				double t3 = t*t*t;
				// Catmull rom spline
			
				m_Object[object_index].m_Vert[j] = p1 * ((-0.5f)*t3 + t2 + (-0.5)*t) +
												   p2 * ((1.5f)*t3 + (-2.5f)*t2 + 1.0f) +
												   p3 * ((-1.5f)*t3 + (2.0f)*t2 + (0.5f)*t) +
												   p4 * ((0.5f)*t3 + (-0.5f)*t2);

			}
		//////////////////////////
		}
		m_Object[object_index].m_bInterpol = true;
	}
	else  {
		if(!m_bCash && !m_bLoop)
			m_Object[object_index].m_bInterpol = false;
		else
			return false;
	}
		//return false;
	return true;
	
}
void CGemRender::RotAni(int object_index,D3DXMATRIX &AniM) {

	//static D3DXQUATERNION global(0.0f,0.0f,0.0f,1.0f);
	D3DXQUATERNION tmp1(0.0f,0.0f,0.0f,1.0f);
	D3DXQUATERNION before(0.0f,0.0f,0.0f,1.0f);
	D3DXQUATERNION after(0.0f,0.0f,0.0f,1.0f);

	D3DXMATRIX trans;
	
	D3DXMatrixIdentity(&trans);
	
	float frame = m_CurrentF * m_UnitF;

	double interpol = 0.0;
	int k=0,i;
	before.x = 0.0f;
	before.y = 0.0f;
	before.z = 0.0f;
	before.w = 1.0f;

	after.x = 0.0f;
	after.y = 0.0f;
	after.z = 0.0f;
	after.w = 1.0f;

	for(k=0;k<m_Object[object_index].m_RotKeyNum;k++) {
		if((float)(m_Object[object_index].m_Rot[k].frame) >=frame)
			break;
	}
			
	if(k !=0) {
		
		if(k >= m_Object[object_index].m_RotKeyNum) { 
			k-=1;
			interpol = 1.0;
	
		}
		else { 
			interpol = (double)(frame - (float)m_Object[object_index].m_Rot[k-1].frame)/(double)((float)m_Object[object_index].m_Rot[k].frame - (float)m_Object[object_index].m_Rot[k-1].frame);
		}
		
		if(interpol >1.0)
			interpol = 1.0;
		else if(interpol <0.0)
			interpol = 0.0;

		for(i=0;i<k;i++) {
			tmp1.x = m_Object[object_index].m_Rot[i].rot.x;
			tmp1.y = m_Object[object_index].m_Rot[i].rot.y;
			tmp1.z = m_Object[object_index].m_Rot[i].rot.z;
			tmp1.w = m_Object[object_index].m_Rot[i].rot.w;
			before *=tmp1;
		}
		after = before;

		tmp1.x = m_Object[object_index].m_Rot[k].rot.x;
		tmp1.y = m_Object[object_index].m_Rot[k].rot.y;
		tmp1.z = m_Object[object_index].m_Rot[k].rot.z;
		tmp1.w = m_Object[object_index].m_Rot[k].rot.w;

		after *=tmp1;

	
	
		D3DXQuaternionSlerp(&global,&(before),&(after),(float)interpol);
	
	}
	else {
		tmp1.x = m_Object[object_index].m_Rot[0].rot.x;
		tmp1.y = m_Object[object_index].m_Rot[0].rot.y;
		tmp1.z = m_Object[object_index].m_Rot[0].rot.z;
		tmp1.w = m_Object[object_index].m_Rot[0].rot.w;

		D3DXQuaternionSlerp(&global,&(tmp1),&(tmp1),0.0f);
	}
	D3DXMatrixRotationQuaternion(&trans,&global);
	D3DXMatrixMultiply(&AniM,&AniM,&trans);
	/*	// y축이 반대로 되므로 변환
		Model.pobject[object_index].rot_quatkey[k-1].y = - Model.pobject[object_index].rot_quatkey[k-1].y;
		Model.pobject[object_index].rot_quatkey[k].y = - Model.pobject[object_index].rot_quatkey[k].y;

		D3DXQuaternionSlerp(&global,&(Model.pobject[object_index].rot_quatkey[k-1]),&(Model.pobject[object_index].rot_quatkey[k]),interpol);
		// 복구
		Model.pobject[object_index].rot_quatkey[k-1].y = - Model.pobject[object_index].rot_quatkey[k-1].y;
		Model.pobject[object_index].rot_quatkey[k].y = - Model.pobject[object_index].rot_quatkey[k].y;

	}	
	else 
		D3DXQuaternionSlerp(&global,&(Model.pobject[object_index].rot_quatkey[k]),&(Model.pobject[object_index].rot_quatkey[k]),0);

	D3DXMatrixRotationQuaternion(&trans,&global);
	*/			
				
}
bool CGemRender::VisAni(int object_index){
	float frame = m_CurrentF * m_UnitF;
	float tmp_vis = 0.0f;
	float s_vis = 0.0f;
	int k=0;
	double interpol  = 0.0;

	for(k=0;k<m_Object[object_index].m_VisKeyNum;k++) {
		if((float)m_Object[object_index].m_Vis[k].frame >=frame)
			break;
	}

	if(k >= m_Object[object_index].m_VisKeyNum) {  
			k-=1;
	}
	if(k != 0) {
		interpol = (double)(frame - (float)m_Object[object_index].m_Vis[k-1].frame)/(double)((float)m_Object[object_index].m_Vis[k].frame - (float)m_Object[object_index].m_Vis[k-1].frame);
					
		if(k >= m_Object[object_index].m_VisKeyNum) {  
			k-=1;
			interpol = 1.0;
		}
		if(interpol >1.0)
			interpol = 1.0;
		else if(interpol <0.0)
			interpol = 0.0;
		
		tmp_vis = m_Object[object_index].m_Vis[k].vis - m_Object[object_index].m_Vis[k-1].vis;

		s_vis = m_Object[object_index].m_Vis[k-1].vis + tmp_vis * (float)interpol;
		
		if(s_vis >1.0f)
			s_vis = 1.0f;
		else if(s_vis <0.0f)
			s_vis = 0.0f;

		m_Object[object_index].m_Color = D3DCOLOR_COLORVALUE((m_Object[object_index].m_Red) / 255.0f,(m_Object[object_index].m_Green) / 255.0f,(m_Object[object_index].m_Blue) / 255.0f,s_vis);
		if(s_vis <= 0.0f)
			return false;
	}
	else {
		m_Object[object_index].m_Color = D3DCOLOR_COLORVALUE((m_Object[object_index].m_Red) / 255.0f,(m_Object[object_index].m_Green) / 255.0f,(m_Object[object_index].m_Blue) / 255.0f,m_Object[object_index].m_Vis[0].vis);
		if(m_Object[object_index].m_Vis[0].vis <= 0.0f)
			return false;
	}
	return true;
}
void CGemRender::PosAni(int object_index,D3DXMATRIX &AniM) {
	
	int k = 0;
	float frame = m_CurrentF * m_UnitF;

	double interpol = 0.0;
	D3DXVECTOR3 tmp_pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 model_pos(0.0f,0.0f,0.0f);

	for(k=0;k<m_Object[object_index].m_PosKeyNum;k++) {
		if((float)m_Object[object_index].m_Pos[k].frame >=frame)
			break;
	}
			
	if(k != 0) {
					
		
					
		if(k >= m_Object[object_index].m_PosKeyNum) {  
			k-=1;
			interpol = 1.0;
		}
		else
			interpol = (double)(frame - (float)m_Object[object_index].m_Pos[k-1].frame)/(double)((float)m_Object[object_index].m_Pos[k].frame - (float)m_Object[object_index].m_Pos[k-1].frame);
		
		if(interpol >1.0)
			interpol = 1.0;
		else if(interpol <0.0)
			interpol = 0.0;
		if(k!=0) {
			if(!m_Object[object_index].m_Pos[k].bez) {	
				tmp_pos.x =	m_Object[object_index].m_Pos[k].pos.x - m_Object[object_index].m_Pos[k-1].pos.x;
				tmp_pos.y =	m_Object[object_index].m_Pos[k].pos.y - m_Object[object_index].m_Pos[k-1].pos.y;
				tmp_pos.z =	m_Object[object_index].m_Pos[k].pos.z - m_Object[object_index].m_Pos[k-1].pos.z;
				
				model_pos.x = m_Object[object_index].m_Pos[k-1].pos.x + tmp_pos.x*(float)interpol;
				model_pos.y = m_Object[object_index].m_Pos[k-1].pos.y + tmp_pos.y*(float)interpol;
				model_pos.z = m_Object[object_index].m_Pos[k-1].pos.z + tmp_pos.z*(float)interpol;
			}
			else { // bez pos 
				float t = (float)interpol;
				float t3 = t * t *t ;
				float t2 = t*t;

				model_pos = (( (2*t3) -(3*t2) +1)*(m_Object[object_index].m_Pos[k-1].pos)) +
							((t3 -(2*t2)+t)*(m_Object[object_index].m_Pos[k-1].outtan)) + 
							((t3 - t2)*(m_Object[object_index].m_Pos[k].intan))+
							((-2*t3 + 3*t2)*(m_Object[object_index].m_Pos[k].pos));

			}
		}
		else {
			model_pos.x = m_Object[object_index].m_Pos[0].pos.x;
			model_pos.y = m_Object[object_index].m_Pos[0].pos.y;
			model_pos.z = m_Object[object_index].m_Pos[0].pos.z;
		}
	}
	else { //k==0
		model_pos.x = m_Object[object_index].m_Pos[0].pos.x;
		model_pos.y = m_Object[object_index].m_Pos[0].pos.y;
		model_pos.z = m_Object[object_index].m_Pos[0].pos.z;
	}
	// 회전 이동
	if(m_AxisSet) {
		if(m_Object[object_index].m_Bil1 || m_Object[object_index].m_Bil3) {
			D3DXMATRIX tm_axis;

			D3DXMATRIX axismat;
			D3DXMatrixIdentity(&axismat);
			D3DXMatrixIdentity(&tm_axis);
			tm_axis._41 = model_pos.x;
			tm_axis._42 = model_pos.y;
			tm_axis._43 = model_pos.z;

			D3DXMatrixRotationQuaternion(&axismat,&m_Axis);
			D3DXMatrixMultiply(&tm_axis,&tm_axis,&axismat);
			model_pos.x = tm_axis._41;
			model_pos.y = tm_axis._42;
			model_pos.z = tm_axis._43;

		}
	
	}
	AniM._41 = model_pos.x;
	AniM._42 = model_pos.y;
	AniM._43 = model_pos.z;
}
void CGemRender::ScaleAni(int object_index,D3DXMATRIX &AniM) {

	int k=0;
	double interpol = 0.0;
	float frame = m_CurrentF * m_UnitF;

	D3DXVECTOR3 tmp_scale(0.0f,0.0f,0.0f);
	D3DXVECTOR3 model_scale(0.0f,0.0f,0.0f);
	
	for(k = 0;k<m_Object[object_index].m_ScaleKeyNum;k++) {
		if((float)(m_Object[object_index].m_Scale[k].frame) >= frame)
			break;
	}
	
	if(k != 0) {
		
		if(k >= m_Object[object_index].m_ScaleKeyNum) {  
			k-=1;
			interpol = 1.0;
		}
		else
			interpol = (double)(frame - (float)m_Object[object_index].m_Scale[k-1].frame)/(double)((float)m_Object[object_index].m_Scale[k].frame - (float)m_Object[object_index].m_Scale[k-1].frame);
					
		if(interpol >1.0)
			interpol = 1.0;
			
		else if(interpol <0.0)
			interpol = 0.0;
		if(k != 0) {
			tmp_scale.x = (m_Object[object_index].m_Scale[k].scale.x) - (m_Object[object_index].m_Scale[k-1].scale.x);
			tmp_scale.y = (m_Object[object_index].m_Scale[k].scale.y) - (m_Object[object_index].m_Scale[k-1].scale.y);
			tmp_scale.z = (m_Object[object_index].m_Scale[k].scale.z) - (m_Object[object_index].m_Scale[k-1].scale.z);	
			model_scale.x =	m_Object[object_index].m_Scale[k-1].scale.x + (tmp_scale.x * (float)interpol);
			model_scale.y = m_Object[object_index].m_Scale[k-1].scale.y + (tmp_scale.y * (float)interpol);
			model_scale.z = m_Object[object_index].m_Scale[k-1].scale.z + (tmp_scale.z * (float)interpol);
		}
		else {
			model_scale.x = m_Object[object_index].m_Scale[0].scale.x;
			model_scale.y =	m_Object[object_index].m_Scale[0].scale.y;
			model_scale.z = m_Object[object_index].m_Scale[0].scale.z;
		}
	}
	else { //k == 0 
		model_scale.x = m_Object[object_index].m_Scale[0].scale.x;
		model_scale.y =	m_Object[object_index].m_Scale[0].scale.y;
		model_scale.z = m_Object[object_index].m_Scale[0].scale.z;
	
	}
	AniM._11 = (float)fabs(model_scale.x);
	AniM._22 = (float)fabs(model_scale.y);
	AniM._33 = (float)fabs(model_scale.z);


}

void CGemRender::CreateAniTm(int i,D3DXMATRIX &world,D3DXMATRIX &AniMatrix){
	int kt = 0;
	int j;
	bool loop = true;
	// vertex 좌표는 월드 이므로 ani key 값이 있으면 local 좌표로 변환뒤 적용 해야 한다.
	// 왜냐하면 ani key 값은 local 에 맞게 되어있으므로 그렇다.
	// local 에 ani key 를 곱하면 world 좌표나 상위좌표가가 된다

	//Inverse TM 을 곱하므로 local 좌표를 구할수 있다
	//자신의 키값이 없어도 상위( parent)의 영향을 받을 수 있으므로 모두 inverse 를 곱해 생각한다.
			
	D3DXMatrixIdentity(&AniMatrix);
	// 자신의 Tm 복사 
	D3DXMATRIX Tm = m_Object[i].m_TmMatrix;
	D3DXMATRIX ParentTm;
	D3DXMATRIX inverse;
	D3DXMATRIX invlocal;

	D3DXMatrixIdentity(&inverse);
	D3DXMatrixIdentity(&ParentTm);

	//D3DXMatrixInverse(&inverse,NULL,&(m_Object[i].m_TmMatrix));
	D3DXMatrixInverse(&invlocal,NULL,&(m_Object[i].m_TmMatrix));

	// local matrix get.
	//D3DXMatrixMultiply(&world,&world,&inverse);

	//ani matrix set
	// 키가 없을시 TM에서 가지고 온다
	//////////////////////////////////////////////////////////
	// TM은 부모가 있을시 부모의 TM 의 Inverse 를 곱해야 부모 local로 
	// 가서 계층 애니가 성립한다.
	kt = i;
	if(m_Object[kt].m_bParent) {
		loop=true;
		while(loop) {
			for(j=0;j<m_ObjectNum;j++) {
				if(!strcmp(m_Object[kt].m_ParentName,m_Object[j].m_Name)) {
					
					ParentTm = m_Object[j].m_TmMatrix;
					D3DXMatrixInverse(&inverse,NULL,&ParentTm);
					D3DXMatrixMultiply(&Tm,&Tm,&inverse);
					// 2004.08.02 wizardbug 수정
					D3DXMatrixMultiply(&world,&world,&ParentTm);

					kt = j;
					break;

				}
			}
			if(j == m_ObjectNum)
				loop = false;
			
		}
	}
	D3DXMatrixMultiply(&world,&world,&invlocal);
	//////////////////////////////////////////////////////////

	if(m_Object[i].m_ScaleKeyNum <=0) {
	//	AniMatrix._11 = Tm._11;
	//	AniMatrix._22 = Tm._22;
	//	AniMatrix._33 = Tm._33;
	//	AniMatrix._44 = Tm._44;
	}
	else
		ScaleAni(i,AniMatrix);


	if(m_Object[i].m_RotKeyNum <=0) {
		if(m_Object[i].m_ScaleKeyNum > 0) { //scale ani 있을때 유지
			//AniMatrix._11 = Tm._11;
			AniMatrix._12 = Tm._12;
			AniMatrix._13 = Tm._13;

			AniMatrix._21 = Tm._21;
			//AniMatrix._22 = Tm._22;
			AniMatrix._23 = Tm._23;

			AniMatrix._31 = Tm._31;
			AniMatrix._32 = Tm._32;
			//AniMatrix._33 = Tm._33;
		}
		else {	// SCALE ANI 없을때
			AniMatrix._11 = Tm._11;
			AniMatrix._12 = Tm._12;
			AniMatrix._13 = Tm._13;

			AniMatrix._21 = Tm._21;
			AniMatrix._22 = Tm._22;
			AniMatrix._23 = Tm._23;

			AniMatrix._31 = Tm._31;
			AniMatrix._32 = Tm._32;
			AniMatrix._33 = Tm._33;
		}
	}
	else
		RotAni(i,AniMatrix);

	//좌표축은 그대로 그림만 빌보드
	if(m_Object[i].m_Bil1) {
		D3DXMATRIX view;
		D3DXMATRIX invers_view;

		m_Device->GetTransform(D3DTS_VIEW,&view);
		view._41 = view._42 = view._43 =0.0;
		D3DXMatrixInverse(&invers_view,NULL,&view);
		D3DXMatrixMultiply(&AniMatrix,&AniMatrix,&invers_view);
		//	m_Device->SetTransform(D3DTS_WORLD,&world);		
	}
	// x축 회전이 빠진 빌보드
	if(m_Object[i].m_Bil3) {
		D3DXMATRIX view;
		D3DXMATRIX invers_view;

		m_Device->GetTransform(D3DTS_VIEW,&view);
		view._41 = view._42 = view._43 =0.0;
		D3DXMatrixInverse(&invers_view,NULL,&view);

		D3DXVECTOR3 vDir =D3DXVECTOR3(invers_view._31,invers_view._32,invers_view._33);
			
		D3DXMATRIX bil;
		D3DXMatrixIdentity(&bil);

		if( vDir.x > 0.0f )
			D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
		else
			D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
		
		D3DXMatrixMultiply(&AniMatrix,&AniMatrix,&bil);
	//	m_Device->SetTransform(D3DTS_WORLD,&world);		

	}
	
	if(m_Object[i].m_PosKeyNum <=0) {
		//axis set
		if(m_AxisSet) { // position value를 회전시켜 준다
			if(m_Object[i].m_Bil1 || m_Object[i].m_Bil3) {
				D3DXMATRIX tm_axis;

				D3DXMATRIX axismat;
				D3DXMatrixIdentity(&axismat);
				D3DXMatrixIdentity(&tm_axis);
				tm_axis._41 = Tm._41;
				tm_axis._42 = Tm._42;
				tm_axis._43 = Tm._43;

				D3DXMatrixRotationQuaternion(&axismat,&m_Axis);
				D3DXMatrixMultiply(&tm_axis,&tm_axis,&axismat);
				Tm._41 = tm_axis._41;
				Tm._42 = tm_axis._42;
				Tm._43 = tm_axis._43;

			}
	
		}
		AniMatrix._41 = Tm._41;
		AniMatrix._42 = Tm._42;
		AniMatrix._43 = Tm._43;
	}
	else
		PosAni(i,AniMatrix);


	D3DXMatrixMultiply(&world,&world,&AniMatrix);				

	m_Device->SetTransform(D3DTS_WORLD,&world);

}
void CGemRender::RenderObject(int i) {
	int j;
	int tex_id;
	// 부모 matrix 곱을 위한 index
	int kt=0;
	bool loop;

	int vcount = 0;

	int submat = 0;
	D3DXMATRIX world;
	D3DXMATRIX back;

	if(m_lpVertexBuffer == NULL)
		return;


	/*	
	if(m_ObjectVertex[i] == NULL)
		return;
	*/

	if(!m_Object[i].m_FaceNum || !m_Object[i].m_VertexNum)
			return;

	D3DXMatrixIdentity(&world);
	m_Device->SetTransform(D3DTS_WORLD,&world);
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	
	//좌표축은 그대로 그림만 빌보드
	if(m_Object[i].m_Bil1) {
		D3DXMATRIX view;
		D3DXMATRIX invers_view;

		m_Device->GetTransform(D3DTS_VIEW,&view);
		view._41 = view._42 = view._43 =0.0;
		D3DXMatrixInverse(&invers_view,NULL,&view);
		D3DXMatrixMultiply(&world,&world,&invers_view);
		m_Device->SetTransform(D3DTS_WORLD,&world);		
	}
		// x축 회전이 빠진 빌보드
	if(m_Object[i].m_Bil3) {
		D3DXMATRIX view;
		D3DXMATRIX invers_view;

		m_Device->GetTransform(D3DTS_VIEW,&view);
		view._41 = view._42 = view._43 =0.0;
		D3DXMatrixInverse(&invers_view,NULL,&view);

		D3DXVECTOR3 vDir =D3DXVECTOR3(invers_view._31,invers_view._32,invers_view._33);
			
		D3DXMATRIX bil;
		D3DXMatrixIdentity(&bil);

		if( vDir.x > 0.0f )
			D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
		else
			D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
		
		D3DXMatrixMultiply(&world,&world,&bil);
		m_Device->SetTransform(D3DTS_WORLD,&world);		

	}
	// morphing data not exist
	if(!m_Object[i].m_MorphNum) {
		D3DXMATRIX AniMatrix;
		CreateAniTm(i,world,AniMatrix);
			
		// parent matrix mul
			
		kt = i;
		if(m_Object[kt].m_bParent) {
			loop=true;
			while(loop) {
				for(j=0;j<m_ObjectNum;j++) {
					if(!strcmp(m_Object[kt].m_ParentName,m_Object[j].m_Name)) {
						//parent ani func process
						CreateAniTm(j,world,AniMatrix);
				
						kt = j;
						break;

					}
				}
				if(j == m_ObjectNum)
					loop = false;
			
			}
			
		}
	}
	else { // mesh morphing data 있을시에는 rot,scale,transpose 등의 변환이 필요 없다
			
		if(VertexInterpolation(i))
			SetVertexBuffer(i);
	}
	//visibility ani
	if(m_Object[i].m_VisKeyNum) {
		VisAni(i);
		m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,m_Object[i].m_Color);
	}	
	//좌표축(운동)까지 빌보드
	if(m_Object[i].m_Bil2) {
		D3DXMATRIX view;
		D3DXMATRIX invers_view;

		m_Device->GetTransform(D3DTS_VIEW,&view);
		view._41 = view._42 = view._43 =0.0;
		D3DXMatrixInverse(&invers_view,NULL,&view);
		D3DXMatrixMultiply(&world,&world,&invers_view);
		m_Device->SetTransform(D3DTS_WORLD,&world);		
	}
	
	//effect scale & pos set
	D3DXMatrixMultiply(&world,&world,&m_Scale);
		
	m_Scale._41 = m_Pos.x;
	m_Scale._42 = m_Pos.y;
	m_Scale._43 = m_Pos.z;
	m_Device->SetTransform(D3DTS_WORLD,&world);
		
	//////////////////render 부 
	
	//zbuffer write on
	if(m_Object[i].m_Zbuffer) 
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	else
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		
	

	//m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTALPHA);
	//m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_DESTALPHA);
	
	m_Device->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	m_Device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	

	//texture id set
	tex_id = m_Object[i].m_MatId;
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	// sub material 존재 할때
	if(m_Mtl[m_Object[i].m_MatId].m_SubNum) {

		//m_Device->BeginScene();										
		
		for(int k=0;k<m_Sub[i].sub_num;k++) {
			
			
			submat = m_Sub[i].sub_mat[k];

			m_Device->SetTexture(0,NULL);
			HRESULT hr = m_Device->SetTexture(0,m_Texture[tex_id].m_Tex[submat].GetTexture());

			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			
			m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
			m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
			m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

//			m_Device->SetStreamSource(0,m_ObjectVertex[i],sizeof(GemVertex));
//			m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
			m_Device->DrawPrimitive(D3DPT_TRIANGLELIST,m_Object[i].m_iStartBufferIndex + vcount*3,m_Sub[i].sub[k]);
				
			vcount +=m_Sub[i].sub[k];
		}
	//	m_Device->EndScene();
	}
	//sub mat 존재 X
	else {
		
	//	m_Device->BeginScene();										
		m_Device->SetTexture(0,NULL);
		// tex ani ,rand ani set
		if(!m_Object[i].m_RandAni && !m_Object[i].m_TexAni)
			HRESULT hr = m_Device->SetTexture(0,m_Texture[tex_id].m_Tex[0].GetTexture());
		else if(m_Object[i].m_RandAni) { 
		
			HRESULT hr = m_Device->SetTexture(0,m_TexAni[i].m_Tex[m_Object[i].m_CurrentAniTexture].GetTexture());
				
		}
		else if(m_Object[i].m_TexAni) {
			HRESULT hr = m_Device->SetTexture(0,m_TexAni[i].m_Tex[(m_Object[i].m_CurrentAniTexture)].GetTexture());
		}
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
//		m_Device->SetStreamSource(0,m_ObjectVertex[i],sizeof(GemVertex));
//		m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		m_Device->DrawPrimitive(D3DPT_TRIANGLELIST,m_Object[i].m_iStartBufferIndex,m_Object[i].m_FaceNum);
	//	m_Device->EndScene();
	}
	m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}
void CGemRender::Render() {
	if(m_lpVertexBuffer == NULL)
		return;

	int i,j;
	int tex_id = 0;
	// 부모 matrix 곱을 위한 index
	int kt=0;
	bool loop = false;

	int vcount = 0;

	int submat = 0;
	DWORD zmode,cullmode,zwritemode;
	DWORD alpha1,alpha2,afunc;
	DWORD atestmode,arefmode;
	DWORD dwlight,dwshade;
	DWORD dwdither,dwspec,dwamb;

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	D3DXMATRIX back;
	D3DXMatrixIdentity(&back);
	m_Device->GetTransform(D3DTS_WORLD,&back);

	m_Device->GetRenderState(D3DRS_CULLMODE,&cullmode);
	m_Device->GetRenderState(D3DRS_ZENABLE,&zmode);
	m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zwritemode);
	
	m_Device->GetRenderState(D3DRS_SRCBLEND,&alpha1);
	m_Device->GetRenderState(D3DRS_DESTBLEND,&alpha2);
	m_Device->GetRenderState(D3DRS_ALPHAFUNC,&afunc);
	m_Device->GetRenderState(D3DRS_ALPHATESTENABLE,&atestmode);
	m_Device->GetRenderState(D3DRS_ALPHAREF,&arefmode);
		
	m_Device->GetRenderState(D3DRS_LIGHTING,&dwlight);
	m_Device->GetRenderState(D3DRS_SHADEMODE,&dwshade);
	m_Device->GetRenderState(D3DRS_DITHERENABLE,&dwdither);
	m_Device->GetRenderState(D3DRS_SPECULARENABLE,&dwspec);
	
	m_Device->GetRenderState(D3DRS_AMBIENT,&dwamb);
		
	if(m_VertexLockPtr)
	{
		m_lpVertexBuffer->Unlock();
		m_VertexLockPtr = NULL;
	}
	m_Device->SetStreamSource(0,m_lpVertexBuffer,sizeof(GemVertex));
	m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

	m_VertexLockPtr = NULL;
	// VertexBuffer Change	
	for(i = 0; i < m_ObjectNum; i++ )
	{
		if(!m_Object[i].m_FaceNum || !m_Object[i].m_VertexNum)
			continue;
		if(m_Object[i].m_MorphNum) 
		{
			if(VertexInterpolation(i))
				SetVertexBuffer(i);
		}

	}
	if(m_VertexLockPtr)
	{
		m_lpVertexBuffer->Unlock();
		m_VertexLockPtr = NULL;
	}
	
	for(i=0;i<m_ObjectNum;i++) {
	
		if(!m_Object[i].m_FaceNum || !m_Object[i].m_VertexNum)
			continue;

	/*	if(m_ObjectVertex[i] == NULL)
			continue;
	*/

		if(m_Object[i].m_bCull) 
			m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
		else 
			m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		vcount = 0;

		D3DXMatrixIdentity(&world);
		

		//m_Device->SetTransform(D3DTS_WORLD,&world);
	

		//전체 좌표축 회전
		//SetAxis(world);

		//visibility ani
		if(!m_bNullTexture) 
			if(m_Object[i].m_VisKeyNum) {
				if(!VisAni(i))
					continue;
			}

		/*
		//좌표축은 그대로 그림만 빌보드
		if(m_Object[i].m_Bil1) {
			D3DXMATRIX view;
			D3DXMATRIX invers_view;

			m_Device->GetTransform(D3DTS_VIEW,&view);
			view._41 = view._42 = view._43 =0.0;
			D3DXMatrixInverse(&invers_view,NULL,&view);
			D3DXMatrixMultiply(&world,&world,&invers_view);
			m_Device->SetTransform(D3DTS_WORLD,&world);		
		}
			// x축 회전이 빠진 빌보드
		if(m_Object[i].m_Bil3) {
			D3DXMATRIX view;
			D3DXMATRIX invers_view;

			m_Device->GetTransform(D3DTS_VIEW,&view);
			view._41 = view._42 = view._43 =0.0;
			D3DXMatrixInverse(&invers_view,NULL,&view);

			D3DXVECTOR3 vDir =D3DXVECTOR3(invers_view._31,invers_view._32,invers_view._33);
			
			D3DXMATRIX bil;
			D3DXMatrixIdentity(&bil);

			if( vDir.x > 0.0f )
				D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
			else
				D3DXMatrixRotationY( &bil, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
		
			D3DXMatrixMultiply(&world,&world,&bil);
			m_Device->SetTransform(D3DTS_WORLD,&world);		

		}*/
		
		// morphing data not exist
		if(!m_Object[i].m_MorphNum) {
			D3DXMATRIX AniMatrix;
			D3DXMatrixIdentity(&AniMatrix);
			CreateAniTm(i,world,AniMatrix);
			
			// parent matrix mul
			
			kt = i;
			if(m_Object[kt].m_bParent) {
				loop=true;
				while(loop) {
					for(j=0;j<m_ObjectNum;j++) {
						if(!strcmp(m_Object[kt].m_ParentName,m_Object[j].m_Name)) {
							//parent ani func process
							CreateAniTm(j,world,AniMatrix);
					
							kt = j;
							break;

						}
					}
					if(j == m_ObjectNum)
						loop = false;
			
				}
			
			}
		}
		

		//object color 설정
		m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,m_Object[i].m_Color);
	
		
		//picking 된 object일 경우
		if(m_bNullTexture) {
			DWORD color = D3DCOLOR_COLORVALUE(1.0f,0.0f,0.0f,1.0f);
			if(m_PickObject == i)
				m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,color);
		}

		if(CSceneManager::m_bSectorLightFactor)		// Sector Light Effect Render 시..
		{
			m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,CSceneManager::m_dwSectorLightFactor);

		}
		//axis set
		if(m_AxisSet) {
			if((m_Object[i].m_MorphNum >0) || (!m_Object[i].m_Bil1 && !m_Object[i].m_Bil3)) {
				D3DXMATRIX axismat;
				D3DXMatrixIdentity(&axismat);
				D3DXMatrixRotationQuaternion(&axismat,&m_Axis);
				D3DXMatrixMultiply(&world,&world,&axismat);	
				m_Device->SetTransform(D3DTS_WORLD,&world);
			}
		}

		//좌표축(운동)까지 빌보드
		if(m_Object[i].m_Bil2) {
			D3DXMATRIX view;
			D3DXMatrixIdentity(&view);
			D3DXMATRIX invers_view;
			D3DXMatrixIdentity(&invers_view);

			m_Device->GetTransform(D3DTS_VIEW,&view);
			view._41 = view._42 = view._43 =0.0;
			D3DXMatrixInverse(&invers_view,NULL,&view);
			D3DXMatrixMultiply(&world,&world,&invers_view);
			//m_Device->SetTransform(D3DTS_WORLD,&world);		
		}
		
		D3DXMATRIX w_rot;
		D3DXMatrixIdentity(&w_rot);

		//rotation (ani mesh use)
		D3DXMatrixIdentity(&w_rot);
		D3DXMatrixRotationY(&w_rot,D3DXToRadian(m_Yrot));
		D3DXMatrixMultiply(&world,&world,&w_rot);
		
		D3DXMatrixIdentity(&w_rot);
		D3DXMatrixRotationX(&w_rot,D3DXToRadian(m_Xrot));
		D3DXMatrixMultiply(&world,&world,&w_rot);

		D3DXMatrixIdentity(&w_rot);
		D3DXMatrixRotationZ(&w_rot,D3DXToRadian(m_Zrot));
		D3DXMatrixMultiply(&world,&world,&w_rot);	
		
	
		
		//effect scale & pos set
		m_Scale._41 = m_Pos.x;
		m_Scale._42 = m_Pos.y;
		m_Scale._43 = m_Pos.z;
		D3DXMatrixMultiply(&world,&world,&m_Scale);

		//m_Device->SetTransform(D3DTS_WORLD,&world);
		D3DXMatrixMultiply(&world,&world,&m_TrMatrix);
	
		m_Device->SetTransform(D3DTS_WORLD,&world);
		
		// Decal Buffer Setting
		if(m_Object[i].m_bDecal) {
			if(m_Object[i].m_VertexBuffer != NULL) {
				// Decal Object Vertex Buffer Init
				// 초기 상태로 돌려주지 않고 계속 변환이 중첩되면 폴리곤의 형태 자체가 
				// 찌그러지거나 변환축이 뒤틀리는 경우가 발생한다 
				SetDecalBufferInit(i);
				int iBufferNum = m_Object[i].m_FaceNum * 3;
				
				D3DXMATRIX matInvWorld;
				D3DXMatrixInverse(&matInvWorld,NULL,&world);

				for(int cNum = 0; cNum < iBufferNum; cNum++) {
					D3DXVECTOR4 vecTrans;
					vector3 vecTarget;

					D3DXVECTOR3 vecLocal = D3DXVECTOR3(m_Object[i].m_VertexBuffer[cNum].x,
														m_Object[i].m_VertexBuffer[cNum].y,
														m_Object[i].m_VertexBuffer[cNum].z);
					
					D3DXVec3Transform(&vecTrans,&vecLocal,&world);
					vecTarget.x = vecTrans.x;
					vecTarget.y = vecTrans.y;
					vecTarget.z = vecTrans.z;
					
					float fTerrainHeight = CSceneManager::m_HeightField.GetHeight(vecTarget);
					
					/*if(fTerrainHeight >= vecTarget.y) {		// 점의 높이가 지형보다 낮다면
						vecTarget.y = fTerrainHeight;		// 일단 보류
						vecTarget.y += 10.0f;
					}*/
					if((fTerrainHeight + 100.0f) < vecTarget.y)
					{
									
					}
					else
						vecTarget.y = fTerrainHeight + 10.0f;		
					
					vecLocal = D3DXVECTOR3(vecTarget.x,vecTarget.y,vecTarget.z);				
					
					D3DXVec3Transform(&vecTrans,&vecLocal,&matInvWorld);
					m_Object[i].m_VertexBuffer[cNum].x = vecTrans.x;
					m_Object[i].m_VertexBuffer[cNum].y = vecTrans.y;
					m_Object[i].m_VertexBuffer[cNum].z = vecTrans.z;

				}
				// Vertex Buffer Setting
				SetDecalBuffer(i);
			}
			if(m_VertexLockPtr)
			{
				m_lpVertexBuffer->Unlock();
				m_VertexLockPtr = NULL;
			}
		}	// if(m_Object[i].m_bDecal)
		
		

		//////////////////render 부 
		
		//mesh ani를 위한 light setting 부분
		if(m_bLight) {
		

			D3DMATERIAL8 mtrl;
			ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
			mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
			mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
			mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
			mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
			m_Device->SetMaterial( &mtrl );
			m_Device->SetRenderState( D3DRS_AMBIENT, CSceneManager::m_WeatherManager.m_InterObjectAmbientColor.c);
	
			m_Device->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
			m_Device->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

			m_Device->SetRenderState(D3DRS_LIGHTING,TRUE);
			m_Device->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
			m_Device->SetLight(0,&CSceneManager::m_WeatherManager.m_SunLight);
			m_Device->LightEnable(0,TRUE);
		
		}
		if(m_Mine) { //내 이펙트 일 경우
			if(m_Object[i].m_Zenable) { // z enable 
				m_Device->SetRenderState(D3DRS_ZENABLE,FALSE);
			}
			else {
				m_Device->SetRenderState(D3DRS_ZENABLE,TRUE);
			}
		}
		

		//zbuffer write on
		if(m_Object[i].m_Zbuffer) 
			m_Device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		else
			m_Device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		
	
		if(!m_Object[i].m_Opacity) 
			m_Device->SetRenderState(D3DRS_SRCBLEND,m_SrcBlend);
		else // opacity 뚫은 dds 이용 
			m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_BOTHSRCALPHA);

		m_Device->SetRenderState(D3DRS_DESTBLEND,m_DstBlend);
		
		m_Device->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		//m_Device->SetRenderState(D3DRS_ALPHAREF,0.5);
		m_Device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
		      

		//texture id set
		tex_id = m_Object[i].m_MatId;

		// sub material 존재 할때
		if(m_Mtl[m_Object[i].m_MatId].m_SubNum) {

		//	m_Device->BeginScene();										
			
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			if(m_bLight) {
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			}
			else {
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
			}
			if(!m_bNullTexture &&!m_bLight)
				m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			else if(!m_bNullTexture && m_bLight)
				m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
			else
				m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);


			for(int k=0;k<m_Sub[i].sub_num;k++) {
				submat = m_Sub[i].sub_mat[k];

				m_Device->SetTexture(0,NULL);
				if(!m_bNullTexture) 
					HRESULT hr = m_Device->SetTexture(0,m_Texture[tex_id].m_Tex[submat].GetTexture());
				
				
				m_Device->DrawPrimitive(D3DPT_TRIANGLELIST,m_Object[i].m_iStartBufferIndex + vcount*3,m_Sub[i].sub[k]);
				
				vcount +=m_Sub[i].sub[k];
			}
		//	m_Device->EndScene();
		}
		//sub mat 존재 X
		else {
//			m_Device->BeginScene();										
			m_Device->SetTexture(0,NULL);
			// tex ani ,rand ani set
			if(!m_Object[i].m_RandAni && !m_Object[i].m_TexAni && !m_Object[i].m_RandStartTexAni) {
				if(!m_bNullTexture)
					HRESULT hr = m_Device->SetTexture(0,m_Texture[tex_id].m_Tex[0].GetTexture());	
			}
			else if(m_Object[i].m_RandAni) { 
				if(!m_bNullTexture)	
					HRESULT hr = m_Device->SetTexture(0,m_TexAni[i].m_Tex[m_Object[i].m_CurrentAniTexture].GetTexture());
					
			}
			else if((m_Object[i].m_TexAni) || (m_Object[i].m_RandStartTexAni)) {
				if(!m_bNullTexture)
					HRESULT hr = m_Device->SetTexture(0,m_TexAni[i].m_Tex[(m_Object[i].m_CurrentAniTexture)].GetTexture());
			}
				
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
			m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			
			if(m_bLight) {
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			}
			else {
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
				m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
			}
			if(!m_bNullTexture) {
				m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			}
			else
				m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);

	//		if(!(m_Object[i].m_CurrentAniTexture)) {
				m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
				m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	//		}
		/*	else { // muti texturing	
				m_Device->SetTexture(1,m_TexAni[i].m_Tex[(m_Object[i].m_BeforeAniTexture)].GetTexture());
				m_Device->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
				m_Device->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_CURRENT);
				m_Device->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_TEXTURE);
				m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_ADDSIGNED);
				m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(100,255,255,255));
				m_Device->SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
				m_Device->SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
				m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			
			}*/
			if(CSceneManager::m_bSectorLightFactor)		// Sector Light Setting
			{				
				m_Device->SetRenderState(D3DRS_ZENABLE,FALSE);
				//m_Device->SetRenderState(D3DRS_ZBIAS,16);
				m_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
				m_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				m_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
			}
			//m_Device->SetStreamSource(0,m_ObjectVertex[i],sizeof(GemVertex));
			//m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
			m_Device->DrawPrimitive(D3DPT_TRIANGLELIST,m_Object[i].m_iStartBufferIndex,m_Object[i].m_FaceNum);
//			m_Device->EndScene();
		}
	}
	if(m_bMulty) {
		for(i=0;i<m_ObjectNum;i++) {
			if(m_Object[i].m_Multy){
				m_Multy->RenderObject(i);
			}
			
		}
	}
	//m_Device->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	//m_Device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
	//m_Device->SetRenderState(D3DRS_ALPHAREF,0x000000FF);
	m_Device->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,255,255,255));
	
	m_Device->SetRenderState(D3DRS_CULLMODE,cullmode);
	m_Device->SetRenderState(D3DRS_ZENABLE,zmode);
	//m_Device->SetRenderState(D3DRS_ZBIAS,0);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zwritemode);

	m_Device->SetRenderState(D3DRS_SRCBLEND,alpha1);
	m_Device->SetRenderState(D3DRS_DESTBLEND,alpha2);
	m_Device->SetRenderState(D3DRS_ALPHAFUNC,afunc);
	m_Device->SetRenderState(D3DRS_ALPHATESTENABLE,atestmode);
	m_Device->SetRenderState(D3DRS_ALPHAREF,arefmode);

	m_Device->SetRenderState(D3DRS_LIGHTING,dwlight);
	m_Device->SetRenderState(D3DRS_SHADEMODE,dwshade);
	m_Device->SetRenderState(D3DRS_DITHERENABLE,dwdither);
	m_Device->SetRenderState(D3DRS_SPECULARENABLE,dwspec);
	m_Device->SetRenderState(D3DRS_AMBIENT,dwamb);

	//
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
	m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	m_Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	
	m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);






}
bool CGemRender::CheckPosition(D3DXVECTOR3 center,D3DXVECTOR3 user) {
	D3DXVECTOR3 sub = center - user;
	float length = D3DXVec3Length(&sub);
	if(length > m_AniRad)
		return false;
	return true;

}
void CGemRender::Update(D3DXVECTOR3 center,D3DXVECTOR3 user) {

	// switch ani 사용 메쉬
	if(m_SwitchAni) {
		m_StartSwitch = CheckPosition(center,user);	
		if(m_StartSwitch) {
			m_CurrentF+=m_Vot;	
	
		}
	}
	else { // switch ani 사용 X 메쉬 
		m_CurrentF+=m_Vot;
	}
	

	float tmp_value = 0.0f;
	float vot_change = 0.0f;
	
	
	tmp_value = m_CurrentF * m_UnitF;

	if(tmp_value >= m_EndF) {

		m_CurrentF = 0.0f;
		if(m_RandAni) {
			if((rand()%10) < 5)
				m_bLoop = true; 
			else
				m_bLoop = false;
			
			vot_change = (float)(rand()%10)/10.0f;
			vot_change = rand() % 2 ? vot_change * 1 : vot_change * -1; 
			m_Vot += vot_change; 
			if(m_Vot <m_RandAniMin)
				m_Vot = m_RandAniMin;
			if(m_Vot >m_RandAniMax)
				m_Vot = m_RandAniMax;

		}

	}
	for(int i = 0;i<m_ObjectNum;i++) {
		if(m_Object[i].m_RandAni) {
			if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
				if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame) % (int)m_Object[i].m_AniChangeFrame)) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
				}
		
					
		}
		else if(m_Object[i].m_TexAni) {
			if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
				if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame)% (int)m_Object[i].m_AniChangeFrame)) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					(m_Object[i].m_CurrentAniTexture)++;
				}
				if(m_Object[i].m_AniTextureNum <= m_Object[i].m_CurrentAniTexture) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					m_Object[i].m_CurrentAniTexture = 0;
				}
		}
	}


}
void CGemRender::Update() {

	float tmp_value = 0.0f;
	float vot_change = 0.0f;
	m_CurrentF+=m_Vot;
	
	tmp_value = m_CurrentF * m_UnitF;

	if(tmp_value >= m_EndF) {

		m_CurrentF = 0.0f;
		if(m_RandAni) {
			if((rand()%10) < 5)
				m_bLoop = true; 
			else
				m_bLoop = false;
			
			vot_change = (float)(rand()%10)/10.0f;
			vot_change = rand() % 2 ? vot_change * 1 : vot_change * -1; 
			m_Vot += vot_change; 
			if(m_Vot <m_RandAniMin)
				m_Vot = m_RandAniMin;
			if(m_Vot >m_RandAniMax)
				m_Vot = m_RandAniMax;

		}

	}
	for(int i = 0;i<m_ObjectNum;i++) {
		if(m_Object[i].m_RandAni) {
			if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
				if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame) % (int)m_Object[i].m_AniChangeFrame)) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					m_Object[i].m_CurrentAniTexture = rand()%(m_TexAni[i].TexNum);
				}
		
					
		}
		else if(m_Object[i].m_TexAni) {
			if(m_CurrentF >= m_Object[i].m_StartTexAniFrame)
				if(!((int)(m_CurrentF - m_Object[i].m_StartTexAniFrame)% (int)m_Object[i].m_AniChangeFrame)) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					(m_Object[i].m_CurrentAniTexture)++;
				}
				if(m_Object[i].m_AniTextureNum <= m_Object[i].m_CurrentAniTexture) {
					m_Object[i].m_BeforeAniTexture = m_Object[i].m_CurrentAniTexture;
					m_Object[i].m_CurrentAniTexture = 0;
				}
		}
	}

}
void CGemRender::UnUpdate() {

	float tmp_value = 0.0f;

	m_CurrentF-=m_Vot;
	
	tmp_value = m_CurrentF * m_UnitF;

	if(tmp_value <= 0.0f) {
		m_CurrentF = (float)m_EndF;
	}
}
void CGemRender::SetDecalBufferInit(int index) {

	int face_num = m_Object[index].m_FaceNum;
	int vertex_num = m_Object[index].m_VertexNum;
	int i;
	int k=0;
	int tmp_index[4] = {0,0,0,0};
	int tmp_coord[3] = {0,0,0};
	
/*	if( m_ObjectVertex[index] == NULL)
		return;
*/
	if(m_lpVertexBuffer == NULL)
		return;

	//비어있는 object
	if(!face_num || !vertex_num)
		return;
	
	if(m_Object[index].m_VertexBuffer == NULL) {						//Vertex 정보Buffer가 비어있을 경우에만 생성(03.03.01 수정)
		m_Object[index].m_VertexBuffer = new GemVertex[face_num * 3];
	}
	if(m_Object[index].m_bInterpol == true) {
		for(i=0;i<face_num;i++) {
		
			tmp_index[0] = (int)m_Object[index].m_Face[i].x;
			tmp_index[1] = (int)m_Object[index].m_Face[i].y;
			tmp_index[2] = (int)m_Object[index].m_Face[i].z;
			tmp_index[3] = (int)m_Object[index].m_Face[i].w;

			tmp_coord[0] = (int)m_Object[index].m_CoordFace[i].x;
			tmp_coord[1] = (int)m_Object[index].m_CoordFace[i].y;
			tmp_coord[2] = (int)m_Object[index].m_CoordFace[i].z;
			//0
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[0]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[0]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[0]].z;
		
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[0]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[0]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[0]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[0]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[0]].y;
			//1
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[1]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[1]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[1]].z;
			
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[1]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[1]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[1]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[1]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[1]].y;
			//2
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[2]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[2]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[2]].z;
			
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[2]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[2]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[2]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[2]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[2]].y;
			
			if(m_bLight) {
				// Normal Vector Setting
				D3DXVECTOR3 vec1 = D3DXVECTOR3(m_Object[index].m_VertexBuffer[k - 3].x - m_Object[index].m_VertexBuffer[k - 2].x,
												m_Object[index].m_VertexBuffer[k - 3].y - m_Object[index].m_VertexBuffer[k - 2].y,
												m_Object[index].m_VertexBuffer[k - 3].z - m_Object[index].m_VertexBuffer[k - 2].z);
				D3DXVECTOR3 vec2 = D3DXVECTOR3(m_Object[index].m_VertexBuffer[k - 1].x - m_Object[index].m_VertexBuffer[k - 2].x,
												m_Object[index].m_VertexBuffer[k - 1].y - m_Object[index].m_VertexBuffer[k - 2].y,
												m_Object[index].m_VertexBuffer[k - 1].z - m_Object[index].m_VertexBuffer[k - 2].z);
				D3DXVec3Normalize(&vec1,&vec1);
				D3DXVec3Normalize(&vec2,&vec2);
				
				D3DXVECTOR3 vecFaceNormal;
				
				D3DXVec3Cross(&vecFaceNormal,&vec1,&vec2);
				m_Object[index].m_VertexBuffer[k - 3].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 3].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 3].nz =vecFaceNormal.z;

				m_Object[index].m_VertexBuffer[k - 2].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 2].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 2].nz =vecFaceNormal.z;
				
				m_Object[index].m_VertexBuffer[k - 1].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 1].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 1].nz =vecFaceNormal.z;
			}
		}
	}
	else {
		if(m_bClearBuffer)		// loop 시 버퍼를 지워주지 않는다.(깜박임 방지)
			memset(m_Object[index].m_VertexBuffer,0,sizeof(GemVertex) * face_num * 3);
	}
	k = 0;

}
void CGemRender::SetDecalBuffer(int index) {
	int face_num = m_Object[index].m_FaceNum;

	if(!m_lpVertexBuffer || !face_num)
		return;

		
	if(m_Object[index].m_VertexBuffer != NULL) {
		
		if(!m_VertexLockPtr)
//			m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
			m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr, 0);
		BYTE *pTmp = (BYTE *)m_VertexLockPtr;

		memcpy(&pTmp[m_Object[index].m_iStartBufferIndex * sizeof(GemVertex)],m_Object[index].m_VertexBuffer,face_num * sizeof(GemVertex) * 3);
			
		/*
		m_ObjectVertex[index]->Lock(0,face_num * sizeof(GemVertex) *3,(BYTE **)&tmp,0);
		memcpy(tmp,m_Object[index].m_VertexBuffer,face_num * sizeof(GemVertex) * 3);
		
		m_ObjectVertex[index]->Unlock();
		*/
	}
}
void CGemRender::SetInitBuffer() {
	int i;
	if(!m_VertexLockPtr)
//		m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
		m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr, 0);

	for(i = 0;i < m_ObjectNum;i++) {
		SetVertexBuffer(i);
	}
	m_lpVertexBuffer->Unlock();
	m_VertexLockPtr = NULL;

}
void CGemRender::SetVertexBuffer(int index){
	
	// By Minbobo(SetVertexBuffer 함수 안에서 다운된다고 스택덤프에 남아서 인덱스 체크를 삽입).
	if(m_ObjectNum<=index)
	{
		return;
	}

	int face_num = m_Object[index].m_FaceNum;
	int vertex_num = m_Object[index].m_VertexNum;
	int i;
	int k=0;
	int tmp_index[4] = {0,0,0,0};
	int tmp_coord[3] = {0,0,0};

	
	if(!m_lpVertexBuffer || !face_num || !vertex_num)
		return;
/*

	if( m_ObjectVertex[index] == NULL)
		return;

	//비어있는 object
	if(!face_num || !vertex_num)
		return;
*/	
	if(m_Object[index].m_VertexBuffer == NULL) {						//Vertex 정보Buffer가 비어있을 경우에만 생성(03.03.01 수정)
		m_Object[index].m_VertexBuffer = new GemVertex[face_num * 3];
	}
	if(m_Object[index].m_bInterpol == true) {
		for(i=0;i<face_num;i++) {
		
			tmp_index[0] = (int)m_Object[index].m_Face[i].x;
			tmp_index[1] = (int)m_Object[index].m_Face[i].y;
			tmp_index[2] = (int)m_Object[index].m_Face[i].z;
			tmp_index[3] = (int)m_Object[index].m_Face[i].w;

			tmp_coord[0] = (int)m_Object[index].m_CoordFace[i].x;
			tmp_coord[1] = (int)m_Object[index].m_CoordFace[i].y;
			tmp_coord[2] = (int)m_Object[index].m_CoordFace[i].z;
			//0
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[0]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[0]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[0]].z;
		
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[0]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[0]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[0]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[0]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[0]].y;
			//1
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[1]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[1]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[1]].z;
			
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[1]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[1]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[1]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[1]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[1]].y;
			//2
			m_Object[index].m_VertexBuffer[k].x = m_Object[index].m_Vert[tmp_index[2]].x;
			m_Object[index].m_VertexBuffer[k].y = m_Object[index].m_Vert[tmp_index[2]].y;
			m_Object[index].m_VertexBuffer[k].z = m_Object[index].m_Vert[tmp_index[2]].z;
			
			//m_Object[index].m_VertexBuffer[k].nx = m_Object[index].m_Normal[tmp_index[2]].x;
			//m_Object[index].m_VertexBuffer[k].nz = m_Object[index].m_Normal[tmp_index[2]].y;
			//m_Object[index].m_VertexBuffer[k].ny = m_Object[index].m_Normal[tmp_index[2]].z;
			
			m_Object[index].m_VertexBuffer[k].s = m_Object[index].m_Coord[tmp_coord[2]].x;
			m_Object[index].m_VertexBuffer[k++].t = m_Object[index].m_Coord[tmp_coord[2]].y;
			
			if(m_bLight) {
				// Normal Vector Setting
				D3DXVECTOR3 vec1 = D3DXVECTOR3(m_Object[index].m_VertexBuffer[k - 3].x - m_Object[index].m_VertexBuffer[k - 2].x,
												m_Object[index].m_VertexBuffer[k - 3].y - m_Object[index].m_VertexBuffer[k - 2].y,
												m_Object[index].m_VertexBuffer[k - 3].z - m_Object[index].m_VertexBuffer[k - 2].z);
				D3DXVECTOR3 vec2 = D3DXVECTOR3(m_Object[index].m_VertexBuffer[k - 1].x - m_Object[index].m_VertexBuffer[k - 2].x,
												m_Object[index].m_VertexBuffer[k - 1].y - m_Object[index].m_VertexBuffer[k - 2].y,
												m_Object[index].m_VertexBuffer[k - 1].z - m_Object[index].m_VertexBuffer[k - 2].z);
				D3DXVec3Normalize(&vec1,&vec1);
				D3DXVec3Normalize(&vec2,&vec2);
				
				D3DXVECTOR3 vecFaceNormal;
				
				D3DXVec3Cross(&vecFaceNormal,&vec1,&vec2);
				m_Object[index].m_VertexBuffer[k - 3].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 3].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 3].nz =vecFaceNormal.z;

				m_Object[index].m_VertexBuffer[k - 2].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 2].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 2].nz =vecFaceNormal.z;
				
				m_Object[index].m_VertexBuffer[k - 1].nx =vecFaceNormal.x;
				m_Object[index].m_VertexBuffer[k - 1].ny =vecFaceNormal.y;
				m_Object[index].m_VertexBuffer[k - 1].nz =vecFaceNormal.z;
			}
		}
	}
	else {
		if(m_bClearBuffer)		// loop 시 버퍼를 지워주지 않는다.(깜박임 방지)
			memset(m_Object[index].m_VertexBuffer,0,sizeof(GemVertex) * face_num * 3);
	}
	if(!m_VertexLockPtr)
	{
//		m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
		m_lpVertexBuffer->Lock(0,0,(BYTE **)&m_VertexLockPtr,0);
	}		
/*
	VOID *tmp = NULL;
	if(m_Object[index].m_MorphNum) {
		m_ObjectVertex[index]->Lock(0,face_num * sizeof(GemVertex) *3,(BYTE **)&tmp,D3DLOCK_DISCARD);
	}
	else {
		m_ObjectVertex[index]->Lock(0,face_num * sizeof(GemVertex) *3,(BYTE **)&tmp,0);
	}*/
	BYTE *pTmp = (BYTE *)m_VertexLockPtr;
	memcpy(&(pTmp[m_Object[index].m_iStartBufferIndex  * sizeof(GemVertex)]),(BYTE *)(m_Object[index].m_VertexBuffer),face_num * sizeof(GemVertex) * 3);

//	m_ObjectVertex[index]->Unlock();
	
	//delete[] m_Vert;
	//m_Vert = NULL;

	k = 0;
}

void CGemRender::SetClearBuffer(bool b)
{
	m_bClearBuffer = b;

}
