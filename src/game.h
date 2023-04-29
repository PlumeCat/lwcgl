#include "tge.h"

class Game {
public:
	Game();
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;
	~Game();

	// input
	bool key_down(int key) const;
	bool key_pressed(int key) const;
	bool key_released(int key) const;
	bool mouse_down(int button) const;
	bool mouse_pressed(int button) const;
	bool mouse_released(int button) const;
	vec2 get_mouse_pos() const;

	// graphics
	bool get_fullscreen();
	void set_fullscreen(bool fs);
	uint32_t load_font(const std::string& fname);
	uint32_t load_sprite(const std::string& fname);

	// sound
	void play_sound(const std::string& fname);
	void* load_sound(const std::string& fname);
	void start_sound(void* sound, bool loop);
	void stop_sound(void* sound);

	void set_sprite(uint32_t sprite);
	void set_font(uint32_t font);

	// draw the whole texture
	void draw_sprite(const vec2& pos, const vec2& origin, const float angle = 0.f, const vec2& scale = { 1, 1 }, const uint32_t color = 0xffffffff);

	// draw a sprite from the spritesheet
	void draw_sprite_sheet(const std::string& name, const vec2& pos, const vec2& origin, const float angle, const vec2& scale = { 1, 1 }, const uint32_t color = 0xffffffff);

	// draw custom part of the texture
	void draw_sprite_part(const vec2& pos, const vec2& origin, const float angle, const vec2& scale, const vec2& tex_pos, const vec2& tex_size, const uint32_t color = 0xffffffff);

	// draw text with the current sprite font
	void draw_text(const std::string& text, const vec2& pos, const uint32_t color = 0xffffffff);
	vec2 measure_text(uint32_t font, const std::string& text);

	// searches for pixel shader 'name.glsl'
	GLuint load_shader(const std::string& name);
	void set_shader(uint32_t shader);
	void set_shader_texture(uint32_t slot, GLuint texture);
	void set_shader_sprite(uint32_t slot, uint32_t sprite);
	void set_shader_float(const std::string& param, float f);
	void set_shader_vec2(const std::string& param, const vec2& v);
	void set_shader_vec3(const std::string& param, const vec3& v);
	void set_shader_vec4(const std::string& param, const vec4& v);
	void set_shader_int(const std::string& param, int i);

	// run/exit
	void run();
	void exit();

private:
	void update_input();
	void begin_frame();
	void flush_batch();
	void end_frame();

	void init();
	void init_window();
	void init_graphics();
	void init_sound();
	void init_script();
	void init_input();

	void cleanup();
	void cleanup_window();
	void cleanup_graphics();
	void cleanup_sound();
	void cleanup_script();
	void cleanup_input();
	
	// window
    GLFWwindow* window = nullptr;

	// script
	Interpreter* script = nullptr;
	ObjectType* mouse = nullptr;
	ObjectType* keys = nullptr;
	ObjectType* system = nullptr;

	// graphics
	GLuint vb;
	GLuint ib;
	GLuint vao;
	std::vector<SpriteSheet> sprites;
	std::vector<SpriteFont> fonts;
	std::vector<GLuint> shaders;
	int display_width = 0;
	int display_height = 0;
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	uint32_t current_font;
	uint32_t current_sprite;
	GLuint current_shader;
	uint32_t default_font;
	uint32_t default_sprite;
	GLuint default_shader;
	bool is_text = true;

	// sound
	irrklang::ISoundEngine* sound;

    // input
	std::array<int, GLFW_KEY_LAST> keys_old = { 0 };
	std::array<int, GLFW_KEY_LAST> keys_new = { 0 };
	std::array<int, GLFW_MOUSE_BUTTON_LAST> mouse_old = { 0 };
	std::array<int, GLFW_MOUSE_BUTTON_LAST> mouse_new = { 0 };
	vec2 mouse_pos;
	GLFWcursor* cursor_arrow;
};
