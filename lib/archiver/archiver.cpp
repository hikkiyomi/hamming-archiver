#include "archiver.h"
#include "../filemaker/filemaker.h"

#include <cassert>
#include <fstream>
#include <iostream>

const uint64_t kFileSizeLimit = 1'073'741'824; // 1 GB

void NormalizeArchivePath(std::filesystem::path&);
char GetUserInput();
void WriteFileInfo(std::ofstream&, const HAFInfo&);
bool CheckOnAvailability(const std::filesystem::path&, const std::string&);
void ReadFileInfo(std::ifstream&, HAFInfo&);
std::string MakeName(const std::filesystem::path&, uint32_t);
void MakeCopy(std::string&);
std::string BeautifySize(uint64_t);
void PrintFileData(const HAFInfo&);
void WriteArchive(std::ofstream&, const std::filesystem::path&);

void NormalizeArchivePath(std::filesystem::path& archive_path) {
    if (!archive_path.has_extension()) {
        archive_path += ".haf";

        return;
    }

    if (archive_path.extension().string() != "haf") {
        std::cerr << archive_path.filename() << " is not an archive." << std::endl;

        exit(1);
    }
}

Archiver::Archiver(const std::filesystem::path& _archive_path) 
    : archive_path(_archive_path)
{
    NormalizeArchivePath(archive_path);
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
            return;
        }
    }

    std::ofstream{archive_path};
}

void WriteFileInfo(std::ofstream& stream, const HAFInfo& header) {
    stream.write(reinterpret_cast<const char*>(&header.file_name_length), sizeof(header.file_name_length));
    stream.write(static_cast<const char*>(header.file_name.data()), header.file_name_length);
    stream.write(reinterpret_cast<const char*>(&header.file_size), sizeof(header.file_size));
}

bool CheckOnAvailability(const std::filesystem::path& archive_path, const std::string& file_name) {
    std::ifstream stream(archive_path, std::ios::binary);

    while (stream.peek() != EOF) {
        HAFInfo current_file;

        ReadFileInfo(stream, current_file);

        if (current_file.file_name == file_name) {
            return false;
        }

        stream.seekg(current_file.file_size, std::ios::cur);
    }

    return true;
}

void Archiver::Append(const std::filesystem::path& file_path) {
    File appending_file(file_path);

    if (std::filesystem::is_directory(file_path)) {
        std::cerr << file_path << " is a directory. Archiving directories is not supported (yet)." << std::endl;

        exit(1);
    }

    HAFInfo info_header = appending_file.ExportIntoHAF();

    if (info_header.file_size > kFileSizeLimit) {
        std::cerr << "The size of the file " << info_header.file_name << " exceeds 1 GB. ";
        std::cerr << "File cannot be archived." << std::endl;

        exit(1);
    }

    if (!CheckOnAvailability(archive_path, info_header.file_name)) {
        std::cout << "Archive already contains file with name " << info_header.file_name << std::endl;
        std::cout << "Do you want to replace it? [y/n] ";

        if (GetUserInput() == 'n') {
            return;
        }

        Delete({info_header.file_name});
    }

    std::ofstream stream(archive_path, std::ios::binary | std::ios::app);

    WriteFileInfo(stream, info_header);

    uint64_t bytes_count = 0; // ! REMOVE THIS ON RELEASE
    std::ifstream file_stream(file_path, std::ios::binary);

    for (char byte; file_stream.get(byte); ++bytes_count) {
        stream.write(static_cast<const char*>(&byte), sizeof(byte));
    }

    assert(bytes_count == info_header.file_size);
}

void ReadFileInfo(std::ifstream& stream, HAFInfo& header) {
    stream.read(reinterpret_cast<char*>(&header.file_name_length), sizeof(header.file_name_length));

    char buffer[header.file_name_length + 1];

    stream.read(buffer, header.file_name_length);

    buffer[header.file_name_length] = '\0';
    header.file_name = std::string(buffer);

    stream.read(reinterpret_cast<char*>(&header.file_size), sizeof(header.file_size));
}

std::string MakeName(const std::filesystem::path& path, uint32_t copy_number) {
    return path.stem().string() + " (" + std::to_string(copy_number) + ")" + path.extension().string();
}

void MakeCopy(std::string& file_name) {
    std::filesystem::path actual_path(file_name);

    for (uint32_t copy_number = 1;; ++copy_number) {
        std::string temp_file_name = MakeName(actual_path, copy_number);

        if (!std::filesystem::exists(temp_file_name)) {
            file_name = temp_file_name;
            break;
        }
    }
}

