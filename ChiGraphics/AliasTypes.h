#ifndef CHISTUDIO_ALIAS_TYPES_H_
#define CHISTUDIO_ALIAS_TYPES_H_

#include <vector>
#include <glm/glm.hpp>

namespace CHISTUDIO {
	using FPositionArray = std::vector<glm::vec3>;
	using FNormalArray = std::vector<glm::vec3>;
	using FColorArray = std::vector<glm::vec4>;
	using FTexCoordArray = std::vector<glm::vec2>;
	using FIndexArray = std::vector<unsigned int>;
}

#endif // !CHISTUDIO_ALIAS_TYPES_H_
