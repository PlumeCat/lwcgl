#ifndef _CUBE_CAMERA_H
#define _CUBE_CAMERA_H

#include "../math3d.h"

struct Camera
{
    float3 position = float3(0, 0, 0);
    float3 target = float3(0, 0, 0);
    float3 up = float3(0, 1, 0);

    float angle = pi / 4;
    float aspect = 1.65;
    float znear = 1;
    float zfar = 10000;

    matrix view;
    matrix proj;

    void update()
    {
        view = matrix::lookAt(position, target, up);
        proj = matrix::perspective(angle, aspect, znear, zfar);
    }
};

#endif