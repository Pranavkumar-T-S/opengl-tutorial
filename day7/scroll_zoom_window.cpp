// Use view matrix in vertex shader to enable scroll and zoom.
// Draw only visible range of matrix of rectangles.
#include <glm/gtc/type_ptr.hpp>
#include "opengl_template.h"

std::string vertexShader =
    "#version 400\n"
    "\n"
    "in vec4 position;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "\n"
    "void main()\n"
    "{\n"
    " //gl_Position = view * model * position;\n"
    "gl_Position = view * model * position ;"
    "}\n";

std::string fragmentShader =
    "#version 400\n"
    "\n"
    "uniform vec4 inp_color;"
    "out vec4 color;"
    "\n"
    "void main()\n"
    "{\n"
    "  color = inp_color;\n"
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
        // std::cout << "moving" << std::endl << xpos << curs;
        translation.x += -(cursorPos.x - xpos) / 500 / scale;
        translation.y += (cursorPos.y - ypos) / 500 / scale;
    }
    cursorPos.x = xpos;
    cursorPos.y = ypos;
    cursorPos.z = 0.0;
    // std::cout << "curPos " << xpos << ' ' << ypos << std::endl;
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
    scale += yoffset / 10;
    scale = scale > 0.0f ? scale : 0.0f;
    // view = glm::scale(view, glm::vec3((1.0f + yoffset / 100.0f), (1.0f + yoffset / 100.0f), 1.0f));
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

void draw_rectangle(float x, float y, float scalex, float scaley, float orientation, unsigned int program_id)
{
    unsigned int uni_model = glGetUniformLocation(program_id, "model");
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::rotate(model, orientation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scalex, scaley, 1.0f));
    // std::cout << ((float)i / rows) * 2 - 1 << " " << ((float)j / columns) * 2 - 1 << std::endl;
    glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
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
    static unsigned int id = CreateShader(vertexShader, fragmentShader);

    // getting input from user
    int rows, columns;
    std::cout << "Enter number of rows and columns: ";
    std::cin >> rows >> columns;

    // setting rectangle object vertices
    unsigned int indices[] = {0, 1, 2, 3};
    float arr[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    unsigned int vbo, ebo, vao;
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

    // setting input event callbacks
    opengl.setCallback(key_callback);
    opengl.setCurPosCallback(cursor_position_callback);
    opengl.setMouseButtonCallback(mouse_button_callback);
    opengl.setScrollCallback(scroll_callback);
    glUseProgram(id);

    // getting uniform locations
    int uni_view = glGetUniformLocation(id, "view");
    int uni_model = glGetUniformLocation(id, "model");
    int uni_color = glGetUniformLocation(id, "inp_color");

    // code to track fps
    int fps = 0;
    float time_for_fps = glfwGetTime();
    while (!glfwWindowShouldClose(opengl.window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // setting camera view
        glm::mat4 view = glm::mat4(1.0);
        view = glm::scale(view, glm::vec3(scale, scale, 1));
        view = translate(view, translation);

        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));

        // setting color
        glUniform4f(uni_color, 0.1 * color[0], 0.1 * color[1], 0.1 * color[2], 1.0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        // drawing all objects
        for (int i = 0; i < rows; i++)
        {
            if (((i + 1) * 2.0 / rows - 1.0 >= -translation.x - 1.0 / scale) && (i * 2.0 / rows - 1.0 <= -translation.x + 1.0 / scale))
            {
                for (int j = 0; j < columns; j++)
                {
                    if (((j + 1) * 2.0 / columns - 1.0 >= -translation.y - 1.0 / scale) && (j * 2.0 / columns - 1.0 <= -translation.y + 1.0 / scale))
                    {
                        draw_rectangle((2.0 * i + 1) / rows - 1.0, (2.0 * j + 1) / columns - 1.0, 0.5 / rows, 0.5 / columns, 0.0, id);
                    }
                }
            }
        }

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