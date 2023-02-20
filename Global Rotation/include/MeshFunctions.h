#pragma once

#include "stdio.h"
#include "string.h"
#include <vector>

#include "Core.h"
#include "Renderer.h"
#include "Shader.h"

namespace midl
{
	class MeshManager
	{
	private:
		
		

	public:
		float newaxis[3], newangle, newmatrix[9];

		int ReadPLY(char* filename, vector<Tuple3f> &v, vector<Tuple3f> &n, vector<Tuple3f> &rgb, vector<Tuple3i> &f);

		bool ComputeFaceNormals(vector<Tuple3f> &vertices, vector<Tuple3i> &faces, vector<Tuple3f> &faceNormals);
		//bool RayTriangleIntersection(float rayStart[3], float rayEnd[3], float p1[3], float p2[3], float p3[3], float &parameter, float intersection[3]);
		bool CastRayOnMesh(float rayStart[3], float rayEnd[3], vector<Tuple3f> &vertices, vector<Tuple3i> &faces, float intersection[3]);

		int RotateMeshData(float angle, float *axis, vector<Tuple3f> &v, vector<Tuple3f> &n);

		void DrawMesh(vector<Tuple3f> &v, vector<Tuple3f> &n, vector<Tuple3f> &rgb, vector<Tuple3i> &f);

		

		int GetAxisAngle(float *axis, float angle);

		int Getmatrix(float *matrix);

		bool Setmatrix(float *finalmatrix);


		
	};
}