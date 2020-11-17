#include "src/Program/gl_texture_handler.h"

/* Initialize a GL Texture Handler
 *
 * These can be used for various purposes, such as storing texture information,
 * or for a byte map for layering.
 * 
 * An example of a full color image that can be accessed by OpenCL would be:
 * GLTextureHandler(480, 480, gpu, GL_FLOAT, GL_RGBA, GL_RGBA32F);
 *
 * An example of a 2D array of bytes that cannot be accessed by OpenCL would be:
 * GLTextureHandler(480, 480, nullptr, GL_BYTE, GL_RED, GL_R8);
 * 
 * 
 * More info on GL texture formats and what they do can be found here:
 * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
 */
GLTextureHandler::GLTextureHandler(GPU* gpu, int width, int height, int type, int format, int colorFormat) {
    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, format, type, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFinish();

    clTexture = clCreateFromGLTexture(gpu->context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, glTexture, NULL);

    inCLUse = false;
}


// Bind the texture for drawing in OpenGL
// Returns false if the texture is currently being processed in OpenCL
bool GLTextureHandler::BindGLTexture() {
    if (inCLUse) return false;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glTexture);
    return true;
}
// Unbinds the texture for drawing in OpenGL
// This function is actually generic
void GLTextureHandler::UnbindGLTexture() {
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Texture is unset
}
// Draws a square with a UV
// The function is actually generic
// TODO: Manually set corner points
void GLTextureHandler::DrawGLTexture() {
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);   // Set UV and draw coordinates
    glTexCoord2i(0, 1); glVertex2i(0, 1);
    glTexCoord2i(1, 1); glVertex2i(1, 1);
    glTexCoord2i(1, 0); glVertex2i(1, 0);
    glEnd();
}

void GLTextureHandler::HoldCLTexture(GPU* gpu) {
    inCLUse = true;
    clEnqueueAcquireGLObjects(gpu->queue, 1, &clTexture, 0, 0, NULL);
    
}
void GLTextureHandler::ReleaseCLTexture(GPU* gpu) {
    clEnqueueReleaseGLObjects(gpu->queue, 1, &clTexture, 0, 0, NULL);
    inCLUse = false;
}


void GLTextureHandler::Free() {
    clReleaseMemObject(clTexture);
    glDeleteTextures(1, &glTexture);
}