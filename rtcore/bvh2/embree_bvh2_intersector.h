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

#ifndef __EMBREE_BVH2_INTERSECTOR_H__
#define __EMBREE_BVH2_INTERSECTOR_H__

#include "embree_bvh2.h"
#include "embree_intersector.h"

namespace embree
{
  /*! BVH2 Traverser. Single ray traversal implementation for a binary BVH. */
  template<typename TriangleIntersector>
  class BVH2Intersector : public Intersector
  {
    /* shortcuts for frequently used types */
    typedef typename TriangleIntersector::Triangle Triangle;
    typedef typename BVH2::Base Base;
    typedef typename BVH2::Node Node;

  public:
    BVH2Intersector (const Ref<BVH2 >& bvh) : bvh(bvh) {}
    void intersect(const Ray& ray, Hit& hit) const;
    bool occluded (const Ray& ray) const;

  private:
    Ref<BVH2> bvh;
  };
}

#endif
