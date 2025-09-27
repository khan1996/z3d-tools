#include <winsock2.h>
#include "SoundMgr.h"
#include "CEffscript.h"
#include "BaseDataDefine.h"
#include "SceneNode.h"

#include "CameraControl.h"
#include "RYLObjectControl.h"

#include "EffDebugLog.h"

#include <Item/ItemConstants.h>

#include "RYLCreatureManager.h"

//#define __EFF_WCREATOR__		// World Creator 컴파일 할시에만 define 해준다		

#include <Utility/Exception.h>
#include "GMMemory.h"


// Pool 사용시에 메모리 해제가 제대로 안되고 있음.. by Vincent
//const char* szName = "이펙트 스크립터";
//CFixedPool CEffScript::ms_myPool(sizeof(CEffScript), 200, szName, strlen(szName));

#ifndef __EFF_WCREATOR__		//client compile 시

extern CCharacterControl g_CharacterData;

#endif

void CEffScript::Clone(CEffScript *Clone)
{
	int i;

	Clone->m_FileLoad = m_FileLoad;
	Clone->m_bVisibility = m_bVisibility;
	Clone->m_SubScriptNum = m_SubScriptNum;
	strcpy(Clone->m_FileName,m_FileName);
	if (!m_SubScriptNum) 
	{
		Clone->m_EffNum = m_EffNum;
		Clone->m_EffList = new CEffInfo[m_EffNum];
		for (i = 0;i < m_EffNum; i++ ) 
		{
			strcpy(Clone->m_EffList[i].m_EffName,m_EffList[i].m_EffName);
			Clone->m_EffList[i].m_Scale = m_EffList[i].m_Scale;
			Clone->m_EffList[i].m_MinFrame = m_EffList[i].m_MinFrame;
			Clone->m_EffList[i].m_FadeInNum = m_EffList[i].m_FadeInNum;
			int iFadeInNum = m_EffList[i].m_FadeInNum;

			if (iFadeInNum) 
			{
				Clone->m_EffList[i].m_FadeInStart = new int[iFadeInNum];
				Clone->m_EffList[i].m_FadeIn = new D3DXVECTOR3[iFadeInNum];
				Clone->m_EffList[i].m_FadeColor = new color[iFadeInNum];
				Clone->m_EffList[i].m_FadeInSwitch = new bool[iFadeInNum];
				memcpy(Clone->m_EffList[i].m_FadeInStart,m_EffList[i].m_FadeInStart,sizeof(int) * iFadeInNum);
				memcpy(Clone->m_EffList[i].m_FadeIn,m_EffList[i].m_FadeIn,sizeof(D3DXVECTOR3) * iFadeInNum);
				memcpy(Clone->m_EffList[i].m_FadeColor,m_EffList[i].m_FadeColor,sizeof(color) * iFadeInNum);
				memcpy(Clone->m_EffList[i].m_FadeInSwitch,m_EffList[i].m_FadeInSwitch,sizeof(bool) * iFadeInNum);
			}

			// clight
			Clone->m_EffList[i].m_CLightNum = m_EffList[i].m_CLightNum;
			int iCLightNum  = m_EffList[i].m_CLightNum;
			if (iCLightNum) {
				Clone->m_EffList[i].m_CLightStart =  new int[iCLightNum];
				Clone->m_EffList[i].m_CLightColor =  new color[iCLightNum];
				Clone->m_EffList[i].m_CLightDistance =  new float[iCLightNum];
				Clone->m_EffList[i].m_CLightTime =  new DWORD[iCLightNum];
				Clone->m_EffList[i].m_CLightSwitch =  new bool[iCLightNum];
				Clone->m_EffList[i].m_CLightTarget =  new int[iCLightNum];

				memcpy(Clone->m_EffList[i].m_CLightStart,m_EffList[i].m_CLightStart,sizeof(int) * iCLightNum);
				memcpy(Clone->m_EffList[i].m_CLightColor,m_EffList[i].m_CLightColor,sizeof(color) * iCLightNum);
				memcpy(Clone->m_EffList[i].m_CLightDistance,m_EffList[i].m_CLightDistance,sizeof(float) * iCLightNum);
				memcpy(Clone->m_EffList[i].m_CLightTime,m_EffList[i].m_CLightTime,sizeof(DWORD) * iCLightNum);
				memcpy(Clone->m_EffList[i].m_CLightSwitch,m_EffList[i].m_CLightSwitch,sizeof(bool) * iCLightNum);
				memcpy(Clone->m_EffList[i].m_CLightTarget,m_EffList[i].m_CLightTarget,sizeof(int) * iCLightNum);
			}

			// speed value
			Clone->m_EffList[i].m_SpeedValue[0] = m_EffList[i].m_SpeedValue[0];
			Clone->m_EffList[i].m_SpeedValue[1] = m_EffList[i].m_SpeedValue[1];
			Clone->m_EffList[i].m_SpeedValue[2] = m_EffList[i].m_SpeedValue[2];

			// Accel
			Clone->m_EffList[i].m_AccelValue[0] = m_EffList[i].m_AccelValue[0];
			Clone->m_EffList[i].m_AccelValue[1] = m_EffList[i].m_AccelValue[1];
			Clone->m_EffList[i].m_AccelValue[2] = m_EffList[i].m_AccelValue[2];

			// startpos
			Clone->m_EffList[i].m_StartPos[0] = m_EffList[i].m_StartPos[0];
			Clone->m_EffList[i].m_StartPos[1] = m_EffList[i].m_StartPos[1];
			Clone->m_EffList[i].m_StartPos[2] = m_EffList[i].m_StartPos[2];
			Clone->m_EffList[i].m_bStartRot = m_EffList[i].m_bStartRot;

			// loop
			Clone->m_EffList[i].m_LoopStart = m_EffList[i].m_LoopStart;
			Clone->m_EffList[i].m_LoopEnd = m_EffList[i].m_LoopEnd;
			Clone->m_EffList[i].m_LoopSwitch = m_EffList[i].m_LoopSwitch;
		
			// camera shake value
			Clone->m_EffList[i].m_ShakeNum = m_EffList[i].m_ShakeNum;
			int iShakeNum = m_EffList[i].m_ShakeNum;
			if (iShakeNum > 0) 
			{
				Clone->m_EffList[i].m_Shake = new CEffShake[iShakeNum];
				memcpy(Clone->m_EffList[i].m_Shake,m_EffList[i].m_Shake,sizeof(CEffShake) * iShakeNum);
			}
			else
			{
				Clone->m_EffList[i].m_Shake = NULL;
			}

			// sound
			Clone->m_EffList[i].m_SoundNum = m_EffList[i].m_SoundNum;
			
			if (m_EffList[i].m_SoundNum> 0) 
			{
				Clone->m_EffList[i].m_Sound = new CEffSound[m_EffList[i].m_SoundNum];
				memcpy(Clone->m_EffList[i].m_Sound,m_EffList[i].m_Sound,sizeof(CEffSound) * m_EffList[i].m_SoundNum);
			}
			else 
			{
				Clone->m_EffList[i].m_Sound = NULL;
			}

			// hit
			Clone->m_EffList[i].m_HitFrameNum = m_EffList[i].m_HitFrameNum;
			
			int iHitNum = m_EffList[i].m_HitFrameNum;

			if (iHitNum> 0) 
			{
				Clone->m_EffList[i].m_HitFrame = new int[iHitNum];
				memcpy(Clone->m_EffList[i].m_HitFrame,m_EffList[i].m_HitFrame,sizeof(int) * iHitNum);
			}

			Clone->m_EffList[i].m_Ext6ValueNum = m_EffList[i].m_Ext6ValueNum;
			Clone->m_EffList[i].m_SnapNum = m_EffList[i].m_SnapNum;

			int iExt6Num = m_EffList[i].m_Ext6ValueNum;

			if (iExt6Num > 0) 
			{ 
				// slide
				Clone->m_EffList[i].m_Ext6Value= new CEffExt6[iExt6Num];
				memcpy(Clone->m_EffList[i].m_Ext6Value,m_EffList[i].m_Ext6Value,sizeof(CEffExt6) * iExt6Num);
			}

			// snap read
			int iSnapNum = m_EffList[i].m_SnapNum;
			if (iSnapNum > 0) 
			{ 
				Clone->m_EffList[i].m_Snap= new CEffSnap[iSnapNum];			
				memcpy(Clone->m_EffList[i].m_Snap,m_EffList[i].m_Snap,sizeof(CEffSnap) * iSnapNum);
			}
		}	//for (Eff)
	
		Clone->m_bExt5ex = m_bExt5ex;
		Clone->m_LningNum = m_LningNum;		//번개
		Clone->m_WheelNum = m_WheelNum;		//궤적
		Clone->m_WeaponEffNum = m_WeaponEffNum;	//weapon effect
		Clone->m_bBezierMid = m_bBezierMid;		// Bezier Middle
		Clone->m_iLight = m_iLight;		// Vertex Light
		Clone->m_bPlayTime	= m_bPlayTime;		// Play Time
		Clone->m_bLightn2 = m_bLightn2;			// Lightning2
		Clone->m_bEffCharacter = m_bEffCharacter;// Character Effect
		Clone->m_bEffWeaponLine = m_bEffWeaponLine;// Weapon Line

		if (m_bExt5ex) { //extension5 extension
			if (Clone->m_Ext5Value == NULL) { 
				Clone->m_Ext5Value = new CEffExt5;
			}	
			Clone->m_bExt5 = m_bExt5;
			strcpy(Clone->m_Ext5Value->m_TextureName,m_Ext5Value->m_TextureName);
			Clone->m_Ext5Value->m_Rand = m_Ext5Value->m_Rand;
			Clone->m_Ext5Value->m_LineSize = m_Ext5Value->m_LineSize;
				
		}
		if (m_LningNum >0) { // 번개효과
			Clone->m_bLightning = m_bLightning; //번개 bit = true
			Clone->m_Lning = new CEffLightn[m_LningNum];
			memcpy(Clone->m_Lning,m_Lning,sizeof(CEffLightn) * m_LningNum);
			
			for (int ln = 0;ln < m_LningNum;ln++) {
				Clone->m_Lning[ln].m_Lightn = NULL;
				Clone->m_Lning[ln].m_Lightn = new CLightning[m_Lning[ln].m_LnNum]; //CLightning 객체 생성
			}
		}
		
		if (m_WheelNum >0) { // 궤적 라인
			Clone->m_bWheel = m_bWheel;
			Clone->m_Wheel = new CEffWheel[m_WheelNum];
			
			for (int ln=0;ln< m_WheelNum;ln++) {
				Clone->m_Wheel[ln].m_BlendMode = m_Wheel[ln].m_BlendMode;
				Clone->m_Wheel[ln].m_PlaneSize = m_Wheel[ln].m_PlaneSize;
				Clone->m_Wheel[ln].m_PlaneValue = m_Wheel[ln].m_PlaneValue;
				Clone->m_Wheel[ln].m_PointNum = m_Wheel[ln].m_PointNum;
				strcpy(Clone->m_Wheel[ln].m_TexName,m_Wheel[ln].m_TexName);
				Clone->m_Wheel[ln].m_UnitUv = m_Wheel[ln].m_UnitUv;

			}
		}

		if (m_WeaponEffNum > 0) {	//weapon effect
			Clone->m_bWeaponEff = m_bWeaponEff;
			Clone->m_WeaponEff = new CEffWeapon[m_WeaponEffNum];
			memcpy(Clone->m_WeaponEff,m_WeaponEff,sizeof(CEffWeapon) * m_WeaponEffNum);
		}
		if (m_bBezierMid == true ) {		// Bezier Middle
			Clone->m_pBezierMid = new CEffBezier;
			memcpy(Clone->m_pBezierMid,m_pBezierMid,sizeof(CEffBezier));

			Clone->m_pBezierMid->m_pBezierLine = NULL;
			Clone->m_pBezierMid->m_nBezierPoint = 30;				// 총 제어점 갯수
		}

		if (m_iLight > 0) {				// Vertex Light
			Clone->m_pLight = new CEffLight[m_iLight];
			memcpy(Clone->m_pLight,m_pLight,sizeof(CEffLight) * m_iLight);
		}
		if (m_bPlayTime == true) {				// PlayTime
			
			Clone->m_iStartPlayTime = m_iStartPlayTime;
			Clone->m_iEndPlayTime = m_iEndPlayTime;
			Clone->m_bVisTime = m_bVisTime;
		}
		if (m_bLightn2 == true) {				// Lightning2
			Clone->m_pLightn2 = new CEffLightn2;
			memcpy(Clone->m_pLightn2,m_pLightn2,sizeof(CEffLightn2));

			Clone->m_pLightn2->m_pLightn = new CEffLightn[(Clone->m_pLightn2->m_iMaxLine)];
			Clone->m_pLightn2->m_pTextureName = new CEffTextureDef[(Clone->m_pLightn2->m_iMaxLine)];
			memcpy(	Clone->m_pLightn2->m_pTextureName,m_pLightn2->m_pTextureName,sizeof(CEffTextureDef) * (m_pLightn2->m_iMaxLine));

			GetBinLightn2();					// Sub Lightning 객체 생성및 처리
		}
		if (m_bEffCharacter == true) {
			Clone->m_pEffCharacter = new CEffChar;
			memcpy(Clone->m_pEffCharacter,m_pEffCharacter,sizeof(CEffChar));
		}
		if (m_bEffWeaponLine == true) {							// 검기 관련 제어 Load (WEAPONLINE:)
			Clone->m_pEffWeaponLine = new CEffWeaponLine;

			memcpy(Clone->m_pEffWeaponLine,m_pEffWeaponLine,sizeof(CEffWeaponLine));
			Clone->m_pEffWeaponLine->m_pLineTexture = new CTexture;	// 검기 관련 텍스쳐
			CTexture::SetPath(EFFECTTEXTUREPATH);
			Clone->m_pEffWeaponLine->m_pLineTexture->Load(Clone->m_pEffWeaponLine->m_strTextureName);
			
		}
		
		//interface pos
		Clone->m_bInterfacePos = m_bInterfacePos;
		Clone->m_InterfacePos[0] = m_InterfacePos[0];
		Clone->m_InterfacePos[1] = m_InterfacePos[1];
		/////////////////////CEffPlay

		// start id info
		Clone->m_EffPlayData.m_StartIdNum = m_EffPlayData.m_StartIdNum;
		Clone->m_EffPlayData.m_StartCount = m_EffPlayData.m_StartCount;
		Clone->m_EffPlayData.m_StartFrameNextEff = m_EffPlayData.m_StartFrameNextEff;
		
		if (m_EffPlayData.m_StartIdNum > 0) {
			Clone->m_EffPlayData.m_StartId = new int[m_EffPlayData.m_StartIdNum];
			memcpy(Clone->m_EffPlayData.m_StartId,m_EffPlayData.m_StartId,sizeof(int) * m_EffPlayData.m_StartIdNum);
		}

		//mid info
		Clone->m_EffPlayData.m_MidEffIdNum = m_EffPlayData.m_MidEffIdNum;
		Clone->m_EffPlayData.m_MidCount = m_EffPlayData.m_MidCount;
		
		if (m_EffPlayData.m_MidEffIdNum > 0) {
			Clone->m_EffPlayData.m_MidEffId = new int[m_EffPlayData.m_MidEffIdNum];
			memcpy(Clone->m_EffPlayData.m_MidEffId,m_EffPlayData.m_MidEffId,sizeof(int) * m_EffPlayData.m_MidEffIdNum);
		}
		//end info 
		Clone->m_EffPlayData.m_EndPos[0] = m_EffPlayData.m_EndPos[0];
		Clone->m_EffPlayData.m_EndPos[1] = m_EffPlayData.m_EndPos[1];
		Clone->m_EffPlayData.m_EndPos[2] = m_EffPlayData.m_EndPos[2];

	
		Clone->m_EffPlayData.m_EndEffIdNum = m_EffPlayData.m_EndEffIdNum;
		Clone->m_EffPlayData.m_EndCount = m_EffPlayData.m_EndCount;
		
		if (m_EffPlayData.m_EndEffIdNum > 0) {
			Clone->m_EffPlayData.m_EndEffId = new int[m_EffPlayData.m_EndEffIdNum];
			memcpy(Clone->m_EffPlayData.m_EndEffId,m_EffPlayData.m_EndEffId,sizeof(int) * m_EffPlayData.m_EndEffIdNum);
		}

		//speed accel min max speed	
		Clone->m_EffPlayData.m_Speed = m_EffPlayData.m_Speed;
		Clone->m_EffPlayData.m_MaxSpeed = m_EffPlayData.m_MaxSpeed;
		Clone->m_EffPlayData.m_MinSpeed = m_EffPlayData.m_MinSpeed;
		Clone->m_EffPlayData.m_Accel = m_EffPlayData.m_Accel;

		// mid direct	
		Clone->m_EffPlayData.m_Direct = m_EffPlayData.m_Direct;
		Clone->m_EffPlayData.m_MidEffNum = m_EffPlayData.m_MidEffNum;
		Clone->m_EffPlayData.m_StartMidNextEff = m_EffPlayData.m_StartMidNextEff;
		/////////////////////////////
		//other value set
		Clone->m_StartPosSet = m_StartPosSet;
		Clone->m_StartPos = m_StartPos;
		Clone->m_MidPos = m_MidPos;
		Clone->m_OldMidPos = m_OldMidPos;
		Clone->m_MidUnit = m_MidUnit;
		Clone->m_MidRot = m_MidRot;
		Clone->m_NotRot = m_NotRot;
		
		// endpos set
		Clone->m_EndPosNum = m_EndPosNum;
		if (m_EndPosNum >0) {
			Clone->m_EndPos = new D3DXVECTOR3[m_EndPosNum];
			memcpy(Clone->m_EndPos,m_EndPos,sizeof(D3DXVECTOR3) * m_EndPosNum );
			
		}
		else
			Clone->m_EndPos = NULL;

		//delay frame tickframe
		Clone->m_DelayFrame = m_DelayFrame;
		Clone->m_TickFrame = m_TickFrame;

		//fade out write
		Clone->m_FadeOut[0] = m_FadeOut[0];
		Clone->m_FadeOut[1] = m_FadeOut[1];
		Clone->m_FadeOut[2] = m_FadeOut[2];

			


	}
	else {
		Clone->m_NextScript = new int[m_SubScriptNum];
		Clone->m_SubScript = new CEffScript[m_SubScriptNum];
		
		memcpy(Clone->m_NextScript,m_NextScript,sizeof(int) * m_SubScriptNum);
		
		for (i=0;i<m_SubScriptNum;i++) {
			strcpy(Clone->m_SubScript[i].m_FileName,m_SubScript[i].m_FileName);
			GetSubClone(i,Clone,Clone->m_SubScript[i].m_FileName);
			
		}
	}

	
}

using namespace Item::ItemType;
void CEffScript::GetSubClone(int index,CEffScript *Clone,char *filename) {
	CEffScript *pTmp = NULL;
	char *dot = NULL;
	pTmp = CSceneManager::ms_pEffectCacheMgr->GetData(filename);

	//pTmp = (CEffScript *)(CSceneManager::m_pEsfCasher->GetCashData(filename));
	if (pTmp) {
		pTmp->Clone(&Clone->m_SubScript[index]);
	}
	else {
		if ((Clone->m_Weapone != ESEMPTY) || (Clone->m_Sex != ESEMPTY)) { 
			dot = strrchr(filename,'.');
			// sub effect script load

			switch(Clone->m_Weapone) {
			case ONEHANDED_SWORD:// 한손 무기
			case ONEHANDED_AXE:
			case ONEHANDED_BLUNT:
				(*dot) = '_';dot++;
				(*dot) = '6';dot++;
				break;
			case TWOHANDED_AXE:
			case TWOHANDED_BLUNT:
				(*dot) = '_';dot++;
				(*dot) = '9';dot++;
				break;
			case TWOHANDED_SWORD:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '1';dot++;
				break;
			case BOW:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '2';dot++;
				break;
			case CROSSBOW:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '3';dot++;
				break;
			case STAFF:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '4';dot++;
				break;
			case DAGGER:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '5';dot++;
				break;
			case COM_BLUNT: // 아칸 한손무기
			case COM_SWORD:
				(*dot) = '_';dot++;
				(*dot) = '3';dot++;
				(*dot) = '3';dot++;
				break;

			case OPP_HAMMER:
			case OPP_AXE:
			case OPP_SLUSHER:
				(*dot) = '_';dot++;
				(*dot) = '6';dot++;
				break;
			default:
				break;
			}
			switch(Clone->m_Sex) {
			case 1:
				(*dot) = '_';dot++;
				(*dot) = 'm';dot++;
				break;
			case 2:
				(*dot) = '_';dot++;
				(*dot) = 'f';dot++;
				break;
			default:
				(*dot) = '_';dot++;
				(*dot) = 'm';dot++;
				break;
			}
			(*dot) = '.';dot++;
			(*dot) = 'e';dot++;
			(*dot) = 's';dot++;
			(*dot) = 'f';dot++;

			pTmp = CSceneManager::ms_pEffectCacheMgr->GetData(filename);
			if (pTmp) {
				pTmp->Clone(&Clone->m_SubScript[index]);
			}
			else {
				MessageBox(NULL,"에러","eee",MB_OK);

				//delete Clone->m_SubScript[index];
				//Clone->m_SubScript[index] = NULL;
			}
		}
		else {
			MessageBox(NULL,"에러","eee",MB_OK);

			/*
			delete Clone->m_SubScript[index];
			Clone->m_SubScript[index] = NULL;*/

		}	
	}

}
CEffScript :: CEffScript() {
	memset(m_FileName,0,sizeof(char) * ESBUF_SIZE);

	m_ScriptValue = ESNORMAL;

	m_StartChr = NULL;
	m_EndChr = NULL;

	m_MidShootFrame = 0;
	m_Weapone = ESEMPTY;
	m_Sex = ESEMPTY;

	del_count = 0;
	m_NotRot = false;
	//m_Wheel = NULL;
	m_MidUnit.x= m_MidUnit.y = m_MidUnit.z = 0.0f;
	m_MidRot.x = m_MidRot.y = m_MidRot.z = 0.0f; 
	m_ScriptFile = NULL;
	m_EffList = NULL;
	m_EffNum = 0;
	m_CurrentNum = 0;
	m_StartPosSet = false;
	m_StartPos.x = m_StartPos.y = m_StartPos.z = 0.0f;
	m_MidPos.x = m_MidPos.y = m_MidPos.z = 0.0f;
	m_bMidFwd = false;
	m_OldMidPos.x = m_OldMidPos.y = m_OldMidPos.z = 0.0f;
	// fwd vector 
	m_ChrFwd.x = m_ChrFwd.y = m_ChrFwd.z = 0.0f;	
	
	// 운동중인 mid effect의 속도
	m_MidSpeed = 0.0f;
	m_EndPos = NULL;
	m_EndPosNum = 0;
	//start
	m_ProcCount = ESEMPTY;
	m_DelayFrame = 0;
	m_FadeOut[0] = m_FadeOut[1] = m_FadeOut[2] = ESEMPTY; 
	m_FadeOutStart = false;
	m_TickFrame = DEFAULTESTICK;
	m_lstEff.clear();
	m_Mine = true;
	m_bSkill = false;

	m_CheckCrash = false;
	m_MulPos = 1.0f;

	m_InterfacePos[0] = 0.0f;
	m_InterfacePos[1] = 0.0f;
	m_bInterfacePos = false;
	// m_InterfacePos 가 화면 좌표인지 설정 (true : 화면 좌표)
	m_bInterfaceSet = false;

	// sub 스크립트 이용시
	m_SubScript = NULL;
	m_NextScript = NULL;
	m_SubFirstLoad = false;
	m_SubScriptNum = 0;
	m_FileLoad = false;
	m_CurrentSubScript = 0;
	srand(timeGetTime());
	DelayTick = 0.0f;
	DelayStart = false;
	DelayFrame = 0;
	
	m_Projection.x = m_Projection.y = m_Projection.z = 0.0f;
	
	m_bWorldEffect = false;
	m_WorldAxis[0] = m_WorldAxis[1] = m_WorldAxis[2] = 0.0f;
	m_bExt1 = false;
	m_bExt2 = false;
	m_bExt3 = false;
	m_bExt3Crash = false;
	m_bExt1Crash = false;

	m_bExt4 = false; 
	m_bExt4InfoSet = false;
	m_Ext4Ptr = NULL; 
	m_Ext4Pivot = ESEMPTY;
	
	m_Ext4Quat.x = 0.0f;
	m_Ext4Quat.y = 0.0f;
	m_Ext4Quat.z = 0.0f;
	m_Ext4Quat.w = 1.0f;
	
	m_Ext4Object.x = 0.0f;
	m_Ext4Object.y = 0.0f;
	m_Ext4Object.z = 0.0f;
	m_Ext4Object.w = 1.0f;

	m_bExt4First = false;

	m_bExt5 = false;
	m_Ext5Value = NULL;
	m_bExt5ex = false;

	oldfwd_vec.x = oldfwd_vec.y = oldfwd_vec.z = 0.0f;	
	fwd_vec.x = fwd_vec.y = fwd_vec.z = 0.0f;
	pos_vec.x = pos_vec.y = pos_vec.z = 0.0f;
	m_EndChrChest = -1.0f;
	m_bCast = false;
	m_bPosSet = true;
    m_pPlayer   = NULL;
    m_ulPlayerID= 0xFFFFFFFF;

	m_bExt7 = false;
	m_bExt8 = false;
	m_bExt9 = false;

	m_bLightning = false;
	m_Lning = NULL;
	m_LningNum = 0;
	m_Chant = false;

	m_Wheel = NULL;
	m_WheelNum = 0;
	m_bWheel = false;
	m_bWheelStart = false;
	
	m_WeaponEff = NULL;
	m_WeaponEffNum = 0;
	m_bWeaponEff = false;
	m_bVisibility = true;

	m_pBezierMid = NULL;
	m_bBezierMid = false;
	m_bBezierMidSet = false;
	
	m_iLight = 0;
	m_pLight = NULL;
	
	m_iStartPlayTime = -1;	// play start time
	m_iEndPlayTime = -1;	// play end time
	m_bPlayTime = false;	
	m_bVisTime = true;
	m_bSectorCull = true;

	m_pLightn2 = NULL;		// Lightning 2
	m_bLightn2 = false;		// lightning2 bool 

	m_pLightn3 = NULL; 
	m_bLightn3 = false;		

	m_bEffCharacter = false;
	m_pEffCharacter = NULL;
	
	m_pEffWeaponLine = NULL;
	m_bEffWeaponLine = false;
	
	m_bFrustumCull = true;

}

CEffScript ::~CEffScript() 
{
	int j;

//	_ASSERTE( _CrtCheckMemory());
	if (m_ScriptFile)
		fclose(m_ScriptFile);

	if (m_SubScript) 
	{
		delete[] m_SubScript;
		m_SubScript = NULL;
	}

	if (m_NextScript) 
	{
		delete[] m_NextScript;
		m_NextScript = NULL;
	}

	for (int i = 0; i < static_cast<int>(m_lstEff.size()); ++i) 
	{	
#ifndef __EFF_WCREATOR__ //client 용	
		if (!m_bExt9) 
		{ 
			// extension 9일때는 snap이 메모리에서 삭제 되지 않는다
			for (j = 0; j < m_EffList[i].m_SnapNum; ++j) 
			{
				// snap effect 있을시 ccreature에서 삭제
                if ( m_pPlayer && m_ulPlayerID != 0xFFFFFFFF ) 
                {
                    if ( RYLCreatureManager::Instance()->GetCreature( m_ulPlayerID ) == m_pPlayer )
                    {
                        // CCreature 몸에 붙는 effect delete 
                        m_pPlayer->GetEffect()->DeleteBodyEffectName( m_EffList[ i ].m_Snap[ j ].m_EffName );
                    }
				}
			}
		}	
#else //world creator 용
		if (!m_bExt9) 
		{
			for (j=0;j<m_EffList[i].m_SnapNum;j++) 
			{
				// snap effect 있을시 ccreature에서 삭제
				if (m_Player != NULL) 
				{
					// CCreature 몸에 붙는 effect delete 
					//m_Player->DeleteBodyEffect(m_EffList[i].m_Snap[j].m_PivotName);
					m_Player->DeleteBodyEffectName(m_EffList[i].m_Snap[j].m_EffName);
					//m_Player->DeleteBodyEffect(m_EffList[i].m_Snap[j].m_EffName);
				}
			}
		}
#endif
		for (j=0;j<m_EffList[i].m_SoundNum;j++) 
		{
			//fade out sound 를 위한 세팅후 루핑 effect 삭제 
			if (m_EffList[i].m_Sound[j].m_Target == ES)
				StopEffSound(i,j,m_StartPos.x,m_StartPos.y,m_StartPos.z,m_EffList[i].m_Sound[j].m_Min,m_EffList[i].m_Sound[j].m_Max);
			else if (m_EffList[i].m_Sound[j].m_Target == EM)
				StopEffSound(i,j,m_MidPos.x,m_MidPos.y,m_MidPos.z,m_EffList[i].m_Sound[j].m_Min,m_EffList[i].m_Sound[j].m_Max);
			else if (m_EffList[i].m_Sound[j].m_Target == EE)
				StopEffSound(i,j,m_EndPos[0].x,m_EndPos[0].y,m_EndPos[0].z,m_EffList[i].m_Sound[j].m_Min,m_EffList[i].m_Sound[j].m_Max);
			
		}

		if (m_lstEff[i] != NULL) 
		{
			delete m_lstEff[i];
			m_lstEff[i] = NULL;
		}
	}

	if (m_EndPos) 
	{
		delete[] m_EndPos;
		m_EndPos = NULL;
	}

	if (m_EffList) 
	{
		delete[] m_EffList;
		m_EffList = NULL;
	}

	if (m_bExt5) 
	{
		if (m_Ext5Value != NULL) 
		{
			delete m_Ext5Value;
			m_Ext5Value = NULL;	
		}
	}

	if (m_bLightning) 
	{
		delete[] m_Lning;
		m_Lning = NULL;
	}

	if (m_bWheel) 
	{ 
		// 궤적 
		delete[] m_Wheel;
		m_Wheel = NULL;
	}

	if (m_bBezierMid) 
	{	
		// Middle로 bezier 스트립을 이용했을때..
		delete m_pBezierMid;
		m_pBezierMid = NULL;
	}
	m_lstEff.clear();
	
	if (m_iLight > 0) 
	{
		delete[] m_pLight;
		m_pLight = NULL;
	}

	if (m_pLightn2 != NULL) 
	{
		delete m_pLightn2;			// Lightning 2
		m_pLightn2 = NULL;

	}

	if (m_pLightn3 != NULL) 
	{
		delete m_pLightn3;	
		m_pLightn3 = NULL; 
	}

	if (m_pEffCharacter != NULL) 
	{	
		// Character Effect
		delete m_pEffCharacter;
		m_pEffCharacter = NULL;

	}

	if (m_WeaponEff != NULL) 
	{		
		// Weapon Effect 
		DeleteWeaponEff();
		delete[] m_WeaponEff;
		m_WeaponEff = NULL;
	}

	if (m_pEffWeaponLine != NULL)	// Weapon Line
	{	
		DeleteWeaponLine();			// 검기를 이 스크립트가 제어하기 전 상태로 되돌림

		delete m_pEffWeaponLine;
		m_pEffWeaponLine = NULL;
	}
//	_ASSERTE( _CrtCheckMemory());
}

