//
//  constants.h
//  Aurora
//
//  Created by Espen Nordahl on 07/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Aurora_constants_h
#define Aurora_constants_h

// colors
#define ISBLACK  0.0001

// exr
#define DRAW_DELAY 10

// ray tracing
#define RAY_BIAS 0.0001

#define MIN_ROULETTE 0.85
#define MAX_ROULETTE 0.9

// threading
#define NUM_THREADS 8
#define SAMPLES_PER_THREAD 10000

// kd tree
#define KD_INTERSECTCOST 1
#define KD_TRAVERSECOST 1
#define KD_EMPTYBONUS 1

#endif