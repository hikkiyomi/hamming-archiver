#include "lib/archiver/archiver.h"
#include "lib/filemaker/filemaker.h"

#include <fstream>
#include <iostream>

int main() {
    Archiver driver("test_archive");
    driver.Create();
    driver.Append("test.txt");

    return 0;
}
