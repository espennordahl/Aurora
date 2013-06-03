//
//  loopSubdivMesh.h
//  Aurora
//
//  Created by Espen Nordahl on 2/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#ifndef __Aurora__loopSubdivMesh__
#define __Aurora__loopSubdivMesh__

#include <iostream>

#include "shape.h"

#include "log.h"
#define lcontext LOG_Subdiv


namespace Aurora{
    
#define NEXT(i) (((i)+1)%3)
#define PREV(i) (((i)+2)%3)
    
    struct SDVertex;
    
    struct SDFace {
                // SDFace Methods
            int vnum(SDVertex *vert) const {
            for (int i = 0; i < 3; ++i)
            if (v[i] == vert) return i;
            LOG_WARNING("Basic logic error in SDFace::vnum()");
            return -1;
        }
        
        SDFace *nextFace(SDVertex *vert){
            return f[vnum(vert)];
        }
        SDFace *prevFace(SDVertex *vert){
            return f[PREV(vnum(vert))];
        }
        
        SDVertex *nextVert(SDVertex *vert){
            return v[NEXT(vnum(vert))];
        }
        SDVertex *prevVert(SDVertex *vert){
            return v[PREV(vnum(vert))];
        }
        
        SDVertex *otherVert(SDVertex *v0, SDVertex *v1){
            for(int i=0; i<3; ++i) {
                if (v[i] != v0 && v[i] != v1){
                    return v[i];
                }
            }
            LOG_WARNING("Basic logic error in SDFace::otherVert()");
            return NULL;
        }
        
        SDVertex *v[3];
        SDFace *f[3];
        SDFace *children[4];
    };
    
    struct SDVertex {
        SDVertex(Point p = Point(0)):
            P(p), startFace(NULL), child(NULL),
            regular(false), boundary(false){}
        
        inline int valence() {
            SDFace *f = startFace;
            if(!boundary){
                    // compute valence of interior vertex
                int nf = 1;
                while ((f = f->nextFace(this)) != startFace){
                       ++nf;
                }
                       return nf;
            } else {
                    // compute valence of boundary vertex
                int nf = 1;
                while ((f = f->nextFace(this)) != NULL){
                    ++nf;
                }
                f = startFace;
                while ((f = f->prevFace(this)) != NULL){
                    ++nf;
                }
                return nf+1;
            }
        }
        
        void oneRing(Point *P){
            if(!boundary){
                    // get one-ring vertices for interior vertex
                SDFace *face = startFace;
                do{
                    *P++ = face->nextVert(this)->P;
                    face = face->nextFace(this);
                } while (face != startFace);
            } else {
                    // get one-ring vertices for boundary vertex
                SDFace *face = startFace, *f2;
                while ((f2 = face->nextFace(this)) != NULL) {
                    face = f2;
                }
                *P++ = face->nextVert(this)->P;
                do {
                    *P++ = face->prevVert(this)->P;
                    face = face->prevFace(this);
                } while (face != NULL);
            }
        }
        
        Point P;
        SDFace *startFace;
        SDVertex *child;
        bool regular;
        bool boundary;
    };
    
    
    struct SDEdge {
        SDEdge(SDVertex *v0 = NULL, SDVertex *v1 = NULL) {
            v[0] = min(v0, v1);
            v[1] = max(v0, v1);
            f[0] = f[1] = NULL;
            f0edgeNum = -1;
        }
        
        bool operator<(const SDEdge &rhs) const{
            if(v[0] == rhs.v[0]){
                return v[1] < rhs.v[1];
            } else {
                return v[0] < rhs.v[0];
            }
        }
        
        SDVertex *v[2];
        SDFace *f[2];
        int f0edgeNum;
    };
    
    class LoopSubdivMesh : public Shape {
    public:
        LoopSubdivMesh(const Transform *o2c, const Transform *c2o, int nfaces, int nvertices, const int *vertexIndices, const Point *P, int nlevels);
        ~LoopSubdivMesh();
        
        BBox objectBound() const;
		BBox worldBound() const;
        
        void dice( std::vector<std::tr1::shared_ptr<Geometry> > &diced);
        
    private:
        static float beta(int valence);
        static float gamma(int valence);

        Point weightOneRing(SDVertex *vert, float beta);
        Point weightBoundary(SDVertex *vert, float beta);

        int m_nlevels;
        std::vector<SDVertex *> m_vertices;
        std::vector<SDFace *> m_faces;
    };
}

#undef lcontext

#endif /* defined(__Aurora__loopSubdivMesh__) */
