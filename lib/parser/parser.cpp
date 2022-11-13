#include "../archiver/archiver.h"
#include "parser.h"

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

// --------------------CONSOLE COMMAND BITMASKS--------------------

const uint8_t kCreateCommandMask = (1 << 1);
const uint8_t kListCommandMask = (1 << 2);
const uint8_t kExtractCommandMask = (1 << 3);
const uint8_t kAppendCommandMask = (1 << 4);
const uint8_t kDeleteCommandMask = (1 << 5);
const uint8_t kMergeCommandMask = (1 << 6);

// ----------------------------------------------------------------

void PrintHelpList();
std::vector<std::string> ParseMonoOption(char* arg);
void PrintUnknownArgumentInformation(std::string arg);

Parser::Parser(int argc, char** argv)
    : argc_(argc)
    , argv_(argv)
    , arguments_mask_(0)
    , restore_(true)
{}

void PrintHelpList() {
    std::cout << "These arguments should be provided: " << std::endl;

    std::cout << "-c (--create) - create new archive" << std::endl;
    std::cout << "-f (--file)=[ARCHIVE_NAME] - name of a certain archive" << std::endl;
    std::cout << "-l (--list) - print the content of archive" << std::endl;
    std::cout << "-x (--extract) - extract one or few provided files" << std::endl;
    std::cout << "-a (--append) - add the file to an archive" << std::endl;
    std::cout << "-d (--delete) - delete the file from an archive" << std::endl;
    std::cout << "-A (--concatenate) - merge two archives" << std::endl;

    std::cout << std::endl;

    std::cout << "--no-restore - goes with -x (--extract), does not restore damaged files" << std::endl;

    std::cout << std::endl;
}

std::vector<std::string> ParseMonoOption(char* arg) {
    std::vector<std::string> result;
    std::string buff;
    bool equal_sign_seen = false;

    for (size_t i = 0; arg[i] != '\0'; ++i) {
        if (arg[i] == '=' && !equal_sign_seen) {
            result.emplace_back(buff);
            buff.clear();

            equal_sign_seen = true;
        } else {
            buff += std::string(1, arg[i]);
        }
    }

    if (!buff.empty()) {
        result.emplace_back(buff);
    }

    if (result.size() != 2) {
        throw std::runtime_error("Incorrect arguments. Try --help for more information.");
    }

    return result;
}

void PrintUnknownArgumentInformation(std::string arg) {
    std::cerr << "Unknown argument " << arg << std::endl;
    std::cerr << "Try --help for more information." << std::endl;
}

bool CheckOnCorrectness(const uint8_t mask) {
    // Exactly one command has to be provided.
    return __builtin_popcount(mask) == 1;
}

void Parser::Parse() {
    for (int i = 1; i < argc_;) {
        if (strcmp(argv_[i], "--help") == 0) {
            PrintHelpList();

            exit(0);
        }

        if (argv_[i][0] != '-') {
            files_.insert(argv_[i]);
            ++i;
            
            continue;
        }

        if (strlen(argv_[i]) == 1) {
            PrintUnknownArgumentInformation(argv_[i]);

            exit(1);
        }

        if (argv_[i][1] == 'f' || (argv_[i][1] == '-' && argv_[i][2] == 'f')) {
            std::vector<std::string> params;

            if (i == argc_ - 1 || argv_[i + 1][0] == '-') {
                params = ParseMonoOption(argv_[i]);
                ++i;
            } else {
                params = {argv_[i], argv_[i + 1]};
                i += 2;
            }

            if (params[0] != "-f" && params[0] != "--file") {
                PrintUnknownArgumentInformation(params[0]);

                exit(1);
            }

            archive_path_ = params[1];

            continue;
        }

        if (strcmp(argv_[i], "-c") == 0 || strcmp(argv_[i], "--create") == 0) {
            arguments_mask_ |= kCreateCommandMask;
        } else if (strcmp(argv_[i], "-l") == 0 || strcmp(argv_[i], "--list") == 0) {
            arguments_mask_ |= kListCommandMask;
        } else if (strcmp(argv_[i], "-x") == 0 || strcmp(argv_[i], "--extract") == 0) {
            arguments_mask_ |= kExtractCommandMask;
        } else if (strcmp(argv_[i], "-a") == 0 || strcmp(argv_[i], "--append") == 0) {
            arguments_mask_ |= kAppendCommandMask;
        } else if (strcmp(argv_[i], "-d") == 0 || strcmp(argv_[i], "--delete") == 0) {
            arguments_mask_ |= kDeleteCommandMask;
        } else if (strcmp(argv_[i], "-A") == 0 || strcmp(argv_[i], "--concatenate") == 0) {
            arguments_mask_ |= kMergeCommandMask;
        } else if (strcmp(argv_[i], "--no-restore") == 0) {
            restore_ = false;
        } else {
            PrintUnknownArgumentInformation(argv_[i]);
            
            exit(1);
        }

        ++i;
    }

    if (!CheckOnCorrectness(arguments_mask_)) {
        std::cerr << "Using less/more than one command is restricted." << std::endl;
        std::cerr << "Try --help for more information." << std::endl;

        exit(1);
    }
}

void Parser::Run() {
    if (archive_path_.empty()) {
        std::cerr << "No archive name was provided." << std::endl;
        
        exit(1);
    }

    Archiver driver(archive_path_, restore_);

    if (arguments_mask_ == kCreateCommandMask) {
        driver.Create();
    } else if (arguments_mask_ == kListCommandMask) {
        driver.ShowData();
    } else if (arguments_mask_ == kExtractCommandMask) {
        driver.Extract(files_);
    } else if (arguments_mask_ == kAppendCommandMask) {
        if (files_.size() > 1) {
            std::cerr << "Only one file can be appended!" << std::endl;

            exit(1);
        }

        driver.Append(*files_.begin());
    } else if (arguments_mask_ == kDeleteCommandMask) {
        driver.Delete(files_);
    } else if (arguments_mask_ == kMergeCommandMask) {
        if (files_.size() > 2) {
            std::cerr << "More than two archives provided!" << std::endl;

            exit(1);
        }

        std::filesystem::path archive1 = *files_.begin();
        files_.erase(files_.begin());
        std::filesystem::path archive2 = *files_.begin();

        driver.Merge(archive1, archive2);
    } else {
        throw std::runtime_error("An error occured while running parser!");
    }
}
