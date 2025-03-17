#include "App.hpp"

#include "Globals.hpp"

App::App(GLFWwindow* window) : window(window) {}

App::~App() {
    glDeleteProgram(shader_prog_ID);
    glDeleteBuffers(1, &VBO_ID);
    glDeleteVertexArrays(1, &VAO_ID);
}

bool App::init() {
    if (!GLEW_ARB_direct_state_access)
        throw std::runtime_error("No DSA :-(");

    init_assets();
    return true;
}

void App::init_assets() {
    // Initialize pipeline: compile, link and use shaders
    const char* vertex_shader =
        "#version 460 core\n"
        "in vec3 attribute_Position;"
        "void main() {"
        "  gl_Position = vec4(attribute_Position, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 460 core\n"
        "uniform vec4 uniform_Color;"
        "out vec4 FragColor;"
        "void main() {"
        "  FragColor = uniform_Color;"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);

    shader_prog_ID = glCreateProgram();
    glAttachShader(shader_prog_ID, fs);
    glAttachShader(shader_prog_ID, vs);
    glLinkProgram(shader_prog_ID);

    glDetachShader(shader_prog_ID, fs);
    glDetachShader(shader_prog_ID, vs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Create and load data into GPU using OpenGL DSA (Direct State Access)
    glCreateVertexArrays(1, &VAO_ID);

    GLint position_attrib_location = glGetAttribLocation(shader_prog_ID, "attribute_Position");

    glEnableVertexArrayAttrib(VAO_ID, position_attrib_location);
    glVertexArrayAttribFormat(VAO_ID, position_attrib_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, position));
    glVertexArrayAttribBinding(VAO_ID, position_attrib_location, 0);

    glCreateBuffers(1, &VBO_ID);
    glNamedBufferData(VBO_ID, triangle_vertices.size() * sizeof(vertex), triangle_vertices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO_ID, 0, VBO_ID, 0, sizeof(vertex));
}

void App::run() {
    GLfloat r, g, b, a;
    r = g = b = a = 1.0f;

    glUseProgram(shader_prog_ID);

    GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
    if (uniform_color_location == -1) {
        std::cerr << "Uniform location is not found in active shader program. Did you forget to activate it?\n";
    }

    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Dynamically change color based on time
        double time = glfwGetTime();
        r = static_cast<float>((sin(time) + 1) / 2);
        g = static_cast<float>((cos(time) + 1) / 2);
        b = static_cast<float>((sin(time * 0.5) + 1) / 2);

        glUniform4f(uniform_color_location, r, g, b, a);

        glBindVertexArray(VAO_ID);

        glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size());

        // Calculate FPS
        frameCount++;
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        if (deltaTime >= 1.0) {
            int fps = static_cast<int>(frameCount / deltaTime);
            std::string title = "OpenGL 4.6 Core Profile | FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}