//
// Created by Daniel Adámek - daniel@nullptr.cz on 9.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp> 
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 m_position;
    glm::vec3 m_front{};
    glm::vec3 m_right{};
    glm::vec3 m_up{}; // camera local UP vector

    GLfloat m_yaw = -90.0f;
    GLfloat m_pitch =  0.0f;;
    GLfloat m_roll = 0.0f;
    
    // Camera options
    GLfloat m_movement_speed = 1.0f;
    GLfloat m_mouse_sensitivity = 0.25f;

    Camera(glm::vec3 position);

    glm::mat4 get_view_matrix() const;

    glm::vec3 handle_input(GLFWwindow* window, GLfloat deltaTime);

    void handle_mouse(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE);

private:
    void updateCameraVectors();
};

#endif //TEXTURE_HPP