//
//  testCore.h
//  Aurora
//
//  Created by Espen Nordahl on 24/11/12.
//
//

#ifndef Aurora_testCore_h
#define Aurora_testCore_h

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "core.h"

using namespace Aurora;

namespace {
#pragma mark -
#pragma mark Catch
    TEST_CASE("core/catch", "Making sure the testing framework works.") {
        REQUIRE(true == true);
    }
    
    
#pragma mark -
#pragma mark Point
    TEST_CASE("core/points/accessors", "3f Points behave like expected.") {
        Point p = Point(1,2,3);
        
            // accessors
        REQUIRE(p.x == 1);
        REQUIRE(p.y == 2);
        REQUIRE(p.z == 3);
        REQUIRE(p.hasNaNs() == false);
        
            // arithmatic
        REQUIRE((p + Point(1,1,1)) == Vector(2,3,4));
        REQUIRE((p - Point(1,1,1)) == Vector(0,1,2));
    }
    
#pragma mark -
#pragma mark stringUtils
    TEST_CASE("core/stringUtils/stringTemplate",
              "StringTemplates should expand variables according to config.json"){
        CHECK(stringTemplate("test") == "test");
        CHECK(stringTemplate("$BASE/test") == "/Users/espennordahl/Documents/Aurora/test");
        CHECK(stringTemplate("$TEXTURE/test.exr") == "/Users/espennordahl/Documents/Aurora/textures/test.exr");
        CHECK(stringTemplate("$CACHE/test.csh") == "/Users/espennordahl/Documents/Aurora/cache/test.csh");
        CHECK(stringTemplate("$RENDER/unitTest_####.exr") == "/Users/espennordahl/Documents/Aurora/data/render/unitTest_0002.exr");
        REQUIRE_FALSE("abc" == "def");
        REQUIRE_FALSE("123" == "456");
    }
}
#endif