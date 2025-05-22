#include <iostream>
#include <opencv2/opencv.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <fstream>
#include <thread>

#include "App.hpp"
#include "Collision.hpp"
#include "gl_err_callback.hpp"
#include "Logger.hpp"
#include "MazeGenerator.hpp"

const size_t maze_width = 32;
const size_t maze_depth = 32;

App::App():
        m_Camera(std::make_unique<Camera>(glm::vec3(0, 0, 2))),
        m_Map(std::make_shared<Map>(maze_width+1, maze_depth+1)) {
    m_Collision = std::make_unique<Collision>(m_Map,  0.25f);
}

bool App::init() {
    load_config();
    try {
        glfwSetErrorCallback(error_callback);
      
        if (!glfwInit()) {
            return false;
        }

        // Set OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, antialiasing_enabled ? 4 : 0);
        glfwWindowHint(GLFW_DEPTH_BITS, 24);  // depth buffer

        // open window (GL canvas) with no special properties
        window = glfwCreateWindow(win_width, win_height, "OpenGL context", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, key_callback);

        // disable cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        print_gl_info();

        // vsync
        glfwSwapInterval(vsync_enabled ? 1 : 0);

        GLenum err = glewInit();
        if (GLEW_OK != err) {
            Logger::error("Error: " + std::string(reinterpret_cast<const char *>(glewGetErrorString(err))));
        }
        glewInit();

        if (antialiasing_enabled)
            glEnable(GL_MULTISAMPLE);  // antialiasing

        glEnable(GL_DEPTH_TEST);        // draw depth - Z buffer
        glDepthFunc(GL_LESS);

        glDebugMessageCallback(MessageCallback, 0);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_CULL_FACE);

        if (!GLEW_ARB_direct_state_access)
            throw std::runtime_error("No DSA :-(");


        glfwSetFramebufferSizeCallback(window, fbsize_callback);    // On GL framebuffer resize callback.
        glfwSetScrollCallback(window, scroll_callback);             // On mouse wheel.
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        
        MazeGenerator maze_generator(maze_depth, maze_width);
        glm::vec2 start = glm::vec2(1, 1);
        glm::vec2 end = glm::vec2(maze_depth - 3, maze_width - 3);
        maze_generator.generate(this->m_Map, start, end);

        m_Camera->m_position = glm::vec3(-10, 1.0f, -10);

        //init_assets("resources"); // transparent and non-transparent models

        init_assets();

        //transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);

        init_imgui();

    }
    catch (std::exception const& e) {
        Logger::error("Init failed : " +  std::string(e.what()));
        throw;
    }
    std::cout << "Initialized...\n";

    return true;
}

void App::init_imgui() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void App::init_assets() {
    shader = ShaderProgram("shaders/basic.vert", "shaders/better.frag");

    // sizes
    constexpr float wall_height = 2.0f;

    // floor
    Model floor = Model("assets/objects/cube_triangles_vnt.obj", shader, "assets/textures/ground.png");
    floor.transparent = false;
    floor.m_origin = glm::vec3(0.0f, 0.0f, 0.0f);
    floor.scale = glm::vec3(64.0f, 0.1f, 64.0f);
    this->add_to_scene("floor", &floor);

    // moving teapots with lights
    Model teapot_model = Model("assets/objects/teapot.obj", shader, "assets/textures/teapot.png");
    float scale = .15f;
    Model tp1(teapot_model);
    tp1.m_origin = glm::vec3(30.0f, 1.0f, 30.0f);
    tp1.scale = glm::vec3(scale);
    this->add_to_scene("tp1", &tp1);
    Model tp2(teapot_model);
    tp2.m_origin = glm::vec3(2.0f, 1.0f, 2.0f);
    tp2.scale = glm::vec3(scale);
    this->add_to_scene("tp2", &tp2);

    // sun
    Model sun = Model("assets/objects/cube_triangles_vnt.obj", shader, "assets/textures/yellow.jpg");
    sun.transparent = false;
    sun.m_origin = glm::vec3(-4.0f, 6.0f, -4.0f);
    sun.scale = glm::vec3(2.0f);
    this->add_to_scene("sun", &sun);


    // walls
    Model wall_template = Model("assets/objects/cube_triangles_vnt.obj", shader, "assets/textures/wall.png");
    Model box_template = Model("assets/objects/cube_triangles_vnt.obj", shader, "assets/textures/red.jpg");
    box_template.transparent = true;

    for (int y = 0; y < m_Map->height(); ++y) {
        for (int x = 0; x < m_Map->width(); ++x) {

            uint8_t cell = m_Map->at(x, y);

            glm::vec3 center{ x + 0.5f, 0.0f, y + 0.5f }; // y+0.5 = center of block

            if (cell == CELL_WALL) {
                std::cout << "█";

                Model wall = wall_template;
                wall.m_origin = center + glm::vec3(0.0f, wall_height - 1.5f, 0.0f);
                wall.scale    = glm::vec3(1.0f, wall_height, 1.0f);

                add_to_scene("wall-" + std::to_string(x) + "-" + std::to_string(y),
                             &wall);
            } else if (cell == CELL_START || cell == CELL_END) {
                std::cout << "X";

                Model box = box_template;
                box.m_origin = center + glm::vec3(0.0f, wall_height - 1.5f, 0.0f);
                box.scale    = glm::vec3(1.0f, 5.0f, 1.0f);

                add_to_scene("tp-" + std::to_string(x) + "-" + std::to_string(y),
                             &box);
            } else {
                std::cout << "·";
            }
        }
        std::cout << '\n';
    }
}


