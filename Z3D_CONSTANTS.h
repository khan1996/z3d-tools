// Z3D_CONSTANTS.h: constants & enums
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3D_CONSTANTS_H__3ADE1A0A_F343_43A2_B978_BEB6807C664A__INCLUDED_)
#define AFX_Z3D_CONSTANTS_H__3ADE1A0A_F343_43A2_B978_BEB6807C664A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



enum Z3D_CHR_ACTION
{
	// ���ŵ���
	Z3D_CA_WAIT = 0,	// ���
	Z3D_CA_WALK,		// �ȱ�
	Z3D_CA_WALK_BACK,	// �ڷΰȱ�
	Z3D_CA_RUN,			// �޸���
	Z3D_CA_CASTING_BEGIN,	// ĳ���ý���
	Z3D_CA_CASTING_END,		// ĳ���� ��
	Z3D_CA_DRINK_BEGIN,		// ���ñ� ����
	Z3D_CA_DRINKING,		// ���ñ�
	Z3D_CA_DRINK_END,		// ���ñ� ��
	Z3D_CA_EAT,				// �Ա�
	Z3D_CA_USE,				// �����ۻ��
	Z3D_CA_SLEEP_BEGIN,		// ����(�ڱ� ����)
	Z3D_CA_SLEEPING,		// ���ڱ�
	Z3D_CA_SLEEP_END,		// �ῡ�� ����(�ڱ� ��)
	Z3D_CA_ATTACK_BEGIN,	// ���� ����
	Z3D_CA_ATTACK_END,		// ���� ��
	Z3D_CA_ATTACK2_BEGIN,	// ����2 ����
	Z3D_CA_ATTACK2_END,		// ����2 ��
	Z3D_CA_ATTACK3_BEGIN,	// ����3 ����
	Z3D_CA_ATTACK3_END,		// ����3 ��
	Z3D_CA_PARRIED,			// �и�����
	Z3D_CA_WEAPON_PARRYING,	// ����� �и�
	Z3D_CA_SHIELD_PARRYING,	// ���з� �и�
	Z3D_CA_ATTACKED_BEGIN,	// ���ݴ��� ����
	Z3D_CA_ATTACKED_END,	// ���ݴ��� ��
	Z3D_CA_CRITICAL_ATTACKED_BEGIN,	// ũ��Ƽ�� ���� ����
	Z3D_CA_CRITICAL_ATTACKED_END,	// ũ��Ƽ�� ���� ��
	Z3D_CA_STUN,			// ����
	Z3D_CA_DIE_BACK,		// �ڷ� �ں��� ����
	Z3D_CA_DIE_FRONT,		// ������ �Ѿ��� ����
	Z3D_CA_RESURRECT_BACK,	// �ں��� ���¿��� ��Ȱ
	Z3D_CA_RESURRECT_FRONT,	// ������ ���¿��� ��Ȱ
	Z3D_CA_PUT_IN_WEAPON,	// ���⸦ ����ִ� ����
	Z3D_CA_RESTORE_HAND,	// ���⸦ ������� ���� ��⵿������ �̵�
	Z3D_CA_GRIP_WEAPON,		//	���� ������ �� ���� ������ġ�� �Ű� ��µ��۱���
	Z3D_CA_TAKE_OUT_WEAPON,	// ���Ⱑ �տ� ����(-_-;)�� ��⵿������ �ű�����
	Z3D_CA_SIT_DOWN,		// �ɱ�
	Z3D_CA_GET_UP,			// �Ͼ��

	// ��ü�� ����
	Z3D_CA_WAIT_LOWER,		// ���
	Z3D_CA_WALK_LOWER,		// �ȱ�
	Z3D_CA_WALK_BACK_LOWER,	// �ڷΰȱ�
	Z3D_CA_RUN_LOWER,		// �޸���

	// ��ü�� ����
	Z3D_CA_ATTACK_BEGIN_UPPER,
	Z3D_CA_ATTACK_END_UPPER,
	Z3D_CA_ATTACK2_BEGIN_UPPER,
	Z3D_CA_ATTACK2_END_UPPER,
	Z3D_CA_ATTACK3_BEGIN_UPPER,
	Z3D_CA_ATTACK3_END_UPPER,


