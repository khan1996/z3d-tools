// matrix.h: interface for the matrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__D1E5A204_11BB_4B9E_902D_D684D3D34E3B__INCLUDED_)
#define AFX_MATRIX_H__D1E5A204_11BB_4B9E_902D_D684D3D34E3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <d3dx8.h>
#include "MathBase.h"
#include "vector.h"

//#include <math.h>
//#include "vector.h"

struct matrix  
{
	union 
	{
        struct 
		{
            float	_11, _12, _13, _14;
            float	_21, _22, _23, _24;
            float	_31, _32, _33, _34;
            float	_41, _42, _43, _44;
        };
        float	m[4][4];
    };
	matrix(){};

	matrix(
		float in11, float in12, float in13, float in14,
        float in21, float in22, float in23, float in24,
        float in31, float in32, float in33, float in34,
        float in41, float in42, float in43, float in44 )
	{
        _11 = in11; _12 = in12; _13 = in13; _14 = in14;
        _21 = in21; _22 = in22; _23 = in23; _24 = in24;
        _31 = in31; _32 = in32; _33 = in33; _34 = in34;
        _41 = in41; _42 = in42; _43 = in43; _44 = in44;
	};
	static matrix Identity;
public:
	void Transpose();
	void MakeNoProjection(float fFov,float fSizeX,float fSizeY,float fFarPlane,float fNearPlane);
	void SetFrustumMatrix(float fLeft,float fRight,float fTop,float fBottom,float fNearPlane,float fFarPlane );
	void MakeAdjustedProjectionMatrix(float fFov,float fAspect,float fNear,float fFar,float fPixDx,float fPixDy,float fVPWidth,float fVPHeight);
	void Rotation(vector3 vecDir,float fAngle);
	void MakeProjection(float fFov,float fAspect,float fNear,float fFar);
	const vector3 GetLoc() const;
	void Inverse(const matrix& in);
	void Translation(const vector3 &loc);
	void CameraLookAt( const vector3& loc, const vector3& lookAt, const vector3& inUp );
	void AxisAngle(const vector3& inaxis,float angle);
	void MakeIdent();
	void ZRotation(float theta);
	void YRotation(float theta);
	void XRotation(float theta);
	operator D3DMATRIX* ()
	{
		return (D3DMATRIX*)(this);
	}		
	operator D3DXMATRIX* ()
	{
		return (D3DXMATRIX*)(this);
	}
};

matrix operator*(matrix const &a, matrix const &b);
vector3 operator*(matrix const &a,vector3 const &b);	// D3D 체계 기준에서는 맞지않는 연산
vector3 operator*(vector3 const &v, matrix const &m);	// 이 연산으로 대체

bool operator==(matrix const &a,matrix const &b);

inline void Multiply( matrix& result, matrix& a, matrix& b )
{
	int i, j;
	for (j = 0; j < 4; j++)         // transform by columns first
		for (i = 0; i < 4; i++)     // then by rows
			result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] +
						   a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
}
#endif // !defined(AFX_MATRIX_H__D1E5A204_11BB_4B9E_902D_D684D3D34E3B__INCLUDED_)
