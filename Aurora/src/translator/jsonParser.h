//
//  jsonParser.h
//  Aurora
//
//  Created by Espen Nordahl on 20/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_jsonParser_h
#define Aurora_jsonParser_h

#include "core.h"
#include "renderEnvironment.h"
#include "auroraObject.h"
#include "light.h"

#include "json.h"

#include <string.h>
#include <fstream>

namespace Aurora {
    class JsonParser{
    public:
        JsonParser(std::string filename, RenderEnvironment *_renderEnv);
            
        std::vector<Reference<AuroraObject> >   getObjects() { return objects; };
        std::vector<Reference<Light> >          getLights()  { return lights;  };

        
            // parses the entire scene description.
            // Returns false if an error occurs, and
            // a string describing the error.
            // Returns true if it succeeds.
        bool parseScene(std::string *error);
        
    private:
            // parses the globals
        void parseOptions(Json::Value &root);
            // parses the camera settings
        void parseCamera(Json::Value &root);
            // parses all materials
        void parseMaterials(Json::Value &root);
            // parses all shaders
        void parseShaders(Json::Value &root);
            // parses geometry
        void parseGeometry(Json::Value &root);
        
            // the file on disk to parse
        std::string filename;
            // We need a RenderEnv pointer to store
            // parts of the data we generate
        RenderEnvironment *renderEnv;
        
            // We need to keep track of a few things
            // that the renderer needs to construct
            // a scene. 
            // TODO: Better way to do this than class variable?
        Transform *cameraTransform;
        std::vector<Reference<AuroraObject> > objects;
        std::vector<Reference<Light> > lights;
    };
}

#endif
