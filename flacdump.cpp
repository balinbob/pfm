#include "streaminfo.h"
#include "flacdump.h"
#include "export_image.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>


uint32_t readLEUint32(const std::vector<uint8_t>& data, size_t offset) {
    return data[offset] |
          (data[offset + 1] << 8) |
          (data[offset + 2] << 16) |
          (data[offset + 3] << 24);
}

std::string blockTypeToString(BlockType type) {
    switch (type) {
        case BlockType::STREAMINFO: return "STREAMINFO";
        case BlockType::PADDING: return "PADDING";
        case BlockType::APPLICATION: return "APPLICATION";
        case BlockType::SEEKTABLE: return "SEEKTABLE";
        case BlockType::VORBIS_COMMENT: return "VORBIS_COMMENT";
        case BlockType::CUESHEET: return "CUESHEET";
        case BlockType::PICTURE: return "PICTURE";
        default: return "UNKNOWN";
    }
}

PictureData parsePictureBlock(std::ifstream& file, uint32_t blockLength) {
    PictureData pic;

    // Helper lambda to read big-endian 32-bit integers
    auto readUint32 = [&](void) {
        uint8_t bytes[4];
        file.read(reinterpret_cast<char*>(bytes), 4);
        return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    };

    // Picture type
    pic.type = static_cast<uint8_t>(readUint32());

    // MIME type
    uint32_t mimeLength = readUint32();
    std::string mimeType(mimeLength, '\0');
    file.read(&mimeType[0], mimeLength);
    pic.mimeType = mimeType;

    // Description (skip)
    uint32_t descLength = readUint32();
    file.seekg(descLength, std::ios::cur);

    // Skip width, height, color depth, and colors used (4 * 4 bytes)
    file.seekg(16, std::ios::cur);

    // Image data
    uint32_t dataLength = readUint32();
    std::vector<uint8_t> imageData(dataLength);
    file.read(reinterpret_cast<char*>(imageData.data()), dataLength);
    pic.imageBytes = std::move(imageData);

    return pic;
}

MetadataBlockHeader readMetadataBlockHeader(std::ifstream& file) {
    uint8_t headerByte;
    file.read(reinterpret_cast<char*>(&headerByte), 1);

    MetadataBlockHeader header;
    header.isLastBlock = headerByte & 0x80;
    header.blockType = static_cast<BlockType>(headerByte & 0x7F);
    header.length = read24BitInt(file);
    return header;
}

void parseFLACMetadata(const std::string& filename) {
    std::vector<PictureData> pictures;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file.\n";
        return;
    }

    // Read and verify the FLAC signature
    char signature[4];
    file.read(signature, 4);
    if (std::string(signature, 4) != "fLaC") {
        std::cerr << "Not a valid FLAC file.\n";
        return;
    }

    // Read metadata blocks
    bool lastBlock = false;
    int blockOffset = 4;
    std::pair <int, int> imageInfo = std::make_pair(0, 0);
    while (!lastBlock && file) {
        std::cout << "Block offset: " << blockOffset << '\t'; // << "\n";
        file.seekg(blockOffset, std::ios::beg);
        MetadataBlockHeader header = readMetadataBlockHeader(file);
        std::cout << "Block Type: " << static_cast<int>(header.blockType)
        << ", Length: " << header.length << "\t"
        << blockTypeToString(static_cast<BlockType>(header.blockType)) << "\n";

        if (header.blockType == BlockType::STREAMINFO) {
            printStreamInfo(file);
        }

        if (header.blockType == BlockType::PICTURE) {
            pictures.push_back(parsePictureBlock(file, header.length));
        }
        
        if (header.blockType == BlockType::VORBIS_COMMENT) {
            std::vector<uint8_t> vcData(header.length);
            file.read(reinterpret_cast<char*>(vcData.data()), header.length);
            uint32_t vendorLength = readLEUint32(vcData, 0);
            std::string vendorString(vcData.begin() + 4, vcData.begin() + 4 + vendorLength);
            size_t offset = 4 + vendorLength;
            std::cout << "Vendor Length: " << vendorLength << "\n";
            std::cout << "Vendor String: " << vendorString << "\n";
            uint32_t numComments = readLEUint32(vcData, offset);
            offset += 4;
            std::cout << "Number of comments: " << numComments << '\n';
            for (int i = 0; i < numComments; ++i) {
                uint32_t commentLength = readLEUint32(vcData, offset);
                std::string comment(vcData.begin() + offset + 4, vcData.begin() + offset + 4 + commentLength);
                std::cout << comment + "\n";
                offset += 4 + commentLength;
            }
        }
        blockOffset += 4 + header.length;
        lastBlock = header.isLastBlock;
    }
    file.close();
    if (pictures.size() > 0) {
        std::cout << "Found " << pictures.size() << " picture blocks.\n";
        processPictures(filename, pictures);
    }
}

int main(int argc, char* argv[]) {
    int arg = 0;
    std::string filename = "";
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <FLAC file>\n";
        return 1;
    }

    filename = argv[1];    
    parseFLACMetadata(filename);
    return 0;
}
// This code reads the metadata blocks from a FLAC file and prints their types and lengths.
// It uses the C++17 standard for file I/O and string manipulation.