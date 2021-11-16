#include "FImage.h"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

static uint8_t ClampColor(float c) {
    int tmp = int(c * 255);
    if (tmp < 0)
        tmp = 0;
    if (tmp > 255)
        tmp = 255;

    return static_cast<uint8_t>(tmp);
}

std::unique_ptr<FImage> FImage::LoadPNG(const std::string& filename, bool y_reversed)
{
    int w, h, n;
    uint8_t* buffer = stbi_load(filename.c_str(), &w, &h, &n, 0);
    if (buffer == nullptr) {
        throw std::runtime_error("Cannot load " + filename + "!");
    }
    if (n != 3) {
        throw std::runtime_error("Wrong number of channels in " + filename + "!");
    }
    auto image = make_unique<FImage>(w, h);

    int dy = y_reversed ? -1 : 1;
    int y_start = y_reversed ? h - 1 : 0;
    int y_end = y_reversed ? -1 : h;
    for (int c = 0, p = 0, y = y_start; y != y_end; y += dy) {
        for (int x = 0; x < w; x++, p++) {
            glm::vec3& pixel = image->Data[p];
            for (int t = 0; t < 3; t++)
                pixel[t] = static_cast<float>(buffer[c++]) / 255.0f;
        }
    }
    stbi_image_free(buffer);
    return image;
}

void FImage::SavePNG(const std::string& filename) const
{
    auto buffer = ToByteData();
    stbi_write_png(filename.c_str(), (int)Width, (int)Height, 3, buffer.data(), (int)Width * 3);
}

std::vector<uint8_t> FImage::ToByteData() const
{
    std::vector<uint8_t> buffer;

    for (int y = (int)Height - 1; y >= 0; y--)
        for (size_t x = 0; x < Width; x++) {
            const glm::vec3& color = GetPixel(x, static_cast<size_t>(y));
            buffer.push_back(ClampColor(color[0]));
            buffer.push_back(ClampColor(color[1]));
            buffer.push_back(ClampColor(color[2]));
        }

    return buffer;
}

std::vector<float> FImage::ToFloatData() const
{
    std::vector<float> buffer;

    for (int y = (int)Height - 1; y >= 0; y--)
        for (size_t x = 0; x < Width; x++) {
            const glm::vec3& color = GetPixel(x, static_cast<size_t>(y));
            buffer.push_back(color[0]);
            buffer.push_back(color[1]);
            buffer.push_back(color[2]);
        }

    return buffer;
}

}