#pragma once
#define NOMINMAX

#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
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

    const glm::vec3& GetPixel(int x, int y) const;
    const std::vector<glm::vec3>& GetData() const { return Data; }
    void SetData(const std::vector<glm::vec3>& InData);
    static std::unique_ptr<FImage> LoadPNG(const std::string& filename, bool y_reversed);
    static std::unique_ptr<FImage> MakeImageCopy(FImage* InImageToCopy);
    void SavePNG(const std::string& filename) const;
    std::vector<uint8_t> ToByteData() const;
    std::vector<float> ToFloatData() const;
    void SetFloatData(const std::vector<float>& InData, bool InInvert = false);

    // Takes normal data in [-1, 1] range and remaps it in [0, 1] for visual display
    void RemapNormalData();

    glm::vec3 SampleHDRI(const glm::vec3& InDirection);
    glm::vec3 BilinearSample(float InX, float InY);
    glm::vec3 SampleWithUV(glm::vec2 InUV);
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
