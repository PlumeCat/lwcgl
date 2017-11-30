#ifndef _CUBE_GAME_H
#define _CUBE_GAME_H


class Config
{
    map<string, string> settings;
    friend class Game;
    
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    ~Config() {}
    Config& operator = (const Config&) = delete;
    Config& operator = (Config&&) = delete;

public:
    Config(const string& fname)
    {
        vector<string> lines;
        readTextFile(fname, lines);

        cout << "Reading config file" << lines.size() << endl;

        for (auto& line : lines)
        {
            auto eq = line.find('=');
            if (eq == string::npos)
                continue;

            string before = strip(line.substr(0, eq));
            string after = strip(line.substr(eq+1, line.size()));

            cout << "CONFIG: {" + before + "} = {" + after + "}" << endl;

            settings[before] = after;

        }
    }
    string get(const string& key, const string& def)
    {
        auto iter = settings.find(key);
        return (iter == settings.end()) ? def : iter->second;
    }
};

class Input
{
    friend class Game;

    bool oldKeys[GLFW_KEY_LAST] = { false };
    bool newKeys[GLFW_KEY_LAST] = { false };
    bool oldMouse[GLFW_MOUSE_BUTTON_LAST] = { false };
    bool newMouse[GLFW_MOUSE_BUTTON_LAST] = { false };
    float2 oldMousePos;
    float2 newMousePos;
    float2 oldScroll;
    float2 newScroll;

public:
    bool keyPressed(int key)    const { return newKeys[key] && !oldKeys[key]; }
    bool keyReleased(int key)   const { return oldKeys[key] && !newKeys[key]; }
    bool keyDown(int key)       const { return newKeys[key]; }
    bool mousePressed(int but)  const { return newMouse[but] && !oldMouse[but]; }
    bool mouseReleased(int but) const { return oldMouse[but] && !newMouse[but]; }
    bool mouseDown(int but)     const { return newMouse[but]; }
    float2 mousePos()   const { return newMousePos; }
    float2 mouseMove()  const { return newMousePos - oldMousePos; }

    void update()
    {
        memcpy(oldKeys, newKeys, sizeof(bool) * GLFW_KEY_LAST);
        memcpy(oldMouse, newMouse, sizeof(bool) * GLFW_MOUSE_BUTTON_LAST);
        oldMousePos = newMousePos;
        oldScroll = newScroll;

        glfwPollEvents();
    }
};


#include "gamestate.h"

class Game
{
    GLFWwindow* window = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;
    bool shouldExit = false;
    int devicePixelRatio = 2;
    GameState* currentState = nullptr;

    int initWindow()
    {
        if (!glfwInit())
        {
            glfwTerminate();
            return 0;
        }

        // TODO: enable fullscreen
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // TODO: disable this

        // load graphics options file
        int width = atoi(config->get("display_width", "1280").c_str());
        int height = atoi(config->get("display_height", "720").c_str());

        window = glfwCreateWindow(width, height, "Cube", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return 0;
        }

        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwSetWindowUserPointer(window, this);

        return 1;
    }
    int initGraphics()
    {
        glfwMakeContextCurrent(window);
        glewInit();

        GLint dims[4];
        glGetIntegerv(GL_VIEWPORT, dims);

        cout << "Display: " << dims[2] << "x" << dims[3] << endl;
        cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
        cout << "Version: " << glGetString(GL_VERSION) << endl;
        cout << "Shader: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glDepthFunc(GL_LESS);

        return 1;
    }
    int initInput()
    {
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS)
                game->input.newKeys[key] = true;
            if (action == GLFW_RELEASE)
                game->input.newKeys[key] = false;
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            if (action == GLFW_PRESS)
                game->input.newMouse[button] = true;
            if (action == GLFW_RELEASE)
                game->input.newMouse[button] = false;
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            game->input.newMousePos.x = x;
            game->input.newMousePos.y = y;
            // game->input.newMousePos.x = min(max(0, game->input.newMousePos.x), game->windowWidth);
            // game->input.newMousePos.y = min(max(0, game->input.newMousePos.y), game->windowWidth);
        });
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xscr, double yscr) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            game->input.newScroll.x = xscr;
            game->input.newScroll.y = yscr;
        });
        
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        return 1;
    }

protected:
    GLFWwindow* getWindow() { return window; }

    void logGlError()
    {
        while (true)
        {
            auto err = glGetError();
            if (err)
            {
                string msg;
                switch (err)
                {
                    case GL_INVALID_ENUM: msg = "invalid enum"; break;
                    case GL_INVALID_VALUE: msg = "invalid value"; break;
                    case GL_INVALID_OPERATION: msg = "invalid operation"; break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "invalid fbo op"; break;
                    case GL_OUT_OF_MEMORY: msg = "oom"; break;
                    case GL_STACK_OVERFLOW: msg = "stackoverflow"; break;
                    case GL_STACK_UNDERFLOW: msg = "stack underflow"; break;
                }
                cout << "get error: " << msg << endl;
            }
            else
            {
                break;
            }
        }
    }

    int init()
    {
        if (!initWindow())
            return 1;
        if (!initGraphics())
            return 2;
        if (!initInput())
            return 3;

        glfwSetErrorCallback([](int err, const char* msg) {
            cout << "GLFW Error: " << err << " - " << msg << endl;
            ::exit(5);
        });

        shaders = new ShaderManager();
        textures = new TextureManager();
        meshes = new MeshManager();

        logGlError();
        cout << "starting main loop" << endl;

        currentState->init();
    }
    void update()
    {
        input.update();
        currentState->update();
    }
    void render()
    {
        glClearColor(0.25, 0.4, 0.8, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        currentState->render();

        glfwSwapBuffers(window);
        logGlError();
    }
    void close()
    {
        currentState->close();
        delete currentState;

        delete meshes;
        delete textures;
        delete shaders;
        delete config;
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void exitGame()
    {
        shouldExit = true;
    }

public:
    Input input;
    ShaderManager* shaders = nullptr;
    TextureManager* textures = nullptr;
    MeshManager* meshes = nullptr;
    Config* config = nullptr;

    void exit()
    {
        shouldExit = true;
    }

    void setState(GameState* state)
    {
        currentState->close();
        delete currentState;
        
        currentState = state;
        currentState->game = this;
        currentState->init();
    }

    int run(GameState* state)
    {
        config = new Config(string(RESOURCE_BASE) + "/options.txt");

        currentState = state;
        currentState->game = this;
        
        if (!init())
            return 1;

        while (!glfwWindowShouldClose(window) && !shouldExit)
        {
            update();
            // TODO: separate thread
            render();

        }

        close();

        return 0;
    }
};

#endif