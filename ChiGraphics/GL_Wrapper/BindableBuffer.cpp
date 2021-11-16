#include "BindableBuffer.h"

#include "../Utilities.h"
#include <type_traits>

namespace CHISTUDIO {

BindableBuffer::BindableBuffer(GLenum InTarget) :Target(InTarget)
{
	GL_CHECK(glGenBuffers(1, &Handle));
}

BindableBuffer::~BindableBuffer() 
{
	Reset();
}

BindableBuffer::BindableBuffer(BindableBuffer&& Other) noexcept 
{
	Handle = Other.Release();
	Target = Other.Target;
}

BindableBuffer& BindableBuffer::operator=(BindableBuffer&& Other) noexcept
{
	Reset(Other.Release());
	Target = Other.Target;
	return *this;
}

void BindableBuffer::Reset(GLuint InHandle) 
{
	GL_CHECK(glDeleteBuffers(1, &Handle));
	Handle = InHandle;
}

GLuint BindableBuffer::Release() 
{
	GLuint handle = Handle;
	Handle = 0;
	return handle;
}

void BindableBuffer::Bind() const {
	GL_CHECK(glBindBuffer(Target, Handle));
}

void BindableBuffer::Unbind() const {
	GL_CHECK(glBindBuffer(Target, 0));
}

static_assert(std::is_move_constructible<BindableBuffer>(), "");
static_assert(std::is_move_assignable<BindableBuffer>(), "");
static_assert(!std::is_copy_constructible<BindableBuffer>(), "");
static_assert(!std::is_copy_assignable<BindableBuffer>(), "");

}