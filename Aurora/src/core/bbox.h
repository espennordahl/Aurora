//
//  bbox.h
//  Aurora
//
//	abstract:
//	desribes a bounding box around an object.

#ifndef BBox_h
#define BBox_h

#include "point.h"
#include "ray.h"

namespace Aurora {
	class BBox{
	public:
		// data
		Point pMin;
		Point pMax;
		
		// constructors
		BBox();
		BBox( const Point &p );
		BBox( const Point &p1, const Point &p2 );
		
		// methods
		void Union( const Point &p );
		void Union(  const BBox &b2 );
		
		bool overlaps( const BBox &b ) const;
		bool inside( const Point &p) const;		
		void expand( float delta );
		float area() const;
		float volume() const;
		int maxExtent() const;
		Point lerp( float x, float y, float z ) const;
		void boudingSphere( Point *c, float *rad ) const;
		float surfaceArea() const;
		
		bool intersectP(const Ray &Ray, float *thit0, float *thit1) const;
		
		friend std::ostream& operator<<(std::ostream &os, BBox const &b){
			os << "Min:" << b.pMin << ", Max:" << b.pMax << ".";
			return os;
		};
	};
}

#endif
