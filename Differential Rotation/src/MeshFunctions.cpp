#include "Mesh.h"

using namespace std;
using namespace midl;

bool _meshfunc_RayTriangleIntersection(float *V1, float *V2, float *V3, float *P1, float *P2, float &param, float *I)
{
	float O[3], D[3];
	SubVectors3(P2, P1, D);
	Normalize3(D);
	O[0] = P1[0]; O[1] = P1[1]; O[2] = P1[2];

	float e1[3], e2[3];  //Edge1, Edge2
	float P[3], Q[3], T[3];
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	SubVectors3(V2, V1, e1);
	SubVectors3(V3, V1, e2);

	//Begin calculating determinant - also used to calculate u parameter
	Cross(D, e2, P);

	//if determinant is near zero, ray lies in plane of triangle
	det = Dot3(e1, P);

	//NOT CULLING
	if (det > -1.0e-7 && det < 1.0e-7) return false;
	inv_det = 1.0 / det;

	//calculate distance from V1 to ray origin
	SubVectors3(O, V1, T);

	//Calculate u parameter and test bound
	u = Dot3(T, P)*inv_det;

	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter
	Cross(T, e1, Q);

	//Calculate V parameter and test bound
	v = Dot3(D, Q)*inv_det;

	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = Dot3(e2, Q)*inv_det;

	if (t > 1.0e-7) { //ray intersection
		param = t;
		I[0] = (1 - u - v)*V1[0] + u*V2[0] + v*V3[0];
		I[1] = (1 - u - v)*V1[1] + u*V2[1] + v*V3[1];
		I[2] = (1 - u - v)*V1[2] + u*V2[2] + v*V3[2];

		return true;
	}

	// No hit, no win
	return false;
}

void MeshManager::DrawMesh(vector<Tuple3f> &v, vector<Tuple3f> &n, vector<Tuple3f> &rgb, vector<Tuple3i> &f)
{
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < f.size(); i++)
	{
		glColor3fv(rgb[f[i].data[0]].data);
		glNormal3fv(n[f[i].data[0]].data);
		glVertex3fv(v[f[i].data[0]].data);

		glColor3fv(rgb[f[i].data[1]].data);
		glNormal3fv(n[f[i].data[1]].data);
		glVertex3fv(v[f[i].data[1]].data);

		glColor3fv(rgb[f[i].data[2]].data);
		glNormal3fv(n[f[i].data[2]].data);
		glVertex3fv(v[f[i].data[2]].data);

		glEnd();
	}
}

