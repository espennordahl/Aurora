//
//  trCamera.cpp
//  Turtle
//
//	Abstract
//
//	Defines a render camera, with associated lense settings.
//	Generates rays and passes the collected radiosity
//	values to the display driver.



#include "Camera.h"

#include <iostream>
#include <assert.h>
#include "math_utils.h"

using namespace Aurora;

Camera::Camera():
m_halton_enum(Halton_enum(1, 1))
{
}


Camera::Camera(float _fov, int width, int height, int _mpixels, Sampler2D *_sampler):
fov(_fov), widthSamples(width), heightSamples(height), pixelSamples(_mpixels), sampler(_sampler),
m_halton_enum(Halton_enum(width, height))
{
    m_sampler.init_faure();
}

//trCamera::~trCamera(){
//	free(sampler);
//}

//void trCamera::setSampler( trSampler2D *s ){
//	sampler = s;
//}

void Camera::generateSamples(){
	sampler->generateSamples(widthSamples, heightSamples, pixelSamples);	
}

bool Camera::nextSample( Sample2D *sample ){
	// we generate 2d samples, and turn them into
	// 3d samples which we pass to the renderer.
	
	if (sampler->nextSample(sample )) {
		return true;
	}
	return false;
}

Sample3D Camera::convertSample( const Sample2D &sample2d, int i){
	Sample3D sample3d;
	float fovx = fov;
	float fovy = ((float)heightSamples/(float)widthSamples)*fovx;

#ifdef USE_HALTON
    const unsigned index = m_halton_enum.get_index(i, sample2d.x, sample2d.y);
    float x = m_sampler.sample(CAMERA_DOMAIN_X, index);
    float y = m_sampler.sample(CAMERA_DOMAIN_Y, index);
    float u = m_halton_enum.scale_x(x);
    float v = m_halton_enum.scale_y(y);

#else
	float u = sample2d.x + (float) rand()/RAND_MAX;//widthSamples;
	float v = sample2d.y + (float) rand()/RAND_MAX;//heightSamples;
#endif

	sample3d.ray.direction.x = ((2*u - widthSamples)/(float)widthSamples) * fovx;
	sample3d.ray.direction.y = ((-2*v + heightSamples)/(float)heightSamples) * fovy;
	sample3d.ray.direction.z = -1.f;

	sample3d.ray.direction = normalize(sample3d.ray.direction);
	
	sample3d.ray.origin = Point(0,0,0);
	sample3d.ray.mint = 0.1f;
	sample3d.ray.maxt = 10000000.f;
	
	return sample3d;
}