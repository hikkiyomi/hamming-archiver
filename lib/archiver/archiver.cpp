#include "archiver.h"
#include "../filemaker/filemaker.h"

#include <cassert>
#include <fstream>
#include <iostream>

void Archiver::Create() {
    archive_path += ".haf";

    if (std::filesystem::exists(archive_path)) {
        std::cout << "Archive " << archive_path.filename() << " already exists." << std::endl;
        std::cout << "Do you want to replace it? [y/n] ";

        char answer;
        
        do {
            std::cin >> answer;

            if (answer != 'y' && answer != 'n') {
                std::cout << "Please, use 'y' or 'n'. ";
            }
        } while (answer != 'y' && answer != 'n');

        if (answer == 'n') {
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
    HAFInfo info_header = appending_file.ExportIntoHAF();

    std::ofstream stream(archive_path, std::ios::binary | std::ios::app);

    WriteFileInfo(stream, info_header);

    uint64_t bytes_count = 0;
    std::ifstream file_stream(file_path, std::ios::binary);

    for (char byte; file_stream.get(byte); ++bytes_count) {
        stream.write(static_cast<const char*>(&byte), sizeof(byte));
    }

    assert(bytes_count == info_header.file_size);
}

Archiver::Archiver(std::filesystem::path archive_path) 
    : archive_path(archive_path)
{}
