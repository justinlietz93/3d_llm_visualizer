#include "Neuron.h"
#include <algorithm>
#include <iostream>

namespace llmvis {

Neuron::Neuron(int id, const glm::vec3& position)
    : m_id(id)
    , m_position(position)
    , m_activation(0.0f)
    , m_targetActivation(0.0f)
    , m_activationSpeed(5.0f)
{
}

Neuron::~Neuron() {
}

void Neuron::update(float deltaTime) {
    // Smoothly transition activation level towards target
    float diff = m_targetActivation - m_activation;
    if (std::abs(diff) > 0.001f) {
        m_activation += diff * m_activationSpeed * deltaTime;
        m_activation = std::clamp(m_activation, 0.0f, 1.0f);
    } else {
        m_activation = m_targetActivation;
    }
}

void Neuron::setActivation(float value) {
    m_targetActivation = std::clamp(value, 0.0f, 1.0f);
}

float Neuron::getActivation() const {
    return m_activation;
}

const glm::vec3& Neuron::getPosition() const {
    return m_position;
}

void Neuron::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Neuron::addConnection(int targetNeuronId, float weight) {
    // Check if connection already exists
    for (auto& conn : m_connections) {
        if (conn.targetId == targetNeuronId) {
            conn.weight = weight;
            return;
        }
    }
    
    // Add new connection
    Connection conn;
    conn.targetId = targetNeuronId;
    conn.weight = weight;
    m_connections.push_back(conn);
}

void Neuron::updateWeight(int targetNeuronId, float newWeight) {
    for (auto& conn : m_connections) {
        if (conn.targetId == targetNeuronId) {
            conn.weight = newWeight;
            return;
        }
    }
    
    std::cerr << "Connection to neuron " << targetNeuronId << " not found!" << std::endl;
}

} // namespace llmvis 