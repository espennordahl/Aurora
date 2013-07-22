import MtoAur
import maya.cmds as cmds
from pymel.core import *

def createGlobals():
    if not cmds.objExists("AURglobals"):
        newLocator = cmds.spaceLocator( name = "AURglobals" )
        
        cmds.addAttr("AURglobalsShape", ln="fileName", dt="string")
        cmds.addAttr("AURglobalsShape", ln="pixelsamples", at="float")
        cmds.addAttr("AURglobalsShape", ln="lightsamples", at="float")
        cmds.addAttr("AURglobalsShape", ln="mindepth", at="float")
        cmds.addAttr("AURglobalsShape", ln="maxdepth", at="float")
        cmds.addAttr("AURglobalsShape", ln="resX", at="float")
        cmds.addAttr("AURglobalsShape", ln="resY", at="float")
        cmds.addAttr("AURglobalsShape", ln="fieldofview", at="float")

        cmds.setAttr("AURglobalsShape.fileName", "$RENDER/aurora_####.exr", type="string")
        cmds.setAttr("AURglobalsShape.pixelsamples", 8)
        cmds.setAttr("AURglobalsShape.lightsamples", 8)
        cmds.setAttr("AURglobalsShape.mindepth", 1)
        cmds.setAttr("AURglobalsShape.maxdepth", 1)
        cmds.setAttr("AURglobalsShape.resX", 512)
        cmds.setAttr("AURglobalsShape.resY", 512)
        cmds.setAttr("AURglobalsShape.fieldofview", 0.48)


class RenderGlobals:
    def __init__(self):
        self.name = "renderGlobals"
        self.title = "Aurora Globals"
        # Check that a locator exists
        createGlobals()
        
        # Begin creating the UI
        if (cmds.window(self.name, q=1, exists=1)): cmds.deleteUI(self.name)
        self.window = cmds.window(self.name, title=self.title)
        
        cmds.columnLayout()
        
        self.fileName = cmds.textFieldGrp( label='fileName', text=cmds.getAttr("AURglobalsShape.fileName") )
        self.resX = cmds.textFieldGrp( label='resolutionX', text=cmds.getAttr("AURglobalsShape.resX") )
        self.resY = cmds.textFieldGrp( label='resolutionY', text=cmds.getAttr("AURglobalsShape.resY") )
        self.pixelsamples = cmds.textFieldGrp( label='pixelSamples', text=cmds.getAttr("AURglobalsShape.pixelsamples") )
        self.lightsamples = cmds.textFieldGrp( label='lightSamples', text=cmds.getAttr("AURglobalsShape.lightsamples") )
        self.mindepth = cmds.textFieldGrp( label='minDepth', text=cmds.getAttr("AURglobalsShape.mindepth") )
        self.maxdepth = cmds.textFieldGrp( label='maxDepth', text=cmds.getAttr("AURglobalsShape.maxdepth") )
        self.fieldofview = cmds.textFieldGrp( label='fieldOfView', text=cmds.getAttr("AURglobalsShape.fieldofview") )
        
        self.renderButton = cmds.button(label='Render', w=100, c=self.render)
        self.lazybutton = cmds.button(label='Render Lazy', w=100, c=self.renderLazy)
        
        cmds.window(self.window, e=1, w=500, h=500)
        cmds.showWindow(self.window)

    def updateGlobals(self):
        cmds.setAttr("AURglobalsShape.fileName", cmds.textFieldGrp(self.fileName, q=True, tx=True), type="string")
        cmds.setAttr("AURglobalsShape.pixelsamples", float(cmds.textFieldGrp(self.pixelsamples, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.lightsamples", float(cmds.textFieldGrp(self.lightsamples, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.mindepth", float(cmds.textFieldGrp(self.mindepth, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.maxdepth", float(cmds.textFieldGrp(self.maxdepth, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.resX", float(cmds.textFieldGrp(self.resX, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.resY", float(cmds.textFieldGrp(self.resY, q=True, tx=True)))
        cmds.setAttr("AURglobalsShape.fieldofview", float(cmds.textFieldGrp(self.fieldofview, q=True, tx=True)))
    
    def render(self, arg):
        reload(MtoAur)
        self.updateGlobals()
        MtoAur.auRender(lazy = False)
    
    def renderLazy(self, arg):
        reload(MtoAur)
        self.updateGlobals()
        MtoAur.auRender(lazy = True)