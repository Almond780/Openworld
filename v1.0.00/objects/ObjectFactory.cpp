
#include "wall.hpp"
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
                Player& player) {

// ------------------------------------------------------------------
//  Cubes
// ------------------------------------------------------------------
struct CubeData {
    glm::vec3 position;
    glm::vec3 halfExtents;
    glm::vec3 rotation;
    bool hasCollision;
    bool isDynamic;
    glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.0f);    // orange
    bool useOutline = true;
};

std::vector<CubeData> cubes = {
        // Dynamic
        { { 0.0f, 1.0f, 0.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  true,  glm::vec3(1.0f, 0.5f, 0.0f), true },
        { {-2.0f, 4.0f, 2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  true,  glm::vec3(1.0f, 0.5f, 0.0f), true },
        { { 2.0f, 4.0f,-2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  true,  glm::vec3(1.0f, 0.5f, 0.0f), true },
        { { 2.0f, 4.0f, 2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  true,  glm::vec3(1.0f, 0.5f, 0.0f), true },
        { {-2.0f, 4.0f,-2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  true,  glm::vec3(1.0f, 0.5f, 0.0f), true },
        // Static
        { {22.0f, 1.0f, 4.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  false, glm::vec3(1.0f, 0.0f, 0.0f), true },
        { {20.0f, 2.0f, 2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  false, glm::vec3(1.0f, 0.0f, 0.0f), true },
        { {24.0f, 2.0f, 2.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  false, glm::vec3(1.0f, 0.0f, 0.0f), true },
        { {20.0f, 2.0f, 6.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  false, glm::vec3(1.0f, 0.0f, 0.0f), true },
        { {24.0f, 2.0f, 6.0f},  {0.5f,0.5f,0.5f}, {0,0,0}, true,  false, glm::vec3(1.0f, 0.0f, 0.0f), true },
        { {-22.0f, 1.0f, 30.0f},{0.5f,0.5f,0.5f}, {0,0,0}, true,  true, glm::vec3(0.2f, 0.5f, 0.1f), true },
        };

for (const auto& c : cubes) {
    GameObject obj;
    obj.position = c.position;
    obj.halfExtents = c.halfExtents;
    obj.rotation = c.rotation;
    obj.hasCollision = c.hasCollision;
    obj.customColor = c.color;
    obj.useOutlineShader = true;

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
    glm::vec3 rotation;
    glm::vec3 color = glm::vec3(0.4f, 0.25f, 0.15f);   // dark brown wood
    bool useOutline = true;
};

std::vector<WallData> walls = {
    { {23.0f, 1.5f, 25.0f}, {0.0f, 0.0f, 0.0f}, glm::vec3(0.4f, 0.25f, 0.15f), false },
    { {23.0f, 4.5f, 25.0f}, {0.0f, 0.0f, 0.0f}, glm::vec3(0.4f, 0.25f, 0.15f), true },
    { {23.0f, 7.5f, 25.0f}, {0.0f, 0.0f, 0.0f}, glm::vec3(0.4f, 0.25f, 0.15f), true },
    // ... rest of your wall entries, each with the same color/outline appended
};

float wallW = 4.0f, wallH = 3.0f, wallD = 0.4f;
glm::vec3 wallHalfExtents(wallW * 0.5f, wallH * 0.5f, wallD * 0.5f);

for (const auto& w : walls) {
    GameObject wall;
    wall.hasCollision = true;
    wall.customColor = w.color;
    wall.useOutlineShader = w.useOutline;
    wall.halfExtents = wallHalfExtents;
    wall.position = w.position;
    wall.rotation = w.rotation;

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
    glm::vec3 halfExtents;
    glm::vec3 rotation;
    bool hasCollision;
    bool isDynamic;
    glm::vec3 color = glm::vec3(0.8f, 0.6f, 0.2f);     // warm orange
    bool useOutline = true;
};

std::vector<RampData> ramps = {
    { { -10.0f, -1.0f, 15.0f }, { 2.0f, 1.0f, 3.0f }, {0,0,0}, true, false, glm::vec3(0.8f, 0.6f, 0.2f), true },
    // add more ramps if you want, same pattern
};

for (const auto& r : ramps) {
    GameObject obj;
    obj.position = r.position;
    obj.halfExtents = r.halfExtents;
    obj.rotation = r.rotation;
    obj.hasCollision = r.hasCollision;
    obj.customColor = r.color;
    obj.useOutlineShader = r.useOutline;

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
    glm::vec3 visualHalfExtents;
    glm::vec3 colliderHalfExtents;
    glm::vec3 rotation;
    bool hasCollision;
    glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.8f);     // blue-grey
    bool useOutline = false;
};

std::vector<FlatQuadData> flatQuads = {
    { { 0.0f, 25.0f, -50.1f }, { 25.0f, 50.0f, 0.0f }, { 25.01f, 50.1f, 0.05f }, {0.0f,0.0f,90.0f}, true, glm::vec3(0.5f, 0.5f, 0.5f), false },
    //{ { -40.0f, 10.0f, -50.0f }, { 10.0f, 10.0f, 10.0f }, { 10.01f, 10.1f, 0.05f }, {0,0,0}, true, glm::vec3(0.5f, 0.5f, 0.8f), true },
    // ... rest
};

for (const auto& q : flatQuads) {
    GameObject obj;
    obj.position = q.position;
    obj.halfExtents = q.visualHalfExtents;           // <-- visual scale
    obj.colliderHalfExtents = q.colliderHalfExtents; // <-- collider size
    obj.rotation = q.rotation;
    obj.hasCollision = q.hasCollision;
    obj.customColor = q.color;
    obj.useOutlineShader = q.useOutline;

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
    glm::vec3 halfExtents;
    glm::vec3 colliderHalfExtents;
    glm::vec3 rotation;
    bool hasCollision;
    glm::vec3 color = glm::vec3(1.0f, 0.3f, 0.3f);     // red-ish
    bool useOutline = true;
};

std::vector<FlatTriData> flatTris = {
      // fill in when you want triangles; example:
      // { { 5.0f, 5.0f, -50.0f }, { 1.5f, 0.05f, 1.5f }, { 0.1f, 0.1f, 0.1f }, {0,0,0}, true, glm::vec3(1.0f, 0.3f, 0.3f), true },
      };

for (const auto& t : flatTris) {
    GameObject obj;
    obj.position = t.position;
    obj.halfExtents = t.halfExtents;
    obj.colliderHalfExtents = t.colliderHalfExtents;
    obj.rotation = t.rotation;
    obj.hasCollision = t.hasCollision;
    obj.customColor = t.color;
    obj.useOutlineShader = t.useOutline;

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Flattri::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* ptr = &objects.back();

    if (t.hasCollision) {
        physicsWorld.addOrientedCollider(t.position, t.halfExtents, t.rotation, false);
    }
}

// ------------------------------------------------------------------
//  Hazard Zones
// ------------------------------------------------------------------
struct LavaData {
    glm::vec3 position;
    glm::vec3 halfExtents;
    glm::vec3 rotation;
    float dps = 15.0f;
    glm::vec3 color = glm::vec3(1.0f, 0.15f, 0.0f);    // bright lava
    bool useOutline = false;                            // flat hot surface
};

std::vector<LavaData> lavas = {
    { { 5.0f, 0.1f, -5.0f }, { 3.0f, 0.1f, 3.0f }, {0,0,0}, 15.0f, glm::vec3(1.0f, 0.15f, 0.0f), false },
    // add more lava pools with custom colors if you like
};

for (const auto& l : lavas) {
    GameObject obj;
    obj.position = l.position;
    obj.halfExtents = l.halfExtents;
    obj.rotation = l.rotation;
    obj.isDamageOverTime = true;
    obj.damagePerSecond = l.dps;
    obj.hasCollision = false;
    obj.customColor = l.color;
    obj.useOutlineShader = l.useOutline;

    // Use a Wall mesh with a lava‑like colour (orange‑red)
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
    GameObject* ptr = &objects.back();

    // Optionally, add a thin oriented collider if you want it to block movement
    // physicsWorld.addOrientedCollider(...)  // not needed for non‑solid
}

struct InstantKillData {
    glm::vec3 position;
    glm::vec3 halfExtents;
    glm::vec3 rotation;
    glm::vec3 color = glm::vec3(0.8f, 0.0f, 1.0f);   // purple
};

std::vector<InstantKillData> spikeZones = {
    { { 15.0f, 0.1f, 10.0f }, { 1.0f, 0.1f, 1.0f }, {0,0,0} },
    };

for (const auto& ik : spikeZones) {
    GameObject obj;
    obj.position       = ik.position;
    obj.halfExtents    = ik.halfExtents;
    obj.rotation       = ik.rotation;
    obj.customColor    = ik.color;
    obj.useOutlineShader = false;
    obj.isInstantKill  = true;
    obj.hasCollision   = false;    // walk through it

    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    objects.push_back(std::move(obj));
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
checkpoint.position = glm::vec3(0.0f, 0.0f, 15.0f);
checkpoint.halfExtents = glm::vec3(1.0f, 0.3f, 1.0f);
checkpoint.isTrigger = true;
checkpoint.hasCollision = false;

// ------------------------------------------------------------------
//  Checkpoints
// ------------------------------------------------------------------
struct Checkpoint1Data {
    glm::vec3 position;
    glm::vec3 halfExtents = glm::vec3(1.0f, 0.3f, 1.0f);   // default size
    std::string name;
};

std::vector<Checkpoint1Data> checkpoints1 = {
    { {  50.0f, 2.0f, 50.0f }, {}, "CP1" },
    { { -50.0f, 2.0f, -50.0f }, {}, "CP2" },
    { {-50.0f, 2.0f, 50.0f }, {}, "CP3" },
    { {  50.0f, 2.0f,  -50.0f }, {}, "CP4" }

};

for (const auto& cp : checkpoints1) {
    GameObject checkpoint;
    checkpoint.position    = cp.position;
    checkpoint.halfExtents = cp.halfExtents;
    checkpoint.isTrigger   = true;
    checkpoint.hasCollision = false;
    checkpoint.label       = cp.name;      // 3D label
    checkpoint.customColor = glm::vec3(0.0f, 1.0f, 0.0f);
    checkpoint.useOutlineShader = true;

    // Optional visual mesh (you can choose any mesh you want)
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    // Capture the name by copy for the lambda
    std::string cpName = cp.name;
    checkpoint.onTriggerEnter = [cpName](Player& p) {
        LevelManager::getInstance().unlockCheckpoint(cpName);
    };

    objects.push_back(std::move(checkpoint));
}
// ===== Level Transition Trigger (Tutorial -> Level1) =====
GameObject levelExit1;
levelExit1.position = glm::vec3(25.0f, 0.1f, 50.0f);
levelExit1.halfExtents = glm::vec3(1.0f, 0.0f, 1.0f);
levelExit1.isTrigger = true;
levelExit1.hasCollision = false;

struct Checkpoint2Data {
    glm::vec3 position;
    glm::vec3 halfExtents = glm::vec3(1.0f, 0.3f, 1.0f);   // default size
    std::string name;
};

std::vector<Checkpoint2Data> checkpoints2 = {
    { {  50.0f, 5.0f, 50.0f }, {}, "L1_CP1" },
    { { -50.0f, 5.0f, -50.0f }, {}, "L1_CP2" },
    { {-50.0f, 5.0f, 50.0f }, {}, "L1_CP3" },
    { {  50.0f, 5.0f,  -50.0f }, {}, "L1_CP4" }

};

for (const auto& cp : checkpoints2) {
    GameObject checkpoint;
    checkpoint.position    = cp.position;
    checkpoint.halfExtents = cp.halfExtents;
    checkpoint.isTrigger   = true;
    checkpoint.hasCollision = false;
    checkpoint.label       = cp.name;
    checkpoint.customColor = glm::vec3(0.0f, 1.0f, 0.0f);
    checkpoint.useOutlineShader = true;

    // Optional visual mesh (you can choose any mesh you want)
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
    checkpoint.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

    // Capture the name by copy for the lambda
    std::string cpName = cp.name;
    checkpoint.onTriggerEnter = [cpName](Player& p) {
        LevelManager::getInstance().unlockCheckpoint(cpName);
    };

    objects.push_back(std::move(checkpoint));
}
// ===== Moving Platform =====
GameObject platform;
platform.position = glm::vec3(-20.0f, 1.0f, 30.0f);
platform.halfExtents = glm::vec3(1.5f, 0.2f, 1.5f);
platform.hasCollision = true;
platform.isMoving = true;
platform.startPos = platform.position;
platform.endPos = platform.position + glm::vec3(0.0f, 0.0f, -50.0f);
platform.moveSpeed = 3.0f;

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
platform2.endPos = platform2.position + glm::vec3(0.0f, 30.0f, 0.0f);
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

