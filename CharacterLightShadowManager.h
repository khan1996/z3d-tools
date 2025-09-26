// CharacterLightShadowManager.h: interface for the CCharacterLightShadowManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARACTERLIGHTSHADOWMANAGER_H__33851909_688F_4AB6_B1C8_A6A5D8F8011A__INCLUDED_)
#define AFX_CHARACTERLIGHTSHADOWMANAGER_H__33851909_688F_4AB6_B1C8_A6A5D8F8011A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "Z3DGeneralChrModel.h"
#include "Z3DAttachment.h"
#include "RenderTexture.h"
#include "HeightFieldScene.h"
#include "CollisionDetection.h"
//#include "Z3DSoundEventHelper.h"
#include "Z3DEventSignal.h"

#define EVENT_HIT 0x00000001<<0
#define EVENT_WEAPON_TRAIL_ON 0x00000001<<1
#define EVENT_WEAPON_TRAIL_OFF 0x00000001<<2
#define EVENT_JUMP 0x00000001<<3
#define EVENT_WALK 0x00000001<<4
#define EVENT_CANCEL 0x00000001<<5
#define EVENT_SHOT 0x00000001<<6
#define EVENT_JUST 0x00000001<<7
#define EVENT_JUSTSTART 0x00000001<<8
#define EVENT_JUSTEND 0x00000001<<9
#define EVENT_HIT_LEFT 0x00000001<<10
//CollisionDetection Type
#define CDT_NONE 0
#define CDT_FULL 1
#define CDT_ONLYTERRAIN 2
#define CDT_FIRSTBOTTOM 3
#define CDT_SKYUNIT 4

#define HOUSETYPENUM 8
const char * const strHouseName[HOUSETYPENUM] = {
	"wt_sg_a000.r3s",					// 길드 요새 완성
	"OuterCastle_FrontDoor.R3S",		// 성문
	"wt_cb_a000.r3s",					// 길드 요새 구축중
	"king_power_7000.r3s",				// 월드 웨폰 (카르테란트)
	"life_eui_GG_6000.r3s",				// 월드 웨폰 (메르카디아)
	"excavator_7000.r3s",				// 채굴기
	"wts_sg_a000.r3s",					// 요새 상점
	"OuterCastle_FrontDoor_Open.R3S"	// 성문 열린 상태
};
const char * const strMedHouseName[HOUSETYPENUM] = {
	"medwt_sg_a000.r3s",
	"",
	"",
	"",
	"",
	"",
	"wtsO_sg_a000.r3s",
	""
};
const char * const strInHouseName[HOUSETYPENUM] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};
class CCharacterLightShadowManager  
{
	CHeightFieldScene *m_pHeightField;	
	class CChrLightNode
	{
	public:
		D3DLIGHT8 m_Light;		
		int m_dwTimer;
	};
	DWORD m_dwCharacterPixelShader;
	DWORD m_dwCharacterVertexShader;
	DWORD m_dwCharacterSpecPixelShader;
	DWORD m_dwCharacterSpecularVertexShader;
	LPDIRECT3DTEXTURE8 m_pTex_DiffSpec_IlluminationMap;	
/*
//	static List<CollisionType> m_CharacterCollisionType;
//	static List<CZ3DGeneralChrModel*> m_CharacterList;
//	static List<CZ3DSoundEventHelper*> m_CharacterSoundHelperList;
//	static List<POINT> m_CharacterScreenPositionList;
//	static List<bool> m_CharacterAttackEventList;
//	static List<bool> m_CharacterTrailOnEventList;
//	static List<bool> m_CharacterTrailOffEventList;
//	static List<int> m_CharacterDelayTimer;
//	static List<CChrLightNode> m_CharacterLightList;
//	static List<vector3> m_CharacterPerFrameMove;
//	static List<vector3> m_CharacterRealPosition;
//	static List<int> m_CharacterRandomPostionTimer;
//	static List<vector3> m_CharacterRandomPostionAdder;
*/
	
public:		

	class CCharacterDataNode
	{
	public:
		CollisionType m_CollisionType;
		CZ3DGeneralChrModel *m_pChrmodel;
		POINT m_ptScreenPosition;
		DWORD m_dwEvent;
		int m_DelayTimer;
		CChrLightNode m_Light;
		vector3 m_vecPerFrameMove;
		float m_fAccelate;
		vector3 m_vecZeroVelocity;		
		vector3 m_vecRealPosition;
		int m_RandomPositionTimer;
		float m_fFallSpeed;
		vector3 m_vecRandomPositionAdder;
		bool m_bFirstAccelate;
		bool m_bCollisionDetectAble;
		bool m_bGravityAble;
		int m_CollisionDetectType;
		bool m_bRender;
		bool m_bHide;			// 캐릭터 Hide (충돌무시, 렌더링안함)
		bool m_bSkipCollision;	// 충돌처리만 안함.
		float m_fAlphaValue;

		int m_cAttackEvent;
		int m_cJumpEvent;
		int m_cWalkEvent;
		int m_cCancelEvent;
		int m_cShotEvent;
		int m_cJustEvent;
		int m_cJustStartEvent;
		int m_cJustEndEvent;
		bool m_bBuild;			// 건설 가능 flag
	};

