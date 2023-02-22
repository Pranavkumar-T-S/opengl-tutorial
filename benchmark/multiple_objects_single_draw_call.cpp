// Use view matrix in shader program and draw multiple objects by single OpenGL draw call
// compute all objects vertices and load it in single VBO
// Add random color to vertices and change colors for one object using glMapBufferRange() each frame
#include "opengl_template.h"
// #include <glm/mat4x4.hpp>
// #include <glm/vec3.hpp>
// #include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::string vertexShader =
    "#version 400\n"
    "in vec4 position;\n"
    "in vec3 color;\n"
    "out vec3 ver_color;\n"
    "uniform mat4 view;\n"
    "\n"
    "void main()\n"
    "{\n"
    " gl_Position = view * position;\n"
    "ver_color = color;\n"
    "}\n";

std::string fragmentShader =
    "#version 400\n"
    "\n"
    "in vec3 ver_color;"
    "out vec4 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "color = vec4(ver_color, 1.0f);\n"
    "}\n";

int color[3] = {10, 10, 10};
int track_cursor = false;
glm::vec3 cursorPos;
glm::vec3 translation = glm::vec3(0.0f);
float scale = 1.0;

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{

    if (track_cursor == 1)
    {
        translation.x += -(cursorPos.x - xpos) / 500 / scale;
        translation.y += (cursorPos.y - ypos) / 500 / scale;
    }
    cursorPos.x = xpos;
    cursorPos.y = ypos;
    cursorPos.z = 0.0;
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        track_cursor = action;
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    scale *= (1 + yoffset / 10);
    scale = scale > 0.0f ? scale : 0.0f;
    // std::cout << "mouse_scroll  " << xoffset << ' ' << yoffset << std::endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        translation.y += 0.1 / scale;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        translation.x -= 0.1 / scale;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        translation.y -= 0.1 / scale;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        translation.x += 0.1 / scale;
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        translation = glm::vec3(0.0);
        scale = 1.0f;
    }

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

    // Generate vertex points for the object/shape
    float r = 0.5;
    int n;
    std::cout << " Enter the number of points to be generated: " << std::endl;
    std::cin >> n;
    std::cout << "Press 'jkl' to decrease colors and 'uio' to increase colors" << std::endl;

    int rows, columns, fps = 0;
    std::cout << "Enter number of objects per row/column: " << std::endl;
    std::cin >> rows;
    columns = rows;
    std::unique_ptr<float[]> vertex_arr = std::unique_ptr<float[]>(new float[2 * n]);
    for (int i = 0; i < n; i++)
    {
        vertex_arr[2 * i] = r * cosf(2 * M_PI * i / n);
        vertex_arr[2 * i + 1] = r * sinf(2 * M_PI * i / n);
    }

    // Generate transformed vertex coefficients and index array for all objects
    std::unique_ptr<float[]> transformed_vertices = std::unique_ptr<float[]>(new float[2 * n * rows * columns]);
    std::unique_ptr<uint[]> indices = std::unique_ptr<uint[]>(new uint[(n + 1) * rows * columns]);

    for (int row = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++)
        {
            // Compute model matrix for each object
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(((float)(column + 0.5) / columns) * 2 - 1, ((float)(row + 0.5) / rows) * 2 - 1, 0.0f));
            trans = glm::scale(trans, glm::vec3(1.0f / rows, 1.0f / columns, 1.0));
            for (int i = 0; i < n; i++)
            // Transform each vertex to model space
            {
                glm::vec4 vertex(vertex_arr[2 * i], vertex_arr[2 * i + 1], 0.0f, 1.0f);
                vertex = trans * vertex;
                transformed_vertices[(((row * columns) + column) * n + i) * 5] = vertex.x;
                transformed_vertices[(((row * columns) + column) * n + i) * 5 + 1] = vertex.y;
                float time = glfwGetTime() > 1 ? glfwGetTime() - (int)glfwGetTime() : glfwGetTime();
                transformed_vertices[(((row * columns) + column) * n + i) * 5 + 2] = (float)(rand()) / RAND_MAX;
                transformed_vertices[(((row * columns) + column) * n + i) * 5 + 3] = (float)(rand()) / RAND_MAX;
                transformed_vertices[(((row * columns) + column) * n + i) * 5 + 4] = (float)(rand()) / RAND_MAX;
                indices[((row * columns) + column) * (n + 1) + i] = ((row * columns) + column) * n + i;
            }
            // Add Primitive Restart Index at last for every object indices
            indices[((row * columns) + column) * (n + 1) + n] = -1;
        }
    }
    // Uncomment to print computed vertices and indices for each object
    // for (int i = 0; i < rows * columns; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         std::cout << transformed_vertices[(i * n) + j * 5] << ' ' << transformed_vertices[(i * n) + j * 5 + 1] << ' ' << transformed_vertices[(i * n) + j * 5 + 2] << ' ' << transformed_vertices[(i * n) + j * 5 + 3] << ' ' << transformed_vertices[(i * n) + j * 5 + 4] << ' ' << indices[i * (n + 1) + j] << "   ";
    //     }
    //     std::cout << indices[i * (n + 1) + n];

    //     std::cout << std::endl;
    // }

    static unsigned int id = CreateShader(vertexShader, fragmentShader);
    glUseProgram(id);
    uint sl_positions, sl_colors;
    sl_positions = glGetAttribLocation(id, "position");
    sl_colors = glGetAttribLocation(id, "color");

    // Transmit vertex points and indices
    unsigned int ebo;
    unsigned int vao, vbo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 5 * n * rows * columns * sizeof(float), transformed_vertices.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (n + 1) * rows * columns * sizeof(uint), indices.get(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(sl_positions);
    glVertexAttribPointer(sl_positions, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(sl_colors);
    glVertexAttribPointer(sl_colors, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)(2 * sizeof(float)));

    // Set GLFW callbacks
    opengl.setCallback(key_callback);
    opengl.setCurPosCallback(cursor_position_callback);
    opengl.setMouseButtonCallback(mouse_button_callback);
    opengl.setScrollCallback(scroll_callback);

    // Set Primitive Restart Index
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex((uint)-1);

    float time_for_fps = glfwGetTime();

    // Get uniform locations
    int uni_view = glGetUniformLocation(id, "view");
    int uni_model = glGetUniformLocation(id, "model");
    int uni_color = glGetUniformLocation(id, "inp_color");

    while (!glfwWindowShouldClose(opengl.window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        /* Render here */

        // Set view matrix
        glm::mat4 view = glm::mat4(1.0);
        view = glm::scale(view, glm::vec3(scale, scale, 1));
        view = translate(view, translation);

        glUniform3f(uni_color, color[0] / 10.0, color[1] / 10.0, color[2] / 10.0);
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_FAN, (n + 1) * rows * columns, GL_UNSIGNED_INT, 0);

        // code to track fps
        fps++;
        if (glfwGetTime() - time_for_fps >= 1)
        {
            time_for_fps = glfwGetTime();
            std::cout << "fps: " << fps << std::endl;
            fps = 0;
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(opengl.window);

        /* Wait for and process events */
        glfwPollEvents();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(id);
}