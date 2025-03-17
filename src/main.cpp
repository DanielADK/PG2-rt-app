#include "App.hpp"
#include "Globals.hpp"
#include "Callbacks.hpp"
#include <iostream>

int main() {

    glfwSetErrorCallback(Callbacks::error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 4.6 Core Profile", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, Callbacks::key_callback);
    glfwSetFramebufferSizeCallback(window, Callbacks::fbsize_callback);
    glfwSetMouseButtonCallback(window, Callbacks::mouse_button_callback);
    glfwSetCursorPosCallback(window, Callbacks::cursor_position_callback);
    glfwSetScrollCallback(window, Callbacks::scroll_callback);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    Utils::printContextInfo();

    App app(window);
    if (!app.init()) {
        std::cerr << "Failed to initialize App\n";
        return -1;
    }

    app.run();

    glfwTerminate();
    return 0;
}
