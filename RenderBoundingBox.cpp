#include "./RenderBoundingBox.h"

#include <d3d8.h>
#include <VECTOR.h>
#include <color.h>


void CRenderBoundingBox::Render(IDirect3DDevice8* pDevice, const vector3 &vMin, const vector3 &vMax)
{
	if (NULL == pDevice)
	{
		return;
	}

	const long BBRENDER_VERTEX_FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
	struct BoundingBoxRenderVertex
	{
		vector3 v;
		color diff;
	};

	static BoundingBoxRenderVertex aBoxVertex[8] =
	{
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)},
		{vector3(0, 0, 0), color(0xff, 0xff, 0xff, 0xff)}
	};

	static WORD awBoxIndex[24] = 
	{
		0, 1, 1, 3, 3, 2, 2, 0,
		4, 5, 5, 7, 7, 6, 6, 4,
		0, 4, 1, 5, 3, 7, 2, 6
	};

	static D3DMATRIX matIdentity = 
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	static D3DMATRIX matStore;


	aBoxVertex[0].v.x=vMax.x;
	aBoxVertex[0].v.y=vMax.y;
	aBoxVertex[0].v.z=vMax.z;	

	aBoxVertex[1].v.x=vMin.x;
	aBoxVertex[1].v.y=vMax.y;
	aBoxVertex[1].v.z=vMax.z;

	aBoxVertex[2].v.x=vMax.x;
	aBoxVertex[2].v.y=vMax.y;
	aBoxVertex[2].v.z=vMin.z;

	aBoxVertex[3].v.x=vMin.x;
	aBoxVertex[3].v.y=vMax.y;
	aBoxVertex[3].v.z=vMin.z;

	aBoxVertex[4].v.x=vMax.x;
	aBoxVertex[4].v.y=vMin.y;
	aBoxVertex[4].v.z=vMax.z;

	aBoxVertex[5].v.x=vMin.x;
	aBoxVertex[5].v.y=vMin.y;
	aBoxVertex[5].v.z=vMax.z;

	aBoxVertex[6].v.x=vMax.x;
	aBoxVertex[6].v.y=vMin.y;
	aBoxVertex[6].v.z=vMin.z;

	aBoxVertex[7].v.x=vMin.x;
	aBoxVertex[7].v.y=vMin.y;
	aBoxVertex[7].v.z=vMin.z;

	DWORD dwLightingState;
	pDevice->GetRenderState(D3DRS_LIGHTING, &dwLightingState);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//	pDevice->GetTransform(D3DTS_WORLD, &matStore);
//	pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	pDevice->SetTexture(0, NULL);
	pDevice->SetVertexShader(BBRENDER_VERTEX_FVF);
	pDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, awBoxIndex, D3DFMT_INDEX16, aBoxVertex, sizeof(BoundingBoxRenderVertex));
	pDevice->SetRenderState(D3DRS_LIGHTING, dwLightingState);
//	pDevice->SetTransform(D3DTS_WORLD, &matStore);
}