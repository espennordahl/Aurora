//
//  loopSubdivMesh.cpp
//  Aurora
//
//  Created by Espen Nordahl on 2/06/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "loopSubdivMesh.h"

#include <set>

#include "triangleMesh.h"

using namespace Aurora;

LoopSubdivMesh::LoopSubdivMesh(const Transform *o2c, const Transform *c2o,
                               int nfaces, int nvertices,
                               const int *vertexIndices, const Point *P,
                               int nlevels):
Shape(o2c, c2o),
m_nlevels(nlevels)
{
        // allocate vertices and faces
    SDVertex *verts = new SDVertex[nvertices];
    for (int i=0; i < nvertices; ++i) {
        verts[i] = SDVertex(P[i]);
        m_vertices.push_back(&verts[i]);
    }
    SDFace *fs = new SDFace[nfaces];
    for(int i=0; i < nfaces; ++i){
        m_faces.push_back(&fs[i]);
    }
        // set face to vertex pointers
    const int *vp = vertexIndices;
    for(int i=0; i<nfaces; ++i){
        SDFace *f = m_faces[i];
        for(int j=0; j<3; ++j){
            SDVertex *v = m_vertices[vp[j]];
            f->v[j] = v;
            v->startFace = f;
        }
        vp += 3;
    }
        // set neighbor pointers in face
    std::set<SDEdge> edges;
    for(int i=0; i<nfaces; ++i){
        SDFace *f = m_faces[i];
        for(int edgeNum=0; edgeNum < 3; ++edgeNum){
                // update neighbor pointer for edgeNum
            int v0 = edgeNum;
            int v1 = NEXT(edgeNum);
            SDEdge e(f->v[v0], f->v[v1]);
            if(edges.find(e) == edges.end()) {
                    // handle new edge
                e.f[0] = f;
                e.f0edgeNum = edgeNum;
                edges.insert(e);
            } else {
                    // handle previously seen edge
                e = *edges.find(e);
                e.f[0]->f[e.f0edgeNum] = f;
                f->f[edgeNum] = e.f[0];
                edges.erase(e);
            }
        }
    }
        // finish vertex initialization
    for (int i=0; i<nvertices; ++i) {
        SDVertex *v = m_vertices[i];
        SDFace *f = v->startFace;
        do {
            f = f->nextFace(v);
        } while (f && f != v->startFace);
        v->boundary = (f == NULL);
        if (!v->boundary && v->valence() == 6){
            v->regular = true;
        } else if (v->boundary && v->valence() == 4){
            v->regular = true;
        } else {
            v->regular = false;
        }
    }
}

LoopSubdivMesh::~LoopSubdivMesh(){
    for (int i=0; i<m_faces.size(); ++i) {
        delete m_faces[i];
    }
    for(int i=0; i<m_vertices.size(); ++i){
        delete m_vertices[i];
    }
}

