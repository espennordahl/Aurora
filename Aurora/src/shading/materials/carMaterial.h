////
////  carMaterial.h
////  Aurora
////
////  Created by Espen Nordahl on 22/12/12.
////
////
//
//#ifndef Aurora_carMaterial_h
//#define Aurora_carMaterial_h
//
//#include "material.h"
//#include "shader.h"
//#include "texture2D.h"
//
//namespace Aurora {
//    
//        // forward decleration
//    class RenderEnvironment;
//    
//	class CarMaterial : public Material {
//	public:
//		CarMaterial(    std::string name, RenderEnvironment *renderEnv,
//                    // base diffuse
//                        int diffuseColIndex,
//                    // base spec bottom
//                        float baseSpecBottomReflectance,
//                        int baseSpecBottomRoughIndex,
//                    // base paint
//                        int basePaintColIndex,
//                        int basePaintThicknessIndex,
//                    // base spec top
//                        float baseSpecTopReflectance,
//                        int baseSpecTopRoughIndex,
//                    // top paint
//                        int topPaintColIndex,
//                        int topPaintThicknessIndex,
//                    // clearcoat
//                        float clearcoatReflectance,
//                        int clearcoatRoughnessIndex,
//
//                        int numSamples);
//
//		Brdf * getBrdf( const Vector &Vn, const Vector &Nn, const ShadingGeometry &shdGeo, bool mattePath, int thread ) ;
//		
//        void frameBegin();
//        void frameEnd();
//        
//	private:
//        float getBottomBaseAlbedo(float costheta, float roughness);
//        float getTopBaseAlbedo(float costheta, float roughness);
//        float getClearcoatAlbedo(float costheta, float roughness);
//        
//            // brdfs
//        Brdf * m_diffBrdf;
//        Brdf * m_bottomBaseSpecBrdf;
//        Brdf * m_topBaseSpecBrdf;
//        Brdf * m_clearcoatSpecBrdf;
//
//            // albedo caches
//        Texture2D *m_bottomBaseSpecAlbedoTexture;
//        Texture2D *m_topBaseSpecAlbedoTexture;
//        Texture2D *m_clearcostAlbedoTexture;
//
//            // base diffuse
//        int m_diffuseColIndex;
//            // base spec bottom
//        float m_baseSpecBottomReflectance;
//        int m_baseSpecBottomRoughIndex;
//            // base paint
//        int m_basePaintColIndex;
//        int m_basePaintThicknessIndex;
//            // base spec top
//        float m_baseSpecTopReflectance;
//        int m_baseSpecTopRoughIndex;
//            // top paint
//        int m_topPaintColIndex;
//        int m_topPaintThicknessIndex;
//            // clearcoat
//        float m_clearcoatReflectance;
//        int m_clearcoatRoughnessIndex;
//	};
//}
//
//
//#endif
