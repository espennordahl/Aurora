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

#include <stdio.h>
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
    
    template <typename T>
    inline int sign(T val) {
        return (T(0) < val) - (val < T(0));
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
//		if (fabsf(v1.x) > fabsf(v1.y)) {
//			float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
//			*v2 = Vector(-v1.z * invLen, 0.f, v1.x * invLen);
//		}
//		else {
//			float invLen = 1.f / sqrt(v1.y*v1.y + v1.z*v1.z);
//			*v2 = Vector(0.f, v1.z * invLen, -v1.y * invLen);
//		}
        if(!v1.x && v1.y && !v1.z){
            *v2 = cross(v1, Vector(1.f, 0.f, 0.f));
        } else {
            *v2 = cross(v1, Vector(0.f, 1.f, 0.f));
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
		return reflectance + powf(1.f-VdotH, 5.f) * (1.f-reflectance);
	}

	inline float log2(float x){
		float invLog2 = 1.f/logf(2.0);
		return logf(x) * invLog2;
	}
    
    enum IntegrationDomain{
        Hemisphere,
        Sphere
    };
    
    inline int floor( float x ) {
        return (int)x - ( x < 0 && (int)x != x );
    } 
    inline float smoothstep( const float & t ) { 
        return t * t * ( 3 - 2 * t ); 
    } 
    inline float mix(float a, float b, float z){
        return (1-z)*a + z*b;
    }

#pragma mark -
#pragma mark Noise
    
 // Perlin Noise Data
 #define NOISE_PERM_SIZE 256
 static int NoisePerm[2 * NOISE_PERM_SIZE] = {
     151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96,
     53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
     // Remainder of the noise permutation table
     8, 99, 37, 240, 21, 10, 23,
        190,  6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
        88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168,  68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
        77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
        102, 143, 54,  65, 25, 63, 161,  1, 216, 80, 73, 209, 76, 132, 187, 208,  89, 18, 169, 200, 196,
        135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,  3, 64, 52, 217, 226, 250, 124, 123,
        5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
        223, 183, 170, 213, 119, 248, 152,  2, 44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172, 9,
        129, 22, 39, 253,  19, 98, 108, 110, 79, 113, 224, 232, 178, 185,  112, 104, 218, 246, 97, 228,
        251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,  81, 51, 145, 235, 249, 14, 239, 107,
        49, 192, 214,  31, 181, 199, 106, 157, 184,  84, 204, 176, 115, 121, 50, 45, 127,  4, 150, 254,
        138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
        151, 160, 137, 91, 90, 15,
        131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
        190,  6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
        88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168,  68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
        77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
        102, 143, 54,  65, 25, 63, 161,  1, 216, 80, 73, 209, 76, 132, 187, 208,  89, 18, 169, 200, 196,
        135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,  3, 64, 52, 217, 226, 250, 124, 123,
        5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
        223, 183, 170, 213, 119, 248, 152,  2, 44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172, 9,
        129, 22, 39, 253,  19, 98, 108, 110, 79, 113, 224, 232, 178, 185,  112, 104, 218, 246, 97, 228,
        251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,  81, 51, 145, 235, 249, 14, 239, 107,
        49, 192, 214,  31, 181, 199, 106, 157, 184,  84, 204, 176, 115, 121, 50, 45, 127,  4, 150, 254,
        138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
 };
    
 inline float Grad(int x, int y, int z, float dx, float dy, float dz) {
     int h = NoisePerm[NoisePerm[NoisePerm[x]+y]+z];
     h &= 15;
     float u = h<8 || h==12 || h==13 ? dx : dy;
     float v = h<4 || h==12 || h==13 ? dy : dz;
     return ((h&1) ? -u : u) + ((h&2) ? -v : v);
 }
 inline float NoiseWeight(float t) {
     float t3 = t*t*t;
     float t4 = t3*t;
     return 6.f*t4*t - 15.f*t4 + 10.f*t3;
 }

}

#endif