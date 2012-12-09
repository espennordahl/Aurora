//
//  testBackEnd.h
//  Aurora
//
//  Created by Espen Nordahl on 26/11/12.
//
//

#ifndef Aurora_testBackEnd_h
#define Aurora_testBackEnd_h

#include "catch.hpp"

#include "core.h"
#include "texture2D.h"
#include "openexrDisplay.h"

using namespace Aurora;

#pragma mark -
#pragma mark constants

#define TEXTURE_SAMPLES 1000000

namespace {
    
        // Compares two images and returns true for a match and false if the
        // average error is higher than the accepted error
    bool textureEquals(std::string path1, std::string path2, double acceptedError){
        Texture2D *tex1 = new Texture2D(path1);
        Texture2D *tex2 = new Texture2D(path2);
        
            // check that size matches
        int width = tex1->getWidth();
        int height = tex1->getHeight();
        
        if (width != tex2->getWidth() || height != tex2->getHeight()) {
            std::cout << "ERROR: Images has different format.";
            return false;
        }

        double avgError = 0;
        for (int x=0; x < width; x++) {
            for (int y=0; y < height; y++) {
                Color a = tex1->read((float)x/width, (float)y/height, 0.f);
                Color b = tex2->read((float)x/width, (float)y/height, 0.f);
                Color diffC = a - b;
                double error = abs(diffC.r) + abs(diffC.g) + abs(diffC.b);
                avgError += error/(height*width);
            }
        }
        if (avgError > acceptedError){
            std::cout << "ERROR: Images are too different: " << avgError;
            return false;
        }

        return true;
    }

#pragma mark -
#pragma mark EnvLight
    TEST_CASE("backEnd/Texture2D", "Importance sampling a texture.") {
            // create a texture, then sample it multiple times and compare
            // sampling frequency to the original image.
        Texture2D *texture = new Texture2D("/Users/espennordahl/Documents/Aurora/hdri/garage.exr");
        texture->initPdf(); //TODO: This shouldn't need to be a client step.
        
        static int WIDTH = 512;
        static int HEIGHT = 256;
        static float meanImgBrightness = 0.14502;
        OpenexrDisplay fileOut(WIDTH, HEIGHT, "/Users/espennordahl/Documents/Aurora/tests/sampling.exr");

        for (int i=0; i < TEXTURE_SAMPLES; i++) {
            float r = (float)rand()/RAND_MAX;
            float u, v;
            texture->sample(r, &u, &v);
            int x = floor(u*WIDTH);
            int y = floor(v*HEIGHT);
            Color value; float tmp;
            fileOut.getPixel(x, y, &value, &tmp);
            value += (HEIGHT*WIDTH*meanImgBrightness)/(float)TEXTURE_SAMPLES;
            fileOut.setPixel(x, y, value, 1.f);
        }
        
        fileOut.draw(HEIGHT);
        
        delete texture;
        REQUIRE(textureEquals("/Users/espennordahl/Documents/Aurora/tests/sampling.exr",
                              "/Users/espennordahl/Documents/Aurora/tests/sampling_true.exr",
                              1.0) == true);
    }
    
}

#endif
