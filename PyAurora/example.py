import core
import geometry
import lights
import math

scene = core.Scene()

# Settings
options = {	"resolution"	: [512, 512],
			"driver"	 	: "openexr",
			"filename"		: "dragon.exr",
			"pixelsamples"	: 256,
			"pixelsampler"	: "adaptive",
			"accelerator"	: "uniformgrid",
			"minraydepth"	: 3,
			"maxraydepth"	: 20,
			"raybias"		: 0.001,
			"loglevel"		: "debug",
			}

scene.setOptions(options)

# Camera
renderCam = core.Camera(fov = math.pi/4)
renderCam.translate(278, 273, -800)

scene.setCamera(renderCam)

# Dragon
dragon = geometry.ObjMesh("dragon", "/Users/espennordahl/Downloads/dino1.obj")
dragon.scale(30, 30, 30)
dragon.rotateY(-15)
dragon.translate(280, 0, 280)

scene.append(dragon)

#material = shaders.MatteSurface(color(1,1,1))
#dragon.material = material

# Cube

cube = geometry.ObjMesh("cube", "/Users/espennordahl/Downloads/dino2.obj")
cube.scale(30, 30, 30)
cube.rotateY(-15)
cube.translate(280, 0, 280)

scene.append(cube)

# Cornell box
# [...]

# Light
#light = lights.SquareAreaLight(exposure = 5.75, color = color(1,1,1))
#light.translate(280, 548.7, 280)

#scene.append(light)

core.render(scene)
