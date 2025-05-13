#include "flacdump.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>  // for std::tolower
#include <filesystem>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Remove characters that are problematic in filenames
std::string sanitizeFilename(const std::string& name) {
    std::string clean;
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ' || c == '_' || c == '-') {
            clean += c;
        } else {
            clean += '_'; // Replace bad characters
        }
    }

    // Trim trailing/leading spaces
    size_t start = clean.find_first_not_of(' ');
    size_t end = clean.find_last_not_of(' ');
    return (start == std::string::npos) ? "image" : clean.substr(start, end - start + 1);
}

// Main function to create image filename
std::string generateImageFilename(const std::string& flacPath, uint8_t pictureType, int index, const std::string& rawMimeType) {
    std::filesystem::path base(flacPath);
    base.replace_extension(); // Remove .flac
    std::string stem = sanitizeFilename(base.filename().string());

    // Handle MIME types safely
    std::string mimeType = rawMimeType;
    size_t nullPos = mimeType.find('\0');
    if (nullPos != std::string::npos) {
        mimeType = mimeType.substr(0, nullPos);  // Trim at null character
    }

    // Map MIME type to file extension
    static const std::unordered_map<std::string, std::string> mimeToExt = {
        {"image/jpeg", "jpg"},
        {"image/png", "png"},
        {"image/gif", "gif"},
        {"image/bmp", "bmp"},
        {"image/tiff", "tiff"}
    };
    std::string extension = "img"; // Default fallback
    auto it = mimeToExt.find(mimeType);
    if (it != mimeToExt.end()) {
        extension = it->second;
    }

    // Compose final filename
    std::ostringstream filename;
    filename << stem << ' '
             << std::setw(2) << std::setfill('0') << static_cast<int>(pictureType)
             << std::setw(2) << std::setfill('0') << index
             << '.' << extension;

    return filename.str();
}

// This function simulates processing all picture blocks in the FLAC
void processPictures(const std::string& flacFilename, const std::vector<PictureData>& pictures) {
    std::vector<std::string> imageFilenames;

    for (size_t i = 0; i < pictures.size(); ++i) {
        const auto& pic = pictures[i];
        std::string filename = generateImageFilename(flacFilename, pic.type, static_cast<int>(i + 1), pic.mimeType);
        imageFilenames.push_back(filename);
    }

    // Print image filenames
    std::cout << "Images found in file:\n";
    for (const auto& name : imageFilenames) {
        std::cout << "  " << name << "\n";
    }

    // Ask user to save
    std::cout << "\nWould you like to save these images to disk? (y/n): ";
    char choice;
    std::cin >> choice;
    choice = std::tolower(choice);

    if (choice == 'y') {
        for (size_t i = 0; i < pictures.size(); ++i) {
            std::ofstream out(imageFilenames[i], std::ios::binary);
            out.write(reinterpret_cast<const char*>(pictures[i].imageBytes.data()), pictures[i].imageBytes.size());
            std::cout << "Saved: " << imageFilenames[i] << "\n";
        }
    } else {
        std::cout << "No files were saved.\n";
    }
}
