#include "tge.h"
#include "internal/gl_helpers.h"
#include "game.h"

const std::string SCRIPT_BASE = "..//..//..//resource//script//";
const std::string SOUND_BASE = "..//..//..//resource//sound//";

void fatal_error(const std::string& error_message);

void run_game() {
    auto game = new Game();
    game->run();
    delete game;
}

float stof_safe(std::string_view s) {
    auto value = 0.f;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec == std::errc()) { // success
        return value;
    }
    return 0.f;
}

bool safe_call(Interpreter* script, const std::string& fn) {
    try {
        script->call(script->get_global(fn), nullptr, 0);
        return true;
    } catch (std::exception& e) {
        log("Script error: "s + e.what());
        return false;
    }
}


void Game::init_window() {
    log("Initializing GLFW");
    if (!glfwInit()) {
        fatal_error("Failed to initialize GLFW");
    }

    // initialize window
    log("Creating window");
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    log("Video mode: ", mode->width, mode->height, mode->refreshRate);
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_REFRESH_RATE, 0);
    display_width = mode->width;
    display_height = mode->height;
    window = glfwCreateWindow(display_width, display_height, "TGE", 
#ifdef _DEBUG
        nullptr,
#else
        glfwGetPrimaryMonitor(),
#endif
    nullptr);
    if (!window) {
        glfwDestroyWindow(window);
        fatal_error("Failed to create window");
    }

    // Make context and load GL
    log("Setting up context");
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwShowWindow(window);
    log("Loading GL");
    if (!gladLoadGL(glfwGetProcAddress)) {
        fatal_error("Failed to load GL");
    }
    // initialize input
    // TODO: custom cursor
    cursor_arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    glfwSetErrorCallback([](int error_code, const char* error) {
        log("GLFW Error", error_code, ": ", error);
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int enter) {
        if (enter == GLFW_TRUE) {
            auto game = (Game*)glfwGetWindowUserPointer(window);
            glfwSetCursor(window, game->cursor_arrow);
        }
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        log("Framebuffer size changed: ", width, height);
        auto game = (Game*)glfwGetWindowUserPointer(window);
        if (game && game->system) {
            game->system->set("display_width", value_from_number(width));
            game->system->set("display_height", value_from_number(height));
        }
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int flags) {
        auto game = (Game*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            game->keys_new[key] = GLFW_TRUE;
        } else if (action == GLFW_RELEASE) {
            game->keys_new[key] = GLFW_FALSE;
        }
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        auto game = (Game*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS) {
            game->mouse_new[button] = true;
        } else if (action == GLFW_RELEASE) {
            game->mouse_new[button] = false;
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        auto game = (Game*)glfwGetWindowUserPointer(window);
        game->mouse_pos = { (float)x, (float)y };
    });

    /*
    // glfwSetCharCallback;
    // glfwSetCharModsCallback;
        // glfwSetCursorEnterCallback;
        // glfwSetCursorPosCallback;
    // glfwSetDropCallback;
        // glfwSetErrorCallback;
        // glfwSetFramebufferSizeCallback
    // glfwSetJoystickCallback;
        // glfwSetKeyCallback
        // glfwSetMouseButtonCallback;
    // glfwSetScrollCallback;

    // glfwSetMonitorCallback;
    // glfwSetWindowCloseCallback;
    // glfwSetWindowContentScaleCallback;
    // glfwSetWindowFocusCallback;
    // glfwSetWindowIconifyCallback;
    // glfwSetWindowMaximizeCallback;
    // glfwSetWindowPosCallback;
    // glfwSetWindowRefreshCallback;
    // glfwSetWindowSizeCallback;
    */

}
void Game::init_graphics() {
    // create buffers
    glGenBuffers(1, &vb);
    glGenBuffers(1, &ib);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (void*)8);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)12);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // initialize graphics
    default_font = load_font("Verdana-20");
    set_font(default_font);
    default_sprite = load_sprite("default");
    set_sprite(default_sprite);
    default_shader = load_shader("sprite_fs");
    set_shader(default_shader);
}
void Game::init_sound() {
    // initialize sound
    sound = irrklang::createIrrKlangDevice();
    if (!sound) {
        fatal_error("failed to initialize sound");
    }
}
void Game::init_input() {
}
void Game::init_script() {
    // initialize script
    try {
        script = new Interpreter();
        script->set_user_pointer(this);
        setup_standard_library(script);

        #define tge_func(name, body) tack_func(name, { auto game = (Game*)vm->get_user_pointer(); body; });

        // bind game
        system = script->alloc_object();
        script->set_global("Game", true, value_from_object(system));
        system->set("display_width", value_from_number(display_width));
        system->set("display_height", value_from_number(display_height));

        // bind mouse
        mouse = script->alloc_object();
        script->set_global("Mouse", true, value_from_object(mouse));
        {
            mouse->set("Button1", value_from_number(0));
            mouse->set("Button2", value_from_number(1));
            mouse->set("Button3", value_from_number(2));
            mouse->set("Button4", value_from_number(3));
            mouse->set("Button5", value_from_number(4));
            mouse->set("Button6", value_from_number(5));
            mouse->set("Button7", value_from_number(6));
            mouse->set("Button8", value_from_number(7));
            mouse->set("Left", value_from_number(0));
            mouse->set("Right", value_from_number(1));
            mouse->set("Middle", value_from_number(2));
        }

        // bind keys
        keys = script->alloc_object();
        script->set_global("Keys", true, value_from_object(keys));
        {
            keys->set("Space", value_from_number(32));
            keys->set("Apostrophe", value_from_number(39)); /* ' */
            keys->set("Comma", value_from_number(44)); /* , */
            keys->set("Minus", value_from_number(45)); /* - */
            keys->set("Period", value_from_number(46)); /* . */
            keys->set("Slash", value_from_number(47)); /* / */
            keys->set("D0", value_from_number(48));
            keys->set("D1", value_from_number(49));
            keys->set("D2", value_from_number(50));
            keys->set("D3", value_from_number(51));
            keys->set("D4", value_from_number(52));
            keys->set("D5", value_from_number(53));
            keys->set("D6", value_from_number(54));
            keys->set("D7", value_from_number(55));
            keys->set("D8", value_from_number(56));
            keys->set("D9", value_from_number(57));
            keys->set("Semicolon", value_from_number(59)); /* ; */
            keys->set("Equal", value_from_number(61)); /*, */
            keys->set("A", value_from_number(65));
            keys->set("B", value_from_number(66));
            keys->set("C", value_from_number(67));
            keys->set("D", value_from_number(68));
            keys->set("E", value_from_number(69));
            keys->set("F", value_from_number(70));
            keys->set("G", value_from_number(71));
            keys->set("H", value_from_number(72));
            keys->set("I", value_from_number(73));
            keys->set("J", value_from_number(74));
            keys->set("K", value_from_number(75));
            keys->set("L", value_from_number(76));
            keys->set("M", value_from_number(77));
            keys->set("N", value_from_number(78));
            keys->set("O", value_from_number(79));
            keys->set("P", value_from_number(80));
            keys->set("Q", value_from_number(81));
            keys->set("R", value_from_number(82));
            keys->set("S", value_from_number(83));
            keys->set("T", value_from_number(84));
            keys->set("U", value_from_number(85));
            keys->set("V", value_from_number(86));
            keys->set("W", value_from_number(87));
            keys->set("X", value_from_number(88));
            keys->set("Y", value_from_number(89));
            keys->set("Z", value_from_number(90));
            keys->set("LeftBracket", value_from_number(91)); /* [ */
            keys->set("Backslash", value_from_number(92)); /* \ */
            keys->set("RightBracket", value_from_number(93)); /* ] */
            keys->set("GraveAccent", value_from_number(96)); /* ` */
            keys->set("World1", value_from_number(161));
            keys->set("World2", value_from_number(162));
            keys->set("Escape", value_from_number(256));
            keys->set("Enter", value_from_number(257));
            keys->set("Tab", value_from_number(258));
            keys->set("Backspace", value_from_number(259));
            keys->set("Insert", value_from_number(260));
            keys->set("Delete", value_from_number(261));
            keys->set("Right", value_from_number(262));
            keys->set("Left", value_from_number(263));
            keys->set("Down", value_from_number(264));
            keys->set("Up", value_from_number(265));
            keys->set("PageUp", value_from_number(266));
            keys->set("PageDown", value_from_number(267));
            keys->set("Home", value_from_number(268));
            keys->set("End", value_from_number(269));
            keys->set("CapsLock", value_from_number(280));
            keys->set("ScrollLock", value_from_number(281));
            keys->set("NumLock", value_from_number(282));
            keys->set("PrintScreen", value_from_number(283));
            keys->set("Pause", value_from_number(284));
            keys->set("F1", value_from_number(290));
            keys->set("F2", value_from_number(291));
            keys->set("F3", value_from_number(292));
            keys->set("F4", value_from_number(293));
            keys->set("F5", value_from_number(294));
            keys->set("F6", value_from_number(295));
            keys->set("F7", value_from_number(296));
            keys->set("F8", value_from_number(297));
            keys->set("F9", value_from_number(298));
            keys->set("F10", value_from_number(299));
            keys->set("F11", value_from_number(300));
            keys->set("F12", value_from_number(301));
            keys->set("F13", value_from_number(302));
            keys->set("F14", value_from_number(303));
            keys->set("F15", value_from_number(304));
            keys->set("F16", value_from_number(305));
            keys->set("F17", value_from_number(306));
            keys->set("F18", value_from_number(307));
            keys->set("F19", value_from_number(308));
            keys->set("F20", value_from_number(309));
            keys->set("F21", value_from_number(310));
            keys->set("F22", value_from_number(311));
            keys->set("F23", value_from_number(312));
            keys->set("F24", value_from_number(313));
            keys->set("F25", value_from_number(314));
            keys->set("Keypad0", value_from_number(320));
            keys->set("Keypad1", value_from_number(321));
            keys->set("Keypad2", value_from_number(322));
            keys->set("Keypad3", value_from_number(323));
            keys->set("Keypad4", value_from_number(324));
            keys->set("Keypad5", value_from_number(325));
            keys->set("Keypad6", value_from_number(326));
            keys->set("Keypad7", value_from_number(327));
            keys->set("Keypad8", value_from_number(328));
            keys->set("Keypad9", value_from_number(329));
            keys->set("KeypadDecimal", value_from_number(330));
            keys->set("KeypadDivide", value_from_number(331));
            keys->set("KeypadMultiply", value_from_number(332));
            keys->set("KeypadSubtract", value_from_number(333));
            keys->set("KeypadAdd", value_from_number(334));
            keys->set("KeypadEnter", value_from_number(335));
            keys->set("KeypadEqual", value_from_number(336));
            keys->set("LeftShift", value_from_number(340));
            keys->set("LeftControl", value_from_number(341));
            keys->set("LeftAlt", value_from_number(342));
            keys->set("LeftSuper", value_from_number(343));
            keys->set("RightShift", value_from_number(344));
            keys->set("RightControl", value_from_number(345));
            keys->set("RightAlt", value_from_number(346));
            keys->set("RightSuper", value_from_number(347));
            keys->set("Menu", value_from_number(348));
        }

        Game* game; // for intellisense >:(
        
        auto vm = script;
        tge_func("exit", game->exit());

        // override existing dofile
        tge_func("dofile", {
            for (auto i = 0; i < nargs; i++) {
                auto s = SCRIPT_BASE + value_to_string(args[i])->data;
                vm->call(vm->load(read_text_file(s).value_or("")), nullptr, 0);
            }
        });

        tge_func("key_pressed", return value_from_number(game->key_pressed((uint32_t)value_to_number(args[0]))));
        tge_func("key_released", return value_from_number(game->key_released((uint32_t)value_to_number(args[0]))));
        tge_func("key_down", return value_from_number(game->key_down((uint32_t)value_to_number(args[0]))));

        tge_func("load_font", return value_from_number(game->load_font(value_to_string(args[0])->data)));
        tge_func("load_sprite", return value_from_number(game->load_sprite(value_to_string(args[0])->data)));
        tge_func("load_shader", return value_from_number(game->load_shader(value_to_string(args[0])->data)));

        tge_func("random", return value_from_number((float)rand() / RAND_MAX));
        tge_func("play_sound", game->play_sound(value_to_string(args[0])->data));
        tge_func("load_sound", return value_from_pointer(game->load_sound(value_to_string(args[0])->data)));
        tge_func("start_sound", game->start_sound(value_to_pointer(args[0]), value_to_boolean(args[1])));
        tge_func("stop_sound", game->stop_sound(value_to_pointer(args[0])));

        tge_func("set_shader", game->set_shader(value_is_null(args[0]) ? -1 : value_to_number(args[0])));
        tge_func("set_sprite", game->set_sprite(value_is_null(args[0]) ? -1 : value_to_number(args[0])));
        tge_func("set_font",   game->set_font(value_is_null(args[0]) ? -1 : value_to_number(args[0])));

        tge_func("set_fullscreen", game->set_fullscreen(value_to_boolean(args[0])));
        tge_func("set_shader_texture", game->set_shader_sprite(
            value_to_number(args[0]),
            value_to_number(args[1])
        ));
        tge_func("set_shader_float", game->set_shader_float(value_to_string(args[0])->data,
            value_to_number(args[1])
        ));
        tge_func("set_shader_vec2", game->set_shader_vec2(value_to_string(args[0])->data, {
            (float)value_to_number(args[1]),
            (float)value_to_number(args[2])
        }));
        tge_func("set_shader_vec3", game->set_shader_vec3(value_to_string(args[0])->data, {
            (float)value_to_number(args[1]),
            (float)value_to_number(args[2]),
            (float)value_to_number(args[3])
        }));
        tge_func("set_shader_vec4", game->set_shader_vec4(value_to_string(args[0])->data, {
            (float)value_to_number(args[1]),
            (float)value_to_number(args[2]),
            (float)value_to_number(args[3]),
            (float)value_to_number(args[4])
        }));
        tge_func("set_shader_int", game->set_shader_int(value_to_string(args[0])->data,
            (int)value_to_number(args[1])
        ));
        
        tge_func("draw_text", {
            auto text = value_to_string(args[0])->data;
            auto x = (float)value_to_number(args[1]);
            auto y = (float)value_to_number(args[2]);
            auto col = (nargs > 3) ? (uint32_t)value_to_number(args[3]) : 0xffffffff;
            game->draw_text(text, { x, y }, col);
        });
        tge_func("draw_sprite", {
            auto x = (float)value_to_number(args[0]);
            auto y = (float)value_to_number(args[1]);
            auto col = (nargs > 2) ? (uint32_t)value_to_number(args[2]) : 0xffffffff;
            game->draw_sprite({ x, y }, { 0, 0 }, 0.f, { 1.f, 1.f }, col);
        });
        tge_func("draw_sprite_ext", {
            auto x = (float)value_to_number(args[0]);
            auto y = (float)value_to_number(args[1]);
            auto ox = (float)value_to_number(args[2]);
            auto oy = (float)value_to_number(args[3]);
            auto angle = (float)value_to_number(args[4]);
            auto sx = (float)value_to_number(args[5]);
            auto sy = (float)value_to_number(args[6]);
            auto col = (nargs > 7) ? (uint32_t)value_to_number(args[7]) : 0xffffffff;
            game->draw_sprite({ x, y }, { ox, oy }, angle, { sx, sy }, col);
        });
        tge_func("draw_sprite_part", {
            auto x = (float)value_to_number(args[0]);
            auto y = (float)value_to_number(args[1]);
            auto x0 = (float)value_to_number(args[2]);
            auto y0 = (float)value_to_number(args[3]);
            auto sx = (float)value_to_number(args[4]);
            auto sy = (float)value_to_number(args[5]);
            auto col = (nargs > 6) ? (uint32_t)value_to_number(args[6]) : 0xffffffff;
            game->draw_sprite_part({ x, y }, { 0, 0 }, 0, { 1, 1 }, { x0, y0 }, { sx, sy }, col);
        });
        tge_func("draw_sprite_sheet", {
            auto name = value_to_string(args[0]);
            auto x = (float)value_to_number(args[1]);
            auto y = (float)value_to_number(args[2]);
            auto ox = (float)value_to_number(args[3]);
            auto oy = (float)value_to_number(args[4]);
            auto angle = (float)value_to_number(args[5]);
            auto sx = (float)value_to_number(args[6]);
            auto sy = (float)value_to_number(args[7]);
            auto col = (nargs > 8) ? (uint32_t)value_to_number(args[8]) : 0xffffffff;
            game->draw_sprite_sheet(name->data, { x, y }, { ox, oy }, angle, { sx, sy }, col);
        });


        #undef tge_func

        auto mainfile = read_text_file(SCRIPT_BASE + "main.tack");
        if (!mainfile.has_value()) {
            fatal_error("main script file missing");
        }
        script->call(script->load(mainfile.value()), nullptr, 0);
    } catch (std::exception& e) {
        log("Script error: "s + e.what());
        script = nullptr;
    }
}

