// Use texture and uniform to animate a moving picture like screen saver
#include "opengl_template.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>
#include <glm/vec3.hpp>                  // glm::vec3
#include <glm/vec4.hpp>                  // glm::vec4
#include <glm/mat4x4.hpp>                // glm::mat4
#include <glm/ext/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>

std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "uniform vec2 translation; "
    "out vec2 texCoord;"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = position + vec4(translation, 0.0f, 0.0f);\n"
    " texCoord = position.xy + vec2(0.5,0.5);"
    "}\n";

std::string fragmentShader =
    "#version 400\n"
    "\n"
    "in vec2 texCoord;"
    "out vec4 color;"
    "uniform vec3 inp_color;"
    "uniform sampler2D texture1; "
    "\n"
    "void main()\n"
    "{\n"
    "  color = vec4(inp_color, 1.0f) * texture(texture1, texCoord);\n"
    "}\n";

glm::vec2 displacement(0, 0);
glm::vec2 velocity(0.15, 0.25);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
    std::string a = "OpenGL window";
    OpenGLContext opengl = OpenGLContext("OpenGL window");
    if (!opengl.isInitiated)
    {
        std::cout << " Unable to init()" << std::endl;
        return -1;
    }
    // setup code
    float r = 0.5;
    int n, width, height, nrChannels;
    static unsigned int id = CreateShader(vertexShader, fragmentShader);
    // loading texture image
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("day5/opengl.png", &width, &height, &nrChannels, 0);

    // Generating vertex coefficients and index array
    std::cout << " Enter the number of points to be generated: " << std::endl;
    std::cin >> n;
    float *arr = new float[2 * n];
    unsigned int *indices = new unsigned int[n];
    for (int i = 0; i < n; i++)
    {
        arr[2 * i] = r * cosf(2 * M_PI * i / n);
        arr[2 * i + 1] = r * sinf(2 * M_PI * i / n);
        indices[i] = i;
    }
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

    float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glBufferData(GL_ARRAY_BUFFER, 2 * n * sizeof(float), arr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);
    opengl.setCallback(key_callback);
    glUseProgram(id);
    // glDrawBuffer(GL_FRONT);
    int fps = 0, color_counter = 0;
    float time_for_render = glfwGetTime(), time_for_fps = time_for_render, time_elapsed;
    std::cout << glfwGetTimerFrequency() << std::endl;
    while (!glfwWindowShouldClose(opengl.window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        time_elapsed = glfwGetTime() - time_for_render;
        time_for_render = glfwGetTime();
        if (displacement.x + velocity.x * time_elapsed >= 0.5 || displacement.x + velocity.x * time_elapsed <= -0.5)
        {
            velocity.x = -velocity.x;
            color_counter+= 3;
        }
        if (displacement.y + velocity.y * time_elapsed >= 0.5 || displacement.y + velocity.y * time_elapsed <= -0.5)
        {
            velocity.y = -velocity.y;
            color_counter+= 9;
        }
        color_counter %= 64;
        displacement = displacement + velocity * time_elapsed;

        int translation = glGetUniformLocation(id, "translation");
        glUniform2f(translation, displacement.x, displacement.y);
        int inp_color = glGetUniformLocation(id, "inp_color");
        glUniform3f(inp_color, (color_counter / 16 % 4) / 3.0, (color_counter / 4 % 4) / 3.0, (color_counter / 1 % 4) / 3.0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLE_FAN, n, GL_UNSIGNED_INT, 0);
        fps++;
        // glFlush();
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
    delete[] indices;
    delete[] arr;
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);

    return 0;
}