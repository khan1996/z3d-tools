#ifndef _COLOR_H
#define _COLOR_H

#pragma once 

#include "MathBase.h"


#pragma warning( disable : 4244 )  

struct color
{
	union{
		struct
		{
			 COLORVALUE b,g,r,a;
		};
		long c;
	};
	color(){};
	color(COLORVALUE inr,COLORVALUE ing,COLORVALUE inb,COLORVALUE ina=255)
	{
		r=inr;
		g=ing;
		b=inb;
		a=ina;
	};
	static color Interpolation(color FirstColor,color LastColor,float fInterval)
	{
		color resultColor;
		if(fInterval>1.0f)
			fInterval=fInterval-(int)fInterval;
		int ElementColor;
		int IntervalColor;

		ElementColor=LastColor.r-FirstColor.r;
		IntervalColor=((float)ElementColor*fInterval);		
		resultColor.r=FirstColor.r+IntervalColor;

		ElementColor=LastColor.g-FirstColor.g;
		IntervalColor=((float)ElementColor*fInterval);		
		resultColor.g=FirstColor.g+IntervalColor;

		ElementColor=LastColor.b-FirstColor.b;
		IntervalColor=((float)ElementColor*fInterval);		
		resultColor.b=FirstColor.b+IntervalColor;

		ElementColor=LastColor.a-FirstColor.a;
		IntervalColor=((float)ElementColor*fInterval);		
		resultColor.a=FirstColor.a+IntervalColor;
		return resultColor;
	};
	color& operator += ( const color& in );
	color& operator -= ( const color& in );
	color& operator *= ( const color& in );
	color& operator /= ( const color& in );
	color& operator *= ( const float& in );
	color& operator /= ( const float& in );
};

inline color& color::operator += ( const color& in )
{
	r += in.r;   
	g += in.g;   
	b += in.b;
	a += in.a;
	return *this;
}
inline color& color::operator -= ( const color& in )
{
	r -= in.r;   
	g -= in.g;   
	b -= in.b;
	a -= in.a;
	return *this;
}
inline color& color::operator *= ( const color& in )
{
	r *= in.r;   
	g *= in.g;   
	b *= in.b;
	a *= in.a;
	return *this;
}
inline color& color::operator /= ( const color& in )
{
	r /= in.r;   
	g /= in.g;   
	b /= in.b;
	a /= in.a;
	return *this;
}
inline color& color::operator *= ( const float& in )
{
	r *= in;   
	g *= in;   
	b *= in;
	a *= in;
	return *this;
}
inline color& color::operator /= ( const float& in )
{
	float inv = 1.f / in;
	r *= inv;   
	g *= inv;   
	b *= inv;
	a *= inv;
	return *this;
}

inline color operator+(color const &a, color const &b)
{
	COLORVALUE inr,ing,inb,ina;
	if(a.r+b.r > 0xff)
		inr=0xff;
	else
		inr=a.r+b.r;

	if(a.g+b.g > 0xff)
		ing=0xff;
	else
		ing=a.g+b.g;

	if(a.b+b.b > 0xff)
		inb=0xff;
	else
		inb=a.b+b.b;

	if(a.a+b.a > 0xff)
		ina=0xff;
	else
		ina=a.a+b.a;

	return color(inr,ing,inb,ina);	
}; 

inline color operator-(color const &a, color const &b)
{
	return color
	(
		a.r-b.r,
		a.g-b.g,
		a.b-b.b,
		a.a-b.a
	);
}; 
inline color operator*(color const &a, float const &b)
{
	return color
	(
		a.r*b,
		a.g*b,
		a.b*b,
		a.a*b
	);
}; 


/**
 * Scales a color by a float : ret = a * b
 */
inline color operator*(float const &a, color const &b)
{
	return color
	(
		a*b.r,
		a*b.g,
		a*b.b,
		a*b.a
	);
}; 


/**
 * Divides a color by a float : ret = a / b
 */
inline color operator/(color const &a, float const &b)
{
	float inv = 1.f / b;
	return color
	(
		a.r*inv,
		a.g*inv,
		a.b*inv,
		a.a*inv
	);
}; 

/**
 * Divides a color by a color (piecewise): ret = a / b
 */
inline color operator/(color const &a, color const &b)
{
	return color
	(
		a.r / b.r,
		a.g / b.g,
		a.b / b.b,
		a.a / b.a
	);
}; 

/**
 * Multiplies a color by a color (piecewise): ret = a / b
 */
inline color operator*(color const &a, color const &b)
{
	return color
	(
		a.r * b.r,
		a.g * b.g,
		a.b * b.b,
		a.a * b.a
	);
}; 


/**
 * color Equality, epsilon used due to numerical imprecision
 */
inline bool operator==(color const &a, color const &b)
{
	if(abs(a.r-b.r)<EPSILON)
		if(abs(a.g-b.g)<EPSILON)
			if(abs(a.b-b.b)<EPSILON)
				if(abs(a.a-b.a)<EPSILON)
					return true;
	return false;
};

#endif 