void Game::cleanup_script() {
    // cleanup script
    delete script;
}
void Game::cleanup_sound() {
    // cleanup sound
    sound->drop();
}
void Game::cleanup_graphics() {
    // cleanup graphics
    for (auto& s : sprites) {
        glDeleteTextures(1, &s.texture);
    }
    sprites.clear();
    for (auto& s : fonts) {
        glDeleteTextures(1, &s.texture);
    }
    fonts.clear();
    for (auto& s : shaders) {
        glDeleteShader(s);
    }
    shaders.clear();

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vb);
    glDeleteBuffers(1, &ib);
}
void Game::cleanup_input() { }
void Game::cleanup_window() {
    // cleanup window
    glfwDestroyCursor(cursor_arrow);
    glfwDestroyWindow(window);
    glfwTerminate();
}

Game::Game() {
    init();
}
Game::~Game() {
    cleanup();
}

void Game::init() {
    init_window();
    init_graphics();
    init_sound();
    init_input();
    init_script();
}
void Game::cleanup() {
    cleanup_script();
    cleanup_input();
    cleanup_sound();
    cleanup_graphics();
    cleanup_window();
}

std::string_view next_cell(std::string_view& line) {
    auto comma = line.find(',');
    if (comma == std::string_view::npos) {
        comma = line.size();
    }
    auto cell = line.substr(0, comma);
    line.remove_prefix(comma == line.size() ? comma : comma + 1);
    return cell;
}

