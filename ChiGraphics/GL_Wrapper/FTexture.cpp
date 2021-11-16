#include "FTexture.h"
#include "ChiGraphics/Utilities.h"
#include "BindGuard.h"

namespace CHISTUDIO {

FTexture::FTexture() {
    Initialize(GetDefaultConfig());
}

FTexture::FTexture(const TextureConfig& config) {
    Initialize(config);
}

void FTexture::Initialize(const TextureConfig& config) {
    GL_CHECK(glGenTextures(1, &handle_));

    BindToUnit(0);

    TextureConfig final_config(GetDefaultConfig());
    // Override default config with config.
    for (auto& kv : config) {
        final_config[kv.first] = kv.second;
    }

    for (auto& kv : final_config) {
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, kv.first, kv.second));
    }
}

const TextureConfig& FTexture::GetDefaultConfig() {
    static TextureConfig config{
        {GL_TEXTURE_WRAP_S, GL_REPEAT},
        {GL_TEXTURE_WRAP_T, GL_REPEAT},
        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
    };

    return config;
}

FTexture::FTexture(FTexture&& other) noexcept {
    handle_ = other.handle_;
    other.handle_ = GLuint(-1);
}

FTexture& FTexture::operator=(FTexture&& other) noexcept {
    handle_ = other.handle_;
    other.handle_ = GLuint(-1);
    return *this;
}

FTexture::~FTexture() {
    if (handle_ != GLuint(-1))
        GL_CHECK(glDeleteTextures(1, &handle_));
}

void FTexture::BindToUnit(int id) const {
    // TODO: bind this texture to texture unit with index id.
    // The GLenum corresponding to the id-th unit is (GL_TEXTURE0 + id).
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + id));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, handle_));
}

void FTexture::UpdateImage(const FImage& image) {
    // Since we use GL_RGB as internal format and GL_UNSIGNED_BYTE as type,
    // we need to make the most general assumption about the data alignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    std::vector<uint8_t> buffer = image.ToByteData();
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)image.GetWidth(),
        (GLsizei)image.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
        buffer.data()));
}

void FTexture::Reserve(GLint internal_format,
    size_t width,
    size_t height,
    GLenum format,
    GLenum type) {
    // Here we call glTexImage2D to allocate a chunk of memory to write
    // the rendered image to as a texture. Pass nullptr as the data pointer (the
    // last parameter) to glTexImage2D since we don't know the actual image data
    // yet.
    Width = width;
    Height = height;
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, (GLsizei)width,
        (GLsizei)height, 0, format, type, nullptr));
}

static_assert(std::is_move_constructible<FTexture>(), "");
static_assert(std::is_move_assignable<FTexture>(), "");

static_assert(!std::is_copy_constructible<FTexture>(), "");
static_assert(!std::is_copy_assignable<FTexture>(), "");

}