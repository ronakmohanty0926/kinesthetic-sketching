#include "Shader.h"

namespace midl {
    // A function to read from a text file
    // that contains the actual GLSL shader
    static char* textFileRead(const char *fileName)
    {
        char* text = NULL;
        if (fileName != NULL) {
            FILE *file = fopen(fileName, "rt");

            if (file != NULL) {
                fseek(file, 0, SEEK_END);
                int count = ftell(file);
                rewind(file);

                if (count > 0) {
                    text = (char*)malloc(sizeof(char) * (count + 1));
                    count = fread(text, sizeof(char), count, file);
                    text[count] = '\0';
                }
                else text = NULL;
                fclose(file);
            }
        }
        return text;
    }

    // GLSL Shader Methods
    Shader::Shader()
    {}

    Shader::Shader(const char *vsFile, const char *fsFile) {
        Initialize(vsFile, fsFile);
    }

    void Shader::Initialize(const char *vsFile, const char *fsFile) {
        shader_vp = glCreateShader(GL_VERTEX_SHADER);
        shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vsText = textFileRead(vsFile);
        const char* fsText = textFileRead(fsFile);

        if (vsText == NULL || fsText == NULL) {
            cerr << "Either vertex shader or fragment shader file not found." << endl;
            return;
        }

        glShaderSource(shader_vp, 1, &vsText, 0);
        glShaderSource(shader_fp, 1, &fsText, 0);

        glCompileShader(shader_vp);
        glCompileShader(shader_fp);

        shader_id = glCreateProgram();
        glAttachShader(shader_id, shader_fp);
        glAttachShader(shader_id, shader_vp);
        glLinkProgram(shader_id);

        cout << vsFile << " read successfully." << endl;
        cout << fsFile << " read successfully." << endl;

        strcpy(shader_name, vsFile);
    }

    Shader::~Shader()
    {
        glDetachShader(shader_id, shader_fp);
        glDetachShader(shader_id, shader_vp);

        glDeleteShader(shader_fp);
        glDeleteShader(shader_vp);
        glDeleteProgram(shader_id);
    }

    unsigned int Shader::Id()
    {
        return shader_id;
    }

    void Shader::Bind()
    {
        glUseProgram(shader_id);
    }

    void Shader::Unbind()
    {
        glUseProgram(0);
    }

}