template<typename RowCallback> int load_csv(const std::string& fname, bool skip_header, const RowCallback& cb) {
    try {
        auto data_opt = read_text_file(fname);
        if (!data_opt.has_value()) {
            return 1;
        } else {
            auto data = data_opt.value();
            auto sol = 0;
            auto eol = 0;
            auto line_number = 0;
            while (eol != data.size()) {
                eol = data.find('\n', sol);
                if (eol == std::string::npos) {
                    eol = data.size();
                }
                auto line = std::string_view(data.begin() + sol, data.begin() + eol);
                if (!line.size()) {
                    break;
                }
                if (line_number > 0 || !skip_header) {
                    cb(line);
                }

                sol = eol + 1;
                line_number += 1;
            }
        }
    } catch (std::exception& e) {
        log("Error processing csv:", fname, e.what());
    }

    return 0;
}

uint32_t Game::load_sprite(const std::string& fname) {
    log("Loading spritesheet: ", fname);
    auto& sprite = sprites.emplace_back();
    auto error = load_csv(TEXTURE_BASE + fname + ".spritesheet", true, [&](std::string_view& line) {
        auto name = next_cell(line);
        auto x = stof_safe(next_cell(line));
        auto y = stof_safe(next_cell(line));
        auto width = stof_safe(next_cell(line));
        auto height = stof_safe(next_cell(line));
        sprite.sprites[std::string(name)] = Sprite {
            .pos = { x, y },
            .size = { width, height }
        };
    });
    if (error) {
        log("Warning: No spritesheet for '", fname, "'");
    }
    
    sprite.texture = load_texture(TEXTURE_BASE + fname + ".png", sprite.texture_width, sprite.texture_height);
    return sprites.size() - 1;
}

