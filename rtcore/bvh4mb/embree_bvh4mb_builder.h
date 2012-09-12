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

#ifndef __EMBREE_BVH4MB_BUILDER_H__
#define __EMBREE_BVH4MB_BUILDER_H__

#include "embree_bvh4mb.h"
#include "embree_primrefalloc.h"
#include "embree_primrefblock.h"
#include "embree_primrefgen.h"
#include "embree_splitter.h"
#include "embree_splitter_parallel.h"

namespace embree
{
  /* BVH4MB builder. The builder is multi-threaded and implements 3
   * different build strategies: 1) Small tasks are finished in a
   * single thread (BuildTask) 2) Medium sized tasks are split into
   * two tasks using a single thread (SplitTask) and 3) Large tasks are
   * split using multiple threads on one processor. */

  template<typename Heuristic>
    class BVH4MBBuilder : public RefCount
  {
  public:

    /*! Type of BVH build */
    typedef BVH4MB Type;

    /*! Split type of the split heuristic. */
    typedef typename Heuristic::Split Split;
    typedef typename Heuristic::PrimInfo PrimInfo;
    
    /*! Normal splitters based on heuristic */
    typedef PrimRefGen<Heuristic,atomic_set<PrimRefBlock> > PrimRefGenNormal;
    typedef MultiThreadedSplitter<Heuristic,atomic_set<PrimRefBlock> > MultiThreadedSplitterNormal;
    typedef Splitter<Heuristic> SplitterNormal;
    
  public:

    /*! Constructor. */
    BVH4MBBuilder(const TriangleType& trity, const std::string& intTy,
                  const BuildTriangle* triangles, size_t numTriangles, const Vec3fa* vertices, size_t numVertices, const BBox3f& bounds, bool freeData);

    /*! Destructor. */
    ~BVH4MBBuilder();

    /*! creates a leaf node */
    BVH4MB::Base* createLeaf(const TaskScheduler::ThreadInfo& thread, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo);

    /*! Selects between full build and single-threaded split strategy. */
    void recurse(const TaskScheduler::ThreadInfo& thread, BVH4MB::Base*& node, size_t depth, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

    /*! Single-threaded task that builds a complete BVH. */
    class BuildTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      BuildTask(const TaskScheduler::ThreadInfo& thread, BVH4MBBuilder* parent, BVH4MB::Base*& node, size_t depth, 
                atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Task entry function. */
      static void run(const TaskScheduler::ThreadInfo& thread, BuildTask* This, size_t elts);

      /*! Recursively finishes the BVH construction. */
      BVH4MB::Base* recurse(size_t depth, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

    private:
      BVH4MBBuilder*                   parent;  //!< Pointer to parent task.
      const TaskScheduler::ThreadInfo* thread;  //!< Task ID for fast thread local storage.
      BVH4MB::Base*&                   dst;     //!< Reference to output the node.
      size_t                           depth;   //!< Recursion depth of the root of this subtree.
      atomic_set<PrimRefBlock>         prims;   //!< The list of primitives.
      PrimInfo                         pinfo;   //!< Bounding info of primitives.
      Split                            split;   //!< The best split for the primitives.
    };

    /*! Single-threaded task that builds a single node and creates subtasks for the children. */
    class SplitTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      SplitTask(const TaskScheduler::ThreadInfo& thread, BVH4MBBuilder* parent, BVH4MB::Base*& node, size_t depth, 
                atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Task entry function. */
      void recurse(const TaskScheduler::ThreadInfo& thread); 
      static void _recurse(const TaskScheduler::ThreadInfo& thread, SplitTask* This, size_t elts) { This->recurse(thread); }

    private:
      BVH4MBBuilder*  parent;         //!< Pointer to parent task.
      BVH4MB::Base*&  dst;            //!< Reference to output the node.
      size_t          depth;          //!< Recursion depth of this node.
      atomic_set<PrimRefBlock> prims; //!< The list of primitives.
      PrimInfo        pinfo;          //!< Bounding info of primitives.
      Split           split;          //!< The best split for the primitives.
    };

    /*! Multi-threaded split task that builds a single node and creates subtasks for the children. */
    class ParallelSplitTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      ParallelSplitTask(const TaskScheduler::ThreadInfo& thread, BVH4MBBuilder* parent, BVH4MB::Base*& node, size_t depth, 
                        atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Called after the parallel binning to creates the node. */
      void loop(const TaskScheduler::ThreadInfo& thread); 
      static void _loop(const TaskScheduler::ThreadInfo& thread, ParallelSplitTask* This) { This->loop(thread); }

    private:
      BVH4MBBuilder*              parent;                 //!< Pointer to parent task.
      BVH4MB::Base*&              dst;                    //!< Reference to output the node ID.
      size_t                      depth;                  //!< Recursion depth of this node.
      atomic_set<PrimRefBlock>    cprims[4];              //!< Primitive lists for the children.
      PrimInfo                    cinfo [4];              //!< Bounding info for the children.
      Split                       csplit[4];              //!< Best next split for the children.
      size_t                      numChildren;            //!< Current number of children.
      ssize_t                     bestChild;              //!< Best child selected for splitting.
      MultiThreadedSplitterNormal splitter;               //!< Parallel splitter
    };

  private:
    const TriangleType& trity;          //!< triangle type stored in BVH
    const BuildTriangle* triangles;     //!< Source triangle array
    size_t numTriangles;                //!< Number of triangles
    const Vec3fa* vertices;              //!< Source vertex array
    size_t numVertices;                 //!< Number of vertices
    PrimRefAlloc alloc;                 //!< Allocator for primitive blocks
    TaskScheduler::QUEUE taskQueue;     //!< Task queue to use

  public:
    Ref<BVH4MB> bvh;                    //!< Output BVH
  };
}

#endif
