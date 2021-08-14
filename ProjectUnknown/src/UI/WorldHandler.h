#pragma once

#include "Engone/UI/IBase.h"

class WorldItem {
private:

public:
	WorldItem(const std::string& name);
	std::string name;

};

class WorldHandler : public engine::IBase {
public:
	WorldHandler(const std::string& name);

	bool MouseEvent(int mx, int my, int action, int button) override;
	bool KeyEvent(int key, int action) override;
	bool ScrollEvent(double scroll) override;

	void Update(float delta) override;
	void Render() override;

	double scrollOffset = 0;
	int selectedItem = -1;
	std::vector<WorldItem> worldItems;
};