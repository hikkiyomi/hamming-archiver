#include "lib/archiver/archiver.h"

#include <fstream>
#include <iostream>

int main() {
    Archiver driver("merged");

    // std::filesystem::path archive1("archive");
    // std::filesystem::path archive2("new_archive");

    // driver.Merge(archive1, archive2);

    // driver.ShowData();
    driver.Extract();

    return 0;
}
