#include "LevelManager.hpp"
#include "objects/Player.hpp"
#include <iostream>
#include <algorithm>

void LevelManager::registerLevel(const Level& level) {
    levels.push_back(level);
    std::cout << "Registered level: " << level.name << std::endl;
}

void LevelManager::loadLevel(const std::string& levelName) {
    auto it = std::find_if(levels.begin(), levels.end(),
                           [&](const Level& l) { return l.name == levelName; });
    if (it == levels.end()) {
        std::cerr << "Level not found: " << levelName << std::endl;
        return;
    }
    currentLevel = &(*it);
    std::cout << "Loading level: " << currentLevel->name << std::endl;

    // Reset all checkpoints in this level to locked (except maybe first?)
    for (auto& cp : currentLevel->checkpoints) {
        cp.unlocked = false;
    }

    // Call level's custom build function if provided
    if (currentLevel->onLoad) {
        currentLevel->onLoad();
    }
}

void LevelManager::unlockCheckpoint(const std::string& checkpointName) {
    if (!currentLevel) return;
    for (auto& cp : currentLevel->checkpoints) {
        if (cp.name == checkpointName) {
            if (!cp.unlocked) {
                cp.unlocked = true;
                std::cout << "Checkpoint unlocked: " << cp.name << " at ("
                          << cp.position.x << ", " << cp.position.y << ", " << cp.position.z << ")" << std::endl;
            }
            return;
        }
    }
    std::cerr << "Checkpoint not found: " << checkpointName << std::endl;
}

glm::vec3 LevelManager::getRespawnPosition() const {
    if (!currentLevel) return glm::vec3(0.0f, 2.0f, 5.0f); // fallback

    // Find the last unlocked checkpoint (most recent)
    const Checkpoint* lastUnlocked = nullptr;
    for (const auto& cp : currentLevel->checkpoints) {
        if (cp.unlocked) lastUnlocked = &cp;
    }
    return lastUnlocked ? lastUnlocked->position : currentLevel->initialSpawn;
}

void LevelManager::respawn(Player& player) {
    glm::vec3 pos = getRespawnPosition();
    player.getRigidBody()->position = pos;
    player.getRigidBody()->velocity = glm::vec3(0.0f);
    std::cout << "Respawning at " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
}

bool LevelManager::isCheckpointUnlocked(const std::string& name) const {
    if (!currentLevel) return false;
    for (const auto& cp : currentLevel->checkpoints) {
        if (cp.name == name) return cp.unlocked;
    }
    return false;
}

void LevelManager::listLevels() const {
    std::cout << "Available levels:\n";
    for (const auto& l : levels) {
        std::cout << "  - " << l.name << " (spawn: " << l.initialSpawn.x << ", " << l.initialSpawn.y << ", " << l.initialSpawn.z << ")\n";
    }
}

void LevelManager::listCheckpoints() const {
    if (!currentLevel) {
        std::cout << "No level loaded.\n";
        return;
    }
    std::cout << "Checkpoints in " << currentLevel->name << ":\n";
    for (const auto& cp : currentLevel->checkpoints) {
        std::cout << "  - " << cp.name << " [" << (cp.unlocked ? "UNLOCKED" : "locked") << "] at "
                  << cp.position.x << ", " << cp.position.y << ", " << cp.position.z << "\n";
    }
}