#include "PhysicsWorld.hpp"
#include <algorithm>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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
    //std::cout << "Static AABB collision! Correction: "
              //<< correction.x << ", " << correction.y << ", " << correction.z << std::endl;

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
                bool aKinematic = (a->mass == 0.0f);
                bool bKinematic = (b->mass == 0.0f);

                if (aKinematic && bKinematic) {
                    // Both kinematic – ignore collision
                }
                else if (aKinematic) {
                    // Only move b (dynamic)
                    b->position -= correction;
                    if (correction.x != 0.0f) b->velocity.x = 0.0f;
                    if (correction.y != 0.0f) { b->velocity.y = 0.0f; if (correction.y > 0.0f) b->isGrounded = true; }
                    if (correction.z != 0.0f) b->velocity.z = 0.0f;
                }
                else if (bKinematic) {
                    // Only move a (dynamic)
                    a->position += correction;
                    if (correction.x != 0.0f) a->velocity.x = 0.0f;
                    if (correction.y != 0.0f) { a->velocity.y = 0.0f; if (correction.y > 0.0f) a->isGrounded = true; }
                    if (correction.z != 0.0f) a->velocity.z = 0.0f;
                }
                else {
                    // Both dynamic – use mass ratios
                    float totalMass = a->mass + b->mass;
                    float aFactor = b->mass / totalMass;
                    float bFactor = a->mass / totalMass;
                    a->position += correction * aFactor;
                    b->position -= correction * bFactor;

                    if (correction.x != 0.0f) { a->velocity.x = 0.0f; b->velocity.x = 0.0f; }
                    if (correction.y != 0.0f) {
                        a->velocity.y = 0.0f; b->velocity.y = 0.0f;
                        if (correction.y > 0.0f) a->isGrounded = true;
                        else b->isGrounded = true;
                    }
                    if (correction.z != 0.0f) { a->velocity.z = 0.0f; b->velocity.z = 0.0f; }
                }

                // Update AABBs after correction
                aabbA.min = a->position - a->halfExtents;
                aabbA.max = a->position + a->halfExtents;
                aabbB.min = b->position - b->halfExtents;
                aabbB.max = b->position + b->halfExtents;
            }
        }
    }

    // Resolve collisions with static AABBs (non‑rotated walls, doors, ramps)
    for (auto* body : bodies) {
        if (body->mass == 0.0f) continue;

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

    // Resolve collisions with oriented colliders (rotated walls, ramps)
    for (auto* body : bodies) {
        if (body->mass == 0.0f) continue;

        AABB bodyAABB;
        bodyAABB.min = body->position - body->halfExtents;
        bodyAABB.max = body->position + body->halfExtents;

        for (const auto& oc : orientedColliders) {
            // Transform player AABB to local space
            glm::mat3 invRot = glm::transpose(oc.rotation);
            glm::vec3 localBodyMin = invRot * (bodyAABB.min - oc.position);
            glm::vec3 localBodyMax = invRot * (bodyAABB.max - oc.position);
            AABB localBody;
            localBody.min = glm::min(localBodyMin, localBodyMax);
            localBody.max = glm::max(localBodyMin, localBodyMax);

            AABB localCollider;
            localCollider.min = -oc.halfExtents;
            localCollider.max =  oc.halfExtents;

            if (oc.isRamp) {
                // ---- RAMP SPECIFIC LOGIC ----
                bool withinX = (localBody.min.x < localCollider.max.x && localBody.max.x > localCollider.min.x);
                bool withinZ = (localBody.min.z < localCollider.max.z && localBody.max.z > localCollider.min.z);
                if (withinX && withinZ) {
                    // Project onto ramp plane
                    glm::vec3 playerPos = body->position;
                    glm::vec3 planeNormal = glm::vec3(oc.rampPlane);
                    float planeDist = oc.rampPlane.w;
                    float t = -(glm::dot(planeNormal, playerPos) + planeDist) / glm::dot(planeNormal, planeNormal);
                    glm::vec3 projectedPos = playerPos + t * planeNormal;

                    float desiredBottomY = projectedPos.y;
                    float currentBottomY = body->position.y - body->halfExtents.y;

                    if (currentBottomY < desiredBottomY) {
                        body->position.y = desiredBottomY + body->halfExtents.y;
                        body->velocity.y = 0.0f;
                        body->isGrounded = true;
                    }

                    // Horizontal collision
                    glm::vec3 localCorrection;
                    if (AABBvsAABB(localBody, localCollider, localCorrection)) {
                        localCorrection.y = 0.0f;
                        glm::vec3 worldCorrection = oc.rotation * localCorrection;
                        body->position += worldCorrection;
                        if (worldCorrection.x != 0.0f) body->velocity.x = 0.0f;
                        if (worldCorrection.z != 0.0f) body->velocity.z = 0.0f;
                    }
                }
            } else {
                // Normal oriented collider
                glm::vec3 localCorrection;
                if (AABBvsAABB(localBody, localCollider, localCorrection)) {
                    glm::vec3 worldCorrection = oc.rotation * localCorrection;
                    body->position += worldCorrection;
                    if (worldCorrection.x != 0.0f) body->velocity.x = 0.0f;
                    if (worldCorrection.y != 0.0f) {
                        body->velocity.y = 0.0f;
                        if (worldCorrection.y > 0.0f) body->isGrounded = true;
                    }
                    if (worldCorrection.z != 0.0f) body->velocity.z = 0.0f;
                }
            }

            // Update body AABB after correction
            bodyAABB.min = body->position - body->halfExtents;
            bodyAABB.max = body->position + body->halfExtents;
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

void PhysicsWorld::addOrientedCollider(const glm::vec3& pos, const glm::vec3& halfExt, const glm::vec3& euler,
                                       bool isRamp, const glm::vec4& rampPlane) {
    OrientedCollider oc;
    oc.position = pos;
    oc.halfExtents = halfExt;
    oc.isRamp = isRamp;
    oc.rampPlane = rampPlane;

    glm::mat4 rotMat(1.0f);
    rotMat = glm::rotate(rotMat, glm::radians(euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, glm::radians(euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, glm::radians(euler.z), glm::vec3(0.0f, 0.0f, 1.0f));
    oc.rotation = glm::mat3(rotMat);

    orientedColliders.push_back(oc);
}

// Moller-Trumbore ray/triangle, adapted for AABB swept test? We'll do discrete overlap.
bool TriangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                            const AABB& aabb, glm::vec3& normal, float& penetration) {
    // Use separating axis theorem (SAT) for triangle vs AABB.
    // For simplicity, we'll implement a basic test that returns the MTV (minimum translation vector).
    // Full SAT implementation is lengthy; a simpler approach is to check if any of the triangle's vertices are inside the AABB,
    // or if any AABB edge penetrates the triangle. For now, we'll use a library function or a simplified version.

    // Placeholder: use a simple triangle-AABB overlap test from online (e.g., Real-Time Collision Detection).
    // For now, we'll rely on a quick test that returns true if overlapping.
    // (Full implementation omitted for brevity but can be provided.)
    return false;
}

void PhysicsWorld::clear() {
    bodies.clear();
    staticAABBs.clear();
    orientedColliders.clear();
    meshColliders.clear();
    hasPlane = false;
    planeNormal = glm::vec3(0,1,0);
    planeDistance = 0.0f;
    // gravity is intentionally left alone; it's an engine setting, not level data.
}