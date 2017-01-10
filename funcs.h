#ifndef _FUNCS_H
#define _FUNCS_H

#include "math.h"
#include "include/soil.h"

void readTextFile(const string& fname, vector<string>& lines)
{
	ifstream file(fname.c_str());
	if (file.is_open())
	{
		while (!file.eof())
		{
			string line;
			getline(file, line);
			lines.push_back(line);
		}
	}
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

string readTextFile(const string& fname)
{
	ifstream file(fname.c_str());
	if (file.is_open())
	{
		string ret;
		while (!file.eof())
		{
			string line;
			getline(file, line);
			ret += line + "\n";
		}
		return ret;
	}
	else
	{
		return 0;
	}
}

GLuint loadCubeMap(const string (&fnames)[6])
{
	GLuint tex = SOIL_load_OGL_cubemap(
		fnames[0].c_str(),
		fnames[1].c_str(),
		fnames[2].c_str(),
		fnames[3].c_str(),
		fnames[4].c_str(),
		fnames[5].c_str(),
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (tex)
	{
		cout << "loaded cubemap: " << fnames[0] << "..." << endl;
	}
	else
	{
		cout << "failed to load cubemap: " << fnames[0] << "..." << endl;
	}
	return tex;
}

GLuint loadTexture(const string& fname)
{
	GLuint tex = SOIL_load_OGL_texture(
		fname.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
	);

	if (tex)
	{
		cout << "loaded texture: " << fname << endl;
	}
	else
	{
		cout << "failed to load texture: " << fname << endl;
	}
	return tex;
}


GLuint loadShader(uint type, const string& fname)
{
	GLuint shader = glCreateShader(type);
	string src = readTextFile(fname);
	const char* str = src.c_str();
	glShaderSource(shader, 1, &str, nullptr);
	glCompileShader(shader);

	// GLint infoLength;
	// glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
	// cout << "loaded shader: " << fname << ": " << shader << endl;
	// if (infoLength)
	// {
	// 	char* logs = new char[infoLength];
	// 	glGetShaderInfoLog(shader, infoLength, nullptr, logs);
	// 	cout << "    debug log: " << logs << endl;
	// 	delete[] logs;
	// }
	return shader;
}

GLuint createProgram(const string& vs, const string& ps, const map<uint, string>& attrs)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, loadShader(GL_VERTEX_SHADER, vs));
	glAttachShader(program, loadShader(GL_FRAGMENT_SHADER, ps));
	for (auto iter = attrs.begin(); iter != attrs.end(); iter++)
	{
		glBindAttribLocation(program, iter->first, iter->second.c_str());
	}
	glLinkProgram(program);
	cout << "loaded program: " << vs << " + " << ps << endl;
	return program;
}
void setTexture(GLuint program, const string& name, GLRESOURCE tex, int slot=0)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(program, name.c_str()), slot);
}
void setTextureCube(GLuint program, const string& name, GLRESOURCE tex, int slot=0)
{
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glUniform1i(glGetUniformLocation(program, name.c_str()), slot);
}
void setFloat4(GLuint program, const string& name, const float4& val)
{
	glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, (float*)(void*)&val);
}
void setMatrix(GLuint program, const string& name, const matrix& val)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, val.m);
}
void setMatrixArray(GLuint program, const string& name, void* m, int count)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), count, GL_FALSE, (float*)(void*)m);
}
GLuint createBuffer(GLuint type, void* data, uint size, GLuint drawType)
{
	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(type, buf);
	glBufferData(type, size, data, drawType);
	return buf;
}
template<typename Element, uint n>
GLuint createBuffer(GLuint type, const Element (&data)[n], GLuint drawType)
{
	return createBuffer(type, (void*)&data[0], sizeof(Element)*n, drawType);
}
void setBufferData(GLuint type, GLuint buf, void* data, uint size, GLuint drawType)
{
	glBindBuffer(type, buf);
	glBufferData(type, size, data, drawType);
	glBindBuffer(type, 0);
}
template<typename Element, uint n>
GLuint setBufferData(GLuint type, GLuint buf, const Element(&data)[n], GLuint drawType)
{
	return setBufferData(type, buf, (void*)&data[0], sizeof(Element)*n, drawType);
}

struct VertexAttr
{
	int bindPos;
	int bindCount;
	int glType;
	bool normalized;
	int stride;
	void* offset;

	VertexAttr(int _bindPos, int _bindCount, int _glType=GL_FLOAT, bool _norm=false, int _stride=0, void* _offset=nullptr)
		: bindPos(_bindPos), bindCount(_bindCount), glType(_glType), normalized(_norm), stride(_stride), offset(_offset) {}
};

template<int n>
GLRESOURCE createVertexArray(const VertexAttr (&attrs)[n], const GLRESOURCE (&vbufs)[n], GLRESOURCE ibuf)
{
	GLRESOURCE vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	for (int i = 0; i < n; i++) 
	{
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, vbufs[i]);
		glVertexAttribPointer(attrs[i].bindPos, attrs[i].bindCount, attrs[i].glType, attrs[i].normalized, attrs[i].stride, attrs[i].offset);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);

	glBindVertexArray(0);
	return vao;
}





#endif