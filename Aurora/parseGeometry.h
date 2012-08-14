//
//  parseGeometry.h
//  Aurora
//
//  Created by Espen Nordahl on 1/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_parseGeometry_h
#define Aurora_parseGeometry_h


#include "core.h"
#include "options.h"
#include "log.h"
#define lcontext LOG_Parser

#include "json.h"

namespace Aurora {
    inline void parseMatteMaterial(Json::Value &root, std::map<Option, double> globals, Reference<Material> *material){
        if( root.size() > 0 ) {
            Color col;
            for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
                if (itr.key().asString() == "color") {
                    Json::Value c = *itr;
                    int i = 0;
                    for( Json::ValueIterator colItr = c.begin() ; colItr != c.end(); colItr++ ) {
                        Json::Value v = *colItr;
                        col[i] = v.asDouble();
                        i++;
                    }
                }
            }
            *material = new MatteMaterial(col, globals[LightSamples]);
        }
        else {
            LOG_ERROR("Empty material.");
        }
    }

    inline void parseKelemenMaterial(Json::Value &root, std::map<Option, double> globals, Reference<Material> *material){
        if( root.size() > 0 ) {
            Color diffCol;
            Color specCol;
            float reflectance;
            float exponent;
            for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
                if (itr.key().asString() == "diffusecolor") {
                    Json::Value c = *itr;
                    int i = 0;
                    for( Json::ValueIterator colItr = c.begin() ; colItr != c.end(); colItr++ ) {
                        Json::Value v = *colItr;
                        diffCol[i] = v.asDouble();
                        i++;
                    }
                }
                else if (itr.key().asString() == "speccolor") {
                    Json::Value c = *itr;
                    int i = 0;
                    for( Json::ValueIterator colItr = c.begin() ; colItr != c.end(); colItr++ ) {
                        Json::Value v = *colItr;
                        specCol[i] = v.asDouble();
                        i++;
                    }
                }
                else if (itr.key().asString() == "reflectance") {
                    Json::Value v = *itr;
                    reflectance = v.asDouble();
                }
                else if (itr.key().asString() == "exponent") {
                    Json::Value v = *itr;
                    exponent = v.asDouble();
                }
            }
            *material = new KelemenMaterial(diffCol, specCol, exponent, reflectance, globals[LightSamples]);
        }   
        else {
            LOG_ERROR("Empty material.");
        }
    }

    
    inline void parseObject(Json::Value &root, std::vector<Reference<AuroraObject> > &objects, std::vector<Reference<Light> > &lights, Transform camTrans, std::map<Option, double> globals){
        
        
        if( root.size() > 0 ) {
                // first loop contains only the name
            for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
                LOG_DEBUG("Found object: " << itr.key().asString());
                Transform invCam = camTrans.inverse(camTrans);
                Transform *transStack = new Transform(invCam);
                Json::Value obj = *itr;
                
                    // we first check if we're dealing with a light or mesh
                std::string objType = "";
                if (obj.size() > 0) {
                    for( Json::ValueIterator objItr = obj.begin() ; objItr != obj.end() ; objItr++ ) {
                        Json::Value value = *objItr;
                            // path
                        if (objItr.key().asString() == "type") {
                            objType = value.asString();
                        }
                    }
                }
                
                if (objType == "geometry"){
                    string objPath = "";
                    Reference<Material> material;

                    for( Json::ValueIterator objItr = obj.begin() ; objItr != obj.end() ; objItr++ ) {
                        Json::Value value = *objItr;
                            // path
                        if (objItr.key().asString() == "path") {
                            objPath = value.asString();
                        } 
                            // name
                        else if (objItr.key().asString() == "name"){
                            ;// ignore for now
                        }
                            // material                        
                        else if (objItr.key().asString() == "material") {
                            if (value.size() > 0) {
                                for( Json::ValueIterator mItr = value.begin() ; mItr != value.end() ; mItr++ ) {
                                        // we find the type and pass the task on
                                    if (mItr.key().asString() == "type") {
                                        Json::Value v = *mItr;
                                        std::string materialType = v.asString();
                                        if (materialType == "matteMaterial") {
                                            parseMatteMaterial(*objItr, globals, &material);
                                        }
                                        else if (materialType == "kelemenMaterial") {
                                            parseKelemenMaterial(*objItr, globals, &material);
                                        }
                                        else {
                                            LOG_ERROR("Couldn't find parser for material " << materialType);
                                        }
                                    }
                                }
                            }
                        } 
                            // transforms
                        else if (objItr.key().asString() == "transforms"){
                            if (value.size() > 0) {
                                for( Json::ValueIterator trItr = value.begin() ; trItr != value.end() ; trItr++ ) {
                                    Json::Value t = *trItr;
                                    if (t.begin().key().asString() == "translate") {
                                        double num[3];
                                        int i = 0;
                                        Json::Value xyz = *t.begin();
                                        for( Json::ValueIterator xyzItr = xyz.begin() ; xyzItr != xyz.end() ; xyzItr++ ) {
                                            Json::Value v = *xyzItr;
                                            num[i] = v.asDouble();
                                            i++;
                                        }
                                        LOG_DEBUG("Translating: " << num[0] << " " << num[1] << " " << num[2]);
                                        transStack->translate(Vector(num[0], num[1], num[2]));
                                    }
                                    else if (t.begin().key().asString() == "scale") {
                                        double num[3];
                                        int i = 0;
                                        Json::Value xyz = *t.begin();
                                        for( Json::ValueIterator xyzItr = xyz.begin() ; xyzItr != xyz.end() ; xyzItr++ ) {
                                            Json::Value v = *xyzItr;
                                            num[i] = v.asDouble();
                                            i++;
                                        }
                                        LOG_DEBUG("Scaling: " << num[0] << " " << num[1] << " " << num[2]);
                                        transStack->scale(num[0], num[1], num[2]);
                                    }
                                    else if (t.begin().key().asString() == "rotateX") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate X: " << num);
                                        transStack->rotateX(num);
                                    }
                                    else if (t.begin().key().asString() == "rotateY") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate Y: " << num);
                                        transStack->rotateY(num);
                                    }                                
                                    else if (t.begin().key().asString() == "rotateZ") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate Z: " << num);
                                        transStack->rotateZ(num);
                                    }                                
                                }
                            }
                        }
                            // unknown
                        else {
                            LOG_WARNING("Unable to parse Object attribute: '" << objItr.key().asString() << "' for object " << itr.key().asString());
                        }
                    }                
                
                    // Create object
                Transform *tInv = new Transform(transStack->inverse(*transStack));
                Reference<Shape> shp = new ObjTriangleMesh(transStack, tInv, objPath);
                Reference<AuroraObject> aurObj = new AuroraObject(shp, material);
                objects.push_back(aurObj);
                
                LOG_INFO("successfully parsed objMesh: " << objPath);
                }
                
                else if (objType == "light" ){
                    float xScale, yScale, exposure;
                    Color color;
                    for( Json::ValueIterator objItr = obj.begin() ; objItr != obj.end() ; objItr++ ) {
                        Json::Value value = *objItr;
                        if (objItr.key().asString() == "scaleX") {
                            xScale = value.asDouble();
                        } 

                        else if (objItr.key().asString() == "scaleY") {
                            yScale = value.asDouble();
                        } 
                        
                        else if (objItr.key().asString() == "exposure") {
                            exposure = value.asDouble();
                        } 
                        
                        else if (objItr.key().asString() == "color") {
                            int i = 0;
                            for( Json::ValueIterator cItr = value.begin() ; cItr != value.end() ; cItr++ ) {
                                Json::Value v = *cItr;
                                color[i] = v.asDouble();
                                i++;
                            }
                        }
                            // transforms
                        else if (objItr.key().asString() == "transforms"){
                            if (value.size() > 0) {
                                for( Json::ValueIterator trItr = value.begin() ; trItr != value.end() ; trItr++ ) {
                                    Json::Value t = *trItr;
                                    if (t.begin().key().asString() == "translate") {
                                        double num[3];
                                        int i = 0;
                                        Json::Value xyz = *t.begin();
                                        for( Json::ValueIterator xyzItr = xyz.begin() ; xyzItr != xyz.end() ; xyzItr++ ) {
                                            Json::Value v = *xyzItr;
                                            num[i] = v.asDouble();
                                            i++;
                                        }
                                        LOG_DEBUG("Translating: " << num[0] << " " << num[1] << " " << num[2]);
                                        transStack->translate(Vector(num[0], num[1], num[2]));
                                    }
                                    else if (t.begin().key().asString() == "scale") {
                                        double num[3];
                                        int i = 0;
                                        Json::Value xyz = *t.begin();
                                        for( Json::ValueIterator xyzItr = xyz.begin() ; xyzItr != xyz.end() ; xyzItr++ ) {
                                            Json::Value v = *xyzItr;
                                            num[i] = v.asDouble();
                                            i++;
                                        }
                                        LOG_DEBUG("Scaling: " << num[0] << " " << num[1] << " " << num[2]);
                                        transStack->scale(num[0], num[1], num[2]);
                                    }
                                    else if (t.begin().key().asString() == "rotateX") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate X: " << num);
                                        transStack->rotateX(num);
                                    }
                                    else if (t.begin().key().asString() == "rotateY") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate Y: " << num);
                                        transStack->rotateY(num);
                                    }                                
                                    else if (t.begin().key().asString() == "rotateZ") {
                                        Json::Value v = *t.begin();
                                        double num = v.asDouble();
                                        LOG_DEBUG("Rotate Z: " << num);
                                        transStack->rotateZ(num);
                                    }                                
                                }
                            }
                        }

                    }
                    Transform *tInv = new Transform(transStack->inverse(*transStack));
                    Reference<Light> sqrLight = new SquareLight(transStack, tInv, exposure, color, xScale, yScale, globals[LightSamples]);
                    lights.push_back(sqrLight);
                }
                else if (objType == "envlight" ){
                    float xScale, yScale, exposure;
                    Color color;
                    for( Json::ValueIterator objItr = obj.begin() ; objItr != obj.end() ; objItr++ ) {
                        Json::Value value = *objItr;
                        if (objItr.key().asString() == "scaleX") {
                            xScale = value.asDouble();
                        } 
                        
                        else if (objItr.key().asString() == "scaleY") {
                            yScale = value.asDouble();
                        } 
                        
                        else if (objItr.key().asString() == "exposure") {
                            exposure = value.asDouble();
                        } 
                        
                        else if (objItr.key().asString() == "color") {
                            int i = 0;
                            for( Json::ValueIterator cItr = value.begin() ; cItr != value.end() ; cItr++ ) {
                                Json::Value v = *cItr;
                                color[i] = v.asDouble();
                                i++;
                            }
                        }
                    }
                    Transform *t = new Transform();
                    Reference<Light> envLight = new InfiniteAreaLight(t, t, exposure, color, globals[LightSamples]);
                    lights.push_back(envLight);
                }
            }
        }
    }
    

}

#undef lcontext
#endif
