#include "olcConsoleGameEngine.h"
#include <fstream>
#include <strstream>
#include <algorithm>
using namespace std;

struct vec3d
{
	float x, y, z;
};
struct triangle
{
	vec3d p[3];

	wchar_t sym;
	short col;
};
struct mesh
{
	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}
};
struct mat4x4
{
	float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine
{

public:
	olcEngine3D()
	{
		m_sAppName = L"3D Demo";
	}
private: 
	mesh meshCube;
	mat4x4 matProj;

	vec3d vCamera;

	float fTheta;

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

	CHAR_INFO GetColor(float lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f * lum);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}

public: 
	virtual bool OnUserCreate() override
	{
		/*meshCube.tris =
		{	
			//SOUTH
			{0.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f },
			{0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f },

			//EAST
			{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f },
			{1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f },

			//NORTH
			{1.0f, 0.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f },
			{1.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f },

			//WEST
			{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f },
			{0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f },

			//TOP
			{0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f },
			{0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 0.0f },

			//BOTTOM
			{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f },
			{1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f },
			

			
		};*/
		meshCube.LoadFromObjectFile("Cube.obj");

		//Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//Clear screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		//Rotation Matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;

		//Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		//Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;


		vector<triangle> vecTrianglesToRaster;


		//Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjection, triTranslation, triRotationZ, triRotationZX;

			//Rotate in Z Axis
			MultiplyMatrixVector(tri.p[0], triRotationZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotationZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotationZ.p[2], matRotZ);

			//Rotate in X Axis
			MultiplyMatrixVector(triRotationZ.p[0], triRotationZX.p[0], matRotX);
			MultiplyMatrixVector(triRotationZ.p[1], triRotationZX.p[1], matRotX);
			MultiplyMatrixVector(triRotationZ.p[2], triRotationZX.p[2], matRotX);

			//Offset onto the screen
			triTranslation = triRotationZX;
			triTranslation.p[0].z = triRotationZX.p[0].z + 8.0f;
			triTranslation.p[1].z = triRotationZX.p[1].z + 8.0f;
			triTranslation.p[2].z = triRotationZX.p[2].z + 8.0f;

			//Use cross-product to get surface normal
			vec3d normal, line1, line2;
			line1.x = triTranslation.p[1].x - triTranslation.p[0].x;
			line1.y = triTranslation.p[1].y - triTranslation.p[0].y;
			line1.z = triTranslation.p[1].z - triTranslation.p[0].z;

			line2.x = triTranslation.p[2].x - triTranslation.p[0].x;
			line2.y = triTranslation.p[2].y - triTranslation.p[0].y;
			line2.z = triTranslation.p[2].z - triTranslation.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			//Normalize the normal
			float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;

			//if (normal.z < 0)
			if (normal.x * (triTranslation.p[0].x - vCamera.x) +
				normal.y * (triTranslation.p[0].y - vCamera.y) +
				normal.z * (triTranslation.p[0].z - vCamera.z) < 0.0f)
			{

				//Illumination
				vec3d light_direction = { 0.0f, 0.0f, -1.0f };
				float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				//Compare normal to light direction
				float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

				//Choose console colors
				CHAR_INFO c = GetColor(dp);
				triTranslation.col = c.Attributes;
				triTranslation.sym = c.Char.UnicodeChar;


				//Project triangles from 3D into 2D
				MultiplyMatrixVector(triTranslation.p[0], triProjection.p[0], matProj);
				MultiplyMatrixVector(triTranslation.p[1], triProjection.p[1], matProj);
				MultiplyMatrixVector(triTranslation.p[2], triProjection.p[2], matProj);
				triProjection.col = triTranslation.col;
				triProjection.col = triTranslation.sym;


				//Scale into view
				triProjection.p[0].x += 1.0f; triProjection.p[0].y += 1.0f;
				triProjection.p[1].x += 1.0f; triProjection.p[1].y += 1.0f;
				triProjection.p[2].x += 1.0f; triProjection.p[2].y += 1.0f;
				triProjection.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjection.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjection.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjection.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjection.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjection.p[2].y *= 0.5f * (float)ScreenHeight();

				vecTrianglesToRaster.push_back(triProjection);
			}
		}
		// Sort triangles from back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});
		for (auto& triProjection : vecTrianglesToRaster)
		{


			FillTriangle(triProjection.p[0].x, triProjection.p[0].y,
				triProjection.p[1].x, triProjection.p[1].y,
				triProjection.p[2].x, triProjection.p[2].y,
				triProjection.sym, triProjection.col);

			/*DrawTriangle(triProjection.p[0].x, triProjection.p[0].y,
				triProjection.p[1].x, triProjection.p[1].y,
				triProjection.p[2].x, triProjection.p[2].y,
				PIXEL_SOLID, FG_WHITE);*/
		}

		return true;
	}
};

int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
	return 0;
}



;