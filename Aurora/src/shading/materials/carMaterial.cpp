////
////  carMaterial.cpp
////  Aurora
////
////  Created by Espen Nordahl on 22/12/12.
////
////
//
//#include "carMaterial.h"
//#include "brdfs.h"
//#include "shadingEngine.h"
//
//#include <math.h>
//#include <sstream>
//#include <assert.h>
//
//using namespace Aurora;
//
//CarMaterial::CarMaterial(   std::string name,
//                            RenderEnvironment *renderEnv,
//                            int diffuseColIndex,
//                            float baseSpecBottomReflectance,
//                            int baseSpecBottomRoughIndex,
//                            int basePaintColIndex,
//                            int basePaintThicknessIndex,
//                            float baseSpecTopReflectance,
//                            int baseSpecTopRoughIndex,
//                            int topPaintColIndex,
//                            int topPaintThicknessIndex,
//                            float clearcoatReflectance,
//                            int clearcoatRoughnessIndex,
//                            int numSamples):
//Material(name, renderEnv),
//m_diffuseColIndex(diffuseColIndex),
//m_baseSpecBottomReflectance(baseSpecBottomReflectance),
//m_baseSpecBottomRoughIndex(baseSpecBottomRoughIndex),
//m_basePaintColIndex(basePaintColIndex),
//m_basePaintThicknessIndex(basePaintThicknessIndex),
//m_baseSpecTopReflectance(baseSpecTopReflectance),
//m_baseSpecTopRoughIndex(baseSpecTopRoughIndex),
//m_topPaintColIndex(topPaintColIndex),
//m_topPaintThicknessIndex(topPaintThicknessIndex),
//m_clearcoatReflectance(clearcoatReflectance),
//m_clearcoatRoughnessIndex(clearcoatRoughnessIndex)
//{
//        // lobes
//    m_diffBrdf = new Lambert( name + ":Lambert", Color(1), std::max(numSamples, 32), renderEnv);
//    m_bottomBaseSpecBrdf = new CookTorrance( name + ":BottomBaseCookTorrance", Color(1), 0.1, m_baseSpecBottomReflectance, std::max(numSamples, 32), renderEnv);
//    m_topBaseSpecBrdf = new CookTorrance( name + ":BottomBaseCookTorrance", Color(1), 0.1, m_baseSpecTopReflectance, std::max(numSamples, 32), renderEnv);
//    m_clearcoatSpecBrdf = new CookTorrance( name + ":BottomBaseCookTorrance", Color(1), 0.1, m_clearcoatReflectance, std::max(numSamples, 32), renderEnv);
//
//    
//        // Reflectance is non varying for now. TODO
//    
//        // Load albedo cache
//        // TODO: Lerp between for specAlbedo
//    std::stringstream ss;//create a stringstream
//    float modrefl = roundf(m_baseSpecBottomReflectance * (SPECALBEDO_REFLECTANCE_ENTRIES-1))/(SPECALBEDO_REFLECTANCE_ENTRIES-1.f);
//    ss << modrefl;//add number to the stream
//    std::string texName = "/Users/espennordahl/Documents/Aurora/cache/exrBeckmanAlbedoTable_" + ss.str() + "_v02.exr";
//    m_bottomBaseSpecAlbedoTexture = new Texture2D(texName);
//
//    std::stringstream ss2;//create a stringstream
//    modrefl = roundf(m_baseSpecTopReflectance * (SPECALBEDO_REFLECTANCE_ENTRIES-1))/(SPECALBEDO_REFLECTANCE_ENTRIES-1.f);
//    ss2 << modrefl;//add number to the stream
//    texName = "/Users/espennordahl/Documents/Aurora/cache/exrBeckmanAlbedoTable_" + ss2.str() + "_v02.exr";
//    m_topBaseSpecAlbedoTexture = new Texture2D(texName);
//
//    std::stringstream ss3;//create a stringstream
//    modrefl = roundf(m_clearcoatReflectance * (SPECALBEDO_REFLECTANCE_ENTRIES-1))/(SPECALBEDO_REFLECTANCE_ENTRIES-1.f);
//    ss3 << modrefl;//add number to the stream
//    texName = "/Users/espennordahl/Documents/Aurora/cache/exrBeckmanAlbedoTable_" + ss3.str() + "_v02.exr";
//    m_clearcostAlbedoTexture = new Texture2D(texName);
//
//}
//
//#pragma mark -
//#pragma mark pipeline
//
//void CarMaterial::frameBegin(){
//    m_diffBrdf->frameBegin();
//    m_bottomBaseSpecBrdf->frameBegin();
//    m_topBaseSpecBrdf->frameBegin();
//    m_clearcoatSpecBrdf->frameBegin();
//}
//
//void CarMaterial::frameEnd(){
//    m_diffBrdf->frameEnd();
//    m_bottomBaseSpecBrdf->frameEnd();
//    m_topBaseSpecBrdf->frameEnd();
//    m_clearcoatSpecBrdf->frameEnd();
//}
//
//
//#pragma mark albedos
//float CarMaterial::getBottomBaseAlbedo(float costheta, float roughness){
//    int i = clamp(floor((costheta * SPECALBEDO_COSTHETA_ENTRIES)), 0., SPECALBEDO_COSTHETA_ENTRIES-1);
//    int roughnessA = clamp(floor(roughness/SPECALBEDO_ROUGHNESSS_ENTRIES), 0, SPECALBEDO_ROUGHNESSS_ENTRIES-1);
//    int roughnessB = roughnessA+1;
//    
//        // first roughness lookup
//    float valueA = m_bottomBaseSpecAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float valueB = m_bottomBaseSpecAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    
//    float mix = costheta * SPECALBEDO_COSTHETA_ENTRIES - (float)i;
//    assert(valueA >= 0. && valueB >= 0.);
//    assert(mix >= 0. && mix <= 1.0);
//    float resultRoughnessA = (1.-mix) * valueA + mix * valueB;
//    
//        // second roughness lookup
//    valueA = m_bottomBaseSpecAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    valueB = m_bottomBaseSpecAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float resultRoughnessB = (1.-mix) * valueA + mix * valueB;
//    
//        // mix lookups
//    float roughnessMix = roughness - roughnessA;
//    return (1.-roughnessMix) * resultRoughnessA + roughnessMix * resultRoughnessB;
//}
//
//float CarMaterial::getTopBaseAlbedo(float costheta, float roughness){
//    int i = clamp(floor((costheta * SPECALBEDO_COSTHETA_ENTRIES)), 0., SPECALBEDO_COSTHETA_ENTRIES-1);
//    int roughnessA = clamp(floor(roughness/SPECALBEDO_ROUGHNESSS_ENTRIES), 0, SPECALBEDO_ROUGHNESSS_ENTRIES-1);
//    int roughnessB = roughnessA+1;
//    
//        // first roughness lookup
//    float valueA = m_topBaseSpecAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float valueB = m_topBaseSpecAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    
//    float mix = costheta * SPECALBEDO_COSTHETA_ENTRIES - (float)i;
//    assert(valueA >= 0. && valueB >= 0.);
//    assert(mix >= 0. && mix <= 1.0);
//    float resultRoughnessA = (1.-mix) * valueA + mix * valueB;
//    
//        // second roughness lookup
//    valueA = m_topBaseSpecAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    valueB = m_topBaseSpecAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float resultRoughnessB = (1.-mix) * valueA + mix * valueB;
//    
//        // mix lookups
//    float roughnessMix = roughness - roughnessA;
//    return (1.-roughnessMix) * resultRoughnessA + roughnessMix * resultRoughnessB;
//}
//
//float CarMaterial::getClearcoatAlbedo(float costheta, float roughness){
//    int i = clamp(floor((costheta * SPECALBEDO_COSTHETA_ENTRIES)), 0., SPECALBEDO_COSTHETA_ENTRIES-1);
//    int roughnessA = clamp(floor(roughness/SPECALBEDO_ROUGHNESSS_ENTRIES), 0, SPECALBEDO_ROUGHNESSS_ENTRIES-1);
//    int roughnessB = roughnessA+1;
//    
//        // first roughness lookup
//    float valueA = m_clearcostAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float valueB = m_clearcostAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessA/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    
//    float mix = costheta * SPECALBEDO_COSTHETA_ENTRIES - (float)i;
//    assert(valueA >= 0. && valueB >= 0.);
//    assert(mix >= 0. && mix <= 1.0);
//    float resultRoughnessA = (1.-mix) * valueA + mix * valueB;
//    
//        // second roughness lookup
//    valueA = m_clearcostAlbedoTexture->read((float)i/SPECALBEDO_COSTHETA_ENTRIES, (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    valueB = m_clearcostAlbedoTexture->read(std::min(float(i)/SPECALBEDO_COSTHETA_ENTRIES, 1.f), (float)roughnessB/SPECALBEDO_ROUGHNESSS_ENTRIES, 0.).r;
//    float resultRoughnessB = (1.-mix) * valueA + mix * valueB;
//    
//        // mix lookups
//    float roughnessMix = roughness - roughnessA;
//    return (1.-roughnessMix) * resultRoughnessA + roughnessMix * resultRoughnessB;
//}
//
//#pragma mark -
//#pragma mark importance sampling
//
//Brdf * CarMaterial::getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) {
//    
//        // Clearcoat layer
//    
//    float roughness = renderEnv->shadingEngine->getFloat(m_clearcoatRoughnessIndex, shdGeo);
//    float clearcoatAlbedo = getClearcoatAlbedo(clamp(dot(Nn, Vn), 0., 1.), roughness); // TODO: should't need to clamp here
//	float r = (float) rand()/RAND_MAX;
//	if (r < clearcoatAlbedo) {
//        cookTorranceParameters params = {
//            Color(1.),
//            roughness,
//            m_clearcoatReflectance
//        };
//        m_clearcoatSpecBrdf->setParameters(&params, thread);
//        m_clearcoatSpecBrdf->initRoughness(mattePath, thread);
//		return m_clearcoatSpecBrdf;
//    }
//    
//        // Top paint layer
//    
//    Color paintCol = renderEnv->shadingEngine->getColor(m_topPaintColIndex, shdGeo);
//        //Color paintCol = Color(1.);
//        // Top base spec layer
//    
//    roughness = renderEnv->shadingEngine->getFloat(m_baseSpecTopRoughIndex, shdGeo);
//    float topSpecAlbedo = getTopBaseAlbedo(clamp(dot(Nn, Vn), 0., 1.), roughness); // TODO: should't need to clamp here
//    r = (float) rand()/RAND_MAX;
//    if (r < topSpecAlbedo) {
//        cookTorranceParameters params = {
//            paintCol,
//            roughness,
//            m_baseSpecTopReflectance
//        };
//        m_topBaseSpecBrdf->setParameters(&params, thread);
//        m_topBaseSpecBrdf->initRoughness(mattePath, thread);
//        return m_topBaseSpecBrdf;
//    }
//
//        // Bottom paint layer
//    
//    paintCol *= renderEnv->shadingEngine->getColor(m_basePaintColIndex, shdGeo);
//    
//        // Bottom base spec layer
//    
//    roughness = renderEnv->shadingEngine->getFloat(m_baseSpecBottomRoughIndex, shdGeo);
//    float bottomSpecAlbedo = getBottomBaseAlbedo(clamp(dot(Nn, Vn), 0., 1.), roughness); // TODO: should't need to clamp here
//    r = (float) rand()/RAND_MAX;
//    if (r < bottomSpecAlbedo) {
//        cookTorranceParameters params = {
//            paintCol,
//            roughness,
//            m_baseSpecBottomReflectance
//        };
//        m_bottomBaseSpecBrdf->setParameters(&params, thread);
//        m_bottomBaseSpecBrdf->initRoughness(mattePath, thread);
//        return m_bottomBaseSpecBrdf;
//    }
//    
//        // Base diffuse layer
//    Color diffCol = renderEnv->shadingEngine->getColor(m_diffuseColIndex, shdGeo);
//    lambertParameters params = {
//        paintCol * diffCol
//    };
//    m_diffBrdf->setParameters(&params, thread);
//    return m_diffBrdf;
//}
