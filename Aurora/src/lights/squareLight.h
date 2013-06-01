//
//  squareLight.h
//  Aurora
//
//  Created by Espen Nordahl on 05/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_squareLight_h
#define Aurora_squareLight_h

#include <tbb/atomic.h>

#include "halton_sampler.h"
#include "core.h"
#include "light.h"
#include "attributeState.h"

namespace Aurora {
	class SquareLight : public Light {
	public:
		SquareLight( Transform *o2c, Transform *c2o, Transform *o2w, Transform *w2o, Transform *c2w, Transform *w2c, float exposure, Color color, float sizeX, float sizeY, std::string name, RenderEnvironment *renderEnv);
		
		inline Sample3D generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain);
		
		inline Color eval( const Sample3D &sample, const Vector &Nn ) ;
		
        inline float pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain);
		
		bool intersectBinary( Ray *ray ) const;
		
		void makeRenderable(std::vector<RenderableTriangle> &renderable, AttributeState *attrs, int index);

        Color emission();
        
        std::tr1::shared_ptr<Shape> shape();
        
        void frameBegin(){};
        void frameEnd(){};
        
	private:
		float xScale;
		float yScale;
        Vector lightN;
        Halton_sampler m_sampler;
        
        static tbb::atomic<int> m_halton_index;

	};
    
    
    Sample3D SquareLight::generateSample( const Point &orig, const Vector &Nn, const IntegrationDomain &integrationDomain){
#ifdef USE_HALTON
        const int i = m_halton_index.fetch_and_increment();
        float x = m_sampler.sample(LIGHT_DOMAIN_X, i);
        float y = m_sampler.sample(LIGHT_DOMAIN_Y, i);
#else
        float x = (float) rand()/RAND_MAX;
        float y = (float) rand()/RAND_MAX;
#endif
        Point lightP = Point((x-0.5) * xScale * 2, (y-0.5) * yScale * 2, 0.f);
        lightP = (*objectToCamera)(lightP);
        Vector dir = lightP - orig;
        float sampleLength = dir.length();
        dir = normalize(dir);
        Sample3D s = Sample3D(Ray(dir, orig, RAY_BIAS, sampleLength - (RAY_BIAS*10)));
        float d = dot(lightN, -dir);
        d *= 1-signbit(d);
        s.color = eval(s, Nn);
        if (d == 0.) {
            s.pdf = 0.;
        }
        else{
            s.pdf = (sampleLength*sampleLength) / (d * xScale * yScale * 4.);
        }
        return s;
    }
    
    Color SquareLight::eval( const Sample3D &sample, const Vector &Nn ) {
            //	Vector Ln = (*objectToCamera)(Vector(0,0,1));
            //    float d = dot(normalize(sample.ray.direction), Ln);
            //	if (d < 0.f)
            //		d = 0;//-dot;
        Color col =  color * powf(2,exposure);
        return col;
    }
    
    float SquareLight::pdf( Sample3D *sample, const Vector &Nn, const IntegrationDomain &integrationDomain){
        if (intersectBinary(&sample->ray)) {
            float d = dot(lightN, normalize(sample->ray.direction));
            d *= sign(d);
            float pdf = (sample->ray.maxt * sample->ray.maxt) / (d * xScale * yScale * 4.);
            sample->ray.maxt -= RAY_BIAS*10;
            return pdf;
        }
        return 0.f;
    }

    
}




#endif
