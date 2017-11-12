#ifndef _CUBE_GRAPHICS_BUFFER_H
#define _CUBE_GRAPHICS_BUFFER_H

struct VertexAttr
{
    string name;
    string shaderType;
    int bindPos = 0;
    int bindCount = 1;
    int glType = GL_FLOAT;
    int normalized = GL_FALSE;
    int stride = 0;
    void* offset = nullptr;
    
    static string getShaderType(int _normalized, int _glType, int _bindCount)
    {
        return (_normalized || _glType == GL_DOUBLE || _glType == GL_FLOAT || _glType == GL_HALF_FLOAT) ? 
            ((_glType == GL_DOUBLE) ? 
                ((_bindCount > 1) ? "dvec"+to_string(_bindCount) : "double"):
                ((_bindCount > 1) ? "vec"+to_string(_bindCount) : "float")):
            ((_glType == GL_INT || _glType == GL_SHORT || _glType == GL_BYTE) ? 
                ((_bindCount > 1) ? "ivec"+to_string(_bindCount) : "int"):
                ((_bindCount > 1) ? "uvec"+to_string(_bindCount) : "uint"));
    }
    static int getNormalization(const string& decl)
    {
        return (decl[decl.length()-1] == 'n') ? GL_TRUE : GL_FALSE;
    }

    VertexAttr(const string& _name,
                int _bindpos,
                int _bindcount,
                int _gltype,
                int _normalize,
                int _stride=0,
                void* _offset=nullptr):
        name(_name),
        bindPos(_bindpos),
        bindCount(_bindcount),
        glType(_gltype),
        normalized(_normalize),
        stride(_stride),
        offset(_offset),
        shaderType(getShaderType(_normalize, _gltype, _bindcount))
        {}
    VertexAttr(const string& _name, const string& _decl, int _bindpos):
        name(_name),
        stride(0),
        bindPos(_bindpos),
        offset(nullptr)
    {
        // _decl must be one of
        // 
        //      [u]byte[2,3,4][n]
        //      [u]short[2,3,4][n]
        //      [u]int[2,3,4][n]
        //      half[2,3,4]
        //      float[2,3,4]
        //      double[2,3,4]
        // 
        // we will quickly parse it to fill out (normalized, bindCount, glType)
        normalized = getNormalization(_decl);
        
        // find the first digit
        auto digit = _decl.find_first_of("234");

        // decide bind count and normalized
        bindCount = (digit == string::npos) ? 1 : stoi(_decl.substr(digit, 1));
        
        // decide input type ("gltype")
        string format = _decl.substr(0, min(normalized ? _decl.size()-1 : _decl.size(), digit));
        if (format == "byte")        { glType = GL_BYTE; }
        else if (format  == "ubyte") { glType = GL_UNSIGNED_BYTE; }
        else if (format == "short")  { glType = GL_SHORT; }
        else if (format == "ushort") { glType = GL_UNSIGNED_SHORT; }
        else if (format == "int")    { glType = GL_INT; }
        else if (format == "uint")   { glType = GL_UNSIGNED_INT; }
        else if (format == "half")   { glType = GL_HALF_FLOAT; }
        else if (format == "float")  { glType = GL_FLOAT; }
        else if (format == "double") { glType = GL_DOUBLE; }
        else { glType = GL_FLOAT; }

        // decide shader type
        shaderType = getShaderType(normalized, glType, bindCount);

    }
    string getShaderDecl()
    {
        return "in " + shaderType + " " + name + ";";
    }
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