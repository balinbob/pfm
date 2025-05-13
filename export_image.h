#ifndef EXPORT_IMAGE_H
#define EXPORT_IMAGE_H

std::string sanitizeFilename(const std::string& name);
std::string generateImageFilename(const std::string& flacPath, uint8_t pictureType, int index, const std::string& mimeType);
void processPictures(const std::string& flacFilename, const std::vector<PictureData>& pictures);



#endif