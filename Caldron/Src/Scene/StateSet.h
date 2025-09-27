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
		// �ʱ�ȭ( InitDevice �� �ʱ�ȭ)
		static bool _Init( IDirect3DDevice8* pDevice );

		// StateSet Manager �ܺο��� d3d device �� ���� �õ��� state change ���� �˻��� device �� StateSet Mgr �� state�� ����ȭ
		static void _GetCurrentDeviceState();

		// ������ StateSet �� ������ D3D Device �� ����
		static void _ApplyStateSet( CStateSet* pStateSet );

	private:
		// �� state ������
	};
}
}