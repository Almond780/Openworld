#include "LevelManager.hpp"
#include "objects/Player.hpp"
#include <algorithm>
#include <iostream>

// ------------------------------------------------------------------
// Internal helpers
// ------------------------------------------------------------------

Level* LevelManager::getCurrentLevel() {
    if (currentLevelIndex < 0 || currentLevelIndex >= static_cast<int>(levels.size()))
        return nullptr;
    return &levels[currentLevelIndex];
}

const Level* LevelManager::getCurrentLevel() const {
    if (currentLevelIndex < 0 || currentLevelIndex >= static_cast<int>(levels.size()))
        return nullptr;
    return &levels[currentLevelIndex];
}

// ------------------------------------------------------------------
// Public API
// ------------------------------------------------------------------

void LevelManager::registerLevel(const Level& level) {
    // Reserve space so the vector never reallocates (keeps pointers stable)
    if (levels.empty())
        levels.reserve(10);
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

    // Store the index instead of a pointer
    currentLevelIndex = static_cast<int>(std::distance(levels.begin(), it));

    // Reset all checkpoints in this level to locked
    Level* level = getCurrentLevel();
    if (level) {
        for (auto& cp : level->checkpoints) {
            cp.unlocked = false;
        }
        // Call the level's optional load callback
        if (level->onLoad) {
            level->onLoad();
        }
    }

    std::cout << "Loading level: " << levelName << std::endl;
}

void LevelManager::unlockCheckpoint(const std::string& checkpointName) {
    Level* level = getCurrentLevel();
    if (!level) return;

    for (auto& cp : level->checkpoints) {
        if (cp.name == checkpointName) {
            if (!cp.unlocked) {
                cp.unlocked = true;
                std::cout << "Checkpoint unlocked: " << cp.name
                          << " at (" << cp.position.x << ", "
                          << cp.position.y << ", "
                          << cp.position.z << ")" << std::endl;
            }
            return;
        }
    }
    std::cerr << "Checkpoint not found: " << checkpointName << std::endl;
}

glm::vec3 LevelManager::getRespawnPosition() const {
    const Level* level = getCurrentLevel();
    if (!level)
        return glm::vec3(0.0f, 2.0f, 5.0f);   // failsafe origin

    // Look for the last unlocked checkpoint
    const Checkpoint* lastUnlocked = nullptr;
    for (const auto& cp : level->checkpoints) {
        if (cp.unlocked)
            lastUnlocked = &cp;
    }

    return lastUnlocked ? lastUnlocked->position : level->initialSpawn;
}

bool LevelManager::isCheckpointUnlocked(const std::string& name) const {
    const Level* level = getCurrentLevel();
    if (!level) return false;

    for (const auto& cp : level->checkpoints) {
        if (cp.name == name)
            return cp.unlocked;
    }
    return false;
}

void LevelManager::respawn(Player& player) {
    glm::vec3 pos = getRespawnPosition();
    std::cout << "[LevelManager] Respawning at "
              << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

    RigidBody* rb = player.getRigidBody();
    if (rb) {
        rb->position = pos;
        rb->velocity = glm::vec3(0.0f);
    }
}

void LevelManager::listLevels() const {
    std::cout << "Available levels:\n";
    for (const auto& l : levels) {
        std::cout << "  - " << l.name << " (spawn: "
                  << l.initialSpawn.x << ", "
                  << l.initialSpawn.y << ", "
                  << l.initialSpawn.z << ")\n";
    }
}

void LevelManager::listCheckpoints() const {
    const Level* level = getCurrentLevel();
    if (!level) {
        std::cout << "No level loaded.\n";
        return;
    }
    std::cout << "Checkpoints in " << level->name << ":\n";
    for (const auto& cp : level->checkpoints) {
        std::cout << "  - " << cp.name
                  << " [" << (cp.unlocked ? "UNLOCKED" : "locked") << "]"
                  << " at " << cp.position.x << ", "
                  << cp.position.y << ", "
                  << cp.position.z << "\n";
    }
}