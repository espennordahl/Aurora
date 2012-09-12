//
//  embreeAccelerator.cpp
//  Aurora
//
//  Created by Espen Nordahl on 4/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "embreeAccelerator.h"


using namespace Aurora;


EmbreeAccelerator::EmbreeAccelerator(const EmbreeMesh &_mesh){
    
    mesh = _mesh;
    mesh.preRender();
    embree::Ref<embree::Accel> accel;
    accel = embree::rtcCreateAccel("bvh4.spatialsplit",             //!< type of acceleration structure to use
                                   "triangle4",                     //!< type of triangle representation to use
                                   mesh.triangles,  //!< array of triangles
                                   mesh.numTriangles,             //!< number of triangles in array
                                   mesh.vertices,     //!< array of vertices, has to be aligned to 16 bytes
                                   mesh.numVertices);             //!< number of vertices in array
//  const BBox3f& bounds = empty,    //!< optional approximate bounding box of the geometry
//  bool freeArrays = true);         //!< if true, triangle and vertex arrays are freed when no 
    
    intersector = accel->queryInterface<embree::Intersector> ();
}

bool EmbreeAccelerator::intersect( Ray *ray, Intersection *intersection) const{
    
    bool hit = false;
    
    embree::Ray r = embree::Ray(embree::Vec3f(ray->origin.x, ray->origin.y, ray->origin.z), embree::Vec3f(ray->direction.x, ray->direction.y, ray->direction.z), ray->mint, ray->maxt);
    embree::Hit isect;
    intersector->intersect(r,isect);

    if (isect == true) {
        hit = true;
        float t = isect.t;
        float u = isect.u;
        float v = isect.v;
        float w = 1. - u - v;

        ray->maxt = t;
        
            // fill in differential geo
        Point hitp = ray->origin + (ray->direction * t);
                
        Vector n1 = mesh.normals[isect.id1*3];
        Vector n2 = mesh.normals[isect.id1*3+1];
        Vector n3 = mesh.normals[isect.id1*3+2];
        Vector norm = Vector(
                     w * n1[0] + u * n2[0] + v * n3[0],
                     w * n1[1] + u * n2[1] + v * n3[1],
                     w * n1[2] + u * n2[2] + v * n3[2]);
            //	if (dot(norm, ray->direction) > 0.) {
            //		norm *= -1;
            //	}
        
        intersection->hitN = normalize(norm);
        intersection->hitP = hitp;
        intersection->attributesIndex = isect.id0;
        
    }
    return hit;
}

bool EmbreeAccelerator::intersectBinary( Ray *ray ) const{
    embree::Ray r = embree::Ray(embree::Vec3f(ray->origin.x, ray->origin.y, ray->origin.z), embree::Vec3f(ray->direction.x, ray->direction.y, ray->direction.z), ray->mint, ray->maxt);

    return intersector->occluded(r);
}