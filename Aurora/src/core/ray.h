//
//  ray.h
//  Aurora
//
//	Abstract:
//
//	Describes a Ray to be used in sampling the scene.

#ifndef Ray_h
#define Ray_h

#include "vector.h"
#include "point.h"

namespace Aurora {
    
    enum RayType{
        CameraRay,
        DiffuseRay,
        SpecularRay,
        MirrorRay,
        ShadowRay
    };
    
	class Ray{
	public:
		Vector direction;
		Point origin;
		float	mint;
		float	maxt;
	
		Ray();
		Ray(const Vector &dir, const Point &orig, float min, float max);
	};
}

#endif
