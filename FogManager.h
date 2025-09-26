#ifndef __CFOGMANAGER_H__
#define __CFOGMANAGER_H__

#pragma once

#include "FogScene.h"

class CFogManager {
public:
	CFogManager();
	~CFogManager();
	int SetFog(int num);
	
	int UpdateFog(int secx,int secy);
	int UpdateFog(int num);
	int FindFog(float ,float ,float );
	
	int Render();
	int Render(int num);
	void Insert_Fog();
	void CalculateLod();


	vector<CFogScene *> m_FogUnit;
	int m_FogNum;

private:

};

#endif