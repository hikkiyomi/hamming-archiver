#include "filemaker.h"

#include <iostream>

File::File(const char* _file_path)
    : file_path(_file_path)
{
    name = file_path.filename().string();
    size = std::filesystem::file_size(file_path);
}

void File::Show() { // FOR DEBUGGING
    std::cout << "Absolute path: " << std::filesystem::absolute(file_path) << std::endl;
    std::cout << "Name of file: " << name << std::endl;
    std::cout << "File size: " << size << " bytes" << std::endl;
}

std::filesystem::path File::GetPath() {
    return std::filesystem::absolute(file_path);
}