void LoopSubdivMesh::dice( std::vector<std::tr1::shared_ptr<Geometry> > &diced){
    std::vector<SDFace *> f = m_faces;
    std::vector<SDVertex *> v = m_vertices;
    for(int i=0; i<m_nlevels; ++i){
            // update f and v for next level of subdivision
        std::vector<SDFace *> newFaces;
        std::vector<SDVertex *> newVertices;
            // allocate next level of children in mesh tree
        for(size_t j=0; j<v.size(); ++j){
            v[j]->child = new SDVertex;
            v[j]->child->regular = v[j]->regular;
            v[j]->child->boundary = v[j]->boundary;
            newVertices.push_back(v[j]->child);
        }
        for(size_t j=0; j<f.size(); ++j){
            for(int k=0; k<4; ++k){
                f[j]->children[k] = new SDFace;
                newFaces.push_back(f[j]->children[k]);
            }
        }
        // update vertex positions and create new edge vertices
            // update vertex position for even vertices
        for(size_t j=0; j<v.size(); ++j){
            if(!v[j]->boundary){
                    // apply one-ring rule for even vertices
                if(v[j]->regular){
                    v[j]->child->P = weightOneRing(v[j], 1.f/16.f);
                } else {
                    v[j]->child->P = weightOneRing(v[j], beta(v[j]->valence()));
                }
            } else {
                    // apply boundary rule for even vertices
                v[j]->child->P = weightBoundary(v[j], 1.f/8.f);
            }
        }
            // compute new odd edge vertices
        std::map<SDEdge, SDVertex *> edgeVerts;
        for(size_t j = 0; j < f.size(); ++j){
            SDFace *face = f[j];
            for(int k=0; k < 3; ++k){
                    //compute odd vertex on kth edge
                SDEdge edge(face->v[k], face->v[NEXT(k)]);
                SDVertex *vert = edgeVerts[edge];
                if(!vert) {
                        // create and initialize new odd vertex
                    vert = new SDVertex;
                    newVertices.push_back(vert);
                    vert->regular = true;
                    vert->boundary = (face->f[k] == NULL);
                    vert->startFace = face->children[3];
                        // apply edge rules to compute new vertex position
                    if(vert->boundary){
                        vert->P =  0.5f * edge.v[0]->P;
                        vert->P += 0.5f * edge.v[1]->P;
                    } else {
                        vert->P =  3.f/8.f * edge.v[0]->P;
                        vert->P =  3.f/8.f * edge.v[1]->P;
                        vert->P += 1.f/8.f * face->otherVert(edge.v[0], edge.v[1])->P;
                        vert->P += 1.f/8.f * face->f[k]->otherVert(edge.v[0], edge.v[1])->P;
                    }
                    edgeVerts[edge] = vert;
                }
            }
        }
        // update new mesh topology
            // update even vertex face pointers
        for (size_t j=0; j<v.size(); ++j) {
            SDVertex *vert = v[j];
            int vertNum = vert->startFace->vnum(vert);
            vert->child->startFace = vert->startFace->children[vertNum];
        }
            // update face neighbor pointers
        for (size_t j=0; j<f.size(); ++j) {
            SDFace *face = f[j];
            for (int k=0; k<3; ++k) {
                face->children[3]->f[k] = face->children[NEXT(k)];
                face->children[j]->f[NEXT(k)] = face->children[3];
                
                SDFace *f2 = face->f[k];
                face->children[k]->f[k] = f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
                f2 = face->f[PREV(k)];
                face->children[k]->f[PREV(k)] = f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
            }
        }
            // update face vertex pointers
        for(size_t j=0; j<f.size(); ++j){
            SDFace *face = f[j];
            for (int k=0; k < 3; ++k) {
                face->children[k]->v[k] = face->v[k]->child;
                
                SDVertex *vert = edgeVerts[SDEdge(face->v[k], face->v[NEXT(k)])];
                face->children[k]->v[NEXT(k)] = vert;
                face->children[NEXT(k)]->v[k] = vert;
                face->children[3]->v[k] = vert;
            }
        }
        
        // prepare for next level of subdivison
        f = newFaces;
        v = newVertices;
    }
        // push vertices to limit surface
    Point *Plimit = new Point[v.size()];
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i]->boundary){
            Plimit[i] = weightBoundary(v[i], 1.f/5.f);
        } else {
            Plimit[i] = weightBoundary(v[i], gamma(v[i]->valence()));
        }
    }
    
        // compute vertex tangents on limit surface
    std::vector<Vector> Ns;
    Ns.reserve(v.size());
    std::vector<Point> Pring(16, Point());
    for (size_t i=0; i < v.size(); ++i) {
        SDVertex *vert = v[i];
        Vector S(0,0,0), T(0,0,0);
        int valence = vert->valence();
        if(valence > (int)Pring.size()){
            Pring.resize(valence);
        }
        vert->oneRing(&Pring[0]);
        if (!vert->boundary) {
                // compute tangents of interior face
            for(int k=0; k<valence; ++k){
                S += Vector(Pring[k]) * cosf(2.f * M_PI * k/valence);
                T += Vector(Pring[k]) * sinf(2.f * M_PI * k/valence);
            }
        } else {
                // copmute tangents of boundary face
            S = Pring[valence-1] - Pring[0];
            if(valence == 2){
                T = Vector(Pring[0] + Pring[1] - 2.f * vert->P);
            } else if (valence == 3){
                T = Pring[1] - vert->P;
            } else if (valence == 4){
                T = Vector(-1*Pring[0] + 2*Pring[1] + 2*Pring[2] + -1*Pring[3] + -2*vert->P);
            } else {
                float theta = M_PI / float(valence-1);
                T = Vector(sinf(theta) * (Pring[0] + Pring[valence-1]));
                for (int k=1; k < valence-1; ++k) {
                    float wt = (2*cosf(theta) - 2) * sinf((k) * theta);
                    T += Vector(wt * Pring[k]);
                }
            }
        }
        Ns.push_back(cross(S, T));
    }

        // create TriangeMesh from subdivisonmesh
    uint32_t ntris = uint32_t(f.size());
    int *verts = new int[3*ntris];
    int *vp = verts;
    uint32_t totVerts = uint32_t(v.size());
    map<SDVertex *, int> usedVerts;
    for (uint32_t i = 0; i < totVerts; ++i){
        usedVerts[v[i]] = i;
    }
    for (uint32_t i = 0; i < ntris; ++i) {
        for (int j = 0; j < 3; ++j) {
            *vp = usedVerts[f[i]->v[j]];
            ++vp;
        }
    }
    
    uv *UVs = new uv[totVerts];

    TriangleMesh trimesh = TriangleMesh(objectToCamera,
                                        cameraToObject,
                                        ntris,
                                        totVerts,
                                        (int)Ns.size(),
                                        verts,
                                        verts, // norm Index... assumed to be the same as vert index
                                        Plimit,
                                        &Ns[0],
                                        UVs
                                        );
    trimesh.dice(diced);
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



