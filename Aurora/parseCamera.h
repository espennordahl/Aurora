//
//  parseCamera.h
//  Aurora
//
//  Created by Espen Nordahl on 1/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_parseCamera_h
#define Aurora_parseCamera_h

#include "core.h"
#include "options.h"
#include "log.h"
#define lcontext LOG_Parser

#include "json.h"

namespace Aurora {
    inline void parseCamera(Json::Value &root, std::map<Option, double> *globals, Transform *transStack){
        if( root.size() > 0 ) {
                // Loop through options
            for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
                LOG_DEBUG("Found camAttr: " << itr.key().asString());
                Json::Value value = *itr;
                    // pixelSamples
                if (itr.key().asString() == "fov") {
                    (*globals)[FieldOfView] = value.asDouble();
                }
                    // transforms
                else if (itr.key().asString() == "transforms"){
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
                    LOG_WARNING("Unable to parse camera attribute: " << itr.key().asString());
                }
                
            }
        }
    }
}
#undef lcontext
#endif