int App::run() {
    try {
        shader.activate();

        glfwGetFramebufferSize(window, &m_width, &m_height);
        update_projection_matrix(window);
        shader.setUniform("uP_m", m_Projection_matrix);

        // fps
        double fps_timer = 0.0;
        int fps_counter_frames = 0;
        int fps_display = 0;

        float lastFrameTime = static_cast<float>(glfwGetTime());
        float speed = 5.0f;

        while (!glfwWindowShouldClose(window)) {
            float current_frame_time = static_cast<float>(glfwGetTime());
            float delta_time = current_frame_time - lastFrameTime;
            lastFrameTime = current_frame_time;

            fps_timer += delta_time;
            fps_counter_frames++;

            if (fps_timer >= 1.0) {
                fps_display = fps_counter_frames;
                fps_counter_frames = 0;
                fps_timer = 0.0;
            }

            shader.setUniform("uP_m", m_Projection_matrix);

            std::vector<std::shared_ptr<Model>> transparent;
            transparent.reserve(m_Scene.size());

            // movement
            glm::vec3 movement = m_Camera->handle_input(window, delta_time * speed);
            // handle XZ movement
            if (!free_cam)
                m_Camera->m_position = m_Collision->movement(m_Camera->m_position, glm::vec3(movement.x, 0.0f, movement.z));
            else m_Camera->m_position += movement;

            // handle Y movement (jump)
            if (!free_cam) {
                m_Camera->m_position.y += movement.y;

                if (is_jumping) {
                    jump_velocity -= 9.81f * delta_time;
                    m_Camera->m_position.y += jump_velocity * delta_time;

                    if (m_Camera->m_position.y <= 1.0f) {
                        m_Camera->m_position.y = 1.0f;
                        is_jumping      = false;
                        jump_velocity   = 0.0f;
                    }
                } else {
                    m_Camera->m_position.y = 1.0f;
                }
            }


            //teapots
            constexpr int MAX_TEAPOTS = 2;
            int teapot_count = 0;

            for (int i = 1; i <= MAX_TEAPOTS; ++i) {
                std::string object_name = "tp" + std::to_string(i);
                if (m_Scene.contains(object_name)) {
                    auto teapot = find_in_scene(object_name);

                    if (teapot && teapot_count < MAX_TEAPOTS) {
                        // animation
                        float amplitude = 0.5f;
                        float anim_speed = 1.5f;
                        float time_offset = i * 0.5f; // offset each teapot's animation
                        float offset = sin((current_frame_time + time_offset) * anim_speed) * amplitude;

                        // Calculate color based on animation
                        float colorIntensity = (sin(current_frame_time * 2.0f + i * 0.5f) + 1.0f) * 0.5f;
                        glm::vec3 baseColor = glm::vec3(0.2f, 0.8f, 0.4f); // Green base color
                        glm::vec3 animatedColor = baseColor * colorIntensity;

                        // Move the teapot
                        glm::vec3 moved_origin = teapot->m_origin;
                        moved_origin.y += offset;

                        // update model matrix
                        teapot->local_model_matrix = glm::translate(glm::mat4(1.0f), moved_origin);
                        teapot->local_model_matrix = glm::scale(teapot->local_model_matrix, teapot->scale);

                        // get position from model matrix
                        glm::vec3 position = glm::vec3(teapot->local_model_matrix[3]);

                        std::string idx = std::to_string(teapot_count);

                        // point light properties
                        shader.setUniform("teapotLight[" + idx + "].position", position);
                        shader.setUniform("teapotLight[" + idx + "].diffuse", animatedColor);
                        shader.setUniform("teapotLight[" + idx + "].specular", glm::vec3(1.0f));
                        shader.setUniform("teapotLight[" + idx + "].constant", 1.0f);
                        shader.setUniform("teapotLight[" + idx + "].linear", 0.09f);
                        shader.setUniform("teapotLight[" + idx + "].exponent", 0.032f);

                        // emissive properties
                        shader.setUniform("teapotEmissive[" + idx + "].color", animatedColor);
                        shader.setUniform("teapotEmissive[" + idx + "].position", position);
                        shader.setUniform("teapotEmissive[" + idx + "].radius", 2.0f);

                        teapot_count++;
                    }
                }
            }

            // set actual number of teapot lights
            shader.setUniform("teapotCount", teapot_count);
            shader.setUniform("pointLightOn", 1);

            // sun cycle
            float angle = (current_frame_time / 30.0f) * glm::two_pi<float>();
            float radius = 60.0f;
            glm::vec3 sun_pos = {
                radius * cos(angle),
                20.0f * sin(angle),
                0.f,
            };
            find_in_scene("sun")->m_origin = sun_pos;

            float brightness = glm::clamp((sun_pos.y + 5.0f) / 10.0f, 0.15f, 1.0f);

            glClearColor(0.85f * brightness, 0.9f * brightness, 1.0f * brightness, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            shader.setUniform("ambient", glm::vec3(0.03f, 0.03f, 0.03f));

            // Spotlight
            shader.setUniform("uV_m", m_Camera->get_view_matrix());

            shader.setUniform("viewPos", m_Camera->m_position);

            shader.setUniform("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            shader.setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.setUniform("spotLight.position", m_Camera->m_position);
            shader.setUniform("spotLight.direction", m_Camera->m_front);
            shader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.0f)));
            shader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(20.0f)));
            shader.setUniform("spotLight.constant", 1.0f);
            shader.setUniform("spotLight.linear", 0.07f);
            shader.setUniform("spotLight.exponent", 0.0017f);

            shader.setUniform("SpotlightLightOn", flashlight_on? 1:0);


            // directional light
            shader.setUniform("directionalLightOn", 1);

            glm::vec3 sun_position = find_in_scene("sun")->m_origin;
            glm::vec3 sun_target = glm::vec3(0.0f);

            glm::vec3 sun_direction = glm::normalize(sun_target - sun_position);
            shader.setUniform("directionLight.direction", sun_direction);

            float sun_intensity = glm::clamp(sun_position.y, 0.0f, 1.0f);
            if (sun_intensity > 0.0f) {
                glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.6f);
                glm::vec3 specular = glm::vec3(0.5f);

                shader.setUniform("directionLight.diffuse", diffuse * sun_intensity);
                shader.setUniform("directionLight.specular", specular * sun_intensity);
                shader.setUniform("directionalLightOn", 1);
            }
            else {
                shader.setUniform("directionalLightOn", 0);  // turn off
            }
            // sun emission
            shader.setUniform("sunEmissive.position", sun_position);
            shader.setUniform("sunEmissive.color", glm::vec3(1.0f, 1.0f, 0.5f));
            shader.setUniform("sunEmissive.radius", 10.0f);  // adjust for spread

            // not transparent objects
            for (auto& [name, model] : m_Scene) {
                if (!model->transparent) {
                    shader.setUniform("tex_scale", (name == "world_floor") ? 20.0f : 1.0f);
                    model->draw();
                }
                else {
                    transparent.emplace_back(model);
                }
            }

            // transparent objects
            std::ranges::sort(transparent, [&](const std::shared_ptr<Model> &a, const std::shared_ptr<Model> &b) {
                auto ta = glm::vec3(a->local_model_matrix[3]);
                auto tb = glm::vec3(b->local_model_matrix[3]);
                return glm::distance(m_Camera->m_position, ta) < glm::distance(m_Camera->m_position, tb);
                });

            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            for (auto p : transparent) p->draw();
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            // IMGUI
            if (show_imgui) {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                ImGui::SetNextWindowPos(ImVec2(10, 10));
				ImGui::SetNextWindowSize(ImVec2(350, 150));
                ImGui::SetNextWindowBgAlpha(0.4f);
                ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("FPS:              %d", fps_display);
                ImGui::Text("VSync:            %s", vsync_enabled ? "ON" : "OFF");
                ImGui::Text("Camera Position: (X:%.2f, Y:%.2f, Z:%.2f)", m_Camera->m_position.x, m_Camera->m_position.y, m_Camera->m_position.z);
                ImGui::Text("FreeCam:          %s", free_cam ? "ON" : "OFF");
                ImGui::Text("Flashlight:       %s", flashlight_on ? "ON" : "OFF");
                ImGui::Text("Antialiasing:     %s", antialiasing_enabled ? "ON" : "OFF");
                ImGui::Text("Multisample:      %s", glIsEnabled(GL_MULTISAMPLE) ? "YES" : "NO");
                ImGui::Text("FOV:              %.1f", m_fov);
                ImGui::End();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }


            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (std::exception const& e) {
        Logger::error("App failed : " + std::string(e.what()));
        return EXIT_FAILURE;
    }

    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}


