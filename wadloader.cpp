#include "wadloader.h"

CMeshMorph::CMeshMorph() {
	frame = 0.0f;
	wadfile = NULL;
	vert = NULL;
	vert2 = NULL;
	max_frame = -1;
	m_NullTexture = false;
	SrcBlend = D3DBLEND_SRCALPHA;
	DstBlend = D3DBLEND_INVSRCALPHA;

	Color = 0xffffffff;
	vot = VOT;
	anirad = ANIRAD;
	m_xrot = m_yrot = m_zrot = 0.0f;

	m_Loop = true;
	m_Start = true;
	m_switchani = false;
	m_RandAni = false;
	m_RandMax = 0.0f;
	m_RandMin = 0.0f;

}

void CMeshMorph::SetColor(DWORD c) {
	Color = c;
}
void CMeshMorph::SetAlpha(int al) {
	Color = D3DCOLOR_ARGB(al,255,255,255);
}
CMeshMorph::~CMeshMorph() {
	int i;


	if(vert != NULL) {
		delete[] vert;
		vert = NULL;
	}
	if(vert2 != NULL) {
		delete[] vert2;
		vert2 = NULL;
	}
	if(texture_count >0)
		delete[] texture;
	
	for(i=0;i<Model.object_num;i++)
		object[i]->Release();
	for(i=0;i<Model.object_num;i++) {
		object_sub[i].sub.clear();
	}
	
	delete[] object_sub;
	object_sub = NULL;

	for(i=0;i<Model.object_num;i++) {
		if(Model.pobject[i].face) {
			delete [] Model.pobject[i].face;
			Model.pobject[i].face = NULL;
		}
		if(Model.pobject[i].texcoord) {
			delete [] Model.pobject[i].texcoord;
			Model.pobject[i].texcoord = NULL;
		}
		if(Model.pobject[i].vertex) {
			delete [] Model.pobject[i].vertex;
			Model.pobject[i].vertex = NULL;
		}
		Model.pobject[i].rot_key.clear();
		Model.pobject[i].pos_key.clear();
	}
	
	Model.pmaterial.clear();
	Model.pobject.clear();

}
bool CMeshMorph::CheckPosition(D3DXVECTOR3 center,D3DXVECTOR3 user) {
	D3DXVECTOR3 sub = center - user;
	float length = D3DXVec3Length(&sub);
	if(length > anirad)
		return false;
	return true;

}
int CMeshMorph::UpdateMesh(D3DXVECTOR3 center,D3DXVECTOR3 user){

	float vot_change = 0.0f;

	// switch ani ani 사용 메쉬
	if(m_switchani) {
		m_Start = CheckPosition(center,user);	
	}
	if(m_Start) {
		frame+=vot;	
	
	}
	
	if(frame >=max_frame) {
		if(m_RandAni) {
			if((rand()%10) < 5)
				m_Loop = true; 
			else
				m_Loop = false;
			
			vot_change = (float)(rand()%10)/10.0f;
			vot_change = rand() % 2 ? vot_change * 1 : vot_change * -1; 
			vot += vot_change; 
			if(vot <m_RandMin)
				vot = m_RandMin;
			if(vot >m_RandMax)
				vot = m_RandMax;

		}
		if(m_Loop) {
			frame = 0.0f;
		}
		else
			frame = max_frame;
	}
	return frame;
}
bool CMeshMorph::LoadWadfile(char *filename,LPDIRECT3DDEVICE8 device) {
	char buf[BUF_SIZE] = {0};
	char *tmp_ptr = NULL;

	char *token;

	int object_num;
	int material_num;
	int effect_num;
	int max;


	int i,j,k;
	int vertex_num,texcoord_num;
	int pos_num,rot_num;
	int morph_num,face_num;
	int scale_num;
	int vert_num;
	


	AsePosKey tmp_pos;
	AseRotKey tmp_rot;
	AseScaleKey tmp_scale;
	AseMorphObject tmp_morph;

	AseMaterial tmp;
	AseObject tmp_object;
	D3DXVECTOR3 tmp_effect;

	wadfile = fopen(filename,"rb");
	if(wadfile == NULL) {
		MessageBox(NULL,"file not found","error",MB_OK);
		PostQuitMessage(0);
		return false;
	}
	fread((char *)buf,1,255,wadfile);
	token = strtok(buf,"\n");
	//wad file check..
	if(strcmp(token,WAD_FILE)) {
		sprintf(buf,"%s is not WAD(Wizardbug Ase Data) file.",filename);
		MessageBox(NULL,buf,"error",MB_OK);
		PostQuitMessage(0);
	}
	
	
	fread((char *)buf,1,255,wadfile);
	token = strtok(buf,"\n ");
	object_num = atoi(token);
	token = strtok(NULL,"\n ");
	material_num = atoi(token);
	token = strtok(NULL,"\n ");
	effect_num  = atoi(token);
	token = strtok(NULL,"\n ");
	max = atoi(token);

	// material num,object num, max face load
	Model.material_num = material_num;
	Model.object_num = object_num;
	Model.effect_num = effect_num;
	max_frame = max;
	// material load.
	
	for(i=0;i<material_num;i++) {
		Model.pmaterial.push_back(tmp);
		fread((AseMaterial *)&(Model.pmaterial[i]),1,sizeof(AseMaterial),wadfile);
		/////////// bmp ->dds
		tmp_ptr = strstr(Model.pmaterial[i].texture_file,"bmp");
		if(tmp_ptr == NULL) {
			tmp_ptr = strstr(Model.pmaterial[i].texture_file,"BMP");
		}
		if(tmp_ptr != NULL) {
			*tmp_ptr = 'd';
			tmp_ptr++;
			*tmp_ptr = 'd';
			tmp_ptr++;
			*tmp_ptr = 's';	
			tmp_ptr = NULL;
		}
		/////////////////
		Model.pmaterial[i].sub = new AseMaterial[Model.pmaterial[i].sub_num];
		
		for(k=0;k<Model.pmaterial[i].sub_num;k++) {
			fread((AseMaterial *)&(Model.pmaterial[i].sub[k]),1,sizeof(AseMaterial),wadfile);
			tmp_ptr = strstr(Model.pmaterial[i].sub[k].texture_file,"bmp");
			if(tmp_ptr == NULL) {
				tmp_ptr = strstr(Model.pmaterial[i].sub[k].texture_file,"BMP");
			}
			if(tmp_ptr != NULL) {
				*tmp_ptr = 'd';
				tmp_ptr++;
				*tmp_ptr = 'd';
				tmp_ptr++;
				*tmp_ptr = 's';	
				tmp_ptr = NULL;
			}
		}
	}

	object = new LPDIRECT3DVERTEXBUFFER8[object_num];

////// object sub face list create/////////////////////////////
	object_sub = new WadFace[object_num];

	// object load.
	for(i=0;i<object_num;i++) {
		Model.pobject.push_back(tmp_object);
		fread((char *)buf,1,255,wadfile);
		
		token = strtok(buf,"\n ");
		Model.pobject[i].btexture = (bool)atoi(token);
		token = strtok(NULL,"\n ");
		face_num = Model.pobject[i].face_num = atoi(token);
		token = strtok(NULL,"\n ");
		vertex_num = Model.pobject[i].vertex_num = atoi(token);
		token = strtok(NULL,"\n ");
		texcoord_num = Model.pobject[i].texcoord_num = atoi(token);
		token = strtok(NULL,"\n ");
		Model.pobject[i].bparent = (bool)atoi(token);
		token = strtok(NULL,"\n ");
		Model.pobject[i].material_id = atoi(token);
		token = strtok(NULL,"\n ");
		Model.pobject[i].bmorph = (bool)atoi(token);
		
		fread((char *)Model.pobject[i].object_name,1,255,wadfile);
		
		if(Model.pobject[i].bparent) {
			fread((char *)Model.pobject[i].parent_name,1,255,wadfile);
		}
		
		//matrix load
		fread((D3DXMATRIX *)&(Model.pobject[i].matrix),1,sizeof(D3DXMATRIX),wadfile);
		// vertex num load
		fread((char *)buf,1,255,wadfile);
		token = strtok(buf,"\n ");
		vert_num  = atoi(token);
		// vertex load
		if(Model.pobject[i].btexture) {
			vert = new custom_vertex[vert_num];
			fread((custom_vertex *)vert,sizeof(custom_vertex),vert_num,wadfile);
		}
		else {
			vert2 = new D3DXVECTOR3[vert_num];
			fread((D3DXVECTOR3 *)vert2,sizeof(D3DXVECTOR3),vert_num,wadfile);
		}

		Model.pobject[i].face = new AseFace[face_num];
		fread((AseFace *)(Model.pobject[i].face),sizeof(AseFace),face_num,wadfile);
		////////////////////////////////////// sub face load///////////////////////////////
		if(Model.material_num >0) {
			if(Model.pmaterial[Model.pobject[i].material_id].sub_num >0)
				LoadSubface(i);
		}
		Model.pobject[i].vertex = new D3DXVECTOR3[vertex_num];
		fread((D3DXVECTOR3 *)(Model.pobject[i].vertex),sizeof(D3DXVECTOR3),vertex_num,wadfile);
		if(Model.pobject[i].btexture) {
			Model.pobject[i].normal = new D3DXVECTOR3[vertex_num];
			fread((D3DXVECTOR3 *)(Model.pobject[i].normal),sizeof(D3DXVECTOR3),vertex_num,wadfile);
		
			Model.pobject[i].texcoord = new D3DXVECTOR2[texcoord_num];
			fread((D3DXVECTOR2 *)(Model.pobject[i].texcoord),sizeof(D3DXVECTOR2),texcoord_num,wadfile);
		}

		//pos key,rot key morph key num load
		fread((char *)buf,1,255,wadfile);
		token = strtok(buf,"\n ");
		pos_num = atoi(token);
		token = strtok(NULL,"\n ");
		rot_num = atoi(token);
		token = strtok(NULL,"\n ");
		scale_num = atoi(token);
		token = strtok(NULL,"\n ");
		morph_num = atoi(token);
		
		Model.pobject[i].pos_keyNum = pos_num;
		Model.pobject[i].rot_keyNum = rot_num;
		Model.pobject[i].scale_keyNum = scale_num;
		Model.pobject[i].morph_num = morph_num;

		
		//pos key load
		for(j=0;j<pos_num;j++) {
			fread((AsePosKey *)&tmp_pos,sizeof(AsePosKey),1,wadfile);
			Model.pobject[i].pos_key.push_back(tmp_pos);
		}
		//rot key load
		for(j=0;j<rot_num;j++) {
			fread((AseRotKey *)&tmp_rot,sizeof(AseRotKey),1,wadfile);
			Model.pobject[i].rot_key.push_back(tmp_rot);
		}
		if(rot_num != 0) {
			//quaternion
			Model.pobject[i].rot_quatkey = new D3DXQUATERNION[Model.pobject[i].rot_keyNum];
			fread((D3DXQUATERNION *)Model.pobject[i].rot_quatkey,sizeof(D3DXQUATERNION),Model.pobject[i].rot_keyNum,wadfile);
		}
		for(j=0;j<scale_num;j++) {
			fread((AseScaleKey *)&tmp_scale,sizeof(AseScaleKey),1,wadfile);
			Model.pobject[i].scale_key.push_back(tmp_scale);
		}

		//morph key load
		if(Model.pobject[i].bmorph) {
			for(j=0;j<morph_num;j++) {
				Model.pobject[i].morph.push_back(tmp_morph);
				Model.pobject[i].morph[j].vertex = new D3DXVECTOR3[vertex_num];
				Model.pobject[i].morph[j].texcoord = new D3DXVECTOR2[texcoord_num];
				Model.pobject[i].morph[j].normal = new D3DXVECTOR3[vertex_num];
				
				fread((char *)buf,1,255,wadfile);
				token = strtok(buf,"\n ");
				Model.pobject[i].morph[j].keynum = atoi(token);
				fread((D3DXVECTOR3 *)(Model.pobject[i].morph[j].vertex),sizeof(D3DXVECTOR3),vertex_num,wadfile);
				fread((D3DXVECTOR2 *)(Model.pobject[i].morph[j].texcoord),sizeof(D3DXVECTOR2),texcoord_num,wadfile);
				fread((D3DXVECTOR3 *)(Model.pobject[i].morph[j].normal),sizeof(D3DXVECTOR3),vertex_num,wadfile);
			}		
		}
		Model.pobject[i].vertex = new D3DXVECTOR3[vertex_num];
		
		Create_Vertex(i,device);
	}
	//eff pos load(dumy position)
	if(Model.effect_num) {
		for(i=0;i<Model.effect_num;i++) {
			Model.pEffectPos.push_back(tmp_effect);
			fread((D3DXVECTOR3 *)&(Model.pEffectPos[i]),sizeof(D3DXVECTOR3),1,wadfile);
		}

	}
	CreateTexture(device);
	
	fclose(wadfile);
	return true;
}

