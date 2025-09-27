#pragma once

#pragma warning( disable : 4786 )
#include "../../include/Caldron.h"

#include "Mesh.h"
#include "Texture.h"


namespace Caldron
{
namespace Scene
{
	class CStateSet
	{
	public:


	};

	class CStateSetMgr
	{
	public:
		// 초기화( InitDevice 성 초기화)
		static bool _Init( IDirect3DDevice8* pDevice );

		// StateSet Manager 외부에서 d3d device 로 직접 시도된 state change 들을 검사해 device 와 StateSet Mgr 의 state를 동기화
		static void _GetCurrentDeviceState();

		// 지정된 StateSet 의 내용을 D3D Device 에 적용
		static void _ApplyStateSet( CStateSet* pStateSet );

	private:
		// 각 state 변수들
	};
}
}