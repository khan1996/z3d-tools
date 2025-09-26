#include "boidscene2.h"
#include "GMMemory.h"



CBoidGroup::CBoidGroup() {
	
	memset(m_BoidName,0,sizeof(char) * 256);

	CloseGroup = -1;
	Space = LAND;
	limit.x = limit.y = limit.z = BOIDUNLIMIT;
	SeenRad = SEENDEFAULT;
	Pattern = BOIDGROUP;
	SubPattern = RELEASE;
	FearValue = FEARFULL;
	Unit_Num = 0;
	Lod = NOT;

	Units = NULL;
	BoidBox = BOIDRAD;
	fwd.x = 1.0f;
	fwd.y = 0.0f;
	fwd.z = 0.3f;
	BoidMesh = NULL;
	Boid_Range.x = 0.0f;
	Boid_Range.y = 0.0f;
	Boid_Range.z = 0.0f;
	Boid_Speed = 1.0f;

	m_SetPos.x = m_SetPos.y= m_SetPos.z = 0.0f;

	srand(time(0));
	pat = 0;
	m_Pick = false;
	m_LimitHeight = 0.0f;
	m_CurrentFrame = timeGetTime();
	m_BeforeFrame = m_CurrentFrame;
	m_BoidKind = 0;

	startup = false;
	tick = 0;


}	
void CBoidGroup::SetBoidName(char *filename,char *path) {
	sprintf(m_BoidName,"%s\\%s",path,filename);


}
char *CBoidGroup::GetBoidName() {
	return m_BoidName;
}
// 회피 :: 장애물이나 다른 적들과 만나지 않게 회피
void CBoidGroup::avoid()
{
}
//////////////////////////////////////////////////////////////////////////////////////////
// 지형과의 충돌

//회피 :: 지형과의 충돌 회피
void CBoidGroup::HeightSeparate(BoidUnit *boid,D3DXVECTOR3 *vec) {

	//////////////////추가되면서 살아나야 한다.
/*
	D3DXVECTOR3 field;
	D3DXVECTOR3 change;

	// 현제 pos 구하기(구현해야함)
	//GetHeigh(&field);
	//boid 위치 보다 지형이 더 높다면..
	if(boid->pos.y < field.y) {
		field.y = boid->pos.y;
		change = boid->pos - field;
		// 위치 복구
		boid->pos = boid->before_pos;

		D3DXVec3Normalize(&change,&change);
		change *=MAX;

		(*vec) +=change;
	}
*/
}

////////////////////////////////////////////////////////////////////////////////////////

//응집 :: 같은 그룹의 boid 들과 목표 위치를 같게 만들기
void CBoidGroup::concenter(BoidUnit *boid,D3DXVECTOR3 *vec) {
	D3DXVECTOR3 div_vec(0.0f,0.0f,0.0f);
	float length;
	int select_num = 0;
	D3DXVECTOR3 tmp(0.0f,0.0f,0.0f);
	
	for(int i=0;i<Unit_Num;i++) {
		div_vec = boid->pos - Units[i].pos;
		length = D3DXVec3Length(&div_vec);
		if(length <BOIDRAD) {
			tmp +=Units[i].pos;
			select_num++;
		}
	}
	if(select_num) {
		tmp /= (float)select_num;
		
		tmp -= boid->pos;
		D3DXVec3Normalize(&tmp,&tmp);
		tmp *=BMIN;
		
		(*vec) +=tmp;
	}
}
void CBoidGroup::SetPos(float x,float y,float z) {
	m_SetPos.x = x;
	m_SetPos.y = y;
	m_SetPos.z = z;

}
// 충돌 피하기 :: 같은 그룹의 boid 들 끼리 부딛히지 않게.
void CBoidGroup::separate(BoidUnit *boid,D3DXVECTOR3 *vec) {

	float length;
	
	float r;
	int min_index = -1;
	float min_length;

	D3DXVECTOR3 min_vec(0.0f,0.0f,0.0f);
	D3DXVECTOR3 change(0.0f,0.0f,0.0f);

	float k = BOIDMAXLIMIT;

	D3DXVECTOR3 tmp;
	
	for(int i=0;i<Unit_Num;i++) {
		if(boid != (&Units[i])) {
			tmp = boid->pos - Units[i].pos;
			
			length = D3DXVec3Length(&tmp);
		
			if(length < k) {
				min_index = i;
				min_length = length;	
				min_vec = Units[i].pos;
				k = length;
			}
				
		}
	}
	if( min_index == -1 )
		return;

	// 최소 거리/피하는 범위
	r = min_length/(BOIDRAD);
//	if(min_length < BOIDRAD) {
		change = Units[min_index].pos - boid->pos;

		// 너무 작거나 클때 r비율이 (0.05~0.1 사이)
		if (r < BMIN) 
			r = BMIN;
		if (r > BMAX) 
			r = BMAX;
		// 충돌 범위 안일때
		if (min_length < BOIDRAD) {
			D3DXVec3Normalize(&change,&change);
			change *=(-0.01f);
			//누적
			(*vec) +=change;
		} 
		// 충돌 범위 밖일때
		else if (min_length> BOIDRAD) {
			D3DXVec3Normalize(&change,&change);
			change *=0.01f;
			//누적
			(*vec) +=change;
		} 
//	}
}

