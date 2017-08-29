#ifndef LIBTHMBNLR_H
#define LIBTHMBNLR_H

#include "libthmbnlr_global.h"
#include <string>
#include <vector>
#include <cstdint>

class LIBTHMBNLR_EXPORT ThumbnailCreator
{
public:
    enum CreationOption {
        Quick,
        GoodQuality
    };

    struct Metadata {
        double m_Duration;
        int64_t m_BitRate;
        double m_FrameRate;
    };

public:
    ThumbnailCreator(const STD_STRING_TYPE &videoPath);

public:
    void setSeekPercentage(int value) { m_SeekPercentage = value; }
    void setCreationOption(CreationOption value) { m_CreationOption = value; }

public:
    // returns data in RGB24 format (8 bytes for each without alpha)
    bool createThumbnail(std::vector<uint8_t> &rgbBuffer, int &width, int &height);
    const Metadata &getMetadata() const { return m_Metadata; }

private:
    STD_STRING_TYPE m_FilePath;
    int m_SeekPercentage;
    CreationOption m_CreationOption;
    Metadata m_Metadata;
};

#endif // LIBTHMBNLR_H
