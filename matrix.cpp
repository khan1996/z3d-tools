// matrix.cpp: implementation of the matrix class.
//
//////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include <math.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void matrix::XRotation(float theta)
{
	float c = (float) cos(theta);
	float s = (float) sin(theta);
	MakeIdent();
	_22 = c;
	_23 = s;
	_32 = -s;
	_33 = c;
}

void matrix::YRotation(float theta)
{
	float c = (float) cos(theta);
	float s = (float) sin(theta);
	MakeIdent();
	_11 = c;
	_13 = -s;
	_31 = s;
	_33 = c;
}

void matrix::ZRotation(float theta)
{
	float c = (float) cos(theta);
	float s = (float) sin(theta);
	MakeIdent();
	_11 = c;
	_12 = s;
	_21 = -s;
	_22 = c;
}

void matrix::MakeIdent()
{
	_11 = 1.f;	_22 = 1.f;	_33 = 1.f;	_44 = 1.f;
	_12 = 0.f;  _13 = 0.f;  _14 = 0.f;	_21 = 0.f;
	_23 = 0.f;	_24 = 0.f;	_31 = 0.f;	_32 = 0.f;	
	_34 = 0.f;	_41 = 0.f;	_42 = 0.f;	_43 = 0.f;
}

void matrix::AxisAngle(const vector3& inaxis, float angle)
{
	
	vector3 axis = inaxis.Normalized();
	float s=sinf(angle);
	float c=cosf(angle);
	float x=axis.x,y=axis.y,z=axis.z;	

	_11 = x*x*(1-c)+c;
	_21 = x*y*(1-c)-(z*s);
	_31 = x*z*(1-c)+(y*s);
	_41 = 0;
	_12 = y*x*(1-c)+(z*s);
	_22 = y*y*(1-c)+c;
	_32 = y*z*(1-c)-(x*s);
	_42 = 0;
	_13 = z*x*(1-c)-(y*s);
	_23 = z*y*(1-c)+(x*s);
	_33 = z*z*(1-c)+c;
	_43 = 0;
	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;
	
}

void matrix::CameraLookAt(const vector3 &loc, const vector3 &lookAt, const vector3 &inUp)
{
	
	vector3 viewVec=lookAt-loc;
	float mag=viewVec.Mag();
	viewVec/=mag;
	
	//vector3 viewVec=lookAt;

	float fDot=inUp*viewVec;

	vector3 upVec=inUp-fDot*viewVec;

	upVec.Normalize();

	vector3 rightVec=upVec^viewVec;

	_11 = rightVec.x;    _12 = upVec.x;    _13 = viewVec.x;
    _21 = rightVec.y;    _22 = upVec.y;    _23 = viewVec.y;
    _31 = rightVec.z;    _32 = upVec.z;    _33 = viewVec.z; 
    
    _41 = - (loc * rightVec);
    _42 = - (loc * upVec);
    _43 = - (loc * viewVec);

    _14 = 0;
    _24 = 0;
    _34 = 0;
    _44 = 1;    
}

void matrix::Translation(const vector3 &loc)
{
	MakeIdent();
	_41=loc.x;
	_42=loc.y;
	_43=loc.z;
}

void matrix::Inverse(const matrix &in)
{
	if( fabs(in._44 - 1.0f) > .001f)        
		return;
    if( fabs(in._14) > .001f || fabs(in._24) > .001f || fabs(in._34) > .001f )        
		return;
    float fDetInv = 1.0f / ( in._11 * ( in._22 * in._33 - in._23 * in._32 ) -
                             in._12 * ( in._21 * in._33 - in._23 * in._31 ) +
                             in._13 * ( in._21 * in._32 - in._22 * in._31 ) );

    _11 =  fDetInv * ( in._22 * in._33 - in._23 * in._32 );
    _12 = -fDetInv * ( in._12 * in._33 - in._13 * in._32 );
    _13 =  fDetInv * ( in._12 * in._23 - in._13 * in._22 );
    _14 = 0.0f;

    _21 = -fDetInv * ( in._21 * in._33 - in._23 * in._31 );
    _22 =  fDetInv * ( in._11 * in._33 - in._13 * in._31 );
    _23 = -fDetInv * ( in._11 * in._23 - in._13 * in._21 );
    _24 = 0.0f;

    _31 =  fDetInv * ( in._21 * in._32 - in._22 * in._31 );
    _32 = -fDetInv * ( in._11 * in._32 - in._12 * in._31 );
    _33 =  fDetInv * ( in._11 * in._22 - in._12 * in._21 );
    _34 = 0.0f;

    _41 = -( in._41 * _11 + in._42 * _21 + in._43 * _31 );
    _42 = -( in._41 * _12 + in._42 * _22 + in._43 * _32 );
    _43 = -( in._41 * _13 + in._42 * _23 + in._43 * _33 );
    _44 = 1.0f;

	/*
	_11 = in._11;
	_12 = in._21;
	_13 = in._31;
	_21 = in._12;
	_22 = in._22;
	_23 = in._32;
	_31 = in._13;
	_32 = in._23;
	_33 = in._33;
	
	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;

	// now get the new translation vector
	vector3 temp=in.GetLoc();	

	_41 = -(temp.x * in._11 + temp.y * in._12 + temp.z * in._13);
	_42 = -(temp.x * in._21 + temp.y * in._22 + temp.z * in._23);
	_43 = -(temp.x * in._31 + temp.y * in._32 + temp.z * in._33);
	*/
}

