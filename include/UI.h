#ifndef UI_H
#define UI_H

#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

class UI {
private: 
    std::string windowTitle = "UI";
    int windowWidth = 1280, windowHeight = 720;
    GLFWwindow * window = nullptr;
    void openglConfig();

public: 
    UI();
    UI(int width, int height, std::string title);
    ~UI();
    void initialize();
    void draw(double deltaTime);
    void update();
    int run();
    void show(); // displays a mesh object in the scene

};

#endif //UI_H
