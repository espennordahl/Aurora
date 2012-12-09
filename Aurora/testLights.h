//
//  testFrontEnd.h
//  Aurora
//
//  Created by Espen Nordahl on 26/11/12.
//
//

#ifndef Aurora_testLights_h
#define Aurora_testLights_h

#include "catch.hpp"


#include "core.h"
#include "lights.h"

using namespace Aurora;

namespace {
#pragma mark -
#pragma mark EnvLight
    TEST_CASE("lights/envLight", "Importance sampling an IBL.") {
//        RenderEnvironment renderEnv = RenderEnvironment();
//        Transform trans = Transform();
//        InfiniteAreaLight *envLight = new InfiniteAreaLight(&trans, &trans, &trans, &trans, &trans, &trans, 0, Color(1), "ENVMAP", 100, "testEnvlight", &renderEnv);
        
        REQUIRE(true == true);
    }
    
}

#endif
