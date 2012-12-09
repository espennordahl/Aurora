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


EmbreeAccelerator::EmbreeAccelerator(const EmbreeMesh &_mesh, AttributeState *attrs){
    
    mesh = _mesh;
    mesh.preRender(attrs);
    embree::Ref<embree::Accel> accel;
    accel = embree::rtcCreateAccel("bvh4.spatialsplit",             //!< type of acceleration structure to use
                                   "triangle4",                     //!< type of triangle representation to use
                                   mesh.triangles,  //!< array of triangles
                                   mesh.numTriangles,             //!< number of triangles in array
                                   mesh.vertices,     //!< array of vertices, has to be aligned to 16 bytes
                                   mesh.numVertices,
                                   embree::BBox3f(),
                                   false);             //!< number of vertices in array
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
        
            // faceforward
            	if (dot(norm, ray->direction) > 0.) {
            		norm *= -1;
            	}
        
        intersection->hitN = normalize(norm);
        intersection->hitP = hitp;
        intersection->attributesIndex = isect.id0;
        
            // st coords
        uv uv1 = mesh.uvs[isect.id1*3];
        uv uv2 = mesh.uvs[isect.id1*3+1];
        uv uv3 = mesh.uvs[isect.id1*3+2];
        float _s =   w * uv1.u + u * uv2.u + v * uv3.u;
        float _t =   w * uv1.v + u * uv2.v + v * uv3.v;
        uv st;
        st.u = _s;
        st.v = _t;
        intersection->shdGeo.P = hitp;
        intersection->shdGeo.Ns = normalize(norm);
        intersection->shdGeo.st = st;
        intersection->shdGeo.triangleIndex = isect.id1;
        intersection->shdGeo.barCoords = Point(u,v,w);
    }
    return hit;
}

bool EmbreeAccelerator::intersectBinary( Ray *ray ) const{
    embree::Ray r = embree::Ray(embree::Vec3f(ray->origin.x, ray->origin.y, ray->origin.z), embree::Vec3f(ray->direction.x, ray->direction.y, ray->direction.z), ray->mint, ray->maxt);

    return intersector->occluded(r);
}

void EmbreeAccelerator::getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo ){
    embree::BuildTriangle tri = mesh.triangles[triangleIndex];
    Point Ptmp  = Point(mesh.vertices[tri.v0].x, 
                        mesh.vertices[tri.v0].y, 
                        mesh.vertices[tri.v0].z);
    shdGeo[0].P = Ptmp;
    Ptmp  =       Point(mesh.vertices[tri.v1].x, 
                        mesh.vertices[tri.v1].y, 
                        mesh.vertices[tri.v1].z);
    shdGeo[1].P = Ptmp;
    Ptmp  =       Point(mesh.vertices[tri.v2].x, 
                        mesh.vertices[tri.v2].y, 
                        mesh.vertices[tri.v2].z);
    shdGeo[2].P = Ptmp;
    
    for (int i=0; i < 3; i++) {
        shdGeo[i].st = mesh.uvs[triangleIndex*3+i];
        shdGeo[i].Ng = mesh.normals[triangleIndex*3+i];
        shdGeo[i].Ns = mesh.normals[triangleIndex*3+i];
        shdGeo[i].triangleIndex = triangleIndex;
    }
}