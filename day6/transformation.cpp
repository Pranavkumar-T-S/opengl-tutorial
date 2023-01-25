// Use uniform in shader program and matrix transformation to rotate a texture
#include "opengl_template.h"
// #include <glm/mat4x4.hpp>
// #include <glm/vec3.hpp>
// #include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "uniform vec3 inp_color;"
    "uniform mat4 transform;"
    "out vec3 i_color;"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = transform *position;\n"
    "i_color = inp_color;"
    "}\n";

std::string fragmentShader =
    "#version 400\n"
    "\n"
    "out vec4 color;"
    "in vec3 i_color;"
    "\n"
    "void main()\n"
    "{\n"
    "  color = vec4(i_color, 1.0f);\n"
    "}\n";

int translate[2] = {0, 0};
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
    OpenGLContext opengl = OpenGLContext(a);
    if (!opengl.isInitiated)
    {
        std::cout << " Unable to init()" << std::endl;
        return -1;
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
    opengl.setCallback(key_callback);
    static unsigned int id = CreateShader(vertexShader, fragmentShader);
    glUseProgram(id);

    while (!glfwWindowShouldClose(opengl.window))
    {

        glClear(GL_COLOR_BUFFER_BIT);

        /* Render here */

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        unsigned int transformLoc = glGetUniformLocation(id, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        int inp_color = glGetUniformLocation(id, "inp_color");
        glUniform3f(inp_color, color[0] / 10.0, color[1] / 10.0, color[2] / 10.0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLE_FAN, n, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(opengl.window);

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