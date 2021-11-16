#pragma once

#include "IWidget.h"

namespace CHISTUDIO {

	class WHierarchy : public IWidget
	{
	public:
		WHierarchy();

		void Render(Application& InApplication);

	private:
		void LoadRowsRecursively(class SceneNode& node, Application& app);
		void TestTable();
	};

}