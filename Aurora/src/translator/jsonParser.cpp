//
//  jsonParser.cpp
//  Aurora
//
//  Created by Espen Nordahl on 20/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "shapes.h"
#include "lights.h"
#include "materials.h"
#include "shadingEngine.h"
#include "shaders.h"
#include "log.h"
#define lcontext LOG_JsonParser

#include <iostream>

#include "jsonParser.h"


using namespace Aurora;

JsonParser::JsonParser(std::string _filename, RenderEnvironment *_renderEnv):
renderEnv(_renderEnv),
filename(_filename)
{
}

bool JsonParser::parseScene(std::string *error){
    
        // root of the json hierarchy
    Json::Value deserializeRoot;
        // json reader object
    Json::Reader reader;

    std::ifstream sceneDescription( filename.c_str() );
    if ( !reader.parse(sceneDescription, deserializeRoot) )
        LOG_ERROR("Couldn't parse file: " << filename);    
    else{
        LOG_INFO("Parsing json file: " << filename);

        if( deserializeRoot.size() <= 0 ) {        
            LOG_ERROR("No keys in json object: " << filename);
        }
        else {
            // TODO: check for invalid/unknown keys
            
            parseOptions(deserializeRoot);
            parseCamera(deserializeRoot);
            parseShaders(deserializeRoot);
            parseMaterials(deserializeRoot);
            parseGeometry(deserializeRoot);
        
        }
    }

    return true;
}

inline double getDoubleAttr(const std::string &name, Json::Value &root){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    return value.asDouble();
}

inline void getDoubleArrayAttr(const std::string &name, Json::Value &root, int numIndices, double *outDouble){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    int i = 0;
    for( Json::ValueIterator Itr = value.begin() ; Itr != value.end() ; Itr++ ) {
        if (i >= numIndices) {
            LOG_ERROR("Json double array overflows expected number of indices.");
        }
        Json::Value v = *Itr;
        outDouble[i] = v.asDouble();
        i++;
    }
    if (i != numIndices) {
        LOG_ERROR("Not enought indices in Json double array.");
    }
}

inline Color getColorAttr(const std::string &name, Json::Value &root){
    double col[3];
    getDoubleArrayAttr(name, root, 3, col);
    
    return Color(col[0], col[1], col[2]);
}


inline int getIntAttr(const std::string &name, Json::Value &root){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    return value.asInt();
}

inline void getIntArrayAttr(const std::string &name, Json::Value &root, int numIndices, int *outInt){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    int i = 0;
    for( Json::ValueIterator Itr = value.begin() ; Itr != value.end() ; Itr++ ) {
        if (i >= numIndices) {
            LOG_ERROR("Json int array overflows expected number of indices.");
        }
        Json::Value v = *Itr;
        outInt[i] = v.asInt();
        i++;
    }
    if (i != numIndices) {
        LOG_ERROR("Not enought indices in Json int array.");
    }
}

inline std::string getStringAttr(const std::string &name, Json::Value &root){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    return value.asString();
}

inline bool isAttributeString(const std::string &name, Json::Value &root){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't find json attribute: " << name);
    }
    return value.isString();
}

inline ShaderAttribute getShaderAttr(const valueType &type, const std::string &name, Json::Value &root){
        // we first check if the json attribute is a string
    if (isAttributeString(name, root)) {
            // if it is, it means it's a previously parsed shader,
            // and we return its identifyer
        return ShaderAttribute(getStringAttr(name, root));
    }
    else{
            // if not, we assume the value is of the requested type
        switch (type) {
            case floatAttr:
                return ShaderAttribute((float)getDoubleAttr(name, root));
            case colorAttr:
                return ShaderAttribute(getColorAttr(name, root));
            default:
                LOG_ERROR("getShaderAttr not implemented for type: " << type);
        }
    }
        // should never get this far
    return ShaderAttribute(0.f);
}

    // TODO: Clean this one up.
