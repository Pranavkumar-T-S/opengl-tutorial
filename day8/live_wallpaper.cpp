#include "opengl_template.h"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include "progressive_waves.frag"

std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "out vec2 texCoord;"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = position;\n"
    " texCoord = (position.xy + vec2(1.0,1.0))/2;"
    "}\n";

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
    std::string a = "OpenGL window";
    OpenGLContext opengl = OpenGLContext("OpenGL window", 720, 720);
    if (!opengl.isInitiated)
    {
        std::cout << " Unable to init()" << std::endl;
        return -1;
    }
    // setup code
    int width, height, nrChannels;
    static unsigned int id = CreateShader(vertexShader, fragmentShader);

    // Generating vertex coefficients and index array
    unsigned int indices[] = {0, 1, 2, 3};
    float arr[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    unsigned int vbo, ebo, vao, texture;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), arr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);
    opengl.setCallback(key_callback);
    glUseProgram(id);

    unsigned int uni_resolution = glGetUniformLocation(id, "resolution");
    int uni_time = glGetUniformLocation(id, "time");

    glUniform2f(uni_resolution, (float) 720, (float) 720);
    
    int fps = 0;
    float time_for_fps = glfwGetTime();
    while (!glfwWindowShouldClose(opengl.window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // setting uniforms
        glUniform1f(uni_time, glfwGetTime());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);

        // code to track fps
        fps++;
        if (glfwGetTime() - time_for_fps >= 1)
        {
            time_for_fps = glfwGetTime();
            std::cout << "fps: " << fps << std::endl;
            fps = 0;
        }
        // /* Swap front and back buffers */
        glfwSwapBuffers(opengl.window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(id);
    return 0;
}