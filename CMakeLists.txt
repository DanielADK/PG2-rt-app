cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
project(PG2-SEMESTRAL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Zapnutí podpory vcpkg
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif()

# Find required packages
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenCV REQUIRED)


# Explicitly define source files instead of using wildcard patterns
set(PROJECT_SOURCES
        src/main.cpp
        src/App.cpp
        src/Callback.cpp
        src/Model.cpp
        src/OBJloader.cpp
        src/ShaderProgram.cpp
        src/Texture.cpp
        src/gl_error_callback.cpp
)

# Define header files separately if needed
set(PROJECT_HEADERS
        src/App.hpp
        src/gl_err_callback.hpp
        src/ShaderProgram.hpp
        src/Texture.hpp
        src/MazeGenerator.cpp
        src/MazeGenerator.hpp
        src/Collision.cpp
        src/Collision.hpp
        src/Logger.cpp
        src/Logger.hpp
        src/Camera.cpp
        src/Map.cpp
        src/Map.hpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Link libraries to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE
        GLEW::GLEW
        glfw
        glm::glm
        nlohmann_json::nlohmann_json
        imgui::imgui
        ${OpenCV_LIBS}

)
# Přidání include adresářů
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OpenCV_INCLUDE_DIRS}
)

# Kopírování potřebných souborů po buildu (pokud existují)
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
endif()