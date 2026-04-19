#include "PhysicsWorld.hpp"
#include <algorithm>
#include <iostream>

static AABB getBodyAABB(const RigidBody* body) {
    AABB aabb;
    aabb.min = body->position - body->halfExtents;
    aabb.max = body->position + body->halfExtents;
    return aabb;
}

void PhysicsWorld::addRigidBody(RigidBody* body) {
    bodies.push_back(body);
}

void PhysicsWorld::addCollisionPlane(const glm::vec3& normal, float distance) {
    planeNormal = glm::normalize(normal);
    planeDistance = distance;
    hasPlane = true;
}

void PhysicsWorld::addStaticAABB(const AABB& aabb) {
    staticAABBs.push_back(aabb);
}

bool PhysicsWorld::AABBvsAABB(const AABB& a, const AABB& b, glm::vec3& correction) {
    // Calculate overlap on each axis
    float dx = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float dy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float dz = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    // If overlapping on all axes
    if (dx > 0.0f && dy > 0.0f && dz > 0.0f) {
        // Find axis of minimum penetration (to push out along the shallowest overlap)
        if (dx < dy && dx < dz) {
            correction = glm::vec3((a.max.x > b.max.x) ? dx : -dx, 0.0f, 0.0f);
        } else if (dy < dx && dy < dz) {
            correction = glm::vec3(0.0f, (a.max.y > b.max.y) ? dy : -dy, 0.0f);
        } else {
            correction = glm::vec3(0.0f, 0.0f, (a.max.z > b.max.z) ? dz : -dz);
        }
        return true;
    }
    correction = glm::vec3(0.0f);
    return false;
}

void PhysicsWorld::step(float deltaTime) {
    // 1. Apply gravity and integrate positions
    for (auto* body : bodies) {
        body->velocity += gravity * deltaTime;
        body->position += body->velocity * deltaTime;
        body->isGrounded = false;
    }

    // 2. Resolve collisions between dynamic bodies (player + cubes)
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            RigidBody* a = bodies[i];
            RigidBody* b = bodies[j];

            AABB aabbA, aabbB;
            aabbA.min = a->position - a->halfExtents;
            aabbA.max = a->position + a->halfExtents;
            aabbB.min = b->position - b->halfExtents;
            aabbB.max = b->position + b->halfExtents;

            glm::vec3 correction;
            if (AABBvsAABB(aabbA, aabbB, correction)) {
                // Push apart based on mass
                float totalMass = a->mass + b->mass;
                float aFactor = b->mass / totalMass;
                float bFactor = a->mass / totalMass;
                a->position += correction * aFactor;
                b->position -= correction * bFactor;

                // Zero velocity along correction axis (simplified)
                if (correction.x != 0.0f) { a->velocity.x = 0.0f; b->velocity.x = 0.0f; }
                if (correction.y != 0.0f) {
                    a->velocity.y = 0.0f;
                    b->velocity.y = 0.0f;
                    if (correction.y > 0.0f) a->isGrounded = true;
                    else b->isGrounded = true;
                }
                if (correction.z != 0.0f) { a->velocity.z = 0.0f; b->velocity.z = 0.0f; }

                // Update AABBs after correction for subsequent checks
                aabbA.min = a->position - a->halfExtents;
                aabbA.max = a->position + a->halfExtents;
                aabbB.min = b->position - b->halfExtents;
                aabbB.max = b->position + b->halfExtents;
            }
        }
    }

    // 3. Resolve collisions between dynamic bodies and static AABBs (walls, etc.)
    for (auto* body : bodies) {
        AABB bodyAABB;
        bodyAABB.min = body->position - body->halfExtents;
        bodyAABB.max = body->position + body->halfExtents;

        for (const auto& staticAABB : staticAABBs) {
            glm::vec3 correction;
            if (AABBvsAABB(bodyAABB, staticAABB, correction)) {
                body->position += correction;
                if (correction.x != 0.0f) body->velocity.x = 0.0f;
                if (correction.y != 0.0f) {
                    body->velocity.y = 0.0f;
                    if (correction.y > 0.0f) body->isGrounded = true;
                }
                if (correction.z != 0.0f) body->velocity.z = 0.0f;

                bodyAABB.min = body->position - body->halfExtents;
                bodyAABB.max = body->position + body->halfExtents;
            }
        }
    }

    // 4. Ground plane collision (for bodies that need it)
    if (hasPlane) {
        for (auto* body : bodies) {
            resolveGroundCollision(body, deltaTime);
        }
    }
}
void PhysicsWorld::resolveGroundCollision(RigidBody* body, float deltaTime) {
    float playerHeight = 2.0f;
    float bottomY = body->position.y - playerHeight / 2.0f;

    if (bottomY < planeDistance) {
        body->position.y = planeDistance + playerHeight / 2.0f;
        body->velocity.y = 0.0f;
        body->isGrounded = true;
    }
}

void PhysicsWorld::removeStaticAABB(const AABB& aabb) {
    auto it = std::find_if(staticAABBs.begin(), staticAABBs.end(),
                           [&aabb](const AABB& other) {
                               return other.min == aabb.min && other.max == aabb.max;
                           });
    if (it != staticAABBs.end()) {
        staticAABBs.erase(it);
    }
}