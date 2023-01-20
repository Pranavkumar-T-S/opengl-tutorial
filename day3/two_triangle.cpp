// Drawing two triangles using Vertex Array Objects.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = &source[0];
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? 
        "vertex" : "fragment") <<" shader " << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
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
// keycallback to close window when Esc is pressed
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit()){
        std::cout << "unable to glfwInit" << std::endl;
        return -1;
    }

     // Define version and compatibility settings
     // Definition of version should be must !!!, otherwise OpenGL 2.1 will be used 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); 
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "unable to open window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    if (glewInit() != GLEW_OK)
        std::cout << " Unable to Init GLEW " << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    float x_offset, r = 0.25;
    int n;
   
    // Generating vertex coefficients for rectangle
    x_offset = 0.5;
    n = 3;
    float* arr1 = (float*)new float(n * sizeof(float));
    for (int i = 0; i < n; i++)
        {
            arr1[2*i] = r * cosf(2 * M_PI * i / n) + x_offset;
            arr1[2*i+1] = r * sinf(2 * M_PI * i / n);
        }

     // Creating Vertex Buffer Object, setting attributes pointer and vertex array object for rectangle
    unsigned int buffer1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);
    glBufferData(GL_ARRAY_BUFFER, 2*n*sizeof(int), arr1, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (const void* )0);

    glBindVertexArray(0);
     // Generating vertex coefficients for triangle
    x_offset = -0.5;
    n = 3;
    float* arr2 = (float*)alloca(n * sizeof(float));
    for (int i = 0; i < n; i++)
        {
            arr2[2*i] = r * cosf(2 * M_PI * i / n) + x_offset;
            arr2[2*i+1] = r * sinf(2 * M_PI * i / n);
        }
    
    // Creating Vertex Buffer Object, setting attributes pointer and vertex array object for triangle
    unsigned int buffer2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, 2*n*sizeof(int), arr2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (const void* )0);


    std::string vertexShader = 
        "#version 400\n"
        "\n"
        "in vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        " gl_Position = position;\n"
        "}\n";

    std::string fragmentShader = 
        "#version 400\n"
        "\n"
        "out vec4 color;"
        "\n"
        "void main()\n"
        "{\n"
        "  color = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int id = CreateShader(vertexShader,fragmentShader);
    glUseProgram(id);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}