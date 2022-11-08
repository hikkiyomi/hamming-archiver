#pragma once

#include <filesystem>
#include <unordered_set>

class Archiver {
public:
    void Create();
    void Append(const std::filesystem::path&);
    void Extract();
    void ShowData();
    void Delete(const std::unordered_set<std::string>&);
    void Merge(const std::filesystem::path&);

    Archiver(const std::filesystem::path&);
private:
    std::filesystem::path archive_path;
};
