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
#include "collision.h"

#define __openbrace__ {
#define __closebrace__ }
#define DEFINE_STATE_BEGIN(cls) class cls : public GameState __openbrace__ void init(); void update(); void render(); void close();
#define DEFINE_STATE_END __closebrace__;

class PlaneGameMenu : public GameState
{
    SpriteFont* font = nullptr;
    Sprite* sprite = nullptr;

    struct MenuItem
    {
        string text;
        float2 position;
        float2 extent = float2(150, 32);
        bool focus = false;
    };
    vector<MenuItem> menu;

    void init();
    void update();
    void render();
    void close();
};

class PlaneGameSettings : public GameState
{
    SpriteFont* font = nullptr;
    Sprite* sprite = nullptr;

    void init();
    void update();
    void render();
    void close();
};

class PlaneGame : public GameState
{
    struct Player {
        float3 position;
        matrix transform;
    };
    struct Bullet{
        float3 position;
        matrix transform;
    };
    struct Enemy {
        float3 position;
        matrix transform;
    };
    struct Wall {
        float3 position;
        float3 extent;
    };

    Camera camera;
    Player player;
    vector<Bullet> bullets;
    vector<Enemy> enemies;
    vector<Wall> walls;

    Mesh* bulletMesh;
    Mesh* wallMesh;
    Mesh* shipMesh;

    SpriteFont* font = nullptr;
    Sprite* sprite = nullptr;
    Shader* meshShader = nullptr;

    void init();
    void update();
    void render();
    void close();

};

void PlaneGameMenu::init()
{
    font = game->textures->getFont("Futura-60 (3)");
    sprite = new Sprite(game->shaders);

    menu.push_back({ "start game", float2(200, 256)});
    menu.push_back({ "settings", float2(200, 288)});
    menu.push_back({ "exit", float2(200, 320)});
}
void PlaneGameMenu::update()
{
    float2 mouse = game->input.mousePos();
    float2 mouseMove = game->input.mouseMove();

    if (game->input.keyPressed(GLFW_KEY_ESCAPE))
    {
        game->exit();
    }

    // focused item
    int focus = -1;

    for (int i = 0; i < menu.size(); i++)
    {
        auto& item = menu[i];
        if (item.focus)
            focus = i;
        item.focus = false;
    }

    if (abs(mouseMove.x) > 0.01 && abs(mouseMove.y) > 0.01)
    {
        focus = -1;
        for (int i =0 ; i < menu.size(); i++)
        {
            auto& item = menu[i];
            if (intersect(Rect(item.position+item.extent/2.f, item.extent/2.f), mouse))
            {
                item.focus = true;
                focus = i;
                break;
            }
        }
    }
    else
    {
        if (game->input.keyPressed(GLFW_KEY_UP))
        {
            focus = max(focus-1, 0);
        }
        if (game->input.keyPressed(GLFW_KEY_DOWN))
        {
            focus = min(focus+1, (int)menu.size()-1);
        }
    }

    menu[focus].focus = true;

    if (game->input.keyPressed(GLFW_KEY_ENTER))
    {
        switch (focus)
        {
            case 0:
                game->setState(new PlaneGame);
                break;
            case 1:
                game->setState(new PlaneGameSettings);
                break;
            case 2:
                game->exit();
                break;
            default:
                break;
        }
    }
}
void PlaneGameMenu::render()
{
    sprite->drawText(font, "dogfight game", float2(200, 200), {1, 1}, {0.75, 0.75, 0, 1});
    for (auto& item : menu)
    {
        float4 def(0.7, 0.7, 0.7, 1);
        float4 focus(0.9, 0.9, 0.9, 1);
        sprite->drawText(font, item.text, item.position, {1,1}, item.focus ? focus : def);
    }

}
void PlaneGameMenu::close()
{
    delete sprite;
}


void PlaneGameSettings::init()
{
    font = game->textures->getFont("Futura-60 (3)");
    sprite = new Sprite(game->shaders);
}
void PlaneGameSettings::update()
{
    if (game->input.keyPressed(GLFW_KEY_ESCAPE))
    {
        game->setState(new PlaneGameMenu);
    }
}
void PlaneGameSettings::render()
{
    sprite->drawText(font, "settings", float2(200, 200), {1, 1}, {0.75, 0.75, 0, 1});
}
void PlaneGameSettings::close() {}

void PlaneGame::init()
{
    font = game->textures->getFont("Futura-60 (3)");
    sprite = new Sprite(game->shaders);

    for (int i = 0; i < 10; i++)
    {
        Wall wall;
        wall.position = float3(
            uniform(-1000, 1000),
            uniform(-1000, 1000),
            uniform(-1000, 1000)
        );
        wall.extent = float3(
            uniform(0, 100),
            uniform(0, 100),
            uniform(0, 100)
        );
        walls.push_back(wall);
    }

    meshShader = game->shaders->getShader("meshvs.glsl", "meshps.glsl");

    MeshBuilder builder;
    builder.box(float3(-1, -1, -1), float3(1, 1, 1), {0,0}, {1,1});
    wallMesh = builder.end(game->shaders->getVertexAttrs("mesh_vertex"));
}

void PlaneGame::update()
{
    if (game->input.keyPressed(GLFW_KEY_ESCAPE))
    {
        game->setState(new PlaneGameMenu);
    }
}
void PlaneGame::render()
{

    camera.position = float3(2000, 2000, 2000);
    camera.target = float3(0, 0, 0);
    camera.up = float3(0, 1, 0);
    camera.znear = 1;
    camera.zfar = 3000;
    camera.angle = pi / 3.f;
    camera.aspect = 1.6f;
    camera.update();

    meshShader->bind();
    meshShader->set("View", camera.view);
    meshShader->set("Proj", camera.proj);

    for (auto& w : walls)
    {
        meshShader->set("World", matrix::scale(w.extent) * matrix::translation(w.position));
        wallMesh->render();
    }
    
    
    sprite->drawText(font, "dogfight game", float2(200, 200), {1, 1}, {0.75, 0.75, 0, 1});
    stringstream str;
    str << "position: " << player.position.x << ", " << player.position.y << ", " << player.position.z;
    sprite->drawText(font, str.str(), float2(0, 0), float2(0.5, 0.5));
}
void PlaneGame::close() {}





int main()
{
    srand(time(nullptr));
    Game* game = new Game();
    game->run(new PlaneGameMenu);
    delete game;
    return 0;
}