#ifndef _CUBE_MESH_H
#define _CUBE_MESH_H

#include "../definitions.h"
#include "../xml/src/xml.h"
#include "mesh.h"

struct MeshSubset
{
    uint startIndex;
    uint numIndices;
    uint materialId;
};
struct Mesh
{
    Buffer* posBuf = nullptr;
    Buffer* normalBuf = nullptr;
    Buffer* texBuf = nullptr;
    Buffer* indexBuf = nullptr;
    VertexArray* array = nullptr;
    vector<MeshSubset> subsets;

    Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator = (const Mesh&) = delete;

    // can be provided to render functions to set up state per-subset
    typedef void(*SubsetRenderCallback)(const MeshSubset&);

    ~Mesh()
    {
        delete posBuf;
        delete normalBuf;
        delete texBuf;
        delete indexBuf;
        delete array;
    }

    void render()
    {
        array->bind();
        for (auto& s : subsets)
        {
            glDrawElements(GL_TRIANGLES, s.numIndices, GL_UNSIGNED_SHORT, (void*)(s.startIndex * sizeof(ushort)));
        }
        array->unbind();
    }
    template<typename Callback>
    void render(Callback callback)
    {
        array->bind();
        for (auto& s : subsets)
        {
            callback(s);
            glDrawElements(GL_TRIANGLES, s.numIndices, GL_UNSIGNED_SHORT, (void*)(s.startIndex * sizeof(ushort)));
        }
        array->unbind();
    }

    void renderInstanced(int numInstances)
    {
        array->bind();
        for (auto& s : subsets)
        {
            glDrawElementsInstanced(GL_TRIANGLES, s.numIndices, GL_UNSIGNED_SHORT,
                (void*)(s.startIndex * sizeof(ushort)), numInstances);
        }
        array->unbind();
    }
    template<typename Callback>
    void renderInstanced(int numInstances, Callback callback)
    {
        array->bind();
        for (auto& s : subsets)
        {
            callback(s);
            glDrawElementsInstanced(GL_TRIANGLES, s.numIndices, GL_UNSIGNED_SHORT,
                (void*)(s.startIndex * sizeof(ushort)), numInstances);

        }
        array->unbind();
    }
};

struct Skeleton
{
    vector<Skeleton*> children;
};

