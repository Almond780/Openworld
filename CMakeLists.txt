cmake_minimum_required(VERSION 3.16)
project(OpenWorld VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# GLM (install with: sudo pacman -S glm)
find_package(glm REQUIRED)

# GLAD
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# Main executable
add_executable(openworld
    main.cpp
    core/Mesh.hpp
    core/Mesh.cpp
    core/Shader.hpp
    core/Shader.cpp
    core/FirstPersonCamera.hpp
    core/FirstPersonCamera.cpp
    core/Frustum.hpp
    core/Frustum.cpp
    core/FPSCounter.hpp
    core/FPSCounter.cpp
    core/ResourceManager.hpp
    core/ResourceManager.cpp
    core/physics/RigidBody.hpp
    core/physics/RigidBody.cpp
    core/physics/Collider.hpp
    core/physics/Collider.cpp
    core/physics/PhysicsWorld.hpp
    core/physics/PhysicsWorld.cpp
    objects/Player.hpp
    objects/Player.cpp
    objects/GameObject.hpp
    objects/GameObject.cpp
    objects/Plane.hpp
    objects/Plane.cpp
    objects/Cube.hpp
    objects/Cube.cpp
    assets/shaders/outline/outline.vert
    assets/shaders/outline/outline.geom
    assets/shaders/outline/outline.frag
    core/core/stb_easy_font.h
    assets/shaders/text/text.vert
    assets/shaders/text/text.frag
    core/TextRenderer.hpp
    core/TextRenderer.cpp
    core/Version.hpp
    objects/wall.hpp
    objects/wall.cpp
    objects/ObjectFactory.hpp
    objects/ObjectFactory.cpp
    core/AABBHelpers.hpp
    objects/Ramp.cpp
    objects/Ramp.hpp
    objects/Flatquad.hpp
    objects/Flatquad.cpp
    objects/Flattri.hpp
    objects/Flattri.cpp
    core/SpriteRenderer.hpp
    core/SpriteRenderer.cpp
    assets/shaders/sprite.vert
    assets/shaders/sprite.frag
    core/LevelManager.hpp
    core/LevelManager.cpp
)

target_compile_definitions(openworld PRIVATE
    PROJECT_VERSION=\"0.0.00\"
    RELEASE_STAGE=\"BETA\"
)

target_include_directories(openworld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(openworld PRIVATE ${GLFW_INCLUDE_DIRS})

target_link_libraries(openworld
    PRIVATE
    OpenGL::GL
    ${GLFW_LIBRARIES}
    glad
    glm::glm
)

add_custom_command(TARGET openworld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# Qt Creator run configuration
set_target_properties(openworld PROPERTIES
    VS_DEBUGGER_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/openworld_alpha"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    VS_DEBUGGER_ENVIRONMENT "__NV_PRIME_RENDER_OFFLOAD=1;__GLX_VENDOR_LIBRARY_NAME=nvidia"
)