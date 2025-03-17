cmake_minimum_required(VERSION 3.30)
project(all_cviceni)

set(CMAKE_CXX_STANDARD 23)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenCV REQUIRED)

# Specify the executable source
add_executable(all_cviceni
        src/Globals.cpp
        src/Globals.hpp
        src/main.cpp
        src/Utils.cpp
        src/Utils.hpp
        src/App.cpp
        src/App.hpp
        src/Callbacks.cpp
        src/Callbacks.hpp
)

# Link libraries to your executable
target_link_libraries(all_cviceni PRIVATE
        OpenGL::GL
        GLEW::GLEW
        glfw
        glm::glm
        ${OpenCV_LIBS}
)