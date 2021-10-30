#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "src/Util/gl_headers.h"
#include "src/Program/file/filemanager.h"

// ShaderGL - Wrapper for GL shader programs. Workflow:
// - instance class
// - attach shader kernels
// - build program
//
// If a program needs to be rebuilt, all shaders must be re-attached
// and ID must be updated
class ShaderGL {
    public:
        ShaderGL();
        ~ShaderGL();

        bool attachKernel(std::string filepath, int flags = GL_FRAGMENT_SHADER);
        bool build();
        void use();

    private:
        bool programBuilt = false;
        GLuint program;
        std::vector<GLuint> shaders;
    
    public:
        GLuint getId();
};