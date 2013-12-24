//
//  shape.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_shape_h
#define Aurora_shape_h

#include "core.h"
#include "geometry.h"

#include <accel.h>
#include "attributeState.h"
#include "attributeChange.h"

#include <vector>

namespace Aurora {
    
    struct EmbreeStructure{
        embree::BuildTriangle* triangles;
        embree::BuildVertex* vertices;
        std::vector<Vector> *normals;
        std::vector< uv > *uvs;
        int *currentTri;
        int *currentVertex;
    };

	class Shape {
	public:
		Shape( const Transform *o2c, const Transform *c2o ) : objectToCamera(o2c), cameraToObject(c2o) { };
		
		virtual ~Shape(){};
        
            // Converts the shape into an embree object. 
        virtual void makeEmbree(EmbreeStructure &geometryOut, AttributeState *attrs, int attributeIndex) = 0;

            // Handy information 
        virtual int numTriangles() = 0;
        virtual int numVertices() = 0;
        virtual BBox objectBound() const = 0;
		virtual BBox worldBound() const = 0;

        virtual void applyAttributeChange(const AttributeChange &change){
            assert(false); // TODO: Not implemented;
        }
        
        virtual void clearCache() = 0;
    
        void setDirtyGeometry(bool dirtyGeo){
            m_dirtygeo = dirtyGeo;
        }
        
        virtual bool isGeometryCached(){
            return !m_dirtygeo;
        }

    protected:
            // Object transforms. It's important that if the object moves,
            // that we create new transforms as they're potentially shared
            // for effeciency
		const Transform *objectToCamera;
		const Transform *cameraToObject;

    private:
        bool m_dirtygeo;
	};
    typedef std::tr1::shared_ptr<Shape> ShapePtr;
    
}

#endif