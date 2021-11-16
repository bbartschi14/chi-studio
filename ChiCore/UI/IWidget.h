#pragma once

namespace CHISTUDIO {
	class Application;

	/** Interface used for widgets
	 */
	class IWidget {
	public:
		virtual ~IWidget() {
		}
		virtual void Render(Application& InApplication) = 0;
	};

}