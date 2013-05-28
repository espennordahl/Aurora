import core

class Mesh(core.Serializable):
	def __init__(self, name, serialize):
		s = serialize
		s.append("name")
		s.append("transforms")
		s.append("material")
		s.append("type")
		core.Serializable.__init__(self, name, s)
		self._s["type"] = "geometry"
	
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

	def setMaterial(self, m):
		self._s["material"] = m

	def serialize(self, root):
		serializedObj = {}
		for key in self._serialize:
			if isinstance(self._s[key], core.Serializable):
				self._s[key].serialize(serializedObj)
			else:
				serializedObj[key] = self._s[key]
		root[self._s["name"]] = serializedObj


class ObjMesh(Mesh):
	def __init__(self, name, path):
		Mesh.__init__(self, name, ["path"])
		self._s["path"] = path
		self._s["type"] = "geometry"

class TriangleMesh(Mesh):
	def __init__(self, name, points, pointIndices):
		Mesh.__init__(self, name, ["points", "pointIndices"])
		self._s["points"] = points
		self._s["pointIndices"] = pointIndices