uint32_t Game::load_font(const std::string& fname) {
    log("Loading font: ", fname);

    auto& font = fonts.emplace_back();
    auto error = load_csv(FONT_BASE + fname + ".spritefont", true, [&](std::string_view& line) {
        auto ch = stof_safe(next_cell(line));
        font.chars[ch].pos1.x = stof_safe(next_cell(line));
        font.chars[ch].pos1.y = stof_safe(next_cell(line));
        font.chars[ch].pos2.x = stof_safe(next_cell(line));
        font.chars[ch].pos2.y = stof_safe(next_cell(line));
        font.chars[ch].off1.x = stof_safe(next_cell(line));
        font.chars[ch].off1.y = stof_safe(next_cell(line));
        font.chars[ch].off2.x = stof_safe(next_cell(line));
        font.chars[ch].off2.y = stof_safe(next_cell(line));
        font.chars[ch].xadvance = round(stof_safe(next_cell(line)));

        font.line_height = std::max((float)font.line_height, font.chars[ch].pos2.y - font.chars[ch].pos1.y);
    });
    if (error) {
        log("Failed to load font: ", fname);
    }

    font.texture = load_texture(FONT_BASE + fname + ".png", font.texture_width, font.texture_height);
    return fonts.size() - 1;
}

GLuint Game::load_shader(const std::string& name) {
    log("Loading shader: ", name);
    auto s = load_shader_program("sprite_vs.glsl", name + ".glsl");
    shaders.emplace_back(s);
    return shaders.size() - 1;
}

