from StringIO import StringIO
import subprocess
import sys
import os
import json
from time import localtime, strftime
import math

###########################
#
#   SERIALIZATION
#
###########################

class Serializable(object):
    """
        Abstract:
        Framework for serializing objects into json strings.
        
        Subclassing:
        All serializable parameters are put into a list of strings
        and passed to the initializer. 
        These parameters are then initialized as keys of the dictionary
        self._s, as an empty list. If a class wants to use a different 
        iterable class (dict etc) to store its data, they need to implement
        this on their own end after calling the Serializable init method.
        """
    def __init__(self, name, serialize):
        self._s = {}
        for key in serialize:
            self._s[key] = []
        self._s["name"] = str(name)
        self._serialize = serialize
	
    def getName(self):
        return self._s["name"]
        
    def serialize(self, root):
        for key in self._serialize:
            if isinstance(self._s[key], type(Serializable)):
                root[key] = self._s[key].serialize(root)
            elif isinstance(self._s[key], list):
                root[key] = {}
                for item in self._s[key]:
                    if isinstance(item, dict):
                        root[key] = self._s[key]
                    else:
                        print "name %s, value %s" % (key, item)
                        item.serialize(root[key])
            else:
                root[key] = self._s[key]

##############################
#
#   BASE CLASSES
#
##############################

class Scene(Serializable):
    def __init__(self):
        Serializable.__init__(self, "scene", ["options", "renderCam", "objects", "materials", "shaders"])
        self._s["options"] = {}

    def setOptions(self, options):
		self._s["options"] = options
    
    def setCamera(self, camera):
        self._s["renderCam"] = [camera]
    
    def append(self, obj):
        self._s["objects"].append(obj)
    
    def appendShader(self, shader):
        self._s["shaders"].append(shader)

    def appendMaterial(self, shader):
        self._s["materials"].append(shader)

class Camera(Serializable):
	def __init__(self, fov = math.pi/4):
		Serializable.__init__(self, "camera", ["transforms", "fov"])
		self._s["fov"] = float(fov)
    
	def translate(self, x, y, z):
		self._s["transforms"].append({"translate": [float(x),float(y),float(z)]})
    
	def scale(self, x, y, z):
		self._s["transforms"].append({"scale": [float(x),float(y),float(z)]})
    
	def rotateX(self, x):
		self._s["transforms"].append({"rotateX": float(x)})
    
	def rotateY(self, x):
		self._s["transforms"].append({"rotateY": float(x)})
    
	def rotateZ(self, x):
		self._s["transforms"].append({"rotateZ": float(x)})

######################################
#
#   RENDERING
#
######################################


def render(scene):
	root = {}
	scene.serialize(root)
    
	finalFile = "// AURORA SCENE FILE\n// generated %s\n" % strftime("%Y-%m-%d %H:%M:%S", localtime())
	finalFile = finalFile + '\n' + json.dumps(root, indent=4)
    
    # TODO: This shouldn't be hard coded
	name = "/Users/espennordahl/Documents/Aurora/pyAurora/tmp.asc"
	final = json.dumps(root, indent=4)
	f = open(name, 'w')
	f.write(final)
	f.close()
	
	os.system("/Users/espennordahl/Library/Developer/Xcode/DerivedData/Aurora-cmdmezamgwsfxmcefttsjvbqkhin/Build/Products/Release/Aurora %s" % name)

####################
#
#   LOGGING
#
####################

#ifndef MtoAurlog
#endif