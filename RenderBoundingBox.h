#pragma once

// forward decl.
struct IDirect3DDevice8;
struct vector3;



class CRenderBoundingBox
{
public:
	static void Render(IDirect3DDevice8* pDevice, const vector3 &vMin, const vector3 &vMax);

private:
	// 개체화시키지 않는 유틸리티클래스임을 명시화
	CRenderBoundingBox() {}
	~CRenderBoundingBox() {}
};
