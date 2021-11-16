#include "RenderBuffer.h"

#include <stdexcept>

#include "BindGuard.h"
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

    RenderBuffer::RenderBuffer() {
        GL_CHECK(glGenRenderbuffers(1, &handle_));
    }

    RenderBuffer::~RenderBuffer() {
        if (handle_ != GLuint(-1))
            GL_CHECK(glDeleteRenderbuffers(1, &handle_));
    }

    RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept {
        handle_ = other.handle_;
        other.handle_ = GLuint(-1);
    }

    RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept {
        handle_ = other.handle_;
        other.handle_ = GLuint(-1);
        return *this;
    }

    void RenderBuffer::Bind() const {
        GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, handle_));
    }

    void RenderBuffer::Unbind() const {
        GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }

    static_assert(std::is_move_constructible<RenderBuffer>(), "");
    static_assert(std::is_move_assignable<RenderBuffer>(), "");

    static_assert(!std::is_copy_constructible<RenderBuffer>(), "");
    static_assert(!std::is_copy_assignable<RenderBuffer>(), "");
}