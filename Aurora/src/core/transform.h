//
//  transform.h
//  Aurora
//

#ifndef Transform_h
#define Transform_h

#include "bbox.h"
#include "ray.h"
#include "point.h"
#include "vector.h"
#include "matrix4x4.h"
#include "math.h"

namespace Aurora {
	class Transform{

	public:		
		Transform();
		Transform(const Matrix4x4 &_m);
		Transform(const Matrix4x4 &_m, const Matrix4x4 &inv);
		
		Transform inverse(const Transform &t);
		
		void translate(const Vector &delta);
		void scale(float x, float y, float z);
		void rotateX(float angle);
		void rotateY(float angle);
		void rotateZ(float angle);
		void rotate(float angle, const Vector &axis);
		void lookAt(const Point &pos, const Point &look, const Vector &up);
		
		Transform operator*(const Transform &t2);
		Transform &operator*=(const Transform &t2);

		
		Point operator()(const Point &p) const;
		void operator()(const Point &p, Point *ptrans) const;
		
		Vector operator()(const Vector &v) const;
		void operator()(const Vector &v, Vector *vtrans) const;
		
		Ray operator()(const Ray &r) const;
		void operator()(const Ray &r, Ray *rtrans) const;
				
		BBox operator()(const BBox &b) const;
		void operator()(const BBox &b, BBox *btrans) const;
		
		bool swapsHandedness() const;
		
		
		friend std::ostream& operator<<(std::ostream &os, Transform const &t){
			os << "(" << t.m.m[0][0] << ", " << t.m.m[0][1] << ", " << t.m.m[0][2] << ", " << t.m.m[0][3] << std::endl <<
			t.m.m[1][0] << ", " << t.m.m[1][1] << ", " << t.m.m[1][2] << ", " << t.m.m[1][3] << std::endl <<
			t.m.m[2][0] << ", " << t.m.m[2][1] << ", " << t.m.m[2][2] << ", " << t.m.m[2][3] << std::endl <<
			t.m.m[3][0] << ", " << t.m.m[3][1] << ", " << t.m.m[3][2] << ", " << t.m.m[3][3] << ")";

			return os;
		}

//	protected:
		Matrix4x4 m;
		Matrix4x4 mInv;

	};
}


#endif
