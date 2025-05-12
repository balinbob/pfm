#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstring>
#include <algorithm>

struct StreamInfo {
    uint16_t minBlockSize;
    uint16_t maxBlockSize;
    uint32_t minFrameSize;   // 24 bits
    uint32_t maxFrameSize;   // 24 bits
    uint32_t sampleRate;     // 20 bits
    uint8_t  channels;       // 3 bits
    uint8_t  bitsPerSample;  // 5 bits
    uint64_t totalSamples;   // 36 bits
    uint8_t  md5[16];
};

uint32_t read24BitInt(std::ifstream& file) {
    uint8_t bytes[3];
    file.read(reinterpret_cast<char*>(bytes), 3);
    return (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
}

uint16_t read16BitInt(std::ifstream& file) {
    uint8_t bytes[2];
    file.read(reinterpret_cast<char*>(bytes), 2);
    return (bytes[0] << 8) | bytes[1];
}

StreamInfo readStreamInfoBlock(std::ifstream& file) {
    StreamInfo streamInfo;

    streamInfo.minBlockSize = read16BitInt(file);
    streamInfo.maxBlockSize = read16BitInt(file);
    streamInfo.minFrameSize = read24BitInt(file);
    streamInfo.maxFrameSize = read24BitInt(file);

    uint8_t packed[8];
    file.read(reinterpret_cast<char*>(packed), 8);

    streamInfo.sampleRate = ((packed[0] << 12) | (packed[1] << 4) | (packed[2] >> 4)) & 0xFFFFF;
    streamInfo.channels = ((packed[2] & 0x0E) >> 1) + 1;
    streamInfo.bitsPerSample = (((packed[2] & 0x01) << 4) | (packed[3] >> 4) + 1);

    streamInfo.totalSamples =
        (static_cast<uint64_t>(packed[3] & 0x0F) << 32) |
        (static_cast<uint64_t>(packed[4]) << 24) |
        (static_cast<uint64_t>(packed[5]) << 16) |
        (static_cast<uint64_t>(packed[6]) << 8) |
        (static_cast<uint64_t>(packed[7]));

    file.read(reinterpret_cast<char*>(streamInfo.md5), 16);

    return streamInfo;
}

void printStreamInfo(std::ifstream& file) {
    StreamInfo streamInfo = readStreamInfoBlock(file);

    std::cout << "Minimum Block Size: " << streamInfo.minBlockSize << "\n";
    std::cout << "Maximum Block Size: " << streamInfo.maxBlockSize << "\n";
    std::cout << "Minimum Frame Size: " << streamInfo.minFrameSize << "\n";
    std::cout << "Maximum Frame Size: " << streamInfo.maxFrameSize << "\n";
    std::cout << "Sample Rate: " << streamInfo.sampleRate << "\n";
    std::cout << "Channels: " << static_cast<int>(streamInfo.channels) << "\n";
    std::cout << "Bits per Sample: " << static_cast<int>(streamInfo.bitsPerSample) << "\n";
    std::cout << "Total Samples: " << streamInfo.totalSamples << "\n";

    return;
}
// This code reads the FLAC file format and extracts the StreamInfo metadata block.
// It uses bit manipulation to read the necessary fields and prints them to the console.