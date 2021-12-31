#pragma once

namespace CHISTUDIO {
	class Application;

	/** Interface used for widgets
	 */
	class IWidget {
	public:
		virtual void Render(Application& InApplication, float InDeltaTime) = 0;
	};

}