const vector3 matrix::GetLoc() const
{
	return vector3(_41,_42,_43);	
}

void matrix::MakeProjection(float fFov, float fAspect, float fNear, float fFar)
{
	if( fabs(fFar-fNear) <0.01f)
		return;
	if( fabs(sin(fFov/2)) < 0.01f)
		return;
	
	float w=fAspect*(cosf(fFov/2)/sinf(fFov/2));
	float h=1.0f*(cosf(fFov/2)/sinf(fFov/2));
	float Q=fFar/(fFar-fNear);    

    ZeroMemory( this, sizeof(matrix) );
    _11 = w;
    _22 = h;
    _33 = Q;
    _34 = 1.0f;
    _43 = -Q*fNear;    
}

matrix operator*(matrix const &a, matrix const &b)
{
    matrix out;      // temporary matrix4 for storing result
    for (int j = 0; j < 4; j++)         // transform by columns first
        for (int i = 0; i < 4; i++)     // then by rows
            out.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] +
                           a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
	return out;
};

vector3 operator*(matrix const &a,vector3 const &b)
{	
	vector3 vecResult;
	
	vecResult.x=a._11*b.x + a._12*b.y + a._13*b.z + a._14*1.0f;
	vecResult.y=a._21*b.x + a._22*b.y + a._23*b.z + a._24*1.0f;
	vecResult.z=a._31*b.x + a._32*b.y + a._33*b.z + a._34*1.0f;
	return vecResult;
}

vector3 operator*(vector3 const &v, matrix const &m)
{	
	vector3 vecResult;

	vecResult.x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	vecResult.y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	vecResult.z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;

	return vecResult;
}

bool operator==(matrix const &a,matrix const &b)
{
	if( a._11 == b._11 && a._12 == b._12 && a._13 == b._13 && a._14 == b._14 &&
		a._21 == b._21 && a._22 == b._22 && a._23 == b._13 && a._24 == b._24 &&
		a._31 == b._31 && a._32 == b._32 && a._33 == b._13 && a._34 == b._34 &&
		a._41 == b._41 && a._42 == b._42 && a._43 == b._13 && a._44 == b._44)
		return true;
	return false;
}


void matrix::Rotation(vector3 vecDir, float fAngle)
{
	float fCos=cosf(fAngle);
	float fSin=sinf(fAngle);
	vecDir.Normalize();
	vector3 v=vecDir;

    _11 = ( v.x * v.x ) * ( 1.0f - fCos ) + fCos;
    _12 = ( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin);
    _13 = ( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin);

    _21 = ( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin);
    _22 = ( v.y * v.y ) * ( 1.0f - fCos ) + fCos ;
    _23 = ( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin);

    _31 = ( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin);
    _32 = ( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin);
    _33 = ( v.z * v.z ) * ( 1.0f - fCos ) + fCos;

    _14 = _24 = _34 = 0.0f;
    _41 = _42 = _43 = 0.0f;
    _44 = 1.0f;
}

void matrix::MakeAdjustedProjectionMatrix(float fFov, float fAspect, float fNear, float fFar, float fPixDx, float fPixDy, float fVPWidth, float fVPHeight)
{
	float h =   1.0f  * ( cosf(fFov/2)/sinf(fFov/2) );
    float fTop = fNear/h;
    float fBottom = -fTop;
    float fRight = fTop * fAspect;
    float fLeft = -fRight;

    float fXWSize = fRight - fLeft;
    float fYWSize = fTop - fBottom;

    float fDx = -( fPixDx*fXWSize/fVPWidth );
    float fDy = -( fPixDy*fYWSize/fVPHeight );
    
    SetFrustumMatrix(fLeft+fDx, fRight+fDx, fTop+fDy, fBottom+fDy, fNear,fFar);
	
}

void matrix::SetFrustumMatrix(float fLeft, float fRight, float fTop, float fBottom, float fNearPlane, float fFarPlane)
{
	float Q = fFarPlane / ( fFarPlane - fNearPlane );
	ZeroMemory(this,sizeof(matrix));
	_11 = ( 2.0f*fNearPlane )/( fRight - fLeft );
    _22 = ( 2.0f*fNearPlane )/( fTop - fBottom );
    _31 = ( fRight + fLeft )/ (fRight - fLeft );
    _32 = ( fTop + fBottom )/ (fTop - fBottom );
    _33 = Q;
    _34 = 1.0f;
    _43 = -Q*fNearPlane;    
}

void matrix::MakeNoProjection(float fFov, float fSizeX, float fSizeY, float fFarPlane, float fNearPlane)
{
	ZeroMemory(this,sizeof(matrix));

	_11=2.0f/fSizeX;
	_22=2.0f/fSizeY;
	_33=1.0f/(fFarPlane-fNearPlane);
	_43=-1.0f*fNearPlane/(fFarPlane-fNearPlane);
	_44=1.0f;
}

void matrix::Transpose()
{
	float t_12,t_13,t_14,t_23,t_24,t_34;

	t_12=_12;
	t_13=_13;
	t_14=_14;
	t_23=_23;
	t_24=_24;
	t_34=_34;

	_12=_21;
	_13=_31;
	_14=_41;
	_23=_32;
	_24=_42;
	_34=_43;	
	
	_21=t_12;
	_31=t_13;
	_41=t_14;
	_32=t_23;
	_42=t_24;
	_43=t_34;	
}
