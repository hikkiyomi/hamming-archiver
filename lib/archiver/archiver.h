#pragma once

#include <filesystem>

class Archiver {
public:
    void Create();
    void Append(std::filesystem::path);
    void Extract();
    void ShowData();

    Archiver(std::filesystem::path);
private:
    std::filesystem::path archive_path;
};
