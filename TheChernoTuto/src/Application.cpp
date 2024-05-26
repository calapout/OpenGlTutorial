#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */

    // SET OPEN GL AS 3.3.0 WITH CORE PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //VSYNC
    glfwSwapInterval(1);
    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error while initializing GLEW\n";
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        // contain the list of vertices position as 2D coordinate
        // The first two numbers are X and Y and the 2 next are U and V


        float positions[] = {
            -100.0f,     100.0f,    0.0f,   0.0f, // 0
            -100.0f,    -100.0f,    1.0f,   0.0f, // 1
             100.0f,    -100.0f,    1.0f,   1.0f, // 2
             100.0f,     100.0f,    0.0f,   1.0f  // 3
        };

        // Store which vertices we need to use to draw each triangle (3 per triangle and match positions indices)
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GlCall(glEnable(GL_BLEND));
        GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        //shader.SetUniform4f("u_Color", 0.3f, 0.4f, 0.3f, 1.0f);

        Texture texture("res/textures/proteccTerra.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        // reset the state
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();
        //texture.Unbind();

        //GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        Renderer renderer;

        // SETUP IMGUI
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontDefault();
        io.Fonts->Build();

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        ImGui_ImplOpenGL3_Init("#version 330");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        glm::vec3 translationA = glm::vec3(0, 0, 0);
        glm::vec3 translationB = glm::vec3(0, 0, 0);

        float r = 0.0f;
        float increment = 0.05f;

        static float f = 0.0f;


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


            //texture.Bind();
            //shader.SetUniform4f("u_Color", r, 0.4f, 0.3f, 1.0f);
            shader.Bind();
            {
                glm::mat4 modelA = glm::translate(glm::mat4(1.0f), translationA);
	            glm::mat4 MVP = proj * view * modelA;
	            shader.SetUniformMat4f("u_MVP", MVP);
	            renderer.Draw(va, ib, shader);
            }

            {
                glm::mat4 modelB = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 MVP = proj * view * modelB;
                shader.SetUniformMat4f("u_MVP", MVP);
                renderer.Draw(va, ib, shader);
            }

            if (r > 1.0f || r < 0.0f)
            {
                increment = -increment;
            }

            r += increment;

            {
                ImGui::Begin("Debug");                          // Create a window called "Hello, world!" and append into it.
                ImGui::SliderFloat3("floatA", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderFloat3("floatB", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            GlCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GlCall(glfwPollEvents());
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