void Archiver::Extract(const std::unordered_set<std::string>& files) {
    std::ifstream input_stream(archive_path, std::ios::binary);

    while (input_stream.peek() != EOF) {
        HAFInfo current_file;

        ReadFileInfo(input_stream, current_file);

        if (!files.empty() && files.find(current_file.file_name) == files.end()) {
            input_stream.seekg(current_file.file_size, std::ios::cur);

            continue;
        }

        if (std::filesystem::exists(current_file.file_name)) {
            std::cout << "File " << current_file.file_name << " already exists." << std::endl;
            std::cout << "Do you want to replace it? [y/n] ";

            if (GetUserInput() == 'n') {
                std::cout << "Do you want to create a copy? [y/n] ";

                if (GetUserInput() == 'n') {
                    continue;
                }

                MakeCopy(current_file.file_name);
                current_file.file_name_length = current_file.file_name.size();
            }
        }

        std::ofstream output_stream(current_file.file_name, std::ios::binary);

        for (size_t i = 0; i < current_file.file_size; ++i) {
            char byte;

            input_stream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
            output_stream.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        }
    }
}

std::string BeautifySize(uint64_t file_size) {
    const uint16_t kOneStep = 1024;
    size_t power = 0;

    while (file_size >= kOneStep) {
        file_size /= kOneStep;
        ++power;
    }

    std::string result = std::to_string(file_size) + " ";
    
    if (power == 1) {
        result += "K";
    } else if (power == 2) {
        result += "M";
    } else if (power == 3) {
        result += "G";
    }

    result += "B";

    return result;
}

void PrintFileData(const HAFInfo& header) {
    std::cout << header.file_name << " " << BeautifySize(header.file_size) << std::endl;
}

void Archiver::ShowData() {
    std::cout << "Archive " << archive_path << " contains:" << std::endl;

    std::ifstream input_stream(archive_path, std::ios::binary);
    uint32_t file_count = 0;
    uint64_t archive_size = 0;

    while (input_stream.peek() != EOF) {
        HAFInfo current_file;

        ReadFileInfo(input_stream, current_file);
        PrintFileData(current_file);

        ++file_count;
        archive_size += current_file.file_size;

        input_stream.seekg(current_file.file_size, std::ios::cur);
    }

    if (file_count == 0) {
        std::cout << "Nothing." << std::endl;

        return;
    }

    std::cout << std::endl;
    std::cout << "Amount of files: " << file_count << std::endl;
    std::cout << "Size archived: " << BeautifySize(archive_size) << std::endl;
}

void Archiver::Delete(const std::unordered_set<std::string>& files) {
    if (files.empty()) {
        std::cerr << "No files provided. See --help for more information." << std::endl;
        
        exit(1);
    }

    std::filesystem::path new_archive = std::filesystem::path(archive_path.stem().string() + ".tmp");
    std::ofstream output_stream(new_archive, std::ios::binary | std::ios::app);
    std::ifstream input_stream(archive_path, std::ios::binary);

    while (input_stream.peek() != EOF) {
        HAFInfo current_file;

        ReadFileInfo(input_stream, current_file);

        if (files.find(current_file.file_name) != files.end()) {
            input_stream.seekg(current_file.file_size, std::ios::cur);

            continue;
        }

        WriteFileInfo(output_stream, current_file);

        for (size_t bytes_read = 0; bytes_read < current_file.file_size; ++bytes_read) {
            char c;

            input_stream.get(c);
            output_stream.write(static_cast<const char*>(&c), sizeof(c));
        }
    }

    input_stream.close();
    output_stream.close();

    std::filesystem::remove(archive_path);
    std::filesystem::rename(new_archive, archive_path);
}

void WriteArchive(std::ofstream& output_stream, const std::filesystem::path& archive_path, std::unordered_set<std::string>& merged_files) {
    std::ifstream input_stream(archive_path, std::ios::binary);

    // for (char byte; input_stream.get(byte);) {
    //     output_stream.write(static_cast<const char*>(&byte), sizeof(byte));
    // }

    HAFInfo appending_file;

    ReadFileInfo(input_stream, appending_file);

    if (merged_files.find(appending_file.file_name) != merged_files.end()) {
        std::filesystem::path temp_path(appending_file.file_name);

        for (uint32_t copy_number = 1;; ++copy_number) {
            std::string temp_file_name = MakeName(temp_path, copy_number);

            if (merged_files.find(temp_file_name) == merged_files.end()) {
                appending_file.file_name = temp_file_name;
                appending_file.file_name_length = temp_file_name.size();
                
                break;
            }
        }
    }

    merged_files.insert(appending_file.file_name);

    WriteFileInfo(output_stream, appending_file);

    for (size_t bytes_read = 0; bytes_read < appending_file.file_size; ++bytes_read) {
        char c;

        input_stream.get(c);
        output_stream.put(c);
    }
}

void Merge(std::filesystem::path& archive_1, std::filesystem::path& archive_2, std::filesystem::path& merge_path) {
    NormalizeArchivePath(archive_1);
    NormalizeArchivePath(archive_2);
    
    if (merge_path.empty()) {
        std::cerr << "No merge archive provided. Merging was not done." << std::endl;

        exit(1);
    }

    NormalizeArchivePath(merge_path);

    std::ofstream output_stream(merge_path, std::ios::binary | std::ios::app);
    std::unordered_set<std::string> merged_files;

    WriteArchive(output_stream, archive_1, merged_files);
    WriteArchive(output_stream, archive_2, merged_files);
}