bool Game::get_fullscreen() {
    return glfwGetWindowMonitor(window) != nullptr;
}
void Game::set_fullscreen(bool fs) {
    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);
    if (fs) {
        log("Video mode: ", mode->width, mode->height, mode->refreshRate);
        //glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        glfwSetWindowMonitor(window, monitor, 0, 0, 1920, 1080, mode->refreshRate);
        mode = glfwGetVideoMode(monitor);
        log("Video mode after fullscreen: ", mode->width, mode->height, mode->refreshRate);

        glfwFocusWindow(window);
        glfwSwapInterval(1);
    } else {
        glfwSetWindowMonitor(window, nullptr, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
        auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        log("Video mode: ", mode->width, mode->height, mode->refreshRate);
    }
}

void Game::run() {
    if (script && !safe_call(script, "on_init")) {
        delete script;
        script = nullptr;
    }

    // main loop
    while (!glfwWindowShouldClose(window)) {
        update_input();

        if (key_pressed(GLFW_KEY_F4)) {
            set_fullscreen(!get_fullscreen());
        }
        if (key_pressed(GLFW_KEY_F1)) {
            cleanup_script();
            cleanup_input();
            cleanup_sound();
            cleanup_graphics();
            // refresh all systems (except window)
            init_graphics();
            init_sound();
            init_input();
            init_script();
        }

        begin_frame();
        
        if (script && !safe_call(script, "on_frame")) {
            delete script;
            script = nullptr;
        }
        
        end_frame();
    }

    if (script && !safe_call(script, "on_exit")) {
        delete script;
        script = nullptr;
    }

    return;
}