	// �߰��ǰų� begin/end�� ����� ����. ����ĳ���� ���۱��� ���� �� �����Ұ���
	Z3D_CA_ATTACKED,
	Z3D_CA_FALLDOWN,
	Z3D_CA_CAST_BEGIN,
	Z3D_CA_CASTING,
	Z3D_CA_CAST_END,
	Z3D_CA_CAST_RETURN,
	Z3D_CA_ATTACK,
	Z3D_CA_ATTACK_SIDE_LEFT,
	Z3D_CA_ATTACK_SIDE_RIGHT,
	Z3D_CA_ATTACK_ADVANCE,
	Z3D_CA_ATTACK_RETREAT,

	// �߰��Ǿ����� ���� �����ȵ� ���۵�
	Z3D_CA_WALK_SIDE_LEFT,
	Z3D_CA_WALK_SIDE_RIGHT,
	Z3D_CA_RUN_SIDE_LEFT,
	Z3D_CA_RUN_SIDE_RIGHT,

	// skill motion
	Z3D_CA_BASH,
	

	Z3D_CA_MOTION_COUNT,	// ������ action���� ���� ����üũ��(���� ��� ����)

	// �� ������ action��(���º���, �׽�Ʈ�� ��..)
	Z3D_CA_NOTIFY_HIT_BY_FIREBALL, // ���̾�� �´�.

	Z3D_CA_MOTION_FINISHED,	// �������� notification(���� ������)

	Z3D_CA_BEGIN_SET_PART,
	Z3D_CA_END_SET_PART,
	
	Z3D_CA_SET_HEAD_PART,	// �Ӹ��κ�(����) ���� ����
	Z3D_CA_SET_BODY_PART,	// ��ü ���� ����
	Z3D_CA_SET_HAND_PART,	// ��(�尩) ���� ����
	Z3D_CA_SET_LEG_PART,	// �ٸ�(����) ���� ����
	Z3D_CA_SET_FOOT_PART,	// ��(�Ź�) ���� ����

	Z3D_CA_SET_BATTLE_MODE,	// �������� ��ȯ
	Z3D_CA_SET_NORMAL_MODE,	// �Ϲݸ��(���������)�� ��ȯ
	Z3D_CA_SET_SITDOWN_MODE,// �ɾ��ִ� ���� ��ȯ
	
	Z3D_CA_SET_LOD_LEVEL,	// LOD ���� ����
	
	Z3D_CA_WEAPON_TRAIL_ON,	// �˱� on
	Z3D_CA_WEAPON_TRAIL_OFF,// �˱� off

	Z3D_CA_WEAPON_FIRE_ON,	// ��Į on
	Z3D_CA_WEAPON_FIRE_OFF,	// ��Į off

	Z3D_CA_BODY_FLAME_ON,	// ���ſ��� �Ҳ� on
	Z3D_CA_BODY_FLAME_OFF,	// ���ſ��� �Ҳ� off

	Z3D_CA_EFFECT_HEALING,	// Healing effect
	Z3D_CA_EFFECT_CASTING,	// Casting effect
	Z3D_CA_EFFECT_BLEEDING,	// ���긮�� effect
	Z3D_CA_EFFECT_FIREBALL,	// Fireball effect
	Z3D_CA_EFFECT_RESPAWN,	// Respawn
	Z3D_CA_EFFECT_LEVELUP,	// Level up

	Z3D_CA_SET_FACE_TYPE,	// ��(���) ���� ����
	Z3D_CA_SET_HAIR_STYLE,	// ��Ÿ�� ���� ����
	Z3D_CA_SET_DEFAULT_TYPE,// �⺻��(�� �ϳ��� �� �������� �ع��� ����) ����
	
	Z3D_CA_TEST1,			// �׽�Ʈ�� 1,2,3 (������)
	Z3D_CA_TEST2,
	Z3D_CA_TEST3,


