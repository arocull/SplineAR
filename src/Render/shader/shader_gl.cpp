#include "src/Render/shader/shader_gl.h"

ShaderGL::ShaderGL() {
    shaders = std::vector<GLuint>();
}
ShaderGL::~ShaderGL() {
    if (programBuilt) {
        glDeleteProgram(program);
    }
}

// Attach Kernel - Attaches a shader kernel to the shader program
bool ShaderGL::attachKernel(std::string filepath, int flags) {
    GLuint shader = glCreateShader(flags);

    std::string src = FileManager::readFile(filepath);
    const char* srcPtr = src.c_str();

    // Compile program
    glShaderSource(shader, 1, &srcPtr, NULL);
    glCompileShader(shader);

    // Fetch compilation results
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        int logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> shaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(shader, logLength, NULL, &shaderError[0]);
        std::cout << "ERROR while compiling GL shader attachment " << filepath << "\n\t" << &shaderError[0];
        return false;
    }

    shaders.push_back(shader);

    return true;
}
// Build - Builds the shader program, will delete a previous program if it existed
bool ShaderGL::build() {
    if (programBuilt) {
        glDeleteProgram(program);
    }

    // Create render program
    program = glCreateProgram();
    programBuilt = true;

    // Attach shaders and link program
    for (int i = 0; i < shaders.size(); i++) {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    // Delete shaders as we no longer need them
    for (int i = 0; i < shaders.size(); i++) {
        glDeleteShader(shaders[i]);
    }
    shaders.clear();

    
    // Fetch compilation results
    GLint result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        int logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError( (logLength > 1) ? logLength : 1 );
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        std::cout  << "ERROR while linking GL program\t" << &programError[0] << std::endl;
        return false;
    }

    return true;
}

GLuint ShaderGL::getId() {
    return program;
}