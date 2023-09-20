#include "filemaker.h"

#include <iostream>

HAFInfo::HAFInfo() {}

HAFInfo::HAFInfo(size_t _file_name_length, const std::string& _file_name, uint64_t _file_size) 
    : file_name_length(_file_name_length)
    , file_name(_file_name)
    , file_size(_file_size)
{}

HAFInfo File::ExportIntoHAF() {
    std::string file_name = GetName();

    return HAFInfo(
        file_name.size(),
        file_name,
        GetSize()
    );
}

File::File(const std::filesystem::path& _file_path)
    : file_path_(_file_path)
{}

void File::Show() { // FOR DEBUGGING
    std::cout << "Absolute path: " << GetAbsolutePath() << std::endl;
    std::cout << "Name of file: " << GetName() << std::endl;
    std::cout << "File size: " << GetSize() << " bytes" << std::endl;
}

std::filesystem::path File::GetAbsolutePath() {
    return std::filesystem::absolute(file_path_);
}

std::string File::GetName() {
    return file_path_.filename().string();
}

uint64_t File::GetSize() {
    return std::filesystem::file_size(file_path_);
}
