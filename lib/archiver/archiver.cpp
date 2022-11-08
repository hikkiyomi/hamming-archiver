#include "archiver.h"
#include "../filemaker/filemaker.h"

#include <cassert>
#include <fstream>
#include <iostream>

Archiver::Archiver(std::filesystem::path _archive_path) 
    : archive_path(_archive_path)
{
    archive_path += ".haf";
}

char GetUserInput() {
    char answer;
        
    do {
        std::cin >> answer;

        if (answer != 'y' && answer != 'n') {
            std::cout << "Please, use 'y' or 'n'. ";
        }
    } while (answer != 'y' && answer != 'n');

    return answer;
}

void Archiver::Create() {
    if (std::filesystem::exists(archive_path)) {
        std::cout << "Archive " << archive_path.filename() << " already exists." << std::endl;
        std::cout << "Do you want to replace it? [y/n] ";

        if (GetUserInput() == 'n') {
            exit(0);
        }
    }

    std::ofstream{archive_path};
}

void WriteFileInfo(std::ofstream& stream, const HAFInfo& header) {
    stream.write(reinterpret_cast<const char*>(&header.file_name_length), sizeof(header.file_name_length));
    stream.write(static_cast<const char*>(header.file_name.data()), header.file_name_length);
    stream.write(reinterpret_cast<const char*>(&header.file_size), sizeof(header.file_size));
}

void Archiver::Append(std::filesystem::path file_path) {
    File appending_file(file_path);

    if (std::filesystem::is_directory(file_path)) {
        std::cout << file_path << " is a directory. Archiving directories is not supported." << std::endl;

        return;
    }

    HAFInfo info_header = appending_file.ExportIntoHAF();

    std::ofstream stream(archive_path, std::ios::binary | std::ios::app);

    WriteFileInfo(stream, info_header);

    uint64_t bytes_count = 0; // ! REMOVE THIS ON RELEASE
    std::ifstream file_stream(file_path, std::ios::binary);

    for (char byte; file_stream.get(byte); ++bytes_count) {
        stream.write(static_cast<const char*>(&byte), sizeof(byte));
    }

    assert(bytes_count == info_header.file_size);
}

void ReadFileInformation(std::ifstream& stream, HAFInfo& header) {
    stream.read(reinterpret_cast<char*>(&header.file_name_length), sizeof(header.file_name_length));

    char buffer[header.file_name_length + 1];

    stream.read(buffer, header.file_name_length);

    buffer[header.file_name_length] = '\0';
    header.file_name = std::string(buffer);

    stream.read(reinterpret_cast<char*>(&header.file_size), sizeof(header.file_size));
}

void Archiver::Extract() {
    std::ifstream stream(archive_path, std::ios::binary);

    while (stream.peek() != EOF) {
        HAFInfo current_file;

        ReadFileInformation(stream, current_file);

        if (std::filesystem::exists(current_file.file_name)) {
            std::cout << "File " << current_file.file_name << " already exists." << std::endl;
            std::cout << "Do you want to replace it? [y/n] "; // Copying files may be added.

            if (GetUserInput() == 'n') {
                continue;
            }
        }

        std::ofstream output_stream(current_file.file_name, std::ios::binary);

        for (size_t i = 0; i < current_file.file_size; ++i) {
            char byte;

            stream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
            output_stream.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        }
    }
}

void Archiver::ShowData() {
    
}
