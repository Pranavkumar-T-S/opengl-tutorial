#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
int CreateShader(const std::string &, const std::string &);
class OpenGLContext
{

private:
    void terminate();

public:
    GLFWwindow *window;
    bool isInitiated;
    OpenGLContext(std::string windowName, int width = 640, int height = 480);
    void setCallback(GLFWkeyfun fun);
    ~OpenGLContext();
};
