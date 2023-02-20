#pragma once

#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
    #include <math.h>
#endif

#include <vector>

#include <Core.h>
#include <Mesh.h>
#include <Renderer.h>

using namespace std;

namespace midl {

    class MESH_DISPLAY
	{
    public:
		enum
		{
            WIREFRAME,
            SOLID_FLAT,
            SOLID_SMOOTH
		};
	};

    void DrawMesh(Mesh &m, int option);
    void DrawMesh(Mesh &m, float *color, int option);
    void DrawMesh(Mesh &m, vector<Tuple3f> &colorList, int option);
    void DrawMesh(Mesh &m, unsigned int shaderID, GLuint &texture);

    void DrawMeshVertices(Mesh &m, int option);
    void DrawMeshVertices(Mesh &m, float *color, int option);
    void DrawMeshEdges(Mesh &m, int option);
    void DrawMeshEdges(Mesh &m, float *color, int option);
}
