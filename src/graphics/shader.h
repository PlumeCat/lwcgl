#ifndef _CUBE_SHADER_H
#define _CUBE_SHADER_H

#include <algorithm>


struct Shader
{
    GLuint program = 0;
    GLuint vs = 0;
    GLuint ps = 0;

    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void bind() { glUseProgram(program); }

    void set(const string& name, float f)              { glUniform1fv(      glGetUniformLocation(program, name.c_str()), 1, &f); }
    void set(const string& name, const float4& f)      { glUniform4fv(      glGetUniformLocation(program, name.c_str()), 1, (float*)(void*)&f); }
    void set(const string& name, const matrix& mat)    { glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, mat.m); }
    template<int n> void set(const string& name, const float4 (&f)[n]) { glUniform4fv(      glGetUniformLocation(program, name.c_str()), n, (float*)(void*)f); }

    void setTexture2D(const string& name, GLuint texture, uint slot=0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, name.c_str()), slot);
    }
    void setTexture2D(const string& name, Texture* texture, uint slot=0)
    {
        setTexture2D(name, texture->texture, slot);
    }
    void setTexture2D(const string& name, Surface* surface, uint slot=0)
    {
        setTexture2D(name, surface->texture, slot);
    }
    void setTextureCube(const string& name, Texture* texture, uint slot=0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->texture);
        glUniform1i(glGetUniformLocation(program, name.c_str()), slot);
    }

};



class ShaderSource
{
    string sourceCode;
    string fileDir;
    string fileName;

    vector<string> includedList;
    string version = "";
    string vertexDef = "";
    GLuint shaderType = 0;

    GLuint glShader = 0;


    string preprocess(const string& line)
    {
        if (line.substr(0, 8) == "#include")
        {
            // get the filename
            auto q1 = line.find('"', 8);
            if (q1 == string::npos)
            {
                cout << "    invalid include: " << line << endl;
                return "";
            }
            auto q2 = line.find('"', q1+1);
            if (q2 == string::npos)
            {
                cout << "    invalid include: " << line << endl;
                return "";
            }

            string includeFile = line.substr(q1+1, q2-q1-1);
            cout << "    including: (" << includeFile << ")" << endl;

            return include(includeFile);
        }
        else if (line.substr(0, 7) == "#vertex")
        {
            // insert vertex definition
            if (vertexDef != "")
            {
                return "";
            }
            else
            {
                cout << "    vertex definition: (" << line << ")" << endl;
                vertexDef = line;
                // TODO: generate and inject the vertex declaration into the shader code before compiling
                // return join("\n", vertexDefinitions.map(toString));
                return "";
            }
        }
        else if (line.substr(0, 8) == "#version")
        {
            if (version != "")
            {
                // eliminate duplicate #version directives
                return "";
            }
            else
            {
                cout << "    version: (" << line << ")" << endl;
                version = line;
                return line;
            }
        }
        else
        {
            // unknown directives are removed
            cout << "    unknown directive: (" << line << ")" << endl;
            return "";
        }

        return "";
    }
    string include(const string& fname)
    {
        // handle include file
        // make sure we don't include a file twice
        if (find(includedList.begin(), includedList.end(), fname) != includedList.end())
        {
            cout << "   file was already included: " << endl;
            return "";
        }

        // we push to "includedList" here before calling preprocess()
        // protects against circular includes
        includedList.push_back(fname);

        // grab the included file
        vector<string> sourceLines;
        readTextFile(fileDir + "/" + fname, sourceLines);
        string source;

        // iterate over lines
        for (auto& line : sourceLines)
        {
            if (line[0] == '#')
            {
                line = preprocess(line);
            }
            source += line + "\n";
        }

        return source;
    }

    void compile()
    {
        // cout << "Source: " << src << endl;
        const char* csrc = sourceCode.c_str();
        const GLint len = -1;
        cout << "    compiling..." << endl;
        glShader = glCreateShader(shaderType);
        glShaderSource(glShader, 1, &csrc, &len);
        glCompileShader(glShader);

        // output errors/info of compilation
        GLint infolen;
        glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &infolen);
        if (infolen)
        {
            char* logs = new char[infolen];
            glGetShaderInfoLog(glShader, infolen, nullptr, logs);
            cout << "    debug logs: " << logs << endl;
            delete[] logs;
        }

        cout << "    done" << endl;
    }
public:
    void load(const string& dir, const string& fname, GLuint type)
    {
        fileDir = dir;
        fileName = fname;
        shaderType = type;

        cout << "    shader source: " << fileDir << "/" << fname << endl;

        sourceCode = include(fname);
        compile();
    }
    inline string getSource() { return sourceCode; }
    inline string getVertexDef() { return vertexDef; }
    inline GLuint getShader() { return glShader; }
};


