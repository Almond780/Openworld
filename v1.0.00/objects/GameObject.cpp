#include "GameObject.hpp"
#include <iostream>

void GameObject::draw(int lodLevel) const {
        //Debug print
    //std::cout << "GameObject::draw called with lodLevel=" << lodLevel
              //<< " lodMeshes.size()=" << lodMeshes.size() << std::endl;
    if (lodLevel >= 0 && lodLevel < static_cast<int>(lodMeshes.size())) {
        //std::cout << "  Drawing mesh at lodLevel " << lodLevel << std::endl;
        lodMeshes[lodLevel]->draw();
    } else {
        //std::cout << "  lodLevel out of range!" << std::endl;
    }
}