inline Transform getTransformAttr(const std::string &name, Json::Value &root){
    Json::Value errorVal = Json::Value(-99999); //TODO: Very temp
    Json::Value value = root.get( name.c_str(), errorVal );
    if (value == errorVal) {
        LOG_ERROR("Can't json attribute: " << name);
    }
    Transform trans;
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
                trans.translate(Vector(num[0], num[1], num[2]));
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
                trans.scale(num[0], num[1], num[2]);
            }
            else if (t.begin().key().asString() == "rotateX") {
                Json::Value v = *t.begin();
                double num = v.asDouble();
                LOG_DEBUG("Rotate X: " << num);
                trans.rotateX(num);
            }
            else if (t.begin().key().asString() == "rotateY") {
                Json::Value v = *t.begin();
                double num = v.asDouble();
                LOG_DEBUG("Rotate Y: " << num);
                trans.rotateY(num);
            }                                
            else if (t.begin().key().asString() == "rotateZ") {
                Json::Value v = *t.begin();
                double num = v.asDouble();
                LOG_DEBUG("Rotate Z: " << num);
                trans.rotateZ(num);
            }                                
        }
    }
    return trans;
}

inline std::tr1::shared_ptr<AuroraObject> getGeometry(Transform cameraTransform, Json::Value &root, RenderEnvironment *renderEnv){
        // read all standard data
    std::string path        = getStringAttr("path", root);
    std::string name        = getStringAttr("name", root);
    std::string material    = getStringAttr("material", root);
    Transform objTransform = getTransformAttr("transforms", root);
    int subdlevels          = getIntAttr("subdlevels", root);

        // construct transforms
    Transform invCam = cameraTransform.inverse(cameraTransform); // inverse came
    Transform *objStack = new Transform(invCam * objTransform); // obj in cam space
    Transform *objInv = new Transform(objStack->inverse(*objStack)); // inv obj in cam space
    shared_ptr<Shape> shp = shared_ptr<Shape>(new ObjTriangleMesh(objStack, objInv, subdlevels, path));
    shared_ptr<AuroraObject> aurObj = shared_ptr<AuroraObject>(new AuroraObject(name, renderEnv, shp, material));

    return aurObj;
}

inline Light* getLight(Transform cameraTransform, Json::Value &root, int lightSamples, RenderEnvironment *renderEnv){
    Transform invCam = cameraTransform.inverse(cameraTransform);
        // read all standard data
        // TODO:    std::string name        = getStringAttr("name", root);
    Transform *objTransform  = new Transform(invCam * getTransformAttr("transforms", root));    
    double xScale           = getDoubleAttr("scaleX", root);
    double yScale           = getDoubleAttr("scaleY", root);
    double exposure         = getDoubleAttr("exposure", root);    
    Color color             = getColorAttr("color", root);
    std::string name        = getStringAttr("name", root);

        // construct transforms
    Transform *c2o = new Transform(objTransform->inverse(*objTransform));
    Transform *c2w = new Transform(cameraTransform);
    Transform *w2c = new Transform(cameraTransform.inverse(cameraTransform));
    Transform *o2w = new Transform(*objTransform * *c2w);
    Transform *w2o = new Transform(*w2c * *c2o);
    Light* sqrLight = new SquareLight(objTransform, c2o, o2w, w2o, c2w, w2c, exposure, color, xScale, yScale, name, renderEnv);
    
    return sqrLight;
}

inline Light* getEnvLight(Transform cameraTransform, Json::Value &root, int lightSamples, RenderEnvironment *renderEnv){
        // read all standard data
        // TODO:    std::string name        = getStringAttr("name", root);
    Transform *objTransform  = new Transform(getTransformAttr("transforms", root));    
    double exposure         = getDoubleAttr("exposure", root);    
    Color color             = getColorAttr("color", root);
    std::string envmap      = getStringAttr("envmap", root);
    std::string name        = getStringAttr("name", root);
    
        // construct transforms
    Transform *c2o = new Transform(objTransform->inverse(*objTransform));
    Transform *c2w = new Transform(cameraTransform);
    Transform *w2c = new Transform(cameraTransform.inverse(cameraTransform));
    Transform *o2w = new Transform(*objTransform * *c2w);
    Transform *w2o = new Transform(*w2c * *c2o);
    Light* envLight = new InfiniteAreaLight(objTransform, c2o, o2w, w2o, c2w, w2c, exposure, color, envmap, name, renderEnv);
    return envLight;
}

