#ifndef _MESH_H
#define _MESH_H

struct MeshMaterial
{
};
struct MeshSubset
{
	uint startIndex;
	uint numIndices;
	uint materialId;
};
struct MeshLoader
{
	vector<float3> vertices;
	vector<float3> normals;
	vector<float2> texcoords;
	vector<ushort> indices;
	vector<MeshSubset> subsets;
	vector<MeshMaterial> materials;
	bool isLoaded;

	bool parseFloat2(const string& line, float2& f)
	{
		int space1 = line.find(' ');
		int space2 = line.find(' ', space1+1);
		if (space1 == string::npos || space2 == string::npos)
		{
			return false;
		}
		const char* s1 = &line[space1+1];
		const char* s2 = &line[space2+1];
		f.x = strtod(s1, nullptr);
		f.y = strtod(s2, nullptr);

		return true;
	}
	bool parseFloat3(const string& line, float3& f)
	{
		int space1 = line.find(' ');
		int space2 = line.find(' ', space1+1);
		int space3 = line.find(' ', space2+1);
		if (space1 == string::npos || space2 == string::npos || space3 == string::npos)
		{
			return false;
		}

		const char* s1 = &line[space1+1];
		const char* s2 = &line[space2+1];
		const char* s3 = &line[space3+1];
		f.x = strtod(s1, nullptr);
		f.y = strtod(s2, nullptr);
		f.z = strtod(s3, nullptr);

		return true;
	}
	bool readFace(const string& line, vector<string>& verts)
	{
		auto ss = line.find(' ');
		auto s0 = ss+1;
		auto e0 = line.find(' ', s0);

		auto sn = e0 + 1;
		auto en = line.find(' ', sn);

		bool end = false;

		do
		{
			auto s1 = sn;
			auto e1 = en;
			sn = e1+1;
			en = line.find(' ', sn);

			if (en == string::npos)
			{
				en = line.size();
				end = true;
			}

			verts.push_back(line.substr(s0, e0 - s0));
			verts.push_back(line.substr(s1, e1 - s1));
			verts.push_back(line.substr(sn, en - sn));
		}
		while (!end);

		return true;
	}

	MeshLoader(const string& fname)
	{
		isLoaded = false;

		vector<string> lines;
		readTextFile(fname, lines);

		vector<float3> pos;
		vector<float3> norm;
		vector<float2> tex;
		vector<string> vertstr;
		map<string, ushort> verttoindex;

		for (auto line : lines)
		{

			if (line[0] == 'v' && line[1] == 't')
			{
				float2 f;
				if (!parseFloat2(line, f))
				{
					return;
				}
				tex.push_back(f);
			}
			else if (line[0] == 'v' && line[1] == 'n')
			{
				float3 f;
				if (!parseFloat3(line, f))
				{
					return;
				}
				norm.push_back(f);
			}
			else if (line[0] == 'v' && line[1] == ' ')
			{
				float3 f;
				if(!parseFloat3(line, f))
				{
					return;
				}
				pos.push_back(f);
			}
			else if (line[0] == 'f')
			{
				readFace(line, vertstr);
			}
		}

		if (!norm.size())
		{
			cout << "    warning: normals not detected" << endl;
		}
		if (!tex.size())
		{
			cout << "    warning: texcoords not detected" << endl;
		}

		for (auto v : vertstr)
		{
			auto iter = verttoindex.find(v);
			if (iter != verttoindex.end())
			{
				indices.push_back(iter->second);
			}
			else
			{
				ushort i = verttoindex.size();
				indices.push_back(i);
				verttoindex[v] = i;

				int posind = -1;
				int normind = -1;
				int texind = -1;

				auto sl1 = v.find('/');
				auto sl2 = v.find('/', sl1+1);
				if (sl2 != string::npos && sl2 != sl1+1)
				{
					// normal
					posind = (int)strtod(&v[0], nullptr) - 1;
					texind = (int)strtod(&v[sl1+1], nullptr) - 1;
					normind = (int)strtod(&v[sl2+1], nullptr) - 1;
				}
				else
				{
					posind = (int)strtod(&v[0], nullptr) - 1;
					texind = (int)strtod(&v[sl1+2], nullptr) - 1;
				}

				vertices.push_back(pos[posind]);
				normals.push_back((normind == -1) ? float3() : norm[normind]);
				texcoords.push_back(tex[texind]);
			}
		}

		isLoaded = true;
	}
};
struct MeshBuilder
{
	vector<float3> vertices;
	vector<float3> normals;
	vector<float2> texcoords;
	vector<ushort> indices;
	vector<MeshSubset> subsets;
	vector<MeshMaterial> material;

