#pragma once

#include <cinttypes>
#include <filesystem>
#include <string>

struct HAFInfo {
    size_t file_name_length;
    std::string file_name;
    uint64_t file_size;

    HAFInfo();
    HAFInfo(size_t, const std::string&, uint64_t);
};

class File {
public:
    HAFInfo ExportIntoHAF();

    File(std::filesystem::path);

    void Show(); // FOR DEBUGGING
    std::filesystem::path GetAbsolutePath();
    std::string GetName();
    uint64_t GetSize();
private:
    std::filesystem::path file_path;
};
