#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

/**
    Compiles a single shader so it can be attached
    @param type The type of the shader
    @param source The source code of the shader as string
    @return an identifier for the compiled shader
*/
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // create the shader
    const char* src = source.c_str(); // return the pointer of the first character of the source
    glShaderSource(id, 1, &src, nullptr); // specify the shader source code
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // Returns the compile status parameter
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char)); // Allocate this on the stack dynamically because 'char message[length]' is not allowed
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/**
    Links the given shaders into a single shader so that it can be bound.
    @param vertexShader Source of the vertexshader as string
    @param fragmentShader Source of the fragmentshader as string
    @return an identifier for the newly created shader
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // create a shader program to attach shader to
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach both shaders to the program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    glLinkProgram(program); // Link the program so the shaders are used
    glValidateProgram(program); // Check if the program can be executed

    // The shaders are linked to the progam, so the shaders can be deleted
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow *window;
  if (!glfwInit()) {
    std::cout << "glfw init err" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  std::cout << "I'm apple machine" << std::endl;
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  unsigned int width = 800;
  unsigned int height = 600;
  window = glfwCreateWindow(width, height, "Learngl", NULL, NULL);
  if (!window) {
    std::cout << " can't create window!!!" << std::endl;
    glfwTerminate();
    return -1;
  }
  unsigned int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  unsigned int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  std::cout << "oepngl shader version: " << major << "." << minor << std::endl;
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cout << "Error ! " << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  float positions[6] = {
    -1.0f, -0.5f,
    0.0f,  0.5f,
    0.5f, -0.5f,
  };
  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);

  std::string vertexShader = "#version 330 core\n"
    "layout (location = 0) in vec4 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = position;\n"
    "}\0";
  std::string fragmentShader = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0, 0.0, 0.5f);\n"
    "}\n\0";
  unsigned int shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}