// script data 를 binary file 로 컨버팅 
void CEffScript::WriteConvertFile(char *name) 
{
	FILE *fp = NULL;
	int effnum = 0;
	int i;
	effnum = m_EffNum;

	char buf[ESBUF_SIZE] = {0};

	fp = fopen(name,"wb");
	if (fp) {
		fwrite((int *)&m_SubScriptNum,sizeof(int),1,fp);
		
		if (!m_SubScriptNum) {
			// eff number write
			fwrite((int *)&effnum,sizeof(int),1,fp);
			for (i=0;i<effnum;i++) { // CEffInfo write
				// eff name
				fwrite((char *)m_EffList[i].m_EffName,sizeof(char) * ESBUF_SIZE,1,fp);
				// scale
				fwrite((float *)&(m_EffList[i].m_Scale),sizeof(float),1,fp);
				//minframe
				fwrite((int *)&(m_EffList[i].m_MinFrame),sizeof(int),1,fp);
				//fade in info
				fwrite((int *)&(m_EffList[i].m_FadeInNum),sizeof(int),1,fp);
				if (m_EffList[i].m_FadeInNum >0) {
					fwrite((int *)(m_EffList[i].m_FadeInStart),sizeof(int),m_EffList[i].m_FadeInNum,fp);
					fwrite((D3DXVECTOR3 *)(m_EffList[i].m_FadeIn),sizeof(D3DXVECTOR3),m_EffList[i].m_FadeInNum,fp);
					fwrite((color *)(m_EffList[i].m_FadeColor),sizeof(color),m_EffList[i].m_FadeInNum,fp);
					fwrite((bool *)(m_EffList[i].m_FadeInSwitch),sizeof(bool),m_EffList[i].m_FadeInNum,fp);
				}
				//clight
				fwrite((int *)&(m_EffList[i].m_CLightNum),sizeof(int),1,fp);
				if (m_EffList[i].m_CLightNum >0) {
				
					fwrite((int *)(m_EffList[i].m_CLightStart),sizeof(int),m_EffList[i].m_CLightNum,fp);
					fwrite((color *)(m_EffList[i].m_CLightColor),sizeof(color),m_EffList[i].m_CLightNum,fp);
					fwrite((float *)(m_EffList[i].m_CLightDistance),sizeof(float),m_EffList[i].m_CLightNum,fp);
					fwrite((DWORD *)(m_EffList[i].m_CLightTime),sizeof(DWORD),m_EffList[i].m_CLightNum,fp);
					fwrite((bool *)(m_EffList[i].m_CLightSwitch),sizeof(bool),m_EffList[i].m_CLightNum,fp);
					fwrite((int *)(m_EffList[i].m_CLightTarget),sizeof(int),m_EffList[i].m_CLightNum,fp);
				}
				// speed value
				fwrite((float *)(m_EffList[i].m_SpeedValue),sizeof(float),3,fp);
				//accel value
				fwrite((float *)(m_EffList[i].m_AccelValue),sizeof(float),3,fp);
				// startpos
				fwrite((float *)(m_EffList[i].m_StartPos),sizeof(float),3,fp);
				fwrite((bool *)&(m_EffList[i].m_bStartRot),sizeof(bool),1,fp);

				//loop
				fwrite((int *)&(m_EffList[i].m_LoopStart),sizeof(int),1,fp);
				fwrite((int *)&(m_EffList[i].m_LoopEnd),sizeof(int),1,fp);
				fwrite((int *)&(m_EffList[i].m_LoopSwitch),sizeof(int),1,fp);
				//camera shake value
				fwrite((int *)&(m_EffList[i].m_ShakeNum),sizeof(int),1,fp);
				if (m_EffList[i].m_ShakeNum > 0)
					fwrite((CEffShake *)m_EffList[i].m_Shake,sizeof(CEffShake),m_EffList[i].m_ShakeNum,fp);
				//sound
				fwrite((int *)&(m_EffList[i].m_SoundNum),sizeof(int),1,fp);
				if (m_EffList[i].m_SoundNum > 0)
					fwrite((CEffSound *)m_EffList[i].m_Sound,sizeof(CEffSound),m_EffList[i].m_SoundNum,fp);
			
			}
			//interface pos
			fwrite((bool *)&(m_bInterfacePos),sizeof(bool),1,fp);
			fwrite((float *)m_InterfacePos,sizeof(float),2,fp);
			
			/////////////////////CEffPlay
			// start id info
			fwrite((int *)&(m_EffPlayData.m_StartIdNum),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_StartCount),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_StartFrameNextEff),sizeof(int),1,fp);
			if (m_EffPlayData.m_StartIdNum > 0) {
				fwrite((int *)m_EffPlayData.m_StartId,sizeof(int),m_EffPlayData.m_StartIdNum,fp);
			}
			//mid info
			fwrite((int *)&(m_EffPlayData.m_MidEffIdNum),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_MidCount),sizeof(int),1,fp);
			if (m_EffPlayData.m_MidEffIdNum > 0) {
				fwrite((int *)m_EffPlayData.m_MidEffId,sizeof(int),m_EffPlayData.m_MidEffIdNum,fp);
			}
			//end info 
			fwrite((float *)m_EffPlayData.m_EndPos,sizeof(float),3,fp);
			
			fwrite((int *)&(m_EffPlayData.m_EndEffIdNum),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_EndCount),sizeof(int),1,fp);
			if (m_EffPlayData.m_EndEffIdNum > 0) {
				fwrite((int *)m_EffPlayData.m_EndEffId,sizeof(int),m_EffPlayData.m_EndEffIdNum,fp);
			}

			//speed accel min max speed	
			fwrite((float *)&(m_EffPlayData.m_Speed),sizeof(float),1,fp);
			fwrite((float *)&(m_EffPlayData.m_MaxSpeed),sizeof(float),1,fp);
			fwrite((float *)&(m_EffPlayData.m_MinSpeed),sizeof(float),1,fp);
			fwrite((float *)&(m_EffPlayData.m_Accel),sizeof(float),1,fp);
			// mid direct	
			fwrite((int *)&(m_EffPlayData.m_Direct),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_MidEffNum),sizeof(int),1,fp);
			fwrite((int *)&(m_EffPlayData.m_StartMidNextEff),sizeof(int),1,fp);
			/////////////////////////////

			//other value set
			fwrite((bool *)&(m_StartPosSet),sizeof(bool),1,fp);
			fwrite((D3DXVECTOR3 *)&(m_StartPos),sizeof(D3DXVECTOR3),1,fp);
			fwrite((D3DXVECTOR3 *)&(m_MidPos),sizeof(D3DXVECTOR3),1,fp);
			fwrite((D3DXVECTOR3 *)&(m_OldMidPos),sizeof(D3DXVECTOR3),1,fp);
			
			fwrite((D3DXVECTOR3 *)&(m_MidUnit),sizeof(D3DXVECTOR3),1,fp);
			fwrite((D3DXVECTOR3 *)&(m_MidRot),sizeof(D3DXVECTOR3),1,fp);
			fwrite((bool *)&(m_NotRot),sizeof(bool),1,fp);
			// endpos set
			fwrite((int *)&m_EndPosNum,sizeof(int),1,fp);
			if (m_EndPosNum >0) {
				fwrite((D3DXVECTOR3 *)m_EndPos,sizeof(D3DXVECTOR3),m_EndPosNum,fp);
			}
			//delay frame tickframe
			fwrite((int *)&m_DelayFrame,sizeof(int),1,fp);
			fwrite((int *)&m_TickFrame,sizeof(int),1,fp);
			//fade out write
			fwrite((float *)m_FadeOut,sizeof(float),3,fp);
			for (i=0;i<effnum;i++) { // CEffInfo write
					//hit
				fwrite((int *)&(m_EffList[i].m_HitFrameNum),sizeof(int),1,fp);
				if (m_EffList[i].m_HitFrameNum >0)
				fwrite((int *)m_EffList[i].m_HitFrame,sizeof(int),m_EffList[i].m_HitFrameNum,fp);
		
			}
			// CEffExt6 write (hit 가 script안에 있다는 가정하에 쓰임)
			// 기존 compile된 script와 호완하기 위해서..
			for (i=0;i<effnum;i++) { // CEffInfo write
				int array[50];

				memset((int *)array,0,sizeof(int) * 50);
				array[0] = m_EffList[i].m_Ext6ValueNum; //slide
				array[1] = m_EffList[i].m_SnapNum; //snap
				array[2] = m_bExt5ex;
				array[3] = m_LningNum;
				array[4] = m_WheelNum;
				array[5] = m_WeaponEffNum;
				array[6] = ((m_bBezierMid == true) ? 1 : 0);		// Bezier Middle
				array[7] = m_iLight;								// Vertex Lighting Num
				array[8] = ((m_bPlayTime == true) ? 1 : 0);			// Play Time
				array[9] = ((m_bLightn2 == true) ? 1 : 0);			// Lightning 2
				array[10] = ((m_bEffCharacter == true) ? 1 : 0);	// Character Effect
				array[11] = ((m_bEffWeaponLine == true) ? 1 : 0);	// 검기
				array[12] = ((m_bLightn3 == true) ? 1 : 0);			// Lightning 3

				fwrite((int *)array,sizeof(int),50,fp);
				
				if (array[0] >0) //slide
					fwrite((CEffExt6 *)m_EffList[i].m_Ext6Value,sizeof(CEffExt6),array[0],fp);

				if (array[1] >0) //snap
					fwrite((CEffSnap *)m_EffList[i].m_Snap,sizeof(CEffSnap),array[1],fp);
				if (array[2] >0) { //extension6 extension
					fwrite((char *)m_Ext5Value->m_TextureName,sizeof(char),ESBUF_SIZE,fp);
					fwrite((bool *)&m_Ext5Value->m_Rand,sizeof(bool),1,fp);
					fwrite((float *)&m_Ext5Value->m_LineSize,sizeof(float),1,fp);
				}
				if (i == 0) { // 처음 한번만 write
					if (array[3] >0) { // 번개효과
						for (int ln = 0;ln < array[3];ln++) {
							if (m_Lning[ln].m_Lightn != NULL) {
								delete[] m_Lning[ln].m_Lightn;
								m_Lning[ln].m_Lightn = NULL;
							}
						}
						fwrite((CEffLightn *)m_Lning,sizeof(CEffLightn),array[3],fp);
					}
				}
				if (i == 0) { // 처음 한번만 write
					if (array[4] >0) { // 궤적 라인
						for (int ln=0;ln< array[4];ln++) {
							fwrite((int *)&(m_Wheel[ln].m_BlendMode),sizeof(int),1,fp);
							fwrite((float *)&(m_Wheel[ln].m_PlaneSize),sizeof(float),1,fp);
							fwrite((int *)&(m_Wheel[ln].m_PlaneValue),sizeof(int),1,fp);
							fwrite((int *)&(m_Wheel[ln].m_PointNum),sizeof(int),1,fp);
							fwrite((char *)m_Wheel[ln].m_TexName,sizeof(char),ESBUF_SIZE,fp);
							fwrite((float *)&(m_Wheel[ln].m_UnitUv),sizeof(float),1,fp);
						}
					}
				}
				if (i == 0) { //처음 한번만 write
					if (array[5] > 0) {	//weapon effect
						fwrite((CEffWeapon *)m_WeaponEff,sizeof(CEffWeapon),array[5],fp);
					}
				}
				if (i == 0) {
					if (array[6] > 0) {
						
						if (m_pBezierMid->m_pBezierLine) {		// Bezier Middle
							delete m_pBezierMid->m_pBezierLine;
							m_pBezierMid->m_pBezierLine = NULL;	
						}
						fwrite((CEffBezier *)m_pBezierMid,sizeof(CEffBezier),1,fp);
					}
				}
				if (i == 0) {
					if (array[7] > 0) {		// Vertex Light
						fwrite((CEffLight *)m_pLight,sizeof(CEffLight),m_iLight,fp);
					}
				}
				if (i == 0) {
					if (array[8] > 0) {		// Play Time
						int iValue[2] = {m_iStartPlayTime,m_iEndPlayTime};
						fwrite((int *)iValue,sizeof(int),2,fp);
					}
				}
				if (i == 0) {
					if (array[9] > 0) {
						CEffTextureDef *pTmpTextureDef = NULL;
						pTmpTextureDef = new CEffTextureDef[m_pLightn2->m_iMaxLine];
						memcpy(pTmpTextureDef,(m_pLightn2->m_pTextureName),sizeof(CEffTextureDef) * m_pLightn2->m_iMaxLine);

						if (m_pLightn2->m_pLightn != NULL) {
							delete[] (m_pLightn2->m_pLightn);
							m_pLightn2->m_pLightn = NULL;
						}
						if (m_pLightn2->m_pTextureName != NULL) {
							delete[] (m_pLightn2->m_pTextureName);
							m_pLightn2->m_pTextureName = NULL;
						}
						fwrite((CEffLightn2 *)m_pLightn2,sizeof(CEffLightn2),1,fp);	
						fwrite((CEffTextureDef *)pTmpTextureDef,sizeof(CEffTextureDef),m_pLightn2->m_iMaxLine,fp);
						delete[] pTmpTextureDef;

					}
				}
				if (i == 0) {
					if (array[10] > 0) {
						fwrite((CEffChar *)m_pEffCharacter,sizeof(CEffChar),1,fp);
					}
				}
				if (i == 0) {
					if (array[11] > 0) {	// 검기
						if (m_pEffWeaponLine->m_pLineTexture	!= NULL) {
							delete (m_pEffWeaponLine->m_pLineTexture);
							m_pEffWeaponLine->m_pLineTexture = NULL;
						}
						fwrite((CEffWeaponLine *)m_pEffWeaponLine,sizeof(CEffWeaponLine),1,fp);
					}
				}
				if (i == 0) {

					if (array[12] > 0) {	// Lightning 3
						
					}
				}
				/*
				//slide
				fwrite((int *)&(m_EffList[i].m_Ext6ValueNum),sizeof(int),1,fp);
				if (m_EffList[i].m_Ext6ValueNum >0)
					fwrite((CEffExt6 *)m_EffList[i].m_Ext6Value,sizeof(CEffExt6),m_EffList[i].m_Ext6ValueNum,fp);
				//snap
				fwrite((int *)&(m_EffList[i].m_SnapNum),sizeof(int),1,fp);
				if (m_EffList[i].m_SnapNum >0)
					fwrite((CEffSnap *)m_EffList[i].m_Snap,sizeof(CEffSnap),m_EffList[i].m_SnapNum,fp);
				*/
				
			}


		}
		//sub script data		
		
		else{

			fwrite((int *)m_NextScript,sizeof(int),m_SubScriptNum,fp);
			for (i=0;i<m_SubScriptNum;i++) {
				//name save code..	
				fwrite((char *)(m_SubScript[i].m_FileName),sizeof(char) * ESBUF_SIZE,1,fp);		
			}
		}
	}
	
	if (fp) {
		fclose(fp);
	}
	
}
// binary 형식의 script data file load
// file load 실패시 false return 
bool CEffScript::GetScriptRealBinData(char *filename,bool vis) 
{
	char filepath[ESBUF_SIZE] = {0};

	FILE *fp;
	int i;
	int fadeinnum = 0;
	int clightnum = 0;
	int shakenum = 0;
	int soundnum = 0;
	int hitnum = 0;
	
	if (filename == NULL) 
	{
		m_FileLoad = false;
		return false;
	}
	// Visibility setting	
	m_bVisibility = vis;

	strcpy(m_FileName,filename);

	strcpy(filepath,EFFECTSCRIPTPATH);
	strcat(filepath,filename);
	
	fp = fopen(filepath,"rb");
	if (fp) 
	{
		m_FileLoad = true;
		fread((int *)&m_SubScriptNum,sizeof(int),1,fp);
		if (!m_SubScriptNum) {
			fread((int *)&m_EffNum,sizeof(int),1,fp);
			m_EffList = new CEffInfo[m_EffNum];
			for (i=0;i<m_EffNum;i++) {
				// eff name
				fread((char *)m_EffList[i].m_EffName,sizeof(char) * ESBUF_SIZE,1,fp);
				// scale
				fread((float *)&(m_EffList[i].m_Scale),sizeof(float),1,fp);
				//minframe
				fread((int *)&(m_EffList[i].m_MinFrame),sizeof(int),1,fp);
				//fade in info
				fread((int *)&(m_EffList[i].m_FadeInNum),sizeof(int),1,fp);
				fadeinnum  = m_EffList[i].m_FadeInNum;

				if (fadeinnum> 0) {
					m_EffList[i].m_FadeInStart = new int[fadeinnum];
					m_EffList[i].m_FadeIn = new D3DXVECTOR3[fadeinnum];
					m_EffList[i].m_FadeColor = new color[fadeinnum];
					m_EffList[i].m_FadeInSwitch = new bool[fadeinnum];
					fread((int *)m_EffList[i].m_FadeInStart,sizeof(int),fadeinnum,fp);
					fread((D3DXVECTOR3 *)m_EffList[i].m_FadeIn,sizeof(D3DXVECTOR3),fadeinnum,fp);
					fread((color *)m_EffList[i].m_FadeColor,sizeof(color),fadeinnum,fp);
					fread((bool *)m_EffList[i].m_FadeInSwitch,sizeof(bool),fadeinnum,fp);
				}
				
				//clight
				fread((int *)&(m_EffList[i].m_CLightNum),sizeof(int),1,fp);
				clightnum = m_EffList[i].m_CLightNum;
				if (clightnum) {
					m_EffList[i].m_CLightStart =  new int[clightnum];
					m_EffList[i].m_CLightColor =  new color[clightnum];
					m_EffList[i].m_CLightDistance =  new float[clightnum];
					m_EffList[i].m_CLightTime =  new DWORD[clightnum];
					m_EffList[i].m_CLightSwitch =  new bool[clightnum];
					m_EffList[i].m_CLightTarget =  new int[clightnum];
					
					fread((int *)m_EffList[i].m_CLightStart,sizeof(int),clightnum,fp);
					fread((color *)m_EffList[i].m_CLightColor,sizeof(color),clightnum,fp);
					fread((float *)m_EffList[i].m_CLightDistance,sizeof(float),clightnum,fp);
					fread((DWORD *)m_EffList[i].m_CLightTime,sizeof(DWORD),clightnum,fp);
					fread((bool *)m_EffList[i].m_CLightSwitch,sizeof(bool),clightnum,fp);
					fread((int *)m_EffList[i].m_CLightTarget,sizeof(int),clightnum,fp);

				}
		
				// speed value
				fread((float *)(m_EffList[i].m_SpeedValue),sizeof(float),3,fp);
				//accel value
				fread((float *)(m_EffList[i].m_AccelValue),sizeof(float),3,fp);
				// startpos
				fread((float *)(m_EffList[i].m_StartPos),sizeof(float),3,fp);
				fread((bool *)&(m_EffList[i].m_bStartRot),sizeof(bool),1,fp);
				//loop
				fread((int *)&(m_EffList[i].m_LoopStart),sizeof(int),1,fp);
				fread((int *)&(m_EffList[i].m_LoopEnd),sizeof(int),1,fp);
				fread((int *)&(m_EffList[i].m_LoopSwitch),sizeof(int),1,fp);
				//camera shake value
				fread((int *)&(m_EffList[i].m_ShakeNum),sizeof(int),1,fp);
				shakenum = m_EffList[i].m_ShakeNum;
				if (shakenum > 0) {
					m_EffList[i].m_Shake = new CEffShake[shakenum];
					fread((CEffShake *)m_EffList[i].m_Shake,sizeof(CEffShake),shakenum,fp);
				}
				//sound
				fread((int *)&(m_EffList[i].m_SoundNum),sizeof(int),1,fp);
				soundnum = m_EffList[i].m_SoundNum;

				if (soundnum> 0) {
					m_EffList[i].m_Sound = new CEffSound[soundnum];
					fread((CEffSound *)m_EffList[i].m_Sound,sizeof(CEffSound),soundnum,fp);
				}
			
			}//for
			//interface pos
			fread((bool *)&(m_bInterfacePos),sizeof(bool),1,fp);
			fread((float *)m_InterfacePos,sizeof(float),2,fp);
			
			/////////////////////CEffPlay
			// start id info
			fread((int *)&(m_EffPlayData.m_StartIdNum),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_StartCount),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_StartFrameNextEff),sizeof(int),1,fp);
			if (m_EffPlayData.m_StartIdNum > 0) {
				m_EffPlayData.m_StartId = new int[m_EffPlayData.m_StartIdNum];
				fread((int *)m_EffPlayData.m_StartId,sizeof(int),m_EffPlayData.m_StartIdNum,fp);
			}
			//mid info
			fread((int *)&(m_EffPlayData.m_MidEffIdNum),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_MidCount),sizeof(int),1,fp);
			if (m_EffPlayData.m_MidEffIdNum > 0) {
				m_EffPlayData.m_MidEffId = new int[m_EffPlayData.m_MidEffIdNum];
				fread((int *)m_EffPlayData.m_MidEffId,sizeof(int),m_EffPlayData.m_MidEffIdNum,fp);
			}

			//end info 
			fread((float *)m_EffPlayData.m_EndPos,sizeof(float),3,fp);
			
			fread((int *)&(m_EffPlayData.m_EndEffIdNum),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_EndCount),sizeof(int),1,fp);
			if (m_EffPlayData.m_EndEffIdNum > 0) {
				m_EffPlayData.m_EndEffId = new int[m_EffPlayData.m_EndEffIdNum];
				fread((int *)m_EffPlayData.m_EndEffId,sizeof(int),m_EffPlayData.m_EndEffIdNum,fp);
			}

			//speed accel min max speed	
			fread((float *)&(m_EffPlayData.m_Speed),sizeof(float),1,fp);
			fread((float *)&(m_EffPlayData.m_MaxSpeed),sizeof(float),1,fp);
			fread((float *)&(m_EffPlayData.m_MinSpeed),sizeof(float),1,fp);
			fread((float *)&(m_EffPlayData.m_Accel),sizeof(float),1,fp);
			// mid direct	
			fread((int *)&(m_EffPlayData.m_Direct),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_MidEffNum),sizeof(int),1,fp);
			fread((int *)&(m_EffPlayData.m_StartMidNextEff),sizeof(int),1,fp);
			/////////////////////////////
			//other value set
			fread((bool *)&(m_StartPosSet),sizeof(bool),1,fp);
			fread((D3DXVECTOR3 *)&(m_StartPos),sizeof(D3DXVECTOR3),1,fp);
			fread((D3DXVECTOR3 *)&(m_MidPos),sizeof(D3DXVECTOR3),1,fp);
			fread((D3DXVECTOR3 *)&(m_OldMidPos),sizeof(D3DXVECTOR3),1,fp);
			
			fread((D3DXVECTOR3 *)&(m_MidUnit),sizeof(D3DXVECTOR3),1,fp);
			fread((D3DXVECTOR3 *)&(m_MidRot),sizeof(D3DXVECTOR3),1,fp);
			fread((bool *)&(m_NotRot),sizeof(bool),1,fp);
			// endpos set
			fread((int *)&m_EndPosNum,sizeof(int),1,fp);
			if (m_EndPosNum >0) {
				m_EndPos = new D3DXVECTOR3[m_EndPosNum];

				fread((D3DXVECTOR3 *)m_EndPos,sizeof(D3DXVECTOR3),m_EndPosNum,fp);
			}
			//delay frame tickframe
			fread((int *)&m_DelayFrame,sizeof(int),1,fp);
			fread((int *)&m_TickFrame,sizeof(int),1,fp);
			//fade out write
			fread((float *)m_FadeOut,sizeof(float),3,fp);
			for (i=0;i<m_EffNum;i++) {
				//hit
				int size = fread((int *)&(m_EffList[i].m_HitFrameNum),sizeof(int),1,fp);
				if (size == 0) {
					m_EffList[i].m_HitFrameNum = 0;
					break;
				}
				hitnum = m_EffList[i].m_HitFrameNum;

				if (hitnum> 0) {
					m_EffList[i].m_HitFrame = new int[hitnum];
					fread((int *)m_EffList[i].m_HitFrame,sizeof(int),hitnum,fp);
				}
			}
			
			for (i=0;i<m_EffNum;i++) { // CEffExt6 read (hit 가 script안에 있다는 가정하에 쓰임)
									  // 기존 compile된 script와 호완하기 위해서..
				int array[50];
				memset((int *)array,0,sizeof(int) * 50);
				
				int size = fread((int *)array,sizeof(int),50,fp);
				// 각종 효과들의 bit or number load
				
				m_EffList[i].m_Ext6ValueNum= array[0];
				m_EffList[i].m_SnapNum = array[1];
				m_bExt5ex = (array[2]>0) ? true:false;
				m_LningNum = array[3];		//번개
				m_WheelNum = array[4];		//궤적
				m_WeaponEffNum = array[5];	//weapon effect
				m_bBezierMid = (array[6] > 0) ? true : false;		// Bezier Middle
				m_iLight = array[7];		// Vertex Light
				m_bPlayTime	= (array[8] > 0) ? true : false;		// Play Time
				m_bLightn2 = (array[9] > 0) ? true : false;			// Lightning2
				m_bEffCharacter = (array[10] > 0) ? true : false;	// Character Effect
				m_bEffWeaponLine = (array[11] > 0) ? true : false;	// Weapon Line

				//
			
				int ext6num = 0;
				int snapnum = 0;

				if (size == 0) {
					m_EffList[i].m_Ext6ValueNum= 0;
					m_EffList[i].m_SnapNum = 0;
					break;
				}
				ext6num = m_EffList[i].m_Ext6ValueNum;

				if (ext6num> 0) { // slide
					m_EffList[i].m_Ext6Value= new CEffExt6[ext6num];
					fread((CEffExt6 *)m_EffList[i].m_Ext6Value,sizeof(CEffExt6),ext6num,fp);
				}
				// snap read
				snapnum = m_EffList[i].m_SnapNum;
				if (snapnum > 0) { 
					m_EffList[i].m_Snap= new CEffSnap[snapnum];
					fread((CEffSnap *)m_EffList[i].m_Snap,sizeof(CEffSnap),snapnum,fp);
				}
				if (m_bExt5ex) { //extension5 extension
					if (m_Ext5Value == NULL) { 
						m_Ext5Value = new CEffExt5;
					}	
					m_bExt5 = true;
					fread((char *)m_Ext5Value->m_TextureName,sizeof(char),ESBUF_SIZE,fp);
					fread((bool *)&m_Ext5Value->m_Rand,sizeof(bool),1,fp);
					fread((float *)&m_Ext5Value->m_LineSize,sizeof(float),1,fp);
					
				}
				if (i == 0) { // 처음 한번만 read
					if (m_LningNum >0) { // 번개효과
						m_bLightning = true; //번개 bit = true
						m_Lning = new CEffLightn[m_LningNum];
						fread((CEffLightn *)m_Lning,sizeof(CEffLightn),m_LningNum,fp);
						for (int ln = 0;ln < m_LningNum;ln++)
							m_Lning[ln].m_Lightn = new CLightning[m_Lning[ln].m_LnNum]; //CLightning 객체 생성

					}
				}
				if (i == 0) { // 처음 한번만 write
					if (m_WheelNum >0) { // 궤적 라인
						m_bWheel = true;
						m_Wheel = new CEffWheel[m_WheelNum];
						
						for (int ln=0;ln< m_WheelNum;ln++) {
							fread((int *)&(m_Wheel[ln].m_BlendMode),sizeof(int),1,fp);
							fread((float *)&(m_Wheel[ln].m_PlaneSize),sizeof(float),1,fp);
							fread((int *)&(m_Wheel[ln].m_PlaneValue),sizeof(int),1,fp);
							fread((int *)&(m_Wheel[ln].m_PointNum),sizeof(int),1,fp);
							fread((char *)m_Wheel[ln].m_TexName,sizeof(char),ESBUF_SIZE,fp);
							fread((float *)&(m_Wheel[ln].m_UnitUv),sizeof(float),1,fp);
						}
					}
				}
				if (i == 0) { //처음 한번만 read
					if (m_WeaponEffNum > 0) {	//weapon effect
						m_bWeaponEff = true;
						m_WeaponEff = new CEffWeapon[m_WeaponEffNum];

						fread((CEffWeapon *)m_WeaponEff,sizeof(CEffWeapon),m_WeaponEffNum,fp);
					}
				}
				if (i == 0) {
					if (m_bBezierMid == true ) {		// Bezier Middle
						m_pBezierMid = new CEffBezier;
						fread((CEffBezier *)m_pBezierMid,sizeof(CEffBezier),1,fp);
						m_pBezierMid->m_pBezierLine = NULL;
						m_pBezierMid->m_nBezierPoint = 30;				// 총 제어점 갯수
					}
				}
				if (i == 0) {
					if (m_iLight > 0) {				// Vertex Light
						m_pLight = new CEffLight[m_iLight];
						fread((CEffLight *)m_pLight,sizeof(CEffLight),m_iLight,fp);
					}
				}
				if (i == 0) {
					if (m_bPlayTime == true) {				// PlayTime
						int iValue[2] = {0};
						fread((int *)iValue,sizeof(int),2,fp);
						m_iStartPlayTime = iValue[0];
						m_iEndPlayTime = iValue[1];
						m_bVisTime = true;
					}
				}
				if (i==0) {
					if (m_bLightn2 == true) {				// Lightning2
						m_pLightn2 = new CEffLightn2;
						fread((CEffLightn2 *)m_pLightn2,sizeof(CEffLightn2),1,fp);

						m_pLightn2->m_pLightn = new CEffLightn[(m_pLightn2->m_iMaxLine)];
						m_pLightn2->m_pTextureName = new CEffTextureDef[(m_pLightn2->m_iMaxLine)];
						
						fread((CEffTextureDef *)(m_pLightn2->m_pTextureName),sizeof(CEffTextureDef),(m_pLightn2->m_iMaxLine),fp);

						GetBinLightn2();					// Sub Lightning 객체 생성및 처리
					}
				}
				if (i == 0) {
					if (m_bEffCharacter == true) {
						m_pEffCharacter = new CEffChar;
						fread((CEffChar *)m_pEffCharacter,sizeof(CEffChar),1,fp);
					}
				}
				if (i == 0) {
					if (m_bEffWeaponLine == true) {							// 검기 관련 제어 Load (WEAPONLINE:)
						m_pEffWeaponLine = new CEffWeaponLine;
						fread((CEffWeaponLine *)m_pEffWeaponLine,sizeof(CEffWeaponLine),1,fp);
						m_pEffWeaponLine->m_pLineTexture = new CTexture;	// 검기 관련 텍스쳐
						CTexture::SetPath(EFFECTTEXTUREPATH);
						m_pEffWeaponLine->m_pLineTexture->Load(m_pEffWeaponLine->m_strTextureName);
						
					}
				}
			}

		}
		else 
		{
			m_NextScript = new int[m_SubScriptNum];
			m_SubScript = new CEffScript[m_SubScriptNum];
			fread((int *)m_NextScript,sizeof(int),m_SubScriptNum,fp);
			SetDevice(m_Device);
			for (i=0;i<m_SubScriptNum;i++) 
			{
				fread((char *)(m_SubScript[i].m_FileName),sizeof(char) * ESBUF_SIZE,1,fp);		
				GetBinSubData(i,m_SubScript[i].m_FileName);
				
			}
		}
		//_ASSERTE( _CrtCheckMemory()); 
		fclose(fp);
	}
	else 
	{
		m_FileLoad = false;
		//char tmp_message[256]={0};
		//sprintf(tmp_message,"%s 이 존재하지 않습니다.",filepath);

		//MessageBox(NULL,tmp_message,"Error",MB_OK);
		return false;
	}

	return true;

}
void CEffScript::GetLnNum() {
	char *tok = NULL;
	tok = strtok(NULL," \t\n");
	m_LningNum  = atoi(tok);
}
void CEffScript::GetLn3Num() {
	int iMaxLine = 0;
	int iMaxDummy = 0;

	char *tok = NULL;
	tok = strtok(NULL," \t\n");
	m_bLightn3 = true;
	m_pLightn3 = new CEffLightn3;
	

	iMaxLine = atoi(tok);		// Max Line Setting
	m_pLightn3->m_iMaxLine = iMaxLine;
	
	iMaxDummy = 2;
	m_pLightn3->m_iEffChar = iMaxDummy;

	tok = strtok(NULL," \t\n");
	m_pLightn3->m_fUvAniMax = atof(tok);	// Max Uv Ani 
	
	tok = strtok(NULL," \t\n");
	m_pLightn3->m_fRange = atof(tok);		// Max Range
	
	tok = strtok(NULL," \n\t");				// Blend
	m_pLightn3->m_iBlend = atoi(tok);

	m_pLightn3->m_pLightn = new CEffLightn[iMaxLine];
	m_pLightn3->m_pEffChar = new CEffChar[iMaxDummy];
	m_pLightn3->m_pTextureName = new CEffTextureDef[iMaxLine];
	

}
void CEffScript::GetLn2Num() {
	int iMaxLine = 0;

	char *tok = NULL;
	tok = strtok(NULL," \t\n");
	m_bLightn2 = true;
	m_pLightn2 = new CEffLightn2;

	iMaxLine = atoi(tok);		// Max Line Setting
	m_pLightn2->m_iMaxLine = iMaxLine;

	tok = strtok(NULL," \t\n");
	m_pLightn2->m_fUvAniMax = atof(tok);	// Max Uv Ani 
	
	tok = strtok(NULL," \t\n");
	m_pLightn2->m_fRange = atof(tok);		// Max Range
	
	tok = strtok(NULL," \n\t");
	m_pLightn2->m_vecPos.x = atof(tok);

	tok = strtok(NULL," \n\t");
	m_pLightn2->m_vecPos.y = atof(tok);

	tok = strtok(NULL," \n\t");
	m_pLightn2->m_vecPos.z = atof(tok);

	tok = strtok(NULL," \n\t");				// Blend
	m_pLightn2->m_iBlend = atoi(tok);

	m_pLightn2->m_pLightn = new CEffLightn[iMaxLine];
	m_pLightn2->m_pTextureName = new CEffTextureDef[iMaxLine];
	

}
void CEffScript::GetEffWeaponNum() {
	char *tok = NULL;
	tok = strtok(NULL," \t\n");
	m_WeaponEffNum = atoi(tok);
}
void CEffScript::GetEffWheelNum() {
	char *tok = NULL;
	tok = strtok(NULL," \t\n");
	m_WheelNum  = atoi(tok);
}
//WEAPONLINE: [Texture Name] [R] [G] [B] [블랜드 모드]
void CEffScript::GetEffWeaponLine() {
	m_bEffWeaponLine = true;
	m_pEffWeaponLine = new CEffWeaponLine;
	char *token = strtok(NULL," \n\t");
	// Texture
	m_pEffWeaponLine->m_pLineTexture = new CTexture;
	CTexture::SetPath(EFFECTTEXTUREPATH);
	strcpy(m_pEffWeaponLine->m_strTextureName,token);
	m_pEffWeaponLine->m_pLineTexture->Load(token);
	// Color 
	m_pEffWeaponLine->m_iColor[0] = atoi(strtok(NULL," \n\t"));
	m_pEffWeaponLine->m_iColor[1] = atoi(strtok(NULL," \n\t"));
	m_pEffWeaponLine->m_iColor[2] = atoi(strtok(NULL," \n\t"));
	// Blend Mode
	m_pEffWeaponLine->m_iBlendMode = atoi(strtok(NULL," \n\t"));

}
void CEffScript::GetEffChar() {
	m_bEffCharacter = true;
	m_pEffCharacter = new CEffChar;
	char * token = strtok(NULL," \n\t[]");
	// Pivot name
	strcpy(m_pEffCharacter->m_PivotName,token);
	// local Position
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecPos.x = atof(token);
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecPos.y = atof(token);
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecPos.z = atof(token);
	// local quaternion
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecQuat.x = atof(token);
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecQuat.y = atof(token);
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecQuat.z = atof(token);
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->vecQuat.w = atof(token);
	// Position, Rotation Update 
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->m_bPosUpdate = (atoi(token)) ? true : false;
	token = strtok(NULL," \n\t[]");
	m_pEffCharacter->m_bRotUpdate = (atoi(token)) ? true : false;
	// First Update Flag False Setting
	m_pEffCharacter->m_bFirstUpdate  = false;

	
}
bool CEffScript::GetScriptData(char *esfname) {
	char buf[ESBUF_SIZE] = {0};
	char filepath[ESBUF_SIZE] = {0};

	char *token = NULL;

	int CurrentLoad = ESEMPTY;
	if (esfname == NULL) {
		return false;
	}
	strcpy(m_FileName,esfname);

	strcpy(filepath,EFFECTSCRIPTPATH);
	strcat(filepath,esfname);

	m_ScriptFile = fopen(filepath,"rt");
	
	if (!m_ScriptFile) {
		char tmp_message[256]={0};
		sprintf(tmp_message,"%s 이 존재하지 않습니다.",filepath);
		MessageBox(NULL,tmp_message,"Error",MB_OK);
#ifdef __EFF_WCREATOR__
		WriteDebug(filepath);
#endif

		return false;
	}
	else {
		m_FileLoad = true;
	}
	while(!feof(m_ScriptFile)) {
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
		token = strtok(buf," \t\n");
		if (token) {
			if (token[0] == '/')
				continue;
			// 라벨 체크
			if (!strcmp(token,ESSTART))
				CurrentLoad = ESS;
			else if (!strcmp(token,ESLIST)) {
				CurrentLoad = EST;
				// effect list num setting
				GetEffNum();
			
			}
			else if (!strcmp(token,ESLOOP)) {
				CurrentLoad = ESL;
			

			}
			else if (!strcmp(token,ESPLAY)) {
				CurrentLoad = ESP;
				GetStartEndNum();
			}
			else if (!strcmp(token,ESSNAP)) {
				CurrentLoad = ESN;
				//GetSnapNum();
			
			}
			else if (!strcmp(token,ESFADEOUT)) {
				CurrentLoad = ESFO;
				
			}
			else if (!strcmp(token,ESFADEIN)) {
				CurrentLoad = ESFI;
				
			}
			else if (!strcmp(token,ESSHAKE)) {
				CurrentLoad = ESSK;
				
			}
			else if (!strcmp(token,ESSOUND)) {
				CurrentLoad = ESO;
			}
			else if (!strcmp(token,ESHIT)) {
				CurrentLoad = ESH;
			}
			else if (!strcmp(token,ESEND)) {
				CurrentLoad = ESE;
				
			}
			else if (!strcmp(token,ESSUB)) {
				CurrentLoad = ESB;
				GetSubNum();

			}
			else if (!strcmp(token,ESCLIGHT)) {
				CurrentLoad = ESCL;
			}
			else if (!strcmp(token,ESSLIDE)) { // Slide
				CurrentLoad = ESSL;
			}
			else if (!strcmp(token,ESEXT5)) {
				CurrentLoad = ESE5;
			}
			else if (!strcmp(token,ESLIGHTN)) { //lightning
				CurrentLoad = ESLN;
				GetLnNum();
			}
			else if (!strcmp(token,ESLIGHTN2)) { //lightning2
				CurrentLoad = ESLN2;
				GetLn2Num();
			}
			else if (!strcmp(token,ESLIGHTN3)) {	//lightning3
				CurrentLoad = ESLN3;
				GetLn3Num();
			}
			else if (!strcmp(token,ESWHEEL)) { // wheel
				CurrentLoad = ESW;
				GetEffWheelNum();
			}	
			else if (!strcmp(token,ESWEAPON)) { //weapon effect
				CurrentLoad = ESWP;
				GetEffWeaponNum();
			}
			else if (!strcmp(token,ESBEZIERM)) {	//Bezier Middle
				CurrentLoad = ESBM;
			
			}
			else if (!strcmp(token,ESLIGHT)) {	// Light
				CurrentLoad = ESLT;
				GetEffLightNum();
			}
			else if (!strcmp(token,ESPLAYT)) {	// Play Time
				CurrentLoad = ESPT;
				GetEffPlayTime();
			}
			else if (!strcmp(token,ESCHAR)) {	// Character
				CurrentLoad = ESCH;
				GetEffChar();
			}
			else if (!strcmp(token,ESWEAPONL)) {	// 검기 관련 제어
				CurrentLoad = ESWL;
				GetEffWeaponLine();

			}
			else { // data 부분 로드
				switch (CurrentLoad) {
				case EST:				// list load
					GetEffList(token); 
					break;
				case ESL:
					GetEffLoop(token);
					break;
				case ESP:
					GetEffPlay(token);
					break;
				case ESN:
					GetEffSnap(token);
					break;
				case ESFO:				//fade out
					GetFadeOut(token);
					break;
				case ESFI:				//fade in
					GetFadeIn(token);
					break;
				case ESSK:				//camera shake
					GetCameraShake(token);
					break;
				case ESO:				//sound
					GetSound(token);
					break;
				case ESB:
					GetSubData(token);
					break;
				case ESCL:				//CLight
					GetCLight(token);
					break;
				case ESH:				//Hit
					GetHit(token);
					break;
				case ESSL:				//slide
					GetSlide(token);
					break;
				case ESE5:
					GetExt5ex(token);
					break;
				case ESLN:				//lightning
					GetLightn(token);
					break;
				case ESLN2:				//lightning2
					GetLightn2(token);
					break;
				case ESLN3:				//lightning3
					GetLightn3(token);
				case ESW:				//Wheel
					GetEffWheel(token);
					break;
				case ESWP:				//weapon Effect
					GetEffWeapon(token);
					break;
				case ESBM:				//Bezier Middle
					GetEffBezier(token);
					break;
				case ESLT:				// Vertex Light 적용되는 eff load
					GetEffLight(token);
					break;
				case ESPT:
					break;
				case ESCH:
					break;
				case ESWL:
					break;

				}
				if (CurrentLoad == ESE)
					break;

			}

		} // if (token)
		
	} // while
	if (m_ScriptFile != NULL) {

		fclose(m_ScriptFile);
		m_ScriptFile = NULL;
		//WriteConvertFile(m_FileName);
	}
	
	return true;

}
void CEffScript::GetEffPlayTime() {				// Play Time Setting (시간 개념 들어갔을때를 준비)
	char *token = strtok(NULL," \n\t");
	m_iStartPlayTime = atoi(token);
	token = strtok(NULL," \n\t");
	m_iEndPlayTime = atoi(token);
	m_bPlayTime = true;
}
void CEffScript::GetEffLightNum() {
	char *token = strtok(NULL," \t\n");
	m_iLight = atoi(token);
	m_pLight = new CEffLight[m_iLight];

}
void CEffScript ::GetEffNum() {
	char *token = strtok(NULL," \t\n");
	m_EffNum = atoi(token);

	m_EffList = new CEffInfo[m_EffNum];
	
}
void CEffScript::GetSubNum() {

	char *token = strtok(NULL," \t\n");
	m_SubScriptNum = atoi(token); 
	m_SubScript = new CEffScript[m_SubScriptNum];
	m_NextScript = new int[m_SubScriptNum];
	if (m_SubScriptNum)
		SetDevice(m_Device);
}
void CEffScript::GetBinSubData(int index,char *filename) {
	char *dot = NULL;
	FILE *fp = NULL;
	char filepath[256] = {0};

	strcpy(filepath,EFFECTSCRIPTPATH);
	strcat(filepath,filename);

	fp = fopen(filepath,"rb");
	
	if (fp == NULL) 
	{ 
		// 열려고 하는 파일 없을시 검사 시작
#ifdef __EFF_WCREATOR__
		WriteDebug(filepath);
#endif

		if ((m_Weapone != ESEMPTY) || (m_Sex != ESEMPTY)) { 
			dot = strrchr(filename,'.');
			// sub effect script load

			switch(m_Weapone) {
			case 6:// 한손 무기
			case 7:
			case 8:
				(*dot) = '_';dot++;
				(*dot) = '6';dot++;
				break;
			case 9:
			case 10:
				(*dot) = '_';dot++;
				(*dot) = '9';dot++;
				break;
			case 11:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '1';dot++;
				break;
			case 12:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '2';dot++;
				break;
			case 13:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '3';dot++;
				break;
			case 14:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '4';dot++;
				break;
			case 15:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '5';dot++;
				break;
			case 33: // 아칸 한손무기
			case 34:
				(*dot) = '_';dot++;
				(*dot) = '3';dot++;
				(*dot) = '3';dot++;
				break;

			case 35:
			case 36:
			case 37:
				(*dot) = '_';dot++;
				(*dot) = '6';dot++;
				break;
			default:
				break;
			}
			switch(m_Sex) {
			case 1:
				(*dot) = '_';dot++;
				(*dot) = 'm';dot++;
				break;
			case 2:
				(*dot) = '_';dot++;
				(*dot) = 'f';dot++;
				break;
			default:
				break;
			}
			(*dot) = '.';dot++;
			(*dot) = 'e';dot++;
			(*dot) = 's';dot++;
			(*dot) = 'f';dot++;
		}
	}
	else
		fclose(fp);

	m_SubScript[index].GetScriptRealBinData(filename);

}
void CEffScript::GetSubData(char *tok) {
	int i = 0;
	FILE *fp;

	char *dot = NULL;

	char *token = NULL;
	char filename[ESBUF_SIZE]= {0};

	char buf[ESBUF_SIZE] = {0};
	
	m_NextScript[i] = atoi(tok);
	token = strtok(NULL," \n\t");
	strcpy(filename,token);
	fp = fopen(filename,"rt");
	
	if (!fp) { // 열려고 하는 파일 없을시 검사 시작
		if ((m_Weapone != ESEMPTY) || (m_Sex != ESEMPTY)) { 
			dot = strrchr(filename,'.');
			// sub effect script load

			switch(m_Weapone) {
			case 6:// 한손 무기
			case 7:
			case 8:
				(*dot) = '_';dot++;
				(*dot) = '6';dot++;
				break;
			case 9:
			case 10:
				(*dot) = '_';dot++;
				(*dot) = '9';dot++;
				break;
			case 11:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '1';dot++;
				break;
			case 12:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '2';dot++;
				break;
			case 13:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '3';dot++;
				break;
			case 14:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '4';dot++;
				break;
			case 15:
				(*dot) = '_';dot++;
				(*dot) = '1';dot++;
				(*dot) = '5';dot++;
				break;
			case 33: // 아칸 한손무기
			case 34:
			case 35:
			case 36:
			case 37:
				(*dot) = '_';dot++;
				(*dot) = '3';dot++;
				(*dot) = '3';dot++;
				break;
			default:
				break;
			}
			switch(m_Sex) {
			case 1:
				(*dot) = '_';dot++;
				(*dot) = 'm';dot++;
				break;
			case 2:
				(*dot) = '_';dot++;
				(*dot) = 'f';dot++;
				break;
			default:
				break;
			}
			(*dot) = '.';dot++;
			(*dot) = 'e';dot++;
			(*dot) = 's';dot++;
			(*dot) = 'f';dot++;
		}
	}
	else {
		fclose(fp);
	}
	m_SubScript[i].GetScriptData(filename);
	
	for (i=1;i<m_SubScriptNum;i++) {
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
		token = strtok(buf," \n\t");
		m_NextScript[i] = atoi(token);

		token = strtok(NULL," \t\n");
		m_SubScript[i].GetScriptData(token);
		
	}
}
void CEffScript ::GetSnapNum() {
	char *token = strtok(NULL," \t\n");
	m_EffPlayData.m_SnapNum = atoi(token);
	if (m_EffPlayData.m_SnapNum)
		m_EffPlayData.m_SnapEff = new CEffSnap[m_EffPlayData.m_SnapNum];

}
void CEffScript ::GetStartEndNum() {
	char *token = strtok(NULL," \t\n");
	m_EffPlayData.m_StartIdNum = atoi(token);
	token = strtok(NULL," \t\n");
	m_EffPlayData.m_MidEffIdNum = atoi(token);
	token = strtok(NULL," \t\n");
	m_EffPlayData.m_EndEffIdNum = atoi(token);
	token = strtok(NULL," \t\n");
	if (token) 
		m_TickFrame = atoi(token);
	
	if (m_EffPlayData.m_StartIdNum)
		m_EffPlayData.m_StartId = new int[m_EffPlayData.m_StartIdNum];
	if (m_EffPlayData.m_MidEffIdNum) {
		m_EffPlayData.m_MidEffId = new int[m_EffPlayData.m_MidEffIdNum];
		// 궤적 (각 mid 마다 하나씩)
		//m_Wheel = new CLightning[m_EffPlayData.m_MidEffIdNum];

	//	m_EffPlayData.m_MidValue = new CEffMidValue[m_EffPlayData.m_MidEffIdNum];
	}
	if (m_EffPlayData.m_EndEffIdNum)
		m_EffPlayData.m_EndEffId = new int[m_EffPlayData.m_EndEffIdNum];

}
void CEffScript::SetSkillInfo(int s,int w) {
	m_Sex = s;
	m_Weapone = w;

}
void CEffScript::SetDevice(LPDIRECT3DDEVICE8 device) { 
	int i;
	m_Device = device;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetDevice(m_Device);
	}
}
void CEffScript::SetRot(D3DXVECTOR3 before,D3DXVECTOR3 now,D3DXVECTOR3 &target) {
	float  roll, pitch, yaw;
	float lateDot = 0.0f;
	roll = pitch = yaw = 0.0f;

	D3DXVec3Normalize(&before,&before);
	D3DXVec3Normalize(&now,&now);

	D3DXVECTOR3 late(0.0f,0.0f,0.0f);
	D3DXVECTOR3 tmp(0.0f,0.0f,0.0f);
	
	D3DXVECTOR3 tmp2(0.0f,0.0f,0.0f);
		
	tmp = now - before;
	D3DXVec3Normalize(&tmp,&tmp);
	
	D3DXVec3Cross(&tmp2,&now,&tmp);
					 
	D3DXVec3Cross(&late,&tmp2,&now);
	D3DXVec3Normalize(&late,&late);

	lateDot = D3DXVec3Dot(&tmp,&late);
	if (lateDot == 0) {
		roll = 0.0f;  
	} 
	else {
		roll = (float) -atan2(ESGRAVITY, lateDot) + ESHALF_PI;
	}

	pitch = (float) -atan(now.y / sqrt((now.z * now.z) + (now.x * now.x)));

	yaw = (float) atan2(now.x, now.z);
					
	target.x = pitch;
	target.y = yaw;
	target.z = roll;
}
void CEffScript ::SetMine(bool m) {
	int i;
	if(m == false)
	{
		if(strstr(m_FileName,"MONWORLD_"))	// World Monster effect 처리.
		{
			m = true;
		}
	}
	m_Mine = m;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetMine(m);
	}
}
void CEffScript::SetVisibility(bool t) {

	m_bVisibility = t;
/*	for ( i = 0; i < m_SubScriptNum; i++ ) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetVisibility(t);

	}
*/
}
void CEffScript ::SetAllLoopOff() {
	int i;
	if ( m_lstEff.size() > 0 ) {
		for (i=0;i<m_EffNum;i++) {
			if (m_lstEff[i] != NULL)
				m_lstEff[i]->SetLoop(false);

		}
	}
	if (m_SubScript != NULL) {				// Sub Script Loop Off
		for ( i = 0; i < m_SubScriptNum; i++ ) {
			m_SubScript[i].SetAllLoopOff();

		}
	}

}
void CEffScript ::SetEndPos(D3DXVECTOR3 *end,int num) {
	if (m_bPosSet) {
		if (m_bExt7) {// extension7 일때 SetEndPos 할시 start pos setting
			int i;
			int index;
			m_StartPosSet = true;
			m_StartPos.x = end[0].x;
			m_StartPos.y = end[0].y;
			m_StartPos.z = end[0].z;

			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
					D3DXVECTOR3 tmp_pos;

					index = m_EffPlayData.m_StartId[i];
								
					tmp_pos = m_StartPos;
					tmp_pos.x += (m_EffList[index].m_StartPos[0]* m_MulPos);
					tmp_pos.y += (m_EffList[index].m_StartPos[1]* m_MulPos);
					tmp_pos.z += (m_EffList[index].m_StartPos[2]* m_MulPos);
					
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}

			}
			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetStartPos(m_StartPos);
			}
		}
		else {  // 기본 end pos setting
			int i;
			int index;
			m_EndPosNum = num;
			
			/*if (m_EndPos) {
				delete[] m_EndPos;
				m_EndPos = NULL;
			}
			*/
			if (m_EndPos == NULL)						// (03.03.01 수정)
				m_EndPos = new D3DXVECTOR3[m_EndPosNum];
			
			memcpy(m_EndPos,end,sizeof(D3DXVECTOR3) * m_EndPosNum);
			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_EndEffIdNum;i++) {
					D3DXVECTOR3 tmp_pos;
					tmp_pos.x = m_EndPos[0].x + (m_EffPlayData.m_EndPos[0] * m_MulPos);
					tmp_pos.y = m_EndPos[0].y + (m_EffPlayData.m_EndPos[1] * m_MulPos);
					tmp_pos.z = m_EndPos[0].z + (m_EffPlayData.m_EndPos[2] * m_MulPos);

					index = m_EffPlayData.m_EndEffId[i];
					if (index == ESEMPTY)
						break;
					// effect 시작
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}
			}
			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetEndPos(end,num);
			}
		}

	}
}
void CEffScript::SetEndPos(float x,float y,float z) {
	if (m_bPosSet) {
		if (m_bExt7) {// extension7 일때 SetEndPos 할시 start pos setting
			int i;
			int index;
			m_StartPosSet = true;
			m_StartPos.x = x;
			m_StartPos.y = y;
			m_StartPos.z = z;

			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
					D3DXVECTOR3 tmp_pos;

					index = m_EffPlayData.m_StartId[i];
								
					tmp_pos = m_StartPos;
					tmp_pos.x += (m_EffList[index].m_StartPos[0]* m_MulPos);
					tmp_pos.y += (m_EffList[index].m_StartPos[1]* m_MulPos);
					tmp_pos.z += (m_EffList[index].m_StartPos[2]* m_MulPos);
					
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}

			}
			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)		
					m_SubScript[i].SetStartPos(m_StartPos);
			}
		}
		else { // 기본 end pos setting

			int i;
			int index;
			m_EndPosNum = 1;
			/*if (m_EndPos) {
			
				delete[] m_EndPos;
				m_EndPos = NULL;
			}*/

			if (m_EndPos == NULL)							// (03.03.01 수정)
				m_EndPos = new D3DXVECTOR3[m_EndPosNum];
			
			m_EndPos[0].x = x;
			m_EndPos[0].y = y;
			m_EndPos[0].z = z;
			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_EndEffIdNum;i++) {
					D3DXVECTOR3 tmp_pos;
					tmp_pos.x = m_EndPos[0].x + (m_EffPlayData.m_EndPos[0]* m_MulPos);
					tmp_pos.y = m_EndPos[0].y + (m_EffPlayData.m_EndPos[1]* m_MulPos);
					tmp_pos.z = m_EndPos[0].z + (m_EffPlayData.m_EndPos[2]* m_MulPos);

					index = m_EffPlayData.m_EndEffId[i];
					if (index == ESEMPTY)
						break;
					// effect 시작
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}
			}

			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetEndPos(m_EndPos[0].x,m_EndPos[0].y,m_EndPos[0].z);
			}
		}
	}
}
void CEffScript::GetEffLight(char *effname) {
	char *token =  NULL;

	if (m_iLight > 0 ) {
		strcpy(m_pLight[0].m_strEff,effname);
		for (int i = 1;i < m_iLight;i++) {
			char strEff[ESBUF_SIZE] = {0};
			fgets(strEff,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
			token = strtok(strEff," \n\t");
			strcpy(m_pLight[0].m_strEff,token);
		}
	}
}
void CEffScript ::GetEffList(char *effname) {
	
	strcpy(m_EffList[m_CurrentNum].m_EffName,effname);
	char *token = strtok(NULL," \t\n");
	if (token) { // scale load
		m_EffList[m_CurrentNum].m_Scale = (float)atoi(token);
		m_EffList[m_CurrentNum].m_Scale /= 100.0f;
	}
	token = strtok(NULL," \t\n");
	if (token) {
		m_EffList[m_CurrentNum].m_MinFrame = atoi(token);
	}
	m_CurrentNum++;

}

void CEffScript ::GetEffLoop(char *tok) {
	int i;
	int index = ESEMPTY;
	char *token = NULL;

	for (i=0;i<m_EffNum;i++) {
		if (!strcmp(tok,m_EffList[i].m_EffName)) {
			index = i;
			break;
		}

	}
	if (index != ESEMPTY) {
		token = strtok(NULL," \t\n");
		m_EffList[index].m_LoopStart = atoi(token);
		token = strtok(NULL," \t\n");
		m_EffList[index].m_LoopEnd = atoi(token);
		token = strtok(NULL," \t\n");
		m_EffList[index].m_LoopSwitch = atoi(token);
	}
}

/*
void CEffScript ::SetWheelPoint(int index,float x,float y,float z) {

	int current_index = m_Wheel[index].m_Current;

	m_Wheel[index].m_Point[current_index].x = x;
	m_Wheel[index].m_Point[current_index].y = y;
	m_Wheel[index].m_Point[current_index].z = z;
	m_Wheel[index].m_Current++;
	if (m_Wheel[index].m_Current >= m_Wheel[index].m_PointNum) {
		m_Wheel[index].m_FullSetting = true;
		m_Wheel[index].m_Current = 0;
	}
}*/
void CEffScript ::GetEffPlay(char *tok) 
{
	char *token = NULL;
	int index = ESEMPTY;

	int i;

	if (!strcmp(tok,"S")) 
	{ //start
		token = strtok(NULL," \t\n");
		for (i=0;i<m_EffNum;i++) {
			if (!strcmp(token,m_EffList[i].m_EffName)) {
				index = i;
				break;
			}

		}
		if (index != ESEMPTY) {
			m_EffPlayData.m_StartId[m_EffPlayData.m_StartCount++] = index;
		}

		token = strtok(NULL," \t\n");
		if (!strcmp(token,"R")) 
		{
			m_EffList[index].m_bStartRot = true;
			token = strtok(NULL," \t\n");
		}

		if (token)
			m_DelayFrame = atoi(token);
		else {
			return;
		}

		token = strtok(NULL," \t\n");
		if (token) 
			m_EffPlayData.m_StartFrameNextEff = atoi(token);
		else {
	
			return;
		}

		token = strtok(NULL," \t\n");
		
		if (token) { // position
			m_EffList[index].m_StartPos[0] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[index].m_StartPos[1] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[index].m_StartPos[2] = atof(token);
		}
		
		token = strtok(NULL," \t\n");
		
		if (token) { // interface position
			m_bInterfacePos = true;
			m_InterfacePos[0] = atof(token);
			token = strtok(NULL," \t\n");
			m_InterfacePos[1] = atof(token);
		}
	}
	else if (!strcmp(tok,"M")) {
	
		token = strtok(NULL," \t\n");
		
		if (token[0] == '[') {
			token = strtok(NULL," \t\n");
			m_MidUnit.x = atof(token);
			token = strtok(NULL," \t\n");
			m_MidUnit.y = atof(token);
			token = strtok(NULL," \t\n");
			m_MidUnit.z = atof(token);
			token = strtok(NULL," \t\n");
			token = strtok(NULL," \t\n");
		}
		

		if (!strcmp(token,"N")) {
			//궤적에 따른 회전 없음
			m_NotRot = true;
			token = strtok(NULL," \t\n");
		}

		for (i=0;i<m_EffNum;i++) {
			if (!strcmp(token,m_EffList[i].m_EffName)) {
				index = i;
				break;
			}

		}
		if (index != ESEMPTY) {
			m_EffPlayData.m_MidEffId[m_EffPlayData.m_MidCount] = index;
		}
		token = strtok(NULL," \t\n"); 
		if (!token) {
			m_EffPlayData.m_MidCount++;	
			return;
		}
		//speed
		m_EffPlayData.m_Speed = atof(token);
		token = strtok(NULL," \t\n");
		m_EffPlayData.m_Accel = atof(token);
	
		token = strtok(NULL," \t\n"); 
		m_EffPlayData.m_MinSpeed = atof(token);
		
		token = strtok(NULL," \t\n"); 
		m_EffPlayData.m_MaxSpeed = atof(token);

		token = strtok(NULL," \t\n");

		if (!strcmp(token,"R")) { //Random 궤적
			// 한계 value get (int 형 절대값으로..)
			D3DXVECTOR3 tmp_s;
			D3DXVECTOR3 tmp_a;
			token = strtok(NULL," \t\n");
			tmp_s.x = atoi(token);
			token = strtok(NULL," \t\n");
			tmp_s.y = atoi(token);
			token = strtok(NULL," \t\n");
			tmp_s.z = atoi(token);
			
			token = strtok(NULL," \t\n");
			tmp_a.x = atoi(token);
			token = strtok(NULL," \t\n");
			tmp_a.y = atoi(token);
			token = strtok(NULL," \t\n");
			tmp_a.z = atoi(token);
		
			if (tmp_s.x) {
				m_EffList[i].m_SpeedValue[0] = (float)(rand() % (int)tmp_s.x);
				if (rand() %2)
					m_EffList[i].m_SpeedValue[0] *= (-1.0f);
			}
			else 
				m_EffList[i].m_SpeedValue[0] = 0.0f;
			
			if (tmp_s.y) {
				m_EffList[i].m_SpeedValue[1] = (float)(rand() % (int)tmp_s.y);
				if (rand() %2)
					m_EffList[i].m_SpeedValue[1] *= (-1.0f);
				}
			else
				m_EffList[i].m_SpeedValue[1] = 0.0f;
			
			if (tmp_s.z) {
				m_EffList[i].m_SpeedValue[2] = (float)(rand() % (int)tmp_s.z);
				if (rand() %2)
					m_EffList[i].m_SpeedValue[2] *= (-1.0f);
			}
			else
				m_EffList[i].m_SpeedValue[2] = 0.0f;
			
			if (tmp_a.x) {
				m_EffList[i].m_AccelValue[0] = (float)(rand() % (int)tmp_a.x);
				if (rand() %2)
					m_EffList[i].m_AccelValue[0] *= (-1.0f);
			}
			else 
				m_EffList[i].m_AccelValue[0] = 0.0f;
			if (tmp_a.y) {
				m_EffList[i].m_AccelValue[1] = (float)(rand() % (int)tmp_a.y);
				if (rand() %2)
					m_EffList[i].m_AccelValue[1] *= (-1.0f);
			}
			else 
				m_EffList[i].m_AccelValue[0] = 0.0f;
			if (tmp_a.z) {
				m_EffList[i].m_AccelValue[2] = (float)(rand() % (int)tmp_a.z);
				if (rand() %2)
					m_EffList[i].m_AccelValue[2] *= (-1.0f);

			}
			else 
				m_EffList[i].m_AccelValue[0] = 0.0f;
		}
		else { 
			m_EffList[i].m_SpeedValue[0] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[i].m_SpeedValue[1] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[i].m_SpeedValue[2] = atof(token);
			
			token = strtok(NULL," \t\n");
			m_EffList[i].m_AccelValue[0] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[i].m_AccelValue[1] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffList[i].m_AccelValue[2] = atof(token);
		}
		// 궤적 라인 DATA 가져옴
		token = strtok(NULL," \t\n");
		if (!token)
			return;
		int point_num = atoi(token);
		//m_Wheel[m_EffPlayData.m_MidCount].CreateList(point_num);
		
		token = strtok(NULL," \t\n");
		//m_Wheel[m_EffPlayData.m_MidCount].SetSize(atof(token));
		
		token = strtok(NULL," \t\n");
		//m_Wheel[m_EffPlayData.m_MidCount].CreateTexture(1);
		//m_Wheel[m_EffPlayData.m_MidCount].SetDevice(m_Device);
		// 라인 갯수
		//m_Wheel[m_EffPlayData.m_MidCount].SetVertNum(1);
		//m_Wheel[m_EffPlayData.m_MidCount].CreateVertexBuffer();
		//m_Wheel[m_EffPlayData.m_MidCount++].SetTexture(0,token);
	
	

			
	/*	m_Wheel[m_EffPlayData.m_MidCount].m_PointNum = atoi(token);
		if (m_Wheel[m_EffPlayData.m_MidCount].m_PointNum) {
			m_Wheel[m_EffPlayData.m_MidCount].m_Point = new D3DXVECTOR3[m_Wheel[m_EffPlayData.m_MidCount].m_PointNum];
		}
		token = strtok(NULL," \t\n");
		m_Wheel[m_EffPlayData.m_MidCount].m_QuadSize = atof(token);
		token = strtok(NULL," \t\n");
		strcpy(m_Wheel[m_EffPlayData.m_MidCount++].m_TexName,token);
	*/	

	}
	else if (!strcmp(tok,"V")) { // mid effect value
	
		token = strtok(NULL," \t\n");
		m_EffPlayData.m_MidEffNum = atoi(token);
		token = strtok(NULL," \t\n");
		m_EffPlayData.m_StartMidNextEff = atoi(token);
	}
	else if (!strcmp(tok,"E")) { // end effect
	
		token = strtok(NULL," \t\n");
		for (i=0;i<m_EffNum;i++) {
			if (!strcmp(token,m_EffList[i].m_EffName)) {
				index = i;
				break;
			}

		}
		if (index != ESEMPTY) {
			m_EffPlayData.m_EndEffId[m_EffPlayData.m_EndCount++] = index;
		}
			token = strtok(NULL," \t\n");
		
		if (token) { // position
			m_EffPlayData.m_EndPos[0] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffPlayData.m_EndPos[1] = atof(token);
			token = strtok(NULL," \t\n");
			m_EffPlayData.m_EndPos[2] = atof(token);
		}

	}
	//start_count = end_count = 0;

}

void CEffScript ::GetSlide(char *tok) { // extension6 : 스킬 사용시 캐릭 미끌어짐 관련 
	char *token = NULL;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int slide_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_Ext6ValueNum = atoi(token);
		slide_num = m_EffList[index].m_Ext6ValueNum;

		if (slide_num > 0) {
			m_EffList[index].m_Ext6Value = new CEffExt6[slide_num];
		}
		for (int j=0;j<slide_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
			// slide startframe, slide speed, slide accel, slide length,slide limitspeed
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_Ext6Value[j].m_SlideStartFrame = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Ext6Value[j].m_SlideSpeed = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Ext6Value[j].m_SlideAccel = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Ext6Value[j].m_SlideLength = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Ext6Value[j].m_LimitSpeed = atof(token);

		}
	}
}

