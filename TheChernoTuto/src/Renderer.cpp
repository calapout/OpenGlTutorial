#include "Renderer.h"
#include <iostream>

void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GlLogCall(const char* function, const char* file, int line)
{
    
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGl Error] (" << error << "):"
        << function << " " << file << ":" << line << "\n";
        return false;
    }
    return true;
}