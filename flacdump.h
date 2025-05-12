#ifndef FLACDUMP_H
#define FLACDUMP_H

#include "streaminfo.h"
#include <vector>

enum class BlockType : uint8_t {
    STREAMINFO = 0,
    PADDING = 1,
    APPLICATION = 2,
    SEEKTABLE = 3,
    VORBIS_COMMENT = 4,
    CUESHEET = 5,
    PICTURE = 6,
    INVALID = 127  // Just in case
};

struct MetadataBlockHeader {
    bool isLastBlock;
    uint8_t blockType;
    uint32_t length;
};

struct VorbisComment {
    uint32_t vendorLength;
    std::string vendorString;
    uint32_t numComments;
    std::vector<std::pair<std::string, std::string>> comments; // key-value pairs
};

uint32_t readLEUint32(const std::vector<uint8_t>& data, size_t offset);
std::string BlockTypeToString(BlockType type);
MetadataBlockHeader readMetadataBlockHeader(std::ifstream& file);
void parseFLACMetadata(const std::string& filename);
int main(int argc, char* argv[]);

#endif