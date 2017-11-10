#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <thread>

using namespace std;

#include "math3d.h"
#include "definitions.h"

#include "graphics/buffer.h"
#include "graphics/texture.h"
#include "graphics/surface.h"
#include "graphics/shader.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/sprite.h"

#include "game.h"

class CubeGame : public Game
{
    Camera camera;
    Mesh* mesh;
    Shader* shader;

    void init()
    {
        camera.position = float3(10, 10, 10);
        camera.target = float3(0, 0, 0);
        camera.up = float3(0, 1, 0);

        shader = shaders->getShader("cubevs.glsl", "cubeps.glsl");

        MeshBuilder builder(shaders->getVertexAttrs("mesh_vertex"));
        builder.addBox({-1, -1, -1}, {1, 1, 1}, {0, 0}, {1, 1}, 0);
        mesh = builder.end();
    }
    void update()
    {

    }
    void render()
    {

    }
    void close()
    {

    }
};

int main()
{
    srand(time(nullptr));
    return CubeGame().run();
}