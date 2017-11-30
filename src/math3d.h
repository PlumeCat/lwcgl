#ifndef _CUBE_MATH3D_H
#define _CUBE_MATH3D_H

#include <cmath>

template <typename field, int dim> struct vec
{
    field _x[dim];

    vec()
    {
        for (int i = 0; i < dim; i++) { _x[i] = field(0); }
    }
    vec(const vec<field,dim>& v)
    {
        memcpy(_x, v._x, sizeof(field)*dim);
    }
    vec<field,dim>& operator = (const vec<field,dim>& v)
    {
        memcpy(_x, v._x, sizeof(field)*dim);
    }
};

#define _(d) for (int i = 0; i < d; i++)
// TODO: vectorise this for small d
template<typename f,int d> vec<f, d>& operator += (vec<f,d>& f1, const vec<f,d>& f2) { _(d) { f1._x[i] += f2._x[i]; } return f1; }
template<typename f,int d> vec<f, d>& operator -= (vec<f,d>& f1, const vec<f,d>& f2) { _(d) { f1._x[i] -= f2._x[i]; } return f1; }
template<typename f,int d> vec<f, d>& operator *= (vec<f,d>& f1, const vec<f,d>& f2) { _(d) { f1._x[i] *= f2._x[i]; } return f1; }
template<typename f,int d> vec<f, d>& operator /= (vec<f,d>& f1, const vec<f,d>& f2) { _(d) { f1._x[i] /= f2._x[i]; } return f1; }
template<typename f,int d> vec<f, d>& operator *= (vec<f,d>& f1, const f& ff)        { _(d) { f1._x[i] *= ff; }       return f1; }
template<typename f,int d> vec<f, d>& operator /= (vec<f,d>& f1, const f& ff)        { _(d) { f1._x[i] /= ff; }       return f1; }

template<typename f,int d> vec<f,d> operator + (const vec<f,d>& v1, const vec<f,d>& v2) { vec<f,d> r; _(d) { r._x[i] = v1._x[i] + v2._x[i]; } return r; }
template<typename f,int d> vec<f,d> operator - (const vec<f,d>& v1, const vec<f,d>& v2) { vec<f,d> r; _(d) { r._x[i] = v1._x[i] - v2._x[i]; } return r; }
template<typename f,int d> vec<f,d> operator * (const vec<f,d>& v1, const vec<f,d>& v2) { vec<f,d> r; _(d) { r._x[i] = v1._x[i] * v2._x[i]; } return r; }
template<typename f,int d> vec<f,d> operator / (const vec<f,d>& v1, const vec<f,d>& v2) { vec<f,d> r; _(d) { r._x[i] = v1._x[i] / v2._x[i]; } return r; }
template<typename f,int d> vec<f,d> operator * (const vec<f,d>& v1, const f& ff)        { vec<f,d> r; _(d) { r._x[i] = v1._x[i] * ff; } return r; }
template<typename f,int d> vec<f,d> operator * (const f& ff,        const vec<f,d>& v1) { vec<f,d> r; _(d) { r._x[i] = v1._x[i] * ff; } return r; }
template<typename f,int d> vec<f,d> operator / (const vec<f,d>& v1, const f& ff)        { vec<f,d> r; _(d) { r._x[i] = v1._x[i] / ff; } return r; }
template<typename f,int d> vec<f,d> operator - (const vec<f,d>& v)                      { vec<f,d> r; _(d) { r._x[i] = -v._x[i]; } return r; }

#undef _