inline int initShader(const std::string &name, const ShaderAttribute &attr, const valueType &type, RenderEnvironment *renderEnv){
    if (attr.isType(type) || attr.isType(stringAttr)) {
        
        switch (attr.getType()) {
            case colorAttr:{
                Shader<Color> *shd = new ConstantShader<Color>(attr.getColor());
                return renderEnv->shadingEngine->registerShaderColor(name, shd);
            }
            case floatAttr:{
                Shader<float> *shd = new ConstantShader<float>(attr.getFloat());
                return renderEnv->shadingEngine->registerShaderFloat(name, shd);
            }
            case stringAttr:{
                LOG_DEBUG("Assigning shader " << attr.getString() << " to requested " << name);
                return renderEnv->shadingEngine->getShaderIndex(attr.getString());
            }
            default:{
                LOG_ERROR("Failed to initialize shader. No shader type for " << type);
                break;
            }
        }
    }
    else{
        LOG_ERROR("Attribute passed not of selected type: " << type);
    }
        // we're not supposed to ever get this far
    return -1;
}

inline Material * getMaterial(std::string type, std::string name, Json::Value &root, RenderEnvironment *renderEnv){
    
    Material * mat;

    if (type == "kelemenMaterial") {
            // diff color
        ShaderAttribute diffCol       = getShaderAttr(colorAttr, "diffusecolor", root);
        int diffColIndex = initShader(name + ":diffuseAlbedo", diffCol, colorAttr, renderEnv);
            // spec color
        ShaderAttribute specCol       = getShaderAttr(colorAttr, "speccolor", root);
        int specColIndex = initShader(name + ":specularColor", specCol, colorAttr, renderEnv);
            // reflectance
        ShaderAttribute reflectance   = getShaderAttr(floatAttr, "reflectance", root);
        //initShader(name + ":specReflectance", reflectance, floatAttr, renderEnv); TODO: Varying reflectance
            // roughness
        ShaderAttribute roughness      = getShaderAttr(floatAttr, "roughness", root);
        int roughnessIndex = initShader(name + ":specRoughness", roughness, floatAttr, renderEnv);

        mat = new KelemenMaterial(name, renderEnv,
                                  diffColIndex, specColIndex,
                                  roughnessIndex, reflectance.getFloat());
    }
    else if (type == "matteMaterial") {
        ShaderAttribute col = getShaderAttr(colorAttr, "color", root);
        int colorIndex = initShader(name + ":diffColor", col, colorAttr, renderEnv);
        mat = new MatteMaterial(name, colorIndex, (*renderEnv->globals)[LightSamples] ,renderEnv);
    }
    else if (type == "carMaterial"){
//            // base diffuse
//        ShaderAttribute diffCol = getShaderAttr(colorAttr, "diffusecolor", root);
//        int diffuseColIndex = initShader(name + ":diffuseAlbedo", diffCol, colorAttr, renderEnv);
//
//            // base spec bottom
//        ShaderAttribute baseSpecBottomReflectance = getShaderAttr(floatAttr, "basespecbottomreflectance", root);
//
//        ShaderAttribute baseSpecBottomRoughness = getShaderAttr(floatAttr, "basespecbottomroughness", root);
//        int baseSpecBottomRoughIndex = initShader(name + ":baseSpecBottomRoughness", baseSpecBottomRoughness, floatAttr, renderEnv);
//        
//            // base paint
//        ShaderAttribute basePaintColor = getShaderAttr(colorAttr, "bottompaintcolor", root);
//        int basePaintColIndex = initShader(name + ":bottomPaintColor", basePaintColor, colorAttr, renderEnv);
//        int basePaintThicknessIndex = 0; // TODO: Make a shader attr
//
//            // base spec top
//        ShaderAttribute baseSpecTopReflectance = getShaderAttr(floatAttr, "basespectopreflectance", root);
//        
//        ShaderAttribute baseSpecTopRoughness = getShaderAttr(floatAttr, "basespectoproughness", root);
//        int baseSpecTopRoughIndex = initShader(name + ":baseSpecTopRoughness", baseSpecTopRoughness, floatAttr, renderEnv);
//        
//            // top paint
//        ShaderAttribute topPaintColor = getShaderAttr(colorAttr, "toppaintcolor", root);
//        int topPaintColIndex = initShader(name + ":topPaintColor", topPaintColor, colorAttr, renderEnv);
//        int topPaintThicknessIndex = 0; // TODO: Make a shader attr
//        
//            // clearcoat
//        ShaderAttribute clearcoatReflectance = getShaderAttr(floatAttr, "clearcoatreflectance", root);
//        
//        ShaderAttribute clearcoatRoughness = getShaderAttr(floatAttr, "clearcoatroughness", root);
//        int clearcoatRoughnessIndex = initShader(name + ":clearcoatroughness", clearcoatRoughness, floatAttr, renderEnv);
//        
//        mat = new CarMaterial(name, renderEnv,
//                    diffuseColIndex,
//                    baseSpecBottomReflectance.getFloat(),
//                    baseSpecBottomRoughIndex,
//                    basePaintColIndex,
//                    basePaintThicknessIndex,
//                    baseSpecTopReflectance.getFloat(),
//                    baseSpecTopRoughIndex,
//                    topPaintColIndex,
//                    topPaintThicknessIndex,
//                    clearcoatReflectance.getFloat(),
//                    clearcoatRoughnessIndex,
//                    (*renderEnv->globals)[LightSamples]);
//
    }
    else if (type == "glassMaterial") {
//        ShaderAttribute specCol       = getShaderAttr(colorAttr, "specColor", root);
//        ShaderAttribute reflectance   = getShaderAttr(floatAttr, "reflectance", root);
//        ShaderAttribute ior      = getShaderAttr(floatAttr, "ior", root);
//        mat = new GlassMaterial(name, specCol.getColor(), Color(1.), reflectance.getFloat(), ior.getFloat(), renderEnv);
    }
    else {
        LOG_ERROR("Unknown material: " << type);
    }
    return mat;
}