	// ���ο� ĳ���� Ŭ������ actions
	Z3D_CA_SET_MOTION_SHEET,	// motion sheet
	Z3D_CA_ACT_MOTION,			// ���ο� ����
	Z3D_CA_SET_STATIC_SLOT,		// static slot(��, ��Ÿ��)����
	Z3D_CA_BEGIN_OUTFIT_SET,	// slot ���� ����(���� ����)
	Z3D_CA_SET_OUTFIT_SLOT,		// slot ����
	Z3D_CA_END_OUTFIT_SET,		// slot ���� ����
	Z3D_CA_SET_WEAPON,			// ���� ����
	Z3D_CA_SET_SHIELD,			// ���� ����

	
	Z3D_CA_FORCE_DWORD = 0x7FFFFFFF
};

// ��ü�� ��ü�� ���۽�Ű�� ���� �Ķ����(lParam1 �� ����)
#define Z3D_CA_BOTH 0
#define Z3D_CA_UPPER 1
#define Z3D_CA_LOWER 2


enum Z3D_CHR_NOTIFICATION
{
	Z3D_CN_NOTHING = 0,

	Z3D_CN_HIT_BY_FIREBALL,

	Z3D_CN_FORCE_DWORD = 0x7FFFFFFF
};


enum Z3D_CHR_TYPE
{
	// ���� ����..
	Z3D_CT_MAN = 0, Z3D_CT_WOMAN,
		
	// unique NPC, monster �� �Ʒ��� �߰���~
	
	// monster~ (����+�߰���)
	Z3D_CT_MON_ELOYEE,
	Z3D_CT_MON_GOBLIN,
	Z3D_CT_MON_HIDDENEYE,
	Z3D_CT_MON_ORC,
	Z3D_CT_MON_GORGON,
	Z3D_CT_MON_FIELD,
	Z3D_CT_MON_BEAR,
	Z3D_CT_MON_DEADSUCKER,
	Z3D_CT_MON_YANG,
	Z3D_CT_MON_LIZARDMAN,
	Z3D_CT_MON_SKELETON,
	Z3D_CT_MON_ZOMBIE,
	Z3D_CT_MON_FRINCLE,
	Z3D_CT_MON_GOLEM,
	Z3D_CT_MON_CARRION,
	Z3D_CT_MON_PAWNPAWN,
	Z3D_CT_MON_AMPHIBIAN,
	Z3D_CT_MON_GOBLIN_WARRIOR,
	Z3D_CT_MON_DARK_WARRIOR,
	Z3D_CT_MON_LIZARDMAN_LEADER,
	Z3D_CT_MON_SKELETON_WARRIOR,
	Z3D_CT_MON_SLUG,
	Z3D_CT_MON_CHULPAWN,
	
	Z3D_CT_MON_GIANT_SPIDER,
	Z3D_CT_MON_NIGHT_WOLF,
	Z3D_CT_MON_WOLF,
	Z3D_CT_MON_ZOMBIE_DOG,
	Z3D_CT_MON_KILLER_BEETLE,
	Z3D_CT_MON_GRUB,
	Z3D_CT_MON_FLOATING_RAY,
	Z3D_CT_MON_BRUTAL_TRIBE,
	Z3D_CT_MON_THAITIG,
	Z3D_CT_MON_TROLL,
	Z3D_CT_MON_SEA_MAN,
	Z3D_CT_MON_GIANT_SQUIRREL,
	Z3D_CT_MON_DIRE_SQUIRREL,


	// NPC
	Z3D_CT_NPC_M_01,
	Z3D_CT_NPC_M_02,
	Z3D_CT_NPC_M_03,
	Z3D_CT_NPC_M_04,
	Z3D_CT_NPC_M_05,
	Z3D_CT_NPC_M_06,
	Z3D_CT_NPC_M_07,
	Z3D_CT_NPC_M_08,
	Z3D_CT_NPC_M_09,
	Z3D_CT_NPC_M_10,
	Z3D_CT_NPC_M_11,
	Z3D_CT_NPC_M_12,
	Z3D_CT_NPC_M_13,

