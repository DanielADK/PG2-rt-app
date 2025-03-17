#include "Callbacks.hpp"
#include "Globals.hpp"

void Callbacks::error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    else if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        // change vsync state
        vsyncEnabled = !vsyncEnabled;
        glfwSwapInterval(vsyncEnabled ? 1 : 0);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        r = (r >= 1.0f) ? 0.0f : r + 0.1f;
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        g = (g >= 1.0f) ? 0.0f : g + 0.1f;
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        b = (b >= 1.0f) ? 0.0f : b + 0.1f;
    }
}

void Callbacks::fbsize_callback(GLFWwindow* window, int width, int height) {
    // Implementation
}

void Callbacks::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // Implementation
}

void Callbacks::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // Implementation
}

void Callbacks::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Implementation
}