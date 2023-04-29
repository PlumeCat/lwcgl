#pragma once

// STL
#include <iostream>
#include <fstream>
#include <filesystem>

#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <memory>
#include <functional>
#include <algorithm>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>


// jlib
#include <jlib/log.h>
#include <jlib/generic_ostream.h>
#include <jlib/text_file.h>


// gl and glfw
#include "internal/glad_wrapper.h"
#include <GLFW/glfw3.h>

// script
#include <interpreter.h>

// stb
#include <stb_image.h>

// irrklang
#include <irrKlang.h>

using namespace std::string_literals;

// common definitions
struct Vertex {
	vec2 pos;
	uint16_t u;
	uint16_t v;
	uint32_t color;
};

struct FontChar {
	vec2 pos1;
	vec2 pos2;
	vec2 off1;
	vec2 off2;
	float xadvance;
};
struct Sprite {
	vec2 pos;
	vec2 size;
};

struct SpriteFont {
	GLuint texture;
	uint32_t texture_width;
	uint32_t texture_height;
	std::array<FontChar, 128> chars;
	uint32_t line_height;
};
struct SpriteSheet {
	GLuint texture;
	uint32_t texture_width;
	uint32_t texture_height;
	std::unordered_map<std::string, Sprite> sprites;
};

extern const std::string TEXTURE_BASE;
extern const std::string SHADER_BASE;
extern const std::string SCRIPT_BASE;
extern const std::string FONT_BASE;
extern const std::string SOUND_BASE;

void run_game();
