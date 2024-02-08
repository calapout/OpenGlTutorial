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

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    // SET THE STATE
    shader.Bind();
    va.Bind();
    ib.Bind();

    GlCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear()
{
    GlCall(glClear(GL_COLOR_BUFFER_BIT));
}

