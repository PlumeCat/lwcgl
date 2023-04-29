#pragma once
#include "../tge.h"

GLuint load_shader_program(const std::string& vs_file, const std::string& fs_file);

GLuint load_texture(const std::string& fname, uint32_t& out_width, uint32_t& out_height);
