#include "VertexArray.h"
#include <iostream>
#include "BindGuard.h"
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO{

VertexArray::VertexArray()
	: DrawMode(EDrawMode::Triangles), PolygonMode(EPolygonMode::Fill)
{
	GL_CHECK(glGenVertexArrays(1, &Handle)); // Get the handle of this vertex array
}

VertexArray::~VertexArray() 
{
	if (Handle != GLuint(-1))
	{
		GL_CHECK(glDeleteVertexArrays(1, &Handle));
	}
}

VertexArray::VertexArray(VertexArray&& other) noexcept 
{
	Handle = other.Handle;
	other.Handle = GLuint(-1); // Invalidate other handle

	PositionBuffer = std::move(other.PositionBuffer);
	NormalBuffer = std::move(other.NormalBuffer);
	ColorBuffer = std::move(other.ColorBuffer);
	TexCoordBuffer = std::move(other.TexCoordBuffer);
	IndexBuffer = std::move(other.IndexBuffer);
	DrawMode = other.DrawMode;
	PolygonMode = other.PolygonMode;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept 
{
	Handle = other.Handle;
	other.Handle = GLuint(-1); // Invalidate other handle

	PositionBuffer = std::move(other.PositionBuffer);
	NormalBuffer = std::move(other.NormalBuffer);
	ColorBuffer = std::move(other.ColorBuffer);
	TexCoordBuffer = std::move(other.TexCoordBuffer);
	IndexBuffer = std::move(other.IndexBuffer);
	DrawMode = other.DrawMode;
	PolygonMode = other.PolygonMode;

	return *this;
}

void VertexArray::Bind() const 
{
	GL_CHECK(glBindVertexArray(Handle));
}

void VertexArray::Unbind() const 
{
	GL_CHECK(glBindVertexArray(0));
}

void VertexArray::CreatePositionBuffer()
{
	PositionBuffer = make_unique<FPositionBuffer>(GL_STATIC_DRAW);
}

void VertexArray::CreateNormalBuffer()
{
	NormalBuffer = make_unique<FNormalBuffer>(GL_STATIC_DRAW);
}

void VertexArray::CreateColorBuffer()
{
	ColorBuffer = make_unique<FColorBuffer>(GL_STATIC_DRAW);
}

void VertexArray::CreateTexCoordBuffer()
{
	TexCoordBuffer = make_unique<FTexCoordBuffer>(GL_STATIC_DRAW);
}

void VertexArray::CreateIndexBuffer()
{
	IndexBuffer = make_unique<FIndexBuffer>(GL_STATIC_DRAW);
	BindGuard VAO_Bg(this);
	// As opposed to other types of vertex buffers, Element Buffer Objects (EBOs) should not be unbounded.
	IndexBuffer->Bind();
}

void VertexArray::UpdatePositions(const FPositionArray& InPositions) const
{
	PositionBuffer->Update(InPositions);
}

void VertexArray::UpdateNormals(const FNormalArray& InNormals) const
{
	NormalBuffer->Update(InNormals);
}

void VertexArray::UpdateColors(const FColorArray& InColors) const
{
	ColorBuffer->Update(InColors);
}

void VertexArray::UpdateTexCoords(const FTexCoordArray& InTexCoords) const
{
	TexCoordBuffer->Update(InTexCoords);
}

void VertexArray::UpdateIndices(const FIndexArray& InIndices) const
{
	IndexBuffer->Update(InIndices);
}

void VertexArray::LinkPositionBuffer(GLuint InAttrIndex) const
{
	BindGuard VAO_Bg(this);
	BindGuard Buffer_Bg(PositionBuffer.get());
	GL_CHECK(glVertexAttribPointer(InAttrIndex, 3, GL_FLOAT, GL_FALSE, 0, 0)); // Attach the vertex buffer to the VAO.
	GL_CHECK(glEnableVertexAttribArray(InAttrIndex));
}

void VertexArray::LinkNormalBuffer(GLuint InAttrIndex) const
{
	BindGuard VAO_Bg(this);
	BindGuard Buffer_Bg(NormalBuffer.get());
	GL_CHECK(glVertexAttribPointer(InAttrIndex, 3, GL_FLOAT, GL_FALSE, 0, 0)); // Attach the vertex buffer to the VAO.
	GL_CHECK(glEnableVertexAttribArray(InAttrIndex));
}

void VertexArray::LinkColorBuffer(GLuint InAttrIndex) const
{
	BindGuard VAO_Bg(this);
	BindGuard Buffer_Bg(ColorBuffer.get());
	GL_CHECK(glVertexAttribPointer(InAttrIndex, 4, GL_FLOAT, GL_FALSE, 0, 0)); // Attach the vertex buffer to the VAO.
	GL_CHECK(glEnableVertexAttribArray(InAttrIndex));
}

void VertexArray::LinkTexCoordBuffer(GLuint InAttrIndex) const
{
	BindGuard VAO_Bg(this);
	BindGuard Buffer_Bg(TexCoordBuffer.get());
	GL_CHECK(glVertexAttribPointer(InAttrIndex, 2, GL_FLOAT, GL_FALSE, 0, 0)); // Attach the vertex buffer to the VAO.
	GL_CHECK(glEnableVertexAttribArray(InAttrIndex));
}

void VertexArray::SetDrawMode(EDrawMode InMode)
{
	DrawMode = InMode;	
}

EDrawMode VertexArray::GetDrawMode()
{
	return DrawMode;
}

void VertexArray::SetPolygonMode(EPolygonMode InMode)
{
	PolygonMode = InMode;
}

EPolygonMode VertexArray::GetPolygonMode()
{
	return PolygonMode;
}

void VertexArray::Render(size_t InStartIndex, size_t InNumberOfIndices) const
{
	BindGuard VAO_Bg(this);

	if (PolygonMode == EPolygonMode::Wireframe) 
	{
		GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	else 
	{
		GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}

	GLint DrawModeID = DrawMode == EDrawMode::Triangles ? GL_TRIANGLES : (DrawMode == EDrawMode::Lines ? GL_LINES : GL_POINTS);

	if (IndexBuffer != nullptr) 
	{
		// Draw using index buffer
		GL_CHECK(glDrawElements(
			DrawModeID, static_cast<GLsizei>(InNumberOfIndices), GL_UNSIGNED_INT,
			reinterpret_cast<void*>(InStartIndex * sizeof(unsigned int))));
	}
	else 
	{
		// Draw Position buffer
		GL_CHECK(glDrawArrays(DrawModeID, (GLint)InStartIndex, (GLsizei)InNumberOfIndices));
	}
}

void VertexArray::Render() const
{
	if (IndexBuffer != nullptr)
	{
		Render(0, IndexBuffer->GetSize());
	}
	else {
		if (PositionBuffer == nullptr)
		{
			throw std::runtime_error("Cannot render VertexArray without positions!");
		}
		Render(0, PositionBuffer->GetSize());
	}
}

static_assert(std::is_move_constructible<VertexArray>(), "");
static_assert(std::is_move_assignable<VertexArray>(), "");

static_assert(!std::is_copy_constructible<VertexArray>(), "");
static_assert(!std::is_copy_assignable<VertexArray>(), "");
}

