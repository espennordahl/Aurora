//
//  ashikhmin.cpp
//  Aurora
//
//  Created by Espen Nordahl on 12/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "ashikhmin.h"

using namespace Aurora;

Ashikhmin::Ashikhmin(Color col, float exp){
	color = col;
	expX = exp;
	expY = exp;
    integrationDomain = Hemisphere;
    brdfType = SpecBrdf;
}

float Ashikhmin::calcPhi(float nu, float nv, float r1){
	return atan(sqrt((nu+1)/(nv+1)) * tan(M_PI*r1/2));
}

Sample3D Ashikhmin::getSample(const Vector &Vn){
	float ph;
	float r1 = (float) rand()/RAND_MAX;
	float r2 = (float) rand()/RAND_MAX;
	// from random page online.. not sure I trust it..
	if (r1 >= 0 && r1 < 0.25)
		ph = calcPhi(expX, expY, 1 - 4*(0.25 - r1));
	else if (r1 >= 0.25 && r1 < 0.5)
		ph = calcPhi(expX, expY, 1 - 4*(0.5 - r1)) + M_PI/2;
	else if (r1 >= 0.5 && r1 < 0.75)
		ph = calcPhi(expX, expY, 1 - 4*(0.75 - r1)) + M_PI;
	else
		ph = calcPhi(expX, expY, 1 - 4*(1 - r1)) - M_PI/2;
	 
	float cosph = cos(ph);
	float sinph = sin(ph);
	float th = acos(pow(1-r2, 1/(expX*cosph*cosph+expY*sinph*sinph+1)));
	     
	Vector u = Vector(1,0,0);
	Vector v = Vector(0,1,0);
	Vector Nn = Vector(0,0,1);
	Vector H = u*cosph*sin(th);
	H += v*sinph*sin(th);
	H += Nn*cos(th);
		 
	H = normalize(H);
	float hdotn = dot(H,Nn);
	float hdotu = dot(H,u);
	float hdotv = dot(H,Vn);
		
	float exponent = (expX*hdotu*hdotu + expY*hdotv*hdotv)/(1-hdotn*hdotn);
		 
	float pdfh = sqrt((expX + 1) * (expY + 1))/(2*M_PI);
	pdfh *= pow(hdotn, exponent);
		
	return Sample3D(Ray(H, Point(0), RAY_BIAS, 1000000.f), pdfh, evalSampleTangent(H, Vn));
}

void Ashikhmin::sampleFirstQuadrant(float u1, float u2, float *phi, float *costheta) const{
	if (expX == expY)
		*phi = M_PI * u1 * 0.5f;
	else
		*phi = atanf(sqrtf((expX+1.f) / (expY+1.f)) * tanf(M_PI * u1 * 0.5f));
	float cosphi = cosf(*phi), sinphi = sinf(*phi);
	*costheta = powf(u2, 1.f/(expX * cosphi * cosphi + expY * sinphi * sinphi + 1));
}

Color Ashikhmin::evalSampleTangent(const Vector &Ln, const Vector &Vn){
	Vector Nn = Vector(0,0,1);
	Vector Hn = Vn + Ln;
	if (Hn.x == 0. && Hn.y == 0. && Hn.z == 0.) return Color(0.f);
	Hn = normalize(Hn);
	
	Vector v;
	Vector u;
	coordinateSystem(Nn, &u, &v);
	u = normalize(u);
	v = normalize(v);
	float hdotu = dot(Hn,u);
	float hdotv = dot(Hn,v);
	float hdotn = dot(Hn,Nn);
	float hdotl = dot(Hn,Ln);
	float ndotv = dot(Nn,Vn);
	float ndotl = dot(Nn,Ln);
	
	float exponent = (expX*hdotu*hdotu + expY*hdotv*hdotv)/(1-hdotn*hdotn);
	
	return color * (pow(hdotn,exponent)/(hdotl*fmax(ndotl, ndotv)));
}


Color Ashikhmin::evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn){
	Vector Hn = Vn + Ln;
	if (Hn.x == 0. && Hn.y == 0. && Hn.z == 0.) return Color(0.f);
	Hn = normalize(Hn);
	
	Vector v;
	Vector u;
	coordinateSystem(Nn, &u, &v);
	u = normalize(u);
	v = normalize(v);
	float hdotu = dot(Hn,u);
	float hdotv = dot(Hn,v);
	float hdotn = dot(Hn,Nn);
	float hdotl = dot(Hn,Ln);
	float ndotv = dot(Nn,Vn);
	float ndotl = dot(Nn,Ln);
	
	float exponent = (expX*hdotu*hdotu + expY*hdotv*hdotv)/(1-hdotn*hdotn);
	 
	float c = sqrt((expX + 1) * (expY + 1))/(8*M_PI);

	
	return color * c * (pow(hdotn,exponent)/(hdotl*fmax(ndotl, ndotv)));

}
