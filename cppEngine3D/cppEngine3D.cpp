#include "olcConsoleGameEngine.h"
using namespace std;

struct vec3d
{
	float x, y, z;
};
struct triangle
{
	vec3d p[3];
};
struct mesh
{
	vector<triangle> tris;
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
public: 
	virtual bool OnUserCreate() override
	{
		meshCube.tris =
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

		};

		//Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[2][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

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

		//Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjection, triTranslate, triRotationZ, triRotationZX;

			//Rotate in Z Axis
			MultiplyMatrixVector(tri.p[0], triRotationZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotationZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotationZ.p[2], matRotZ);

			//Rotate in X Axis
			MultiplyMatrixVector(triRotationZ.p[0], triRotationZX.p[0], matRotX);
			MultiplyMatrixVector(triRotationZ.p[1], triRotationZX.p[1], matRotX);
			MultiplyMatrixVector(triRotationZ.p[2], triRotationZX.p[2], matRotX);


			triTranslate = triRotationZX;
			triTranslate.p[0].z = triRotationZX.p[0].z + 3.0f;
			triTranslate.p[1].z = triRotationZX.p[1].z + 3.0f;
			triTranslate.p[2].z = triRotationZX.p[2].z + 3.0f;

			MultiplyMatrixVector(triTranslate.p[0], triProjection.p[0], matProj);
			MultiplyMatrixVector(triTranslate.p[1], triProjection.p[1], matProj);
			MultiplyMatrixVector(triTranslate.p[2], triProjection.p[2], matProj);


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


			DrawTriangle(triProjection.p[0].x, triProjection.p[0].y,
				triProjection.p[1].x, triProjection.p[1].y,
				triProjection.p[2].x, triProjection.p[2].y,
				PIXEL_SOLID, FG_WHITE);

		}
		return true;
	}
};

int main()
{
	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 4, 4) == false)
		demo.Start();
	demo.ConstructConsole(256, 240, 2, 2);
	demo.Start();
	return 0;
}



;