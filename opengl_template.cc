#include "opengl_template.h"
static unsigned int CompileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void OpenGLContext::terminate()
{
    glfwTerminate();
}

OpenGLContext::OpenGLContext(std::string windowName, int width, int height)
{
    this->isInitiated = false;
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "unable to glfwInit" << std::endl;
        return;
    }

    // Define version and compatibility settings
    // Definition of version should be must !!!, otherwise OpenGL 2.1 will be used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, &windowName[0], NULL, NULL);

    if (!this->window)
    {
        std::cout << "unable to open window" << std::endl;
        this->terminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(this->window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << " Unable to Init GLEW " << std::endl;
        terminate();
        return;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    this->isInitiated = true;
}

void OpenGLContext::setCallback(GLFWkeyfun fun)
{
    glfwSetKeyCallback(this->window, fun);
}

OpenGLContext::~OpenGLContext()
{
    this->terminate();
}