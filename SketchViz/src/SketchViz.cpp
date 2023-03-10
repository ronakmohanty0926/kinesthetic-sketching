// ABCSketch1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <cstdio>
#include <fstream>
#include <crtdbg.h>
#include <queue>
#include <deque>

#include "Renderer.h"
#include "Shader.h"
#include "Core.h"
#include "Curve.h"

#include "opencv2\opencv.hpp"

#define GL_WIN_SIZE_X 800
#define GL_WIN_SIZE_Y 600

using namespace std;
using namespace midl;

float eye[] = { 0.0,0.0,5.0 };
float center[] = { 0.0,0.0,0.0 };
float head[] = { 0.0, 1.0, 0.0 };

//---- Open GL View
PerspectiveView view;

//---- Data Variables
vector<Tuple3f>sketchverts;
vector<int>index;
vector<Curve3>curve;

float maxZ, minZ;
Shader colorShader;

bool isLeftClicked = false;
float currenDragPoint[] = { 0.0,0.0 };
float previousDragPoint[] = { 0.0,0.0 };

bool isShadowDisplayed = false;
bool isSequenceInitiated = false;
bool isSketchDisplayed = true;
int button = 0; int i = 0;
char mode[1000];


void CleanupAndExit()
{
	exit(0);
}

void ReadSketch(char* filename)
{
	printf("Loading %s...\n", filename);
	FILE *stream = fopen(filename, "r");
	if (stream != NULL)
	{
		while (!feof(stream))
		{
			Tuple3f vertex;
			char buffer[10000];
			fgets(buffer, 10000, stream);
			fscanf(stream, "%f %f %f", &vertex.data[0], &vertex.data[1], &vertex.data[2]);
			sketchverts.push_back(vertex);
			if (strncmp("Sketch", buffer, strlen("Sketch")) == 0)
			{
				Curve3 C;
				curve.push_back(C);

				int s = sketchverts.size() - 1;
				index.push_back(s);
			}
		}
	}
	else cerr << "File not found" << endl;
}

void InitialCurve()
{
	int size,fsize ;
	fsize = 0;
	//sz = index[i];
	
	for (int i = 1; i < curve.size(); i++)
	{	
		size = index[i];
		for (int j = fsize; j < size; j ++)
		{
			curve[i-1].AddPoint(sketchverts[j].data);				
		}	
		fsize = size;
	}	
}

void FinalCurve()
{
	int size;
	size = index[index.size()-1];
	for (int k = size; k < sketchverts.size(); k++)
	{
		curve[curve.size() - 1].AddPoint(sketchverts[k].data);
	}	
}

void SortSketchZ()
{
	minZ = FLT_MAX;
	maxZ = -FLT_MAX;

	float p[3];
	for (int i = 0; i < curve.size(); i++)
	{
		for (int j = 0; j < curve[i].GetNumVertices(); j++)
		{
			curve[i].GetVertex(j, p);
			if (p[2] < minZ) minZ = p[2];
			if (p[2] > maxZ) maxZ = p[2];
		}
	}
}

void DrawSketch(Curve3 &C)
{	
	glColor3fv(Black3f);
	glLineWidth(5.0);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBegin(GL_LINES);
	for (int i = 1; i < C.GetNumVertices(); i++)
	{
		float p1[3], p2[3];
		
		C.GetVertex(i - 1, p1);
		C.GetVertex(i, p2);
		float r = maxZ - minZ;
		float c1, c2;
		c1 = 1 - (0.5*((p1[2] - minZ) / r) + 0.3);
		c2 = 1 - (0.5*((p2[2] - minZ) / r) + 0.3);
		glColor3f(c1, c1, c1);
		glVertex3f(p1[0], p1[1], p1[2]);
		glColor3f(c2, c2, c2);
		glVertex3f(p2[0], p2[1], p2[2]);
	}
	glEnd();
}

void DrawShadow(Curve3 &C)
{
	for (int i = 1; i < C.GetNumVertices(); i++)
	{
		float p1[3], p2[3];
		C.GetVertex(i-1, p1);
		C.GetVertex(i, p2);
		float PlanePt[] = { 0.0, -0.5, 0.0 };
		float dist1 = PointToPlaneDist(p1, PlanePt, Yaxis);
		float dist2 = PointToPlaneDist(p2, PlanePt, Yaxis);
		p1[1] -= dist1; p2[1] -= dist2;

		glLineWidth(4.0);
		glColor3fv(Gray3f);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
		glEnd();
	}
}

void glutDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	view.Bind();

	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 2.0f);

	if (isSequenceInitiated)
	{
		for (int j = 0; j < i; j++)
		{
			DrawSketch(curve[j]);
			if (isShadowDisplayed)DrawShadow(curve[j]);
		}
	}

	else if (isSketchDisplayed)
	{
		for (int i = 0; i < curve.size(); i++)
		{
			DrawSketch(curve[i]);
		}
	}

	glDisable(GL_POLYGON_OFFSET_LINE);

	view.Unbind();

	glutSwapBuffers();
}

