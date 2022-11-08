#include "lib/archiver/archiver.h"
#include "lib/filemaker/filemaker.h"

#include <fstream>
#include <iostream>

int main() {
    Archiver driver("merged");
    
    // driver.Create();
    // driver.Append("test.txt");

    std::filesystem::path archive_1 = "archive";
    std::filesystem::path archive_2 = "new_archive";
    std::filesystem::path merge_path = "merged";

    Merge(archive_1, archive_2, merge_path); // ! FIX COLLISIONS

    driver.ShowData();

    return 0;
}
