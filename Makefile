LIBS = 		-L./ \
			-L/usr/local/lib \
			-lglfw3 -lSOIL -lGLEW \
			-fms-extensions \
			-framework Cocoa \
			-framework OpenGL \
			-framework IOKit \
			-framework CoreVideo

INCLUDE = 	-I/usr/local/include

CLANG_ARGS =-Wall \
			-m32 \
			-arch i386 \
			-std=c++14 \
			-g

clean:
	rm -r build/debug
	mkdir build/debug

build_debug: clean src/main.cpp
	clang++ src/main.cpp -o build/debug/cubes $(CLANG_ARGS) $(INCLUDE) $(LIBS) -DRESOURCE_BASE="string(\"resource\")"

debug: build_debug
	./build/debug/cubes