class MeshBuilder
{
    vector<float3> vertices;
    vector<float3> normals;
    vector<float2> texcoords;
    vector<ushort> indices;
    vector<MeshSubset> subsets;

public:
    void begin()
    {
        clear();
    }
    void geometry(const float3* vert, const float3* norm, const float2* tex, const ushort* ind, int numvert, int numind, int materialId=0)
    {
        // create new subset
        subsets.push_back({ indices.size(), numind, materialId });

        // add indices, offset by previous number of vertices
        int v = vertices.size();
        for (int i = 0; i < numind; i++)
        {
            indices.push_back(ind[i] + v);
        }

        // add vertices
        for (int i = 0; i < numvert; i++)
        {
            vertices.push_back(vert[i]);
            normals.push_back(norm[i]);
            texcoords.push_back(tex[i]);
        }

    }
    void sphere(const float3& center, float radius, int sectors, int rings, int materialId=0)
    {
        int numVertices = sectors * (rings+1);
        int numIndices = sectors * rings * 6;

        float3* vertices = new float3[numVertices];
        float3* normals = new float3[numVertices];
        float2* texcoords = new float2[numVertices];
        ushort* indices = new ushort[numIndices];

        for (int i = 0; i < sectors; i++)
        {
            for (int j = 0; j < rings+1; j++)
            {
                float u = ((float)i / (float)sectors);
                float v = ((float)j / (float)rings+1);
                float lon = u * pi * 2;
                float lat = (v - 0.5f) * pi;
                float x = sin(lon) * cos(lat);
                float y = sin(lat);
                float z = cos(lon) * cos(lat);

                vertices[i*(rings+1)+j]  = float3(x, y, z) * radius;
                normals[i*(rings+1)+j]   = float3(x, y, z);
                texcoords[i*(rings+1)+j] = float2(u, v);
            }
        }

        for (int i = 0; i < sectors; i++)
        {
            for (int j = 0; j < rings; j++)
            {
                int k = (i+1) % sectors;
                indices[(i*rings+j)*6+0] = i * (rings+1) + j;
                indices[(i*rings+j)*6+1] = i * (rings+1) + j+1;
                indices[(i*rings+j)*6+2] = k * (rings+1) + j;
                indices[(i*rings+j)*6+3] = k * (rings+1) + j;
                indices[(i*rings+j)*6+4] = i * (rings+1) + j+1;
                indices[(i*rings+j)*6+5] = k * (rings+1) + j+1;
            }
        }

        geometry(vertices, normals, texcoords, indices, numVertices, numIndices, materialId);

        delete[] vertices;
        delete[] normals;
        delete[] texcoords;
        delete[] indices;
    }
    void box(const float3& min, const float3& max, const float2& tmin, const float2& tmax, int materialId=0)
    {
        float3 vert[] = {
			// left
			float3(min.x, min.y, min.z),
			float3(min.x, min.y, max.z),
			float3(min.x, max.y, min.z),
			float3(min.x, max.y, max.z),

			// right
			float3(max.x, min.y, min.z),
			float3(max.x, min.y, max.z),
			float3(max.x, max.y, min.z),
			float3(max.x, max.y, max.z),

			// bottom
			float3(min.x, min.y, min.z),
			float3(min.x, min.y, max.z),
			float3(max.x, min.y, min.z),
			float3(max.x, min.y, max.z),

			// top
			float3(min.x, max.y, min.z),
			float3(min.x, max.y, max.z),
			float3(max.x, max.y, min.z),
			float3(max.x, max.y, max.z),

			// front
			float3(min.x, max.y, min.z),
			float3(max.x, max.y, min.z),
			float3(min.x, min.y, min.z),
			float3(max.x, min.y, min.z),

			// back
			float3(max.x, min.y, max.z),
			float3(max.x, max.y, max.z),
			float3(min.x, min.y, max.z),
			float3(min.x, max.y, max.z),
		};
		float3 norm[] = {
			float3(-1, 0, 0), float3(-1, 0, 0), float3(-1, 0, 0), float3(-1, 0, 0),
			float3(1, 0,  0), float3( 1, 0, 0),  float3(1, 0, 0),  float3(1, 0, 0),
			float3(0, -1, 0), float3(0, -1, 0), float3(0, -1, 0), float3(0, -1, 0),
			float3(0, 1,  0), float3(0,  1, 0),  float3(0, 1, 0),  float3(0, 1, 0),
			float3(0, 0, -1), float3(0, 0, -1), float3(0, 0, -1), float3(0, 0, -1),
			float3(0, 0,  1), float3(0, 0,  1), float3(0, 0,  1), float3(0, 0,  1),
		};
		float2 tex[] = {
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
			tmin, float2(tmax.x, tmin.y), float2(tmin.x, tmax.y), tmax,
		};
		ushort ind[] = {
			0, 1, 2, 1, 3, 2,
			4, 6, 5, 5, 6, 7,
			8, 10,9, 9, 10,11,
			12,13,14,13,15,14,
			16,17,18,17,19,18,
			20,21,22,21,23,22,
		};

		geometry(vert, norm, tex, ind, 24, 36, materialId);
    }
    void prism(const vector<float3>& points, const float3& origin, const float3& extr, const float2& tmin, const float2& tmax)
    {
        // "points" must be vertices of a convex polygon
        // TODO: triangulate with ear clipping (allows concavities)
        // TODO: remove degenerate/interior points, etc (?)

        // the prism is composed of 2 "faces" (convex N-gon) and a "ring" of N quads joining them
        // for the 2 faces:
        //      2N vertices (1 vertex per point per face)
        //      2N-2 triangles = 6(N-2) indices
        // for the ring:
        //      4N vertices (2 vertices per quad, 2 quads per point)
        //      6N indices (6 indices per quad)
        // total:
        //      6N vertices
        //      12N-12 indices

        float3* vertices = new float3[points.size() * 6];
        float3* normals = new float3[points.size() * 6];
        float2* texcoords = new float2[points.size() * 6];
        ushort* indices = new ushort[points.size() * 12 - 12];

        for (int i = 0; i < points.size(); i++)
        {
            // vertices for top "face"
            // vertices for bottom "face"
        }


        // vertices for ring
        
        // indices for top "face"

        // indices for bottom "face" (reversed?)

        // indcices for ring

        delete[] vertices;
        delete[] indices;
        delete[] normals;
        delete[] texcoords;
    }
    Mesh* end(const vector<VertexAttr>& attrs)
    {
        Mesh* mesh = new Mesh();
        mesh->posBuf = new Buffer(&vertices[0], sizeof(float3)*vertices.size());
        mesh->normalBuf = new Buffer(&normals[0], sizeof(float3)*normals.size());
        mesh->texBuf = new Buffer(&texcoords[0], sizeof(float2)*texcoords.size());
        mesh->indexBuf = new Buffer(&indices[0], sizeof(ushort)*indices.size(), true);
        mesh->array = new VertexArray(attrs, { mesh->posBuf, mesh->normalBuf, mesh->texBuf }, mesh->indexBuf);
        mesh->subsets = subsets;
        
        return mesh;
    }
    void clear()
    {
        vertices.clear();
        normals.clear();
        texcoords.clear();
        indices.clear();
        subsets.clear();
    }
};


class MeshManager
{
    string baseDir;

    map<string, Mesh*> meshes;
    map<string, Skeleton*> skeletons;

    void loadMesh(const string& fname)
    {
        try
        {
            Mesh* mesh = nullptr;
            Skeleton* skeleton = nullptr;
            
            xml::Element document;
            xml::load(baseDir + fname, document);

            // do things to doc... terrible things...

            meshes[fname] = mesh;
            skeletons[fname] = skeleton;
        }
        catch (const exception& e)
        {
            cout << "   failed to load: " << fname << "(" << e.what() << ")" << endl;
        }
    }

public:
    MeshManager(const string& dir=string(RESOURCE_BASE)+"/meshes") : baseDir(dir) {}
    Mesh* getMesh(const string& fname)
    {
        cout << "loading mesh: " << fname << endl;

        auto mesh = meshes.find(fname);
        if (mesh == meshes.end())
        {
            loadMesh(fname);
            return meshes[fname];
        }
        else
        {
            cout << "    using cache" << endl;
        }
    }
    Skeleton* getSkeleton(const string& fname)
    {
        cout << "loading skeleton: " << fname << endl;
        auto skeleton = skeletons.find(fname);
        if (skeleton == skeletons.end())
        {
            loadMesh(fname);
            return skeletons[fname];
        }
        else
        {
            cout << "    using cache" << endl;
        }
    }
};

#endif