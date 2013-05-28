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

#ifndef __EMBREE_RTCORE_DEFAULT_H__
#define __EMBREE_RTCORE_DEFAULT_H__

#include "embree_platform.h"
#include "embree_ref.h"
#include "embree_intrinsics.h"
#include "embree_atomic.h"
#include "embree_vector.h"
#include "embree_string.h"
#include "embree_taskscheduler.h"

#include "embree_math.h"
#include "embree_vec2.h"
#include "embree_vec3.h"
#include "embree_vec4.h"
#include "embree_bbox.h"
#include "embree_affinespace.h"

#include "embree_simd.h"

#include "embree_stat.h"

namespace embree
{
  typedef Vec2<sseb> sse2b;
  typedef Vec3<sseb> sse3b;
  typedef Vec2<ssei> sse2i;
  typedef Vec3<ssei> sse3i;
  typedef Vec2<ssef> sse2f;
  typedef Vec3<ssef> sse3f;

  typedef Vec2<avxb> avx2b;
  typedef Vec3<avxb> avx3b;
  typedef Vec2<avxi> avx2i;
  typedef Vec3<avxi> avx3i;
  typedef Vec2<avxf> avx2f;
  typedef Vec3<avxf> avx3f;
}

#endif