void CEffScript ::GetEffSnap(char *tok) { // character에 effect 붙이기 관련

	char *token = NULL;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int snap_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_SnapNum = atoi(token);
		snap_num = m_EffList[index].m_SnapNum;

		if (snap_num > 0) {
			m_EffList[index].m_Snap = new CEffSnap[snap_num];
		}
		for (int j=0;j<snap_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
			// snap effect start frame
			token = strtok(buf," \n\t");
			m_EffList[index].m_Snap[j].m_SnapStartFrame = atoi(token);
			
			token = strtok(NULL," \n\t");
			//esf name
			strcpy(m_EffList[index].m_Snap[j].m_EffName,token);
			//pivot name
			token = strtok(NULL," \n\t");
			strcpy(m_EffList[index].m_Snap[j].m_PivotName,token);
			
			token = strtok(NULL," \n\t");
			if (token[0] == 'L')
				m_EffList[index].m_Snap[j].m_Loop = 1; // character object rot 적용true
			else 
				m_EffList[index].m_Snap[j].m_Loop = 0; // character object rot 적용false
		}
	}

}
void CEffScript::RenderLightn3() {
	int i,j;
	if (!m_pLightn3->m_bStart || m_pLightn3->m_bEnd)
		return;
	for (i = 0;i < m_pLightn3->m_iMaxLine;i++) {
		CEffLightn *ptr = &(m_pLightn3->m_pLightn[i]);
		for (j=0; j < ptr->m_LnNum; j++) {
			if (ptr->m_Lightn != NULL) {
				
				ptr->m_Lightn[j].Render(ptr->m_BlendMode);
			}
		}
	}
}
void CEffScript::RenderLightn2() {
	int i,j;
	if (!m_pLightn2->m_bStart || m_pLightn2->m_bEnd)
		return;
	for (i = 0;i < m_pLightn2->m_iMaxLine;i++) {
		CEffLightn *ptr = &(m_pLightn2->m_pLightn[i]);
		for (j=0; j < ptr->m_LnNum; j++) {
			if (ptr->m_Lightn != NULL) {
				
				ptr->m_Lightn[j].Render(ptr->m_BlendMode);
			}
		}
	}
}
void CEffScript::RenderLightn() {
	int i,j;
	for (i=0;i<m_LningNum;i++) {
		if ((m_Lning[i].m_bLightnStart)) {// && (!m_Lning[i].m_bLightnEnd)) {
			if (m_Lning[i].m_Lightn != NULL) {
				for (j=0;j<m_Lning[i].m_LnNum;j++) {
					m_Lning[i].m_Lightn[j].Render(m_Lning[i].m_BlendMode);		
				}
			}
		}
	}	
}
void CEffScript::RenderWheel() {
	int i;
	for (i=0;i<m_WheelNum;i++) {
		m_Wheel[i].m_WheelBody.Render2(m_Wheel[i].m_BlendMode);
	}
}

