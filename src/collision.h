#ifndef _CUBE_COLLISION_H
#define _CUBE_COLLISION_H

struct Circle
{
    float2 centre;
    float radius;

    Circle() {}
    Circle(const float2& _centre, float _r) : centre(_centre), radius(_r) {}
    Circle(const Circle& c) : centre(c.centre), radius(c.radius) {}
};
struct Rect
{
    float2 centre;
    float2 extent;

    Rect() {}
    Rect(const float2& _centre, const float2& _extent) : centre(_centre), extent(_extent) {}
    Rect(const Rect& b) : centre(b.centre), extent(b.extent) {}
};

bool intersect(const Circle& circle, const Rect& box)
{
    return false;
}
bool intersect(const Circle& circle, const float2& point)
{
    return false;
}
bool intersect(const Rect& r, const float2& point)
{
    float2 d = r.centre - point;
    return (abs(d.x) < abs(r.extent.x)) && (abs(d.y) < abs(r.extent.y));
}
bool intersect(const Rect& r1, const Rect& r2)
{
    return false;
}
bool intersect(const Circle& c1, const Circle& c2)
{
    return false;
}

struct Ray
{
    float3 origin;
    float3 direction;

    Ray() {}
    Ray(const float3& orig, const float3& dir) : origin(orig), direction(dir) {}
    Ray(const Ray& r) : origin(r.origin), direction(r.direction) {}
};
struct Box
{
    float3 centre;
    float3 extent;
    Box() {}
    Box(const float3& _centre, const float3& _extent) : centre(_centre), extent(_extent) {}
    Box(const Box& b) : centre(b.centre), extent(b.extent) {}
};
struct Sphere
{
    float3 centre;
    float radius;

    Sphere() {}
    Sphere(const float3& _centre, float _r) : centre(_centre), radius(_r) {}
    Sphere(const Sphere& s) : centre(s.centre), radius(s.radius) {}
};

bool intersect(const Ray& r1, const Sphere& s, float& outDist)
{
    return false;
}
bool intersect(const Ray& r1, const Box& b, float& outDist)
{
    return false;
}
bool intersect(const Sphere& s, const Box& b)
{
    return false;
}
bool intersect(const Sphere& s1, const Sphere& s2)
{
    return false;
}
bool intersect(const Box& b1, const Box& b2)
{
    return false;
}
bool intersect(const Sphere& s, const float3& pos)
{
    return false;
}
bool intersect(const Box& b, const float3& pos)
{
    return false;
}

#endif