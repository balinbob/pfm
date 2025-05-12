#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

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

uint32_t read24BitInt(std::ifstream& file);
uint16_t read16BitInt(std::ifstream& file);
StreamInfo readStreamInfoBlock(std::ifstream& file);
void printStreamInfo(std::ifstream& file);