void CEffScript::PlayWheel()
{ 
	// 궤적 라인 play
	int i;

	CLightning *ptr = NULL;
	m_bWheelStart = true;
	for (i = 0; i < m_WheelNum; ++i) 
	{
		ptr = &(m_Wheel[i].m_WheelBody);
		ptr->SetDevice(m_Device);
		ptr->CreateList(m_Wheel[i].m_PointNum);
		ptr->SetSize(m_Wheel[i].m_PlaneSize);
		
/*
		// insert point
		for (int j=0;j<8;j+=2) 
		{
			D3DXVECTOR3 tmp_pos;
			int insert_pos = m_Wheel[i].m_WheelBody.m_InsertCount;
			if (insert_pos >= 2) 
			{
				D3DXVec3CatmullRom(&tmp_pos,
									&(m_Wheel[i].m_WheelBody.m_PointList[insert_pos-2]),
									&(m_Wheel[i].m_WheelBody.m_PointList[insert_pos-1]),
									&(m_MidPos),
									&(m_MidPos),(float)j/10.0f);
			}
			else 
			{
				D3DXVec3CatmullRom(&tmp_pos,
									&(m_OldMidPos),
									&(m_OldMidPos),
									&(m_MidPos),
									&(m_MidPos),(float)j/10.0f);
			}
		
			m_Wheel[i].m_WheelBody.InsertEmptyPoint(tmp_pos.x,tmp_pos.y,tmp_pos.z);
	
		}
*/	
		ptr->InsertEmptyPoint(m_MidPos.x,m_MidPos.y,m_MidPos.z);

		ptr->SetPlaneValue(m_Wheel[i].m_PlaneValue);
		ptr->CreateTexture(1);
		ptr->SetTexture(0,m_Wheel[i].m_TexName);
		ptr->SetVertNum(1);
		ptr->SetColor(D3DCOLOR_ARGB(255,255,255,255));
		ptr->SetUnitUv(m_Wheel[i].m_UnitUv);
		ptr->CreateVertexBuffer();
	}
}

void CEffScript::PlayWeaponLine() 
{
}

// 검기를 이 스크립트가 제어하기 전 상태로 되돌림
void CEffScript::DeleteWeaponLine() 
{							
}

void CEffScript::DeleteWeaponEff() 
{
	int i;

	if (m_WeaponEff != NULL) 
	{
#ifndef __EFF_WCREATOR__	// client 용
        if ( m_pPlayer && m_ulPlayerID != 0xFFFFFFFF )
        {
            if ( RYLCreatureManager::Instance()->GetCreature( m_ulPlayerID ) == m_pPlayer )
            {
                for( i = 0 ; i < m_WeaponEffNum ; ++i )
                {
                    m_pPlayer->GetEffect()->DeleteWeaponEffect( m_WeaponEff[ i ].m_EffName, m_WeaponEff[ i ].m_EffNum );
                }
            }
        }
#else						// world creator 용
        if ( m_pPlayer )
        {
            for( i = 0 ; i < m_WeaponEffNum ; ++i )
            {
                m_pPlayer->GetEffect()->DeleteWeaponEffect( m_WeaponEff[ i ].m_EffName, m_WeaponEff[ i ].m_EffNum );
            }
        }
#endif //__EFF_WCREATOR__
	}
}
void CEffScript::PlayWeaponEff() 
{
	int i;

#ifndef __EFF_WCREATOR__ //client 용
        if ( m_pPlayer && m_ulPlayerID != 0xFFFFFFFF )
        {
            if ( RYLCreatureManager::Instance()->GetCreature( m_ulPlayerID ) == m_pPlayer )
            {
                for( i = 0 ; i < m_WeaponEffNum ; ++i )
                {
                    m_pPlayer->GetEffect()->AddWeaponEffect( m_WeaponEff[ i ].m_EffName, m_WeaponEff[ i ].m_EffNum, m_bVisibility );
                }
            }
        }
#else //world creator 용
        if ( m_pPlayer )
        {
            for( i = 0 ; i < m_WeaponEffNum ; ++i )
            {
                m_pPlayer->GetEffect()->AddWeaponEffect( m_WeaponEff[ i ].m_EffName, m_WeaponEff[ i ].m_EffNum, m_bVisibility );
            }
        }
#endif		
}

void CEffScript::PlayLning3(int index,int obj_index,float currentframe,D3DXVECTOR3 vecCharEff) 
{
	int i, j;
	CLightning *ptr = NULL;

	if (!m_bLightn3 || (m_pLightn3 == NULL))
		return;

	// Dummy pos Get
	D3DXVECTOR3 vecDumyPos[2];
	
	CZ3DObject *body = NULL;
	D3DXVECTOR3 bodypos;
	D3DXMATRIX matQuatRot;
	D3DXMATRIX matResult;
	D3DXMATRIX matTmp;
	if (m_StartChr != NULL) 
	{
#ifndef __EFF_WCREATOR__		// Client
        if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
		{
			if (m_pLightn3 != NULL) 
			{
				m_pLightn3->m_bEnd = true;
				if (m_pLightn3->m_pLightn != NULL) 
				{
					for (i = 0;i < m_pLightn3->m_iMaxLine;i++)
					{
						if (m_pLightn3->m_pLightn[i].m_Lightn != NULL) 
						{
							delete[] m_pLightn3->m_pLightn[i].m_Lightn;
							m_pLightn3->m_pLightn[i].m_Lightn = NULL;
						}
					}
				}
			}

			return; 
		}
#endif
		// Pivot Info Get 
		for ( i = 0; i < 2; i++ )
		{
			vecDumyPos[i] = m_StartPos;
			
			body = m_StartChr->GetSkeletonPartObject(m_pLightn3->m_pEffChar[i].m_PivotName);
			if (body != NULL)
			{
				vector3 body_tm;
				quaternion body_q;
				D3DXQUATERNION tmp;
				float body_scale = 0.0f;
				body_scale = m_StartChr->GetScale();
				 
				body->GetDirection(body_q);			// Direction
				body->GetPosition(body_tm);			// Position
				
				tmp.x = body_q.x;
				tmp.y = body_q.y;
				tmp.z = body_q.z;
				tmp.w = body_q.w;

				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = m_pLightn3->m_pEffChar[i].vecPos.x;
				matTmp._42 = m_pLightn3->m_pEffChar[i].vecPos.y;
				matTmp._43 = m_pLightn3->m_pEffChar[i].vecPos.z;
				// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
				D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
				D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
				// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
				tmp = m_pLightn3->m_pEffChar[i].vecQuat * tmp;									

				body_q.x = tmp.x;
				body_q.y = tmp.y;
				body_q.z = tmp.z;
				body_q.w = tmp.w;

				bodypos.x = body_tm.x + matResult._41;
				bodypos.y = body_tm.y + matResult._42;
				bodypos.z = body_tm.z + matResult._43;
			
				bodypos *= body_scale;
				
				vecDumyPos[i] = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
								   m_StartPos.y + bodypos.y,
								   m_StartPos.z + bodypos.z);
			}
			
		}
	}
	//////
	if (!m_pLightn3->m_bStart) {
		if ((m_pLightn3->m_iStartEff == obj_index) && (m_pLightn3->m_iStartFrame <= (int)currentframe)) {
			m_pLightn3->m_bStart = true;
			m_pLightn3->m_StartTime = timeGetTime();
			for (i = 0 ; i< m_pLightn3->m_iMaxLine; i++) {
				for (j = 0;j < m_pLightn3->m_pLightn[i].m_LnNum;j++) {
					ptr = &(m_pLightn3->m_pLightn[i].m_Lightn[j]);
					// Blend Mode 
					m_pLightn3->m_pLightn[i].m_BlendMode = m_pLightn3->m_iBlend;

					if (ptr != NULL) { // clight setting
						ptr->SetDevice(m_Device);
						ptr->CreateList(m_pLightn3->m_pLightn[i].m_LPointNum);
						ptr->SetSize((rand() % (int)m_pLightn3->m_pLightn[i].m_PlaneSize) + 0.01f);
						ptr->SetShakeValue(m_pLightn3->m_pLightn[i].m_ShakeMin,m_pLightn3->m_pLightn[i].m_ShakeMax);
						
						ptr->SetStartPoint(vecDumyPos[0].x,vecDumyPos[0].y,vecDumyPos[0].z);
						// endpos setting
						
						ptr->SetEndPoint(vecDumyPos[1].x,vecDumyPos[1].y,vecDumyPos[1].z);
						
						ptr->SetRandPoint();
						ptr->SetPlaneValue(m_pLightn3->m_pLightn[i].m_PlaneValue);

						ptr->CreateTexture(1);
						ptr->SetTexture(0,m_pLightn3->m_pLightn[i].m_TexName);
						ptr->SetVertNum(1);
						ptr->SetColor(m_pLightn3->m_pLightn[i].m_Color);
						ptr->CreateVertexBuffer();
						ptr->SetUnitUv(m_pLightn3->m_pLightn[i].m_UnitUv);
				
					}
				}
			}
		}
	}//if (!m_pLightn2->m_bStart) {
	else { // 이미 lightning이 시작된 이후라면..
		//중단 프레임에 도달 했을때 중단
		if ((m_pLightn3->m_iEndEff == obj_index) && (m_pLightn3->m_iEndFrame <= (int)currentframe)) {
			m_pLightn3->m_bEnd = true;
			for (i = 0;i < m_pLightn3->m_iMaxLine;i++) {
				if (m_pLightn3->m_pLightn[i].m_Lightn != NULL) {
					delete[] m_pLightn3->m_pLightn[i].m_Lightn;
					m_pLightn3->m_pLightn[i].m_Lightn = NULL;

				}
			}

		}
		else { //play (라이트닝 업데이트)
			
			UpdateLning3(vecCharEff);
		}
	}

}
void CEffScript::PlayLning2(int index,int obj_index,float currentframe,D3DXVECTOR3 vecCharEff) {
	int i,j;
	CLightning *ptr = NULL;

	if (!m_bLightn2 || (m_pLightn2 == NULL))
		return;
	if (!m_pLightn2->m_bStart) {
		if ((m_pLightn2->m_iStartEff == obj_index) && (m_pLightn2->m_iStartFrame <= (int)currentframe)) {
			m_pLightn2->m_bStart = true;
			m_pLightn2->m_StartTime = timeGetTime();
			for (i = 0 ; i< m_pLightn2->m_iMaxLine; i++) {
				for (j = 0;j < m_pLightn2->m_pLightn[i].m_LnNum;j++) {
					ptr = &(m_pLightn2->m_pLightn[i].m_Lightn[j]);
					// Blend Mode 
					m_pLightn2->m_pLightn[i].m_BlendMode = m_pLightn2->m_iBlend;

					if (ptr != NULL) { // clight setting
						ptr->SetDevice(m_Device);
						ptr->CreateList(m_pLightn2->m_pLightn[i].m_LPointNum);
						ptr->SetSize((rand() % (int)m_pLightn2->m_pLightn[i].m_PlaneSize) + 0.01f);
						ptr->SetShakeValue(m_pLightn2->m_pLightn[i].m_ShakeMin,m_pLightn2->m_pLightn[i].m_ShakeMax);
						
						///	
						D3DXVECTOR3 tmprot;
						D3DXVECTOR3 tmpfwd = m_ChrFwd;

						tmpfwd *= 1000000.0f;
						tmpfwd.y = 0.0f;
						
						SetRot(m_StartPos,m_StartPos+tmpfwd,tmprot);
						
						D3DXMATRIX tmp_rotm;
						D3DXMATRIX tmp_unitm;
						D3DXMATRIX tmp_roted;

						D3DXMatrixIdentity(&tmp_unitm);
						tmp_unitm._41 = m_pLightn2->m_vecPos.x;
						tmp_unitm._42 = m_pLightn2->m_vecPos.y;
						tmp_unitm._43 = m_pLightn2->m_vecPos.z;

						D3DXMatrixRotationYawPitchRoll(&tmp_rotm,tmprot.y,tmprot.x,tmprot.z);
						D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&tmp_rotm);
						if (m_bEffCharacter)	// Character Effect 경우
							ptr->SetStartPoint(vecCharEff.x,vecCharEff.y,vecCharEff.z);
						else
							ptr->SetStartPoint(m_StartPos.x + tmp_roted._41,m_StartPos.y + tmp_roted._42,m_StartPos.z + tmp_roted._43);
						// endpos setting
						D3DXMATRIX matRot;
						D3DXVECTOR3 tmp_end;
						tmpfwd = m_ChrFwd;
						D3DXVec3Normalize(&tmpfwd,&tmpfwd);
						D3DXMatrixRotationYawPitchRoll(&matRot,D3DXToRadian( rand() % 360),
																D3DXToRadian( rand() % 360),
																D3DXToRadian( rand() % 360));
			
						D3DXMatrixIdentity(&tmp_unitm);
						tmp_unitm._41 = tmpfwd.x;
						tmp_unitm._42 = tmpfwd.y;
						tmp_unitm._43 = tmpfwd.z;

						D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&matRot);
						tmp_end = D3DXVECTOR3(tmp_roted._41,tmp_roted._42,tmp_roted._43);
						D3DXVec3Normalize(&tmp_end,&tmp_end);
						int tmpRange = m_pLightn2->m_fRange * 100.0f;
						tmp_end *= ((rand() % tmpRange) / 100.0f + 0.01f);
						tmp_end += (ptr->m_PointList[0]);
						
						if ( m_pLightn2->m_pLightn[i].m_RandSize > 0.0f) { // rand value setting

							float rand_size =  m_pLightn2->m_pLightn[i].m_RandSize;
							D3DXVECTOR3 rand_value;

							rand_value.x = (float)(rand() % (int)(rand_size * 1000.0f))/ 1000.0f;
							if (rand() %2) {
								rand_value.x *= -1.0f;
							}
							rand_value.y = (float)(rand() % (int)(rand_size * 1000.0f))/ 1000.0f;
							if (rand() %2) {
								rand_value.y *= -1.0f;
							}
							rand_value.z = (float)(rand() % (int)(rand_size * 1000.0f))/ 1000.0f;
							if (rand() %2) {
								rand_value.z *= -1.0f;
							}
							tmp_end += rand_value;

						}

						ptr->SetEndPoint(tmp_end.x,tmp_end.y,tmp_end.z);
						
						ptr->SetRandPoint();
						ptr->SetPlaneValue(m_pLightn2->m_pLightn[i].m_PlaneValue);

						ptr->CreateTexture(1);
						ptr->SetTexture(0,m_pLightn2->m_pLightn[i].m_TexName);
						ptr->SetVertNum(1);
						ptr->SetColor(m_pLightn2->m_pLightn[i].m_Color);
						ptr->CreateVertexBuffer();
						ptr->SetUnitUv(m_pLightn2->m_pLightn[i].m_UnitUv);
				
					}
				}
			}
		}
	}//if (!m_pLightn2->m_bStart) {
	else { // 이미 lightning이 시작된 이후라면..
		//중단 프레임에 도달 했을때 중단
		if ((m_pLightn2->m_iEndEff == obj_index) && (m_pLightn2->m_iEndFrame <= (int)currentframe)) {
			m_pLightn2->m_bEnd = true;
			for (i = 0;i < m_pLightn2->m_iMaxLine;i++) {
				if (m_pLightn2->m_pLightn[i].m_Lightn != NULL) {
					delete[] m_pLightn2->m_pLightn[i].m_Lightn;
					m_pLightn2->m_pLightn[i].m_Lightn = NULL;

				}
			}

		}
		else { //play (라이트닝 업데이트)
			
			UpdateLning2(vecCharEff);
		}
	}

}
void CEffScript::PlayLning(int index,int obj_index,float currentframe,D3DXVECTOR3 vecCharEff) {
	int i;
	CLightning *ptr = NULL;

	if (!(m_Lning[index].m_bLightnStart) && m_StartPosSet && (m_EndPosNum > 0)) { //lightning not start  
		if ((m_Lning[index].m_StartEffIndex == obj_index) && (m_Lning[index].m_StartFrame <= (unsigned int)currentframe)) {
			
			m_Lning[index].m_bLightnStart = true;

			m_Lning[index].m_StartTime = timeGetTime();
			
			for (i=0;i<m_Lning[index].m_LnNum;i++) {	
				ptr = &(m_Lning[index].m_Lightn[i]);
				if (ptr != NULL) { // clight setting
					ptr->SetDevice(m_Device);
					ptr->CreateList(m_Lning[index].m_LPointNum);
					ptr->SetSize(m_Lning[index].m_PlaneSize);
					ptr->SetShakeValue(m_Lning[index].m_ShakeMin,m_Lning[index].m_ShakeMax);
					///	
					D3DXVECTOR3 tmprot;
					D3DXVECTOR3 tmpfwd = m_ChrFwd;

					tmpfwd *= 1000000.0f;
					tmpfwd.y = 0.0f;
					
					SetRot(m_StartPos,m_StartPos+tmpfwd,tmprot);
					
					D3DXMATRIX tmp_rotm;
					D3DXMATRIX tmp_unitm;
					D3DXMATRIX tmp_roted;

					D3DXMatrixIdentity(&tmp_unitm);
					
					tmp_unitm._41 = m_Lning[index].m_Pos.x;
					tmp_unitm._42 = m_Lning[index].m_Pos.y;
					tmp_unitm._43 = m_Lning[index].m_Pos.z;

					D3DXMatrixRotationYawPitchRoll(&tmp_rotm,tmprot.y,tmprot.x,tmprot.z);
					D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&tmp_rotm);
/*
					m_Lning[index].m_Pos.x = tmp_roted._41;
//					m_MidUnit.y = tmp_roted._42;
					m_Lning[index].m_Pos.z = tmp_roted._43;
*/
					if (m_bEffCharacter)	//캐릭터 effect
					{
						ptr->SetStartPoint(vecCharEff.x,vecCharEff.y,vecCharEff.z);
					}
					else
					{
						ptr->SetStartPoint(m_StartPos.x + tmp_roted._41,m_StartPos.y + m_Lning[index].m_Pos.y,m_StartPos.z + tmp_roted._43);
					}
					// endpos setting
					
					// target 의 chest 얻기
					float tmp_Chest = 0.0f;

					CZ3DObject *body;
					if (m_EndChr != NULL) 
					{
#ifndef __EFF_WCREATOR__		// Client
                        if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
						{
							tmp_Chest = 100.0f;
						}
						else 
						{
							float body_scale = 0.0f;
							body = m_EndChr->GetSkeletonPartObject("CHEST");
							body_scale = m_EndChr->GetScale();
							
							if (body != NULL) 
							{
								matrix *chest = body->GetTM();
								if (chest != NULL) 
								{
									//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
									tmp_Chest = chest->_42; //무조건 chest 높이로 세팅
									tmp_Chest *= body_scale;
								}
								else 
								{ // chest 가 없을시
									tmp_Chest = 150.0f;
									tmp_Chest *= body_scale;
								}
							}
						}
#else
						float body_scale = 0.0f;
						body = m_EndChr->GetSkeletonPartObject("CHEST");
						body_scale = m_EndChr->GetScale();
						
						if (body != NULL) 
						{
							matrix *chest = body->GetTM();
							if (chest != NULL) 
							{
								//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
								tmp_Chest = chest->_42; //무조건 chest 높이로 세팅
								tmp_Chest *= body_scale;
							}
							else 
							{ 
								// chest 가 없을시
								tmp_Chest = 150.0f;
								tmp_Chest *= body_scale;
							}
						}
#endif
					}
					else 
					{
						tmp_Chest = 100.0f;
					}

					D3DXVECTOR3 tmp_end = D3DXVECTOR3(m_EndPos[0].x,m_EndPos[0].y + tmp_Chest,m_EndPos[0].z);
					
					if (m_Lning[index].m_RandSize > 0.0f) 
					{ 
						// rand value setting
						float rand_size = m_Lning[index].m_RandSize;
						D3DXVECTOR3 rand_value;

						rand_value.x = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) 
						{
							rand_value.x *= -1.0f;
						}
						rand_value.y = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) 
						{
							rand_value.y *= -1.0f;
						}
						rand_value.z = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) 
						{
							rand_value.z *= -1.0f;
						}
						tmp_end += rand_value;
					}

					ptr->SetEndPoint(tmp_end.x,tmp_end.y,tmp_end.z);
					ptr->SetRandPoint();
					ptr->SetPlaneValue(m_Lning[index].m_PlaneValue);

					ptr->CreateTexture(1);
					ptr->SetTexture(0,m_Lning[index].m_TexName);
					ptr->SetVertNum(1);
					ptr->SetColor(m_Lning[index].m_Color);
					ptr->CreateVertexBuffer();
					ptr->SetUnitUv(m_Lning[index].m_UnitUv);
				}
			}
			
		}
	}
	else 
	{ 
		// 이미 lightning이 시작된 이후라면 중단 프레임에 도달 했을때 중단
		if ((m_Lning[index].m_EndEffIndex == obj_index) && 
			(m_Lning[index].m_EndFrame <= static_cast<unsigned int>(currentframe))) 
		{
			m_Lning[index].m_bLightnEnd = true;	
			if (m_Lning[index].m_Lightn != NULL) 
			{
				delete[] m_Lning[index].m_Lightn;
				m_Lning[index].m_Lightn = NULL;
			}
		}
		else 
		{ 
			// play (라이트닝 업데이트)
			UpdateLning(index,vecCharEff);
		}
	}
}

