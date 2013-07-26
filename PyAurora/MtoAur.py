import core
reload(core)
import lights
reload(lights)
import geometry
reload(geometry)
import shaders
reload(shaders)
import math
import maya.cmds as cmds
from pymel.core import *

def parseShader(inputShd, valuetype):
    """
        Parses a shader and returns the pyAurora shader object.
    """
    if "file" in inputShd.type():
        texname = cmds.getAttr(inputShd + ".fileTextureName")

        # hack: Filter size means it's triplanar
        if (cmds.getAttr(inputShd + ".preFilter") == 1):
            scale = cmds.getAttr(inputShd + ".scale")
            shd = shaders.TriplanarTextureShader(inputShd.name(), valuetype, str(texname), scale)
            return shd
        else:
            shd = shaders.Texture2DShader(inputShd.name(), valuetype, str(texname))
            return shd
    else:
        print "unknown shader type :%s" % inputShd.type()
        raise Exception


def getShaderValue(inputShd, valuename, valuetype, scene):
    # check if anything is connected
    connections = listConnections(inputShd.name() + "." + valuename, d=False, s=True)
    if len(connections) > 0:
        shader = parseShader(connections[0], valuetype)
        scene.appendShader(shader)
        return shader.getName()
    else:
        result = cmds.getAttr(inputShd + "." + valuename)
        if valuetype is "color":
            return [result[0][0], result[0][1], result[0][2]]
        else:
            return result


def parseMaterial(inputShd, objName, scene):
    """
        Parses a material assigned to the mesh. 
        Puts the material object and all children into the material pool 
        and returns the name of material.
    """
    col = cmds.getAttr(inputShd + ".color")[0]
    
    material = "" ## init
    if "lambert" in inputShd.type():
        diffcol = getShaderValue(inputShd, "color", "color", scene)
        material = shaders.MatteMaterial(inputShd.name(), diffcol)
    elif "blinn" in inputShd.type():
        # First check if refractions are on
        if cmds.getAttr(inputShd + ".refractions") == 1:
            speccol = cmds.getAttr(inputShd + ".specularColor")[0]
            
            if (speccol[0] + speccol[1] + speccol[2] == 0):
                # If specCol is black we create a TransmitMaterial
                ior = cmds.getAttr(inputShd + ".refractiveIndex")
                material = shaders.TransmitMaterial(inputShd.name(), [speccol[0],speccol[1],speccol[2]],ior)
            else:
                # If not, we create a glass shader
                ior = cmds.getAttr(inputShd + ".refractiveIndex")
                reflectance = cmds.getAttr(inputShd + ".reflectivity")
                transmitcol = col
                material = shaders.GlassMaterial(inputShd.name(), [speccol[0],speccol[1],speccol[2]],
                                                 [transmitcol[0],transmitcol[1],transmitcol[2]],
                                                     reflectance, ior)
        else:
            # Check if this is a car paint shader
            print cmds.listAttr(inputShd.name())
            if "clearcoatReflectance" in cmds.listAttr(inputShd.name()):
                diffcol = [col[0], col[1], col[2]]
                clearcoatRefl = cmds.getAttr(inputShd + ".clearcoatReflectance")
                clearcoatRough = cmds.getAttr(inputShd + ".clearcoatRoughness")
                topPaintCol = [cmds.getAttr(inputShd + ".topPaintColorR"),
                               cmds.getAttr(inputShd + ".topPaintColorG"),
                               cmds.getAttr(inputShd + ".topPaintColorB")]
                topSpecRefl = cmds.getAttr(inputShd + ".baseTopSpecReflectance")
                topSpecRough = cmds.getAttr(inputShd + ".baseTopSpecRoughness")
                bottomPaintCol = [cmds.getAttr(inputShd + ".bottomPaintColorR"),
                                  cmds.getAttr(inputShd + ".bottomPaintColorG"),
                                  cmds.getAttr(inputShd + ".bottomPaintColorB")]
                bottomSpecRefl = cmds.getAttr(inputShd + ".baseBottomSpecReflectance")
                bottomSpecRough = cmds.getAttr(inputShd + ".baseBottomSpecRoughness")
                material = shaders.CarMaterial(inputShd.name(),
                                               diffcol,
                                               clearcoatRefl,
                                               clearcoatRough,
                                               topPaintCol,
                                               topSpecRefl,
                                               topSpecRough,
                                               bottomPaintCol,
                                               bottomSpecRefl,
                                               bottomSpecRough)
            else:
                # If there are no refractions or car paint attributes we create a KelemenShader
                # base parameters
                diffcol = getShaderValue(inputShd, "color", "color", scene)
                speccol = getShaderValue(inputShd, "specularColor", "color", scene)
                roughness = getShaderValue(inputShd, "specularRollOff", "float", scene)
                reflectance = getShaderValue(inputShd, "reflectivity", "float", scene)
                connections = listConnections(inputShd.name() + ".color", d=False, s=True)
                material = shaders.KelemenMaterial(inputShd.name(),
                                                   diffcol,
                                                   speccol,
                                                   roughness,
                                                   reflectance)
    elif "phongE" in inputShd.type():
        reflectance = getShaderValue(inputShd, "color", "color", scene)
        roughnessA = getShaderValue(inputShd, "roughness", "float", scene)
        roughnessB = getShaderValue(inputShd, "highlightSize", "float", scene)
        mix = getShaderValue(inputShd, "reflectivity", "float", scene)
        gain = getShaderValue(inputShd, "specularColor", "color", scene)
        material = shaders.MetalMaterial(inputShd.name(), reflectance, roughnessA, roughnessB, mix, gain)
    else:
        print "ERROR: Can't find shader type for object %s" % objName
        raise Exception

    # Look for normal map shader
    if "normalmap" in cmds.listAttr(inputShd.name()):
        material.setNormalMap(getShaderValue(inputShd, "normalmap", "color", scene))
    return material

