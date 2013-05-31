////
////  cookTorrance.h
////  Aurora
////
////  Created by Espen Nordahl on 23/07/12.
////  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
////
//
//#ifndef Aurora_cookTorrance_h
//#define Aurora_cookTorrance_h
//
//#include "core.h"
//#include "brdf.h"
//#include "texture2D.h"
//#include "openexrDisplay.h"
//
//namespace Aurora {
//    
//    struct cookTorranceParameters{
//        Color specColor;
//        float roughness;
//        float reflectance;
//    };
//
//    
//	class CookTorrance : public Brdf{
//	public:
//		CookTorrance(std::string name, Color col, float roughness, float reflectance, int numSamples, RenderEnvironment *renderEnv);
//		
//		Sample3D getSample(const Vector &Vn, const Vector &Nn, int depth, int thread);
//		Color evalSampleTangent(const Vector &Ln, const Vector &Vn, int thread);
//		Color evalSampleWorld(const Vector &Ln, const Vector &Vn, const Vector &Nn, int thread);
//		float pdf(const Vector &Ln, const Vector &Vn, const Vector Nn, int thread) const;
//        void setParameters(void *params, int thread);
//        void initRoughness(bool mattePath, int thread);
//        
//        void frameBegin();
//        void frameEnd();
//
//        
//	private:
//        void generateSampleBuffer(int i, int t);
//        void preCalcNormTable();
//        float getNormWeight(float costheta, float roughness);
//        std::vector<float> randomU[NUM_THREADS][3];
//        std::vector<float> randomV[NUM_THREADS][3];
//        Texture2D *normTable;
//		Color color[NUM_THREADS];
//		float roughness[NUM_THREADS];
//        float baseRoughness[NUM_THREADS];
//        float reflectance[NUM_THREADS];
//        int numSamples;
//	};
//}
//
//#endif
