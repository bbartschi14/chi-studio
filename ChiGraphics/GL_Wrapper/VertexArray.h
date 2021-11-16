#ifndef CHISTUDIO_VERTEX_ARRAY_H_
#define CHISTUDIO_VERTEX_ARRAY_H_

#include "IBindable.h"
#include "ChiGraphics/External.h"
#include "ChiGraphics/AliasTypes.h"
#include "VertexBuffer.h"

namespace CHISTUDIO {

enum class EDrawMode { Triangles, Lines, Points };

enum class EPolygonMode { Wireframe, Fill };

class VertexArray : public IBindable
{
public:
	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	// Allow both move-construct and move-assign.
	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray&& other) noexcept;

	void Bind() const override;
	void Unbind() const override;

	void CreatePositionBuffer();
	void CreateNormalBuffer();
	void CreateColorBuffer();
	void CreateTexCoordBuffer();
	void CreateIndexBuffer();
	void UpdatePositions(const FPositionArray& InPositions) const;
	void UpdateNormals(const FNormalArray& InNormals) const;
	void UpdateColors(const FColorArray& InColors) const;
	void UpdateTexCoords(const FTexCoordArray& InTexCoords) const;
	void UpdateIndices(const FIndexArray& InIndices) const;
	void LinkPositionBuffer(GLuint InAttrIndex) const;
	void LinkNormalBuffer(GLuint InAttrIndex) const;
	void LinkColorBuffer(GLuint InAttrIndex) const;
	void LinkTexCoordBuffer(GLuint InAttrIndex) const;

	bool HasPositionBuffer() const {
		return PositionBuffer != nullptr;
	}

	bool HasNormalBuffer() const {
		return NormalBuffer != nullptr;
	}

	bool HasColorBuffer() const {
		return ColorBuffer != nullptr;
	}

	bool HasTexCoordBuffer() const {
		return TexCoordBuffer != nullptr;
	}

	bool HasIndexBuffer() const {
		return IndexBuffer != nullptr;
	}

	void SetDrawMode(EDrawMode InMode);
	EDrawMode GetDrawMode();
	void SetPolygonMode(EPolygonMode InMode);
	EPolygonMode GetPolygonMode();

	void Render(size_t InStartIndex, size_t InNumberOfIndices) const;
	void Render() const;

private:
	// Buffers are invisible to the outside.
	using FPositionBuffer = VertexBuffer<glm::vec3, GL_ARRAY_BUFFER>;
	using FNormalBuffer = VertexBuffer<glm::vec3, GL_ARRAY_BUFFER>;
	using FColorBuffer = VertexBuffer<glm::vec4, GL_ARRAY_BUFFER>;
	using FTexCoordBuffer = VertexBuffer<glm::vec2, GL_ARRAY_BUFFER>;
	using FIndexBuffer = VertexBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER>;

	std::unique_ptr<FPositionBuffer> PositionBuffer;
	std::unique_ptr<FNormalBuffer> NormalBuffer;
	std::unique_ptr<FColorBuffer> ColorBuffer;
	std::unique_ptr<FTexCoordBuffer> TexCoordBuffer;
	std::unique_ptr<FIndexBuffer> IndexBuffer;

	EDrawMode DrawMode;
	EPolygonMode PolygonMode;
	GLuint Handle{ GLuint(-1) };
};

}


#endif
