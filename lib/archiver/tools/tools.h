#pragma once

#include <cinttypes>
#include <deque>
#include <fstream>
#include <utility>
#include <vector>

class Manipulator {
public:
    Manipulator();

    void LoadData(std::ofstream& stream, const char* byte_seq, size_t length);
    void UnloadData(std::ifstream& stream, char* byte_seq, size_t length, bool restore);
private:
    size_t block_size_;
    std::deque<uint8_t> buffer_in_;
    std::deque<uint8_t> buffer_out_;

    void WriteByte(std::ofstream& stream);
    void WriteData(std::ofstream& stream);
    void SaveBits(uint8_t byte);
    void FinalCheck(std::ofstream& stream);

    std::vector<uint8_t> Encode(const std::vector<uint8_t>& data);
    std::vector<uint8_t> Decode(std::vector<uint8_t>& data, bool restore);
};

char GetUserInput();
