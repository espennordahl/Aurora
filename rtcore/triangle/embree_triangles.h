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

#ifndef __EMBREE_ACCEL_TRIANGLES_H__
#define __EMBREE_ACCEL_TRIANGLES_H__

/*! include all triangles */
#include "embree_triangle1i.h"
#include "embree_triangle4i.h"
#include "embree_triangle1v.h"
#include "embree_triangle4v.h"
#include "embree_triangle1.h"
#include "embree_triangle4.h"
#include "embree_triangle8.h"

/*! include single ray triangle intersectors */
#include "embree_triangle1i_intersector1_moeller.h"
#include "embree_triangle1i_intersector1_pluecker.h"
#include "embree_triangle4i_intersector1_moeller.h"
#include "embree_triangle4i_intersector1_pluecker.h"
#include "embree_triangle4i_intersector1_moeller_mb.h"
#include "embree_triangle4i_intersector1_pluecker_mb.h"
#include "embree_triangle1v_intersector1_moeller.h"
#include "embree_triangle1v_intersector1_pluecker.h"
#include "embree_triangle4v_intersector1_moeller.h"
#include "embree_triangle4v_intersector1_pluecker.h"
#include "embree_triangle1_intersector1_moeller.h"
#include "embree_triangle4_intersector1_moeller.h"
#include "embree_triangle8_intersector1_moeller.h"

#define INSTANTIATE_TEMPLATE_BY_INTERSECTOR(Base)                       \
  template class Base<Triangle1iIntersectorMoellerTrumbore>;            \
  template class Base<Triangle1iIntersectorPluecker>;                   \
  template class Base<Triangle4iIntersectorMoellerTrumbore>;            \
  template class Base<Triangle4iIntersectorPluecker>;                   \
  template class Base<Triangle1vIntersectorMoellerTrumbore>;            \
  template class Base<Triangle1vIntersectorPluecker>;                   \
  template class Base<Triangle4vIntersectorMoellerTrumbore>;            \
  template class Base<Triangle4vIntersectorPluecker>;                   \
  template class Base<Triangle1IntersectorMoellerTrumbore>;             \
  template class Base<Triangle4IntersectorMoellerTrumbore>;             \
  template class Base<Triangle8IntersectorMoellerTrumbore>;             \

#define INSTANTIATE_TEMPLATE_BY_INTERSECTOR_MB(Base)            \
  template class Base<Triangle4iIntersectorMoellerTrumboreMB>;  \
  template class Base<Triangle4iIntersectorPlueckerMB>;

#endif
