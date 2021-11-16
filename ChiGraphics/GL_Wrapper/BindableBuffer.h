#ifndef CHISTUDIO_BINDABLE_BUFFER_H_
#define CHISTUDIO_BINDABLE_BUFFER_H_

#include <glad/glad.h>

#include "IBindable.h"

namespace CHISTUDIO {

class BindableBuffer : public IBindable
{
public:
	BindableBuffer(GLenum InTarget);
	virtual ~BindableBuffer();

	// Allow both move-construct and move-assign.
	BindableBuffer(BindableBuffer&& Other) noexcept;
	BindableBuffer& operator=(BindableBuffer&& Other) noexcept;

	// Do not allow the copy construction or copy assigning
	BindableBuffer(const BindableBuffer&) = delete;
	BindableBuffer& operator=(const BindableBuffer&) = delete;

	void Reset(GLuint InHandle = 0);
	GLuint Release();

	void Bind() const override;
	void Unbind() const override;

private:
	GLuint Handle;

protected:
	GLenum Target;
};

}


#endif