void JsonParser::parseOptions(Json::Value &root){
    LOG_DEBUG("Parsing Options.");
    bool foundOptions = false;
        // Options
    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
        std::string key = itr.key().asString();
        if (key == "options") {
            foundOptions = true;
            
                // read all standard data
            int r_pixelSamples =   getIntAttr("pixelsamples", *itr);
            int r_lightSamples =   getIntAttr("lightsamples", *itr);
            int r_mindepth =       getIntAttr("mindepth", *itr);
            int r_maxdepth =       getIntAttr("maxdepth", *itr);
            int r_useEmbree =      getIntAttr("useEmbree", *itr);
            int r_resolution[2];   getIntArrayAttr("resolution", *itr, 2, r_resolution);
            std::string r_fileName = getStringAttr("filename", *itr);
            r_fileName = stringTemplate(r_fileName);
                // TODO: check for any ones we've skipped and warn the user
            
                // TODO: handle errors gracefully
            
                // apply to globals
            LOG_DEBUG("Setting pixel samples to: " << r_pixelSamples);
            (*renderEnv->globals)[PixelSamples] = (double)r_pixelSamples;
            LOG_DEBUG("Setting light samples to: " << r_lightSamples);
            (*renderEnv->globals)[LightSamples] = (double)r_lightSamples;
            LOG_DEBUG("Setting min trace depth to: " << r_mindepth);
            (*renderEnv->globals)[MinDepth] = (double)r_mindepth;
            LOG_DEBUG("Setting max trace depth to: " << r_maxdepth);
            (*renderEnv->globals)[MaxDepth] = (double)r_maxdepth;
            LOG_DEBUG("Setting trace kernel to: " << (r_useEmbree > 0 ? "embree" : "aurora") );
            if (r_useEmbree > 0.) {
                (*renderEnv->globals)[AccelStructure] = ACCEL_EMBREE;
            }
            LOG_DEBUG("Setting resolution to: " << r_resolution[0] << " * " << r_resolution[1]);
            (*renderEnv->globals)[ResolutionX] = r_resolution[0];
            (*renderEnv->globals)[ResolutionY] = r_resolution[1];
            LOG_DEBUG("Setting file output to: " << r_fileName);
            (*renderEnv->stringGlobals)["fileName"] = r_fileName;
            
            // We're done, so we exit out of root iterator loop
            LOG_DEBUG("Done parsing Options.\n");
            break;
        }
    }
    if (!foundOptions) {
        LOG_WARNING("No options found in file " << filename << ".");
    }
}

void JsonParser::parseCamera(Json::Value &root){
    LOG_DEBUG("Parsing Camera.");
    bool foundCamera = false;    
        // Options
    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
        std::string key = itr.key().asString();
        if (key == "renderCam") {
            foundCamera = true;
            
                // read all standard data
            double r_fieldOfView = getDoubleAttr("fov", *itr);
            Transform r_transform = getTransformAttr("transforms", *itr);

                // TODO: check for any ones we've skipped and warn the user
            
                // TODO: handle errors gracefully
            
                // apply read data
            (*renderEnv->globals)[FieldOfView] = r_fieldOfView;
            cameraTransform = new Transform(r_transform);

            LOG_DEBUG("Done parsing Camera.\n");
            break;
        }
    }
    if (!foundCamera) {
        LOG_WARNING("No render camera found in file " << filename << ".");
    }
}

