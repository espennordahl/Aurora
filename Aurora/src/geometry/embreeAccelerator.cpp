//
//  embreeAccelerator.cpp
//  Aurora
//
//  Created by Espen Nordahl on 4/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "embreeAccelerator.h"

#include "log.h"
#define lcontext LOG_Embree

using namespace Aurora;


EmbreeAccelerator::EmbreeAccelerator(const EmbreeMesh &mesh, AttributeState *attrs){
    
    m_mesh = mesh;
    m_mesh.preRender(attrs);
    embree::Ref<embree::Accel> accel;
    accel = embree::rtcCreateAccel("bvh4.spatialsplit", //!< type of acceleration structure to use
                                   "triangle4",         //!< type of triangle representation to use
                                   m_mesh.triangles,    //!< array of triangles
                                   m_mesh.numTriangles, //!< number of triangles in array
                                   m_mesh.vertices,     //!< array of vertices, has to be aligned to 16 bytes
                                   m_mesh.numVertices,  //!< number of vertices in array
                                   embree::empty,       //!< optional approximate bounding box of the geometry
                                   true);               //!< if true, triangle and vertex arrays are freed when no longer needed
    m_intersector = accel->queryInterface<embree::Intersector> ();
    
    LOG_INFO("Size of prim vars: " <<  (m_mesh.uvs.capacity() * sizeof(uv) + m_mesh.normals.capacity() *sizeof(Vector)) / 1024 / 1024 << "MB");
}

EmbreeAccelerator::~EmbreeAccelerator(){
    
}

bool EmbreeAccelerator::intersect( Ray *ray, Intersection *intersection) const{
    
    bool hit = false;
    
    embree::Ray r = embree::Ray(embree::Vec3f(ray->origin.x, ray->origin.y, ray->origin.z), embree::Vec3f(ray->direction.x, ray->direction.y, ray->direction.z), ray->mint, ray->maxt);
    embree::Hit isect;
    m_intersector->intersect(r,isect);

    if (isect == true) {
        hit = true;
        float t = isect.t;
        float u = isect.u;
        float v = isect.v;
        float w = 1. - u - v;

        ray->maxt = t;
        
            // fill in differential geo
        Point hitp = ray->origin + (ray->direction * t);
                
        Vector n1 = m_mesh.normals[isect.id1*3];
        Vector n2 = m_mesh.normals[isect.id1*3+1];
        Vector n3 = m_mesh.normals[isect.id1*3+2];
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
        uv uv1 = m_mesh.uvs[isect.id1*3];
        uv uv2 = m_mesh.uvs[isect.id1*3+1];
        uv uv3 = m_mesh.uvs[isect.id1*3+2];
        float _s =   w * uv1.u + u * uv2.u + v * uv3.u;
        float _t =   w * uv1.v + u * uv2.v + v * uv3.v;
        uv st;
        st.u = _s;
        st.v = _t;
        intersection->shdGeo.P = hitp;
        intersection->shdGeo.Ng = intersection->hitN;
        intersection->shdGeo.st = st;
        intersection->shdGeo.triangleIndex = isect.id1;
        intersection->shdGeo.barCoords = Point(u,v,w);
    }
    return hit;
}

bool EmbreeAccelerator::intersectBinary( Ray *ray ) const{
    embree::Ray r = embree::Ray(embree::Vec3f(ray->origin.x, ray->origin.y, ray->origin.z), embree::Vec3f(ray->direction.x, ray->direction.y, ray->direction.z), ray->mint, ray->maxt);

    return m_intersector->occluded(r);
}

void EmbreeAccelerator::getShadingTriangles( int triangleIndex, ShadingGeometry *shdGeo ){
    embree::BuildTriangle tri = m_mesh.triangles[triangleIndex];
    Point Ptmp  = Point(m_mesh.vertices[tri.v0].x,
                        m_mesh.vertices[tri.v0].y,
                        m_mesh.vertices[tri.v0].z);
    shdGeo[0].P = Ptmp;
    Ptmp  =       Point(m_mesh.vertices[tri.v1].x,
                        m_mesh.vertices[tri.v1].y,
                        m_mesh.vertices[tri.v1].z);
    shdGeo[1].P = Ptmp;
    Ptmp  =       Point(m_mesh.vertices[tri.v2].x,
                        m_mesh.vertices[tri.v2].y,
                        m_mesh.vertices[tri.v2].z);
    shdGeo[2].P = Ptmp;
    
    for (int i=0; i < 3; i++) {
        shdGeo[i].st = m_mesh.uvs[triangleIndex*3+i];
        shdGeo[i].Ng = m_mesh.normals[triangleIndex*3+i];
        shdGeo[i].Ns = m_mesh.normals[triangleIndex*3+i];
        shdGeo[i].triangleIndex = triangleIndex;
    }
}