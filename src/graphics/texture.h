#ifndef _CUBE_TEXTURE_H
#define _CUBE_TEXTURE_H

#include <SOIL.h>
#include "../definitions.h"

struct Texture
{
    GLuint texture = 0;
    int width = 0;
    int height = 0;

    Texture() = default;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    ~Texture()
    {
        glDeleteTextures(1, &texture);
    }
};

struct SpriteChar
{
    char ch;
    float left;
    float top;
    float width;
    float height;
};
struct SpriteFont
{
    Texture* texture;
    SpriteChar chars[256];
};

class TextureManager
{
    string baseDir;
    string fontDir;
    map<string, Texture*> textures;
    map<string, Texture*> cubetextures;
    map<string, SpriteFont*> fonts;

    void getTextureSize(GLuint tex, int& w, int& h)
    {
        glBindTexture(GL_TEXTURE_2D, tex);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint loadTexture(const string& fname)
    {
        cout << "    loading texture from file: " << fname << "...";
        GLuint tex = SOIL_load_OGL_texture(
            fname.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
        );

        if (tex)
        {
            cout << "success" << endl;
        }
        else
        {
            cout << "FAILED" << endl;
        }
        return tex;
    }

    GLuint loadCubeTexture(const string (&fnames)[6])
    {
        return 0;
    }

public:
    TextureManager(const string& dir=string(RESOURCE_BASE)+"/textures", const string& fdir=string(RESOURCE_BASE)+"/fonts") : baseDir(dir), fontDir(fdir) {}
    Texture* getTexture(const string& fname)
    {
        cout << "loading texture" << fname << endl;
        auto tex = textures.find(fname);
        if (tex == textures.end())
        {
            Texture* texture = new Texture();
            texture->texture = loadTexture(baseDir + "/" + fname);
            getTextureSize(texture->texture, texture->width, texture->height);
            cout << "    size: " << texture->width << "x" << texture->height << endl;
            textures[fname] = texture;
            return texture;
        }
        else
        {
            cout << "    using cache" << endl;
            return tex->second;
        }
    }
    Texture* getCubeTexture(const string (&fnames)[6])
    {
        cout << "loading cube map: " << fnames[0] << " (etc)" << endl;

        string key = fnames[0]+"&"+
                     fnames[1]+"&"+
                     fnames[2]+"&"+
                     fnames[3]+"&"+
                     fnames[4]+"&"+
                     fnames[5];

        auto tex = cubetextures.find(key);
        if (tex == textures.end())
        {
            Texture* texture = new Texture();
            texture->texture = loadCubeTexture(fnames);
            textures[key] = texture;
            return texture;
        }
        else
        {
            cout << "    using cached" << endl;
            return tex->second;
        }
    }
    void loadFontChars(const string& fname, SpriteChar (&chars)[256])
    {
        cout << "    loading chars... ";

        vector<string> lines;
        readTextFile(fname, lines);
        for (int i = 0; i < lines.size(); i++)
        {
            const string& line = lines[i];

            string keys[6] = { "ch", "code", "l", "t", "w", "h" };
            string values[6] = { "", "", "", "", "", "" };
            auto scanpos = string::size_type(0);

            for (int j = 0; j < arraylen(keys); j++)
            {
                auto start = line.find(keys[j]+"=", scanpos);
                auto end = (j!=arraylen(keys)-1) ? line.find(" ", start+keys[j].size()+1) : line.size();
                if (start == string::npos || end == string::npos)
                {
                    cout << "    invalid line: (" << i << ") " << line << endl;
                    break;
                }
                else
                {
                    start += keys[j].size()+1;
                    values[j] = line.substr(start, end-start);
                    // cout << "(" << values[j] << ")";
                    scanpos = end + 1;
                }
            }

            int ch = atoi(values[0].c_str());
            int code = atoi(values[1].c_str());
            chars[code].ch = code;
            stringstream(values[2]) >> chars[code].left;
            stringstream(values[3]) >> chars[code].top;
            stringstream(values[4]) >> chars[code].width;
            stringstream(values[5]) >> chars[code].height;
            // chars[code].left   = atoi(values[2].c_str());
            // chars[code].top    = atoi(values[3].c_str());
            // chars[code].width  = atoi(values[4].c_str());
            // chars[code].height = atoi(values[5].c_str());
        }

        cout << "success" << endl;
    }
    SpriteFont* getFont(const string& fname)
    {
        cout << "loading font: " << fname << endl;

        auto fontiter = fonts.find(fname);
        if (fontiter == fonts.end())
        {
            SpriteFont* font = new SpriteFont();
            font->texture = new Texture();
            font->texture->texture = loadTexture(fontDir + "/" + fname + ".png");
            getTextureSize(font->texture->texture, font->texture->width, font->texture->height);
            cout << "    size: " << font->texture->width << "x" << font->texture->height << endl;
            loadFontChars(fontDir + "/" + fname + ".txt", font->chars);
            fonts[fname] = font;
            return font;
        }
        else
        {
            cout << "    using cache" << endl;
            return fontiter->second;
        }
    }
};

#endif