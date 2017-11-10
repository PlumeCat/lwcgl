#ifndef _CUBE_GRAPHICS_BUFFER_H
#define _CUBE_GRAPHICS_BUFFER_H

struct VertexAttr
{
    string name;
    int bindPos;
    int bindCount;
    int glType;
    bool normalized;
    int stride;
    void* offset;

    VertexAttr(const string& _name,
                int _bindpos,
                int _bindcount,
                int _gltype,
                bool _normalize,
                int _stride,
                void* _offset):
        name(_name),
        bindPos(_bindpos),
        bindCount(_bindcount),
        glType(_gltype),
        normalized(_normalize),
        stride(_stride),
        offset(_offset) {}
};

struct Buffer
{
    GLuint type = 0;
    GLuint buffer = 0;

    Buffer(void* data=nullptr, uint size=0, bool ibuf=false)
    {
        type = ibuf ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
        glGenBuffers(1, &buffer);
        if (data)
        {
            write(data, size);
        }
    }
    ~Buffer()
    {
        glDeleteBuffers(1, &buffer);
    }

    void write(void* data, uint size)
    {
        glBindBuffer(type, buffer);
        glBufferData(type, size, data, GL_STATIC_DRAW);
        glBindBuffer(type, 0);
    }
};
struct VertexArray
{
    GLuint vao = 0;

    VertexArray(const vector<VertexAttr>& attrs, const vector<Buffer*>& bufs, Buffer* ibuf)
    {
        // create and bind new VAO object
        glGenVertexArrays(1, &vao);
        bind();

        // bind the vertex buffers
        for (int i = 0; i < attrs.size(); i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER, bufs[i]->buffer);
            glVertexAttribPointer(attrs[i].bindPos, attrs[i].bindCount, attrs[i].glType, attrs[i].normalized, attrs[i].stride, attrs[i].offset);
            glEnableVertexAttribArray(attrs[i].bindPos);
        }

        // bind the index buffer if one was provided
        if (ibuf)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf->buffer);
        }

        unbind();
    }
    ~VertexArray()
    {
        glDeleteVertexArrays(1, &vao);
    }

// private:
//     struct ScopedBind
//     {
//         ScopedBind(const VertexArray& va)
//         {
//             glBindVertexArray(va->vao);
//         }
//         ScopedBind(ScopedBind&&) {}
//         ScopedBind(const ScopedBind&) = delete;
//         ~ScopedBind()
//         {
//             glBindVertexArray(0);
//         }
//         ScopedBind& operator = (const ScopedBind&) = delete;
//         ScopedBind& operator = (ScopedBind&& b) {}
//     };
// public:
//     ScopedBind bindScope()
//     {
//         return ScopedBind(*this);
//     }
    void bind()
    {
        glBindVertexArray(vao);
    }
    void unbind()
    {
        glBindVertexArray(0);
    }
};

#endif