#include "opengl_template.h"
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>
#include "gaussian_blur2.frag"
std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "uniform mat4 transformation; "
    "out vec2 texCoord;"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = transformation * position;\n"
    " texCoord = (position.xy + vec2(1.0,1.0))/2;"
    "}\n";

float sigma = 0.0f;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        sigma += 0.001;
        std::cout << "sigma: " << sigma << std::endl;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        sigma = sigma - 0.001 <= 0.0001 ? 0 : sigma - 0.001;
        std::cout << "sigma: " << sigma << std::endl;
    }
}

int main()
{
    std::string a = "OpenGL window";
    OpenGLContext opengl = OpenGLContext("OpenGL window", 1024, 1024);
    if (!opengl.isInitiated)
    {
        std::cout << " Unable to init()" << std::endl;
        return -1;
    }
    // setup code
    int width, height, nrChannels;
    static unsigned int id = CreateShader(vertexShader, fragmentShader);
    // loading texture image
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("day5/opengl.png", &width, &height, &nrChannels, 0);

    // Generating vertex coefficients and index array
    unsigned int indices[] = {0, 1, 2, 3};
    float arr[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    unsigned int vbo, ebo, vao, texture;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    if (!data)
    {
        std::cout << "Unable to load texture image" << std::endl;
        return 0;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), arr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);
    opengl.setCallback(key_callback);
    glUseProgram(id);
    // glDrawBuffer(GL_FRONT);
    glm::mat4 trans;
    unsigned int uni_transform = glGetUniformLocation(id, "transformation");
    int uni_sigma = glGetUniformLocation(id, "sigma");

    int fps = 0;
    float time_for_fps = glfwGetTime();
    while (!glfwWindowShouldClose(opengl.window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        // setting uniforms

        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 0.0f));
        trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

        glUniformMatrix4fv(uni_transform, 1, GL_FALSE, glm::value_ptr(trans));
        glUniform1f(uni_sigma, 0.0f);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);

        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, 0.0f, 0.0f));
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        glUniformMatrix4fv(uni_transform, 1, GL_FALSE, glm::value_ptr(trans));
        // std::cout << sigma << std::endl;
        glUniform1f(uni_sigma, sigma);

        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);

        // glFlush();
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