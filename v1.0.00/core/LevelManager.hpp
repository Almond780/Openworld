// core/LevelManager.hpp
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

class Player;  // forward declaration – avoids circular include

struct Checkpoint {
    glm::vec3 position;
    std::string name;
    bool unlocked = false;
};

struct Level {
    std::string name;
    glm::vec3 initialSpawn;
    std::vector<Checkpoint> checkpoints;
    std::function<void()> onLoad;
};

class LevelManager {
public:
    static LevelManager& getInstance() {
        static LevelManager instance;
        return instance;
    }

    void registerLevel(const Level& level);
    void loadLevel(const std::string& levelName);
    void unlockCheckpoint(const std::string& checkpointName);
    glm::vec3 getRespawnPosition() const;
    bool isCheckpointUnlocked(const std::string& name) const;
    void respawn(Player& player);

    const std::string& getCurrentLevelName() const;
    const Level* getCurrentLevel() const;
    Level* getCurrentLevel();          // non-const overload

    void listLevels() const;
    void listCheckpoints() const;

private:
    LevelManager() = default;
    std::vector<Level> levels;
    int currentLevelIndex = -1;   // -1 means no level loaded
    std::string emptyName;
};