void App::update_projection_matrix(GLFWwindow* window) {
    auto instance = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (instance->m_height < 1)
        instance->m_height = 1;   // avoid division by 0

    float ratio = static_cast<float>(instance->m_width) / instance->m_height;

    instance->m_Projection_matrix = glm::perspective(
        glm::radians(instance->m_fov),   // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        100.0f             // 20000.0f Far clipping plane. Keep as little as possible.
    );
}

void App::add_to_scene(const std::string& name, Model* model) {
    if (model == nullptr) {
        Logger::error("Attempting to add a null model to the scene.");
        return;
    }

    m_Scene[name] = std::make_shared<Model>(*model);
}


App::~App() {
    shader.clear();
    if (window)
        glfwDestroyWindow(window);

    // destroy ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    cv::destroyAllWindows();
}

void App::print_gl_info() {
    Logger::info("\n============= :: GL Info :: =============\n");
    Logger::info("GL Vendor:\t" + std::string(reinterpret_cast<const char *>(glGetString(GL_VENDOR))));
    Logger::info("GL Renderer:\t" + std::string(reinterpret_cast<const char *>(glGetString(GL_RENDERER))));
    Logger::info("GL Version:\t" + std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION))));
    Logger::info(
        "GL Shading ver:\t" + std::string(reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION))) +
        "\n");

    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (const auto errorCode = glGetError()) {
        Logger::info("[!] Pending GL error while obtaining profile: " + std::to_string(errorCode));
        //return;
    }
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        Logger::info("Core profile");
    } else {
        Logger::info("Compatibility profile");
    }
    Logger::info("=========================================\n\n");
}

