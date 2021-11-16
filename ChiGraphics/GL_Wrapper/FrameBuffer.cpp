#include "FrameBuffer.h"

#include <stdexcept>

#include "BindGuard.h"
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

Framebuffer::Framebuffer() {
    GL_CHECK(glGenFramebuffers(1, &handle_));
}

Framebuffer::~Framebuffer() {
    if (handle_ != GLuint(-1))
        GL_CHECK(glDeleteFramebuffers(1, &handle_));
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
    handle_ = other.handle_;
    other.handle_ = GLuint(-1);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    handle_ = other.handle_;
    other.handle_ = GLuint(-1);
    return *this;
}

void Framebuffer::Bind() const {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, handle_));
}

void Framebuffer::Unbind() const {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::AssociateTexture(const FTexture& texture, GLenum attachment) {
    Bind();
    // Call glFramebufferTexture2D with correct arguments.
    // Make sure you use GL_CHECK to detect potential errors.
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER,
        attachment, GL_TEXTURE_2D, texture.GetHandle(), 0));

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Incomplete framebuffer!");
    }
    Unbind();
}

static_assert(std::is_move_constructible<Framebuffer>(), "");
static_assert(std::is_move_assignable<Framebuffer>(), "");

static_assert(!std::is_copy_constructible<Framebuffer>(), "");
static_assert(!std::is_copy_assignable<Framebuffer>(), "");
}