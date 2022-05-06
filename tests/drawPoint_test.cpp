#include <stdio.h>
#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include "ogldev_math_3d.h"

// struct Vector3f
// {
//     union {
//         float x;
//         float r;
//     };

//     union {
//         float y;
//         float g;
//     };

//     union {
//         float z;
//         float b;
//     };

//     Vector3f() {}

//     Vector3f(float _x, float _y, float _z)
//     {
//         x = _x;
//         y = _y;
//         z = _z;
//     }

//     Vector3f(const float* pFloat)
//     {
//         x = pFloat[0];
//         y = pFloat[1];
//         z = pFloat[2];
//     }

//     Vector3f(float f)
//     {
//         x = y = z = f;
//     }

//     Vector3f(const Vector4f& v);

//     Vector3f& operator+=(const Vector3f& r)
//     {
//         x += r.x;
//         y += r.y;
//         z += r.z;

//         return *this;
//     }

//     Vector3f& operator-=(const Vector3f& r)
//     {
//         x -= r.x;
//         y -= r.y;
//         z -= r.z;

//         return *this;
//     }

//     Vector3f& operator*=(float f)
//     {
//         x *= f;
//         y *= f;
//         z *= f;

//         return *this;
//     }

//     bool operator==(const Vector3f& r)
//     {
//         return ((x == r.x) && (y == r.y) && (z == r.z));
//     }

//     bool operator!=(const Vector3f& r)
//     {
//         return !(*this == r);
//     }

//     operator const float*() const
//     {
//         return &(x);
//     }


//     Vector3f Cross(const Vector3f& v) const;

//     float Dot(const Vector3f& v) const
//     {
//         float ret = x * v.x + y * v.y + z * v.z;
//         return ret;
//     }

//     float Distance(const Vector3f& v) const
//     {
//         float delta_x = x - v.x;
//         float delta_y = y - v.y;
//         float delta_z = z - v.z;
//         float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
//         return distance;
//     }

//     float Length() const
//     {
//         float len = sqrtf(x * x + y * y + z * z);
//         return len;
//     }

//     Vector3f& Normalize();

//     void Rotate(float Angle, const Vector3f& Axis);

//     Vector3f Negate() const;

//     void Print(bool endl = true) const
//     {
//         printf("(%f, %f, %f)", x, y, z);

//         if (endl) {
//             printf("\n");
//         }
//     }
// };

// GLuint VBO;

// static void RenderSceneCB()
// {
//     glClear(GL_COLOR_BUFFER_BIT);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);

//     glEnableVertexAttribArray(0);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

//     glDrawArrays(GL_POINTS, 0, 1);

//     glDisableVertexAttribArray(0);

//     glutSwapBuffers();
// }


// static void CreateVertexBuffer()
// {
//     Vector3f Vertices[1];
//     Vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);

//     glGenBuffers(1, &VBO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
// }


// int main(int argc, char** argv)
// {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
//     int width = 1920;
//     int height = 1080;
//     glutInitWindowSize(width, height);

//     int x = 200;
//     int y = 100;
//     glutInitWindowPosition(x, y);
//     int win = glutCreateWindow("Tutorial 02");
//     printf("window id: %d\n", win);

//     // Must be done after glut is initialized!
//     GLenum res = glewInit();
//     if (res != GLEW_OK) {
//         fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
//         return 1;
//     }

//     GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
//     glClearColor(Red, Green, Blue, Alpha);

//     CreateVertexBuffer();

//     glutDisplayFunc(RenderSceneCB);

//     glutMainLoop();

//     return 0;
// }
int main() {
    return 0;
}

// void openglConfig() {
//     // Request that OpenGL is 3.3
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     // Only enable core functionalities (disable features from older OpenGL versions that were removed in 3.3)
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//     // Enable forward compatibility with newer OpenGL versions by removing deprecated functionalities
//     // This is necessary for some platforms
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

//     //Make window size fixed (User can't resize it)
//     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

//     //Set Number of sample used in MSAA (0 = Disabled)
//     glfwWindowHint(GLFW_SAMPLES, 0);

//     //Enable Double Buffering
//     glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

//     //Set the bit-depths of the frame buffer
//     glfwWindowHint(GLFW_RED_BITS, 8);
//     glfwWindowHint(GLFW_GREEN_BITS, 8);
//     glfwWindowHint(GLFW_BLUE_BITS, 8);
//     glfwWindowHint(GLFW_ALPHA_BITS, 8);

//     //Set Bits for Depth Buffer
//     glfwWindowHint(GLFW_DEPTH_BITS, 24);

//     //Set Bits for Stencil Buffer
//     glfwWindowHint(GLFW_STENCIL_BITS, 8);

//     //Set the refresh rate of the window (GLFW_DONT_CARE = Run as fast as possible)
//     glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
// }

// int main() {
//     int windowWidth = 1280, windowHeight = 720;
//     GLFWwindow * window = nullptr;
//     // Set the function to call when an error occurs.
//     //glfwSetErrorCallback(glfw_error_callback);

//     // Initialize GLFW and exit if it failed
//     if(!glfwInit()){
//         std::cerr << "Failed to Initialize GLFW" << std::endl;
//         return -1;
//     }

//     openglConfig(); 
//     window = glfwCreateWindow(windowWidth, windowHeight, "UI", nullptr, nullptr);
//     if(!window) {
//         std::cerr << "Failed to Create Window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);         // Tell GLFW to make the context of our window the main context on the current thread.

//     gladLoadGL(glfwGetProcAddress);         // Load the OpenGL functions from the driver

//     // Print information about the OpenGL context
//     std::cout << "VENDOR          : " << glGetString(GL_VENDOR) << std::endl;
//     std::cout << "RENDERER        : " << glGetString(GL_RENDERER) << std::endl;
//     std::cout << "VERSION         : " << glGetString(GL_VERSION) << std::endl;
//     std::cout << "GLSL VERSION    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

//     glClearColor(0.5f, 0.2f, 0.7f, 1.0f);

//     while(!glfwWindowShouldClose(window)){
//         glfwPollEvents();
//         glViewport(0, 0, windowWidth, windowHeight);
//         glClear(GL_COLOR_BUFFER_BIT);
//         glfwSwapBuffers(window);
//     }

//     // Destroy the window
//     glfwDestroyWindow(window);

//     // And finally terminate GLFW
//     glfwTerminate();
//     return 0; // Good bye
// }