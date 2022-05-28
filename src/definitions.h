#ifndef _CUBE_DEFS_H
#define _CUBE_DEFS_H

#include <cassert>
#include <fstream>
#include <cstdint>

#define GLFW_INCLUDE_NONE
#include "../dep/glad.h"
#include <GLFW/glfw3.h>

typedef uint32_t uint;
typedef uint16_t ushort;
typedef uint8_t uchar, byte;

static const float pi = 3.1415926535f;
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) < (b)) ? (a) : (b))
#define random() ((double)rand() / (double)RAND_MAX)
#define uniform(a, b) (a + (b - a) * random())
#define arraylen(x) (sizeof(x) / sizeof(x[0]))



string readTextFile(const string& fname)
{
    ifstream file(fname);
    string source, line;
    while (getline(file, line))
    {
        source += line + "\n";
    }
    return source;
}
bool readTextFile(const string& fname, vector<string>& lines)
{
    ifstream file(fname);
    if (!file.is_open())
    {
        return false;
    }

    string line;
    while (getline(file, line))
    {
        lines.push_back(line);
    }

    return true;
}

void split(const string& str, char c, vector<string>& tokens)
{
    int s = 0;
    int e = str.find(c, 0);
    while (true)
    {
        if (e - s > 0)
        {
            // copy...
            tokens.push_back(str.substr(s, e-s));
        }

        s = e+1;
        e = str.find(c, s);

        if (e == string::npos)
        {
            tokens.push_back(str.substr(s, str.size()-s));
            break;
        }
    }
}
// remove leading and trailing whitespace from a string
string strip(const string& str)
{
    auto whitespace = " \r\n\t";
    auto first = str.find_first_not_of(whitespace);
    if (first == string::npos)
    {
        return "";
    }
    
    auto last = str.find_last_not_of(whitespace);
    // if first_not_of didn't return string::npos, then last_not_of can't either
    // so no need to check last == string::npos
    return string(str.begin() + first, str.begin() + last + 1);
}


#ifndef RESOURCE_BASE
#define RESOURCE_BASE "resource"
#endif

#endif