#ifndef _CUBE_SPRITE_H
#define _CUBE_SPRITE_H

#include "../definitions.h"
#include "../math3d.h"

class Sprite
{
    static const uint MaxQuads = 1024;

    Shader* defaultShader = nullptr;
    Shader* fontShader = nullptr;

    Buffer* posBuf = nullptr;
    Buffer* texBuf = nullptr;
    Buffer* colBuf = nullptr;
    Buffer* indexBuf = nullptr;
    VertexArray* array = nullptr;

    float2 vertices[MaxQuads * 4];
    float4 colours[MaxQuads * 4];
    float2 texcoords[MaxQuads * 4];
    ushort indices[MaxQuads * 6];
    uint numQuads = 0;
    GLuint texture = 0;
    Shader* currentShader = nullptr;

public:
    Sprite() = delete;
    Sprite(const Sprite&) = delete;
    Sprite& operator = (const Sprite&) = delete;

    void getVertexAttrs(vector<VertexAttr>& attrs)
    {
        attrs.push_back(VertexAttr("iPos", 0, 2, GL_FLOAT, false, 0, nullptr));
        attrs.push_back(VertexAttr("iTex", 1, 2, GL_FLOAT, false, 0, nullptr));
        attrs.push_back(VertexAttr("iCol", 2, 4, GL_FLOAT, false, 0, nullptr));
    }

    Sprite(ShaderManager* shaders)
    {
        vector<VertexAttr> attrs;
        getVertexAttrs(attrs);

        defaultShader = shaders->getShader("spritevs.glsl", "spriteps.glsl");
        fontShader = shaders->getShader("spritevs.glsl", "fontps.glsl");

        posBuf = new Buffer();
        texBuf = new Buffer();
        colBuf = new Buffer();
        indexBuf = new Buffer(nullptr, 0, true);
        array = new VertexArray(attrs, { posBuf, texBuf, colBuf }, indexBuf);
    }
    ~Sprite()
    {
        delete posBuf;
        delete texBuf;
        delete colBuf;
        delete indexBuf;
        delete array;
    }
    void drawText(SpriteFont* font, const string& text, const float2& pos, const float2& scale={1,1}, Shader* shader=nullptr)
    {
        begin(font->texture->texture, fontShader);

        float2 offset = pos;

        for (int i = 0; i < text.size(); i++)
        {
            char c = text[i];

            // handle these characters specially
            if (c == ' ') { offset.x += font->chars[(uint)'a'].width; continue; }
            if (c == '\t') { offset.x += font->chars[(uint)'a'].width*4; continue; }
            if (c == '\n') { offset.x = pos.x; offset.y -= font->chars[(uint)'a'].height; continue; }

            // skip unrenderable characters
            if (c < 32 || c > 126) continue;

            const SpriteChar& ch = font->chars[(uint)c];

            float2 tmn((float)ch.left/(float)font->texture->width, 1-(float)(ch.top+ch.height)/(float)font->texture->height);
            float2 tmx((float)(ch.left+ch.width)/(float)font->texture->width, 1-(float)(ch.top)/(float)font->texture->height);

            addSprite(pos+offset*scale, float2(ch.width, ch.height)*scale, {1,1,1,1}, tmn, tmx);

            offset.x += ch.width;
        }

        end();
    }
    void begin(GLuint tex, Shader* shader = nullptr)
    {
        numQuads = 0;
        texture = tex;
        currentShader = shader ? shader : defaultShader;
        currentShader->bind();
    }
    void addSprite(const float2& pos, const float2& size, const float4& col={1,1,1,1}, const float2& tmin={0,0}, const float2& tmax={1,1})
    {
        vertices[numQuads * 4 + 0] = pos;
        vertices[numQuads * 4 + 1] = pos + float2(size.x,0);
        vertices[numQuads * 4 + 2] = pos + float2(0,size.y);
        vertices[numQuads * 4 + 3] = pos + size;

        texcoords[numQuads * 4 + 0] = { tmin.x, tmin.y };
        texcoords[numQuads * 4 + 1] = { tmax.x, tmin.y };
        texcoords[numQuads * 4 + 2] = { tmin.x, tmax.y };
        texcoords[numQuads * 4 + 3] = { tmax.x, tmax.y };

        colours[numQuads * 4 + 0] = col;
        colours[numQuads * 4 + 1] = col;
        colours[numQuads * 4 + 2] = col;
        colours[numQuads * 4 + 3] = col;

        indices[numQuads * 6 + 0] = numQuads * 4 + 0;
        indices[numQuads * 6 + 1] = numQuads * 4 + 1;
        indices[numQuads * 6 + 2] = numQuads * 4 + 2;
        indices[numQuads * 6 + 3] = numQuads * 4 + 1;
        indices[numQuads * 6 + 4] = numQuads * 4 + 3;
        indices[numQuads * 6 + 5] = numQuads * 4 + 2;

        numQuads++;
    }
    void end()
    {
        GLint dims[4];
        glGetIntegerv(GL_VIEWPORT, dims);

        matrix view = matrix::lookAt(float3(0, 0, 100), float3(0, 0, 0), float3(0, 1, 0));
        matrix proj = matrix::ortho(dims[2], dims[3], 1, 1024);

        currentShader->setTexture2D("diffuseMap", texture);
        currentShader->set("View", view);
        currentShader->set("Proj", proj);

        posBuf->write(vertices, sizeof(float2)*numQuads*4);
        texBuf->write(texcoords, sizeof(float2)*numQuads*4);
        colBuf->write(colours, sizeof(float4)*numQuads*4);
        indexBuf->write(indices, sizeof(ushort)*numQuads*6);

        array->bind();
        glDrawElements(GL_TRIANGLES, numQuads*6, GL_UNSIGNED_SHORT, nullptr);
        array->unbind();
    }
};

#endif