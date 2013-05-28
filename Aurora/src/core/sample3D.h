//
//  Sample.h
//  Aurora
//

#ifndef Sample3D_h
#define Sample3D_h

#include "ray.h"
#include "color.h"

namespace Aurora {
	class Sample3D {
	public:
		Ray ray;
		Color color;
		float pdf;
		
		Sample3D();
		Sample3D(const Ray &_Ray);
		Sample3D(const Ray &_Ray, float _pdf, const Color &col);
	};
}


#endif