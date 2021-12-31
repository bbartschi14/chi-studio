#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <math.h>
#include <functional>

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

    const glm::vec3& GetPixel(int x, int y) const {
        if (x < 0)
        {
            x = -x - 1;
        }
        else if (x >= Width)
        {
            x = 2 * Width - x - 1;
        }

        if (y < 0)
        {
            y = -y - 1;
        }
        else if (y >= Height)
        {
            y = 2 * Height - y - 1;
        }

        return Data[glm::max((int)glm::min(y, (int)Height - 1), 0) * Width + glm::max((int)glm::min(x, (int)Width - 1), 0)];

        /*if (x < Width && y < Height) {
            return Data[y * Width + x];
        }
        else {
            std::cout << "(" << x << "," << y << ")" << std::endl;
            throw std::runtime_error("Unable to get a pixel outside of image range.");
        }*/
    }
    const std::vector<glm::vec3>& GetData() const { return Data; }
    void SetData(const std::vector<glm::vec3>& InData);
    static std::unique_ptr<FImage> LoadPNG(const std::string& filename, bool y_reversed);
    static std::unique_ptr<FImage> MakeImageCopy(FImage* InImageToCopy);
    void SavePNG(const std::string& filename) const;
    std::vector<uint8_t> ToByteData() const;
    std::vector<float> ToFloatData() const;

    glm::vec3 SampleHDRI(const glm::vec3& InDirection);
    glm::vec3 BilinearSample(float InX, float InY);
    std::string ImportedFileName;

    // Apply a gaussian blur with a number of iterations
    void ApplyGaussianBlur(int InNumIterations);

    // Set pixels to zero that return false from the given lambda function
    void MaskPixels(std::function<bool(glm::vec3)> InFunction);

    void AdditiveBlend(FImage* InImageToBlend);
private:
    std::vector<glm::vec3> Data;
    size_t Width;
    size_t Height;
};

}
