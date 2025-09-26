// VECTOR.h: interface for the VECTOR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__10479201_0625_493F_9ABD_2DB66EC951B3__INCLUDED_)
#define AFX_VECTOR_H__10479201_0625_493F_9ABD_2DB66EC951B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathBase.h"

#include "FastMath.h"

#pragma warning( disable : 4786 )

#define vector3mul(a,b) a.x*b.x+a.y*b.y+a.z*b.z

struct vector3
{
	union
	{
		struct 
		{
			float x,y,z;
		};
		float v[3];
	};
	vector3(){};
	vector3(float xp,float yp,float zp)
	{
		x=xp;
		y=yp;
		z=zp;
	};	
	vector3& operator+=(const vector3& in);
	vector3& operator-=(const vector3& in);
	vector3& operator*=(const float& in);
	vector3& operator/=(const float& in);	
	vector3 operator-();
	// Return the magnitude of a point
	float Mag() const;
	void Normalize() ;
	vector3 Normalized() const;
	float GetLens()
	{
		float fTest=x*x+y*y+z*z;		
		/*
		if(fabsf(fTest)<=0.001f)
		{
			return 0.0f;
		}
		*/
		
		return sqrtf(fTest);
		//return CFastMath::FastSqrt(x*x+y*y+z*z);
	};
	void Identity() { x = 0.0f; y = 0.0f; z = 0.0f; };
	/*
	static const vector3 Zero;
	static const vector3 i;
	static const vector3 j;
	static const vector3 k;
	*/
};
/*
const vector3 vector3::Zero( 0.f,0.f,0.f );
const vector3 vector3::i( 1.f, 0.f, 0.f );
const vector3 vector3::j( 0.f, 1.f, 0.f );
const vector3 vector3::k( 0.f, 0.f, 1.f );
*/

inline vector3& vector3::operator+=(const vector3& in)
{
	x+=in.x;
	y+=in.y;
	z+=in.z;
	return *this;
}
inline vector3& vector3::operator-=(const vector3& in)
{
	x-=in.x;
	y-=in.y;
	z-=in.z;
	return *this;

}

inline vector3& vector3::operator*=(const float& in)
{
	x*=in;
	y*=in;
	z*=in;
	return *this;
}

inline vector3& vector3::operator/=(const float& in)
{
	float inv=1.0f/in;
	x*=inv;
	y*=inv;
	z*=inv;
	return *this;
}
inline vector3 vector3::operator -()
{
	return vector3(-x,-y,-z);
}

inline const vector3 operator+(vector3 const &a,vector3 const &b)
{
	return vector3(a.x+b.x,a.y+b.y,a.z+b.z);
}
inline const vector3 operator-(vector3 const &a,vector3 const &b)
{
	return vector3(a.x-b.x,a.y-b.y,a.z-b.z);
}
inline const vector3 operator*(vector3 const &a,float const &b)
{
	return vector3(a.x*b,a.y*b,a.z*b);
}
inline const vector3 operator*(float const &a,vector3 const &b)
{
	return vector3(a*b.x,a*b.y,a*b.z);
}
inline const vector3 operator/(vector3 const &a,float const &b)
{
	float inv=1.0f/b;
	return vector3(a.x*inv,a.y*inv,a.z*inv);
}
inline const vector3 operator^(vector3 const &a,vector3 const &b)
{
	return vector3( (a.y*b.z-a.z*b.y),
				   (a.z*b.x-a.x*b.z),
				   (a.x*b.y-a.y*b.x));
}
inline const float operator*(vector3 const &a,vector3 const &b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
inline bool operator==(vector3 const &a,vector3 const &b)
{
	if(fabs(a.x-b.x)<EPSILON)
	{
		if(fabs(a.y-b.y)<EPSILON)
		{
			if(fabs(a.z-b.z)<EPSILON)
			{
				return true;
			}
		}
	}
	return false;
}

inline float vector3::Mag() const
{
	//x*b.x+a.y*b.y+a.z*b.z
	//return CFastMath::FastSqrt( x*x+y*y+z*z );
	//vector3mul
	float fTest=x*x+y*y+z*z;
	/*
	if(fabsf(fTest)<=0.01f)
	{
		return 0.0f;
	}
	*/
	return sqrtf(fTest);	
}
inline void vector3::Normalize()
{
	float fMag=Mag();	
	/*if(fMag<=0.001f)
		return;	
	*/
	if(fMag <= 0.00000001f)
	{
		(*this).x = 0.0f;
		(*this).y = 0.0f;
		(*this).z = 0.0f;
		return;

	}
	float invMag = 1.0f/fMag;	

	//float invMag = 1.0/sqrt(x*x+y*y+z*z);
	//float Mag=sqrt(x*x+y*y+z*z);
	//float invMag=1.0/Mag;

	(*this) *= invMag;	
}
inline vector3 vector3::Normalized() const
{
	float fMag = Mag();
	if(fMag <= 0.00000001f)
		return vector3(0.0f,0.0f,0.0f);

	float invMag = 1.f/fMag;
	
	return (*this) * invMag;
}
#endif // !defined(AFX_vector3_H__10479201_0625_493F_9ABD_2DB66EC951B3__INCLUDED_)

