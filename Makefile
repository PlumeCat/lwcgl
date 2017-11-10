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
			-std=c++14

build_main: src/main.cpp
	clang++ src/main.cpp -o cubes $(CLANG_ARGS) $(INCLUDE) $(LIBS)

run: build_main
	./cubes