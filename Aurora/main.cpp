//
//  main.cpp
//  Aurora
//
//  Created by Espen Nordahl on 30/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string.h>

#include <tbb/tbb.h>

#include "renderer.h"

#include "log.h"
#define lcontext LOG_Main

#include "catch.hpp"

using namespace Aurora;

int main (int argc, char* const argv[])
{
//    tbb::task_scheduler_init init(1);
    tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
    LOG_INFO("Starting Aurora.");

        // unit tests
        //    #ifndef NDEBUG
        Renderer ren = Renderer((char *)"/Users/espennordahl/Documents/Aurora/pyAurora/cubes_v01.asc");
        ren.render();
    return 0;
        // #endif

    LOG_INFO("Checking command line arguments");
    
    if ( argc != 2 ) // argc should be 2 for correct execution
                     // We print argv[0] assuming it is the program name
        LOG_ERROR("usage: "<< argv[0] <<" <filename>.");
    else {
            // We assume argv[1] is a filename to open
        ifstream the_file ( argv[1] );
            // check to see if file opening succeeded
        if ( !the_file.is_open() )
            LOG_ERROR("Could not open file " << argv[1] << ".");
        else {
                // Pass file to renderer
            Renderer ren = Renderer((char *)argv[1]);
            ren.render();
        }
    }
            // the_file is closed implicitly here
	return 0;
}