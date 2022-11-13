#include "tools.h"

#include <iostream>
#include <vector>

const uint8_t kBitsInBytes = 8;

uint8_t MakeByte(const std::vector<uint8_t>& bits);
std::vector<size_t> GetRedundants(size_t index);
void Flip(uint8_t& bit);

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

Manipulator::Manipulator()
    : block_size(kBitsInBytes)
{}

void Manipulator::SaveBits(uint8_t byte) {
    for (size_t bit = 0; bit < kBitsInBytes; ++bit) {
        buffer_in.push_back((byte >> bit) & 1);
    }
}

uint8_t MakeByte(const std::vector<uint8_t>& bits) {
    uint8_t byte = 0;

    for (size_t i = 0; i < kBitsInBytes; ++i) {
        byte ^= (bits[i] << i);
    }

    return byte;
}

void Manipulator::WriteByte(std::ofstream& stream) {
    std::vector<uint8_t> bits(kBitsInBytes);

    for (size_t i = 0; i < kBitsInBytes && !buffer_out.empty(); ++i) {
        bits[i] = buffer_out.front();
        buffer_out.pop_front();
    }

    uint8_t byte = MakeByte(bits);

    stream.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
}

void Manipulator::WriteData(std::ofstream& stream) {
    WriteByte(stream);
    WriteByte(stream);
}

void Manipulator::LoadData(std::ofstream& stream, const char* byte_seq, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        SaveBits(byte_seq[i]);

        std::vector<uint8_t> data(block_size);

        for (size_t j = 0; j < block_size; ++j) {
            data[j] = buffer_in.front();
            buffer_in.pop_front();
        }

        std::vector<uint8_t> hamming_code = Encode(data);

        for (auto& bit: hamming_code) {
            buffer_out.push_back(bit);
        }

        WriteData(stream);
    }
}

std::vector<size_t> GetRedundants(size_t index) {
    std::vector<size_t> result;

    for (size_t bit_number = 0; bit_number < kBitsInBytes * sizeof(index); ++bit_number) {
        if ((index >> bit_number) & 1) {
            result.push_back(bit_number);
        }
    }

    return result;
}

void Flip(uint8_t& bit) {
    bit ^= 1;
}

std::vector<uint8_t> Manipulator::Encode(const std::vector<uint8_t>& data) {
    size_t redundants = 0;

    while ((1 << redundants) < data.size() + redundants + 1) {
        ++redundants;
    }

    size_t current_power = 1;
    size_t added_redundants = 0;
    uint8_t total_xor = 0;
    std::vector<uint8_t> hamming;

    for (size_t i = 0; i < data.size(); ++i) {
        while (hamming.size() == current_power - 1) {
            hamming.push_back(0);
            current_power <<= 1;
            ++added_redundants;
        }

        uint8_t bit = data[i];
        std::vector<size_t> correlating = GetRedundants(hamming.size() + 1);

        if (bit) {
            for (auto& redundant_number: correlating) {
                Flip(hamming[(1 << redundant_number) - 1]);
            }

            if (correlating.size() % 2 == 1) {
                Flip(total_xor);
            }
        }

        total_xor ^= bit;
        hamming.push_back(bit);
    }

    hamming.push_back(total_xor);

    return hamming;
}

std::vector<uint8_t> GetBits(std::ifstream& stream) {
    uint16_t bytes;
    std::vector<uint8_t> bits(2 * kBitsInBytes);

    stream.read(reinterpret_cast<char*>(&bytes), sizeof(bytes));

    for (size_t bit = 0; bit < 2 * kBitsInBytes; ++bit) {
        bits[bit] = (bytes >> bit) & 1;
    }

    return bits;
}

std::pair<size_t, size_t> GetDamagedIndex(const std::vector<uint8_t>& data) {
    size_t current_power = 1;
    uint8_t total_xor = 0;
    std::vector<uint8_t> reds;

    for (size_t i = 0; i < data.size() - 1; ++i) {
        if (i == current_power - 1) {
            reds.push_back(data[i]);
            total_xor ^= data[i];
            current_power <<= 1;

            continue;
        }

        uint8_t bit = data[i];
        std::vector<size_t> correlating = GetRedundants(i + 1);

        if (bit) {
            for (auto& redundant_number: correlating) {
                Flip(reds[redundant_number]);
            }
        }

        total_xor ^= bit;
    }

    size_t error_place = 0;

    for (size_t i = 0; i < reds.size(); ++i) {
        error_place ^= (reds[i] << i);
    }

    return {error_place, total_xor ^ data.back()};
}

std::vector<uint8_t> ExtractOriginalBits(const std::vector<uint8_t>& data) {
    size_t current_power = 1;
    std::vector<uint8_t> result;

    for (size_t i = 0; i < data.size() - 1; ++i) {
        if (i == current_power - 1) {
            current_power <<= 1;

            continue;
        }

        result.push_back(data[i]);
    }

    return result;
}

std::vector<uint8_t> Manipulator::Decode(std::vector<uint8_t>& data, bool restore) {
    auto [error_place, extra_bit] = GetDamagedIndex(data);

    if (error_place != 0) {
        if (!extra_bit) {
            std::cout << "Data is damaged and cannot be restored." << std::endl;
            std::cout << "Do you still want to extract it? (could be impossible) [y/n] ";

            if (GetUserInput() == 'n') {
                exit(0);
            }
        } else {
            if (restore) {
                Flip(data[error_place - 1]);
            }
        }
    }

    return ExtractOriginalBits(data);
}

void Manipulator::UnloadData(std::ifstream& stream, char* byte_seq, size_t length, bool restore) {
    for (size_t i = 0; i < length; ++i) {
        std::vector<uint8_t> encoded = GetBits(stream);

        while (encoded.size() > 13) {
            encoded.pop_back();
        }

        std::vector<uint8_t> decoded = Decode(encoded, restore);
        
        byte_seq[i] = MakeByte(decoded);
    }
}
