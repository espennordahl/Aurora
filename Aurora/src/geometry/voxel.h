//
//  voxel.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_voxel_h
#define Aurora_voxel_h

#include "renderableTriangle.h"
#include "Ray.h"
#include "Intersection.h"

#include <vector>

namespace Aurora {
	class Voxel{
	public:
		Voxel(){};
		Voxel(RenderableTriangle tri){ objects.push_back(tri); };
		
		std::vector<RenderableTriangle> objects;
		
		void addObject(RenderableTriangle object){
			objects.push_back(object);
		}
		
		bool intersect( Ray *ray, Intersection *intersection) const{
			bool hitSomething = false;
			for (int i=0; i < objects.size(); i++) {
				if (objects[i].intersect(ray, intersection)) {
					hitSomething = true;
				}
			}
			return hitSomething;
		}
		
		bool intersectBinary( Ray *ray ) const{
			for (int i=0; i < objects.size(); i++) {
				if (objects[i].intersectBinary(ray)) {
					return true;
				}
			}
			return false;
		}
		
		int numObjs(){
			return (int)objects.size();
		}
	};
}

#endif
