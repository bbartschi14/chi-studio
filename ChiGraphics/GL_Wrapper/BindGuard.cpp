#include "BindGuard.h"
#include <type_traits>

namespace CHISTUDIO {

BindGuard::BindGuard(const IBindable* InBinder) : Binder(InBinder)
{
	Binder->Bind();
}

BindGuard::BindGuard(BindGuard&& Other) noexcept
{
	Binder = Other.Binder;
	Other.Binder = nullptr;
}

BindGuard::~BindGuard()
{
	if (Binder != nullptr)
	{
		Binder->Unbind();
	}
}

// Static asserts to match the constructor setup of the BindGuard class
static_assert(std::is_move_constructible<BindGuard>(), "");
static_assert(!std::is_copy_constructible<BindGuard>(), "");
static_assert(!std::is_copy_assignable<BindGuard>(), "");
static_assert(!std::is_move_assignable<BindGuard>(), "");

}