//정렬 :: 같은 그룹의 boid 들과 방향벡터를 같게 만들기
void CBoidGroup::align(BoidUnit *boid,D3DXVECTOR3 *vec) {
	
	float k = BOIDMAXLIMIT;
	int min_index = -1;
	
	float length;

	D3DXVECTOR3 change(0.0f,0.0f,0.0f);
	D3DXVECTOR3 tmp(0.0f,0.0f,0.0f);
	
	for(int i=0;i<Unit_Num;i++) {
		if(boid != (&Units[i])) {
			
			tmp = boid->pos - Units[i].pos;
			length = D3DXVec3Length(&tmp);
			
			if(length < k) {
				min_index = i;
				k = length;
			}
		}
	}
	if(min_index != -1) {
		change = Units[min_index].fwd;
		D3DXVec3Normalize(&change,&change);
		change *=BMIN;
		(*vec) +=change;
	}


}
void CBoidGroup::RandomRot(CBoidGroup::BoidUnit *boid,float rot) {
	static int minus = 1;
	int mulyaxis = false;
	D3DXVECTOR3 tmp = boid->fwd;
	D3DXMATRIX mat;
	D3DXMATRIX ver;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixIdentity(&ver);

	ver._41 = tmp.x;
	ver._42 = tmp.y;
	ver._43 = tmp.z;

	minus = minus > 0 ? -1 : 1;

	
	D3DXMatrixRotationY(&mat,D3DXToRadian(rot * (float)minus));
	//D3DXMatrixRotationX(&mat,D3DXToRadian(rot * (float)minus));
	

	D3DXMatrixMultiply(&mat,&ver,&mat);

	boid->fwd.x = mat._41;
	boid->fwd.y = mat._42;
	boid->fwd.z = mat._43;
	
/*	do {
		if(fabs(boid->fwd.y) < 0.1f)
			mulyaxis = true;
		else 
			mulyaxis = false;
		boid->fwd.y *=10.0f;
	} while(mulyaxis);*/

}
// 경계를 벗어난 boid의 방향 벡터 재 설정
void CBoidGroup::Bound(CBoidGroup::BoidUnit *boid) {
	D3DXVECTOR3 tmp = boid->fwd;
	D3DXMATRIX mat;
	D3DXMATRIX ver;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixIdentity(&ver);
	ver._41 = tmp.x;
	ver._42 = tmp.y;
	ver._43 = tmp.z;

	switch(m_BoidKind) {
	case 0:
		D3DXMatrixRotationY(&mat,D3DXToRadian(2.0f));
		break;
	case 1:
		D3DXMatrixRotationY(&mat,D3DXToRadian(15.0f));
		break;
	}
	D3DXMatrixMultiply(&mat,&ver,&mat);
	
	//float tmp;

	float size_x = Boid_Range.x;
	float size_z=  Boid_Range.z;
	
	// 방향을 바꾸지 않은 상태
	
	if(((boid->pos.x - m_SetPos.x) > size_x) || ((m_SetPos.x - boid->pos.x) > size_x)) {
	//	tmp = (boid->fwd.x) * (-0.4f);
	//	boid->fwd.x -= tmp;
		boid->fwd.x = mat._41;
		boid->fwd.y = mat._42;
		boid->fwd.z = mat._43;
	

	}
	if(((boid->pos.z - m_SetPos.z) > size_z) || ((m_SetPos.z - boid->pos.z) > size_z)) {
	//	tmp = (boid->fwd.z) * (-0.4f);
	//	boid->fwd.z -=tmp;
		boid->fwd.x = mat._41;
		boid->fwd.y = mat._42;
		boid->fwd.z = mat._43;
			
	}
	
		
	
	/*
	if(boid->pos.x <(float)(TEST_SECNUM * TEST_SECSIZE)) 
		boid->pos.x = ((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE);
	else if(boid->pos.x >((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE))
		boid->pos.x = (float)(TEST_SECNUM * TEST_SECSIZE);

	if(boid->pos.y <(float)(TEST_SECNUM * TEST_SECSIZE))
		boid->pos.y = ((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE);
	else if(boid->pos.y >((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE))
		boid->pos.y = (float)(TEST_SECNUM * TEST_SECSIZE);
		
	if(boid->pos.z <(float)(TEST_SECNUM * TEST_SECSIZE))
		boid->pos.z = ((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE);
	else if(boid->pos.z >((float)(TEST_SECNUM * TEST_SECSIZE) + TEST_SECSIZE))
		boid->pos.z = (float)(TEST_SECNUM * TEST_SECSIZE);
	
*/
	
}
CBoidGroup::~CBoidGroup() {

	if(Unit_Num || (Units != NULL)) {
		delete[] Units;
		Units = NULL;
	}
/*	if(BoidMesh) 
		delete BoidMesh;*/
}
void CBoidGroup::SetUnitNum(int n) {
	Unit_Num = n;
	Units = new BoidUnit[Unit_Num];

	SetUnit();

}

