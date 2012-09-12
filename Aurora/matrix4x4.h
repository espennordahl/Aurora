//
//  matrix4x4.h
//  Aurora
//
//	Abstract:
//	4 by 4 tranformation Matrix. Mostly used as datastructure 
//	in Transform class.

#ifndef Matrix4x4_h
#define Matrix4x4_h

namespace Aurora {

	class Matrix4x4{
	public:
		float m[4][4];
		
		Matrix4x4();
        Matrix4x4(float i[4][4]);
		Matrix4x4(float t00, float t01, float t02, float t03,
				  float t10, float t11, float t12, float t13,
				  float t20, float t21, float t22, float t23,
				  float t30, float t31, float t32, float t33);
		
		Matrix4x4 operator*(const Matrix4x4 &m2);
		Matrix4x4 &operator*=(const Matrix4x4 &m2);
		
		Matrix4x4 transpose();
		Matrix4x4 inverse();
        
        friend std::ostream& operator<<(std::ostream &os, Matrix4x4 const &m){
			os << "(\n" << m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << ", " << m.m[0][3] << std::endl <<
			m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << ", " << m.m[1][3] << std::endl <<
			m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << ", " << m.m[2][3] << std::endl <<
			m.m[3][0] << ", " << m.m[3][1] << ", " << m.m[3][2] << ", " << m.m[3][3] << ")";
            
			return os;
		}

	};
}

#endif
