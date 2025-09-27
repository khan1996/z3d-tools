#ifndef __EFFSCRIPTDEFINE_H__
#define __EFFSCRIPTDEFINE_H__


#define ESBUF_SIZE 256
#define ESEMPTY -1

#define ESSTART "INDEX"
#define ESEND "FIN:"
#define ESLIST "LIST:"
#define ESLOOP "LOOP:"
#define ESPLAY "PLAY:"
#define ESSNAP "SNAP:"
#define ESFADEOUT "OUT:"
#define ESFADEIN "IN:"
#define ESSHAKE "SHAKE:"
#define ESSOUND "SOUND:"
#define ESSUB "SUB:"
#define ESCLIGHT "CLIGHT:"
#define ESHIT "HIT:"
#define ESSLIDE "SLIDE:"
#define ESEXT5 "EXT5:"
#define ESLIGHTN "LIGHTNING:"
#define ESLIGHTN2 "LIGHTNING2:"
#define ESLIGHTN3 "LIGHTNING3:"
#define ESWHEEL "WHEEL:"
#define ESWEAPON "WEAPON:"
#define ESBEZIERM "GHOST:"
#define ESLIGHT "LIGHT:"
#define ESPLAYT "PLAYTIME:"
#define ESCHAR "CHARACTER:"
#define ESWEAPONL "WEAPONLINE:"	

#define ESPI        3.14159f
#define ESHALF_PI   (ESPI/2)
#define ESGRAVITY   9.806650f 

#define ESEXTLINESIZE 8.0f //line size  

//m_TickFrame deault 값
#define DEFAULTESTICK 40

enum EZVALUE {
	ESNORMAL, // normal effect
	ESINTERFACE, // interface effect
};

enum EDIR{
	EUP,
	ELEFT,
	ERIGHT,
	EFRONT,
	ERAND
};
enum ECURRENT{ // 파일에서 지금 읽어들이는 데이터 종류 
	ESS,	//start
	ESE,	// end
	EST,	// list
	ESL,	//loop
	ESP,	//play
	ESFO,	//FADE out
	ESFI,	//FADE IN
	ESSK,	// camera shake
	ESN,	// snap
	ESO,	// sound
	ESB,	// Sub
	ESCL,	// CLight
	ESH,	// Hit
	ESSL,	// slide 
	ESE5,	//ext5extension
	ESLN,	//lightning
	ESLN2,	//lightning2
	ESLN3,	//lightning3
	ESW,	// wheel
	ESWP,	// Weapon
	ESBM,	// Bezier Middle
	ESLT,	// Light
	ESPT,	// Play Time
	ESCH,	// Character
	ESWL,
};
enum EVALUE { // 어떤 것을 진행중인지..
	ED, // Delay frame
	ES, // S effect
	EM, // M effect
	EE, // E effect
	EN, //Snap effect
	EF, //finish effect
};
enum EPIVOT { // extension4 용 pivot index
	EHEAD,
	ENECK,
	ECHEST,
	EWAIST,
	EPELVIS,
	ER_HAND,
	ER_FOREARM,
	ER_UPPERARM,
	ER_THIGH,
	ER_CALF,
	ER_FOOT,
	EL_HAND,
	EL_FOREARM,
	EL_UPPERARM,
	EL_THIGH,
	EL_CALF,
	EL_FOOT,
};
#endif
