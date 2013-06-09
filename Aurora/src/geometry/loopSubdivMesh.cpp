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

struct Aurora::SDVertex {
        // SDVertex Constructor
    SDVertex(const Point *pt = NULL, float uu = 0.0f, float vv = 0.0f) :
    P(pt), u(uu), v(vv),
    startFace(NULL), child(NULL), regular(false), boundary(false) { }
    
        // SDVertex Methods
    size_t valence() const;
    void oneRing(Point *P) const;
        
    bool operator==(const SDVertex &v2) const {
        return *P == *v2.P;
    }
    
    bool operator<(const SDVertex &v2) const {
        return *P < *v2.P;
    }
    
    const Point *P;
    float u, v;
    SDFace *startFace;
    SDVertex *child;
    bool regular, boundary;
};

struct Aurora::SDFace {
        // SDFace Constructor
    SDFace() {
        for (size_t i = 0; i < 3; ++i) {
            v[i] = NULL;
            f[i] = NULL;
        }
        for (size_t i = 0; i < 4; ++i)
            children[i] = NULL;
    }
    
        // SDFace Methods
    size_t vnum(const Point *p) const {
        for (size_t i = 0; i < 3; ++i) {
            if (v[i]->P == p)
                return i;
        }
        LOG_ERROR("Basic logic error in SDFace::vnum()");
        return 0;
    }
    size_t fnum(const SDFace *face) const {
        for (size_t i = 0; i < 3; ++i) {
            if (f[i] == face)
                return i;
        }
        LOG_ERROR("Basic logic error in SDFace::fnum()");
        return 0;
    }
    SDFace *nextFace(const Point *p) const {
        return f[vnum(p)];
    }
    SDFace *prevFace(const Point *p) const {
        return f[PREV(vnum(p))];
    }
    SDVertex *nextVert(const Point *p) const {
        return v[NEXT(vnum(p))];
    }
    SDVertex *prevVert(const Point *p) const {
        return v[PREV(vnum(p))];
    }
    SDVertex *otherVert(const Point *p0, const Point *p1) const {
        for (size_t i = 0; i < 3; ++i) {
            if (v[i]->P != p0 && v[i]->P != p1)
                return v[i];
        }
        LOG_ERROR("Basic logic error in SDVertex::otherVert()");
        return NULL;
    }
    SDVertex *v[3];
    SDFace *f[3];
    SDFace *children[4];
};

struct Aurora::SDEdge {
        // SDEdge Constructor
    SDEdge(const SDVertex *v0, const SDVertex *v1) {
        if (*v0->P < *v1->P) {
            v[0] = v0;
            v[1] = v1;
        } else {
            v[0] = v1;
            v[1] = v0;
        }
        f[0] = f[1] = NULL;
        f0edgeNum = -1;
    }
    
    bool operator<(const SDEdge &e2) const {
        if(*v[0] == *e2.v[0]){
            return *v[1] < *e2.v[1];
        } else {
            return *v[0] < *e2.v[0];
        }
    }
    
    const SDVertex *v[2];
    SDFace *f[2];
    size_t f0edgeNum;
};

static bool CheckDegenerate(SDFace *face)
{
    bool degenerate = false;
    for (size_t i = 0; i < 3; ++i) {
            // If the vertex is NULL, the face has already been checked
            // and it is degenerate
        if (!face->v[i])
            return true;
        if (face->v[i] != face->v[NEXT(i)])
            continue;
        degenerate = true;
        if (face->f[PREV(i)])
            face->f[PREV(i)]->f[face->f[PREV(i)]->fnum(face)] = face->f[NEXT(i)];
        else
            face->v[NEXT(i)]->boundary = true;
        if (face->f[NEXT(i)])
            face->f[NEXT(i)]->f[PREV(face->f[NEXT(i)]->fnum(face))] = face->f[PREV(i)];
        else
            face->v[i]->boundary = true;
        break;
    }
        // Update vertex start face if it is degenerate
    if (degenerate) {
        for (size_t i = 0; i < 3; ++i) {
            SDVertex *vert = face->v[i];
                // Clear vertex to detect the degenerate face later
            face->v[i] = NULL;
            if (vert->startFace != face)
                continue;
            if (face->f[i])
                vert->startFace = face->f[i];
            else
                vert->startFace = face->f[PREV(i)];
        }
    }
    return degenerate;
}