void CEffScript::UpdateLning3(D3DXVECTOR3 vecCharEff) 
{
	int i, j;
	int iUpdateFrame = m_pLightn3->m_pLightn[0].m_UpdateFrame;

	unsigned long tmp = timeGetTime();
	// Dummy pos Get
	D3DXVECTOR3 vecDumyPos[2];
	
	CZ3DObject *body = NULL;
	D3DXVECTOR3 bodypos;
	D3DXMATRIX matQuatRot;
	D3DXMATRIX matResult;
	D3DXMATRIX matTmp;
	if (m_StartChr != NULL) 
	{
		// Pivot Info Get 
		for ( i = 0; i < 2; i++ )
		{
			vecDumyPos[i] = m_StartPos;
			
			body = m_StartChr->GetSkeletonPartObject(m_pLightn3->m_pEffChar[i].m_PivotName);
			if (body != NULL)
			{
				
				vector3 body_tm;
				quaternion body_q;
				D3DXQUATERNION tmp;
				float body_scale = 0.0f;
				body_scale = m_StartChr->GetScale();
				 
				body->GetDirection(body_q);			// Direction
				body->GetPosition(body_tm);			// Position
				
				tmp.x = body_q.x;
				tmp.y = body_q.y;
				tmp.z = body_q.z;
				tmp.w = body_q.w;

				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = m_pLightn3->m_pEffChar[i].vecPos.x;
				matTmp._42 = m_pLightn3->m_pEffChar[i].vecPos.y;
				matTmp._43 = m_pLightn3->m_pEffChar[i].vecPos.z;
				// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
				D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
				D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
				// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
				tmp = m_pLightn3->m_pEffChar[i].vecQuat * tmp;									

				body_q.x = tmp.x;
				body_q.y = tmp.y;
				body_q.z = tmp.z;
				body_q.w = tmp.w;

				bodypos.x = body_tm.x + matResult._41;
				bodypos.y = body_tm.y + matResult._42;
				bodypos.z = body_tm.z + matResult._43;
			
				bodypos *= body_scale;
				
				vecDumyPos[i] = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
								   m_StartPos.y + bodypos.y,
								   m_StartPos.z + bodypos.z);
			}
			
		}
	}
	//////

	if (1 || ((tmp - m_pLightn3->m_StartTime)/DEFAULTESTICK) >= (unsigned long)iUpdateFrame) {
		m_pLightn3->m_StartTime = tmp;
		
		for (i=0;i<m_pLightn3->m_iMaxLine;i++) {
			CEffLightn *pTmp = &(m_pLightn3->m_pLightn[i]);
			for (j = 0;j < pTmp->m_LnNum;j++) { 
				if ((pTmp->m_Lightn) != NULL)  {
					
					pTmp->m_Lightn[j].SetStartPoint(vecDumyPos[0].x,vecDumyPos[0].y,vecDumyPos[0].z);
			
					pTmp->m_Lightn[j].SetEndPoint(vecDumyPos[1].x,vecDumyPos[1].y,vecDumyPos[1].z);


					D3DXVECTOR3 dermi;
					pTmp->m_Lightn[j].UpdateLightning(dermi,false);

				}
			}
		}
	}

}
void CEffScript::UpdateLning2(D3DXVECTOR3 vecCharEff) {
	int i,j;
	int iUpdateFrame = m_pLightn2->m_pLightn[0].m_UpdateFrame;

	unsigned long tmp = timeGetTime();
	if (1 || ((tmp - m_pLightn2->m_StartTime)/DEFAULTESTICK) >= (unsigned long)iUpdateFrame) {
		m_pLightn2->m_StartTime = tmp;
		
		for (i=0;i<m_pLightn2->m_iMaxLine;i++) {
			CEffLightn *pTmp = &(m_pLightn2->m_pLightn[i]);
			for (j = 0;j < pTmp->m_LnNum;j++) { 
				if ((pTmp->m_Lightn) != NULL)  {
					
					D3DXVECTOR3 tmp_end = D3DXVECTOR3(0.0f,0.0f,0.0f);
					// endpos setting
					D3DXMATRIX matRot;
					
					D3DXVECTOR3 tmpfwd;
					D3DXMATRIX tmp_unitm;
					D3DXMATRIX tmp_roted;

					tmpfwd = m_ChrFwd;
					D3DXVec3Normalize(&tmpfwd,&tmpfwd);
					D3DXMatrixRotationYawPitchRoll(&matRot,D3DXToRadian( rand() % 360),
															D3DXToRadian( rand() % 360),
															D3DXToRadian( rand() % 360));
		
					D3DXMatrixIdentity(&tmp_unitm);
					tmp_unitm._41 = tmpfwd.x;
					tmp_unitm._42 = tmpfwd.y;
					tmp_unitm._43 = tmpfwd.z;

					D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&matRot);
					//스타트 위치 업데이트
					if (m_bEffCharacter)
						pTmp->m_Lightn[j].SetStartPoint(vecCharEff.x,vecCharEff.y,vecCharEff.z);
					else
						pTmp->m_Lightn[j].SetStartPoint(m_StartPos.x + tmp_roted._41,m_StartPos.y + tmp_roted._42,m_StartPos.z + tmp_roted._43);

					tmp_end = D3DXVECTOR3(tmp_roted._41,tmp_roted._42,tmp_roted._43);
					D3DXVec3Normalize(&tmp_end,&tmp_end);
					int tmpRange = m_pLightn2->m_fRange * 100.0f;
					tmp_end *= ((rand() % tmpRange) / 100.0f + 0.01f);
					tmp_end += (pTmp->m_Lightn[j].m_PointList[0]);
					
					if ( m_pLightn2->m_pLightn[i].m_RandSize > 0.0f) { // rand value setting

						float rand_size =  m_pLightn2->m_pLightn[i].m_RandSize;
						D3DXVECTOR3 rand_value;

						rand_value.x = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) {
							rand_value.x *= -1.0f;
						}
						rand_value.y = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) {
							rand_value.y *= -1.0f;
						}
						rand_value.z = (float)(rand() % (int)(rand_size * 1000.0f) + 0.1f)/ 1000.0f;
						if (rand() %2) {
							rand_value.z *= -1.0f;
						}
						tmp_end += rand_value;

					}
					//tmp_end += pTmp->m_Lightn[j].m_PointList[(pTmp->m_Lightn[j].m_PointNum - 1)];
					pTmp->m_Lightn[j].SetEndPoint(tmp_end.x,tmp_end.y,tmp_end.z);


					D3DXVECTOR3 dermi;
					pTmp->m_Lightn[j].UpdateLightning(dermi,false);

				}
			}
		}
	}
}
void CEffScript::UpdateLning(int index,D3DXVECTOR3 vecCharEff) {
	int i;
	unsigned long tmp = timeGetTime();
	if (((tmp - m_Lning[index].m_StartFrame)/DEFAULTESTICK) >= m_Lning[index].m_UpdateFrame) {
		m_Lning[index].m_StartFrame = tmp;		
		for (i=0;i<m_Lning[index].m_LnNum;i++) {
			if (m_Lning[index].m_Lightn != NULL)  { 
				
				if (m_Lning[index].m_Follow) { // user 이동시 따라오는 번개
					///	상대적 rot 적용 
					D3DXVECTOR3 tmprot;
					D3DXVECTOR3 tmpfwd = m_ChrFwd;

					tmpfwd *= 1000000.0f;
					tmpfwd.y = 0.0f;
					
					SetRot(m_StartPos,m_StartPos+tmpfwd,tmprot);
					
					D3DXMATRIX tmp_rotm;
					D3DXMATRIX tmp_unitm;
					D3DXMATRIX tmp_roted;

					D3DXMatrixIdentity(&tmp_unitm);
					
					tmp_unitm._41 = m_Lning[index].m_Pos.x;
					tmp_unitm._42 = m_Lning[index].m_Pos.y;
					tmp_unitm._43 = m_Lning[index].m_Pos.z;

					D3DXMatrixRotationYawPitchRoll(&tmp_rotm,tmprot.y,tmprot.x,tmprot.z);
					D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&tmp_rotm);
/*
					m_Lning[index].m_Pos.x = tmp_roted._41;
//					m_MidUnit.y = tmp_roted._42;
					m_Lning[index].m_Pos.z = tmp_roted._43;
*/
					if (m_bEffCharacter)
					{
						m_Lning[index].m_Lightn[i].SetStartPoint(vecCharEff.x,vecCharEff.y,vecCharEff.z);
					}
					else
					{
						m_Lning[index].m_Lightn[i].SetStartPoint(
							m_StartPos.x + tmp_roted._41,m_StartPos.y + m_Lning[index].m_Pos.y,m_StartPos.z + tmp_roted._43);
					}
				}

				// end point는 항상 따라간다
				// target 의 chest 얻기
				float tmp_Chest = 0.0f;

				CZ3DObject *body;
				if (m_EndChr != NULL) 
				{
#ifndef __EFF_WCREATOR__		// Client
                    if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
					{
						tmp_Chest = 100.0f;
					}
					else 
					{
						float body_scale = 0.0f;
						body = m_EndChr->GetSkeletonPartObject("CHEST");
						body_scale = m_EndChr->GetScale();
						
						if (body != NULL) 
						{
							matrix *chest = body->GetTM();
							if (chest != NULL) 
							{
								//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
								tmp_Chest = chest->_42; //무조건 chest 높이로 세팅
								tmp_Chest *= body_scale;
							}
							else 
							{ 
								// chest 가 없을시
								tmp_Chest = 150.0f;
								tmp_Chest *= body_scale;
							}
						}
					}
#else
					float body_scale = 0.0f;
					body = m_EndChr->GetSkeletonPartObject("CHEST");
					body_scale = m_EndChr->GetScale();
					
					if (body != NULL) 
					{
						matrix *chest = body->GetTM();
						if (chest != NULL)
						{
							//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
							tmp_Chest = chest->_42; //무조건 chest 높이로 세팅
							tmp_Chest *= body_scale;
						}
						else 
						{ 
							// chest 가 없을시
							tmp_Chest = 150.0f;
							tmp_Chest *= body_scale;
						}
					}
#endif
				}
				else 
				{
					tmp_Chest = 100.0f;
				}

				m_Lning[index].m_Lightn[i].SetEndPoint(m_EndPos[0].x, m_EndPos[0].y + tmp_Chest,m_EndPos[0].z);

				// update lightning
				D3DXVECTOR3 dermi;

				m_Lning[index].m_Lightn[i].UpdateLightning(dermi, false);
			}
		}
	}
}
// nx,ny,nz 는 다음mid 점의 예상좌표 
void CEffScript::UpdateWheel(float x, float y, float z, float nx, float ny, float nz, int value) 
{ 
	// 궤적 point update
	int i;
	if (value == 0) 
	{
		for (i = 0; i < m_WheelNum; ++i) 
		{	
			// insert point
			D3DXVECTOR3 pos = D3DXVECTOR3(x,y,z);
			D3DXVECTOR3 npos = D3DXVECTOR3(nx,ny,nz);

			// 이번점이 들어갈 위치
			int insert_pos = m_Wheel[i].m_WheelBody.m_InsertCount;
			
			if (insert_pos == 0) 
			{	
				// List에 아무점도 없을때
				m_Wheel[i].m_WheelBody.InsertEmptyPoint(x,y,z);	
			}
			else 
			{					
				// List에 점이 들어있다면
				D3DXVECTOR3 interpol = D3DXVECTOR3(0.0f,0.0f,0.0f);
				D3DXVECTOR3 bp1;
				if (insert_pos == 1)
					bp1 = m_Wheel[i].m_WheelBody.m_PointList[insert_pos-1];
				else 
					bp1 = m_Wheel[i].m_WheelBody.m_PointList[insert_pos-2];

				D3DXVECTOR3 bp2 = m_Wheel[i].m_WheelBody.m_PointList[insert_pos-1];
				D3DXVECTOR3 np1 = pos;
				D3DXVECTOR3 np2 = npos;

				for (int j=0;j<20;j++) {

					D3DXVec3CatmullRom(&interpol,
											&bp1,
											&bp2,
											&np1,
											&np2,
											(float)(j+1)/20.0f);
					m_Wheel[i].m_WheelBody.InsertEmptyPoint(interpol.x,interpol.y,interpol.z);

				}
			}
		
			
			//
			m_Wheel[i].m_WheelBody.SetVertexBuffer(0,1);
		}
	}
	else { //목적지 까지 도착했을때 라인 줄여주기
		for (i=0;i<m_WheelNum;i++) { 
			CLightning *ptr = &(m_Wheel[i].m_WheelBody);
			if (ptr != NULL) //end setting
				ptr->SetEffectEnd(true,(int)m_Wheel[i].m_UnitUv);
		}
	}
}

void CEffScript::GetEffBezier(char *tok) {			// Bezier Middle data load
	char *token = NULL;

	m_bBezierMid = true;
	m_pBezierMid = new CEffBezier;

	token = strtok(NULL," \n\t");
	strcpy(m_pBezierMid->m_strTexture,token);				// Texture Name
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fUv = atof(token);						// Unit Uv
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fPlaneSize = atof(token);				// Plane Size
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fFadeOut = atof(token);					// Unit Fade Out
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fIncreaseSpeed = atof(token);			// 초기 속도(비율) ex> 0.1f
			
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fIncreaseAccel = atof(token);			// 가속도(비율)	ex> 0.04f 
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_nBezierPoint = atoi(token);				// 총 제어점 갯수
	
	m_pBezierMid->m_nBezierPoint = 30;				// 총 제어점 갯수
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fLimitLength = atof(token);				// Limit Length
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_iEndSpeed = atoi(token);				// 꼬리 속도 

	token = strtok(NULL," \n\t");
	m_pBezierMid->m_iEndAccel = atoi(token);				// 꼬리 가속도
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_iUvMode = atoi(token);					// Uv Mode (0 or 1)

	token = strtok(NULL," \n\t");
	m_pBezierMid->m_iAxis = atoi(token);

	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fBaseLength = atof(token);				// 제어점 의 위치(비율)
	
	token = strtok(NULL," \n\t");
	m_pBezierMid->m_fBaseHeight = atof(token);				// 제어점의 높이

}
void CEffScript::GetEffWeapon(char *tok) {
	int i;
	char *token = NULL;
	m_WeaponEff = new CEffWeapon[m_WeaponEffNum];
	m_bWeaponEff = true;
	for ( i=0; i < m_WeaponEffNum; i++) {
		char buf[ESBUF_SIZE] = {0};
		// texture name
		if (i != 0) {
			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);		
			token = strtok(buf," \n\t");
			strcpy(m_WeaponEff[i].m_EffName,token);
		}
		else {
			strcpy(m_WeaponEff[i].m_EffName,tok);	
		}
		token = strtok(NULL," \n\t");
		m_WeaponEff[i].m_EffNum = atoi(token);

	}
}
void CEffScript::GetEffWheel(char *tok) {
	int i;
	char *token = NULL;
	int index = ESEMPTY;
	m_Wheel = new CEffWheel[m_WheelNum];
	m_bWheel =  true;
	for (i=0;i<m_WheelNum;i++) {
		char buf[ESBUF_SIZE] = {0};
		// texture name
		if (i != 0) {
			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);		
			token = strtok(buf," \n\t");
			strcpy(m_Wheel[i].m_TexName,token);
		}
		else {
			strcpy(m_Wheel[i].m_TexName,tok);
		}
		//plane size
		token = strtok(NULL," \n\t");
		m_Wheel[i].m_PlaneSize = atof(token);
		// point num
		token = strtok(NULL," \n\t");
		m_Wheel[i].m_PointNum = atoi(token);
		// 줄어드는 라인 unit
		token = strtok(NULL," \n\t");
		m_Wheel[i].m_UnitUv = atof(token);
		//blend mode
		token = strtok(NULL," \n\t");
		m_Wheel[i].m_BlendMode = atoi(token);
		//plane value
		token = strtok(NULL," \n\t");
		m_Wheel[i].m_PlaneValue = atoi(token);

	}
}
void CEffScript::GetBinLightn2() {

	int i;

	int iEndFrame = m_pLightn2->m_iEndFrame;
	int iStartFrame = m_pLightn2->m_iStartFrame;
	int iEndIndex = m_pLightn2->m_iEndEff;
	int iStartIndex = m_pLightn2->m_iStartEff;

	for (i=0;i< (m_pLightn2->m_iMaxLine); i++) {
	
		//start eff setting
		strcpy((m_pLightn2->m_pLightn[i].m_TexName),(m_pLightn2->m_pTextureName[i].m_strName));
		m_pLightn2->m_pLightn[i].m_Lightn = new CLightning[1];
		m_pLightn2->m_pLightn[i].m_EndEffIndex = iEndIndex;
		m_pLightn2->m_pLightn[i].m_EndFrame = iEndFrame;
		m_pLightn2->m_pLightn[i].m_StartEffIndex = iStartIndex;
		m_pLightn2->m_pLightn[i].m_StartFrame = iStartFrame;
		m_pLightn2->m_pLightn[i].m_Follow = true;
		m_pLightn2->m_pLightn[i].m_LnNum = 1;
		m_pLightn2->m_pLightn[i].m_LPointNum  = 4;
		// Plane Size
		float fPlaneRange = m_pLightn2->m_fMaxSize - m_pLightn2->m_fMinSize;
		int iPlaneRange = (int)(fPlaneRange * 10000.0f);
		
		m_pLightn2->m_pLightn[i].m_PlaneSize = (rand() % iPlaneRange) / 10000.0f + m_pLightn2->m_fMinSize;
		m_pLightn2->m_pLightn[i].m_PlaneValue = 0;
		m_pLightn2->m_pLightn[i].m_RandSize = (rand() % 800) + 1;
		m_pLightn2->m_pLightn[i].m_ShakeMax = 20.0f;
		m_pLightn2->m_pLightn[i].m_ShakeMin = -20.0f;
		// Blend
		m_pLightn2->m_pLightn[i].m_BlendMode = (m_pLightn2->m_iBlend);
		int iUnitUv = (m_pLightn2->m_fUvAniMax * 10000.0f);
		if (iUnitUv > 0)
			m_pLightn2->m_pLightn[i].m_UnitUv = (rand() % iUnitUv) / 10000.0f + 0.01f;
		else
			m_pLightn2->m_pLightn[i].m_UnitUv = 0.0f;
		m_pLightn2->m_pLightn[i].m_UpdateFrame = 1;

	}
}
void CEffScript::GetLightn3(char *tok) 
{

	int i;
	if (tok == NULL)
		return;

	char *token = tok;
	int index = ESEMPTY;
	char buf[ESBUF_SIZE] = {0};
	
	int iStartIndex = 0;
	int iEndIndex = 0;
	
	int iEndFrame = 0;
	int iStartFrame = 0;


	for (i=0;i < m_EffNum;i++) {
		if (!(strcmp(token,m_EffList[i].m_EffName))) {
			iStartIndex = i;
			break;
		}
	}
	m_pLightn3->m_iStartEff = iStartIndex;			// Start Index
	token = strtok(NULL," \n\t");
	m_pLightn3->m_iStartFrame = atoi(token);		// Start Frame
	token = strtok(NULL," \n\t");
	
	for (i=0;i < m_EffNum;i++) {
		if (!(strcmp(token,m_EffList[i].m_EffName))) {
			iEndIndex = i;
			break;
		}
	}
	m_pLightn3->m_iEndEff = iEndIndex;				// End Index
	token = strtok(NULL," \n\t");
	m_pLightn3->m_iEndFrame = atoi(token);			// End Frame
	
	token = strtok(NULL," \n\t");					// 최소 사이즈
	m_pLightn3->m_fMinSize = atof(token);

	token = strtok(NULL," \n\t");					// 최대 사이즈
	m_pLightn3->m_fMaxSize = atof(token);


	iEndFrame = m_pLightn3->m_iEndFrame;
	iStartFrame = m_pLightn3->m_iStartFrame;
	iEndIndex = m_pLightn3->m_iEndEff;
	iStartIndex = m_pLightn3->m_iStartEff;

	for (i=0;i< (m_pLightn3->m_iMaxLine); i++) {
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		//start eff setting
		token = strtok(buf," \n\t");
		strcpy((m_pLightn3->m_pLightn[i].m_TexName),token);
		strcpy((m_pLightn3->m_pTextureName[i].m_strName),token);

		m_pLightn3->m_pLightn[i].m_Lightn = new CLightning[1];
		m_pLightn3->m_pLightn[i].m_EndEffIndex = iEndIndex;
		m_pLightn3->m_pLightn[i].m_EndFrame = iEndFrame;
		m_pLightn3->m_pLightn[i].m_StartEffIndex = iStartIndex;
		m_pLightn3->m_pLightn[i].m_StartFrame = iStartFrame;
		m_pLightn3->m_pLightn[i].m_Follow = true;
		m_pLightn3->m_pLightn[i].m_LnNum = 1;
		m_pLightn3->m_pLightn[i].m_LPointNum  = 4;
		// Plane Size
		float fPlaneRange = m_pLightn3->m_fMaxSize - m_pLightn3->m_fMinSize;
		int iPlaneRange = (int)(fPlaneRange * 10000.0f);		
		m_pLightn3->m_pLightn[i].m_PlaneSize = (rand() % iPlaneRange) / 10000.0f + m_pLightn3->m_fMinSize;

		m_pLightn3->m_pLightn[i].m_PlaneValue = 0;
		m_pLightn3->m_pLightn[i].m_RandSize = (rand() % 800) + 1;
		m_pLightn3->m_pLightn[i].m_ShakeMax = 20.0f;
		m_pLightn3->m_pLightn[i].m_ShakeMin = -20.0f;
		int iUnitUv = (m_pLightn3->m_fUvAniMax * 10000.0f);
		if (iUnitUv > 0)
			m_pLightn3->m_pLightn[i].m_UnitUv = (rand() % iUnitUv) / 10000.0f + 0.01f;
		else
			m_pLightn3->m_pLightn[i].m_UnitUv = 0.0f;
		m_pLightn3->m_pLightn[i].m_UpdateFrame = 1;

	}
	// Character Effect Setting
	for ( i = 0; i < m_pLightn3->m_iEffChar; i++ ) {
		CEffChar *pEffChar = &(m_pLightn3->m_pEffChar[i]);

		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		// Pivot name get
		token = strtok(buf," \n\t");
		strcpy(pEffChar->m_PivotName,token);
		// Pos
		token = strtok(NULL," \n\t");
		pEffChar->vecPos.x = atof(token);
		token = strtok(NULL," \n\t");
		pEffChar->vecPos.y = atof(token);
		token = strtok(NULL," \n\t");
		pEffChar->vecPos.z = atof(token);
		// Quaternion
		token = strtok(NULL," \n\t");
		pEffChar->vecQuat.x = atof(token);
		token = strtok(NULL," \n\t");
		pEffChar->vecQuat.y = atof(token);
		token = strtok(NULL," \n\t");
		pEffChar->vecQuat.z = atof(token);
		token = strtok(NULL," \n\t");
		pEffChar->vecQuat.w = atof(token);

	}
}
void CEffScript::GetLightn2(char *tok) {
	int i;
	if (tok == NULL)
		return;

	char *token = tok;
	int index = ESEMPTY;
	char buf[ESBUF_SIZE] = {0};
	
	int iStartIndex = 0;
	int iEndIndex = 0;
	
	int iEndFrame = 0;
	int iStartFrame = 0;


	for (i=0;i < m_EffNum;i++) {
		if (!(strcmp(token,m_EffList[i].m_EffName))) {
			iStartIndex = i;
			break;
		}
	}
	m_pLightn2->m_iStartEff = iStartIndex;			// Start Index
	token = strtok(NULL," \n\t");
	m_pLightn2->m_iStartFrame = atoi(token);		// Start Frame
	token = strtok(NULL," \n\t");
	
	for (i=0;i < m_EffNum;i++) {
		if (!(strcmp(token,m_EffList[i].m_EffName))) {
			iEndIndex = i;
			break;
		}
	}
	m_pLightn2->m_iEndEff = iEndIndex;				// End Index
	token = strtok(NULL," \n\t");
	m_pLightn2->m_iEndFrame = atoi(token);			// End Frame
	
	token = strtok(NULL," \n\t");					// 최소 사이즈
	m_pLightn2->m_fMinSize = atof(token);

	token = strtok(NULL," \n\t");					// 최대 사이즈
	m_pLightn2->m_fMaxSize = atof(token);


	iEndFrame = m_pLightn2->m_iEndFrame;
	iStartFrame = m_pLightn2->m_iStartFrame;
	iEndIndex = m_pLightn2->m_iEndEff;
	iStartIndex = m_pLightn2->m_iStartEff;

	for (i=0;i< (m_pLightn2->m_iMaxLine); i++) {
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		//start eff setting
		token = strtok(buf," \n\t");
		strcpy((m_pLightn2->m_pLightn[i].m_TexName),token);
		strcpy((m_pLightn2->m_pTextureName[i].m_strName),token);

		m_pLightn2->m_pLightn[i].m_Lightn = new CLightning[1];
		m_pLightn2->m_pLightn[i].m_EndEffIndex = iEndIndex;
		m_pLightn2->m_pLightn[i].m_EndFrame = iEndFrame;
		m_pLightn2->m_pLightn[i].m_StartEffIndex = iStartIndex;
		m_pLightn2->m_pLightn[i].m_StartFrame = iStartFrame;
		m_pLightn2->m_pLightn[i].m_Follow = true;
		m_pLightn2->m_pLightn[i].m_LnNum = 1;
		m_pLightn2->m_pLightn[i].m_LPointNum  = 4;
		// Plane Size
		float fPlaneRange = m_pLightn2->m_fMaxSize - m_pLightn2->m_fMinSize;
		int iPlaneRange = (int)(fPlaneRange * 10000.0f);		
		m_pLightn2->m_pLightn[i].m_PlaneSize = (rand() % iPlaneRange) / 10000.0f + m_pLightn2->m_fMinSize;

		m_pLightn2->m_pLightn[i].m_PlaneValue = 0;
		m_pLightn2->m_pLightn[i].m_RandSize = (rand() % 800) + 1;
		m_pLightn2->m_pLightn[i].m_ShakeMax = 20.0f;
		m_pLightn2->m_pLightn[i].m_ShakeMin = -20.0f;
		int iUnitUv = (m_pLightn2->m_fUvAniMax * 10000.0f);
		if (iUnitUv > 0)
			m_pLightn2->m_pLightn[i].m_UnitUv = (rand() % iUnitUv) / 10000.0f + 0.01f;
		else
			m_pLightn2->m_pLightn[i].m_UnitUv = 0.0f;
		m_pLightn2->m_pLightn[i].m_UpdateFrame = 1;

	}
		
}
void CEffScript::GetLightn(char *tok) {
	int i,j;
	char *token = NULL;
	int index = ESEMPTY;

	m_Lning = new CEffLightn[m_LningNum];
	m_bLightning = true;
	for (i=0;i<m_LningNum;i++) {
		char buf[ESBUF_SIZE] = {0};
		
		if (i!=0) {
			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
			//start eff setting
			token = strtok(buf," \n\t");
		}
		//texture name
		token = strtok(NULL," \n\t");
		strcpy(m_Lning[i].m_TexName,token);
		//uv ani시 한번에 이동하는 단위
		token = strtok(NULL," \n\t");
		m_Lning[i].m_UnitUv = atof(token);
		// 분기점 갯수
		token = strtok(NULL," \n\t");
		m_Lning[i].m_LPointNum = atoi(token);
		// 번개 갯수
		token = strtok(NULL," \n\t");
		m_Lning[i].m_LnNum = atoi(token);
		// pos.x :: 스킬쓴 캐릭 위치에서 상대적인 위치 
		token = strtok(NULL," \n\t");
		m_Lning[i].m_Pos.x = atof(token);
		//pos.y
		token = strtok(NULL," \n\t");
		m_Lning[i].m_Pos.y = atof(token);
		//pos.z
		token = strtok(NULL," \n\t");
		m_Lning[i].m_Pos.z = atof(token);
		// plane size
		token = strtok(NULL," \n\t");
		m_Lning[i].m_PlaneSize = atof(token);
		//color
		m_Lning[i].m_Color = D3DCOLOR_ARGB(255,255,255,255);
		// shake value 최소값
		token = strtok(NULL," \n\t");
		m_Lning[i].m_ShakeMin = atof(token);
		// shake value 최대값
		token = strtok(NULL," \n\t");
		m_Lning[i].m_ShakeMax = atof(token);
		// 업데이트 주기(흔들림 속도)
		token = strtok(NULL," \n\t");
		m_Lning[i].m_UpdateFrame = atoi(token);
		//캐릭 위치가 업데이트 되었을때 따라올것인지아닌지
		token = strtok(NULL," \n\t");
		m_Lning[i].m_Follow = (atoi(token) > 0)? true : false;
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		// blendmode
		token = strtok(buf," \n\t");
		m_Lning[i].m_BlendMode = atoi(token);
		// random 폭 size
		token = strtok(NULL," \n\t");
		m_Lning[i].m_RandSize = atof(token);
		// 십자 플랜인지 1자 플랜인지
		token = strtok(NULL," \n\t");
		m_Lning[i].m_PlaneValue = atoi(token);
		//세번째 줄
		memset(buf,0,sizeof(char) * ESBUF_SIZE);
		fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		//start eff setting
		token = strtok(buf," \n\t");
		
		for (j=0;j<m_EffNum;j++) {
			if (!(strcmp(token,m_EffList[j].m_EffName))) {
				index = j;
				break;
			}
		}
		m_Lning[i].m_StartEffIndex = index;
		
		token = strtok(NULL," \n\t");
		m_Lning[i].m_StartFrame = atoi(token);
		
		//end eff setting
		token = strtok(NULL," \n\t");
	
		for (j=0;j<m_EffNum;j++) {
			if (!(strcmp(token,m_EffList[j].m_EffName))) {
				index = j;
				break;
			}
		}
		m_Lning[i].m_EndEffIndex = index;
		
		token = strtok(NULL," \n\t");
		m_Lning[i].m_EndFrame = atoi(token);
		// lightning 객체 malloc
		m_Lning[i].m_Lightn = new CLightning[m_Lning[i].m_LnNum];
	}
		
}
void CEffScript ::GetSound(char *tok) {
	char *token = NULL;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int sound_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_SoundNum = atoi(token);
		sound_num = m_EffList[index].m_SoundNum;

		if (sound_num > 0) {
			m_EffList[index].m_Sound = new CEffSound[sound_num];
		}
		for (int j=0;j<sound_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_Sound[j].m_StartFrame = atoi(token);
			token = strtok(NULL," \n\t");
			strcpy(m_EffList[index].m_Sound[j].m_Name,token);
			token = strtok(NULL," \n\t");
			if (token == NULL)
				continue;
			m_EffList[index].m_Sound[j].m_Min = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Sound[j].m_Max = atof(token);
			token = strtok(NULL," \n\t");
			if (token[0] == 'S')
				m_EffList[index].m_Sound[j].m_Target = ES;
			else if (token[0] == 'M')
				m_EffList[index].m_Sound[j].m_Target = EM;
			else if (token[0] == 'E')
				m_EffList[index].m_Sound[j].m_Target = EE;
		}
	}
}
void CEffScript ::GetHit(char *tok) {
	char *token = NULL;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int hit_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_HitFrameNum = atoi(token);
		hit_num = m_EffList[index].m_HitFrameNum;

		if (hit_num > 0) {
			m_EffList[index].m_HitFrame = new int[hit_num];
		}
		for (int j=0;j<hit_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_HitFrame[j] = atoi(token);
		}
	}
}
void CEffScript ::GetFadeOut(char *tok) {
	char *token = NULL;
	m_FadeOut[0] = atof(tok);
	token = strtok(NULL," \n\t");
	m_FadeOut[1] = atof(token);
	token = strtok(NULL," \n\t");
	m_FadeOut[2] = atof(token);

}
void CEffScript ::GetCLight(char *tok) {

	char *token;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int clight_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_CLightNum = atoi(token);
		clight_num = m_EffList[index].m_CLightNum;

		if (clight_num > 0) {
			m_EffList[index].m_CLightColor = new color[clight_num];
			m_EffList[index].m_CLightDistance = new float[clight_num];
			m_EffList[index].m_CLightStart = new int[clight_num];
			m_EffList[index].m_CLightSwitch = new bool[clight_num];
			m_EffList[index].m_CLightTime = new DWORD[clight_num];
			m_EffList[index].m_CLightTarget = new int[clight_num];

			
		}
		for (int j=0;j<clight_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_CLightStart[j] = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightColor[j].r = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightColor[j].g = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightColor[j].b = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightColor[j].a = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightDistance[j] = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_CLightTime[j] = atoi(token);
			token = strtok(NULL," \n\t");
			if (token[0] == 'S')
				m_EffList[index].m_CLightTarget[j] = ESS;
			else if (token[0] == 'E')
				m_EffList[index].m_CLightTarget[j] = ESE;

			m_EffList[index].m_CLightSwitch[j] = false;
		}
	}

}
void CEffScript::GetExt5ex(char *tok) {
	char *token;
	
	m_bExt5ex = true;
	m_bExt5 = true;
	m_Ext5Value = new CEffExt5;
	strcpy(m_Ext5Value->m_TextureName,tok);
	token = strtok(NULL," \n\t");
	m_Ext5Value->m_LineSize = atof(token);
	token = strtok(NULL," \n\t");
	m_Ext5Value->m_Rand = (atoi(token) > 0) ? true : false;
	
}

void CEffScript ::GetFadeIn(char *tok) {
	char *token;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int fade_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[index].m_FadeInNum = atoi(token);
		fade_num = m_EffList[index].m_FadeInNum;

		if ((m_EffList[index].m_FadeInNum) > 0) {
			m_EffList[index].m_FadeColor = new color[fade_num];
			m_EffList[index].m_FadeInStart = new int[fade_num];
			m_EffList[index].m_FadeIn = new D3DXVECTOR3[fade_num];
			m_EffList[index].m_FadeInSwitch = new bool[fade_num];
		}
		for (int j=0;j<fade_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_FadeInStart[j] = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeIn[j].x = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeIn[j].y = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeIn[j].z = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeColor[j].r = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeColor[j].g = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_FadeColor[j].b = atoi(token);
			token = strtok(NULL," \n\t");
			if (token) {
				m_EffList[index].m_FadeColor[j].a = atoi(token);
			}
			m_EffList[index].m_FadeInSwitch[j] = false;
		}
	}
}

