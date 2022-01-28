#pragma once
#define NOMINMAX

#include <unordered_map>

#include "ChiGraphics/External.h"
#include "ChiGraphics/Textures/FImage.h"

namespace CHISTUDIO {

using TextureConfig = std::unordered_map<GLenum, GLint>;

class FTexture {
public:
    FTexture();
    FTexture(const TextureConfig& config);
    ~FTexture();

    FTexture(const FTexture&) = delete;
    FTexture& operator=(const FTexture&) = delete;

    // Allow both move-construct and move-assign.
    FTexture(FTexture&& other) noexcept;
    FTexture& operator=(FTexture&& other) noexcept;

    // Bind the current texture to a texture unit ("id")
    void BindToUnit(int id) const;
    // Update the texture contents with "image"
    void UpdateImage(const FImage& image);
    // Allocate space for the texture without storing the data
    void Reserve(GLint internal_format,
        size_t width,
        size_t height,
        GLenum format,
        GLenum type);

    GLuint GetHandle() const {
        return handle_;
    }
    
    
    size_t Width;
    size_t Height;

private:
    void Initialize(const TextureConfig& config);
    static const TextureConfig& GetDefaultConfig();

    GLuint handle_{ GLuint(-1) };

    
};

}
