#ifndef _JAMESGL_H
#define _JAMESGL_H

#include <GL/glew.h>
#include <GL/wglew.h>	
#include <GLFW/glfw3.h>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar, byte;
typedef int64_t int64;

#define assert(x) { if (!x) { cout << "Assert Failed: " << __LINE__ << endl; abort(); }}
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define random() ((double)rand() / (double)RAND_MAX)
#define uniform(a, b) (a + (b-a) * random())
#define pi 3.1415926535f

typedef GLuint GLRESOURCE;

#endif