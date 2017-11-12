#ifndef _CUBE_SHADER_H
#define _CUBE_SHADER_H

#include <algorithm>
#include "../definitions.h"

struct Shader
{
    GLuint program = 0;

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

class ShaderManager
{
    struct ShaderSource
    {
        vector<string> included; // the first line should be the actual filename
        string sourceCode;
        string version;
        string vertexDef;
        GLuint shader = 0;
        GLuint shaderType = 0;
    };
    string baseDir;
    // map<string, ShaderSource> vshaders;
    // map<string, ShaderSource> pshaders;
    map<string, ShaderSource> vertexShaders;
    map<string, ShaderSource> pixelShaders;
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
        
        string currentVertexName;

        for (auto& line: lines)
        {
            string l = strip(line);
            if (!l.length())
            {
                continue;
            }


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
                cout << "vertex type: " << currentVertexName << endl;
                vertexTypes[currentVertexName] = vector<VertexAttr>();
            }
            else if (l.find("ATTRIBUTE") == 0)
            {   
                vector<string> tokens;
                split(l, ' ', tokens);
                if (tokens.size() != 3)
                {
                    throw runtime_error("error parsing attribute file: '" + l + "'");
                }
                
                auto attr = VertexAttr(tokens[1], tokens[2], vertexTypes[currentVertexName].size());
                cout << "    " << currentVertexName << " attribute: " << attr.name << " " << attr.bindPos << " " << attr.bindCount << " " << attr.glType << " " << attr.shaderType << endl;
                vertexTypes[currentVertexName].push_back(attr);

            }
        }
    }


    string preprocessLine(const string& line, ShaderSource& source)
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
            return includeShaderFile(includeFile, source);
        }
        else if (line.substr(0, 7) == "#vertex")
        {
            // insert vertex definition
            if (source.vertexDef != "")
            {
                return "";
            }
            else
            {
                vector<string> tokens;
                split(line, ' ', tokens);
                source.vertexDef = tokens[1];
                cout << "    vertex definition: " << source.vertexDef << endl;

                // inject the vertex declaration
                string retval = "";
                auto attrs = getVertexAttrs(source.vertexDef);
                for (auto& attr : attrs)
                {
                    retval += attr.getShaderDecl() + "\n\n";
                }
                return retval;
            }
        }
        else if (line.substr(0, 8) == "#version")
        {
            if (source.version != "")
            {
                // eliminate duplicate #version directives
                return "";
            }
            else
            {
                cout << "    version: (" << line << ")" << endl;
                source.version = line;
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
    string includeShaderFile(const string& fname, ShaderSource& source)
    {
        cout << "    including: " << fname;
        // handle include file
        // make sure we don't include a file twice
        if (find(source.included.begin(), source.included.end(), fname) != source.included.end())
        {
            cout << " (already included)" << endl;
            return "";
        }
        cout << endl;

        // we push to "includedList" here before calling preprocess()
        // protects against circular includes
        source.included.push_back(fname);

        // grab the included file
        vector<string> sourceLines;
        if (!readTextFile(baseDir+"/"+fname, sourceLines))
        {
            cout << "    ERROR: could not open file " << fname << endl;
            return "";
        }

        string sourceCode = "";
        // iterate over lines and preprocess if necessary
        for (auto& line : sourceLines)
        {
            if (line[0] == '#')
            {
                line = preprocessLine(line, source);
            }
            sourceCode += line + "\n";
        }
        return sourceCode;
    }
    ShaderSource loadShader(const string& fname, GLuint shaderType)
    {
        // load shader source object
        ShaderSource source;
        cout << "loading shader: " << baseDir + "/" + fname << endl;
        source.shaderType = shaderType;
        source.sourceCode = includeShaderFile(fname, source);

        // compile
        const char* csrc = source.sourceCode.c_str();
        const GLint len = -1;
        cout << "    compiling..." << endl;
        source.shader = glCreateShader(source.shaderType);
        glShaderSource(source.shader, 1, &csrc, &len);
        glCompileShader(source.shader);
        
        // output errors/info of compilation
        GLint infolen;
        glGetShaderiv(source.shader, GL_INFO_LOG_LENGTH, &infolen);
        if (infolen)
        {
            char* logs = new char[infolen];
            glGetShaderInfoLog(source.shader, infolen, nullptr, logs);
            cout << "    debug logs: " << logs << endl;
            delete[] logs;
        }
        
        cout << "    done" << endl;
        
        return source;
    }

public:
    ShaderManager(const string& dir=string(RESOURCE_BASE)+"/shaders", const string& attrfile=string(RESOURCE_BASE)+"/vertex.txt")
    {
        baseDir = dir;

        try
        {
            readVertexFile(attrfile);
        }
        catch (const exception& e)
        {
            cout << "exception occurred reading the vertex attribute file: "<< attrfile << "\n" << e.what() << endl;
            exit(2);
        }
    }

    vector<VertexAttr>& getVertexAttrs(const string& vertexDef)
    {
        return vertexTypes[vertexDef];
    }

    Shader* getShader(const string& vsfile, const string& psfile)
    {
        // ShaderSource vshader, pshader;
        GLuint vshader = 0;
        GLuint pshader = 0;
        string vertexDef;

        // load the vertex shader or use cached version
        auto vsiter = vertexShaders.find(vsfile);
        if (vsiter == vertexShaders.end())
        {
            auto source = loadShader(vsfile, GL_VERTEX_SHADER);
            vertexShaders[vsfile] = source;
            vshader = source.shader;
            vertexDef = source.vertexDef;
        }
        else
        {
            vshader = vsiter->second.shader;
        }

        // load pixel shader or use cached
        auto psiter = pixelShaders.find(psfile);
        if (psiter == pixelShaders.end())
        {
            // pshader.load(baseDir, vsfile, GL_FRAGMENT_SHADER);
            // pshaders[psfile] = pshader;
            auto source = loadShader(psfile, GL_FRAGMENT_SHADER);
            pixelShaders[psfile] = source;
            pshader = source.shader;
        }
        else
        {
            pshader = psiter->second.shader;
        }

        // create new shader
        Shader* shader = new Shader();
        shader->program = buildProgram(vshader, pshader, getVertexAttrs(vertexDef));
        return shader;
    }

    void clear()
    {
        // TODO: some kind of detaching system
        // you can detach and delete shaders immediately after linking program
        // this which frees a lot of memory up
        // (glDetachShader, glDeleteShader)

        for (auto &vs: vertexShaders)
        {
            glDeleteShader(vs.second.shader);
        }
        vertexShaders.clear();

        for (auto& ps: pixelShaders)
        {
            glDeleteShader(ps.second.shader);
        }
        pixelShaders.clear();
    }
};


#endif