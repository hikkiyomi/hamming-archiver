#include "archiver.h"
#include "../filemaker/filemaker.h"

#include <fstream>
#include <iostream>

void Archiver::Create() {
    std::filesystem::path archive_name = archive_path;
    archive_name += ".haf";

    if (std::filesystem::exists(archive_name)) {
        std::cout << "Archive " << archive_name.filename() << " already exists." << std::endl;
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

    std::ofstream{archive_name};
}

void Archiver::Append(std::filesystem::path file_path) {

}

Archiver::Archiver(std::filesystem::path archive_name) 
    : archive_path(archive_name)
{}
