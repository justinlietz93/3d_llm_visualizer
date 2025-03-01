#include "Camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>

namespace llmvis {

Camera::Camera(const glm::vec3& position)
    : m_position(position)
    , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_up(glm::vec3(0.0f, 1.0f, 0.0f))
    , m_right(glm::vec3(1.0f, 0.0f, 0.0f))
    , m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_movementSpeed(2.5f)
    , m_mouseSensitivity(0.1f)
    , m_zoom(45.0f)
{
    updateCameraVectors();
}

Camera::~Camera() {
}

void Camera::update(float deltaTime) {
    // Nothing to update in the camera per frame
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_zoom), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboard(int key, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    
    if (key == GLFW_KEY_W)
        m_position += m_front * velocity;
    else if (key == GLFW_KEY_S)
        m_position -= m_front * velocity;
    else if (key == GLFW_KEY_A)
        m_position -= m_right * velocity;
    else if (key == GLFW_KEY_D)
        m_position += m_right * velocity;
    else if (key == GLFW_KEY_Q)
        m_position += m_up * velocity;
    else if (key == GLFW_KEY_E)
        m_position -= m_up * velocity;
        
    // Update view matrix
    updateCameraVectors();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;
    
    m_yaw += xoffset;
    m_pitch += yoffset;
    
    // Constrain pitch to avoid flipping
    if (constrainPitch) {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }
    
    // Update front, right and up vectors
    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    m_zoom -= yOffset;
    m_zoom = std::clamp(m_zoom, 1.0f, 45.0f);
}

glm::vec3 Camera::getPosition() const {
    return m_position;
}

glm::vec3 Camera::getFront() const {
    return m_front;
}

glm::vec3 Camera::getRayDirection(float mouseX, float mouseY, int screenWidth, int screenHeight) const {
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    float z = 1.0f;
    
    // Homogeneous clip coordinates
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Eye coordinates
    float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
    glm::mat4 projection = glm::perspective(glm::radians(m_zoom), aspectRatio, 0.1f, 100.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    // World coordinates
    glm::mat4 view = getViewMatrix();
    glm::vec4 rayWorld = glm::inverse(view) * rayEye;
    
    // Now we have a ray in world space
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));
    
    return rayDirection;
}

void Camera::updateCameraVectors() {
    // Scale down movement speed for better control
    m_movementSpeed = 0.8f;
    m_mouseSensitivity = 0.05f;

    // Calculate new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    
    // Recalculate right and up vectors
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace llmvis 