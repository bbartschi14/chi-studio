#ifndef CHISTUDIO_I_BINDABLE_H_
#define CHISTUDIO_I_BINDABLE_H_

namespace CHISTUDIO {

/** Interface used for objects that will be bound for openGL use in the renderer, 
 * e.g. shader programs and vertex arrays 
 */
class IBindable {
public:
	virtual ~IBindable() {
	}
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;
};

}  // namespace CHISTUDIO

#endif