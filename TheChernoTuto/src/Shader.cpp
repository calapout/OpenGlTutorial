#include "Shader.h"
#include "GL/glew.h"
#include "Renderer.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

Shader::Shader(const std::string& filepath)
	:m_Filepath(filepath), m_RendererID(0)
{
    // Parse the Basic.shader file and gives une back a struct containing the Vertex shader and Fragment shader
    ShaderProgramSources sources = ParseShader(filepath);
    // Create a program that compile and bind our vertex and fragment shader and then bind it to our state
    m_RendererID = CreateShader(sources.VertexSource, sources.FragmentSource);
}

Shader::~Shader()
{
    GlCall(glDeleteProgram(m_RendererID));
}


void Shader::Bind() const
{
    GlCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GlCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    // Create a uniform vector4f to set the color from c++
    GlCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    GlCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if(location == -1)
    {
        std::cout << "Warning: Uniform '" << name << "' doesn't exist!\n";
    }

    m_UniformLocationCache[name] = location;
    return location;
}

/**
 * \brief Compiles a shader using it's type and source code and returns it's id
 * \param type Shader type
 * \param source the source code of the shader to compile
 * \return 0 if the compilation fails, otherwise we return the id of the shader
 */
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    GlCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();

    GlCall(glShaderSource(id, 1, &src, nullptr));
    GlCall(glCompileShader(id));

    int result;
    GlCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    // shader didn't compiled properly
    if (result == GL_FALSE)
    {
        int length;
        GlCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));

        GlCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GlCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

/**
 * \brief Compile a vertex shader and a fragment shader and associate them in a program and return it's id
 * \param vertexShader The source code of the vertexShader
 * \param fragmentShader The source code of the fragmentShader
 * \return the id of a program
 */
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GlCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GlCall(glAttachShader(program, vs));
    GlCall(glAttachShader(program, fs));
    GlCall(glLinkProgram(program));
    GlCall(glValidateProgram(program));

    GlCall(glDeleteShader(vs));
    GlCall(glDeleteShader(fs));

    return program;
}

ShaderProgramSources Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::stringstream ss[2];
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
            continue;
        }

        ss[(int)type] << line << "\n";
    }

    return ShaderProgramSources{ ss[0].str(), ss[1].str() };
}