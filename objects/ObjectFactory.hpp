#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "GameObject.hpp"
#include "Player.hpp"
#include "core/physics/PhysicsWorld.hpp"

void buildWorld(std::vector<GameObject>& objects,
                PhysicsWorld& physicsWorld,
                std::vector<std::pair<GameObject*, RigidBody*>>& dynamicObjects,
                Player& player);