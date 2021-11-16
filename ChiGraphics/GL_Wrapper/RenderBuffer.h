#pragma once

#include "BindGuard.h"
#include "ChiGraphics/External.h"

namespace CHISTUDIO {

class RenderBuffer : public IBindable
{
public:
	RenderBuffer();
	~RenderBuffer();

	RenderBuffer(const RenderBuffer&) = delete;
	RenderBuffer& operator=(const RenderBuffer&) = delete;

	// Allow both move-construct and move-assign.
	RenderBuffer(RenderBuffer&& other) noexcept;
	RenderBuffer& operator=(RenderBuffer&& other) noexcept;

	void Bind() const override;
	void Unbind() const override;

private:
	GLuint handle_{ GLuint(-1) };
};

}
