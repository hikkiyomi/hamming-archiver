#pragma once

#include <filesystem>

class Archiver {
public:
    void Create();
    void Append(std::filesystem::path);

    Archiver(std::filesystem::path);
private:
    std::filesystem::path archive_path;
};