	Z3D_CT_NPC_M_14,
	Z3D_CT_NPC_M_15,
	Z3D_CT_NPC_M_16,


	Z3D_CT_NPC_W_01,
	Z3D_CT_NPC_W_02,
	Z3D_CT_NPC_W_03,
	Z3D_CT_NPC_W_04,
	Z3D_CT_NPC_W_05,

	Z3D_CT_NPC_W_06,

	Z3D_CT_NPC_DONKEY,

	Z3D_CT_PC_AKHAN_A,
	
	Z3D_CT_FORCE_DWORD = 0x7FFFFFFF
};


enum Z3D_CHR_HAIR_STYLE
{
	Z3D_CHS_NONE = 0,	// ��Ӹ� �ֳ�-_-? .... �ֱ���-_-;;
	Z3D_CHS_LONG1,
	Z3D_CHS_LONG2,
	Z3D_CHS_LONG3,
	Z3D_CHS_LONG4,
	Z3D_CHS_LONG5,
	
	Z3D_CHS_SHORT1,
	Z3D_CHS_SHORT2,
	Z3D_CHS_SHORT3,
	Z3D_CHS_SHORT4,
	
	Z3D_CHS_TYPE1,
	Z3D_CHS_TYPE2,
	Z3D_CHS_TYPE3,
	Z3D_CHS_TYPE4,
	Z3D_CHS_TYPE5,
	Z3D_CHS_TYPE6,
	Z3D_CHS_TYPE7,
	Z3D_CHS_TYPE8,
	Z3D_CHS_TYPE9,
	
	Z3D_CHS_FORCE_DWORD = 0x7FFFFFFF
};


enum Z3D_CHR_FACE_TYPE
{
	Z3D_CFT_TYPE1 = 0,
	Z3D_CFT_TYPE2,
	Z3D_CFT_TYPE3,
	Z3D_CFT_TYPE4,
	Z3D_CFT_TYPE5,
	Z3D_CFT_TYPE6,
	Z3D_CFT_TYPE7,
	Z3D_CFT_TYPE8,
	Z3D_CFT_TYPE9,
	Z3D_CFT_TYPE10,
	
	Z3D_CFT_FORCE_DWORD = 0x7FFFFFFF
};

enum Z3D_CHR_DEFAULT_TYPE
{
	Z3D_CDT_TYPE1 = 0,
	Z3D_CDT_TYPE2,
	Z3D_CDT_TYPE3,
	/*Z3D_CDT_TYPE4,
	Z3D_CDT_TYPE5,
	Z3D_CDT_TYPE6,*/

	Z3D_CDT_FORCE_DWORD = 0x7FFFFFFF
};


enum Z3D_CHR_SKELETONPART
{
	Z3D_SP_NONE = 0,

	Z3D_SP_HEAD,

	Z3D_SP_NECK,
	Z3D_SP_CHEST,
	Z3D_SP_WAIST,
	Z3D_SP_PELVIS,
		
	Z3D_SP_R_HAND,
	Z3D_SP_R_FOREARM,
	Z3D_SP_R_UPPERARM,
	Z3D_SP_R_THIGH,
	Z3D_SP_R_CALF,
	Z3D_SP_R_FOOT,
	
	Z3D_SP_L_HAND,
	Z3D_SP_L_FOREARM,
	Z3D_SP_L_UPPERARM,
	Z3D_SP_L_THIGH,
	Z3D_SP_L_CALF,
	Z3D_SP_L_FOOT
};


