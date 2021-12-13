#pragma once

#include "IWidget.h"
#include <vector>

namespace CHISTUDIO {

class WHierarchy : public IWidget
{
public:
	WHierarchy();

	void Render(Application& InApplication) override;

private:
	void LoadRowsRecursively(class SceneNode& node, Application& app);
	void TestTable();
	std::vector<class SceneNode*> NodesToDelete;
};

}