def parseLight(light):
    """
        Parses a light and returns the light object.
        Supports infinite area lights and naively assumes everyhing 
        else is a rectangular area light.
    """
    liObj = ""
    ## Envlight. TODO: There's probably a better way to detect light type..
    if "ambientShade" in cmds.listAttr(light.name()):
        intensity = cmds.getAttr(light.name() + ".intensity")
        color = cmds.getAttr(light.name() + ".color")[0]
        envmap = cmds.getAttr(light.name() + ".envmap")
        liObj = lights.EnvLight(light.name(), [color[0], color[1], color[2]], math.log(intensity,2), envmap)
    ## Area Light
    else:
        intensity = cmds.getAttr(light.name() + ".intensity")
        color = cmds.getAttr(light.name() + ".color")[0]
        trans = light.getParent().getTranslation()
        rot = light.getParent().getRotation()  
        scale = light.getParent().getScale()

        liObj = lights.SquareLight(light.name(), scale[0], scale[1], [color[0], color[1], color[2]], math.log(intensity,2))
        liObj.translate(trans[0], trans[1], trans[2])

        liObj.rotateZ(rot[2])
        liObj.rotateY(rot[1])
        liObj.rotateX(rot[0])
    return liObj

def parseMesh(mesh, lazy):
    path = "/Users/espennordahl/Documents/Aurora/data/geo/"

    # select mesh
    cmds.select(mesh.name())
    if not lazy:
        cmds.file(path + mesh.name(), force=True, pr=1,typ="OBJexport",es=1, op="groups=0; ptgroups=0; materials=0; smoothing=0; normals=0")
    subdlevels = 0
    if "subdlevels" in cmds.listAttr(mesh.name()):
        subdlevels = cmds.getAttr(mesh.name() + ".subdlevels")
    obj = geometry.ObjMesh(mesh.name(), subdlevels, path + mesh.name() + ".obj")
    
    return obj

def auRender(lazy = False):
    scene = core.Scene()


    ## globals
    options = {
                "filename"        : cmds.getAttr("AURglobals.fileName"),
                "lightsamples"    : cmds.getAttr("AURglobals.lightsamples"),
                "mindepth"        : cmds.getAttr("AURglobals.mindepth"),
                "maxdepth"        : cmds.getAttr("AURglobals.maxdepth"),
                "pixelsamples"	  : cmds.getAttr("AURglobals.pixelsamples"),
                "resolution"	  : [cmds.getAttr("AURglobals.resX"), cmds.getAttr("AURglobals.resY")],
                "useEmbree"		  : cmds.getAttr("AURglobals.useEmbree"),
    }

    scene.setOptions(options)

    ## camera
    cam = ls(type='camera')[1] ## TODO: pretty darn hacky to take the 2nd arg here..
    for c in ls(type='camera'):
        if "persp" in c.name():
            cam = c
    trans = cam.getParent().getTranslation()
    rot = cam.getParent().getRotation()
        
    renderCam = core.Camera(fov = cmds.getAttr("AURglobals.fieldofview"))

    renderCam.translate(trans[0], trans[1], trans[2])

    renderCam.rotateZ(rot[2])
    renderCam.rotateY(rot[1])
    renderCam.rotateX(rot[0])

    scene.setCamera(renderCam)

    ## objects
    print "MtoAur: Parsing objects."
    for mesh in ls(type='mesh', visible=True):
        obj = parseMesh(mesh, lazy)
        
        # get shading groups from shapes:
        shadingGrps = listConnections(mesh,type='shadingEngine')
        
        # get the shaders:
        shd = ls(listConnections(shadingGrps),materials=1) 
        material = parseMaterial(shd[0], mesh.name(), scene)
        # appen to shader pool
        scene.appendMaterial(material)
        obj.setMaterial(material.getName())
        scene.append(obj)

    ## lights
    print "MtoAur: Parsing lights."
    for light in ls(type="light"):
        liObj = parseLight(light)
        scene.append(liObj)
			
    print "MtoAur: Rendering"
    core.render(scene)
    print "MtoAur: Done rendering."
