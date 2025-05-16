
#include "App.hpp"

void App::error_callback(int error, const char* description) {
    Logger::error(error + ": " + std::string(description));
}

// Key callback
void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));

    // Only process key press events for most keys
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;

            case GLFW_KEY_SPACE:
                if (!app->free_cam && !app->is_jumping) {
                    app->is_jumping = true;
                    app->jump_velocity = 2.0f;
                }
                break;

            case GLFW_KEY_F:
                app->flashlight_on = !app->flashlight_on;
                Logger::info("Flashlight: " + std::string(app->flashlight_on ? "ON" : "OFF"));
                break;

            case GLFW_KEY_F1:
                app->show_imgui = !app->show_imgui;
                break;

            case GLFW_KEY_F2:
                app->free_cam = !app->free_cam;
                Logger::info("FreeCam: " + std::string(app->free_cam ? "ON" : "OFF"));
                break;

            case GLFW_KEY_F3:
                app->toggle_vsync();
                break;

            case GLFW_KEY_F12:
                app->toggle_fullscreen();
                break;

            default:
                break;
        }
    }
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    
    this_inst->m_fov += static_cast<float>(yoffset);
    this_inst->m_fov = std::clamp(this_inst->m_fov, 30.0f, 90.0f);

    this_inst->update_projection_matrix(window);
}


void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    static bool firstMouse = true;
    static float lastX = 400, lastY = 300;  // Assume the screen center initially

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Call the function on your Camera object (assuming it's named 'camera')
    this_inst->m_Camera->handle_mouse(xoffset, yoffset);
}


void App::fbsize_callback(GLFWwindow* window, int width, int height) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->m_width = width;
    app->m_height = height;
    glViewport(0, 0, width, height);
    app->update_projection_matrix(window);
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT: {
                int mode = glfwGetInputMode(window, GLFW_CURSOR);
                if (mode == GLFW_CURSOR_NORMAL) {
                    // we are outside of application, catch the cursor
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                else {
                    // we are already inside our game: shoot, click, etc.
                    std::cout << "Bang!\n";
                }
                break;
            }
            case GLFW_MOUSE_BUTTON_RIGHT: {
                // release the cursor
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            }
            default:
                break;
        }
    }
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Get the App instance from the window user pointer
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));

    // Update the viewport to match the new window dimensions
    glViewport(0, 0, width, height);

    if (app) {
        app->m_width = width;
        app->m_height = height;

        // Update the projection matrix to maintain correct aspect ratio
        app->update_projection_matrix(window);
    }
}