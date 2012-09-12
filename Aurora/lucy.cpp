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
	Camera cam = Camera(M_PI/7.f, 1024, 778, 32, cameraSampler);
	
	
	// Inverse camera transform
	Transform camTrans = Transform();
	camTrans.translate(Vector( 278.0, 273.0, -800.0 ));
	Transform camTransInv = camTrans.inverse(camTrans);
	
	// FLOOR
	float floorX = 500;
	float floorZ = 400;
	Point PFloor[4] = {
		Point( 552.8+floorX, 0.0,   0.0-floorZ),   
		Point(   0.0-floorX, 0.0,   0.0-floorZ),
		Point(   0.0-floorX, 0.0, 559.2),
		Point( 549.6+floorX, 0.0, 559.2)};
	
	int floorIndex[6] = {0, 1, 2,
		0, 2, 3};
	
	Reference<Shape> floorShape = new TriangleMesh(&camTransInv, &camTrans, 2, 4, floorIndex, PFloor);
	Reference<Material> floorSurf = new MatteMaterial(Color(0.8f));		
	//	Reference<Material> floorSurf = new SpecMaterial(Color(0.8f), 10.f);		
	Reference<AuroraObject> floorPrim = new AuroraObject(floorShape, floorSurf);
	
	// CEILING
	Point PCeiling[4]= {Point( 556.0, 548.8,   0.0),   
		Point( 556.0, 548.8, 559.2),
		Point(   0.0, 548.8, 559.2),
		Point(   0.0, 548.8,   0.0)};
	
	int ceilingIndex[6] = {0, 1, 2,
		0, 2, 3};
	
	Reference<Shape> ceilingShape = new TriangleMesh(&camTransInv, &camTrans, 2, 4, ceilingIndex, PCeiling);
	Reference<Material> ceilingSurf = new MatteMaterial(Color(0.75f));		
	Reference<AuroraObject> ceilingPrim = new AuroraObject(ceilingShape, ceilingSurf);
	
	// BACK WALL
	float floorY = 250.f;
	Point PBackWall[4] = {
		Point( 549.6+floorX,   0.0, 559.2),   
		Point(   0.0-floorX,   0.0, 559.2),
		Point(   0.0-floorX, 548.8+floorY, 559.2),
		Point( 556.0+floorX, 548.8+floorY, 559.2)};
	
	int backWallIndex[6] = {0, 1, 2,
		0, 2, 3};
	
	Reference<Shape> backWallShape = new TriangleMesh(&camTransInv, &camTrans, 2, 4, backWallIndex, PBackWall);
	Reference<Material> backWallSurf = new MatteMaterial(Color(0.75f));		
	Reference<AuroraObject> backWallPrim = new AuroraObject(backWallShape, backWallSurf);
	
	// LEFT WALL
	Point PLeftWall[4] = {
		Point( 552.8,   0.0,   0.0),   
		Point( 549.6,   0.0, 559.2),
		Point( 556.0, 548.8, 559.2),
		Point( 556.0, 548.8,   0.0)};
	
	int leftWallIndex[6] = {0, 1, 2,
		0, 2, 3};
	
	Reference<Shape> leftWallShape = new TriangleMesh(&camTransInv, &camTrans, 2, 4, leftWallIndex, PLeftWall);
	Reference<Material> leftWallSurf = new MatteMaterial(Color(0.55, 0.015, 0.002));		
	Reference<AuroraObject> leftWallPrim = new AuroraObject(leftWallShape, leftWallSurf);
	
	// RIGHT WALL
	Point PRightWall[4] = {Point(  0.0,   0.0,   0.0),   
		Point(   0.0,   0.0, 559.2),
		Point(   0.0, 548.8, 559.2),
		Point(   0.0, 548.8,   0.0)};
	
	int rightWallIndex[6] = {0, 1, 2,
		0, 2, 3};
	
	Reference<Shape> rightWallShape = new TriangleMesh(&camTransInv, &camTrans, 2, 4, rightWallIndex, PRightWall);
	Reference<Material> rightWallSurf = new MatteMaterial(Color(0.025, 0.25, 0.05));		
	Reference<AuroraObject> rightWallPrim = new AuroraObject(rightWallShape, rightWallSurf);
	
	// CUBE A
	Point PCubeA[20] = {
		Point(423.0, 330.0, 247.0),
		Point(265.0, 330.0, 296.0),
		Point(314.0, 330.0, 456.0),
		Point(472.0, 330.0, 406.0),
		
		Point(423.0,   0.0, 247.0),
		Point(423.0, 330.0, 247.0),
		Point(472.0, 330.0, 406.0),
		Point(472.0,   0.0, 406.0),
		
		Point(472.0,   0.0, 406.0),
		Point(472.0, 330.0, 406.0),
		Point(314.0, 330.0, 456.0),
		Point(314.0,   0.0, 456.0),
		
		Point(314.0,   0.0, 456.0),
		Point(314.0, 330.0, 456.0),
		Point(265.0, 330.0, 296.0),
		Point(265.0,   0.0, 296.0),
		
		Point(265.0,   0.0, 296.0),
		Point(265.0, 330.0, 296.0),
		Point(423.0, 330.0, 247.0),
		Point(423.0,   0.0, 247.0)};
	
	int cubeAIndex[30] = {
		0, 1, 2,
		0, 2, 3,
		
		4, 5, 6,
		4, 6, 7,
		
		8, 9, 10,
		8, 10, 11,
		
		12, 13, 14,
		12, 14, 15,
		
		16, 17, 18,
		16, 18, 19
	};
	
	Reference<Shape> cubeAShape = new TriangleMesh(&camTransInv, &camTrans, 10, 20, cubeAIndex, PCubeA);
	Reference<Material> cubeASurf = new PlasticMaterial(Color(0.8, 0.15, 0.15), 35.f, 0.05f, 1.0);	
	Reference<AuroraObject> cubeAPrim = new AuroraObject(cubeAShape, cubeASurf);
	
	
	// CUBE B
	//	Point PCubeB[20] = {
	//		Point(130.0, 165.0,  65.0),
	//		Point(82.0, 165.0, 225.0),
	//		Point(240.0, 165.0, 272.0),
	//		Point(290.0, 165.0, 114.0),
	//		
	//		Point(290.0,   0.0, 114.0),
	//		Point(290.0, 165.0, 114.0),
	//		Point(240.0, 165.0, 272.0),
	//		Point(240.0,   0.0, 272.0),
	//		
	//		Point(130.0,   0.0,  65.0),
	//		Point(130.0, 165.0,  65.0),
	//		Point(290.0, 165.0, 114.0),
	//		Point(290.0,   0.0, 114.0),
	//		
	//		Point(82.0,   0.0, 225.0),
	//		Point(82.0, 165.0, 225.0),
	//		Point(130.0, 165.0,  65.0),
	//		Point(130.0,  0.0,  65.0),
	//		
	//		Point(240.0,   0.0, 272.0),
	//		Point(240.0, 165.0, 272.0),
	//		Point(82.0, 165.0, 225.0),
	//		Point(82.0,   0.0, 225.0)
	//	};
	//	int cubeBIndex[30] = {
	//		0, 1, 2,
	//		0, 2, 3,
	//		
	//		4, 5, 6,
	//		4, 6, 7,
	//		
	//		8, 9, 10,
	//		8, 10, 11,
	//		
	//		12, 13, 14,
	//		12, 14, 15,
	//		
	//		16, 17, 18,
	//		16, 18, 19
	//	};
	//	
	//	Reference<Shape> cubeBShape = new TriangleMesh(&camTransInv, &camTrans, 10, 20, cubeBIndex, PCubeB);
	//	Reference<Material> cubeBSurf = new MatteMaterial(Color(0.8f));		
	//	Reference<AuroraObject> cubeBPrim = new AuroraObject(cubeBShape, cubeBSurf);
	//	
	
	// OBJ object one
	Transform objTrans;
	float scale = 500;
	objTrans.rotateY(180);
	//	objTrans.translate(Vector(300, 0, 25));
	objTrans.translate(Vector(0, scale/2, 0));
	objTrans.scale(scale, scale, scale);
	objTrans.translate(Vector(-430.f/scale, 0.f, -230.f/scale));
	objTrans.translate(Vector(278.f/scale, -273.f/scale, -800.f/scale ));
	Transform objTransInv = objTrans.inverse(objTrans);
	
	Reference<Shape> shape = new ObjTriangleMesh(&objTrans, &objTransInv, "/Users/nordahl/Downloads/lucy.obj");
	
	Reference<Material> cubeBSurf = new PlasticMaterial(Color(0.6, 0.1, 0.1), 35.f, 0.1f, 0.4);	
	Reference<AuroraObject> cubeBPrim = new AuroraObject(shape, cubeBSurf);
	
	
	// OBJ object one
	Transform objTrans2;
	float scale2 = 500;
	objTrans2.rotateY(180);
	//	objTrans.translate(Vector(300, 0, 25));
	objTrans2.translate(Vector(0, scale2/2, 0));
	objTrans2.scale(scale2, scale2, scale2);
	objTrans2.translate(Vector(-70.f/scale2, 0.f, -230.f/scale2));
	objTrans2.translate(Vector(278.f/scale2, -273.f/scale2, -800.f/scale2 ));
	Transform objTransInv2 = objTrans2.inverse(objTrans2);
	
	Reference<Shape> shape2 = new ObjTriangleMesh(&objTrans2, &objTransInv2, "/Users/nordahl/Downloads/lucy.obj");
	
	Reference<Material> cubeBSurf2 = new MatteMaterial(Color(0.2, 0.3, 0.8f));	
	Reference<AuroraObject> cubeBPrim2 = new AuroraObject(shape2, cubeBSurf2);
	
	
	
	// prims
	std::vector<Reference<AuroraObject> > primitives;
	primitives.push_back(backWallPrim);
	//	primitives.push_back(ceilingPrim);
	primitives.push_back(floorPrim);
	//	primitives.push_back(rightWallPrim);
	//	primitives.push_back(leftWallPrim);
	//	primitives.push_back(cubeAPrim);
	primitives.push_back(cubeBPrim);
	primitives.push_back(cubeBPrim2);
	
	
	// lights
	Transform lightTrans;
	lightTrans.translate(Vector(500, 1500, 150));
	lightTrans *= camTransInv;
	Transform lightTransInv = lightTrans.inverse(lightTrans);
	Reference<Light> aLight = new SquareLight(&lightTrans, &lightTransInv, 9.5f, Color(1.f), 100, 100);
	std::vector<Reference<Light> > lights;
	lights.push_back(aLight);
	
	
	Reference<Light> envLight = new InfiniteAreaLight(&lightTrans, &lightTransInv, -2.f, Color(0.25, 0.3, 0.5));	
	
	//	lights.push_back(envLight);
	
	
	Renderer ren = Renderer();
	ren.renderCam = cam;
	ren.objects = primitives;
	ren.lights = lights;
	
	ren.render();
	// 452 minutes 31 sec
	return 0;
}