int CMeshMorph::Render(LPDIRECT3DDEVICE8 device,D3DXVECTOR3 vecCenter) {
	int i,j;
	int tex_id;
	int tex_i;
	int tex_j;
	
	int vcount = 0;
	bool loop = true;

	D3DXMATRIX world;
	D3DXMATRIX back;
	D3DXMATRIX rot;
	//effect pos calculate
/*	for(i=0;i<Model.effect_num;i++) {
		Model.pEffectPos[i] +=vecCenter;
	}
*/
	device->GetTransform(D3DTS_WORLD,&back);
	//alphatest set
	device->SetRenderState(D3DRS_ZENABLE,TRUE);
	device->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESS);
	device->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	
	device->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	//null texture
	if(m_NullTexture)
		device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	
	for(i=0;i<Model.object_num;i++) {
		vcount = 0;
		D3DXMatrixIdentity(&world);
		//rot
		//D3DXMatrixRotationY(&world,rot);
		//rot+=0.0001f;
		//D3DXMatrixRotationX(&world,90.0f);
		device->SetTransform(D3DTS_WORLD,&world);
		
//////////////////////////////////////////////////////////////////////////////////////////////
// ani
/////////////////////////////////////////////////////////////////////////////////////////////		
		//global matrix get. (rot ,pos 키가 있으면 그 키를 곱할시 곧바로 global 좌표로 바뀌어
		//이 작업이 필요 없다.
		if(Model.pobject[i].pos_keyNum<=0 && Model.pobject[i].rot_keyNum<=0) {
			D3DXMatrixMultiply(&world,&(Model.pobject[i].matrix),&world);
			device->SetTransform(D3DTS_WORLD,&world);
		}
		else if(Model.pobject[i].pos_keyNum <= 0) {
			AsePosKey tmp;

			Model.pobject[i].pos_key.push_back(tmp);
			Model.pobject[i].pos_key[0].x = Model.pobject[i].matrix._41;
			Model.pobject[i].pos_key[0].y = Model.pobject[i].matrix._42;
			Model.pobject[i].pos_key[0].z = Model.pobject[i].matrix._43;
	
			Model.pobject[i].pos_key[0].frame_num = 0;
		}

		D3DXMATRIX inverse;
		D3DXMatrixInverse(&inverse,NULL,&(Model.pobject[i].matrix));
			
		// local matrix get.
		D3DXMatrixMultiply(&world,&inverse,&world);
		
		device->SetTransform(D3DTS_WORLD,&world);	
		
		if(Model.pobject[i].pos_keyNum>0 || Model.pobject[i].rot_keyNum>0) {
			world = RotateObject(device,i,frame,&world);
			world = TranslateObject(device,i,frame,&world);	
			device->SetTransform(D3DTS_WORLD,&world);
		}
		// wordl matrix get.
		// 부모의 matrix을 계속 곱한다.
		int kt = i;
		if(Model.pobject[kt].bparent) {
			loop=true;
				while(loop) {
					for(j=0;j<Model.object_num;j++) {
						if(!strcmp(Model.pobject[kt].parent_name,Model.pobject[j].object_name)) {
							//parent ani func process
							world = RotateObject(device,j,frame,&world);
							world = TranslateObject(device,j,frame,&world);
		
							device->SetTransform(D3DTS_WORLD,&world);
							kt = j;
							break;

						}
					}
					if(j == Model.object_num)
						loop = false;
			
				}
		}

	//////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////			
		if(Model.pobject[i].bmorph) {
			vertex_interpolation(i,frame);
			Change_Vertexbuffer(i);
		}
		D3DXMatrixIdentity(&rot);
		D3DXMatrixRotationY(&rot,D3DXToRadian(m_yrot));
		D3DXMatrixMultiply(&world,&world,&rot);
		
		D3DXMatrixIdentity(&rot);
		D3DXMatrixRotationX(&rot,D3DXToRadian(m_xrot));
		D3DXMatrixMultiply(&world,&world,&rot);

		D3DXMatrixIdentity(&rot);
		D3DXMatrixRotationZ(&rot,D3DXToRadian(m_zrot));
		D3DXMatrixMultiply(&world,&world,&rot);

		world._41 += vecCenter.x;
		world._42 += vecCenter.y;
		world._43 += vecCenter.z;

		device->SetTransform(D3DTS_WORLD,&world);
		//////////////// dumy point set
		
		

		////////////////////
		//SetColor(D3DCOLOR_ARGB(255,255,255,255));
		// alpha set
		
		device->SetRenderState(D3DRS_SRCBLEND,SrcBlend);
		device->SetRenderState(D3DRS_DESTBLEND,DstBlend);
		
		device->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
		
		device->SetRenderState(D3DRS_TEXTUREFACTOR,Color);
		
		if(Model.pobject[i].btexture == true) {
			if(Model.pmaterial[Model.pobject[i].material_id].sub_num > 0) {
				//device->BeginScene();

				for(int k=0;k<object_sub[i].sub_num;k++) {	
					tex_id = Model.pobject[i].face[vcount].vertex_index[3];
					//잘못된 메터리얼 번호 수정
					if(tex_id >= Model.pmaterial[Model.pobject[i].material_id].sub_num) 
						tex_j = 0;
					else
						tex_j = Model.pmaterial[Model.pobject[i].material_id].sub[tex_id].tex_id;

					tex_i = Model.pmaterial[Model.pobject[i].material_id].tex_id;
					device->SetTexture(0,NULL);
					if(!m_NullTexture) 
						HRESULT hr = device->SetTexture(0,texture[tex_j].GetTexture());
					else	
						device->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f));
					
					device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
					device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
					device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
					device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

					device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
					device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
					device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

					device->SetStreamSource(0,object[i],sizeof(custom_vertex));
					device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
					//d_device->SetIndices(object_index[i],0);
					//d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,loader.Model.pobject[i].vertex_num,0,loader.Model.pobject[i].face_num);
				
				
					device->DrawPrimitive(D3DPT_TRIANGLELIST,vcount*3,object_sub[i].sub[k]);
					
					vcount +=object_sub[i].sub[k];

				}
				//device->EndScene();
				
			}
			else {
				device->SetTexture(0,NULL);
				if(!m_NullTexture)
					device->SetTexture(0,texture[Model.pmaterial[Model.pobject[i].material_id].tex_id].GetTexture());
				else
					device->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f));
				//device->SetTexture(0,texture[Model.pmaterial[Model.pobject[i].material_id].tex_id]);
				
			
				device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
				device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
				device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
				device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

				device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
				device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);	
				device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
				device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

			//	device->BeginScene();
				device->SetStreamSource(0,object[i],sizeof(custom_vertex));
				device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
				//d_device->SetIndices(object_index[i],0);
				//d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,loader.Model.pobject[i].vertex_num,0,loader.Model.pobject[i].face_num);
				int nFace=Model.pobject[i].face_num;
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,nFace);
			//	device->EndScene();
			}
					
		}
		else {

			//device->BeginScene();
			device->SetStreamSource(0,object[i],sizeof(D3DXVECTOR3));
			device->SetVertexShader(D3DFVF_XYZ);
			//d_device->SetIndices(object_index[i],0);
			//d_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,loader.Model.pobject[i].vertex_num,0,loader.Model.pobject[i].face_num);
			int nFace=Model.pobject[i].face_num;
			device->DrawPrimitive(D3DPT_TRIANGLELIST,0,nFace);
			//device->EndScene();
			
		} //texture not exist. 
		
		//world = push;
		//d_device->SetTransform(D3DTS_WORLD,&world);
	}
	if(m_NullTexture)
	device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	device->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

	//device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	device->SetTransform(D3DTS_WORLD,&back);
	return frame;
}
D3DXMATRIX CMeshMorph ::RotateObject(LPDIRECT3DDEVICE8 d_device,int object_index,float frame,D3DXMATRIX *w) {
	D3DXMATRIX trans;	
	D3DXMATRIX tk;

	static D3DXQUATERNION global;
	D3DXQUATERNION local;
	double interpol = 0.0;
	int k=0;

	if(Model.pobject[object_index].rot_keyNum) {
		for(k=0;k<Model.pobject[object_index].rot_keyNum;k++) {
			if(Model.pobject[object_index].rot_key[k].frame_num >=frame)
				break;
		}
			
		if(k !=0) {
			interpol = (double)(frame - Model.pobject[object_index].rot_key[k-1].frame_num)/(double)(Model.pobject[object_index].rot_key[k].frame_num - Model.pobject[object_index].rot_key[k-1].frame_num);
					
			if(k == Model.pobject[object_index].rot_keyNum) { 
				k-=1;
				interpol = 1.0;
			}
			if(interpol >1.0)
				interpol = 1.0;
			else if(interpol <0.0)
				interpol = 0.0;

			Model.pobject[object_index].rot_quatkey[k-1].y = - Model.pobject[object_index].rot_quatkey[k-1].y;
			Model.pobject[object_index].rot_quatkey[k].y = - Model.pobject[object_index].rot_quatkey[k].y;

			D3DXQuaternionSlerp(&global,&(Model.pobject[object_index].rot_quatkey[k-1]),&(Model.pobject[object_index].rot_quatkey[k]),interpol);
			
			Model.pobject[object_index].rot_quatkey[k-1].y = - Model.pobject[object_index].rot_quatkey[k-1].y;
			Model.pobject[object_index].rot_quatkey[k].y = - Model.pobject[object_index].rot_quatkey[k].y;

		}	
		else 
			D3DXQuaternionSlerp(&global,&(Model.pobject[object_index].rot_quatkey[k]),&(Model.pobject[object_index].rot_quatkey[k]),0);

		D3DXMatrixRotationQuaternion(&trans,&global);
				
		D3DXMatrixMultiply(w,w,&trans);
	//	d_device->SetTransform(D3DTS_WORLD,w);
		return *w;
				
	}
	else {
		return *w;
	}

}
D3DXMATRIX CMeshMorph::TranslateObject(LPDIRECT3DDEVICE8 d_device,int object_index,float frame,D3DXMATRIX *w) {
	int k = 0;
	double interpol = 0.0;
	D3DXVECTOR3 tmp_pos;
	D3DXVECTOR3 model_pos;
	D3DXMATRIX tm;
	D3DXMATRIX tk;
	D3DXMATRIX ta = *w;
	if(Model.pobject[object_index].pos_keyNum) {
		for(k=0;k<Model.pobject[object_index].pos_keyNum;k++) {
			if(Model.pobject[object_index].pos_key[k].frame_num >=frame)
				break;
		}
			
		if(k != 0) {
					
			interpol = (double)(frame - Model.pobject[object_index].pos_key[k-1].frame_num)/(double)(Model.pobject[object_index].pos_key[k].frame_num - Model.pobject[object_index].pos_key[k-1].frame_num);
					
			if(k >= Model.pobject[object_index].pos_keyNum) {  
				k-=1;
				interpol = 1.0;
			}
			if(interpol >1.0)
				interpol = 1.0;
			else if(interpol <0.0)
				interpol = 0.0;

			tmp_pos.x =	Model.pobject[object_index].pos_key[k].x - Model.pobject[object_index].pos_key[k-1].x;
			tmp_pos.y = Model.pobject[object_index].pos_key[k].y - Model.pobject[object_index].pos_key[k-1].y;
			tmp_pos.z = Model.pobject[object_index].pos_key[k].z - Model.pobject[object_index].pos_key[k-1].z;
			
			model_pos.x = Model.pobject[object_index].pos_key[k-1].x + tmp_pos.x*interpol;
			model_pos.y = Model.pobject[object_index].pos_key[k-1].y + tmp_pos.y*interpol;
			model_pos.z = Model.pobject[object_index].pos_key[k-1].z + tmp_pos.z*interpol;
			
		}
		else {
		
			tmp_pos.x =tmp_pos.y = tmp_pos.z = 0.0f;
			model_pos.x = Model.pobject[object_index].pos_key[k].x + tmp_pos.x*interpol;
			model_pos.y = Model.pobject[object_index].pos_key[k].y + tmp_pos.y*interpol;
			model_pos.z = Model.pobject[object_index].pos_key[k].z + tmp_pos.z*interpol;
		}
		
		D3DXMatrixTranslation(&tm,model_pos.x,model_pos.y,model_pos.z);
	
		
		ta._41  += tm._41;
		ta._42  += tm._42;
		ta._43  += tm._43;

		//d_device->SetTransform(D3DTS_WORLD,&ta);
		return ta;
				
	}	
	//pos key not exist
	else {	//TM * parent matrix
		tmp_pos.x =tmp_pos.y = tmp_pos.z = 0.0f;
		model_pos.x = Model.pobject[object_index].pos_key[k].x + tmp_pos.x*interpol;
		model_pos.y = Model.pobject[object_index].pos_key[k].y + tmp_pos.y*interpol;
		model_pos.z = Model.pobject[object_index].pos_key[k].z + tmp_pos.z*interpol;
		
		D3DXMatrixIdentity(&tm);
		D3DXMatrixTranslation(&tm,model_pos.x,model_pos.y,model_pos.z);
	
		
		ta._41  += tm._41;
		ta._42  += tm._42;
		ta._43  += tm._43;

		//d_device->SetTransform(D3DTS_WORLD,&ta);
		return ta;
	}
}
void CMeshMorph::Create_Vertex(int i,LPDIRECT3DDEVICE8 device) {
		int index_num = Model.pobject[i].face_num;
		if(Model.pobject[i].btexture) { 
			device->CreateVertexBuffer( index_num*sizeof(custom_vertex) * 3,0,
										 D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,D3DPOOL_MANAGED,&(object[i]));
		
			VOID *tmp;
			object[i]->Lock(0,index_num *sizeof(custom_vertex) * 3,
							(BYTE **)&tmp,0);
			memcpy(tmp,vert,index_num *sizeof(custom_vertex) * 3);
		
			object[i]->Unlock();
		
			delete[] vert;
			vert = NULL;
		}
		else {
			device->CreateVertexBuffer(index_num*3*sizeof(D3DXVECTOR3),0,
										D3DFVF_XYZ,D3DPOOL_MANAGED,&(object[i]));
			VOID *tmp;
			object[i]->Lock(0,index_num*3*sizeof(D3DXVECTOR3),
						(BYTE **)&tmp,0);
			memcpy(tmp,vert2,index_num*3*sizeof(D3DXVECTOR3));
			object[i]->Unlock();
			delete[] vert2;
			vert2 = NULL;
		
		}	

}