class ShaderManager
{
    string baseDir;
    map<string, ShaderSource> vshaders;
    map<string, ShaderSource> pshaders;
    map<string, vector<VertexAttr>> vertexTypes;

    GLuint buildProgram(GLuint vs, GLuint ps, const vector<VertexAttr>& attrs)
    {
        // create the program
        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, ps);

        // attach vertex attributes
        for (auto& a : attrs)
        {
            glBindAttribLocation(program, a.bindPos, a.name.c_str());
        }

        glLinkProgram(program);
        glDetachShader(program, vs);
        glDetachShader(program, ps);

        return program;
    }

    void readVertexFile(const string& attrfile)
    {
        // read vertex definitions file
        vector<string> lines;
        readTextFile(attrfile, lines);

        if (!lines.size())
        {
            throw runtime_error("error reading attribute file");
        }
        for (auto& line: lines)
        {
            string l = strip(line);
            if (!l.length())
            {
                continue;
            }

            string currentVertexName;

            if (l.find("VERTEX") == 0)
            {
                vector<string> tokens;
                split(l, ' ', tokens);
                if (tokens.size() != 2)
                {
                    throw runtime_error("error parsing attribute file: '" + l + "'");
                }

                // start a new vertex
                currentVertexName = tokens[1];
                vertexTypes[currentVertexName] = vector<VertexAttr>();
            }
            else if (l.find("ATTRIBUTE") == 0)
            {
                auto& attributeList = vertexTypes[currentVertexName];

                vector<string> tokens;
                split(l, ' ', tokens);
                if (tokens.size() != 3)
                {
                    throw runtime_error("error parsing attribute file: '" + l + "'");
                }

                string name = tokens[1];
                string stype = tokens[2];

                // int count = 1; // component count, eg float2 -> 2, uint4 -> 4
                // int type = GL_BYTE;
                // bool normalize = false;

                // // find the first digit
                // auto digit = stype.find_first_of("234");
                // if (digit != string::npos)
                // {
                //     count = stoi(stype.substr(digit, 1));
                //     auto n = stype.find("n", digit+1);
                //     if (n != string::npos)
                //     {
                //         normalize = true;
                //     }
                // }
                
                // string sformat = stype.substr(0, min(stype.size(), digit));

                int count = 1;
                int type = 0;
                bool normalize = false;

                if (stype == "byte4")
                {

                }
                else if (stype == "byte4n")
                {
                }
                else if (stype == "ubyte4")
                {

                }
                else if (stype == "ubyte4n")
                {
                }
                else if (stype == "short2")
                {

                }
                else if (stype == "short4")
                {
                    
                }

                // GL_BYTE
                // GL_UNSIGNED_BYTE
                // GL_SHORT
                // GL_UNSIGNED_SHORT
                // GL_INT
                // GL_UNSIGNED_INT
                // GL_FLOAT
                // GL_HALF_FLOAT
                // GL_DOUBLE

                // TODO: fill in attr
                attributeList.push_back(VertexAttr(name, attributeList.size()-1, count, type, normalize, 0, nullptr));

            }
        }
    }

public:
    ShaderManager(const string& dir="resource/shaders", const string& attrfile="resource/vertex.txt")
    {
        baseDir = dir;
        readVertexFile(attrfile);
    }

    vector<VertexAttr>& getVertexAttrs(const string& vertexDef)
    {
        return vertexTypes[vertexDef];
    }

    Shader* getShader(const string& vsfile, const string& psfile)
    {
        ShaderSource vshader, pshader;
        string vertexDef;

        // load the vertex shader or use cached version
        auto vsiter = vshaders.find(vsfile);
        if (vsiter == vshaders.end())
        {
            vshader.load(baseDir, vsfile, GL_VERTEX_SHADER);
            vshaders[vsfile] = vshader;
        }
        else
        {
            vshader = vsiter->second;
        }

        // load pixel shader or use cached
        auto psiter = pshaders.find(psfile);
        if (psiter == pshaders.end())
        {
            pshader.load(baseDir, vsfile, GL_FRAGMENT_SHADER);
            pshaders[psfile] = pshader;
        }
        else
        {
            pshader = psiter->second;
        }

        // create new shader
        Shader* shader = new Shader();
        shader->vs = vshader.getShader();
        shader->ps = pshader.getShader();
        shader->program = buildProgram(vshader.getShader(), pshader.getShader(), getVertexAttrs(vshader.getVertexDef()));
        return shader;
    }

    void clear()
    {
        // TODO: some kind of detaching system
        // you can detach and delete shaders immediately after linking program, which frees a lot of memory up
        // glDetachShader / glDeleteShader I think

        for (auto &vs: vshaders)
        {
            glDeleteShader(vs.second.getShader());
        }
        vshaders.clear();

        for (auto& ps: pshaders)
        {
            glDeleteShader(ps.second.getShader());
        }
        pshaders.clear();
    }
};


#endif