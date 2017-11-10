#ifndef _CUBE_SURFACE_H
#define _CUBE_SURFACE_H

class Surface
{
public:
    uint width = 0;
    uint height = 0;
    uint defaultwidth = 0;
    uint defaultheight = 0;

    GLuint frameBuffer = 0;
    GLuint texture = 0;

    Surface(uint w, uint h, GLenum iformat=GL_RGBA8)
    {
        width = w;
        height = h;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLint dims[4] = { 0 };
        glGetIntegerv(GL_VIEWPORT, dims);
        defaultwidth = dims[2];
        defaultheight = dims[3];
    }
    ~Surface()
    {
        glDeleteTextures(1, &texture);
        glDeleteFramebuffers(1, &frameBuffer);
    }
    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glViewport(0, 0, width, height);
    }
    void unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,defaultwidth, defaultheight);
    }
};

#endif