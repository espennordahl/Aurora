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

namespace Aurora{
	
	class Vector{
	public:
		float x;
		float y;
		float z;
		
		Vector(float _x, float _y, float _z);
		Vector(float f);
		Vector();
	
		Vector operator+(const Vector &v) const;
		Vector &operator+=(const Vector &v);
		
		Vector operator-(const Vector &v) const;
		Vector &operator-=(const Vector &v);
		
		
		Vector operator*(float f) const;
		Vector &operator*=(float f);
		
		Vector operator/(float f) const;
		Vector operator/=(float f);
		
		Vector operator-();
		
		float& operator[](int index);
		const float& operator[](int index) const;
		
		float lengthSquared() const;
		float length() const;
		
		friend std::ostream& operator<<(std::ostream &os, Vector const &v){
			os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
			return os;
		};

	private:
		bool hasNaNs();
	};
}

#endif
