#pragma once

// forward decl.
struct IDirect3DDevice8;
struct vector3;



class CRenderBoundingBox
{
public:
	static void Render(IDirect3DDevice8* pDevice, const vector3 &vMin, const vector3 &vMax);

private:
	// ��üȭ��Ű�� �ʴ� ��ƿ��ƼŬ�������� ���ȭ
	CRenderBoundingBox() {}
	~CRenderBoundingBox() {}
};
