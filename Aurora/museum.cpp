//
//  main.cpp
//  Aurora
//
//  Created by Espen Nordahl on 30/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "renderer.h"
#include "adaptiveRndSampler2D.h"
#include "matteMaterial.h"
#include "specMaterial.h"
#include "plasticMaterial.h"
#include "squareLight.h"
#include "shapes.h"

using namespace Aurora;

int main (int argc, const char * argv[])
{
	
	// insert code here...
	std::cout << "Hello, World!\n";
    
	
	// CAMERA
	AdaptiveRndSampler2D *cameraSampler = new AdaptiveRndSampler2D();
	Camera cam = Camera(M_PI/5.f, 1024, 1024, 32, cameraSampler);
	
	
	// Inverse camera transform
	Transform camTrans = Transform();
	camTrans.rotateY(45);
	camTrans.rotateY(45);
	camTrans.translate(Vector(-200,90,-130));
	Transform camTransInv = camTrans.inverse(camTrans);
	
	
	// OBJ object one
	Transform objTrans1;
	Transform objTransInv1 = objTrans1.inverse(objTrans1);
	
	Reference<Shape> shape1 = new ObjTriangleMesh(&camTransInv, &camTrans, "/Users/nordahl/Downloads/dino1.obj");
	
	Reference<Material> cubeBSurf1 = new MatteMaterial(Color(0.8, 0.1, 0.1f));	
	Reference<AuroraObject> museum1Prim = new AuroraObject(shape1, cubeBSurf1);
	
	// OBJ object one
	Transform objTrans3;
	Transform objTransInv3 = objTrans3.inverse(objTrans3);
	
	Reference<Shape> shape3 = new ObjTriangleMesh(&camTransInv, &camTrans, "/Users/nordahl/Downloads/dino2.obj");
	
	Reference<Material> cubeBSurf3 = new MatteMaterial(Color(0.1, 0.1, 0.8f));	
	Reference<AuroraObject> museum3Prim = new AuroraObject(shape3, cubeBSurf3);
	
	
	// OBJ object one
	Transform objTrans2;
	Transform objTransInv2 = objTrans2.inverse(objTrans2);
	
	Reference<Shape> shape2 = new ObjTriangleMesh(&camTransInv, &camTrans, "/Users/nordahl/Downloads/museum.obj");
	
	Reference<Material> cubeBSurf2 = new MatteMaterial(Color(0.5, 0.5, 0.5f));	
	Reference<AuroraObject> museumPrim = new AuroraObject(shape2, cubeBSurf2);
	
	
	
	// prims
	std::vector<Reference<AuroraObject> > primitives;
	primitives.push_back(museumPrim);
	//	primitives.push_back(museum1Prim);
	//	primitives.push_back(museum3Prim);
	
	
	
	// lights
	Transform lightTrans = camTransInv;
	lightTrans.translate(Vector(200, 175, -150));
	Transform lightTransInv = lightTrans.inverse(lightTrans);
	Reference<Light> aLight = new SquareLight(&lightTrans, &lightTransInv, 4.5f, Color(1.f,0.9,0.5), 200, 30);
	std::vector<Reference<Light> > lights;
	lights.push_back(aLight);
	
	
	Reference<Light> envLight = new InfiniteAreaLight(&lightTrans, &lightTransInv, 2.f, Color(.3,0.5,1.f));	
	
	lights.push_back(envLight);
	
	
	Renderer ren = Renderer();
	ren.renderCam = cam;
	ren.objects = primitives;
	ren.lights = lights;
	
	ren.render();
	// 452 minutes 31 sec
	return 0;
}