//최종 단계 rot
void CBoidGroup::RotSet(CBoidGroup::BoidUnit *U) {
	float  roll, pitch, yaw;
	float lateDot;
	
	D3DXVECTOR3 late,tmp;
	D3DXVECTOR3 tmp2;

	// stop 상태
	if(U->fwd == U->before_fwd)
		return;
		
	tmp = U->fwd - U->before_fwd;
					
	D3DXVec3Cross(&tmp2,&U->fwd,&tmp);
					 

	D3DXVec3Cross(&late,&tmp2,&U->fwd);

	D3DXVec3Normalize(&late,&late);

	lateDot = D3DXVec3Dot(&tmp,&late);
	if (lateDot == 0) {
		roll = 0.0f;  
	} 
	else {
		roll = (float) -atan2(BGRAVITY, lateDot) + BHALF_PI;
	}

	pitch = (float) -atan(U->fwd.y / sqrt((U->fwd.z * U->fwd.z ) + (U->fwd.x * U->fwd.x)));

	yaw = (float) atan2(U->fwd.x, U->fwd.z);
					
	U->rot[0] = pitch;
	U->rot[1] = yaw;
	U->rot[2] = roll;
}
void CBoidGroup::SetFwd() {
	int i;
	for(i=0;i<Unit_Num;i++) {
		Units[i].fwd.x = BRAND();
		Units[i].fwd.y = BRAND();
		Units[i].fwd.z = BRAND();

		if(rand()%2)
			Units[i].fwd.x *=-1.0f;
		if(rand()%2)
			Units[i].fwd.y *=-1.0f;
		if(rand()%2)
			Units[i].fwd.z *=-1.0f;
		if(rand()%2) {
			//Units[i].fwd.x = Units[i].fwd.y = Units[i].fwd.z = 0.0f;
			//Units[i].before_fwd.x = Units[i].before_fwd.y = Units[i].before_fwd.z = 0.0f;
		}

		//fwd length
		Units[i].speed = D3DXVec3Length(&Units[i].fwd);
	}

}
void CBoidGroup::separate2(BoidUnit *boid) {
	int i;
	D3DXVECTOR3 tmp;
	D3DXVECTOR3 tf;
	float length;
	D3DXVECTOR3 min_vec;
	int min_index;
	float k = BOIDMAXLIMIT;

	for(i=0;i<Unit_Num;i++) {
		if(boid != (&Units[i])) {
			tmp = boid->pos - Units[i].pos;
			length = D3DXVec3Length(&tmp);

			if(length < k) {
				min_vec = tmp;
				min_index = i;
				k= length;	
			}
		}
	}
	if(k < BOIDRAD) {
		D3DXVec3Normalize(&min_vec,&min_vec);
		D3DXVec3Normalize(&tf,&boid->fwd);
		tmp = tf + min_vec;
		//튕기기
		tmp *=BMIN;
		boid->fwd += tmp;
	}
}
void CBoidGroup::Update() {
	int i,j,count; 

	

	if(startup == false) {
		m_CurrentFrame = timeGetTime();
		m_BeforeFrame = m_CurrentFrame;
		startup = true;
	}

	m_CurrentFrame = timeGetTime();

	count = (int)(m_CurrentFrame - m_BeforeFrame)/30;

	for(j=0;j<count;j++) {
		
		m_CurrentFrame = timeGetTime();
		m_BeforeFrame = m_CurrentFrame;

		D3DXVECTOR3 change(0.0f,0.0f,0.0f);
		
		if(!(tick % 35))
			pat = (pat >0) ? 0:1;
		tick++;
		//pat = 1;

		for(i=0;i<Unit_Num;i++) {
			
			Units[i].frame+=(Units[i].vot);
			
			if((Units[i].frame * BoidMesh->m_UnitF) >= BoidMesh->m_EndF) {
					
					float kvalue = (float)((float)(Units[i].frame * BoidMesh->m_UnitF) / BoidMesh->m_EndF);
					Units[i].frame  = (Units[i].frame * BoidMesh->m_UnitF) - (BoidMesh->m_EndF * kvalue);
					Units[i].frame /= (float)BoidMesh->m_UnitF;

					if(Units[i].frame <= 0.0f)
						Units[i].frame = 0.0f;
						
					//Units[i].vot = Units[i].backup_vot;

			}
				

				
		/*	if((Units[i].frame * BoidMesh->m_UnitF) >= BoidMesh->m_EndF) {
				
				Units[i].frame = (BoidMesh->m_StartF / BoidMesh->m_UnitF);
				Units[i].vot = Units[i].backup_vot;

			}
		*/
		//	if(Units[i].active != SLEEP) {

				Units[i].before_pos = Units[i].pos;
				Units[i].before_fwd = Units[i].fwd;
				Units[i].pos +=(Units[i].fwd);	
				// 집단 행동 설정시
				if(pat) {		
				
					// 같은 group의 boid 들 끼리의 충돌 방지(기본 반지름 2.0f)
					// 흔들림이 있어서 BOIDRAD 안의 것들만 적용되게 해놓았음
					separate(&Units[i],&change);
					// 옆의 boid 와 방향을 비슷 하게 만든다
					align(&Units[i],&change);
					//응집 :: 같은 그룹의 boid 들과 목표 위치를 같게 만들기
					concenter(&Units[i],&change);
					randomvec(&Units[i],&change);
						//계산되어진 결과 추가
					if(D3DXVec3Length(&change) > BMAXCHANGE) {
						D3DXVec3Normalize(&change,&change);
						change *=BMAXCHANGE;		
					}
				
				}
				else {
					
					separate2(&Units[i]);
				}
					// 지형과의 높이 체크
				//HeightSeparate(&Units[i],&change);
				
				Units[i].fwd +=change;

				// 흔들림 조절 (y 축 값 조정)
				Units[i].fwd.y *= 0.4f;
				
				
				//butterfly(Units[i]);

				//speed 조절 (1.0)이하로
			   if ((Units[i].speed = D3DXVec3Length(&Units[i].fwd)) > BMAXSPEED) {
					D3DXVec3Normalize(&Units[i].fwd,&Units[i].fwd);
					Units[i].fwd *=(BMAXSPEED);
					Units[i].speed = BMAXSPEED;
			   }
			   // 속도 조절 
				(Units[i].fwd) *= Boid_Speed;
				switch(m_BoidKind) {
				case 0:
					break;

				case 1:
					if(!(tick % (30 /*+ (15 - (int)Boid_Speed)*/)))
					RandomRot(&Units[i],25.0f); 
					break;

				}
				//  방향의 지그재그 적용
				// 나비 일때 로 세팅 중 
				// 속도가 15일때 20 프레임에 한번씩 업데이트
		
				
				Bound(&Units[i]);
				
				RotSet(&Units[i]);
				// 경계 박스와의 충돌시 방향 벡터 수정
				
			/*	if(!pat) {
					//테스트 Land live
					if(Units[i].height<0)
						Units[i].pos.y++;
					else if(Units[i].height>0) {
						Units[i].pos.y--;
					}
					if(Units[i].pos.y == 500 || Units[i].pos.y == 0)
						Units[i].height *= (-1);
				}*/
		//	}
		}
	}

}
void CBoidGroup::randomvec(BoidUnit *boid,D3DXVECTOR3 *vec) {
/*	
	D3DXVECTOR3 tmp(0.0f,0.0f,0.0f);
	

   float  diff = (boid->speed - 0.05f)/ MAX;
   float  urgency = (float) fabs(diff);


   if (urgency < MIN) urgency = MIN;
   if (urgency > MAX) urgency = MAX;

   float jitter = RAND();

   if (jitter < 0.45f) {
      vec->x += MIN * SIGN(diff);
   } else if (jitter < 0.90f) {
      vec->z += MIN * SIGN(diff);
   } else {
      vec->y += MIN * SIGN(diff);  
   }

   if(rand() %2)
		(*vec) *= -0.001;
	else
		(*vec) *= 0.001;	
	//if(rand() %10 == 7)

/*	if(rand() %2) {
		tmp.x = rand()%10 -5;
		tmp.y = rand()%10 -5;
		tmp.z = rand()%10 -5;
		
		(*vec)  += tmp;	
	//	if(rand() %2)
	//		boid->fwd.x = boid->fwd.y = boid->fwd.z = 0.0f;
	}*/
}
void CBoidGroup::SetUnitVot(float vot) {
	for(int i=0;i<Unit_Num;i++) {
		Units[i].vot = vot;
		Units[i].backup_vot = vot;

	}
}
void CBoidGroup::SetUnit() {
	int i;
	// boid 별 시작프레임 set
	float last_frame =  (float)((float)BoidMesh->m_EndF / BoidMesh->m_UnitF);

	float frame_set[6] = {0.0f,
						last_frame * (1.0f/5.0f),
						last_frame * (2.0f/5.0f),
						last_frame * (3.0f/5.0f),
						last_frame * (4.0f/5.0f),
						last_frame * (5.0f/5.0f),
	};

	//SECTORSIZE

	for(i=0;i<Unit_Num;i++) {
		Units[i].rot[0] = Units[i].rot[1] = Units[i].rot[2] = 0.0f;
		
		// pos		
		Units[i].pos.x = m_SetPos.x + (float)(rand()%(int)Boid_Range.x);
		Units[i].pos.y = m_SetPos.y + (float)(rand()%(int)Boid_Range.y);
		Units[i].pos.z = m_SetPos.z + (float)(rand()%(int)Boid_Range.z);
		
		if(Units[i].pos.y < m_SetPos.y + m_LimitHeight) { // 최저 높이 세팅 
			Units[i].pos.y = m_SetPos.y + m_LimitHeight;
			
		}

		// live
		Units[i].Live = true;
		Units[i].active = rand()%5;
		
		Units[i].fwd.x = BRAND();
		Units[i].fwd.y = BRAND();
		Units[i].fwd.z = BRAND();
		
		Units[i].height = -1;

		if(rand()%2)
			Units[i].fwd.x *=-1.0f;
		if(rand()%2)
			Units[i].fwd.y *=-1.0f;
		if(rand()%2)
			Units[i].fwd.z *=-1.0f;
		
		//fwd length
		Units[i].speed = D3DXVec3Length(&Units[i].fwd);

		Units[i].vot = 1.0f;
		Units[i].frame  = frame_set[(rand() % 6)];

		Units[i].backup_frame = Units[i].frame;

		//BoidMesh->SetVot(Units[i].vot);

	/*	//speed
		switch(SubPattern) {
			case RELEASE:
				//Units[i].fwd.x = 0.1f;
				//Units[i].fwd.y = 0.0f;
				//Units[i].fwd.z = 1.0f;

		
				break;
			case AVOID:

				Units[i].fwd.x = fwd.x + (float)(rand()%1000)/1000.0f - 0.5f;
				Units[i].fwd.y = fwd.y + (float)(rand()%1000)/1000.0f - 0.5f;
				Units[i].fwd.z = fwd.z + (float)(rand()%1000)/1000.0f - 0.5f;

			 break;
		}
		
		switch(Units[i].active) {
			case SLEEP:
				Units[i].fwd.x = Units[i].fwd.y = Units[i].fwd.z =0.0f;
				break;
			case PEACE:
				Units[i].fwd.x = (float)(rand()%1000)/1000.0f - 0.5f;
				Units[i].fwd.y = 0.0f;//(float)(rand()%1000)/1000.0f - 0.5f;
				Units[i].fwd.z = (float)(rand()%1000)/1000.0f - 0.5f;
				Units[i].fwd *= 0.1f;
				break;
			case WALK:
				Units[i].fwd.x = fwd.x;
				Units[i].fwd.y = 0.0f;//fwd.y;
				Units[i].fwd.z = fwd.z;
				Units[i].fwd *=0.01f;
				break;
			case RUN:
				Units[i].fwd.x = fwd.x;
				Units[i].fwd.y = 0.0f;//fwd.y;
				Units[i].fwd.z = fwd.z;
				break;
			case FLYING:
				Units[i].fwd.x = fwd.x;
				Units[i].fwd.y = fwd.y;
				Units[i].fwd.z = fwd.z;
				break;
		}

	*/	

		switch(Space) {
			case LAND:
			//	Units[i].pos.y = GetHeight(Units[i].pos.x,Units[i].pos.z);
			//	Units[i].speed.y = 0.0f;
			//	Units[i].pos.y = 0.0f;	
			//	Units[i].pos.y = 0.0f;
			//	Units[i].fwd.y = 0.0f;
				
			

				break;
			case WATER:
				break;
			case FLY:
				break;
		}
	
	}
	
}