void JsonParser::parseMaterials(Json::Value &root){
    LOG_DEBUG("Parsing Materials.");
    bool foundMaterials = false;
    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
        std::string key = itr.key().asString();
        if (key == "materials") {
            foundMaterials = true;
            
                // loop through materials
            Json::Value objRoot = *itr;
            for( Json::ValueIterator objItr = objRoot.begin() ; objItr != objRoot.end() ; objItr++ ) {
                std::string matName = objItr.key().asString();
                LOG_DEBUG("Found material: " << matName);
                
                    // we first check if we're dealing with a light or mesh
                std::string matType = getStringAttr("type", *objItr);
                Material * mat = getMaterial(matType, matName, *objItr, renderEnv); 
            
                    // add to shading engine
                renderEnv->shadingEngine->registerMaterial(matName, mat);
            }       
            LOG_DEBUG("Done parsing materials.\n");
            break;
        }
    }
    if (!foundMaterials) {
        LOG_WARNING("No materials found in file " << filename << ".");
    }
}

inline Shader<Color>* getColorShader(const std::string &name, Json::Value &root){
    
    Shader<Color> *newShd;
    
    std::string shdType = getStringAttr("type", root);
    
    if (shdType == "texture2DShader") {
        std::string texturePath = getStringAttr("texturename", root);
        newShd = new Texture2DShader<Color>(texturePath);
    }
    else{
        LOG_ERROR("Unknown color shader type: " << shdType);
    }
    return newShd;
}

void JsonParser::parseShaders(Json::Value &root){
    LOG_DEBUG("Parsing Shaders.");
    bool foundShaders = false;
    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
        std::string key = itr.key().asString();
        if (key == "shaders") {
            foundShaders = true;
                // loop through shaders
            Json::Value objRoot = *itr;
            for( Json::ValueIterator objItr = objRoot.begin() ; objItr != objRoot.end() ; objItr++ ) {
                std::string shdName = objItr.key().asString();
                LOG_DEBUG("Found shader: " << shdName);

                    // find attribute type
                std::string attributeType = getStringAttr("attributetype", *objItr);
                
                if (attributeType == "color"){
                    Shader<Color> *newShd = getColorShader(shdName, *objItr);
                    renderEnv->shadingEngine->registerShaderColor(shdName, newShd);
                }
                else{
                    LOG_ERROR("Unable to parse shader for attribute type: " << attributeType);
                }
                

            }
            LOG_DEBUG("Done parsing shaders.\n");
            break;
        }
    }
    if (!foundShaders) {
        LOG_WARNING("No shaders found in file " << filename << ".");
    }

}

void JsonParser::parseGeometry(Json::Value &root){
    LOG_DEBUG("Parsing Geometry.");
    bool foundGeometry = false;
    for( Json::ValueIterator itr = root.begin() ; itr != root.end() ; itr++ ) {
        std::string key = itr.key().asString();
        if (key == "objects") {
            foundGeometry = true;
            
                // find object
            Json::Value objRoot = *itr;
            for( Json::ValueIterator objItr = objRoot.begin() ; objItr != objRoot.end() ; objItr++ ) {
                LOG_DEBUG("Found object: " << objItr.key().asString());
                
                    // check if we're dealing with a light or mesh
                std::string objType = getStringAttr("type", *objItr);
                if (objType == "geometry"){
                    std::tr1::shared_ptr<AuroraObject> geo = getGeometry(*cameraTransform, *objItr, renderEnv);
                    objects.push_back(geo);
                }
                else if (objType == "light"){
                    Light* light = getLight(*cameraTransform, *objItr, (*renderEnv->globals)[LightSamples], renderEnv);
                    lights.push_back(light);
                }
                else if (objType == "envlight"){
                    Light* light = getEnvLight(*cameraTransform, *objItr, (*renderEnv->globals)[LightSamples], renderEnv);
                    lights.push_back(light);
                }
                else {
                    LOG_WARNING("Unknown object type found: " << objType);
                }
            
            }
                // apply to globals
            LOG_DEBUG("Done parsing Geometry.\n");
            break;

        }
        if (!foundGeometry) {
            LOG_WARNING("No geometry found in file " << filename << ".");
        }
    }
}