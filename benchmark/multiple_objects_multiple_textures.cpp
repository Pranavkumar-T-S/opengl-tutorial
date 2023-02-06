// Use uniform in shader program and matrix transformation to rotate multiple objects with multiple textures
#include "opengl_template.h"
// #include <glm/mat4x4.hpp>
// #include <glm/vec3.hpp>
// #include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "uniform mat4 transform;"
    "out vec2 texCoord;"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = transform *position;\n"
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
    "color = vec4(inp_color, 1.0f) * texture(texture1, texCoord);\n"
    "}\n";

int color[3] = {10, 10, 10};

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
        color[0] = ((color[0] < 1 ? 10 : color[0]) - 1) % 10;
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
        color[1] = ((color[1] < 1 ? 10 : color[1]) - 1) % 10;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        color[2] = ((color[2] < 1 ? 10 : color[2]) - 1) % 10;
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
        color[0] = (color[0] + 1) % 10;
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        color[1] = (color[1] + 1) % 10;
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        color[2] = (color[2] + 1) % 10;
}

int main()
{
    std::string a = "OpenGL window";
    OpenGLContext opengl = OpenGLContext(a, 1024, 1024);
    if (!opengl.isInitiated)
    {
        std::cout << " Unable to init()" << std::endl;
        return -1;
    }

    // loading texture image
    int width, height, nrChannels, num_of_textures = 50;
    stbi_set_flip_vertically_on_load(true);
    unsigned int textures[num_of_textures];
    glGenTextures(num_of_textures, textures);

    for (int i = 0; i < num_of_textures; i++)
    {
        char texture_name[50];
        snprintf(texture_name, 50, "textures/%d.png", i + 1);
        unsigned char *data = stbi_load(texture_name, &width, &height, &nrChannels, 0);
        if (!data)
        {
            std::cout << "Unable to load texture image" << std::endl;
            return 0;
        }
        // transmitting texture image
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        stbi_image_free(data);
    }
    // generate vertex points
    float r = 0.5;
    int n;
    std::cout << " Enter the number of points to be generated: " << std::endl;
    std::cin >> n;
    std::cout << "Press 'jkl' to decrease colors and 'uio' to increase colors" << std::endl;

    // Generating vertex coefficients and index array
    float *arr = new float[2 * n];
    unsigned int *indices = new unsigned int[n];
    for (int i = 0; i < n; i++)
    {
        arr[2 * i] = r * cosf(2 * M_PI * i / n);
        arr[2 * i + 1] = r * sinf(2 * M_PI * i / n);
        indices[i] = i;
    }
    // transmitting vertex points and indices
    unsigned int vbo, ebo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, 2 * n * sizeof(float), arr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);

    // setting keyboard callback and loading shader program
    opengl.setCallback(key_callback);
    static unsigned int id = CreateShader(vertexShader, fragmentShader);
    glUseProgram(id);

    // getting input from user
    int rows, columns, fps = 0;
    std::cout << "Enter number of objects per row/column: " << std::endl;
    std::cin >> rows;
    columns = rows;
    float time_for_fps = glfwGetTime();
    long long cpu_time = 0, gpu_time = 0;

    while (!glfwWindowShouldClose(opengl.window))
    {

        glClear(GL_COLOR_BUFFER_BIT);

        /* Render here */

        int inp_color = glGetUniformLocation(id, "inp_color");
        glUniform3f(inp_color, color[0] / 10.0, color[1] / 10.0, color[2] / 10.0);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {

                glBindTexture(GL_TEXTURE_2D, textures[(i * rows + j) % num_of_textures]);
                glm::mat4 trans = glm::mat4(1.0f);
                trans = glm::translate(trans, glm::vec3((i + 0.5) / rows * 2 - 1, ((j + 0.5) / columns) * 2 - 1, 0.0f));
                trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
                trans = glm::scale(trans, glm::vec3(1.0f / rows, 1.0f / columns, 1.0));
                // std::cout << ((float)i / rows) * 2 - 1 << " " << ((float)j / columns) * 2 - 1 << std::endl;
                unsigned int transformLoc = glGetUniformLocation(id, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glDrawElements(GL_TRIANGLE_FAN, n, GL_UNSIGNED_INT, 0);
            }
        }
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        cpu_time += std::chrono::duration_cast<std::chrono::microseconds>(
                        elapsed)
                        .count();

        // code to track fps
        fps++;
        if (glfwGetTime() - time_for_fps >= 1)
        {
            time_for_fps = glfwGetTime();
            std::cout << "fps: " << fps << " cpu time: " << cpu_time << "  gpu time: " << gpu_time << std::endl;
            fps = 0;
            cpu_time = gpu_time = 0;
        }
        start = std::chrono::high_resolution_clock::now();
        /* Swap front and back buffers */
        glfwSwapBuffers(opengl.window);
        elapsed = std::chrono::high_resolution_clock::now() - start;

        gpu_time += std::chrono::duration_cast<std::chrono::microseconds>(
                        elapsed)
                        .count();

        /* Wait for and process events */
        glfwPollEvents();
    }
    delete[] arr;
    delete[] indices;
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(id);
}