#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <math.h>

namespace CHISTUDIO {

class FImage {
public:
    FImage(size_t InWidth, size_t InHeight) {
        Width = InWidth;
        Height = InHeight;
        Data.resize(Width * Height);
    }

    size_t GetWidth() const {
        return Width;
    }

    size_t GetHeight() const {
        return Height;
    }

    void SetPixel(size_t x, size_t y, const glm::vec3& color) {
        if (x < Width && y < Height) {
            Data[y * Width + x] = color;
        }
        else {
            throw std::runtime_error("Unable to set a pixel outside of image range.");
        }
    }

    const glm::vec3& GetPixel(size_t x, size_t y) const {
        return Data[glm::min(y, Height - 1) * Width + glm::min(x, Width - 1)];

        /*if (x < Width && y < Height) {
            return Data[y * Width + x];
        }
        else {
            std::cout << "(" << x << "," << y << ")" << std::endl;
            throw std::runtime_error("Unable to get a pixel outside of image range.");
        }*/
    }

    static std::unique_ptr<FImage> LoadPNG(const std::string& filename, bool y_reversed);
    void SavePNG(const std::string& filename) const;
    std::vector<uint8_t> ToByteData() const;
    std::vector<float> ToFloatData() const;

    glm::vec3 SampleHDRI(const glm::vec3& InDirection);
    glm::vec3 BilinearSample(float InX, float InY);
    std::string ImportedFileName;
private:
    std::vector<glm::vec3> Data;
    size_t Width;
    size_t Height;
};

}
