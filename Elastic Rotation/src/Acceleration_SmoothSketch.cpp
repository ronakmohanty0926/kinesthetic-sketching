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


#include "ABCSketchManager.h"
#include "HapticsEventManager.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "opencv2\opencv.hpp"

#define GL_WIN_SIZE_X 1280
#define GL_WIN_SIZE_Y 960

using namespace std;
using namespace midl;

// Event Manager
static HapticsEventManagerGeneric *hapticsEvent = 0;
static ABCSketchManager *skEvent = 0;
static Mesh *meshEvent = 0;;

// Video and snapshots
int snapID = 0;
int isVideo, isHaptics;
bool isRecord = false;
cv::VideoWriter capture;

float eye[] = { 0.0,0.0,5.0 };
float center[] = { 0.0,0.0,0.0 };
float head[] = { 0.0, 1.0, 0.0 };

//---- Open GL View
PerspectiveView view;
//OrthographicView view;

//---- Open GL Lighting
Light light;
GLfloat lightPos[] = { 0.0, 0.0, 10.0, 1.0 };
GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

float scale[] = { 1.0 ,1.0, 1.0 };


void CleanupAndExit()
{
	if (hapticsEvent)
	{
		hapticsEvent->Cleanup();
		HapticsEventManagerGeneric::Delete(hapticsEvent);
	}
	//if (skEvent)delete skEvent;	
	if (isVideo > 0)capture.release();

	exit(0);
}

float follower[3];
GLdouble trans[16];
Transformation T;
//bool rotation = skEvent->RotationOn();
void glutDisplay(void)
{	
	hapticsEvent->UpdateState();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	view.Bind();	

	light.Bind();	
	skEvent->DrawRefMesh();
	light.Unbind();


	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 2.0f);		
	
	skEvent->Render();

	glDisable(GL_POLYGON_OFFSET_LINE);

	if (isVideo > 0)
	{
		cv::Mat img(GL_WIN_SIZE_Y, GL_WIN_SIZE_X, CV_8UC3);
		glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
		glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
		//cerr << img.cols << " " << img.rows << endl;
		glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
		cv::Mat flipped(img);
		cv::flip(img, flipped, 0);
		capture.write(img);
	}

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
			// When the left button is pressed
			// down, do something
		}
		else if (state == GLUT_UP)
		{
			// When the left button is lifted
			// up, do something else
		}
	}
}

void mouseActiveMotionCallback(int x, int y)
{
	// You can define anythging here
	// Right now, we do not want to 
	// do anything when the mouse moves!
	// So, let us leave this function empty!
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
	case 'r': //Let us Reset the scene
		skEvent->~ABCSketchManager();
		break;
	case 'R':
		skEvent->~ABCSketchManager();
		break;
	case 'x':
		skEvent->UndoSketch();
		break;
	case 'z':
		skEvent->RedoSketch();
		break;
	case 's':
		skEvent->SaveSketchData();
		skEvent->SaveLog();
		skEvent->SaveMeshData();
		skEvent->SaveMetric();
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
	glutCreateWindow("SS: Acceleration");

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

	//---- Initialize Haptics device
	hapticsEvent = HapticsEventManager::Initialize();
	hapticsEvent->Setup(skEvent);
	
	//---- Initialize the openGL camera 
	//---- and projection parameters
	view.SetParameters(35, 0.1, 10000.0);
	//view.SetParameters(1.0, -1.0, 1.0, -1.0, 0.1, 10000.0);
	view.SetCameraCenter(0.0, 0.0, 0.0);
	view.SetCameraEye(0.0, 0.2, 2.0);
	view.SetCameraHead(0.0, 1.0, 0.0);

	// Shader Initialization:
	skEvent->InitShaders();

	light.SetPosition(lightPos);
	light.SetDiffuseColor(diffuse);
	light.SetAmbientColor(ambient);
	light.SetSpecularColor(specular);

	glutReshapeFunc(glutReshape);
	glutDisplayFunc(glutDisplay);
	glutKeyboardFunc(glutKeyboard);
	//glutPassiveMotionFunc(mousePassiveMotionCallback);
	glutIdleFunc(glutIdle);

}

int main(int argc, char** argv)
{

	skEvent = new ABCSketchManager();
	char filename[1000];

	cerr << "UserID: ";
	cin >> skEvent->userID;

	cerr << "Capture video (0/1)? ";
	cin >> isVideo;

	cerr << "Activate Haptics (0/1)?";
	cin >> isHaptics;
	
	cout << "Input Mesh File Name ? ";
	cin >> filename;

	if (isHaptics == 1) skEvent->SetHaptics();

	if (isVideo > 0)
	{
		//---- Video writer initialization
		string _videoName;
		char videoName[100];
		sprintf(videoName, "Acceleration_user_%d.avi", skEvent->userID);
		_videoName.append(videoName);
		cv::Size _sss(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
		capture.open(_videoName, CV_FOURCC('M', 'P', 'E', 'G'), 30, _sss, true);
	}	
	
	skEvent->ReadMesh(filename);
	
	
	atexit(CleanupAndExit);

	glInit(&argc, argv);
	glutMainLoop();

	return 0;
}

