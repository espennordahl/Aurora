//
//  loopSubdivMesh.cpp
//  Aurora
//
//  Created by Espen Nordahl on 2/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "loopSubdivMesh.h"

#include <set>

#include "log.h"
#define lcontext LOG_Subdiv

using namespace Aurora;


    // LoopSubdiv Macros
#define NEXT(i) (((i)+1)%3)
#define PREV(i) (((i)+2)%3)

    // LoopSubdiv Local Structures
struct Aurora::SDVertex {
        // SDVertex Constructor
    SDVertex(Point pt = Point(0,0,0))
    : P(pt), startFace(NULL), child(NULL),
    regular(false), boundary(false) { }
    
        // SDVertex Methods
    int valence();
    void oneRing(Point *P);
    Point P;
    SDFace *startFace;
    SDVertex *child;
    bool regular, boundary;
};

struct Aurora::SDFace {
        // SDFace Constructor
    SDFace() {
        int i;
        for (i = 0; i < 3; ++i) {
            v[i] = NULL;
            f[i] = NULL;
        }
        for (i = 0; i < 4; ++i)
            children[i] = NULL;
    }
    
        // SDFace Methods
    int vnum(SDVertex *vert) const {
        for (int i = 0; i < 3; ++i)
            if (v[i] == vert) return i;
        LOG_ERROR("Basic logic error in SDFace::vnum()");
        return -1;
    }
    SDFace *nextFace(SDVertex *vert) {
        return f[vnum(vert)];
    }
    SDFace *prevFace(SDVertex *vert) {
        return f[PREV(vnum(vert))];
    }
    SDVertex *nextVert(SDVertex *vert) {
        return v[NEXT(vnum(vert))];
    }
    SDVertex *prevVert(SDVertex *vert) {
        return v[PREV(vnum(vert))];
    }
    SDVertex *otherVert(SDVertex *v0, SDVertex *v1) {
        for (int i = 0; i < 3; ++i)
            if (v[i] != v0 && v[i] != v1)
                return v[i];
        LOG_ERROR("Basic logic error in SDVertex::otherVert()");
        return NULL;
    }
    SDVertex *v[3];
    SDFace *f[3];
    SDFace *children[4];
};


struct Aurora::SDEdge {
        // SDEdge Constructor
    SDEdge(SDVertex *v0 = NULL, SDVertex *v1 = NULL) {
        v[0] = min(v0, v1);
        v[1] = max(v0, v1);
        f[0] = f[1] = NULL;
        f0edgeNum = -1;
    }
    
        // SDEdge Comparison Function
    bool operator<(const SDEdge &e2) const {
        if (v[0] == e2.v[0]) return v[1] < e2.v[1];
        return v[0] < e2.v[0];
    }
    SDVertex *v[2];
    SDFace *f[2];
    int f0edgeNum;
};

void SDVertex::oneRing(Point *P) {
    if (!boundary) {
            // Get one-ring vertices for interior vertex
        SDFace *face = startFace;
        do {
            *P++ = face->nextVert(this)->P;
            face = face->nextFace(this);
        } while (face != startFace);
    }
    else {
            // Get one-ring vertices for boundary vertex
        SDFace *face = startFace, *f2;
        while ((f2 = face->nextFace(this)) != NULL)
            face = f2;
        *P++ = face->nextVert(this)->P;
        do {
            *P++ = face->prevVert(this)->P;
            face = face->prevFace(this);
        } while (face != NULL);
    }
}


    // LoopSubdiv Inline Functions
inline int SDVertex::valence() {
    SDFace *f = startFace;
    if (!boundary) {
            // Compute valence of interior vertex
        int nf = 1;
        while ((f = f->nextFace(this)) != startFace)
            ++nf;
        return nf;
    }
    else {
            // Compute valence of boundary vertex
        int nf = 1;
        while ((f = f->nextFace(this)) != NULL)
            ++nf;
        f = startFace;
        while ((f = f->prevFace(this)) != NULL)
            ++nf;
        return nf+1;
    }
}

LoopSubdivMesh::LoopSubdivMesh(const Transform *o2c, const Transform *c2o,
                               int nfaces, int nvertices,
                               const int *vertexIndices, const Point *P,
                               int nlevels):