void App::toggle_vsync() {
    vsync_enabled = !vsync_enabled;
    glfwMakeContextCurrent(window); // ensure correct context
    glfwSwapInterval(vsync_enabled ? 1 : 0);
    Logger::info("VSync: " + std::string(vsync_enabled ? "ON" : "OFF"));
}

void App::load_config() {
    std::ifstream f("config.json");
    if (!f.is_open()) {
        Logger::error("Cannot open config.json. Using defaults.");
        return;
    }

    nlohmann::json config;
    try {
        f >> config;
        antialiasing_enabled = config.value("antialiasing", false);
        vsync_enabled = config.value("vsync", false);
        fullscreen = config.value("fullscreen", false);
        free_cam = config.value("free_cam", false);
        flashlight_on = config.value("flashlight", false);
        win_width = config.value("window_width", 800);
        win_height = config.value("window_height", 600);
    }
    catch (...) {
        Logger::error("Invalid config.json");
    }
}

void App::toggle_fullscreen() {
    if (!fullscreen) {
        // switch to fullscreen – save window position and size
        glfwGetWindowPos(window, &windowed_x, &windowed_y);
        glfwGetWindowSize(window, &windowed_width, &windowed_height);
    }

    fullscreen = !fullscreen;

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int new_width = fullscreen ? mode->width : windowed_width;
    int new_height = fullscreen ? mode->height : windowed_height;
    int xpos = fullscreen ? 0 : windowed_x;
    int ypos = fullscreen ? 0 : windowed_y;

    glfwSetWindowMonitor(window, monitor, xpos, ypos, new_width, new_height, GLFW_DONT_CARE);

    win_width = new_width;
    win_height = new_height;

    glViewport(0, 0, win_width, win_height);
    update_projection_matrix(window);

    Logger::error("WINDOW: " + std::string(fullscreen ? "FULLSCREEN" : "WINDOWED"));
}

std::shared_ptr<Model> App::find_in_scene(const std::string& name)  {
    auto it = m_Scene.find(name);
    if (it != m_Scene.end()) {
        return it->second;
    }
    Logger::error("Model not found: " + name);
    return {};
};
