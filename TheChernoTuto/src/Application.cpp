#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

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

        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);


        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.3f, 0.4f, 0.3f, 1.0f);

        // reset the state
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();
        //GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        Renderer renderer;


        float r = 0.0f;
        float increment = 0.05f;


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.4f, 0.3f, 1.0f);

            renderer.Draw(va, ib, shader);

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
    }

    glfwTerminate();
    return 0;
}