enum Z3D_CHR_EQUIPMENT
{
	Z3D_CE_NONE = 0,
	Z3D_CE_ARBALEST,
	Z3D_CE_BATTLE_AXE,
	Z3D_CE_BROAD_SWORD,
	Z3D_CE_BUCKLER,
	Z3D_CE_CLAYMORE,
	Z3D_CE_COMPOSITE_BOW,
	Z3D_CE_CROSSBOW,
	Z3D_CE_CUTLAS,
	Z3D_CE_DART,
	Z3D_CE_DOUBLE_BLADED_AXE,
	Z3D_CE_FALCHION,
	Z3D_CE_GREAT_AXE,
	Z3D_CE_GREAT_SWORD,
	Z3D_CE_HAND_AXE,
	Z3D_CE_HEAVY_CROSSBOW,
	Z3D_CE_KITE_SHIELD,
	Z3D_CE_KNIFE,
	Z3D_CE_LONG_SPEAR,
	Z3D_CE_LONG_SWORD,
	Z3D_CE_LONGBOW,
	Z3D_CE_MACE,
	Z3D_CE_MILITARY_FORK,
	Z3D_CE_MORNING_STAR,
	Z3D_CE_PUNCHING_DAGGER,
	Z3D_CE_QUARTER_STAFF,
	Z3D_CE_RAPIER,
	Z3D_CE_SABER,
	Z3D_CE_SHARPEN_AXE,
	Z3D_CE_SHORT_SPEAR,
	Z3D_CE_SHORT_SWORD,
	Z3D_CE_SLING,
	Z3D_CE_STAFF,
	Z3D_CE_THROWING_DAGGER,
	Z3D_CE_TOWER_SHIELD,
	Z3D_CE_TRIDENT,
	Z3D_CE_TWO_HANDED_SWORD,
	Z3D_CE_VIKING_SWORD,
	Z3D_CE_WAR_HAMMER,
	
	Z3D_CE_COUNT,		// enumerator to count the number
	
	Z3D_CE_FORCE_DWORD = 0x7FFFFFFF
};


enum Z3D_CHR_OUTFIT // head, body, hand, leg, foot �ټ������� ������
{
	// �⺻��... ���߰�ħ����(body�� leg)�� ��� '����'�� ��Ÿ���⵵ �Ѵ�.
	Z3D_CO_DEFAULT = 0,  // head, body, hand, leg, foot ��ο� ��밡��
		
	Z3D_CO_DEFAULT_BODY, // ĳ���͸� ���� ���翡�� ������ ������, ����� �ε����� ���(?)
	Z3D_CO_DEFAULT_HAND,
	Z3D_CO_DEFAULT_LEG,
	Z3D_CO_DEFAULT_FOOT,
	
	// head - ��� Ÿ��(���� ��� �Ӹ�ī���巯��..�°���-_-? )
	Z3D_CO_LEATHER_HELMET,
	Z3D_CO_CHAIN_HELMET,
	Z3D_CO_KNIGHT_HELM,
	Z3D_CO_FULL_HELM,
	Z3D_CO_OPEN_HELM,
	Z3D_CO_CLOSED_HELM,
	
	// body
	Z3D_CO_LEATHER_SHIRTS,
	Z3D_CO_CHAIN_SHIRTS,
	Z3D_CO_SCALE_MAIL,
	Z3D_CO_LEATHER_TUNIC,
	Z3D_CO_CHAIN_TUNIC,
	Z3D_CO_PLATE_MAIL,
	Z3D_CO_SCALE_BREAST,
	Z3D_CO_PLATE_BREAST,

	Z3D_CO_SHIRTS_A, // �����߰� ���ڴ�� �۸� ����->�⺻������ �Űܿ�
	Z3D_CO_SHIRTS_B,
	Z3D_CO_SHIRTS_C,

	
	// hand	
	Z3D_CO_LEATHER_GLOVE,
	Z3D_CO_CHAIN_GLOVE,
	Z3D_CO_GAUNTLET,
	
	// leg
	Z3D_CO_TROUSERS,
	Z3D_CO_CHAIN_TROUSERS,
	Z3D_CO_PLATE_TROUSERS,
	Z3D_CO_PLATE_TROUSERS2,
	
	// foot	
	Z3D_CO_CHAIN_BOOTS,
	Z3D_CO_LEATHER_BOOTS,
	Z3D_CO_PLATE_BOOTS,
	
	//enumerator to count total available enum.s
	Z3D_CO_COUNT,
	
	Z3D_CO_FORCE_DWORD = 0x7FFFFFFF
};





#endif // !defined(AFX_Z3D_CONSTANTS_H__3ADE1A0A_F343_43A2_B978_BEB6807C664A__INCLUDED_)
