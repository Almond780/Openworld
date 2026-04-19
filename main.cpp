
//The file structure
//openworld_alpha/
//├── CMakeLists.txt
//├── main.cpp                         # Launcher – sets up window, main loop, orchestrates scene
//│
//├── headerfiles/                     # Same as core/ but as .hpp
//├── core/                            # Engine-level reusable components
//│   ├── Mesh.hpp / Mesh.cpp          # VAO/VBO wrapper, draw calls
//|   ├── core/
//|   |    ├── stb_easy_font.h
//│   ├── Shader.hpp / Shader.cpp      # Compiles and manages shader programs
//│   ├── Camera.hpp / Camera.cpp      # Orbit camera and input handling
//│   ├── FPSCounter.hpp / FPSCounter.cpp  # FPS measurement
//│   └── ResourceManager.hpp / ResourceManager.cpp  # central asset loading
//│   │
//│   └── physics/                         #  New physics subsystem
//│       ├── RigidBody.hpp / .cpp         # Mass, velocity, forces, gravity
//│       ├── Collider.hpp / .cpp          # Bounding boxes, spheres, collision detection
//│       └── PhysicsWorld.hpp / .cpp      # Manages all physics objects, applies gravity, resolves collisions
//│
//├── objects/                         # Specific game objects (each with its own data/logic)
//│   ├── Cube.hpp / Cube.cpp          # Generates cube vertices (low, med, high variants)
//│   ├── Sphere.hpp / Sphere.cpp      # Generates sphere vertices with LOD levels
//│   ├── Table.hpp / Table.cpp        # Could load from file or generate procedurally
//|   ├── Player.hpp / .cpp                # Player-specific logic (camera + physics body)
//│   └── ... (any other mesh types)
//│
//├── assets/                          #  Raw data files (not code)
//│   ├── models/                      # .obj, .gltf, or custom .mesh files
//│   │   ├── table_high.obj
//│   │   ├── table_med.obj
//│   │   └── table_low.obj
//│   ├── textures/                    # .png, .jpg, .hdr
//│   └── shaders/
//│       ├── basic.vert
//│       ├── basic.frag
//│       ├── text/
//│       │    ├── text.frag / text.vert
//│       └── outline/                    # Outline shaders
//│           ├── outline.vert
//│           ├── outline.geom
//│           └── outline.frag
//│
//├── glad/                            # OpenGL loader
//│   ├── include/
//│   └── src/
//│
//└── build/# CMake output

// main.cpp

//1u = 1 unit = 1 meter



#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/FirstPersonCamera.hpp"
#include "core/Frustum.hpp"
#include "core/FPSCounter.hpp"
#include "core/ResourceManager.hpp"
#include "core/physics/PhysicsWorld.hpp"
#include "objects/Player.hpp"
#include "objects/GameObject.hpp"
#include "objects/Plane.hpp"
#include "objects/Cube.hpp"
#include "core/TextRenderer.hpp"
#include "core/Version.hpp"

// ------------------------------------------------------------------
// AABB Helper Functions (put these after all #includes)
// ------------------------------------------------------------------

