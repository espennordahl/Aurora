//
//  camera.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_camera_h
#define Aurora_camera_h

#include <tbb/atomic.h>

#include "halton_sampler.h"
#include "halton_enum.h"
#include "core.h"

namespace Aurora {
	class Camera {
    public:
        Camera();
        Camera(float fov, int width, int height, int mpixels);
        
        Sample3D convertSample( const Sample2D &sample2d, int i );
    
        int getWidthSamples() { return m_widthSamples; }
        int getHeightSamples() { return m_heightSamples; }
        int getPixelSamples(){ return m_pixelSamples; }
        
    
    protected:
        int m_widthSamples;
        int m_heightSamples;
        int m_pixelSamples;
        float m_fov;

        Halton_enum m_halton_enum;
        Halton_sampler m_sampler;
        static tbb::atomic<int> m_halton_index;
	};
}
	

#endif