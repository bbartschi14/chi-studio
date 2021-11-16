#ifndef CHISTUDIO_VERTEX_BUFFER_H_
#define CHISTUDIO_VERTEX_BUFFER_H_

#include "BindableBuffer.h"
#include <vector>
#include <glad/glad.h>
#include "BindGuard.h"
#include "../Utilities.h"

namespace CHISTUDIO {

template <class T, GLenum Target>
class VertexBuffer : public BindableBuffer {
public:
	VertexBuffer(GLenum InUsage);

	void Update(const std::vector<T>& InArray);

	size_t GetSize() const 
	{
		return Size;
	}

private:
	size_t Size;
	GLenum Usage;
};

template<class T, GLenum target>
VertexBuffer<T, target>::VertexBuffer(GLenum InUsage)
	: BindableBuffer(target), Usage(InUsage) {
}

template<class T, GLenum target>
void VertexBuffer<T, target>::Update(const std::vector<T>& array)
{
	BindGuard Bg(this);
	GL_CHECK(glBufferData(Target, sizeof(T) * array.size(), array.data(), Usage));
	Size = array.size();
}

}

#endif
