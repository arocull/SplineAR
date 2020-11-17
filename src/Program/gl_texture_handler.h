#pragma once

#include "src/config.h"

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
        void DrawGLTexture();

        void HoldCLTexture(GPU* gpu);
        void ReleaseCLTexture(GPU* gpu);

        void Free();
};