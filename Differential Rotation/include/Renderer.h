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
#include <Shader.h>

using namespace std;

namespace midl {

    class GRID_PLANE
    {
    public:
        enum
        {
            XY,
            YZ,
            ZX
        };
    };

    class POINT_DISPLAY
    {
    public:
        enum
        {
            PIXEL,
            QUAD_LINE,
            QUAD_FILL,
            CIRCLE_LINE,
            CIRCLE_FILL,
            SPHERE
        };
    };

    class LINE_DISPLAY
    {
    public:
        enum {
            PIXEL,
            CYLINDER,
            BOX
        };
    };

    //---- Standard Colors
    static float Red3f[]={1.0,0.0,0.0};
    static float Green3f[]={0.0,1.0,0.0};
    static float Blue3f[]={0.0,0.0,1.0};
    static float Cyan3f[]={0.0,1.0,1.0};
    static float Yellow3f[]={1.0,1.0,0.0};
    static float Magenta3f[]={1.0,0.0,1.0};
    static float Black3f[]={0.0,0.0,0.0};
    static float Gray3f[]={0.5,0.5,0.5};
    static float White3f[]={1.0,1.0,1.0};

    static float cdClay13f[]={1.0,0.90,0.30};
    static float cdViolet13f[]={0.26,0.22,0.53};

    // This class will manage all GL parameters
    // such as camera parameters, convert view to
    // model coordinates etc.
    class PerspectiveView
    {
    private:
        // GL Model space to Screen space variables
        float prj_fov, prj_near, prj_far;

        // Viewport and Matrices
        int viewport[4];
        GLdouble projectionMatrix[16];
        GLdouble modelMatrix[16];

        // Camera parameters
        float eye[3];
        float center[3];
        float head[3];
    public:
        PerspectiveView();
        ~PerspectiveView();

        void SetParameters(float fov, float near prj_near, float far prj_far);

        void SetCameraEye(float x, float y, float z);
        void SetCameraEye(float *eye);

        void SetCameraCenter(float x, float y, float z);
        void SetCameraCenter(float *center);

        void SetCameraHead(float x, float y, float z);
        void SetCameraHead(float *head);

        void PixelToWindow(int px, int py, float *windowCoordinates);
        void PixelToNormalized(int px, int py, float *normalizedCoordinates);
        void PixelToWorld(int px, int py, float depth, float *worldCoordinates);
        void PixelToRay(int px, int py, float *rayStart, float *rayEnd);
        void Reshape(int w, int h);
        void Bind();
        void Bind(float *matrix);
        void Unbind();
    };

    class Light
    {
    private:
        GLfloat lightpos[4];
        GLfloat diffuse[4];
        GLfloat ambient[4];
        GLfloat specular[4];
        Transformation rotation;
    public:
        Light();
        ~Light();

        void SetPosition(float *pos);
        void SetDiffuseColor(float *color);
        void SetAmbientColor(float *color);
        void SetSpecularColor(float *color);

        void Rotate(float *axis, float angle);

        void Bind();
        void Unbind();
    };

    // Texture loading from RAW file
    GLuint LoadTexture(const char * filename, int width, int height);
    GLuint LoadTexture(int width, int height,unsigned char * data,bool alphaFlag);

    // NOTE: All positions, centers etc are in 3D
    // For 2D entities, simply set position[2] = 0.0 before passing to these functions

    // Grid for a 3D interface
    void DrawGrid(int plane, float side, int resolution, float *color);

    // Quads, Circles, Spheres
    void DrawQuad(float width, float height, float *center, bool isFilled, float color[], float transparency = 1.0f);
	void DrawQuadShadow(float width, float height, float *center, bool isFilled, float color[], float transparency = 1.0f);
	void DrawQuad(float width, float height, float *center, float *normal, bool isFilled, float color[], float transparency = 1.0f);
	void DrawQuadShadow(float width, float height, float *center, float *normal, bool isFilled, float color[], float transparency = 1.0f);
    void DrawQuad(float width, float height, float *center, unsigned int shaderID, GLuint &texture);
    void DrawCircle(float *center, float radius, bool isFilled, float *color, float transparency = 1.0f, int resolution = 50);

    void DrawSphere(float *center, float radius, float *color, float transparency = 1.0f);
    void DrawCylinder(float *p1, float *p2, float radius, float *color, float transparency = 1.0f, bool areEndsSolid = true, int resolution = 50);

    // Basic geometric primitives
    void DrawPoint(float *position, float *color, float transparency = 1.0f, float pointSize = 0.1f, int option = POINT_DISPLAY::PIXEL);
    void DrawLine(float *p1, float *p2, float *color, float transparency = 1.0f, float thickness = 0.1f, int option = LINE_DISPLAY::PIXEL);

    void DrawPointCloud(vector<float> &pcl, float *color, float transparency = 1.0f, float pointSize = 0.1f, int option = POINT_DISPLAY::PIXEL);
    void DrawPointCloud(vector<Tuple3f> &pcl, float *color, float transparency = 1.0f, float pointSize = 0.1f, int option = POINT_DISPLAY::PIXEL);

    void DrawEdgeList(vector<float> &vertices, vector<int> &edges, float *color, float transparency = 1.0f, float thickness = 0.1f, int option = LINE_DISPLAY::PIXEL);
    void DrawEdgeList(vector<Tuple3f> &vertices, vector<int> &edges, float *color, float transparency = 1.0f, float thickness = 0.1f, int option = LINE_DISPLAY::PIXEL);
    void DrawEdgeList(vector<float> &vertices, vector<Tuple2i> &edges, float *color, float transparency = 1.0f, float thickness = 0.1f, int option = LINE_DISPLAY::PIXEL);
    void DrawEdgeList(vector<Tuple3f> &vertices, vector<Tuple2i> &edges, float *color, float transparency = 1.0f, float thickness = 0.1f, int option = LINE_DISPLAY::PIXEL);

    void DrawEdges(vector<Tuple3f> &vertices, vector<Tuple2i> &edges);

    // Framebuffer Object Functions
    GLuint RenderScreenToTexture(int w,int h,bool isDepth=false);
}
