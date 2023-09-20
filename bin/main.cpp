#include "lib/parser/parser.h"

#include <fstream>
#include <iostream>

// void Damage() {
//     std::vector<uint8_t> bytes;
//     std::ifstream input_stream("archive.haf", std::ios::binary);

//     for (char byte; input_stream.get(byte);) {
//         bytes.push_back(byte);
//     }

//     // bytes[bytes.size() - 2] = 86; // Data is damaged but it can be restored.
//     // bytes[bytes.size() - 2] = 87; // Data is damaged and cannot be restored.

//     std::ofstream output_stream("archive.haf", std::ios::binary);

//     for (char byte: bytes) {
//         output_stream.put(byte);
//     }
// }

int main(int argc, char** argv) {
    Parser parser(argc, argv);
    
    parser.Parse();
    parser.Run();

    return 0;
}