Shape(o2c, c2o),
m_nlevels(nlevels)
{
        // Allocate _LoopSubdiv_ vertices and faces
    int i;
    SDVertex *verts = new SDVertex[nvertices];
    for (i = 0; i < nvertices; ++i) {
        verts[i] = SDVertex(P[i]);
        m_vertices.push_back(&verts[i]);
    }
    SDFace *fs = new SDFace[nfaces];
    for (i = 0; i < nfaces; ++i)
        m_faces.push_back(&fs[i]);
    
        // Set face to vertex pointers
    const int *vp = vertexIndices;
    for (i = 0; i < nfaces; ++i) {
        SDFace *f = m_faces[i];
        for (int j = 0; j < 3; ++j) {
            SDVertex *v = m_vertices[vp[j]];
            f->v[j] = v;
            v->startFace = f;
        }
        vp += 3;
    }
    
        // Set neighbor pointers in _faces_
    set<SDEdge> edges;
    for (i = 0; i < nfaces; ++i) {
        SDFace *f = m_faces[i];
        for (int edgeNum = 0; edgeNum < 3; ++edgeNum) {
                // Update neighbor pointer for _edgeNum_
            int v0 = edgeNum, v1 = NEXT(edgeNum);
            SDEdge e(f->v[v0], f->v[v1]);
            if (edges.find(e) == edges.end()) {
                    // Handle new edge
                e.f[0] = f;
                e.f0edgeNum = edgeNum;
                edges.insert(e);
            }
            else {
                    // Handle previously seen edge
                e = *edges.find(e);
                e.f[0]->f[e.f0edgeNum] = f;
                f->f[edgeNum] = e.f[0];
                edges.erase(e);
            }
        }
    }
    
        // Finish vertex initialization
    for (i = 0; i < nvertices; ++i) {
        SDVertex *v = m_vertices[i];
        SDFace *f = v->startFace;
        do {
            f = f->nextFace(v);
        } while (f && f != v->startFace);
        v->boundary = (f == NULL);
        if (!v->boundary && v->valence() == 6)
            v->regular = true;
        else if (v->boundary && v->valence() == 4)
            v->regular = true;
        else
            v->regular = false;
    }
    
        // Create trimesh (TODO: This code should be elsewhere...
    
    vector<SDFace *> f = m_faces;
    vector<SDVertex *> v = m_vertices;
    for (int i = 0; i < m_nlevels; ++i) {
            // Update _f_ and _v_ for next level of subdivision
        vector<SDFace *> newFaces;
        vector<SDVertex *> newVertices;
        
            // Allocate next level of children in mesh tree
        for (uint32_t j = 0; j < v.size(); ++j) {
            v[j]->child = new SDVertex;
            v[j]->child->regular = v[j]->regular;
            v[j]->child->boundary = v[j]->boundary;
            newVertices.push_back(v[j]->child);
        }
        for (uint32_t j = 0; j < f.size(); ++j)
            for (int k = 0; k < 4; ++k) {
                f[j]->children[k] = new SDFace;
                newFaces.push_back(f[j]->children[k]);
            }
        
            // Update vertex positions and create new edge vertices
        
            // Update vertex positions for even vertices
        for (uint32_t j = 0; j < v.size(); ++j) {
            if (!v[j]->boundary) {
                    // Apply one-ring rule for even vertex
                if (v[j]->regular)
                    v[j]->child->P = weightOneRing(v[j], 1.f/16.f);
                else
                    v[j]->child->P = weightOneRing(v[j], beta(v[j]->valence()));
            }
            else {
                    // Apply boundary rule for even vertex
                v[j]->child->P = weightBoundary(v[j], 1.f/8.f);
            }
        }
        
            // Compute new odd edge vertices
        map<SDEdge, SDVertex *> edgeVerts;
        for (uint32_t j = 0; j < f.size(); ++j) {
            SDFace *face = f[j];
            for (int k = 0; k < 3; ++k) {
                    // Compute odd vertex on _k_th edge
                SDEdge edge(face->v[k], face->v[NEXT(k)]);
                SDVertex *vert = edgeVerts[edge];
                if (!vert) {
                        // Create and initialize new odd vertex
                    vert = new SDVertex;
                    newVertices.push_back(vert);
                    vert->regular = true;
                    vert->boundary = (face->f[k] == NULL);
                    vert->startFace = face->children[3];
                    
                        // Apply edge rules to compute new vertex position
                    if (vert->boundary) {
                        vert->P = 0.5f * edge.v[0]->P;
                        vert->P += 0.5f * edge.v[1]->P;
                    }
                    else {
                        vert->P = 3.f/8.f * edge.v[0]->P;
                        vert->P += 3.f/8.f * edge.v[1]->P;
                        vert->P += 1.f/8.f * face->otherVert(edge.v[0], edge.v[1])->P;
                        vert->P += 1.f/8.f *
                        face->f[k]->otherVert(edge.v[0], edge.v[1])->P;
                    }
                    edgeVerts[edge] = vert;
                }
            }
        }
        
            // Update new mesh topology
        
            // Update even vertex face pointers
        for (uint32_t j = 0; j < v.size(); ++j) {
            SDVertex *vert = v[j];
            int vertNum = vert->startFace->vnum(vert);
            vert->child->startFace =
            vert->startFace->children[vertNum];
        }
        
            // Update face neighbor pointers
        for (uint32_t j = 0; j < f.size(); ++j) {
            SDFace *face = f[j];
            for (int k = 0; k < 3; ++k) {
                    // Update children _f_ pointers for siblings
                face->children[3]->f[k] = face->children[NEXT(k)];
                face->children[k]->f[NEXT(k)] = face->children[3];
                
                    // Update children _f_ pointers for neighbor children
                SDFace *f2 = face->f[k];
                face->children[k]->f[k] =
                f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
                f2 = face->f[PREV(k)];
                face->children[k]->f[PREV(k)] =
                f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
            }
        }
        
            // Update face vertex pointers
        for (uint32_t j = 0; j < f.size(); ++j) {
            SDFace *face = f[j];
            for (int k = 0; k < 3; ++k) {
                    // Update child vertex pointer to new even vertex
                face->children[k]->v[k] = face->v[k]->child;
                
                    // Update child vertex pointer to new odd vertex
                SDVertex *vert = edgeVerts[SDEdge(face->v[k], face->v[NEXT(k)])];
                face->children[k]->v[NEXT(k)] = vert;
                face->children[NEXT(k)]->v[k] = vert;
                face->children[3]->v[k] = vert;
            }
        }
        
            // Prepare for next level of subdivision
        f = newFaces;
        v = newVertices;
    }
        // Push vertices to limit surface
    Point *Plimit = new Point[v.size()];
    for (uint32_t i = 0; i < v.size(); ++i) {
        if (v[i]->boundary)
            Plimit[i] = weightBoundary(v[i], 1.f/5.f);
        else
            Plimit[i] = weightOneRing(v[i], gamma(v[i]->valence()));
    }
    for (uint32_t i = 0; i < v.size(); ++i)
        v[i]->P = Plimit[i];
    
        // Compute vertex tangents on limit surface
    vector<Vector> Ns;
    Ns.reserve(v.size());
    vector<Point> Pring(16, Point());
    for (uint32_t i = 0; i < v.size(); ++i) {
        SDVertex *vert = v[i];
        Vector S(0,0,0), T(0,0,0);
        int valence = vert->valence();
        if (valence > (int)Pring.size())
            Pring.resize(valence);
        vert->oneRing(&Pring[0]);
        if (!vert->boundary) {
                // Compute tangents of interior face
            for (int k = 0; k < valence; ++k) {
                S += Vector(Pring[k]) * cosf(2.f*M_PI*k/valence);
                T += Vector(Pring[k]) * sinf(2.f*M_PI*k/valence);
            }
        } else {
                // Compute tangents of boundary face
            S = Pring[valence-1] - Pring[0];
            if (valence == 2)
                T = Vector(Pring[0] + Pring[1] - 2 * vert->P);
            else if (valence == 3)
                T = Pring[1] - vert->P;
            else if (valence == 4) // regular
                T = Vector(-1*Pring[0] + 2*Pring[1] + 2*Pring[2] +
                           -1*Pring[3] + -2*vert->P);
            else {
                float theta = M_PI / float(valence-1);
                T = Vector(sinf(theta) * (Pring[0] + Pring[valence-1]));
                for (int k = 1; k < valence-1; ++k) {
                    float wt = (2*cosf(theta) - 2) * sinf((k) * theta);
                    T += Vector(wt * Pring[k]);
                }
                T = -T;
            }
        }
        Ns.push_back(cross(S, T));
    }
    
    
        // create TriangeMesh from subdivisonmesh
    uint32_t ntris = uint32_t(f.size());
    int *trimesh_verts = new int[3*ntris];
    int *trimesh_vp = trimesh_verts;
    uint32_t totVerts = uint32_t(v.size());
    map<SDVertex *, int> usedVerts;
    for (uint32_t i = 0; i < totVerts; ++i){
        usedVerts[v[i]] = i;
    }
    for (uint32_t i = 0; i < ntris; ++i) {
        for (int j = 0; j < 3; ++j) {
            *trimesh_vp = usedVerts[f[i]->v[j]];
            ++trimesh_vp;
        }
    }
    
    uv *UVs = new uv[ntris*3];
    
    m_trimesh = TriangleMesh(o2c,
                             c2o,
                             ntris,
                             totVerts,
                             (int)Ns.size(),
                             trimesh_verts,
                             trimesh_verts, // norm Index... assumed to be the same as vert index
                             Plimit,
                             &Ns[0],
                             UVs
                             );
}

LoopSubdivMesh::~LoopSubdivMesh(){
    for (int i=0; i<m_faces.size(); ++i) {
        delete m_faces[i];
    }
    for(int i=0; i<m_vertices.size(); ++i){
        delete m_vertices[i];
    }
}


Point LoopSubdivMesh::weightOneRing(SDVertex *vert, float beta){
    int valence = vert->valence();
    Point *Pring = new Point[valence];
    vert->oneRing(Pring);
    
    Point P = (1 - valence * beta) * vert->P;
    for(int i=0; i < valence; ++i){
        P += beta * Pring[i];
    }
    return P;
}

Point LoopSubdivMesh::weightBoundary(SDVertex *vert, float beta){
    int valence = vert->valence();
    Point *Pring = new Point[valence];
    vert->oneRing(Pring);
    
    Point P = (1-2*beta) * vert->P;
    P += beta * Pring[0];
    P += beta * Pring[valence-1];
    return P;
}

float LoopSubdivMesh::beta(int valence) {
    if (valence == 3){
        return 3.f/16.f;
    } else {
        return 3.f/(8.f * valence);
    }
}

float LoopSubdivMesh::gamma(int valence) {
    return 1.f/ (valence + 3.f / (8.f * beta(valence)));
}



