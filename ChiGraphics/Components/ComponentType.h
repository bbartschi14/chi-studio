#pragma once

#include <typeinfo>
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

enum class EComponentType
{
	Undefined = 0,
	Rendering,
	Shading,
	Material,
	Camera,
	Light
};

template <typename T>
struct ComponentTrait {
	static EComponentType GetType() {
		static_assert(always_false<T>::value, "Unimplemented component type!");
		return EComponentType::Undefined;
	}
};

#define CREATE_COMPONENT_TRAIT(C, T) \
  template <>                        \
  struct ComponentTrait<C> {         \
    static EComponentType GetType() { \
      return T;                      \
    }                                \
  };
}