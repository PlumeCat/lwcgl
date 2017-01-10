#ifndef _SPRITE_H
#define _SPRITE_H

class Font
{
public:
	struct Character
	{
		float2 pos;
		float2 size;
		float2 offset;
		float2 advance;		
	};

private:

	Character characters[256];
	GLRESOURCE texture;
	float2 scale;
	float lineHeight;
	friend class Sprite;

	Font() = default;
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

public:
	static Font* create(const string& fname)
	{
		Font* font = new Font;
		vector<string> lines;
		readTextFile(fname, lines);

		float lineHeight;
		for (auto line : lines)
		{
			vector<string> tokens;
			split(line, ' ', tokens);
			if (tokens[0] == "common")
			{
				font->lineHeight = (float)strtod(&(tokens[1])[11], nullptr);
				font->scale.x = (float)strtod(&(tokens[3])[7], nullptr);
				font->scale.y = (float)strtod(&(tokens[4])[7], nullptr);
			}
			if (tokens[0] == "page")
			{
				int q1 = tokens[2].find('"')+1;
				int q2 = tokens[2].find('"', q1);
				font->texture = loadTexture("resources//"+tokens[2].substr(q1, q2-q1));
			}
			else if (tokens[0] == "char")
			{
				Character ch;
				int id = (int)strtod(&(tokens[1])[3], nullptr);
				ch.pos.x = (float)strtod(&(tokens[2])[2], nullptr);
				ch.pos.y = (float)strtod(&(tokens[3])[2], nullptr);
				ch.size.x = (float)strtod(&(tokens[4])[6], nullptr);
				ch.size.y = (float)strtod(&(tokens[5])[7], nullptr);
				ch.offset.x=(float)strtod(&(tokens[6])[8], nullptr);
				ch.offset.y=(float)strtod(&(tokens[7])[8], nullptr);
				ch.advance.x=(float)strtod(&(tokens[8])[9], nullptr);
				ch.advance.y=lineHeight;
				font->characters[id] = ch;
			}
		}

		return font;
	}
};

class Sprite
{
private:
	// persistent
	GLRESOURCE vbPos;
	GLRESOURCE vbCol;
	GLRESOURCE vbTex;
	GLRESOURCE ib;
	GLRESOURCE vao;
	uint maxQuads;

	// transient
	GLRESOURCE tex;
	GLRESOURCE program;
	matrix view;
	matrix proj;
	uint numQuads;
	float3* positions;
	float4* colours;
	float2* texcoords;
	ushort* indices;

	Sprite() = default;
	Sprite(const Sprite& other) = delete;
	Sprite& operator=(const Sprite& other) = delete;

public:
	static Sprite* create(uint maxQuads=1024)
	{
		maxQuads = min(maxQuads, 10000); // safely below 65k vertices

		Sprite* sprite = new Sprite;
		sprite->maxQuads = maxQuads;

		sprite->numQuads = 0;
		sprite->positions = new float3[maxQuads*4];
		sprite->texcoords = new float2[maxQuads*4];
		sprite->colours = new float4[maxQuads*4];
		sprite->indices = new ushort[maxQuads*6];
		sprite->vbPos = createBuffer(GL_ARRAY_BUFFER, sprite->positions, sizeof(float3)*maxQuads*4, GL_STATIC_DRAW);
		sprite->vbCol = createBuffer(GL_ARRAY_BUFFER, sprite->colours, sizeof(float4)*maxQuads*4, GL_STATIC_DRAW);
		sprite->vbTex = createBuffer(GL_ARRAY_BUFFER, sprite->texcoords, sizeof(float2)*maxQuads*4, GL_STATIC_DRAW);
		sprite->ib = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite->indices, sizeof(ushort)*maxQuads*6, GL_STATIC_DRAW);

		VertexAttr attributes[] = {
			{ 0, 3, GL_FLOAT, false, 0, nullptr },
			{ 1, 4, GL_FLOAT, false, 0, nullptr },
			{ 2, 2, GL_FLOAT, false, 0, nullptr },
		};
		sprite->vao = createVertexArray(attributes, { sprite->vbPos, sprite->vbCol, sprite->vbTex }, sprite->ib);

