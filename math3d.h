#ifndef _MATH3D_H
#define _MATH3D_H

struct float2;
struct float3;
struct float4;
struct matrix;

struct float2
{
	float x, y;
	float2() : x(0), y(0) {}
	float2(const float2& v) : x(v.x), y(v.y) {}
	float2(float _x, float _y) : x(_x), y(_y) {}
	
	float2 operator+(const float2& v) const { return float2(x+v.x, y+v.y); }
	float2 operator-(const float2& v) const { return float2(x-v.x, y-v.y); }
	float2 operator*(const float2& f) const { return float2(x*f.x, y*f.y); }
	float2 operator/(const float2& f) const { return float2(x/f.x, y/f.y); }
	float2 operator*(const float f) const 	{ return float2(x*f, y*f); }
	float2 operator/(const float f) const 	{ return float2(x/f, y/f); }

	float2& operator+=(const float2& f) { x += f.x; y += f.y; return *this; }
	float2& operator-=(const float2& f) { x -= f.x; y -= f.y; return *this; }
};
struct float3
{
	float x, y, z;
	float3() : x(0), y(0), z(0) {}
	float3(const float3& v) : x(v.x), y(v.y), z(v.z) {}
	float3(const float2& xy, float _z) : x(xy.x), y(xy.y), z(_z) {}
	float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	float3 operator-() const { return float3(-x, -y, -z); }
	float3 operator+() const { return float3(x, y, z); }
	float3 operator+(const float3& v) const { return float3(x+v.x, y+v.y, z+v.z); }
	float3 operator-(const float3& v) const { return float3(x-v.x, y-v.y, z-v.z); }
	float3 operator*(const float3& v) const { return float3(x*v.x, y*v.y, z*v.z); }
	float3 operator/(const float3& v) const { return float3(x/v.x, y/v.y, z/v.z); }
	float3 operator*(const float f) const 	{ return float3(x*f, y*f, z*f); }
	float3 operator/(const float f) const 	{ return float3(x/f, y/f, z/f); }

	float3& operator+=(const float3& f) { (*this) = (*this) + f; return *this; }
	float3& operator-=(const float3& f) { (*this) = (*this) - f; return *this; }
	float3& operator*=(const float3& f) { (*this) = (*this) * f; return *this; }
	float3& operator/=(const float3& f) { (*this) = (*this) / f; return *this; }
	float3& operator*=(const float f) { (*this) = (*this) * f; return *this; }
	float3& operator/=(const float f) { (*this) = (*this) / f; return *this; }
};
struct float4
{
	float x, y, z, w;

	float4() : x(0), y(0), z(0), w(0) {}
	float4(const float4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	float4(const float3& xyz, float _w) : x(xyz.x), y(xyz.y), z(xyz.z), w(_w) {}
	float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	float4 operator+(const float4& v) const { return float4(x+v.x, y+v.y, z+v.z, w+v.w); }
	float4 operator-(const float4& v) const { return float4(x-v.x, y-v.y, z-v.z, w-v.w); }
};

float length(const float2& f)
{
	return sqrt(f.x*f.x + f.y*f.y);
}
float length(const float3& f)
{
	return sqrt(f.x*f.x + f.y*f.y + f.z*f.z);
}
float length(const float4& f)
{
	return sqrt(f.x*f.x + f.y*f.y + f.z*f.z + f.w*f.w);
}
float length2(const float3& f)
{
	return f.x*f.x + f.y*f.y + f.z*f.z;
}
float3 normalize(const float3& f)
{
	return f / length(f);
}
float dot(const float2& v1, const float2& v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}
float dot(const float3& v1, const float3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
float dot(const float4& v1, const float4& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}
float3 cross(const float3& u, const float3& v)
{
	return float3(
		u.y*v.z - u.z*v.y,
		u.z*v.x - u.x*v.z,
		u.x*v.y - u.y*v.x);
}

template<typename type>
type lerp(const type& u, const type& v, float t)
{
	return u + (v - u) * t;
}

matrix transpose(const matrix& m);

struct matrix
{
	float m[16];

	float3 left()
	{
		// return float3(m[0], m[4], m[8]);
		return float3(m[0], m[1], m[2]);
	}
	float3 up()
	{
		// return float3(m[1], m[5], m[9]);
		return float3(m[4], m[5], m[6]);
	}
	float3 forward()
	{
		// return float3(m[2], m[6], m[10]);
		return float3(m[8], m[9], m[10]);
	}

	matrix operator*(const matrix& o) const
	{
		const matrix& v = o;
		const matrix& u = *this;

		float4 r1(u.m[0], u.m[1], u.m[2], u.m[3]);
		float4 r2(u.m[4], u.m[5], u.m[6], u.m[7]);
		float4 r3(u.m[8], u.m[9], u.m[10],u.m[11]);
		float4 r4(u.m[12],u.m[13],u.m[14],u.m[15]);

		float4 c1(v.m[0], v.m[4], v.m[8], v.m[12]);
		float4 c2(v.m[1], v.m[5], v.m[9], v.m[13]);
		float4 c3(v.m[2], v.m[6], v.m[10],v.m[14]);
		float4 c4(v.m[3], v.m[7], v.m[11],v.m[15]);

		matrix w = {{
			dot(r1,c1), dot(r1,c2), dot(r1,c3), dot(r1,c4),
			dot(r2,c1), dot(r2,c2), dot(r2,c3), dot(r2,c4),
			dot(r3,c1), dot(r3,c2), dot(r3,c3), dot(r3,c4),
			dot(r4,c1), dot(r4,c2), dot(r4,c3), dot(r4,c4),
		}};

		return w;
	}
	float3 operator*(const float3& f) const
	{
		return float3(
			f.x*m[0] + f.y*m[4] + f.z*m[8],
			f.x*m[1] + f.y*m[5] + f.z*m[9],
			f.x*m[2] + f.y*m[6] + f.z*m[10]);
	}
	matrix& operator*=(const matrix& o)
	{
		(*this) = (*this) * o;
		return *this;
	}

	static matrix rotateX(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);

		matrix m = {{
			1, 0, 0, 0,
			0, c,-s, 0,
			0, s, c, 0,
			0, 0, 0, 1,
		}};
		return m;
	}
	static matrix rotateY(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);

		matrix m = {{
			c, 0, s, 0,
			0, 1, 0, 0,
			-s,0, c, 0,
			0, 0, 0, 1
		}};

		return m;
	}
	static matrix rotateZ(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);

