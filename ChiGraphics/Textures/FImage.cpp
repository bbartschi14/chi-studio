#include "FImage.h"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "ChiGraphics/Utilities.h"
#include <glm/gtx/compatibility.hpp>
#include <iostream>

namespace CHISTUDIO {

static uint8_t ClampColor(float c) {
    int tmp = int(c * 255);
    if (tmp < 0)
        tmp = 0;
    if (tmp > 255)
        tmp = 255;

    return static_cast<uint8_t>(tmp);
}

void FImage::SetData(const std::vector<glm::vec3>& InData)
{
    Data = InData;
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
    image->ImportedFileName = filename;
    return image;
}

std::unique_ptr<FImage> FImage::MakeImageCopy(FImage* InImageToCopy)
{
    auto image = make_unique<FImage>(InImageToCopy->GetWidth(), InImageToCopy->GetHeight());

    image->SetData(InImageToCopy->GetData());

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

glm::vec3 FImage::SampleHDRI(const glm::vec3& InDirection)
{
    glm::vec3 direction = glm::normalize(InDirection);
    float azimuth = atan2(direction.z, direction.x) + kPi;
    float polar = acos(direction.y);
    float x = azimuth / (kPi * 2.0f) * (Width - 1);
    float y = polar / kPi * (Height - 1);
    return BilinearSample(x,y);
}

glm::vec3 FImage::BilinearSample(float InX, float InY)
{
    int x0 = glm::min((int)InX, (int)Width - 1); // (x as u32).min(self.width - 1);
    int y0 = glm::min((int)InY, (int)Height - 1);
    float ax = InX - x0;
    float ay = InY - y0;

    return glm::lerp(glm::lerp(GetPixel(x0, y0), GetPixel(x0 + 1, y0), ax), glm::lerp(GetPixel(x0, y0 + 1), GetPixel(x0 + 1, y0 + 1), ax), ay);
}

void FImage::ApplyGaussianBlur(int InNumIterations)
{
    float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    bool horizontal = true;

    for (int iteration = 0; iteration < InNumIterations * 2; iteration++)
    {
        std::vector<glm::vec3> NewData;
        //if (horizontal)
        //{
        //    std::cout << "Horizontal" << std::endl;
        //}
        //else
        //{
        //    std::cout << "Vertical" << std::endl;
        //}

        for (int y = 0; y < GetHeight(); y++)
        {
            for (int x = 0; x < GetWidth(); x++)
            {
                glm::vec3 result = GetPixel(x, y) * weight[0];
                
                if (horizontal)
                {
                    for (int i = 1; i < 5; ++i)
                    {
                        result += GetPixel(x + i, y) * weight[i];
                        result += GetPixel(x - i, y) * weight[i];
                    }
                }
                else
                {
                    for (int i = 1; i < 5; ++i)
                    {
                        result += GetPixel(x, y + i) * weight[i];
                        result += GetPixel(x, y - i) * weight[i];
                    }
                }
                
                NewData.push_back(result);
            }
        }
        horizontal = !horizontal;
        Data = NewData;
    }
    
}

void FImage::MaskPixels(std::function<bool(glm::vec3)> InFunction)
{
    for (int y = 0; y < GetHeight(); y++)
    {
        for (int x = 0; x < GetWidth(); x++)
        {
            bool result = InFunction(GetPixel(x, y));
            if (!result)
            {
                SetPixel(x, y, glm::vec3(0.0f));
            }
        }
    }
}

void FImage::AdditiveBlend(FImage* InImageToBlend)
{
    const float exposure = 1.0f;
    for (int y = 0; y < GetHeight(); y++)
    {
        for (int x = 0; x < GetWidth(); x++)
        {
            glm::vec3 result = glm::min(GetPixel(x, y), 1.0f) + InImageToBlend->GetPixel(x, y);
            //result = glm::vec3(1.0f) - glm::exp(-result * exposure); // tone mapping
            //result = glm::pow(result, glm::vec3(1.0f / 2.2f));

            SetPixel(x, y, result);
        }
    }
}

}