		return sprite;
	}
	~Sprite()
	{
		delete[] positions;
		delete[] texcoords;
		delete[] colours;
		delete[] indices;
	}

	void drawString(GLRESOURCE program, Font* font, const string& str, const float2& pos)
	{
		begin(program, font->texture, matrix::identity(), matrix::orthoOffCenter(0, 1920, 0, 1080, 1, 1024));
		float2 curPos = pos;
		for (int i = 0; i < str.size(); i++)
		{
			char c = str[i];
			if (c == '\n')
			{
				curPos.x = pos.x;
				curPos.y += font->characters['|'].size.y;
			}
			else if (c == ' ')
			{
				curPos.x += font->characters[32].advance.x;
			}
			else
			{
				Font::Character ch = font->characters[c];
				float2 tp(ch.pos.x / font->scale.x, 1 - (ch.pos.y+ch.size.y) / font->scale.y);
				float2 ts(ch.size.x/ font->scale.x, ch.size.y/ font->scale.y);
				addQuad(curPos+ch.offset, ch.size, tp, ts);
				curPos += ch.advance;
			}
		}
		end();
	}
	void begin(GLRESOURCE program, GLRESOURCE tex, const matrix& view, const matrix& proj)
	{
		this->view = view;
		this->proj = proj;
		this->tex = tex;
		this->program = program;
		this->numQuads = 0;
	}
	void addQuad(const float3& pos, const float2& size, const float2& texpos=float2(0,0), const float2& texsize=float2(1,1), const float4& col=float4(1,1,1,1))
	{
		float2 e = size/2;
		float3 _pos[] = {
			pos+float3(-e.x, -e.y, 0),
			pos+float3(e.x, -e.y, 0),
			pos+float3(-e.x, e.y, 0),
			pos+float3(e.x, e.y, 0),
		};
		float4 _col[] = { col, col, col, col, };
		float2 _tex[] = {
			texpos,
			texpos+float2(texsize.x, 0),
			texpos+float2(0, texsize.y),
			texpos+texsize,
		};
		ushort _ind[] = {
			numQuads*4,
			numQuads*4 + 1,
			numQuads*4 + 2,
			numQuads*4 + 1,
			numQuads*4 + 3,
			numQuads*4 + 2,
		};

		memcpy(&positions[numQuads*4], _pos, sizeof(float3)*4);
		memcpy(&colours[numQuads*4], _col, sizeof(float4)*4);
		memcpy(&texcoords[numQuads*4], _tex, sizeof(float2)*4);
		memcpy(&indices[numQuads*6], _ind, sizeof(ushort)*6);
		numQuads++;

		if (numQuads > maxQuads - 1)
		{
			end();
			begin(program, tex, view, proj);
		}
	}
	void addQuad(const float2& pos, const float2& size, const float2& texpos=float2(0,0), const float2& texsize=float2(1,1), const float4& col=float4(1,1,1,1))
	{
		float3 _pos[] = 
		{
			float3(pos, 1),
			float3(pos+float2(size.x, 0), 1),
			float3(pos+float2(0, size.y), 1),
			float3(pos+size, 1),
		};

		float4 _col[] = { col, col, col, col };

		float2 _tex[] = 
		{
			texpos,
			texpos+float2(texsize.x, 0),
			texpos+float2(0, texsize.y),
			texpos+texsize,
		};

		ushort _ind[] = 
		{
			0 + numQuads * 4,
			1 + numQuads * 4,
			2 + numQuads * 4,
			1 + numQuads * 4,
			3 + numQuads * 4,
			2 + numQuads * 4,
		};

		memcpy(&positions[numQuads*4], _pos, sizeof(float3)*4);
		memcpy(&colours[numQuads*4], _col, sizeof(float4)*4);
		memcpy(&texcoords[numQuads*4], _tex, sizeof(float2)*4);
		memcpy(&indices[numQuads*6], _ind, sizeof(ushort)*6);

		numQuads++;

		if (numQuads > maxQuads-1)
		{
			end();
			begin(program, tex, view, proj);
		}
	}
	void end()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbPos);
		glBufferData(GL_ARRAY_BUFFER, numQuads*4*sizeof(float3), (void*)positions, GL_STATIC_DRAW); 
		glBindBuffer(GL_ARRAY_BUFFER, vbTex);
		glBufferData(GL_ARRAY_BUFFER, numQuads*4*sizeof(float2), (void*)texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbCol);
		glBufferData(GL_ARRAY_BUFFER, numQuads*4*sizeof(float4), (void*)colours, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numQuads*6*sizeof(ushort), (void*)indices, GL_STATIC_DRAW);

		glUseProgram(program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(program, "texture"), 0);
		
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, view.m);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, proj.m);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, numQuads*6, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}
};
class Primitive
{
	GLRESOURCE program;
	GLRESOURCE vbPos;
	GLRESOURCE vbCol;
	GLRESOURCE vao;

	uint maxVertices;

	uint ptype;
	matrix view;
	matrix proj;
	float3* positions;
	float4* colours;

	Primitive() = default;
	Primitive(const Primitive&) = delete;
	Primitive& operator=(const Primitive&) = delete;

public:
	static Primitive* create(uint maxVertices)
	{
		Primitive* prim;
		prim->maxVertices = maxVertices;
		prim->positions = new float3[maxVertices];
		prim->colours = new float4[maxVertices];

		prim->vbPos = createBuffer(GL_ARRAY_BUFFER, prim->positions, sizeof(float3)*maxVertices, GL_STATIC_DRAW);
		prim->vbCol = createBuffer(GL_ARRAY_BUFFER, prim->colours, sizeof(float4)*maxVertices, GL_STATIC_DRAW);

		VertexAttr attributes[] = {
			{ 0, 3, GL_FLOAT, false, 0, nullptr },
			{ 1, 4, GL_FLOAT, false, 0, nullptr },
		};
		prim->vao = createVertexArray(attributes, { prim->vbPos, prim->vbCol }, 0);
		// prim->program = createProgram("..//jamesgl//shaders//primitivevs.glsl", "..//jamesgl//shaders//primitiveps.glsl");

		return prim;
	}
	~Primitive()
	{
		delete[] positions;
		delete[] colours;
	}

	// void begin(uint ptype, const matrix& view, const matrix& proj)
	// {
	// 	this->view = view;
	// 	this->proj = proj;
	// 	this->ptype = ptype;
	// }
	// void addVertex(const float3& pos, const float4& col)
	// {
	// 	positions[numVertices] = pos;
	// 	colours[numVertices] = col;
	// 	numVertices++;
	// 	if (numVertices > maxVertices)
	// 	{
	// 		end();
	// 		begin(ptype, view, proj);
	// 	}
	// }
	// void end()
	// {

	// }
};

#endif