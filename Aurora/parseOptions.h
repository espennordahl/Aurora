//
//  parseOptions.h
//  Aurora
//
//  Created by Espen Nordahl on 1/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_parseOptions_h
#define Aurora_parseOptions_h

#include "options.h"
#include "log.h"
#define lcontext LOG_Parser

#include "json.h"


inline void parseGlobals(Json::Value &root, std::map<Option, double> *globals){
    if( root.size() > 0 ) {
            // Loop through options
	    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
            LOG_DEBUG("Found Option: " << itr.key().asString());
            Json::Value value = *itr;
                // Resolution
            if (itr.key().asString() == "resolution") {
                int resolution[2];
                Json::Value res = *itr;
                int i = 0;
                for( Json::ValueIterator resItr = res.begin() ; resItr != res.end() ; resItr++ ) {
                    Json::Value v = *resItr;
                    resolution[i] = v.asInt();
                    i++;
                }
                (*globals)[ResolutionX] = resolution[0];
                (*globals)[ResolutionY] = resolution[1];
            }
                // pixelSamples
            else if (itr.key().asString() == "pixelsamples") {
                (*globals)[PixelSamples] = value.asInt();
            }
            else if (itr.key().asString() == "lightsamples") {
                (*globals)[LightSamples] = value.asInt();
            }
            else if (itr.key().asString() == "mindepth") {
                (*globals)[MinDepth] = value.asInt();
            }
            else if (itr.key().asString() == "maxdepth") {
                (*globals)[MaxDepth] = value.asInt();
            }
            else if (itr.key().asString() == "useEmbree") {
                double useEmbree = 0;
                useEmbree = value.asDouble();
                if (useEmbree > 0.) {
                    (*globals)[AccelStructure] = ACCEL_EMBREE;
                }
            }
                // unknown
            else {
                LOG_WARNING("Unable to parse global attribute: " << itr.key().asString());
            }
            
        }
    }
}

#undef lcontext

#endif
