#ifndef CHISTUDIO_BIND_GUARD_H_
#define CHISTUDIO_BIND_GUARD_H_

#include "IBindable.h"

namespace CHISTUDIO {

/** Used as a wrapper for calling IBindable binds for openGL purposes.
 *   Also see https://riptutorial.com/opengl/example/24911/in-cplusplus11-and-later
 */
class BindGuard {
public:
	// Should be constructed with a pointer to the binder or move-constructed
	BindGuard(const IBindable* InBinder);
	BindGuard(const BindGuard&) = delete;
	BindGuard& operator=(const BindGuard&) = delete;

	// Move-Constructible
	BindGuard(BindGuard&& Other) noexcept;
	// Not move-assignable
	BindGuard& operator=(BindGuard&&) = delete;

	~BindGuard();

private:
	const IBindable* Binder;
};

}
#endif