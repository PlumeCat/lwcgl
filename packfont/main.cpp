#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <charconv>
#include <algorithm>

#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#include "stb_image_write.h"

#define JLIB_LOG_VISUALSTUDIO
#define JLIB_IMPLEMENTATION
#include <jlib/log.h>
#include <jlib/defer.h>
#include <jlib/text_file.h>

using namespace std;

std::optional<std::vector<unsigned char>> read_binary_file(const std::string& fname) {
	auto file = std::ifstream(fname, ios::binary);
	if (!file.is_open()) {
		return std::nullopt;
	}
	auto start = file.tellg();
	file.seekg(0, std::ios::end);
	auto end = file.tellg();
	auto size = (size_t)(end - start);
	file.seekg(0, std::ios::beg);
	auto data = vector<unsigned char>(size);
	file.read((char*)data.data(), size);
	return std::optional(data);
}



int main(int argc, char* argv[]) {
	// packfont.exe fontname size output-name font-index oversampling
	log<true, false>("Packfont");
	if (argc < 4) {
		log<true, false>("error: expected at least 3 arguments");
		log<true, false>("usage: packfont <font-name> <size> <output-name> [ <font-index> [ <oversampling> ] ]");
		return 1;
	}

	const auto fname = string(argv[1]);
	const auto font_size = max((float)atof(argv[2]), 12.f);
	const auto output = string(argv[3]);
	const auto font_index = (argc >= 5) ? atoi(argv[4]) : 0;
	const auto oversampling = (argc >= 6) ? atoi(argv[5]) : 1;
	const auto padding = 1;

	log<true, false>("Settings: ", fname, font_size, output, font_index, oversampling, padding);
	
	const auto start_char = 0;
	const auto end_char = 128;
	const auto num_chars = end_char - start_char;
	auto char_data = array<stbtt_packedchar, num_chars>{};
	auto font_data = read_binary_file("C://Windows//Fonts//" + fname + ".ttf")
		.value_or(read_binary_file("C://Windows//Fonts//" + fname + ".ttc")
			.value_or(vector<unsigned char>{}));
	if (font_data.empty()) {
		log<true, false>("error: failed to load font: ", fname);
		return 2;
	}
	
	auto pack_success = false;
	auto pixels = vector<unsigned char>{};
	auto texture_size = 64;
	for (; texture_size <= 2048; texture_size *= 2) {
		log<true, false>("attempting pack: texture size is ", texture_size);
		pixels.clear();
		pixels.resize(texture_size * texture_size);
		auto context = stbtt_pack_context{};
		defer{ stbtt_PackEnd(&context); };
		if (0 == stbtt_PackBegin(&context, pixels.data(), texture_size, texture_size, 0, padding, nullptr)) {
			// failed
			log<true, false>("note: PackBegin failed");
			return 3;
		}
		stbtt_PackSetOversampling(&context, oversampling, oversampling);

		// check for font index
		auto index = 0;
		log<true, false>("checking font indices");
		while (true) {
			if (stbtt_GetFontOffsetForIndex(font_data.data(), index) == -1) {
				log<true, false>(index, "is not a font index");
				break;
			}
			log<true, false>(index, "is a valid font index");
			index++;
		}

		if (0 == stbtt_PackFontRange(&context, font_data.data(), font_index, font_size, start_char, num_chars, char_data.data())) {
			log<true, false>("note: PackFontRange failed");
		} else {
			pack_success = true;
			break;
		}
	}
	if (pack_success) {
		// write texture
		auto pixels32 = vector<uint32_t>{};
		for (auto p : pixels) {
			pixels32.push_back((uint32_t(p) << 24) | 0xffffff);
		}
		if (0 == stbi_write_png((output + ".png").c_str(), texture_size, texture_size, 4, pixels32.data(), texture_size * 4)) {
			log<true, false>("error: write png failed");
			return 5;
		}

		// write spritefont
		auto data = stringstream{};
		data << "CHAR,x0,y0,x1,y1,xoff,yoff,xoff2,yoff2,xadvance\n";
		for (auto i = start_char; i < end_char; i++) {
			data << i << ','
				<< char_data[i].x0 << ','
				<< char_data[i].y0 << ','
				<< char_data[i].x1 << ','
				<< char_data[i].y1 << ','
				<< char_data[i].xoff << ','
				<< char_data[i].yoff << ','
				<< char_data[i].xoff2 << ','
				<< char_data[i].yoff2 << ','
				<< char_data[i].xadvance
				<< '\n';
		}
		write_text_file(output + ".spritefont", data.str());

		log<true, false>("success");
	} else {
		log<true, false>("error: could not pack into texture up to maximum size");
		return 6;
	}


#ifdef _DEBUG
	log<true, false>("Press enter to continue");
	getchar();
#endif

	return 0;
}
