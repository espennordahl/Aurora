// ======================================================================== //
// Copyright 2009-2012 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#ifndef __EMBREE_ACCEL_TRIANGLE4I_INTERSECTOR_PLUECKER_H__
#define __EMBREE_ACCEL_TRIANGLE4I_INTERSECTOR_PLUECKER_H__

#include "embree_triangle4i.h"
#include "embree_ray.h"
#include "embree_hit.h"

namespace embree
{
  /*! Pluecker Intersector for 4 triangles from an indexed face set
   *  with a single ray. The test first shifts the ray origin into the
   *  origin of the coordinate system and then uses Pluecker
   *  coordinates for the intersection. Due to the shift, the Pluecker
   *  coordinate calculation simplifies.*/
  struct Triangle4iIntersectorPluecker
  {
    typedef Triangle4i Triangle;

    /*! Intersect a ray with the 4 triangles and updates the hit. */
    static __forceinline void intersect(const Ray& ray, Hit& hit, const Triangle4i& tri, const Vec3fa* vertices = NULL)
    {
      STAT3(normal.trav_tris,1,1,1);

      /* gather vertices */
      const ssef* vert = (const ssef*) vertices;
      sse3f p0; transpose(vert[tri.v0[0]],vert[tri.v0[1]],vert[tri.v0[2]],vert[tri.v0[3]], p0.x,p0.y,p0.z);
      sse3f p1; transpose(vert[tri.v1[0]],vert[tri.v1[1]],vert[tri.v1[2]],vert[tri.v1[3]], p1.x,p1.y,p1.z);
      sse3f p2; transpose(vert[tri.v2[0]],vert[tri.v2[1]],vert[tri.v2[2]],vert[tri.v2[3]], p2.x,p2.y,p2.z);
      
      /* calculate edges */
      const sse3f O = ray.org;
      const sse3f D = ray.dir;
      const sse3f e0 = p2-p0;
      const sse3f e1 = p0-p1;
      const sse3f e2 = p1-p2;

      /* perform edge tests */
      const sse3f Nv = cross(p1-O,e1);
      const sse3f Nw = cross(p2-O,e2);
      const sse3f Ng = cross(e1  ,e0);
      const sse3f Nu = cross(p0-O,e0);
      const ssef det = dot(Ng,D);
      const ssef _U = dot(Nu,D);
      const ssef absDet = abs(det);
      const ssef sgnDet = signmsk(det);
      const ssef U = _U ^ sgnDet;
      const ssef V = dot(Nv,D) ^ sgnDet;
      const ssef W = dot(Nw,D) ^ sgnDet;
      sseb valid = tri.valid() & (U >= 0.0f) & (V >= 0.0f) & (W >= 0.0f);
      if (unlikely(none(valid))) return;

      /* perform depth test */
      const ssef T = dot(p0-O,Ng) ^ sgnDet;
      valid &= (det != ssef(zero)) & (T >= absDet*ssef(ray.near)) & (absDet*ssef(hit.t) >= T);
      if (unlikely(none(valid))) return;

      /* update hit information */
      const ssef rcpAbsDet = rcp(absDet);
      const ssef u = U * rcpAbsDet;
      const ssef v = V * rcpAbsDet;
      const ssef t = T * rcpAbsDet;
      const size_t i = select_min(valid,t);
      hit.t = t[i];
      hit.u = u[i];
      hit.v = v[i];
      hit.id0 = tri.id0[i] & 0x7FFFFFFF;
      hit.id1 = tri.id1[i];
    }

    /*! Test if the ray is occluded by one of the triangles. */
    static __forceinline bool occluded(const Ray& ray, const Triangle4i& tri, const Vec3fa* vertices = NULL)
    {
      STAT3(shadow.trav_tris,1,1,1);

      /* gather vertices */
      const ssef* vert = (const ssef*) vertices;
      sse3f p0; transpose(vert[tri.v0[0]],vert[tri.v0[1]],vert[tri.v0[2]],vert[tri.v0[3]], p0.x,p0.y,p0.z);
      sse3f p1; transpose(vert[tri.v1[0]],vert[tri.v1[1]],vert[tri.v1[2]],vert[tri.v1[3]], p1.x,p1.y,p1.z);
      sse3f p2; transpose(vert[tri.v2[0]],vert[tri.v2[1]],vert[tri.v2[2]],vert[tri.v2[3]], p2.x,p2.y,p2.z);

      /* calculate edges */
      const sse3f O = ray.org;
      const sse3f D = ray.dir;
      const sse3f e0 = p2-p0;
      const sse3f e1 = p0-p1;
      const sse3f e2 = p1-p2;

      /* perform edge tests */
      const sse3f Nv = cross(p1-O,e1);
      const sse3f Nw = cross(p2-O,e2);
      const sse3f Ng = cross(e1  ,e0);
      const sse3f Nu = cross(p0-O,e0);
      const ssef det = dot(Ng,D);
      const ssef _U = dot(Nu,D);
      const ssef absDet = abs(det);
      const ssef sgnDet = signmsk(det);
      const ssef U = _U ^ sgnDet;
      const ssef V = dot(Nv,D) ^ sgnDet;
      const ssef W = dot(Nw,D) ^ sgnDet;
      sseb valid = tri.valid() & (U >= 0.0f) & (V >= 0.0f) & (W >= 0.0f);
      if (unlikely(none(valid))) return false;
      
      /* perform depth test */
      const ssef T = dot(p0-O,Ng) ^ sgnDet;
      valid &= (det != ssef(zero)) & (T >= absDet*ssef(ray.near)) & (absDet*ssef(ray.far) >= T);
      if (unlikely(none(valid))) return false;

      return true;
    }
  };
}

#endif
