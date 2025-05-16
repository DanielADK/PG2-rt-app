#pragma once
#include <random>
#include <unordered_map>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <nlohmann/json.hpp>

#include "Camera.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "Collision.hpp"
#include "Logger.hpp"
#include "Map.hpp"


class App {
public:
    App();

    bool init();
    int run();
    ~App();

    static void print_gl_info();

    static void update_projection_matrix(GLFWwindow* window);
    void add_to_scene(const std::string& name, Model* model);

    bool is_jumping = false;
    float jump_velocity = 0.0f;


private:
    bool vsync_enabled = true;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Collision> m_Collision = nullptr;


    bool antialiasing_enabled = false;
    bool fullscreen = false;
    int win_width = 800;
    int win_height = 600;

    // window mode
    int windowed_x = 100, windowed_y = 100;
    int windowed_width = win_width, windowed_height = win_height;


    void load_config(); // config loader


    bool show_imgui = false;
    bool flashlight_on = false;
    bool free_cam = false; // fly mode

    GLFWwindow* window = nullptr;
    ShaderProgram shader;

    void init_assets();
    void init_imgui() const;

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    void toggle_vsync();
    void toggle_fullscreen();

protected:
    std::shared_ptr<Model> find_in_scene(const std::string& name);

    // projection
    int m_width{ 0 }, m_height{ 0 };
    float m_fov = 60.0f;
    glm::mat4 m_Projection_matrix = glm::identity<glm::mat4>();

    // scene
    std::unordered_map<std::string, std::shared_ptr<Model>> m_Scene;
    std::shared_ptr<Map> m_Map;

};
