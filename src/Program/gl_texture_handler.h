#pragma once

#include "src/config.h"

#ifdef DEBUG_VERBOSE
    #include <cstdio>
#endif

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "src/Program/gpu.h"

class GLTextureHandler {
    public:
        GLTextureHandler(GPU* gpu, int width, int height, int type = GL_FLOAT, int format = GL_RGBA, int colorFormat = GL_RGBA32F);

    private:
        GLuint glTexture;
        cl_mem clTexture;
        bool inCLUse;
    
    public:
        bool BindGLTexture();
        void UnbindGLTexture();
        void DrawGLTexture(float x1 = 0.0f, float y1 = 0.0f, float x2 = 0.0f, float y2 = 0.0f);

        void HoldCLTexture(GPU* gpu);
        void ReleaseCLTexture(GPU* gpu);

        GLuint GetGL();
        cl_mem GetCL();
        cl_mem* GetCLReference();

        void Free();
};