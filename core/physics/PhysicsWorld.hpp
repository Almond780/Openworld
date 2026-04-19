#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "RigidBody.hpp"
#include "Collider.hpp"

class PhysicsWorld {
public:
    void setGravity(const glm::vec3& g) { gravity = g; }
    void addRigidBody(RigidBody* body);
    void addCollisionPlane(const glm::vec3& normal, float distance);
    void addStaticAABB(const AABB& aabb);
    void step(float deltaTime);
    const std::vector<RigidBody*>& getBodies() const { return bodies; }
    void removeStaticAABB(const AABB& aabb);

private:
    std::vector<RigidBody*> bodies;
    std::vector<AABB> staticAABBs;
    glm::vec3 gravity;

    // Ground plane
    glm::vec3 planeNormal = glm::vec3(0,1,0);
    float planeDistance = 0;
    bool hasPlane = false;

    void resolveGroundCollision(RigidBody* body, float deltaTime);
    bool AABBvsAABB(const AABB& a, const AABB& b, glm::vec3& correction);
    void resolveBodyCollisions(float deltaTime);
};