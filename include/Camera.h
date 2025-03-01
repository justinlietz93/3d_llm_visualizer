#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace llmvis {

class Camera {
public:
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f));
    ~Camera();
    
    void update(float deltaTime);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    void processKeyboard(int key, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void processMouseScroll(float yOffset);
    
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    
    // For ray casting (picking)
    glm::vec3 getRayDirection(float mouseX, float mouseY, int screenWidth, int screenHeight) const;
    
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    
    float m_yaw;
    float m_pitch;
    
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
    
    void updateCameraVectors();
};

} // namespace llmvis 