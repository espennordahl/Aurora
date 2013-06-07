import core

class Texture2DShader(core.Serializable):
    def __init__(self, name, attributetype, texturename):
        core.Serializable.__init__(self, name, ["texturename", "type", "attributetype"])
        self._s["texturename"] = texturename
        self._s["type"] = "texture2DShader"
        self._s["attributetype"] = attributetype

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
        root[self.getName()] = serializedObj


class MatteMaterial(core.Serializable):
    def __init__(self, name, color = [1,1,1]):
        if not isinstance(color, str):
            color[0] = float(color[0])
            color[1] = float(color[1])
            color[2] = float(color[2])
        core.Serializable.__init__(self, name, ["color", "type"])
        self._s["color"] = color
        self._s["type"] = "matteMaterial"

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
        root[self.getName()] = serializedObj

class MetalMaterial(core.Serializable):
    def __init__(self, name, reflectance, roughnessA, roughnessB, mix):
        if not isinstance(reflectance, str):
            reflectance[0] = float(reflectance[0])
            reflectance[1] = float(reflectance[1])
            reflectance[2] = float(reflectance[2])
        core.Serializable.__init__(self, name, ["reflectance", "roughnessA", "roughnessB", "mix", "type"])
        self._s["reflectance"] = reflectance
        self._s["roughnessA"] = roughnessA
        self._s["roughnessB"] = roughnessB
        self._s["mix"] = mix
        self._s["type"] = "metalMaterial"

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
        root[self.getName()] = serializedObj


class TransmitMaterial(core.Serializable):
    def __init__(self, name, color = [1,1,1], ior = 1.333):
        if not isinstance(color, str):
            color[0] = float(color[0])
            color[1] = float(color[1])
            color[2] = float(color[2])
        core.Serializable.__init__(self, name, ["color", "ior", "type"])
        self._s["color"] = color
        self._s["ior"] = float(ior)
        self._s["type"] = "transmitMaterial"

	def serialize(self, root):
		serializedObj = {}
		for key in self._serialize:
			if isinstance(self._s[key], core.Serializable):
				serializedObj[key] = {}
				self._s[key].serialize(serializedObj[key])
			else:
				serializedObj[key] = self._s[key]
		root[self.getName()] = serializedObj

class GlassMaterial(core.Serializable):
    def __init__(self, name, specCol = [1,1,1], transmitCol = [1,1,1], reflectance = 1.0, ior = 1.333):
        if not isinstance(specCol, str):
            specCol[0] = float(specCol[0])
            specCol[1] = float(specCol[1])
            specCol[2] = float(specCol[2])
        if not isinstance(transmitCol, str):
            transmitCol[0] = float(transmitCol[0])
            transmitCol[1] = float(transmitCol[1])
            transmitCol[2] = float(transmitCol[2])
        core.Serializable.__init__(self, name, ["specColor", "transmitColor", "reflectance", "ior", "type"])
        self._s["specColor"] = specCol
        self._s["transmitColor"] = transmitCol
        self._s["reflectance"] = float(reflectance)
        self._s["ior"] = float(ior)
        self._s["type"] = "glassMaterial"

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
        root[self.getName()] = serializedObj

class CarMaterial(core.Serializable):
    def __init__(self,
                 name,
                 diffcolor = [1,1,1],
                 clearcoatreflectance = 0.1,
                 clearcoatroughness = 0.1,
                 toppaintcolor = [1,1,1],
                 basespectopreflectance = 0.2,
                 basespectoproughness = 0.2,
                 bottompaintcolor = [1,1,1],
                 basespecbottomreflectance = 0.3,
                 basespecbottomroughness = 0.3,
                 ):
        if not isinstance(diffcolor, str):
            diffcolor[0] = float(diffcolor[0])
            diffcolor[1] = float(diffcolor[1])
            diffcolor[2] = float(diffcolor[2])
        if not isinstance(toppaintcolor, str):
            toppaintcolor[0] = float(toppaintcolor[0])
            toppaintcolor[1] = float(toppaintcolor[1])
            toppaintcolor[2] = float(toppaintcolor[2])
        if not isinstance(bottompaintcolor, str):
            bottompaintcolor[0] = float(bottompaintcolor[0])
            bottompaintcolor[1] = float(bottompaintcolor[1])
            bottompaintcolor[2] = float(bottompaintcolor[2])
        core.Serializable.__init__(self, name,
                                   ["diffusecolor",
                                    "clearcoatreflectance",
                                    "clearcoatroughness",
                                    "toppaintcolor",
                                    "basespectopreflectance",
                                    "basespectoproughness",
                                    "bottompaintcolor",
                                    "basespecbottomreflectance",
                                    "basespecbottomroughness",
                                    "type"])
        self._s["diffusecolor"] = diffcolor
        self._s["clearcoatreflectance"] = float(clearcoatreflectance)
        self._s["clearcoatroughness"] = float(clearcoatroughness)
        self._s["toppaintcolor"] = toppaintcolor
        self._s["basespectopreflectance"] = float(basespectopreflectance)
        self._s["basespectoproughness"] = float(basespectoproughness)
        self._s["bottompaintcolor"] = bottompaintcolor
        self._s["basespecbottomreflectance"] = float(basespecbottomreflectance)
        self._s["basespecbottomroughness"] = float(basespecbottomroughness)
        self._s["type"] = "carMaterial"

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
            root[self.getName()] = serializedObj


class KelemenMaterial(core.Serializable):
    def __init__(self,
				name,
				diffcolor = [1,1,1], 
				speccolor = [1,1,1], 
				roughness = 50,
				reflectance = 0.1,
				):
        if not isinstance(diffcolor, str):
            diffcolor[0] = float(diffcolor[0])
            diffcolor[1] = float(diffcolor[1])
            diffcolor[2] = float(diffcolor[2])
        if not isinstance(speccolor, str):
            speccolor[0] = float(speccolor[0])
            speccolor[1] = float(speccolor[1])
            speccolor[2] = float(speccolor[2])
        core.Serializable.__init__(self, name,
                                   [   "diffusecolor",
                                        "speccolor",
                                        "roughness",
                                        "reflectance",
                                        "type"])
        self._s["diffusecolor"] = diffcolor
        self._s["speccolor"] = speccolor
        self._s["roughness"] = float(roughness)
        self._s["reflectance"] = float(reflectance)
        self._s["type"] = "kelemenMaterial"

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                serializedObj[key] = {}
                self._s[key].serialize(serializedObj[key])
            else:
                serializedObj[key] = self._s[key]
            root[self.getName()] = serializedObj
