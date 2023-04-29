#include "gl_helpers.h"

const std::string TEXTURE_BASE = "..//..//..//resource//texture//";
const std::string SHADER_BASE = "..//..//..//resource//shader//";
const std::string FONT_BASE = "..//..//..//resource//font//";

GLuint load_shader_program(const std::string& vs_file, const std::string& fs_file) {
	auto check_error = [&](GLuint shader, const std::string& filename, bool is_shader = true) -> bool {
		auto success = 0;
		(is_shader ? glGetShaderiv : glGetProgramiv)(shader, is_shader ? GL_COMPILE_STATUS : GL_LINK_STATUS, &success);
		auto log_length = 0;
		(is_shader ? glGetShaderiv : glGetProgramiv)(shader, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length) {
			auto info = std::string(log_length, 'a');
			(is_shader ? glGetShaderInfoLog : glGetProgramInfoLog)(shader, log_length, nullptr, info.data());
			log("Compiler output: ", filename, "\n", info, "\n");
		}
		return success == GL_TRUE;
	};
	auto load_shader = [&](const std::string& filename, int type) -> GLuint {
		auto shader = glCreateShader(type);
		auto data = read_text_file(filename);
		
		if (!data.has_value()) {
			log("Could not open shader file: ", filename);
			return -1;
		}
		
		auto data_ptr = data.value().c_str();
		glShaderSource(shader, 1, &data_ptr, nullptr);
		glCompileShader(shader);
		if (!check_error(shader, filename)) {
			log("Could not compile shader: ", filename);
			return -1;
		}
		return shader;
	};

	log("Loading shader: ", vs_file);
	auto vs = load_shader(SHADER_BASE + vs_file, GL_VERTEX_SHADER);
	if (vs == -1) {
		return -1;
	}

	log("Loading shader: ", fs_file);
	auto fs = load_shader(SHADER_BASE + fs_file, GL_FRAGMENT_SHADER);
	if (fs == -1) {
		return -1;
	}

	log("Linking shader program: ", vs_file, fs_file);
	auto program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	if (!check_error(program, "", false)) {
		log(" - Could not link shaders", vs_file, fs_file);
		glDeleteProgram(program);
		return -1;
	}
	return program;
}

GLuint load_texture(const std::string& fname, uint32_t& out_width, uint32_t& out_height) {
	auto width = 0;
	auto height = 0;
	auto channels = 0;
	log("Loading texture: ", fname);
	auto texture_data = stbi_load((fname).c_str(), &width, &height, &channels, 0);
	if (!texture_data) {
		log("Failed to load texture: ", fname);
		return -1;
	}
	if (channels != 4) {
		log("Invalid texture format: ", fname);
		stbi_image_free(texture_data);
		return -1;
	}

	auto texture = GLuint { 0 };
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texture_data);

	out_width = width;
	out_height = height;
	return texture;
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
void fatal_error(const std::string& error_message) {
	MessageBox(nullptr, error_message.c_str(), "Fatal Error", MB_OK | MB_ICONERROR);
	std::exit(0);
}