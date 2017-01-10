#ifndef _WORLD_H
#define _WORLD_H

class World
{
private:
	World() = default;
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	int width;
	int height;
	short* data;

public:
	~World()
	{
		delete[] data;
	}
	static World* create(int width, int height)
	{
		World* world = new World;

		world->width = width;
		world->height = height;
		world->data = new short[width*height];

		memset(world->data, 0, sizeof(short)*width*height);

		world->Generate();
		return world;
	}

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }

	void Generate()
	{
		srand(100);
		int ph = 12;
		for (int i = 0; i < width; i++)
		{
			int h = ph + ((rand()%10==0) ? ((rand()%2) ? 1 : -1) : 0);
			for (int j = 0; j < height; j++)
			{
				if (j < h)
				{
					data[i*height+j] = 1;
				}
				else
				{
					data[i*height+j] = 0;
				}
			}
			ph = h;
		}
	}
	void Set(int x, int y, short val)
	{
		if (x > -1 && x < width &&
			y > -1 && y < height)
		{
			data[x*height+y] = val;
		}
	}
	short Get(int x, int y)
	{
		if (x > -1 && x < width &&
			y > -1 && y < height)
		{
			return data[x*height+y];
		}
		return 0;
	}

	bool GetPoint(const float2& pos)
	{
		float2 npos = pos / 16.f;
		int ix = (int)npos.x;
		int iy = (int)npos.y;
		return Get(ix, iy);
	}
	bool Collision(const float2& min, const float2& max)
	{
		float2 nmin = min / 16.f;
		float2 nmax = max / 16.f;
		int minx = (int)nmin.x;
		int miny = (int)nmin.y;
		int maxx = (int)nmax.x+1;
		int maxy = (int)nmax.y+1;
		for (int i = minx; i < maxx; i++)
		{
			for (int j = miny; j < maxy; j++)
			{
				if (Get(i, j))
				{
					return true;
				}
			}
		}

		return false;
	}
};

#endif