void CEffScript::SetInterfaceSet(bool b) {
	int i;

	m_bInterfaceSet = b;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetInterfaceSet(b);
	}
}
void CEffScript::SetStartPos(float x,float y,float z) {

	D3DXVECTOR3 bodypos = D3DXVECTOR3(0.0f,0.0f,0.0f);

	if (m_bEffCharacter) 
	{		// Character Effect
		CZ3DObject *body;
		
		D3DXMATRIX matQuatRot;
		D3DXMATRIX matResult;
		D3DXMATRIX matTmp;
		float body_scale = 0.0f;

		if (m_StartChr != NULL) 
		{

		
#ifndef __EFF_WCREATOR__		// Client
            if ( RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
			{
				// Pivot Info Get 
				body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
				if (body != NULL) {

					vector3 body_tm;
					quaternion body_q;
					D3DXQUATERNION tmp;
					
					body_scale = m_StartChr->GetScale();
						
					body->GetDirection(body_q);			// Direction
					body->GetPosition(body_tm);			// Position
					
					tmp.x = body_q.x;
					tmp.y = body_q.y;
					tmp.z = body_q.z;
					tmp.w = body_q.w;

					D3DXMatrixIdentity(&matTmp);
					matTmp._41 = m_pEffCharacter->vecPos.x;
					matTmp._42 = m_pEffCharacter->vecPos.y;
					matTmp._43 = m_pEffCharacter->vecPos.z;
					// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
					D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
					D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
					// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
					tmp = m_pEffCharacter->vecQuat * tmp;									

					body_q.x = tmp.x;
					body_q.y = tmp.y;
					body_q.z = tmp.z;
					body_q.w = tmp.w;

					bodypos.x = body_tm.x + matResult._41;
					bodypos.y = body_tm.y + matResult._42;
					bodypos.z = body_tm.z + matResult._43;
				
					bodypos *= body_scale;
					x += bodypos.x;
					y += bodypos.y;
					z += bodypos.z;

				}
			}
#endif 
		}
		if (m_lstEff.size()) {
			int i;
			int index;
			for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
				D3DXVECTOR3 tmp_pos;

				index = m_EffPlayData.m_StartId[i];
							
				
				if (m_lstEff[index] != NULL) {

					m_lstEff[i]->SetScale(m_EffList[i].m_Scale * body_scale,m_EffList[i].m_Scale  * body_scale,m_EffList[i].m_Scale  * body_scale);
				}
			}

		}

		

	}

	if (m_bPosSet) {
		if (m_bExt7) { // extension7 일때 SetStartPos 할시 end pos setting
			int i;
			int index;
			m_EndPosNum = 1;
			if (m_EndPos) {
			
				delete[] m_EndPos;
				m_EndPos = NULL;
			}
			m_EndPos = new D3DXVECTOR3[m_EndPosNum];
			m_EndPos[0].x = x;
			m_EndPos[0].y = y;
			m_EndPos[0].z = z;
			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_EndEffIdNum;i++) {
					D3DXVECTOR3 tmp_pos;
					tmp_pos.x = m_EndPos[0].x + (m_EffPlayData.m_EndPos[0]* m_MulPos);
					tmp_pos.y = m_EndPos[0].y + (m_EffPlayData.m_EndPos[1]* m_MulPos);
					tmp_pos.z = m_EndPos[0].z + (m_EffPlayData.m_EndPos[2]* m_MulPos);

					index = m_EffPlayData.m_EndEffId[i];
					if (index == ESEMPTY)
						break;
					// effect 시작
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}
			}
			m_EndPos[0].x = x - bodypos.x;
			m_EndPos[0].y = y - bodypos.y;
			m_EndPos[0].z = z - bodypos.z;

			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)		
					m_SubScript[i].SetEndPos(m_EndPos[0].x,m_EndPos[0].y,m_EndPos[0].z);
			}
		}
		else { // 기본 start pos settting
			int i;
			int index;
			m_StartPosSet = true;
			m_StartPos.x = x;
			m_StartPos.y = y;
			m_StartPos.z = z;

			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
					D3DXVECTOR3 tmp_pos;

					index = m_EffPlayData.m_StartId[i];
								
					tmp_pos = m_StartPos;
					tmp_pos.x += (m_EffList[index].m_StartPos[0]* m_MulPos);
					tmp_pos.y += (m_EffList[index].m_StartPos[1]* m_MulPos);
					tmp_pos.z += (m_EffList[index].m_StartPos[2]* m_MulPos);
					
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}

			}
			m_StartPos.x = x - bodypos.x;
			m_StartPos.y = y - bodypos.y;
			m_StartPos.z = z - bodypos.z;

			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetStartPos(m_StartPos);
			}
		}
	}
}
void CEffScript::SetScriptValue(int n) {
	int i;
	m_ScriptValue = n;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetScriptValue(n);
	}
}
void CEffScript::SetStartPos(D3DXVECTOR3 start) {
	D3DXVECTOR3 bodypos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	if (m_bEffCharacter) 
	{		// Character Effect
		CZ3DObject *body;
		
		D3DXMATRIX matQuatRot;
		D3DXMATRIX matResult;
		D3DXMATRIX matTmp;
		if (m_StartChr != NULL) 
		{
		
#ifndef __EFF_WCREATOR__		// Client
            if ( RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
			{
				// Pivot Info Get 
				body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
				if (body != NULL) {

					vector3 body_tm;
					quaternion body_q;
					D3DXQUATERNION tmp;
					float body_scale = 0.0f;
					body_scale = m_StartChr->GetScale();
						
					body->GetDirection(body_q);			// Direction
					body->GetPosition(body_tm);			// Position
					
					tmp.x = body_q.x;
					tmp.y = body_q.y;
					tmp.z = body_q.z;
					tmp.w = body_q.w;

					D3DXMatrixIdentity(&matTmp);
					matTmp._41 = m_pEffCharacter->vecPos.x;
					matTmp._42 = m_pEffCharacter->vecPos.y;
					matTmp._43 = m_pEffCharacter->vecPos.z;
					// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
					D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
					D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
					// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
					tmp = m_pEffCharacter->vecQuat * tmp;									

					body_q.x = tmp.x;
					body_q.y = tmp.y;
					body_q.z = tmp.z;
					body_q.w = tmp.w;

					bodypos.x = body_tm.x + matResult._41;
					bodypos.y = body_tm.y + matResult._42;
					bodypos.z = body_tm.z + matResult._43;
				
					bodypos *= body_scale;
					start.x += bodypos.x;
					start.y += bodypos.y;
					start.z += bodypos.z;

				}
			}
#endif 
		}
	}
	if (m_bPosSet) {
		if (m_bExt7) {// extension7 일때 SetStartPos 할시 end pos setting
			int i;
			int index;
			m_EndPosNum = 1;
			if (m_EndPos) {
			
				delete[] m_EndPos;
				m_EndPos = NULL;
			}
			m_EndPos = new D3DXVECTOR3[m_EndPosNum];
			m_EndPos[0].x = start.x;
			m_EndPos[0].y = start.y;
			m_EndPos[0].z = start.z;
			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_EndEffIdNum;i++) {
					D3DXVECTOR3 tmp_pos;
					tmp_pos.x = m_EndPos[0].x + (m_EffPlayData.m_EndPos[0]* m_MulPos);
					tmp_pos.y = m_EndPos[0].y + (m_EffPlayData.m_EndPos[1]* m_MulPos);
					tmp_pos.z = m_EndPos[0].z + (m_EffPlayData.m_EndPos[2]* m_MulPos);

					index = m_EffPlayData.m_EndEffId[i];
					if (index == ESEMPTY)
						break;
					// effect 시작
					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}
			}
			m_EndPos[0].x = start.x - bodypos.x;
			m_EndPos[0].y = start.y - bodypos.y;
			m_EndPos[0].z = start.z - bodypos.z;

			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetEndPos(m_EndPos[0].x,m_EndPos[0].y,m_EndPos[0].z);			
			}
		}
		else { // 기본 startpos setting
			int i;
			int index;
			m_StartPos = start;
			m_StartPosSet = true;
			if (m_lstEff.size()) {
				for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
					D3DXVECTOR3 tmp_pos;

					index = m_EffPlayData.m_StartId[i];
								
					tmp_pos = m_StartPos;
					tmp_pos.x += (m_EffList[index].m_StartPos[0]* m_MulPos);
					tmp_pos.y += (m_EffList[index].m_StartPos[1]* m_MulPos);
					tmp_pos.z += (m_EffList[index].m_StartPos[2]* m_MulPos);

					if (m_lstEff[index] != NULL) {
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					}
				}

			}
			m_StartPos = start - bodypos;
			for (i=0;i<m_SubScriptNum;i++) {
				if (m_SubScript != NULL)
					m_SubScript[i].SetStartPos(m_StartPos);
			}
		}
	}
}
D3DXQUATERNION CEffScript::GetExt4Quat() {
	if (m_bExt4InfoSet)
		return m_Ext4Quat;
	else {
		for (int i=0;i<m_SubScriptNum;i++) {
			if (m_SubScript[i].m_bExt4InfoSet)
				return m_SubScript[i].GetExt4Quat();
		}
	}
	return m_Ext4Quat;

}
void CEffScript::GetCameraShake(char *tok) {
	
	char *token;
	int i;
	int index = ESEMPTY;

	for (i=0;i<m_EffNum;i++) {
		if (!(strcmp(tok,m_EffList[i].m_EffName))) {
			index = i;
			break;
		}
	}
	if (index != ESEMPTY) {
		int shake_num = 0;

		token = strtok(NULL," \n\t");
		m_EffList[i].m_ShakeNum = atoi(token);
		shake_num = m_EffList[i].m_ShakeNum;

		if (shake_num > 0) 
			m_EffList[i].m_Shake = new CEffShake[shake_num];
		
		for (int j=0;j<shake_num;j++) {
			char buf[ESBUF_SIZE] = {0};

			memset(buf,0,sizeof(char) * ESBUF_SIZE);
			fgets(buf,sizeof(char) * ESBUF_SIZE,m_ScriptFile);
		
			token = strtok(buf," \n\t");
			m_EffList[index].m_Shake[j].m_StartTime = atoi(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Shake[j].m_ShakeTime = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Shake[j].m_length[0] = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Shake[j].m_length[1] = atof(token);
			token = strtok(NULL," \n\t");
			m_EffList[index].m_Shake[j].m_length[2] = atof(token);

		}
	}

	
}

void CEffScript::PlayEffSound(int index,int j,float x,float y,float z,float mind,float maxd) {

	static bool bEnable = true;

	if ( !bEnable )
		return;

	char buf[ESBUF_SIZE] = {0};
	int sindex = ESEMPTY;
//	try
//	{
		/* 기존사운드코드 
		CSoundManager &smanager  = CSoundManager::GetInstance();
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		ISoundObject & sbuffer = smanager.GetBuffer(buf,true,false,30);
		if ((m_EffList[index].m_Sound[j].m_Name[0] == 'L')&&(m_EffList[index].m_Sound[j].m_Name[1] == '_')) {
			//looping sound
			sindex = sbuffer.Play(true);
		}
		else 
			sindex = sbuffer.Play(false);
		sbuffer.SetPosition(sindex,x,y,z);
		if (!m_Mine) { // 다른 사람이 쓴 스킬 
			float oppmind = mind / 2.0f;
			sbuffer.SetDistance(sindex,oppmind,maxd);
		}
		else {	// 내가 쓴 스킬 
			float oppmind = mind * 1.5f;
			float oppmaxd = maxd * 1.5f;

			sbuffer.SetDistance(sindex,oppmind,oppmaxd);
		}*/

		//새로삽입한코드 
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		CSound3D* pSound3D; 
		bool bResult = CSoundMgr::_GetInstance()->GetSound3D( pSound3D, buf ) ;
		if ( !bResult ) return; 

		D3DVECTOR Pos; 
		Pos.x = x; Pos.y = y; Pos.z = z; 
		pSound3D->SetPosition( Pos ); 
	
		if (!m_Mine) 
		{ // 다른 사람이 쓴 스킬 
			float oppmind = mind / 2.0f;
			pSound3D->SetMinDistance( oppmind ); 
			pSound3D->SetMaxDistance( maxd ); 
		}
		else 
		{	// 내가 쓴 스킬 
			float oppmind = mind * 1.5f;
			float oppmaxd = maxd * 1.5f;
			pSound3D->SetMinDistance( oppmind ); 
			pSound3D->SetMaxDistance( oppmaxd ); 
		}

		if ((m_EffList[index].m_Sound[j].m_Name[0] == 'L')&&(m_EffList[index].m_Sound[j].m_Name[1] == '_')) 
			pSound3D->SetLooping( true ); 
		else 
			pSound3D->SetLooping( false );

		pSound3D->SetVolume(1);
		pSound3D->Play(); 
/*	}
	catch( std::exception & e )
	{
		MessageBox( NULL, e.what(), " ", MB_OK );
		bEnable = false;
	}
*/
}

void CEffScript::PlaySlide(int index,int j) 
{ 
	// 캐릭터를 뒤로 미끌리는 효과 
	if (m_EffList[index].m_Ext6Value[j].m_SlideLength != 0.0f) 
	{
        if ( m_pPlayer && m_ulPlayerID != 0xFFFFFFFF )
        {
#ifndef __EFF_WCREATOR__
            if ( RYLCreatureManager::Instance()->GetCreature( m_ulPlayerID ) == m_pPlayer )
            {
                // SetPlayer로 지정된 RYLCreature*에 슬라이드 value setting
                // slide 에 관한 직접적 처리는 CCreature class에서 일어난다
                m_pPlayer->SetSlideValue( m_EffList[ index ].m_Ext6Value[ j ] );
            }
#else
            // SetPlayer로 지정된 RYLCreature*에 슬라이드 value setting
			// slide 에 관한 직접적 처리는 CCreature class에서 일어난다
            m_pPlayer->SetSlideValue( m_EffList[ index ].m_Ext6Value[ j ] );
#endif //__EFF_WCREATOR__
        }
	}
}

void CEffScript::PlaySnap(int index,int j) 
{ 
	// snap effect
	if ( m_pPlayer ) 
    {
		bool tmp = ((m_EffList[index].m_Snap[j].m_Loop) > 0) ? true : false;

#ifndef __EFF_WCREATOR__
        if ( m_ulPlayerID != 0xFFFFFFFF )
        {
            if ( RYLCreatureManager::Instance()->GetCreature( m_ulPlayerID ) == m_pPlayer )
            {
                // SetPlayer로 지정된 CCreature *에 슬라이드 value setting
				// SnapEffect는 CCreature에 effect 붙이는 명령 실행시킨다
                m_pPlayer->GetEffect()->AddBodyEffect( m_EffList[ index ].m_Snap[ j ].m_EffName, m_EffList[ index ].m_Snap[ j ].m_PivotName , tmp );
            }
        }
#else		
		// SetPlayer로 지정된 CCreature *에 슬라이드 value setting
		// SnapEffect는 CCreature에 effect 붙이는 명령 실행시킨다
		m_pPlayer->GetEffect()->AddBodyEffect( m_EffList[ index ].m_Snap[ j ].m_EffName, m_EffList[ index ].m_Snap[ j ].m_PivotName , tmp );
#endif// __EFF_WCREATOR__
	}
}

void CEffScript::PlayEffSound(int index,int j) 
{
	char buf[ESBUF_SIZE] = {0};
	int sindex = ESEMPTY;

//	try
//	{
		// 새로삽입할코드 
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		CSound* pSound; 
		if ( !CSoundMgr::_GetInstance()->GetSound( pSound, buf ) ) return; 

		if ((m_EffList[index].m_Sound[j].m_Name[0] == 'L')&&(m_EffList[index].m_Sound[j].m_Name[1] == '_'))
		{
			pSound->SetLooping( true );
			pSound->SetVolume(1);
			pSound->Play(); 
		}
		else
		{
			pSound->SetLooping( false ); 
			pSound->SetVolume(1);
			pSound->Play();
		}		
/*	}
	catch( std::exception & e )
	{
		MessageBox( NULL, e.what(), " ", MB_OK );
	}
*/
}
void CEffScript::StopEffSound(int index,int j,float x,float y,float z,float mind,float maxd) {
	
	if (strstr(m_EffList[index].m_Sound[j].m_Name,"L_chage.wav") != NULL) {
		//casting effect 종료시 루핑을 돌던 sound 파일과 새 effect의 사운드 파일 재생의 
		//빈 공간을 채워 주는 sound play
		char buf[ESBUF_SIZE] = {0};
		int sindex = ESEMPTY;

		/* 기존사운드코드 
		CSoundManager &smanager  = CSoundManager::GetInstance();

		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		ISoundObject & sbuffer = smanager.GetBuffer(buf,false);
		// looping sound
		sbuffer.StopAll();
		////
		// fade out sound (부드럽게 넘어가기 위해서)
		
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,"ch.wav");
		try
		{
			ISoundObject & sfadeoutbuffer = smanager.GetBuffer(buf,false);
		
			sindex = sfadeoutbuffer.Play(false);
		
			sfadeoutbuffer.SetPosition(sindex,x,y,z);
			sfadeoutbuffer.SetDistance(sindex,mind,maxd);
		}
		catch( std::exception & e )
		{
			MessageBox( NULL, e.what(), " ", MB_OK );
		}
		*/
		
		//새로삽입한코드 
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		CSound3D* pSound3D; 
		if ( !CSoundMgr::_GetInstance()->GetSound3D( pSound3D, buf ) ) return; 

		pSound3D->Stop(); 

		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,"ch.wav");
//		try
		{	
			CSound3D* pSound3D; 
			if ( !CSoundMgr::_GetInstance()->GetSound3D( pSound3D, buf ) ) return; 

			D3DVECTOR Pos; 
			Pos.x = x; Pos.y = y; Pos.z = z; 
			pSound3D->SetPosition( Pos );
			pSound3D->SetMinDistance( mind ); 
			pSound3D->SetMaxDistance( maxd ); 

			pSound3D->SetVolume(1);
			pSound3D->Play(); 
		}
/*		catch( std::exception & e )
		{
			MessageBox( NULL, e.what(), " ", MB_OK ); 
		}
*/
	}
	else if ((strstr(m_EffList[index].m_Sound[j].m_Name,"archargeo2.wav") != NULL) || (strstr(m_EffList[index].m_Sound[j].m_Name,"TPR12.wav") != NULL)) {
		// aimed shot sound kill
		// turnpunch ready sound kill
		char buf[ESBUF_SIZE] = {0};
		int sindex = ESEMPTY;

		/* 기존사운드코드 
		CSoundManager &smanager  = CSoundManager::GetInstance();

		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		ISoundObject & sbuffer = smanager.GetBuffer(buf,false);
		// looping sound
		sbuffer.StopAll();
		*/ 

		//새로삽입한코드 
		sprintf(buf,"%s%s",EFFECTSOUNDMPATH,m_EffList[index].m_Sound[j].m_Name);
		CSound3D* pSound3D; 
		if ( !CSoundMgr::_GetInstance()->GetSound3D( pSound3D, buf ) ) return; 
		pSound3D->Stop();
	}
}

void CEffScript::PlayFadeIn(int index,int i) 
{
	if (m_EffList[index].m_FadeInSwitch[i] == false) 
	{
		CCameraControl::SetFadeIn(
			m_EffList[index].m_FadeIn[i].x,
			m_EffList[index].m_FadeIn[i].y,
			m_EffList[index].m_FadeIn[i].z,
			m_EffList[index].m_FadeColor[i]);

		m_EffList[index].m_FadeInSwitch[i] = true;
	}
}

void CEffScript::PlayCLight(int index,int i) 
{
	if (m_EffList[index].m_CLightSwitch[i] == false)
	{
		if (m_EffList[index].m_CLightTarget[i] == ESS) 
		{
			vector3 pos;
			DWORD lightcolor;
			lightcolor = D3DCOLOR_ARGB(m_EffList[index].m_CLightColor[i].a,
								  m_EffList[index].m_CLightColor[i].r,
								  m_EffList[index].m_CLightColor[i].g,
								  m_EffList[index].m_CLightColor[i].b);

			pos.x = m_StartPos.x;// + 198.5f;
			pos.y = m_StartPos.y + 10.0f;// + 350.0f;
			pos.z = m_StartPos.z;// + 185.0f;
			if (m_StartChr != NULL) {
#ifndef __EFF_WCREATOR__		// Client
                if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
				{
					CSceneManager::AddChrLight(m_StartChr,pos,lightcolor,m_EffList[index].m_CLightDistance[i],m_EffList[index].m_CLightTime[i]);
				}
#else
				CSceneManager::AddChrLight(m_StartChr,pos,lightcolor,m_EffList[index].m_CLightDistance[i],m_EffList[index].m_CLightTime[i]);
#endif
		
			}
		}
		else if (m_EffList[index].m_CLightTarget[i] == ESE) 
		{
			vector3 pos;
			DWORD lightcolor;
			lightcolor = D3DCOLOR_ARGB(m_EffList[index].m_CLightColor[i].a,
								  m_EffList[index].m_CLightColor[i].r,
								  m_EffList[index].m_CLightColor[i].g,
								  m_EffList[index].m_CLightColor[i].b);


			pos.x = m_EndPos[0].x; //+ 198.5f;
			pos.y = m_EndPos[0].y + 10.0f;
			pos.z = m_EndPos[0].z; //+ 185.0f;

			CSceneManager::AddChrLight(m_EndChr,pos,lightcolor,m_EffList[index].m_CLightDistance[i],m_EffList[index].m_CLightTime[i]);
		
		}
		m_EffList[index].m_CLightSwitch[i] = true;
	}
}
// Bezier Middle 의 초기 정보 세팅
void CEffScript::SetInitBezier(float startx,float starty,float startz,float endx,float endy,float endz)
{
	CLightning *ptr = NULL;

	D3DXVECTOR3 tmp_length = D3DXVECTOR3(endx - startx,endy - starty,endz - startz);
	float length = D3DXVec3Length(&tmp_length);

	// 시작점과 끝점 사이에 거리 세팅
	m_pBezierMid->m_fStartEndLength = length;
	ptr = new CLightning;

	m_pBezierMid->m_pBezierLine = ptr;
	ptr->SetLightningValue(LV_BEZIER);
	ptr->SetDevice(m_Device);
	ptr->CreateList(m_pBezierMid->m_nBezierPoint);
	
	ptr->SetSize(m_pBezierMid->m_fPlaneSize);
	
	ptr->SetStartPoint(startx,starty,startz);
	ptr->SetEndPoint(endx,endy,endz);

	ptr->CreateTexture(1);
	ptr->SetTexture(0,m_pBezierMid->m_strTexture);
	ptr->SetVertNum(1);
	ptr->SetColor(D3DCOLOR_ARGB(255,255,255,255));
	ptr->SetUnitUv(m_pBezierMid->m_fUv);
	ptr->SetPlaneValue(LPLANE_CROSS);
	ptr->CreateVertexBuffer();

	ptr->SetBezierControl(m_pBezierMid->m_fBaseLength,m_pBezierMid->m_fBaseHeight,
							m_pBezierMid->m_fIncreaseSpeed,m_pBezierMid->m_fIncreaseAccel,
							m_pBezierMid->m_fStartEndLength,m_pBezierMid->m_fFadeOut,m_pBezierMid->m_fLimitLength,
							m_pBezierMid->m_iAxis,m_pBezierMid->m_iEndSpeed,m_pBezierMid->m_iEndAccel,m_pBezierMid->m_iUvMode);
	
	// 충돌 후에 구조적으로 궤적라이트닝을 독립 시킨다..?
	//CSceneManager::m_EffectManager.AddLightning(ptr);

	m_bBezierMidSet = true;


}

