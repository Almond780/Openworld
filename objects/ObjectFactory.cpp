
#include "core/LevelManager.hpp"
#include "Flattri.hpp"
#include "Flatquad.hpp"
#include "Ramp.hpp"
#include "Cube.hpp"
#include "ObjectFactory.hpp"
#include "core/AABBHelpers.hpp"   // <-- use shared helpers
#include <iostream>

void buildWorld(std::vector<GameObject>& objects,
                PhysicsWorld& physicsWorld,
                std::vector<std::pair<GameObject*, RigidBody*>>& dynamicObjects,
                Player& player) {   // removed glm::vec3& respawnPosition {


// ------------------------------------------------------------------
//  Cubes
// ------------------------------------------------------------------
struct CubeData {
    glm::vec3 position;
    glm::vec3 halfExtents;
    glm::vec3 rotation;
    bool hasCollision;
    bool isDynamic;
};

std::vector<CubeData> cubes = {
    //Dynamic
    { { 0.0f, 1.0f, 0.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true,  true },
    { {-2.0f, 4.0f, 2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true,  true },
    { { 2.0f, 4.0f,-2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true,  true },
    { { 2.0f, 4.0f, 2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true,  true },
    { {-2.0f, 4.0f,-2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true,  true },
    // static
    { {22.0f, 1.0f, 4.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true, false },
    { {20.0f, 2.0f, 2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true, false },
    { {24.0f, 2.0f, 2.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true, false },
    { {20.0f, 2.0f, 6.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true, false },
    { {24.0f, 2.0f, 6.0f}, {0.5f,0.5f,0.5f}, {0,0,0}, true, false },
    { {-22.0f, 1.0f, 30.0f}, {0.5f,0.5f,0.5f}, {0,0.01f,0}, true,  true },
    };

for (const auto& c : cubes) {
    GameObject obj;
    obj.position = c.position;
    obj.halfExtents = c.halfExtents;
    obj.rotation = c.rotation;
    obj.hasCollision = c.hasCollision;

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* objPtr = &objects.back();

    if (c.hasCollision && !c.isDynamic) {
        // Check if the cube has any rotation
        if (c.rotation != glm::vec3(0.0f)) {
            // Rotated cube → use oriented collider
            physicsWorld.addOrientedCollider(c.position, c.halfExtents, c.rotation, false);
        } else {
            // Axis-aligned cube → use fast static AABB
            AABB aabb;
            aabb.min = c.position - c.halfExtents;
            aabb.max = c.position + c.halfExtents;
            physicsWorld.addStaticAABB(aabb);
        }
    }

    // Dynamic cubes get a RigidBody
    if (c.isDynamic) {
        RigidBody* rb = new RigidBody(c.position, 10.0f, c.halfExtents);
        physicsWorld.addRigidBody(rb);
        dynamicObjects.push_back({objPtr, rb});
    }
}

// ------------------------------------------------------------------
// Walls
// ------------------------------------------------------------------

struct WallData {
    glm::vec3 position;
    glm::vec3 rotation;   // Euler angles in degrees
};

std::vector<WallData> walls = {

    { {23.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {23.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {23.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    //{ {19.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {19.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {19.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {15.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {15.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {15.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {11.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {11.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {11.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    //{ {7.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {7.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {7.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {3.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {3.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },
    { {3.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f} },


};

float wallW = 4.0f, wallH = 3.0f, wallD = 0.4f;
glm::vec3 wallHalfExtents(wallW * 0.5f, wallH * 0.5f, wallD * 0.5f);

for (const auto& w : walls) {
    GameObject wall;
    wall.position = w.position;
    wall.rotation = w.rotation;          // <-- set rotation
    wall.hasCollision = true;
    wall.halfExtents = wallHalfExtents;

    wall.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    wall.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    wall.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    objects.push_back(std::move(wall));
    GameObject* ptr = &objects.back();

    AABB aabb;
    aabb.min = ptr->position - ptr->halfExtents;
    aabb.max = ptr->position + ptr->halfExtents;
    physicsWorld.addStaticAABB(aabb);

    physicsWorld.addOrientedCollider(ptr->position, ptr->halfExtents, ptr->rotation, false);
}


// ------------------------------------------------------------------
//  Ramps
// ------------------------------------------------------------------
struct RampData {
    glm::vec3 position;
    glm::vec3 halfExtents;   // half-width, half-height, half-depth
    glm::vec3 rotation;
    bool hasCollision;
    bool isDynamic;          // usually false for ramps
};

std::vector<RampData> ramps = {
    //-1.0f for no jumping issue
    { { -10.0f, -1.0f,  15.0f }, { 2.0f, 1.0f, 3.0f }, {0,0,0}, true,  false },
    //w{ {   5.0f, 0.0f, -10.0f }, { 1.5f, 1.0f, 2.0f }, {0,0,0}, true,  false },
    //{ {  15.0f, 0.0f,  20.0f }, { 2.5f, 1.5f, 4.0f }, {0,0,0}, true,  false },
    // add as many as you want
};

for (const auto& r : ramps) {
    GameObject obj;
    obj.position = r.position;
    obj.halfExtents = r.halfExtents;
    obj.rotation = r.rotation;
    obj.hasCollision = r.hasCollision;

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Ramp::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Ramp::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Ramp::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* ptr = &objects.back();

    if (r.hasCollision && !r.isDynamic) {
        // --- Compute ramp plane (sloped surface) ---
        float w = r.halfExtents.x;          // half width
        float h = r.halfExtents.y * 2.0f;    // full height (halfExtents.y is half height)
        float d = r.halfExtents.z;          // half depth

        // Points on the sloped plane in local space (before rotation/translation)
        glm::vec3 p1(-w, 0.0f,  d);   // front bottom left
        glm::vec3 p2( w, 0.0f,  d);   // front bottom right
        glm::vec3 p3(-w,    h, -d);   // back top left

        // Build rotation matrix from Euler angles
        glm::mat4 rotMat = glm::mat4(1.0f);
        rotMat = glm::rotate(rotMat, glm::radians(r.rotation.x), glm::vec3(1,0,0));
        rotMat = glm::rotate(rotMat, glm::radians(r.rotation.y), glm::vec3(0,1,0));
        rotMat = glm::rotate(rotMat, glm::radians(r.rotation.z), glm::vec3(0,0,1));

        // Transform points to world space
        p1 = glm::vec3(rotMat * glm::vec4(p1, 1.0f)) + r.position;
        p2 = glm::vec3(rotMat * glm::vec4(p2, 1.0f)) + r.position;
        p3 = glm::vec3(rotMat * glm::vec4(p3, 1.0f)) + r.position;

        // Compute plane normal (cross product of two edges)
        glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
        float d_plane = -glm::dot(normal, p1);
        glm::vec4 rampPlane(normal, d_plane);

        // Register as oriented collider with ramp info
        physicsWorld.addOrientedCollider(r.position, r.halfExtents, r.rotation, true, rampPlane);
    }

    // If you ever want dynamic ramps (falling/moving), add here:
    // if (r.isDynamic) { ... }
}

// ------------------------------------------------------------------
//  Flat Quad (static, using Flatquad mesh)
// ------------------------------------------------------------------
struct FlatQuadData {
    glm::vec3 position;
    glm::vec3 visualHalfExtents;   // width/2, height/2, depth/2 for mesh scaling
    glm::vec3 colliderHalfExtents; // collider size (can be different)
    glm::vec3 rotation;
    bool hasCollision;
};

std::vector<FlatQuadData> flatQuads = {
    // Example: Visual is 2x0.05x2, but collider is 3x0.1x3 (bigger platform)
    { { -40.0f, 10.0f, -70.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {0,0,0}, true },
    { { -40.0f, 10.0f, -50.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {0,0,0}, true },
    { { -40.0f, 0.0f, -60.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {90.0f,0.0f,90.0f}, true },
    { { -40.0f, 20.0f, -60.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {90.0f,0.0f,90.0f}, true },
    { { -50.0f, 10.0f, -60.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {0,90.0f,0}, true },
    { { -30.0f, 10.0f, -60.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {0,90.0f,0}, true },
    };

for (const auto& q : flatQuads) {
    GameObject obj;
    obj.position = q.position;
    obj.halfExtents = q.visualHalfExtents;           // <-- visual scale
    obj.colliderHalfExtents = q.colliderHalfExtents; // <-- collider size
    obj.rotation = q.rotation;
    obj.hasCollision = q.hasCollision;

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flatquad::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flatquad::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flatquad::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* ptr = &objects.back();

    if (q.hasCollision) {
        // Use oriented collider with very thin depth to match the flat mesh
        physicsWorld.addOrientedCollider(q.position, q.colliderHalfExtents, q.rotation, false);
    }
}

// ------------------------------------------------------------------
//  Flat Triangle (static, using Flattri mesh)
// ------------------------------------------------------------------
struct FlatTriData {
    glm::vec3 position;
    glm::vec3 halfExtents;   // width/2, thickness/2, depth/2
    glm::vec3 colliderHalfExtents; // collider size
    glm::vec3 rotation;
    bool hasCollision;
};

std::vector<FlatTriData> flatTris = {
  // position                halfExtents (w/2, thin, d/2)   rotation   hasCollision
    //{ { 5.0f, 5.0f, -50.0f }, { 1.5f, 0.05f, 1.5f }, {0,0,0}, true },
    //{ { 5.0f, 5.0f, -50.0f }, { 10.0f, 10.1f, 10.0f }, { 0.0f, 0.1f, 0.0f }, {0,0,0}, true },
  };

for (const auto& t : flatTris) {
    GameObject obj;
    obj.position = t.position;
    obj.halfExtents = t.halfExtents;
    obj.colliderHalfExtents = t.colliderHalfExtents;
    obj.rotation = t.rotation;
    obj.hasCollision = t.hasCollision;

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* ptr = &objects.back();

    if (t.hasCollision) {
        physicsWorld.addOrientedCollider(t.position, t.halfExtents, t.rotation, false);
    }
}

// ===== Jump Pad Trigger (Player Only) =====
GameObject jumpPad;
jumpPad.position = glm::vec3(0.0f, 0.0f, -10.0f);
jumpPad.halfExtents = glm::vec3(2.0f, 0.2f, 2.0f);
jumpPad.isTrigger = true;
jumpPad.hasCollision = false;

// Visual mesh
jumpPad.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
jumpPad.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
jumpPad.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

// Player launch only
jumpPad.onTriggerEnter = [&physicsWorld, &player, &jumpPad](Player& p) {
    // Launch player
    p.getRigidBody()->velocity.y = 12.0f;
    //std::cout << "Boing! Player launched." << std::endl;

    // Define pad AABB
    AABB padAABB;
    padAABB.min = jumpPad.position - jumpPad.halfExtents;
    padAABB.max = jumpPad.position + jumpPad.halfExtents;

    // Check all dynamic bodies (cubes) for overlap
    for (RigidBody* rb : physicsWorld.getBodies()) {
        // Skip the player
        if (rb == p.getRigidBody()) continue;

        // Build AABB for the body
        AABB bodyAABB;
        bodyAABB.min = rb->position - rb->halfExtents;
        bodyAABB.max = rb->position + rb->halfExtents;

        if (checkAABBOverlap(padAABB, bodyAABB)) {
            // Apply upward impulse
            rb->velocity.y = 10.0f;  // slightly less than player for fun
            //std::cout << "Cube launched!" << std::endl;
        }
    }
};
objects.push_back(std::move(jumpPad));




// ===== Animated Door =====
GameObject door;
door.position = glm::vec3(7.0f, 1.5f, 25.0f);
door.halfExtents = glm::vec3(2.0f, 1.5f, 0.2f);
door.hasCollision = true;
door.isAnimated = true;
door.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
door.openAngle = 90.0f;
door.hingeOffset = glm::vec3(-2.0f, 0.0f, 0.0f);

// Visual mesh
door.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
door.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
door.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

objects.push_back(std::move(door));
GameObject* doorPtr = &objects.back();

// Store the door's AABB
AABB doorAABB;
doorAABB.min = doorPtr->position - doorPtr->halfExtents;
doorAABB.max = doorPtr->position + doorPtr->halfExtents;
physicsWorld.addStaticAABB(doorAABB);

// ===== Animated Door 2 =====
GameObject door2;
door2.position = glm::vec3(19.0f, 1.5f, 25.0f);
door2.halfExtents = glm::vec3(2.0f, 1.5f, 0.2f);
door2.hasCollision = true;
door2.isAnimated = true;
door2.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
door2.openAngle = 90.0f;
door2.hingeOffset = glm::vec3(-2.0f, 0.0f, 0.0f);

// Visual mesh
door2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
door2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
door2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

objects.push_back(std::move(door2));
GameObject* door2Ptr = &objects.back();

// Store the door's AABB
AABB door2AABB;
door2AABB.min = door2Ptr->position - door2Ptr->halfExtents;
door2AABB.max = door2Ptr->position + door2Ptr->halfExtents;
physicsWorld.addStaticAABB(door2AABB);

// ===== Checkpoint Trigger =====
GameObject checkpoint;
checkpoint.position = glm::vec3(0.0f, 0.0f, 15.0f); // Adjust position as needed
checkpoint.halfExtents = glm::vec3(1.0f, 0.3f, 1.0f);
checkpoint.isTrigger = true;
checkpoint.hasCollision = false;

// Assign a unique name for this checkpoint
std::string cpName = "CP1";
checkpoint.onTriggerEnter = [cpName](Player& p) {
    LevelManager::getInstance().unlockCheckpoint(cpName);
};

objects.push_back(std::move(checkpoint));
// ===== Moving Platform =====
GameObject platform;
platform.position = glm::vec3(-20.0f, 1.0f, 30.0f);
platform.halfExtents = glm::vec3(1.5f, 0.2f, 1.5f); // wide and thin
platform.hasCollision = true;
platform.isMoving = true;
platform.startPos = platform.position;
platform.endPos = platform.position + glm::vec3(0.0f, 0.0f, -50.0f); // move 8 units in X
platform.moveSpeed = 3.0f; // units per second

// Visual mesh (scaled cube)
platform.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
platform.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
platform.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

objects.push_back(std::move(platform));
GameObject* platformPtr = &objects.back();

// Create a kinematic RigidBody (mass = 0) so physics interacts with it
RigidBody* rb = new RigidBody(platformPtr->position, 0.0f, platformPtr->halfExtents);
physicsWorld.addRigidBody(rb);
dynamicObjects.push_back({platformPtr, rb});


// ===== Second Moving Platform =====
GameObject platform2;
platform2.position = glm::vec3(10.0f, -1.0f, 20.0f);
platform2.halfExtents = glm::vec3(1.5f, 0.2f, 1.5f);
platform2.hasCollision = true;
platform2.isMoving = true;
platform2.startPos = platform2.position;
platform2.endPos = platform2.position + glm::vec3(0.0f, 30.0f, 0.0f); // moves along Z
platform2.moveSpeed = 2.5f;

// Visual mesh
platform2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
platform2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
platform2.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

objects.push_back(std::move(platform2));
GameObject* platform2Ptr = &objects.back();

RigidBody* rb2 = new RigidBody(platform2Ptr->position, 0.0f, platform2Ptr->halfExtents);
physicsWorld.addRigidBody(rb2);
dynamicObjects.push_back({platform2Ptr, rb2});


}

