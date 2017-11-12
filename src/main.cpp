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

// inplace-filter a C++ list
template<typename Type, typename Predicate>
void filter(std::vector<Type>& v, Predicate p)
{
    auto i = v.begin();
    while (i != v.end())
    {
        if (!p(*i))
        {
            i = v.erase(i);
        }
        else
        {
            i++;
        }
    }
}

class CubeGame : public Game
{
    Camera camera;
    Mesh* mesh;
    Shader* shader;
    Sprite* sprite;
    SpriteFont* font;
    float f = 0;

    vector<float3> cubes;
    float3 player;
    float cubeSpeed = 0.5;
    float cameraTilt = 0;

    void spawnCube()
    {
        cubes.push_back(float3(player.x + uniform(-20, 20), 0, 45));
    }

    void init()
    {
        camera.position = float3(0, 10, -20);
        camera.target = float3(0, 5, 0);
        camera.up = float3(0, 1, 0);
        camera.update();

        shader = shaders->getShader("cubevs.glsl", "cubeps.glsl");
        font = textures->getFont("Futura-60");

        MeshBuilder builder;
        builder.addBox({-0.5, 0, -0.5}, {0.5, 1, 0.5}, {0, 0}, {1, 1}, 0);
        mesh = builder.end(shaders->getVertexAttrs("mesh_vertex"));

        sprite = new Sprite(shaders);

        // set up player
        player.x = 0;
        player.y = 0;
        player.z = 0;

        spawnCube();
        spawnCube();
    }
    void update()
    {
        // move all cubes towards the player
        for (auto& cube : cubes)
        {
            cube.z -= cubeSpeed;
        }

        if (cubeSpeed < 1)
        {
            cubeSpeed *= 1.001;
        }

        // remove cubes that are past
        filter(cubes, [](const float3& cube) {
            return cube.z > -10;
        });

        // spawn some more cubes
        if (uniform(0, 0.2/cubeSpeed) < 0.1)
        {
            spawnCube();
        }
    }
    void render()
    {
        f += 0.01;

        shader->bind();
        shader->set("View", camera.view);
        shader->set("Proj", camera.proj);

        // draw floor
        shader->set("Colour", float4(0.6, 0.6, 0.6, 1));
        shader->set("World", matrix::translation(0, -1, 0) * matrix::scale(100, 1, 100));
        mesh->render();
        
        // draw player
        shader->set("Colour", float4(1, 0, 0, 1));
        shader->set("World", matrix::translation(player));
        mesh->render();

        // draw cubes
        shader->set("Colour", float4(0.4, 0.7, 0.3, 1));
        for (auto& c : cubes)
        {
            shader->set("World", matrix::translation(c));
            mesh->render();
        }

        sprite->drawText(font, "Score: 0", {-635, 320});
        sprite->drawText(font, (stringstream("Speed: ") << cubeSpeed).str(), {-635, 280});
        
        sprite->begin(font->texture->texture);
        sprite->addSprite({0, 0}, {100, 100});
        sprite->end();
    }
    void close()
    {
        delete sprite;
        delete mesh;
    }
};

int main()
{
    srand(time(nullptr));
    return CubeGame().run();
}