void CBoidGroup::butterfly(CBoidGroup::BoidUnit &Unit){
	//나비의 y축 움직임
//	Unit.fwd.y += 0.3f;
	if(Unit.m_fly<100) {
		
		Unit.pos.y += 1.0f;
		Unit.m_fly++;
	}
	else if(Unit.m_fly >=100 && Unit.m_fly <=200){
		Unit.pos.y -= 1.0f;
		Unit.m_fly++;	
	}
	else if(Unit.m_fly >200){
		Unit.m_fly = 0;
	}
	
}
 bool CBoidGroup::Render(LPDIRECT3DDEVICE8 device) {
	D3DXMATRIX world; 
	device->GetTransform(D3DTS_WORLD,&world);
	

	D3DXMATRIX back = world;
	D3DXMATRIX quater;
	D3DXQUATERNION qt;
	int i;
	
	
	D3DXMatrixIdentity(&quater);
	int lod = 0;
	switch(lod) {
		case NOT:
			for(i=0;i<Unit_Num;i++) {
				world = back;
			

				D3DXQuaternionRotationYawPitchRoll(&qt,Units[i].rot[1],Units[i].rot[0],Units[i].rot[2]);
				D3DXMatrixRotationQuaternion(&world,&qt);
				
				world._41 = Units[i].pos.x;
				world._42 = Units[i].pos.y;
				world._43 = Units[i].pos.z;

				if(strstr(m_BoidName,"Boid_Gull_T1.GEM") != NULL)
					world._42 += 1500.0f;


				
			
				BoidMesh->SetTransMatrix(world);
				
				/*if((Units[i].frame * BoidMesh->m_UnitF) >= BoidMesh->m_EndF) {
					
					float kvalue = (float)((float)(Units[i].frame * BoidMesh->m_UnitF) / BoidMesh->m_EndF);
					Units[i].frame  = (Units[i].frame * BoidMesh->m_UnitF) - (BoidMesh->m_EndF * kvalue);
					Units[i].frame /= (float)BoidMesh->m_UnitF;

					if(Units[i].frame <= 0.0f)
						Units[i].frame = 0.0f;
						
					//Units[i].vot = Units[i].backup_vot;

				}
				*/
				/*
				if((Units[i].frame * BoidMesh->m_UnitF) >= BoidMesh->m_EndF) {
				
					//Units[i].frame = (BoidMesh->m_StartF / BoidMesh->m_UnitF);
					Units[i].frame = (int)(Units[i].frame * BoidMesh->m_UnitF) % (int)BoidMesh->m_EndF;
					if(Units[i].frame <= 0.0f)
						Units[i].frame = 0.0f;
					else
						Units[i].frame /= (float)BoidMesh->m_UnitF;
					
					Units[i].vot = Units[i].backup_vot;

				}
				*/

				//BOID 마다 플레이 프레임 다르게
				BoidMesh->SetCurrentFrame(Units[i].frame);
				BoidMesh->SetBlend(D3DBLEND_ONE,D3DBLEND_ZERO);
				BoidMesh->SetNullTexture(m_Pick);
				BoidMesh->Render();

				//Update();
				
				
				//BoidMesh->Update();
			}
			break;
		case MIDDLE:
			break;
	}
	return true;
}
