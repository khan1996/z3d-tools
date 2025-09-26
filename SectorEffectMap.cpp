// SectorEffectMap.cpp: implementation of the CSectorEffectMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorEffectMap.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSectorEffectMap::CSectorEffectMap()
{	
	Sector_EffectNum = 0;
	pick_index = -1;
}

CSectorEffectMap::~CSectorEffectMap()
{
	int i;
	for(i=0;i<(int)m_strEffectName.size();i++) {
		if(m_strEffectName[i])
			delete[] m_strEffectName[i];
	}

	m_strEffectName.clear();
	//for(i=0;i<Sector_EffectNum;i++) {
		
	//}
}

//DEL void CSectorEffectMap::Add_Effect(char *name, vector3 pos, int index_x, int index_y)
//DEL {		
//DEL 	CEffectMap tmp;
//DEL 	strcpy(tmp.FileName,name);
//DEL 	tmp.IndexX = index_x;
//DEL 	tmp.IndexY = index_y;
//DEL 	memcpy(&tmp.pos,&pos,sizeof(vector3));
//DEL 
//DEL 	m_Effect.Add(tmp);
//DEL 	m_vecEffectPos.Add(pos);
//DEL 	Effect_Num++;
//DEL 
//DEL }



//DEL bool CSectorEffectMap::EffectSectorIn(int x_index, int y_index,int i)
//DEL {
//DEL 
//DEL 	
//DEL 	if( m_Effect[i].IndexY == y_index && m_Effect[i].IndexX == x_index) 
//DEL 		return true;
//DEL 
//DEL 	return false;
//DEL }


