
#include "Camera.hpp"

Camera::Camera(glm::vec3 position):
        m_position(position) {
    this->m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    // initialization of the camera reference system
    this->updateCameraVectors();
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(this->m_position, this->m_position + this->m_front, this->m_up);
}

glm::vec3 Camera::handle_input(GLFWwindow* window, GLfloat deltaTime){
    glm::vec3 direction{0};

    if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        direction += m_front;

    if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
        direction += -m_front;

    if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
        direction += -m_right;       // add unit vector to final direction

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += m_right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction += m_up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        direction += -m_up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // sprint
        m_movement_speed = 2.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        m_movement_speed = 1.0f;

    if (glm::length(direction) < 0.00001)
        return glm::vec3(0.0f);

    return glm::normalize(direction) * m_movement_speed * deltaTime;
}

void Camera::handle_mouse(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch) {
    xoffset *= this->m_mouse_sensitivity;
    yoffset *= this->m_mouse_sensitivity;

    this->m_yaw   += xoffset;
    this->m_pitch += yoffset;

    if (constraintPitch)
    {
        if (this->m_pitch > 89.0f)
            this->m_pitch = 89.0f;
        if (this->m_pitch < -89.0f)
            this->m_pitch = -89.0f;
    }

    this->updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    front.y = sin(glm::radians(this->m_pitch));
    front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));

    this->m_front = glm::normalize(front);
    this->m_right = glm::normalize(glm::cross(this->m_front, glm::vec3(0.0f,1.0f,0.0f)));
    this->m_up    = glm::normalize(glm::cross(this->m_right, this->m_front));
}