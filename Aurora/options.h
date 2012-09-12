//
//  options.h
//  Aurora
//
//  Created by Espen Nordahl on 1/08/12.
//
//  Abstract:
//  Options track the global attribute state.

#ifndef Aurora_options_h
#define Aurora_options_h

#include <map>
#include <math.h>


enum Option {
        // rendered image
    ResolutionX,
    ResolutionY,
    PixelSamples,
        // camera
    FieldOfView,

        // ray tracing
    MinDepth,
    MaxDepth,
    LightSamples,
    AccelStructure,
        // kd tree
    KD_MaxLeaf,
    KD_MaxDepth
};

#define ACCEL_KDTREE 0
#define ACCEL_EMBREE 1

inline void setDefaultOptions( std::map<Option, double> *globals ){
    globals->insert(std::make_pair(ResolutionX,     512));
    globals->insert(std::make_pair(ResolutionY,     512));
    globals->insert(std::make_pair(FieldOfView,     M_PI/4));
    globals->insert(std::make_pair(PixelSamples,    64));
    globals->insert(std::make_pair(MinDepth,        1));
    globals->insert(std::make_pair(MaxDepth,        16));
    globals->insert(std::make_pair(LightSamples,    64));
    globals->insert(std::make_pair(KD_MaxLeaf,      8));
    globals->insert(std::make_pair(KD_MaxDepth,     25));
    globals->insert(std::make_pair(AccelStructure, ACCEL_KDTREE));
}
 

#endif
