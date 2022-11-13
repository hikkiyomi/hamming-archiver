#pragma once

#include <cinttypes>
#include <filesystem>
#include <unordered_set>
#include <string>

class Parser {
public:
    Parser(int argc, char** argv);

    void Parse();
    void Run();
private:
    int argc_;
    char** argv_;
    uint8_t arguments_mask_;
    bool restore_;
    std::unordered_set<std::string> files_;
    std::filesystem::path archive_path_;
};