	void addSkybox(float radius)
	{
		vertices = {
			float3(-radius, -radius, -radius),
			float3(-radius, -radius, radius),
			float3(radius, -radius, -radius),
			float3(radius, -radius, radius),

			float3(-radius, radius, -radius),
			float3(-radius, radius, radius),
			float3(radius, radius, -radius),
			float3(radius, radius, radius),
		};
		normals = {
			normalize(float3(-1, -1, -1)),
			normalize(float3(-1, -1, 1)),
			normalize(float3(1, -1, -1)),
			normalize(float3(1, -1, 1)),

			normalize(float3(-1, 1, -1)),
			normalize(float3(-1, 1, 1)),
			normalize(float3(1, 1, -1)),
			normalize(float3(1, 1, 1)),
		};
		texcoords = {
			float2(0, 0),
			float2(1, 0),
			float2(0, 1),
			float2(1, 1),
			float2(0, 0),
			float2(1, 0),
			float2(0, 1),
			float2(1, 1),
		};
		indices = {
			0, 1, 2, 1, 3, 2,
			4, 6, 5, 5, 6, 7,

			0, 4, 1, 1, 4, 5,
			2, 3, 6, 3, 7, 6,

			0, 2, 4, 2, 6, 4,
			1, 5, 3, 3, 5, 7,
		};
	}
};
struct Mesh
{
private:
	GLuint vertexBuf;
	GLuint normalBuf;
	GLuint texBuf;
	GLuint indexBuf;
	GLuint vao;
	vector<MeshSubset> subsets;

	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
public:
	static Mesh* createFromFile(const string& fname)
	{
		Mesh* mesh = new Mesh();
		mesh->loadObj(fname);
		VertexAttr attrs[] = {
			{ 0, 3, GL_FLOAT, false, 0, nullptr },
			{ 1, 3, GL_FLOAT, false, 0, nullptr },
			{ 2, 2, GL_FLOAT, false, 0, nullptr },
		};
		mesh->vao = createVertexArray(attrs, { mesh->vertexBuf, mesh->normalBuf, mesh->texBuf }, mesh->indexBuf);

		return mesh;
	}
	static Mesh* createSkybox()
	{

	}
	static Mesh* create(MeshBuilder& builder)
	{
		Mesh* mesh = new Mesh();

		mesh->vertexBuf = createBuffer(GL_ARRAY_BUFFER, &builder.vertices[0], sizeof(float3)*builder.vertices.size(), GL_STATIC_DRAW);
		mesh->texBuf = createBuffer(GL_ARRAY_BUFFER, &builder.texcoords[0], sizeof(float2)*builder.texcoords.size(), GL_STATIC_DRAW);
		mesh->normalBuf = createBuffer(GL_ARRAY_BUFFER, &builder.normals[0], sizeof(float3)*builder.normals.size(), GL_STATIC_DRAW);
		mesh->indexBuf = createBuffer(GL_ELEMENT_ARRAY_BUFFER, &builder.indices[0], sizeof(ushort)*builder.indices.size(), GL_STATIC_DRAW);

		MeshSubset s;
		s.startIndex = 0;
		s.numIndices = builder.indices.size();
		s.materialId = 0;
		mesh->subsets.push_back(s);

		VertexAttr attrs[] = {
			{ 0, 3, GL_FLOAT, false, 0, nullptr },
			{ 1, 3, GL_FLOAT, false, 0, nullptr },
			{ 2, 2, GL_FLOAT, false, 0, nullptr },
		};
		mesh->vao = createVertexArray(attrs, { mesh->vertexBuf, mesh->normalBuf, mesh->texBuf }, mesh->indexBuf);

		return mesh;
	}

	void loadObj(const string& fname)
	{
		MeshLoader loader(fname);

		if (loader.isLoaded)
		{
			vertexBuf = createBuffer(GL_ARRAY_BUFFER, &loader.vertices[0], sizeof(float3)*loader.vertices.size(), GL_STATIC_DRAW);
			texBuf = createBuffer(GL_ARRAY_BUFFER, &loader.texcoords[0], sizeof(float2)*loader.texcoords.size(), GL_STATIC_DRAW);
			normalBuf = createBuffer(GL_ARRAY_BUFFER, &loader.normals[0], sizeof(float3)*loader.normals.size(), GL_STATIC_DRAW);
			indexBuf = createBuffer(GL_ELEMENT_ARRAY_BUFFER, &loader.indices[0], sizeof(ushort)*loader.indices.size(), GL_STATIC_DRAW);

		}

		cout << "loaded mesh:  " << fname << "\n";
		cout << "    vertices: " << loader.vertices.size() << "\n";
		cout << "    indices:  " << loader.indices.size() << endl;

		MeshSubset sub;
		sub.startIndex = 0;
		sub.numIndices = loader.indices.size();
		sub.materialId = 0;
		subsets.push_back(sub);
	}

	void drawSubset(int subset)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, subsets[0].numIndices, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}
	void drawSubsetInstanced(int subset, int count)
	{
		glBindVertexArray(vao);
		glDrawElementsInstanced(GL_TRIANGLES, subsets[0].numIndices, GL_UNSIGNED_SHORT, 0, count);
		glBindVertexArray(0);
	}
};

#endif