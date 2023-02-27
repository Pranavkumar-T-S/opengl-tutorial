#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define M_PI 3.14159265358979323846
#include <chrono>
#define uint unsigned int

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
    void setMouseButtonCallback(GLFWmousebuttonfun fun);
    void setCurPosCallback(GLFWcursorposfun fun);
    void setScrollCallback(GLFWscrollfun fun);
    ~OpenGLContext();
};