LoopSubdivMesh::LoopSubdivMesh(const Transform *o2c, const Transform *c2o,
                               int nfaces, int nvertices,
                               const int *vertexIndices, const Point *P,
                               const uv *UVin,
                               int nlevels):
Shape(o2c, c2o)
{
    bool hasUV = (UVin != NULL);
    size_t nLevels = nlevels;
    set<Point, PointCompare> uniqueVertices;
    vector<SDVertex *> vertices;
    vector<SDFace *> faces;
    
        // Identify all unique vertices
    for (size_t i = 0; i < nvertices; ++i){
        uniqueVertices.insert(P[i]);
    }
    
        // Allocate _LoopSubdiv_ vertices and faces
    SDVertex *verts = new SDVertex[nvertices];
    vertices.reserve(nvertices);
    for (size_t i = 0; i < nvertices; ++i) {
        verts[i] = SDVertex(&(*uniqueVertices.find(P[i])),
                            hasUV ? UVin[i].u : 0.f,
                            hasUV ? UVin[i].v : 0.f);
        vertices.push_back(&verts[i]);
    }
    
    SDFace *fs = new SDFace[nfaces];
    faces.reserve(nfaces);
        // Set face to vertex pointers
    const int *vp = vertexIndices;
    SDFace *fp = fs;
    for (size_t i = 0; i < nfaces; ++i) {
            // Skip degenerate triangles
        if (vertices[vp[0]]->P == vertices[vp[1]]->P ||
            vertices[vp[0]]->P == vertices[vp[2]]->P ||
            vertices[vp[1]]->P == vertices[vp[2]]->P) {
            vp +=3;
            LOG_WARNING("Degenerate triangle. Skipping.");
            continue;
        }
        for (size_t j = 0; j < 3; ++j) {
            SDVertex *v = vertices[vp[j]];
            fp->v[j] = v;
            fp->f[j] = NULL;
            fp->children[j] = NULL;
            v->startFace = fp;
        }
        fp->children[3] = NULL;
        vp += 3;
        faces.push_back(fp++);
    }
        // Update real number of faces
    nfaces = fp - fs;
    
        // Set neighbor pointers in _faces_
    set<SDEdge> edges;
    for (size_t i = 0; i < nfaces; ++i) {
        SDFace *f = faces[i];
        for (size_t edgeNum = 0; edgeNum < 3; ++edgeNum) {
                // Update neighbor pointer for _edgeNum_
            size_t v0 = edgeNum, v1 = NEXT(edgeNum);
            SDEdge e(f->v[v0], f->v[v1]);
            if (edges.find(e) == edges.end()) {
                    // Handle new edge
                e.f[0] = f;
                e.f0edgeNum = edgeNum;
                edges.insert(e);
            } else {
                    // Handle previously-seen edge
                e = *edges.find(e);
                e.f[0]->f[e.f0edgeNum] = f;
                f->f[edgeNum] = e.f[0];
                    // NOTE - lordcrc - check winding of
                    // other face is opposite of the
                    // current face, otherwise we have
                    // inconsistent winding
                size_t otherv0 = e.f[0]->vnum(f->v[v0]->P);
                size_t otherv1 = e.f[0]->vnum(f->v[v1]->P);
                if (PREV(otherv0) != otherv1) {
                    LOG_ERROR("Error while subdividing: Inconsistent winding");
                };
                edges.erase(e);
            }
        }
    }
    
        // Finish vertex initialization
    for (size_t i = 0; i < nvertices; ++i) {
        SDVertex *v = vertices[i];
        SDFace *f = v->startFace;
            // Skip unused vertices
        if (!f){
            LOG_WARNING("Unused vertex");
            continue;
        }
        do {
            f = f->nextFace(v->P);
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
    
    LOG_INFO("Applying " << nLevels << " levels of loop subdivision to " << faces.size() << " triangles");
    
    vector<SDFace *> f = faces;
    vector<SDVertex *> v = vertices;
    set<Point, PointCompare> u = uniqueVertices;
    
    for (size_t i = 0; i < nLevels; ++i) {
            // Update _f_ and _v_ for next level of subdivision
        vector<SDFace *> newFaces;
        vector<SDVertex *> newVertices;
        set<Point, PointCompare> newUniqueVertices;
        
            // Allocate next level of children in mesh tree
        newFaces.reserve(f.size());
        for (size_t j = 0; j < f.size(); ++j) {
            SDFace *face = f[j];
                // Verify that the face is not degenerate
            bool degenerate = CheckDegenerate(face);
            for (size_t k = 0; k < 4; ++k) {
                if (degenerate) {
                    face->children[k] = NULL;
                } else {
                    face->children[k] = new SDFace;
                    newFaces.push_back(face->children[k]);
                }
            }
        }
        newVertices.reserve(v.size());
        for (size_t j = 0; j < v.size(); ++j) {
            SDVertex *vert = v[j];
            if (!vert->startFace)
                continue;
            vert->child = new SDVertex;
            vert->child->regular = v[j]->regular;
            vert->child->boundary = v[j]->boundary;
                // Update vertex positions for even vertices
            if (!vert->boundary) {
                    // Apply one-ring rule for even vertex
                if (vert->regular)
                    weightOneRing(newUniqueVertices, vert->child, vert, 1.f/16.f);
                else
                    weightOneRing(newUniqueVertices, vert->child, vert, beta((int)vert->valence()));
            } else {
                    // Apply boundary rule for even vertex
                weightBoundary(newUniqueVertices, vert->child, vert, 1.f/8.f);
            }
                // Update even vertex face pointers
            const SDFace *sf = vert->startFace;
            vert->child->startFace = sf->children[sf->vnum(vert->P)];
            newVertices.push_back(v[j]->child);
        }
        
            // Compute new odd edge vertices
            // Update new mesh topology
        map<SDEdge, SDVertex *> edgeVerts;
        for (size_t j = 0; j < f.size(); ++j) {
            SDFace *face = f[j];
                // Skip degenerate faces
            if (face->children[0] == NULL)
                continue;
            for (size_t k = 0; k < 3; ++k) {
                    // Update face neighbor pointers
                    // Update children _f_ pointers for siblings
                face->children[3]->f[k] = face->children[NEXT(k)];
                face->children[k]->f[NEXT(k)] = face->children[3];
                    // Update children _f_ pointers for neighbor children
                SDFace *f2 = face->f[PREV(k)];
                face->children[k]->f[PREV(k)] =
                f2 ? f2->children[f2->vnum(face->v[k]->P)] : NULL;
                f2 = face->f[k];
                face->children[k]->f[k] =
                f2 ? f2->children[f2->vnum(face->v[k]->P)] : NULL;
                    // Update child vertex pointer to new even vertex
                face->children[k]->v[k] = face->v[k]->child;
                    // Compute odd vertex on _k_th edge
                SDVertex *v0 = face->v[k], *v1 = face->v[NEXT(k)];
                SDEdge edge(v0, v1);
                SDVertex *vert = edgeVerts[edge];
                if (!vert) {
                    edge.f[0] = face;
                    edge.f0edgeNum = k;
                        // Create and initialize new odd vertex
                    vert = new SDVertex;//new (vertexArena) SDVertex;
                    newVertices.push_back(vert);
                    vert->regular = true;
                    vert->boundary = (f2 == NULL);
                    vert->startFace = face->children[3];
                        // Apply edge rules to compute new vertex position
                    if (vert->boundary) {
                        vert->P =  &(*(newUniqueVertices.insert(0.5f * (*(v0->P) + *(v1->P))).first));
                        
                        vert->u = 0.5f * (v0->u + v1->u);
                        vert->v = 0.5f * (v0->v + v1->v);
                    } else {
                        SDVertex *ov1 = face->v[PREV(k)];
                        SDVertex *ov2 = f2->otherVert(edge.v[0]->P, edge.v[1]->P);
                        Point P = 3.f / 8.f * (*(v0->P) + *(v1->P));
                        P += 1.f / 8.f * (*(ov1->P) + *(ov2->P));
                        vert->P = &(*(newUniqueVertices.insert(P).first));
                        
                            // If UV are different on each side of the edge interpolate as boundary
                        if (f2->v[f2->vnum(v0->P)]->u == v0->u &&
                            f2->v[f2->vnum(v0->P)]->v == v0->v &&
                            f2->v[f2->vnum(v1->P)]->u == v1->u &&
                            f2->v[f2->vnum(v1->P)]->v == v1->v) {
                            vert->u = 3.f/8.f * (v0->u + v1->u);
                            vert->u += 1.f/8.f * (ov1->u + ov2->u);
                            
                            vert->v = 3.f/8.f * (v0->v + v1->v);
                            vert->v += 1.f/8.f * (ov1->v + ov2->v);
                            
                        } else {
                            vert->u = 0.5f * (v0->u + v1->u);
                            vert->v = 0.5f * (v0->v + v1->v);
                        }
                    }
                    edgeVerts[edge] = vert;
                } else {
                        // If UV are different on each side of the edge create a new vertex
                    if (!vert->boundary &&
                        (f2->v[f2->vnum(v0->P)]->u != v0->u ||
                         f2->v[f2->vnum(v0->P)]->v != v0->v ||
                         f2->v[f2->vnum(v1->P)]->u != v1->u ||
                         f2->v[f2->vnum(v1->P)]->v != v1->v)) {
                            const Point *P = vert->P;
                            SDFace *startFace = vert->startFace;
                            vert = new SDVertex;//new (vertexArena) SDVertex;
                            newVertices.push_back(vert);
                            vert->regular = true;
                            vert->boundary = false;
                            vert->startFace = startFace;
                                // Standard point interpolation
                            vert->P = P;
                                // Boundary interpolation for UV
                            vert->u = 0.5f * (v0->u + v1->u);
                            vert->v = 0.5f * (v0->v + v1->v);
                        }
                    edgeVerts.erase(edge);
                }
                    // Update face vertex pointers
                    // Update child vertex pointer to new odd vertex
                face->children[k]->v[NEXT(k)] = vert;
                face->children[NEXT(k)]->v[k] = vert;
                face->children[3]->v[k] = vert;
            }
        }
        edgeVerts.clear();
        
            // Prepare for next level of subdivision
        f = newFaces;
        v = newVertices;
            // Swap to preserve data location since pointers are held by SDVertex
        u.swap(newUniqueVertices);
    }
        // Check for degenerate faces
    for (size_t i = 0; i < f.size(); ++i) {
        CheckDegenerate(f[i]);
    }
    
        // Push vertices to limit surface
    SDVertex *Vlimit = new SDVertex[v.size()];
    set<Point, PointCompare> uniqueLimit;
    for (size_t i = 0; i < v.size(); ++i) {
            // Skip unused vertices
        if (!v[i]->startFace)
            continue;
        if (v[i]->boundary)
            weightBoundary(uniqueLimit, &Vlimit[i], v[i], 1.f/5.f);
        else
            weightOneRing(uniqueLimit, &Vlimit[i], v[i], gamma((int)v[i]->valence()));
    }
    for (size_t i = 0; i < v.size(); ++i) {
        v[i]->P = Vlimit[i].P;
//        v[i]->u = Vlimit[i].u;
//        v[i]->v = Vlimit[i].v;
    }
    delete[] Vlimit;
        // Swap to preserve data location since pointers are held by SDVertex
    u.swap(uniqueLimit);
    uniqueLimit.clear();
    
        // Create _TriangleMesh_ from subdivision mesh
    size_t ntris = f.size();
    size_t nverts = v.size();
    int *trimesh_verts = new int[3*ntris];
    int *trimesh_vp = trimesh_verts;
    map<SDVertex *, int> usedVerts;
    for (size_t i = 0; i < nverts; ++i)
        usedVerts[v[i]] = (int)i;
    for (size_t i = 0; i < ntris; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            *trimesh_vp = usedVerts[f[i]->v[j]];
            ++trimesh_vp;
        }
    }

        // Dade - calculate vertex UVs if required
    uv *UVLimit = NULL;
    if (hasUV) {
        UVLimit = new uv[nverts];
        for (size_t i = 0; i < nverts; ++i) {
            UVLimit[i].u = v[i]->u;
            UVLimit[i].v = v[i]->v;
        }
    }
    
        // Dade - create trianglemesh vertices
        // Dummy initialization for unused vertices
    Point *Plimit = new Point[nverts];
    for (size_t i = 0; i < nverts; ++i) {
        if (v[i]->startFace)
            Plimit[i] = *(v[i]->P);
        else
            Plimit[i] = Point(0, 0, 0);
    }

    Vector *Ns = new Vector[v.size()];

    size_t ringSize = 16;
    Point *Pring = new Point[ringSize];
    for (size_t i = 0; i < v.size(); ++i) {
        SDVertex *vert = v[i];
            // Skip unused vertices
        if (!vert->startFace)
            continue;
        Vector S(0,0,0), T(0,0,0);
        size_t valence = vert->valence();
        if (valence > ringSize) {
            ringSize = valence;
            delete[] Pring;
            Pring = new Point[ringSize];
        }
        vert->oneRing(Pring);
        
        if (!vert->boundary) {
                // Compute tangents of interior face
            for (size_t k = 0; k < valence; ++k) {
                S += Vector(Pring[k]) * cosf(2.f*M_PI*k/valence);
                T += Vector(Pring[k]) * sinf(2.f*M_PI*k/valence);
            }
        } else {
                // Compute tangents of boundary face
            S = Pring[valence-1] - Pring[0];
            if (valence == 2)
                T = Vector(Pring[0] + Pring[1] - 2 * *(vert->P));
            else if (valence == 3)
                T = Pring[1] - *(vert->P);
            else if (valence == 4) // regular
                T = Vector(-1*Pring[0] + 2*Pring[1] + 2*Pring[2] +
                           -1*Pring[3] + -2* *(vert->P));
            else {
                float theta = M_PI / static_cast<float>(valence - 1);
                T = Vector(sinf(theta) * (Pring[0] + Pring[valence-1]));
                for (size_t k = 1; k < valence - 1; ++k) {
                    float wt = (2*cosf(theta) - 2) * sinf((k) * theta);
                    T += Vector(wt * Pring[k]);
                }
                T = -T;
            }
        }
        Ns[i] = Vector(normalize(cross(T, S)));
    }
    
    LOG_INFO("Subdivision complete, got " << ntris << " triangles and " << nverts << " vertices");
    
    m_trimesh = TriangleMesh(o2c,
                             c2o,
                             (int)ntris,
                             (int)nverts,
                             trimesh_verts,
                             Plimit,
                             Ns,
                             UVLimit
                             );
    
    delete[] Pring;
    delete[] Ns;
    delete[] UVLimit;
    delete[] Plimit;
    delete[] trimesh_verts;
    delete[] vertices[0];
    delete[] faces[0];
}


void LoopSubdivMesh::weightOneRing(set<Point, PointCompare> &unique, SDVertex *destVert, SDVertex *vert,
                               float beta) const
{
        // Put _vert_ one-ring in _Pring_
    size_t valence = vert->valence();
    SDVertex **Vring = (SDVertex **)alloca(valence * sizeof(SDVertex *));
    SDVertex **VR = Vring;
        // Get one ring vertices for interior vertex
    SDFace *face = vert->startFace;
    bool uvSplit = false;
    do {
        SDVertex *v = face->v[face->vnum(vert->P)];
        
        if (v->u != vert->u || v->v != vert->v)
            uvSplit = true;
        
        SDVertex *v2 = face->nextVert(vert->P);
        float vu = v2->u;
        float vv = v2->v;
        
        *VR++ = v2;
        if (face != face->nextFace(vert->P)->prevFace(vert->P))
            break;
        face = face->nextFace(vert->P);
        v2 = face->prevVert(vert->P);
        
        if (vu != v2->u || vv != v2->v)
            uvSplit = true;
    } while (face != vert->startFace);
    
    Point P((1 - valence * beta) * *(vert->P));
    float u = (1 - valence * beta) * vert->u;
    float v = (1 - valence * beta) * vert->v;
    
    for (size_t i = 0; i < valence; ++i) {
        P += beta * *(Vring[i]->P);
        u += beta * Vring[i]->u;
        v += beta * Vring[i]->v;
    }
    
    destVert->P = &(*(unique.insert(P).first));
    if (uvSplit) {
        destVert->u = vert->u;
        destVert->v = vert->v;
    } else {
        destVert->u = u;
        destVert->v = v;
    }
}

void SDVertex::oneRing(Point *Pring) const
{
    if (!boundary) {
            // Get one ring vertices for interior vertex
        SDFace *face = startFace;
        do {
            *Pring++ = *(face->nextVert(P)->P);
            if (face != face->nextFace(P)->prevFace(P))
                break;
            face = face->nextFace(P);
        } while (face != startFace);
    } else {
            // Get one ring vertices for boundary vertex
        SDFace *face = startFace, *f2;
        while ((f2 = face->nextFace(P)) != NULL)
            face = f2;
        *Pring++ = *(face->nextVert(P)->P);
        do {
            *Pring++ = *(face->prevVert(P)->P);
            face = face->prevFace(P);
        } while (face != NULL);
    }
}

void LoopSubdivMesh::weightBoundary(set<Point, PointCompare> &unique, SDVertex *destVert,  SDVertex *vert,
                                float beta) const
{
        // Put _vert_ one-ring in _Pring_
    size_t valence = vert->valence();

    SDVertex **Vring = (SDVertex **)alloca(valence * sizeof(SDVertex *));
    SDVertex **VR = Vring;
        // Get one ring vertices for boundary vertex
    SDFace *face = vert->startFace, *f2;
        // Go to the last face in the list
    while ((f2 = face->nextFace(vert->P)) != NULL)
        face = f2;
        // Add the last vertex (on the boundary)
    *VR++ = face->nextVert(vert->P);
        // Add all vertices up to the first one (on the boundary)
    bool uvSplit = false;
    do {
        SDVertex *v = face->v[face->vnum(vert->P)];
        
        if (v->u != vert->u || v->v != vert->v)
            uvSplit = true;
        
        SDVertex *v2 = face->prevVert(vert->P);
        float vu = v2->u;
        float vv = v2->v;
        
        *VR++ = v2;
        face = face->prevFace(vert->P);
        if (face) {
            v2 = face->nextVert(vert->P);
            
            if (vu != v2->u || vv != v2->v)
                uvSplit = true;
        }
    } while (face != NULL);
    
    Point P((1 - 2 * beta) * *(vert->P));
    P += beta * *(Vring[0]->P);
    P += beta * *(Vring[valence - 1]->P);
    destVert->P = &(*(unique.insert(P).first));
    
    if (uvSplit) {
        destVert->u = vert->u;
        destVert->v = vert->v;
    } else {
        float u = (1.f - 2.f * beta) * vert->u;
        float v = (1.f - 2.f * beta) * vert->v;
        u += beta * (Vring[0]->u + Vring[valence - 1]->u);
        v += beta * (Vring[0]->v + Vring[valence - 1]->v);
        destVert->u = u;
        destVert->v = v;
    }
}

size_t SDVertex::valence() const {
    SDFace *f = startFace;
    if (!boundary) {
            // Compute valence of interior vertex
        size_t nf = 0;
        do {
            SDVertex *v = f->v[f->vnum(P)];
            if (v->startFace != startFace)
                v->startFace = startFace;
            ++nf;
            if (f != f->nextFace(v->P)->prevFace(v->P))
                break;
            f = f->nextFace(v->P);
        } while (f != startFace);
        if (f != startFace)
            LOG_WARNING("abnormal face sequence");
        return nf;
    } else {
            // Compute valence of boundary vertex
        size_t nf = 0;
        while (f) {
            SDVertex *v = f->v[f->vnum(P)];
            if (v->startFace != startFace)
                v->startFace = startFace;
            ++nf;
            f = f->nextFace(v->P);
        }
        
        f = startFace;
        while (f) {
            SDVertex *v = f->v[f->vnum(P)];
            if (v->startFace != startFace)
                v->startFace = startFace;
            ++nf;
            f = f->prevFace(v->P);
        }
        return nf;
    }
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