void glutReshape(int w, int h)
{
	// Just set the viewport and get out
	// All view processing in displayCallback
	// glViewport (0, 0, (GLsizei)w, (GLsizei)h);

	// glMatrixMode (GL_PROJECTION);
	// glLoadIdentity();
	// gluPerspective (prj_fov, (GLfloat)w / (GLfloat)h, prj_near, prj_far);
	// glMatrixMode (GL_MODELVIEW);
	view.Reshape(w, h);
}

void glutIdle(void)
{
	// We just set the backgournd to White!
	// and do nothing.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutPostRedisplay();
}

void mouseClickCallback(int button, int state, int x, int y)
{
	// Example usage:
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			isLeftClicked = true;
		}
		else if (state == GLUT_UP)
		{
			GLfloat WinCoords[2];
			view.PixelToWindow(x, y, WinCoords);

			currenDragPoint[0] = WinCoords[0];
			currenDragPoint[1] = WinCoords[1];
		}
		glutPostRedisplay();
	}
}

void InitShaders()
{
	colorShader.Initialize(".//Shaders//colorShader.vert", ".//Shaders//colorShader.frag");
}

void mouseActiveMotionCallback(int x, int y)
{
	if (isLeftClicked)
	{
		GLfloat WinCoords[2];
		view.PixelToWindow(x, y, WinCoords);

		previousDragPoint[0] = currenDragPoint[0];
		previousDragPoint[1] = currenDragPoint[1];

		currenDragPoint[0] = WinCoords[0];
		currenDragPoint[1] = WinCoords[1];

		float displacement[2];
		SubVectors2(currenDragPoint, previousDragPoint, displacement);
		if (Norm2(displacement) > FLOAT_EPSILON) // If the displacement is greater than 0
		{
			float angle = 0.05*Norm2(displacement); // 1 pixel movement leads to 1 degree rotation
			float axis[] = { -displacement[1],displacement[0], 0.0 };
			Normalize3(axis);
			float matrix[9];

			AxisAngle(axis, angle, matrix);
			for (int i = 0; i < curve.size(); i++)
			{
				curve[i].Rotate(angle, axis, Origin);
			}
		}
		glutPostRedisplay();
	}
}

void mousePassiveMotionCallback(int x, int y)
{
}

void glutKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // 27 is ASCII code for the "Esc" key
		CleanupAndExit();
		break;
	case 'b': // Let us change the background color to black when 'b' is pressed

		break;
	case 'w': // Let us change the background color to white when 'w' is pressed

		break;	
	case 's' :
		button++;
		if (button% 2 == 0)	isShadowDisplayed = true;
		else isShadowDisplayed = false;
		break;

	case 'a':		
		if (i < curve.size())
		{
			isSequenceInitiated = true;
			isSketchDisplayed = false;
			i++;
		}
		else i = 0;
		break;
	case 'd':
		isSketchDisplayed = true;
		isSequenceInitiated = false;
		break;
	}
}

void specialKeyCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	}
}

void glInit(int * pargc, char ** argv)
{
	glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
	// Create a window of size GL_WIN_SIZE_X, GL_WIN_SIZE_Y
	// See where GL_WIN_SIZE_X and GL_WIN_SIZE_Y are defined!!
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);

	// Create a window with some name
	glutCreateWindow("SketchViz");

	// GLEW is another library that
	// will be used as a standard for
	// initializing Shaders. 
	// Do not worry about this now!
	GLenum err = glewInit();
	if (GLEW_OK != err) // If GLEW was not properly initialized, we create a pop-up window and tell the user!
	{
		printf("Error: %s\n", glewGetErrorString(err));
		MessageBox(NULL, L"GLEW unable to initialize.", L"ERROR", MB_OK);
		return;
	}

	//---- Initialize the openGL camera 
	//---- and projection parameters
	view.SetParameters(35, 0.1, 10000.0);
	view.SetCameraCenter(0.0, 0.0, 0.0);
	view.SetCameraEye(0.0, 0.2, 2.0);
	view.SetCameraHead(0.0, 1.0, 0.0);

	// Shader Initialization:
	InitShaders();

	glutReshapeFunc(glutReshape);
	glutDisplayFunc(glutDisplay);
	glutKeyboardFunc(glutKeyboard);
	glutMouseFunc(mouseClickCallback);
	glutMotionFunc(mouseActiveMotionCallback);
	glutPassiveMotionFunc(mousePassiveMotionCallback);
	glutIdleFunc(glutIdle);

}

int main(int argc, char** argv)
{	
	char filename[1000];

	cout << "Input Text File Name ? ";
	cin >> filename;

	ReadSketch(filename);
	InitialCurve();
	FinalCurve();
	SortSketchZ();

	atexit(CleanupAndExit);

	glInit(&argc, argv);
	glutMainLoop();

	return 0;
}

