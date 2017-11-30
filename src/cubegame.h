#ifndef _CUBEGAME_H
#define _CUBEGAME_H


class CubeGame : public GameState
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

        shader = game->shaders->getShader("cubevs.glsl", "cubeps.glsl");
        font = game->textures->getFont("Futura-60");

        MeshBuilder builder;
        builder.box({-0.5, 0, -0.5}, {0.5, 1, 0.5}, {0, 0}, {1, 1}, 0);
        mesh = builder.end(game->shaders->getVertexAttrs("mesh_vertex"));

        sprite = new Sprite(game->shaders);

        spawnCube();
        spawnCube();
    }
    void update()
    {
        // update cubes
        for (auto cube = cubes.begin(); cube != cubes.end(); )
        {
            cube->z -= cubeSpeed;
            float xdist = abs(player.x - cube->x);
            float zdist = abs(player.z - cube->z);
            if (xdist < 1 && zdist < 1)
            {
                // game over!
                score = 0;
                cubes.clear();
                break;
            }

            if (cube->z < -10)
            {
                cube = cubes.erase(cube);
            }
            else
            {
                cube++;
            }
        }
        if (cubeSpeed < 1)
        {
            cubeSpeed *= 1.001;
        }

        // spawn some more cubes
        if (uniform(0, 0.2/cubeSpeed) < 0.1)
        {
            spawnCube();
        }

        // update player
        score++;
        if (game->input.keyDown(GLFW_KEY_RIGHT))
        {
            tilt += (0.15 - tilt) / 20.f;
            player.x -= 0.24;
        }
        if (game->input.keyDown(GLFW_KEY_LEFT))
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
        shader->set("World", matrix::scale(200, 1, 200) * matrix::translation(player.x, -1, 0));
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
        sprite->drawText(font, (stringstream("cubes: ") << cubes.size()).str(), {-635, 300});
        
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

#endif