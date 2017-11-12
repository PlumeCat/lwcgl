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
    float tilt = 0;
    int score = 0;

    void spawnCube()
    {
        cubes.push_back(float3(player.x + uniform(-20, 20), 0, 99));
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

        // update player
        score++;
        if (input.keyDown(GLFW_KEY_RIGHT))
        {
            tilt += (0.15 - tilt) / 20.f;
            player.x -= 0.24;
        }
        if (input.keyDown(GLFW_KEY_LEFT))
        {
            tilt += (-0.15 - tilt) / 20.f;
            player.x += 0.24;
        }
        tilt -= tilt / 20.f;

        // update camera
        camera.up = float3(sin(tilt), cos(tilt), 0);
        camera.target = player + camera.up * 5.f;
        camera.position = player + float3(0, 0, -20) + camera.up * 10.f;
        camera.update();
    }
    void render()
    {
        f += 0.01;

        shader->bind();
        shader->set("View", camera.view);
        shader->set("Proj", camera.proj);

        // draw floor
        shader->set("Colour", float4(0.6, 0.6, 0.6, 1));
        shader->set("World", matrix::translation(0, -1, 0) * matrix::scale(200, 1, 200));
        mesh->render();

        matrix shadow = {{
            1, 0, 0, 0,
            -1.5, 0, -0.5, 0,
            0, 0, 1, 0,
            0, 0.01, 0, 1,
        }};
        
        // draw player
        shader->set("Colour", float4(1, 0, 0, 1));
        shader->set("World", matrix::translation(player));
        mesh->render();
        shader->set("Colour", float4(0, 0, 0, 0.25));
        shader->set("World", shadow * matrix::translation(player));
        mesh->render();

        // draw cubes
        for (auto& c : cubes)
        {
            shader->set("World", matrix::translation(c));
            shader->set("Colour", float4(0.4, 0.7, 0.3, 1));
            mesh->render();

            shader->set("World", matrix::translation(c+float3(0, 0.1, 0)) * shadow);
            shader->set("Colour", float4(0,0,0,0.25));
            mesh->render();
        }

        stringstream scorestr;
        scorestr << "Score: " << score;

        sprite->drawText(font, scorestr.str(), {-635, 320});
        
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