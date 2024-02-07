#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSources
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& filepath)
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
    while(std::getline(stream, line))
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


/**
 * \brief Compiles a shader using it's type and source code and returns it's id
 * \param type Shader type
 * \param source the source code of the shader to compile
 * \return 0 if the compilation fails, otherwise we return the id of the shader
 */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    GlCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    
    GlCall(glShaderSource(id, 1, &src, nullptr));
    GlCall(glCompileShader(id));

    int result;
    GlCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    
    // shader didn't compiled properly
    if(result == GL_FALSE)
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
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */

    // SET OPEN GL AS 3.3.0 WITH CORE PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error while initializing GLEW\n";
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        // contain the list of vertices position as 2D coordinate
        float positions[] = {
            -0.5f,  -0.5f,
             0.5f,  -0.5f,
             0.5f,   0.5f,
            -0.5f,   0.5f,
        };

        // Store which vertices we need to use to draw each triangle (3 per triangle and match positions indices)
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // created a vertex array object
        unsigned int vao;
        GlCall(glGenVertexArrays(1, &vao));
        GlCall(glBindVertexArray(vao));


        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);


        // Parse the Basic.shader file and gives une back a struct containing the Vertex shader and Fragment shader
        ShaderProgramSources source = ParseShader("res/shaders/Basic.shader");

        // Create a program that compile and bind our vertex and fragment shader and then bind it to our state
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GlCall(glUseProgram(shader));

        // Create a uniform vector4f to set the color from c++
        GlCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        GlCall(glUniform4f(location, 0.3f, 0.4f, 0.3f, 1.0f));

        // reset the state
        GlCall(glBindVertexArray(0));
        GlCall(glUseProgram(0));
        GlCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GlCall(glClear(GL_COLOR_BUFFER_BIT));

            // SET THE STATE
            GlCall(glUseProgram(shader));
            GlCall(glUniform4f(location, r, 0.4f, 0.3f, 1.0f));

            va.Bind();
            ib.Bind();
        	GlCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f || r < 0.0f)
            {
                increment = -increment;
            }

            r += increment;

            /* Swap front and back buffers */
            GlCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GlCall(glfwPollEvents());
        }

        GlCall(glDeleteProgram(shader));
    }

    glfwTerminate();
    return 0;
}