	CCharacterDataNode GetCharacterDataNode(CZ3DGeneralChrModel *pChrmodel);
	void SetCharacterDataNode(CCharacterDataNode SetNode);
	void CharacterCollisionDetectType(CZ3DGeneralChrModel *pChrmodel,int nType);
	void CharacterGravityAble(CZ3DGeneralChrModel *pChrmodel,bool bGravityAble);
	static void CharacterCollisionAble(CZ3DGeneralChrModel *pChrmodel, bool bCollisionDetectionAble);
	void CharacterFirstAccelate(CZ3DGeneralChrModel *pChrmodel,bool bFirstAccelate);
	float GetCharacterVelocity(CZ3DGeneralChrModel *pChrmodel);
	CollisionType GetCharacterCollisionType(CZ3DGeneralChrModel *pChrmodel);
	static std::vector<CCharacterDataNode> m_CharacterList;
	
	void RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void CharacterRandomPos(CZ3DGeneralChrModel *pChrmodel,int nTimer,vector3 vecRand);
	vector3 GetCharacterMovementVector(CZ3DGeneralChrModel *pChrmodel);
	void CharacterMovementVector(CZ3DGeneralChrModel *pChrmodel,vector3 vecChrMove,float fAccelate=0.0f);
	void DelChrLight(DWORD dwLightID);
	void UpdateLight();
	void AddChrLight(CZ3DGeneralChrModel *pChrmodel,vector3 vecPos,DWORD dwColor,float fLens,DWORD dwTime);
	void CreateEditor();
	void AddDelayTimer(CZ3DGeneralChrModel *pChr,int nDelay);
	int GetCharacterEvent(CZ3DGeneralChrModel *pChrmodel,DWORD Event);
	void CharacterSoundPlay(CZ3DGeneralChrModel *pChrmodel, char *strSoundFile);
	void HeightFiledOnChrFixPos();
	CollisionType CharacterCollisionType(CZ3DGeneralChrModel* pFindCharacter);	

	static CRenderTexture m_pShadowTexture;//그림자	
	CTexture *m_pCharacterLight;// 밤용 light
	
	void CameraTest(LPDIRECT3DDEVICE8 pd3dDevice);	
	void SetHeightField(CHeightFieldScene *pHeightField){m_pHeightField=pHeightField;};
	
	void UpdateFrame(int m_nFocuesCharacter);
	void SetChrBuildEnable(bool bBuild, int nFocusCharacter = 0);
	bool GetChrBuildEnable(int nFocusCharacter = 0);
	void ActionProcess();

	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// flush accumulated events of all charaters register in character manager
	void FlushAccumulatedCharacterEvents();

	//CZ3DGeneralChrModel* AddCharacter( Z3D_CHR_TYPE ct, Z3D_CHR_FACE_TYPE ft, Z3D_CHR_HAIR_STYLE hs, Z3D_CHR_DEFAULT_TYPE dt );	
	CZ3DGeneralChrModel* AddCharacter( const char* szGCMDSName, const char* szFaceType, const char* szHairStyle ,int iValue = -1,CZ3DGeneralChrModel **ppOld = NULL,bool bHouse = false,int iHouseType = -1,D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f));	
	CZ3DGeneralChrModel* ReplaceCharacter( CZ3DGeneralChrModel* pChrOldModel, const char* szGCMDSName, vector3& vecPos = vector3(0.0f,0.0f,0.0f) );
	CZ3DGeneralChrModel* SwitchingModel(int iNum,CZ3DGeneralChrModel *pNew);
	
	//----------------------------------------------------------------------------------
	// by ichabod
	void				DeleteHouse( CZ3DGeneralChrModel* pDelChr ) ;
	CCharacterDataNode* GetCharacterDataNodeEx( CZ3DGeneralChrModel *pModel ) ;
	CCharacterDataNode  UndeleteListChr( CZ3DGeneralChrModel* pModel ) ;
	void			    UnallocListChr( CCharacterDataNode pModel, BOOL bFirst = FALSE ) ;
	int					GetCharacterDataNodeIndex( CZ3DGeneralChrModel *pModel ) ;
	VOID				SetNodeModel( CZ3DGeneralChrModel *pOrg, CZ3DGeneralChrModel *pNew ) ;
	void				SwapElement( CCharacterLightShadowManager::CCharacterDataNode Node1, 
									 CCharacterLightShadowManager::CCharacterDataNode Node2 ) ;
	//----------------------------------------------------------------------------------

	void DeleteCharacter(CZ3DGeneralChrModel* pDelChr);
	void DeleteElement(CZ3DGeneralChrModel* pDelChr);

	float GetCollisionChrCamera(vector3 vecStart,vector3 vecEnd);
	void HeightFieldShadowRender(LPDIRECT3DDEVICE8 pd3dDevice);
	void HeightFieldCharacterLightRender(LPDIRECT3DDEVICE8 pd3dDevice);

	void InHouseShadowRender(LPDIRECT3DDEVICE8 pd3dDevice);
	void CharacterLightSetting(int nChr,LPDIRECT3DDEVICE8 pd3dDevice);
	
	void Create();
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderGlow(IDirect3DDevice8* pDevice);
	void RenderEnv(LPDIRECT3DDEVICE8	pd3dDevice);

	CCharacterLightShadowManager();
	virtual ~CCharacterLightShadowManager();
};

#endif // !defined(AFX_CHARACTERLIGHTSHADOWMANAGER_H__33851909_688F_4AB6_B1C8_A6A5D8F8011A__INCLUDED_)