int MeshManager::ReadPLY(char* filename, vector<Tuple3f> &vertices, vector<Tuple3f> &VerticesNormal, vector<Tuple3f> &VerticesRGB, vector<Tuple3i> &faces)
{
	printf("Loading %s...\n", filename);
	char* pch = strstr(filename, ".ply");

	if (pch != NULL)
	{
		FILE* file = fopen(filename, "r");
		if (file != NULL)
		{
			int i = 0;
			int Triangle_index = 0;
			int TotalPoints = 0;
			int TotalFaces = 0;
			char buffer[1000];

			fgets(buffer, 1000, file);
			cerr << strncmp("element vertex", buffer, strlen("element vertex")) << endl;
			// Find number of vertices
			while (1)
			{
				if (strncmp("element vertex", buffer, strlen("element vertex")) == 0)break;
				fgets(buffer, 1000, file);
			}

			/*while (strncmp("element vertex", buffer, strlen("element vertex")) != 0);
			{
			fgets(buffer, 1000, file);
			cerr << buffer << "  "<<strncmp("element vertex", buffer, strlen("element vertex")) << endl;
			}*/
			strcpy(buffer, buffer + strlen("element vertex"));
			sscanf(buffer, "%i", &TotalPoints);

			// Find number of faces
			while (1)
			{
				if (strncmp("element face", buffer, strlen("element face")) == 0)break;
				fgets(buffer, 1000, file);
			}

			strcpy(buffer, buffer + strlen("element face"));
			sscanf(buffer, "%i", &TotalFaces);

			// Go to end_header
			while (1)
			{
				if (strncmp("end_header", buffer, strlen("end_header")) == 0)break;
				fgets(buffer, 1000, file);
			}


			// Read vertices, normals, colors
			i = 0;
			for (i = 0; i < TotalPoints; i++)
			{
				char tmp[1];
				fgets(buffer, 1000, file);
				Tuple3f vertex, normal;
				int C[3];
				Tuple3f color;
				sscanf(buffer, "%f %f %f %f %f %f %d %d %d %c", &vertex.data[0], &vertex.data[1], &vertex.data[2],
					&normal.data[0], &normal.data[1], &normal.data[2], &C[0], &C[1], &C[2], tmp);
				color.data[0] = (float)C[0] / 255.0;
				color.data[1] = (float)C[1] / 255.0;
				color.data[2] = (float)C[2] / 255.0;

				vertices.push_back(vertex);
				VerticesNormal.push_back(normal);
				VerticesRGB.push_back(color);

				//cout << "Vertex: " << vertex.data[0] << " " << vertex.data[1] << " " << vertex.data[2] << endl;
				//cout << "Normal: " << normal.data[0] << " " << normal.data[1] << " " << normal.data[2] << endl;
				//cout << "Color: " << color.data[0] << " " << color.data[1] << " " << color.data[2] << endl;
			}

			// Read faces
			//i = 0;
			for (i = 1; i <= TotalFaces; i++)
			{
				fgets(buffer, 1000, file);
				if (buffer[0] == '3') //Face is contructed by 3 points
				{
					buffer[0] = ' ';
					Tuple3i faceTri;

					sscanf(buffer, "%i %i %i", &faceTri.data[0], &faceTri.data[1], &faceTri.data[2]);

					faces.push_back(faceTri);

					////float x, y, z;
					//Vertex v;			
					//for (int j = 0; j < 3; j++)
					//{
					//	v.Vert[j].data[0] = vertices[faceTri.data[j]].data[0];
					//	v.Vert[j].data[1] = vertices[faceTri.data[j]].data[1];
					//	v.Vert[j].data[2] = vertices[faceTri.data[j]].data[2];
					//    //cout << "x,y,z->" << "for face " << i << "with index " << face.data[j];
					//}
					//
					//Face f;
					//f.FaceVertex[i] = v;
					//FaceVertices.push_back(f);
					//
					//cout << "The vertices are:" << endl;
					//for (int k = 0; k < 3; k++) 
					//{
					//	cout << "Vertex " <<k<<":"<<endl;
					//	cout << v.Vert[k].data[0] << "," << v.Vert[k].data[1] << "," << v.Vert[k].data[2] << endl;
					//}

					//cout << "Face: " << faceTri.data[0] << " " << faceTri.data[1] << " " << faceTri.data[2] << endl;
				}
			}
			//cout << faces.size() << endl;
			/*cout << faces[0].data[0] << endl;
			cout << faces[0].data[1] << endl;
			cout << faces[0].data[2] << endl;
			cout << vertices[faces[0].data[0]].data[0] << endl;
			cout << vertices[faces[0].data[0]].data[1] << endl;
			cout << vertices[faces[0].data[0]].data[2] << endl;
			cout << vertices[faces[0].data[1]].data[0] << endl;
			cout << vertices[faces[0].data[1]].data[1] << endl;
			cout << vertices[faces[0].data[1]].data[2] << endl;
			cout << vertices[faces[0].data[2]].data[0] << endl;
			cout << vertices[faces[0].data[2]].data[1] << endl;
			cout << vertices[faces[0].data[2]].data[2] << endl;*/

			fclose(file);
			printf("%s is Loaded!\n", filename);
		}
		else
		{
			printf("File can not be opened.");
		}
	}
	else
	{
		printf("File is not a .PLY extension.");
	}
	return 0;
}