int CEffScript::ProcessEffect(bool bWeatherEffect) { // 이 스크립트의 처리가 끝나면 0 return
								  // 0 return 하면 script list에서 해제
								  // 수행 도중은 1 return
								  //  S - M - E 구조에서 M ~ E 사이의 충돌 순간 2 return

// end pos 사용되는 것은 end pos가 세팅이 안되어 있을시 0을 리턴  
// load 한 스크립트 data 에 따라 effect 구동 및 발생 제어	
	int i,j;
	int index = 0;
	bool hit_return = false;

	bool crash  = false;
	
	D3DXVECTOR3	g_vecCharEff = D3DXVECTOR3(0.0f,0.0f,0.0f);

//	bool wend = false;
//	static int DelayTick = 0.0f;
//	static bool DelayStart = false;
//	static int DelayFrame = 0;
//	static int del_count = 0;
	

	if (!(m_FileLoad))
		return 0;

	if (m_SubScriptNum > 0 ) {

		if (!m_SubFirstLoad) { // sub effect 미리 로딩 해 놓는 부분 
			for (int sl = 0;sl <m_SubScriptNum;sl++) {
				m_SubScript[sl].m_bVisibility = m_bVisibility;
				m_SubScript[sl].ProcessEffect(); // sub script effect 들 로딩순간
			}
			m_SubFirstLoad = true;	
		}
		
		int subcount = 0;
		bool subcrash = false;
		int subreturn = 0;

		if (!DelayStart) {
			m_dwTick = timeGetTime();
			DelayTick = m_dwTick;
			DelayStart = true;
		}
		else 
			m_dwTick = timeGetTime();

		if ( (m_dwTick-DelayTick)  >= DEFAULTESTICK) {//m_TickFrame) {
			DelayFrame +=((int)(m_dwTick-DelayTick)/ DEFAULTESTICK);
			DelayTick = m_dwTick;
		}
		if (m_CurrentSubScript < m_SubScriptNum) {
			if (DelayFrame >= m_NextScript[m_CurrentSubScript]) {
				DelayStart = false;
				DelayFrame = 0;
				DelayTick = 0.0f;
				m_CurrentSubScript++;
			}
		}
		else {
			DelayStart = false;
			DelayFrame = 0;
			DelayTick = 0.0f;
		}
		if (m_CurrentSubScript > m_SubScriptNum)
			m_CurrentSubScript = m_SubScriptNum;
		
		for (i=0;i<m_CurrentSubScript;i++) {
			if (!(subreturn = m_SubScript[i].ProcessEffect())) {
				subcount++;
			}
			if (subreturn == 2) {
				// sub quaternion setting
				if (m_SubScript != NULL)
					m_Ext4Quat = m_SubScript[i].m_Ext4Quat;
				subcrash = true;

			}
		}
		if (subcount == m_SubScriptNum) {
			m_CheckCrash = false;
			DelayTick = 0.0f;
			DelayStart = false;
			DelayFrame = 0;
			
			return 0;
		}
		else if (subcrash) {

			subcrash = false;
			DelayTick = 0.0f;
			DelayStart = false;
			DelayFrame = 0;

			m_CheckCrash = true;
			return 2;
		}
	
	}
	else {
		//script 가 처음 수행되었을때 eff load
		if (m_ProcCount == ESEMPTY) {
			if (!m_lstEff.size()) {
				PushEff();
				
				if (m_bWeaponEff) {	//무기에 붙는 이팩트 가 있을경우
					PlayWeaponEff();
				}
				if (m_bEffWeaponLine) {	// 검기 세팅
					PlayWeaponLine();
				}
				m_ProcCount = ED;
				


			}
			m_CheckCrash = false;
			return 1;
		}
		else if (m_ProcCount >=ED && m_ProcCount <=EF){
		
		// 시작 했으면..
			switch(m_ProcCount) {

			case ED: //delay frame
				if (!DelayStart) {
					m_dwTick = timeGetTime();
					DelayTick = m_dwTick;
					DelayStart = true;
				}
				else 
					m_dwTick = timeGetTime();

				if ( m_dwTick-DelayTick  >= DEFAULTESTICK) {//m_TickFrame) {
					DelayFrame +=((int)(m_dwTick-DelayTick)/DEFAULTESTICK);
					DelayTick = m_dwTick;
				}
				if (DelayFrame >= (int)m_DelayFrame) {
					DelayStart = false;
					DelayFrame = 0;
					m_ProcCount = ES;
				}
				
				break;

			case ES: // S와 N effect 발생 

				// edith 2009.02.14 스킬사용시 화면 어두워지는 이팩트 
				// fade out
				if (m_bVisibility && bWeatherEffect) {
					if (m_FadeOut[0] != ESEMPTY) {
						if (m_Mine) {
							if (!m_FadeOutStart) {
								float fTime = CSceneManager::GetWeatherTime();
								if (( fTime < 16.0f) && (fTime > 9.0f))
									CCameraControl::SetFadeOut(m_FadeOut[0],m_FadeOut[1],m_FadeOut[2]);
								m_FadeOutStart = true;
							}
						}
					}
				}

				for (i=0;i<m_EffPlayData.m_StartIdNum;i++) {
					D3DXVECTOR3 tmp_pos;

					index = m_EffPlayData.m_StartId[i];
					// effect 시작
					
					m_lstEff[index]->SetAxis(0.0f,0.0f,0.0f);
					tmp_pos = m_StartPos;
					tmp_pos.x += (m_EffList[index].m_StartPos[0]* m_MulPos);
					tmp_pos.y += (m_EffList[index].m_StartPos[1]* m_MulPos);
					tmp_pos.z += (m_EffList[index].m_StartPos[2]* m_MulPos);

					m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
					m_lstEff[index]->SetTick(timeGetTime());
					m_lstEff[index]->SetEffectStart(true);
					m_lstEff[index]->SetLoop(false);

				}
					
				m_ProcCount = EM;
				break;
			case EM:
				if (m_EffPlayData.m_MidEffId == NULL) 
				{
					m_ProcCount = EE;
					break;
				}
				index = m_EffPlayData.m_MidEffId[0];
				if (index == ESEMPTY) 
				{
					m_ProcCount = EE;
					break;
				}
				if (m_EffPlayData.m_StartId) 
				{
					index = m_EffPlayData.m_StartId[0];
					
					if (m_EffPlayData.m_StartFrameNextEff == ESEMPTY)
						break;
				}
				if (!m_EffPlayData.m_StartId || (m_EffPlayData.m_StartFrameNextEff <= m_lstEff[index]->GetFrame())) 
				{
					// target의 chest 얻기 
					CZ3DObject *body;
					if (m_EndChr != NULL) 
					{
#ifndef __EFF_WCREATOR__		// Client
                        if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
						{

							float body_scale = 0.0f;
							body = m_EndChr->GetSkeletonPartObject("CHEST");
							body_scale = m_EndChr->GetScale();
							
							if (body != NULL) 
							{
								matrix *chest = body->GetTM();
								if (chest != NULL) 
								{
									//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
									m_EndChrChest = chest->_42; //무조건 chest 높이로 세팅
									m_EndChrChest *= body_scale;
								}
								else 
								{ 
									// chest 가 없을시
									m_EndChrChest = 150.0f;
									m_EndChrChest *= body_scale;
								}
							}
						}
#else
						float body_scale = 0.0f;
						body = m_EndChr->GetSkeletonPartObject("CHEST");
						body_scale = m_EndChr->GetScale();
						
						if (body != NULL) 
						{
							matrix *chest = body->GetTM();
							if (chest != NULL) 
							{
								//if (m_MidUnit.y > chest->_42) // chest 높이보다 mid unit의 위치가 높다면 chest 높이로 세팅 
								m_EndChrChest = chest->_42; //무조건 chest 높이로 세팅
								m_EndChrChest *= body_scale;
							}
							else 
							{ 
								// chest 가 없을시
								m_EndChrChest = 150.0f;
								m_EndChrChest *= body_scale;
							}
						}
#endif
					}
					// mid unit pos 캐릭 방향에 따라 회전
					// 지금까지 script 안의 좌표 값이 절대좌표였던것을 상대 좌표로 바꿈 
					if ((m_MidUnit.x || m_MidUnit.y || m_MidUnit.z) && !m_bMidFwd) {
						m_bMidFwd = true;
						
						D3DXVECTOR3 tmprot;
						D3DXVECTOR3 tmpfwd = m_ChrFwd;

						tmpfwd *= 1000000.0f;
						tmpfwd.y = 0.0f;
						
						SetRot(m_StartPos,m_StartPos+tmpfwd,tmprot);
						
						D3DXMATRIX tmp_rotm;
						D3DXMATRIX tmp_unitm;
						D3DXMATRIX tmp_roted;

						D3DXMatrixIdentity(&tmp_unitm);
						
						tmp_unitm._41 = m_MidUnit.x;
						tmp_unitm._42 = m_MidUnit.y;
						tmp_unitm._43 = m_MidUnit.z;

						D3DXMatrixRotationYawPitchRoll(&tmp_rotm,tmprot.y,tmprot.x,tmprot.z);
						D3DXMatrixMultiply(&tmp_roted,&tmp_unitm,&tmp_rotm);
						
						m_MidUnit.x = tmp_roted._41;
//						m_MidUnit.y = tmp_roted._42;
						m_MidUnit.z = tmp_roted._43;
					}

					// ---------------------------------------------------------------------------------------
					
					D3DXVECTOR3 vecChrEffPos;

					if (m_bEffCharacter) 
					{		
						// Character Effect
						CZ3DObject *body;
						D3DXVECTOR3 bodypos;
						D3DXMATRIX matQuatRot;
						D3DXMATRIX matResult;
						D3DXMATRIX matTmp;
						if (m_StartChr != NULL) 
						{
#ifndef __EFF_WCREATOR__		// Client
                            if ( !RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
							{

								// Pivot Info Get 
								body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
								if (body != NULL) 
								{
									vector3 body_tm;
									quaternion body_q;
									D3DXQUATERNION tmp;
									float body_scale = 0.0f;
									body_scale = m_StartChr->GetScale();
									 
									body->GetDirection(body_q);			// Direction
									body->GetPosition(body_tm);			// Position
									
									tmp.x = body_q.x;
									tmp.y = body_q.y;
									tmp.z = body_q.z;
									tmp.w = body_q.w;

									D3DXMatrixIdentity(&matTmp);
									matTmp._41 = m_pEffCharacter->vecPos.x;
									matTmp._42 = m_pEffCharacter->vecPos.y;
									matTmp._43 = m_pEffCharacter->vecPos.z;

									// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
									D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
									D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	

									// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
									tmp = m_pEffCharacter->vecQuat * tmp;									

									body_q.x = tmp.x;
									body_q.y = tmp.y;
									body_q.z = tmp.z;
									body_q.w = tmp.w;

									bodypos.x = body_tm.x + matResult._41;
									bodypos.y = body_tm.y + matResult._42;
									bodypos.z = body_tm.z + matResult._43;
								
									bodypos *= body_scale;
									
									vecChrEffPos = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
													   m_StartPos.y + bodypos.y,
													   m_StartPos.z + bodypos.z);
								}	//	if (body != NULL)
							}
#else 
// Pivot Info Get 
							body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
							if (body != NULL) {
		
								vector3 body_tm;
								quaternion body_q;
								D3DXQUATERNION tmp;
								float body_scale = 0.0f;
								body_scale = m_StartChr->GetScale();
								 
								body->GetDirection(body_q);			// Direction
								body->GetPosition(body_tm);			// Position
								
								tmp.x = body_q.x;
								tmp.y = body_q.y;
								tmp.z = body_q.z;
								tmp.w = body_q.w;

								D3DXMatrixIdentity(&matTmp);
								matTmp._41 = m_pEffCharacter->vecPos.x;
								matTmp._42 = m_pEffCharacter->vecPos.y;
								matTmp._43 = m_pEffCharacter->vecPos.z;
								// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
								D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
								D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
								// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
								tmp = m_pEffCharacter->vecQuat * tmp;									

								body_q.x = tmp.x;
								body_q.y = tmp.y;
								body_q.z = tmp.z;
								body_q.w = tmp.w;

								bodypos.x = body_tm.x + matResult._41;
								bodypos.y = body_tm.y + matResult._42;
								bodypos.z = body_tm.z + matResult._43;
							
								bodypos *= body_scale;
								
								vecChrEffPos = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
												   m_StartPos.y + bodypos.y,
												   m_StartPos.z + bodypos.z);

										

							}	//	if (body != NULL)

#endif
						}	//	if (m_StartChr != NULL)
						for (int k=0;k < m_EffPlayData.m_MidEffIdNum;k++) {
							index = m_EffPlayData.m_MidEffId[k];
							if (index == ESEMPTY)
								break;
							m_lstEff[index]->SetLoop(true);
							m_lstEff[index]->SetAxis(0.0f,0.0f,0.0f);
							m_lstEff[index]->SetCenter(vecChrEffPos.x,vecChrEffPos.y,vecChrEffPos.z);
							m_lstEff[index]->SetTick(timeGetTime());
							m_lstEff[index]->SetEffectStart(true);
							m_lstEff[index]->SetMoveState(true);
						}
						SetMidPos(vecChrEffPos.x,vecChrEffPos.y,vecChrEffPos.z);
						SetOldMidPos(vecChrEffPos.x,vecChrEffPos.y,vecChrEffPos.z);

					}	//	if (m_bEffCharacter)
					else {
						/////////////////////////
						for (int k=0;k < m_EffPlayData.m_MidEffIdNum;k++) {
							index = m_EffPlayData.m_MidEffId[k];
							if (index == ESEMPTY)
								break;
							m_lstEff[index]->SetLoop(true);
							m_lstEff[index]->SetAxis(0.0f,0.0f,0.0f);
							m_lstEff[index]->SetCenter(m_StartPos.x + m_MidUnit.x,m_StartPos.y+ m_MidUnit.y,m_StartPos.z+ m_MidUnit.z);
							m_lstEff[index]->SetTick(timeGetTime());
							m_lstEff[index]->SetEffectStart(true);
							m_lstEff[index]->SetMoveState(true);
						}
						SetMidPos(m_StartPos.x + m_MidUnit.x,m_StartPos.y+ m_MidUnit.y,m_StartPos.z+ m_MidUnit.z);
						SetOldMidPos(m_StartPos.x + m_MidUnit.x,m_StartPos.y+ m_MidUnit.y,m_StartPos.z+ m_MidUnit.z);
					}
					SetMidSpeed(m_EffPlayData.m_Speed);
					m_MidShootFrame = timeGetTime();
					m_MidMoveFrame = m_MidShootFrame;
					m_ProcCount = EE;
					
					if (m_bWheel) {
						PlayWheel();
					}
					if (m_bBezierMid) {			// Bezier Mid 이용할때
						if (!m_bEffCharacter) {
							SetInitBezier(m_StartPos.x + m_MidUnit.x,m_StartPos.y + m_MidUnit.y,m_StartPos.z + m_MidUnit.z,
										m_EndPos[0].x,m_EndPos[0].y + m_EndChrChest,m_EndPos[0].z);
						}
						else {
							SetInitBezier(vecChrEffPos.x,vecChrEffPos.y,vecChrEffPos.z,
										m_EndPos[0].x,m_EndPos[0].y + m_EndChrChest,m_EndPos[0].z);
						}
					}
				}
				
				break;
			case EE:
				if (m_EffPlayData.m_MidEffId)
					index = m_EffPlayData.m_MidEffId[0];
				else
					index = ESEMPTY;
				// mid effect 가 없거나 끝났다면
				if ((index == ESEMPTY) || (m_lstEff[index] == NULL) ||(!m_lstEff[index]->GetEffectStart()) || (m_bExt1 && m_bExt1Crash) || (m_bExt3 && m_bExt3Crash)) {
					for (i=0;i<m_EffPlayData.m_EndEffIdNum;i++) {
						D3DXVECTOR3 tmp_pos;
						// end pos not setting 시 0리턴 하여 스크립트 삭제
						if (m_EndPosNum <=0) {
							m_ProcCount = EF;	
							break;
						}
						tmp_pos.x = m_EndPos[0].x + (m_EffPlayData.m_EndPos[0]* m_MulPos);
						tmp_pos.y = m_EndPos[0].y + (m_EffPlayData.m_EndPos[1]* m_MulPos);
						tmp_pos.z = m_EndPos[0].z + (m_EffPlayData.m_EndPos[2]* m_MulPos);

						index = m_EffPlayData.m_EndEffId[i];
						if (index == ESEMPTY)
							break;
						// effect 시작
						m_lstEff[index]->SetLoop(false);
						m_lstEff[index]->SetAxis(0.0f,0.0f,0.0f);
						m_lstEff[index]->SetCenter(tmp_pos.x,tmp_pos.y,tmp_pos.z);
						//m_lstEff[index]->SetCenter(m_StartPos.x,m_StartPos.y,m_StartPos.z);
						m_lstEff[index]->SetTick(timeGetTime());
						m_lstEff[index]->SetEffectStart(true);

					}
					m_ProcCount = EF;
				}
				break;
		/*	case EF:
				for (i=0;i<m_EffPlayData.m_MidEffIdNum;i++) {
					int count = 0;
					if (m_Wheel[i].GetEnd())
						count++;
					if (count == m_EffPlayData.m_MidEffIdNum) {
						wend = true;
					}
					else
						wend = false;
				}

				break;
*/
			default:
				break;
			
			}
	////////////////
			// midunit 에서 y 값만 있는것
			// End Pos Settting 할때 이용한다.
			D3DXVECTOR3 CurrentMidUnit = D3DXVECTOR3(0.0f,m_MidUnit.y,0.0f);
			
			CX3DEffect *pEffect = NULL;
		
			m_Device->GetTransform(D3DTS_VIEW, (D3DMATRIX *)&m_View);

			// delay time 이 아니면
			if (m_ProcCount != ED) {
				for (i = 0;i < (int)m_lstEff.size();i++) {
					pEffect = m_lstEff[i];
					// 시작 되었으면 update
					if (m_lstEff[i] == NULL) 
					{
						del_count++;
						continue;
					}
							
					if (pEffect->GetEffectStart()) 
					{
						//set loop
						if (m_EffList[i].m_LoopStart != ESEMPTY) 
						{
							if (!(m_EffList[i].m_LoopAction)) 
							{
								if (1) {//m_EffList[i].m_LoopSwitch <= m_lstEff[i]->GetFrame()) {
									m_EffList[i].m_LoopAction = true;
									m_lstEff[i]->SetLoop(true);
									m_lstEff[i]->SetMaxFrame(m_EffList[i].m_LoopEnd);
									m_lstEff[i]->SetMinFrame(m_EffList[i].m_LoopStart);

								}
							}
						}
						bool bCharEff = false;
							
						if (m_EffPlayData.m_StartIdNum > 0) 
						{
							int cStart;
							for (cStart = 0; cStart < m_EffPlayData.m_StartIdNum; cStart++) 
							{
								int iCurrentList = m_EffPlayData.m_StartId[cStart];
								if (m_lstEff[iCurrentList] == pEffect)
									bCharEff = true;			// Start Effect 이면 True (Character Effect 적용)
							}
						}
						if (m_bEffCharacter && bCharEff) 
						{		// Character Effect
							CZ3DObject *body;
							D3DXVECTOR3 bodypos;
							D3DXMATRIX matQuatRot;
							D3DXMATRIX matResult;
							D3DXMATRIX matTmp;
							if (m_StartChr != NULL) 
							{
#ifndef __EFF_WCREATOR__		// Client
                                if ( RYLCreatureManager::Instance()->GetCreature( m_StartChr ) )
								{
									// Pivot Info Get 
									body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
									if (body != NULL) {
				
										vector3 body_tm;
										quaternion body_q;
										D3DXQUATERNION tmp;
										float body_scale = 0.0f;
										body_scale = m_StartChr->GetScale();
										 
										body->GetDirection(body_q);			// Direction
										body->GetPosition(body_tm);			// Position
										
										tmp.x = body_q.x;
										tmp.y = body_q.y;
										tmp.z = body_q.z;
										tmp.w = body_q.w;

										D3DXMatrixIdentity(&matTmp);
										matTmp._41 = m_pEffCharacter->vecPos.x;
										matTmp._42 = m_pEffCharacter->vecPos.y;
										matTmp._43 = m_pEffCharacter->vecPos.z;
										// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
										D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
										D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
										// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
										tmp = m_pEffCharacter->vecQuat * tmp;									

										body_q.x = tmp.x;
										body_q.y = tmp.y;
										body_q.z = tmp.z;
										body_q.w = tmp.w;

										bodypos.x = body_tm.x + matResult._41;
										bodypos.y = body_tm.y + matResult._42;
										bodypos.z = body_tm.z + matResult._43;
									
										bodypos *= body_scale;
										// Effect Pos Setting ( World Position + 최종 로컬 effect Position )
										if (m_pEffCharacter->m_bPosUpdate || !m_pEffCharacter->m_bFirstUpdate) { 
										/***/	pEffect->SetCenter(m_StartPos.x + bodypos.x, 
															   m_StartPos.y + bodypos.y,
															   m_StartPos.z + bodypos.z);
											
											g_vecCharEff = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
															   m_StartPos.y + bodypos.y,
															   m_StartPos.z + bodypos.z);
										}
										else {	// 처음 한번만 업데이트 
											pEffect->SetCenter(m_pEffCharacter->m_vecPosBackup.x,
																m_pEffCharacter->m_vecPosBackup.y,
																m_pEffCharacter->m_vecPosBackup.z);

											g_vecCharEff = D3DXVECTOR3(m_pEffCharacter->m_vecPosBackup.x,
																m_pEffCharacter->m_vecPosBackup.y,
																m_pEffCharacter->m_vecPosBackup.z);
										}
										if (m_pEffCharacter->m_bRotUpdate || !m_pEffCharacter->m_bFirstUpdate) {
											// Effect Rotation Setting//***
											pEffect->SetAxis(&body_q);	
										}
										else {	// 처음 한번만 로테이션
											quaternion Init_Quat = quaternion(0.0f,0.0f,0.0f,1.0f);
											//pEffect->SetAxis(&(m_pEffCharacter->m_RotBackup));
											pEffect->SetAxis(&Init_Quat);

										}

										if (m_pEffCharacter->m_bFirstUpdate == false)
										{
											m_pEffCharacter->m_bFirstUpdate = true;
											m_pEffCharacter->m_vecPosBackup = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
																						m_StartPos.y + bodypos.y,
																						m_StartPos.z + bodypos.z);
											m_pEffCharacter->m_RotBackup = D3DXQUATERNION(body_q.x,
																							body_q.y,
																							body_q.z,
																							body_q.w);
										}
										

									}	//	if (body != NULL)
								}
#else

								// Pivot Info Get 
								body = m_StartChr->GetSkeletonPartObject(m_pEffCharacter->m_PivotName);
								if (body != NULL) {
			
									vector3 body_tm;
									quaternion body_q;
									D3DXQUATERNION tmp;
									float body_scale = 0.0f;
									body_scale = m_StartChr->GetScale();
									 
									body->GetDirection(body_q);			// Direction
									body->GetPosition(body_tm);			// Position
									
									tmp.x = body_q.x;
									tmp.y = body_q.y;
									tmp.z = body_q.z;
									tmp.w = body_q.w;

									D3DXMatrixIdentity(&matTmp);
									matTmp._41 = m_pEffCharacter->vecPos.x;
									matTmp._42 = m_pEffCharacter->vecPos.y;
									matTmp._43 = m_pEffCharacter->vecPos.z;
									// 상대좌표를 head에서의 world 좌표로 변환 ( World Pivot Pos + Local Effect Pos * rotation[character] ) 
									D3DXMatrixRotationQuaternion(&matQuatRot,&tmp);									
									D3DXMatrixMultiply(&matResult,&matTmp,&matQuatRot);	
									// 상대 rotation 값 적용 ( rotation[character] * rotation[local Effect] )
									tmp = m_pEffCharacter->vecQuat * tmp;									

									body_q.x = tmp.x;
									body_q.y = tmp.y;
									body_q.z = tmp.z;
									body_q.w = tmp.w;

									bodypos.x = body_tm.x + matResult._41;
									bodypos.y = body_tm.y + matResult._42;
									bodypos.z = body_tm.z + matResult._43;
								
									bodypos *= body_scale;
									// Effect Pos Setting ( World Position + 최종 로컬 effect Position )
									if (m_pEffCharacter->m_bPosUpdate || !m_pEffCharacter->m_bFirstUpdate) { 
										pEffect->SetCenter(m_StartPos.x + bodypos.x, 
														   m_StartPos.y + bodypos.y,
														   m_StartPos.z + bodypos.z);
										
										g_vecCharEff = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
														   m_StartPos.y + bodypos.y,
														   m_StartPos.z + bodypos.z);
									}
									else {	// 처음 한번만 업데이트 
										pEffect->SetCenter(m_pEffCharacter->m_vecPosBackup.x,
															m_pEffCharacter->m_vecPosBackup.y,
															m_pEffCharacter->m_vecPosBackup.z);

										g_vecCharEff = D3DXVECTOR3(m_pEffCharacter->m_vecPosBackup.x,
															m_pEffCharacter->m_vecPosBackup.y,
															m_pEffCharacter->m_vecPosBackup.z);
									}
									if (m_pEffCharacter->m_bRotUpdate || !m_pEffCharacter->m_bFirstUpdate) {
										// Effect Rotation Setting
										pEffect->SetAxis(&body_q);	
									}
									else {	// 처음 한번만 로테이션
										quaternion Init_Quat = quaternion(0.0f,0.0f,0.0f,1.0f);
										//pEffect->SetAxis(&(m_pEffCharacter->m_RotBackup));
										pEffect->SetAxis(&Init_Quat);

									}

									if (m_pEffCharacter->m_bFirstUpdate == false)
									{
										m_pEffCharacter->m_bFirstUpdate = true;
										m_pEffCharacter->m_vecPosBackup = D3DXVECTOR3(m_StartPos.x + bodypos.x, 
																					m_StartPos.y + bodypos.y,
																					m_StartPos.z + bodypos.z);
										m_pEffCharacter->m_RotBackup = D3DXQUATERNION(body_q.x,
																						body_q.y,
																						body_q.z,
																						body_q.w);
									}
									

								}	//	if (body != NULL)
#endif

							}	//	if (m_StartChr != NULL)
						}	//	if (m_bEffCharacter)
					if ((m_EffList[i].m_bStartRot)&&(!m_EffList[i].m_bRotStart)) {
						
						D3DXVECTOR3 startfwd = m_ChrFwd;

						if (!m_bExt2) // m_bExt2 == true :: start effect 회전이 시작 이후에도 계속 적용
							m_EffList[i].m_bRotStart = true;
						
						startfwd *= 1000000.0f;
						startfwd.y = 0.0f;
						/*
						m_ChrFwd *= 1000000.0f;
						m_ChrFwd.y = 0.0f;
						SetRot(m_StartPos,m_StartPos+m_ChrFwd,m_EffList[i].m_StartRot);
						
						*/
						
						SetRot(m_StartPos,m_StartPos+startfwd,m_EffList[i].m_StartRot);
						
						// quaternion 생성
						

						D3DXQUATERNION sqt;
						// 03.06.04 수정 roll 값의 영향을 받지 않게(z 축중심의 회전 X).
						//D3DXQuaternionRotationYawPitchRoll(&sqt,m_EffList[i].m_StartRot.y,m_EffList[i].m_StartRot.x,m_EffList[i].m_StartRot.z);
						D3DXQuaternionRotationYawPitchRoll(&sqt,m_EffList[i].m_StartRot.y,m_EffList[i].m_StartRot.x,0.0f);
						
						if (m_bExt4InfoSet) {
							if (!m_bExt4First) {
								// 화살이 꼽힌 object의 방향 quaternion의 역 quaternion을 생성
								D3DXQUATERNION sqt2;
								sqt2.x = -m_Ext4Object.x;
								sqt2.y = -m_Ext4Object.y;
								sqt2.z = -m_Ext4Object.z;
								sqt2.w = m_Ext4Object.w;
								
								/*
								sqt2.x = -sqt.x;
								sqt2.y = -sqt.y;
								sqt2.z = -sqt.z;
								sqt2.w = sqt.w;
								*/

								// world에서 화살의 방향을 object 방향에 상대적 값으로 계산 
								m_Ext4Quat = m_Ext4Quat * sqt2;
								// 상대적이 각도는 처음 한번만 계산해 주면 됨
								m_bExt4First = true;
							}
							//sqt = m_Ext4Object * m_Ext4Quat;
							// 매 업데이트 순간마다 화살의 상대적인 각도에 object의 방향 값을 곱하여 준다
							sqt = m_Ext4Quat * m_Ext4Object;
							//sqt = sqt * m_Ext4Quat;
			
						}

						quaternion sqtt;
						sqtt.x = sqt.x;
						sqtt.y = sqt.y;
						sqtt.z = sqt.z;
						sqtt.w = sqt.w;	
						if (m_lstEff[i] != NULL) { 
							
							m_lstEff[i]->SetAxis(&sqtt);
						}
					}
					
					//world effect 
					if (m_bWorldEffect) {
						PlayTime();				// Set Play Time
				
						D3DXQUATERNION sqt;
						quaternion sqtt;
						D3DXQuaternionRotationYawPitchRoll(&sqt,D3DXToRadian(m_WorldAxis[1]),D3DXToRadian(m_WorldAxis[0]),D3DXToRadian(m_WorldAxis[2]));
						sqtt.x = sqt.x;
						sqtt.y = sqt.y;
						sqtt.z = sqt.z;
						sqtt.w = sqt.w;
									
						m_lstEff[i]->SetAxis(&sqtt);
					}
					if (m_bVisibility) {
						if (m_Mine) {
							// CLight
							for (j=0;j<m_EffList[i].m_CLightNum;j++) {
								if (m_EffList[i].m_CLightStart[j] != ESEMPTY) {
									if (m_EffList[i].m_CLightStart[j] <= m_lstEff[i]->GetFrame())
										PlayCLight(i,j);
								}
							}
							//fade in 부분
							for (j=0;j<m_EffList[i].m_FadeInNum;j++) {
								if (m_EffList[i].m_FadeInStart[j] != ESEMPTY) {
									if (m_EffList[i].m_FadeInStart[j] <= m_lstEff[i]->GetFrame())
										PlayFadeIn(i,j);
								}
							}
							
							//카메라 쉐이킹
							for (j=0;j<m_EffList[i].m_ShakeNum;j++) {
								if (!(m_EffList[i].m_Shake[j].m_Action)) {
									if (m_EffList[i].m_Shake[j].m_length[0] != ESEMPTY) {
										if (m_EffList[i].m_Shake[j].m_StartTime <= m_lstEff[i]->GetFrame()) {
											m_EffList[i].m_Shake[j].m_Action = true;
											CCameraControl::m_fCameraShakeRateTime = m_EffList[i].m_Shake[j].m_ShakeTime;
											CCameraControl::m_fCameraShakeNowTime = 0.0f;
											CCameraControl::m_vecCameraShakeLength = vector3(m_EffList[i].m_Shake[j].m_length[0],
																							 m_EffList[i].m_Shake[j].m_length[1],
																							 m_EffList[i].m_Shake[j].m_length[2]);
										}
									}
								}
							}
						}//mine
					}// if (m_bVisibility)
						int skipframe = 0;
						for (int k=0;k<m_EffPlayData.m_MidEffIdNum;k++) {
							if (i == m_EffPlayData.m_MidEffId[k]) { // mid effect 이면 궤적 처리
									
								if (k==0) {
									
									//fwd_vec = m_EndPos[0] - m_StartPos;
						
									// end pos not setting 시 0리턴 하여 스크립트 삭제
									if (m_EndPosNum <=0) {
										
										return 0;
									}
									//frame skip
									unsigned long move_frame = timeGetTime();
									
									if ( move_frame - m_MidMoveFrame >= DEFAULTESTICK) {//m_TickFrame) {
										skipframe +=((int)(move_frame-m_MidMoveFrame)/DEFAULTESTICK);
										m_MidMoveFrame = move_frame;
									}
									
									for (int fs = 0;fs<skipframe;fs++) {
										if (!m_bBezierMid) {
											if (m_EndChrChest == -1.0f) {
												oldfwd_vec = (m_EndPos[0] + CurrentMidUnit) - m_OldMidPos;
												fwd_vec = (m_EndPos[0] + CurrentMidUnit) - m_MidPos;
											}
											else {
												D3DXVECTOR3 tmpmid;
												tmpmid = CurrentMidUnit;
												tmpmid.y = m_EndChrChest;

												oldfwd_vec = (m_EndPos[0] + tmpmid) - m_OldMidPos;
												fwd_vec = (m_EndPos[0] + tmpmid) - m_MidPos;
											}
											pos_vec = m_MidPos;
										
										
											D3DXVec3Normalize(&fwd_vec,&fwd_vec);
											pos_vec.x += fwd_vec.x * (m_MidSpeed) + m_EffList[i].m_SpeedValue[0];
											pos_vec.y += fwd_vec.y * (m_MidSpeed) + m_EffList[i].m_SpeedValue[1];
											pos_vec.z += fwd_vec.z * (m_MidSpeed) + m_EffList[i].m_SpeedValue[2];
											
											m_EffList[i].m_SpeedValue[0] += m_EffList[i].m_AccelValue[0];
											m_EffList[i].m_SpeedValue[1] += m_EffList[i].m_AccelValue[1];
											m_EffList[i].m_SpeedValue[2] += m_EffList[i].m_AccelValue[2];
											// 영향력 감소
											m_EffList[i].m_SpeedValue[0] *= 0.9f;
											m_EffList[i].m_SpeedValue[1] *= 0.9f;
											m_EffList[i].m_SpeedValue[2] *= 0.9f;
										
										
											m_MidSpeed += m_EffPlayData.m_Accel;
										} //if (!m_bBezierMid) {
										/////////// Bezier Line update
										else {
											if (m_pBezierMid != NULL) {
												if (m_pBezierMid->m_pBezierLine != NULL)
													crash = m_pBezierMid->m_pBezierLine->UpdateLightning(pos_vec,false);
											}
											
										}
										//////////
									}
									
									//extension 5
									if (m_bExt5) { // mid effect 처음 시작시 plane setting 과 알파 세팅
										//if ((m_Ext5Value->m_bExt5ValueSet) == false) {
																					
											//D3DXVECTOR3 PlaneFwd = m_EndPos[0] - m_StartPos;
											
											D3DXVECTOR3 PlaneFwd =  m_EndPos[0] - m_StartPos;

											D3DXVECTOR3 PlaneNormal(0.0f,1.0f,0.0f);
											D3DXVECTOR3 PlaneRight;

											D3DXVec3Normalize(&PlaneFwd,&PlaneFwd);
											D3DXVec3Normalize(&PlaneNormal,&PlaneNormal);
											D3DXVec3Cross(&PlaneRight,&PlaneFwd,&PlaneNormal);
											D3DXVec3Normalize(&PlaneRight,&PlaneRight);
											
											m_Ext5Value->m_PlaneStartPos = m_StartPos + m_MidUnit;
											//높이 세팅
											if ((pos_vec.x == 0.0f) && (pos_vec.y == 0.0f) && (pos_vec.z == 0.0f)) {
												m_Ext5Value->m_PlaneEndPos = m_StartPos + m_MidUnit;
											}
											else {
												D3DXVECTOR3 length1;
												if (m_EndChrChest == -1.0f) // 가슴 높이
													length1 = (m_EndPos[0] + CurrentMidUnit) - m_StartPos;
												else {
													D3DXVECTOR3 tempmid = CurrentMidUnit;
													tempmid.y = m_EndChrChest;

													length1 = (m_EndPos[0] + tempmid) - m_StartPos;
												}
												D3DXVECTOR3 length2 = pos_vec - m_StartPos;
												float len1 = D3DXVec3Length(&length1);
												float len2 = D3DXVec3Length(&length2);
												if (len1 > len2) // 충돌전
													m_Ext5Value->m_PlaneEndPos = pos_vec;
												else {//충돌후 
													if (m_EndChrChest == -1.0f)
														m_Ext5Value->m_PlaneEndPos = (m_EndPos[0] + CurrentMidUnit);
													else {
														D3DXVECTOR3 tmpmid = CurrentMidUnit;
														tmpmid.y = m_EndChrChest;

														m_Ext5Value->m_PlaneEndPos = (m_EndPos[0] + tmpmid);
													}
												}
											}

											// EXT5 : setting 시 m_Ext5Value->m_Rand 값에 영향 받는다
											// start pos에 random vector 적용시킨다
											// 기본치 30 * 30 거리 
											
											if (!m_bExt5ex ||(m_Ext5Value->m_Rand)) {
												if ((m_Ext5Value->m_bExt5ValueSet) == false) {
													m_Ext5Value->m_bExt5ValueSet = true;
													
													m_Ext5Value->m_RandValue.x= rand() % (int)(ESEXTLINESIZE * 3);
													m_Ext5Value->m_RandValue.y= rand() % (int)(ESEXTLINESIZE * 3);
													m_Ext5Value->m_RandValue.z= rand() % (int)(ESEXTLINESIZE * 3);
													
													if (rand()%2)
														m_Ext5Value->m_RandValue.x *=-1.0f;
													if (rand()%2)
														m_Ext5Value->m_RandValue.y *=-1.0f;
													if (rand()%2)
														m_Ext5Value->m_RandValue.z *=-1.0f;
													
													m_Ext5Value->m_PlaneStartPos +=m_Ext5Value->m_RandValue;
												}
												else {
													m_Ext5Value->m_PlaneStartPos +=m_Ext5Value->m_RandValue;
												}
											}
											else {
												m_Ext5Value->m_bExt5ValueSet = true;

											}
											//화살 line setting 부분
											CTexture::SetPath(EFFECTTEXTUREPATH);
											if (!m_bExt5ex) {
												m_Ext5Value->m_PlaneTexture.Load("lightningbeam2.DDS");
											}
											else { //스크립트 내부의 EXT5: 에서 세팅해준 정보 적용
												m_Ext5Value->m_PlaneTexture.Load(m_Ext5Value->m_TextureName);
											}
											//EXT5: 세팅시 plane size 배율 적용시켜 준다 :: m_Ext5Value->m_LineSize 적용	
											//가로 plane
											m_Ext5Value->m_Plane[0].x = m_Ext5Value->m_PlaneStartPos.x - (PlaneRight.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[0].y = m_Ext5Value->m_PlaneStartPos.y - (PlaneRight.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[0].z = m_Ext5Value->m_PlaneStartPos.z - (PlaneRight.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[0].s = 0.0f;
											m_Ext5Value->m_Plane[0].t = 0.0f;
											m_Ext5Value->m_Plane[0].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[0]);

											m_Ext5Value->m_Plane[2].x = m_Ext5Value->m_PlaneStartPos.x + (PlaneRight.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[2].y = m_Ext5Value->m_PlaneStartPos.y + (PlaneRight.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[2].z = m_Ext5Value->m_PlaneStartPos.z + (PlaneRight.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[2].s = 1.0f;
											m_Ext5Value->m_Plane[2].t = 0.0f;
											m_Ext5Value->m_Plane[2].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[2]);


											m_Ext5Value->m_Plane[1].x = m_Ext5Value->m_PlaneEndPos.x - (PlaneRight.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[1].y = m_Ext5Value->m_PlaneEndPos.y - (PlaneRight.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[1].z = m_Ext5Value->m_PlaneEndPos.z - (PlaneRight.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[1].s = 0.0f;
											m_Ext5Value->m_Plane[1].t = 1.0f;
											m_Ext5Value->m_Plane[1].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[1]);

											m_Ext5Value->m_Plane[3].x = m_Ext5Value->m_PlaneEndPos.x + (PlaneRight.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[3].y = m_Ext5Value->m_PlaneEndPos.y + (PlaneRight.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[3].z = m_Ext5Value->m_PlaneEndPos.z + (PlaneRight.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[3].s = 1.0f;
											m_Ext5Value->m_Plane[3].t = 1.0f;
											m_Ext5Value->m_Plane[3].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[3]);
											//세로 플랜
											m_Ext5Value->m_Plane[4].x = m_Ext5Value->m_PlaneStartPos.x - (PlaneNormal.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[4].y = m_Ext5Value->m_PlaneStartPos.y - (PlaneNormal.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[4].z = m_Ext5Value->m_PlaneStartPos.z - (PlaneNormal.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[4].s = 0.0f;
											m_Ext5Value->m_Plane[4].t = 0.0f;
											m_Ext5Value->m_Plane[4].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[0]);

											m_Ext5Value->m_Plane[6].x = m_Ext5Value->m_PlaneStartPos.x + (PlaneNormal.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[6].y = m_Ext5Value->m_PlaneStartPos.y + (PlaneNormal.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[6].z = m_Ext5Value->m_PlaneStartPos.z + (PlaneNormal.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[6].s = 1.0f;
											m_Ext5Value->m_Plane[6].t = 0.0f;
											m_Ext5Value->m_Plane[6].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[2]);


											m_Ext5Value->m_Plane[5].x = m_Ext5Value->m_PlaneEndPos.x - (PlaneNormal.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[5].y = m_Ext5Value->m_PlaneEndPos.y - (PlaneNormal.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[5].z = m_Ext5Value->m_PlaneEndPos.z - (PlaneNormal.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[5].s = 0.0f;
											m_Ext5Value->m_Plane[5].t = 1.0f;
											m_Ext5Value->m_Plane[5].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[1]);

											m_Ext5Value->m_Plane[7].x = m_Ext5Value->m_PlaneEndPos.x + (PlaneNormal.x * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[7].y = m_Ext5Value->m_PlaneEndPos.y + (PlaneNormal.y * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[7].z = m_Ext5Value->m_PlaneEndPos.z + (PlaneNormal.z * (ESEXTLINESIZE/2.0f) * (m_Ext5Value->m_LineSize));
											m_Ext5Value->m_Plane[7].s = 1.0f;
											m_Ext5Value->m_Plane[7].t = 1.0f;
											m_Ext5Value->m_Plane[7].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[3]);

										//}
										
									}
									//


									//effect rot set
									//SetRot(oldfwd_vec,fwd_vec);
									if (!m_NotRot) {
										SetRot(-oldfwd_vec,-fwd_vec,m_MidRot);
		
										// quaternion 생성
										D3DXQUATERNION qt;
										quaternion qtt;
										D3DXQuaternionRotationYawPitchRoll(&qt,m_MidRot.y,m_MidRot.x,m_MidRot.z);
										qtt.x = qt.x;
										qtt.y = qt.y;
										qtt.z = qt.z;
										qtt.w = qt.w;
									
										pEffect->SetAxis(&qtt);
									}


									if (skipframe) {
										//현제 position setting
										SetMidPos(pos_vec.x,pos_vec.y,pos_vec.z);

									}
								}
								
			//					m_Wheel[k].InsertEmptyPoint(m_MidPos.x,m_MidPos.y+150.0f,m_MidPos.z);		
								//////////////////////////////////////////////////////////
								// loop off test(목적지에 도착했는지 test)
								if (skipframe) {
									
									D3DXVECTOR3 test_vec1;
									D3DXVECTOR3 test_vec2;
									D3DXVECTOR3 tmp_v;
									if (m_EndChrChest == -1.0f) {
										test_vec1 = (m_EndPos[0] + CurrentMidUnit) - m_OldMidPos;
										test_vec2 = (m_EndPos[0] + CurrentMidUnit) - m_MidPos;
										tmp_v = (m_EndPos[0] + CurrentMidUnit) - m_MidPos;
									}
									else {
										D3DXVECTOR3 tmpmid = CurrentMidUnit;
										tmpmid.y = m_EndChrChest;

										test_vec1 = (m_EndPos[0] + tmpmid) - m_OldMidPos;
										test_vec2 = (m_EndPos[0] + tmpmid) - m_MidPos;
										tmp_v = (m_EndPos[0] + tmpmid) - m_MidPos;
									}
									D3DXVECTOR3 up_vec(0.0f,1.0f,0.0f);
									D3DXVECTOR3 cro1;
									D3DXVECTOR3 cro2;
									float dot_value;

									
									
									float crash_length = D3DXVec3Length(&tmp_v);
									D3DXVec3Cross(&cro1,&test_vec1,&up_vec);
									D3DXVec3Cross(&cro2,&test_vec2,&up_vec);
									dot_value = D3DXVec3Dot(&cro1,&cro2);
									
									unsigned long test_tick = timeGetTime();
									test_tick -= m_MidShootFrame;
									
									if ((dot_value < 0) || (crash_length <50.0f) || ((test_tick / DEFAULTESTICK) >= 100) || crash) {


										//pEffect->SetLoop(false);
										pEffect->SetMoveState(false);
										//pEffect->SetEffectStart(false);
										// 궤적 페이드 아웃 시작
										/*for (int t=0;t<m_EffPlayData.m_MidEffIdNum;t++) {
											m_Wheel[t].SetFadeStart(true);
										}*/
										
										
										if (m_bExt3) {
											/////
											int ie_count = 0;
											for (int ie=0;ie<m_EffPlayData.m_EndEffIdNum;ie++) {
												int ieindex = m_EffPlayData.m_EndEffId[ie];
												if (ieindex == ESEMPTY) {
													ie_count++;	
													continue;
												}
												if (m_lstEff[ieindex] == NULL)
													ie_count++;
											}
											if (ie_count == m_EffPlayData.m_EndEffIdNum) {
												if (pEffect != NULL)
													pEffect->SetLoop(false);
												
											}
											/////
											m_bExt3Crash = true;
											if (pEffect != NULL) {
												if (m_EndChrChest == -1.0f)
													pEffect->SetCenter(m_EndPos[0].x + CurrentMidUnit.x,m_EndPos[0].y + CurrentMidUnit.y,m_EndPos[0].z + CurrentMidUnit.z);
												else
													pEffect->SetCenter(m_EndPos[0].x + CurrentMidUnit.x,m_EndPos[0].y + m_EndChrChest,m_EndPos[0].z + CurrentMidUnit.z);
												pEffect->SetStopParticle(pEffect->GetFrame());
											}

										}
										else if (m_bExt1){ // ext1
											pEffect->SetLoop(false);
											m_bExt1Crash = true;
											if (pEffect != NULL) {
												if (m_EndChrChest == -1.0f)
													pEffect->SetCenter(m_EndPos[0].x + CurrentMidUnit.x,m_EndPos[0].y + CurrentMidUnit.y,m_EndPos[0].z + CurrentMidUnit.z);
												else
													pEffect->SetCenter(m_EndPos[0].x + CurrentMidUnit.x,m_EndPos[0].y + m_EndChrChest,m_EndPos[0].z + CurrentMidUnit.z);
											}
										}
										else {
											del_count++;
											pEffect->SetLoop(false);
											delete pEffect;
											pEffect = NULL;
											m_lstEff[i] = NULL;
										}
										D3DXQUATERNION qt;
										D3DXQuaternionRotationYawPitchRoll(&qt,m_MidRot.y,m_MidRot.x,m_MidRot.z);

										//extension 4
										if (m_bExt4) {
										
											//extesion4 info setting
											SetExt4Info(EHEAD,qt);
										}
										
										m_Ext4Quat = qt;

										/*m_Ext4Quat.x = m_MidRot.x;
										m_Ext4Quat.y = m_MidRot.y;
										m_Ext4Quat.z = m_MidRot.z;
										*/
										crash = true;	
										// wheel end setting
										if (m_bWheel) 
											UpdateWheel(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1);

										continue;

									}
									else if (pEffect->GetMoveState()) {// 지나친것이 아니므로 위치 업데이트 
										/////////// Bezier Line update
										//if (m_bBezierMid)
										//	m_pBezierMid->m_pBezierLine->UpdateLightning(pos_vec,true);

										//////////


										pEffect->SetCenter(pos_vec.x,pos_vec.y,pos_vec.z);
										
										if (m_bWheel) {
											//next position 계산
											D3DXVECTOR3 npos = D3DXVECTOR3(0.0f,0.0f,0.0f);
											D3DXVECTOR3 fwd = D3DXVECTOR3(0.0f,0.0f,0.0f);

											npos = m_MidPos;
											
											if (m_EndChrChest == -1.0f) {
											
												fwd = (m_EndPos[0] + CurrentMidUnit) - m_MidPos;
											}
											else {
												D3DXVECTOR3 tmpmid;
												tmpmid = CurrentMidUnit;
												tmpmid.y = m_EndChrChest;

											
												fwd = (m_EndPos[0] + tmpmid) - m_MidPos;
											}

											D3DXVec3Normalize(&fwd,&fwd);
											npos.x += fwd.x * (m_MidSpeed) + m_EffList[i].m_SpeedValue[0];
											npos.y += fwd.y * (m_MidSpeed) + m_EffList[i].m_SpeedValue[1];
											npos.z += fwd.z * (m_MidSpeed) + m_EffList[i].m_SpeedValue[2];

											UpdateWheel(pos_vec.x,pos_vec.y,pos_vec.z,npos.x,npos.y,npos.z);
										}
									}
									////////////////////////////////////////////////////////////////
									/*for (int fr = 0;fr <skipframe;fr++) {
										m_MidSpeed += m_EffPlayData.m_Accel;
									}*/
								}
								// min max check
								if (m_MidSpeed < m_EffPlayData.m_MinSpeed)
									m_MidSpeed = m_EffPlayData.m_MinSpeed;
								else if (m_MidSpeed > m_EffPlayData.m_MaxSpeed)
									m_MidSpeed = m_EffPlayData.m_MaxSpeed;
							}	
							
						}
						
						if (pEffect == NULL)
							continue;
						// 랜덤 사운드 일때 사용	
						int soundlimit = 0;
						int psound = 0;

					
						if (m_bVisibility) {					// Visibility 값이 true 일때
							//sound 출력
							for (j=0;j<m_EffList[i].m_SoundNum;j++) {
								if (m_EffList[i].m_Sound) {
									if (!(m_EffList[i].m_Sound[j].m_bPlay)) {
										if (m_EffList[i].m_Sound[j].m_StartFrame <= m_lstEff[i]->GetFrame()) {
											//랜덤 사운드 체크
											for (int k=j;k<(m_EffList[i].m_SoundNum);k++) {
												if (m_EffList[i].m_Sound[j].m_StartFrame == m_EffList[i].m_Sound[k].m_StartFrame) {
													soundlimit++;
													m_EffList[i].m_Sound[k].m_bPlay = true;
												}
												else
													break;
											}
											if (soundlimit == 1) {
												psound = j;
											}
											else if (soundlimit >1) {
												psound = j + (rand() % soundlimit);
											}
											if (!m_bInterfaceSet) { //interface effect 가 아니면..
												if (m_EffList[i].m_Sound[psound].m_Target == ES)
													PlayEffSound(i,psound,m_StartPos.x,m_StartPos.y,m_StartPos.z,m_EffList[i].m_Sound[psound].m_Min,m_EffList[i].m_Sound[psound].m_Max);
												else if (m_EffList[i].m_Sound[psound].m_Target == EM)
													PlayEffSound(i,psound,m_MidPos.x,m_MidPos.y,m_MidPos.z,m_EffList[i].m_Sound[psound].m_Min,m_EffList[i].m_Sound[psound].m_Max);
												else if (m_EffList[i].m_Sound[psound].m_Target == EE)
													PlayEffSound(i,psound,m_EndPos[0].x,m_EndPos[0].y,m_EndPos[0].z,m_EffList[i].m_Sound[psound].m_Min,m_EffList[i].m_Sound[psound].m_Max);
											}
											else { // interface effect sound
												PlayEffSound(i,psound);	
											}

											break;
										}
									}// if (!(m_EffList[i].m_Sound[j].m_bPlay))
									else if (m_lstEff[i] == NULL) { // 이미 죽은 effect 이면..
												
									}  
									else {	// 재생되었던 사운드
										if (m_bWorldEffect) {
											if (m_lstEff[i]->GetFrame() < m_lstEff[i]->GetBeforeFrame()) {
												m_EffList[i].m_Sound[j].m_bPlay = false;
											}
										}
									}
								}// if (m_EffList[i].m_Sound) 
							} //for
							if (m_bLightning) { //lightning use
								for (j=0;j<m_LningNum;j++) { //lightning setting
									PlayLning(j,i,m_lstEff[i]->GetFrame(),g_vecCharEff);
								}
							}
							if (m_bLightn2) { //lightning2 use
								PlayLning2(j,i,m_lstEff[i]->GetFrame(),g_vecCharEff);
							}
							if (m_bLightn3) { // lightning3 use
								PlayLning3(j,i,m_lstEff[i]->GetFrame(),g_vecCharEff);
							}
						}	// m_bVisibility
						//hit return 
						if (m_EffList[i].m_HitFrameNum >0) {
							for (j=m_EffList[i].m_HitFrameCount;j<m_EffList[i].m_HitFrameNum;j++) {
								if (m_EffList[i].m_HitFrame[j] != ESEMPTY) {
									if (m_EffList[i].m_HitFrame[j] <= m_lstEff[i]->GetFrame()) {
										hit_return = true;
										m_EffList[i].m_HitFrameCount++;
									}
								}
							}
					
						}
						//if (m_Mine) { //내 것일때만 실행
						//slide
						if (m_EffList[i].m_Ext6ValueNum >0) {
							for (j=m_EffList[i].m_Ext6ValueCount;j<m_EffList[i].m_Ext6ValueNum;j++) {
								if (m_EffList[i].m_Ext6Value[j].m_SlideStartFrame != ESEMPTY) {
									if (m_EffList[i].m_Ext6Value[j].m_SlideStartFrame <= m_lstEff[i]->GetFrame()) {
										PlaySlide(i,j);
										m_EffList[i].m_Ext6ValueCount++;
									}
								}
							}
						}
						//}
						if (m_bVisibility) {
							if (m_EffList[i].m_SnapNum > 0) { //snap effect play
								for (j=m_EffList[i].m_SnapCount;j<m_EffList[i].m_SnapNum;j++) {
									if (m_EffList[i].m_Snap[j].m_SnapStartFrame != ESEMPTY) {
										if (m_EffList[i].m_Snap[j].m_SnapStartFrame <= m_lstEff[i]->GetFrame()) {
											PlaySnap(i,j);
											m_EffList[i].m_SnapCount++;
										}
									}
								}
							}
						}	// if (m_bVisibility)
						m_dwTick = timeGetTime();
						if (m_dwTick - pEffect->GetTick() >= pEffect->GetFrameTick())
						{
						
							if (!pEffect->Interpolation(m_dwTick - pEffect->GetTick()))
							{
								del_count++;
							
								delete pEffect;
								pEffect = NULL;
								m_lstEff[i] = NULL;
								//m_lstEff[i]->SetEffectStart(false);

								//m_lstEff.erase(&(m_lstEff[i]));
								
							} else
							{
							
								pEffect->SetTick(m_dwTick);
							}
						}
					}
				}//for
				/////////// Bezier Line update
				if (m_bBezierMid) {
					if ((m_pBezierMid != NULL) && ((m_pBezierMid->m_pBezierLine) != NULL)) {
						unsigned long tmp_tick = timeGetTime();
						int extframe = 0;

						if ((m_pBezierMid->m_lTick) != 0) {
							if ((tmp_tick - m_pBezierMid->m_lTick) > DEFAULTESTICK) {
								extframe = (int)((tmp_tick - m_pBezierMid->m_lTick)/ DEFAULTESTICK);
								m_pBezierMid->m_lTick = tmp_tick;
							}
						}
						else {
							extframe = 1;
							m_pBezierMid->m_lTick = tmp_tick;
						}
						for (int fr = 0;fr<extframe;fr++) {
							m_pBezierMid->m_pBezierLine->UpdateLightning(pos_vec,true);
						}
					}
				}
				//////////


				if (m_bExt5){ // extension5 일때 서서히 사라지는 라인
					if (m_Ext5Value->m_bExt5ValueSet) {
						unsigned long tmp_tick = timeGetTime();
						int extframe = 0;

						if ((m_Ext5Value->m_UpdateTime) != 0) {
							if ((tmp_tick - m_Ext5Value->m_UpdateTime) > DEFAULTESTICK) {
								extframe = (int)((tmp_tick - m_Ext5Value->m_UpdateTime)/ DEFAULTESTICK);
								m_Ext5Value->m_UpdateTime = tmp_tick;
							}
						}
						else {
							extframe = 1;
							m_Ext5Value->m_UpdateTime = tmp_tick;
						}
						for (int fr = 0;fr<extframe;fr++) {
							for (int pa = 0;pa<4;pa++) {
								if ((pa == 0) || (pa == 2)) // 내쪽 정점 
									m_Ext5Value->m_PlaneAlpha[pa] -= 0.06f;
								else  // targer쪽 정점 
									m_Ext5Value->m_PlaneAlpha[pa] -= 0.04f;
								if ((m_Ext5Value->m_PlaneAlpha[pa]) <=0.0f) {
									m_Ext5Value->m_PlaneAlpha[pa] = 0.0f;
								}
								m_Ext5Value->m_Plane[pa].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[pa]);
								m_Ext5Value->m_Plane[pa+4].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,m_Ext5Value->m_PlaneAlpha[pa]);
							}
						}
					}
				}
			/*	for (int k=0;k<m_EffPlayData.m_EndEffIdNum;k++) {
					if (m_ProcCount == EF) {
						m_Wheel[k].SetEnd();
					}
					m_Wheel[k].SetVertexBuffer(0);
				}
			*/
			} //if
			if (del_count == m_lstEff.size()) {
				del_count = 0;
				
				m_CheckCrash = false;
			//	if (wend)
				if (hit_return) {
					hit_return = false;
					return 2;
				}
				if (m_bExt5) { // extension 5 수행중이면 계속유지
					if (m_Ext5Value->m_bExt5End == false) //끝났으면 0을 리턴 아니면 1 리턴
						return 1;
				}
				return 0;
			/*	else
					return 1;
			*/
			}
			else {
				del_count = 0;	
				if (crash) {
					crash = false;
					m_CheckCrash = true;
					
					if (hit_return) {
						hit_return = false;
						return 2;
					}
					return 1; //충돌시 2return 하는것 취소
				}
				else{
					m_CheckCrash = false;
					if (hit_return) {
						hit_return = false;
						return 2;
					}
					return 1;
				}
			}

		}	
	}
	del_count = 0;
	m_CheckCrash = false;
	if (hit_return) {
		hit_return = false;
		return 2;
	}
	return 1;
}
void CEffScript::SetEffectRot(float x,float y,float z) {
	m_WorldAxis[0] = x;
	m_WorldAxis[1] = y;
	m_WorldAxis[2] = z;

/*	
	for (int i=0;i<m_EffNum;i++) {
		D3DXQUATERNION sqt;
		quaternion sqtt;
		D3DXQuaternionRotationYawPitchRoll(&sqt,D3DXToRadian(y),D3DXToRadian(x),D3DXToRadian(z));
		sqtt.x = sqt.x;
		sqtt.y = sqt.y;
		sqtt.z = sqt.z;
		sqtt.w = sqt.w;
					
		m_lstEff[i]->SetAxis(&sqtt);
	}*/
}

void CEffScript::Render() {
	
	
	int i=0;
	
	if (m_SubScriptNum) { // sub script 처리
		for (i=0;i<m_CurrentSubScript;i++) {
			if (m_SubScript != NULL)
			{	// Sub Script Visibility Setting
				m_SubScript[i].m_bVisibility = m_bVisibility;
				m_SubScript[i].Render();
			}
		}
	}
	else {
		
		if (m_bVisibility) {	 // Visibility 가 false이면 render 하지 않는다. 
	
			m_Device->SetVertexShader(LVERTEXFVF);

			m_Device->SetTexture(0, NULL);
			m_Device->SetTexture(1, NULL);

			m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

			m_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			m_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			m_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

			m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
			m_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			m_Device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			m_Device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

			m_Device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			m_Device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);


			m_Device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_Device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_Device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
			m_Device->SetRenderState(D3DRS_ALPHAREF,0x00000000);
			m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			
			// interface effect 일 경우 z-buffer끔
			if (m_ScriptValue == ESINTERFACE) {
				m_Device->SetRenderState(D3DRS_ZENABLE,FALSE);
			}
			for (i =0 ;i < (int)m_lstEff.size();i++)
			{
					
				if (m_lstEff[i] == NULL)
					continue;
				if (!(m_lstEff[i]->GetEffectStart()))
					continue;
				//if (effectmap->EffectSectorIn(
				//camera pos
				matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
				vector3 vecViewPos=matViewPosition->GetLoc();
				
				int indexx=(int)(vecViewPos.x/SECTORSIZE);
				int indexy=(int)(vecViewPos.z/SECTORSIZE);

				if (m_EffList[i].m_bFogOff)			// Fog off flag true
					m_Device->SetRenderState(D3DRS_FOGENABLE,FALSE);
				else
					m_Device->SetRenderState(D3DRS_FOGENABLE,TRUE);

				//culling 
				CSceneNode CheckNode;

				CheckNode.m_fRad=100.0f;
				CheckNode.m_AccumulateTM.Translation(*m_lstEff[i]->GetCenter());
				m_lstEff[i]->SetScale(m_EffList[i].m_Scale,m_EffList[i].m_Scale,m_EffList[i].m_Scale);
				if (m_lstEff[i]->map_effect == true) {
					if (CheckNode.isCulling())
					{
					//if (effectmap->EffectSectorIn(indexx,indexy,i)) {
						if (m_lstEff[i]) m_lstEff[i]->Render();
					//}
					
					}
				}
				else {
					if (m_lstEff[i]) m_lstEff[i]->Render();
				}
				
			}
		} //if (m_bVisibility)

		if (m_bExt5) {
			if (m_Ext5Value->m_bExt5ValueSet) { // 화살 라인 렌더링
				RenderExt5();
			}
		}
		if (m_bVisibility) {	 // Visibility 가 false이면 render 하지 않는다. 

			if (m_bLightning) {
				RenderLightn();	
			}
			if (m_bLightn2) {
				RenderLightn2();

			}
			if (m_bLightn3) {
				RenderLightn3();
			}
			if (m_bWheel) {
				if (m_bWheelStart)
					RenderWheel();
			}
			////////////// Bezier middle
			if (m_bBezierMid && m_bBezierMidSet)
				m_pBezierMid->m_pBezierLine->Render(0);
			
		
			matrix matWorld;
			matWorld.MakeIdent();
			m_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);

			if (m_ScriptValue == ESINTERFACE) {
				m_Device->SetRenderState(D3DRS_ZENABLE,TRUE);
			}
			m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			m_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
			m_Device->SetRenderState(D3DRS_FOGENABLE,TRUE);	
		}
	}
}
void CEffScript::RenderExt5() { // 화살 라인 렌더링 Code
	if (m_bVisibility) {	 // Visibility 가 false이면 render 하지 않는다. 

		DWORD cull,zbuffer;
		
		D3DXMATRIX beforeworld;
		m_Device->GetTransform(D3DTS_WORLD,&beforeworld);
		m_Device->GetRenderState(D3DRS_CULLMODE,&cull);
		m_Device->GetRenderState(D3DRS_ZWRITEENABLE,&zbuffer);
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		m_Device->SetTransform(D3DTS_WORLD,&world);
		m_Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	/*	m_Device->SetRenderState(D3DRS_ALPHAREF,0x00000000);
		m_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);*/
	/*	m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	*/
		m_Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
			
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		m_Device->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_Device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);	

		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
		m_Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

		m_Device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		m_Device->SetTexture(0,m_Ext5Value->m_PlaneTexture.GetTexture());
		
		m_Device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		m_Device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		
		m_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		
		m_Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,(void *)(m_Ext5Value->m_Plane),sizeof(CEffExt5Vertex));
		m_Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,(void *)(&(m_Ext5Value->m_Plane[4])),sizeof(CEffExt5Vertex));


		m_Device->SetTransform(D3DTS_WORLD,&beforeworld);
		m_Device->SetRenderState(D3DRS_CULLMODE,cull);
		m_Device->SetRenderState(D3DRS_ZWRITEENABLE,zbuffer);
	}
	
	int count = 0;
	for (int i=0;i<4;i++) {
		if (m_Ext5Value->m_PlaneAlpha[i] == 0.0f)
			count++;
	}
	if (count == 4) // 라인이 완전히 사라졌을경우 끝났음을 세팅(메모리 해제용 flag)
		m_Ext5Value->m_bExt5End = true;
}
//궤적 strip 그려줌 
/*void CEffScript::RenderStrip(int index) {

}*/
void CEffScript::SetLight() {			// Vertex Light Setting
	int i,j;
	if (m_iLight > 0) {
		for (i=0;i<m_iLight;i++) {

			for (j=0;j<m_EffNum;j++) {
				if (!strcmp(m_EffList[j].m_EffName,m_pLight[i].m_strEff)) {	
					m_EffList[j].m_bLight = true;
				}
			}
		}
	}
}
void CEffScript::PushEff() {
	
	int i;

	m_Device->GetTransform(D3DTS_VIEW, (D3DMATRIX *)&m_View);
	// Vertex Light Setting
	SetLight();

	for (i=0;i<m_EffNum;i++) {
		float tmp_frame = 0.0f;
		CX3DEffect *tmp;
		tmp = new CX3DEffect;
		m_lstEff.push_back(tmp);

		m_lstEff[i]->SetDevice(m_Device);
		m_lstEff[i]->SetViewMatrix(&m_View);
		m_lstEff[i]->SetEffectSetting();
		m_lstEff[i]->SetScale(m_EffList[i].m_Scale,m_EffList[i].m_Scale,m_EffList[i].m_Scale);
		// Light Setting
		if (m_EffList[i].m_bLight) {
			m_lstEff[i]->SetLight(true);	
		}
		
		// extesion 1 : mid effect가 충돌한뒤에도 자기 생명이 끝날때 까지 유지
		if (strstr(m_EffList[i].m_EffName,"EXT1_") != NULL)
			m_bExt1 = true;
		// extesion 2 : start effect가 계속 캐릭 방향에 따라 회전
		if (strstr(m_EffList[i].m_EffName,"EXT2_") != NULL)
			m_bExt2 = true;
		// extesion 3 : aimed shot 용 particle 유지 
		if (strstr(m_EffList[i].m_EffName,"EXT3_") != NULL)
			m_bExt3 = true;
		// extesion 4 : effect 양도 플레그 (ex.화살 꽃히는 등의 용도)
		if (strstr(m_EffList[i].m_EffName,"EXT4_") != NULL){
			m_bExt4 = true;
			m_bExt2 = true;
		}
		// extesion 5 : 화살 라인 그려주는 확장  
		if (strstr(m_EffList[i].m_EffName,"EXT5_") != NULL) {
			m_bExt5 = true;
			if (m_bExt5ex == false) { 
				m_Ext5Value = new CEffExt5;
			}
		}
		//extension 7 : end pos 와 start pos 위치 swap
		if (strstr(m_EffList[i].m_EffName,"EXT7_") != NULL) { 
			CZ3DGeneralChrModel *t_c;
			m_bExt7 = true;
			//start 와 end 위치 swap
			D3DXVECTOR3 t_p = m_StartPos;
			m_StartPos = m_EndPos[0];
			m_EndPos[0] = t_p;
			// start 와 end model swap
			t_c = m_EndChr;
			m_EndChr = m_StartChr;
			m_StartChr = t_c;
		}
		//extension 8: 이 스크립트가 적용되는 캐릭터의 스캐일에 따라 effect 크기변화 적용 
		if (strstr(m_EffList[i].m_EffName,"EXT8_") != NULL) {
			m_bExt8 = true;
			if (m_StartChr != NULL) {
				float human_size = 150.0f;
				float chr_size = m_StartChr->GetSizeFactor();
				float percentage = chr_size / human_size;

				m_EffList[i].m_Scale *= percentage;
				m_lstEff[i]->SetScale(m_EffList[i].m_Scale,m_EffList[i].m_Scale,m_EffList[i].m_Scale);

			}
		}
		// extension 9: 이 캐릭터에 붙는 snap effect 가 이 이팩트가 끝나더라도 삭제 되지 않는다.
		if (strstr(m_EffList[i].m_EffName,"EXT9_") != NULL) {
			m_bExt9 = true;
		}
		if (strstr(m_EffList[i].m_EffName,"EXT14_") != NULL) { // fog on
			m_EffList[i].m_bFogOff = false;
		}
		else
		{
			m_EffList[i].m_bFogOff = true;
		}
		if (strstr(m_EffList[i].m_EffName,"EXT11_") != NULL) { // cull mode off
			m_bSectorCull = false;
		}
		if (strstr(m_EffList[i].m_EffName,"EXT13_") != NULL) { // cull mode off
			m_bFrustumCull = false;

		}	
		tmp->Load(EFFECTPATH,m_EffList[i].m_EffName,m_bVisibility);
		tmp->SetEffectStart(false);
		tmp->SetMine(m_Mine);

		if (m_TickFrame != DEFAULTESTICK) {
			if ((80 - m_TickFrame) <=0)
				tmp_frame = 1;
			else 
				tmp_frame = 80 - m_TickFrame;
			tmp->SetFrameTick(tmp_frame);
		}
		// 시작 프레임 세팅
		if ((m_EffList[i].m_MinFrame) != ESEMPTY) {
			m_lstEff[i]->SetMinFrame(m_EffList[i].m_MinFrame);
			m_lstEff[i]->SetFrame(m_EffList[i].m_MinFrame);
		}
		// world effect 일때 flag setting
		if (m_bWorldEffect)
			m_lstEff[i]->SetWorldEffect(m_bWorldEffect);
	

	}
	

}
void CEffScript::SetChr(CZ3DGeneralChrModel *s,CZ3DGeneralChrModel *e) {
	int i;
	m_StartChr = s;
	m_EndChr = e;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetChr(s,e);
	}
	
}
void CEffScript::SetChrFwd(float x,float y,float z) {
	int i;
	m_ChrFwd.x = x;
	m_ChrFwd.y = y;
	m_ChrFwd.z = z;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetChrFwd(x,y,z);
	}
}
void CEffScript::SetExt4Ptr(CEffScript *ptr) { // extesin4 용 pointer setting
	int i;
	m_Ext4Ptr = ptr;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetExt4Ptr(ptr);
	}
}
void CEffScript::SetCast(bool t) {
	int i;
	m_bCast = t;
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetCast(t);
	}
}
void CEffScript::SetExt4SelfInfo(D3DXQUATERNION rot) {
	int i;
	m_bExt4InfoSet = true;

	m_Ext4Quat.x = rot.x;
	m_Ext4Quat.y = rot.y;
	m_Ext4Quat.z = rot.z;
	m_Ext4Quat.w = rot.w;
	for (i = 0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetExt4SelfInfo(rot);
	}

}
void CEffScript::SetExt4Rot(D3DXQUATERNION rot) {
	int i;
	m_Ext4Object = rot;
	for (i = 0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetExt4Rot(rot);
	}

}
void CEffScript::SetExt4Info(int pivotnum,D3DXQUATERNION rot) { //extension 4 용 data setting
	if (m_Ext4Ptr != NULL) {
		m_Ext4Ptr->m_Ext4Pivot = pivotnum;
		m_Ext4Ptr->m_Ext4Quat.x = rot.x;
		m_Ext4Ptr->m_Ext4Quat.y = rot.y;
		m_Ext4Ptr->m_Ext4Quat.z = rot.z;
		m_Ext4Ptr->m_Ext4Quat.w = rot.w;
		m_Ext4Ptr->m_bExt4InfoSet = true;
	}
}
void CEffScript::PlayTime() {			// 시간적용 되었을때를 위해  미리 준비
	
	float fCurrentTime = CSceneManager::GetWeatherTime();

	if (m_bPlayTime) {
		// 시간 돌아가는 순서를 알기 위해
		int sub = m_iStartPlayTime - m_iEndPlayTime;
		
		if (sub > 0 ) {	// 큰시간부터 돌아갈때 ex) 23~5
			if ( (fCurrentTime > (float)m_iEndPlayTime) && ( fCurrentTime < (float)m_iStartPlayTime))
				m_bVisTime = false;
			else
				m_bVisTime = true;
		}
		else {
			if ( (fCurrentTime >= (float)m_iStartPlayTime) && ( fCurrentTime <= (float)m_iEndPlayTime))			
				m_bVisTime = true;
			else
				m_bVisTime = false;
		}
		
	}
}
void CEffScript::SetScale(float fScale) {
	int i;
	
	if (m_EffList != NULL) {
		for ( i = 0;  i < m_EffNum; i++ ) {
			m_EffList[i].m_Scale = fScale;
		}
	}
	for (i=0;i<m_SubScriptNum;i++) {
		if (m_SubScript != NULL)
			m_SubScript[i].SetScale(fScale);
	}

}

