#include "Utils/GCamera.hpp"
#include <cmath>
#include <cstring>
#include <cstdio>

namespace Abetare
{
    GCamera::GCamera(void)
    {
	SetLookAtStandardPosition(); 
    }
    
    GCamera::~GCamera(void)
    {
    }
    
    void GCamera::SetLookAt(const double eye_x,
			    const double eye_y,
			    const double eye_z,
			    const double center_x,
			    const double center_y,
			    const double center_z,
			    const double up_x,
			    const double up_y,
			    const double up_z)
    { 
	m_eye[0] = eye_x;
	m_eye[1] = eye_y;
	m_eye[2] = eye_z;
	
	m_center[0] = center_x;
	m_center[1] = center_y;
	m_center[2] = center_z;
	
	double d = 1.0 / sqrt((center_x - eye_x) * (center_x - eye_x) +
			      (center_y - eye_y) * (center_y - eye_y) +
			      (center_z - eye_z) * (center_z - eye_z));
	m_forward[0] = d * (center_x - eye_x);
	m_forward[1] = d * (center_y - eye_y);
	m_forward[2] = d * (center_z - eye_z);
	
	//right = view x up
	m_right[0] = m_forward[1] * up_z - m_forward[2] * up_y;
	m_right[1] = m_forward[2] * up_x - m_forward[0] * up_z;
	m_right[2] = m_forward[0] * up_y - m_forward[1] * up_x;
	d = 1.0 / sqrt(m_right[0] * m_right[0] + m_right[1] * m_right[1] + m_right[2] * m_right[2]);
	m_right[0] *= d;
	m_right[1] *= d;
	m_right[2] *= d;
	
	//up = right x view so that base is 90 degrees
	m_up[0] = m_right[1] * m_forward[2] - m_right[2] * m_forward[1];
	m_up[1] = m_right[2] * m_forward[0] - m_right[0] * m_forward[2];
	m_up[2] = m_right[0] * m_forward[1] - m_right[1] * m_forward[0];
	
/*	    printf("up   : %f %f %f [%f]\n", m_up[0], m_up[1], m_up[2], 
	    sqrt(m_up[0] * m_up[0] + m_up[1] * m_up[1] + m_up[2] * m_up[2]));
	    
	    printf("view : %f %f %f [%f]\n", m_forward[0], m_forward[1], m_forward[2],
	    sqrt(m_forward[0] * m_forward[0] + m_forward[1] * m_forward[1] + m_forward[2] * m_forward[2]));
	    
	    printf("right: %f %f %f [%f]\n", m_right[0], m_right[1], m_right[2],
	    sqrt(m_right[0] * m_right[0] + m_right[1] * m_right[1] + m_right[2] * m_right[2]));    
*/
    }
    
    void GCamera::SetLookAtStandardPosition(void)
    {
	SetLookAt(0.0,  0.0, 1.0,
		  0.0,  0.0, 0.0,
		  0.0,  1.0, 0.0);
    }
    
    void GCamera::GetModelViewMatrixOpenGL(double * const m)
    {
	m[0] = m_right[0];
	m[4] = m_right[1];
	m[8] = m_right[2];
	
	m[1] = m_up[0];
	m[5] = m_up[1];
	m[9] = m_up[2];
	
	m[2] = -m_forward[0];
	m[6] = -m_forward[1];
	m[10]= -m_forward[2];
	
	m[12] = -m_eye[0] * m_right[0] - m_eye[1] * m_right[1] - m_eye[2] * m_right[2];
	m[13] = -m_eye[0] * m_up[0]    - m_eye[1] * m_up[1]    - m_eye[2] * m_up[2];
	m[14] =  m_eye[0] * m_forward[0]  + m_eye[1] * m_forward[1]  + m_eye[2] * m_forward[2];
	
	m[3]  = m[7] = m[11] = 0;
	m[15] = 1;
    }
    
    void GCamera::MoveForward(const double d)
    {
	m_eye[0] += d * m_forward[0];
	m_eye[1] += d * m_forward[1];
	m_eye[2] += d * m_forward[2];
    }
    
    void GCamera::MoveRight(const double d)
    {
	m_eye[0] += d * m_right[0];
	m_eye[1] += d * m_right[1];
	m_eye[2] += d * m_right[2];
    }
    
    void GCamera::MoveUp(const double d)
    {
	m_eye[0] += d * m_up[0];
	m_eye[1] += d * m_up[1];
	m_eye[2] += d * m_up[2];
    }
    
    void GCamera::Move(const double df, const double dr, const double du)
    {
	m_eye[0] += df * m_forward[0] + dr * m_right[0] + du * m_up[0];
	m_eye[1] += df * m_forward[1] + dr * m_right[1] + du * m_up[1];
	m_eye[2] += df * m_forward[2] + dr * m_right[2] + du * m_up[2];
    }
    
    void GCamera::RotateAtEye(const double r0, const double r1, const double r2,
			      const double r3, const double r4, const double r5,
			      const double r6, const double r7, const double r8)
    {
	double v[3];
	
	v[0] = m_up[0]; v[1] = m_up[1]; v[2] = m_up[2];
	m_up[0] = r0 * v[0] + r1 * v[1] + r2 * v[2];
	m_up[1] = r3 * v[0] + r4 * v[1] + r5 * v[2];
	m_up[2] = r6 * v[0] + r7 * v[1] + r8 * v[2];

	v[0] = m_right[0]; v[1] = m_right[1]; v[2] = m_right[2];
	m_right[0] = r0 * v[0] + r1 * v[1] + r2 * v[2];
	m_right[1] = r3 * v[0] + r4 * v[1] + r5 * v[2];
	m_right[2] = r6 * v[0] + r7 * v[1] + r8 * v[2];
	
	v[0] = m_forward[0]; v[1] = m_forward[1]; v[2] = m_forward[2];
	m_forward[0] = r0 * v[0] + r1 * v[1] + r2 * v[2];
	m_forward[1] = r3 * v[0] + r4 * v[1] + r5 * v[2];
	m_forward[2] = r6 * v[0] + r7 * v[1] + r8 * v[2];
    }
    
    void GCamera::RotateAtPoint(const double r0, const double r1, const double r2,
				const double r3, const double r4, const double r5,
				const double r6, const double r7, const double r8,
				const double px, const double py, const double pz)
    {
	RotateAtEye(r0, r1, r2, r3, r4, r5, r6, r7, r8);
	
	const double v[3] = {m_eye[0] - px, m_eye[1] - py, m_eye[2] - pz};
	
	m_eye[0] = px + r0 * v[0] + r1 * v[1] + r2 * v[2];
	m_eye[1] = py + r3 * v[0] + r4 * v[1] + r5 * v[2];
	m_eye[2] = pz + r6 * v[0] + r7 * v[1] + r8 * v[2];
    }
    
    
    void GCamera::RotateAroundAxisAtPoint(const double theta, 
					  const double vx, const double vy, const double vz,
					  const double px, const double py, const double pz)
    {
	const double d = 1.0 / sqrt(vx * vx + vy * vy + vz * vz);
	const double x = vx * d;
	const double y = vy * d;
	const double z = vz * d;
	const double c = cos(theta);
	const double s = sin(theta);
	const double t = 1 - c;
	
	RotateAtPoint(t * x * x + c,     t * x * y - z * s, t * x * z + y * s,
		      t * x * y + z * s, t * y * y + c,     t * y * z - x * s,
		      t * x * z - y * s, t * z * y + x * s, t * z * z + c,
		      px, py, pz);	    
    }
}






