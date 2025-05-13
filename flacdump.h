#ifndef FLACDUMP_H
#define FLACDUMP_H

#include "streaminfo.h"
#include <vector>
#include <string>
#include <cstdint>

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

struct PictureData {
    uint8_t type;                    // Picture type code (e.g., 3 = front cover)
    std::string mimeType;           // MIME type like "image/png"
    std::vector<uint8_t> imageBytes; // Actual binary data of the image
};

uint32_t readLEUint32(const std::vector<uint8_t>& data, size_t offset);
std::string BlockTypeToString(BlockType type);
MetadataBlockHeader readMetadataBlockHeader(std::ifstream& file);
void parseFLACMetadata(const std::string& filename);
PictureData parsePictureBlock(std::ifstream& file, uint32_t blockLength);
int main(int argc, char* argv[]);

#endif