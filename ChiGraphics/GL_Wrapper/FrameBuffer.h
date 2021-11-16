#pragma once

#include "BindGuard.h"
#include "ChiGraphics/External.h"
#include "FTexture.h"

namespace CHISTUDIO {

class Framebuffer : public IBindable {
public:
	Framebuffer();
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	// Allow both move-construct and move-assign.
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	void Bind() const override;
	void Unbind() const override;
	void AssociateTexture(const FTexture& texture, GLenum attachment);

private:
	GLuint handle_{ GLuint(-1) };
};

}