// Simple overlap test (no correction)
inline bool checkAABBOverlap(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// Overlap test with penetration correction (for push/pull)
inline bool AABBvsAABB(const AABB& a, const AABB& b, glm::vec3& correction) {
    float dx = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float dy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float dz = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);
    if (dx > 0.0f && dy > 0.0f && dz > 0.0f) {
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


// ------------------------------------------------------------------
// Window and input globals
// ------------------------------------------------------------------
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool keys[1024] = {false};
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    FirstPersonCamera* cam = static_cast<FirstPersonCamera*>(glfwGetWindowUserPointer(window));
    if (cam) cam->processMouseMovement(xoffset, yoffset);
}

// ------------------------------------------------------------------
// Main
// ------------------------------------------------------------------
int main() {



    // GLFW / OpenGL init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenWorld", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    TextRenderer::init();

    Shader outlineShader("assets/shaders/outline/outline.vert",
                         "assets/shaders/outline/outline.frag",
                         "assets/shaders/outline/outline.geom");
    Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenWorld " << RELEASE_STAGE << " v" << PROJECT_VERSION << std::endl;

    glEnable(GL_DEPTH_TEST);

    // Physics world
    PhysicsWorld physicsWorld;
    physicsWorld.setGravity(glm::vec3(0.0f, -8.0f, 0.0f));

    // Player
    Player player(glm::vec3(0.0f, 2.0f, 5.0f));
    // Optionally set custom halfExtents for player (default is 0.5,1.0,0.5 in Player constructor)
    // We'll update Player constructor to accept halfExtents.
    physicsWorld.addRigidBody(player.getRigidBody());

    // Camera
    FirstPersonCamera camera(&player, glm::vec3(0.0f, 1.0f, 0.0f));
    glfwSetWindowUserPointer(window, &camera);

    // Ground plane (visual + collision)
    Plane ground(50.0f, 50.0f);
    physicsWorld.addCollisionPlane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);

    // All game objects
    std::vector<GameObject> objects;
    // Dynamic objects list for push/pull
    std::vector<std::pair<GameObject*, RigidBody*>> dynamicObjects;

    objects.reserve(1000);   // or however many objects you plan to create

    // ------------------------------------------------------------------
    //  Cubes (some dynamic, some static)
    // ------------------------------------------------------------------
    std::vector<glm::vec3> cubePositions = {
        //Dynamic
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-2.0f, 4.0f, 2.0f),
        glm::vec3(2.0f, 4.0f, -2.0f),
        glm::vec3(2.0f, 4.0f, 2.0f),
        glm::vec3(-2.0f, 4.0f, -2.0f),
        //static
        glm::vec3(22.0f, 1.0f, 4.0f),
        glm::vec3(20.0f, 2.0f, 2.0f),
        glm::vec3(24.0f, 2.0f, 2.0f),
        glm::vec3(20.0f, 2.0f, 6.0f),
        glm::vec3(24.0f, 2.0f, 6.0f),
    };

    // Custom half‑extents for each cube (half‑width, half‑height, half‑depth)
    std::vector<glm::vec3> cubeHalfExtents = {

        glm::vec3(0.5f, 0.5f, 0.5f),   // cube 0: 1x1x1
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),

        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
    };

    std::vector<bool> hasCollisionFlags = {true, true, true, true, true, true, true, true, true, true,};
    std::vector<bool> isDynamicFlags   = {true, true, true, true, true, false, false, false, false, false,}; // true, false,

    for (size_t i = 0; i < cubePositions.size(); ++i) {
        GameObject obj;
        obj.position = cubePositions[i];
        obj.hasCollision = hasCollisionFlags[i];
        obj.halfExtents = cubeHalfExtents[i];

        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

        objects.push_back(std::move(obj));
        GameObject* objPtr = &objects.back();

        // Only add static AABB if the cube is NOT dynamic but still has collision
        if (hasCollisionFlags[i] && !isDynamicFlags[i]) {
            AABB aabb;
            aabb.min = cubePositions[i] - cubeHalfExtents[i];
            aabb.max = cubePositions[i] + cubeHalfExtents[i];
            physicsWorld.addStaticAABB(aabb);
        }

        if (isDynamicFlags[i]) {
            RigidBody* rb = new RigidBody(cubePositions[i], 10.0f, cubeHalfExtents[i]); // pass size
            physicsWorld.addRigidBody(rb);
            dynamicObjects.push_back({objPtr, rb});
        }
    }

    // ------------------------------------------------------------------
    // Walls (static, using scaled cube mesh)
    // ------------------------------------------------------------------
    std::vector<glm::vec3> wallPositions = {
        glm::vec3(23.0f, 1.5f, 25.0f),
        glm::vec3(23.0f, 4.5f, 25.0f),
        glm::vec3(23.0f, 7.5f, 25.0f),
        //Door2
        //glm::vec3(19.0f, 1.5f, 25.0f),
        glm::vec3(19.0f, 4.5f, 25.0f),
        glm::vec3(19.0f, 7.5f, 25.0f),
        glm::vec3(15.0f, 1.5f, 25.0f),
        glm::vec3(15.0f, 4.5f, 25.0f),
        glm::vec3(15.0f, 7.5f, 25.0f),
        glm::vec3(11.0f, 1.5f, 25.0f),
        glm::vec3(11.0f, 4.5f, 25.0f),
        glm::vec3(11.0f, 7.5f, 25.0f),
        //Door1
        //glm::vec3(7.0f, 1.5f, 25.0f),

        glm::vec3(7.0f, 4.5f, 25.0f),
        glm::vec3(7.0f, 7.5f, 25.0f),
        glm::vec3(3.0f, 1.5f, 25.0f),
        glm::vec3(3.0f, 4.5f, 25.0f),
        glm::vec3(3.0f, 7.5f, 25.0f),
    };
    float wallW = 4.0f, wallH = 3.0f, wallD = 0.4f;
    glm::vec3 wallHalfExtents(wallW * 0.5f, wallH * 0.5f, wallD * 0.5f);

    for (const auto& pos : wallPositions) {
        GameObject obj;
        obj.position = pos;
        obj.hasCollision = true;
        obj.halfExtents = wallHalfExtents;

        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateHighPoly(0.5f))));
        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateMediumPoly(0.5f))));
        obj.lodMeshes.push_back(std::make_shared<Mesh>(std::move(Cube::generateLowPoly(0.5f))));

        objects.push_back(std::move(obj));

        AABB aabb;
        aabb.min = pos - wallHalfExtents;
        aabb.max = pos + wallHalfExtents;
        physicsWorld.addStaticAABB(aabb);
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
    door.hingeOffset = glm::vec3(-2.0f, 0.0f, 0.0f); // left edge

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

    // ------------------------------------------------------------------
    // FPS Counter and Frustum
    // ------------------------------------------------------------------
    FPSCounter fpsCounter;
    Frustum frustum;

    // ------------------------------------------------------------------
    // Main Loop
    // ------------------------------------------------------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        if (keys[GLFW_KEY_ESCAPE]) glfwSetWindowShouldClose(window, true);
        camera.processKeyboard(keys, deltaTime);


        // Door interaction (F key)
        if (keys[GLFW_KEY_F]) {
            glm::vec3 playerPos = player.getPosition();
            float interactRange = 10.0f;

            for (auto& obj : objects) {
                if (!obj.isAnimated) continue;

                float dist = glm::distance(playerPos, obj.position);
                if (dist < interactRange) {
                    obj.isOpen = !obj.isOpen;
                    //std::cout << "Door " << (obj.isOpen ? "opened" : "closed") << std::endl;

                    // Toggle collision AABB
                    AABB doorAABB;
                    doorAABB.min = obj.position - obj.halfExtents;
                    doorAABB.max = obj.position + obj.halfExtents;

                    if (obj.isOpen) {
                        physicsWorld.removeStaticAABB(doorAABB);
                        obj.hasCollision = false; // also update the visual flag
                    } else {
                        physicsWorld.addStaticAABB(doorAABB);
                        obj.hasCollision = true;
                    }
                    break;
                }
            }
        }

        // Jump
        if (keys[GLFW_KEY_SPACE] && player.getRigidBody()->isGrounded) {
            player.getRigidBody()->velocity.y = 5.0f;
        }


        // Push / Pull dynamic objects (E key)
        if (keys[GLFW_KEY_E]) {
            glm::vec3 playerPos = player.getPosition();
            glm::vec3 forward = camera.getForward();
            float reach = 2.5f;

            AABB playerAABB;
            playerAABB.min = playerPos - glm::vec3(0.5f, 1.0f, 0.5f);
            playerAABB.max = playerPos + glm::vec3(0.5f, 1.0f, 0.5f);
            playerAABB.min += forward * reach * 0.5f;
            playerAABB.max += forward * reach * 0.5f;

            for (auto& [obj, rb] : dynamicObjects) {
                AABB objAABB;
                objAABB.min = obj->position - obj->halfExtents;
                objAABB.max = obj->position + obj->halfExtents;

                glm::vec3 correction;
                if (AABBvsAABB(playerAABB, objAABB, correction)) {
                    float forceMagnitude = 500.0f * deltaTime;
                    glm::vec3 forceDir = (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT])
                                             ? glm::normalize(playerPos - obj->position)
                                             : forward;
                    rb->applyForce(forceDir * forceMagnitude, deltaTime);
                    break;
                }
            }
        }

        // Physics step
        physicsWorld.step(deltaTime);

        // Update dynamic object positions from physics
        for (auto& [obj, rb] : dynamicObjects) {
            obj->position = rb->position;
        }

        // Trigger detection (after physics step)
        AABB playerAABB;
        playerAABB.min = player.getPosition() - glm::vec3(0.5f, 1.0f, 0.5f);
        playerAABB.max = player.getPosition() + glm::vec3(0.5f, 1.0f, 0.5f);

        for (auto& obj : objects) {
            if (!obj.isTrigger) continue;

            AABB triggerAABB;
            triggerAABB.min = obj.position - obj.halfExtents;
            triggerAABB.max = obj.position + obj.halfExtents;

            if (checkAABBOverlap(playerAABB, triggerAABB)) {
                if (!obj.triggerFired && obj.onTriggerEnter) {
                    obj.triggerFired = true;
                    obj.onTriggerEnter(player);
                }
            } else {
                obj.triggerFired = false;
            }

        }


        // Updated animations
        for (auto& obj : objects) {
            if (!obj.isAnimated) continue;

            float targetRotation = obj.isOpen ? obj.openAngle : 0.0f;
            // Smoothly interpolate current rotation toward target
            float diff = targetRotation - obj.currentRotation;
            obj.currentRotation += diff * obj.animationSpeed * deltaTime;

            // Clamp to avoid overshoot
            if (fabs(diff) < 0.1f) obj.currentRotation = targetRotation;
            
        }

        // Camera update
        camera.update(deltaTime);

        // View / Projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()),
                                                (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        frustum.extractPlanes(projection * view);

        // FPS & Window Title
        fpsCounter.update();
        char title[128];
        snprintf(title, sizeof(title), "OpenWorld %s v%s | FPS: %.1f | Pos: (%.1f, %.1f, %.1f)",
                 RELEASE_STAGE, PROJECT_VERSION, fpsCounter.getCurrentFPS(),
                 player.getPosition().x, player.getPosition().y, player.getPosition().z);
        glfwSetWindowTitle(window, title);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Ground plane (basic shader, no outline)
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        shader.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 1.0f)); // cyan
        ground.draw();

        // All other objects with outline shader
        outlineShader.use();
        outlineShader.setMat4("projection", projection);
        outlineShader.setMat4("view", view);
        outlineShader.setVec3("edgeColor", glm::vec3(0.0f, 0.0f, 0.0f));
        outlineShader.setFloat("edgeWidth", 0.04f);

        glm::vec3 camPos = camera.getPosition();
        for (auto& obj : objects) {
            // Frustum culling
            glm::vec3 min = obj.position - obj.halfExtents;
            glm::vec3 max = obj.position + obj.halfExtents;
            if (!frustum.isAABBVisible(min, max)) continue;

            // LOD selection
            float dist = glm::distance(camPos, obj.position);
            int lodLevel = 0;
            if (dist > 30.0f) lodLevel = 2;
            else if (dist > 15.0f) lodLevel = 1;
            else lodLevel = 0;

            // Color based on type
            glm::vec3 color;
            if (obj.isTrigger) {
                color = glm::vec3(0.0f, 1.0f, 0.0f); // cyan = trigger
            } else if (obj.hasCollision) {
                color = glm::vec3(1.0f, 0.0f, 0.0f); // red = solid
            } else {
                color = glm::vec3(0.5f, 0.5f, 0.5f); // grey = non-solid
            }

            glm::mat4 model = glm::mat4(1.0f);

            if (obj.isAnimated) {
                // Hinged door: translate to hinge, rotate, translate back, then scale
                model = glm::translate(model, obj.position + obj.hingeOffset);
                model = glm::rotate(model, glm::radians(obj.currentRotation), obj.rotationAxis);
                model = glm::translate(model, -obj.hingeOffset);
                model = glm::scale(model, obj.halfExtents * 2.0f);
            } else {
                // Standard objects (cubes, walls, jump pad)
                float angle = (obj.halfExtents == glm::vec3(0.0f)) ? currentFrame * 0.5f : 0.0f;
                model = glm::translate(model, obj.position);
                model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
                if (obj.halfExtents != glm::vec3(0.5f)) {
                    model = glm::scale(model, obj.halfExtents * 2.0f);
                }
            }

            outlineShader.setMat4("model", model);
            outlineShader.setVec3("objectColor", color);
            obj.draw(lodLevel);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Text overlay (currently non-functional, but calls are harmless)
        TextRenderer::printFPS(fpsCounter.getCurrentFPS(), 10.0f, SCR_HEIGHT - 30.0f, 1.0f);
        TextRenderer::print("TEST", 100.0f, 100.0f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));


    }

    TextRenderer::shutdown();
    glfwTerminate();
    return 0;
}