void Game::exit() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Game::begin_frame() {
    glfwGetFramebufferSize(window, &display_width, &display_height);

    // Setup rendering
    glViewport(0, 0, display_width, display_height);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Program
    is_text = false;
    set_shader(default_shader);
    set_sprite(default_sprite);

}


void Game::set_sprite(uint32_t sprite) {
    flush_batch();
    is_text = false;
    current_sprite = (sprite < sprites.size()) ? sprite : default_sprite;
    set_shader_texture(0, sprites[current_sprite].texture);
}

void Game::set_font(uint32_t font) {
    flush_batch();
    is_text = true;
    current_font = (font < fonts.size()) ? font : default_font;
    set_shader_texture(0, fonts[current_font].texture);
}
void Game::set_shader(uint32_t shader) {
    flush_batch();
    current_shader = (shader != 0xffffffff) ? shaders[shader] : shaders[default_shader];
    glUseProgram(current_shader);
    set_shader_vec2("DisplaySize", { (float)display_width, (float)display_height });
    set_shader_texture(0, is_text ? fonts[current_font].texture : sprites[current_sprite].texture);
}
void Game::set_shader_sprite(uint32_t slot, uint32_t sprite) {
    set_shader_texture(slot, (sprite < sprites.size()) ? sprites[sprite].texture : default_sprite);
}
void Game::set_shader_texture(uint32_t slot, GLuint texture) {
    if (slot < 4) {
        auto slot_names = std::array<const char*, 4> {{ "Texture0", "Texture1", "Texture2", "Texture3" }};
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);        
        glUniform1i(glGetUniformLocation(current_shader, slot_names[slot]), slot);
    }
}

