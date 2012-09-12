//
//  math.h
//  Aurora
//
//	abstract:
//	some handy math functions to operate on the geometry
//	and numeric classes.


#ifndef math_h
#define math_h

#include "point.h"
#include "vector.h"
#include "bbox.h"

#include <math.h>
#include <iostream>

namespace Aurora {

#define EPSILON 0.000001f
#define M_PI_INV 1.f/M_PI
#define M_2PI_INV 0.5/M_PI
#define INV_TWOPI  0.15915494309189533577f
	
#pragma mark -
#pragma mark Vectors

	inline float dot(const Vector &v1, const Vector &v2) {
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline float absDot(const Vector &v1, const Vector &v2) {
		return fabsf(dot(v1, v2));
	}

	inline Vector normalize(const Vector &v){
		return (v / v.length());
	}

	// Vector cross()
	// Left handed cross product of two Vectors
	inline Vector cross(const Vector &v1, const Vector &v2){
		return Vector((v1.y * v2.z) - (v1.z * v2.y),
						(v1.z * v2.x) - (v1.x * v2.z),
						(v1.x * v2.y) - (v1.y * v2.x));
	}
	
	// SampleHemisphereCosine()
	// Takes two floats in the range 0-1 and returns the cartesian
	// Vector representation.
	inline Vector SampleHemisphereCosine(float r1, float r2){
		float z = r1;
		float r = sqrtf(fmax(0.f, 1.f- z*z));
		float phi = M_PI * 2 * r2;
		float x = r * cosf(phi);
		float y = r * sinf(phi);
		
		return Vector(x,y,z);
	}
	
	inline Vector SampleHemisphereUniform(float r1, float r2) {
		float z = r1;
		float r = sqrtf(fmax(0.f, 1.f - z*z));
		float phi = 2 * M_PI * r2;
		float x = r * cosf(phi);
		float y = r * sinf(phi);
		
		return Vector(x, y, z);
	}

	// we need to pre declare this one
	void coordinateSystem(const Vector &v1, Vector *v2, Vector *v3);
	
	inline Vector tangentToWorld(const Vector &dir, const Vector &Nn){
		Vector dPdu;
		Vector dPdv;
		coordinateSystem(Nn, &dPdu, &dPdv);
		Vector RayDir = normalize(dPdu) * dir.x;
		RayDir = RayDir + (normalize(dPdv) * dir.y);
		RayDir = RayDir + (Nn * dir.z);

		return RayDir;
	}
	
	inline Vector worldToTangent(const Vector &dir, const Vector &Nn){
		Vector dPdu;
		Vector dPdv;
		coordinateSystem(Nn, &dPdu, &dPdv);
		return Vector(dot(dir, dPdu), dot(dir, dPdv), dot(dir, Nn));
	}
	
	inline Vector reflect(Vector wi, Vector Nn){
		return wi - (Nn*2*dot(wi, Nn));
	}
	
	inline Vector SphericalDirection(float sintheta, float costheta, float phi) {
		return Vector(sintheta * cosf(phi), sintheta * sinf(phi), costheta);
	}

#pragma mark -
#pragma mark Coordinate systems


	// void coordinateSystem ()
	// Takes three Pointers to Vectors and changes the two last
	// so they form a left handed coordinate system based on the first.
	inline void coordinateSystem(const Vector &v1, Vector *v2, Vector *v3){
		if (fabsf(v1.x) > fabsf(v1.y)) {
			float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
			*v2 = Vector(-v1.z * invLen, 0.f, v1.x * invLen);
		}
		else {
			float invLen = 1.f / sqrt(v1.y*v1.y + v1.z*v1.z);
			*v2 = Vector(0.f, v1.z * invLen, -v1.y * invLen);
		}
		*v3 = cross(v1, *v2);
	}


#pragma mark -
#pragma mark Points
		
	// float distance()
	// Computes the distance between two Points
	inline float distance(const Point &p1, const Point &p2) {
		return (p1 - p2).length();
	}
	
	// Point * Point for some reason needs to be declared outside
	// of the class.. TODO: Find out why this was needed
	inline Point operator*(float f, const Point &p ){
		return p*f;
	}
	
	
#pragma mark -
#pragma mark BBox

	// union()
	// returns a BBox that ecompasses a BBox and a Point.
	inline BBox Union( const BBox &b, const Point &p ){
		BBox ret = b;
		ret.pMin.x = std::min(b.pMin.x, p.x);
		ret.pMin.y = std::min(b.pMin.y, p.y);
		ret.pMin.z = std::min(b.pMin.z, p.z);
		ret.pMax.x = std::max(b.pMax.x, p.x);
		ret.pMax.y = std::max(b.pMax.y, p.y);
		ret.pMax.z = std::max(b.pMax.z, p.z);
		return ret;
	}
	
	
	// union()
	// combines two BBoxes to a new one, encompassing both
	inline BBox Union( const BBox &b, const BBox &b2 ){
		BBox ret = b;
		ret.pMin.x = std::min(b.pMin.x, b2.pMin.x);
		ret.pMin.y = std::min(b.pMin.y, b2.pMin.y);
		ret.pMin.z = std::min(b.pMin.z, b2.pMin.z);
		ret.pMax.x = std::max(b.pMax.x, b2.pMax.x);
		ret.pMax.y = std::max(b.pMax.y, b2.pMax.y);
		ret.pMax.z = std::max(b.pMax.z, b2.pMax.z);
		return ret;
	}

#pragma mark -
#pragma mark General handiness
	
	//	flerp
	//	interpolates between a and b, based on t.
	inline float flerp(float t, float a, float b){
		return a + t * (b - a);
	}
	
	
	//	quadratic()
	//  solves the quadratic equation, and returns t0 and t1 if solvable.
	inline bool Quadratic(float A, float B, float C, float *t0, float *t1) {
		// Find quadratic discriminant
		float discrim = B * B - 4.f * A * C;
		if (discrim < 0.) return false;
		float rootDiscrim = sqrtf(discrim);
		// Compute quadratic _t_ values
		float q;
		if (B < 0) q = -.5f * (B - rootDiscrim);
		else       q = -.5f * (B + rootDiscrim);
		*t0 = q / A;
		*t1 = C / q;
		if (*t0 > *t1) std::swap(*t0, *t1);
		return true;
	}

	//	equals, with an epsilon. Handy when dealing with numeric instability.
	inline bool softCompare(double a, double b)
	{
		return fabs(a - b) < EPSILON;
	}

	//	degrees to radians
	inline float radians(float degrees){
		return degrees * M_PI/180.f;
	}
	
	inline float clamp(float x, float _min, float _max){
		float result = x < _min ? _min : x;
		result = result > _max ? _max : result;
		return result;
	}
	
	inline float schlick(float VdotH, float reflectance){
		return reflectance + (1-reflectance)*(1-VdotH);
	}

	inline float log2(float x){
		float invLog2 = 1.f/logf(2.0);
		return logf(x) * invLog2;
	}
    
    enum IntegrationDomain{
        Hemisphere,
        Sphere
    };
	
}

#endif