//
//  Vector.cpp
//  Aurora
//
//	Abstract:
//	Standard 3d Vector.


#include "vector.h"
#include "point.h"

using namespace Aurora;

Vector::Vector(const Point &p):
x(p.x), y(p.y), z(p.z)
{
}
