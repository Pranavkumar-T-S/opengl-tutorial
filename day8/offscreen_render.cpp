#include "opengl_template.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "../filters/gaussian_blur2.frag"

std::string vertexShader = "#version 400\n"
                           "\n"
                           "in vec4 position;"
                           "uniform mat4 transformation; "
                           "out vec2 texCoord;"
                           "\n"
                           "void main()\n"
                           "{\n"
                           " gl_Position = transformation * position;\n"
                           " texCoord = (position.xy + vec2(1.0,1.0))/2;"
                           "}\n";

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
  std::string a = "OpenGL window";
  OpenGLContext opengl = OpenGLContext("OpenGL window", WIDTH, HEIGHT);
  if (!opengl.isInitiated) {
    std::cout << " Unable to init()" << std::endl;
    return -1;
  }

  static unsigned int id = CreateShader(vertexShader, fragmentShader);

  // reading image
  int iwidth, iheight, inrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load("day5/opengl.png", &iwidth, &iheight, &inrChannels, 0);

  if (!data) {
    std::cout << "Unable to load texture image" << std::endl;
    return 0;
  }
  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  float borderColor[] = {1.0f, 0.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iwidth, iheight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  // creating offscreen texture based frame buffer
  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  GLuint texture2;
  float xscale, yscale;
  glfwGetWindowContentScale(opengl.window, &xscale, &yscale);
  int width = xscale * WIDTH, height = yscale * HEIGHT;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture2, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "offscreen frame buffer incomplete" << std::endl;
  }

  // Generating vertex coefficients and index array
  unsigned int indices[] = {0, 1, 2, 3};
  float arr[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
  unsigned int vbo, ebo, vao;

  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), arr, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (const void *)0);
  glEnableVertexAttribArray(0);

  glUseProgram(id);

  glm::mat4 trans;
  unsigned int uni_transform = glGetUniformLocation(id, "transformation");
  int uni_sigma = glGetUniformLocation(id, "sigma");

  glBindTexture(GL_TEXTURE_2D, texture);

  auto start = std::chrono::high_resolution_clock::now();

  /* Render here */
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glClear(GL_COLOR_BUFFER_BIT);

  // drawing original image
  trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 0.0f));
  trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

  glUniformMatrix4fv(uni_transform, 1, GL_FALSE, glm::value_ptr(trans));
  glUniform1f(uni_sigma, 0.0f);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);

  // drawing blurred image
  trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(0.5f, 0.0f, 0.0f));
  trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

  glUniformMatrix4fv(uni_transform, 1, GL_FALSE, glm::value_ptr(trans));
  glUniform1f(uni_sigma, 0.05f);

  glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
  glFinish();

  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now() - start)
                   .count()
            << std::endl;

  GLsizei nrChannels = 4;
  GLsizei stride = nrChannels * width;
  GLsizei bufferSize = stride * height;
  char buffer[bufferSize];

  glPixelStorei(GL_PACK_ALIGNMENT, 4);

  glBindTexture(GL_TEXTURE_2D, texture2);
  //   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  start = std::chrono::high_resolution_clock::now();
  //   glReadPixels(0, 0, width, height , GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  std::cout << "read pixel/ getTexImage tme: "
            << std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now() - start)
                   .count()
            << std::endl;

  stbi_flip_vertically_on_write(true);
  stbi_write_png("output.png", width, height, nrChannels, buffer, stride);
}