void Game::set_shader_float(const std::string& param, float f) {
    glUniform1f(glGetUniformLocation(current_shader, param.c_str()), f);
}

void Game::set_shader_vec2(const std::string& param, const vec2& v) {
    glUniform2f(glGetUniformLocation(current_shader, param.c_str()), v.x, v.y);
}

void Game::set_shader_vec3(const std::string& param, const vec3& v) {
    glUniform3f(glGetUniformLocation(current_shader, param.c_str()), v.x, v.y, v.z);
}

void Game::set_shader_vec4(const std::string& param, const vec4& v) {
    glUniform4f(glGetUniformLocation(current_shader, param.c_str()), v.x, v.y, v.z, v.w);
}

void Game::set_shader_int(const std::string& param, int i) {
    glUniform1i(glGetUniformLocation(current_shader, param.c_str()), i);
}

void Game::play_sound(const std::string& fname) {
    auto f = SOUND_BASE + fname;
    log("Playing sound: ", f);
    sound->play2D(f.c_str());
}

void* Game::load_sound(const std::string& fname) {
    auto f = SOUND_BASE + fname;
    log("Loading sound: ", f);
    auto s = sound->play2D(f.c_str(), false, true, true, irrklang::ESM_AUTO_DETECT, true);
    return (void*)s;
}

void Game::start_sound(void* sound, bool loop) {
    auto s = (irrklang::ISound*)sound;
    s->setPlayPosition(0);
    s->setIsPaused(false);
    s->setIsLooped(loop);
}

void Game::stop_sound(void* sound) {
    auto s = (irrklang::ISound*)sound;
    s->setIsPaused(true);
}


void Game::end_frame() {
    flush_batch();
    glFinish();
    glFlush();
    glfwSwapBuffers(window);
}

void Game::update_input() {
    mouse_old = mouse_new;
    keys_old = keys_new;
    glfwPollEvents();

    // update script
    if (script) {
        mouse->set("x", value_from_number(mouse_pos.x));
        mouse->set("y", value_from_number(mouse_pos.y));
    }
}

bool Game::key_down(int key) const {
    return keys_new[key];
}

bool Game::key_pressed(int key) const {
    return keys_new[key] && !keys_old[key];
}

bool Game::key_released(int key) const {
    return keys_old[key] && !keys_new[key];
}

bool Game::mouse_down(int button) const {
    return mouse_new[button];
}

bool Game::mouse_pressed(int button) const {
    return mouse_new[button] && !mouse_old[button];
}

bool Game::mouse_released(int button) const {
    return mouse_old[button] && !mouse_new[button];
}

vec2 Game::get_mouse_pos() const { return mouse_pos; }



void Game::flush_batch() {
	if (!indices.size()) {
		return;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, nullptr);
	
    vertices.clear();
	indices.clear();
}

void Game::draw_sprite_sheet(const std::string& name, const vec2& pos, const vec2& origin, const float angle, const vec2& scale, const uint32_t color) {
    auto& spritesheet = sprites[current_sprite].sprites;
	if (auto iter = spritesheet.find(name); iter != spritesheet.end()) {
        draw_sprite_part(pos, origin, angle, scale, iter->second.pos, iter->second.size, color);
	}
}

void Game::draw_sprite(const vec2& pos, const vec2& origin, const float angle, const vec2& scale, const uint32_t color) {
	draw_sprite_part(pos, origin, angle, scale, { 0, 0 }, {
        (float)sprites[current_sprite].texture_width,
        (float)sprites[current_sprite].texture_height
    }, color);
}

