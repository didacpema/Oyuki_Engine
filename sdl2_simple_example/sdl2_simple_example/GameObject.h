#pragma once
#include <string>
#include <vector>

class GameObject {
public:
    std::string name;
    std::vector<GameObject*> children;

    GameObject(const std::string& name) : name(name) {}
    ~GameObject() {
        for (auto* child : children) delete child;
    }

    void addChild(GameObject* child) { children.push_back(child); }
};