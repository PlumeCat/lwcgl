#ifndef _CUBE_GAME_H
#define _CUBE_GAME_H

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


class Game
{
    GLFWwindow* window = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;
    bool shouldExit = false;

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

        window = glfwCreateWindow(1280, 720, "Cube", nullptr, nullptr);
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

        cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
        cout << "Version: " << glGetString(GL_VERSION) << endl;
        cout << "Shader: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
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
            game->input.newMousePos.y = (float)game->windowHeight - y;
        });
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xscr, double yscr) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            game->input.newScroll.x = xscr;
            game->input.newScroll.y = yscr;
        });

        return 1;
    }

protected:
    GLFWwindow* getWindow() { return window; }
    void exitGame()
    {
        shouldExit = true;
    }

public:
    Input input;
    ShaderManager* shaders = nullptr;
    TextureManager* textures = nullptr;
    MeshManager* meshes = nullptr;

    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void close() = 0;

    int run()
    {
        if (!initWindow())
            return 1;
        if (!initGraphics())
            return 2;
        if (!initInput())
            return 3;

        glfwSetErrorCallback([](int err, const char* msg) {
            cout << "GLFW Error: " << err << " - " << msg << endl;
            exit(5);
        });

        shaders = new ShaderManager();
        textures = new TextureManager();
        meshes = new MeshManager();

        init();

        while (!glfwWindowShouldClose(window) && !shouldExit)
        {
            input.update();
            update();

            render();
            glfwSwapBuffers(window);
        }

        close();

        delete meshes;
        delete textures;
        delete shaders;

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }
};

#endif