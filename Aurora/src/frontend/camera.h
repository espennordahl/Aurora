//
//  camera.h
//  Aurora
//
//  Created by Espen Nordahl on 01/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_camera_h
#define Aurora_camera_h

#include "core.h"

namespace Aurora {
	class Camera {
		public:
			Camera();
			Camera(float _fov, int _width, int _height, int _mpixels, Sampler2D *Sampler);
			
			void generateSamples();

			bool nextSample( Sample2D *sample );

			Sample3D convertSample( const Sample2D &sample2d );
		
			int getWidthSamples() { return widthSamples; }
			int getHeightSamples() { return heightSamples; }
			int getPixelSamples(){ return pixelSamples; }
			
		
		protected:
			int widthSamples;
			int heightSamples;
			int pixelSamples;
			float fov;

			Sampler2D *sampler;
	};
}
	

#endif