/* ************************************************************************* *\
 INTEL CORPORATION PROPRIETARY INFORMATION
 This software is supplied under the terms of a license agreement or 
 nondisclosure agreement with Intel Corporation and may not be copied 
 or disclosed except in accordance with the terms of that agreement. 
 Copyright (C) 2009 Intel Corporation. All Rights Reserved.
 \* ************************************************************************* */

#ifndef __EMBREE_SIMD_H__
#define __EMBREE_SIMD_H__

#include "embree_math.h"
#include "embree_pairb.h"
#include "embree_pairi.h"
#include "embree_pairf.h"

/* we require SSE for simd4 types */
#include "embree_sse.h"
namespace embree
{
  typedef sseb simd4b;
  typedef ssei simd4i;
  typedef ssef simd4f;
}

/* we choose between AVX and 2x SSE for simd8 types */
#if defined(__AVX__)
#include "embree_avx.h"
#define AVX_ZERO_UPPER() _mm256_zeroupper()
namespace embree
{
  typedef avxb simd8b;
  typedef avxi simd8i;
  typedef avxf simd8f;
}
#else
#define AVX_ZERO_UPPER()
namespace embree
{
  typedef pairb<sseb> avxb;
  typedef pairi<ssei> avxi;
  typedef pairf<ssef> avxf;

  typedef pairb<simd4b> simd8b;
  typedef pairi<simd4i> simd8i;
  typedef pairf<simd4f> simd8f;
}
#endif

#endif