CEffScript *CEffectCacheMgr::LoadData(const char *strFileName)
{
	CEffScript *pNode = new CEffScript;
	if (pNode->GetScriptRealBinData((char *)strFileName))
	{
		pNode->SetDevice(CSceneManager::GetDevice());


		unsigned long DataID = GetHashID(strFileName);
		EFFCACHEITER itr  = m_CacheTable.find(DataID);
		
		if (itr == m_CacheTable.end())
		{
			
			CEffCacheObj *pObj = new CEffCacheObj(pNode);
		
			m_CacheTable.insert(EFFCACHETABLEOBJ(DataID,pObj));
			m_iCachesNum++;
		}
		else 
		{// map 안에 똑같은 Hash Index 를 가지는 요소 존재
			
			((*itr).second)->Add_Back(pNode);	
		}

		return pNode;
	}
	else
	{
		delete pNode;
		return NULL;

	}
	
	return NULL;

}
CEffScript *CEffectCacheMgr::GetData(const char *strFileName)
{

	if (strlen(strFileName) < 3 || !strcmp(strFileName,"NULL"))
		return NULL;
	unsigned long DataID = GetHashID(strFileName);
		
	EFFCACHEITER itr  = m_CacheTable.find(DataID);
	
	if (itr == m_CacheTable.end()) 
	{
		return LoadData(strFileName);	
	}
	else	// Table 안에 이미 texture 가 로드되어 있을때 
	{
		if ((*itr).second != NULL) 
		{
			for (int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
			{
				if (!strcmp((*itr).second->m_lstDatas[iNum]->m_FileName,strFileName))
				{
					
					return (CEffScript *)((*itr).second->m_lstDatas[iNum]);				
				}
			}
			return LoadData(strFileName);	// Table 의 obj는 있지만 내가 원하는 obj 없을시
		}

	}
	
	return NULL;


}

void CEffectCacheMgr::InitLoad()
{
	if (!m_bInitLoadScript)
	{
		m_bFirst = false;
		char strEsfList[256] = {0};
		char strEsfName[256] = {0};

		sprintf(strEsfList,"%s\\Esf.info",EFFECTSCRIPTPATH);
		
		FILE *fp = fopen(strEsfList,"rt");
		
		if (fp == NULL)
			return;
		while(!feof(fp))
		{
			memset(strEsfName,0,sizeof(char) * 256);
			fgets(strEsfName,255,fp);
			unsigned int iLength = strlen(strEsfName);
			if (iLength > 3)
				strEsfName[iLength - 1] = '\0';
			GetData(strEsfName);

		}
		fclose(fp);
		m_bInitLoadScript = true;
	}
}
bool CEffScript::GetScriptBinData(char *bin_name,bool vis)
{
	if (strlen(bin_name) < 3 || !strcmp(bin_name,"NULL"))
		return NULL;
	CEffScript *pTmp = CSceneManager::ms_pEffectCacheMgr->GetData(bin_name);
	if (pTmp)
	{
		pTmp->Clone(this);
		m_bVisibility = vis;
	}
	return true;
}