void CMeshMorph::Change_Vertexbuffer(int object_index){
	/////////////// face 번호에 따라 다른 버텍스 버퍼에 넣어줌
	int index_num = 0;
	int vertex_num = 0;
	int j;
	int k=0;
	int tmp_index[4] = {0,0,0,0};
	int tmp_coord[3] = {0,0};

	vertex_num = Model.pobject[object_index].vertex_num;
	index_num = Model.pobject[object_index].face_num;
		
	if(!Model.pobject[object_index].btexture) {
		vert2 = new D3DXVECTOR3[index_num*3];
			
		for(j=0;j<index_num;j++) {
			memcpy(tmp_index,&(Model.pobject[object_index].face[j].vertex_index),sizeof(int)*4);
			
			vert2[k++] = Model.pobject[object_index].vertex[tmp_index[0]];
			vert2[k++] = Model.pobject[object_index].vertex[tmp_index[1]];
			vert2[k++] = Model.pobject[object_index].vertex[tmp_index[2]];
		}
	}
	else if(Model.pobject[object_index].btexture) {
		vert = new custom_vertex[index_num*3];
		for(j=0;j<index_num;j++) {
			
			memcpy(tmp_index,&(Model.pobject[object_index].face[j].vertex_index),sizeof(int)*4);
			memcpy(tmp_coord,&(Model.pobject[object_index].face[j].coord_index),sizeof(int)*3);

			vert[k].x = Model.pobject[object_index].vertex[tmp_index[0]].x;
			vert[k].y = Model.pobject[object_index].vertex[tmp_index[0]].y;
			vert[k].z = Model.pobject[object_index].vertex[tmp_index[0]].z;
				
			vert[k].nx = Model.pobject[object_index].normal[tmp_index[0]].x;
			vert[k].ny = Model.pobject[object_index].normal[tmp_index[0]].y;
			vert[k].nz = Model.pobject[object_index].normal[tmp_index[0]].z;
		

			vert[k].s = Model.pobject[object_index].texcoord[tmp_coord[0]].x;
			vert[k++].t = Model.pobject[object_index].texcoord[tmp_coord[0]].y;
				
			vert[k].x = Model.pobject[object_index].vertex[tmp_index[1]].x;
			vert[k].y = Model.pobject[object_index].vertex[tmp_index[1]].y;
			vert[k].z = Model.pobject[object_index].vertex[tmp_index[1]].z;

			vert[k].nx = Model.pobject[object_index].normal[tmp_index[1]].x;
			vert[k].ny = Model.pobject[object_index].normal[tmp_index[1]].y;
			vert[k].nz = Model.pobject[object_index].normal[tmp_index[1]].z;
		

			vert[k].s = Model.pobject[object_index].texcoord[tmp_coord[1]].x;
			vert[k++].t = Model.pobject[object_index].texcoord[tmp_coord[1]].y;

			vert[k].x = Model.pobject[object_index].vertex[tmp_index[2]].x;
			vert[k].y = Model.pobject[object_index].vertex[tmp_index[2]].y;
			vert[k].z = Model.pobject[object_index].vertex[tmp_index[2]].z;

			vert[k].nx = Model.pobject[object_index].normal[tmp_index[2]].x;
			vert[k].ny = Model.pobject[object_index].normal[tmp_index[2]].y;
			vert[k].nz = Model.pobject[object_index].normal[tmp_index[2]].z;
			

			vert[k].s = Model.pobject[object_index].texcoord[tmp_coord[2]].x;
			vert[k++].t = Model.pobject[object_index].texcoord[tmp_coord[2]].y;
		}
	}

/////////////////////////////////
// vertex buffer create
////////////////////////////////

	if(Model.pobject[object_index].btexture) { 
		VOID *tmp;
		object[object_index]->Lock(0,index_num *sizeof(custom_vertex) * 3,
						(BYTE **)&tmp,0);
		memcpy(tmp,vert,index_num *sizeof(custom_vertex) * 3);

		object[object_index]->Unlock();
		
			delete[] vert;
			vert = NULL;
	}
	else {
			
		VOID *tmp;
		object[object_index]->Lock(0,index_num*3*sizeof(D3DXVECTOR3),
						(BYTE **)&tmp,0);
		memcpy(tmp,vert2,index_num*3*sizeof(D3DXVECTOR3));
		object[object_index]->Unlock();
		delete[] vert2;
		vert2 = NULL;
		
		}
	k=0;
}
void CMeshMorph::vertex_interpolation(int object_index,float now_frame) {
	
	D3DXVECTOR3 tmp;
	D3DXVECTOR3 move_vector;
	D3DXVECTOR3 next_f;
	D3DXVECTOR3 before_f;
	
	D3DXVECTOR2 next_ft;
	D3DXVECTOR2 before_ft;
	D3DXVECTOR2 tmp_t;

	D3DXVECTOR3 next_n;
	D3DXVECTOR3 before_n;
	D3DXVECTOR3 tmp_n;
	int next_frame;
	int before_frame;
	double interpol = 0.0;


	for(int i=0;i<Model.pobject[object_index].morph_num;i++) {
		if(Model.pobject[object_index].morph[i].keynum > now_frame) {
			next_frame = i;
			before_frame = i-1;
			break;
		}
	}
	if(i != Model.pobject[object_index].morph_num && i !=0) {		
		
		interpol = (double)(now_frame - Model.pobject[object_index].morph[before_frame].keynum) / (double)(Model.pobject[object_index].morph[next_frame].keynum - Model.pobject[object_index].morph[before_frame].keynum);
	
		if(interpol <0.0f)
			interpol = 0.0f;
		else if(interpol >1.0f)
			interpol = 1.0f;
		for(int j=0;j<Model.pobject[object_index].vertex_num;j++) {
			memcpy(&next_f,&(Model.pobject[object_index].morph[next_frame].vertex[j]),sizeof(D3DXVECTOR3));
			memcpy(&before_f,&(Model.pobject[object_index].morph[before_frame].vertex[j]),sizeof(D3DXVECTOR3));
			
			memcpy(&next_ft,&(Model.pobject[object_index].morph[next_frame].texcoord[j]),sizeof(D3DXVECTOR2));
			memcpy(&before_ft,&(Model.pobject[object_index].morph[before_frame].texcoord[j]),sizeof(D3DXVECTOR2));

			memcpy(&next_n,&(Model.pobject[object_index].morph[next_frame].normal[j]),sizeof(D3DXVECTOR3));
			memcpy(&before_n,&(Model.pobject[object_index].morph[before_frame].normal[j]),sizeof(D3DXVECTOR3));

			tmp.x = next_f.x - before_f.x;
			tmp.y = next_f.y - before_f.y;
			tmp.z = next_f.z - before_f.z;
			
			tmp_t.x = next_ft.x - before_ft.x;
			tmp_t.y = next_ft.y - before_ft.y;
			
			tmp_n.x = next_n.x - before_n.x;
			tmp_n.y = next_n.y - before_n.y;
			tmp_n.z = next_n.z - before_n.z;
			
			Model.pobject[object_index].vertex[j].x = before_f.x + tmp.x * interpol;
			Model.pobject[object_index].vertex[j].y = before_f.y + tmp.y * interpol;
			Model.pobject[object_index].vertex[j].z = before_f.z + tmp.z * interpol;
			
			Model.pobject[object_index].texcoord[j].x = before_ft.x + tmp_t.x * interpol;
			Model.pobject[object_index].texcoord[j].y = before_ft.y + tmp_t.y * interpol;
		
			
			Model.pobject[object_index].normal[j].x = before_n.x + tmp_n.x * interpol;
			Model.pobject[object_index].normal[j].y = before_n.y + tmp_n.y * interpol;
			Model.pobject[object_index].normal[j].z = before_n.z + tmp_n.z * interpol;
			
		}	
	}
	
}
void CMeshMorph ::CreateTexture(LPDIRECT3DDEVICE8 device) {
	int i,j;
	
	int texture_num = 0;

	int mat_num = Model.material_num;
	int subnum;
	

	////////////////material num/////////////////
	for(i=0;i<mat_num;i++) {
		subnum = Model.pmaterial[i].sub_num;
		if(subnum >0)
			texture_num +=subnum;
		else
			texture_num++;
	}
	CTexture::SetPath(OBJECTTEXTUREPATH);
	
	texture = new CTexture[texture_num];
	texture_count = 0;

	for(i=0;i<mat_num;i++) {
		
		subnum = Model.pmaterial[i].sub_num;
		if(subnum >0) {
			for(j=0;j<subnum;j++) {
				
				if(strlen(Model.pmaterial[i].sub[j].texture_file) >0) {
					
					texture[texture_count].Load(Model.pmaterial[i].sub[j].texture_file);
					Model.pmaterial[i].sub[j].tex_id = texture_count++;
				}
				else 
					Model.pmaterial[i].sub[j].tex_id = -1;
			}
		}
		else {
			
			if(strlen(Model.pmaterial[i].texture_file) >0) {
				texture[texture_count].Load(Model.pmaterial[i].texture_file);	
				Model.pmaterial[i].tex_id = texture_count++;
			}
			else 
				Model.pmaterial[i].tex_id = -1;
		}
				
	}
}
void CMeshMorph :: LoadSubface(int object_index) {
	int i,j,k=0;
	int tmp;
	int face_num = Model.pobject[object_index].face_num;
	
	for(i=0;i<face_num;i++) {
		if(i == 0)	{
			j = Model.pobject[object_index].face[i].vertex_index[3];
			object_sub[object_index].sub.push_back(tmp);
			object_sub[object_index].sub[k] = 0;
			object_sub[object_index].sub_num = 0;

			object_sub[object_index].sub[k]++;
			object_sub[object_index].sub_num++;
		}
		else if(j != Model.pobject[object_index].face[i].vertex_index[3]) {
			k++;
			j = Model.pobject[object_index].face[i].vertex_index[3];
			object_sub[object_index].sub.push_back(tmp);
			object_sub[object_index].sub_num++;
			object_sub[object_index].sub[k] = 0;
			object_sub[object_index].sub[k]++;
		}
		else {
			object_sub[object_index].sub[k]++;
		}
	}	
}