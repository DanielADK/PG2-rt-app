#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "App.hpp"

App app;

int main(int argc, char** argv) {
    try {
        if (app.init())
            return app.run();
    }
    catch (std::exception const& e) {
        Logger::error("App failed : " + std::string(e.what()));
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}