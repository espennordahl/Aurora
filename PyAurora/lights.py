import core

class Light(core.Serializable):
    def __init__(self, name, serialize):
        s = serialize
        s.append("transforms")
        s.append("type")
        s.append("name")
        core.Serializable.__init__(self, name, s)
        self._s["type"] = "light"

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

    def serialize(self, root):
        serializedObj = {}
        for key in self._serialize:
            if isinstance(self._s[key], core.Serializable):
                self._s[key].serialize(serializedObj)
            else:
                serializedObj[key] = self._s[key]
        root[self._s["name"]] = serializedObj

class SquareLight(Light):
	def __init__(self, name, scaleX, scaleY, color, exposure):
		Light.__init__(self, name, ["scaleX", "scaleY", "color", "exposure"])
		i = 0
		for c in color:
			color[i] = float(c)
			i += 1
		self._s["color"] = color
		self._s["exposure"] = float(exposure)
		self._s["scaleX"] = float(scaleX)
		self._s["scaleY"] = float(scaleY)

class EnvLight(Light):
	def __init__(self, name, color, exposure, envmap):
		Light.__init__(self, name, ["color", "exposure", "envmap"])
		i = 0
		for c in color:
			color[i] = float(c)
			i += 1
		self._s["color"] = color
		self._s["exposure"] = float(exposure)
		self._s["type"] = "envlight"
		self._s["envmap"] = envmap
