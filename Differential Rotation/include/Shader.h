#pragma once

// Common C++ Headers you should include
#include <iostream>
#include <Windows.h>


#include "GL/glew.h"
#include "GL/glut.h"

using namespace std;

namespace midl {
    // GLSL Shader Class
    class Shader
    {
    private:
        unsigned int shader_id;
        unsigned int shader_vp;
        unsigned int shader_fp;
        char shader_name[1000];
    public:
        Shader();
        Shader(const char *vsFile, const char *fsFile);
        ~Shader();

        void Initialize(const char *vsFile, const char *fsFile);

        void Bind();
        void Unbind();

        unsigned int Id();
    };
}