template<> struct vec<float, 2>
{
    union
    {
        float _x[2];
        struct { float x, y; };
    };

    vec()
    {
        x = 0;
        y = 0;
    }
    vec(const vec<float, 2>& v)
    {
        x = v.x;
        y = v.y;
    }
    vec(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};
template<> struct vec<int, 2>
{
    union
    {
        int _x[2];
        struct { int x, y; };
    };

    vec()
    {
        x = 0;
        y = 0;
    }
    vec(const vec<int, 2>& v)
    {
        x = v.x;
        y = v.y;
    }
    vec(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
};
template<> struct vec<float, 3>
{
    union
    {
        float _x[3];
        struct { float x, y, z; };
    };

    vec()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    vec(const vec<float, 3>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    vec(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
};
template<> struct vec<int, 3>
{
    union
    {
        int _x[3];
        struct { int x, y, z; };
    };

    vec()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    vec(const vec<int, 3>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    vec(int _x, int _y, int _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }
};
template<> struct vec<float, 4>
{
    union
    {
        float _x[4];
        struct { float x, y, z, w; };
    };

    vec()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }
    vec(const vec<float,4>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }
    vec(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
};
template<> struct vec<int, 4>
{
    union
    {
        int _x[4];
        struct { int x, y, z, w; };
    };

    vec()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }
    vec(const vec<int, 4>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }
    vec(int _x, int _y, int _z, int _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
};

typedef vec<float, 2> float2;
typedef vec<float, 3> float3;
typedef vec<float, 4> float4;

typedef vec<int, 2> int2;
typedef vec<int, 3> int3;
typedef vec<int, 4> int4;

template<typename f, int d> f 
dot(const vec<f,d>& v1, const vec<f,d>& v2)
{
    f r(0);
    for (int i = 0; i < d; i++) r += v1._x[i] * v2._x[i];
    return r;
}
template<typename f, int d> f 
len(const vec<f,d>& v)
{
    return sqrt(dot(v, v));
}
template <typename f, int d> f 
len2(const vec<f, d>& v)
{
    return dot(v, v);
}
template <typename f, int d> 
vec<f,d> normalize(const vec<f, d>& v)
{
    return v / len(v);
}
template <typename f, int d> 
vec<f,d> lerp(const vec<f,d>& v1, const vec<f,d>& v2, const f& t)
{
    return v1 * (f(1) - t) + v2 * t;
}

// cross product for 3-vectors
template <typename f> vec<f,3> 
cross(const vec<f, 3>& u, const vec<f, 3>& v)
{
    return float3(
        u._x[1]*v._x[2] - u._x[2]*v._x[1],
        u._x[2]*v._x[0] - u._x[0]*v._x[2],
        u._x[0]*v._x[1] - u._x[1]*v._x[0]
    );
}



struct matrix
{
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    matrix() = default;
    matrix(const matrix& m_) = default;
    // matrix(float (&m_)[16]) { memcpy(m, m_, 16); }
    static matrix identity() { return matrix(); }

    matrix& operator *= (const matrix& m1)
    {
        float4 r1(m[0], m[1], m[2], m[3]);
        float4 r2(m[4], m[5], m[6], m[7]);
        float4 r3(m[8], m[9], m[10],m[11]);
        float4 r4(m[12],m[13],m[14],m[15]);

        float4 c1(m1.m[0], m1.m[4], m1.m[8], m1.m[12]);
        float4 c2(m1.m[1], m1.m[5], m1.m[9], m1.m[13]);
        float4 c3(m1.m[2], m1.m[6], m1.m[10],m1.m[14]);
        float4 c4(m1.m[3], m1.m[7], m1.m[11],m1.m[15]);

        float _m[] = {
            dot(r1,c1),dot(r1,c2),dot(r1,c3),dot(r1,c4),
            dot(r2,c1),dot(r2,c2),dot(r2,c3),dot(r2,c4),
            dot(r3,c1),dot(r3,c2),dot(r3,c3),dot(r3,c4),
            dot(r4,c1),dot(r4,c2),dot(r4,c3),dot(r4,c4),
        };
        memcpy(m, _m, 64);
        return *this;
    }

    float3 left()    { return { m[0], m[1], m[2] }; }
    float3 up()      { return { m[4], m[5], m[6] }; }
    float3 forward() { return { m[8], m[9], m[10] }; }

    // linear transformations
    static matrix translation(const float3& f)
    {
        return {{
            1,  0,  0,  0,
            0,  1,  0,  0,
            0,  0,  1,  0,
            f.x,f.y,f.z,1
        }};
    }
    static matrix translation(float x, float y, float z)
    {
        return {{
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            x,y,z,1,
        }};
    }
    static matrix scale(const float3& f)
    {
        return {{
            f.x,0,  0,  0,
            0,  f.y,0,  0,
            0,  0,  f.z,0,
            0,  0,  0,  1
        }};
    }
    static matrix scale(float x, float y, float z)
    {
        return {{
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        }};
    }
    static matrix rotateX(float r)
    {
        float c = cos(r);
        float s = sin(r);
        return {{
            1, 0, 0, 0,
            0, c,-s, 0,
            0, s, c, 0,
            0, 0, 0, 1
        }};
    }
    static matrix rotateY(float r)
    {
        float c = cos(r);
        float s = sin(r);
        return {{
            c, 0, s, 0,
            0, 1, 0, 0,
            -s,0, c, 0,
            0, 0, 0, 1
        }};
    }
    static matrix rotateZ(float r)
    {
        float c = cos(r);
        float s = sin(r);
        return {{
            c,-s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }};
    }
    static matrix rotateAxis(const float3& axis, float r)
    {
        float3 u = normalize(axis);

        float c = cos(r);
        float rc = 1-c;
        float s = sin(r);

        return {{
            c + u.x*u.x*rc,    u.x*u.y*rc-u.z*s,  u.x*u.z*rc+u.y*s,  0,
            u.y*u.x*rc+u.z*s,  c+u.y*u.y*rc,      u.y*u.z*rc-u.x*s,  0,
            u.z*u.x*rc-u.y*s,  u.z*u.y*rc+u.x*s,  c+u.z*u.z*rc,      0,
            0,                 0,                 0,                 1
        }};

    }
    static matrix lookAt(const float3& pos, const float3& target, const float3& up)
    {
        float3 az = normalize(pos - target);
        float3 ax = normalize(cross(up, az));
        float3 ay = cross(az, ax);

        float xp = -dot(ax, pos);
        float yp = -dot(ay, pos);
        float zp = -dot(az, pos);

        return {{
            ax._x[0], ay._x[0], az._x[0], 0,
            ax._x[1], ay._x[1], az._x[1], 0,
            ax._x[2], ay._x[2], az._x[2], 0,
            xp,   yp,   zp,   1
        }};
    }

    // projections
    static matrix ortho(float w, float h, float zn, float zf)
    {
        float d = zf - zn;
        float zb = -(zn + zf) / (d);

        return {{
            2/w,0,  0,  0,
            0,  2/h,0,  0,
            0,  0, -2/d,0,
            0,  0,  zb, 1,
        }};
    }
    static matrix ortho(float l, float r, float t, float b, float zn, float zf)
    {
        float d = zf - zn;
        float zb = -(zn + zf) / d;
        float x = l + r;
        float y = t + b;

        float w = l - r;
        float h = t - b;

        return {{
            -2/w, 0, 0, 0,
            0, -2/h, 0, 0,
            0, 0, 1/d, 0,
            x/w, y/h, zn/d, 1
        }};
    }
    static matrix perspective(float angle, float aspect, float zn, float zf)
    {
        float d = zf - zn;

        float ys = 1.f / tan(angle / 2.f);
        float xs = ys / aspect;
        float zs = -(zf + zn) / d;
        float zb = -2 * zf * zn / d;

        return {{
            xs, 0, 0, 0,
            0, ys, 0, 0,
            0, 0, zs, -1,
            0, 0, zb, 0,
        }};
    }
};

matrix operator * (const matrix& m1, const matrix& m2)
{
    float4 r1(m1.m[0], m1.m[1], m1.m[2], m1.m[3]);
    float4 r2(m1.m[4], m1.m[5], m1.m[6], m1.m[7]);
    float4 r3(m1.m[8], m1.m[9], m1.m[10],m1.m[11]);
    float4 r4(m1.m[12],m1.m[13],m1.m[14],m1.m[15]);

    float4 c1(m2.m[0], m2.m[4], m2.m[8], m2.m[12]);
    float4 c2(m2.m[1], m2.m[5], m2.m[9], m2.m[13]);
    float4 c3(m2.m[2], m2.m[6], m2.m[10],m2.m[14]);
    float4 c4(m2.m[3], m2.m[7], m2.m[11],m2.m[15]);

    return {{
        dot(r1,c1),dot(r1,c2),dot(r1,c3),dot(r1,c4),
        dot(r2,c1),dot(r2,c2),dot(r2,c3),dot(r2,c4),
        dot(r3,c1),dot(r3,c2),dot(r3,c3),dot(r3,c4),
        dot(r4,c1),dot(r4,c2),dot(r4,c3),dot(r4,c4),
    }};
}

matrix transpose(const matrix& m)
{
    return {{
        m.m[0], m.m[1], m.m[2], m.m[3],
        m.m[4], m.m[5], m.m[6], m.m[7],
        m.m[8], m.m[9], m.m[10],m.m[11],
        m.m[12],m.m[13],m.m[14],m.m[15],
    }};
}

float2 mul(const matrix& m, const float2& f)
{
    float4 r1(m.m[0], m.m[1], m.m[2], m.m[3]);
    float4 r2(m.m[4], m.m[5], m.m[6], m.m[7]);

    float4 c1(f._x[0], f._x[1], 0, 1);

    return {
        dot(r1, c1),
        dot(r2, c1),
    };
}
float3 mul(const matrix& m, const float3& f)
{
    float4 r1(m.m[0], m.m[4], m.m[8], m.m[12]);
    float4 r2(m.m[1], m.m[5], m.m[9], m.m[13]);
    float4 r3(m.m[2], m.m[6], m.m[10],m.m[14]);
    float4 r4(m.m[3], m.m[7], m.m[11],m.m[15]);

    float4 c1(f._x[0], f._x[1], f._x[2], 1);

    return {
        dot(r1, c1),
        dot(r2, c1),
        dot(r3, c1),
        // dot(r4, c1),
    };
}
float4 mul(const matrix& m, const float4& f)
{
    float4 r1(m.m[0], m.m[4], m.m[8], m.m[12]);
    float4 r2(m.m[1], m.m[5], m.m[9], m.m[13]);
    float4 r3(m.m[2], m.m[6], m.m[10],m.m[14]);
    float4 r4(m.m[3], m.m[7], m.m[11],m.m[15]);

    return {
        dot(r1, f),
        dot(r2, f),
        dot(r3, f),
        dot(r4, f),
    };
}

#endif