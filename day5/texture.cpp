// Use uniform in shader program to translate and change color of polygon textured with a image
#include "opengl_template.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>

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
    " texCoord = (position.xy + vec2(1.0,1.0))/2;"
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

int translate[2] = {0, 0};
int color[3] = {10, 10, 10};

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        translate[1]++;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        translate[1]--;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        translate[0]++;
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        translate[0]--;
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
    std::cout << "Press 'wasd' to translate polygon and 'jkl' to decrease colors and 'uio' to increase colors" << std::endl;
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

    float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
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
    glDrawBuffer(GL_FRONT);
    while (!glfwWindowShouldClose(opengl.window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        int translation = glGetUniformLocation(id, "translation");
        glUniform2f(translation, 1.0 * translate[0] / 10, 1.0 * translate[1] / 10);
        int inp_color = glGetUniformLocation(id, "inp_color");
        glUniform3f(inp_color, 1.0 * color[0] / 10, 1.0 * color[1] / 10, 1.0 * color[2] / 10);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLE_FAN, n, GL_UNSIGNED_INT, 0);

        glFlush();
        // /* Swap front and back buffers */
        // glfwSwapBuffers(opengl.window);

        /* Poll for and process events */
        glfwWaitEvents();
    }
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &ebo);

    return 0;
}