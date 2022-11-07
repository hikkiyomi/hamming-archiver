#pragma once

#include <cinttypes>
#include <filesystem>
#include <string>

class File {
public:
    File(const char* file_path);

    void Show(); // FOR DEBUGGING
    std::filesystem::path GetPath();
private:
    std::filesystem::path file_path;
    std::string name;
    uint64_t size;
};
