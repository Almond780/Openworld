#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "RigidBody.hpp"
#include "Collider.hpp"

class PhysicsWorld {
public:
    // --- Public types ---
    struct OrientedCollider {
        glm::vec3 position;
        glm::vec3 halfExtents;
        glm::mat3 rotation;
        bool isRamp = false;
        glm::vec4 rampPlane;
    };
    struct MeshCollider {
        std::vector<glm::vec3> vertices;      // world-space positions
        std::vector<unsigned int> indices;    // triangle indices
        glm::mat4 transform;                  // model matrix (position + rotation + scale)
    };

    void addMeshCollider(const std::vector<glm::vec3>& localVertices,
                         const std::vector<unsigned int>& indices,
                         const glm::mat4& transform);

    // --- Public methods ---
    void setGravity(const glm::vec3& g) { gravity = g; }
    void addRigidBody(RigidBody* body);
    void addCollisionPlane(const glm::vec3& normal, float distance);
    void addStaticAABB(const AABB& aabb);
    void addOrientedCollider(const glm::vec3& pos, const glm::vec3& halfExt, const glm::vec3& eulerAngles);
    void step(float deltaTime);
    const std::vector<RigidBody*>& getBodies() const { return bodies; }
    void removeStaticAABB(const AABB& aabb);
    void addOrientedCollider(const glm::vec3& pos, const glm::vec3& halfExt, const glm::vec3& euler,
                             bool isRamp = false, const glm::vec4& rampPlane = glm::vec4(0,1,0,0));


    // --- Debug getters (now fully public) ---
    const std::vector<AABB>& getStaticAABBs() const { return staticAABBs; }
    const std::vector<OrientedCollider>& getOrientedColliders() const { return orientedColliders; }
    size_t getStaticAABBCount() const { return staticAABBs.size(); }
    size_t getOrientedCollidersCount() const { return orientedColliders.size(); }

private:
    // --- Private members ---
    std::vector<RigidBody*> bodies;
    std::vector<AABB> staticAABBs;
    std::vector<OrientedCollider> orientedColliders;
    glm::vec3 gravity;

    glm::vec3 planeNormal = glm::vec3(0,1,0);
    float planeDistance = 0;
    bool hasPlane = false;

    void resolveGroundCollision(RigidBody* body, float deltaTime);
    bool AABBvsAABB(const AABB& a, const AABB& b, glm::vec3& correction);
    void resolveBodyCollisions(float deltaTime);
    std::vector<MeshCollider> meshColliders;
};