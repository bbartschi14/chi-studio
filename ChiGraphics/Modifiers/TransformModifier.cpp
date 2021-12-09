#include "TransformModifier.h"
#include "ChiGraphics/Meshes/VertexObject.h"
#include <unordered_map>
#include "ChiCore/UI/UILibrary.h"
#include <glm/gtc/type_ptr.hpp>

namespace CHISTUDIO {

void TransformModifier::ApplyModifier(VertexObject* InObjectToModify) const
{
	// Calculate transform matrix
	glm::mat4 transformMatrix(1.f);
	
	// Order: scale, rotate, translate
	transformMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.0f)) * transformMatrix;
	transformMatrix = glm::mat4_cast(glm::quat(glm::radians(glm::vec3(0.0f)))) * transformMatrix;
	transformMatrix = glm::translate(glm::mat4(1.f), Translation) * transformMatrix;

	auto& verts = InObjectToModify->GetVertices();
	size_t numVertices = verts.size();
	for (size_t i = 0; i < numVertices; i++)
	{
		glm::vec3 newPos = transformMatrix * glm::vec4(verts[i]->GetPosition(), 1.0f);
		verts[i]->SetPosition(newPos);
	}
}

bool TransformModifier::RenderUI()
{
	bool wasModified = false;
	wasModified |= UILibrary::DrawVector3Control("Translation", (Translation), 1.0f);
	return wasModified;
}



}
