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
        camera.update();

        shader = shaders->getShader("cubevs.glsl", "cubeps.glsl");

        MeshBuilder builder;
        builder.addBox({-1, -1, -1}, {1, 1, 1}, {0, 0}, {1, 1}, 0);
        mesh = builder.end(shaders->getVertexAttrs("mesh_vertex"));
    }
    void update()
    {

    }
    void render()
    {
        glClearColor(0.25, 0.4, 0.8, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        matrix transform = matrix::identity();

        shader->bind();
        shader->set("World", transform);
        shader->set("View", camera.view);
        shader->set("Proj", camera.proj);
        mesh->render();
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