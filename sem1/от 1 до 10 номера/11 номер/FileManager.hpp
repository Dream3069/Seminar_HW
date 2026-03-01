#pragma once
#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <string>
#include "Group.hpp"

class FileManager {
public:
    static bool saveGroup(const Group& group, const std::string& filename);
    static bool loadGroup(Group& group, const std::string& filename);
};

#endif
