// Drawing polygons using glBegin() and glEnd()
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

int main()
{
    GLFWwindow *window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "polygons", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    std::cout << glGetString(GL_VERSION) << std::endl;

    float r = 0.5;
    int n;
    std::cout << "Enter the number of sides of the polygon to be generated : ";
    std::cin >> n;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POLYGON);
        for (int i = 0; i < n; i++)
        {
            glVertex3f(r * cosf(2 * M_PI * i / n), r * sinf(2 * M_PI * i / n), 0.0f);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}