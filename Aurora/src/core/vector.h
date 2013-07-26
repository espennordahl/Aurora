//
//  vector.h
//  Aurora
//
//  Created by Espen Nordahl on 22/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <ostream>
#include <assert.h>
#include <math.h>

#include "log.h"
#define lcontext LOG_Vector


namespace Aurora{
	
    class Point;
    
	class Vector{
	public:
		float x;
		float y;
		float z;
		
#define EPSILON_LENGTH 0.0001
        
#pragma mark -
#pragma mark Constructors
        
        /* constructors */
        explicit Vector(float _x, float _y, float _z):
        x(_x), y(_y), z(_z) {
                // check for NaNs
            assert(!hasNaNs());
        }
        
        explicit Vector( float f ):
        x(f), y(f), z(f)
        {
            assert(!hasNaNs());
        }
        
        explicit Vector(){
            Vector(0,1,0);
        }
        
        explicit Vector(const Point &p);
        
#pragma mark -
#pragma mark Handy Methods
        
        inline bool hasNaNs(){
            return (isnan(x) || isnan(y) || isnan(z));
        }
        
        inline bool isZero(){
            return !(x || y || z);
        }
        
        inline float lengthSquared() const{
            return x*x + y*y + z*z;
        }
        
        inline float length() const{
            float l = sqrtf(lengthSquared());
            if (l == 0.) {
//                if (x == 0. && y == 0. && z == 0.) {
//                    LOG_ERROR("Error: Vector 0 has no length");
//                }
//                else {
                    return EPSILON_LENGTH;
//                }
            }
            return l;
        }
        
#pragma mark -
#pragma mark Symbol Overloading
        
            //	addition
        inline Vector operator+(const Vector &v) const{
            return Vector(x+v.x, y+v.y, z+v.z);
        }
        
        inline Vector &operator+=(const Vector &v){
            x += v.x; y += v.y; z += v.z;
            return *this;
        }
        
            //	subtraction
        inline Vector operator-(const Vector &v) const{
            return Vector(x-v.x, y-v.y, z-v.z);
        }
        
        inline Vector &operator-=(const Vector &v){
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }
        
            //	multiplication
        inline const Vector operator*(float f) const{
            return Vector(f*x, f*y, f*z);
        }
        
        inline Vector &operator*=(float f) {
            x *= f; y *= f; z *= f;
            return *this;
        }
        
            //	division
        inline Vector operator/(float f) const{
                //	assert(f != 0);
            float inv = 1.f / f;
            return Vector(x * inv, y * inv, z * inv);
        }
        
        inline Vector operator/=(float f) {
                //	assert(f != 0);
            float inv = 1.f / f;
            x *= inv; y *= inv; z *= inv;
            return *this;
        }
        
            //	unary
        inline Vector operator-() {
            return Vector (-x, -y, -z);
        }
        
        inline float& operator[](int index){
            if (index == 0)
                return x;
            else if (index == 1)
                return y;
            else if (index == 2)
                return z;
            LOG_ERROR("Operator access out of range");
            return x;
        }
        
        inline const float& operator[](int index) const{
            if (index == 0)
                return x;
            else if (index == 1)
                return y;
            else if (index == 2)
                return z;
            LOG_ERROR("Operator access out of range");
            return x;
        }
        
            // comparison
        inline bool operator==(const Vector &v) const {
            return v.x == x && v.y == y && v.z == z;
        }
        
        inline bool operator!=(const Vector &v) const {
            return v.x != x || v.y == y || v.z == z;
        }
        
        friend std::ostream& operator<<(std::ostream &os, Vector const &v){
			os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
			return os;
            };
        
	};
}

#undef lcontext

#endif