bool MeshManager::ComputeFaceNormals(vector<Tuple3f> &vertices, vector<Tuple3i> &faces, vector<Tuple3f> &faceNormals)
{
	faceNormals.clear();

	float v1[3], v2[3];
	for (size_t i = 0; i < faces.size(); i++)
	{
		Tuple3f faceNormal;
		SubVectors3(vertices[faces[i].data[0]].data, vertices[faces[i].data[1]].data, v1); // v1 = trianglepoint2 - trianglepoint1
		SubVectors3(vertices[faces[i].data[0]].data, vertices[faces[i].data[2]].data, v2); // v2 = trianglepoint3 - trianglepoint1
		Cross(v1, v2, faceNormal.data);
		Normalize3(faceNormal.data);
		faceNormals.push_back(faceNormal);
		//cerr << faceNormal.data[0] << "  " << faceNormal.data[1] << " " << faceNormal.data[2] << endl;
	}
	return true;
}

bool MeshManager::CastRayOnMesh(float rayStart[3], float rayEnd[3], vector<Tuple3f> &vertices, vector<Tuple3i> &faces, float intersection[3])
{
	// Assume that there is no intersection
	// In this case, we do so by setting the 
	// intersection point to "infinity"
	// Since infinity does not actually exist in a computer
	// we achieve this by setting intersection to the maximum 
	// possible values that C/C++ allows us to do!!
	intersection[0] = FLT_MAX;
	intersection[1] = FLT_MAX;
	intersection[2] = FLT_MAX;

	// Note that this function does not take a parameter as an argument.
	// So we create a "local" variable to store this parameter
	// More importantly, we intialize this to FLT_MAX - i.e. the maximum value a float can have in C/C++
	// WHY??
	float parameter = FLT_MAX;

	// Let us also create "temporary" variables
	// that will actually store the parameter and intersection point
	// ASK ME ABOUT WHY WE DID THIS
	float tempParameter;
	float tempIntersection[3];

	// We want this value to be returned by this function
	// But we do not know if the ray intersects the mesh or not
	// For now, let us assume it doesn't - i.e. we set this to false.
	bool returnValue = false;

	for (size_t i = 0; i < faces.size(); i++)
	{
		bool b = _meshfunc_RayTriangleIntersection(vertices[faces[i].data[0]].data, vertices[faces[i].data[1]].data, vertices[faces[i].data[2]].data
			, rayStart, rayEnd, tempParameter, tempIntersection);

		if (b == true)// This means there is an intersection, and the intersection point is WITHIN the ray and triangle bounds!
		{
			// lets check if the variable "parameter" is the minimum.
			// note that for the first triangle, it is not going to be the case 
			// This is the reason for the FLT_MAX initialization :)
			if (tempParameter < parameter)
			{
				// tempParameter was less [i.e. the intersection point is closer to us]
				// So, we set our non-temporary [i.e. actual] parameter and intersection
				// to the temporary variables
				parameter = tempParameter;
				intersection[0] = tempIntersection[0];
				intersection[1] = tempIntersection[1];
				intersection[2] = tempIntersection[2];

				// Even if one triangle is intersected
				// the function must return true
				returnValue = true;
			}
		}
	}

	return returnValue;
}

int MeshManager::RotateMeshData(float angle, float *axis, vector<Tuple3f> &v, vector<Tuple3f> &n)
{
	Transformation matrix;
	matrix.SetAxisAngleRotation(axis, angle);
	for (int i = 0; i < v.size(); i++)
	{
		matrix.ApplyTo(v[i].data);
		matrix.ApplyTo(n[i].data);
	}
	return 0;
}

//int MeshManager::Update(float *axis, float angle)
int MeshManager::Getmatrix(float *matrix)
{
	for (int i = 0; i < 9; i++)
	{
		newmatrix[i] = matrix[i];
	}
	return true;
}

bool MeshManager::Setmatrix(float *finalmatrix)
{
	for (int i = 0; i < 9; i++)
	{
		finalmatrix[i] = newmatrix[i];
	}
	return true;
}

int MeshManager::GetAxisAngle(float *axis, float angle)
{
	newaxis[0] = axis[0];
	newaxis[1] = axis[1];
	newaxis[2] = axis[2];

	newangle = angle;

	float newmatrix[9];

	float temp[] = { newaxis[0], newaxis[1], newaxis[2] };

	AxisAngle(temp, newangle, newmatrix);
	return Getmatrix(newmatrix);
}