void Game::draw_sprite_part(const vec2& pos, const vec2& origin, const float angle, const vec2& scale, const vec2& tex_pos, const vec2& tex_size, const uint32_t color) {
	// apply rotation
	auto c = cos(angle), s = sin(angle);
	auto basis_x = vec2 { c * scale.x, s * scale.x };
	auto basis_y = vec2 { -s * scale.y, c * scale.y };
    auto tex_max = vec2 { tex_pos.x + tex_size.x, tex_pos.y + tex_size.y };

	auto corners = std::array<vec2, 4> {{
		vec2 {
			pos.x + (-origin.x)             * basis_x.x + (-origin.y)               * basis_x.y,
			pos.y + (-origin.x)             * basis_y.x + (-origin.y)               * basis_y.y
		},
		vec2 {
			pos.x + (tex_size.x - origin.x) * basis_x.x + (-origin.y)               * basis_x.y,
			pos.y + (tex_size.x - origin.x) * basis_y.x + (-origin.y)               * basis_y.y
		},
		vec2 {
			pos.x + (-origin.x)             * basis_x.x + (tex_size.y - origin.y)   * basis_x.y,
			pos.y + (-origin.x)             * basis_y.x + (tex_size.y - origin.y)   * basis_y.y
		},
		vec2 {
			pos.x + (tex_size.x - origin.x) * basis_x.x + (tex_size.y - origin.y)   * basis_x.y,
			pos.y + (tex_size.x - origin.x) * basis_y.x + (tex_size.y - origin.y)   * basis_y.y
		}
	}};

	auto i = (uint16_t)vertices.size();
	vertices.emplace_back(Vertex { corners[0], (uint16_t)tex_pos.x, (uint16_t)tex_pos.y, color });
	vertices.emplace_back(Vertex { corners[1], (uint16_t)tex_max.x, (uint16_t)tex_pos.y, color });
	vertices.emplace_back(Vertex { corners[2], (uint16_t)tex_pos.x, (uint16_t)tex_max.y, color });
	vertices.emplace_back(Vertex { corners[3], (uint16_t)tex_max.x, (uint16_t)tex_max.y, color });

	indices.emplace_back(i);
	indices.emplace_back(i + 1);
	indices.emplace_back(i + 2);
	indices.emplace_back(i + 1);
	indices.emplace_back(i + 3);
	indices.emplace_back(i + 2);
}

vec2 Game::measure_text(uint32_t font, const std::string& text) {
	auto cursor = vec2 { 0, 0 };
	auto max_x = 0.f;
    auto& spritefont = fonts[font];
	for (auto& c : text) {
		if (c < 0 || c > 127) {
			continue;
		} else if (c == '\t') {
			cursor.x += spritefont.chars[' '].xadvance * 4;
		} else if (c == '\n') {
			cursor.y += spritefont.line_height;
			cursor.x = 0;
		} else {
			cursor.x += spritefont.chars[c].xadvance;
		}
		max_x = std::max(max_x, cursor.x);
	}
	cursor.y += spritefont.line_height;
	return { max_x, cursor.y };
}

void Game::draw_text(const std::string& text, const vec2& pos, const uint32_t color) {
	auto cursor = pos;
    auto& spritefont = fonts[current_font];
	for (auto& c : text) {
		if (c < 0 || c > 127) {
			continue;
		} else if (c == ' ') {
			cursor.x += spritefont.chars[' '].xadvance;
		} else if (c == '\t') {
			cursor.x += spritefont.chars[' '].xadvance * 4;
		} else if (c == '\n') {
			cursor.y += spritefont.line_height;
			cursor.x = pos.x;
		} else if (!isspace(c)){
			auto& ch = spritefont.chars[c];
			auto w = ch.pos2.x - ch.pos1.x;
			auto h = ch.pos2.y - ch.pos1.y;
			auto orig = vec2 { -ch.off1.x, -ch.off1.y - spritefont.line_height };
            draw_sprite_part(cursor, orig, 0.f, { 1.f, 1.f }, ch.pos1, { w, h }, color);
			cursor.x += ch.xadvance;
		}
	}
}
