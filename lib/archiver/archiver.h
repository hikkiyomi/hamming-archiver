#pragma once

#include "tools/tools.h"
#include "../filemaker/filemaker.h"

#include <filesystem>
#include <unordered_set>

class Archiver {
public:
    Archiver(const std::filesystem::path& _archive_path, bool _restore = false);

    void Create();
    void Append(const std::filesystem::path& file_path);
    void Extract(const std::unordered_set<std::string>& files = {});
    void ShowData();
    void Delete(const std::unordered_set<std::string>& files);
    void Merge(std::filesystem::path& archive_1, std::filesystem::path& archive_2);
private:
    std::filesystem::path archive_path;
    Manipulator manipulator;
    bool restore;

    bool CheckOnAvailability(const std::filesystem::path& archive_path, const std::string& file_name);
    void ReadFileInfo(std::ifstream& stream, HAFInfo& header);
    void WriteFileInfo(std::ofstream& stream, const HAFInfo& header);
    void WriteArchive(std::ofstream& output_stream, const std::filesystem::path& path, std::unordered_set<std::string>& merged_files);
};