		matrix m = {{
			c,-s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		}};
		return m;
	}
	static matrix rotateAxis(const float3& axis, float angle)
	{
		float3 u = normalize(axis);
		
		float c = cos(angle);
		float rc = 1-c;
		float s = sin(angle);
		
		matrix m = {{
			c + u.x*u.x*rc,		u.x*u.y*rc-u.z*s, 	u.x*u.z*rc+u.y*s, 	0,
			u.y*u.x*rc+u.z*s,	c+u.y*u.y*rc,		u.y*u.z*rc-u.x*s, 	0,
			u.z*u.x*rc-u.y*s,	u.z*u.y*rc+u.x*s,	c+u.z*u.z*rc,	  	0,
			0,					0,					0,					1
		}};

		return m;
	}
	static matrix scale(const float3& s)
	{
		matrix m = {{
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1,
		}};
		return m;
	}
	static matrix scale(const float f)
	{
		matrix m = {{
			f,0,0,0,
			0,f,0,0,
			0,0,f,0,
			0,0,0,1
		}};
		return m;
	}
	static matrix translate(float x, float y, float z)
	{
		matrix m = {{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1
		}};
		return m;
	}
	static matrix translate(const float3& t)
	{
		matrix m = {{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			t.x,t.y,t.z,1
		}};
		return m;
	}

	static matrix identity()
	{
		matrix m = {{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		}};
		return m;
	}
	static matrix ortho(float w, float h, float zn, float zf)
	{
		float d = zf - zn;
		matrix m = {{
			2/w,0,0,0,
			0,2/h,0,0,
			0,0,2/d,0,
			0,0,-(zf+zn)/d,1
		}};
		return m;
	};
	static matrix orthoOffCenter(float l, float r, float b, float t, float zn, float zf)
	{
		float w = r - l;
		float h = t - b;
		float d = zf - zn;

		matrix m = {{
			2/w, 			 0,   	0,   0,
			0,   		   2/h, 	0,   0,
			0,				 0, 	2/d, 0,
			-(r+l)/w, -(t+b)/h,-(zf+zn)/d,1
		}};

		return m;
	}
	static matrix lookAt(const float3& pos, const float3& target, const float3& up)
	{
		float3 az = normalize(pos - target);
		float3 ax = normalize(cross(up, az));
		float3 ay = cross(az, ax);
		matrix m = {{
			ax.x,		ay.x,		az.x,		0,
			ax.y,		ay.y,		az.y,		0,
			ax.z,		ay.z,		az.z,		0,
			-dot(ax,pos),-dot(ay,pos),-dot(az,pos),1
		}};
		return m;
		/**
		 * 	zaxis = normal(cameraPosition - cameraTarget)
			xaxis = normal(cross(cameraUpVector, zaxis))
			yaxis = cross(zaxis, xaxis)

			 xaxis.x           yaxis.x           zaxis.x          0
			 xaxis.y           yaxis.y           zaxis.y          0
			 xaxis.z           yaxis.z           zaxis.z          0
			-dot(xaxis, cameraPosition)  -dot(yaxis, cameraPosition)  -dot(zaxis, cameraPosition)  1
		 */
	}
	static matrix perspective(float fov, float aspect, float znear, float zfar)
	{
	    float yscale = 1.f / tan(fov / 2);
	    float xscale = yscale / aspect;
	    float nmf = znear - zfar;
	    matrix m = {{
	        xscale, 0, 0, 0,
	        0, yscale, 0, 0,
	        0, 0, (zfar + znear) / nmf, -1,
	        0, 0, 2*zfar*znear/nmf, 0 
	    }};
	    return m;
	}
};

float3 mul(const float3& f, const matrix& m)
{
	return float3(
		f.x*m.m[0] + f.y*m.m[4] + f.z*m.m[8] + m.m[12],
		f.x*m.m[1] + f.y*m.m[5] + f.z*m.m[9] + m.m[13],
		f.x*m.m[2] + f.y*m.m[6] + f.z*m.m[10]+ m.m[14]);
}
float4 mul(const float4& f, const matrix& m)
{
	return float4(
		f.x*m.m[0] + f.y*m.m[4] + f.z*m.m[8] + f.w*m.m[12],
		f.x*m.m[1] + f.y*m.m[5] + f.z*m.m[9] + f.w*m.m[13],
		f.x*m.m[2] + f.y*m.m[6] + f.z*m.m[10]+ f.w*m.m[14],
		f.x*m.m[3] + f.y*m.m[7] + f.z*m.m[11]+ f.w*m.m[15]);
}

matrix transpose(const matrix& m)
{
	matrix t = {{
		m.m[0], m.m[4], m.m[8], m.m[12],
		m.m[1], m.m[5], m.m[9], m.m[13],
		m.m[2], m.m[6], m.m[10], m.m[14],
		m.m[3], m.m[7], m.m[11], m.m[15]
	}};
	return t;
}

#endif