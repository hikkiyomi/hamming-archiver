#pragma once

#include <filesystem>
#include <unordered_set>

class Archiver {
public:
    void Create();
    void Append(const std::filesystem::path&);
    void Extract(const std::unordered_set<std::string>& files = {});
    void ShowData();
    void Delete(const std::unordered_set<std::string>&);

    Archiver(const std::filesystem::path&);
private:
    std::filesystem::path archive_path;
};

void Merge(std::filesystem::path& archive_1, std::filesystem::path& archive_2, std::filesystem::path& merge_path);
