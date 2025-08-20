#include "PhysicsComponent.h"

void PhysicsComponent::update(float deltaTime, float gravity, float drag, float limit)
{
    velocity.y += gravity * deltaTime;
    velocity.x *= drag;
    position += velocity * deltaTime;

    // Bouncing
    if (position.y - 0.5f < -1.0f) {
        position.y = -1.0f + 0.5f;
        velocity.y = -velocity.y * 0.8f;
    }
    if (position.x > limit - 0.5f) {
        position.x = limit - 0.5f;
        velocity.x = -velocity.x * 0.5f;
    }
    if (position.x < -limit + 0.5f) {
        position.x = -limit + 0.5f;
        velocity.x = -velocity.x * 0.5f;
    }
    if (position.z > limit - 0.5f) {
        position.z = limit - 0.5f;
        velocity.z = -velocity.z * 0.5f;
    }
    if (position.z < -limit + 0.5f) {
        position.z = -limit + 0.5f;
        velocity.z = -velocity.z * 0.5f;
    }
}
