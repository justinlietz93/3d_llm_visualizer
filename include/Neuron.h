#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace llmvis {

class Neuron {
public:
    Neuron(int id, const glm::vec3& position);
    ~Neuron();
    
    void update(float deltaTime);
    void setActivation(float value);
    float getActivation() const;
    
    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);
    
    void addConnection(int targetNeuronId, float weight);
    void updateWeight(int targetNeuronId, float newWeight);
    
private:
    int m_id;
    glm::vec3 m_position;
    float m_activation;
    float m_targetActivation;
    float m_activationSpeed;
    
    struct Connection {
        int targetId;
        float weight;
    };
    
    std::vector<Connection> m_connections;
};

} // namespace llmvis 