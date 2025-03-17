//
// Created by daniel on 17.3.25.
//

#include "Utils.hpp"

void Utils::printExtensions() {
    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    std::cout << "\n=== Supported OpenGL Extensions ===\n";
    for (GLint i = 0; i < numExtensions; i++) {
        const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
        std::cout << extension << std::endl;
    }
}

void Utils::printContextInfo() {
    // After initializing GLEW
    std::cout << "=== OpenGL Context Information ===\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

    // Verify OpenGL version
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major < 4 || (major == 4 && minor < 1)) {
        throw std::runtime_error("OpenGL 4.6 not supported");
    }

    // Verify profile
    int profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (!(profile & GL_CONTEXT_CORE_PROFILE_BIT)) {
        throw std::runtime_error("Core profile not active");
    }

    // Print GLFW version
    int glfwMajor, glfwMinor, glfwRevision;
    glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
    std::cout << "\n=== GLFW Information ===\n";
    std::cout << "GLFW Version: " << glfwMajor << "." << glfwMinor << "." << glfwRevision << '\n';

    // Print GLM version
    std::cout << "\n=== GLM Information ===\n";
    std::cout << "GLM Version: " << GLM_VERSION_MAJOR << "." << GLM_VERSION_MINOR << "." << GLM_VERSION_PATCH << '\n';

    // Print OpenGL extensions
    int numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    std::cout << "\n=== OpenGL Extensions ===\n";
    std::cout << "Number of supported extensions: " << numExtensions << '\n';

    bool debugSupported = false;
    for (int i = 0; i < numExtensions; i++) {
        const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (strcmp(ext, "GL_KHR_debug") == 0) {
            debugSupported = true;
            break;
        }
    }
    std::cout << "GL_KHR_debug: " << (debugSupported ? "Supported" : "Not supported") << std::endl;

    if (glewIsSupported("GL_ARB_debug_output")) {
        std::cout << "GL_ARB_debug_output is supported!" << std::endl;
    }
    else {
        std::cout << "GL_ARB_debug_output is NOT supported!" << std::endl;
    }

    printExtensions();
}
