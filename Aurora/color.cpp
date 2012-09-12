//
//  Color.cpp
//  Aurora
//
//  Created by Espen Nordahl on 13/05/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "constants.h"
#include "color.h"
#include <assert.h>
#include <math.h>


using namespace Aurora;

#pragma mark -
#pragma mark Constructors

Color::Color(){
	r=0; g=0; b=0;
}
Color::Color(float _r, float _g, float _b){
	r=_r; g=_g; b=_b;
}
Color::Color(float c){
	r=c; g=c; b=c;
}

Color::Color(const Vector &v){
	r = v.x, g = v.y, b = v.z;
}

Color::Color(const Point &p){
	r = p.x, g = p.y, b = p.z;
}

#pragma mark -
#pragma mark Operators

// Addition
Color Color::operator+(const Color &col){
	return Color(r+col.r, g+col.g, b+col.b);
}
Color &Color::operator+=(const Color &col){
	r += col.r; g += col.g; b += col.b;
	return *this;
}
Color Color::operator+(float f){
	return Color(r+f, g+f, b+f);
}
Color &Color::operator+=(float f){
	r += f; g += f; b += f;
	return *this;
}

// Subtraction
Color Color::operator-(const Color &col){
	return Color(r+col.r, g+col.g, b+col.b);
}
Color &Color::operator-=(const Color &col){
	r -= col.r; g -= col.g; b -= col.b;
	return *this;
}
Color Color::operator-(float f){
	return Color(r-f, g-f, b-f);
}
Color &Color::operator-=(float f){
	r -= f; g -= f; b -= f;
	return *this;
}

// Multiplication
Color Color::operator*(const Color &col){
	return Color(r*col.r, g*col.g, b*col.b);
}
Color &Color::operator*=(const Color &col){
	r *= col.r; g *= col.g; b *= col.b;
	return *this;
}
Color Color::operator*(float f){
	return Color(r*f, g*f, b*f);
}
Color &Color::operator*=(float f){
	r *= f; g *= f; b *= f;
	return *this;
}
Color Color::operator*(double d){
	return Color(r*d, g*d, b*d);
}
Color &Color::operator*=(double d){
	r *= d; g *= d; b *= d;
	return *this;
}

// Division
Color Color::operator/(const Color &col){
	return Color(r/col.r, g/col.g, b/col.b);
}
Color &Color::operator/=(const Color &col){
	r /= col.r; g /= col.g; b /= col.b;
	return *this;
}
Color Color::operator/(float f){
	float d = 1/f;
	return Color(r*d, g*d, b*d);
}
Color &Color::operator/=(float f){
	float d = 1/f;
	r *= d; g *= d; b *= d;
	return *this;
}

float &Color::operator[](int index){
	if (index == 0)
		return r;
	else if (index == 1)
		return g;
	else
		return b;
}

const float Color::operator[](int index) const{
	if (index == 0)
		return r;
	else if (index == 1)
		return g;
	else
		return b;
}

bool Color::isBlack(){
	return (r < ISBLACK && g < ISBLACK && b < ISBLACK);
}

bool Color::hasNaNs(){
	return (isnan(r) || isnan(g) || isnan(b));
}

float Color::lum(){
	return 0.2126*r + 0.7152*g + 0.0722*b; 
}