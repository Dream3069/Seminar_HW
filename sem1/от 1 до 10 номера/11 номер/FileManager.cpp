#include "FileManager.hpp"
#include <iostream>
#include <fstream>

bool FileManager::saveGroup(const Group& group, const std::string& filename) {
    return group.saveToFile(filename);
}

bool FileManager::loadGroup(Group& group, const std::string& filename) {
    return group.loadFromFile(filename);
}
